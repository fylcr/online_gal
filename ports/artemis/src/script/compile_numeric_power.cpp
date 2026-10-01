#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
uint32_t EmitInstruction(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t ExpressionToRegister(EngineContext* ctx, uint32_t func_state, uint32_t expr);
double ComputeNumericPower(EngineContext* ctx, double base, double exponent);

namespace {

constexpr uint32_t kCompileNumericPowerNoChild = 0xFFFFFFFFu;  // an absent child operand
constexpr uint32_t kCompileNumericPowerNumberTag = 5u;  // tag of a numeric constant node
constexpr uint32_t kCompileNumericPowerResultTag = 11u;  // tag of the folded numeric result
constexpr uint32_t kCompileNumericPowerAddOp = 12u;  // tag of '+', the first operator we fold
constexpr uint32_t kCompileNumericPowerNegOp = 18u;  // tag of unary '-', the first operator kept
constexpr uint32_t kCompileNumericPowerRegisterTag = 12u;  // tag of a register reference
constexpr uint32_t kCompileNumericPowerConstBit = 256u;  // set while a register holds a constant
constexpr uint32_t kCompileNumericPowerSaveFrameBytes = 24u;  // saved words below the resume code
constexpr uint32_t kCompileNumericPowerResumeWordBytes = 4u;  // resume code slot

}  // namespace

// Compiles a numeric power: when both operands are numeric constants the
// operator is folded here, otherwise the operands are compiled into
// registers and the instruction is emitted.  The register allocations and
// the emit are suspendable, so the operands and the two register numbers
// are kept in the frame.
void CompileNumericPower(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
  EnterFrame();
  uint32_t called = 0u;      // the result of the last call
  uint32_t skip_fold = 0u;   // set when the left operand is not a number
  uint32_t reg_left = 0u;    // the register the left operand compiles into
  uint32_t reg_right = 0u;   // the register the right operand compiles into
  uint32_t reg_index = 0u;   // register index taken out of an operand node
  double left_value = 0;     // the left operand's numeric value
  double right_value = 0;    // the right operand's numeric value
  double folded = 0;         // the folded result, written back into the left operand
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 24-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kCompileNumericPowerSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    a = LoadU32(ctx, frame);
    c = LoadU32(ctx, frame + 8u);
    d = LoadU32(ctx, frame + 12u);
    reg_left = LoadU32(ctx, frame + 16u);
    reg_right = LoadU32(ctx, frame + 20u);
    b = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame that saves the resume code.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kCompileNumericPowerResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    skip_fold = LoadU32(ctx, c) != kCompileNumericPowerNumberTag;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Fold only when both operands are plain numeric constants.
    if (skip_fold != 0u) { goto general_path; }
    if (LoadU32(ctx, c + 16u) != kCompileNumericPowerNoChild) { goto general_path; }
    if (LoadU32(ctx, c + 20u) != kCompileNumericPowerNoChild) { goto general_path; }
    if (LoadU32(ctx, d) != kCompileNumericPowerNumberTag) { goto general_path; }
    if (LoadU32(ctx, d + 16u) != kCompileNumericPowerNoChild) { goto general_path; }
    if (LoadU32(ctx, d + 20u) != kCompileNumericPowerNoChild) { goto general_path; }
    right_value = LoadF64(ctx, d + 8u);
    left_value = LoadF64(ctx, c + 8u);
    switch (b - kCompileNumericPowerAddOp) {
      case 0: folded = left_value + right_value; break;
      case 1: folded = left_value - right_value; break;
      case 2: folded = left_value * right_value; break;
      case 3:
        if (right_value == 0.0) { goto emit_path; }
        folded = left_value / right_value;
        break;
      case 4:
        if (right_value == 0.0) { goto emit_path; }
        folded = left_value - right_value * wasm_floor(left_value / right_value);
        break;
      case 5: folded = ComputeNumericPower(ctx, left_value, right_value); break;
      case 6: folded = -left_value; break;
      case 7: goto store_result;
      case 8: goto finished;
      default: goto store_result;
    }
    // A NaN result is never folded.
    if (folded != folded) { goto general_path; }
store_result:
    StoreF64(ctx, c + 8u, folded);
    goto exit_path;
general_path:
    // The operators from -18 up are not folded here.
    switch (b - kCompileNumericPowerNegOp) {
      case 0: goto finished;
      case 2: goto finished;
      default: break;
    }
emit_path:;
  }
  // The emit path: the right operand's register is allocated first, so the
  // left operand can still be released while it is used.
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    called = ExpressionToRegister(ctx, a, d);
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    reg_right = called;
  }
finished:
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    called = ExpressionToRegister(ctx, a, c);
    resume_point = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    reg_left = called;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Release the operand registers that only held this operator's operands.
    if ((int32_t)reg_left > (int32_t)reg_right) {
      if (LoadU32(ctx, c) != kCompileNumericPowerRegisterTag) { goto c_slot_kept; }
      reg_index = LoadU32(ctx, c + 8u);
      if ((reg_index & kCompileNumericPowerConstBit) != 0u) { goto c_slot_kept; }
      if ((int32_t)LoadU8(ctx, a + 50u) > (int32_t)reg_index) { goto c_slot_kept; }
      StoreU32(ctx, a + 36u, LoadU32(ctx, a + 36u) - 1u);
c_slot_kept:
      if (LoadU32(ctx, d) != kCompileNumericPowerRegisterTag) { goto build_instruction; }
      reg_index = LoadU32(ctx, d + 8u);
      if ((reg_index & kCompileNumericPowerConstBit) != 0u) { goto build_instruction; }
      if ((int32_t)LoadU8(ctx, a + 50u) > (int32_t)reg_index) { goto build_instruction; }
      StoreU32(ctx, a + 36u, LoadU32(ctx, a + 36u) - 1u);
      goto build_instruction;
    }
    if (LoadU32(ctx, d) != kCompileNumericPowerRegisterTag) { goto d_slot_kept; }
    reg_index = LoadU32(ctx, d + 8u);
    if ((reg_index & kCompileNumericPowerConstBit) != 0u) { goto d_slot_kept; }
    if ((int32_t)LoadU8(ctx, a + 50u) > (int32_t)reg_index) { goto d_slot_kept; }
    StoreU32(ctx, a + 36u, LoadU32(ctx, a + 36u) - 1u);
d_slot_kept:
    if (LoadU32(ctx, c) != kCompileNumericPowerRegisterTag) { goto build_instruction; }
    reg_index = LoadU32(ctx, c + 8u);
    if ((reg_index & kCompileNumericPowerConstBit) != 0u) { goto build_instruction; }
    if ((int32_t)LoadU8(ctx, a + 50u) > (int32_t)reg_index) { goto build_instruction; }
    StoreU32(ctx, a + 36u, LoadU32(ctx, a + 36u) - 1u);
build_instruction:
    // Pack both register numbers into the instruction word.
    d = LoadU32(ctx, LoadU32(ctx, a + 12u) + 8u);
    b = (reg_right << 14u) | b | (reg_left << 23u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
    called = EmitInstruction(ctx, a, b, d);
    resume_point = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    a = called;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The left operand becomes the folded result.
    StoreU32(ctx, c, kCompileNumericPowerResultTag);
    StoreU32(ctx, c + 8u, a);
  }
  goto exit_path;
suspend_point:
  // Suspend: write the resume code, then push the frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kCompileNumericPowerResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, a);
    StoreU32(ctx, frame + 4u, b);
    StoreU32(ctx, frame + 8u, c);
    StoreU32(ctx, frame + 12u, d);
    StoreU32(ctx, frame + 16u, reg_left);
    StoreU32(ctx, frame + 20u, reg_right);
  }
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kCompileNumericPowerSaveFrameBytes);
exit_path:
  LeaveFrame();
}

}  // namespace artc
