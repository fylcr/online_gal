#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step drives.
uint32_t EmitInstruction(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void DischargeExpression(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t AddFunctionConstant(EngineContext* ctx, uint32_t func_state, uint32_t key, uint32_t value);

namespace {

constexpr uint32_t kExpressionToOperandSlotMask = 0x7FFFFFu;         // low 23 bits of a slot word
constexpr uint32_t kExpressionToOperandKeptFlag = 0x800000u;         // bit 23: slot keeps its value
constexpr uint32_t kExpressionToOperandOperandClear = 0xFFFFC03Fu;   // clears the 8-bit operand field
constexpr uint32_t kExpressionToOperandFrameBytes = 24u;             // saved frame: six words
constexpr uint32_t kExpressionToOperandResumeWordBytes = 4u;         // resume word written before it

}  // namespace

// Folds one expression node into the operand the emitter will fold into the
// instruction being built: an already free constant-pool slot is patched in
// place, otherwise a fresh instruction word is handed to EmitInstruction.
void ExpressionToOperand(EngineContext* ctx, uint32_t func_state, uint32_t expr, uint32_t operand) {
  EnterFrame();
  uint32_t instruction = 0u;
  uint32_t slot = 0u;
  uint32_t value = 0u;
  uint32_t resume_point = 0u;
  uint32_t scratch_frame = 0u;
  double constant = 0;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the frame this step saved when it suspended.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kExpressionToOperandFrameBytes);
    value = LoadU32(ctx, ReturnFrameSp(ctx));
    func_state = LoadU32(ctx, value);
    operand = LoadU32(ctx, value + 8u);
    instruction = LoadU32(ctx, value + 12u);
    slot = LoadU32(ctx, value + 16u);
    scratch_frame = LoadU32(ctx, value + 20u);
    expr = LoadU32(ctx, value + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word naming the call this step stopped at.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kExpressionToOperandResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Claim the 16-byte scratch area this step needs on the guest stack.
    instruction = GuestStackSp(ctx);
    scratch_frame = instruction - 16u;
    GuestStackSp(ctx) = scratch_frame;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    DischargeExpression(ctx, func_state, expr);
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The node kind drives the encoding; it is 1-based, so kind 0 means the
    // operand can be folded into the constant slot the pool already holds.
    instruction = LoadU32(ctx, expr);
    slot = instruction - 1u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    switch (slot) {
      case 0: goto reuse_free_slot;
      case 1: goto literal_operand;
      case 2: goto literal_operand;
      case 3: goto shifted_operand;
      case 4: goto constant_operand;
      case 5: goto release_scratch;
      case 6: goto release_scratch;
      case 7: goto release_scratch;
      case 8: goto release_scratch;
      case 9: goto release_scratch;
      case 10: goto patch_operand_field;
      case 11: goto keyed_operand;
      default: goto release_scratch;
    }
  reuse_free_slot:
    instruction = LoadU32(ctx, func_state + 24u);
    if ((int32_t)instruction <= (int32_t)LoadU32(ctx, func_state + 28u)) goto fallback_operand;
    if (instruction == 0u) {
      if ((int32_t)LoadU8(ctx, func_state + 50u) <= (int32_t)operand) goto store_operand;
      goto fallback_operand;
    }
    slot = LoadU32(ctx, LoadU32(ctx, func_state) + 12u) + (instruction << 2u) - 4u;
    instruction = LoadU32(ctx, slot);
    if ((instruction & 63u) != 3u) goto fallback_operand;
    if ((uint32_t)((int32_t)((instruction >> 6u) & 255u) > (int32_t)operand)) goto fallback_operand;
    value = instruction >> 23u;
    if ((uint32_t)((int32_t)(value + 1u) < (int32_t)operand)) goto fallback_operand;
    if ((uint32_t)((int32_t)operand <= (int32_t)value)) goto store_operand;
    StoreU32(ctx, slot, (instruction & kExpressionToOperandSlotMask) | (operand << 23u));
    goto store_operand;
  literal_operand:
    instruction = (operand << 6u) | (instruction == 2u ? kExpressionToOperandKeptFlag : 2u) | 2u;
    slot = LoadU32(ctx, LoadU32(ctx, func_state + 12u) + 8u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    value = EmitInstruction(ctx, func_state, instruction, slot);
    resume_point = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    func_state = value;
  }
  if (ResumePhase(ctx) == kResumeRunning) goto store_operand;
  shifted_operand:
  if (ResumePhase(ctx) == kResumeRunning) {
    instruction = (operand << 6u) | (LoadU32(ctx, expr + 8u) << 14u) | 1u;
    slot = LoadU32(ctx, LoadU32(ctx, func_state + 12u) + 8u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
    value = EmitInstruction(ctx, func_state, instruction, slot);
    resume_point = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    func_state = value;
  }
  if (ResumePhase(ctx) == kResumeRunning) goto store_operand;
  constant_operand:
  if (ResumePhase(ctx) == kResumeRunning) {
    // The float payload is staged in the scratch area for AddFunctionConstant.
    constant = LoadF64(ctx, expr + 8u);
    StoreU32(ctx, scratch_frame + 8u, 3u);
    StoreF64(ctx, scratch_frame, constant);
    instruction = operand << 6u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
    value = AddFunctionConstant(ctx, func_state, scratch_frame, scratch_frame);
    resume_point = 3u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    slot = value;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    instruction = (slot << 14u) | instruction | 1u;
    slot = LoadU32(ctx, LoadU32(ctx, func_state + 12u) + 8u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
    value = EmitInstruction(ctx, func_state, instruction, slot);
    resume_point = 4u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    func_state = value;
  }
  if (ResumePhase(ctx) == kResumeRunning) goto store_operand;
  patch_operand_field:
  if (ResumePhase(ctx) == kResumeRunning) {
    func_state = LoadU32(ctx, LoadU32(ctx, func_state) + 12u) + (LoadU32(ctx, expr + 8u) << 2u);
    instruction = (LoadU32(ctx, func_state) & kExpressionToOperandOperandClear) |
                  ((operand << 6u) & 16320u);
    StoreU32(ctx, func_state, instruction);
    goto store_operand;
  }
  keyed_operand:
  if (ResumePhase(ctx) == kResumeRunning) {
    instruction = LoadU32(ctx, expr + 8u);
    if (instruction == operand) goto store_operand;
    instruction = (instruction << 23u) | (operand << 6u);
    slot = LoadU32(ctx, LoadU32(ctx, func_state + 12u) + 8u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
    value = EmitInstruction(ctx, func_state, instruction, slot);
    resume_point = 5u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    func_state = value;
  }
  if (ResumePhase(ctx) == kResumeRunning) goto store_operand;
  fallback_operand:
  if (ResumePhase(ctx) == kResumeRunning) {
    instruction = (operand << 23u) | (operand << 6u) | 3u;
    slot = LoadU32(ctx, LoadU32(ctx, func_state + 12u) + 8u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
    EmitInstruction(ctx, func_state, instruction, slot);
    resume_point = 6u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  store_operand:
  if (ResumePhase(ctx) == kResumeRunning) {
    // The node now carries the operand the emitter reads back.
    StoreU32(ctx, expr, 12u);
    StoreU32(ctx, expr + 8u, operand);
  }
  release_scratch:
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = scratch_frame + 16u;
  }
  goto step_done;
suspend_point:
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kExpressionToOperandResumeWordBytes);
  value = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, value, func_state);
  StoreU32(ctx, value + 4u, expr);
  StoreU32(ctx, value + 8u, operand);
  StoreU32(ctx, value + 12u, instruction);
  StoreU32(ctx, value + 16u, slot);
  StoreU32(ctx, value + 20u, scratch_frame);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kExpressionToOperandFrameBytes);
step_done:
  LeaveFrame();
}

}  // namespace artc
