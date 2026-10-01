#include "util/engine_context.h"

namespace artc {

// Forward declarations of callees defined elsewhere in the tree.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void Panic(EngineContext* ctx, uint32_t a0);

namespace {

constexpr uint32_t kUnorderedGetBucketMsgBuckets = 0xB5B6Cu;      // buckets_
constexpr uint32_t kUnorderedGetBucketMsgBoostHeader = 0xB5B75u;  // unordered/detail/implementation.hpp
constexpr uint32_t kUnorderedGetBucketMsgGetBucket = 0xB5BB8u;    // get_bucket
constexpr uint32_t kUnorderedGetBucketLastException = 0xD015Cu;   // pending exception slot
constexpr uint32_t kUnorderedGetBucketSignedLimit = 0xFFFFFFFFu;  // -1 as the signed bound tested below
constexpr uint32_t kUnorderedGetBucketSaveFrameBytes = 8u;
constexpr uint32_t kUnorderedGetBucketResumeWordBytes = 4u;

}  // namespace

// Answers the bucket that owns a hash: the index is the chain head plus the
// table's bucket count scaled by the low-order mask, and a table that cannot be
// queried panics through the boost assertion route before its chain is drained.
void UnorderedGetBucket(EngineContext* ctx, uint32_t table) {
  uint32_t chain = 0u;         // bucket chain head taken from the table
  uint32_t current = 0u;       // node being drained from that chain
  uint32_t next_node = 0u;     // its successor while draining
  uint32_t resume_code = 0u;   // which call a rewinding pass replays
  uint32_t frame_base = 0u;    // where a suspended call left its frame
  uint32_t suspend_code = 0u;  // resume word pushed when a call suspends
  uint32_t tmp = 0u;
  uint32_t tmp2 = 0u;
  EnterFrame();
  if (ResumePhase(ctx) == kResumeRewinding) {
    tmp = ReturnFrameSp(ctx);
    tmp2 = ReturnFrameSp(ctx);
    tmp2 = LoadU32(ctx, tmp2);
    tmp2 -= kUnorderedGetBucketSaveFrameBytes;
    StoreU32(ctx, tmp, tmp2);
    frame_base = ReturnFrameSp(ctx);
    frame_base = LoadU32(ctx, frame_base);
    table = LoadU32(ctx, frame_base);
    frame_base = LoadU32(ctx, frame_base + 4u);
    chain = frame_base;
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    tmp = ReturnFrameSp(ctx);
    tmp2 = ReturnFrameSp(ctx);
    tmp2 = LoadU32(ctx, tmp2);
    tmp2 -= kUnorderedGetBucketResumeWordBytes;
    StoreU32(ctx, tmp, tmp2);
    tmp = ReturnFrameSp(ctx);
    tmp = LoadU32(ctx, tmp);
    tmp = LoadU32(ctx, tmp);
    resume_code = tmp;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    chain = LoadU32(ctx, table + 32u);
  }
  if ((chain != 0u) | (ResumePhase(ctx) == kResumeRewinding)) {
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, table + 16u);
      tmp <<= (2u & 31u);
      current = LoadU32(ctx, chain + tmp);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (current == 0u) {
        goto drop_chain;
      }
      // Drain the bucket chain, then re-read the head: the table grew under us.
      while (current != 0u) {
        next_node = LoadU32(ctx, current);
        DeallocateRawBlock(ctx, current);
        current = next_node;
      }
      chain = LoadU32(ctx, table + 32u);
      if (chain != 0u) {
        goto drop_chain;
      }
      StoreU32(ctx, kUnorderedGetBucketLastException, 0u);
    }
    if ((resume_code == 0u) | (ResumePhase(ctx) == kResumeRunning)) {
      HostFailAssertion(ctx, 857u, kUnorderedGetBucketMsgBuckets,
                        kUnorderedGetBucketMsgBoostHeader, 2956u,
                        kUnorderedGetBucketMsgGetBucket);
      suspend_code = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto save_suspend_frame;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, kUnorderedGetBucketLastException);
      table = tmp;
      StoreU32(ctx, kUnorderedGetBucketLastException, 0u);
      if (table != 1u) {
        goto trap_path;
      }
      table = FindMatchingCatch(ctx, 0u);
      HostTempRet0(ctx);
    }
    if ((resume_code == 1u) | (ResumePhase(ctx) == kResumeRunning)) {
      Panic(ctx, table);
      suspend_code = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto save_suspend_frame;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      TrapUnreachable();
    }
  drop_chain:;
    if (ResumePhase(ctx) == kResumeRunning) {
      DeallocateRawBlock(ctx, chain);
      StoreU64(ctx, table + 28u, 0ull);
      StoreU32(ctx, table + 20u, 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadS8(ctx, table + 11u);
    tmp2 = kUnorderedGetBucketSignedLimit;
    tmp = (uint32_t)((int32_t)tmp <= (int32_t)tmp2);
    if (tmp != 0u) {
      tmp = LoadU32(ctx, table);
      DeallocateRawBlock(ctx, tmp);
    }
    goto frame_stored;
  }
trap_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  goto frame_stored;
save_suspend_frame:;
  // A call suspended: push the resume word, then save the live words in the
  // frame above the stack pointer.
  tmp = ReturnFrameSp(ctx);
  tmp = LoadU32(ctx, tmp);
  StoreU32(ctx, tmp, suspend_code);
  tmp = ReturnFrameSp(ctx);
  tmp2 = ReturnFrameSp(ctx);
  tmp2 = LoadU32(ctx, tmp2);
  tmp2 += kUnorderedGetBucketResumeWordBytes;
  StoreU32(ctx, tmp, tmp2);
  tmp = ReturnFrameSp(ctx);
  tmp = LoadU32(ctx, tmp);
  frame_base = tmp;
  StoreU32(ctx, frame_base, table);
  StoreU32(ctx, frame_base + 4u, chain);
  tmp = ReturnFrameSp(ctx);
  tmp2 = ReturnFrameSp(ctx);
  tmp2 = LoadU32(ctx, tmp2);
  tmp2 += kUnorderedGetBucketSaveFrameBytes;
  StoreU32(ctx, tmp, tmp2);
frame_stored:;
  LeaveFrame();
}
}  // namespace artc
