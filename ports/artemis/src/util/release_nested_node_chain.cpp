#include "util/engine_context.h"

namespace artc {

// Cross-product callees, defined in core/exception_cleanup.cpp and by the heap.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void ResumeExceptionCleanup(EngineContext* ctx, uint32_t owner, uint32_t block);

namespace {

constexpr uint32_t kReleaseNestedNodeChainConstFFFFFFFF = 0xFFFFFFFFu;
constexpr uint32_t kReleaseNestedNodeChainSaveFrameBytes = 12u;
constexpr uint32_t kReleaseNestedNodeChainResumeWordBytes = 4u;

}  // namespace

// Frees a node together with the chain hanging off it: the first child link is
// released recursively, then the second, then the node's tail chain is unwound
// through the exception cleanup path before the node block itself goes back to
// the heap. Answers nothing.
void ReleaseNestedNodeChain(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t frame = 0u;
  uint32_t child = 0u;
  uint32_t resume_code = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the saved frame and read its words back in the saved order.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t frame_top = ReturnFrameSp(ctx);
    frame_top = LoadU32(ctx, frame_top) - kReleaseNestedNodeChainSaveFrameBytes;
    StoreU32(ctx, sp, frame_top);
    frame = ReturnFrameSp(ctx);
    uint32_t base = LoadU32(ctx, frame);
    a = LoadU32(ctx, base);
    b = LoadU32(ctx, base + 4u);
    child = LoadU32(ctx, base + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word that names the call that was in flight.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t word_top = ReturnFrameSp(ctx);
    word_top = LoadU32(ctx, word_top) - kReleaseNestedNodeChainResumeWordBytes;
    StoreU32(ctx, sp, word_top);
    uint32_t word = ReturnFrameSp(ctx);
    word = LoadU32(ctx, word);
    resume_code = LoadU32(ctx, word);
  }
  uint32_t entered = b | static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (entered != 0u) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // A running pass starts from the node's first child link.
      child = LoadU32(ctx, b);
    }
    uint32_t phase = ResumePhase(ctx);
    if (phase == kResumeRunning || resume_code == 0u) {
      ReleaseNestedNodeChain(ctx, a, child);
      resume_code = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_here;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      child = LoadU32(ctx, b + 4u);
    }
    phase = ResumePhase(ctx);
    if (phase == kResumeRunning || resume_code == 1u) {
      ReleaseNestedNodeChain(ctx, a, child);
      resume_code = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_here;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The node's tail chain hangs at +32, and cleanup is handed its address.
      child = LoadU32(ctx, b + 32u);
      a = b + 28u;
    }
    phase = ResumePhase(ctx);
    if (phase == kResumeRunning || resume_code == 2u) {
      ResumeExceptionCleanup(ctx, a, child);
      resume_code = 2u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_here;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // A node whose link byte signed as negative owns a payload block of its
      // own; every node gives its node block back.
      if (static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, b + 27u)) <=
                                static_cast<int32_t>(kReleaseNestedNodeChainConstFFFFFFFF))) {
        uint32_t payload = LoadU32(ctx, b + 16u);
        DeallocateRawBlock(ctx, payload);
      }
      DeallocateRawBlock(ctx, b);
    }
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
  word_top = LoadU32(ctx, word_top) + kReleaseNestedNodeChainResumeWordBytes;
  StoreU32(ctx, sp, word_top);
  uint32_t base = ReturnFrameSp(ctx);
  base = LoadU32(ctx, base);
  StoreU32(ctx, base, a);
  StoreU32(ctx, base + 4u, b);
  StoreU32(ctx, base + 8u, child);
  sp = ReturnFrameSp(ctx);
  word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + kReleaseNestedNodeChainSaveFrameBytes;
  StoreU32(ctx, sp, word_top);
  LeaveFrame();
}

}  // namespace artc
