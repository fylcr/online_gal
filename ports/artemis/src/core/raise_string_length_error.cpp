#include "util/engine_context.h"

namespace artc {

// The allocator-limit refusal this step reports.
void RaiseAllocLimitError(EngineContext* ctx, uint32_t state);

namespace {
// The refusal's record in the data segment.
constexpr uint32_t kRaiseStringLengthErrorMessage = 0xC2DE9u;  // basic_string
}  // namespace

// Raises the string-length refusal through the allocator-limit route. Suspendable:
// the resume point travels through the return frame, and a pass that comes back
// running traps instead of returning.
void RaiseStringLengthError(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  // Zero means the refusal below never completed, so it is issued again.
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  // A running pass always owes the refusal; a rewinding one owes it only at the
  // resume point it popped above.
  uint32_t issue_error = static_cast<uint32_t>(!resume_point);
  issue_error = ResumePhase(ctx) != kResumeRunning ? issue_error : 1u;
  if (issue_error) {
    RaiseAllocLimitError(ctx, kRaiseStringLengthErrorMessage);
    if (ResumePhase(ctx) == kResumeSuspended) {
      // The refusal reports zero, which is also the resume point saved below.
      goto spill_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  LeaveFrame();
  return;
spill_point:;
  // Suspend: push the resume point on its own 4-byte frame.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), 0u);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  LeaveFrame();
  return;
}

}  // namespace artc
