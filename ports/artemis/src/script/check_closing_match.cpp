#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
uint32_t FormatPushString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
void StepRecordResume(EngineContext* ctx, uint32_t a0);
uint32_t FormatCharValue(EngineContext* ctx, uint32_t a, uint32_t b);

namespace {

constexpr uint32_t kCheckClosingExpectedMessage = 0x2B625u;  // "'%s' expected"
constexpr uint32_t kCheckClosingExpectedToCloseMessage = 0x2B7D6u;  // the close message
constexpr uint32_t kCheckClosingSaveFrameBytes = 28u;  // saved state words below the resume code
constexpr uint32_t kCheckClosingResumeWordBytes = 4u;  // resume code slot

}  // namespace

// Checks a closing token against the opener on the block stack: a match is
// recorded and the tokens are walked, a mismatch raises a syntax error.  The
// steps that format and raise can suspend, so the arguments and the working
// pointers are kept in the frame.
void CheckClosingMatch(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
  EnterFrame();
  uint32_t scratch = 0u;   // the 32-byte record taken off the guest stack
  uint32_t opener = 0u;    // the token the block stack expects
  uint32_t target = 0u;    // the message target (a match flag before that)
  uint32_t matched = 0u;   // whether the closer is the expected token
  uint32_t result = 0u;    // what a formatter hands back
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 28-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kCheckClosingSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    a = LoadU32(ctx, frame);
    c = LoadU32(ctx, frame + 8u);
    d = LoadU32(ctx, frame + 12u);
    scratch = LoadU32(ctx, frame + 16u);
    target = LoadU32(ctx, frame + 20u);
    opener = LoadU32(ctx, frame + 24u);
    b = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame that saves the resume code.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kCheckClosingResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A 32-byte record is taken off the guest stack for the closer below.
    scratch = GuestStackSp(ctx) - 32u;
    GuestStackSp(ctx) = scratch;
    target = LoadU32(ctx, a + 16u) == b;
  }
  if (target != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      // The closer is the one the stack expects, so the step is recorded.
      StepRecordResume(ctx, a);
      resume_point = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto release_scratch; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    opener = LoadU32(ctx, a + 4u);
    target = LoadU32(ctx, a + 52u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    result = FormatCharValue(ctx, a, b);
    resume_point = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    b = result;
  }
  // Only a running pass still holds the closer: past that phase the opener
  // alone decides whether the tail below runs.
  matched = ResumePhase(ctx) ? opener : static_cast<uint32_t>(d == opener);
  if (matched != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The closer lands in the record the formatter reads.
      StoreU32(ctx, scratch, b);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
      result = FormatPushString(ctx, target, kCheckClosingExpectedMessage, scratch);
      resume_point = 2u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      b = result;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
      RaiseSyntaxError(ctx, a, b);
      resume_point = 3u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto release_scratch; }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
    result = FormatCharValue(ctx, a, c);
    resume_point = 4u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    c = result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The record is handed out: the two tokens and the line they sit on.
    StoreU32(ctx, scratch + 24u, d);
    StoreU32(ctx, scratch + 20u, c);
    StoreU32(ctx, scratch + 16u, b);
    b = scratch + 16u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
    result = FormatPushString(ctx, target, kCheckClosingExpectedToCloseMessage, b);
    resume_point = 5u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    b = result;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
    RaiseSyntaxError(ctx, a, b);
    resume_point = 6u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
release_scratch:
  if (ResumePhase(ctx) == kResumeRunning) {
    // Give the 32-byte record back to the guest stack.
    GuestStackSp(ctx) = scratch + 32u;
  }
  goto exit_path;
suspend_point:
  // Suspend: write the resume code, then push the frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kCheckClosingResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, a);
    StoreU32(ctx, frame + 4u, b);
    StoreU32(ctx, frame + 8u, c);
    StoreU32(ctx, frame + 12u, d);
    StoreU32(ctx, frame + 16u, scratch);
    StoreU32(ctx, frame + 20u, target);
    StoreU32(ctx, frame + 24u, opener);
  }
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kCheckClosingSaveFrameBytes);
exit_path:
  LeaveFrame();
}

}  // namespace artc
