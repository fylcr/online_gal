#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
void Panic(EngineContext* ctx, uint32_t a0);
uint32_t FillBytes(EngineContext* ctx, uint32_t dst, uint32_t value, uint32_t length);

namespace {

constexpr uint32_t kCreateRecordVtable = 0x8AF98u;  // the record's dispatch table
constexpr uint32_t kCreateRecordValueVtable = 0x8B050u;  // the value slot's table
constexpr uint32_t kCreateRecordDataVtable = 0x8B07Cu;  // the payload area's table
constexpr uint32_t kCreateRecordPendingFlag = 0xD015Cu;  // the pending-call flag word
constexpr uint32_t kCreateRecordSizeMask = 0xFFFFFFFCu;  // rounds a payload size down to a word
constexpr uint32_t kCreateRecordSaveFrameBytes = 20u;  // bytes the suspend path pushes
constexpr uint32_t kCreateRecordResumeWordBytes = 4u;  // bytes of the resume word

}  // namespace

// Builds a shared record for the value and links it into the shared owner's
// list, laying out the record and its payload area through the layout helper
// and releasing both again when a pending-throw check fails.
//
// Suspendable: every call below can suspend, so each site carries a resume
// point, and state, value, record, scratch and data are the live words pushed
// before the step returns; the two rewind gates at the top restore them.
void CreateSharedRecord(EngineContext* ctx, uint32_t state, uint32_t value) {
  EnterFrame();
  uint32_t record = 0u;
  uint32_t frame = 0u;
  uint32_t scratch = 0u;
  uint32_t resume_point = 0u;
  uint32_t data = 0u;
  uint32_t pending = 0u;
  // Rewind: drop the 20-byte frame, then the resume word above it.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kCreateRecordSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    record = LoadU32(ctx, frame + 8u);
    scratch = LoadU32(ctx, frame + 12u);
    data = LoadU32(ctx, frame + 16u);
    value = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kCreateRecordResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, kCreateRecordPendingFlag, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    frame = HostFoldLayoutOffset(ctx, 1032u, 40u);
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    record = frame;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    scratch = LoadU32(ctx, kCreateRecordPendingFlag);
    StoreU32(ctx, kCreateRecordPendingFlag, 0u);
    scratch = scratch == 1u;
  }
  if (scratch != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      state = FindMatchingCatch(ctx, 0u);
      value = HostTempRet0(ctx);
      state = HostRestorePendingThrow(ctx, state);
      StoreU32(ctx, kCreateRecordPendingFlag, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
      HostMarkRegionSeen(ctx, 1019u);
      resume_point = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      state = LoadU32(ctx, kCreateRecordPendingFlag);
      StoreU32(ctx, kCreateRecordPendingFlag, 0u);
      state = state == 1u;
    }
    if (state != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        state = HostRet0(ctx);
        value = HostTempRet0(ctx);
        StoreU32(ctx, kCreateRecordPendingFlag, 0u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
        HostMarkRegionSeen(ctx, 1021u);
        resume_point = 2u;
        if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        value = LoadU32(ctx, kCreateRecordPendingFlag);
        StoreU32(ctx, kCreateRecordPendingFlag, 0u);
        if (value != 1u) goto release_pending_and_trap;
        state = FindMatchingCatch(ctx, 0u);
        value = HostTempRet0(ctx);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
        Panic(ctx, state);
        resume_point = 3u;
        if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      TrapUnreachable();
    }
  }
  // Lay out the record header: refcounts, flags and the value slot table.
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU64(ctx, record + 4u, 4294967297ull);
    StoreU8(ctx, record + 16u, 0u);
    StoreU32(ctx, record + 12u, 0u);
    StoreU32(ctx, record, kCreateRecordVtable);
    value = LoadU32(ctx, value);
    StoreU32(ctx, record + 20u, kCreateRecordValueVtable);
    StoreU32(ctx, kCreateRecordPendingFlag, 0u);
    scratch = (value + 23u) & kCreateRecordSizeMask;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
    frame = HostFoldLayoutOffset(ctx, 1032u, scratch);
    resume_point = 4u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    scratch = frame;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    data = LoadU32(ctx, kCreateRecordPendingFlag);
    StoreU32(ctx, kCreateRecordPendingFlag, 0u);
    pending = data != 1u;
  } else {
    // Rewinding: reuse the flag word restored from the frame.
    pending = data;
  }
  // The layout call left a throw pending: release the record and rethrow.
  if (pending != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU64(ctx, scratch + 4u, 4294967297ull);
      StoreU32(ctx, scratch + 16u, value);
      StoreU32(ctx, scratch, kCreateRecordDataVtable);
      data = scratch + 20u;
      if (value != 0u) {
        FillBytes(ctx, data, 0u, value);
      }
      StoreU32(ctx, record + 36u, value);
      StoreU32(ctx, record + 32u, 0u);
      StoreU32(ctx, record + 28u, scratch);
      StoreU32(ctx, record + 24u, data);
      StoreU8(ctx, record + 16u, 1u);
      StoreU32(ctx, state + 4u, record);
      value = record + 20u;
      StoreU32(ctx, state, value);
      state = LoadU32(ctx, record + 4u);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (state != 0u) goto attach_done;
      state = LoadU32(ctx, LoadU32(ctx, record) + 8u);
      StoreU32(ctx, kCreateRecordPendingFlag, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
      HostReleaseSlotPair(ctx, state, record);
      resume_point = 5u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      state = LoadU32(ctx, kCreateRecordPendingFlag);
      StoreU32(ctx, kCreateRecordPendingFlag, 0u);
      state = state == 1u;
      if (state != 0u) goto rethrow_pending;
      state = LoadU32(ctx, record + 8u);
      value = state - 1u;
      StoreU32(ctx, record + 8u, value);
      state = state != 1u;
      if (state != 0u) goto attach_done;
      state = LoadU32(ctx, LoadU32(ctx, record) + 12u);
      StoreU32(ctx, kCreateRecordPendingFlag, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
      HostReleaseSlotPair(ctx, state, record);
      resume_point = 6u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      state = LoadU32(ctx, kCreateRecordPendingFlag);
      StoreU32(ctx, kCreateRecordPendingFlag, 0u);
      state = state == 1u;
      if (state != 0u) goto rethrow_pending;
    }
    attach_done:;
    if (ResumePhase(ctx) == kResumeRunning) {
      goto exit_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    state = HostRet0(ctx);
    HostTempRet0(ctx);
    value = LoadU32(ctx, record + 4u);
    StoreU32(ctx, record + 4u, value - 1u);
    value = value != 1u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (value != 0u) goto release_record_and_trap;
    value = LoadU32(ctx, LoadU32(ctx, record) + 8u);
    StoreU32(ctx, kCreateRecordPendingFlag, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 7u) {
    HostReleaseSlotPair(ctx, value, record);
    resume_point = 7u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    value = LoadU32(ctx, kCreateRecordPendingFlag);
    StoreU32(ctx, kCreateRecordPendingFlag, 0u);
    if (value == 1u) goto rethrow_pending;
    value = LoadU32(ctx, record + 8u);
    StoreU32(ctx, record + 8u, value - 1u);
    if (value != 1u) goto release_record_and_trap;
    value = LoadU32(ctx, LoadU32(ctx, record) + 12u);
    StoreU32(ctx, kCreateRecordPendingFlag, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 8u) {
    HostReleaseSlotPair(ctx, value, record);
    resume_point = 8u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    value = LoadU32(ctx, kCreateRecordPendingFlag);
    StoreU32(ctx, kCreateRecordPendingFlag, 0u);
    if (value == 1u) goto rethrow_pending;
  }
  // Failure paths: release the record slot, then trap out of the step.
  release_record_and_trap:;
  if (ResumePhase(ctx) == kResumeRunning) {
    HostOsRelease(ctx, state);
    TrapUnreachable();
  }
  release_pending_and_trap:;
  if (ResumePhase(ctx) == kResumeRunning) {
    HostOsRelease(ctx, state);
    TrapUnreachable();
  }
  // Rethrow the throw object the failed layout left pending.
  rethrow_pending:;
  if (ResumePhase(ctx) == kResumeRunning) {
    state = FindMatchingCatch(ctx, 0u);
    HostTempRet0(ctx);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 9u) {
    Panic(ctx, state);
    resume_point = 9u;
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
           LoadU32(ctx, ReturnFrameSp(ctx)) + kCreateRecordResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, value);
  StoreU32(ctx, frame + 8u, record);
  StoreU32(ctx, frame + 12u, scratch);
  StoreU32(ctx, frame + 16u, data);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kCreateRecordSaveFrameBytes);
  exit_path:;
  LeaveFrame();
}

}  // namespace artc
