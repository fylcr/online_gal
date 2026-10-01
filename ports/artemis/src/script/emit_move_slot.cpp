#include "util/engine_context.h"

namespace artc {

uint32_t ExpressionToRegister(EngineContext* ctx, uint32_t func_state, uint32_t expr);

// Emits a move: puts the expression into a register and records the move of that
// register into the destination slot. Returns nothing. Suspendable -- the
// expression conversion can suspend.
void EmitMoveSlot(EngineContext* ctx, uint32_t func_state, uint32_t dst_slot, uint32_t expression) {
  EnterFrame();
  uint32_t resume_point = 0u;
  // The word the suspend path saves (below).
  uint32_t resume_at = 0u;
  // The register the expression landed in, on the running pass.
  uint32_t reg = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 12-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 12u);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    func_state = LoadU32(ctx, frame);
    dst_slot = LoadU32(ctx, frame + 4u);
    expression = LoadU32(ctx, frame + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed with that frame.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  // The conversion is issued on every pass except a rewind that already passed it.
  uint32_t issue_move = 1u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    issue_move = static_cast<uint32_t>(!resume_point);
  }
  if (issue_move) {
    reg = ExpressionToRegister(ctx, func_state, expression);
    if (ResumePhase(ctx) == kResumeSuspended) {
      resume_at = 0u;
      goto spill_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The move's own opcode, then the register it copies from.
    StoreU32(ctx, dst_slot, 9u);
    StoreU32(ctx, dst_slot + 12u, reg);
  }
  LeaveFrame();
  return;
spill_point:;
  // Suspend: push the resume point, then the 12-byte frame holding the state, the
  // destination slot and the expression.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_at);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, saved_frame, func_state);
  StoreU32(ctx, saved_frame + 4u, dst_slot);
  StoreU32(ctx, saved_frame + 8u, expression);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 12u);
  LeaveFrame();
  return;
}

}  // namespace artc
