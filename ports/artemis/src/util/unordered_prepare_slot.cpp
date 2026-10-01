#include "util/engine_context.h"

namespace artc {

// Forward declarations of callees defined elsewhere in the tree.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);

namespace {

constexpr uint32_t kUnorderedPrepareSlotMsgBuckets = 0x8FAFCu;      // buckets_
constexpr uint32_t kUnorderedPrepareSlotMsgBoostHeader = 0x8FB05u;  // unordered/detail/implementation.hpp
constexpr uint32_t kUnorderedPrepareSlotMsgGetBucket = 0x8FB48u;    // get_bucket
constexpr uint32_t kUnorderedPrepareSlotPendingException = 0xD015Cu;
constexpr uint32_t kUnorderedPrepareSlotBucketMask = 0x7FFFFFFFu;
constexpr uint32_t kUnorderedPrepareSlotSignedLimit = 0xFFFFFFFFu;  // -1 as the signed bound tested below
constexpr uint32_t kUnorderedPrepareSlotSaveFrameBytes = 16u;
constexpr uint32_t kUnorderedPrepareSlotResumeWordBytes = 4u;

}  // namespace

// Prepares an unordered-map slot for insertion: grows the bucket table when the
// scheduler asks for it, splices the key into its chain, and bumps the element
// count; a table that cannot be queried fails the boost assertion instead.
uint32_t UnorderedPrepareSlot(EngineContext* ctx, uint32_t map, uint32_t key, uint32_t hash) {
  uint32_t count = 0u;         // element count plus the new key
  uint32_t status = 0u;        // exception slot read back after the grow
  uint32_t bucket_count = 0u;  // buckets the table holds
  uint32_t bucket = 0u;        // bucket index for the key's hash
  uint32_t slot = 0u;          // chain address of that bucket
  uint32_t node = 0u;          // chain node living there, if any
  uint32_t other_slot = 0u;    // chain address rebuilt from the bucket count
  uint32_t next_node = 0u;     // node that address points at
  uint32_t resume_code = 0u;   // which call a rewinding pass replays
  uint32_t frame_base = 0u;    // where a suspended call left its frame
  uint32_t suspend_code = 0u;  // resume word pushed when a call suspends
  uint32_t result = 0u;        // slot address handed back to the caller
  uint32_t tmp = 0u;
  uint32_t tmp2 = 0u;
  EnterFrame();
  if (ResumePhase(ctx) == kResumeRewinding) {
    tmp = ReturnFrameSp(ctx);
    tmp2 = ReturnFrameSp(ctx);
    tmp2 = LoadU32(ctx, tmp2);
    tmp2 -= kUnorderedPrepareSlotSaveFrameBytes;
    StoreU32(ctx, tmp, tmp2);
    frame_base = ReturnFrameSp(ctx);
    frame_base = LoadU32(ctx, frame_base);
    map = LoadU32(ctx, frame_base);
    key = LoadU32(ctx, frame_base + 4u);
    hash = LoadU32(ctx, frame_base + 8u);
    count = LoadU32(ctx, frame_base + 12u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    tmp = ReturnFrameSp(ctx);
    tmp2 = ReturnFrameSp(ctx);
    tmp2 = LoadU32(ctx, tmp2);
    tmp2 -= kUnorderedPrepareSlotResumeWordBytes;
    StoreU32(ctx, tmp, tmp2);
    tmp = ReturnFrameSp(ctx);
    tmp = LoadU32(ctx, tmp);
    tmp = LoadU32(ctx, tmp);
    resume_code = tmp;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    count = LoadU32(ctx, map + 8u);
    StoreU32(ctx, kUnorderedPrepareSlotPendingException, 0u);
    count += 1u;
  }
  if ((resume_code == 0u) | (ResumePhase(ctx) == kResumeRunning)) {
    HostOsSchedule(ctx, 1784u, map, count);
    suspend_code = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto save_suspend_frame;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    status = LoadU32(ctx, kUnorderedPrepareSlotPendingException);
    StoreU32(ctx, kUnorderedPrepareSlotPendingException, 0u);
    if (status != 1u) {
      bucket_count = LoadU32(ctx, map + 4u);
      bucket = REM_U(hash, bucket_count);
      hash = LoadU32(ctx, map + 20u);
      if (hash == 0u) {
        goto fail_assert;
      }
      tmp = bucket & kUnorderedPrepareSlotBucketMask;
      StoreU32(ctx, key + 4u, tmp);
      slot = bucket << (2u & 31u);
      slot += hash;
      node = LoadU32(ctx, slot);
      if (node == 0u) {
        // Empty bucket: splice the key between the two chains of the table.
        other_slot = bucket_count << (2u & 31u);
        other_slot += hash;
        next_node = LoadU32(ctx, other_slot);
        if (next_node != 0u) {
          tmp = LoadU32(ctx, next_node + 4u);
          tmp = tmp << (2u & 31u);
          tmp += hash;
          StoreU32(ctx, tmp, key);
        }
        StoreU32(ctx, slot, other_slot);
        tmp = LoadU32(ctx, other_slot);
        StoreU32(ctx, key, tmp);
        StoreU32(ctx, other_slot, key);
        goto finish_insert;
      }
      tmp = LoadU32(ctx, node);
      StoreU32(ctx, key, tmp);
      tmp = LoadU32(ctx, slot);
      StoreU32(ctx, tmp, key);
    finish_insert:;
      tmp = LoadU32(ctx, map + 8u);
      tmp += 1u;
      StoreU32(ctx, map + 8u, tmp);
      result = key;
      goto frame_stored;
    }
    // The grow failed: release whatever the key still owns, then the map.
    map = HostRet0(ctx);
    HostTempRet0(ctx);
    if (key != 0u) {
      tmp = LoadS8(ctx, key + 31u);
      tmp2 = kUnorderedPrepareSlotSignedLimit;
      tmp = (uint32_t)((int32_t)tmp <= (int32_t)tmp2);
      if (tmp != 0u) {
        tmp = LoadU32(ctx, key + 20u);
        DeallocateRawBlock(ctx, tmp);
      }
      tmp = LoadS8(ctx, key + 19u);
      tmp2 = kUnorderedPrepareSlotSignedLimit;
      tmp = (uint32_t)((int32_t)tmp <= (int32_t)tmp2);
      if (tmp != 0u) {
        tmp = LoadU32(ctx, key + 8u);
        DeallocateRawBlock(ctx, tmp);
      }
      DeallocateRawBlock(ctx, key);
    }
    HostOsRelease(ctx, map);
    TrapUnreachable();
  fail_assert:;
    HostAssertFail(ctx, kUnorderedPrepareSlotMsgBuckets, kUnorderedPrepareSlotMsgBoostHeader,
                   2956u, kUnorderedPrepareSlotMsgGetBucket);
    TrapUnreachable();
  }
  TrapUnreachable();
save_suspend_frame:;
  // A call suspended: push the resume word, then save the live words in the
  // frame above the stack pointer.
  tmp = ReturnFrameSp(ctx);
  tmp = LoadU32(ctx, tmp);
  StoreU32(ctx, tmp, suspend_code);
  tmp = ReturnFrameSp(ctx);
  tmp2 = ReturnFrameSp(ctx);
  tmp2 = LoadU32(ctx, tmp2);
  tmp2 += kUnorderedPrepareSlotResumeWordBytes;
  StoreU32(ctx, tmp, tmp2);
  tmp = ReturnFrameSp(ctx);
  tmp = LoadU32(ctx, tmp);
  frame_base = tmp;
  StoreU32(ctx, frame_base, map);
  StoreU32(ctx, frame_base + 4u, key);
  StoreU32(ctx, frame_base + 8u, hash);
  StoreU32(ctx, frame_base + 12u, count);
  tmp = ReturnFrameSp(ctx);
  tmp2 = ReturnFrameSp(ctx);
  tmp2 = LoadU32(ctx, tmp2);
  tmp2 += kUnorderedPrepareSlotSaveFrameBytes;
  StoreU32(ctx, tmp, tmp2);
  result = 0u;
frame_stored:;
  LeaveFrame();
  return result;
}
}  // namespace artc
