#include "util/engine_context.h"

namespace artc {

// Moves the record's live slots into a freshly allocated one.
void RelocateElementArray(EngineContext* ctx, uint32_t state, uint32_t table);

// Grows the value stack: reallocates the record so it can hold at least `needed`
// slots past its live count, and moves the slots that are already there. Returns
// nothing. Suspendable -- the reallocation can suspend.
void GrowValueStack(EngineContext* ctx, uint32_t record, uint32_t needed) {
  EnterFrame();
  // Which call site suspended this body; zero is the relocation below, the only one
  // there is.
  uint32_t resume_point = 0u;
  // How many slots the record can hold, once the running pass has sized it.
  uint32_t capacity = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 8-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 8u);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    capacity = LoadU32(ctx, frame);
    record = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed with that frame.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The new size is the live count plus `needed`, unless the count has already
    // caught up with `needed`, in which case it doubles.
    uint32_t const slots = LoadU32(ctx, record + 44u);
    capacity = static_cast<int32_t>(slots) < static_cast<int32_t>(needed) ? needed + slots : slots << 1;
  }
  uint32_t issue_relocate = static_cast<uint32_t>(!resume_point);
  issue_relocate = ResumePhase(ctx) != kResumeRunning ? issue_relocate : 1u;
  if (issue_relocate) {
    RelocateElementArray(ctx, record, capacity);
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto spill_point;
    }
  }
  LeaveFrame();
  return;
spill_point:;
  // Suspend: push the resume point, then the 8-byte frame holding the new capacity
  // and the record whose slots are being moved.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), 0u);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, saved_frame, capacity);
  StoreU32(ctx, saved_frame + 4u, record);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 8u);
  LeaveFrame();
  return;
}

}  // namespace artc
