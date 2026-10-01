#include "util/engine_context.h"

namespace artc {

// Reallocates a node's array part to a new size.
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size);

namespace {

// The shared empty array descriptor that a node's array field points at while
// the node holds no elements of its own.
constexpr uint32_t kReshapePowerOfTwoNodeEmptyArray = 0x2BBD0u;
constexpr uint32_t kReshapePowerOfTwoNodeSaveFrameBytes = 16u;
constexpr uint32_t kReshapePowerOfTwoNodeResumeWordBytes = 4u;

}  // namespace

// Reshapes the array part of one node so that its capacity stays a power of
// two.  A detached array is reallocated to the shifted element count first,
// then the node's own array to its compacted size; the resume code that a
// rewinding pass reads names which of the two reallocations to replay.
void ReshapePowerOfTwoNode(EngineContext* ctx, uint32_t state, uint32_t node) {
  uint32_t block = 0u;         // the array block currently linked to the node
  uint32_t old_size = 0u;      // the element count the block is reallocated from
  uint32_t fits_flag = 0u;     // the array field is not the shared empty array
  uint32_t resume_code = 0u;   // which reallocation a rewinding pass replays
  uint32_t block_result = 0u;  // what the first reallocation returned
  uint32_t suspend_code = 0u;  // resume word pushed when a call suspends
  uint32_t frame_sp = 0u;
  uint32_t frame_base = 0u;
  uint32_t word_a = 0u;
  uint32_t word_b = 0u;
  EnterFrame();
  // Rewinding: pop this step's saved frame and restore the live words.
  if (ResumePhase(ctx) == kResumeRewinding) {
    uint32_t arg_sp = ReturnFrameSp(ctx);
    uint32_t saved_sp = ReturnFrameSp(ctx);
    uint32_t saved = LoadU32(ctx, saved_sp);
    StoreU32(ctx, arg_sp, saved - kReshapePowerOfTwoNodeSaveFrameBytes);
    uint32_t live_sp = ReturnFrameSp(ctx);
    uint32_t frame = LoadU32(ctx, live_sp);
    state = LoadU32(ctx, frame);
    node = LoadU32(ctx, frame + 4u);
    old_size = LoadU32(ctx, frame + 8u);
    block = LoadU32(ctx, frame + 12u);
  }
  // Rewinding: drop the pushed resume word and reload the resume code.
  if (ResumePhase(ctx) == kResumeRewinding) {
    uint32_t arg_sp = ReturnFrameSp(ctx);
    uint32_t word_sp = ReturnFrameSp(ctx);
    uint32_t word = LoadU32(ctx, word_sp);
    StoreU32(ctx, arg_sp, word - kReshapePowerOfTwoNodeResumeWordBytes);
    uint32_t code_sp = ReturnFrameSp(ctx);
    uint32_t code_word = LoadU32(ctx, code_sp);
    resume_code = LoadU32(ctx, code_word);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    block = LoadU32(ctx, node + 16u);
    fits_flag = block != kReshapePowerOfTwoNodeEmptyArray ? 1u : 0u;
  }
  if ((fits_flag != 0u) | (ResumePhase(ctx) == kResumeRewinding)) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The element count the detached array was allocated for.
      old_size = 32u << (LoadU8(ctx, node + 7u) & 31u);
    }
    if ((ResumePhase(ctx) == kResumeRunning) | (resume_code == 0u)) {
      block_result = ReallocateBlock(ctx, state, block, old_size, 0u);
      suspend_code = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto save_suspend_frame;
      }
      block = block_result;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    old_size = LoadU32(ctx, node + 28u) << (4u & 31u);
    block = LoadU32(ctx, node + 12u);
  }
  if ((ResumePhase(ctx) == kResumeRunning) | (resume_code == 1u)) {
    ReallocateBlock(ctx, state, block, old_size, 0u);
    suspend_code = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto save_suspend_frame;
    }
  }
  if ((ResumePhase(ctx) == kResumeRunning) | (resume_code == 2u)) {
    ReallocateBlock(ctx, state, node, 32u, 0u);
    suspend_code = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto save_suspend_frame;
    }
  }
  goto frame_stored;
save_suspend_frame:;
  // A call suspended: hand the live words to the frame above the stack
  // pointer, push the resume word naming the call, then leave.
  frame_sp = ReturnFrameSp(ctx);
  frame_base = LoadU32(ctx, frame_sp);
  StoreU32(ctx, frame_base, suspend_code);
  frame_sp = ReturnFrameSp(ctx);
  word_a = ReturnFrameSp(ctx);
  word_b = LoadU32(ctx, word_a);
  word_b += kReshapePowerOfTwoNodeResumeWordBytes;
  StoreU32(ctx, frame_sp, word_b);
  frame_sp = ReturnFrameSp(ctx);
  frame_base = LoadU32(ctx, frame_sp);
  StoreU32(ctx, frame_base, state);
  StoreU32(ctx, frame_base + 4u, node);
  StoreU32(ctx, frame_base + 8u, old_size);
  StoreU32(ctx, frame_base + 12u, block);
  frame_sp = ReturnFrameSp(ctx);
  word_a = ReturnFrameSp(ctx);
  word_b = LoadU32(ctx, word_a);
  word_b += kReshapePowerOfTwoNodeSaveFrameBytes;
  StoreU32(ctx, frame_sp, word_b);
frame_stored:;
  LeaveFrame();
}
}  // namespace artc
