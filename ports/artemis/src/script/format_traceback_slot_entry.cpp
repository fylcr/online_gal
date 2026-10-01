#include "util/engine_context.h"

namespace artc {

// Makes room in the table's array part when it is full.
void EnsureTableArrayRoom(EngineContext* ctx, uint32_t a);
uint32_t FormatNumber(EngineContext* ctx, uint32_t state, uint32_t value, uint32_t options);

// Appends one traceback frame's source position to the traceback: makes room in the
// slot table's array part when it is full, then formats the position into it.
// Returns nothing. Suspendable -- both steps can suspend.
void FormatTracebackSlotEntry(EngineContext* ctx, uint32_t state, uint32_t value, uint32_t options) {
  EnterFrame();
  // Which call site suspended this body; zero is the room-making below, one the
  // number format.
  uint32_t resume_point = 0u;
  // The word the suspend path saves (below).
  uint32_t resume_at = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 12-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 12u);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    value = LoadU32(ctx, frame + 4u);
    options = LoadU32(ctx, frame + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed with that frame.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  // The array part is full once its live count has reached its limit. A rewinding
  // pass owes this step whatever the counters say, so it always takes it.
  bool array_full = ResumePhase(ctx) == kResumeRewinding;
  if (ResumePhase(ctx) == kResumeRunning) {
    uint32_t const array = LoadU32(ctx, state + 16u);
    array_full = LoadU32(ctx, array + 68u) >= LoadU32(ctx, array + 64u);
  }
  if (array_full) {
    // A rewind makes room unless its resume point says the call already ran; a
    // running pass always does.
    if (ResumePhase(ctx) == kResumeRunning || !resume_point) {
      EnsureTableArrayRoom(ctx, state);
      if (ResumePhase(ctx) == kResumeSuspended) {
        resume_at = 0u;
        goto spill_point;
      }
    }
  }
  // The format runs at resume point one, and on every running pass.
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    FormatNumber(ctx, state, value, options);
    if (ResumePhase(ctx) == kResumeSuspended) {
      resume_at = 1u;
      goto spill_point;
    }
  }
  LeaveFrame();
  return;
spill_point:;
  // Suspend: push the resume point, then the 12-byte frame holding all three
  // parameters.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_at);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, saved_frame, state);
  StoreU32(ctx, saved_frame + 4u, value);
  StoreU32(ctx, saved_frame + 8u, options);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 12u);
  LeaveFrame();
  return;
}

}  // namespace artc
