#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void Panic(EngineContext* ctx, uint32_t a0);

namespace {

constexpr uint32_t kResetSharedArrayControlBlockVtable = 0x8BA0Cu;  // the control block's vtable
constexpr uint32_t kResetSharedArrayAssertExpr = 0x8DEE0u;  // assertion text: "p == 0 || p != px"
constexpr uint32_t kResetSharedArrayAssertFile = 0x8DEF2u;  // assertion file in shared_array.hpp
constexpr uint32_t kResetSharedArrayAssertFunc = 0x8DF2Cu;  // assertion function: "reset"
constexpr uint32_t kResetSharedArrayPendingFlag = 0xD015Cu;  // the pending-call flag word
constexpr uint32_t kResetSharedArraySaveFrameBytes = 24u;  // live state bytes pushed on suspend
constexpr uint32_t kResetSharedArrayResumeWordBytes = 4u;  // resume word pushed above that frame

}  // namespace

// Drops what a shared array points at: the element array held by the control
// block is released, the control block's own counts are decremented, and the
// remaining references are pushed through the same release path.
//
// Every callee here can suspend, so each call site carries a resume point naming
// it, and a, b, control_block, slot_value, scratch and pending_flag are the live
// words pushed before the step returns; the two rewind gates at the top restore
// them.
void ResetSharedArray(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t frame = 0u;
  uint32_t control_block = 0u;
  uint32_t resume_point = 0u;
  uint32_t slot_value = 0u;
  uint32_t scratch = 0u;
  uint32_t pending_flag = 0u;
  uint32_t pending_call = 0u;
  // Rewind: drop the 24-byte frame, then the resume word above it.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kResetSharedArraySaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    a = LoadU32(ctx, frame);
    control_block = LoadU32(ctx, frame + 8u);
    slot_value = LoadU32(ctx, frame + 12u);
    scratch = LoadU32(ctx, frame + 16u);
    pending_flag = LoadU32(ctx, frame + 20u);
    b = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kResetSharedArrayResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  // Borrow a 16-byte staging block from the guest stack.
  if (ResumePhase(ctx) == kResumeRunning) {
    scratch = GuestStackSp(ctx) - 16u;
    GuestStackSp(ctx) = scratch;
    control_block = (b == 0u);
  }
  // A zero pointer means an empty array: lay out a fresh control block, and
  // unwind the pending call if the layout left one behind.
  if (control_block != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kResetSharedArrayPendingFlag, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      frame = HostFoldLayoutOffset(ctx, 1032u, 20u);
      resume_point = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      control_block = frame;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot_value = LoadU32(ctx, kResetSharedArrayPendingFlag);
      StoreU32(ctx, kResetSharedArrayPendingFlag, 0u);
      slot_value = (slot_value != 1u);
      if (slot_value != 0u) goto build_shared_record;
      a = FindMatchingCatch(ctx, 0u);
      b = HostTempRet0(ctx);
      a = HostRestorePendingThrow(ctx, a);
      StoreU32(ctx, kResetSharedArrayPendingFlag, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
      HostMarkRegionSeen(ctx, 1019u);
      resume_point = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      a = LoadU32(ctx, kResetSharedArrayPendingFlag);
      StoreU32(ctx, kResetSharedArrayPendingFlag, 0u);
      a = (a != 1u);
      if (a != 0u) goto finished;
      a = HostRet0(ctx);
      b = HostTempRet0(ctx);
      StoreU32(ctx, kResetSharedArrayPendingFlag, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
      HostMarkRegionSeen(ctx, 1021u);
      resume_point = 2u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      b = LoadU32(ctx, kResetSharedArrayPendingFlag);
      StoreU32(ctx, kResetSharedArrayPendingFlag, 0u);
      if (b != 1u) goto release_catch_and_trap;
      a = FindMatchingCatch(ctx, 0u);
      b = HostTempRet0(ctx);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
      Panic(ctx, a);
      resume_point = 3u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    if (ResumePhase(ctx) == kResumeRunning) goto finished;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot_value = LoadU32(ctx, a);
    control_block = (slot_value != b);
  }
  // The slot still points at a different array: release what it holds before
  // the new control block is installed over it.
  if (control_block != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kResetSharedArrayPendingFlag, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
      frame = HostFoldLayoutOffset(ctx, 1032u, 20u);
      resume_point = 4u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      control_block = frame;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      pending_flag = LoadU32(ctx, kResetSharedArrayPendingFlag);
      StoreU32(ctx, kResetSharedArrayPendingFlag, 0u);
      pending_call = (pending_flag == 1u);
    } else {
      // Rewinding: reuse the flag word restored from the frame.
      pending_call = pending_flag;
    }
    if (pending_call != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        a = FindMatchingCatch(ctx, 0u);
        control_block = HostTempRet0(ctx);
        a = HostRestorePendingThrow(ctx, a);
        DeallocateRawBlock(ctx, b);
        StoreU32(ctx, kResetSharedArrayPendingFlag, 0u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
        HostMarkRegionSeen(ctx, 1019u);
        resume_point = 5u;
        if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        a = LoadU32(ctx, kResetSharedArrayPendingFlag);
        StoreU32(ctx, kResetSharedArrayPendingFlag, 0u);
        a = (a != 1u);
        if (a != 0u) goto finished;
        a = HostRet0(ctx);
        b = HostTempRet0(ctx);
        StoreU32(ctx, kResetSharedArrayPendingFlag, 0u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
        HostMarkRegionSeen(ctx, 1021u);
        resume_point = 6u;
        if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        b = LoadU32(ctx, kResetSharedArrayPendingFlag);
        StoreU32(ctx, kResetSharedArrayPendingFlag, 0u);
        if (b != 1u) goto release_slot_and_trap;
        goto rethrow_pending;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU64(ctx, control_block + 4u, 4294967297ull);
      StoreU32(ctx, control_block + 12u, b);
      StoreU32(ctx, control_block, kResetSharedArrayControlBlockVtable);
      goto install_control_block;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    HostAssertFail(ctx, kResetSharedArrayAssertExpr, kResetSharedArrayAssertFile, 178u,
                   kResetSharedArrayAssertFunc);
    TrapUnreachable();
  }
  // Build the control block header for a freshly laid-out block.
  build_shared_record:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU64(ctx, control_block + 4u, 4294967297ull);
    StoreU32(ctx, control_block + 12u, 0u);
    StoreU32(ctx, control_block, kResetSharedArrayControlBlockVtable);
    slot_value = LoadU32(ctx, a);
  }
  // Splice the new control block into the array's slot and keep the old pointer
  // in the staging block while the counts are dropped.
  install_control_block:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, scratch + 8u, slot_value);
    StoreU32(ctx, a, b);
    b = LoadU32(ctx, a + 4u);
    StoreU32(ctx, a + 4u, control_block);
    StoreU32(ctx, scratch + 12u, b);
    a = (b == 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (a != 0u) goto pop_scratch;
    a = LoadU32(ctx, b + 4u);
    StoreU32(ctx, b + 4u, a - 1u);
    a = (a != 1u);
    if (a != 0u) goto pop_scratch;
    a = LoadU32(ctx, LoadU32(ctx, b) + 8u);
    StoreU32(ctx, kResetSharedArrayPendingFlag, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 7u) {
    HostReleaseSlotPair(ctx, a, b);
    resume_point = 7u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    a = LoadU32(ctx, kResetSharedArrayPendingFlag);
    StoreU32(ctx, kResetSharedArrayPendingFlag, 0u);
    a = (a == 1u);
    if (a != 0u) goto rethrow_pending;
    a = LoadU32(ctx, b + 8u);
    StoreU32(ctx, b + 8u, a - 1u);
    a = (a != 1u);
    if (a != 0u) goto pop_scratch;
    a = LoadU32(ctx, LoadU32(ctx, b) + 12u);
    StoreU32(ctx, kResetSharedArrayPendingFlag, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 8u) {
    HostReleaseSlotPair(ctx, a, b);
    resume_point = 8u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    a = LoadU32(ctx, kResetSharedArrayPendingFlag);
    StoreU32(ctx, kResetSharedArrayPendingFlag, 0u);
    a = (a == 1u);
    if (a != 0u) goto rethrow_pending;
  }
  // Done: give the staging block back to the guest stack.
  pop_scratch:;
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = scratch + 16u;
    goto exit_path;
  }
  release_catch_and_trap:;
  if (ResumePhase(ctx) == kResumeRunning) {
    HostOsRelease(ctx, a);
    TrapUnreachable();
  }
  release_slot_and_trap:;
  if (ResumePhase(ctx) == kResumeRunning) {
    HostOsRelease(ctx, a);
    TrapUnreachable();
  }
  finished:;
  if (ResumePhase(ctx) == kResumeRunning) {
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
  // Suspend: push the resume point, then the 24-byte frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kResetSharedArrayResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, a);
  StoreU32(ctx, frame + 4u, b);
  StoreU32(ctx, frame + 8u, control_block);
  StoreU32(ctx, frame + 12u, slot_value);
  StoreU32(ctx, frame + 16u, scratch);
  StoreU32(ctx, frame + 20u, pending_flag);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kResetSharedArraySaveFrameBytes);
  exit_path:;
  LeaveFrame();
}

}  // namespace artc
