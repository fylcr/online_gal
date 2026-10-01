#include "util/engine_context.h"

namespace artc {

// Forward declarations -- the emitter and the syntax-error reporter.
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
uint32_t EmitInstruction(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

// The bias the encoded entry offset is stored relative to, and the largest
// offset that still fits its field.
constexpr uint32_t kResolveBranchOffsetOffsetBias = 0x1FFFFu;
constexpr uint32_t kResolveBranchOffsetOffsetLimit = 0x20000u;
constexpr uint32_t kResolveBranchOffsetTooLongMessage = 0x2A394u;
constexpr uint32_t kResolveBranchOffsetPatchOpcode = 0x7FFF8016u;
constexpr uint32_t kResolveBranchOffsetEntryTag = 0x7FFFC000u;
constexpr uint32_t kResolveBranchOffsetNoOffset = 0xFFFFFFFFu;
constexpr uint32_t kResolveBranchOffsetSaveFrameBytes = 36u;
constexpr uint32_t kResolveBranchOffsetResumeWordBytes = 4u;

}  // namespace

// Emits the branch instruction for one control structure, then folds the jump
// offset of the enclosing block into the branch table entry it points at.
// Raises when the offset no longer fits the field the entry stores it in.
uint32_t ResolveBranchOffset(EngineContext* ctx, uint32_t state, uint32_t opcode, uint32_t operands, uint32_t extra, uint32_t target) {
  uint32_t resume_code = 0u;   // which emitter call a rewinding pass replays
  uint32_t saved_offset = 0u;  // offset stashed by the previous resolve
  uint32_t entry_ptr = 0u;     // branch table entry being patched
  uint32_t entry_addr = 0u;    // its address in guest memory
  uint32_t offset_flag = 0u;   // computed jump offset, and the patch gate
  uint32_t frame_base = 0u;    // where a suspended call left its frame
  uint32_t call_result = 0u;   // what a resumable call returned
  uint32_t suspend_code = 0u;  // resume word pushed when a call suspends
  uint32_t result = 0u;
  uint32_t tmp = 0u;
  uint32_t tmp2 = 0u;
  uint32_t tmp3 = 0u;
  uint32_t tmp4 = 0u;
  EnterFrame();
  // Rewinding: pop this step's saved frame and restore the live words.
  if (ResumePhase(ctx) == kResumeRewinding) {
    tmp = ReturnFrameSp(ctx);
    tmp2 = ReturnFrameSp(ctx);
    tmp2 = LoadU32(ctx, tmp2);
    tmp3 = kResolveBranchOffsetSaveFrameBytes;
    tmp2 -= tmp3;
    StoreU32(ctx, tmp, tmp2);
    frame_base = ReturnFrameSp(ctx);
    frame_base = LoadU32(ctx, frame_base);
    state = LoadU32(ctx, frame_base);
    operands = LoadU32(ctx, frame_base + 8u);
    extra = LoadU32(ctx, frame_base + 12u);
    target = LoadU32(ctx, frame_base + 16u);
    saved_offset = LoadU32(ctx, frame_base + 20u);
    entry_ptr = LoadU32(ctx, frame_base + 24u);
    entry_addr = LoadU32(ctx, frame_base + 28u);
    offset_flag = LoadU32(ctx, frame_base + 32u);
    opcode = LoadU32(ctx, frame_base + 4u);
  }
  // Rewinding: drop the pushed resume word and reload the resume code.
  if (ResumePhase(ctx) == kResumeRewinding) {
    tmp = ReturnFrameSp(ctx);
    tmp2 = ReturnFrameSp(ctx);
    tmp2 = LoadU32(ctx, tmp2);
    tmp3 = kResolveBranchOffsetResumeWordBytes;
    tmp2 -= tmp3;
    StoreU32(ctx, tmp, tmp2);
    tmp = ReturnFrameSp(ctx);
    tmp = LoadU32(ctx, tmp);
    tmp = LoadU32(ctx, tmp);
    resume_code = tmp;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Pack the short form of the instruction out of its four operand fields.
    opcode = (operands << (6u & 31u)) | opcode;
    opcode |= (extra << (23u & 31u));
    opcode |= (target << (14u & 31u));
    tmp = LoadU32(ctx, state + 12u);
    operands = LoadU32(ctx, tmp + 8u);
  }
  if ((resume_code == 0u) | (ResumePhase(ctx) == kResumeRunning)) {
    call_result = EmitInstruction(ctx, state, opcode, operands);
    suspend_code = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto save_suspend_frame;
    }
    opcode = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Take the offset the previous resolve stashed and clear the slot.
    saved_offset = LoadU32(ctx, state + 32u);
    tmp3 = kResolveBranchOffsetNoOffset;
    StoreU32(ctx, state + 32u, tmp3);
    tmp = LoadU32(ctx, state + 12u);
    opcode = LoadU32(ctx, tmp + 8u);
  }
  if ((resume_code == 1u) | (ResumePhase(ctx) == kResumeRunning)) {
    call_result = EmitInstruction(ctx, state, kResolveBranchOffsetPatchOpcode, opcode);
    suspend_code = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto save_suspend_frame;
    }
    opcode = call_result;
  }
  // A running pass works out whether this branch carries an offset at all; a
  // rewinding pass keeps the packed operands it restored and carries on.
  operands = (ResumePhase(ctx) != 0u) ? operands : (saved_offset != kResolveBranchOffsetNoOffset);
  if ((operands != 0u) | (ResumePhase(ctx) == kResumeRewinding)) {
    if (ResumePhase(ctx) == kResumeRunning) {
      if (opcode == kResolveBranchOffsetNoOffset) {
        result = saved_offset;
        goto frame_stored;
      }
      tmp = LoadU32(ctx, state);
      entry_ptr = LoadU32(ctx, tmp + 12u);
      target = opcode;
      // Walk the branch table chain the target names until it runs out.
      for (;;) {
        tmp4 = target;
        operands = tmp4;
        entry_addr = entry_ptr + (tmp4 << (2u & 31u));
        extra = LoadU32(ctx, entry_addr);
        target = (extra >> (14u & 31u)) - kResolveBranchOffsetOffsetBias;
        tmp = kResolveBranchOffsetNoOffset;
        tmp2 = operands + target + 1u;
        tmp3 = (target == kResolveBranchOffsetNoOffset);
        target = tmp3 ? tmp : tmp2;
        if (target != kResolveBranchOffsetNoOffset) {
          continue;
        }
        break;
      }
      entry_ptr = entry_addr;
      // Fold the distance to the branch into the entry's offset field.
      operands = saved_offset + (operands ^ kResolveBranchOffsetNoOffset);
      target = (uint32_t)((int32_t)operands >> (31u & 31u));
      tmp = operands + target;
      target = target ^ tmp;
      target = (uint32_t)((int32_t)target >= (int32_t)kResolveBranchOffsetOffsetLimit);
    }
    offset_flag = (ResumePhase(ctx) != 0u) ? offset_flag : target;
    if ((offset_flag != 0u) | (ResumePhase(ctx) == kResumeRewinding)) {
      if (ResumePhase(ctx) == kResumeRunning) {
        state = LoadU32(ctx, state + 12u);
      }
      if ((resume_code == 2u) | (ResumePhase(ctx) == kResumeRunning)) {
        RaiseSyntaxError(ctx, state, kResolveBranchOffsetTooLongMessage);
        suspend_code = 2u;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto save_suspend_frame;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        extra = LoadU32(ctx, entry_addr);
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        tmp = extra & 16383u;
        tmp |= operands << (14u & 31u);
        tmp2 = kResolveBranchOffsetEntryTag;
        tmp += tmp2;
        StoreU32(ctx, entry_ptr, tmp);
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    result = opcode;
    goto frame_stored;
  }
  TrapUnreachable();
save_suspend_frame:;
  // A call suspended: store the resume word, push it, then hand the live
  // words to the frame above the stack pointer.
  tmp = ReturnFrameSp(ctx);
  tmp = LoadU32(ctx, tmp);
  StoreU32(ctx, tmp, suspend_code);
  tmp = ReturnFrameSp(ctx);
  tmp2 = ReturnFrameSp(ctx);
  tmp2 = LoadU32(ctx, tmp2);
  tmp3 = kResolveBranchOffsetResumeWordBytes;
  tmp2 += tmp3;
  StoreU32(ctx, tmp, tmp2);
  tmp = ReturnFrameSp(ctx);
  tmp = LoadU32(ctx, tmp);
  frame_base = tmp;
  tmp2 = state;
  StoreU32(ctx, frame_base, tmp2);
  StoreU32(ctx, frame_base + 4u, opcode);
  StoreU32(ctx, frame_base + 8u, operands);
  StoreU32(ctx, frame_base + 12u, extra);
  StoreU32(ctx, frame_base + 16u, target);
  StoreU32(ctx, frame_base + 20u, saved_offset);
  StoreU32(ctx, frame_base + 24u, entry_ptr);
  StoreU32(ctx, frame_base + 28u, entry_addr);
  StoreU32(ctx, frame_base + 32u, offset_flag);
  tmp = ReturnFrameSp(ctx);
  tmp2 = ReturnFrameSp(ctx);
  tmp2 = LoadU32(ctx, tmp2);
  tmp3 = kResolveBranchOffsetSaveFrameBytes;
  tmp2 += tmp3;
  StoreU32(ctx, tmp, tmp2);
  result = 0u;
frame_stored:;
  LeaveFrame();
  return result;
}
}  // namespace artc
