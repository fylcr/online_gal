#include "util/engine_context.h"

namespace artc {

// Forward declarations -- the byte filler, the block reallocator and the object
// header relocator.
uint32_t FillBytes(EngineContext* ctx, uint32_t dst, uint32_t value, uint32_t length);
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size);
void RelocateObjectHeader(EngineContext* ctx, uint32_t state, uint32_t source, uint32_t tag);

// Reserves room for an integer vector: the payload is reallocated to four bytes
// per element behind the object header, the header is moved onto the new block,
// the length and the element count are re-stamped and the payload is cleared.
// Returns the object's address, or zero when the pass suspended.
uint32_t ReserveIntVector(EngineContext* ctx, uint32_t state, uint32_t count, uint32_t size) {
  EnterFrame();
  // The reused slots: the word naming the call the suspend path was taken in, the
  // block the reallocation returned, the address the old object lived at, the size
  // the payload is reallocated to, the payload length now, and the word handed
  // over on the way out.
  uint32_t resume_word = 0u;
  uint32_t block = 0u;
  uint32_t old_header = 0u;
  uint32_t alloc_size = 0u;
  uint32_t payload_bytes = 0u;
  uint32_t spilled = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 24-byte frame the suspend path pushed and rebuild the
    // object, the element count, the request size, the old object, the payload
    // length and the reallocated size from it.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 24u);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, saved_frame);
    count = LoadU32(ctx, saved_frame + 4u);
    size = LoadU32(ctx, saved_frame + 8u);
    old_header = LoadU32(ctx, saved_frame + 12u);
    payload_bytes = LoadU32(ctx, saved_frame + 16u);
    alloc_size = LoadU32(ctx, saved_frame + 20u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the second pop names the call that was in flight. The two pops
    // stay separate -- the driver keeps one region at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Four bytes per element, and the twenty-byte object header in front of them.
    payload_bytes = count << 2u;
    alloc_size = payload_bytes + 20u;
    old_header = state;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(!resume_word) : 1u) {
    block = ReallocateBlock(ctx, state, 0u, 0u, alloc_size);
    spilled = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    state = block;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    RelocateObjectHeader(ctx, old_header, state, 6u);
    StoreU32(ctx, state + 12u, size);
    StoreU8(ctx, state + 6u, 0u);
    StoreU8(ctx, state + 7u, count);
    if (count != 0u) {
      FillBytes(ctx, state + 20u, 0u, payload_bytes);
    }
    LeaveFrame();
    return state;
  }
  TrapUnreachable();
 suspend_point:;
  {
    // The stack pointer lives in the word the frame pointer names, so every push
    // is a read-modify-write of that word.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), spilled);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, saved_frame, state);
    StoreU32(ctx, saved_frame + 4u, count);
    StoreU32(ctx, saved_frame + 8u, size);
    StoreU32(ctx, saved_frame + 12u, old_header);
    StoreU32(ctx, saved_frame + 16u, payload_bytes);
    StoreU32(ctx, saved_frame + 20u, alloc_size);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 24u);
  }
  LeaveFrame();
  return 0u;
}

}  // namespace artc
