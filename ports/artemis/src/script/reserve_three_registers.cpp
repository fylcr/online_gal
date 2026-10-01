#include "util/engine_context.h"

namespace artc {

// Forward declaration -- defined in core/raise_syntax_error.cpp.
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);

// Reserves the three value-stack slots a one-operand form always needs: the new
// top is stored back and, past the frame's ceiling, the frame raises the
// "function or expression too complex" error first. Answers nothing.
void ReserveThreeRegisters(EngineContext* ctx, uint32_t state) {
  EnterFrame();
  uint32_t const kSaveFrameBytes = 12u;
  uint32_t const kResumeWordBytes = 4u;
  uint32_t const kComplexMessage = 0x2A371u;
  // The frame only reports the error once its top has grown past this many slots.
  int32_t const kSlotLimit = 250;
  uint32_t frame = 0u;
  // The frame record whose +75 byte holds the stack limit, then the value the
  // error path reads back.
  uint32_t record = 0u;
  // The new top of the value stack.
  uint32_t top = 0u;
  uint32_t over_ceiling = 0u;
  uint32_t resume_word = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 12-byte parameter frame, in the order it was written.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    record = LoadU32(ctx, frame + 4u);
    top = LoadU32(ctx, frame + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the second pop names the call that was in flight.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    top = LoadU32(ctx, state + 36u) + 3u;
    record = LoadU32(ctx, state);
    over_ceiling = static_cast<uint32_t>(static_cast<int32_t>(top) >
                                         static_cast<int32_t>(LoadU8(ctx, record + 75u)));
  }
  if (over_ceiling != 0u || ResumePhase(ctx) == kResumeRewinding) {
    // Only a frame that also passed the hard slot limit reports: a rewind
    // replaying this block reports again, a resumed call has reported already.
    if (ResumePhase(ctx) == kResumeRewinding ||
        (ResumePhase(ctx) == kResumeRunning && static_cast<int32_t>(top) >= kSlotLimit)) {
      if (ResumePhase(ctx) == kResumeRunning) {
        record = LoadU32(ctx, state + 12u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_word == 0u) : 1u) {
        RaiseSyntaxError(ctx, record, kComplexMessage);
        if (ResumePhase(ctx) == kResumeSuspended) {
          // Suspended: park the call word above the frame, step past it, then
          // write the parameter frame a rewind pass reads back.
          StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), 0u);
          StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
          frame = LoadU32(ctx, ReturnFrameSp(ctx));
          StoreU32(ctx, frame, state);
          StoreU32(ctx, frame + 4u, record);
          StoreU32(ctx, frame + 8u, top);
          StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
          LeaveFrame();
          return;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        record = LoadU32(ctx, LoadU32(ctx, state));
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, record + 75u, top);
  }
  LeaveFrame();
}

}  // namespace artc
