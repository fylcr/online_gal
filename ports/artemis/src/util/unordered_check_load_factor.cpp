// unordered_check_load_factor.cpp -- the unordered-map load-factor guard: recalculates and rehashes when the map crosses its threshold..
//
// UnorderedCheckLoadFactor -- the unordered-map load-factor guard: recalculates and rehashes when the map crosses its threshold.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 689991-689991 (f1550); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void UnorderedUpdateLoadFactor(EngineContext* ctx, uint32_t a0, uint32_t a1);
void UnorderedRehash(EngineContext* ctx, uint32_t a0, uint32_t a1);

namespace {

constexpr uint32_t kF1550Const8ADC0 = 0x8ADC0u;  // 
constexpr uint32_t kF1550Const8AE54 = 0x8AE54u;  // 
constexpr uint32_t kF1550MsgBuckets = 0x8AE58u;  // buckets_
constexpr uint32_t kF1550MsgBoostBoostUnorderedDetail = 0x8FB05u;  // ../../../../boost_1_66_0/boost/unordered/detail/implementation.hpp
constexpr uint32_t kF1550MsgMlfMinimumMaxLoad = 0x8FB97u;  // mlf_ >= minimum_max_load_factor
constexpr uint32_t kF1550MsgMinBucketsForSize = 0x8FBB7u;  // min_buckets_for_size
constexpr uint32_t kF1550ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF1550SaveFrameBytes = 12u;
constexpr uint32_t kF1550ResumeWordBytes = 4u;

}  // namespace

void UnorderedCheckLoadFactor(EngineContext* ctx, uint32_t map, uint32_t count) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; float l7 = 0; double l8 = 0;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  float f0 = 0; float f1 = 0;
  double d0 = 0; double d1 = 0; double d2 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1550SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0);
    map = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 4u);
    count = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1550ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = map;
    i0 = LoadU32(ctx, i0 + 20u);
    i0 = !(i0);
    l2 = i0;
  }
  i0 = l2;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = map;
      f0 = LoadF32(ctx, i0 + 12u);
      l7 = f0;
      f1 = 0.00100000005f;
      i0 = f0 >= f1;
      i0 = !(i0);
      if (i0) {goto block_3;}
      i0 = count;
      d0 = (double)(i0);
      f1 = l7;
      d1 = (double)wasm_quietf(f1);
      d0 /= d1;
      d0 = wasm_floor(d0);
      d1 = 1.0;
      d0 += d1;
      d1 = 4294967295.0;
      d0 = FMIN(d0, d1);
      l8 = d0;
      d1 = 4294967296.0;
      i0 = d0 < d1;
      d1 = l8;
      d2 = 0.0;
      i1 = d1 >= d2;
      i0 &= i1;
      if (i0) {
        d0 = l8;
        i0 = I32_TRUNC_U_F64(d0);
        goto block_8;
      }
      i0 = 0u;
      block_8:;
      l2 = i0;
      i0 = 38u;
      l3 = i0;
      i0 = kF1550Const8ADC0;
      count = i0;
      loop_10: 
        i0 = l3;
        i1 = 1u;
        i0 >>= (i1 & 31);
        l5 = i0;
        i1 = 2u;
        i0 <<= (i1 & 31);
        i1 = count;
        i0 += i1;
        l4 = i0;
        i1 = 4u;
        i0 += i1;
        i1 = count;
        i2 = l4;
        i2 = LoadU32(ctx, i2);
        i3 = l2;
        i2 = i2 < i3;
        l4 = i2;
        i0 = i2 ? i0 : i1;
        count = i0;
        i0 = l5;
        i1 = kF1550ConstFFFFFFFF;
        i0 ^= i1;
        i1 = l3;
        i0 += i1;
        i1 = l5;
        i2 = l4;
        i0 = i2 ? i0 : i1;
        l3 = i0;
        if (i0) {goto loop_10;}
      i0 = map;
      l2 = i0;
      i0 = LoadU32(ctx, i0 + 4u);
      map = i0;
      i1 = kF1550Const8AE54;
      i2 = count;
      i3 = count;
      i4 = kF1550MsgBuckets;
      i3 = i3 == i4;
      i1 = i3 ? i1 : i2;
      i1 = LoadU32(ctx, i1);
      count = i1;
      i0 = i0 < i1;
      l3 = i0;
      i0 = count;
      i1 = map;
      i2 = l3;
      i0 = i2 ? i0 : i1;
      map = i0;
    }
    i0 = l6;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = map;
      UnorderedRehash(ctx, i0, i1);
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      goto func_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = map;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = count;
    i0 = i0 >= i1;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {goto block_14;}
    i0 = map;
    f0 = LoadF32(ctx, i0 + 12u);
    l7 = f0;
    f1 = 0.00100000005f;
    i0 = f0 >= f1;
    i0 = !(i0);
    if (i0) {goto block_3;}
    i0 = map;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i1 = 1u;
    i0 >>= (i1 & 31);
    i1 = l3;
    i0 += i1;
    l3 = i0;
    i1 = count;
    i0 = i0 > i1;
    l2 = i0;
    i0 = l3;
    i1 = count;
    i2 = l2;
    i0 = i2 ? i0 : i1;
    d0 = (double)(i0);
    f1 = l7;
    d1 = (double)wasm_quietf(f1);
    d0 /= d1;
    d0 = wasm_floor(d0);
    d1 = 1.0;
    d0 += d1;
    d1 = 4294967295.0;
    d0 = FMIN(d0, d1);
    l8 = d0;
    d1 = 4294967296.0;
    i0 = d0 < d1;
    d1 = l8;
    d2 = 0.0;
    i1 = d1 >= d2;
    i0 &= i1;
    if (i0) {
      d0 = l8;
      i0 = I32_TRUNC_U_F64(d0);
      goto block_16;
    }
    i0 = 0u;
    block_16:;
    l2 = i0;
    i0 = 38u;
    l3 = i0;
    i0 = kF1550Const8ADC0;
    count = i0;
    loop_18: 
      i0 = l3;
      i1 = 1u;
      i0 >>= (i1 & 31);
      l5 = i0;
      i1 = 2u;
      i0 <<= (i1 & 31);
      i1 = count;
      i0 += i1;
      l4 = i0;
      i1 = 4u;
      i0 += i1;
      i1 = count;
      i2 = l4;
      i2 = LoadU32(ctx, i2);
      i3 = l2;
      i2 = i2 < i3;
      l4 = i2;
      i0 = i2 ? i0 : i1;
      count = i0;
      i0 = l5;
      i1 = kF1550ConstFFFFFFFF;
      i0 ^= i1;
      i1 = l3;
      i0 += i1;
      i1 = l5;
      i2 = l4;
      i0 = i2 ? i0 : i1;
      l3 = i0;
      if (i0) {goto loop_18;}
    i0 = kF1550Const8AE54;
    i1 = count;
    i2 = count;
    i3 = kF1550MsgBuckets;
    i2 = i2 == i3;
    i0 = i2 ? i0 : i1;
    i0 = LoadU32(ctx, i0);
    count = i0;
    i1 = map;
    i1 = LoadU32(ctx, i1 + 4u);
    i0 = i0 == i1;
    if (i0) {goto block_14;}
  }
  i0 = l6;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = map;
    i1 = count;
    UnorderedUpdateLoadFactor(ctx, i0, i1);
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_14:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    goto func_exit;
  }
  block_3:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1550MsgMlfMinimumMaxLoad;
    i1 = kF1550MsgBoostBoostUnorderedDetail;
    i2 = 3027u;
    i3 = kF1550MsgMinBucketsForSize;
    HostAssertFail(ctx, i0, i1, i2, i3);
    TrapUnreachable();
  }
  goto func_exit;
  spill_frame:;
  l3 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l3;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1550ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = map;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = count;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 12u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
