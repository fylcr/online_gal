#include "util/engine_context.h"

namespace artc {

// Forward declaration -- the byte setter the string is grown and copied through.
uint32_t SetStringBytes(EngineContext* ctx, uint32_t text, uint32_t bytes, uint32_t length);

// Replaces a string's contents from a source string: the source's payload is
// unwrapped first, then the bytes are handed to the grow-and-copy step. Returns
// the destination string, or zero when the pass suspended.
uint32_t SetStringFromSource(EngineContext* ctx, uint32_t text, uint32_t source) {
  EnterFrame();
  // The reused slots: the destination string, the word naming the call the suspend
  // path was taken in, and the value the suspend path hands over.
  uint32_t dest = 0u;
  uint32_t resume_word = 0u;
  uint32_t spilled = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 12-byte frame the suspend path pushed and rebuild the
    // payload, the length and the destination from it.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 12u);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    text = LoadU32(ctx, saved_frame);
    source = LoadU32(ctx, saved_frame + 4u);
    dest = LoadU32(ctx, saved_frame + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the second pop names the call that was in flight. The two pops
    // stay separate -- the driver keeps one region at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  // A string is never set from itself, so only the rewinding passes come through
  // with both arguments the same.
  if (static_cast<uint32_t>(text != source) | static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding)) {
    if (ResumePhase(ctx) == kResumeRunning) {
      dest = text;
      // A source whose tag byte has its top bit set keeps its payload out of line:
      // the word at zero is the payload and the word at four its length.
      if (LoadU8(ctx, source + 11u) >> 7u) {
        text = LoadU32(ctx, source);
      } else {
        text = source;
      }
      if (LoadU8(ctx, source + 11u) >> 7u) {
        source = LoadU32(ctx, source + 4u);
      } else {
        source = LoadU8(ctx, source + 11u);
      }
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(!resume_word) : 1u) {
      uint32_t const result = SetStringBytes(ctx, dest, text, source);
      spilled = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      text = result;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    LeaveFrame();
    return text;
  }
  TrapUnreachable();
 suspend_point:;
  {
    // The stack pointer lives in the word the frame pointer names, so every push
    // is a read-modify-write of that word.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), spilled);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, saved_frame, text);
    StoreU32(ctx, saved_frame + 4u, source);
    StoreU32(ctx, saved_frame + 8u, dest);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 12u);
  }
  LeaveFrame();
  return 0u;
}

}  // namespace artc
