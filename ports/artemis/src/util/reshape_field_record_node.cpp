#include "util/engine_context.h"

namespace artc {

// Forward declaration -- the block reallocator.
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size,
                         uint32_t new_size);

// Reshapes a node whose payload is a record of fixed-width fields: the node is
// unlinked from the list of records with the same field kind, then each of its
// field arrays is released. Suspendable -- every call can suspend.
void ReshapeFieldRecordNode(EngineContext* ctx, uint32_t state, uint32_t node) {
  EnterFrame();
  // The call index the suspend path saves, the block being released with its old
  // byte size, and the block a reallocation hands back.
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
  // Each gate below releases one of the record's field arrays: the block sits at
  // a fixed offset in the node, and its old size is the element count times the
  // element width.
  if (ResumePhase(ctx) == kResumeRunning) {
    // Unlink the record from the list of records with the same field kind.
    uint32_t const next = LoadU32(ctx, node + 88u);
    uint32_t const prev = LoadU32(ctx, node + 84u);
    if (prev) {
      StoreU32(ctx, prev + 88u, next);
    } else {
      StoreU32(ctx, LoadU32(ctx, state + 16u) + 256u, next);
    }
    if (next) {
      StoreU32(ctx, next + 84u, prev);
    }
    // The record's own storage, in sixteen-byte entries.
    old_size = LoadU32(ctx, node + 80u) << 4;
    block = LoadU32(ctx, node + 76u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    reallocated = ReallocateBlock(ctx, state, block, old_size, 0u);
    resume_at = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto spill_point;
    }
    block = reallocated;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    old_size = LoadU32(ctx, node + 44u) << 2;
    block = LoadU32(ctx, node + 12u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    reallocated = ReallocateBlock(ctx, state, block, old_size, 0u);
    resume_at = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto spill_point;
    }
    block = reallocated;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    old_size = LoadU32(ctx, node + 52u) << 2;
    block = LoadU32(ctx, node + 16u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
    reallocated = ReallocateBlock(ctx, state, block, old_size, 0u);
    resume_at = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto spill_point;
    }
    block = reallocated;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    old_size = LoadU32(ctx, node + 40u) << 4;
    block = LoadU32(ctx, node + 8u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
    reallocated = ReallocateBlock(ctx, state, block, old_size, 0u);
    resume_at = 3u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto spill_point;
    }
    block = reallocated;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    old_size = LoadU32(ctx, node + 48u) << 2;
    block = LoadU32(ctx, node + 20u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
    reallocated = ReallocateBlock(ctx, state, block, old_size, 0u);
    resume_at = 4u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto spill_point;
    }
    block = reallocated;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    old_size = LoadU32(ctx, node + 56u) * 12u;
    block = LoadU32(ctx, node + 24u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
    reallocated = ReallocateBlock(ctx, state, block, old_size, 0u);
    resume_at = 5u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto spill_point;
    }
    block = reallocated;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    old_size = LoadU32(ctx, node + 36u) << 2;
    block = LoadU32(ctx, node + 28u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
    ReallocateBlock(ctx, state, block, old_size, 0u);
    resume_at = 6u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto spill_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 7u) {
    // The record's fixed 92-byte tail.
    ReallocateBlock(ctx, state, node, 92u, 0u);
    resume_at = 7u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto spill_point;
    }
  }
  LeaveFrame();
  return;
spill_point:;
  // Suspend: push the resume word, then the 16-byte frame holding the state, the
  // node, the block and its old size.
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
