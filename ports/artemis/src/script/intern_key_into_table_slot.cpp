#include "util/engine_context.h"

namespace artc {

// Forward declarations -- the intern worker and the array-part grower.
uint32_t InternString(EngineContext* ctx, uint32_t state, uint32_t bytes, uint32_t length);
void EnsureTableArrayRoom(EngineContext* ctx, uint32_t a);

// Appends one 16-byte table slot holding the interned form of a byte range: the
// array part is grown first when it is already full, the range is interned
// through the shared worker, and the string-tagged slot is linked into the
// array part's free position. Answers nothing.
void InternKeyIntoTableSlot(EngineContext* ctx, uint32_t state, uint32_t bytes, uint32_t length) {
  EnterFrame();
  uint32_t const kSaveFrameBytes = 16u;
  uint32_t const kResumeWordBytes = 4u;
  uint32_t frame = 0u;
  // The array part's record first, then the free slot the entry is written into.
  uint32_t slot = 0u;
  uint32_t resume_word = 0u;
  uint32_t array_full = 0u;
  // The call a suspend reports; set at the one suspend site below.
  uint32_t call_code = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 16-byte parameter frame and restore it, in the order
    // the suspend path wrote it -- the state, the range, its length, the slot.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    length = LoadU32(ctx, frame + 8u);
    slot = LoadU32(ctx, frame + 12u);
    bytes = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the second pop names the call that was in flight. The two pops
    // stay separate -- one region is read back at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the array part is full once its count has reached its capacity.
    slot = LoadU32(ctx, state + 16u);
    uint32_t const count = LoadU32(ctx, slot + 68u);
    array_full = static_cast<uint32_t>(count >= LoadU32(ctx, slot + 64u));
  }
  if (array_full != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_word == 0u) : 1u) {
      EnsureTableArrayRoom(ctx, state);
      if (ResumePhase(ctx) == kResumeSuspended) {
        call_code = 0u;
        goto suspend_frame;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, state + 8u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_word == 1u) : 1u) {
    uint32_t const interned = InternString(ctx, state, bytes, length);
    if (ResumePhase(ctx) == kResumeSuspended) {
      call_code = 1u;
      goto suspend_frame;
    }
    bytes = interned;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // One string tag, the interned key, then the free slot advances a whole
    // 16-byte record.
    StoreU32(ctx, slot + 8u, 4u);
    StoreU32(ctx, slot, bytes);
    StoreU32(ctx, state + 8u, LoadU32(ctx, state + 8u) + 16u);
  }
  LeaveFrame();
  return;
suspend_frame:
  // Suspended: park the word naming the call above the frame, step past it,
  // then write the parameter frame a rewind pass reads back.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), call_code);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, bytes);
  StoreU32(ctx, frame + 8u, length);
  StoreU32(ctx, frame + 12u, slot);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  LeaveFrame();
}

}  // namespace artc
