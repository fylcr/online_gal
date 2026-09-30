#include "util/engine_context.h"

namespace artc {

// The parser's syntax-error raiser: formats the text against the lexer state and
// raises it.
void RaiseSyntaxError(EngineContext* ctx, uint32_t lex_state, uint32_t message);

// Discharges one expression descriptor into a register.
void DischargeExpression(EngineContext* ctx, uint32_t func_state, uint32_t expression);

// Compiles one expression so that a jump can use its operand.
void CompileExpression(EngineContext* ctx, uint32_t func_state, uint32_t expression, uint32_t operand);

namespace {

// The text raised when a function outgrows the parser's own jump fields.
constexpr uint32_t kEmitConditionJumpTooComplexText = 0x2A371u;  // function or expression too complex

// The kinds and value bits this body judges: the condition kind that carries an
// integer constant, the value bit that gives a register back, and the register
// index at which the prototype counts as out of registers.
constexpr uint32_t kEmitConditionJumpIntegerKind = 12u;
constexpr uint32_t kEmitConditionJumpLiteralBit = 256u;
constexpr uint32_t kEmitConditionJumpRegisterLimit = 249u;

// The suspend frame: the words this body saves, and the resume word pushed with
// them, whose number names the guarded call that was in flight.
constexpr uint32_t kEmitConditionJumpSaveFrameBytes = 24u;
constexpr uint32_t kEmitConditionJumpResumeWordBytes = 4u;
constexpr uint32_t kEmitConditionJumpFirstDischargeCall = 0u;
constexpr uint32_t kEmitConditionJumpCompileCall = 1u;
constexpr uint32_t kEmitConditionJumpSecondDischargeCall = 2u;
constexpr uint32_t kEmitConditionJumpRaiseCall = 3u;
constexpr uint32_t kEmitConditionJumpLastCompileCall = 4u;

}  // namespace

// Emits the jump for one condition. An integer constant that already stands in
// the prototype's constant table goes straight into the jump; everything else is
// discharged and compiled first, and a condition that takes one register too
// many raises the parser's complexity text. The answer is the word the
// condition's slot holds.
uint32_t EmitConditionJump(EngineContext* ctx, uint32_t func_state, uint32_t condition) {
  EnterFrame();
  // The working word the current step computes: first whether the condition
  // still has to be compiled, then the jump's answer. One cell, because the
  // suspend frame carries it in one word.
  uint32_t result = 0u;
  // The guarded call a rewinding pass re-issues, and the number a spilling pass
  // saves.
  uint32_t resume_code = 0u;
  // The condition's operand: its constant while it is one, the register it was
  // compiled into afterwards.
  uint32_t value = 0u;
  // The function record that func_state's first word names.
  uint32_t proto = 0u;
  // The register the condition needs, and later whether it ran past the
  // prototype's own register count.
  uint32_t reg = 0u;
  // Whether the first and the last compiled step owe their work.
  bool needs_compile = true;
  // Whether the register the condition took already fits the prototype's count.
  bool register_fits = false;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 24-byte frame and read its words back in the saved
    // order.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kEmitConditionJumpSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    func_state = LoadU32(ctx, frame);
    condition = LoadU32(ctx, frame + 4u);
    result = LoadU32(ctx, frame + 8u);
    value = LoadU32(ctx, frame + 12u);
    reg = LoadU32(ctx, frame + 16u);
    proto = LoadU32(ctx, frame + 20u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word pushed with that frame. It names the call
    // that was in flight, so only that call is issued again.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kEmitConditionJumpResumeWordBytes);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  // A running pass always owes the first discharge; a rewinding one owes it only
  // when that is the call it is replaying.
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kEmitConditionJumpFirstDischargeCall) {
    DischargeExpression(ctx, func_state, condition);
    resume_code = kEmitConditionJumpFirstDischargeCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A condition that is not an integer constant has to be compiled; so has one
    // whose constant stands outside the prototype's constant table.
    result = static_cast<uint32_t>(LoadU32(ctx, condition) != kEmitConditionJumpIntegerKind);
    if (result == 0u) {
      if (LoadU32(ctx, condition + 20u) == LoadU32(ctx, condition + 16u)) {
        // The condition already carries the jump that was emitted for it.
        uint32_t const answer = LoadU32(ctx, condition + 8u);
        LeaveFrame();
        return answer;
      }
      value = LoadU32(ctx, condition + 8u);
      result = static_cast<uint32_t>(static_cast<int32_t>(value) <
                                     static_cast<int32_t>(LoadU8(ctx, func_state + 50u)));
    }
    needs_compile = result == 0u;
  }
  if (needs_compile) {
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kEmitConditionJumpCompileCall) {
      CompileExpression(ctx, func_state, condition, value);
      resume_code = kEmitConditionJumpCompileCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      uint32_t const answer = LoadU32(ctx, condition + 8u);
      LeaveFrame();
      return answer;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kEmitConditionJumpSecondDischargeCall) {
    DischargeExpression(ctx, func_state, condition);
    resume_code = kEmitConditionJumpSecondDischargeCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A constant that stands inside the table keeps the register it was given;
    // one that fell outside it gives the register back before the count moves on.
    if (LoadU32(ctx, condition) == kEmitConditionJumpIntegerKind) {
      value = LoadU32(ctx, condition + 8u);
      if ((value & kEmitConditionJumpLiteralBit) == 0u &&
          static_cast<int32_t>(value) >= static_cast<int32_t>(LoadU8(ctx, func_state + 50u))) {
        StoreU32(ctx, func_state + 36u, LoadU32(ctx, func_state + 36u) - 1u);
      }
    }
    reg = LoadU32(ctx, func_state + 36u);
    value = reg + 1u;
    proto = LoadU32(ctx, func_state);
    result = static_cast<uint32_t>(static_cast<int32_t>(reg) <
                                   static_cast<int32_t>(LoadU8(ctx, proto + 75u)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A register that already fits the prototype's count needs no recording; one
    // that stands beyond it is noted, and so is whether it reached the limit the
    // compiler allows.
    register_fits = result != 0u;
    if (!register_fits) {
      reg = static_cast<uint32_t>(static_cast<int32_t>(reg) >=
                                  static_cast<int32_t>(kEmitConditionJumpRegisterLimit));
    }
    result = value;
  }
  if (!register_fits && (reg != 0u || ResumePhase(ctx) == kResumeRewinding)) {
    // Out of registers: the raise is formatted against the lexer state, and the
    // register count steps past the register this condition wanted.
    if (ResumePhase(ctx) == kResumeRunning) {
      result = LoadU32(ctx, func_state + 12u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kEmitConditionJumpRaiseCall) {
      RaiseSyntaxError(ctx, result, kEmitConditionJumpTooComplexText);
      resume_code = kEmitConditionJumpRaiseCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      proto = LoadU32(ctx, func_state);
      result = LoadU32(ctx, func_state + 36u) + 1u;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The prototype records the register the condition took.
      StoreU8(ctx, proto + 75u, value);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, func_state + 36u, result);
    result = result - 1u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kEmitConditionJumpLastCompileCall) {
    CompileExpression(ctx, func_state, condition, result);
    resume_code = kEmitConditionJumpLastCompileCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The answer is the word the condition's slot holds.
    uint32_t const answer = LoadU32(ctx, condition + 8u);
    LeaveFrame();
    return answer;
  }
  TrapUnreachable();
suspend_point:;
  // Push the resume word naming the call that was in flight, then the frame the
  // rewind pops; the words are saved in the order that pop reads them.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kEmitConditionJumpResumeWordBytes);
  uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, func_state);
  StoreU32(ctx, frame + 4u, condition);
  StoreU32(ctx, frame + 8u, result);
  StoreU32(ctx, frame + 12u, value);
  StoreU32(ctx, frame + 16u, reg);
  StoreU32(ctx, frame + 20u, proto);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kEmitConditionJumpSaveFrameBytes);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
