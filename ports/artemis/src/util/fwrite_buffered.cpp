#include "util/engine_context.h"

namespace artc {

// Callees defined below in this file.
uint32_t PrepareStreamWrite(EngineContext* ctx, uint32_t stream);
uint32_t CopyBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

constexpr uint32_t kFwriteBufferedSaveFrameBytes = 24u;
constexpr uint32_t kFwriteBufferedResumeWordBytes = 4u;

}  // namespace

// Buffered write core: copies count bytes from src into the stream buffer,
// opening or growing that buffer through PrepareStreamWrite when it is unset.
// Answers the number of bytes handled, and is suspendable at the two writes.
uint32_t FwriteBuffered(EngineContext* ctx, uint32_t src, uint32_t count, uint32_t stream) {
  EnterFrame();
  uint32_t phase = 0u;
  uint32_t resume_code = 0u;  // names the write a rewinding pass has to replay
  uint32_t flush_fn = 0u;
  uint32_t buffer_end = 0u;   // one frame word: f->wend, then the bytes through the last '\n'
  uint32_t cursor = 0u;
  uint32_t does_not_fit = 0u;
  uint32_t unbuffered = 0u;
  uint32_t scan_left = 0u;
  uint32_t written = 0u;
  phase = ResumePhase(ctx);
  if (phase == kResumeRewinding) {
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t frame_top = ReturnFrameSp(ctx);
    frame_top = LoadU32(ctx, frame_top);
    StoreU32(ctx, sp, frame_top - kFwriteBufferedSaveFrameBytes);
    uint32_t frame = ReturnFrameSp(ctx);
    frame = LoadU32(ctx, frame);
    src = LoadU32(ctx, frame);
    count = LoadU32(ctx, frame + 4u);
    stream = LoadU32(ctx, frame + 8u);
    flush_fn = LoadU32(ctx, frame + 12u);
    buffer_end = LoadU32(ctx, frame + 16u);
    cursor = LoadU32(ctx, frame + 20u);
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRewinding) {
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t word_top = ReturnFrameSp(ctx);
    word_top = LoadU32(ctx, word_top);
    StoreU32(ctx, sp, word_top - kFwriteBufferedResumeWordBytes);
    uint32_t frame = ReturnFrameSp(ctx);
    frame = LoadU32(ctx, frame);
    resume_code = LoadU32(ctx, frame);
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    buffer_end = LoadU32(ctx, stream + 16u);   // f->wend
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    if (buffer_end == 0u) {
      if (PrepareStreamWrite(ctx, stream) != 0u) {
        goto write_done;
      }
      buffer_end = LoadU32(ctx, stream + 16u);
    }
    cursor = LoadU32(ctx, stream + 20u);       // f->wpos
    does_not_fit = count > buffer_end - cursor;
  }
  phase = ResumePhase(ctx);
  if (does_not_fit != 0u || phase == kResumeRewinding) {
    phase = ResumePhase(ctx);
    if (phase == kResumeRunning) {
      flush_fn = LoadU32(ctx, stream + 36u);   // f->write
    }
    phase = ResumePhase(ctx);
    if (phase == kResumeRunning || resume_code == 0u) {
      uint32_t flushed = CallTableT4(ctx, flush_fn, stream, src, count);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kResumeRunning;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_here;
      }
      src = flushed;
    }
    phase = ResumePhase(ctx);
    if (phase == kResumeRunning) {
      LeaveFrame();
      return src;
    }
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    unbuffered = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, stream + 75u)) < 0);
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    if (unbuffered != 0u) {
      buffer_end = 0u;
      goto copy_tail;
    }
    scan_left = count;
    for (;;) {
      buffer_end = scan_left;
      if (buffer_end == 0u) {
        goto copy_tail;
      }
      scan_left = buffer_end - 1u;
      if (LoadU8(ctx, src + scan_left) == 10u) {   // the last '\n' ends the flush
        break;
      }
    }
    flush_fn = LoadU32(ctx, stream + 36u);
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_code == kResumeSuspended) {
    written = CallTableT4(ctx, flush_fn, stream, src, buffer_end);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kResumeSuspended;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    if (written < buffer_end) {
      goto write_done;                           // a short write ends the call
    }
    count -= buffer_end;
    cursor = LoadU32(ctx, stream + 20u);
    src += buffer_end;
  }
copy_tail:;
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    CopyBytes(ctx, cursor, src, count);
    uint32_t wpos = LoadU32(ctx, stream + 20u);
    StoreU32(ctx, stream + 20u, count + wpos);
    written = count + buffer_end;
  }
write_done:;
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    LeaveFrame();
    return written;
  }
  TrapUnreachable();
suspend_here:;
  // Suspended: store the resume word the replay reads, then the frame words.
  uint32_t slot = ReturnFrameSp(ctx);
  slot = LoadU32(ctx, slot);
  StoreU32(ctx, slot, resume_code);
  uint32_t sp = ReturnFrameSp(ctx);
  uint32_t word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top);
  StoreU32(ctx, sp, word_top + kFwriteBufferedResumeWordBytes);
  uint32_t frame = ReturnFrameSp(ctx);
  frame = LoadU32(ctx, frame);
  StoreU32(ctx, frame, src);
  StoreU32(ctx, frame + 4u, count);
  StoreU32(ctx, frame + 8u, stream);
  StoreU32(ctx, frame + 12u, flush_fn);
  StoreU32(ctx, frame + 16u, buffer_end);
  StoreU32(ctx, frame + 20u, cursor);
  sp = ReturnFrameSp(ctx);
  word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top);
  StoreU32(ctx, sp, word_top + kFwriteBufferedSaveFrameBytes);
  LeaveFrame();
  return 0u;
}
}  // namespace artc
