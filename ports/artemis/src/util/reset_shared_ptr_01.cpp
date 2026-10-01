#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
void Panic(EngineContext* ctx, uint32_t a0);

namespace {

constexpr uint32_t kResetSharedPtr01AssertExpr = 0x8D4ACu;  // the shared_ptr assertion expression
constexpr uint32_t kResetSharedPtr01AssertFile = 0x8D4BEu;  // the header the assert blames
constexpr uint32_t kResetSharedPtr01AssertFunc = 0x8D4F6u;  // the function the assert blames
constexpr uint32_t kResetSharedPtr01ControlBlockVtable = 0x8D504u;  // the control block's vtable
constexpr uint32_t kResetSharedPtr01PendingFlag = 0xD015Cu;  // the pending-call flag word
constexpr uint32_t kResetSharedPtr01SaveFrameBytes = 20u;  // bytes the suspend path pushes
constexpr uint32_t kResetSharedPtr01ResumeWordBytes = 4u;  // bytes of the resume word

}  // namespace

// Releases the pointer this shared slot owns and lays out a fresh control
// block for the incoming one; the shared_ptr.hpp assertion trio in the data
// segment guards the swap and reports the call site when it fails.
//
// Suspendable: every call below can suspend, so each site carries a resume
// point, and a, b, temporary, control_block and pending_throw are the live
// words pushed before the step returns; the rewind gates at the top restore
// them.
void ResetSharedPtr01(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t temporary = 0u;
  uint32_t control_block = 0u;
  uint32_t frame = 0u;
  uint32_t resume_point = 0u;
  uint32_t pending_throw = 0u;
  // Rewind: drop the 20-byte frame, then the resume word above it.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kResetSharedPtr01SaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    a = LoadU32(ctx, frame);
    temporary = LoadU32(ctx, frame + 8u);
    control_block = LoadU32(ctx, frame + 12u);
    pending_throw = LoadU32(ctx, frame + 16u);
    b = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kResetSharedPtr01ResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    temporary = GuestStackSp(ctx) - 16u;
    GuestStackSp(ctx) = temporary;
    control_block = !(b);
  }
  if (control_block != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU64(ctx, temporary + 8u, 0ull);
      StoreU32(ctx, kResetSharedPtr01PendingFlag, 0u);
      b = 0u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      frame = HostFoldLayoutOffset(ctx, 1032u, 16u);
      resume_point = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      control_block = frame;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      pending_throw = LoadU32(ctx, kResetSharedPtr01PendingFlag);
      StoreU32(ctx, kResetSharedPtr01PendingFlag, 0u);
      pending_throw = pending_throw != 1u;
      if (pending_throw != 0u) goto install_control_block;
      a = FindMatchingCatch(ctx, 0u);
      b = HostTempRet0(ctx);
      a = HostRestorePendingThrow(ctx, a);
      StoreU32(ctx, kResetSharedPtr01PendingFlag, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
      HostMarkRegionSeen(ctx, 1019u);
      resume_point = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      a = LoadU32(ctx, kResetSharedPtr01PendingFlag);
      StoreU32(ctx, kResetSharedPtr01PendingFlag, 0u);
      a = a == 1u;
    }
    if (a != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        b = HostRet0(ctx);
        a = HostTempRet0(ctx);
        StoreU32(ctx, kResetSharedPtr01PendingFlag, 0u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
        HostMarkRegionSeen(ctx, 1021u);
        resume_point = 2u;
        if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        a = LoadU32(ctx, kResetSharedPtr01PendingFlag);
        StoreU32(ctx, kResetSharedPtr01PendingFlag, 0u);
        a = a != 1u;
        if (a != 0u) goto release_owned;
        a = FindMatchingCatch(ctx, 0u);
        b = HostTempRet0(ctx);
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
    release_owned:;
    if (ResumePhase(ctx) == kResumeRunning) {
      a = LoadU32(ctx, temporary + 12u);
      temporary = !(a);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (temporary != 0u) goto abort_double_release;
      temporary = LoadU32(ctx, a + 4u);
      control_block = temporary - 1u;
      StoreU32(ctx, a + 4u, control_block);
      temporary = temporary != 1u;
      if (temporary != 0u) goto abort_double_release;
      temporary = LoadU32(ctx, LoadU32(ctx, a) + 8u);
      StoreU32(ctx, kResetSharedPtr01PendingFlag, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
      HostReleaseSlotPair(ctx, temporary, a);
      resume_point = 4u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      temporary = LoadU32(ctx, kResetSharedPtr01PendingFlag);
      StoreU32(ctx, kResetSharedPtr01PendingFlag, 0u);
      if (temporary == 1u) goto pending_throw_unwind;
      temporary = LoadU32(ctx, a + 8u);
      control_block = temporary - 1u;
      StoreU32(ctx, a + 8u, control_block);
      temporary = temporary != 1u;
      if (temporary != 0u) goto abort_double_release;
      temporary = LoadU32(ctx, LoadU32(ctx, a) + 12u);
      StoreU32(ctx, kResetSharedPtr01PendingFlag, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
      HostReleaseSlotPair(ctx, temporary, a);
      resume_point = 5u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      a = LoadU32(ctx, kResetSharedPtr01PendingFlag);
      StoreU32(ctx, kResetSharedPtr01PendingFlag, 0u);
      a = a == 1u;
      if (a != 0u) goto pending_throw_unwind;
    }
    abort_double_release:;
    if (ResumePhase(ctx) == kResumeRunning) {
      HostOsRelease(ctx, b);
      TrapUnreachable();
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    control_block = LoadU32(ctx, a) != b;
  }
  if (control_block != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, temporary + 12u, 0u);
      StoreU32(ctx, temporary + 8u, b);
      StoreU32(ctx, kResetSharedPtr01PendingFlag, 0u);
      pending_throw = (temporary + 8u) | 4u;
      control_block = temporary + 8u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
      HostSyncSlot(ctx, 1467u, control_block, b, pending_throw);
      resume_point = 6u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      b = LoadU32(ctx, kResetSharedPtr01PendingFlag);
      StoreU32(ctx, kResetSharedPtr01PendingFlag, 0u);
      if (b != 1u) {
        control_block = LoadU32(ctx, temporary + 12u);
        b = LoadU32(ctx, temporary + 8u);
        goto finished;
      }
      b = HostRet0(ctx);
      a = LoadU32(ctx, temporary + 12u);
      temporary = !(a);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (temporary != 0u) goto abort_unwind_release;
      temporary = LoadU32(ctx, a + 4u);
      control_block = temporary - 1u;
      StoreU32(ctx, a + 4u, control_block);
      temporary = temporary != 1u;
      if (temporary != 0u) goto abort_unwind_release;
      temporary = LoadU32(ctx, LoadU32(ctx, a) + 8u);
      StoreU32(ctx, kResetSharedPtr01PendingFlag, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 7u) {
      HostReleaseSlotPair(ctx, temporary, a);
      resume_point = 7u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      temporary = LoadU32(ctx, kResetSharedPtr01PendingFlag);
      StoreU32(ctx, kResetSharedPtr01PendingFlag, 0u);
      if (temporary == 1u) goto pending_throw_unwind;
      temporary = LoadU32(ctx, a + 8u);
      control_block = temporary - 1u;
      StoreU32(ctx, a + 8u, control_block);
      temporary = temporary != 1u;
      if (temporary != 0u) goto abort_unwind_release;
      temporary = LoadU32(ctx, LoadU32(ctx, a) + 12u);
      StoreU32(ctx, kResetSharedPtr01PendingFlag, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 8u) {
      HostReleaseSlotPair(ctx, temporary, a);
      resume_point = 8u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      a = LoadU32(ctx, kResetSharedPtr01PendingFlag);
      StoreU32(ctx, kResetSharedPtr01PendingFlag, 0u);
      a = a == 1u;
      if (a != 0u) goto pending_throw_unwind;
    }
    abort_unwind_release:;
    if (ResumePhase(ctx) == kResumeRunning) {
      HostOsRelease(ctx, b);
      TrapUnreachable();
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    HostAssertFail(ctx, kResetSharedPtr01AssertExpr, kResetSharedPtr01AssertFile, 698u,
                   kResetSharedPtr01AssertFunc);
    TrapUnreachable();
  }
  install_control_block:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU64(ctx, control_block + 4u, 4294967297ull);
    StoreU32(ctx, control_block + 12u, 0u);
    StoreU32(ctx, control_block, kResetSharedPtr01ControlBlockVtable);
  }
  finished:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, temporary + 8u, LoadU32(ctx, a));
    StoreU32(ctx, a, b);
    b = LoadU32(ctx, a + 4u);
    StoreU32(ctx, a + 4u, control_block);
    StoreU32(ctx, temporary + 12u, b);
    a = !(b);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (a != 0u) goto release_done;
    a = LoadU32(ctx, b + 4u);
    StoreU32(ctx, b + 4u, a - 1u);
    a = a != 1u;
    if (a != 0u) goto release_done;
    a = LoadU32(ctx, LoadU32(ctx, b) + 8u);
    StoreU32(ctx, kResetSharedPtr01PendingFlag, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 9u) {
    HostReleaseSlotPair(ctx, a, b);
    resume_point = 9u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    a = LoadU32(ctx, kResetSharedPtr01PendingFlag);
    StoreU32(ctx, kResetSharedPtr01PendingFlag, 0u);
    a = a == 1u;
    if (a != 0u) goto pending_throw_unwind;
    a = LoadU32(ctx, b + 8u);
    StoreU32(ctx, b + 8u, a - 1u);
    a = a != 1u;
    if (a != 0u) goto release_done;
    a = LoadU32(ctx, LoadU32(ctx, b) + 12u);
    StoreU32(ctx, kResetSharedPtr01PendingFlag, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 10u) {
    HostReleaseSlotPair(ctx, a, b);
    resume_point = 10u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    a = LoadU32(ctx, kResetSharedPtr01PendingFlag);
    StoreU32(ctx, kResetSharedPtr01PendingFlag, 0u);
    a = a == 1u;
    if (a != 0u) goto pending_throw_unwind;
  }
  release_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = temporary + 16u;
    goto exit_path;
  }
  pending_throw_unwind:;
  if (ResumePhase(ctx) == kResumeRunning) {
    a = FindMatchingCatch(ctx, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 11u) {
    Panic(ctx, a);
    resume_point = 11u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  goto exit_path;
  suspend_point:;
  // Suspend: push the resume point, then the 20-byte frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kResetSharedPtr01ResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, a);
  StoreU32(ctx, frame + 4u, b);
  StoreU32(ctx, frame + 8u, temporary);
  StoreU32(ctx, frame + 12u, control_block);
  StoreU32(ctx, frame + 16u, pending_throw);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kResetSharedPtr01SaveFrameBytes);
  exit_path:;
  LeaveFrame();
}

}  // namespace artc
