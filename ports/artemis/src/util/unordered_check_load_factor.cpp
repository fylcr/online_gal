#include "util/engine_context.h"

namespace artc {

// Forward declarations of callees defined elsewhere in the tree.
void UnorderedUpdateLoadFactor(EngineContext* ctx, uint32_t a0, uint32_t a1);
void UnorderedRehash(EngineContext* ctx, uint32_t a0, uint32_t a1);

namespace {

constexpr uint32_t kUnorderedCheckLoadFactorBucketArray = 0x8ADC0u;
constexpr uint32_t kUnorderedCheckLoadFactorFallbackBuckets = 0x8AE54u;
constexpr uint32_t kUnorderedCheckLoadFactorMsgBuckets = 0x8AE58u;  // buckets_
constexpr uint32_t kUnorderedCheckLoadFactorMsgBoostDetail = 0x8FB05u;
constexpr uint32_t kUnorderedCheckLoadFactorMsgMinimumFactor = 0x8FB97u;
constexpr uint32_t kUnorderedCheckLoadFactorMsgMinBuckets = 0x8FBB7u;
constexpr uint32_t kUnorderedCheckLoadFactorSignedLimit = 0xFFFFFFFFu;
constexpr uint32_t kUnorderedCheckLoadFactorSaveFrameBytes = 12u;
constexpr uint32_t kUnorderedCheckLoadFactorResumeWordBytes = 4u;

}  // namespace

// Guards an unordered map's load factor: works out the bucket count the map
// needs, rehashes or rebuilds the bucket array when it falls short, and moves
// the load-factor counters up once the map has grown.
void UnorderedCheckLoadFactor(EngineContext* ctx, uint32_t map, uint32_t count) {
  uint32_t carry = 0u;         // bucket target carried across a suspension
  uint32_t frame = 0u;         // frame the suspended registers were saved in
  uint32_t resume_code = 0u;   // code of the suspension being resumed
  uint32_t suspend_code = 0u;  // code stored when this call suspends
  uint32_t shift = 0u;         // bit index the bucket search walks from
  uint32_t probe = 0u;         // bucket probe, then the probe's comparison
  uint32_t probe_shift = 0u;   // halved bit index of the bucket probe
  uint32_t sample = 0u;        // bucket count sampled from the map
  uint32_t grew = 0u;          // the rebuilt table is larger than the old one
  uint32_t tmp = 0u;
  uint32_t tmp2 = 0u;
  uint32_t tmp3 = 0u;
  uint32_t tmp4 = 0u;
  float factor = 0;
  float operand = 0;
  float max_load = 0;
  double acc = 0;
  double bound = 0;
  double lower = 0;
  double needed_f = 0;
  EnterFrame();
  if (ResumePhase(ctx) == kResumeRewinding) {
    tmp = ReturnFrameSp(ctx);
    tmp2 = ReturnFrameSp(ctx);
    tmp2 = LoadU32(ctx, tmp2);
    tmp2 -= kUnorderedCheckLoadFactorSaveFrameBytes;
    StoreU32(ctx, tmp, tmp2);
    tmp = ReturnFrameSp(ctx);
    tmp = LoadU32(ctx, tmp);
    frame = tmp;
    map = LoadU32(ctx, frame);
    tmp = LoadU32(ctx, frame + 4u);
    count = tmp;
    tmp = LoadU32(ctx, frame + 8u);
    carry = tmp;
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    tmp = ReturnFrameSp(ctx);
    tmp2 = ReturnFrameSp(ctx);
    tmp2 = LoadU32(ctx, tmp2);
    tmp2 -= kUnorderedCheckLoadFactorResumeWordBytes;
    StoreU32(ctx, tmp, tmp2);
    tmp = ReturnFrameSp(ctx);
    tmp = LoadU32(ctx, tmp);
    resume_code = LoadU32(ctx, tmp);
  }
  if (ResumePhase(ctx) == 0u) {
    tmp = LoadU32(ctx, map + 20u);
    carry = (tmp == 0u);
  }
  if ((carry | (ResumePhase(ctx) == kResumeRewinding)) != 0u) {
    if (ResumePhase(ctx) == 0u) {
      factor = LoadF32(ctx, map + 12u);
      max_load = factor;
      operand = 0.00100000005f;
      tmp = (factor >= operand);
      tmp ^= 1u;
      if (tmp != 0u) {
        goto minimum_load;
      }
      acc = (double)(count);
      operand = max_load;
      bound = (double)wasm_quietf(operand);
      acc /= bound;
      acc = wasm_floor(acc);
      bound = 1.0;
      acc += bound;
      bound = 4294967295.0;
      acc = FMIN(acc, bound);
      needed_f = acc;
      bound = 4294967296.0;
      tmp = (acc < bound);
      bound = needed_f;
      lower = 0.0;
      tmp2 = (bound >= lower);
      tmp &= tmp2;
      tmp3 = 0u;
      if (tmp != 0u) {
        acc = needed_f;
        tmp3 = I32_TRUNC_U_F64(acc);
      }
      carry = tmp3;
      shift = 38u;
      count = kUnorderedCheckLoadFactorBucketArray;
      for (;;) {
        tmp = shift >> (1u & 31u);
        probe_shift = tmp;
        tmp = probe_shift << (2u & 31u);
        tmp += count;
        probe = tmp;
        tmp += 4u;
        tmp2 = count;
        tmp3 = LoadU32(ctx, probe);
        tmp3 = (tmp3 < carry);
        probe = tmp3;
        tmp = (tmp3 != 0u) ? tmp : tmp2;
        count = tmp;
        tmp = probe_shift;
        tmp2 = kUnorderedCheckLoadFactorSignedLimit;
        tmp ^= tmp2;
        tmp2 = shift;
        tmp += tmp2;
        tmp2 = probe_shift;
        tmp3 = probe;
        tmp = (tmp3 != 0u) ? tmp : tmp2;
        shift = tmp;
        if (tmp != 0u) {
          continue;
        }
        break;
      }
      carry = map;
      map = LoadU32(ctx, map + 4u);
      tmp = kUnorderedCheckLoadFactorFallbackBuckets;
      tmp2 = count;
      tmp3 = count;
      tmp4 = kUnorderedCheckLoadFactorMsgBuckets;
      tmp3 = (tmp3 == tmp4);
      tmp = (tmp3 != 0u) ? tmp : tmp2;
      tmp = LoadU32(ctx, tmp);
      count = tmp;
      tmp = (map < count);
      grew = tmp;
      tmp = count;
      tmp2 = map;
      tmp3 = grew;
      tmp = (tmp3 != 0u) ? tmp : tmp2;
      map = tmp;
    }
    tmp = (resume_code == 0u);
    tmp = (ResumePhase(ctx) != 0u) ? tmp : 1u;
    if (tmp != 0u) {
      UnorderedRehash(ctx, carry, map);
      suspend_code = 0u;
      tmp = ResumePhase(ctx);
      if (tmp == kResumeSuspended) {
        goto save_suspend_frame;
      }
    }
    if (ResumePhase(ctx) == 0u) {
      goto frame_stored;
    }
  }
  if (ResumePhase(ctx) == 0u) {
    tmp = LoadU32(ctx, map + 16u);
    carry = (tmp >= count);
  }
  if (ResumePhase(ctx) == 0u) {
    if (carry != 0u) {
      goto load_factor_needed;
    }
    factor = LoadF32(ctx, map + 12u);
    max_load = factor;
    operand = 0.00100000005f;
    tmp = (factor >= operand);
    tmp ^= 1u;
    if (tmp != 0u) {
      goto minimum_load;
    }
    tmp = LoadU32(ctx, map + 8u);
    sample = tmp;
    tmp = sample >> (1u & 31u);
    tmp += sample;
    sample = tmp;
    tmp = (sample > count);
    carry = tmp;
    tmp = sample;
    tmp2 = count;
    tmp3 = carry;
    tmp = (tmp3 != 0u) ? tmp : tmp2;
    acc = (double)(tmp);
    operand = max_load;
    bound = (double)wasm_quietf(operand);
    acc /= bound;
    acc = wasm_floor(acc);
    bound = 1.0;
    acc += bound;
    bound = 4294967295.0;
    acc = FMIN(acc, bound);
    needed_f = acc;
    bound = 4294967296.0;
    tmp = (acc < bound);
    bound = needed_f;
    lower = 0.0;
    tmp2 = (bound >= lower);
    tmp &= tmp2;
    tmp3 = 0u;
    if (tmp != 0u) {
      acc = needed_f;
      tmp3 = I32_TRUNC_U_F64(acc);
    }
    carry = tmp3;
    shift = 38u;
    count = kUnorderedCheckLoadFactorBucketArray;
    for (;;) {
      tmp = shift >> (1u & 31u);
      probe_shift = tmp;
      tmp = probe_shift << (2u & 31u);
      tmp += count;
      probe = tmp;
      tmp += 4u;
      tmp2 = count;
      tmp3 = LoadU32(ctx, probe);
      tmp3 = (tmp3 < carry);
      probe = tmp3;
      tmp = (tmp3 != 0u) ? tmp : tmp2;
      count = tmp;
      tmp = probe_shift;
      tmp2 = kUnorderedCheckLoadFactorSignedLimit;
      tmp ^= tmp2;
      tmp2 = shift;
      tmp += tmp2;
      tmp2 = probe_shift;
      tmp3 = probe;
      tmp = (tmp3 != 0u) ? tmp : tmp2;
      shift = tmp;
      if (tmp != 0u) {
        continue;
      }
      break;
    }
    tmp = kUnorderedCheckLoadFactorFallbackBuckets;
    tmp2 = count;
    tmp3 = count;
    tmp4 = kUnorderedCheckLoadFactorMsgBuckets;
    tmp3 = (tmp3 == tmp4);
    tmp = (tmp3 != 0u) ? tmp : tmp2;
    tmp = LoadU32(ctx, tmp);
    count = tmp;
    tmp2 = LoadU32(ctx, map + 4u);
    if (count == tmp2) {
      goto load_factor_needed;
    }
  }
  tmp = (resume_code == 1u);
  tmp = (ResumePhase(ctx) != 0u) ? tmp : 1u;
  if (tmp != 0u) {
    UnorderedUpdateLoadFactor(ctx, map, count);
    suspend_code = 1u;
    tmp = ResumePhase(ctx);
    if (tmp == kResumeSuspended) {
      goto save_suspend_frame;
    }
  }
load_factor_needed:;
  if (ResumePhase(ctx) == 0u) {
    goto frame_stored;
  }
minimum_load:;
  if (ResumePhase(ctx) == 0u) {
    HostAssertFail(ctx, kUnorderedCheckLoadFactorMsgMinimumFactor,
                   kUnorderedCheckLoadFactorMsgBoostDetail, 3027u,
                   kUnorderedCheckLoadFactorMsgMinBuckets);
    TrapUnreachable();
  }
  goto frame_stored;
save_suspend_frame:;
  resume_code = suspend_code;
  tmp = ReturnFrameSp(ctx);
  tmp = LoadU32(ctx, tmp);
  StoreU32(ctx, tmp, resume_code);
  tmp = ReturnFrameSp(ctx);
  tmp2 = ReturnFrameSp(ctx);
  tmp2 = LoadU32(ctx, tmp2);
  tmp2 += kUnorderedCheckLoadFactorResumeWordBytes;
  StoreU32(ctx, tmp, tmp2);
  tmp = ReturnFrameSp(ctx);
  tmp = LoadU32(ctx, tmp);
  frame = tmp;
  StoreU32(ctx, frame, map);
  StoreU32(ctx, frame + 4u, count);
  StoreU32(ctx, frame + 8u, carry);
  tmp = ReturnFrameSp(ctx);
  tmp2 = ReturnFrameSp(ctx);
  tmp2 = LoadU32(ctx, tmp2);
  tmp2 += kUnorderedCheckLoadFactorSaveFrameBytes;
  StoreU32(ctx, tmp, tmp2);
frame_stored:;
  LeaveFrame();
}
}  // namespace artc
