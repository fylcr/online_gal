#include "util/engine_context.h"

namespace artc {

// Forward declarations -- the parser steps and the diagnostics this list walk
// drives.
void CompileMainBody(EngineContext* ctx, uint32_t a0, uint32_t a1);
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
void StepRecordResume(EngineContext* ctx, uint32_t a0);
uint32_t CheckStatementLimits(EngineContext* ctx, uint32_t state, uint32_t func_state,
                              uint32_t message);
uint32_t EmitOpInstruction(EngineContext* ctx, uint32_t func_state, uint32_t op, uint32_t arg_a,
                           uint32_t arg_b, uint32_t line);
void CheckExprComplexity(EngineContext* ctx, uint32_t a, uint32_t b);
void CheckClosingMatch(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d);
uint32_t EncodeConstant(EngineContext* ctx, uint32_t func_state, uint32_t value);
void AllocateExpressionRegister(EngineContext* ctx, uint32_t state, uint32_t instruction,
                                uint32_t expression);

namespace {

// "ambiguous syntax (function call x new statement)", by message table index.
constexpr uint32_t kParseArgumentsAmbiguousSyntaxMsg = 0x2B789u;
// "function arguments expected", by message table index.
constexpr uint32_t kParseArgumentsArgumentsExpectedMsg = 0x2B7BAu;
// The register handle that marks an expression slot as not yet allocated.
constexpr uint32_t kParseArgumentsNoRegister = 0xFFFFFFFFu;

}  // namespace

// Parses the argument list of a call: every argument expression is compiled into
// the next parameter slot, the list's closing token is matched against its
// opener, and the call is emitted with the operands the list produced.  The first
// parameter is the lexer state (word +16 holds the current token, +48 the
// compiler handle); the second is the descriptor the compiled call lands in.
// Suspendable: every parser step below can suspend.
void ParseArguments(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t const kSaveFrameBytes = 28u;
  uint32_t const kResumeWordBytes = 4u;
  // The codes the fourteen spill sites below name.
  uint32_t const kAmbiguousSyntaxCall = 0u;
  uint32_t const kFirstRecordCall = 1u;
  uint32_t const kFirstLimitsCall = 2u;
  uint32_t const kSecondRecordCall = 3u;
  uint32_t const kFirstComplexityCall = 4u;
  uint32_t const kSecondLimitsCall = 5u;
  uint32_t const kAllocateRegisterCall = 6u;
  uint32_t const kClosingMatchCall = 7u;
  uint32_t const kEncodeConstantCall = 8u;
  uint32_t const kThirdRecordCall = 9u;
  uint32_t const kArgumentsExpectedCall = 10u;
  uint32_t const kCompileBodyCall = 11u;
  uint32_t const kSecondComplexityCall = 12u;
  uint32_t const kEmitInstructionCall = 13u;
  // The frame saves one word per slot below, so each slot keeps a single name
  // across the whole walk even where a later region reuses it as a carry:
  // `value` starts as the current token, turns into a scratch flag, and ends as
  // the operand the emitted call carries; `flag` is the walk's own flag and then
  // the scratch word that decides how the call is emitted.
  uint32_t value = 0u;
  uint32_t flag = 0u;
  uint32_t argument = 0u;
  uint32_t compiler = 0u;
  uint32_t scratch_frame = 0u;
  uint32_t resume_code = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 28-byte frame the suspend path pushed and rebuild the
    // seven saved slots from it.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    a = LoadU32(ctx, frame);
    value = LoadU32(ctx, frame + 8u);
    scratch_frame = LoadU32(ctx, frame + 12u);
    compiler = LoadU32(ctx, frame + 16u);
    flag = LoadU32(ctx, frame + 20u);
    argument = LoadU32(ctx, frame + 24u);
    b = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // The second pop names the call that was in flight.  The two pops stay
    // separate -- one region is read back at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Claim the 32-byte scratch area the operands are built in, then sample the
    // current token, the compiler handle and the lexer's own word.
    scratch_frame = GuestStackSp(ctx) - 32u;
    GuestStackSp(ctx) = scratch_frame;
    value = LoadU32(ctx, a + 16u);
    flag = static_cast<uint32_t>(value != 286u);
    compiler = LoadU32(ctx, a + 48u);
    argument = LoadU32(ctx, a + 4u);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // A '(' opens the argument list proper and a '<' an operator argument;
      // anything else means the call has no list here, and the lexer's word is
      // compared against the token the parser saved.
      flag = static_cast<uint32_t>(value == 123u);
      if (flag != 0u) {
        goto compile_call;
      }
      value = static_cast<uint32_t>(value != 40u);
      if (value != 0u) {
        goto arguments_expected;
      }
      value = static_cast<uint32_t>(argument != LoadU32(ctx, a + 8u));
    }
    if (value != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kAmbiguousSyntaxCall) : 1u) {
        // The list opened on a line of its own: the call and the statement that
        // follows cannot be told apart.
        RaiseSyntaxError(ctx, a, kParseArgumentsAmbiguousSyntaxMsg);
        resume_code = kAmbiguousSyntaxCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstRecordCall) : 1u) {
      // Record where the list began, for the error report.
      StepRecordResume(ctx, a);
      resume_code = kFirstRecordCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      value = static_cast<uint32_t>(LoadU32(ctx, a + 16u) == 41u);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // A ')' closes the list at once, so the operand slot stays empty.
      if (value != 0u) {
        StoreU32(ctx, scratch_frame + 8u, 0u);
        goto check_closing;
      }
      value = scratch_frame + 8u;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstLimitsCall) : 1u) {
      // Charge the list against the statement limits before walking it.
      uint32_t const limits = CheckStatementLimits(ctx, a, value, 0u);
      resume_code = kFirstLimitsCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      value = limits;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      value = static_cast<uint32_t>(LoadU32(ctx, a + 16u) == 44u);
    }
    if (value != 0u || ResumePhase(ctx) == kResumeRewinding) {
      for (;;) {
        if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondRecordCall) : 1u) {
          StepRecordResume(ctx, a);
          resume_code = kSecondRecordCall;
          if (ResumePhase(ctx) == kResumeSuspended) {
            goto suspend_point;
          }
        }
        if (ResumePhase(ctx) == kResumeRunning) {
          // The operand under construction lives in the scratch area's first
          // word; the compiler handle is re-read for the step below.
          flag = scratch_frame + 8u;
          value = LoadU32(ctx, a + 48u);
        }
        if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kFirstComplexityCall) : 1u) {
          CheckExprComplexity(ctx, value, flag);
          resume_code = kFirstComplexityCall;
          if (ResumePhase(ctx) == kResumeSuspended) {
            goto suspend_point;
          }
        }
        if (ResumePhase(ctx) == kResumeRunning) {
          value = scratch_frame + 8u;
        }
        if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondLimitsCall) : 1u) {
          uint32_t const limits = CheckStatementLimits(ctx, a, value, 0u);
          resume_code = kSecondLimitsCall;
          if (ResumePhase(ctx) == kResumeSuspended) {
            goto suspend_point;
          }
          value = limits;
        }
        if (ResumePhase(ctx) == kResumeRunning) {
          // The list continues while the lexer still sits on the separator.
          value = static_cast<uint32_t>(LoadU32(ctx, a + 16u) == 44u);
          if (value != 0u) {
            continue;
          }
        }
        break;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      value = scratch_frame + 8u;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kAllocateRegisterCall) : 1u) {
      // Hand the operand to the slot allocator; no register holds it yet.
      AllocateExpressionRegister(ctx, compiler, value, kParseArgumentsNoRegister);
      resume_code = kAllocateRegisterCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
  check_closing:;
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kClosingMatchCall) : 1u) {
      // The list has to end on the token its opener promised.
      CheckClosingMatch(ctx, a, 41u, 40u, argument);
      resume_code = kClosingMatchCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      goto emit_call;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    value = LoadU32(ctx, a + 24u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kEncodeConstantCall) : 1u) {
    // Encode the constant the list reduced to.
    uint32_t const encoded = EncodeConstant(ctx, compiler, value);
    resume_code = kEncodeConstantCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    value = encoded;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Fill the scratch list with the constant that was just encoded.
    StoreU64(ctx, scratch_frame + 24u, 0xFFFFFFFFFFFFFFFFull);
    StoreU32(ctx, scratch_frame + 16u, value);
    StoreU32(ctx, scratch_frame + 8u, 4u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kThirdRecordCall) : 1u) {
    StepRecordResume(ctx, a);
    resume_code = kThirdRecordCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto emit_call;
  }
arguments_expected:;
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kArgumentsExpectedCall) : 1u) {
    // What follows the callee is not an argument list at all.
    RaiseSyntaxError(ctx, a, kParseArgumentsArgumentsExpectedMsg);
    resume_code = kArgumentsExpectedCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto release_scratch;
  }
compile_call:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The list is complete: its operand pointer is the scratch area's first slot.
    value = scratch_frame + 8u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kCompileBodyCall) : 1u) {
    // Compile the call body the argument list belongs to.
    CompileMainBody(ctx, a, value);
    resume_code = kCompileBodyCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
emit_call:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Sample the scratch word that names the list's shape, and the descriptor
    // word the call's operand is measured against.
    value = 0u;
    flag = LoadU32(ctx, scratch_frame + 8u);
    a = LoadU32(ctx, b + 8u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // An empty list skips the operand load; a register or constant form skips
    // the operand resolution as well.
    if (flag == 0u) {
      goto slot_operand;
    }
    if (flag == 13u || flag == 14u) {
      goto emit_instruction;
    }
    value = scratch_frame + 8u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondComplexityCall) : 1u) {
    CheckExprComplexity(ctx, compiler, value);
    resume_code = kSecondComplexityCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
slot_operand:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The operand is the slot distance from the frame base.
    value = LoadU32(ctx, compiler + 36u) - a;
  }
emit_instruction:;
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kEmitInstructionCall) : 1u) {
    uint32_t const emitted = EmitOpInstruction(ctx, compiler, 28u, a, value, 2u);
    resume_code = kEmitInstructionCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    value = emitted;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Answer the caller with the compiled call: its kind and operand on the
    // descriptor, and the argument word patched into the emitted code.
    StoreU64(ctx, b + 16u, 0xFFFFFFFFFFFFFFFFull);
    StoreU32(ctx, b + 8u, value);
    StoreU32(ctx, b, 13u);
    uint32_t const code_base = LoadU32(ctx, LoadU32(ctx, compiler) + 20u);
    StoreU32(ctx, code_base + (LoadU32(ctx, compiler + 24u) << 2u) - 4u, argument);
    StoreU32(ctx, compiler + 36u, a + 1u);
  }
release_scratch:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Give the 32-byte scratch area back to the guest stack.
    GuestStackSp(ctx) = scratch_frame + 32u;
  }
  goto done;
suspend_point:;
  {
    // Suspend: push the code naming the call in flight, step past it, then push
    // the 28-byte frame a rewinding pass reads back.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, a);
    StoreU32(ctx, frame + 4u, b);
    StoreU32(ctx, frame + 8u, value);
    StoreU32(ctx, frame + 12u, scratch_frame);
    StoreU32(ctx, frame + 16u, compiler);
    StoreU32(ctx, frame + 20u, flag);
    StoreU32(ctx, frame + 24u, argument);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  }
done:;
  LeaveFrame();
}

}  // namespace artc
