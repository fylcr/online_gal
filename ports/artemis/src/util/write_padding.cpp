#include "util/engine_context.h"

namespace artc {

// Callees used below: the buffer filler and the callback runner.
uint32_t FillBytes(EngineContext* ctx, uint32_t dst, uint32_t value, uint32_t length);
void InvokeCallbackWhenFlagUnset(EngineContext* ctx, uint32_t state, uint32_t buffer, uint32_t bytes);

namespace {

constexpr uint32_t kWritePaddingSaveFrameBytes = 12u;
constexpr uint32_t kWritePaddingResumeWordBytes = 4u;

}  // namespace

// Pads a formatted field out to its requested width: the padding character is
// copied into a 256-byte stack buffer that is flushed in whole blocks, one host
// call per block.  Nothing is written when the text already reaches the width,
// or when the left-adjust or zero-pad flags are set.
void WritePadding(EngineContext* ctx, uint32_t stream, uint32_t pad_char, uint32_t width, uint32_t used, uint32_t flags) {
  uint32_t buffer = 0u;       // 256-byte block buffer carved off the guest stack
  uint32_t resume_code = 0u;  // which call a rewinding pass has to replay
  uint32_t width_fits = 0u;   // the text already reaches the requested width
  uint32_t more_blocks = 0u;  // at least one more whole block is still owed
  uint32_t early_out = 0u;    // no padding is needed at all
  uint32_t suspend_code = 0u; // resume word pushed when a call suspends
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
    StoreU32(ctx, arg_sp, saved - kWritePaddingSaveFrameBytes);
    uint32_t live_sp = ReturnFrameSp(ctx);
    uint32_t frame = LoadU32(ctx, live_sp);
    stream = LoadU32(ctx, frame);
    width = LoadU32(ctx, frame + 4u);
    buffer = LoadU32(ctx, frame + 8u);
  }
  // Rewinding: drop the pushed resume word and reload the resume code.
  if (ResumePhase(ctx) == kResumeRewinding) {
    uint32_t arg_sp = ReturnFrameSp(ctx);
    uint32_t word_sp = ReturnFrameSp(ctx);
    uint32_t word = LoadU32(ctx, word_sp);
    StoreU32(ctx, arg_sp, word - kWritePaddingResumeWordBytes);
    uint32_t code_sp = ReturnFrameSp(ctx);
    uint32_t code_word = LoadU32(ctx, code_sp);
    resume_code = LoadU32(ctx, code_word);
  }
  // First pass: take the buffer off the guest stack and decide whether any
  // padding is needed.  A negative width is treated as huge, as usual.
  if (ResumePhase(ctx) == kResumeRunning) {
    buffer = GuestStackSp(ctx) - 256u;
    GuestStackSp(ctx) = buffer;
    int32_t pad_width = static_cast<int32_t>(width);
    int32_t written = static_cast<int32_t>(used);
    width_fits = pad_width <= written ? 1u : 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (width_fits) {
      early_out = 1u;
    } else if ((flags & 73728u) != 0u) {
      // Left-adjust and zero-pad are handled by the caller.
      early_out = 1u;
    } else {
      uint32_t fill_byte = pad_char & 255u;
      width -= used;
      uint32_t one_block = width < 256u;
      uint32_t block_bytes = one_block ? width : 256u;
      FillBytes(ctx, buffer, fill_byte, block_bytes);
      more_blocks = one_block ? 0u : 1u;
    }
  }
  if (!early_out) {
    // Flush whole blocks until fewer than 256 bytes are left to pad.
    if ((more_blocks != 0u) | (ResumePhase(ctx) == kResumeRewinding)) {
      for (;;) {
        if ((ResumePhase(ctx) == kResumeRunning) | (resume_code == 0u)) {
          InvokeCallbackWhenFlagUnset(ctx, stream, buffer, 256u);
          suspend_code = 0u;
          if (ResumePhase(ctx) == kResumeSuspended) {
            goto save_suspend_frame;
          }
        }
        if (ResumePhase(ctx) == kResumeRunning) {
          width -= 256u;
          if (width > 255u) {
            continue;
          }
        }
        break;
      }
    }
    // Flush the remaining partial block.
    if ((ResumePhase(ctx) == kResumeRunning) | (resume_code == 1u)) {
      InvokeCallbackWhenFlagUnset(ctx, stream, buffer, width);
      suspend_code = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto save_suspend_frame;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = buffer + 256u;
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
  word_b += kWritePaddingResumeWordBytes;
  StoreU32(ctx, frame_sp, word_b);
  frame_sp = ReturnFrameSp(ctx);
  frame_base = LoadU32(ctx, frame_sp);
  StoreU32(ctx, frame_base, stream);
  StoreU32(ctx, frame_base + 4u, width);
  StoreU32(ctx, frame_base + 8u, buffer);
  frame_sp = ReturnFrameSp(ctx);
  word_a = ReturnFrameSp(ctx);
  word_b = LoadU32(ctx, word_a);
  word_b += kWritePaddingSaveFrameBytes;
  StoreU32(ctx, frame_sp, word_b);
frame_stored:;
  LeaveFrame();
}
}  // namespace artc
