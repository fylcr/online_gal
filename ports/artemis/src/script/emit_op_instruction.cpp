#include "util/engine_context.h"

namespace artc {

uint32_t EmitInstruction(EngineContext* ctx, uint32_t state, uint32_t opcode, uint32_t code_cursor);

// Packs an opcode and its three operand fields into one instruction word and
// hands it to the emitter. Answers the state the emitter leaves behind.
// Suspendable -- the emit can suspend.
uint32_t EmitOpInstruction(EngineContext* ctx, uint32_t func_state, uint32_t op, uint32_t arg_a, uint32_t arg_b, uint32_t line) {
  EnterFrame();
  uint32_t resume_point = 0u;
  // The code buffer's write point, read from the current scope.
  uint32_t code_cursor = 0u;
  // The word the suspend path saves (below).
  uint32_t resume_at = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 12-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 12u);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    func_state = LoadU32(ctx, frame);
    op = LoadU32(ctx, frame + 4u);
    code_cursor = LoadU32(ctx, frame + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed with that frame.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Pack the fields: the opcode low, A at bit 6, the line at 14, B at 23.
    op = (arg_a << 6) | op | (arg_b << 23) | (line << 14);
    code_cursor = LoadU32(ctx, LoadU32(ctx, func_state + 12u) + 8u);
  }
  uint32_t issue_emit = static_cast<uint32_t>(!resume_point);
  issue_emit = ResumePhase(ctx) != kResumeRunning ? issue_emit : 1u;
  if (issue_emit) {
    uint32_t const emitted = EmitInstruction(ctx, func_state, op, code_cursor);
    if (ResumePhase(ctx) == kResumeSuspended) {
      resume_at = 0u;
      goto spill_point;
    }
    func_state = emitted;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    LeaveFrame();
    return func_state;
  }
  TrapUnreachable();
spill_point:;
  // Suspend: push the resume point, then the 12-byte frame holding the state, the
  // packed word and the write point.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_at);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, saved_frame, func_state);
  StoreU32(ctx, saved_frame + 4u, op);
  StoreU32(ctx, saved_frame + 8u, code_cursor);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 12u);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
