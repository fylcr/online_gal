#include "util/engine_context.h"

namespace artc {

// Cross-product callees, defined by the heap and by the string helpers.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
uint32_t CopyBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t StringLength(EngineContext* ctx, uint32_t text);
void RaiseStringLengthError(EngineContext* ctx, uint32_t a);

namespace {

constexpr uint32_t kMakeStringFromBytesStringSlotAddr = 0xD015Cu;
constexpr uint32_t kMakeStringFromBytesHeapFlag = 0x80000000u;
constexpr uint32_t kMakeStringFromBytesSizeMask = 0xFFFFFFF0u;
constexpr uint32_t kMakeStringFromBytesConstFFFFFFFF = 0xFFFFFFFFu;
constexpr uint32_t kMakeStringFromBytesSaveFrameBytes = 32u;
constexpr uint32_t kMakeStringFromBytesResumeWordBytes = 4u;

}  // namespace

// Builds a string object from a byte range: measures the text, allocates a heap
// buffer when the text is long (short text lives inside the frame), copies the
// bytes in, NUL terminates and finally answers through the host call.
void MakeStringFromBytes(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t frame_sp = 0u;
  uint32_t string_data = 0u;
  uint32_t text_len = 0u;
  uint32_t alloc_size = 0u;
  uint32_t state_ptr = 0u;
  uint32_t resume_code = 0u;
  uint32_t phase = 0u;
  // Set below, once the text length is known.
  uint32_t long_pass = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the frame the suspension saved and read its words back in
    // the order the push wrote them.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t frame_top = ReturnFrameSp(ctx);
    frame_top = LoadU32(ctx, frame_top) - kMakeStringFromBytesSaveFrameBytes;
    StoreU32(ctx, sp, frame_top);
    uint32_t base = ReturnFrameSp(ctx);
    base = LoadU32(ctx, base);
    a = LoadU32(ctx, base);
    c = LoadU32(ctx, base + 8u);
    frame_sp = LoadU32(ctx, base + 12u);
    string_data = LoadU32(ctx, base + 16u);
    text_len = LoadU32(ctx, base + 20u);
    alloc_size = LoadU32(ctx, base + 24u);
    state_ptr = LoadU32(ctx, base + 28u);
    b = LoadU32(ctx, base + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word that names the call that was in flight.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t word_top = ReturnFrameSp(ctx);
    word_top = LoadU32(ctx, word_top) - kMakeStringFromBytesResumeWordBytes;
    StoreU32(ctx, sp, word_top);
    uint32_t word = ReturnFrameSp(ctx);
    word = LoadU32(ctx, word);
    resume_code = LoadU32(ctx, word);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A running pass takes its frame off the guest stack and looks for a pending
    // operation; this slot holds the test result before it becomes the string's
    // data pointer.
    frame_sp = GuestStackSp(ctx) - 32u;
    GuestStackSp(ctx) = frame_sp;
    state_ptr = LoadU32(ctx, a + 20u);
    state_ptr = LoadU32(ctx, state_ptr + 2680u);
    string_data = (state_ptr == 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (string_data != 0u) {
      goto recheck_pending;
    }
    text_len = StringLength(ctx, c);
    if (text_len >= kMakeStringFromBytesSizeMask) {
      goto raise_length;
    }
    string_data = (text_len >= 11u);
  }
  // Long text needs a heap block, and so does any rewinding pass.
  long_pass = string_data | static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (long_pass != 0u) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // Rounded up so the header stays aligned.
      string_data = text_len + 16u;
      alloc_size = string_data & kMakeStringFromBytesSizeMask;
    }
    phase = ResumePhase(ctx);
    if (phase == kResumeRunning || resume_code == 0u) {
      uint32_t buffer = AllocateStringBuffer(ctx, alloc_size);
      resume_code = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_here;
      }
      string_data = buffer;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Long text: the object points at the heap block, with the flag bit set
      // and the exact length alongside.
      uint32_t flagged_size = alloc_size | kMakeStringFromBytesHeapFlag;
      StoreU32(ctx, frame_sp + 8u, flagged_size);
      StoreU32(ctx, frame_sp, string_data);
      StoreU32(ctx, frame_sp + 4u, text_len);
      goto copy_text;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Short text: the length and the bytes stay inside the frame itself.
    StoreU8(ctx, frame_sp + 11u, text_len);
    string_data = frame_sp;
    if (text_len == 0u) {
      goto copy_done;
    }
  }
copy_text:;
  if (ResumePhase(ctx) == kResumeRunning) {
    c = CopyBytes(ctx, string_data, c, text_len);
  }
copy_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, string_data + text_len, 0u);
    StoreU32(ctx, kMakeStringFromBytesStringSlotAddr, 0u);
    c = frame_sp + 16u;
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_code == 1u) {
    HostOsSchedule(ctx, 2328u, c, frame_sp);
    resume_code = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    c = LoadU32(ctx, kMakeStringFromBytesStringSlotAddr);
    StoreU32(ctx, kMakeStringFromBytesStringSlotAddr, 0u);
    c = (c == 1u);
    if (c != 0u) {
      goto host_result;
    }
    c = frame_sp + 16u;
    // The frame's own sign byte, compared as signed, says which block is freed.
    if (static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame_sp + 27u)) >
                              static_cast<int32_t>(kMakeStringFromBytesConstFFFFFFFF))) {
      goto recheck_pending;
    }
    c = LoadU32(ctx, frame_sp + 16u);
  }
recheck_pending:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, kMakeStringFromBytesStringSlotAddr, 0u);
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_code == 2u) {
    HostFailAssertion(ctx, 3373u, a, b, c, 0u);
    resume_code = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The scratch frame this pass took off the guest stack, freed on the way out.
    uint32_t frame_block = 0u;
    a = LoadU32(ctx, kMakeStringFromBytesStringSlotAddr);
    StoreU32(ctx, kMakeStringFromBytesStringSlotAddr, 0u);
    if (a == 1u) {
      goto host_result_again;
    }
    if (state_ptr == 0u) {
      goto frame_cleanup;
    }
    if (static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame_sp + 27u)) <=
                              static_cast<int32_t>(kMakeStringFromBytesConstFFFFFFFF))) {
      uint32_t payload = LoadU32(ctx, frame_sp + 16u);
      DeallocateRawBlock(ctx, payload);
    }
    if (static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame_sp + 11u)) >
                              static_cast<int32_t>(kMakeStringFromBytesConstFFFFFFFF))) {
      goto frame_cleanup;
    }
    frame_block = LoadU32(ctx, frame_sp);
    DeallocateRawBlock(ctx, frame_block);
  frame_cleanup:;
    GuestStackSp(ctx) = frame_sp + 32u;
    LeaveFrame();
    return;
  }
host_result:;
  if (ResumePhase(ctx) == kResumeRunning) {
    c = HostRet0(ctx);
    HostTempRet0(ctx);
    goto release_inline_block;
  }
host_result_again:;
  if (ResumePhase(ctx) == kResumeRunning) {
    c = HostRet0(ctx);
    HostTempRet0(ctx);
    if (state_ptr == 0u) {
      goto release_and_trap;
    }
    if (static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame_sp + 27u)) >
                              static_cast<int32_t>(kMakeStringFromBytesConstFFFFFFFF))) {
      goto release_inline_block;
    }
    uint32_t payload = LoadU32(ctx, frame_sp + 16u);
    DeallocateRawBlock(ctx, payload);
  }
release_inline_block:;
  if (ResumePhase(ctx) == kResumeRunning) {
    if (static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame_sp + 11u)) >
                              static_cast<int32_t>(kMakeStringFromBytesConstFFFFFFFF))) {
      goto release_and_trap;
    }
    uint32_t frame_block = LoadU32(ctx, frame_sp);
    DeallocateRawBlock(ctx, frame_block);
  }
release_and_trap:;
  if (ResumePhase(ctx) == kResumeRunning) {
    HostOsRelease(ctx, c);
    TrapUnreachable();
  }
raise_length:;
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_code == 3u) {
    RaiseStringLengthError(ctx, frame_sp);
    resume_code = 3u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  LeaveFrame();
  return;
suspend_here:;
  // Push the resume word naming the call that was in flight, then the frame the
  // rewind pops; the words go in the order that pop reads them back.
  uint32_t slot = ReturnFrameSp(ctx);
  slot = LoadU32(ctx, slot);
  StoreU32(ctx, slot, resume_code);
  uint32_t sp = ReturnFrameSp(ctx);
  uint32_t word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + kMakeStringFromBytesResumeWordBytes;
  StoreU32(ctx, sp, word_top);
  uint32_t base = ReturnFrameSp(ctx);
  base = LoadU32(ctx, base);
  StoreU32(ctx, base, a);
  StoreU32(ctx, base + 4u, b);
  StoreU32(ctx, base + 8u, c);
  StoreU32(ctx, base + 12u, frame_sp);
  StoreU32(ctx, base + 16u, string_data);
  StoreU32(ctx, base + 20u, text_len);
  StoreU32(ctx, base + 24u, alloc_size);
  StoreU32(ctx, base + 28u, state_ptr);
  sp = ReturnFrameSp(ctx);
  word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + kMakeStringFromBytesSaveFrameBytes;
  StoreU32(ctx, sp, word_top);
  LeaveFrame();
}

}  // namespace artc
