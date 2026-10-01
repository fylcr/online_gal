#include "util/engine_context.h"

namespace artc {

// Forward declarations -- the block reallocator and the node chain releaser.
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size,
                         uint32_t new_size);
void ReleaseNodeChain(EngineContext* ctx, uint32_t state, uint32_t chain);

// Reshapes a node whose payload is a counted array: the storage chain the node
// still owns is released, then its two arrays and its fixed-size tail are
// released. Suspendable -- every call can suspend.
void ReshapeCountedArrayNode(EngineContext* ctx, uint32_t state, uint32_t node) {
  EnterFrame();
  // The call index the suspend path saves, the block being released with its
  // old byte size, and the block a reallocation hands back.
  uint32_t resume_at = 0u;
  uint32_t resume_point = 0u;
  uint32_t block = 0u;
  uint32_t old_size = 0u;
  uint32_t reallocated = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 16-byte frame the suspend path pushed.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 16u);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    node = LoadU32(ctx, frame + 4u);
    block = LoadU32(ctx, frame + 8u);
    old_size = LoadU32(ctx, frame + 12u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word pushed with that frame.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A running pass takes the storage chain out of the node's header.
    block = LoadU32(ctx, node + 32u);
  }
  // Resume point zero releases the chain the node still owns.
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    ReleaseNodeChain(ctx, node, block);
    resume_at = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto spill_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The counted array's stride: element count times twenty-four bytes.
    old_size = LoadU32(ctx, node + 48u) * 24u;
    block = LoadU32(ctx, node + 40u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    // The returned block is taken on only once the call has run to completion:
    // a suspended call is replayed with the arguments kept in the frame.
    reallocated = ReallocateBlock(ctx, state, block, old_size, 0u);
    resume_at = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto spill_point;
    }
    block = reallocated;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A second counted array: element count times the sixteen-byte stride.
    old_size = LoadU32(ctx, node + 44u) << 4;
    block = LoadU32(ctx, node + 32u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
    reallocated = ReallocateBlock(ctx, state, block, old_size, 0u);
    resume_at = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto spill_point;
    }
    block = reallocated;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
    // The tail storage is a fixed 120-byte block.
    ReallocateBlock(ctx, state, node, 120u, 0u);
    resume_at = 3u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto spill_point;
    }
  }
  LeaveFrame();
  return;
spill_point:;
  // Suspend: push the resume word, then the 16-byte frame holding the state, the
  // node and the pending release.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_at);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, node);
  StoreU32(ctx, frame + 8u, block);
  StoreU32(ctx, frame + 12u, old_size);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 16u);
  LeaveFrame();
  return;
}

}  // namespace artc
