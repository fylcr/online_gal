#include "util/engine_context.h"

namespace artc {

// The engine panic the rethrow lands in.
void Panic(EngineContext* ctx, uint32_t code);

namespace {
// The pending-throw latch this landing pad clears.
constexpr uint32_t kRethrowPendingExceptionThrowLatch = 0xD015Cu;
}  // namespace

// The rethrow landing pad: takes the pending-throw latch down, and answers
// nothing unless it read back 1, in which case it asks the host for the matching
// catch and panics with the recovered value. Suspendable, so that value and the
// resume point travel through the return frame.
void RethrowPendingException(EngineContext* ctx, uint32_t recovered) {
  EnterFrame();
  // Zero means the panic below never completed, so it is issued again.
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame holding the recovered value.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    recovered = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed with that frame.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The latch is cleared both before and after it is read.
    StoreU32(ctx, kRethrowPendingExceptionThrowLatch, 0u);
    recovered = LoadU32(ctx, kRethrowPendingExceptionThrowLatch);
    StoreU32(ctx, kRethrowPendingExceptionThrowLatch, 0u);
    if (recovered != 1u) {
      LeaveFrame();
      return;
    }
    recovered = FindMatchingCatch(ctx, 0u);
    HostTempRet0(ctx);
  }
  // A running pass always owes the panic; a rewinding one owes it only at the
  // resume point it popped above.
  uint32_t issue_panic = static_cast<uint32_t>(!resume_point);
  issue_panic = ResumePhase(ctx) != kResumeRunning ? issue_panic : 1u;
  if (issue_panic) {
    Panic(ctx, recovered);
    if (ResumePhase(ctx) == kResumeSuspended) {
      // The panic reports zero, which is also the resume point saved below.
      goto spill_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  LeaveFrame();
  return;
spill_point:;
  // Suspend: push the resume point, then the 4-byte frame holding the recovered
  // value the rewinding pop restores.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), 0u);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), recovered);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  LeaveFrame();
  return;
}

}  // namespace artc
