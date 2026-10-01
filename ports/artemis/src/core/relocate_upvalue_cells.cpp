#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
void RelocateElementArray(EngineContext* ctx, uint32_t state, uint32_t table);

namespace {

constexpr uint32_t kRelocateUpvalueSaveFrameBytes = 28u;  // saved state words below the resume code
constexpr uint32_t kRelocateUpvalueResumeWordBytes = 4u;  // resume code slot

}  // namespace

// Relocates upvalue cells after the value stack moved: every closed cell is
// re-pointed through the callback the compiler state registered, and the free
// pointers are rebased by the distance the stack moved.  Suspendable: both the
// array relocation and the callback can suspend.
void RelocateUpvalueCells(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t scratch = 0u;
  uint32_t new_free = 0u;
  uint32_t old_free = 0u;
  uint32_t head = 0u;
  uint32_t no_callback = 0u;
  uint32_t callback = 0u;
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 28-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kRelocateUpvalueSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    a = LoadU32(ctx, frame);
    b = LoadU32(ctx, frame + 4u);
    scratch = LoadU32(ctx, frame + 8u);
    old_free = LoadU32(ctx, frame + 12u);
    head = LoadU32(ctx, frame + 16u);
    callback = LoadU32(ctx, frame + 20u);
    new_free = LoadU32(ctx, frame + 24u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame that saves the resume code.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kRelocateUpvalueResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Claim the 112-byte scratch area that describes the relocation.
    scratch = GuestStackSp(ctx) - 112u;
    GuestStackSp(ctx) = scratch;
    callback = LoadU32(ctx, a + 68u);
    no_callback = callback == 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (no_callback != 0u) goto reclaim_scratch;
    if (LoadU8(ctx, a + 57u) == 0u) goto reclaim_scratch;
    uint32_t const region = LoadU32(ctx, a + 20u);
    new_free = LoadU32(ctx, region + 8u);
    old_free = LoadU32(ctx, a + 32u);
    head = LoadU32(ctx, a + 8u);
    StoreU32(ctx, scratch + 28u, c);
    StoreU32(ctx, scratch + 8u, b);
    // The cell's index into the value stack, or 0 when it is not a live cell.
    StoreU32(ctx, scratch + 104u,
             b != 4u ? I32_DIV_S(region - LoadU32(ctx, a + 40u), 24u) : 0u);
    // Only relocate when the stack is within one page of its limit.
    b = (uint32_t)((int32_t)(LoadU32(ctx, a + 28u) - head) <= (int32_t)320u);
  }
  if (b != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // Small tables grow by 20 records, larger ones double.
      b = LoadU32(ctx, a + 44u);
      b = (int32_t)b < 20 ? b + 20u : b << 1u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      RelocateElementArray(ctx, a, b);
      resume_point = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, LoadU32(ctx, a + 20u) + 8u, LoadU32(ctx, a + 8u) + 320u);
    StoreU8(ctx, a + 57u, 0u);
    b = scratch + 8u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    CallTableT0(ctx, callback, a, b);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, a + 57u, 1u);
    b = LoadU32(ctx, a + 32u);
    // Rebase the two free pointers by the distance the live region moved.
    StoreU32(ctx, LoadU32(ctx, a + 20u) + 8u, b + (new_free - old_free));
    StoreU32(ctx, a + 8u, head - old_free + b);
  }
reclaim_scratch:
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = scratch + 112u;
  }
  goto exit_path;
suspend_point:
  // Suspend: write the resume code, then push the frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kRelocateUpvalueResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, a);
    StoreU32(ctx, frame + 4u, b);
    StoreU32(ctx, frame + 8u, scratch);
    StoreU32(ctx, frame + 12u, old_free);
    StoreU32(ctx, frame + 16u, head);
    StoreU32(ctx, frame + 20u, callback);
    StoreU32(ctx, frame + 24u, new_free);
  }
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kRelocateUpvalueSaveFrameBytes);
exit_path:
  LeaveFrame();
}

}  // namespace artc
