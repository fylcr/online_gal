#include "util/engine_context.h"

namespace artc {

// Appends one instruction word to a function's code array.
uint32_t EmitInstruction(EngineContext* ctx, uint32_t func_state, uint32_t instruction, uint32_t line);

// Discharges one expression into the next free register.
uint32_t ExpressionToRegister(EngineContext* ctx, uint32_t func_state, uint32_t expression);

// Emits the jump for one condition.
uint32_t EmitConditionJump(EngineContext* ctx, uint32_t func_state, uint32_t condition);

// Discharges one expression, jump and all, into a register.
void CompileExpression(EngineContext* ctx, uint32_t func_state, uint32_t code, uint32_t line);

namespace {

// The first variable kind this body answers to, and the kind that means the
// reserved register already holds an integer constant.
constexpr uint32_t kEmitVariableLoadFirstKind = 6u;
constexpr uint32_t kEmitVariableLoadIntegerKind = 12u;
constexpr uint32_t kEmitVariableLoadConstantBit = 256u;

// The fields one load word carries, and the token each kind annotates its word
// with.
constexpr uint32_t kEmitVariableLoadRegisterShift = 6u;
constexpr uint32_t kEmitVariableLoadOperandShift = 14u;
constexpr uint32_t kEmitVariableLoadValueShift = 23u;
constexpr uint32_t kEmitVariableLoadTopToken = 8u;
constexpr uint32_t kEmitVariableLoadOperandToken = 7u;
constexpr uint32_t kEmitVariableLoadCompileToken = 9u;

// The suspend frame: the words this body saves, and the resume word pushed with
// them, whose number names the call that was in flight.
constexpr uint32_t kEmitVariableLoadSaveFrameBytes = 24u;
constexpr uint32_t kEmitVariableLoadResumeWordBytes = 4u;
constexpr uint32_t kEmitVariableLoadCompileCall = 0u;
constexpr uint32_t kEmitVariableLoadTopCall = 1u;
constexpr uint32_t kEmitVariableLoadOperandCall = 2u;
constexpr uint32_t kEmitVariableLoadRegisterCall = 3u;
constexpr uint32_t kEmitVariableLoadEmitCall = 4u;

}  // namespace

// Emits the load for one variable kind into the register the caller reserved.
// The four kinds past six each build their own word, and the tail gives the
// reserved register back when it holds a constant inside the limit.
void EmitVariableLoad(EngineContext* ctx, uint32_t state, uint32_t variable, uint32_t register_hint) {
  EnterFrame();
  // The suspend frame base: a rewind pops it and the suspend path writes its six
  // words into it.
  uint32_t frame = 0u;
  // The word being built, before it is handed back into the variable cell.
  uint32_t word = 0u;
  // The variable's kind past the first six — the switch key — which the word
  // then carries as the token its own field needs, and at the end the line the
  // word is recorded at. One cell, because the suspend frame carries it in one
  // word.
  uint32_t kind = 0u;
  // The function state as the record the word is handed to, kept in its own
  // word.
  uint32_t record = 0u;
  // The register the condition's jump, or the discharged expression, took.
  uint32_t target = 0u;
  // The call a rewinding pass re-issues, and the number a spilling pass saves.
  uint32_t resume_point = 0u;
  uint32_t resume_code = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 24-byte frame and read its words back in the saved
    // order.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kEmitVariableLoadSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    variable = LoadU32(ctx, frame + 4u);
    register_hint = LoadU32(ctx, frame + 8u);
    kind = LoadU32(ctx, frame + 12u);
    record = LoadU32(ctx, frame + 16u);
    target = LoadU32(ctx, frame + 20u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word pushed with that frame. It names the call
    // that was in flight, so only that call is issued again.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kEmitVariableLoadResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The kind decides how the load is built; the word is handed to the function
    // state itself.
    kind = LoadU32(ctx, variable) - kEmitVariableLoadFirstKind;
    record = state;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Only the first four kinds build a word; every other kind has none of its
    // own and goes straight to the tail.
    switch (kind) {
      case 0: goto compile_into_register;
      case 1: goto fill_value_field;
      case 2: goto fill_operand_field;
      case 3: goto fill_folded_fields;
      default: goto exit_path;
    }
  compile_into_register:;
    // The reserved register gives its slot back unless it already holds a
    // constant inside the limit.
    if (LoadU32(ctx, register_hint) == kEmitVariableLoadIntegerKind) {
      uint32_t const constant = LoadU32(ctx, register_hint + 8u);
      if ((constant & kEmitVariableLoadConstantBit) == 0u &&
          static_cast<int32_t>(constant) < static_cast<int32_t>(LoadU8(ctx, state + 50u))) {
        StoreU32(ctx, state + 36u, LoadU32(ctx, state + 36u) - 1u);
      }
    }
    // This kind names the expression to compile rather than carrying a word.
    variable = LoadU32(ctx, variable + 8u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kEmitVariableLoadCompileCall) {
    CompileExpression(ctx, state, register_hint, variable);
    resume_code = kEmitVariableLoadCompileCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The expression was discharged into the reserved register; this kind has
    // nothing left to emit.
    goto done;
  }
fill_value_field:;
  if (ResumePhase(ctx) == kResumeRunning) {
    kind = kEmitVariableLoadTopToken;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kEmitVariableLoadTopCall) {
    uint32_t const answer = EmitConditionJump(ctx, state, register_hint);
    resume_code = kEmitVariableLoadTopCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    target = answer;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The load's own value takes the word's top field, the register the
    // condition took its register field.
    word = (LoadU32(ctx, variable + 8u) << kEmitVariableLoadValueShift) |
           (target << kEmitVariableLoadRegisterShift);
    goto finish_word;
  }
fill_operand_field:;
  if (ResumePhase(ctx) == kResumeRunning) {
    kind = kEmitVariableLoadOperandToken;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kEmitVariableLoadOperandCall) {
    uint32_t const answer = EmitConditionJump(ctx, state, register_hint);
    resume_code = kEmitVariableLoadOperandCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    target = answer;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // This kind's value lands in the operand field instead.
    word = (LoadU32(ctx, variable + 8u) << kEmitVariableLoadOperandShift) |
           (target << kEmitVariableLoadRegisterShift);
    goto finish_word;
  }
fill_folded_fields:;
  if (ResumePhase(ctx) == kResumeRunning) {
    kind = kEmitVariableLoadCompileToken;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kEmitVariableLoadRegisterCall) {
    uint32_t const answer = ExpressionToRegister(ctx, state, register_hint);
    resume_code = kEmitVariableLoadRegisterCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    target = answer;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // This kind fills three fields: the register the expression took, the
    // record's own value and its name.
    uint32_t const value = LoadU32(ctx, variable + 8u);
    word = (target << kEmitVariableLoadOperandShift) | (value << kEmitVariableLoadRegisterShift) |
           (LoadU32(ctx, variable + 12u) << kEmitVariableLoadValueShift);
  } else {
    // A rewinding pass has no word to build, so the cell it carries stands.
    word = variable;
  }
finish_word:;
  variable = word;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The word takes the token this kind annotates it with, and the line the
    // function state holds.
    variable |= kind;
    kind = LoadU32(ctx, LoadU32(ctx, state + 12u) + 8u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kEmitVariableLoadEmitCall) {
    EmitInstruction(ctx, record, variable, kind);
    resume_code = kEmitVariableLoadEmitCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
exit_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The reserved register gives its slot back the same way here.
    if (LoadU32(ctx, register_hint) == kEmitVariableLoadIntegerKind) {
      variable = LoadU32(ctx, register_hint + 8u);
      if ((variable & kEmitVariableLoadConstantBit) == 0u &&
          static_cast<int32_t>(LoadU8(ctx, state + 50u)) <= static_cast<int32_t>(variable)) {
        StoreU32(ctx, state + 36u, LoadU32(ctx, state + 36u) - 1u);
      }
    }
  }
  // Nothing is in flight here, so no frame is saved.
  goto done;
suspend_point:;
  // Push the resume word naming the call that was in flight, then the frame the
  // rewind pops; the words are saved in the order that pop reads them.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kEmitVariableLoadResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, variable);
  StoreU32(ctx, frame + 8u, register_hint);
  StoreU32(ctx, frame + 12u, kind);
  StoreU32(ctx, frame + 16u, record);
  StoreU32(ctx, frame + 20u, target);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kEmitVariableLoadSaveFrameBytes);
done:;
  LeaveFrame();
}

}  // namespace artc
