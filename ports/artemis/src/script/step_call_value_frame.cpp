#include "util/engine_context.h"

namespace artc {

// Forward declaration -- the shared call worker.
void InvokeCallValueWithStack(EngineContext* ctx, uint32_t call_value);

// Steps a call frame: the callable is projected onto the stack and the call is
// handed to the shared worker. The step is suspendable, so the parameter and the
// call word are spilled around the call. Answers zero.
uint32_t StepCallValueFrame(EngineContext* ctx, uint32_t call_value) {
  EnterFrame();
  uint32_t const kResumeWordBytes = 4u;
  uint32_t resume_word = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the parameter cell the suspend path wrote and restore the
    // callable it holds.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    call_value = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the second pop names the call that was in flight. The two pops
    // stay separate -- one region is read back at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_word == 0u) : 1u) {
    InvokeCallValueWithStack(ctx, call_value);
    if (ResumePhase(ctx) == kResumeSuspended) {
      // Suspended: park the word naming the call above the frame, step past it,
      // then write the parameter cell a rewind pass reads back.
      StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), 0u);
      StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
      const uint32_t frame = LoadU32(ctx, ReturnFrameSp(ctx));
      StoreU32(ctx, frame, call_value);
      StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
      LeaveFrame();
      return 0u;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    LeaveFrame();
    return 0u;
  }
  TrapUnreachable();
}

}  // namespace artc
