#include "util/engine_context.h"

namespace artc {

uint32_t FwriteBuffered(EngineContext* ctx, uint32_t src, uint32_t count, uint32_t stream);

namespace {

constexpr uint32_t kFwriteConstFFFFFFFF = 0xFFFFFFFFu;
constexpr uint32_t kFwriteSaveFrameBytes = 24u;
constexpr uint32_t kFwriteResumeWordBytes = 4u;

}  // namespace

// Writes size * count bytes from ptr into the stream through the buffered write,
// then answers the number of complete size-byte records that landed. The write
// is skipped when the stream state word at +76 signs negative; suspendable.
uint32_t Fwrite(EngineContext* ctx, uint32_t ptr, uint32_t size, uint32_t count, uint32_t stream) {
  EnterFrame();
  // The write's own result, the byte count handed to it, and the byte count a
  // complete write has to account for.
  uint32_t written = 0u;
  uint32_t byte_count = 0u;
  uint32_t expected_bytes = 0u;
  // The code naming the call that was in flight, and the phase read per gate.
  uint32_t resume_code = 0u;
  uint32_t phase = 0u;
  uint32_t gate = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the saved frame and read its words back in the saved order.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t frame_top = ReturnFrameSp(ctx);
    frame_top = LoadU32(ctx, frame_top) - kFwriteSaveFrameBytes;
    StoreU32(ctx, sp, frame_top);
    uint32_t base = ReturnFrameSp(ctx);
    base = LoadU32(ctx, base);
    ptr = LoadU32(ctx, base);
    size = LoadU32(ctx, base + 4u);
    count = LoadU32(ctx, base + 8u);
    stream = LoadU32(ctx, base + 12u);
    byte_count = LoadU32(ctx, base + 16u);
    expected_bytes = LoadU32(ctx, base + 20u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word that names the call that was in flight.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t word_top = ReturnFrameSp(ctx);
    word_top = LoadU32(ctx, word_top) - kFwriteResumeWordBytes;
    StoreU32(ctx, sp, word_top);
    uint32_t word = ReturnFrameSp(ctx);
    word = LoadU32(ctx, word);
    resume_code = LoadU32(ctx, word);
  }
  gate = ResumePhase(ctx);
  if (gate != kResumeRunning) {
    // A resumed entry carries the return register's old value into the gate.
    gate = written;
  } else {
    byte_count = size * count;
    expected_bytes = byte_count;
    uint32_t state_word = LoadU32(ctx, stream + 76u);
    // A state word that signs negative is the stream's own error flag.
    gate = static_cast<uint32_t>(static_cast<int32_t>(state_word) <=
                                 static_cast<int32_t>(kFwriteConstFFFFFFFF));
  }
  // The rewinding bit is orred into the gate rather than tested beside it.
  gate |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (gate != 0u) {
    phase = ResumePhase(ctx);
    if (phase == kResumeRunning || resume_code == 0u) {
      written = FwriteBuffered(ctx, ptr, byte_count, stream);
      resume_code = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_here;
      }
      ptr = written;
    }
    phase = ResumePhase(ctx);
    if (phase == kResumeRunning) {
      goto write_done;
    }
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_code == kResumeSuspended) {
    written = FwriteBuffered(ctx, ptr, byte_count, stream);
    resume_code = kResumeSuspended;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
    ptr = written;
  }
write_done:;
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    if (ptr == expected_bytes) {
      // A full write accounts for every requested byte, so every record stands.
      LeaveFrame();
      return size != 0u ? count : 0u;
    }
    LeaveFrame();
    return DIV_U(ptr, size);
  }
  TrapUnreachable();
suspend_here:;
  // Push the resume word naming the call in flight, then the saved frame the
  // rewind pops; the words go out in the order a rewind reads them back.
  uint32_t slot = ReturnFrameSp(ctx);
  slot = LoadU32(ctx, slot);
  StoreU32(ctx, slot, resume_code);
  uint32_t sp = ReturnFrameSp(ctx);
  uint32_t word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + kFwriteResumeWordBytes;
  StoreU32(ctx, sp, word_top);
  uint32_t base = ReturnFrameSp(ctx);
  base = LoadU32(ctx, base);
  StoreU32(ctx, base, ptr);
  StoreU32(ctx, base + 4u, size);
  StoreU32(ctx, base + 8u, count);
  StoreU32(ctx, base + 12u, stream);
  StoreU32(ctx, base + 16u, byte_count);
  StoreU32(ctx, base + 20u, expected_bytes);
  sp = ReturnFrameSp(ctx);
  word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + kFwriteSaveFrameBytes;
  StoreU32(ctx, sp, word_top);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
