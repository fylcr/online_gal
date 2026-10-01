#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step calls.
void ReleaseExpressionSlots(EngineContext* ctx, uint32_t state, uint32_t instruction, uint32_t count, uint32_t unused);
void CheckMatch(EngineContext* ctx, uint32_t lex_state, uint32_t expected);
void EmitVariableLoad(EngineContext* ctx, uint32_t state, uint32_t variable, uint32_t register_hint);
uint32_t FormatPushString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
void StepRecordResume(EngineContext* ctx, uint32_t a0);
uint32_t CheckStatementLimits(EngineContext* ctx, uint32_t state, uint32_t func_state, uint32_t message);
uint32_t FormatCharValue(EngineContext* ctx, uint32_t a, uint32_t b);
void LexError(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t EmitOpInstruction(EngineContext* ctx, uint32_t func_state, uint32_t op, uint32_t arg_a, uint32_t arg_b, uint32_t line);
void CheckExprComplexity(EngineContext* ctx, uint32_t a, uint32_t b);
void PatchInstructionTarget(EngineContext* ctx, uint32_t state, uint32_t pending);
void ReserveRegisters(EngineContext* ctx, uint32_t state, uint32_t count);

namespace {

// The messages this step formats or raises: the token it expected, the two variable-limit
// diagnostics it chooses between, and the syntax error it reports for an unusable target.
constexpr uint32_t kParseAssignmentMsgExpected = 0x2B625u;              // '%s' expected
constexpr uint32_t kParseAssignmentMsgMainOverflow = 0x2B6C0u;         // main function: too many
constexpr uint32_t kParseAssignmentMsgFunctionOverflow = 0x2B6E2u;     // function: too many
constexpr uint32_t kParseAssignmentMsgSyntaxError = 0x2B872u;          // syntax error
constexpr uint32_t kParseAssignmentMsgVariablesInAssignment = 0x2B87Fu; // variables in assignment

}  // namespace

// Parses an assignment: it reconciles the target list left of the `=` with the values right of
// it, recording every target in the pending instruction list of the statement so the values can
// be released in the order the statement wants. Every call that can run guest code is a suspend
// gate: a rewinding pass restores the slots below and replays the one call its resume code
// names, and each call stores its resume code before it checks whether it suspended.
void ParseAssignment(EngineContext* ctx, uint32_t state, uint32_t target_slot, uint32_t value_slot) {
  EnterFrame();
  uint32_t const kSaveFrameBytes = 36u;
  uint32_t const kResumeWordBytes = 4u;
  // The codes the calls below spill, in the order they run: the syntax errors, the statement
  // records, the target match and the character and message formatters, then the statement and
  // complexity limits, the slot release, the emit and reserve steps, the two overflow messages,
  // the lex error and finally the recursive parse of the value list.
  uint32_t const kSyntaxErrorCall = 0u;
  uint32_t const kRecordStatementCall = 1u;
  uint32_t const kMatchExpectedCall = 2u;
  uint32_t const kFormatCharCall = 3u;
  uint32_t const kFormatExpectedCall = 4u;
  uint32_t const kSyntaxErrorValueCall = 5u;
  uint32_t const kRecordMainCall = 6u;
  uint32_t const kCheckMainLimitCall = 7u;
  uint32_t const kRecordLoopCall = 8u;
  uint32_t const kCheckComplexityCall = 9u;
  uint32_t const kCheckLoopLimitCall = 10u;
  uint32_t const kReleaseSlotsCall = 11u;
  uint32_t const kEmitOpCall = 12u;
  uint32_t const kReserveRegistersCall = 13u;
  uint32_t const kFormatMainOverflowCall = 14u;
  uint32_t const kFormatFunctionOverflowCall = 15u;
  uint32_t const kLexErrorCall = 16u;
  uint32_t const kRecurseCall = 17u;
  uint32_t const kEmitVariableLoadCall = 18u;
  // The frame saves one word per slot below, so each slot keeps a single name across the whole
  // walk even where a later region reuses it: the result of the call in flight, the carry the
  // flattened driver works in, the 80-byte scratch frame this step reserves on the guest stack,
  // the compiler handle, whether a variable has been bound, the base register of the variable
  // list (reused as that bound flag), the pending variable node, and the resume code.
  uint32_t call_result = 0u;
  uint32_t scratch = 0u;
  uint32_t guest_frame = 0u;
  uint32_t handle = 0u;
  uint32_t bound = 0u;
  uint32_t register_base = 0u;
  uint32_t variable = 0u;
  uint32_t resume_code = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 36-byte frame and restore every slot from it, in the order the
    // suspend path saved them.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, saved_frame);
    value_slot = LoadU32(ctx, saved_frame + 8u);
    scratch = LoadU32(ctx, saved_frame + 12u);
    guest_frame = LoadU32(ctx, saved_frame + 16u);
    handle = LoadU32(ctx, saved_frame + 20u);
    bound = LoadU32(ctx, saved_frame + 24u);
    register_base = LoadU32(ctx, saved_frame + 28u);
    variable = LoadU32(ctx, saved_frame + 32u);
    target_slot = LoadU32(ctx, saved_frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // The second pop names the call that was in flight. The two pops stay separate -- the
    // driver keeps one region at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Reserve the 80-byte scratch frame this step keeps its temporaries in, and note whether
    // the target is one of the kinds this parser can assign to.
    guest_frame = GuestStackSp(ctx) - 80u;
    GuestStackSp(ctx) = guest_frame;
    scratch = static_cast<uint32_t>((LoadU32(ctx, target_slot + 8u) - 6u) >= 4u);
  }
  if (scratch != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSyntaxErrorCall) : 1u) {
      // The target cannot take an assignment: report a syntax error.
      RaiseSyntaxError(ctx, state, kParseAssignmentMsgSyntaxError);
      resume_code = kSyntaxErrorCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Remember the pending list the target names, and the token type that picks which arm of
    // the assignment this is.
    variable = target_slot + 8u;
    scratch = LoadU32(ctx, state + 16u) - 44u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A plain name, the `:` that opens a variable list, or anything the assignment rejects.
    if (scratch == 0u) {
      goto variable_target;
    }
    if (scratch == 17u) {
      goto variable_list_target;
    }
    goto expected_equal_path;
  variable_target:;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kRecordStatementCall) : 1u) {
    StepRecordResume(ctx, state);
    resume_code = kRecordStatementCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, guest_frame + 48u, target_slot);
    scratch = guest_frame + 56u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kMatchExpectedCall) : 1u) {
    // Consume the `=` the target has to be followed by.
    CheckMatch(ctx, state, scratch);
    resume_code = kMatchExpectedCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A `=` opens the value list; a `:` opens a variable list whose entries are walked here.
    scratch = static_cast<uint32_t>(LoadU32(ctx, guest_frame + 56u) != 6u);
    if (scratch != 0u) {
      goto skip_variable_list;
    }
    handle = LoadU32(ctx, state + 48u);
    register_base = LoadU32(ctx, handle + 36u);
    for (;;) {
      if (LoadU32(ctx, target_slot + 8u) == 9u) {
        scratch = LoadU32(ctx, guest_frame + 64u);
        if (scratch == LoadU32(ctx, target_slot + 16u)) {
          StoreU32(ctx, target_slot + 16u, register_base);
          bound = 1u;
          scratch = LoadU32(ctx, guest_frame + 64u);
        }
        if (LoadU32(ctx, target_slot + 20u) == scratch) {
          // The second match closes this entry and steps to the next pending target.
          StoreU32(ctx, target_slot + 20u, register_base);
          bound = 1u;
          target_slot = LoadU32(ctx, target_slot);
          if (target_slot != 0u) {
            continue;
          }
          break;
        }
      }
      target_slot = LoadU32(ctx, target_slot);
      if (target_slot != 0u) {
        continue;
      }
      target_slot = static_cast<uint32_t>(bound == 0u);
      if (target_slot != 0u) {
        goto skip_variable_list;
      }
      scratch = LoadU32(ctx, guest_frame + 64u);
      break;
    }
    goto emit_instruction;
  }
  expected_equal_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    target_slot = LoadU32(ctx, state + 52u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFormatCharCall) : 1u) {
    // Format the `=` the message is about and leave it on the scratch frame.
    call_result = FormatCharValue(ctx, state, 61u);
    resume_code = kFormatCharCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    scratch = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, guest_frame, scratch);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFormatExpectedCall) : 1u) {
    call_result = FormatPushString(ctx, target_slot, kParseAssignmentMsgExpected, guest_frame);
    resume_code = kFormatExpectedCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    target_slot = call_result;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSyntaxErrorValueCall) : 1u) {
    RaiseSyntaxError(ctx, state, target_slot);
    resume_code = kSyntaxErrorValueCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  variable_list_target:;
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kRecordMainCall) : 1u) {
    StepRecordResume(ctx, state);
    resume_code = kRecordMainCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    target_slot = guest_frame + 48u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kCheckMainLimitCall) : 1u) {
    call_result = CheckStatementLimits(ctx, state, target_slot, 0u);
    resume_code = kCheckMainLimitCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    target_slot = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // While the names keep coming the target list grows, and each new name is recorded.
    scratch = static_cast<uint32_t>(LoadU32(ctx, state + 16u) == 44u);
    target_slot = 1u;
  }
  if (scratch != 0u || ResumePhase(ctx) == kResumeRewinding) {
    for (;;) {
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kRecordLoopCall) : 1u) {
        StepRecordResume(ctx, state);
        resume_code = kRecordLoopCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        handle = guest_frame + 48u;
        scratch = LoadU32(ctx, state + 48u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kCheckComplexityCall) : 1u) {
        CheckExprComplexity(ctx, scratch, handle);
        resume_code = kCheckComplexityCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        scratch = guest_frame + 48u;
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kCheckLoopLimitCall) : 1u) {
        call_result = CheckStatementLimits(ctx, state, scratch, 0u);
        resume_code = kCheckLoopLimitCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        scratch = call_result;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        target_slot = target_slot + 1u;
        scratch = static_cast<uint32_t>(LoadU32(ctx, state + 16u) == 44u);
        if (scratch != 0u) {
          continue;
        }
        break;
      }
      break;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Release the expression slots once the whole target list is known.
    handle = static_cast<uint32_t>(target_slot != value_slot);
    scratch = LoadU32(ctx, state + 48u);
  }
  if (handle != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      handle = guest_frame + 48u;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kReleaseSlotsCall) : 1u) {
      ReleaseExpressionSlots(ctx, scratch, value_slot, target_slot, handle);
      resume_code = kReleaseSlotsCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Hand the registers back: if the target list is no longer than the value list the
      // statement is done, otherwise the extra registers move down by the difference.
      if (static_cast<int32_t>(target_slot) <= static_cast<int32_t>(value_slot)) {
        goto statement_done;
      }
      scratch = LoadU32(ctx, state + 48u);
      target_slot = LoadU32(ctx, scratch + 36u) + (value_slot - target_slot);
      StoreU32(ctx, scratch + 36u, target_slot);
      goto statement_done;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // One `=` with one value: patch the pending target and emit the load for it.
    target_slot = guest_frame + 48u;
    PatchInstructionTarget(ctx, scratch, target_slot);
    state = LoadU32(ctx, state + 48u);
    goto emit_load;
  }
  emit_instruction:;
  if (ResumePhase(ctx) == kResumeRunning) {
    target_slot = LoadU32(ctx, handle + 36u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kEmitOpCall) : 1u) {
    // Emit the operator that performs the assignment.
    call_result = EmitOpInstruction(ctx, handle, 0u, target_slot, scratch, 0u);
    resume_code = kEmitOpCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    target_slot = call_result;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kReserveRegistersCall) : 1u) {
    ReserveRegisters(ctx, handle, 1u);
    resume_code = kReserveRegistersCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  skip_variable_list:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Bound the registers the operands may take: the value list has to fit in what the
    // statement still allows.
    uint32_t const limit = 200u - LoadU16(ctx, LoadU32(ctx, state + 52u) + 52u);
    target_slot = limit;
    scratch = static_cast<uint32_t>(static_cast<int32_t>(value_slot) > static_cast<int32_t>(limit));
  }
  if (scratch != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // Recompute what the overflow messages need: the compiler handle, the variable node and
      // whether the variable list was bound. The bound flag reuses the register-base slot.
      scratch = LoadU32(ctx, state + 48u);
      handle = LoadU32(ctx, scratch + 16u);
      bound = LoadU32(ctx, LoadU32(ctx, scratch) + 60u);
      register_base = static_cast<uint32_t>(bound == 0u);
    }
    if (register_base != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        // The main function cannot take more variables: format the count that overflowed.
        StoreU32(ctx, guest_frame + 20u, kParseAssignmentMsgVariablesInAssignment);
        StoreU32(ctx, guest_frame + 16u, target_slot);
        target_slot = guest_frame + 16u;
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFormatMainOverflowCall) : 1u) {
        call_result = FormatPushString(ctx, handle, kParseAssignmentMsgMainOverflow, target_slot);
        resume_code = kFormatMainOverflowCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        target_slot = call_result;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        goto report_function_limit;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // A nested function instead: the message names the variable and its line.
      StoreU32(ctx, guest_frame + 40u, kParseAssignmentMsgVariablesInAssignment);
      StoreU32(ctx, guest_frame + 36u, target_slot);
      StoreU32(ctx, guest_frame + 32u, bound);
      target_slot = guest_frame + 32u;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFormatFunctionOverflowCall) : 1u) {
      call_result = FormatPushString(ctx, handle, kParseAssignmentMsgFunctionOverflow,
                                     target_slot);
      resume_code = kFormatFunctionOverflowCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      target_slot = call_result;
    }
  report_function_limit:;
    if (ResumePhase(ctx) == kResumeRunning) {
      scratch = LoadU32(ctx, scratch + 12u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kLexErrorCall) : 1u) {
      LexError(ctx, scratch, target_slot, 0u);
      resume_code = kLexErrorCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Recurse into the value list: the values after the `,` are parsed by the same step.
    value_slot = value_slot + 1u;
    target_slot = guest_frame + 48u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kRecurseCall) : 1u) {
    ParseAssignment(ctx, state, target_slot, value_slot);
    resume_code = kRecurseCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  statement_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Close the value list: the pending node is linked back and its ends marked.
    state = LoadU32(ctx, state + 48u);
    target_slot = LoadU32(ctx, state + 36u);
    StoreU64(ctx, guest_frame + 64u, 0xFFFFFFFFFFFFFFFFull);
    StoreU32(ctx, guest_frame + 48u, 12u);
    target_slot = target_slot - 1u;
    StoreU32(ctx, guest_frame + 56u, target_slot);
  }
  emit_load:;
  if (ResumePhase(ctx) == kResumeRunning) {
    target_slot = guest_frame + 48u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kEmitVariableLoadCall) : 1u) {
    // Load the variable the assignment targets.
    EmitVariableLoad(ctx, state, variable, target_slot);
    resume_code = kEmitVariableLoadCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Give the scratch frame back.
    GuestStackSp(ctx) = guest_frame + 80u;
  }
  goto exit_path;
suspend_point:;
  {
    // Suspend: store the code of the call in flight, step past it, then push the 36-byte frame
    // a rewinding pass reads back -- one word per slot, in the order they were saved.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, saved_frame, state);
    StoreU32(ctx, saved_frame + 4u, target_slot);
    StoreU32(ctx, saved_frame + 8u, value_slot);
    StoreU32(ctx, saved_frame + 12u, scratch);
    StoreU32(ctx, saved_frame + 16u, guest_frame);
    StoreU32(ctx, saved_frame + 20u, handle);
    StoreU32(ctx, saved_frame + 24u, bound);
    StoreU32(ctx, saved_frame + 28u, register_base);
    StoreU32(ctx, saved_frame + 32u, variable);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  }
exit_path:;
  LeaveFrame();
}

}  // namespace artc
