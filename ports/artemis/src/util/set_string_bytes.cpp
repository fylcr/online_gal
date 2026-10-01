#include "util/engine_context.h"

namespace artc {

// Forward declarations -- the copy helper and the growing replace worker.
void CopyStringBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void GrowStringAndReplace(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7);

namespace {

// The widest length a string header can hold, plus the frame widths.
constexpr uint32_t kSetStringBytesMaxLength = 0x7FFFFFFFu;
constexpr uint32_t kSetStringBytesSaveFrameBytes = 32u;
constexpr uint32_t kSetStringBytesResumeWordBytes = 4u;

}  // namespace

// Replaces the contents of a string with the given bytes, copying in place
// while the current block is long enough and reallocating through the growing
// worker when it is not.  Returns the string header.
uint32_t SetStringBytes(EngineContext* ctx, uint32_t text, uint32_t bytes, uint32_t length) {
  uint32_t resume_code = 0u;   // which worker call a rewinding pass replays
  uint32_t text_ptr = 0u;      // the text header, then its data pointer
  uint32_t header = 0u;        // header handed to the growing worker
  uint32_t overflow = 0u;      // capacity, then the bytes past the capacity
  uint32_t capacity = 0u;      // how much the current block holds
  uint32_t stack_frame = 0u;   // scratch on the guest stack for one byte
  uint32_t fits = 0u;          // new length still fits the current block
  uint32_t data_ptr = 0u;      // where the old contents start
  uint32_t frame_base = 0u;    // where a suspended call left its frame
  uint32_t suspend_code = 0u;  // resume word pushed when a call suspends
  uint32_t result = 0u;
  uint32_t tmp = 0u;
  uint32_t tmp2 = 0u;
  uint32_t tmp3 = 0u;
  EnterFrame();
  // Rewinding: pop this step's saved frame and restore the live words.
  if (ResumePhase(ctx) == kResumeRewinding) {
    tmp = ReturnFrameSp(ctx);
    tmp2 = ReturnFrameSp(ctx);
    tmp2 = LoadU32(ctx, tmp2);
    tmp3 = kSetStringBytesSaveFrameBytes;
    tmp2 -= tmp3;
    StoreU32(ctx, tmp, tmp2);
    frame_base = ReturnFrameSp(ctx);
    frame_base = LoadU32(ctx, frame_base);
    text = LoadU32(ctx, frame_base);
    bytes = LoadU32(ctx, frame_base + 4u);
    length = LoadU32(ctx, frame_base + 8u);
    text_ptr = LoadU32(ctx, frame_base + 12u);
    header = LoadU32(ctx, frame_base + 16u);
    overflow = LoadU32(ctx, frame_base + 20u);
    stack_frame = LoadU32(ctx, frame_base + 24u);
    capacity = LoadU32(ctx, frame_base + 28u);
  }
  // Rewinding: drop the pushed resume word and reload the resume code.
  if (ResumePhase(ctx) == kResumeRewinding) {
    tmp = ReturnFrameSp(ctx);
    tmp2 = ReturnFrameSp(ctx);
    tmp2 = LoadU32(ctx, tmp2);
    tmp3 = kSetStringBytesResumeWordBytes;
    tmp2 -= tmp3;
    StoreU32(ctx, tmp, tmp2);
    tmp = ReturnFrameSp(ctx);
    tmp = LoadU32(ctx, tmp);
    tmp = LoadU32(ctx, tmp);
    resume_code = tmp;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // One scratch byte for the terminator the copy leaves at the end.
    stack_frame = GuestStackSp(ctx) - 16u;
    GuestStackSp(ctx) = stack_frame;
    fits = length;
    text_ptr = text;
    tmp = LoadU8(ctx, text_ptr + 11u) >> (7u & 31u);
    if (tmp != 0u) {
      tmp = LoadU32(ctx, text_ptr + 8u) & kSetStringBytesMaxLength;
      tmp -= 1u;
    } else {
      tmp = 10u;
    }
    capacity = tmp;
    fits = (fits <= capacity);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (fits != 0u) {
      // The block already holds enough: copy straight over it.
      data_ptr = text_ptr;
      tmp = LoadU8(ctx, text_ptr + 11u) >> (7u & 31u);
      if (tmp != 0u) {
        data_ptr = LoadU32(ctx, text_ptr);
      }
      CopyStringBytes(ctx, data_ptr, bytes, length);
      tmp = 0u;
      StoreU8(ctx, stack_frame + 15u, tmp);
      tmp = length + data_ptr;
      tmp2 = LoadU8(ctx, stack_frame + 15u);
      StoreU8(ctx, tmp, tmp2);
      tmp = LoadU8(ctx, text_ptr + 11u) >> (7u & 31u);
      if (tmp != 0u) {
        tmp2 = length;
        StoreU32(ctx, text + 4u, tmp2);
      } else {
        tmp2 = length;
        StoreU8(ctx, text + 11u, tmp2);
      }
      goto string_stored;
    }
    header = text_ptr;
    // The growing worker is handed the current capacity and the bytes past it,
    // so the overflow is measured against the capacity just read, not against
    // the zero the slot starts out with.
    overflow = length - capacity;
    tmp = LoadU8(ctx, text_ptr + 11u) >> (7u & 31u);
    if (tmp != 0u) {
      text_ptr = LoadU32(ctx, text + 4u);
    } else {
      text_ptr = LoadU8(ctx, text + 11u);
    }
  }
  if ((resume_code == 0u) | (ResumePhase(ctx) == kResumeRunning)) {
    GrowStringAndReplace(ctx, header, capacity, overflow, text_ptr, 0u, text_ptr, length, bytes);
    suspend_code = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto save_suspend_frame;
    }
  }
string_stored:;
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = stack_frame + 16u;
    result = text;
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
  tmp3 = kSetStringBytesResumeWordBytes;
  tmp2 += tmp3;
  StoreU32(ctx, tmp, tmp2);
  tmp = ReturnFrameSp(ctx);
  tmp = LoadU32(ctx, tmp);
  frame_base = tmp;
  tmp2 = text;
  StoreU32(ctx, frame_base, tmp2);
  StoreU32(ctx, frame_base + 4u, bytes);
  StoreU32(ctx, frame_base + 8u, length);
  StoreU32(ctx, frame_base + 12u, text_ptr);
  StoreU32(ctx, frame_base + 16u, header);
  StoreU32(ctx, frame_base + 20u, overflow);
  StoreU32(ctx, frame_base + 24u, stack_frame);
  StoreU32(ctx, frame_base + 28u, capacity);
  tmp = ReturnFrameSp(ctx);
  tmp2 = ReturnFrameSp(ctx);
  tmp2 = LoadU32(ctx, tmp2);
  tmp3 = kSetStringBytesSaveFrameBytes;
  tmp2 += tmp3;
  StoreU32(ctx, tmp, tmp2);
  result = 0u;
frame_stored:;
  LeaveFrame();
  return result;
}
}  // namespace artc
