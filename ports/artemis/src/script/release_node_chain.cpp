#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size);
void ResetNodeChainFlags(EngineContext* ctx, uint32_t a, uint32_t b);

namespace {

constexpr uint32_t kReleaseNodeChainSaveFrameBytes = 28u;   // live state pushed on suspend
constexpr uint32_t kReleaseNodeChainResumeWordBytes = 4u;   // resume word above that frame
constexpr uint32_t kReleaseNodeChainHeadOffset = 104u;      // chain head word in the state
constexpr uint32_t kReleaseNodeChainBlockBytes = 32u;       // the recycled block's size
constexpr uint32_t kReleaseNodeChainAllBits = 0xFFFFFFFFu;  // complements the byte it masks with
constexpr uint32_t kReleaseNodeChainFlagMask = 3u;          // the node's two low flag bits

}  // namespace

// Unlinks one node from the compiler-state chain whose head word lives at +104
// and hands the node's payload block back to the allocator.  The step is
// suspendable: the gate below holds the one call, and the resume code it saves
// is replayed while rewinding.  The node's +5 byte holds flag bits that the
// mask byte at record +20 clears; the rest of the walk is address bookkeeping.
//
// Frame, 28 bytes: [0] state, [4] chain, [8] node, [12] block, [16] word,
// [20] flags, [24] record.
void ReleaseNodeChain(EngineContext* ctx, uint32_t state, uint32_t chain) {
  EnterFrame();
  uint32_t node = 0u;
  uint32_t block = 0u;
  uint32_t word = 0u;
  uint32_t flags = 0u;
  uint32_t record = 0u;
  uint32_t resume_point = 0u;

  // Rewinding: pop the 28-byte frame the suspend path saved.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kReleaseNodeChainSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    node = LoadU32(ctx, frame + 8u);
    block = LoadU32(ctx, frame + 12u);
    word = LoadU32(ctx, frame + 16u);
    flags = LoadU32(ctx, frame + 20u);
    record = LoadU32(ctx, frame + 24u);
    chain = LoadU32(ctx, frame + 4u);
  }

  // Rewinding: pop the resume word pushed below that frame.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kReleaseNodeChainResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }

  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, state + kReleaseNodeChainHeadOffset);
    if (node == 0u) { goto exit_path; }
    record = LoadU32(ctx, state + 16u);
  }

walk_chain:;
  if (ResumePhase(ctx) == kResumeRunning) {
    word = LoadU32(ctx, node + 8u);
    if (word < chain) { goto exit_path; }
    StoreU32(ctx, state + kReleaseNodeChainHeadOffset, LoadU32(ctx, node));
    block = node + 16u;
    uint32_t const flag_byte = LoadU8(ctx, node + 5u);
    uint32_t const mask_byte = LoadU8(ctx, record + 20u);
    flags = flag_byte & (mask_byte ^ kReleaseNodeChainAllBits) & kReleaseNodeChainFlagMask;
  }

  if (flags != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      word = static_cast<uint32_t>(block != word);
      if (word != 0u) {
        uint32_t const tail = LoadU32(ctx, block + 4u);
        uint32_t const head = LoadU32(ctx, block);
        StoreU32(ctx, tail + 16u, head);
        word = LoadU32(ctx, block);
        block = LoadU32(ctx, block + 4u);
        StoreU32(ctx, word + 20u, block);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      uint32_t const called = ReallocateBlock(ctx, state, node, kReleaseNodeChainBlockBytes, 0u);
      resume_point = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      node = called;
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto reload_chain; }
  }

  if (ResumePhase(ctx) == kResumeRunning) {
    uint32_t const tail = LoadU32(ctx, node + 20u);
    uint32_t const head = LoadU32(ctx, node + 16u);
    StoreU32(ctx, tail + 16u, head);
    uint32_t const payload = LoadU32(ctx, node + 16u);
    flags = LoadU32(ctx, node + 20u);
    StoreU32(ctx, payload + 20u, flags);
    word = LoadU32(ctx, node + 8u);
    uint64_t const pair = LoadU64(ctx, word);
    StoreU64(ctx, node + 16u, pair);
    word = LoadU32(ctx, word + 8u);
    StoreU32(ctx, node + 24u, word);
    StoreU32(ctx, node + 8u, block);
    ResetNodeChainFlags(ctx, state, node);
  }

reload_chain:;
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, state + kReleaseNodeChainHeadOffset);
    if (node != 0u) { goto walk_chain; }
  }

suspend_point:;
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kReleaseNodeChainResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, state);
    StoreU32(ctx, frame + 4u, chain);
    StoreU32(ctx, frame + 8u, node);
    StoreU32(ctx, frame + 12u, block);
    StoreU32(ctx, frame + 16u, word);
    StoreU32(ctx, frame + 20u, flags);
    StoreU32(ctx, frame + 24u, record);
  }
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kReleaseNodeChainSaveFrameBytes);

exit_path:;
  LeaveFrame();
}
}  // namespace artc
