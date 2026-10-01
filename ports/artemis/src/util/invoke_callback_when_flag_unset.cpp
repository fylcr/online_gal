#include "util/engine_context.h"

namespace artc {

// Forward declaration -- defined in util/fwrite_buffered.cpp.
uint32_t FwriteBuffered(EngineContext* ctx, uint32_t src, uint32_t count, uint32_t stream);

// Runs the write a slot still owes while its guard flag is clear: the flag is
// read from the state word, and the buffered write is only taken on a running
// pass whose flag bit is unset. A suspend saves the state, the slot and the
// callback into the frame before unwinding. Returns nothing.
void InvokeCallbackWhenFlagUnset(EngineContext* ctx, uint32_t state, uint32_t slot, uint32_t callback) {
  EnterFrame();
  // The reused slots: the resume word naming the call in flight, and the word
  // the suspend path leaves above the frame.
  uint32_t resume_word = 0u;
  uint32_t spilled = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 12-byte frame the suspend path pushed and rebuild the
    // state, the slot and the callback from it.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 12u);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, saved_frame);
    slot = LoadU32(ctx, saved_frame + 4u);
    callback = LoadU32(ctx, saved_frame + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the second pop names the call that was in flight. The two pops
    // stay separate -- the driver keeps one region at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  // Only a running pass reads the guard flag; a rewinding pass is covered by the
  // bit below.
  uint32_t flag_clear = 0u;
  if (ResumePhase(ctx) == kResumeRunning) {
    flag_clear = static_cast<uint32_t>((LoadU8(ctx, state) & 32u) == 0u);
  }
  // The rewinding bit is orred into the clear flag rather than tested beside it.
  if (flag_clear | static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding)) {
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_word == 0u) : 1u) {
      FwriteBuffered(ctx, slot, callback, state);
      spilled = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
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
    StoreU32(ctx, saved_frame + 4u, slot);
    StoreU32(ctx, saved_frame + 8u, callback);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 12u);
  }
 exit_path:;
  LeaveFrame();
}

}  // namespace artc
