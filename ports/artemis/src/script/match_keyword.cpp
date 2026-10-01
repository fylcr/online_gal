#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
uint32_t FormatPushString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
void StepRecordResume(EngineContext* ctx, uint32_t a0);
uint32_t CheckStatementLimits(EngineContext* ctx, uint32_t state, uint32_t func_state, uint32_t message);
uint32_t FormatCharValue(EngineContext* ctx, uint32_t a, uint32_t b);
void CompileKeywordCondition(EngineContext* ctx, uint32_t a, uint32_t b);

namespace {

constexpr uint32_t kMatchKeywordExpectedMessage = 0x2B625u;  // "'%s' expected"
constexpr uint32_t kMatchKeywordCloseBracket = 93u;          // ']'
constexpr uint32_t kMatchKeywordSaveFrameBytes = 16u;        // words saved below the resume code
constexpr uint32_t kMatchKeywordResumeWordBytes = 4u;        // resume code slot

}  // namespace

// Matches a keyword: the lexer's keyword table is consulted and the keyword id
// is answered.  The calls can suspend, so the parser state, the keyword, the
// working value and the formatter's scratch record are kept in the frame.
void MatchKeyword(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t value = 0u;         // the keyword's slot, then the formatter's argument
  uint32_t scratch = 0u;       // the 16-byte formatter record on the guest stack
  uint32_t called = 0u;        // result of the call in the gate above
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 16-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kMatchKeywordSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    a = LoadU32(ctx, frame);
    value = LoadU32(ctx, frame + 8u);
    scratch = LoadU32(ctx, frame + 12u);
    b = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame that saves the resume code.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kMatchKeywordResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    scratch = GuestStackSp(ctx) - 16u;
    GuestStackSp(ctx) = scratch;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    StepRecordResume(ctx, a);
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    called = CheckStatementLimits(ctx, a, b, 0u);
    resume_point = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    value = called;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    value = LoadU32(ctx, a + 48u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
    CompileKeywordCondition(ctx, value, b);
    resume_point = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    b = LoadU32(ctx, a + 16u) != kMatchKeywordCloseBracket;
  }
  if (b != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      b = LoadU32(ctx, a + 52u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
      called = FormatCharValue(ctx, a, kMatchKeywordCloseBracket);
      resume_point = 3u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      value = called;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, scratch, value);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
      called = FormatPushString(ctx, b, kMatchKeywordExpectedMessage, scratch);
      resume_point = 4u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      b = called;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
      RaiseSyntaxError(ctx, a, b);
      resume_point = 5u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
    StepRecordResume(ctx, a);
    resume_point = 6u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = scratch + 16u;
  }
  goto exit_path;
suspend_point:
  // Suspend: write the resume code, then push the frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kMatchKeywordResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, a);
    StoreU32(ctx, frame + 4u, b);
    StoreU32(ctx, frame + 8u, value);
    StoreU32(ctx, frame + 12u, scratch);
  }
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kMatchKeywordSaveFrameBytes);
exit_path:
  LeaveFrame();
}

}  // namespace artc
