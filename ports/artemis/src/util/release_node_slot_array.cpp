#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
void Panic(EngineContext* ctx, uint32_t a0);

namespace {

constexpr uint32_t kReleaseNodeSlotControlBlockVtable = 0x90ACCu;  // the control block's vtable
constexpr uint32_t kReleaseNodeSlotPendingFlag = 0xD015Cu;  // the pending-call flag word
constexpr uint32_t kReleaseNodeSlotSaveFrameBytes = 16u;  // live state bytes pushed on suspend
constexpr uint32_t kReleaseNodeSlotResumeWordBytes = 4u;  // resume word pushed above that frame

}  // namespace

// Gives a node a fresh slot array: an 84-byte control block is laid out and
// filled in, the node's array pointer is moved onto it, and the counts of the
// block it replaced are dropped, releasing the old slot pairs as they reach
// zero.
//
// Every callee here can suspend, so each call site carries a resume point naming
// it, and a, control_block, count and scratch are the live words pushed before
// the step returns; the two rewind gates at the top restore them.
void ReleaseNodeSlotArray(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t control_block = 0u;
  uint32_t count = 0u;
  uint32_t frame = 0u;
  uint32_t resume_point = 0u;
  uint32_t scratch = 0u;
  uint32_t pending_flag = 0u;
  // Rewind: drop the 16-byte frame, then the resume word above it.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kReleaseNodeSlotSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    a = LoadU32(ctx, frame);
    count = LoadU32(ctx, frame + 8u);
    scratch = LoadU32(ctx, frame + 12u);
    control_block = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kReleaseNodeSlotResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, kReleaseNodeSlotPendingFlag, 0u);
  }
  // Lay out the 84-byte control block for the node's slot array.
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    frame = HostFoldLayoutOffset(ctx, 1032u, 84u);
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    control_block = frame;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    count = LoadU32(ctx, kReleaseNodeSlotPendingFlag);
    StoreU32(ctx, kReleaseNodeSlotPendingFlag, 0u);
    count = (count == 1u);
  }
  if (count != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      a = FindMatchingCatch(ctx, 0u);
      control_block = HostTempRet0(ctx);
      a = HostRestorePendingThrow(ctx, a);
      StoreU32(ctx, kReleaseNodeSlotPendingFlag, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
      HostMarkRegionSeen(ctx, 1019u);
      resume_point = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      a = LoadU32(ctx, kReleaseNodeSlotPendingFlag);
      StoreU32(ctx, kReleaseNodeSlotPendingFlag, 0u);
      a = (a == 1u);
    }
    if (a != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        a = HostRet0(ctx);
        control_block = HostTempRet0(ctx);
        StoreU32(ctx, kReleaseNodeSlotPendingFlag, 0u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
        HostMarkRegionSeen(ctx, 1021u);
        resume_point = 2u;
        if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        control_block = LoadU32(ctx, kReleaseNodeSlotPendingFlag);
        StoreU32(ctx, kReleaseNodeSlotPendingFlag, 0u);
        if (control_block != 1u) goto abort_missing_call;
        a = FindMatchingCatch(ctx, 0u);
        control_block = HostTempRet0(ctx);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
        Panic(ctx, a);
        resume_point = 3u;
        if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      TrapUnreachable();
    }
  }
  // Fill the block in: vtable, counts and the byte count of the slot area.
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU64(ctx, control_block + 4u, 4294967297ull);
    StoreU8(ctx, control_block + 16u, 0u);
    StoreU32(ctx, control_block + 12u, 0u);
    StoreU32(ctx, control_block, kReleaseNodeSlotControlBlockVtable);
    StoreU32(ctx, kReleaseNodeSlotPendingFlag, 0u);
    count = control_block + 20u;
  }
  // Lay out the slot area the control block points at.
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
    frame = HostFoldLayoutOffset(ctx, 1288u, count);
    resume_point = 4u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    scratch = frame;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    scratch = LoadU32(ctx, kReleaseNodeSlotPendingFlag);
    StoreU32(ctx, kReleaseNodeSlotPendingFlag, 0u);
    pending_flag = (scratch != 1u);
  } else {
    // Rewinding: reuse the flag word restored from the frame.
    pending_flag = scratch;
  }
  // The slot array is ready: move the node onto it and drop the old block.
  if (pending_flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU8(ctx, control_block + 16u, 1u);
      StoreU32(ctx, a + 4u, control_block);
      StoreU32(ctx, a, count);
      a = LoadU32(ctx, control_block + 4u);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (a != 0u) goto done;
      a = LoadU32(ctx, LoadU32(ctx, control_block) + 8u);
      StoreU32(ctx, kReleaseNodeSlotPendingFlag, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
      HostReleaseSlotPair(ctx, a, control_block);
      resume_point = 5u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      a = LoadU32(ctx, kReleaseNodeSlotPendingFlag);
      StoreU32(ctx, kReleaseNodeSlotPendingFlag, 0u);
      a = (a == 1u);
      if (a != 0u) goto rethrow_pending;
      a = LoadU32(ctx, control_block + 8u);
      count = a - 1u;
      StoreU32(ctx, control_block + 8u, count);
      a = (a != 1u);
      if (a != 0u) goto done;
      a = LoadU32(ctx, LoadU32(ctx, control_block) + 12u);
      StoreU32(ctx, kReleaseNodeSlotPendingFlag, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
      HostReleaseSlotPair(ctx, a, control_block);
      resume_point = 6u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      a = LoadU32(ctx, kReleaseNodeSlotPendingFlag);
      StoreU32(ctx, kReleaseNodeSlotPendingFlag, 0u);
      a = (a == 1u);
      if (a != 0u) goto rethrow_pending;
    }
    done:;
    if (ResumePhase(ctx) == kResumeRunning) {
      goto exit_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    a = HostRet0(ctx);
    HostTempRet0(ctx);
    count = LoadU32(ctx, control_block + 4u);
    StoreU32(ctx, control_block + 4u, count - 1u);
    count = (count != 1u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (count != 0u) goto release_object_and_trap;
    count = LoadU32(ctx, LoadU32(ctx, control_block) + 8u);
    StoreU32(ctx, kReleaseNodeSlotPendingFlag, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 7u) {
    HostReleaseSlotPair(ctx, count, control_block);
    resume_point = 7u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    count = LoadU32(ctx, kReleaseNodeSlotPendingFlag);
    StoreU32(ctx, kReleaseNodeSlotPendingFlag, 0u);
    if (count == 1u) goto rethrow_pending;
    count = LoadU32(ctx, control_block + 8u);
    StoreU32(ctx, control_block + 8u, count - 1u);
    if (count != 1u) goto release_object_and_trap;
    count = LoadU32(ctx, LoadU32(ctx, control_block) + 12u);
    StoreU32(ctx, kReleaseNodeSlotPendingFlag, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 8u) {
    HostReleaseSlotPair(ctx, count, control_block);
    resume_point = 8u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    control_block = LoadU32(ctx, kReleaseNodeSlotPendingFlag);
    StoreU32(ctx, kReleaseNodeSlotPendingFlag, 0u);
    if (control_block == 1u) goto rethrow_pending;
  }
  // Both counts reached zero: release the object block and trap out of the step.
  release_object_and_trap:;
  if (ResumePhase(ctx) == kResumeRunning) {
    HostOsRelease(ctx, a);
    TrapUnreachable();
  }
  abort_missing_call:;
  if (ResumePhase(ctx) == kResumeRunning) {
    HostOsRelease(ctx, a);
    TrapUnreachable();
  }
  // Rethrow the throw object the failed release left pending.
  rethrow_pending:;
  if (ResumePhase(ctx) == kResumeRunning) {
    a = FindMatchingCatch(ctx, 0u);
    HostTempRet0(ctx);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 9u) {
    Panic(ctx, a);
    resume_point = 9u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  goto exit_path;
  suspend_point:;
  // Suspend: push the resume point, then the 16-byte frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kReleaseNodeSlotResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, a);
  StoreU32(ctx, frame + 4u, control_block);
  StoreU32(ctx, frame + 8u, count);
  StoreU32(ctx, frame + 12u, scratch);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kReleaseNodeSlotSaveFrameBytes);
  exit_path:;
  LeaveFrame();
}

}  // namespace artc
