#include "util/engine_context.h"

namespace artc {

namespace {

// The resume word pushed above the saved frame when a call suspends.
constexpr uint32_t kSyncPropertyVectorResumeWordBytes = 4u;

}  // namespace

// Compares the cached property vector against the incoming one, takes the
// equality adapter for the pair when any component drifts, and folds the
// dirty bits back into the state afterwards.
void SyncPropertyVector(EngineContext* ctx, uint32_t state, uint32_t index, float scale, uint32_t flag) {
  uint32_t resume_code = 0u;   // which adapter call a rewinding pass replays
  uint32_t mask = 0u;          // dirty bits taken from the state
  uint32_t frame_base = 0u;    // where a suspended call left its frame
  uint32_t suspend_code = 0u;  // resume word pushed when a call suspends
  uint32_t tmp = 0u;
  uint32_t tmp2 = 0u;
  float comp4 = 0;             // incoming components kept across the compares
  float comp8 = 0;
  float comp16 = 0;
  float comp20 = 0;
  float work = 0;
  float other = 0;
  EnterFrame();
  // Rewinding: pop this step's saved frame and restore the live words.
  if (ResumePhase(ctx) == kResumeRewinding) {
    tmp = ReturnFrameSp(ctx);
    tmp2 = ReturnFrameSp(ctx);
    tmp2 = LoadU32(ctx, tmp2);
    tmp2 -= 20u;
    StoreU32(ctx, tmp, tmp2);
    frame_base = ReturnFrameSp(ctx);
    frame_base = LoadU32(ctx, frame_base);
    state = LoadU32(ctx, frame_base);
    index = LoadU32(ctx, frame_base + 4u);
    scale = LoadF32(ctx, frame_base + 8u);
    flag = LoadU32(ctx, frame_base + 12u);
    mask = LoadU32(ctx, frame_base + 16u);
  }
  // Rewinding: drop the pushed resume word and reload the resume code.
  if (ResumePhase(ctx) == kResumeRewinding) {
    tmp = ReturnFrameSp(ctx);
    tmp2 = ReturnFrameSp(ctx);
    tmp2 = LoadU32(ctx, tmp2);
    tmp2 -= kSyncPropertyVectorResumeWordBytes;
    StoreU32(ctx, tmp, tmp2);
    tmp = ReturnFrameSp(ctx);
    tmp = LoadU32(ctx, tmp);
    tmp = LoadU32(ctx, tmp);
    resume_code = tmp;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadF32(ctx, state + 24u) <= 0.0f) {
      goto frame_stored;
    }
    if (LoadF32(ctx, state + 28u) <= 0.0f) {
      goto frame_stored;
    }
    comp8 = LoadF32(ctx, index + 8u);
    mask = LoadU32(ctx, state + 40u);
    comp4 = LoadF32(ctx, index + 4u);
    work = comp4;
    other = LoadF32(ctx, state + 88u);
    work -= other;
    work = wasm_fabsf(work);
    other = 1.1920929e-07;
    if (work >= other) {
      goto both_stale;
    }
    work = comp8;
    other = LoadF32(ctx, state + 92u);
    work -= other;
    work = wasm_fabsf(work);
    other = 1.1920929e-07;
    if (work >= other) {
      goto both_stale;
    }
    work = LoadF32(ctx, index + 12u);
    other = LoadF32(ctx, state + 96u);
    work -= other;
    work = wasm_fabsf(work);
    other = 1.1920929e-07;
    if (work >= other) {
      goto later_components;
    }
    work = LoadF32(ctx, index + 16u);
    other = LoadF32(ctx, state + 100u);
    work -= other;
    work = wasm_fabsf(work);
    other = 1.1920929e-07;
    if (work >= other) {
      goto later_components;
    }
    work = LoadF32(ctx, index + 20u);
    other = LoadF32(ctx, state + 104u);
    work -= other;
    work = wasm_fabsf(work);
    other = 1.1920929e-07;
    if (work >= other) {
      goto later_components;
    }
    work = LoadF32(ctx, index + 24u);
    other = LoadF32(ctx, state + 108u);
    work -= other;
    work = wasm_fabsf(work);
    other = 1.1920929e-07;
    tmp = (work >= other);
    tmp ^= 1u;
    if (tmp != 0u) {
      goto skip_stores;
    }
  later_components:;
    work = LoadF32(ctx, index + 16u);
    comp16 = work;
    other = LoadF32(ctx, state + 100u);
    work -= other;
    work = wasm_fabsf(work);
    other = 1.1920929e-07;
    if (work >= other) {
      goto both_stale;
    }
    work = LoadF32(ctx, index + 20u);
    comp20 = work;
    other = LoadF32(ctx, state + 104u);
    work -= other;
    work = wasm_fabsf(work);
    other = 1.1920929e-07;
    if (work >= other) {
      goto both_stale;
    }
    mask |= 1u;
    goto apply_mask;
  both_stale:;
    comp20 = LoadF32(ctx, index + 20u);
    comp16 = LoadF32(ctx, index + 16u);
    mask |= 5u;
  apply_mask:;
    // Write the incoming components into the cache and fold in the new bits.
    StoreF32(ctx, state + 88u, comp4);
    StoreF32(ctx, state + 92u, comp8);
    comp8 = LoadF32(ctx, index + 12u);
    StoreF32(ctx, state + 104u, comp20);
    StoreF32(ctx, state + 100u, comp16);
    StoreF32(ctx, state + 96u, comp8);
    work = LoadF32(ctx, index + 24u);
    StoreF32(ctx, state + 108u, work);
  skip_stores:;
    index = mask & 1u;
  }
  if ((index != 0u) | (ResumePhase(ctx) == kResumeRewinding)) {
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, state);
      index = LoadU32(ctx, tmp + 152u);
    }
    if ((resume_code == 0u) | (ResumePhase(ctx) == kResumeRunning)) {
      CallTableT1(ctx, index, state);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      suspend_code = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto save_suspend_frame;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, state + 52u) | 8u;
    index = tmp;
    StoreU32(ctx, state + 52u, tmp);
    if (LoadU8(ctx, state + 38u) == 0u) {
      goto frame_stored;
    }
    flag = !flag;
    scale = LoadF32(ctx, state + 12u) * scale;
  }
  if ((flag != 0u) | (ResumePhase(ctx) == kResumeRewinding)) {
    if (ResumePhase(ctx) == kResumeRunning) {
      flag = state + 56u;
      tmp = LoadU32(ctx, state);
      index = LoadU32(ctx, tmp + 156u);
    }
    if ((resume_code == 1u) | (ResumePhase(ctx) == kResumeRunning)) {
      CallTableT16(ctx, index, state, flag, scale, mask);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      suspend_code = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto save_suspend_frame;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      goto frame_stored;
    }
  }
  flag = (ResumePhase(ctx) != 0u) ? flag : (uint32_t)(scale >= 1.1920929e-07);
  if ((flag != 0u) | (ResumePhase(ctx) == kResumeRewinding)) {
    if (ResumePhase(ctx) == kResumeRunning) {
      flag = state + 56u;
      tmp = LoadU32(ctx, state);
      index = LoadU32(ctx, tmp + 160u);
    }
    if ((resume_code == 2u) | (ResumePhase(ctx) == kResumeRunning)) {
      CallTableT16(ctx, index, state, flag, scale, mask);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      suspend_code = 2u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto save_suspend_frame;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, state + 44u);
      tmp |= LoadU32(ctx, state + 52u);
      index = tmp;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, state + 48u) | index;
    StoreU32(ctx, state + 52u, tmp);
  }
  goto frame_stored;
save_suspend_frame:;
  // A call suspended: store the resume word, push it, then hand the live
  // words to the frame above the stack pointer.
  tmp = ReturnFrameSp(ctx);
  tmp = LoadU32(ctx, tmp);
  StoreU32(ctx, tmp, suspend_code);
  tmp = ReturnFrameSp(ctx);
  tmp2 = ReturnFrameSp(ctx);
  tmp2 = LoadU32(ctx, tmp2);
  tmp2 += kSyncPropertyVectorResumeWordBytes;
  StoreU32(ctx, tmp, tmp2);
  tmp = ReturnFrameSp(ctx);
  tmp = LoadU32(ctx, tmp);
  frame_base = tmp;
  tmp2 = state;
  StoreU32(ctx, frame_base, tmp2);
  StoreU32(ctx, frame_base + 4u, index);
  StoreF32(ctx, frame_base + 8u, scale);
  StoreU32(ctx, frame_base + 12u, flag);
  StoreU32(ctx, frame_base + 16u, mask);
  tmp = ReturnFrameSp(ctx);
  tmp2 = ReturnFrameSp(ctx);
  tmp2 = LoadU32(ctx, tmp2);
  tmp2 += 20u;
  StoreU32(ctx, tmp, tmp2);
frame_stored:;
  LeaveFrame();
}
}  // namespace artc
