#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
void RaiseSyntaxError(EngineContext* ctx, uint32_t state, uint32_t message);
uint32_t EmitInstruction(EngineContext* ctx, uint32_t func_state, uint32_t instruction,
                         uint32_t line);
void DischargeExpression(EngineContext* ctx, uint32_t func_state, uint32_t expr);
uint32_t EmitConditionJump(EngineContext* ctx, uint32_t func_state, uint32_t condition);
void CompileNumericPower(EngineContext* ctx, uint32_t func_state, uint32_t opcode, uint32_t expr,
                         uint32_t operand);
void ExpressionToOperand(EngineContext* ctx, uint32_t func_state, uint32_t expr, uint32_t operand);

namespace {

// The byte table a code word's low six bits are classified with, the base an
// operand index is counted from, and the masks a rewritten code word is rebuilt
// from.
constexpr uint32_t kEmitStatementOperandKinds = 0x2B460u;
constexpr uint32_t kEmitStatementIndexBase = 0x1FFFFu;
constexpr uint32_t kEmitStatementValueMask = 0x7FC000u;
constexpr uint32_t kEmitStatementOperandMask = 0xFFFFC03Fu;
constexpr uint32_t kEmitStatementNone = 0xFFFFFFFFu;
constexpr uint64_t kEmitStatementNonePair = 0xFFFFFFFFFFFFFFFFull;
constexpr uint32_t kEmitStatementTooComplexMsg = 0x2A371u;  // function or expression too complex

}  // namespace

// Emits the code for one statement: the statement kind the caller passes -- a
// reserved word, an operator or a nested block -- selects the emit path, and the
// register window the statement opened is closed again. The body is suspendable
// -- a rewinding pass restores the slots below from the frame it pushed and
// replays the one call its resume code names.
void EmitStatement(EngineContext* ctx, uint32_t func_state, uint32_t value, uint32_t expr) {
  EnterFrame();
  uint32_t const kSaveFrameBytes = 32u;
  uint32_t const kResumeWordBytes = 4u;
  uint32_t const kConditionJumpCall = 0u;
  uint32_t const kPowerCall = 1u;
  uint32_t const kDischargeCall = 2u;
  uint32_t const kRaiseErrorCall = 3u;
  uint32_t const kToOperandCall = 4u;
  uint32_t const kInstructionCall = 5u;
  uint32_t const kSecondConditionJumpCall = 6u;
  uint32_t const kSecondPowerCall = 7u;
  uint32_t slot = 0u;
  uint32_t word = 0u;
  uint32_t index = 0u;
  uint32_t window = 0u;
  uint32_t work = 0u;
  uint32_t resume_code = 0u;
  // Rewinding: pop the 32-byte frame the suspend path pushed and rebuild the
  // eight slots it saved.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    func_state = LoadU32(ctx, frame);
    value = LoadU32(ctx, frame + 4u);
    expr = LoadU32(ctx, frame + 8u);
    slot = LoadU32(ctx, frame + 12u);
    word = LoadU32(ctx, frame + 16u);
    index = LoadU32(ctx, frame + 20u);
    window = LoadU32(ctx, frame + 24u);
    work = LoadU32(ctx, frame + 28u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // The second pop names the call that was in flight when the frame was pushed.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A 32-byte register window is carved off the guest stack; its record starts
    // eight bytes in, holds no operand and ends the window chain.
    uint32_t const sp = GuestStackSp(ctx);
    window = sp - 32u;
    GuestStackSp(ctx) = window;
    StoreU64(ctx, window + 24u, kEmitStatementNonePair);
    StoreU64(ctx, window + 16u, 0ull);
    StoreU32(ctx, window + 8u, 5u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The statement kind selects the emit path.
    switch (value) {
      case 0: goto window_path;
      case 1: goto discharge_path;
      case 2: goto condition_path;
      default: goto statement_done;
    }
  window_path:;
    value = static_cast<uint32_t>(LoadU32(ctx, expr) != 5u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // An expression already sitting in the window takes the window record; one
    // that still has operands is emitted through the conditional jump emitter.
    if (value == 0u) {
      value = static_cast<uint32_t>(LoadU32(ctx, expr + 16u) != kEmitStatementNone);
      if (value == 0u) {
        value = static_cast<uint32_t>(LoadU32(ctx, expr + 20u) == kEmitStatementNone);
        if (value != 0u) {
          goto window_record;
        }
      }
    }
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kConditionJumpCall) : 1u) {
    uint32_t const jump = EmitConditionJump(ctx, func_state, expr);
    resume_code = kConditionJumpCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    value = jump;
  }
  // Merge point: the value the statement leaves behind is ready.
window_record:;
  value = ResumePhase(ctx) != kResumeRunning ? value : (window + 8u);
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kPowerCall) : 1u) {
    CompileNumericPower(ctx, func_state, 18u, expr, value);
    resume_code = kPowerCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto statement_done;
  }
discharge_path:;
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kDischargeCall) : 1u) {
    DischargeExpression(ctx, func_state, expr);
    resume_code = kDischargeCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    value = LoadU32(ctx, expr) - 1u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The operator picks how the two operands it was given are combined.
    switch (value) {
      case 0: goto store_two;
      case 1: goto store_three;
      case 2: goto store_two;
      case 3: goto store_three;
      case 4: goto store_three;
      case 5: case 6: case 7: case 8: goto window_done;
      case 9: goto load_operand;
      case 10: goto open_register;
      case 11: goto close_operand_path;
      default: goto window_done;
    }
  store_two:;
    StoreU32(ctx, expr, 2u);
    goto window_done;
  store_three:;
    StoreU32(ctx, expr, 3u);
    goto window_done;
  load_operand:;
    slot = LoadU32(ctx, LoadU32(ctx, func_state) + 12u);
    value = LoadU32(ctx, expr + 8u);
    slot = slot + (value << 2u);
    if (static_cast<int32_t>(value) >= 1) {
      word = slot - 4u;
      value = LoadU32(ctx, word);
      if (static_cast<int32_t>(LoadS8(ctx, (value & 63u) + kEmitStatementOperandKinds)) < 0) {
        goto operand_rewrite;
      }
    }
    value = LoadU32(ctx, slot);
    word = slot;
  operand_rewrite:;
    // Set the "operand is a constant" bit unless the operand already carries one.
    value = (value & kEmitStatementOperandMask) |
            (static_cast<uint32_t>((value & 16320u) == 0u) << 6u);
    StoreU32(ctx, word, value);
    goto window_done;
  open_register:;
    index = LoadU32(ctx, func_state + 36u);
    word = index + 1u;
    slot = LoadU32(ctx, func_state);
    value = static_cast<uint32_t>(
        static_cast<int32_t>(index) < static_cast<int32_t>(LoadU8(ctx, slot + 75u)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (value != 0u) {
      value = word;
      goto window_ready;
    }
    work = static_cast<uint32_t>(static_cast<int32_t>(index) >= 249);
    value = word;
  }
  if (work || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      value = LoadU32(ctx, func_state + 12u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kRaiseErrorCall) : 1u) {
      RaiseSyntaxError(ctx, value, kEmitStatementTooComplexMsg);
      resume_code = kRaiseErrorCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    // A resumed pass keeps the code pointer it saved; the running pass restarts
    // the window at the next register.
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, func_state);
      value = LoadU32(ctx, func_state + 36u) + 1u;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, slot + 75u, word);
  }
  // Merge point: the register the statement closed the window at.
window_ready:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, func_state + 36u, value);
    value = value - 1u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kToOperandCall) : 1u) {
    ExpressionToOperand(ctx, func_state, expr, value);
    resume_code = kToOperandCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    value = static_cast<uint32_t>(LoadU32(ctx, expr) != 12u);
    if (value == 0u) {
      goto close_operand_path;
    }
  }
close_operand_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Give the register back unless the operand reserves one of its own or the
    // register it landed in is at the depth limit.
    value = LoadU32(ctx, expr + 8u);
    slot = value & 256u;
    if (slot == 0u) {
      slot = LoadU8(ctx, func_state + 50u);
      value = static_cast<uint32_t>(static_cast<int32_t>(slot) > static_cast<int32_t>(value));
      if (value == 0u) {
        value = LoadU32(ctx, func_state + 36u) - 1u;
        StoreU32(ctx, func_state + 36u, value);
      }
    }
  }
instruction_ready:;
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = (LoadU32(ctx, expr + 8u) << 23u) | 19u;
    value = LoadU32(ctx, LoadU32(ctx, func_state + 12u) + 8u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kInstructionCall) : 1u) {
    uint32_t const instruction = EmitInstruction(ctx, func_state, slot, value);
    resume_code = kInstructionCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    value = instruction;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, expr, 11u);
    StoreU32(ctx, expr + 8u, value);
  }
window_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The statement is emitted: the window it opened takes the tail of the
    // operand chain over from the expression it was compiled for.
    value = LoadU32(ctx, expr + 20u);
    index = LoadU32(ctx, expr + 16u);
    StoreU32(ctx, expr + 20u, index);
    StoreU32(ctx, expr + 16u, value);
    if (index != kEmitStatementNone) {
      work = LoadU32(ctx, LoadU32(ctx, func_state) + 12u);
      while (true) {
        value = work + (index << 2u);
        if (static_cast<int32_t>(index) >= 1) {
          slot = value - 4u;
          word = LoadU32(ctx, slot);
          if (static_cast<int32_t>(LoadS8(ctx, (word & 63u) + kEmitStatementOperandKinds)) < 0) {
            goto word_loaded;
          }
        }
        word = LoadU32(ctx, value);
        slot = value;
      word_loaded:;
        if ((word & 63u) == 27u) {
          // Rebuild the operand that pointed at the window as a register
          // reference.
          StoreU32(ctx, slot, ((word >> 17u) & 32704u) | (word & kEmitStatementValueMask) | 26u);
        }
        value = (LoadU32(ctx, value) >> 14u) - kEmitStatementIndexBase;
        index = (value == kEmitStatementNone) ? kEmitStatementNone : (index + value + 1u);
        if (index == kEmitStatementNone) {
          break;
        }
      }
      value = LoadU32(ctx, expr + 16u);
    }
    if (value == kEmitStatementNone) {
      goto statement_done;
    }
    // The statement is closed, so the expression slot takes the code array over
    // and the function record slot carries the word address it is walked with.
    expr = LoadU32(ctx, LoadU32(ctx, func_state) + 12u);
    while (true) {
      func_state = expr + (value << 2u);
      if (static_cast<int32_t>(value) >= 1) {
        slot = func_state - 4u;
        word = LoadU32(ctx, slot);
        if (static_cast<int32_t>(LoadS8(ctx, (word & 63u) + kEmitStatementOperandKinds)) < 0) {
          goto trailing_loaded;
        }
      }
      word = LoadU32(ctx, func_state);
      slot = func_state;
    trailing_loaded:;
      if ((word & 63u) == 27u) {
        StoreU32(ctx, slot, ((word >> 17u) & 32704u) | (word & kEmitStatementValueMask) | 26u);
      }
      func_state = (LoadU32(ctx, func_state) >> 14u) - kEmitStatementIndexBase;
      value = (func_state == kEmitStatementNone) ? kEmitStatementNone : (value + func_state + 1u);
      if (value == kEmitStatementNone) {
        break;
      }
    }
    goto statement_done;
  }
condition_path:;
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondConditionJumpCall) : 1u) {
    uint32_t const jump = EmitConditionJump(ctx, func_state, expr);
    resume_code = kSecondConditionJumpCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    value = jump;
  }
  value = ResumePhase(ctx) != kResumeRunning ? value : (window + 8u);
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondPowerCall) : 1u) {
    CompileNumericPower(ctx, func_state, 20u, expr, value);
    resume_code = kSecondPowerCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  // Merge point: every statement kind leaves through here.
statement_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = window + 32u;
  }
  goto exit_path;
suspend_point:;
  {
    // Suspend: push the code naming the call in flight, step past it, then push
    // the 32-byte frame a rewinding pass reads back. The stack pointer lives in
    // the word the frame pointer itself names, so every push is a
    // read-modify-write of that word.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, func_state);
    StoreU32(ctx, frame + 4u, value);
    StoreU32(ctx, frame + 8u, expr);
    StoreU32(ctx, frame + 12u, slot);
    StoreU32(ctx, frame + 16u, word);
    StoreU32(ctx, frame + 20u, index);
    StoreU32(ctx, frame + 24u, window);
    StoreU32(ctx, frame + 28u, work);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  }
exit_path:;
  LeaveFrame();
}
}  // namespace artc
