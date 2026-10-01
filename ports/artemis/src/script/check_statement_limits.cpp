#include "util/engine_context.h"

namespace artc {

// Forward declarations -- registered callees.
void ParseParamList(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d);
void CheckMatch(EngineContext* ctx, uint32_t lex_state, uint32_t expected);
void CompileMainBody(EngineContext* ctx, uint32_t a0, uint32_t a1);
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
void StepRecordResume(EngineContext* ctx, uint32_t a0);
void LexError(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t EmitOpInstruction(EngineContext* ctx, uint32_t func_state, uint32_t op,
                           uint32_t arg_a, uint32_t arg_b, uint32_t line);
void EmitExpression(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d);
void EmitCondition(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
void EmitStatement(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t EncodeConstant(EngineContext* ctx, uint32_t func_state, uint32_t value);

namespace {

// The two bytes each syntax level owns: the first says whether the caller's
// message already reaches the level, the second is the message the level passes
// on to the expression it emits.
constexpr uint32_t kCheckStatementLimitsLevelGate = 0x2B660u;
constexpr uint32_t kCheckStatementLimitsLevelMessage = 0x2B661u;
// The expression descriptor's "no numeric value" sentinel.
constexpr uint64_t kCheckStatementLimitsNoNumber = 0xFFFFFFFFFFFFFFFFull;
// Parser diagnostics this step raises.
constexpr uint32_t kCheckStatementLimitsMsgTooManyLevels = 0x2B633u;
constexpr uint32_t kCheckStatementLimitsMsgCannotUseOutside = 0x2B67Eu;

}  // namespace

// Parses one statement of the function being compiled: it spends a syntax level,
// dispatches on the statement's first token, fills the expression descriptor,
// then emits the condition and the expression of every level the statement
// reaches. Returns the level it stopped at. The body is suspendable -- a
// rewinding pass restores the slots below from the frame it pushed and replays
// the one call its resume code names.
uint32_t CheckStatementLimits(EngineContext* ctx, uint32_t state, uint32_t func_state,
                              uint32_t message) {
  EnterFrame();
  uint32_t const kSaveFrameBytes = 32u;
  uint32_t const kResumeWordBytes = 4u;
  uint32_t const kLevelLimit = 201u;
  uint32_t const kLevelCount = 15u;
  uint32_t const kTooManyLevelsCall = 0u;
  uint32_t const kStatementStepCall = 1u;
  uint32_t const kNestedLimitsCall = 2u;
  uint32_t const kEmitStatementCall = 3u;
  uint32_t const kEncodeConstantCall = 4u;
  uint32_t const kVarargErrorCall = 5u;
  uint32_t const kVarargOpCall = 6u;
  uint32_t const kParamStepCall = 7u;
  uint32_t const kParamListCall = 8u;
  uint32_t const kMatchCall = 9u;
  uint32_t const kExpressionStepCall = 10u;
  uint32_t const kMainBodyCall = 11u;
  uint32_t const kLevelStepCall = 12u;
  uint32_t const kConditionCall = 13u;
  uint32_t const kNestedLevelCall = 14u;
  uint32_t const kExpressionCall = 15u;
  uint32_t carry = 0u;
  uint32_t level = 0u;
  uint32_t record = 0u;
  uint32_t holder = 0u;
  uint32_t scratch_base = 0u;
  uint32_t result = 0u;
  uint32_t resume_code = 0u;
  // Rewinding: pop the 32-byte frame the suspend path pushed and rebuild the
  // eight slots it saved.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    message = LoadU32(ctx, frame + 8u);
    carry = LoadU32(ctx, frame + 12u);
    level = LoadU32(ctx, frame + 16u);
    record = LoadU32(ctx, frame + 20u);
    holder = LoadU32(ctx, frame + 24u);
    scratch_base = LoadU32(ctx, frame + 28u);
    func_state = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // The second pop names the call that was in flight when the frame was pushed.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // One syntax level is spent on this statement; the counter lives in the
    // function record the state points at, and the frame slot below is scratch.
    scratch_base = GuestStackSp(ctx) - 32u;
    GuestStackSp(ctx) = scratch_base;
    record = LoadU32(ctx, state + 52u);
    carry = LoadU16(ctx, record + 52u) + 1u;
    StoreU16(ctx, record + 52u, carry);
    carry = static_cast<uint32_t>((carry & 0xFFFFu) >= kLevelLimit);
    level = 1u;
  }
  if (carry || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kTooManyLevelsCall) : 1u) {
      LexError(ctx, state, kCheckStatementLimitsMsgTooManyLevels, 0u);
      resume_code = kTooManyLevelsCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    carry = LoadU32(ctx, state + 16u);
    record = carry - 263u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The statement's first token picks the path; the reserved statement words
    // map onto expression kinds, the rest go through the operator check.
    switch (record) {
      case 0: goto emit_false;
      case 2: goto parse_params;
      case 6: goto emit_nil;
      case 7: goto statement_kind_ready;
      case 16: goto vararg_guard;
      case 21: goto emit_number;
      case 23: goto emit_constant;
      case 12: goto emit_true;
      case 1: case 3: case 4: case 5: case 8: case 9: case 10: case 11:
      case 13: case 14: case 15: case 17: case 18: case 19: case 20: case 22:
        goto match_expected;
      default: break;
    }
    switch (carry - 35u) {
      case 0: level = 2u; break;  // '#' length
      case 10: level = 0u; break;  // '-' unary minus
      case 1: case 2: case 3: case 4: case 5:
      case 6: case 7: case 8: case 9: goto match_expected;
      default: goto open_block;
    }
  statement_kind_ready:;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kStatementStepCall) : 1u) {
    StepRecordResume(ctx, state);
    resume_code = kStatementStepCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kNestedLimitsCall) : 1u) {
    uint32_t const nested = CheckStatementLimits(ctx, state, func_state, 8u);
    resume_code = kNestedLimitsCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    carry = nested;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    carry = LoadU32(ctx, state + 48u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kEmitStatementCall) : 1u) {
    EmitStatement(ctx, carry, level, func_state);
    resume_code = kEmitStatementCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto emit_levels;
  }
  emit_constant:;
  if (ResumePhase(ctx) == kResumeRunning) {
    carry = LoadU32(ctx, state + 24u);
    level = LoadU32(ctx, state + 48u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kEncodeConstantCall) : 1u) {
    uint32_t const constant = EncodeConstant(ctx, level, carry);
    resume_code = kEncodeConstantCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    level = constant;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU64(ctx, func_state + 16u, kCheckStatementLimitsNoNumber);
    StoreU32(ctx, func_state + 8u, level);
    StoreU32(ctx, func_state, 4u);
    goto expression_ready;
  }
  emit_nil:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU64(ctx, func_state + 16u, kCheckStatementLimitsNoNumber);
    StoreU32(ctx, func_state + 8u, 0u);
    StoreU32(ctx, func_state, 1u);
    goto expression_ready;
  }
  emit_true:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU64(ctx, func_state + 16u, kCheckStatementLimitsNoNumber);
    StoreU32(ctx, func_state + 8u, 0u);
    StoreU32(ctx, func_state, 2u);
    goto expression_ready;
  }
  emit_false:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU64(ctx, func_state + 16u, kCheckStatementLimitsNoNumber);
    StoreU32(ctx, func_state + 8u, 0u);
    StoreU32(ctx, func_state, 3u);
    goto expression_ready;
  }
  vararg_guard:;
  if (ResumePhase(ctx) == kResumeRunning) {
    record = LoadU32(ctx, state + 48u);
    level = LoadU32(ctx, record);
    carry = LoadU8(ctx, level + 74u);
    holder = static_cast<uint32_t>(carry == 0u);
  }
  if (holder || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kVarargErrorCall) : 1u) {
      RaiseSyntaxError(ctx, state, kCheckStatementLimitsMsgCannotUseOutside);
      resume_code = kVarargErrorCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) != kResumeRunning) {
      // A pass that resumes rebuilds the flag byte out of the record.
      level = LoadU32(ctx, record);
      carry = LoadU8(ctx, level + 74u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    carry = carry & 251u;
    StoreU8(ctx, level + 74u, carry);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kVarargOpCall) : 1u) {
    uint32_t const op_index = EmitOpInstruction(ctx, record, 37u, 0u, 1u, 0u);
    resume_code = kVarargOpCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    level = op_index;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU64(ctx, func_state + 16u, kCheckStatementLimitsNoNumber);
    StoreU32(ctx, func_state + 8u, level);
    StoreU32(ctx, func_state, 14u);
    goto expression_ready;
  }
  parse_params:;
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kParamStepCall) : 1u) {
    StepRecordResume(ctx, state);
    resume_code = kParamStepCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    carry = LoadU32(ctx, state + 4u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kParamListCall) : 1u) {
    ParseParamList(ctx, state, func_state, 0u, carry);
    resume_code = kParamListCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto emit_levels;
  }
  open_block:;
  if (ResumePhase(ctx) == kResumeRunning) {
    carry = static_cast<uint32_t>(carry == 123u);
    if (carry != 0u) {
      goto compile_body;
    }
  }
  // Merge point: every token that is not a statement keyword ends up here.
  match_expected:;
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kMatchCall) : 1u) {
    CheckMatch(ctx, state, func_state);
    resume_code = kMatchCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto emit_levels;
  }
  emit_number:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU64(ctx, func_state + 16u, kCheckStatementLimitsNoNumber);
    StoreU32(ctx, func_state + 8u, 0u);
    StoreU32(ctx, func_state, 5u);
    StoreF64(ctx, func_state + 8u, LoadF64(ctx, state + 24u));
  }
  // Merge point: the expression descriptor is filled in.
  expression_ready:;
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kExpressionStepCall) : 1u) {
    StepRecordResume(ctx, state);
    resume_code = kExpressionStepCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto emit_levels;
  }
  compile_body:;
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kMainBodyCall) : 1u) {
    CompileMainBody(ctx, state, func_state);
    resume_code = kMainBodyCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  // Merge point: the statement's own token decides the level it emits at.
  emit_levels:;
  if (ResumePhase(ctx) == kResumeRunning) {
    carry = kLevelCount;
    record = LoadU32(ctx, state + 16u);
    holder = record - 37u;
    level = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    switch (holder) {
      case 0: level = 4u; break;   // '%'
      case 5: level = 2u; break;   // '*'
      case 6: break;               // '+' is level 0
      case 8: level = 1u; break;   // '-'
      case 10: level = 3u; break;  // '/'
      case 23: level = 9u; break;  // '<'
      case 25: level = 11u; break;  // '>'
      case 57: level = 5u; break;  // '^'
      default:
        switch (record - 257u) {
          case 0: level = 13u; break;
          case 14: level = 14u; break;
          case 21: level = 6u; break;
          case 23: level = 8u; break;
          case 24: level = 12u; break;
          case 25: level = 10u; break;
          case 26: level = 7u; break;
          default: goto statement_done;
        }
    }
  }
  while (true) {
    // Walk the levels the statement reaches, emitting each level's condition and
    // expression and passing the message on to the level below.
    if (ResumePhase(ctx) == kResumeRunning) {
      carry = level << 1u;
      record = static_cast<uint32_t>(LoadU8(ctx, carry + kCheckStatementLimitsLevelGate) <=
                                     message);
      if (record != 0u) {
        carry = level;
        break;
      }
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kLevelStepCall) : 1u) {
      StepRecordResume(ctx, state);
      resume_code = kLevelStepCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      record = LoadU32(ctx, state + 48u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kConditionCall) : 1u) {
      EmitCondition(ctx, record, level, func_state);
      resume_code = kConditionCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      carry = LoadU8(ctx, carry + kCheckStatementLimitsLevelMessage);
      record = scratch_base + 8u;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kNestedLevelCall) : 1u) {
      uint32_t const nested = CheckStatementLimits(ctx, state, record, carry);
      resume_code = kNestedLevelCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      record = nested;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      holder = LoadU32(ctx, state + 48u);
      carry = scratch_base + 8u;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kExpressionCall) : 1u) {
      EmitExpression(ctx, holder, level, func_state, carry);
      resume_code = kExpressionCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      carry = kLevelCount;
      level = record;
      record = static_cast<uint32_t>(level != kLevelCount);
      if (record != 0u) {
        continue;
      }
    }
    break;
  }
  // Merge point: the statement ends, so the syntax level is given back and the
  // scratch frame is released.
  statement_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    state = LoadU32(ctx, state + 52u);
    func_state = LoadU16(ctx, state + 52u) - 1u;
    StoreU16(ctx, state + 52u, func_state);
    GuestStackSp(ctx) = scratch_base + 32u;
    result = carry;
    goto exit_path;
  }
  TrapUnreachable();
suspend_point:;
  {
    // Suspend: push the code naming the call in flight, step past it, then push
    // the 32-byte frame a rewinding pass reads back. The stack pointer lives in
    // the word the frame pointer itself names, so every push is a
    // read-modify-write of that word.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, state);
    StoreU32(ctx, frame + 4u, func_state);
    StoreU32(ctx, frame + 8u, message);
    StoreU32(ctx, frame + 12u, carry);
    StoreU32(ctx, frame + 16u, level);
    StoreU32(ctx, frame + 20u, record);
    StoreU32(ctx, frame + 24u, holder);
    StoreU32(ctx, frame + 28u, scratch_base);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  }
exit_path:;
  LeaveFrame();
  return result;
}
}  // namespace artc
