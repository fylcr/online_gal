#include "util/engine_context.h"

namespace artc {

// Defined elsewhere in the tree.
void InvokeCallValueWithStack(EngineContext* ctx, uint32_t a);
uint32_t FormatPushString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void FormatSourceId(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t FormatNumber(EngineContext* ctx, uint32_t state, uint32_t value, uint32_t options);

namespace {

// Format string the formatted message is pushed with: "%s:%d: %s".
constexpr uint32_t kRaiseScriptErrorMessageFormat = 0x2A80Du;
// Source name the message carries when the value has no usable source.
constexpr uint32_t kRaiseScriptErrorNoSource = 0xFFFFFFFFu;
// Bytes of the caller's frame spilled while this step is suspended.
constexpr uint32_t kRaiseScriptErrorFrameBytes = 24u;
// Bytes of the resume word that rides with the spilled frame.
constexpr uint32_t kRaiseScriptErrorResumeWordBytes = 4u;

}  // namespace

// Raises a script error: formats the source position and the message into a
// stack buffer, then hands the result to the value's error handler.
void RaiseScriptError(EngineContext* ctx, uint32_t state, uint32_t value, uint32_t options) {
  EnterFrame();
  uint32_t frame = 0u;
  uint32_t word = 0u;
  uint32_t buffer = 0u;
  uint32_t number = 0u;
  uint32_t result = 0u;
  uint32_t resume_code = 0u;
  uint32_t next_resume_code = 0u;
  uint32_t sp = 0u;
  uint32_t prev = 0u;
  uint32_t entry = 0u;
  uint32_t source_name = 0u;
  uint32_t dest = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    sp = ReturnFrameSp(ctx);
    prev = ReturnFrameSp(ctx);
    prev = LoadU32(ctx, prev);
    StoreU32(ctx, sp, prev - kRaiseScriptErrorFrameBytes);
    sp = ReturnFrameSp(ctx);
    frame = LoadU32(ctx, sp);
    state = LoadU32(ctx, frame);
    value = LoadU32(ctx, frame + 4u);
    options = LoadU32(ctx, frame + 8u);
    word = LoadU32(ctx, frame + 12u);
    buffer = LoadU32(ctx, frame + 16u);
    number = LoadU32(ctx, frame + 20u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    sp = ReturnFrameSp(ctx);
    prev = ReturnFrameSp(ctx);
    prev = LoadU32(ctx, prev);
    StoreU32(ctx, sp, prev - kRaiseScriptErrorResumeWordBytes);
    sp = ReturnFrameSp(ctx);
    sp = LoadU32(ctx, sp);
    sp = LoadU32(ctx, sp);
    resume_code = sp;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Carve the message buffer out of the guest stack.
    word = GuestStackSp(ctx);
    buffer = word - 80u;
    GuestStackSp(ctx) = buffer;
    StoreU32(ctx, buffer + 12u, options);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == 0u) {
    result = FormatNumber(ctx, state, value, options);
    next_resume_code = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_frame;
    }
    number = result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    value = LoadU32(ctx, state + 20u);
    options = LoadU32(ctx, value + 4u);
    word = LoadU32(ctx, options + 8u) != 6u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (word != 0u) {
      goto invoke_error;
    }
    word = LoadU32(ctx, options);
    word = LoadU8(ctx, word + 6u);
    if (word != 0u) {
      goto invoke_error;
    }
    // Resolve the source name the message reports, if the value has one.
    word = LoadU32(ctx, state + 24u);
    StoreU32(ctx, value + 12u, word);
    source_name = kRaiseScriptErrorNoSource;
    entry = LoadU32(ctx, options);
    entry = LoadU32(ctx, entry + 16u);
    options = entry;
    word -= LoadU32(ctx, options + 12u);
    if ((uint32_t)((int32_t)word < (int32_t)4u)) {
      goto source_resolved;
    }
    source_name = 0u;
    value = LoadU32(ctx, options + 20u);
    if (value == 0u) {
      goto source_resolved;
    }
    word = (uint32_t)((int32_t)word >> (2u & 31u));
    word -= 1u;
    word <<= (2u & 31u);
    word += value;
    source_name = LoadU32(ctx, word);
  source_resolved:;
    dest = buffer + 16u;
    entry = LoadU32(ctx, options + 32u);
    FormatSourceId(ctx, dest, entry + 16u, 60u);
    StoreU32(ctx, buffer + 8u, number);
    StoreU32(ctx, buffer + 4u, source_name);
    StoreU32(ctx, buffer, buffer + 16u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == 1u) {
    FormatPushString(ctx, state, kRaiseScriptErrorMessageFormat, buffer);
    next_resume_code = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_frame;
    }
  }
invoke_error:;
  if (ResumePhase(ctx) == kResumeRunning || resume_code == 2u) {
    InvokeCallValueWithStack(ctx, state);
    next_resume_code = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_frame;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = buffer + 80u;
  }
  LeaveFrame();
  return;
suspend_frame:;
  // The call suspended: spill the live locals above the return frame and hand
  // the resume code back to the runner so it can replay this step.
  sp = ReturnFrameSp(ctx);
  sp = LoadU32(ctx, sp);
  StoreU32(ctx, sp, next_resume_code);
  sp = ReturnFrameSp(ctx);
  prev = ReturnFrameSp(ctx);
  prev = LoadU32(ctx, prev);
  StoreU32(ctx, sp, prev + kRaiseScriptErrorResumeWordBytes);
  sp = ReturnFrameSp(ctx);
  frame = LoadU32(ctx, sp);
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, value);
  StoreU32(ctx, frame + 8u, options);
  StoreU32(ctx, frame + 12u, word);
  StoreU32(ctx, frame + 16u, buffer);
  StoreU32(ctx, frame + 20u, number);
  sp = ReturnFrameSp(ctx);
  prev = ReturnFrameSp(ctx);
  prev = LoadU32(ctx, prev);
  StoreU32(ctx, sp, prev + kRaiseScriptErrorFrameBytes);
  LeaveFrame();
  return;
}

}  // namespace artc
