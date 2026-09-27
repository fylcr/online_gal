// append_string_bytes.cpp — append a byte range to a guest string record.
//
// AppendStringBytes is the engine's basic_string append（const char*, size_type）:
// it copies n bytes in behind the record's current contents, and when the
// record's spare capacity cannot take them it hands the relocation to the
// suspendable grow-and-replace product first. The record is libc++-shaped, so
// capacity and length are read from whichever of its two forms is live: the
// long form keeps a data pointer at +0, the length at +4 and a biased capacity
// word at +8, while the short form holds its bytes inline with the length in
// the low bits of byte 11. The top bit of byte 11 is the long-form flag for
// both, so the capacity is that biased word with the flag masked off and one
// subtracted, or the short form's fixed inline width of ten, and the append
// fits exactly while capacity minus length still covers n. The record's bytes
// sit behind the long form's pointer and inside the short record itself.
//
// The body is suspendable: the grow helper is an engine call that may park the
// worker, so the generated body parks its arguments and the live locals on the
// event return frame when the call suspends and restores them on a rewind
// re-entry before retaking the call. That frame traffic, the phase tests, the
// order in which the record is inspected, and the rule that a fitting append
// never calls the helper at all are the engine's own. The two jumps to the
// shared completion point — the fitting append and the empty one — are folded
// into a return at the point they leave, because that point is reached from
// inside the running phase only, where the generated completion test is
// already known to hold. The helper's call keeps its own gate: forward
// execution that reaches it provably did not fit, a resumed phase re-enters
// only while the word the frame carried still says the call is pending.
//
// The generated body also takes a 16-byte scratch frame from the guest stack
// pointer and parks the string's terminator byte in it, reading the byte
// straight back out; the frame and that round trip are kept as written.
// Accessors re-read the memory view on every call, so the generated
// growth-refresh lines have no counterpart here.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//
#include "util/engine_context.h"

namespace artc {

// Forward declaration — defined in util/copy_if_nonempty.cpp.
void CopyIfNonEmpty(EngineContext* ctx, uint32_t dst, uint32_t src, uint32_t n);

uint32_t AppendStringBytes(EngineContext* ctx, uint32_t text, uint32_t src, uint32_t n) {
  EnterFrame();
  // One name per slot, across every role the slot plays: `spill` is the guest
  // scratch frame and later the frame base a rewind pass reads back,
  // `scratch_word` is the capacity probe and later the word the event frame
  // carries and then that frame's base, `text_ref` is the record's alias and
  // later the growth the helper has to absorb, `text_span` is the capacity and
  // later the base the record's bytes live at.
  uint32_t spill = 0u;
  uint32_t scratch_word = 0u;
  uint32_t text_ref = 0u;
  uint32_t text_size = 0u;
  uint32_t text_span = 0u;
  uint32_t call_result = 0u;
  // Rewind, first frame: pop the saved argument-and-local frame and restore it.
  // The word behind the event return-frame pointer holds that frame's base, and
  // the scratch frame the suspended run had taken is its last word.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 28u);
    spill = LoadU32(ctx, ReturnFrameSp(ctx));
    text = LoadU32(ctx, spill);
    src = LoadU32(ctx, spill + 4u);
    n = LoadU32(ctx, spill + 8u);
    text_ref = LoadU32(ctx, spill + 12u);
    text_size = LoadU32(ctx, spill + 16u);
    text_span = LoadU32(ctx, spill + 20u);
    spill = LoadU32(ctx, spill + 24u);
  }
  // Rewind, second frame: pop the word the suspend point parked above the
  // argument frame and keep it as the grow call's completion flag. The helper
  // returns nothing, so that word is always zero and the call is retaken.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    call_result = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A fresh scratch frame for the terminator round trip, then the record's
    // capacity and length: the long form's biased word and length, or the
    // short form's fixed width and inline size byte.
    spill = GuestStackSp(ctx) - 16u;
    GuestStackSp(ctx) = spill;
    text_ref = text;
    if ((LoadU8(ctx, text_ref + 11u) >> 7u) != 0u) {
      text_span = (LoadU32(ctx, text_ref + 8u) & 0x7FFFFFFFu) - 1u;
    } else {
      text_span = 10u;
    }
    if ((LoadU8(ctx, text_ref + 11u) >> 7u) != 0u) {
      text_size = LoadU32(ctx, text + 4u);
    } else {
      text_size = LoadU8(ctx, text + 11u);
    }
    // The append fits while the spare capacity still covers it.
    scratch_word = ((text_span - text_size) >= n) ? 1u : 0u;
    if (scratch_word != 0u) {
      if (n != 0u) {
        // Where the record's bytes are, and where the append lands: behind the
        // long form's pointer, or the short record itself. text_span turns into
        // that base.
        if ((LoadU8(ctx, text + 11u) >> 7u) != 0u) {
          text_span = LoadU32(ctx, text_ref);
        } else {
          text_span = text_ref;
        }
        CopyIfNonEmpty(ctx, text_size + text_span, src, n);
        // The record's length grows by the append: the long form keeps it in
        // the word at +4, the short form in its inline size byte.
        uint32_t const new_size = n + text_size;
        if ((LoadU8(ctx, text_ref + 11u) >> 7u) != 0u) {
          StoreU32(ctx, text + 4u, new_size);
        } else {
          StoreU8(ctx, text + 11u, new_size);
        }
        // The terminator byte: parked in the scratch frame and read straight
        // back out of it, exactly as the generated body leaves it.
        StoreU8(ctx, spill + 15u, 0u);
        uint32_t const terminator = new_size + text_span;
        StoreU8(ctx, terminator, LoadU8(ctx, spill + 15u));
      }
      // The shared completion point for an append that fit — and for the empty
      // one, which jumps here before touching the record: drop the scratch
      // frame and hand the record back. Only the running phase reaches this
      // point, so its completion test always holds here.
      GuestStackSp(ctx) = spill + 16u;
      LeaveFrame();
      return text;
    }
    // The capacity cannot take the append, so the helper has to absorb the
    // overflow: the appended length plus the current length, minus the capacity
    // already there. text_ref turns into that growth.
    text_ref = (n + text_size) - text_span;
  }
  // The suspendable grow-and-replace call and its gate. Forward execution takes
  // it unconditionally, because a fitting append left through the completion
  // point above; a resumed phase takes it only while the word its frame carried
  // is the zero the void call stored there.
  uint32_t const call_reported_zero = (call_result == 0u) ? 1u : 0u;
  uint32_t const grow_gate =
      (ResumePhase(ctx) != kResumeRunning) ? call_reported_zero : 1u;
  if (grow_gate != 0u) {
    GrowStringAndReplace(ctx, text, text_span, text_ref, text_size, text_size, 0u, n, src);
    // A suspended helper parks the void call's zero above this frame and then
    // parks the arguments and live locals behind it, so a rewind re-entry can
    // restore them and retake the call.
    if (ResumePhase(ctx) == kResumeSuspended) {
      scratch_word = 0u;
      StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), scratch_word);
      StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
      scratch_word = LoadU32(ctx, ReturnFrameSp(ctx));
      StoreU32(ctx, scratch_word, text);
      StoreU32(ctx, scratch_word + 4u, src);
      StoreU32(ctx, scratch_word + 8u, n);
      StoreU32(ctx, scratch_word + 12u, text_ref);
      StoreU32(ctx, scratch_word + 16u, text_size);
      StoreU32(ctx, scratch_word + 20u, text_span);
      StoreU32(ctx, scratch_word + 24u, spill);
      StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 28u);
      LeaveFrame();
      return 0u;
    }
  }
  // The helper completed in the running phase: drop the scratch frame and hand
  // the record back. Only the rewind phase could fall past this point, and the
  // machine has no such state, so it traps instead.
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = spill + 16u;
    LeaveFrame();
    return text;
  }
  TrapUnreachable();
  // The epilogue the trap above never lets execution reach.
  LeaveFrame();
  return 0u;
}

}  // namespace artc
