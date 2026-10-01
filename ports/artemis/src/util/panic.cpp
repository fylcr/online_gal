#include "util/engine_context.h"

namespace artc {

// Forward declaration — defined in util/engine_context.h.
void ResetSuspendableFrame(EngineContext* ctx);

namespace {

// One word on the suspend stack: the resume code this step writes on its way out.
constexpr uint32_t kPanicResumeWordBytes = 4u;

}  // namespace

// The failure handoff: it restores the host's saved exception slot, resets the
// suspend entry the caller left behind, and ends in the unreachable trap.  When
// the reset suspends instead, the resume code is spilled for the rewinding pass.
void Panic(EngineContext* ctx, uint32_t code) {
  EnterFrame();
  uint32_t resume_code = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame that saves the resume code.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kPanicResumeWordBytes);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Restores the saved exception slot; this step itself has no use for it.
    (void)HostRestorePendingThrow(ctx, code);
  }
  // A running pass always resets the frame; a rewinding one only at resume code 0.
  if (ResumePhase(ctx) == kResumeRunning || resume_code == 0u) {
    ResetSuspendableFrame(ctx);
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  LeaveFrame();
  return;
suspend_point:
  // Suspend: the reset reports zero, which is also the resume code written here.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), 0u);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kPanicResumeWordBytes);
  LeaveFrame();
}

}  // namespace artc
