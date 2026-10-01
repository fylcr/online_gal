#include "util/engine_context.h"

namespace artc {

// Forward declaration -- the block reallocator.
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size);

// Reshapes a node that carries a single key/value pair inline: when the pair has
// been moved out of line the two halves are pointed back at each other, then the
// pair storage is reallocated to thirty-two bytes. Returns nothing.
void ReshapeInlinePairNode(EngineContext* ctx, uint32_t state, uint32_t node) {
  EnterFrame();
  // The reused slots: the word naming the call the suspend path was taken in, and
  // the value the suspend path hands over.
  uint32_t resume_word = 0u;
  uint32_t spilled = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 8-byte frame the suspend path pushed and rebuild the
    // state and the node from it.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 8u);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, saved_frame);
    node = LoadU32(ctx, saved_frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the second pop names the call that was in flight. The two pops
    // stay separate -- the driver keeps one region at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A pair still held inline sits at the node's own word sixteen, so anything
    // else there is a pointer to one that was moved away.
    if (LoadU32(ctx, node + 8u) != node + 16u) {
      // Each half takes the other's address in its word at sixteen. Both halves
      // are read again for the second store: the first one may have written the
      // word the second reads.
      StoreU32(ctx, LoadU32(ctx, node + 20u) + 16u, LoadU32(ctx, node + 16u));
      StoreU32(ctx, LoadU32(ctx, node + 16u) + 20u, LoadU32(ctx, node + 20u));
    }
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(!resume_word) : 1u) {
    ReallocateBlock(ctx, state, node, 32u, 0u);
    spilled = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  goto exit_path;
 suspend_point:;
  {
    // The stack pointer lives in the word the frame pointer names, so every push
    // is a read-modify-write of that word.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), spilled);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, saved_frame, state);
    StoreU32(ctx, saved_frame + 4u, node);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 8u);
  }
 exit_path:;
  LeaveFrame();
}

}  // namespace artc
