#include "util/engine_context.h"

namespace artc {

// Releases a string buffer left on the pending list.
void ReleaseStringBufferPending(EngineContext* ctx, uint32_t a, uint32_t b);

// Hands a string buffer's storage back to the pool and clears its capacity. The
// first word is dead on this path: the body reuses the slot as the frame cursor.
// Suspendable, so the resume point and the two live words travel through the
// return frame.
void DeallocateStringBuffer(EngineContext* ctx, uint32_t a, uint32_t storage, uint32_t capacity) {
  EnterFrame();
  // Zero means the release below never completed, so it is issued again.
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 8-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 8u);
    a = LoadU32(ctx, ReturnFrameSp(ctx));
    storage = LoadU32(ctx, a);
    capacity = LoadU32(ctx, a + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed with that frame.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  // A running pass always owes the release; a rewinding one owes it only at the
  // resume point it popped above.
  uint32_t issue_release = static_cast<uint32_t>(!resume_point);
  issue_release = ResumePhase(ctx) != kResumeRunning ? issue_release : 1u;
  if (issue_release) {
    ReleaseStringBufferPending(ctx, storage, capacity);
    if (ResumePhase(ctx) == kResumeSuspended) {
      // The release reports zero, which is also the resume point saved below.
      goto spill_point;
    }
  }
  LeaveFrame();
  return;
spill_point:;
  // Suspend: push the resume point, then an 8-byte frame holding the two words
  // the rewinding pop restores.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), 0u);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  a = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, a, storage);
  StoreU32(ctx, a + 4u, capacity);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 8u);
  LeaveFrame();
  return;
}

}  // namespace artc
