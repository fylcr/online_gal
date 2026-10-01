#include "util/engine_context.h"

namespace artc {

// Hands the pending bytes of one output buffer to its sink callback, resets the
// counters and answers whether anything was pending (-1 when there was not).
// Suspendable -- both calls below spill and are replayed on a rewind pass.
// `handle` carries the sink slot, the pending count and the start cursor in turn.
uint32_t FlushOutputBuffer(EngineContext* ctx, uint32_t buffer) {
  EnterFrame();
  uint32_t const kNoPending = 0xFFFFFFFFu;  // -1: there was nothing to hand over
  uint32_t const kSaveFrameBytes = 24u;
  uint32_t const kResumeWordBytes = 4u;
  uint32_t frame = 0u;
  uint32_t resume_word = 0u;
  uint32_t call_code = 0u;
  uint32_t handle = 0u;
  uint32_t stop = 0u;
  uint64_t span = 0u;
  uint32_t gate = 0u;
  uint32_t drained = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 24-byte frame -- buffer, handle, stop, span, gate.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    buffer = LoadU32(ctx, frame);
    stop = LoadU32(ctx, frame + 8u);
    span = LoadU64(ctx, frame + 12u);
    gate = LoadU32(ctx, frame + 20u);
    handle = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the second pop names the call that was in flight.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The stop word has caught up with the pending count: the sink is not called.
    stop = LoadU32(ctx, buffer + 28u);
    drained = static_cast<uint32_t>(stop >= LoadU32(ctx, buffer + 20u));
  }
  if (drained == 0u && (ResumePhase(ctx) == kResumeRunning || resume_word == 0u)) {
    const uint32_t flushed = CallTableT4(ctx, handle, buffer, 0u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    if (ResumePhase(ctx) == kResumeSuspended) {
      call_code = 0u;
      goto suspend_frame;
    }
    handle = flushed;
    if (ResumePhase(ctx) == kResumeRunning) {
      handle = LoadU32(ctx, buffer + 20u);
      if (handle == 0u) {
        LeaveFrame();
        return kNoPending;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The start cursor and the stop word bound the bytes the sink still owes.
    handle = LoadU32(ctx, buffer + 4u);
    stop = LoadU32(ctx, buffer + 8u);
    gate = static_cast<uint32_t>(handle < stop);
  }
  if (gate != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      span = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(handle - stop)));
      handle = LoadU32(ctx, buffer + 40u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_word == 1u) {
      CallTableT31(ctx, handle, buffer, span, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      if (ResumePhase(ctx) == kResumeSuspended) {
        call_code = 1u;
        goto suspend_frame;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, buffer + 28u, 0u);
    StoreU64(ctx, buffer + 16u, 0ull);
    StoreU64(ctx, buffer + 4u, 0ull);
    LeaveFrame();
    return 0u;
  }
  TrapUnreachable();
suspend_frame:;
  // Suspended: park the word naming the call above the frame, step past it, then
  // write the 24-byte frame a rewind pass reads back.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), call_code);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, buffer);
  StoreU32(ctx, frame + 4u, handle);
  StoreU32(ctx, frame + 8u, stop);
  StoreU64(ctx, frame + 12u, span);
  StoreU32(ctx, frame + 20u, gate);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
