#include "util/engine_context.h"

namespace artc {

// Callees that live in their own translation unit.
uint32_t FormatPushString(EngineContext* ctx, uint32_t buffer, uint32_t format, uint32_t args);
void LexError(EngineContext* ctx, uint32_t message, uint32_t argument, uint32_t flag);
void StoreListEntry(EngineContext* ctx, uint32_t table, uint32_t target, uint32_t entry);
uint32_t GrowArray(EngineContext* ctx, uint32_t pool, uint32_t block, uint32_t count_addr,
                   uint32_t entry_bytes, uint32_t limit, uint32_t what);

namespace {

// The messages this walk raises once a list has run out of room, and the word
// naming what ran out.
constexpr uint32_t kAppendWithLimitMainFunctionFmt = 0x2B6C0u;  // main function has more than %d %s
constexpr uint32_t kAppendWithLimitFunctionAtLineFmt = 0x2B6E2u;  // function at line %d has more than %d %s
constexpr uint32_t kAppendWithLimitUpvaluesText = 0x2B77Fu;  // upvalues
constexpr uint32_t kAppendWithLimitEmptyText = 0x2B788u;  // the empty growth message
// The entry count a single list may reach, and the largest count its entry
// array block may be grown to.
constexpr uint32_t kAppendWithLimitEntryCeiling = 60u;
constexpr uint32_t kAppendWithLimitGrowLimit = 0x7FFFFFFDu;

}  // namespace

// Appends one entry to a growable list unless the list already holds it, and
// raises the named-limit error once the list has run out of room. The body is
// suspendable: a rewinding pass restores the ten slots below from the frame it
// pushed and replays the one call its resume code names.
uint32_t AppendWithLimit(EngineContext* ctx, uint32_t list, uint32_t entry, uint32_t limit, uint32_t what) {
  EnterFrame();
  uint32_t const kSaveFrameBytes = 40u;
  uint32_t const kResumeWordBytes = 4u;
  // The codes the five call sites below spill.
  uint32_t const kRecursiveAppend = 0u;
  uint32_t const kFormatMainFmt = 1u;
  uint32_t const kFormatLineFmt = 2u;
  uint32_t const kRaiseError = 3u;
  uint32_t const kGrowArrayCall = 4u;
  // The index a limit record carries when there is no entry to name.
  uint64_t const kNoIndex = 0xFFFFFFFFFFFFFFFFull;
  // The frame saves one word per slot below, so each slot keeps a single name
  // across the whole walk even where a later region reuses it as a carry:
  // list, entry, limit and what are the call's own arguments, and count,
  // cursor, target, probe, stack_slot and slot are the walk's.
  uint32_t count = 0u;
  uint32_t cursor = 0u;
  uint32_t target = 0u;
  uint32_t probe = 0u;
  uint32_t stack_slot = 0u;
  uint32_t slot = 0u;
  uint32_t resume_code = 0u;
  uint32_t result = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 40-byte frame the suspend path pushed and rebuild the
    // ten slots from it.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    list = LoadU32(ctx, frame);
    entry = LoadU32(ctx, frame + 4u);
    limit = LoadU32(ctx, frame + 8u);
    what = LoadU32(ctx, frame + 12u);
    count = LoadU32(ctx, frame + 16u);
    cursor = LoadU32(ctx, frame + 20u);
    target = LoadU32(ctx, frame + 24u);
    probe = LoadU32(ctx, frame + 28u);
    stack_slot = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, frame + 36u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // The second pop names the call that was in flight. The two pops stay
    // separate -- one region is read back at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A 32-byte guest-stack slot backs the message arguments built below.
    stack_slot = GuestStackSp(ctx) - 32u;
    GuestStackSp(ctx) = stack_slot;
    cursor = static_cast<uint32_t>(list == 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (cursor) {
      // No list at all: answer 8, with the entry ceiling left at its largest
      // value and no index to name.
      StoreU64(ctx, limit + 16u, kNoIndex);
      StoreU32(ctx, limit + 8u, 255u);
      cursor = 8u;
      StoreU32(ctx, limit, 8u);
      goto answer_path;
    }
    // Look the key up in the list's bucket table, most recent entry first.
    cursor = LoadU8(ctx, list + 50u);
    for (;;) {
      probe = cursor;
      if (static_cast<int32_t>(cursor) < 1) {
        goto not_found;
      }
      slot = LoadU32(ctx, LoadU32(ctx, list) + 24u);
      cursor = probe - 1u;
      slot += static_cast<uint32_t>(LoadU16(ctx, list + (cursor << 1) + 172u)) * 12u;
      if (LoadU32(ctx, slot) == entry) {
        break;
      }
    }
    // The list already holds it: answer 6 and name the slot it sits in. The
    // caller can ask for the block that owns it to be left alone.
    StoreU64(ctx, limit + 16u, kNoIndex);
    StoreU32(ctx, limit + 8u, cursor);
    cursor = 6u;
    StoreU32(ctx, limit, 6u);
    if (what) {
      goto answer_path;
    }
    list = LoadU32(ctx, list + 20u);
    if (list == 0u) {
      goto answer_path;
    }
    while (static_cast<int32_t>(probe) <= static_cast<int32_t>(LoadU8(ctx, list + 8u))) {
      list = LoadU32(ctx, list);
      if (list == 0u) {
        goto answer_path;
      }
    }
    StoreU8(ctx, list + 9u, 1u);
    goto answer_path;
  not_found:;
    // Not there: append it. The list's own parent is appended into first, and
    // the answer it gives decides whether this walk keeps going; the render
    // below starts from the list's capacity word.
    cursor = 8u;
    count = LoadU32(ctx, list + 8u);
    what = 0u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kRecursiveAppend) : 1u) {
    result = AppendWithLimit(ctx, count, entry, limit, 0u);
    resume_code = kRecursiveAppend;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    count = result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (count == 8u) {
      // The parent reported the missing-list case too.
      goto answer_path;
    }
    // The object that owns the list, and its entry count byte at +72.
    target = LoadU32(ctx, list);
    cursor = LoadU32(ctx, target + 36u);
    count = cursor;
    probe = LoadU8(ctx, target + 72u);
    slot = static_cast<uint32_t>(probe == 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (slot) {
      goto write_entry;
    }
    // The list already holds entries: take the first one whose tag and index
    // both match this entry, if there is one.
    slot = LoadU32(ctx, limit);
    count = 0u;
    do {
      what = list + (count << 1);
      if (slot == LoadU8(ctx, what + 51u) &&
          LoadU8(ctx, what + 52u) == LoadU32(ctx, limit + 8u)) {
        goto append_done;
      }
      count += 1u;
    } while (probe != count);
    count = cursor;
    what = probe;
    if (probe < kAppendWithLimitEntryCeiling) {
      goto write_entry;
    }
    probe = LoadU32(ctx, list + 16u);
    what = LoadU32(ctx, target + 60u);
    count = static_cast<uint32_t>(what == 0u);
  }
  if (count || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // "main function has more than 60 upvalues".
      StoreU32(ctx, stack_slot + 4u, kAppendWithLimitUpvaluesText);
      StoreU32(ctx, stack_slot, kAppendWithLimitEntryCeiling);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFormatMainFmt) : 1u) {
      result = FormatPushString(ctx, probe, kAppendWithLimitMainFunctionFmt, stack_slot);
      resume_code = kFormatMainFmt;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      count = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      goto raise_error;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // "function at line %d has more than 60 upvalues".
    StoreU32(ctx, stack_slot + 24u, kAppendWithLimitUpvaluesText);
    StoreU32(ctx, stack_slot + 20u, kAppendWithLimitEntryCeiling);
    StoreU32(ctx, stack_slot + 16u, what);
    what = stack_slot + 16u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFormatLineFmt) : 1u) {
    result = FormatPushString(ctx, probe, kAppendWithLimitFunctionAtLineFmt, what);
    resume_code = kFormatLineFmt;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    count = result;
  }
raise_error:;
  if (ResumePhase(ctx) == kResumeRunning) {
    what = LoadU32(ctx, list + 12u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kRaiseError) : 1u) {
    LexError(ctx, what, count, 0u);
    resume_code = kRaiseError;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    count = LoadU32(ctx, target + 36u);
    what = LoadU8(ctx, target + 72u);
  }
write_entry:;
  // The count has outrun the array block it was written against.
  what = ResumePhase(ctx)
             ? what
             : static_cast<uint32_t>(static_cast<int32_t>(count) <=
                                     static_cast<int32_t>(what & 255u));
  if (what || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      probe = LoadU32(ctx, list + 16u);
      count = LoadU32(ctx, target + 28u);
      what = target + 36u;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kGrowArrayCall) : 1u) {
      result = GrowArray(ctx, probe, count, what, 4u, kAppendWithLimitGrowLimit,
                         kAppendWithLimitEmptyText);
      resume_code = kGrowArrayCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      what = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Running: keep the block that came back and re-read the entry count.
      StoreU32(ctx, target + 28u, what);
      count = LoadU32(ctx, target + 36u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Zero the slots the block added beyond the count it grew from.
    if (static_cast<int32_t>(count) > static_cast<int32_t>(cursor)) {
      while (cursor != count) {
        StoreU32(ctx, LoadU32(ctx, target + 28u) + (cursor << 2), 0u);
        cursor += 1u;
      }
    }
    // The entry goes in at the end of the block, and the limit that admitted it
    // is recorded in the list's parallel tag and index bytes.
    cursor = LoadU32(ctx, target + 28u);
    count = LoadU8(ctx, target + 72u);
    StoreU32(ctx, cursor + (count << 2), entry);
    if ((LoadU8(ctx, entry + 5u) & 3u) == 0u) {
      goto list_entry_done;
    }
    if ((LoadU8(ctx, target + 5u) & 4u) == 0u) {
      goto list_entry_done;
    }
    StoreListEntry(ctx, LoadU32(ctx, list + 16u), target, entry);
    count = LoadU8(ctx, target + 72u);
  list_entry_done:;
    list += count << 1;
    StoreU8(ctx, list + 51u, LoadU32(ctx, limit));
    StoreU8(ctx, list + 52u, LoadU32(ctx, limit + 8u));
    StoreU8(ctx, target + 72u, count + 1u);
  }
append_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Answer 7: the entry was appended, at the count just reached.
    StoreU32(ctx, limit, 7u);
    StoreU32(ctx, limit + 8u, count);
    cursor = 7u;
  }
answer_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The guest-stack slot goes back and the walk answers with its status: 6
    // when the entry was already there, 7 when it was appended, 8 when the list
    // itself was missing.
    GuestStackSp(ctx) = stack_slot + 32u;
    result = cursor;
    goto exit_path;
  }
  TrapUnreachable();
suspend_point:;
  {
    // Suspend: push the code naming the call in flight, step past it, then push
    // the 40-byte frame a rewinding pass reads back. The stack pointer lives in
    // the word the frame pointer itself names, so every push is a
    // read-modify-write of that word.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, list);
    StoreU32(ctx, frame + 4u, entry);
    StoreU32(ctx, frame + 8u, limit);
    StoreU32(ctx, frame + 12u, what);
    StoreU32(ctx, frame + 16u, count);
    StoreU32(ctx, frame + 20u, cursor);
    StoreU32(ctx, frame + 24u, target);
    StoreU32(ctx, frame + 28u, probe);
    StoreU32(ctx, frame + 32u, stack_slot);
    StoreU32(ctx, frame + 36u, slot);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  }
  result = 0u;
exit_path:;
  LeaveFrame();
  return result;
}

}  // namespace artc
