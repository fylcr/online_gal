#include "util/engine_context.h"

namespace artc {

// Returns a raw block to the pool.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
// Reports a runtime error that has no handler.
void Panic(EngineContext* ctx, uint32_t a0);
// Fills a range of bytes with one value.
uint32_t FillBytes(EngineContext* ctx, uint32_t dst, uint32_t value, uint32_t length);

namespace {
// The source location of the assertion below and the guard word it keeps its
// state in.
constexpr uint32_t kClearUnorderedBucketsBucketsLabel = 0x8FAFCu;
constexpr uint32_t kClearUnorderedBucketsHeaderPath = 0x8FB05u;
constexpr uint32_t kClearUnorderedBucketsGetBucketLabel = 0x8FB48u;
constexpr uint32_t kClearUnorderedBucketsGuardWord = 0xD015Cu;
constexpr uint32_t kClearUnorderedBucketsAllBits = 0xFFFFFFFFu;
// The words a save frame costs, and the word that names a call in flight.
constexpr uint32_t kClearUnorderedBucketsSaveFrameBytes = 8u;
constexpr uint32_t kClearUnorderedBucketsResumeWordBytes = 4u;
}  // namespace

// Drops a hash table's bucket array: the live buckets are set back to the empty
// bucket, every block the last bucket chained is handed back to the pool, and
// the table is left holding no bucket array at all.
void ClearUnorderedBuckets(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  // The base of the save frame and the stack pointer it comes off.
  uint32_t frame = 0u;
  uint32_t sp = 0u;
  uint32_t top = 0u;
  // The word the save frame carries: first whether the table holds anything,
  // then the bucket array itself and the block chain it starts.
  uint32_t hold = 0u;
  // The call a rewinding pass replays, and the word a suspending pass pushes.
  uint32_t resume_point = 0u;
  uint32_t resume_code = 0u;
  // The bucket count, the bytes those buckets cover, and the bucket the scan
  // stops at.
  uint32_t bucket_count = 0u;
  uint32_t cleared_bytes = 0u;
  uint32_t base = 0u;
  // Set once this pass still has work to replay further down.
  uint32_t gate = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the save frame and read its words back.
    sp = ReturnFrameSp(ctx);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top) - kClearUnorderedBucketsSaveFrameBytes;
    StoreU32(ctx, sp, top);
    sp = ReturnFrameSp(ctx);
    frame = LoadU32(ctx, sp);
    a = LoadU32(ctx, frame);
    hold = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word, which names the call that was in flight.
    sp = ReturnFrameSp(ctx);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top) - kClearUnorderedBucketsResumeWordBytes;
    StoreU32(ctx, sp, top);
    sp = ReturnFrameSp(ctx);
    top = LoadU32(ctx, sp);
    resume_point = LoadU32(ctx, top);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    hold = LoadU32(ctx, a + 8u) == 0u;
    if (hold != 0u) {
      // A table that already holds nothing has no bucket array to drop.
      goto scan_done;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    hold = LoadU32(ctx, a + 20u);
    gate = hold == 0u;
  }
  gate |= ResumePhase(ctx) == kResumeRewinding;
  if (gate) {
    // The guard word is only cleared on the pass that does the work; a
    // rewinding pass replays the assertion instead.
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kClearUnorderedBucketsGuardWord, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      HostFailAssertion(ctx, 857u, kClearUnorderedBucketsBucketsLabel,
                        kClearUnorderedBucketsHeaderPath, 2956u,
                        kClearUnorderedBucketsGetBucketLabel);
      resume_code = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto save_frame;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      a = LoadU32(ctx, kClearUnorderedBucketsGuardWord);
      StoreU32(ctx, kClearUnorderedBucketsGuardWord, 0u);
      if (a == 1u) {
        goto unwind;
      }
      TrapUnreachable();
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Empty the buckets the array covers, then hand back every block the last
    // bucket chained.
    bucket_count = LoadU32(ctx, a + 4u);
    cleared_bytes = bucket_count << (2u & 31);
    base = cleared_bytes + hold;
    if (bucket_count != 0u) {
      FillBytes(ctx, hold, 0u, cleared_bytes);
    }
    hold = LoadU32(ctx, base);
    StoreU32(ctx, base, 0u);
    StoreU32(ctx, a + 8u, 0u);
    if (hold != 0u) {
      do {
        a = LoadU32(ctx, hold);
        if ((int32_t)LoadS8(ctx, hold + 19u) <= (int32_t)kClearUnorderedBucketsAllBits) {
          top = LoadU32(ctx, hold + 8u);
          DeallocateRawBlock(ctx, top);
        }
        DeallocateRawBlock(ctx, hold);
        hold = a;
      } while (hold != 0u);
    }
  }
scan_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    LeaveFrame();
    return;
  }
unwind:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Nothing is left to drop, so the caller is answered with the pending
    // trap value.
    a = FindMatchingCatch(ctx, 0u);
    HostTempRet0(ctx);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    Panic(ctx, a);
    resume_code = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto save_frame;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  LeaveFrame();
  return;
save_frame:;
  // Push the word naming the call in flight, then the save frame itself; both
  // go out in the order a rewind reads them back.
  sp = ReturnFrameSp(ctx);
  top = LoadU32(ctx, sp);
  StoreU32(ctx, top, resume_code);
  sp = ReturnFrameSp(ctx);
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top) + kClearUnorderedBucketsResumeWordBytes;
  StoreU32(ctx, sp, top);
  sp = ReturnFrameSp(ctx);
  frame = LoadU32(ctx, sp);
  StoreU32(ctx, frame, a);
  StoreU32(ctx, frame + 4u, hold);
  sp = ReturnFrameSp(ctx);
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top) + 8u;
  StoreU32(ctx, sp, top);
  LeaveFrame();
  return;
}

}  // namespace artc
