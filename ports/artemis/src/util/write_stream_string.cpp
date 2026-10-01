#include "util/engine_context.h"

namespace artc {

// Forward declarations -- the string measurer and the buffered writer.
uint32_t StringLength(EngineContext* ctx, uint32_t text);
uint32_t Fwrite(EngineContext* ctx, uint32_t ptr, uint32_t size, uint32_t count, uint32_t stream);

// Writes a NUL-terminated string to an output stream: the string is measured and
// its bytes are handed to the buffered writer as single units. Returns nothing.
void WriteStreamString(EngineContext* ctx, uint32_t str, uint32_t stream) {
  EnterFrame();
  // The reused slots: the string's length, the word naming the call the suspend
  // path was taken in, and the value the suspend path hands over.
  uint32_t length = 0u;
  uint32_t resume_word = 0u;
  uint32_t spilled = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 12-byte frame the suspend path pushed and rebuild the
    // string, the stream and the length from it.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 12u);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    str = LoadU32(ctx, saved_frame);
    stream = LoadU32(ctx, saved_frame + 4u);
    length = LoadU32(ctx, saved_frame + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the second pop names the call that was in flight. The two pops
    // stay separate -- the driver keeps one region at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    length = StringLength(ctx, str);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(!resume_word) : 1u) {
    Fwrite(ctx, str, 1u, length, stream);
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
    StoreU32(ctx, saved_frame, str);
    StoreU32(ctx, saved_frame + 4u, stream);
    StoreU32(ctx, saved_frame + 8u, length);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 12u);
  }
 exit_path:;
  LeaveFrame();
}

}  // namespace artc
