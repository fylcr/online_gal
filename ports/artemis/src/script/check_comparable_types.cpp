#include "util/engine_context.h"

namespace artc {

// Forward declaration for the callee this step drives.
void RaiseScriptError(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

namespace {

constexpr uint32_t kCompareMsgTwoValues = 0x2A776u;  // attempt to compare two %s values
constexpr uint32_t kCompareMsgWithValues = 0x2A797u;  // attempt to compare %s with %s
constexpr uint32_t kCompareClassWordTable = 0x2BD80u;  // record pointer per value class
constexpr uint32_t kCheckComparableTypesSaveFrameBytes = 16u;
constexpr uint32_t kCheckComparableTypesResumeWordBytes = 4u;

}  // namespace

// Rejects an operand pair that cannot be compared, before the comparison is
// emitted.  Both operands are slots; the record a slot's class word points at
// carries the class tag at byte 2.  Suspendable: either error call can suspend,
// so the frame below holds the operands and the resume code names the call.
uint32_t CheckComparableTypes(EngineContext* ctx, uint32_t state, uint32_t left, uint32_t right) {
  EnterFrame();
  uint32_t scratch = 0u;
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 16-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kCheckComparableTypesSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    left = LoadU32(ctx, frame + 4u);
    right = LoadU32(ctx, frame + 8u);
    scratch = LoadU32(ctx, frame + 12u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame that saves the resume code.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kCheckComparableTypesResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  // Take the scratch record, then resolve both operands and compare their class
  // tags.  A rewind pass skips the comparison and replays the error it suspended
  // inside, which is what forcing the flag below is for.
  uint32_t same_class = 0u;
  if (ResumePhase(ctx) == kResumeRunning) {
    scratch = GuestStackSp(ctx) - 32u;
    GuestStackSp(ctx) = scratch;
    left = LoadU32(ctx, kCompareClassWordTable + (LoadU32(ctx, left + 8u) << 2));
    uint32_t const left_tag = LoadU8(ctx, left + 2u);
    right = LoadU32(ctx, kCompareClassWordTable + (LoadU32(ctx, right + 8u) << 2));
    same_class = static_cast<uint32_t>(left_tag == LoadU8(ctx, right + 2u));
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    same_class = 1u;
  }
  if (same_class != 0u) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The message reads the operand back out of the scratch record.
      StoreU32(ctx, scratch, left);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      RaiseScriptError(ctx, state, kCompareMsgTwoValues, scratch);
      resume_point = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto finish; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The two-operand message reads a pair of slots, so the pair goes into the
    // record and the pointer to it is the argument.
    StoreU32(ctx, scratch + 20u, right);
    StoreU32(ctx, scratch + 16u, left);
    left = scratch + 16u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    RaiseScriptError(ctx, state, kCompareMsgWithValues, left);
    resume_point = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
finish:
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = scratch + 32u;
    goto exit_path;
  }
  TrapUnreachable();
suspend_point:
  // Suspend: write the resume code, then push the frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kCheckComparableTypesResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, state);
    StoreU32(ctx, frame + 4u, left);
    StoreU32(ctx, frame + 8u, right);
    StoreU32(ctx, frame + 12u, scratch);
  }
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kCheckComparableTypesSaveFrameBytes);
exit_path:
  LeaveFrame();
  return 0u;
}

}  // namespace artc
