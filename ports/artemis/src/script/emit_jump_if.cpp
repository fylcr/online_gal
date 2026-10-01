#include "util/engine_context.h"

namespace artc {

// The parser's syntax-error raiser: formats the text against the lexer state and
// raises it.
void RaiseSyntaxError(EngineContext* ctx, uint32_t lex_state, uint32_t message);

// Appends one instruction word to a function's code array.
uint32_t EmitInstruction(EngineContext* ctx, uint32_t func_state, uint32_t instruction, uint32_t line);

// Discharges one expression into the next free register.
uint32_t ExpressionToRegister(EngineContext* ctx, uint32_t func_state, uint32_t expression);

// Emits the jump for one condition.
uint32_t EmitConditionJump(EngineContext* ctx, uint32_t func_state, uint32_t condition);

namespace {

// The text raised when a function outgrows the parser's own jump fields.
constexpr uint32_t kEmitJumpIfTooComplexText = 0x2A371u;  // function or expression too complex

// The kinds and value bits this body judges, and the register index at which the
// prototype counts as out of registers.
constexpr uint32_t kEmitJumpIfIntegerKind = 12u;
constexpr uint32_t kEmitJumpIfLiteralBit = 256u;
constexpr uint32_t kEmitJumpIfRegisterLimit = 248u;

// The fields one branch word carries: the value's own field, the register's, the
// target expression's, and the branch opcode.
constexpr uint32_t kEmitJumpIfValueShift = 23u;
constexpr uint32_t kEmitJumpIfRegisterShift = 6u;
constexpr uint32_t kEmitJumpIfOperandShift = 14u;
constexpr uint32_t kEmitJumpIfBranchOpcode = 11u;

// The suspend frame: the words this body saves, and the resume word pushed with
// them, whose number names the guarded call that was in flight.
constexpr uint32_t kEmitJumpIfSaveFrameBytes = 32u;
constexpr uint32_t kEmitJumpIfResumeWordBytes = 4u;
constexpr uint32_t kEmitJumpIfConditionCall = 0u;
constexpr uint32_t kEmitJumpIfRaiseCall = 1u;
constexpr uint32_t kEmitJumpIfRegisterCall = 2u;
constexpr uint32_t kEmitJumpIfEmitCall = 3u;

}  // namespace

// Emits a conditional jump: the condition's own jump is emitted first, the
// condition is re-read as a constant or a register, the target expression is put
// into a register and the branch word is emitted for the pair. Raises the
// parser's complexity text when the function has outgrown its register block.
void EmitJumpIf(EngineContext* ctx, uint32_t func_state, uint32_t condition, uint32_t expression) {
  EnterFrame();
  // The suspend frame base: a rewind pops it and the suspend path writes its
  // eight words into it.
  uint32_t frame = 0u;
  // The working value of the current step: the condition's register test, the
  // condition's constant, the register the target expression took, and the line
  // the branch is written at. One cell, because the suspend frame carries it in
  // one word.
  uint32_t result = 0u;
  // The branch word being built, and while it is being built the register line
  // the condition counts against.
  uint32_t word = 0u;
  // The register the condition needs, and later the branch's own register field.
  uint32_t reg = 0u;
  // The function record that func_state's first word names.
  uint32_t proto = 0u;
  // Whether the condition took the last register the prototype holds.
  uint32_t regs_short = 0u;
  // The guarded call a rewinding pass re-issues, and the number a spilling pass
  // saves.
  uint32_t resume_code = 0u;
  // Whether the register the condition took has to be recorded.
  bool needs_register = false;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 32-byte frame and read its words back in the saved
    // order.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kEmitJumpIfSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    func_state = LoadU32(ctx, frame);
    expression = LoadU32(ctx, frame + 8u);
    result = LoadU32(ctx, frame + 12u);
    word = LoadU32(ctx, frame + 16u);
    reg = LoadU32(ctx, frame + 20u);
    proto = LoadU32(ctx, frame + 24u);
    regs_short = LoadU32(ctx, frame + 28u);
    condition = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word pushed with that frame. It names the call
    // that was in flight, so only that call is issued again.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kEmitJumpIfResumeWordBytes);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  // A running pass always owes the condition's own jump; a rewinding one owes it
  // only when that is the call it is replaying.
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kEmitJumpIfConditionCall) {
    uint32_t const answer = EmitConditionJump(ctx, func_state, condition);
    resume_code = kEmitJumpIfConditionCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    result = answer;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A constant that stands inside the table gives the register it was given
    // back before the count moves on.
    if (LoadU32(ctx, condition) == kEmitJumpIfIntegerKind) {
      word = LoadU32(ctx, condition + 8u);
      if ((word & kEmitJumpIfLiteralBit) == 0u &&
          static_cast<int32_t>(word) >= static_cast<int32_t>(LoadU8(ctx, func_state + 50u))) {
        StoreU32(ctx, func_state + 36u, LoadU32(ctx, func_state + 36u) - 1u);
      }
    }
    reg = LoadU32(ctx, func_state + 36u);
    word = reg + 2u;
    proto = LoadU32(ctx, func_state);
    result = static_cast<uint32_t>(static_cast<int32_t>(word) <=
                                   static_cast<int32_t>(LoadU8(ctx, proto + 75u)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The registers the branch needs are recorded in the prototype unless the
    // condition already fits the count the prototype holds.
    needs_register = result == 0u;
    if (needs_register) {
      regs_short = static_cast<uint32_t>(static_cast<int32_t>(reg) >=
                                        static_cast<int32_t>(kEmitJumpIfRegisterLimit));
    }
    result = word;
  }
  if (ResumePhase(ctx) == kResumeRunning && needs_register) {
    StoreU8(ctx, proto + 75u, word);
  }
  if (regs_short != 0u || ResumePhase(ctx) == kResumeRewinding) {
    // Out of registers: the raise is formatted against the lexer state, and the
    // register count steps past the two registers the branch needs.
    if (ResumePhase(ctx) == kResumeRunning) {
      result = LoadU32(ctx, func_state + 12u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kEmitJumpIfRaiseCall) {
      RaiseSyntaxError(ctx, result, kEmitJumpIfTooComplexText);
      resume_code = kEmitJumpIfRaiseCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      proto = LoadU32(ctx, func_state);
      result = LoadU32(ctx, func_state + 36u) + 2u;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The branch's register line and value: the count is given back to the
    // function state and the condition's constant moves into the word's top
    // field.
    StoreU32(ctx, func_state + 36u, result);
    result = LoadU32(ctx, condition + 8u) << kEmitJumpIfValueShift;
    word = result | (reg << kEmitJumpIfRegisterShift);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kEmitJumpIfRegisterCall) {
    uint32_t const answer = ExpressionToRegister(ctx, func_state, expression);
    resume_code = kEmitJumpIfRegisterCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    result = answer;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The branch word takes the target expression's register, and the line the
    // instruction has to be recorded at comes from the function's line record.
    word = (word | (result << kEmitJumpIfOperandShift)) | kEmitJumpIfBranchOpcode;
    result = LoadU32(ctx, LoadU32(ctx, func_state + 12u) + 8u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kEmitJumpIfEmitCall) {
    EmitInstruction(ctx, func_state, word, result);
    resume_code = kEmitJumpIfEmitCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The target expression gives its register back the same way the condition
    // did, and the condition is left holding the register the branch uses.
    if (LoadU32(ctx, expression) == kEmitJumpIfIntegerKind) {
      expression = LoadU32(ctx, expression + 8u);
      if ((expression & kEmitJumpIfLiteralBit) == 0u &&
          static_cast<int32_t>(LoadU8(ctx, func_state + 50u)) <= static_cast<int32_t>(expression)) {
        StoreU32(ctx, func_state + 36u, LoadU32(ctx, func_state + 36u) - 1u);
      }
    }
    StoreU32(ctx, condition, kEmitJumpIfIntegerKind);
    StoreU32(ctx, condition + 8u, reg);
  }
  // Nothing is in flight here, so no frame is saved.
  goto done;
suspend_point:;
  // Push the resume word naming the call that was in flight, then the frame the
  // rewind pops; the words are saved in the order that pop reads them.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kEmitJumpIfResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, func_state);
  StoreU32(ctx, frame + 4u, condition);
  StoreU32(ctx, frame + 8u, expression);
  StoreU32(ctx, frame + 12u, result);
  StoreU32(ctx, frame + 16u, word);
  StoreU32(ctx, frame + 20u, reg);
  StoreU32(ctx, frame + 24u, proto);
  StoreU32(ctx, frame + 28u, regs_short);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kEmitJumpIfSaveFrameBytes);
done:;
  LeaveFrame();
}

}  // namespace artc
