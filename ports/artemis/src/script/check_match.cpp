#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step calls.
void ExpectToken(EngineContext* ctx, uint32_t a, uint32_t b);
uint32_t AppendWithLimit(EngineContext* ctx, uint32_t list, uint32_t entry, uint32_t limit, uint32_t what);
void MatchKeyword(EngineContext* ctx, uint32_t a, uint32_t b);
void ParseArguments(EngineContext* ctx, uint32_t a, uint32_t b);
void EmitMoveSlot(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t FormatPushString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
void StepRecordResume(EngineContext* ctx, uint32_t a0);
uint32_t CheckStatementLimits(EngineContext* ctx, uint32_t state, uint32_t func_state, uint32_t message);
uint32_t FormatCharValue(EngineContext* ctx, uint32_t a, uint32_t b);
void CheckExprComplexity(EngineContext* ctx, uint32_t a, uint32_t b);
void DischargeExpression(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t EmitConditionJump(EngineContext* ctx, uint32_t func_state, uint32_t condition);
void CheckClosingMatch(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d);
void EmitJumpIf(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t EncodeConstant(EngineContext* ctx, uint32_t func_state, uint32_t value);

namespace {

// Message templates the raising step formats into the scratch record.
constexpr uint32_t kCheckMatchMsgExpected = 0x2B625u;  // '%s' expected
constexpr uint32_t kCheckMatchMsgUnexpected = 0x2B76Du;  // unexpected symbol
constexpr uint32_t kCheckMatchSaveFrameBytes = 28u;
constexpr uint32_t kCheckMatchResumeWordBytes = 4u;

}  // namespace

// Consumes the token `expected` from the lexer and records the match on the
// expectation record.  When some other token turns up instead the step raises
// the syntax error that names what showed up and where.  The 32-byte scratch
// record parked on the guest stack carries the pieces of that message.
void CheckMatch(EngineContext* ctx, uint32_t lex_state, uint32_t expected) {
  EnterFrame();
  // Live across suspensions; also a scratch cell for intermediate values.
  uint32_t token = 0u;
  uint32_t scratch = 0u;
  uint32_t compiler = 0u;
  uint32_t flag = 0u;
  uint32_t appended = 0u;
  uint32_t resume_point = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kCheckMatchSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    lex_state = LoadU32(ctx, frame);
    token = LoadU32(ctx, frame + 8u);
    flag = LoadU32(ctx, frame + 12u);
    scratch = LoadU32(ctx, frame + 16u);
    compiler = LoadU32(ctx, frame + 20u);
    appended = LoadU32(ctx, frame + 24u);
    expected = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // The resume word sits one slot below the frame the spill pushed.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kCheckMatchResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    scratch = GuestStackSp(ctx) - 32u;
    GuestStackSp(ctx) = scratch;
    compiler = LoadU32(ctx, lex_state + 48u);
    token = LoadU32(ctx, lex_state + 16u);
    flag = token != 285u;
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // Only the end-of-statement marker gets matched here; anything else is
      // the "unexpected symbol" case.
      if (token != 40u) { goto unexpected_symbol; }
      token = LoadU32(ctx, lex_state + 4u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      StepRecordResume(ctx, lex_state);
      resume_point = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
      flag = CheckStatementLimits(ctx, lex_state, expected, 0u);
      resume_point = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
      CheckClosingMatch(ctx, lex_state, 41u, 40u, token);
      resume_point = 2u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      token = LoadU32(ctx, lex_state + 48u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
      DischargeExpression(ctx, token, expected);
      resume_point = 3u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto dispatch; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    token = LoadU32(ctx, lex_state + 24u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
    StepRecordResume(ctx, lex_state);
    resume_point = 4u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    flag = LoadU32(ctx, lex_state + 48u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
    appended = AppendWithLimit(ctx, flag, token, expected, 1u);
    resume_point = 5u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning && appended != 8u) { goto dispatch; }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
    token = EncodeConstant(ctx, flag, token);
    resume_point = 6u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, expected + 8u, token);
    goto dispatch;
  }
unexpected_symbol:
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 7u) {
    RaiseSyntaxError(ctx, lex_state, kCheckMatchMsgUnexpected);
    resume_point = 7u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
dispatch:
  if (ResumePhase(ctx) == kResumeRunning) {
    token = LoadU32(ctx, lex_state + 16u);
    flag = static_cast<uint32_t>(static_cast<int32_t>(token) <= 90);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (flag != 0u) {
      // Punctuation that opens a construct ends the step differently from the
      // tokens that simply close it.
      switch (token - 40u) {
        case 0: goto check_complexity;
        case 6: goto expect_token;
        case 18: goto raise_expected;
        default: goto done;
      }
    }
    flag = static_cast<uint32_t>(token == 91u);
    if (flag != 0u) { goto emit_condition; }
    flag = static_cast<uint32_t>(token == 123u);
    if (flag != 0u) { goto check_complexity; }
    token = static_cast<uint32_t>(token != 286u);
    if (token != 0u) { goto done; }
  check_complexity:;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 8u) {
    CheckExprComplexity(ctx, compiler, expected);
    resume_point = 8u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 9u) {
    ParseArguments(ctx, lex_state, expected);
    resume_point = 9u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) { goto dispatch; }
expect_token:
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 10u) {
    ExpectToken(ctx, lex_state, expected);
    resume_point = 10u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) { goto dispatch; }
emit_condition:
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 11u) {
    uint32_t const emitted = EmitConditionJump(ctx, compiler, expected);
    resume_point = 11u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    token = ResumePhase(ctx) != kResumeRunning ? emitted : scratch + 8u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 12u) {
    MatchKeyword(ctx, lex_state, token);
    resume_point = 12u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    token = scratch + 8u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 13u) {
    EmitMoveSlot(ctx, compiler, expected, token);
    resume_point = 13u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) { goto dispatch; }
raise_expected:
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 14u) {
    StepRecordResume(ctx, lex_state);
    resume_point = 14u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Anything that is not the statement terminator is worth reporting.
    token = static_cast<uint32_t>(LoadU32(ctx, lex_state + 16u) != 285u);
  }
  if (token != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      token = LoadU32(ctx, lex_state + 52u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 15u) {
      flag = FormatCharValue(ctx, lex_state, 285u);
      resume_point = 15u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, scratch, flag);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 16u) {
      token = FormatPushString(ctx, token, kCheckMatchMsgExpected, scratch);
      resume_point = 16u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 17u) {
      RaiseSyntaxError(ctx, lex_state, token);
      resume_point = 17u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    token = LoadU32(ctx, lex_state + 24u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 18u) {
    StepRecordResume(ctx, lex_state);
    resume_point = 18u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    flag = LoadU32(ctx, lex_state + 48u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 19u) {
    token = EncodeConstant(ctx, flag, token);
    resume_point = 19u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Fill the record the jump is emitted against: a one-slot list holding the
    // constant that was just encoded.
    StoreU64(ctx, scratch + 24u, 0xFFFFFFFFFFFFFFFFull);
    StoreU32(ctx, scratch + 16u, token);
    StoreU32(ctx, scratch + 8u, 4u);
    token = scratch + 8u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 20u) {
    EmitJumpIf(ctx, compiler, expected, token);
    resume_point = 20u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 21u) {
    ParseArguments(ctx, lex_state, expected);
    resume_point = 21u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) { goto dispatch; }
done:
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = scratch + 32u;
  }
  goto exit_path;
suspend_point:
  // Suspend: record the resume point, then push the frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kCheckMatchResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, lex_state);
    StoreU32(ctx, frame + 4u, expected);
    StoreU32(ctx, frame + 8u, token);
    StoreU32(ctx, frame + 12u, flag);
    StoreU32(ctx, frame + 16u, scratch);
    StoreU32(ctx, frame + 20u, compiler);
    StoreU32(ctx, frame + 24u, appended);
  }
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kCheckMatchSaveFrameBytes);
exit_path:
  LeaveFrame();
}

}  // namespace artc
