#include "util/engine_context.h"

namespace artc {

// The script error this refusal is reported through.
void RaiseScriptError(EngineContext* ctx, uint32_t state, uint32_t message, uint32_t argument);

namespace {
// The refusal's record in the data segment.
constexpr uint32_t kRaiseBlockTooBigMessage = 0x2AF6Eu;  // memory allocation error: block too big
}  // namespace

// Refuses a block allocation that is too big and reports it through the script
// error route. Answers zero; the step is suspendable, so the state word and the
// resume point travel through the return frame.
uint32_t RaiseBlockTooBig(EngineContext* ctx, uint32_t state) {
  EnterFrame();
  // Zero means the refusal below never completed, so it is issued again.
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame holding the state word.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    state = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed with that frame.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  // A running pass always owes the refusal; a rewinding one owes it only at the
  // resume point it popped above.
  uint32_t issue_error = static_cast<uint32_t>(!resume_point);
  issue_error = ResumePhase(ctx) != kResumeRunning ? issue_error : 1u;
  if (issue_error) {
    RaiseScriptError(ctx, state, kRaiseBlockTooBigMessage, 0u);
    if (ResumePhase(ctx) == kResumeSuspended) {
      // The refusal reports zero, which is also the resume point saved below.
      goto spill_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    LeaveFrame();
    return 0u;
  }
  TrapUnreachable();
spill_point:;
  // Suspend: push the resume point, then the 4-byte frame holding the state word
  // the rewinding pop restores.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), 0u);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), state);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
