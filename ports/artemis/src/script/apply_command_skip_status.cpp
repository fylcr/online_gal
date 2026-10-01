#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void Panic(EngineContext* ctx, uint32_t a0);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
uint32_t FormatLuaEventFilterLine(EngineContext* ctx, uint32_t a0, uint32_t a1);

namespace {

constexpr uint32_t kApplySkipKeyLow = 0xB4A2Eu;  // first 8 bytes of "s.status.commandskip"
constexpr uint32_t kApplySkipKeyHigh = 0xB4A36u;  // next 8 bytes of it
constexpr uint32_t kApplySkipKeyTail = 0xB4A3Eu;  // last 4 bytes of it
constexpr uint32_t kApplySkipAssertPx = 0xB5E31u;  // assertion text "px != 0"
constexpr uint32_t kApplySkipAssertFile = 0xB5E39u;  // the boost header path it names
constexpr uint32_t kApplySkipAssertFunc = 0xB5E71u;  // assertion text "operator->"
constexpr uint32_t kApplySkipStatusSlot = 0xD015Cu;  // status word the host reads
constexpr uint32_t kApplySkipNegativeOne = 0xFFFFFFFFu;  // the "release the block" mark
constexpr uint64_t kApplySkipRecordTag = 9223372174293729300ull;  // the record's tag pair
constexpr uint32_t kApplySkipSaveFrameBytes = 20u;  // saved state words below the resume code
constexpr uint32_t kApplySkipResumeWordBytes = 4u;  // resume code slot

}  // namespace

// Publishes the command-skip status: the flag is written onto the script state
// and the pending command step is released.  Several of the steps below can
// suspend, so the arguments and the two working pointers live in the frame.
void ApplyCommandSkipStatus(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t record = 0u;    // the 16-byte record taken off the guest stack
  uint32_t step = 0u;      // the pending command step (a plain flag before that)
  uint32_t result = 0u;    // what a suspendable call hands back
  uint32_t status = 0u;    // the status word the step left behind
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 20-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kApplySkipSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    a = LoadU32(ctx, frame);
    c = LoadU32(ctx, frame + 8u);
    record = LoadU32(ctx, frame + 12u);
    step = LoadU32(ctx, frame + 16u);
    b = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame that saves the resume code.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kApplySkipResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A 16-byte record is taken off the guest stack for the step below.
    record = GuestStackSp(ctx) - 16u;
    GuestStackSp(ctx) = record;
    step = b == 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (step == 0u && c == 0u && LoadU8(ctx, a + 48u) == 0u) { goto release_scratch; }
    if (LoadU8(ctx, a + 49u) == b) { goto release_scratch; }
    // The skip flag changed: write it down and take the pending step.
    StoreU8(ctx, a + 49u, b);
    step = LoadU32(ctx, a + 112u);
    c = step == 0u;
  }
  if (c != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The host reads the status slot from zero.
      StoreU32(ctx, kApplySkipStatusSlot, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      // The assertion the pending step expected to have passed.
      HostFailAssertion(ctx, 857u, kApplySkipAssertPx, kApplySkipAssertFile, 734u,
          kApplySkipAssertFunc);
      resume_point = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The step answered through the status slot rather than trapping.
      a = LoadU32(ctx, kApplySkipStatusSlot);
      StoreU32(ctx, kApplySkipStatusSlot, 0u);
      if (a != 1u) { goto trap_path; }
      a = FindMatchingCatch(ctx, 0u);
      b = HostTempRet0(ctx);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
      Panic(ctx, a);
      resume_point = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) { TrapUnreachable(); }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
    result = AllocateStringBuffer(ctx, 32u);
    resume_point = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    c = result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Fill the record: the step, its tag pair, and the key it looks up.
    StoreU32(ctx, record, c);
    StoreU64(ctx, record + 4u, kApplySkipRecordTag);
    StoreU8(ctx, c + 20u, 0u);
    StoreU32(ctx, c + 16u, LoadU32(ctx, kApplySkipKeyTail));
    StoreU64(ctx, c + 8u, LoadU64(ctx, kApplySkipKeyHigh));
    StoreU64(ctx, c, LoadU64(ctx, kApplySkipKeyLow));
    c = LoadU32(ctx, LoadU32(ctx, step) + 20u);
    StoreU32(ctx, kApplySkipStatusSlot, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
    result = HostDispatchInvoke(ctx, c, step, record, b);
    resume_point = 3u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    c = result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The dispatch may have answered through the status slot as well.
    c = LoadU32(ctx, kApplySkipStatusSlot);
    StoreU32(ctx, kApplySkipStatusSlot, 0u);
    status = c != 1u;
  } else {
    status = c;
  }
  if (status != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // Release the block the record owned, then name the next step.
      if ((int32_t)LoadS8(ctx, record + 11u) <= (int32_t)kApplySkipNegativeOne) {
        DeallocateRawBlock(ctx, LoadU32(ctx, record));
      }
      b = b == 0u;
      if (b != 0u) { goto publish_step; }
      b = a + 13852u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
      result = FormatLuaEventFilterLine(ctx, a, b);
      resume_point = 4u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      a = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto release_scratch; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The step trapped: release everything, then hand the status to the host.
    a = HostRet0(ctx);
    HostTempRet0(ctx);
    if ((int32_t)LoadS8(ctx, record + 11u) <= (int32_t)kApplySkipNegativeOne) {
      DeallocateRawBlock(ctx, LoadU32(ctx, record));
    }
    HostOsRelease(ctx, a);
    TrapUnreachable();
  }
publish_step:
  if (ResumePhase(ctx) == kResumeRunning) {
    b = a + 13896u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
    FormatLuaEventFilterLine(ctx, a, b);
    resume_point = 5u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
release_scratch:
  if (ResumePhase(ctx) == kResumeRunning) {
    // Give the 16-byte record back to the guest stack.
    GuestStackSp(ctx) = record + 16u;
    goto exit_path;
  }
trap_path:
  if (ResumePhase(ctx) == kResumeRunning) { TrapUnreachable(); }
  goto exit_path;
suspend_point:
  // Suspend: write the resume code, then push the frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kApplySkipResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, a);
    StoreU32(ctx, frame + 4u, b);
    StoreU32(ctx, frame + 8u, c);
    StoreU32(ctx, frame + 12u, record);
    StoreU32(ctx, frame + 16u, step);
  }
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kApplySkipSaveFrameBytes);
exit_path:
  LeaveFrame();
}

}  // namespace artc
