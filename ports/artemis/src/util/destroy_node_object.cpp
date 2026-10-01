#include "util/engine_context.h"

namespace artc {

// Returns a raw block to the pool.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
// Releases everything a node holds, child nodes included.
void ReleaseNodeTree(EngineContext* ctx, uint32_t state, uint32_t node);
// Points a node at its own empty storage.
void InitNodeStorage(EngineContext* ctx, uint32_t a);

namespace {
// The tags a destroyed node leaves behind, and the negated offset of the slot
// range it releases.
constexpr uint32_t kDestroyNodeObjectHeaderTag = 0xB5E84u;
constexpr uint32_t kDestroyNodeObjectPayloadTag = 0xB5F40u;
constexpr uint32_t kDestroyNodeObjectRangeBase = 0xFFFFFFC0u;
constexpr uint32_t kDestroyNodeObjectAllBits = 0xFFFFFFFFu;
// The words a save frame costs, and the word that names a call in flight.
constexpr uint32_t kDestroyNodeObjectSaveFrameBytes = 8u;
constexpr uint32_t kDestroyNodeObjectResumeWordBytes = 4u;
}  // namespace

// Destroys a node object: the payload it holds and the slots it owns go back to
// the pool, its storage is reset, and its own tags are cleared.
uint32_t DestroyNodeObject(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  // The base of the save frame and the stack pointer it comes off.
  uint32_t frame = 0u;
  uint32_t sp = 0u;
  uint32_t top = 0u;
  // The word the save frame carries: the node's storage, then the slot range
  // being released.
  uint32_t cursor = 0u;
  // The call a rewinding pass replays, and the word a suspending pass pushes.
  uint32_t resume_point = 0u;
  uint32_t resume_code = 0u;
  // The far end of the slot range, and the count the range is left with.
  uint32_t bound = 0u;
  uint32_t left = 0u;
  // The second operand of the one call below that needs two of them.
  uint32_t arg = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the save frame and read its words back.
    sp = ReturnFrameSp(ctx);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top) - kDestroyNodeObjectSaveFrameBytes;
    StoreU32(ctx, sp, top);
    sp = ReturnFrameSp(ctx);
    frame = LoadU32(ctx, sp);
    a = LoadU32(ctx, frame);
    cursor = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word, which names the call that was in flight.
    sp = ReturnFrameSp(ctx);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top) - kDestroyNodeObjectResumeWordBytes;
    StoreU32(ctx, sp, top);
    sp = ReturnFrameSp(ctx);
    top = LoadU32(ctx, sp);
    resume_point = LoadU32(ctx, top);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Stamp the object as dead, then give back the payload it held and the
    // children it owned.
    top = kDestroyNodeObjectPayloadTag;
    StoreU32(ctx, a + 84u, top);
    StoreU32(ctx, a, kDestroyNodeObjectHeaderTag);
    if ((int32_t)LoadS8(ctx, a + 123u) <= (int32_t)kDestroyNodeObjectAllBits) {
      top = LoadU32(ctx, a + 112u);
      DeallocateRawBlock(ctx, top);
    }
    arg = a + 100u;
    top = LoadU32(ctx, a + 104u);
    ReleaseNodeTree(ctx, arg, top);
    if ((int32_t)LoadS8(ctx, a + 99u) <= (int32_t)kDestroyNodeObjectAllBits) {
      top = LoadU32(ctx, a + 88u);
      DeallocateRawBlock(ctx, top);
    }
    cursor = a + 60u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    // Reset the node's own storage. This is the call that can suspend.
    InitNodeStorage(ctx, cursor);
    resume_code = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto save_frame;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Hand back the slots the node owns, one at a time.
    cursor = LoadU32(ctx, a - kDestroyNodeObjectRangeBase);
    bound = LoadU32(ctx, a + 68u);
    if (cursor != bound) {
      do {
        top = LoadU32(ctx, cursor);
        DeallocateRawBlock(ctx, top);
        cursor = cursor + 4u;
      } while (bound != cursor);
      cursor = LoadU32(ctx, a + 68u);
      bound = LoadU32(ctx, a + 64u);
      if (cursor != bound) {
        // The range loses one slot: the far end is stepped back over it.
        left = cursor - bound;
        left = left - 4u;
        left = left >> (2u & 31);
        left = left ^ kDestroyNodeObjectAllBits;
        left = left << (2u & 31);
        left = left + cursor;
        StoreU32(ctx, a + 68u, left);
      }
    }
    // Then the storage the node pointed at, and the buffers it owned.
    cursor = LoadU32(ctx, a + 60u);
    if (cursor != 0u) {
      DeallocateRawBlock(ctx, cursor);
    }
    if ((int32_t)LoadS8(ctx, a + 59u) <= (int32_t)kDestroyNodeObjectAllBits) {
      top = LoadU32(ctx, a + 48u);
      DeallocateRawBlock(ctx, top);
    }
    if ((int32_t)LoadS8(ctx, a + 39u) <= (int32_t)kDestroyNodeObjectAllBits) {
      top = LoadU32(ctx, a + 28u);
      DeallocateRawBlock(ctx, top);
    }
    if ((int32_t)LoadS8(ctx, a + 15u) <= (int32_t)kDestroyNodeObjectAllBits) {
      top = LoadU32(ctx, a + 4u);
      DeallocateRawBlock(ctx, top);
    }
    LeaveFrame();
    return a;
  }
  TrapUnreachable();
save_frame:;
  // Push the word naming the call in flight, then the save frame itself; both
  // go out in the order a rewind reads them back.
  sp = ReturnFrameSp(ctx);
  top = LoadU32(ctx, sp);
  StoreU32(ctx, top, resume_code);
  sp = ReturnFrameSp(ctx);
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top) + kDestroyNodeObjectResumeWordBytes;
  StoreU32(ctx, sp, top);
  sp = ReturnFrameSp(ctx);
  frame = LoadU32(ctx, sp);
  StoreU32(ctx, frame, a);
  StoreU32(ctx, frame + 4u, cursor);
  sp = ReturnFrameSp(ctx);
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top) + 8u;
  StoreU32(ctx, sp, top);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
