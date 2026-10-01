#include "util/engine_context.h"

namespace artc {

// Allocates a block owned by the same state, which may grow the arena.
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size,
                         uint32_t new_size);

namespace {
// Every bit set in the flag byte a block header carries.
constexpr uint32_t kAdjustNodeCapacityAllBits = 0xFFFFFFFFu;
// The words a save frame costs, and the word that names a call in flight.
constexpr uint32_t kAdjustNodeCapacitySaveFrameBytes = 16u;
constexpr uint32_t kAdjustNodeCapacityResumeWordBytes = 4u;
}  // namespace

// Gives back the node the resize asked for: walks the free list of the state
// for a block whose capacity matches, and stamps the node's header onto it,
// splitting it off from its owner when the owner still holds a next block.
// When nothing matches, a fresh block is allocated and stamped instead.
uint32_t AdjustNodeCapacity(EngineContext* ctx, uint32_t state, uint32_t node) {
  EnterFrame();
  // The address the found node hangs off, its owner block, and the list cursor.
  uint32_t link_slot = 0u;
  uint32_t owner = 0u;
  uint32_t link = 0u;
  // The capacity the cursor block can hold.
  uint32_t capacity = 0u;
  // The node's header byte and the owner byte it is merged with.
  uint32_t flags = 0u;
  uint32_t merged = 0u;
  // The freshly allocated block, and the block that followed the owner.
  uint32_t block = 0u;
  uint32_t old_next = 0u;
  // Whatever value the current step stores, held aside so the address it goes
  // to is computed after it.
  uint32_t kind = 0u;
  // The base of the save frame and the stack pointer it comes off.
  uint32_t frame = 0u;
  uint32_t sp = 0u;
  uint32_t top = 0u;
  // The call a rewinding pass re-issues, and the word a suspending pass pushes.
  uint32_t resume_point = 0u;
  uint32_t resume_code = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the save frame and read its words back in the order they
    // were written.
    sp = ReturnFrameSp(ctx);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top) - kAdjustNodeCapacitySaveFrameBytes;
    StoreU32(ctx, sp, top);
    sp = ReturnFrameSp(ctx);
    frame = LoadU32(ctx, sp);
    state = LoadU32(ctx, frame);
    link_slot = LoadU32(ctx, frame + 8u);
    owner = LoadU32(ctx, frame + 12u);
    node = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word. It names the call that was in flight, so
    // only that call is issued again.
    sp = ReturnFrameSp(ctx);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top) - kAdjustNodeCapacityResumeWordBytes;
    StoreU32(ctx, sp, top);
    sp = ReturnFrameSp(ctx);
    top = LoadU32(ctx, sp);
    resume_point = LoadU32(ctx, top);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Walk the state's free list from the head slot, and from the state itself
    // read the owner block the free nodes belong to.
    link_slot = state + 104u;
    owner = LoadU32(ctx, state + 16u);
    link = LoadU32(ctx, state + 104u);
    if (link != 0u) {
      capacity = LoadU32(ctx, link + 8u);
      if (capacity >= node) {
        while (true) {
          link_slot = link;
          if (node == capacity) {
            // Hit: keep the node's own kind bits and take the owner's, so that
            // the header ends up with the kind the owner carries.
            flags = LoadU8(ctx, link_slot + 5u);
            merged = LoadU8(ctx, owner + 20u) ^ kAdjustNodeCapacityAllBits;
            flags &= merged;
            flags &= 3u;
            if (flags == 0u) {
              LeaveFrame();
              return link_slot;
            }
            flags ^= 3u;
            StoreU8(ctx, link_slot + 5u, flags);
            LeaveFrame();
            return link_slot;
          }
          link = LoadU32(ctx, link_slot);
          if (link == 0u) {
            break;
          }
          capacity = LoadU32(ctx, link + 8u);
          if (capacity < node) {
            break;
          }
        }
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    // No free block fits: ask for a fresh one, which is the step that can
    // suspend. Until it returns, the state keeps its old value.
    block = ReallocateBlock(ctx, state, 0u, 0u, 32u);
    resume_code = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto save_frame;
    }
    state = block;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Stamp the new block as the node: its kind, its capacity, the flags it
    // inherits from the owner, and the links on both sides of the owner's ring.
    kind = 10u;
    StoreU8(ctx, state + 4u, kind);
    merged = LoadU8(ctx, owner + 20u);
    StoreU32(ctx, state + 8u, node);
    flags = merged & 3u;
    StoreU8(ctx, state + 5u, flags);
    top = LoadU32(ctx, link_slot);
    StoreU32(ctx, state, top);
    StoreU32(ctx, link_slot, state);
    top = owner + 120u;
    StoreU32(ctx, state + 16u, top);
    old_next = LoadU32(ctx, owner + 140u);
    StoreU32(ctx, state + 20u, old_next);
    StoreU32(ctx, old_next + 16u, state);
    StoreU32(ctx, owner + 140u, state);
    LeaveFrame();
    return state;
  }
  TrapUnreachable();
save_frame:;
  // Push the word naming the call in flight, then the save frame itself; the
  // words go out in the order a rewind reads them back.
  frame = ReturnFrameSp(ctx);
  top = LoadU32(ctx, frame);
  StoreU32(ctx, top, resume_code);
  sp = ReturnFrameSp(ctx);
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top) + kAdjustNodeCapacityResumeWordBytes;
  StoreU32(ctx, sp, top);
  sp = ReturnFrameSp(ctx);
  frame = LoadU32(ctx, sp);
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, node);
  StoreU32(ctx, frame + 8u, link_slot);
  StoreU32(ctx, frame + 12u, owner);
  sp = ReturnFrameSp(ctx);
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top) + kAdjustNodeCapacitySaveFrameBytes;
  StoreU32(ctx, sp, top);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
