#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);

namespace {

constexpr uint32_t kAllocateTooComplexMessage = 0x2A371u;  // "function or expression too complex"
constexpr uint32_t kAllocateLowFieldClear = 0xFF803FFFu;  // the operand word without bits 14..22
constexpr uint32_t kAllocateLowFieldMask = 0x7FC000u;  // bits 14..22, the low operand field
constexpr uint32_t kAllocateLowFieldBias = 16384u;  // one register as that field counts it
constexpr uint32_t kAllocateHighFieldKeep = 0x7FFFFFu;  // the operand word below bit 23
constexpr uint32_t kAllocateHighFieldBias = 0x800000u;  // one register as that field counts it
constexpr uint32_t kAllocateSlotClear = 0xFFFFC03Fu;  // the operand word without bits 6..13
constexpr uint32_t kAllocateSlotMask = 16320u;  // bits 6..13, the register slot field
constexpr uint32_t kAllocateRegisterLimit = 249u;  // most registers a frame may hold
constexpr uint32_t kAllocateSaveFrameBytes = 16u;  // saved state words below the resume code
constexpr uint32_t kAllocateResumeWordBytes = 4u;  // resume code slot

}  // namespace

// Gives an expression the register that holds it and records that register on
// the instruction that produced it.  The operand cell carries the register in
// one of two fields, and a frame with no room left calls for more room.
void AllocateExpressionRegister(EngineContext* ctx, uint32_t state, uint32_t instruction, uint32_t expression) {
  EnterFrame();
  uint32_t registers = 0u;   // the register frame the expression lives in
  uint32_t cell = 0u;        // the operand cell the instruction points at
  uint32_t reg_base = 0u;    // where the frame's register slots begin
  uint32_t reg_index = 0u;   // the register the expression was given
  uint32_t at_limit = 0u;    // the frame cannot hold another register
  uint32_t resume_point = 0u;
  uint32_t merged = 0u;      // the operand word once the register is in it
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 16-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kAllocateSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    instruction = LoadU32(ctx, frame + 4u);
    expression = LoadU32(ctx, frame + 8u);
    registers = LoadU32(ctx, frame + 12u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame that saves the resume code.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kAllocateResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    switch (LoadU32(ctx, instruction) - 13u) {
      case 0u: goto patch_low_field;
      case 1u: goto patch_high_field;
      default: goto skip_patch;
    }
  patch_low_field:
    // Opcode 13 keeps its register in bits 14..22.
    cell = LoadU32(ctx, LoadU32(ctx, state) + 12u) + (LoadU32(ctx, instruction + 8u) << 2u);
    merged = (LoadU32(ctx, cell) & kAllocateLowFieldClear)
        | (((expression << 14u) + kAllocateLowFieldBias) & kAllocateLowFieldMask);
    StoreU32(ctx, cell, merged);
    goto exit_path;
  patch_high_field:
    // Opcode 14 keeps its register in bits 23..31, and the operand cell it
    // points at carries the register slot in bits 6..13 as well.
    registers = LoadU32(ctx, state);
    reg_base = LoadU32(ctx, registers + 12u);
    cell = reg_base + (LoadU32(ctx, instruction + 8u) << 2u);
    merged = ((LoadU32(ctx, cell) & kAllocateHighFieldKeep) | (expression << 23u))
        + kAllocateHighFieldBias;
    StoreU32(ctx, cell, merged);
    reg_index = LoadU32(ctx, state + 36u);
    cell = reg_base + (LoadU32(ctx, instruction + 8u) << 2u);
    merged = ((reg_index << 6u) & kAllocateSlotMask) | (LoadU32(ctx, cell) & kAllocateSlotClear);
    StoreU32(ctx, cell, merged);
    reg_index = LoadU32(ctx, state + 36u);
    instruction = reg_index + 1u;
    expression = (int32_t)reg_index < (int32_t)LoadU8(ctx, registers + 75u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (expression != 0u) {
      // The frame has room: the register just taken is what the caller stores.
      expression = instruction;
      goto commit;
    }
    at_limit = (int32_t)reg_index >= (int32_t)kAllocateRegisterLimit;
    expression = instruction;
  }
  if (at_limit != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The expression register is the frame's own free slot.
      expression = LoadU32(ctx, state + 12u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      RaiseSyntaxError(ctx, expression, kAllocateTooComplexMessage);
      resume_point = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // A fresh register: the frame's free slot becomes the result, and the
      // register frame is reloaded for the count below.
      registers = LoadU32(ctx, state);
      expression = LoadU32(ctx, state + 36u) + 1u;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The frame now holds the new register as well.
    StoreU8(ctx, registers + 75u, instruction);
  }
commit:
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, state + 36u, expression);
  }
skip_patch:
  goto exit_path;
suspend_point:
  // Suspend: write the resume code, then push the frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kAllocateResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, state);
    StoreU32(ctx, frame + 4u, instruction);
    StoreU32(ctx, frame + 8u, expression);
    StoreU32(ctx, frame + 12u, registers);
  }
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kAllocateSaveFrameBytes);
exit_path:
  LeaveFrame();
}

}  // namespace artc
