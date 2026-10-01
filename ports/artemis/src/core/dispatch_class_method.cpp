#include "util/engine_context.h"

namespace artc {

namespace {

// Bytes of the caller's frame spilled while this step is suspended.
constexpr uint32_t kDispatchClassMethodFrameBytes = 12u;
// Bytes of the resume word that rides with the spilled frame.
constexpr uint32_t kDispatchClassMethodResumeWordBytes = 4u;

}  // namespace

// Walks the class method table: a class id selects the per-class word that
// names the entry, the child path takes the low six bits, and everything else
// indexes the slot table directly. Returns the value the caller continues with.
uint32_t DispatchClassMethod(EngineContext* ctx, uint32_t state, uint32_t key) {
  EnterFrame();
  uint32_t frame = 0u;
  uint32_t target = 0u;
  uint32_t out_of_range = 0u;
  uint32_t result = 0u;
  uint32_t resume_code = 0u;
  uint32_t next_resume_code = 0u;
  uint32_t sp = 0u;
  uint32_t prev = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    sp = ReturnFrameSp(ctx);
    prev = ReturnFrameSp(ctx);
    prev = LoadU32(ctx, prev);
    StoreU32(ctx, sp, prev - kDispatchClassMethodFrameBytes);
    sp = ReturnFrameSp(ctx);
    frame = LoadU32(ctx, sp);
    state = LoadU32(ctx, frame);
    target = LoadU32(ctx, frame + 8u);
    key = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    sp = ReturnFrameSp(ctx);
    prev = ReturnFrameSp(ctx);
    prev = LoadU32(ctx, prev);
    StoreU32(ctx, sp, prev - kDispatchClassMethodResumeWordBytes);
    sp = ReturnFrameSp(ctx);
    sp = LoadU32(ctx, sp);
    resume_code = LoadU32(ctx, sp);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A replay keeps the target restored from the frame; while running the
    // class id records whether it is past the slot table.
    out_of_range = key > 319u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (out_of_range) {
      goto fallback_dispatch;
    }
    // A class id in range selects its word out of the slot table.
    target = LoadU32(ctx, state + (key << (2u & 31u)) + 8516u);
    // A set low bit marks an entry that names a child path of its own.
    if (target & 63u) {
      goto child_path;
    }
    if (key > 255u) {
      goto fallback_dispatch;
    }
    // The same id indexes a second table; a zero entry falls back as well.
    if (LoadU32(ctx, state + key * 24u + 2368u) == 0u) {
      goto fallback_dispatch;
    }
    target = LoadU32(ctx, state);
    target = LoadU32(ctx, target + 28u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == 0u) {
    // Replay the dispatch of the class entry itself.
    result = CallTableT3(ctx, target, state, key);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    next_resume_code = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_frame;
    }
    state = result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    LeaveFrame();
    return state;
  }
fallback_dispatch:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Read the fallback entry the state itself points at.
    target = LoadU32(ctx, state);
    target = LoadU32(ctx, target + 24u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == 1u) {
    // Replay the fallback dispatch.
    result = CallTableT3(ctx, target, state, key);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    next_resume_code = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_frame;
    }
    state = result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    LeaveFrame();
    return state;
  }
child_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The child entry answers with bit 5 of its class word.
    LeaveFrame();
    return (target & 32u) >> (5u & 31u);
  }
  TrapUnreachable();
suspend_frame:;
  // The call suspended: spill the live locals above the return frame and hand
  // the resume code back to the runner so it can replay this step.
  sp = ReturnFrameSp(ctx);
  sp = LoadU32(ctx, sp);
  StoreU32(ctx, sp, next_resume_code);
  sp = ReturnFrameSp(ctx);
  prev = ReturnFrameSp(ctx);
  prev = LoadU32(ctx, prev);
  StoreU32(ctx, sp, prev + kDispatchClassMethodResumeWordBytes);
  sp = ReturnFrameSp(ctx);
  frame = LoadU32(ctx, sp);
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, key);
  StoreU32(ctx, frame + 8u, target);
  sp = ReturnFrameSp(ctx);
  prev = ReturnFrameSp(ctx);
  prev = LoadU32(ctx, prev);
  StoreU32(ctx, sp, prev + kDispatchClassMethodFrameBytes);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
