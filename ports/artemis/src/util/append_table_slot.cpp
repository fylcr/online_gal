#include "util/engine_context.h"

namespace artc {

// Forward declarations -- the array-part grower and the table allocator.
void EnsureTableArrayRoom(EngineContext* ctx, uint32_t state);
uint32_t AllocateTable(EngineContext* ctx, uint32_t state, uint32_t entries, uint32_t array);

// Appends one 16-byte tagged table slot to the state's array part: the part is
// grown first when it is full, the slot's payload is allocated, and the slot is
// then written at the part's free end with its tag. The step is suspendable, so
// each call spills the frame and is replayed on resume.
void AppendTableSlot(EngineContext* ctx, uint32_t state, uint32_t entries, uint32_t array) {
  EnterFrame();
  uint32_t const kSaveFrameBytes = 16u;
  uint32_t const kResumeWordBytes = 4u;
  uint32_t frame = 0u;
  uint32_t resume_word = 0u;
  // The array part's state: while running, first whether the part is full, then
  // the free slot an entry is written into. A rewind pass reads it back.
  uint32_t array_state = 0u;
  uint32_t call_code = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 16-byte parameter frame and restore it in the order the
    // suspend path read it back -- the state, the array, the part, the entries.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    array = LoadU32(ctx, frame + 8u);
    array_state = LoadU32(ctx, frame + 12u);
    entries = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the second pop names the call that was in flight. The two pops
    // stay separate -- one region is read back at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the array part is full once its count has reached its capacity.
    const uint32_t array_part = LoadU32(ctx, state + 16u);
    const uint32_t count = LoadU32(ctx, array_part + 68u);
    array_state = static_cast<uint32_t>(count >= LoadU32(ctx, array_part + 64u));
  }
  if (array_state != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_word == 0u) : 1u) {
      EnsureTableArrayRoom(ctx, state);
      if (ResumePhase(ctx) == kResumeSuspended) {
        call_code = 0u;
        goto suspend_frame;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    array_state = LoadU32(ctx, state + 8u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_word == 1u) : 1u) {
    const uint32_t allocated = AllocateTable(ctx, state, entries, array);
    if (ResumePhase(ctx) == kResumeSuspended) {
      call_code = 1u;
      goto suspend_frame;
    }
    entries = allocated;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The tag first, the payload, then the free end advances a whole 16-byte
    // slot.
    StoreU32(ctx, array_state + 8u, 5u);
    StoreU32(ctx, array_state, entries);
    StoreU32(ctx, state + 8u, LoadU32(ctx, state + 8u) + 16u);
  }
  LeaveFrame();
  return;
suspend_frame:
  // Suspended: park the word naming the call above the frame, step past it, then
  // write the parameter frame a rewind pass reads back.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), call_code);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, entries);
  StoreU32(ctx, frame + 8u, array);
  StoreU32(ctx, frame + 12u, array_state);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  LeaveFrame();
}

}  // namespace artc
