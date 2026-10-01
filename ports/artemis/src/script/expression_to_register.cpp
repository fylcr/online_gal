#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
void DischargeExpression(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t EmitConditionJump(EngineContext* ctx, uint32_t func_state, uint32_t condition);
uint32_t AddFunctionConstant(EngineContext* ctx, uint32_t func_state, uint32_t key, uint32_t value);

namespace {

constexpr uint32_t kExpressionToRegisterSaveFrameBytes = 20u;  // saved words below the resume code
constexpr uint32_t kExpressionToRegisterResumeWordBytes = 4u;  // resume code slot

}  // namespace

// Discharges one expression into its next free register, emitting whatever
// moves the expression shape demands.  A scratch record is built on the
// guest stack and handed to AddFunctionConstant, which can suspend, so the
// record pointer and the key and value words are kept in the frame.
uint32_t ExpressionToRegister(EngineContext* ctx, uint32_t func_state, uint32_t expr) {
  EnterFrame();
  uint32_t called = 0u;        // the result of the last call
  uint32_t two_operands = 0u;  // set when the expression has two operands
  uint32_t key = 0u;           // node tag, then the key of the constant record
  uint32_t kind = 0u;          // the node tag, one-based
  uint32_t record = 0u;        // the 32-byte scratch record on the guest stack
  uint32_t value = 0u;         // the record's value word
  double number = 0;           // a numeric constant's value
  uint32_t result = 0u;        // the register the expression was discharged into
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 20-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kExpressionToRegisterSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    func_state = LoadU32(ctx, frame);
    key = LoadU32(ctx, frame + 8u);
    record = LoadU32(ctx, frame + 12u);
    value = LoadU32(ctx, frame + 16u);
    expr = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame that saves the resume code.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kExpressionToRegisterResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    record = GuestStackSp(ctx) - 32u;
    GuestStackSp(ctx) = record;
    two_operands = LoadU32(ctx, expr + 20u) != LoadU32(ctx, expr + 16u);
  }
  if (two_operands != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      called = EmitConditionJump(ctx, func_state, expr);
      resume_point = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto dispatch; }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    DischargeExpression(ctx, func_state, expr);
    resume_point = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
dispatch:
  if (ResumePhase(ctx) == kResumeRunning) {
    key = LoadU32(ctx, expr);
    kind = key - 1u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    switch (kind) {
      case 0: case 1: case 2: case 4: goto constant_operand;
      case 3: goto register_operand;
      default: goto emit_condition;
    }
constant_operand:
    // A constant of any kind: only 256 constant-table slots are available.
    if ((int32_t)LoadU32(ctx, func_state + 40u) > 255) { goto emit_condition; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    switch (kind) {
      case 0: goto number_constant;
      case 1: case 2: case 3: goto boolean_constant;
      case 4: goto double_constant;
      default: goto boolean_constant;
    }
number_constant:
    // The record names the constant slot and holds no value of its own.
    StoreU32(ctx, record + 8u, 0u);
    key = LoadU32(ctx, func_state + 4u);
    StoreU32(ctx, record + 24u, 5u);
    StoreU32(ctx, record + 16u, key);
    key = record + 16u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
    called = AddFunctionConstant(ctx, func_state, key, record);
    resume_point = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    func_state = called;
  }
  if (ResumePhase(ctx) == kResumeRunning) { goto have_register; }
double_constant:
  if (ResumePhase(ctx) == kResumeRunning) {
    // A numeric constant: the record carries the double itself.
    number = LoadF64(ctx, expr + 8u);
    StoreU32(ctx, record + 24u, 3u);
    StoreF64(ctx, record + 16u, number);
    value = record + 16u;
    key = record + 16u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
    called = AddFunctionConstant(ctx, func_state, key, value);
    resume_point = 3u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    func_state = called;
  }
  if (ResumePhase(ctx) == kResumeRunning) { goto have_register; }
boolean_constant:
  if (ResumePhase(ctx) == kResumeRunning) {
    // The record carries a boolean: only the node tag 2 is true.
    StoreU32(ctx, record + 24u, 1u);
    StoreU32(ctx, record + 16u, key == 2u);
    value = record + 16u;
    key = record + 16u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
    called = AddFunctionConstant(ctx, func_state, key, value);
    resume_point = 4u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    func_state = called;
  }
have_register:
  if (ResumePhase(ctx) == kResumeRunning) {
    // The expression becomes a register reference: its node now names the
    // register the constant was discharged into.
    StoreU32(ctx, expr, 4u);
    StoreU32(ctx, expr + 8u, func_state);
    called = func_state | 256u;
    goto register_ready;
  }
register_operand:
  if (ResumePhase(ctx) == kResumeRunning) {
    key = LoadU32(ctx, expr + 8u);
    if ((int32_t)key > 255) { goto emit_condition; }
    called = key | 256u;
    goto register_ready;
  }
emit_condition:
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
    called = EmitConditionJump(ctx, func_state, expr);
    resume_point = 5u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  } else {
    called = expr;
  }
register_ready:
  expr = called;
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = record + 32u;
    result = expr;
    goto exit_path;
  }
  TrapUnreachable();
suspend_point:
  // Suspend: write the resume code, then push the frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kExpressionToRegisterResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, func_state);
    StoreU32(ctx, frame + 4u, expr);
    StoreU32(ctx, frame + 8u, key);
    StoreU32(ctx, frame + 12u, record);
    StoreU32(ctx, frame + 16u, value);
  }
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kExpressionToRegisterSaveFrameBytes);
exit_path:
  LeaveFrame();
  return result;
}

}  // namespace artc
