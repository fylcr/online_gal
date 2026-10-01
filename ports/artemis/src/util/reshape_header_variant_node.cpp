#include "util/engine_context.h"

namespace artc {

// Forward declaration -- the block reallocator.
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size);

// Reshapes a node whose size is carried in the header variant field: the byte
// that picks the variant also fixes the shift applied to the node's type byte,
// and the storage is reallocated to the size that works out. Returns nothing.
void ReshapeHeaderVariantNode(EngineContext* ctx, uint32_t state, uint32_t node) {
  EnterFrame();
  // The reused slots: the word naming the call the suspend path was taken in, the
  // size the node's header works out to, and the value the suspend path hands
  // over.
  uint32_t resume_word = 0u;
  uint32_t old_size = 0u;
  uint32_t spilled = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 12-byte frame the suspend path pushed and rebuild the
    // size, the node and the state from it.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 12u);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    old_size = LoadU32(ctx, saved_frame);
    node = LoadU32(ctx, saved_frame + 4u);
    state = LoadU32(ctx, saved_frame + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the second pop names the call that was in flight. The two pops
    // stay separate -- the driver keeps one region at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The type byte is shifted by two or four bits and the byte at six picks
    // which, along with the header size the node carries.
    uint32_t const variant = LoadU8(ctx, node + 7u);
    uint32_t const flag = LoadU8(ctx, node + 6u);
    old_size = (variant << (flag ? 4u : 2u)) + (flag ? 24u : 20u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(!resume_word) : 1u) {
    ReallocateBlock(ctx, state, node, old_size, 0u);
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
    StoreU32(ctx, saved_frame, old_size);
    StoreU32(ctx, saved_frame + 4u, node);
    StoreU32(ctx, saved_frame + 8u, state);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 12u);
  }
 exit_path:;
  LeaveFrame();
}

}  // namespace artc
