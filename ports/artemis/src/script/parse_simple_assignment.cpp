#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
uint32_t FormatPushString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
void StepRecordResume(EngineContext* ctx, uint32_t a0);
uint32_t CheckStatementLimits(EngineContext* ctx, uint32_t state, uint32_t func_state, uint32_t message);
uint32_t FormatCharValue(EngineContext* ctx, uint32_t a, uint32_t b);
void CloseBlockScope(EngineContext* ctx, uint32_t state);
void EmitVariableStore(EngineContext* ctx, uint32_t state, uint32_t variable);

namespace {

constexpr uint32_t kParseSimpleAssignmentExpectedToken = 274u;  // the token the form expects
constexpr uint32_t kParseSimpleAssignmentExpectedMessage = 0x2B625u;  // '%s' expected
constexpr uint32_t kParseSimpleAssignmentBooleanTag = 1u;  // the tag of a boolean value
constexpr uint32_t kParseSimpleAssignmentNumberTag = 3u;  // the tag of a number value
constexpr uint32_t kParseSimpleAssignmentGuestFrameBytes = 32u;  // room for the pending record
constexpr uint32_t kParseSimpleAssignmentSaveFrameBytes = 20u;
constexpr uint32_t kParseSimpleAssignmentResumeWordBytes = 4u;

}  // namespace

// Parses the single-target assignment form: it records the parse step, checks
// the statement limit for the operand, emits the store into the target
// variable and closes the block scope; a statement that is not an assignment
// raises "'%s' expected" for the token the form wanted.  The pending record
// lives in a 32-byte area reserved on the guest stack -- its +8 word is the
// type tag of the value it holds, its +28 word the operand this step returns.
// In the compiler state, +16 is the token the parse reached, +48 the
// func_state and +52 the buffer the error text is formatted from.
//
// Suspendable: every call below can suspend, so whichever values must outlive
// a suspend go into the frame, and the resume code (0..7) names the call that
// has to be replayed.
uint32_t ParseSimpleAssignment(EngineContext* ctx, uint32_t state) {
  EnterFrame();
  uint32_t operand = 0u;
  uint32_t target_slot = 0u;
  uint32_t guest_frame = 0u;
  uint32_t expected_char = 0u;
  uint32_t resume_point = 0u;
  uint32_t result = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 20-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kParseSimpleAssignmentSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    guest_frame = LoadU32(ctx, frame + 8u);
    target_slot = LoadU32(ctx, frame + 12u);
    expected_char = LoadU32(ctx, frame + 16u);
    operand = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame that saves the resume code.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kParseSimpleAssignmentResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    guest_frame = GuestStackSp(ctx) - kParseSimpleAssignmentGuestFrameBytes;
    GuestStackSp(ctx) = guest_frame;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    StepRecordResume(ctx, state);
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    operand = guest_frame + 8u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    uint32_t const called = CheckStatementLimits(ctx, state, operand, 0u);
    resume_point = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    operand = called;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadU32(ctx, guest_frame + 8u) == kParseSimpleAssignmentBooleanTag) {
      StoreU32(ctx, guest_frame + 8u, kParseSimpleAssignmentNumberTag);
    }
    target_slot = guest_frame + 8u;
    operand = LoadU32(ctx, state + 48u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
    EmitVariableStore(ctx, operand, target_slot);
    resume_point = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    target_slot = static_cast<uint32_t>(LoadU32(ctx, state + 16u) !=
        kParseSimpleAssignmentExpectedToken);
    operand = LoadU32(ctx, guest_frame + 28u);
  }
  if (target_slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      target_slot = LoadU32(ctx, state + 52u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
      uint32_t const called = FormatCharValue(ctx, state, kParseSimpleAssignmentExpectedToken);
      resume_point = 3u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      expected_char = called;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, guest_frame, expected_char);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
      uint32_t const called = FormatPushString(ctx, target_slot,
          kParseSimpleAssignmentExpectedMessage, guest_frame);
      resume_point = 4u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      target_slot = called;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
      RaiseSyntaxError(ctx, state, target_slot);
      resume_point = 5u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
    StepRecordResume(ctx, state);
    resume_point = 6u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 7u) {
    CloseBlockScope(ctx, state);
    resume_point = 7u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = guest_frame + kParseSimpleAssignmentGuestFrameBytes;
    result = operand;
    goto exit_path;
  }
  TrapUnreachable();
suspend_point:
  // Suspend: write the resume code, then push the frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kParseSimpleAssignmentResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, state);
    StoreU32(ctx, frame + 4u, operand);
    StoreU32(ctx, frame + 8u, guest_frame);
    StoreU32(ctx, frame + 12u, target_slot);
    StoreU32(ctx, frame + 16u, expected_char);
  }
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kParseSimpleAssignmentSaveFrameBytes);
exit_path:
  LeaveFrame();
  return result;
}

}  // namespace artc
