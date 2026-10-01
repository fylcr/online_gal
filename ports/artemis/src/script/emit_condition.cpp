#include "util/engine_context.h"

namespace artc {

// Forward declarations -- registered callees.
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
uint32_t ExpressionToRegister(EngineContext* ctx, uint32_t func_state, uint32_t expr);
void DischargeExpression(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t ResolveBranchOffset(EngineContext* ctx, uint32_t state, uint32_t opcode,
                             uint32_t operands, uint32_t extra, uint32_t target);
void CompileExpression(EngineContext* ctx, uint32_t func_state, uint32_t code, uint32_t line);
void EmitVariableStore(EngineContext* ctx, uint32_t state, uint32_t variable);
void ExpressionToOperand(EngineContext* ctx, uint32_t func_state, uint32_t expr,
                         uint32_t operand);

namespace {

// How a bytecode cell names a jump: the low 14 bits carry the operand and the
// upper bits the distance, biased by the widest offset a jump can name.
constexpr uint32_t kEmitConditionJumpFieldMask = 16383u;
constexpr uint32_t kEmitConditionMaxJumpOffset = 0x1FFFFu;
constexpr uint32_t kEmitConditionJumpBias = 0x7FFFC000u;
constexpr uint32_t kEmitConditionJumpRangeLimit = 0x20000u;
// The expression record's "no jump target" sentinel.
constexpr uint32_t kEmitConditionNoTarget = 0xFFFFFFFFu;
// Parser diagnostics raised when the condition cannot be encoded.
constexpr uint32_t kEmitConditionMsgFunctionOrExpressionToo = 0x2A371u;
constexpr uint32_t kEmitConditionMsgControlStructureTooLong = 0x2A394u;

}  // namespace

// Emits the condition of a control statement as a comparison plus a conditional
// jump, and patches the jump distance into the bytecode cell the test occupies.
// The body is suspendable -- a rewinding pass restores the slots below from the
// frame it pushed and replays the one call its resume code names.
void EmitCondition(EngineContext* ctx, uint32_t func_state, uint32_t opcode, uint32_t expr) {
  EnterFrame();
  uint32_t const kSaveFrameBytes = 44u;
  uint32_t const kResumeWordBytes = 4u;
  uint32_t const kVariableStoreCall = 0u;
  uint32_t const kDischargeCall = 1u;
  uint32_t const kJumpOffsetCall = 2u;
  uint32_t const kOperandComplexityCall = 3u;
  uint32_t const kOperandCall = 4u;
  uint32_t const kSecondOffsetCall = 5u;
  uint32_t const kJumpRangeCall = 6u;
  uint32_t const kDischargeOnlyCall = 7u;
  uint32_t const kDischargeComplexityCall = 8u;
  uint32_t const kCompileCall = 9u;
  uint32_t const kRegisterCall = 10u;
  uint32_t const kRegisterFallbackCall = 11u;
  uint32_t const kTargetRangeCall = 12u;
  uint32_t operand = 0u;
  uint32_t scratch = 0u;
  uint32_t value = 0u;
  uint32_t position = 0u;
  uint32_t cell = 0u;
  uint32_t table_base = 0u;
  uint32_t over_complex = 0u;
  uint32_t jump_out_of_range = 0u;
  uint32_t target_out_of_range = 0u;
  uint32_t resume_code = 0u;
  // Rewinding: pop the 44-byte frame the suspend path pushed and rebuild the
  // eleven slots it saved.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    func_state = LoadU32(ctx, frame);
    expr = LoadU32(ctx, frame + 8u);
    scratch = LoadU32(ctx, frame + 12u);
    value = LoadU32(ctx, frame + 16u);
    position = LoadU32(ctx, frame + 20u);
    cell = LoadU32(ctx, frame + 24u);
    table_base = LoadU32(ctx, frame + 28u);
    over_complex = LoadU32(ctx, frame + 32u);
    jump_out_of_range = LoadU32(ctx, frame + 36u);
    target_out_of_range = LoadU32(ctx, frame + 40u);
    operand = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // The second pop names the call that was in flight when the frame was pushed.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    switch (opcode) {
      case 0: case 1: case 2: case 3: case 4: case 5: goto simple_condition;
      case 6: goto discharge_only;
      case 13: break;  // The variable store is the only work this kind does.
      case 14: goto discharge_path;
      default: goto register_path;
    }
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kVariableStoreCall) : 1u) {
    EmitVariableStore(ctx, func_state, expr);
    resume_code = kVariableStoreCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto exit_path;
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
    operand = LoadU32(ctx, expr) - 1u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    switch (operand) {
      case 0:
      case 2: goto condition_tail;
      case 9: goto folded_operand;
      case 10: goto table_operand;
      case 11: goto patch_operand;
      default: goto direct_operand;
    }
  folded_operand:;
    operand = LoadU32(ctx, expr + 8u);
    goto emit_jump;
  table_operand:;
    operand = LoadU32(ctx, func_state);
    scratch = LoadU32(ctx, LoadU32(ctx, operand + 12u) + (LoadU32(ctx, expr + 8u) << 2u));
    value = static_cast<uint32_t>((scratch & 63u) != 19u);
    if (value != 0u) {
      goto write_operand;
    }
    StoreU32(ctx, func_state + 24u, LoadU32(ctx, func_state + 24u) - 1u);
    operand = scratch >> 23u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kJumpOffsetCall) : 1u) {
    uint32_t const offset = ResolveBranchOffset(ctx, func_state, 26u, operand, 0u, 0u);
    resume_code = kJumpOffsetCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    operand = offset;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto emit_jump;
  }
  direct_operand:;
  if (ResumePhase(ctx) == kResumeRunning) {
    operand = LoadU32(ctx, func_state);
  }
  // Merge point: the operand byte is written by whichever form produced one.
  write_operand:;
  if (ResumePhase(ctx) == kResumeRunning) {
    position = LoadU32(ctx, func_state + 36u);
    value = position + 1u;
    scratch = static_cast<uint32_t>(static_cast<int32_t>(position) <
                                     static_cast<int32_t>(LoadU8(ctx, operand + 75u)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (scratch != 0u) {
      scratch = value;
      goto operand_recorded;
    }
    cell = static_cast<uint32_t>(static_cast<int32_t>(position) >= 249);
    scratch = value;
  }
  // Only the running pass derives this flag; a pass that resumes keeps the copy
  // the frame restored.
  if (ResumePhase(ctx) == kResumeRunning) {
    over_complex = cell;
  }
  if (over_complex || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      operand = LoadU32(ctx, func_state + 12u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kOperandComplexityCall) : 1u) {
      RaiseSyntaxError(ctx, operand, kEmitConditionMsgFunctionOrExpressionToo);
      resume_code = kOperandComplexityCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      scratch = LoadU32(ctx, func_state + 36u) + 1u;
      operand = LoadU32(ctx, func_state);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, operand + 75u, value);
  }
  // Merge point: the operand byte is accounted for and the cursor advances.
  operand_recorded:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, func_state + 36u, scratch);
    operand = scratch - 1u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kOperandCall) : 1u) {
    ExpressionToOperand(ctx, func_state, expr, operand);
    resume_code = kOperandCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadU32(ctx, expr) != 12u) {
      goto operand_loaded;
    }
  }
  patch_operand:;
  if (ResumePhase(ctx) == kResumeRunning) {
    operand = LoadU32(ctx, expr + 8u);
    scratch = operand & 256u;
    if (scratch != 0u) {
      goto operand_loaded;
    }
    scratch = LoadU8(ctx, func_state + 50u);
    operand = static_cast<uint32_t>(static_cast<int32_t>(scratch) >
                                    static_cast<int32_t>(operand));
    if (operand != 0u) {
      goto operand_loaded;
    }
    operand = LoadU32(ctx, func_state + 36u) - 1u;
    StoreU32(ctx, func_state + 36u, operand);
  }
  // Merge point: the operand cell has been chosen.
  operand_loaded:;
  if (ResumePhase(ctx) == kResumeRunning) {
    operand = LoadU32(ctx, expr + 8u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSecondOffsetCall) : 1u) {
    uint32_t const offset = ResolveBranchOffset(ctx, func_state, 27u, 255u, operand, 1u);
    resume_code = kSecondOffsetCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    operand = offset;
  }
  // Merge point: both operand forms continue at the jump encoder.
  emit_jump:;
  if (ResumePhase(ctx) == kResumeRunning) {
    position = operand;
    if (operand == kEmitConditionNoTarget) {
      goto condition_tail;
    }
    operand = LoadU32(ctx, expr + 16u);
    if (operand == kEmitConditionNoTarget) {
      goto store_target;
    }
    table_base = LoadU32(ctx, LoadU32(ctx, func_state) + 12u);
    // Follow the chain of cells the target names, summing the distances they
    // carry until a cell ends the chain.
    do {
      scratch = operand;
      uint32_t acc = kEmitConditionNoTarget;
      uint32_t next = scratch;
      cell = table_base + (operand << 2u);
      value = LoadU32(ctx, cell);
      operand = (value >> 14u) - kEmitConditionMaxJumpOffset;
      next += operand + 1u;
      acc = (operand == kEmitConditionNoTarget) ? acc : next;
      operand = acc;
      if (operand == kEmitConditionNoTarget) {
        break;
      }
    } while (true);
    table_base = cell;
    operand = position + (scratch ^ kEmitConditionNoTarget);
    scratch = static_cast<uint32_t>(static_cast<int32_t>(operand) >> 31);
    position = operand + scratch;
    scratch = static_cast<uint32_t>(static_cast<int32_t>(scratch ^ position) >=
                                    static_cast<int32_t>(kEmitConditionJumpRangeLimit));
  }
  // Only the running pass derives this flag; a resumed pass keeps the frame's copy.
  if (ResumePhase(ctx) == kResumeRunning) {
    jump_out_of_range = scratch;
  }
  if (jump_out_of_range || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      scratch = LoadU32(ctx, func_state + 12u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kJumpRangeCall) : 1u) {
      RaiseSyntaxError(ctx, scratch, kEmitConditionMsgControlStructureTooLong);
      resume_code = kJumpRangeCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      value = LoadU32(ctx, cell);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, table_base,
             ((value & kEmitConditionJumpFieldMask) | (operand << 14u)) +
                 kEmitConditionJumpBias);
    goto condition_tail;
  }
  discharge_only:;
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kDischargeOnlyCall) : 1u) {
    DischargeExpression(ctx, func_state, expr);
    resume_code = kDischargeOnlyCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadU32(ctx, expr) != 12u) {
      goto patch_discharged;
    }
    operand = LoadU32(ctx, expr + 8u);
    if ((operand & 256u) != 0u) {
      goto patch_discharged;
    }
    if (static_cast<int32_t>(LoadU8(ctx, func_state + 50u)) > static_cast<int32_t>(operand)) {
      goto patch_discharged;
    }
    StoreU32(ctx, func_state + 36u, LoadU32(ctx, func_state + 36u) - 1u);
    // Merge point: the discharged operand is patched in place.
    patch_discharged:;
    position = LoadU32(ctx, func_state + 36u);
    scratch = position + 1u;
    value = LoadU32(ctx, func_state);
    operand = static_cast<uint32_t>(static_cast<int32_t>(position) <
                                    static_cast<int32_t>(LoadU8(ctx, value + 75u)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (operand != 0u) {
      operand = scratch;
      goto discharged_operand;
    }
    cell = static_cast<uint32_t>(static_cast<int32_t>(position) >= 249);
    operand = scratch;
  }
  if (cell || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      operand = LoadU32(ctx, func_state + 12u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kDischargeComplexityCall) : 1u) {
      RaiseSyntaxError(ctx, operand, kEmitConditionMsgFunctionOrExpressionToo);
      resume_code = kDischargeComplexityCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      value = LoadU32(ctx, func_state);
      operand = LoadU32(ctx, func_state + 36u) + 1u;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, value + 75u, scratch);
  }
  // Merge point: the discharged operand byte is accounted for.
  discharged_operand:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, func_state + 36u, operand);
    operand = operand - 1u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kCompileCall) : 1u) {
    CompileExpression(ctx, func_state, expr, operand);
    resume_code = kCompileCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto exit_path;
  }
  simple_condition:;
  // A plain variable or constant test needs no operand work: it goes straight to
  // the register conversion.
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadU32(ctx, expr) == 5u && LoadU32(ctx, expr + 16u) == kEmitConditionNoTarget &&
        LoadU32(ctx, expr + 20u) == kEmitConditionNoTarget) {
      goto register_done;
    }
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kRegisterCall) : 1u) {
    uint32_t const register_number = ExpressionToRegister(ctx, func_state, expr);
    resume_code = kRegisterCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    func_state = register_number;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto exit_path;
  }
  register_path:;
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kRegisterFallbackCall) : 1u) {
    uint32_t const register_number = ExpressionToRegister(ctx, func_state, expr);
    resume_code = kRegisterFallbackCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    func_state = register_number;
  }
  // Merge point: both register forms share the same tail.
  register_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    goto exit_path;
  }
  // Merge point: a jump with no resolvable target still records its cell.
  store_target:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, expr + 16u, position);
  }
  // Merge point: the condition tail a jump lands on when it has no distance to
  // encode.
  condition_tail:;
  if (ResumePhase(ctx) == kResumeRunning) {
    position = LoadU32(ctx, expr + 20u);
    StoreU32(ctx, func_state + 28u, LoadU32(ctx, func_state + 24u));
    operand = static_cast<uint32_t>(position == kEmitConditionNoTarget);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (operand != 0u) {
      goto clear_patch;
    }
    operand = LoadU32(ctx, func_state + 32u);
    scratch = static_cast<uint32_t>(operand != kEmitConditionNoTarget);
  }
  if (scratch || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      table_base = LoadU32(ctx, LoadU32(ctx, func_state) + 12u);
      // Follow the chain of cells the recorded target names, exactly as the
      // jump encoder above does.
      do {
        scratch = operand;
        uint32_t acc = kEmitConditionNoTarget;
        uint32_t next = scratch;
        cell = table_base + (operand << 2u);
        value = LoadU32(ctx, cell);
        operand = (value >> 14u) - kEmitConditionMaxJumpOffset;
        next += operand + 1u;
        acc = (operand == kEmitConditionNoTarget) ? acc : next;
        operand = acc;
        if (operand == kEmitConditionNoTarget) {
          break;
        }
      } while (true);
      table_base = cell;
      operand = position + (scratch ^ kEmitConditionNoTarget);
      scratch = static_cast<uint32_t>(static_cast<int32_t>(operand) >> 31);
      position = operand + scratch;
      scratch = static_cast<uint32_t>(static_cast<int32_t>(scratch ^ position) >=
                                      static_cast<int32_t>(kEmitConditionJumpRangeLimit));
    }
    // Only the running pass derives this flag; a resumed pass keeps the frame's copy.
    if (ResumePhase(ctx) == kResumeRunning) {
      target_out_of_range = scratch;
    }
    if (target_out_of_range || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        func_state = LoadU32(ctx, func_state + 12u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kTargetRangeCall) : 1u) {
        RaiseSyntaxError(ctx, func_state, kEmitConditionMsgControlStructureTooLong);
        resume_code = kTargetRangeCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        value = LoadU32(ctx, cell);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, table_base,
               ((value & kEmitConditionJumpFieldMask) | (operand << 14u)) +
                   kEmitConditionJumpBias);
      goto clear_patch;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, func_state + 32u, position);
  }
  // Merge point: the cell that named a jump no longer names one.
  clear_patch:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, expr + 20u, kEmitConditionNoTarget);
  }
  goto exit_path;
suspend_point:;
  {
    // Suspend: push the code naming the call in flight, step past it, then push
    // the 44-byte frame a rewinding pass reads back. The stack pointer lives in
    // the word the frame pointer itself names, so every push is a
    // read-modify-write of that word.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, func_state);
    StoreU32(ctx, frame + 4u, operand);
    StoreU32(ctx, frame + 8u, expr);
    StoreU32(ctx, frame + 12u, scratch);
    StoreU32(ctx, frame + 16u, value);
    StoreU32(ctx, frame + 20u, position);
    StoreU32(ctx, frame + 24u, cell);
    StoreU32(ctx, frame + 28u, table_base);
    StoreU32(ctx, frame + 32u, over_complex);
    StoreU32(ctx, frame + 36u, jump_out_of_range);
    StoreU32(ctx, frame + 40u, target_out_of_range);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  }
exit_path:;
  LeaveFrame();
}
}  // namespace artc
