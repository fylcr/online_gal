#include "util/engine_context.h"

namespace artc {

uint32_t EmitInstruction(EngineContext* ctx, uint32_t state, uint32_t opcode, uint32_t code_cursor);

// Emits an unconditional jump to a target, using the full 23-bit offset field.
// Returns nothing. Suspendable -- the emit can suspend.
void EmitJumpInstruction(EngineContext* ctx, uint32_t state, uint32_t register_hint, uint32_t target) {
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
    state = LoadU32(ctx, frame);
    register_hint = LoadU32(ctx, frame + 4u);
    code_cursor = LoadU32(ctx, frame + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed with that frame.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Pack the jump: the target sits at bit 23, stored one-based, then the
    // register hint at bit 6 and the opcode's low six bits.
    register_hint = ((target << 23) + 0x800000u) | (register_hint << 6) | 30u;
    code_cursor = LoadU32(ctx, LoadU32(ctx, state + 12u) + 8u);
  }
  uint32_t issue_emit = static_cast<uint32_t>(!resume_point);
  issue_emit = ResumePhase(ctx) != kResumeRunning ? issue_emit : 1u;
  if (issue_emit) {
    EmitInstruction(ctx, state, register_hint, code_cursor);
    if (ResumePhase(ctx) == kResumeSuspended) {
      resume_at = 0u;
      goto spill_point;
    }
  }
  LeaveFrame();
  return;
spill_point:;
  // Suspend: push the resume point, then the 12-byte frame holding the state, the
  // packed word and the write point.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_at);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, saved_frame, state);
  StoreU32(ctx, saved_frame + 4u, register_hint);
  StoreU32(ctx, saved_frame + 8u, code_cursor);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 12u);
  LeaveFrame();
  return;
}

}  // namespace artc
