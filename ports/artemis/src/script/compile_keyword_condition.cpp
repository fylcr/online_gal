#include "util/engine_context.h"

namespace artc {

void DischargeExpression(EngineContext* ctx, uint32_t func_state, uint32_t expression);
uint32_t EmitConditionJump(EngineContext* ctx, uint32_t func_state, uint32_t condition);

// Compiles a keyword condition: when the condition's two test words differ it
// emits a conditional jump around the body, and then discharges the expression.
// Suspendable -- both calls it makes can suspend.
void CompileKeywordCondition(EngineContext* ctx, uint32_t func_state, uint32_t condition) {
  EnterFrame();
  uint32_t resume_point = 0u;
  // The word the suspend path saves (below).
  uint32_t resume_at = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 8-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 8u);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    func_state = LoadU32(ctx, frame);
    condition = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed with that frame.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  // A rewind always takes the jump block so the suspended call is replayed; a
  // running pass takes it only when the two words differ.
  uint32_t take_jump = 0u;
  if (ResumePhase(ctx) == kResumeRunning) {
    take_jump = LoadU32(ctx, condition + 16u) != LoadU32(ctx, condition + 20u);
  } else if (ResumePhase(ctx) == kResumeRewinding) {
    take_jump = 1u;
  }
  if (take_jump) {
    // A rewind replays the jump unless its resume point says the call already
    // ran; a running pass always emits it.
    if (ResumePhase(ctx) == kResumeRunning || !resume_point) {
      func_state = EmitConditionJump(ctx, func_state, condition);
      if (ResumePhase(ctx) == kResumeSuspended) {
        resume_at = 0u;
        goto spill_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      LeaveFrame();
      return;
    }
  }
  // The discharge runs at resume point one, and on every running pass.
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    DischargeExpression(ctx, func_state, condition);
    if (ResumePhase(ctx) == kResumeSuspended) {
      resume_at = 1u;
      goto spill_point;
    }
  }
  LeaveFrame();
  return;
spill_point:;
  // Suspend: push the resume point, then the 8-byte frame holding both words.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_at);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, saved_frame, func_state);
  StoreU32(ctx, saved_frame + 4u, condition);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 8u);
  LeaveFrame();
  return;
}

}  // namespace artc
