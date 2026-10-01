#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
uint32_t StringLength(EngineContext* ctx, uint32_t text);
uint32_t InternString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t MeasureFormattedNumber(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

constexpr uint32_t kNumberToStringFormatMessage = 0x2BED9u;  // "%.14g"
constexpr uint32_t kNumberToStringBufferBytes = 48u;  // room for the formatted text
constexpr uint32_t kNumberToStringFormatOffset = 16u;  // where the text starts in the buffer
constexpr uint32_t kNumberToStringNumberTag = 3u;  // the tag of a number value
constexpr uint32_t kNumberToStringStringTag = 4u;  // the tag it gets once converted
constexpr uint32_t kNumberToStringSaveFrameBytes = 20u;
constexpr uint32_t kNumberToStringResumeWordBytes = 4u;

}  // namespace

// Converts a number value into its string form: the double is formatted with
// "%.14g" into a scratch buffer on the guest stack, the text is measured and
// interned, and the value is rewritten in place as a string.  A value that is
// not a number is left alone.  The value being converted is a tagged record --
// +8 holds the type tag, +0 the payload.
//
// Suspendable: both calls can suspend, so every value that outlives a suspend
// goes into the frame below, and the resume code (0 or 1) names the call that
// has to be replayed.
uint32_t NumberToString(EngineContext* ctx, uint32_t value, uint32_t options) {
  EnterFrame();
  uint32_t text = 0u;
  uint32_t buffer = 0u;
  uint32_t count = 0u;
  uint32_t resume_point = 0u;
  uint32_t result = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 20-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kNumberToStringSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    value = LoadU32(ctx, frame);
    text = LoadU32(ctx, frame + 8u);
    buffer = LoadU32(ctx, frame + 12u);
    count = LoadU32(ctx, frame + 16u);
    options = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame that saves the resume code.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kNumberToStringResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    buffer = GuestStackSp(ctx) - kNumberToStringBufferBytes;
    GuestStackSp(ctx) = buffer;
    text = static_cast<uint32_t>(LoadU32(ctx, options + 8u) == kNumberToStringNumberTag);
  }
  // A value that is not a number keeps the text it already has; the rewinding
  // pass still walks the block to pick up the string the intern call produced.
  if (text != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreF64(ctx, buffer, LoadF64(ctx, options));
      text = buffer + kNumberToStringFormatOffset;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      uint32_t const called =
          MeasureFormattedNumber(ctx, text, kNumberToStringFormatMessage, buffer);
      resume_point = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      text = called;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      count = StringLength(ctx, buffer + kNumberToStringFormatOffset);
      text = buffer + kNumberToStringFormatOffset;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
      uint32_t const called = InternString(ctx, value, text, count);
      resume_point = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      value = called;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, options + 8u, kNumberToStringStringTag);
      StoreU32(ctx, options, value);
      count = 1u;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = buffer + kNumberToStringBufferBytes;
    result = count;
    goto exit_path;
  }
  TrapUnreachable();
suspend_point:
  // Suspend: write the resume code, then push the frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kNumberToStringResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, value);
    StoreU32(ctx, frame + 4u, options);
    StoreU32(ctx, frame + 8u, text);
    StoreU32(ctx, frame + 12u, buffer);
    StoreU32(ctx, frame + 16u, count);
  }
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kNumberToStringSaveFrameBytes);
exit_path:
  LeaveFrame();
  return result;
}

}  // namespace artc
