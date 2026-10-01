#include "util/engine_context.h"

namespace artc {

// Cross-product callees: the raw-block deallocator and the chain flusher.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
uint32_t FlushBufferedChain(EngineContext* ctx, uint32_t a);

namespace {

constexpr uint32_t kReleaseBufferedChainNodeHeadAddr = 0xCF8CCu;
constexpr uint32_t kReleaseBufferedChainNodeSaveFrameBytes = 20u;
constexpr uint32_t kReleaseBufferedChainNodeResumeWordBytes = 4u;

}  // namespace

// Unlinks one buffered-chain node, flushes the bytes it still buffers, runs the
// teardown hook it carries and frees the node. Answers the flush result OR-ed
// with the hook result, or zero when the step re-suspends.
uint32_t ReleaseBufferedChainNode(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t next_node = 0u;
  uint32_t prev_node = 0u;
  uint32_t sign_flag = 0u;
  uint32_t marked = 0u;
  uint32_t resume_code = 0u;
  uint32_t result = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the saved frame and read its words back in saved order.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t frame_top = ReturnFrameSp(ctx);
    frame_top = LoadU32(ctx, frame_top) - kReleaseBufferedChainNodeSaveFrameBytes;
    StoreU32(ctx, sp, frame_top);
    uint32_t frame = ReturnFrameSp(ctx);
    frame = LoadU32(ctx, frame);
    a = LoadU32(ctx, frame);
    prev_node = LoadU32(ctx, frame + 8u);
    sign_flag = LoadU32(ctx, frame + 12u);
    marked = LoadU32(ctx, frame + 16u);
    next_node = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word naming the call that was in flight.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t word_top = ReturnFrameSp(ctx);
    word_top = LoadU32(ctx, word_top) - kReleaseBufferedChainNodeResumeWordBytes;
    StoreU32(ctx, sp, word_top);
    uint32_t word = ReturnFrameSp(ctx);
    word = LoadU32(ctx, word);
    resume_code = LoadU32(ctx, word);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A fresh pass starts by unlinking the node from the global chain.
    sign_flag = static_cast<uint32_t>(
        static_cast<int32_t>(LoadU32(ctx, a + 76u)) >= static_cast<int32_t>(0u));
    marked = LoadU32(ctx, a) & 1u;
    if (marked == 0u) {
      prev_node = LoadU32(ctx, a + 52u);
      if (prev_node != 0u) {
        uint32_t successor = LoadU32(ctx, a + 56u);
        StoreU32(ctx, prev_node + 56u, successor);
      }
      next_node = LoadU32(ctx, a + 56u);
      if (next_node != 0u) {
        StoreU32(ctx, next_node + 52u, prev_node);
      }
      if (LoadU32(ctx, kReleaseBufferedChainNodeHeadAddr) == a) {
        StoreU32(ctx, kReleaseBufferedChainNodeHeadAddr, next_node);
      }
    }
  }
  uint32_t phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_code == 0u) {
    uint32_t flushed = FlushBufferedChain(ctx, a);
    resume_code = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
    prev_node = flushed;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    next_node = LoadU32(ctx, a + 12u);
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_code == 1u) {
    uint32_t hook_result = CallTableT2(ctx, next_node, a);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
    next_node = hook_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    uint32_t payload = LoadU32(ctx, a + 96u);
    if (payload != 0u) {
      DeallocateRawBlock(ctx, payload);
    }
    // The node block goes back only when the low flag bit is clear.
    if (marked == 0u) {
      DeallocateRawBlock(ctx, a);
    }
    result = next_node | prev_node;  // flush result OR hook result
    LeaveFrame();
    return result;
  }
  TrapUnreachable();
suspend_here:;
  // Push the resume word naming the call that was in flight, then the saved
  // frame; the words go back in the order the rewind pops them.
  uint32_t slot = ReturnFrameSp(ctx);
  slot = LoadU32(ctx, slot);
  StoreU32(ctx, slot, resume_code);
  uint32_t sp = ReturnFrameSp(ctx);
  uint32_t word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + kReleaseBufferedChainNodeResumeWordBytes;
  StoreU32(ctx, sp, word_top);
  uint32_t frame = ReturnFrameSp(ctx);
  frame = LoadU32(ctx, frame);
  StoreU32(ctx, frame, a);
  StoreU32(ctx, frame + 4u, next_node);
  StoreU32(ctx, frame + 8u, prev_node);
  StoreU32(ctx, frame + 12u, sign_flag);
  StoreU32(ctx, frame + 16u, marked);
  sp = ReturnFrameSp(ctx);
  word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + kReleaseBufferedChainNodeSaveFrameBytes;
  StoreU32(ctx, sp, word_top);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
