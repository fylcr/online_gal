#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step drives.
void MatchKeyword(EngineContext* ctx, uint32_t a, uint32_t b);
uint32_t FormatPushString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
void StepRecordResume(EngineContext* ctx, uint32_t a0);
uint32_t CheckStatementLimits(EngineContext* ctx, uint32_t state, uint32_t func_state,
                              uint32_t message);
uint32_t FormatCharValue(EngineContext* ctx, uint32_t a, uint32_t b);
uint32_t ExpressionToRegister(EngineContext* ctx, uint32_t func_state, uint32_t expr);
void LexError(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t EmitOpInstruction(EngineContext* ctx, uint32_t func_state, uint32_t op, uint32_t arg_a,
                           uint32_t arg_b, uint32_t line);
uint32_t EncodeConstant(EngineContext* ctx, uint32_t func_state, uint32_t value);

namespace {

// The four pool strings this step formats its diagnosics from.
constexpr uint32_t kRaiseItemLimitMsgExpected = 0x2B625u;          // "%s expected"
constexpr uint32_t kRaiseItemLimitMsgConstructorItems = 0x2B6A9u;  // "items in a constructor"
constexpr uint32_t kRaiseItemLimitMsgMainTooMany = 0x2B6C0u;       // "main function has more ..."
constexpr uint32_t kRaiseItemLimitMsgFunctionTooMany = 0x2B6E2u;   // "function at line ..."
// Format-item tags and the counters they carry.
constexpr uint32_t kRaiseItemLimitItemMarker = 0x7FFFFFFDu;
constexpr uint32_t kRaiseItemLimitCountCap = 0x7FFFFFFEu;
// Scratch minus this is scratch + 64: the stack area the message items live in.
constexpr uint32_t kRaiseItemLimitFrameBackset = 0xFFFFFFC0u;
constexpr uint32_t kRaiseItemLimitSaveFrameBytes = 28u;
constexpr uint32_t kRaiseItemLimitResumeWordBytes = 4u;

}  // namespace

// Reports an item limit or a missing token: it formats the "more than %d %s"
// diagnostic (or the "'%s' expected" one) into the scratch area, raises it
// through the lexer, and compiles the constant the failed item would have
// produced.  Suspendable: the formatters, the lexer and the encoder can all
// suspend, so the live state spills into the suspend frame.
void RaiseItemLimitError(EngineContext* ctx, uint32_t state, uint32_t limit) {
  EnterFrame();
  uint32_t value = 0u;
  uint32_t result = 0u;
  uint32_t ptr = 0u;
  uint32_t scratch = 0u;
  uint32_t func_state = 0u;
  uint32_t freereg = 0u;
  uint32_t flag = 0u;
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 28-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kRaiseItemLimitSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    limit = LoadU32(ctx, frame + 4u);
    value = LoadU32(ctx, frame + 8u);
    ptr = LoadU32(ctx, frame + 12u);
    scratch = LoadU32(ctx, frame + 16u);
    func_state = LoadU32(ctx, frame + 20u);
    freereg = LoadU32(ctx, frame + 24u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame that saves the resume point.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kRaiseItemLimitResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Claim the 112-byte scratch area the format items are built in.
    scratch = GuestStackSp(ctx) - 112u;
    GuestStackSp(ctx) = scratch;
    func_state = LoadU32(ctx, state + 48u);
    freereg = LoadU32(ctx, func_state + 36u);
    value = LoadU32(ctx, state + 16u) == 285u;
  }
  if (value != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      value = (uint32_t)((int32_t)LoadU32(ctx, limit + 28u) < (int32_t)kRaiseItemLimitCountCap);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Still below the cap: nothing to report, jump straight to the tail.
      flag = value;
      if (flag != 0u) { goto error_done; }
      value = LoadU32(ctx, func_state + 16u);
      ptr = LoadU32(ctx, LoadU32(ctx, func_state) + 60u);
      flag = ptr == 0u;
    }
    if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        // One format item: the "items in a constructor" text.
        StoreU32(ctx, scratch + 36u, kRaiseItemLimitMsgConstructorItems);
        StoreU32(ctx, scratch + 32u, kRaiseItemLimitItemMarker);
        ptr = scratch + 32u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
        result = FormatPushString(ctx, value, kRaiseItemLimitMsgMainTooMany, ptr);
        resume_point = 0u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        value = result;
      }
      if (ResumePhase(ctx) == kResumeRunning) { goto message_built; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The other shape of the same message: the text, the marker and the
      // count being reported.
      StoreU32(ctx, scratch + 56u, kRaiseItemLimitMsgConstructorItems);
      StoreU32(ctx, scratch + 52u, kRaiseItemLimitItemMarker);
      StoreU32(ctx, scratch + 48u, ptr);
      ptr = scratch + 48u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
      result = FormatPushString(ctx, value, kRaiseItemLimitMsgFunctionTooMany, ptr);
      resume_point = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      value = result;
    }
  message_built:
    if (ResumePhase(ctx) == kResumeRunning) {
      ptr = LoadU32(ctx, func_state + 12u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
      LexError(ctx, ptr, value, 0u);
      resume_point = 2u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      value = LoadU32(ctx, state + 16u) == 285u;
      if (value != 0u) { goto error_done; }
      value = LoadU32(ctx, state + 52u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
      result = FormatCharValue(ctx, state, 285u);
      resume_point = 3u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      ptr = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, scratch + 16u, ptr);
      ptr = scratch + 16u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
      result = FormatPushString(ctx, value, kRaiseItemLimitMsgExpected, ptr);
      resume_point = 4u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      value = result;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
      RaiseSyntaxError(ctx, state, value);
      resume_point = 5u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
  }
error_done:
  if (ResumePhase(ctx) == kResumeRunning) {
    value = LoadU32(ctx, state + 24u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
    StepRecordResume(ctx, state);
    resume_point = 6u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ptr = LoadU32(ctx, state + 48u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 7u) {
    result = EncodeConstant(ctx, ptr, value);
    resume_point = 7u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    value = result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A string item is already a constant: its expression is set up here with
    // no jump lists.
    StoreU64(ctx, scratch + 104u, 18446744073709551615ull);
    StoreU32(ctx, scratch + 96u, value);
    StoreU32(ctx, scratch + 88u, 4u);
    goto step_tail;
  }
  value = (ResumePhase(ctx) != kResumeRunning) ? value : (scratch + 88u);
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 8u) {
    MatchKeyword(ctx, state, value);
    resume_point = 8u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
step_tail:
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, limit + 28u, LoadU32(ctx, limit + 28u) + 1u);
    value = LoadU32(ctx, state + 16u) != 61u;
  }
  if (value != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      value = LoadU32(ctx, state + 52u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 9u) {
      result = FormatCharValue(ctx, state, 61u);
      resume_point = 9u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      ptr = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, scratch, ptr);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 10u) {
      result = FormatPushString(ctx, value, kRaiseItemLimitMsgExpected, scratch);
      resume_point = 10u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      value = result;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 11u) {
      RaiseSyntaxError(ctx, state, value);
      resume_point = 11u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 12u) {
    StepRecordResume(ctx, state);
    resume_point = 12u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  value = (ResumePhase(ctx) != kResumeRunning) ? value : (scratch + 88u);
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 13u) {
    result = ExpressionToRegister(ctx, func_state, value);
    resume_point = 13u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    value = result;
  }
  ptr = (ResumePhase(ctx) != kResumeRunning) ? ptr : (scratch - kRaiseItemLimitFrameBackset);
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 14u) {
    result = CheckStatementLimits(ctx, state, ptr, 0u);
    resume_point = 14u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    state = result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    state = LoadU32(ctx, LoadU32(ctx, limit + 24u) + 8u);
    limit = scratch - kRaiseItemLimitFrameBackset;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 15u) {
    result = ExpressionToRegister(ctx, func_state, limit);
    resume_point = 15u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    limit = result;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 16u) {
    EmitOpInstruction(ctx, func_state, 9u, state, value, limit);
    resume_point = 16u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, func_state + 36u, freereg);
    GuestStackSp(ctx) = scratch + 112u;
  }
  goto exit_path;
suspend_point:
  // Suspend: record the resume point, then push the frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kRaiseItemLimitResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, state);
    StoreU32(ctx, frame + 4u, limit);
    StoreU32(ctx, frame + 8u, value);
    StoreU32(ctx, frame + 12u, ptr);
    StoreU32(ctx, frame + 16u, scratch);
    StoreU32(ctx, frame + 20u, func_state);
    StoreU32(ctx, frame + 24u, freereg);
  }
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kRaiseItemLimitSaveFrameBytes);
exit_path:
  LeaveFrame();
}

}  // namespace artc
