#include "util/engine_context.h"

namespace artc {

// Forward declarations of callees defined elsewhere in the tree.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
void UnlinkListNode(EngineContext* ctx, uint32_t node, uint32_t victim);

namespace {

constexpr uint32_t kUnorderedInsertBucketNodeNodeTag = 0xB6AD4u;
constexpr uint32_t kUnorderedInsertBucketNodeLastException = 0xD015Cu;
constexpr uint32_t kUnorderedInsertBucketNodeSignedLimit = 0xFFFFFFFFu;
constexpr uint32_t kUnorderedInsertBucketNodeSaveFrameBytes = 32u;
constexpr uint32_t kUnorderedInsertBucketNodeResumeWordBytes = 4u;

}  // namespace

// Links a node into the bucket chain its key's hash selects: the chain is
// walked comparing key bytes until the node's place is found, otherwise a
// fresh bucket node is built at the end of the chain.  Suspendable at every
// host call.
void UnorderedInsertBucketNode(EngineContext* ctx, uint32_t map, uint32_t bucket, uint32_t key,
                               uint32_t node, uint32_t value, uint32_t link) {
  uint32_t frame = 0u;         // frame a suspension saved its registers in
  uint32_t field_addr = 0u;    // address of the node's key field
  uint32_t cmp_result = 0u;    // byte comparison of the two keys
  uint32_t new_node = 0u;      // freshly allocated bucket node
  uint32_t suspend_code = 0u;  // code stored when this call suspends
  uint32_t carry = 0u;         // carried register: key length, then the map copy
  uint32_t aux = 0u;           // carried register: short-key flag, then a pointer
  uint32_t resume_code = 0u;   // code of the suspension being resumed
  uint32_t key_data = 0u;      // pointer to the key's bytes
  uint32_t key_longer = 0u;    // the key is longer than the node's key
  uint32_t cmp_len = 0u;       // bytes compared for this chain node
  uint32_t prev_key = 0u;      // the key pointer from before the suspension
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
    tmp2 -= kUnorderedInsertBucketNodeSaveFrameBytes;
    StoreU32(ctx, tmp, tmp2);
    tmp = ReturnFrameSp(ctx);
    tmp = LoadU32(ctx, tmp);
    frame = tmp;
    map = LoadU32(ctx, frame);
    tmp = LoadU32(ctx, frame + 8u);
    key = tmp;
    tmp = LoadU32(ctx, frame + 12u);
    node = tmp;
    tmp = LoadU32(ctx, frame + 16u);
    value = tmp;
    tmp = LoadU32(ctx, frame + 20u);
    link = tmp;
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
    tmp2 -= kUnorderedInsertBucketNodeResumeWordBytes;
    StoreU32(ctx, tmp, tmp2);
    tmp = ReturnFrameSp(ctx);
    tmp = LoadU32(ctx, tmp);
    resume_code = LoadU32(ctx, tmp);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, bucket + 4u);
    if (node != 0u) {
      tmp = LoadU32(ctx, key);
      tmp2 = key;
      tmp3 = LoadU8(ctx, key + 11u);
      link = tmp3;
      tmp3 = tmp3 << (24u & 31u);
      tmp4 = 24u;
      tmp3 = (uint32_t)((int32_t)tmp3 >> (tmp4 & 31u));
      tmp4 = 0u;
      tmp3 = (uint32_t)((int32_t)tmp3 < (int32_t)tmp4);
      carry = tmp3;
      tmp = (tmp3 != 0u) ? tmp : tmp2;
      key_data = tmp;
      tmp = LoadU32(ctx, key + 4u);
      tmp2 = link;
      tmp3 = carry;
      tmp = (tmp3 != 0u) ? tmp : tmp2;
      carry = tmp;
      link = bucket + 4u;
      for (;;) {
        tmp = LoadU8(ctx, node + 27u);
        key = tmp;
        tmp = key << (24u & 31u);
        tmp2 = 24u;
        tmp = (uint32_t)((int32_t)tmp >> (tmp2 & 31u));
        tmp2 = 0u;
        tmp = (uint32_t)((int32_t)tmp < (int32_t)tmp2);
        aux = tmp;
        tmp = carry;
        tmp2 = LoadU32(ctx, node + 20u);
        tmp3 = key;
        tmp2 = (aux != 0u) ? tmp2 : tmp3;
        key = tmp2;
        tmp = (carry > key);
        key_longer = tmp;
        tmp = key;
        tmp2 = carry;
        tmp3 = key_longer;
        tmp = (tmp3 != 0u) ? tmp : tmp2;
        cmp_len = tmp;
        if (cmp_len != 0u) {
          tmp = key_data;
          tmp2 = node + 16u;
          field_addr = tmp2;
          tmp2 = LoadU32(ctx, tmp2);
          tmp3 = field_addr;
          tmp4 = aux;
          tmp2 = (tmp4 != 0u) ? tmp2 : tmp3;
          aux = tmp2;
          tmp3 = cmp_len;
          tmp = CompareBytes(ctx, key_data, aux, tmp3);
          cmp_result = tmp;
          if (tmp == 0u) {
            tmp = (key > carry);
            if (tmp != 0u) {
              goto walk_greater;
            }
            goto compare_tails;
          }
          tmp = (uint32_t)((int32_t)cmp_result >
                           (int32_t)kUnorderedInsertBucketNodeSignedLimit);
          if (tmp != 0u) {
            goto compare_tails;
          }
          goto walk_greater;
        }
        tmp = (key <= carry);
        if (tmp != 0u) {
          goto tail_shorter;
        }
      walk_greater:;
        key = LoadU32(ctx, node);
        if (key != 0u) {
          goto descend_chain;
        }
        goto insert_at_tail;
      compare_tails:;
        tmp = CompareBytes(ctx, aux, key_data, cmp_len);
        key = tmp;
        if (tmp != 0u) {
          goto tail_longer;
        }
      tail_shorter:;
        if (key_longer != 0u) {
          goto advance_bucket;
        }
        goto walk_done;
      tail_longer:;
        tmp = (uint32_t)((int32_t)key > (int32_t)kUnorderedInsertBucketNodeSignedLimit);
        if (tmp != 0u) {
          goto walk_done;
        }
      advance_bucket:;
        link = node + 4u;
        key = LoadU32(ctx, node + 4u);
        aux = (key == 0u);
        if (key == 0u) {
          goto walk_done;
        }
        node = link;
      descend_chain:;
        link = node;
        node = key;
        continue;
      }
      TrapUnreachable();
    }
    node = bucket + 4u;
  insert_at_tail:;
    link = node;
  walk_done:;
    carry = map;
    key = LoadU32(ctx, link);
  }
  tmp = bucket;
  tmp2 = 0u;
  tmp = (ResumePhase(ctx) != 0u) ? tmp : tmp2;
  tmp2 = bucket;
  tmp3 = prev_key;
  tmp4 = key;
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
      new_node = AllocateStringBuffer(ctx, 48u);
      suspend_code = 0u;
      tmp = ResumePhase(ctx);
      if (tmp == kResumeSuspended) {
        goto save_suspend_frame;
      }
      key = new_node;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      value = LoadU32(ctx, value);
      StoreU32(ctx, kUnorderedInsertBucketNodeLastException, 0u);
      aux = key + 16u;
    }
    tmp = (resume_code == kResumeSuspended);
    tmp = (ResumePhase(ctx) != 0u) ? tmp : 1u;
    if (tmp != 0u) {
      HostOsQuery(ctx, 873u, aux, value);
      suspend_code = 1u;
      tmp = ResumePhase(ctx);
      if (tmp == kResumeSuspended) {
        goto save_suspend_frame;
      }
    }
    if (ResumePhase(ctx) != 0u) {
      tmp = bucket;
    } else {
      value = LoadU32(ctx, kUnorderedInsertBucketNodeLastException);
      StoreU32(ctx, kUnorderedInsertBucketNodeLastException, 0u);
      if (value == kResumeSuspended) {
        goto unhandled_exception;
      }
      zero64 = 0ull;
      StoreU64(ctx, key + 36u, zero64);
      StoreU8(ctx, key + 32u, 0u);
      tmp2 = kUnorderedInsertBucketNodeNodeTag;
      StoreU32(ctx, key + 28u, tmp2);
      StoreU32(ctx, key + 8u, node);
      StoreU64(ctx, key, zero64);
      StoreU32(ctx, key + 44u, 0u);
      StoreU32(ctx, link, key);
      value = key;
      tmp = LoadU32(ctx, bucket);
      node = LoadU32(ctx, tmp);
      if (node != 0u) {
        StoreU32(ctx, bucket, node);
        value = LoadU32(ctx, link);
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
    StoreU8(ctx, carry + 4u, bucket);
    StoreU32(ctx, map, key);
    goto frame_stored;
  }
unhandled_exception:;
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = HostRet0(ctx);
    map = tmp;
    HostTempRet0(ctx);
    DeallocateRawBlock(ctx, key);
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
  tmp2 += kUnorderedInsertBucketNodeResumeWordBytes;
  StoreU32(ctx, tmp, tmp2);
  tmp = ReturnFrameSp(ctx);
  tmp = LoadU32(ctx, tmp);
  frame = tmp;
  StoreU32(ctx, frame, map);
  StoreU32(ctx, frame + 4u, bucket);
  StoreU32(ctx, frame + 8u, key);
  StoreU32(ctx, frame + 12u, node);
  StoreU32(ctx, frame + 16u, value);
  StoreU32(ctx, frame + 20u, link);
  StoreU32(ctx, frame + 24u, carry);
  StoreU32(ctx, frame + 28u, aux);
  tmp = ReturnFrameSp(ctx);
  tmp2 = ReturnFrameSp(ctx);
  tmp2 = LoadU32(ctx, tmp2);
  tmp2 += kUnorderedInsertBucketNodeSaveFrameBytes;
  StoreU32(ctx, tmp, tmp2);
frame_stored:;
  LeaveFrame();
}
}  // namespace artc
