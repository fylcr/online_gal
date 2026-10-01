#include "util/engine_context.h"

namespace artc {

// Forward declaration -- defined in core/raise_syntax_error.cpp.
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);

// Reserves `count` more value-stack slots: the new top is stored back when it
// still fits under the frame's limit, and past the ceiling the frame raises the
// "function or expression too complex" error before storing it anyway. Answers
// nothing.
void ReserveRegisters(EngineContext* ctx, uint32_t state, uint32_t count) {
  EnterFrame();
  uint32_t const kSaveFrameBytes = 20u;
  uint32_t const kResumeWordBytes = 4u;
  uint32_t const kComplexMessage = 0x2A371u;
  uint32_t frame = 0u;
  // The frame record that carries the stack limit byte, then the value the
  // error path reads back.
  uint32_t record = 0u;
  uint32_t target = 0u;
  // The new top of the value stack.
  uint32_t top = 0u;
  uint32_t resume_word = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 20-byte parameter frame, in the order it was written.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    count = LoadU32(ctx, frame + 4u);
    target = LoadU32(ctx, frame + 8u);
    top = LoadU32(ctx, frame + 12u);
    record = LoadU32(ctx, frame + 16u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the second pop names the call that was in flight.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  uint32_t over_ceiling = 0u;
  if (ResumePhase(ctx) == kResumeRunning) {
    top = LoadU32(ctx, state + 36u) + count;
    record = LoadU32(ctx, state);
    // Room left while the new top still sits under the frame's limit byte.
    if (static_cast<int32_t>(top) <= static_cast<int32_t>(LoadU8(ctx, record + 75u))) {
      StoreU32(ctx, state + 36u, top);
      LeaveFrame();
      return;
    }
    target = top;
    over_ceiling = static_cast<uint32_t>(static_cast<int32_t>(top) >= 250);
  }
  if (over_ceiling != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      target = LoadU32(ctx, state + 12u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_word == 0u) : 1u) {
      RaiseSyntaxError(ctx, target, kComplexMessage);
      if (ResumePhase(ctx) == kResumeSuspended) {
        // Suspended: park the call word above the frame, step past it, then
        // write the parameter frame a rewind pass reads back.
        StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), 0u);
        StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
        frame = LoadU32(ctx, ReturnFrameSp(ctx));
        StoreU32(ctx, frame, state);
        StoreU32(ctx, frame + 4u, count);
        StoreU32(ctx, frame + 8u, target);
        StoreU32(ctx, frame + 12u, top);
        StoreU32(ctx, frame + 16u, record);
        StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
        LeaveFrame();
        return;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The frame record is re-read because the error path may have grown the
      // stack under this call.
      record = LoadU32(ctx, state);
      target = LoadU32(ctx, state + 36u) + count;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, record + 75u, top);
    StoreU32(ctx, state + 36u, target);
  }
  LeaveFrame();
}

}  // namespace artc
