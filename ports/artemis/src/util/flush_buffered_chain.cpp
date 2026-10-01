#include "util/engine_context.h"

namespace artc {

// Forward declaration -- the single-buffer flush every step below drains with.
uint32_t FlushOutputBuffer(EngineContext* ctx, uint32_t buffer);

// Flushes the write buffer of `node` and then drains the global chains, flushing
// every node that still has pending output. Answers the pending flag the walk
// accumulated. Suspendable -- each of the four calls spills and is replayed.
uint32_t FlushBufferedChain(EngineContext* ctx, uint32_t node) {
  EnterFrame();
  uint32_t const kFlushChainCell = 0xC8710u;    // head of the chain being drained
  uint32_t const kPendingChainCell = 0xCF8CCu;  // the list drained after it
  uint32_t const kSaveFrameBytes = 12u;
  uint32_t const kResumeWordBytes = 4u;
  uint32_t frame = 0u;
  uint32_t resume_word = 0u;
  uint32_t call_code = 0u;
  uint32_t pending = 0u;
  uint32_t dirty = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 12-byte frame -- the node, the pending flag, the state.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    node = LoadU32(ctx, frame);
    dirty = LoadU32(ctx, frame + 8u);
    pending = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the second pop names the call that was in flight.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (node != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // A negative buffered count means the node is still holding bytes.
      pending = static_cast<uint32_t>(static_cast<int32_t>(LoadU32(ctx, node + 76u)) < 0);
    }
    if (pending != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning || resume_word == 0u) {
        const uint32_t flushed = FlushOutputBuffer(ctx, node);
        if (ResumePhase(ctx) == kResumeSuspended) {
          call_code = 0u;
          goto suspend_frame;
        }
        node = flushed;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        LeaveFrame();
        return node;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_word == 1u) {
      const uint32_t flushed = FlushOutputBuffer(ctx, node);
      if (ResumePhase(ctx) == kResumeSuspended) {
        call_code = 1u;
        goto suspend_frame;
      }
      node = flushed;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      LeaveFrame();
      return node;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, kFlushChainCell);
  }
  if (node != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      node = LoadU32(ctx, kFlushChainCell);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_word == 2u) {
      const uint32_t chained = FlushBufferedChain(ctx, node);
      if (ResumePhase(ctx) == kResumeSuspended) {
        call_code = 2u;
        goto suspend_frame;
      }
      pending = chained;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, kPendingChainCell);
  }
  if (node != 0u || ResumePhase(ctx) == kResumeRewinding) {
    for (;;) {
      if (ResumePhase(ctx) == kResumeRunning) {
        // Probe the buffered count first; the cursor comparison is what decides.
        LoadU32(ctx, node + 76u);
        dirty = static_cast<uint32_t>(LoadU32(ctx, node + 20u) > LoadU32(ctx, node + 28u));
      }
      if (dirty != 0u || ResumePhase(ctx) == kResumeRewinding) {
        if (ResumePhase(ctx) == kResumeRunning || resume_word == 3u) {
          const uint32_t flushed = FlushOutputBuffer(ctx, node);
          if (ResumePhase(ctx) == kResumeSuspended) {
            call_code = 3u;
            goto suspend_frame;
          }
          dirty = flushed;
        }
        if (ResumePhase(ctx) == kResumeRunning) {
          pending = pending | dirty;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        node = LoadU32(ctx, node + 56u);
        if (node != 0u) continue;
      }
      break;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    LeaveFrame();
    return pending;
  }
  TrapUnreachable();
suspend_frame:;
  // Suspended: park the word naming the call above the frame, step past it, then
  // write the 12-byte frame a rewind pass reads back.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), call_code);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, node);
  StoreU32(ctx, frame + 4u, pending);
  StoreU32(ctx, frame + 8u, dirty);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
