#include "util/engine_context.h"

namespace artc {

// Hands back a fresh string buffer of the requested size.
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
// Raises the error a text that grew past its limit ends in.
void RaiseStringLengthError(EngineContext* ctx, uint32_t a);
// Copies bytes into a text's storage; a zero length copies nothing.
void CopyIfNonEmpty(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {
// The flag that marks a text's storage as heap allocated, the largest length a
// text may take before the count overflows, and the mask that rounds the
// allocation size up to a multiple of sixteen.
constexpr uint32_t kAppendTextBytesHeapFlag = 0x80000000u;
constexpr uint32_t kAppendTextBytesMaxLength = 0xFFFFFFEFu;
constexpr uint32_t kAppendTextBytesAlignMask = 0xFFFFFFF0u;
// The words a save frame costs, and the word that names a call in flight.
constexpr uint32_t kAppendTextBytesSaveFrameBytes = 28u;
constexpr uint32_t kAppendTextBytesResumeWordBytes = 4u;
}  // namespace

// Appends a byte string to a text. A text of at most ten characters keeps them
// in the object itself; a longer one gets a fresh heap buffer, and a length
// that would overflow raises the length error.
void AppendTextBytes(EngineContext* ctx, uint32_t text, uint32_t bytes, uint32_t length) {
  EnterFrame();
  // The stack pointer a frame comes off, and scratch words for the values that
  // pass through it.
  uint32_t sp = 0u;
  uint32_t top = 0u;
  uint32_t arg = 0u;
  uint32_t third = 0u;
  // The word the save frame carries: the short-text flag on a running pass,
  // and the storage the bytes are copied to once that storage exists.
  uint32_t hold = 0u;
  // The object whose storage field the new buffer is written into.
  uint32_t target = 0u;
  // The freshly allocated buffer, and later the base of the save frame.
  uint32_t scratch = 0u;
  // Sixteen bytes cut off the guest stack for the terminator byte that follows
  // the copied text.
  uint32_t pad = 0u;
  // The size asked of the allocator.
  uint32_t capacity = 0u;
  // The call a rewinding pass replays, and the word a suspending pass pushes.
  uint32_t resume_index = 0u;
  uint32_t resume_code = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the save frame and read its words back.
    sp = ReturnFrameSp(ctx);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top) - kAppendTextBytesSaveFrameBytes;
    StoreU32(ctx, sp, top);
    target = ReturnFrameSp(ctx);
    target = LoadU32(ctx, target);
    text = LoadU32(ctx, target);
    bytes = LoadU32(ctx, target + 4u);
    length = LoadU32(ctx, target + 8u);
    hold = LoadU32(ctx, target + 12u);
    pad = LoadU32(ctx, target + 16u);
    capacity = LoadU32(ctx, target + 20u);
    target = LoadU32(ctx, target + 24u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word, which names the call that was in flight.
    sp = ReturnFrameSp(ctx);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top) - kAppendTextBytesResumeWordBytes;
    StoreU32(ctx, sp, top);
    sp = ReturnFrameSp(ctx);
    top = LoadU32(ctx, sp);
    resume_index = LoadU32(ctx, top);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Cut the scratch area off the guest stack, then hold the length against
    // the largest count that cannot overflow it.
    top = GuestStackSp(ctx);
    top = top - 16u;
    pad = top;
    GuestStackSp(ctx) = top;
    hold = length <= kAppendTextBytesMaxLength;
  }
  top = hold;
  arg = ResumePhase(ctx) == kResumeRewinding;
  top = top | arg;
  if (top != 0u) {
    // The length fits, or a rewinding pass is replaying this attempt.
    third = length <= 10u;
    top = ResumePhase(ctx);
    hold = top ? hold : third;
    if (ResumePhase(ctx) == kResumeRunning) {
      if (hold != 0u) {
        // A short text: the characters stay in the object, so only its length
        // has to be noted.
        StoreU8(ctx, text + 11u, length);
        hold = text;
        goto text_done;
      }
      target = text;
      if (length >= 11u) {
        // Round the allocation up to sixteen byte steps.
        arg = length + 16u;
        arg = arg & kAppendTextBytesAlignMask;
        third = arg - 1u;
        third = third == 11u ? arg : third;
      } else {
        third = 10u;
      }
      hold = third;
      capacity = hold + 1u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_index == 0u) {
      // The allocation is the call that can suspend.
      scratch = AllocateStringBuffer(ctx, capacity);
      resume_code = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto save_frame;
      }
      hold = scratch;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Point the object at the new buffer.
      StoreU32(ctx, target, hold);
      arg = capacity | kAppendTextBytesHeapFlag;
      StoreU32(ctx, text + 8u, arg);
      StoreU32(ctx, text + 4u, length);
    }
  text_done:;
    if (ResumePhase(ctx) == kResumeRunning) {
      // Copy the bytes in, then put the terminator byte after them.
      CopyIfNonEmpty(ctx, hold, bytes, length);
      StoreU8(ctx, pad + 15u, 0u);
      arg = length + hold;
      top = LoadU8(ctx, pad + 15u);
      StoreU8(ctx, arg, top);
      top = pad + 16u;
      GuestStackSp(ctx) = top;
      LeaveFrame();
      return;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_index == 1u) {
    // The length did not fit: this is the error the caller ends in.
    RaiseStringLengthError(ctx, text);
    resume_code = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto save_frame;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  LeaveFrame();
  return;
save_frame:;
  // Push the word naming the call in flight, then the save frame; a rewind
  // reads them back in that order.
  scratch = resume_code;
  sp = ReturnFrameSp(ctx);
  top = LoadU32(ctx, sp);
  StoreU32(ctx, top, scratch);
  sp = ReturnFrameSp(ctx);
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top) + kAppendTextBytesResumeWordBytes;
  StoreU32(ctx, sp, top);
  sp = ReturnFrameSp(ctx);
  scratch = LoadU32(ctx, sp);
  StoreU32(ctx, scratch, text);
  arg = bytes;
  StoreU32(ctx, scratch + 4u, arg);
  arg = length;
  StoreU32(ctx, scratch + 8u, arg);
  arg = hold;
  StoreU32(ctx, scratch + 12u, arg);
  arg = pad;
  StoreU32(ctx, scratch + 16u, arg);
  arg = capacity;
  StoreU32(ctx, scratch + 20u, arg);
  arg = target;
  StoreU32(ctx, scratch + 24u, arg);
  sp = ReturnFrameSp(ctx);
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top) + 28u;
  StoreU32(ctx, sp, top);
  LeaveFrame();
}

}  // namespace artc
