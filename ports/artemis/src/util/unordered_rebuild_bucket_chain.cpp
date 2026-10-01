#include "util/engine_context.h"

namespace artc {

// Forward declarations of callees defined elsewhere in the tree.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
void UnlinkListNode(EngineContext* ctx, uint32_t node, uint32_t victim);

namespace {

constexpr uint32_t kUnorderedRebuildBucketChainBucketArray = 0x8ADC0u;
constexpr uint32_t kUnorderedRebuildBucketChainMsgBucketsAlt = 0x8AE54u;
constexpr uint32_t kUnorderedRebuildBucketChainMsgBuckets = 0x8AE58u;
constexpr uint32_t kUnorderedRebuildBucketChainLastException = 0xD015Cu;
constexpr uint32_t kUnorderedRebuildBucketChainSignedLimit = 0xFFFFFFFFu;
constexpr uint32_t kUnorderedRebuildBucketChainSaveFrameBytes = 32u;
constexpr uint32_t kUnorderedRebuildBucketChainResumeWordBytes = 4u;

}  // namespace

// Rebuilds one bucket chain of the unordered table: the chain is walked by
// comparing key bytes, then a fresh node is built, probed into the rebuilt
// bucket index and linked in place of the old chain.  Suspendable at every
// host call.
void UnorderedRebuildBucketChain(EngineContext* ctx, uint32_t map, uint32_t bucket,
                                 uint32_t work, uint32_t value) {
  uint32_t frame = 0u;         // frame a suspension saved its registers in
  uint32_t chain = 0u;         // current node of the bucket chain
  uint32_t short_key = 0u;     // the key is stored inline
  uint32_t node_len = 0u;      // the node's key length
  uint32_t next_node = 0u;     // chain node to descend to, or a field run
  uint32_t cmp_head = 0u;      // head-key byte comparison
  uint32_t cmp_tail = 0u;      // tail-key byte comparison
  uint32_t field_addr = 0u;    // address of the node's key field
  uint32_t cmp_len = 0u;       // bytes compared for this chain node
  uint32_t node_shorter = 0u;  // the node's key is shorter than the key
  uint32_t prev_key = 0u;      // the key pointer from before the suspension
  uint32_t probe = 0u;         // bucket-table probe address during the rebuild
  uint32_t alloc_node = 0u;    // freshly allocated chain node
  uint32_t suspend_code = 0u;  // code stored when this call suspends
  uint32_t carry = 0u;         // carried register: key length, then a chain slot
  uint32_t aux = 0u;           // carried register: short-key flag, then the map
  uint32_t resume_code = 0u;   // code of the suspension being resumed
  uint32_t key_data = 0u;      // the key's bytes, then the node built for it
  uint32_t shift = 0u;         // bit count the rebuild probe walks down
  uint32_t tmp = 0u;
  uint32_t tmp2 = 0u;
  uint32_t tmp3 = 0u;
  uint32_t tmp4 = 0u;
  uint64_t zero64 = 0;
  EnterFrame();
  if (ResumePhase(ctx) == kResumeRewinding) {
    tmp = ReturnFrameSp(ctx);
    tmp2 = ReturnFrameSp(ctx);
    tmp2 = LoadU32(ctx, tmp2);
    tmp2 -= kUnorderedRebuildBucketChainSaveFrameBytes;
    StoreU32(ctx, tmp, tmp2);
    tmp = ReturnFrameSp(ctx);
    tmp = LoadU32(ctx, tmp);
    frame = tmp;
    map = LoadU32(ctx, frame);
    tmp = LoadU32(ctx, frame + 8u);
    work = tmp;
    tmp = LoadU32(ctx, frame + 12u);
    value = tmp;
    tmp = LoadU32(ctx, frame + 16u);
    key_data = tmp;
    tmp = LoadU32(ctx, frame + 20u);
    chain = tmp;
    tmp = LoadU32(ctx, frame + 24u);
    carry = tmp;
    tmp = LoadU32(ctx, frame + 28u);
    aux = tmp;
    tmp = LoadU32(ctx, frame + 4u);
    bucket = tmp;
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    tmp = ReturnFrameSp(ctx);
    tmp2 = ReturnFrameSp(ctx);
    tmp2 = LoadU32(ctx, tmp2);
    tmp2 -= kUnorderedRebuildBucketChainResumeWordBytes;
    StoreU32(ctx, tmp, tmp2);
    tmp = ReturnFrameSp(ctx);
    tmp = LoadU32(ctx, tmp);
    resume_code = LoadU32(ctx, tmp);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    chain = LoadU32(ctx, bucket + 4u);
    if (chain != 0u) {
      tmp = LoadU32(ctx, work);
      tmp2 = work;
      tmp3 = LoadU8(ctx, work + 11u);
      carry = tmp3;
      tmp3 = tmp3 << (24u & 31u);
      tmp4 = 24u;
      tmp3 = (uint32_t)((int32_t)tmp3 >> (tmp4 & 31u));
      tmp4 = 0u;
      tmp3 = (uint32_t)((int32_t)tmp3 < (int32_t)tmp4);
      short_key = tmp3;
      tmp = (tmp3 != 0u) ? tmp : tmp2;
      key_data = tmp;
      tmp = LoadU32(ctx, work + 4u);
      tmp2 = carry;
      tmp3 = short_key;
      tmp = (tmp3 != 0u) ? tmp : tmp2;
      work = tmp;
      carry = bucket + 4u;
      for (;;) {
        tmp = LoadU8(ctx, chain + 27u);
        node_len = tmp;
        tmp = tmp << (24u & 31u);
        tmp2 = 24u;
        tmp = (uint32_t)((int32_t)tmp >> (tmp2 & 31u));
        tmp2 = 0u;
        tmp = (uint32_t)((int32_t)tmp < (int32_t)tmp2);
        aux = tmp;
        tmp = LoadU32(ctx, chain + 20u);
        tmp2 = node_len;
        tmp3 = aux;
        tmp = (tmp3 != 0u) ? tmp : tmp2;
        node_len = tmp;
        tmp = (node_len < work);
        node_shorter = tmp;
        tmp = node_len;
        tmp2 = work;
        tmp3 = node_shorter;
        tmp = (tmp3 != 0u) ? tmp : tmp2;
        cmp_len = tmp;
        if (cmp_len != 0u) {
          tmp = key_data;
          tmp2 = chain + 16u;
          field_addr = tmp2;
          tmp2 = LoadU32(ctx, tmp2);
          tmp3 = field_addr;
          tmp4 = aux;
          tmp2 = (tmp4 != 0u) ? tmp2 : tmp3;
          aux = tmp2;
          tmp3 = cmp_len;
          tmp = CompareBytes(ctx, key_data, aux, tmp3);
          cmp_head = tmp;
          if (tmp == 0u) {
            tmp = (work < node_len);
            if (tmp != 0u) {
              goto walk_greater;
            }
            goto compare_tails;
          }
          tmp = (uint32_t)((int32_t)cmp_head >
                           (int32_t)kUnorderedRebuildBucketChainSignedLimit);
          if (tmp != 0u) {
            goto compare_tails;
          }
          goto walk_greater;
        }
        tmp = (work >= node_len);
        if (tmp != 0u) {
          goto tail_shorter;
        }
      walk_greater:;
        next_node = LoadU32(ctx, chain);
        if (next_node != 0u) {
          goto descend_chain;
        }
        goto insert_at_tail;
      compare_tails:;
        tmp = CompareBytes(ctx, aux, key_data, cmp_len);
        cmp_tail = tmp;
        if (tmp != 0u) {
          goto tail_longer;
        }
      tail_shorter:;
        if (node_shorter != 0u) {
          goto advance_bucket;
        }
        goto walk_done;
      tail_longer:;
        tmp = (uint32_t)((int32_t)cmp_tail >
                         (int32_t)kUnorderedRebuildBucketChainSignedLimit);
        if (tmp != 0u) {
          goto walk_done;
        }
      advance_bucket:;
        carry = chain + 4u;
        next_node = LoadU32(ctx, chain + 4u);
        if (next_node == 0u) {
          goto walk_done;
        }
        chain = carry;
      descend_chain:;
        carry = chain;
        chain = next_node;
        continue;
      }
      TrapUnreachable();
    }
    chain = bucket + 4u;
  insert_at_tail:;
    carry = chain;
  walk_done:;
    aux = map;
    key_data = LoadU32(ctx, carry);
  }
  tmp = bucket;
  tmp2 = 0u;
  tmp = (ResumePhase(ctx) != 0u) ? tmp : tmp2;
  tmp2 = bucket;
  tmp3 = prev_key;
  tmp4 = key_data;
  tmp3 = (ResumePhase(ctx) != 0u) ? tmp3 : tmp4;
  prev_key = tmp3;
  tmp4 = (ResumePhase(ctx) == kResumeRewinding);
  tmp3 |= tmp4;
  tmp = (tmp3 != 0u) ? tmp : tmp2;
  bucket = tmp;
  tmp = (prev_key == 0u);
  tmp2 = (ResumePhase(ctx) == kResumeRewinding);
  tmp |= tmp2;
  if (tmp != 0u) {
    tmp = (resume_code == 0u);
    tmp = (ResumePhase(ctx) != 0u) ? tmp : 1u;
    if (tmp != 0u) {
      alloc_node = AllocateStringBuffer(ctx, 52u);
      suspend_code = 0u;
      tmp = ResumePhase(ctx);
      if (tmp == kResumeSuspended) {
        goto save_suspend_frame;
      }
      key_data = alloc_node;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      work = LoadU32(ctx, value);
      StoreU32(ctx, kUnorderedRebuildBucketChainLastException, 0u);
      value = key_data + 16u;
    }
    tmp = (resume_code == kResumeSuspended);
    tmp = (ResumePhase(ctx) != 0u) ? tmp : 1u;
    if (tmp != 0u) {
      HostOsQuery(ctx, 873u, value, work);
      suspend_code = 1u;
      tmp = ResumePhase(ctx);
      if (tmp == kResumeSuspended) {
        goto save_suspend_frame;
      }
    }
    if (ResumePhase(ctx) != 0u) {
      tmp = bucket;
    } else {
      work = LoadU32(ctx, kUnorderedRebuildBucketChainLastException);
      StoreU32(ctx, kUnorderedRebuildBucketChainLastException, 0u);
      if (work == kResumeSuspended) {
        goto unhandled_exception;
      }
      StoreU8(ctx, key_data + 28u, 0u);
      shift = 38u;
      work = kUnorderedRebuildBucketChainBucketArray;
      for (;;) {
        tmp = shift;
        tmp = tmp >> (1u & 31u);
        value = tmp;
        tmp = tmp << (2u & 31u);
        tmp += work;
        probe = tmp;
        tmp += kUnorderedRebuildBucketChainResumeWordBytes;
        tmp2 = work;
        tmp3 = LoadU32(ctx, probe);
        tmp4 = 11u;
        tmp3 = (tmp3 < tmp4);
        probe = tmp3;
        tmp = (tmp3 != 0u) ? tmp : tmp2;
        work = tmp;
        tmp = value;
        tmp2 = kUnorderedRebuildBucketChainSignedLimit;
        tmp ^= tmp2;
        tmp2 = shift;
        tmp += tmp2;
        tmp2 = value;
        tmp3 = probe;
        tmp = (tmp3 != 0u) ? tmp : tmp2;
        shift = tmp;
        if (shift != 0u) {
          continue;
        }
        break;
      }
      tmp2 = kUnorderedRebuildBucketChainMsgBucketsAlt;
      tmp = (work == kUnorderedRebuildBucketChainMsgBuckets) ? tmp2 : work;
      work = LoadU32(ctx, tmp);
      zero64 = 0ull;
      StoreU64(ctx, key_data + 44u, zero64);
      zero64 = 4575657221408423936ull;
      StoreU64(ctx, key_data + 36u, zero64);
      StoreU32(ctx, key_data + 32u, work);
      StoreU32(ctx, key_data + 8u, chain);
      zero64 = 0ull;
      StoreU64(ctx, key_data, zero64);
      StoreU32(ctx, carry, key_data);
      value = key_data;
      tmp = LoadU32(ctx, bucket);
      work = LoadU32(ctx, tmp);
      if (work != 0u) {
        StoreU32(ctx, bucket, work);
        value = LoadU32(ctx, carry);
      }
      tmp = LoadU32(ctx, bucket + 4u);
      UnlinkListNode(ctx, tmp, value);
      tmp = LoadU32(ctx, bucket + 8u);
      tmp += 1u;
      StoreU32(ctx, bucket + 8u, tmp);
      tmp = 1u;
    }
    bucket = tmp;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, aux + 4u, bucket);
    StoreU32(ctx, map, key_data);
    goto frame_stored;
  }
unhandled_exception:;
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = HostRet0(ctx);
    map = tmp;
    HostTempRet0(ctx);
    DeallocateRawBlock(ctx, key_data);
    HostOsRelease(ctx, map);
    TrapUnreachable();
  }
  goto frame_stored;
save_suspend_frame:;
  tmp = suspend_code;
  tmp2 = ReturnFrameSp(ctx);
  tmp2 = LoadU32(ctx, tmp2);
  StoreU32(ctx, tmp2, tmp);
  tmp = ReturnFrameSp(ctx);
  tmp2 = ReturnFrameSp(ctx);
  tmp2 = LoadU32(ctx, tmp2);
  tmp2 += kUnorderedRebuildBucketChainResumeWordBytes;
  StoreU32(ctx, tmp, tmp2);
  tmp = ReturnFrameSp(ctx);
  tmp = LoadU32(ctx, tmp);
  frame = tmp;
  StoreU32(ctx, frame, map);
  StoreU32(ctx, frame + 4u, bucket);
  StoreU32(ctx, frame + 8u, work);
  StoreU32(ctx, frame + 12u, value);
  StoreU32(ctx, frame + 16u, key_data);
  StoreU32(ctx, frame + 20u, chain);
  StoreU32(ctx, frame + 24u, carry);
  StoreU32(ctx, frame + 28u, aux);
  tmp = ReturnFrameSp(ctx);
  tmp2 = ReturnFrameSp(ctx);
  tmp2 = LoadU32(ctx, tmp2);
  tmp2 += kUnorderedRebuildBucketChainSaveFrameBytes;
  StoreU32(ctx, tmp, tmp2);
frame_stored:;
  LeaveFrame();
}
}  // namespace artc
