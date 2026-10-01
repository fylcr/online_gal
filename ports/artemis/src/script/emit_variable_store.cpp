#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step drives.
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
void DischargeExpression(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t ResolveBranchOffset(EngineContext* ctx, uint32_t state, uint32_t opcode,
                             uint32_t operands, uint32_t extra, uint32_t target);
void ExpressionToOperand(EngineContext* ctx, uint32_t func_state, uint32_t expr, uint32_t operand);

namespace {

constexpr uint32_t kDataVariableStoreOpcodes = 0x2B460u;            // opcode flag table in guest memory
constexpr uint32_t kEmitVariableStoreComplexMessage = 0x2A371u;    // "function or expression too complex"
constexpr uint32_t kEmitVariableStoreTooLongMessage = 0x2A394u;    // "control structure too long"
constexpr uint32_t kEmitVariableStoreIndexBias = 0x1FFFFu;         // bias off a chained slot index
constexpr uint32_t kEmitVariableStoreIndexLimit = 0x20000u;        // widest chained slot index
constexpr uint32_t kEmitVariableStoreOpcodeBase = 0x7FFFC000u;     // base of an encoded opcode word
constexpr uint32_t kEmitVariableStoreOperandClear = 0xFFFFC03Fu;   // clears the operand field
constexpr uint32_t kEmitVariableStoreNone = 0xFFFFFFFFu;           // "no value" marker
constexpr uint32_t kEmitVariableStoreFrameBytes = 44u;             // saved frame: eleven words
constexpr uint32_t kEmitVariableStoreResumeWordBytes = 4u;         // resume word written before it

}  // namespace

// Emits the store for one variable kind, from a plain local slot up to an
// upvalue cell: the slot form of the store decides whether the slot is
// rewritten in place or a new store word is emitted for the pool.
void EmitVariableStore(EngineContext* ctx, uint32_t state, uint32_t variable) {
  EnterFrame();
  uint32_t slot = 0u;
  uint32_t word = 0u;
  uint32_t frame = 0u;
  uint32_t call_result = 0u;
  uint32_t target = 0u;
  uint32_t count = 0u;
  uint32_t pool = 0u;
  uint32_t entry = 0u;
  uint32_t resume_point = 0u;
  uint32_t too_many_slots = 0u;
  uint32_t patch_start = 0u;
  uint32_t patch_end = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the frame this step saved when it suspended.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kEmitVariableStoreFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    slot = LoadU32(ctx, frame + 8u);
    word = LoadU32(ctx, frame + 12u);
    target = LoadU32(ctx, frame + 16u);
    count = LoadU32(ctx, frame + 20u);
    pool = LoadU32(ctx, frame + 24u);
    entry = LoadU32(ctx, frame + 28u);
    too_many_slots = LoadU32(ctx, frame + 32u);
    patch_start = LoadU32(ctx, frame + 36u);
    patch_end = LoadU32(ctx, frame + 40u);
    variable = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word naming the call this step stopped at.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kEmitVariableStoreResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    DischargeExpression(ctx, state, variable);
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The store kind, biased by two, picks the slot form to rewrite.
    slot = LoadU32(ctx, variable) - 2u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    switch (slot) {
      case 0: goto common_store;
      case 1: goto indexed_store;
      case 2: goto common_store;
      case 3: goto common_store;
      case 4: goto indexed_store;
      case 5: goto indexed_store;
      case 6: goto indexed_store;
      case 7: goto indexed_store;
      case 8: goto local_store;
      case 9: goto upvalue_store;
      case 10: goto global_store;
      default: goto indexed_store;
    }
  local_store:
    // Rewrite the slot naming the local itself, keeping its operand field.
    word = LoadU32(ctx, variable + 8u);
    slot = LoadU32(ctx, LoadU32(ctx, state) + 12u) + (word << 2u);
    if ((uint32_t)((int32_t)word >= (int32_t)1)) {
      // The slot in front of it names the opcode this store gets.
      target = slot - 4u;
      word = LoadU32(ctx, target);
      count = (uint32_t)((int32_t)LoadS8(ctx, (word & 63u) + kDataVariableStoreOpcodes) < (int32_t)0);
      if (count) goto write_slot;
    }
    word = LoadU32(ctx, slot);
    target = slot;
  write_slot:
    word = (word & kEmitVariableStoreOperandClear) | ((word & 16320u) == 0u ? 64u : 0u);
    StoreU32(ctx, target, word);
    target = LoadU32(ctx, variable + 8u);
    goto target_ready;
  upvalue_store:
    // An upvalue store only drops the pool count for its own slot.
    word = LoadU32(ctx, state);
    slot = LoadU32(ctx, LoadU32(ctx, word + 12u) + (LoadU32(ctx, variable + 8u) << 2u));
    target = (uint32_t)((slot & 63u) != 19u);
    if (target) goto check_capacity;
    target = LoadU32(ctx, state + 24u) - 1u;
    StoreU32(ctx, state + 24u, target);
    slot = slot >> 23u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    call_result = ResolveBranchOffset(ctx, state, 26u, slot, 0u, 1u);
    resume_point = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    target = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) goto target_ready;
indexed_store:
  if (ResumePhase(ctx) == kResumeRunning) {
    word = LoadU32(ctx, state);
  }
check_capacity:
  if (ResumePhase(ctx) == kResumeRunning) {
    // One more slot may still fit unless the pool count ran past its end.
    count = LoadU32(ctx, state + 36u);
    target = count + 1u;
    slot = (uint32_t)((int32_t)count < (int32_t)LoadU8(ctx, word + 75u));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (slot) {
      slot = target;
      goto store_slot;
    }
    count = (uint32_t)((int32_t)count >= (int32_t)249);
    slot = target;
  }
  too_many_slots = (ResumePhase(ctx) == kResumeRunning) ? count : too_many_slots;
  if (too_many_slots || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, state + 12u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
      RaiseSyntaxError(ctx, slot, kEmitVariableStoreComplexMessage);
      resume_point = 2u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      word = LoadU32(ctx, state);
      slot = LoadU32(ctx, state + 36u) + 1u;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, word + 75u, target);
  }
store_slot:
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, state + 36u, slot);
    slot = slot - 1u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
    ExpressionToOperand(ctx, state, variable, slot);
    resume_point = 3u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = (uint32_t)(LoadU32(ctx, variable) != 12u);
    if (slot) goto load_operand;
  }
global_store:
  if (ResumePhase(ctx) == kResumeRunning) {
    // A global store keeps its key slot unless the key needs a wide operand.
    word = LoadU32(ctx, variable + 8u);
    slot = word & 256u;
    if (slot) goto load_operand;
    slot = (uint32_t)((int32_t)word < (int32_t)LoadU8(ctx, state + 50u));
    if (slot) goto load_operand;
    slot = LoadU32(ctx, state + 36u) - 1u;
    StoreU32(ctx, state + 36u, slot);
  }
load_operand:
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, variable + 8u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
    call_result = ResolveBranchOffset(ctx, state, 27u, 255u, slot, 0u);
    resume_point = 4u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    target = call_result;
  }
target_ready:
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = (uint32_t)(target == kEmitVariableStoreNone);
    if (slot) goto common_store;
    word = LoadU32(ctx, variable + 20u);
    slot = (uint32_t)(word != kEmitVariableStoreNone);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The recorded jump target is rewritten: walk the slot chain it names.
      pool = LoadU32(ctx, LoadU32(ctx, state) + 12u);
      for (;;) {
        slot = word;
        entry = pool + (slot << 2u);
        count = LoadU32(ctx, entry);
        word = (count >> 14u) - kEmitVariableStoreIndexBias;
        word = (word == kEmitVariableStoreNone) ? kEmitVariableStoreNone : (slot + word + 1u);
        if (word == kEmitVariableStoreNone) break;
      }
      pool = entry;
      word = target + (slot ^ kEmitVariableStoreNone);
      slot = (uint32_t)((int32_t)word >> 31);
      target = word + slot;
      slot = (uint32_t)((int32_t)(slot ^ target) >= (int32_t)kEmitVariableStoreIndexLimit);
    }
    patch_start = (ResumePhase(ctx) != kResumeRunning) ? patch_start : slot;
    if (patch_start || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, state + 12u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
        RaiseSyntaxError(ctx, slot, kEmitVariableStoreTooLongMessage);
        resume_point = 5u;
        if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        count = LoadU32(ctx, entry);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      target = word << 14u;
      slot = (target | (count & 16383u)) + kEmitVariableStoreOpcodeBase;
      StoreU32(ctx, pool, slot);
      goto common_store;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, variable + 20u, target);
  }
common_store:
  if (ResumePhase(ctx) == kResumeRunning) {
    // The store's own target is rewritten the same way, from the break slot.
    target = LoadU32(ctx, variable + 16u);
    StoreU32(ctx, state + 28u, LoadU32(ctx, state + 24u));
    slot = (uint32_t)(target != kEmitVariableStoreNone);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      word = LoadU32(ctx, state + 32u);
      if (word == kEmitVariableStoreNone) goto store_end;
      pool = LoadU32(ctx, LoadU32(ctx, state) + 12u);
      for (;;) {
        slot = word;
        entry = pool + (slot << 2u);
        count = LoadU32(ctx, entry);
        word = (count >> 14u) - kEmitVariableStoreIndexBias;
        word = (word == kEmitVariableStoreNone) ? kEmitVariableStoreNone : (slot + word + 1u);
        if (word == kEmitVariableStoreNone) break;
      }
      pool = entry;
      word = target + (slot ^ kEmitVariableStoreNone);
      slot = (uint32_t)((int32_t)word >> 31);
      target = word + slot;
      slot = (uint32_t)((int32_t)(slot ^ target) >= (int32_t)kEmitVariableStoreIndexLimit);
    }
    patch_end = (ResumePhase(ctx) != kResumeRunning) ? patch_end : slot;
    if (patch_end || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        word = LoadU32(ctx, state + 12u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
        RaiseSyntaxError(ctx, word, kEmitVariableStoreTooLongMessage);
        resume_point = 6u;
        if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        count = LoadU32(ctx, entry);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      word = ((count & 16383u) | (target << 14u)) + kEmitVariableStoreOpcodeBase;
      StoreU32(ctx, pool, word);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, variable + 16u, kEmitVariableStoreNone);
    goto step_done;
  }
store_end:
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, state + 32u, target);
    StoreU32(ctx, variable + 16u, kEmitVariableStoreNone);
  }
  goto step_done;
suspend_point:
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kEmitVariableStoreResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, variable);
  StoreU32(ctx, frame + 8u, slot);
  StoreU32(ctx, frame + 12u, word);
  StoreU32(ctx, frame + 16u, target);
  StoreU32(ctx, frame + 20u, count);
  StoreU32(ctx, frame + 24u, pool);
  StoreU32(ctx, frame + 28u, entry);
  StoreU32(ctx, frame + 32u, too_many_slots);
  StoreU32(ctx, frame + 36u, patch_start);
  StoreU32(ctx, frame + 40u, patch_end);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kEmitVariableStoreFrameBytes);
step_done:
  LeaveFrame();
}

}  // namespace artc
