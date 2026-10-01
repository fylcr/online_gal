#include "util/engine_context.h"

namespace artc {

// Forward declarations -- the raw block pool and the bucket lookup.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void UnorderedGetBucket(EngineContext* ctx, uint32_t a);

// Tears a hashed node table down: the chain hanging off each of the bucket
// storage's two words is released first, then the storage is handed to the
// bucket lookup, and the storage itself goes back to the pool last. Returns
// nothing.
void ReleaseHashedNodeTable(EngineContext* ctx, uint32_t table, uint32_t buckets) {
  EnterFrame();
  // The reused slots: the node chain taken off the bucket storage, the resume
  // word naming the call in flight, and the value the suspend path hands over.
  uint32_t node = 0u;
  uint32_t resume_word = 0u;
  uint32_t spilled = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 12-byte frame the suspend path pushed and rebuild the
    // table, the bucket storage and the node from it.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 12u);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    table = LoadU32(ctx, saved_frame);
    buckets = LoadU32(ctx, saved_frame + 4u);
    node = LoadU32(ctx, saved_frame + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the second pop names the call that was in flight. The two pops
    // stay separate -- the driver keeps one region at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  // The rewinding bit is orred into the storage word rather than tested beside
  // it: null storage means there is nothing to tear down.
  if (buckets | static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding)) {
    if (ResumePhase(ctx) == kResumeRunning) {
      node = LoadU32(ctx, buckets);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_word == 0u) : 1u) {
      ReleaseHashedNodeTable(ctx, table, node);
      spilled = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      node = LoadU32(ctx, buckets + 4u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_word == 1u) : 1u) {
      ReleaseHashedNodeTable(ctx, table, node);
      spilled = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    // A running pass looks the storage's own bucket up; a rewinding pass keeps
    // the address its frame restored.
    table = ResumePhase(ctx) ? table : (buckets + 16u);
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_word == 2u) : 1u) {
      UnorderedGetBucket(ctx, table);
      spilled = 2u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      DeallocateRawBlock(ctx, buckets);
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
    StoreU32(ctx, saved_frame, table);
    StoreU32(ctx, saved_frame + 4u, buckets);
    StoreU32(ctx, saved_frame + 8u, node);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 12u);
  }
 exit_path:;
  LeaveFrame();
}

}  // namespace artc
