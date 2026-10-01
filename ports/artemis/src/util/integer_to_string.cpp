#include "util/engine_context.h"

namespace artc {

// Cross-product callee: the heap's block release.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);

namespace {

constexpr uint32_t kIntegerToStringConstD015C = 0xD015Cu;
constexpr uint32_t kIntegerToStringConst7FFFFFFF = 0x7FFFFFFFu;
constexpr uint32_t kIntegerToStringConstFFFFFFFF = 0xFFFFFFFFu;
constexpr uint32_t kIntegerToStringResumeWordBytes = 4u;

}  // namespace

// Formats the number in `value` as signed decimal text in the string object at
// `dest`: the digits are produced backwards in a scratch frame and then copied
// forwards, and the resulting length is stored back into `dest`.
void IntegerToString(EngineContext* ctx, uint32_t dest, uint32_t value) {
  EnterFrame();
  uint32_t frame = 0u;
  uint32_t budget = 0u;
  uint32_t number = 0u;
  uint32_t saved_word = 0u;
  uint32_t limit = 0u;
  uint32_t buffer_end = 0u;
  uint32_t resume_code = 0u;
  uint32_t selected = 0u;
  uint32_t flag = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the saved frame and read its words back in the saved order.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t frame_top = ReturnFrameSp(ctx);
    frame_top = LoadU32(ctx, frame_top) - 32u;
    StoreU32(ctx, sp, frame_top);
    uint32_t saved = ReturnFrameSp(ctx);
    saved = LoadU32(ctx, saved);
    dest = LoadU32(ctx, saved);
    frame = LoadU32(ctx, saved + 8u);
    budget = LoadU32(ctx, saved + 12u);
    number = LoadU32(ctx, saved + 16u);
    saved_word = LoadU32(ctx, saved + 20u);
    limit = LoadU32(ctx, saved + 24u);
    buffer_end = LoadU32(ctx, saved + 28u);
    value = LoadU32(ctx, saved + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word that names the call that was in flight.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t word_top = ReturnFrameSp(ctx);
    word_top = LoadU32(ctx, word_top) - kIntegerToStringResumeWordBytes;
    StoreU32(ctx, sp, word_top);
    uint32_t word = ReturnFrameSp(ctx);
    word = LoadU32(ctx, word);
    resume_code = LoadU32(ctx, word);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Carve the scratch frame; the digits are written backwards inside it.
    frame = GuestStackSp(ctx) - 48u;
    GuestStackSp(ctx) = frame;
    StoreU64(ctx, dest, 0ull);
    StoreU32(ctx, dest + 8u, 0u);
    buffer_end = frame + 23u;
    StoreU32(ctx, frame + 28u, buffer_end);
    // The saved frame pointer goes in with bit 1 set.
    uint32_t tagged = frame | 2u;
    StoreU32(ctx, frame + 24u, tagged);
    number = LoadU32(ctx, value);
    StoreU32(ctx, kIntegerToStringConstD015C, 0u);
    // The header repeats this value in two fields, a word and a byte.
    uint32_t size_word{48u};
    StoreU32(ctx, frame + 44u, size_word);
    uint32_t size_byte{48u};
    StoreU8(ctx, frame + 40u, size_byte);
    StoreU32(ctx, frame + 36u, buffer_end);
    // The sign as a mask, zero or all ones; the flattened form masks the shift
    // count with 31 as well.
    uint32_t sign_mask = static_cast<uint32_t>(static_cast<int32_t>(number) >> (31u & 31u));
    saved_word = number + sign_mask;
    StoreU32(ctx, frame + 32u, sign_mask ^ saved_word);
    value = frame + 32u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == 0u) {
    // The folded layout offset becomes the cursor the digit stores walk
    // backwards from.
    uint32_t layout = HostFoldLayoutOffset(ctx, 1275u, value);
    resume_code = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
    value = layout;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Read and clear the flag at 0xD015C.
    uint32_t guard = LoadU32(ctx, kIntegerToStringConstD015C);
    StoreU32(ctx, kIntegerToStringConstD015C, 0u);
    flag = guard == 1u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (flag != 0u) {
      goto unresumable;
    }
    // A negative number gets a leading minus; the length byte read back below is
    // sign-extended into the flag that picks the digit budget.
    uint32_t first = 0u;
    if (static_cast<int32_t>(number) >= 0) {
      first = 0u;
    } else {
      value = value - 1u;
      StoreU8(ctx, value, 45u);
      first = LoadU8(ctx, dest + 11u);
    }
    StoreU32(ctx, frame + 24u, value);
    StoreU32(ctx, frame + 28u, buffer_end);
    number = buffer_end - value;
    // Sign-extend the low byte; both shift counts are masked by the flattening.
    first = first << (24u & 31u);
    first = static_cast<uint32_t>(static_cast<int32_t>(first) >> (24u & 31u));
    limit = static_cast<uint32_t>(static_cast<int32_t>(first) <=
                                  static_cast<int32_t>(kIntegerToStringConstFFFFFFFF));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (limit != 0u) {
      // The sign-extended length byte came out negative, so the budget is taken
      // from the string's spare room, one short of what may be filled.
      uint32_t room = LoadU32(ctx, dest + 8u);
      budget = (room & kIntegerToStringConst7FFFFFFF) - 1u;
      if (number <= budget) {
        goto resume_digits;
      }
      saved_word = LoadU32(ctx, dest + 4u);
    } else {
      // Otherwise the budget is ten digits.
      budget = 10u;
      selected = dest;
      if (number <= 10u) {
        goto digit_start;
      }
    }
    // Clear the flag before the conversion runs.
    StoreU32(ctx, kIntegerToStringConstD015C, 0u);
    limit = number - budget;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == 1u) {
    HostInvokeViiiiiii(ctx, 1276u, dest, budget, limit, saved_word, 0u, saved_word, 0u);
    resume_code = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The re-entry flag is read and cleared again: one means the conversion
    // already ran and this call cannot be resumed.
    uint32_t guard = LoadU32(ctx, kIntegerToStringConstD015C);
    StoreU32(ctx, kIntegerToStringConstD015C, 0u);
    if (guard == 1u) {
      goto unresumable;
    }
    selected = dest;
    // A non-negative length byte jumps straight to the digit walk.
    if (LoadS8(ctx, dest + 11u) > static_cast<int32_t>(kIntegerToStringConstFFFFFFFF)) {
      goto digit_start;
    }
  }
resume_digits:;
  // On a rewind pass the digit pointer is either already restored or read back
  // out of the string object.
  selected = ResumePhase(ctx);
  if (selected != 0u) {
    selected = limit;
  } else {
    selected = LoadU32(ctx, dest);
  }
digit_start:;
  limit = selected;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The digits were written backwards, so the copy walks them forwards; the
    // cursor ends up on the terminator.
    uint32_t cursor = limit;
    if (value != buffer_end) {
    copy_digits:;
      uint32_t digit = LoadU8(ctx, value);
      StoreU8(ctx, cursor, digit);
      cursor = cursor + 1u;
      value = value + 1u;
      if (value != buffer_end) {
        goto copy_digits;
      }
    }
    StoreU8(ctx, cursor, 0u);
    // A negative length byte takes the wide store.
    if (LoadS8(ctx, dest + 11u) <= static_cast<int32_t>(kIntegerToStringConstFFFFFFFF)) {
      StoreU32(ctx, dest + 4u, number);
    } else {
      StoreU8(ctx, dest + 11u, number);
    }
    GuestStackSp(ctx) = frame + 48u;
    goto finish;
  }
unresumable:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The call cannot be resumed: the pending return value and the string
    // object are released, then the trap fires.
    value = HostRet0(ctx);
    HostTempRet0(ctx);
    if (LoadS8(ctx, dest + 11u) <= static_cast<int32_t>(kIntegerToStringConstFFFFFFFF)) {
      uint32_t text = LoadU32(ctx, dest);
      DeallocateRawBlock(ctx, text);
    }
    HostOsRelease(ctx, value);
    TrapUnreachable();
  }
finish:;
  LeaveFrame();
  return;
suspend_here:;
  // Push the resume word naming the call that was in flight, then the frame the
  // rewind pops; the words go in the order that pop reads them back.
  uint32_t slot = ReturnFrameSp(ctx);
  slot = LoadU32(ctx, slot);
  StoreU32(ctx, slot, resume_code);
  uint32_t sp = ReturnFrameSp(ctx);
  uint32_t word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + kIntegerToStringResumeWordBytes;
  StoreU32(ctx, sp, word_top);
  uint32_t base = ReturnFrameSp(ctx);
  base = LoadU32(ctx, base);
  StoreU32(ctx, base, dest);
  StoreU32(ctx, base + 4u, value);
  StoreU32(ctx, base + 8u, frame);
  StoreU32(ctx, base + 12u, budget);
  StoreU32(ctx, base + 16u, number);
  StoreU32(ctx, base + 20u, saved_word);
  StoreU32(ctx, base + 24u, limit);
  StoreU32(ctx, base + 28u, buffer_end);
  sp = ReturnFrameSp(ctx);
  word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + 32u;
  StoreU32(ctx, sp, word_top);
  LeaveFrame();
}

}  // namespace artc
