#include "util/engine_context.h"

namespace artc {

// Forward declarations -- the block freer and the buffered-chain releaser.
void DeallocateRawBlock(EngineContext* ctx, uint32_t block);
uint32_t ReleaseBufferedChainNode(EngineContext* ctx, uint32_t chain);

// The file object's teardown: restores the class vtable, releases the child
// stream it owns, and gives back the write buffer it holds itself. The step is
// suspendable, so the frame is spilled around the release. Answers the file.
uint32_t DestroyFileObject(EngineContext* ctx, uint32_t file) {
  EnterFrame();
  uint32_t const kFileVtable = 0xAE298u;
  uint32_t const kSaveFrameBytes = 8u;
  uint32_t const kResumeWordBytes = 4u;
  uint32_t frame = 0u;
  uint32_t child_stream = 0u;
  uint32_t resume_word = 0u;
  uint32_t skip_release = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 8-byte parameter frame and restore it in the order the
    // suspend path wrote it -- the file, then the child stream.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    file = LoadU32(ctx, frame);
    child_stream = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the second pop names the call that was in flight. The two pops
    // stay separate -- one region is read back at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the class vtable goes back before anything of it is used again,
    // and a file that owns no child stream has nothing to release.
    StoreU32(ctx, file, kFileVtable);
    child_stream = LoadU32(ctx, file + 8u);
    skip_release = static_cast<uint32_t>(child_stream == 0u);
  }
  if (skip_release == 0u) {
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_word == 0u) : 1u) {
      ReleaseBufferedChainNode(ctx, child_stream);
      if (ResumePhase(ctx) == kResumeSuspended) {
        // Suspended: park the word naming the call above the frame, step past
        // it, then write the parameter frame a rewind pass reads back.
        StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), 0u);
        StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
        frame = LoadU32(ctx, ReturnFrameSp(ctx));
        StoreU32(ctx, frame, file);
        StoreU32(ctx, frame + 4u, child_stream);
        StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
        LeaveFrame();
        return 0u;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, file + 8u, 0u);
      // The flag byte is signed: a negative one means the write buffer is owned
      // here, so it is cleared through its pointer, otherwise in place.
      if (static_cast<int32_t>(LoadS8(ctx, file + 23u)) <= -1) {
        StoreU8(ctx, LoadU32(ctx, file + 12u), 0u);
        StoreU32(ctx, file + 16u, 0u);
      } else {
        StoreU8(ctx, file + 23u, 0u);
        StoreU8(ctx, file + 12u, 0u);
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (static_cast<int32_t>(LoadS8(ctx, file + 23u)) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, file + 12u));
    }
    LeaveFrame();
    return file;
  }
  TrapUnreachable();
}

}  // namespace artc
