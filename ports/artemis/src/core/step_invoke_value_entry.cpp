#include "util/engine_context.h"

namespace artc {

// The value stack this step grows.
void ResizeValueStack(EngineContext* ctx, uint32_t state, uint32_t count);
// Checks one argument's type, refusing a mismatch.
void CheckArgumentType(EngineContext* ctx, uint32_t value, uint32_t expected, uint32_t index);
// Advances the table iterator and answers the entry it lands on.
uint32_t AdvanceTableIterator(EngineContext* ctx, uint32_t state, uint32_t table);

// Advances one entry of an invoke queue: the value is type-checked, the value
// stack grown, and the table iterator stepped on. Answers 1, or 2 when the
// queue's current entry is still pending; a suspended pass answers 0. Suspendable,
// so the state and the current entry travel through the return frame.
uint32_t StepInvokeValueEntry(EngineContext* ctx, uint32_t state) {
  EnterFrame();
  // The queue entry the step is on, and the resume point this pass is at.
  uint32_t entry = 0u;
  uint32_t resume_point = 0u;
  uint32_t result = 0u;
  // The resume point the suspend path saves (below).
  uint32_t resume_at = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 8-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 8u);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    entry = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed with that frame.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  // The first call is replayed only at resume point zero.
  if (resume_point == 0u || ResumePhase(ctx) == kResumeRewinding) {
    CheckArgumentType(ctx, state, 1u, 5u);
    if (ResumePhase(ctx) == kResumeSuspended) {
      resume_at = 0u;
      goto spill_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ResizeValueStack(ctx, state, 2u);
  }
  // The second call is replayed only at resume point one.
  if (resume_point == 1u || ResumePhase(ctx) == kResumeRewinding) {
    entry = AdvanceTableIterator(ctx, state, 1u);
    if (ResumePhase(ctx) == kResumeSuspended) {
      resume_at = 1u;
      goto spill_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (entry) {
      // The queue still holds the entry it was on.
      result = 2u;
    } else {
      entry = LoadU32(ctx, state + 8u);
      // The zeroing store lands on the entry just read, not on the queue field.
      StoreU32(ctx, entry + 8u, 0u);
      StoreU32(ctx, state + 8u, entry + 16u);
      result = 1u;
    }
    LeaveFrame();
    return result;
  }
  TrapUnreachable();
spill_point:;
  // Suspend: push the resume point, then the 8-byte frame holding the state and
  // the entry the rewinding pop restores.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_at);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, saved_frame, state);
  StoreU32(ctx, saved_frame + 4u, entry);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 8u);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
