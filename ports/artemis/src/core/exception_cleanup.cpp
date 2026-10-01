#include "util/engine_context.h"

namespace artc {

// Defined elsewhere in the tree.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void Panic(EngineContext* ctx, uint32_t a0);

namespace {

// Fixed cell that carries the unwind marker while a cleanup is in flight.
constexpr uint32_t kResumeExceptionCleanupStatusAddr = 0xD015Cu;
// Bytes of the caller's frame spilled while this step is suspended.
constexpr uint32_t kResumeExceptionCleanupFrameBytes = 12u;
// Bytes of the resume word that rides with the spilled frame.
constexpr uint32_t kResumeExceptionCleanupResumeWordBytes = 4u;

}  // namespace

// Releases the slot pairs an unwind still holds and hands the throw on to the
// catch frame that matches it; when no frame matches, it panics and traps.
void ResumeExceptionCleanup(EngineContext* ctx, uint32_t owner, uint32_t block) {
  EnterFrame();
  uint32_t frame = 0u;
  uint32_t word = 0u;
  uint32_t count = 0u;
  uint32_t resume_code = 0u;
  uint32_t next_resume_code = 0u;
  uint32_t sp = 0u;
  uint32_t prev = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    sp = ReturnFrameSp(ctx);
    prev = ReturnFrameSp(ctx);
    prev = LoadU32(ctx, prev);
    StoreU32(ctx, sp, prev - kResumeExceptionCleanupFrameBytes);
    sp = ReturnFrameSp(ctx);
    frame = LoadU32(ctx, sp);
    owner = LoadU32(ctx, frame);
    block = LoadU32(ctx, frame + 4u);
    word = LoadU32(ctx, frame + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    sp = ReturnFrameSp(ctx);
    prev = ReturnFrameSp(ctx);
    prev = LoadU32(ctx, prev);
    StoreU32(ctx, sp, prev - kResumeExceptionCleanupResumeWordBytes);
    sp = ReturnFrameSp(ctx);
    sp = LoadU32(ctx, sp);
    resume_code = LoadU32(ctx, sp);
  }
  if (ResumePhase(ctx) == kResumeRewinding || block != 0u) {
    if (ResumePhase(ctx) == kResumeRunning) {
      word = LoadU32(ctx, block);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == 0u) {
      ResumeExceptionCleanup(ctx, owner, word);
      next_resume_code = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_frame;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      word = LoadU32(ctx, block + 4u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == 1u) {
      ResumeExceptionCleanup(ctx, owner, word);
      next_resume_code = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_frame;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      owner = LoadU32(ctx, block + 24u);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (owner == 0u) {
        goto cleanup_done;
      }
      count = LoadU32(ctx, owner + 4u);
      StoreU32(ctx, owner + 4u, count - 1u);
      if (count != 1u) {
        goto cleanup_done;
      }
      word = LoadU32(ctx, owner);
      word = LoadU32(ctx, word + 8u);
      StoreU32(ctx, kResumeExceptionCleanupStatusAddr, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == 2u) {
      HostReleaseSlotPair(ctx, word, owner);
      next_resume_code = 2u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_frame;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      word = LoadU32(ctx, kResumeExceptionCleanupStatusAddr);
      StoreU32(ctx, kResumeExceptionCleanupStatusAddr, 0u);
      if (word == 1u) {
        goto no_matching_catch;
      }
      count = LoadU32(ctx, owner + 8u);
      StoreU32(ctx, owner + 8u, count - 1u);
      if (count != 1u) {
        goto cleanup_done;
      }
      word = LoadU32(ctx, owner);
      word = LoadU32(ctx, word + 12u);
      StoreU32(ctx, kResumeExceptionCleanupStatusAddr, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == 3u) {
      HostReleaseSlotPair(ctx, word, owner);
      next_resume_code = 3u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_frame;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The marker cell reports whether the unwind was already taken.
      owner = LoadU32(ctx, kResumeExceptionCleanupStatusAddr);
      StoreU32(ctx, kResumeExceptionCleanupStatusAddr, 0u);
      owner = owner == 1u;
      if (owner != 0u) {
        goto no_matching_catch;
      }
    }
  cleanup_done:;
    if (ResumePhase(ctx) == kResumeRunning) {
      DeallocateRawBlock(ctx, block);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    LeaveFrame();
    return;
  }
no_matching_catch:;
  if (ResumePhase(ctx) == kResumeRunning) {
    owner = FindMatchingCatch(ctx, 0u);
    HostTempRet0(ctx);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == 4u) {
    Panic(ctx, owner);
    next_resume_code = 4u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_frame;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  LeaveFrame();
  return;
suspend_frame:;
  // The call suspended: spill the live locals above the return frame and hand
  // the resume code back to the runner so it can replay this step.
  sp = ReturnFrameSp(ctx);
  sp = LoadU32(ctx, sp);
  StoreU32(ctx, sp, next_resume_code);
  sp = ReturnFrameSp(ctx);
  prev = ReturnFrameSp(ctx);
  prev = LoadU32(ctx, prev);
  StoreU32(ctx, sp, prev + kResumeExceptionCleanupResumeWordBytes);
  sp = ReturnFrameSp(ctx);
  frame = LoadU32(ctx, sp);
  StoreU32(ctx, frame, owner);
  StoreU32(ctx, frame + 4u, block);
  StoreU32(ctx, frame + 8u, word);
  sp = ReturnFrameSp(ctx);
  prev = ReturnFrameSp(ctx);
  prev = LoadU32(ctx, prev);
  StoreU32(ctx, sp, prev + kResumeExceptionCleanupFrameBytes);
  LeaveFrame();
  return;
}

}  // namespace artc
