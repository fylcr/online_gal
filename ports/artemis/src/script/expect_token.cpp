#include "util/engine_context.h"

namespace artc {

// The calls this step makes, each one defined in its own file.
uint32_t EmitConditionJump(EngineContext* ctx, uint32_t func_state, uint32_t condition);
void EmitMoveSlot(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t FormatPushString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t FormatCharValue(EngineContext* ctx, uint32_t a, uint32_t b);
void StepRecordResume(EngineContext* ctx, uint32_t a0);
uint32_t EncodeConstant(EngineContext* ctx, uint32_t func_state, uint32_t value);
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);

namespace {

// The text the raise carries: '%s' expected.
constexpr uint32_t kExpectTokenExpectedText = 0x2B625u;

// The token kind this step expects. A different kind is what makes the raise
// below name the character value instead.
constexpr uint32_t kExpectTokenExpectedKind = 285u;

// The value record the step reserves on the guest stack for the token it moves:
// its tag, its value and the upper word that says the value has no second half.
constexpr uint32_t kExpectTokenRecordBytes = 32u;
constexpr uint32_t kExpectTokenTagOffset = 8u;
constexpr uint32_t kExpectTokenValueOffset = 16u;
constexpr uint32_t kExpectTokenUpperOffset = 24u;
constexpr uint32_t kExpectTokenTag = 4u;
constexpr uint64_t kExpectTokenNoValue = 0xFFFFFFFFFFFFFFFFull;

// The suspend frame: the words this body saves, and the resume word pushed with
// them, whose number names the call that was in flight.
constexpr uint32_t kExpectTokenSaveFrameBytes = 24u;
constexpr uint32_t kExpectTokenResumeWordBytes = 4u;
constexpr uint32_t kExpectTokenJumpCall = 0u;
constexpr uint32_t kExpectTokenTrackerCall = 1u;
constexpr uint32_t kExpectTokenCharCall = 2u;
constexpr uint32_t kExpectTokenPushCall = 3u;
constexpr uint32_t kExpectTokenRaiseCall = 4u;
constexpr uint32_t kExpectTokenAdvanceCall = 5u;
constexpr uint32_t kExpectTokenEncodeCall = 6u;
constexpr uint32_t kExpectTokenMoveCall = 7u;

}  // namespace

// Checks that the token the parser stands on is the one expected. A matching
// token is folded into the function's constant table and moved through the parse
// position; a mismatch raises the parser's own "'%s' expected" against the
// character value found.
void ExpectToken(EngineContext* ctx, uint32_t state, uint32_t token) {
  EnterFrame();
  // The suspend frame base: a rewind pops it and the suspend path writes its six
  // words into it.
  uint32_t frame = 0u;
  // The value record built on the guest stack for the token that is moved.
  uint32_t record = 0u;
  // The record the token stream is held in, and the character text the expected
  // token is formatted as.
  uint32_t stream = 0u;
  uint32_t char_text = 0u;
  // Whether the token found is a different one, then the arguments the expected
  // text is formatted with, and at the end the formatted text the raise carries.
  uint32_t message = 0u;
  // The parse position the token moves from, and the constant it folded into.
  uint32_t target = 0u;
  // The call a rewinding pass re-issues, and the number a spilling pass saves.
  uint32_t resume_point = 0u;
  uint32_t resume_code = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 24-byte frame and read its words back in the saved
    // order.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kExpectTokenSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    message = LoadU32(ctx, frame + 8u);
    record = LoadU32(ctx, frame + 12u);
    stream = LoadU32(ctx, frame + 16u);
    char_text = LoadU32(ctx, frame + 20u);
    token = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word pushed with that frame. It names the call
    // that was in flight, so only that call is issued again.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kExpectTokenResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The record this step builds lives on the guest stack, so it lasts exactly
    // as long as the call does.
    GuestStackSp(ctx) -= kExpectTokenRecordBytes;
    record = GuestStackSp(ctx);
    stream = LoadU32(ctx, state + 48u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == kExpectTokenJumpCall) {
    uint32_t const answer = EmitConditionJump(ctx, stream, token);
    resume_code = kExpectTokenJumpCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    target = answer;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == kExpectTokenTrackerCall) {
    StepRecordResume(ctx, state);
    resume_code = kExpectTokenTrackerCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The token found is a different one when its kind is not the kind this step
    // expects.
    message = static_cast<uint32_t>(LoadU32(ctx, state + 16u) != kExpectTokenExpectedKind);
  }
  message |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (message) {
    if (ResumePhase(ctx) == kResumeRunning) {
      message = LoadU32(ctx, state + 52u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == kExpectTokenCharCall) {
      uint32_t const answer = FormatCharValue(ctx, state, kExpectTokenExpectedKind);
      resume_code = kExpectTokenCharCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      char_text = answer;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The text the expected token formatted as is the argument the text below
      // is built with.
      StoreU32(ctx, record, char_text);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == kExpectTokenPushCall) {
      uint32_t const answer = FormatPushString(ctx, message, kExpectTokenExpectedText, record);
      resume_code = kExpectTokenPushCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      message = answer;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == kExpectTokenRaiseCall) {
      RaiseSyntaxError(ctx, state, message);
      resume_code = kExpectTokenRaiseCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // What the parse position advances to is the token the stream holds.
    target = LoadU32(ctx, state + 24u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == kExpectTokenAdvanceCall) {
    StepRecordResume(ctx, state);
    resume_code = kExpectTokenAdvanceCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The token is folded into the function's constant table.
    state = LoadU32(ctx, state + 48u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == kExpectTokenEncodeCall) {
    uint32_t const answer = EncodeConstant(ctx, state, target);
    resume_code = kExpectTokenEncodeCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    state = answer;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The record the token is moved from: its upper word says the value has no
    // second half, and its tag is the token's own.
    StoreU64(ctx, record + kExpectTokenUpperOffset, kExpectTokenNoValue);
    StoreU32(ctx, record + kExpectTokenValueOffset, state);
    StoreU32(ctx, record + kExpectTokenTagOffset, kExpectTokenTag);
    state = record + kExpectTokenTagOffset;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == kExpectTokenMoveCall) {
    EmitMoveSlot(ctx, stream, token, state);
    resume_code = kExpectTokenMoveCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The record reserved on the guest stack goes back.
    GuestStackSp(ctx) = record + kExpectTokenRecordBytes;
  }
  // Nothing is in flight here, so no frame is saved.
  goto done;
suspend_point:;
  // Push the resume word naming the call that was in flight, then the frame the
  // rewind pops; the words are saved in the order that pop reads them.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kExpectTokenResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, token);
  StoreU32(ctx, frame + 8u, message);
  StoreU32(ctx, frame + 12u, record);
  StoreU32(ctx, frame + 16u, stream);
  StoreU32(ctx, frame + 20u, char_text);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kExpectTokenSaveFrameBytes);
done:;
  LeaveFrame();
}

}  // namespace artc
