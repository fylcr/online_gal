// grow_string_and_replace.cpp — relocate a guest string's bytes into a larger
// buffer.
//
// GrowStringAndReplace is the engine's reallocating splice: it hands the record
// a bigger byte buffer and refills that buffer as head, inserted run, surviving
// tail. The record is libc++-shaped, and the call carries the same bookkeeping
// the library's grow-and-replace takes（the capacity the record had, the growth
// asked of it, the size it held, and the copy widths for the head, the deleted
// span and the inserted bytes）. The new capacity is the amortized growth the
// generated body computes: the larger of twice the old capacity and the
// requested capacity, aligned up to sixteen and floored at the short form's ten
// bytes. A 16-byte operand frame carved off the guest stack carries the two
// candidates while the larger is chosen（a nested 16-byte frame is taken and
// given straight back around the two loads that pick it）, and the terminator
// byte parks in that frame's last word and is read straight back out of it.
//
// Three of the steps are engine calls that may park the worker — the buffer
// allocation, the release of the old long-form buffer, and the length-error
// path — so this body is a suspendable one and is laid out as its generated
// driver runs it rather than as straight-line relocation. The blocks stay apart
// in driver order: the two rewinding pops（the 44-byte argument-and-live-local
// frame first, then the 4-byte resume point above it — the reverse of the push
// order, which is the asyncify stack contract）, the running setup, the capacity
// select, the allocation, the three copies, the release, the header rewrite and
// the two shared exits. Each call keeps its own gate: a forward pass that
// reaches it owes it, a rewound pass re-issues only the one the parked resume
// point names, and a pass that is merely unwinding takes none of them. The
// length-error call sits after a running pass has already left through the
// header rewrite, so only a rewound pass naming it can reach it.
//
// The running header rewrite captures the record's address before the record's
// slot is reused for the grown size, which is the round trip through a
// temporary the generated body makes; it is kept as written. Accessors re-read
// the memory view on every call, so the generated growth-refresh lines have no
// counterpart here.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//
#include "util/engine_context.h"

namespace artc {

// Forward declaration — defined in util/copy_if_nonempty.cpp.
void CopyIfNonEmpty(EngineContext* ctx, uint32_t dst, uint32_t src, uint32_t n);

// Forward declaration — defined in util/allocate_string_buffer.cpp.
// PROVISIONAL: the generated f201 body is not registered in rewrite-names.tsv
// yet, so this callee name and its presumed product file are both provisional.
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t bytes);

// Forward declaration — defined in util/deallocate_string_buffer.cpp.
// PROVISIONAL: the generated f560 body is not registered in rewrite-names.tsv
// yet, so this callee name and its presumed product file are both provisional.
void DeallocateStringBuffer(EngineContext* ctx, uint32_t text, uint32_t old_data, uint32_t old_cap);

// Forward declaration — defined in util/raise_string_length_error.cpp.
// PROVISIONAL: the generated f248 body is not registered in rewrite-names.tsv
// yet, so this callee name and its presumed product file are both provisional.
void RaiseStringLengthError(EngineContext* ctx, uint32_t text);

void GrowStringAndReplace(EngineContext* ctx, uint32_t arg0, uint32_t arg1, uint32_t arg2,
                          uint32_t arg3, uint32_t arg4, uint32_t arg5, uint32_t arg6,
                          uint32_t arg7) {
  EnterFrame();
  // The ABI-frozen parameter spelling above is bound once to the guest's own
  // names. One name per slot, across every role the slot plays: `text` is the
  // record and, after relocation, the size it now holds; `delta_cap` is the
  // growth asked of the record and later the nested operand frame, the
  // amortized capacity and the base of the new buffer; `new_stuff` is the
  // inserted run and later the surviving tail's width; `capacity` is an operand
  // address, then the aligned capacity, then the bytes the allocation asks for;
  // `old_data` first carries the doubling test and then the byte base the
  // copies read; `n_copy` is the head's width and later whether the grown
  // record has left the short form; `spill` is the rewinding frame base, an
  // operand address, the allocation's result and the frame the suspend path
  // pushes.
  uint32_t text = arg0;
  uint32_t old_cap = arg1;
  uint32_t delta_cap = arg2;
  uint32_t old_size = arg3;
  uint32_t n_copy = arg4;
  uint32_t n_del = arg5;
  uint32_t n_add = arg6;
  uint32_t new_stuff = arg7;
  uint32_t spill = 0u;
  uint32_t scratch = 0u;
  uint32_t capacity = 0u;
  uint32_t old_data = 0u;
  uint32_t resume_point = 0u;
  uint32_t requested = 0u;
  uint32_t doubled = 0u;
  // The value each engine call reports when it parks the worker — the resume
  // point the suspend path pushes — and the gate of each call site.
  uint32_t suspend_marker = 0u;
  uint32_t allocation_gate = 0u;
  uint32_t release_gate = 0u;
  uint32_t release_pending = 0u;
  uint32_t length_error_gate = 0u;
  // Rewinding, first frame: pop the 44-byte frame the suspend path saved and
  // restore the record, the six arguments and the three live slots from it. The
  // capacity word is read last, in the generated order.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 44u);
    spill = LoadU32(ctx, ReturnFrameSp(ctx));
    text = LoadU32(ctx, spill);
    delta_cap = LoadU32(ctx, spill + 8u);
    old_size = LoadU32(ctx, spill + 12u);
    n_copy = LoadU32(ctx, spill + 16u);
    n_del = LoadU32(ctx, spill + 20u);
    n_add = LoadU32(ctx, spill + 24u);
    new_stuff = LoadU32(ctx, spill + 28u);
    scratch = LoadU32(ctx, spill + 32u);
    capacity = LoadU32(ctx, spill + 36u);
    old_data = LoadU32(ctx, spill + 40u);
    old_cap = LoadU32(ctx, spill + 4u);
  }
  // Rewinding, second frame: pop the resume point parked above that frame. It
  // names the call the rewound pass still owes, zero being the allocation.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A 16-byte operand frame for the two capacity candidates, then the
    // doubling test: whether twice the old capacity still fits the shared slot.
    // The test lands in the slot that later holds the record's byte base.
    scratch = GuestStackSp(ctx) - 16u;
    GuestStackSp(ctx) = scratch;
    old_data = (((old_cap ^ 0xFFFFFFFFu) - 17u) >= delta_cap) ? 1u : 0u;
  }
  // The block below is entered on a forward pass whose doubling fits — and then
  // always runs — or on any rewound pass, where the phase alone carries it. A
  // merely unwinding pass enters nothing. The flag is joined to the phase test
  // as the generated body joins them, the doubling answer first.
  if ((old_data | ((ResumePhase(ctx) == kResumeRewinding) ? 1u : 0u)) != 0u) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The record's byte base: the long form keeps it behind the pointer at
      // +0, the short form holds it inline, and the top bit of byte 11 says
      // which. The generated arms share one merge point, which is the select.
      if ((LoadU8(ctx, text + 11u) >> 7u) != 0u) {
        old_data = LoadU32(ctx, text);
      } else {
        old_data = text;
      }
      if (old_cap < 0x7FFFFFE7u) {
        // The two candidates go into the operand frame: twice the old capacity
        // at +8 and the requested capacity at +12. A nested 16-byte frame is
        // taken and given straight back around the two loads that pick the
        // larger of them, which is the amortized capacity.
        StoreU32(ctx, scratch + 8u, old_cap << 1u);
        StoreU32(ctx, scratch + 12u, old_cap + delta_cap);
        delta_cap = GuestStackSp(ctx) - 16u;
        GuestStackSp(ctx) = delta_cap;
        capacity = scratch + 12u;
        requested = LoadU32(ctx, capacity);
        spill = scratch + 8u;
        doubled = LoadU32(ctx, spill);
        GuestStackSp(ctx) = delta_cap + 16u;
        delta_cap = LoadU32(ctx, (requested < doubled) ? spill : capacity);
        // Align the capacity up to sixteen and floor it at the short form's ten
        // bytes; the decremented candidate is the merge point's other arm.
        if (delta_cap >= 11u) {
          delta_cap = (delta_cap + 16u) & 0xFFFFFFF0u;
          capacity = delta_cap;
          delta_cap = delta_cap - 1u;
          spill = (delta_cap == 11u) ? 1u : 0u;
          delta_cap = (spill != 0u) ? capacity : delta_cap;
        } else {
          delta_cap = 10u;
        }
      } else {
        delta_cap = 0xFFFFFFEEu;
      }
      capacity = delta_cap + 1u;
    }
    // The allocation is owed by every forward pass that got here and, on a
    // rewound pass, only while the popped resume point is zero. The select
    // reads the phase for truthiness, so any non-running phase keeps the
    // popped answer.
    allocation_gate = (ResumePhase(ctx) != kResumeRunning) ? ((resume_point == 0u) ? 1u : 0u) : 1u;
    if (allocation_gate != 0u) {
      spill = AllocateStringBuffer(ctx, capacity);
      // The zero this void-shaped call reports is the resume point the suspend
      // path pushes, and the restored capacity is the one it asks for again.
      suspend_marker = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      delta_cap = spill;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Head, inserted run, surviving tail — each copy gated by its own width,
      // in the generated order. The tail starts behind the head and the inserted
      // run, and reads from behind the head and the deleted span of the old
      // byte base.
      if (n_copy != 0u) {
        CopyIfNonEmpty(ctx, delta_cap, old_data, n_copy);
      }
      if (n_add != 0u) {
        CopyIfNonEmpty(ctx, delta_cap + n_copy, new_stuff, n_add);
      }
      old_size = old_size - n_del;
      new_stuff = old_size - n_copy;
      if (new_stuff != 0u) {
        CopyIfNonEmpty(ctx, delta_cap + n_copy + n_add, n_del + (n_copy + old_data), new_stuff);
      }
      // The grown capacity decides whether the record is still short: the short
      // form's inline width is ten, so a record past it is long.
      old_cap = old_cap + 1u;
      n_copy = (old_cap != 11u) ? 1u : 0u;
    }
    // The release of the old long-form buffer is owed only by a record that has
    // left the short form, and by every rewound pass.
    release_gate = (n_copy != 0u) | ((ResumePhase(ctx) == kResumeRewinding) ? 1u : 0u);
    if (release_gate != 0u) {
      release_pending = (ResumePhase(ctx) != kResumeRunning) ? ((resume_point == 1u) ? 1u : 0u) : 1u;
      if (release_pending != 0u) {
        DeallocateStringBuffer(ctx, text, old_data, old_cap);
        suspend_marker = 1u;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The header rewrite: the new buffer at +0, the biased capacity word at
      // +8, the grown size at +4, and the terminator byte — parked in the
      // operand frame and read straight back out of it — at the new buffer's
      // end. The record's address is captured before the record slot turns into
      // the grown size, so the two header stores keep the captured pointer.
      uint32_t const record = text;
      StoreU32(ctx, record, delta_cap);
      StoreU32(ctx, record + 8u, capacity | 0x80000000u);
      text = old_size + n_add;
      StoreU32(ctx, record + 4u, text);
      StoreU8(ctx, scratch + 7u, 0u);
      StoreU8(ctx, text + delta_cap, LoadU8(ctx, scratch + 7u));
      GuestStackSp(ctx) = scratch + 16u;
      goto exit_path;
    }
  }
  // The length-error path is owed by a rewound pass whose popped resume point
  // names it. No forward pass reaches it: the header rewrite above left through
  // the shared exit.
  length_error_gate = (ResumePhase(ctx) != kResumeRunning) ? ((resume_point == 2u) ? 1u : 0u) : 1u;
  if (length_error_gate != 0u) {
    RaiseStringLengthError(ctx, text);
    suspend_marker = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  goto exit_path;
suspend_point:;
  // Push the resume point, then the 44-byte frame holding the record, the seven
  // arguments and the three live slots, in the generated order.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), suspend_marker);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  spill = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, spill, text);
  StoreU32(ctx, spill + 4u, old_cap);
  StoreU32(ctx, spill + 8u, delta_cap);
  StoreU32(ctx, spill + 12u, old_size);
  StoreU32(ctx, spill + 16u, n_copy);
  StoreU32(ctx, spill + 20u, n_del);
  StoreU32(ctx, spill + 24u, n_add);
  StoreU32(ctx, spill + 28u, new_stuff);
  StoreU32(ctx, spill + 32u, scratch);
  StoreU32(ctx, spill + 36u, capacity);
  StoreU32(ctx, spill + 40u, old_data);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 44u);
exit_path:;
  LeaveFrame();
}

}  // namespace artc
