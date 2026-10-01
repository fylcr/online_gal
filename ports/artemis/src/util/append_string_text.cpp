#include "util/engine_context.h"

namespace artc {

// Forward declaration -- the checked append worker.
void AppendTextBytes(EngineContext* ctx, uint32_t text, uint32_t bytes, uint32_t length);

// Appends a string's text to a text object through the checked append worker,
// answering the updated text handle. The step is suspendable, so a 16-byte
// parameter frame is spilled around the call and read back on a rewind pass.
uint32_t AppendStringText(EngineContext* ctx, uint32_t text, uint32_t source) {
  EnterFrame();
  uint32_t const kSaveFrameBytes = 16u;
  uint32_t const kResumeWordBytes = 4u;
  // The scratch frame this step lends the guest stack, given back on the way out.
  uint32_t scratch = 0u;
  uint32_t resume_word = 0u;
  // The source string's bytes, or the flag saying it carries them itself.
  uint32_t contents = 0u;
  uint32_t self_contained = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 16-byte parameter frame and restore it in the order the
    // suspend path wrote it -- the text, the source, its bytes, the scratch base.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    const uint32_t frame = LoadU32(ctx, ReturnFrameSp(ctx));
    text = LoadU32(ctx, frame);
    source = LoadU32(ctx, frame + 4u);
    contents = LoadU32(ctx, frame + 8u);
    scratch = LoadU32(ctx, frame + 12u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the second pop names the call that was in flight. The two pops
    // stay separate -- one region is read back at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    scratch = GuestStackSp(ctx) - 16u;
    GuestStackSp(ctx) = scratch;
    // A source whose high flag byte is clear carries its own text.
    self_contained = static_cast<uint32_t>((LoadU8(ctx, source + 11u) >> 7u) == 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (self_contained != 0u) {
      StoreU32(ctx, text + 8u, LoadU32(ctx, source + 8u));
      StoreU64(ctx, text, LoadU64(ctx, source));
      GuestStackSp(ctx) = scratch + 16u;
      LeaveFrame();
      return text;
    }
    contents = LoadU32(ctx, source);
    source = LoadU32(ctx, source + 4u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_word == 0u) : 1u) {
    AppendTextBytes(ctx, text, contents, source);
    if (ResumePhase(ctx) == kResumeSuspended) {
      // Suspended: park the word naming the call above the frame, step past it,
      // then write the parameter frame a rewind pass reads back.
      StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), 0u);
      StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
      const uint32_t frame = LoadU32(ctx, ReturnFrameSp(ctx));
      StoreU32(ctx, frame, text);
      StoreU32(ctx, frame + 4u, source);
      StoreU32(ctx, frame + 8u, contents);
      StoreU32(ctx, frame + 12u, scratch);
      StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
      LeaveFrame();
      return 0u;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = scratch + 16u;
    LeaveFrame();
    return text;
  }
  TrapUnreachable();
}

}  // namespace artc
