#include "util/engine_context.h"

namespace artc {

// Interns a constant under a key and answers the table entry it landed on.
uint32_t AddFunctionConstant(EngineContext* ctx, uint32_t func_state, uint32_t key, uint32_t value);

// Maps one value to the constant-table operand that stands for it, interning the
// constant on the way when the table does not hold it yet. Answers the operand.
// Suspendable, so the state and the pushed value slot travel through the return
// frame.
uint32_t EncodeConstant(EngineContext* ctx, uint32_t func_state, uint32_t value) {
  EnterFrame();
  // The guest-stack slot the value is pushed into, and the resume point.
  uint32_t value_slot = 0u;
  uint32_t resume_point = 0u;
  // The resume point the suspend path saves (below).
  uint32_t resume_at = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 8-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 8u);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    func_state = LoadU32(ctx, frame);
    value_slot = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed with that frame.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Push the value as a 16-byte guest value: the word, then its tag.
    value_slot = GuestStackSp(ctx) - 16u;
    GuestStackSp(ctx) = value_slot;
    StoreU32(ctx, value_slot + 8u, 4u);
    StoreU32(ctx, value_slot, value);
  }
  // The interning call is replayed only at resume point zero.
  uint32_t issue_add = static_cast<uint32_t>(!resume_point);
  issue_add = ResumePhase(ctx) != kResumeRunning ? issue_add : 1u;
  if (issue_add) {
    value = AddFunctionConstant(ctx, func_state, value_slot, value_slot);
    if (ResumePhase(ctx) == kResumeSuspended) {
      resume_at = 0u;
      goto spill_point;
    }
    func_state = value;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = value_slot + 16u;
    LeaveFrame();
    return func_state;
  }
  TrapUnreachable();
spill_point:;
  // Suspend: push the resume point, then the 8-byte frame holding the state and
  // the value slot the rewinding pop restores.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_at);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, saved_frame, func_state);
  StoreU32(ctx, saved_frame + 4u, value_slot);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 8u);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
