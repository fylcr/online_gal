// clear_unordered_buckets.cpp -- clears a boost unordered bucket array: the bucket range is set back to the empty bucket through FillBytes and the old storage goes back to the pool..
//
// ClearUnorderedBuckets -- clears a boost unordered bucket array: the bucket range is set back to the empty bucket through FillBytes and the old storage goes back to the pool.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 460152-460152 (f1072); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void Panic(EngineContext* ctx, uint32_t a0);
uint32_t FillBytes(EngineContext* ctx, uint32_t dst, uint32_t value, uint32_t length);

namespace {

constexpr uint32_t kF1072MsgBuckets = 0x8FAFCu;  // buckets_
constexpr uint32_t kF1072MsgBoostBoostUnorderedDetail = 0x8FB05u;  // ../../../../boost_1_66_0/boost/unordered/detail/implementation.hpp
constexpr uint32_t kF1072MsgGetBucket = 0x8FB48u;  // get_bucket
constexpr uint32_t kF1072ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF1072ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF1072SaveFrameBytes = 8u;
constexpr uint32_t kF1072ResumeWordBytes = 4u;

}  // namespace

void ClearUnorderedBuckets(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1072SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l1 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 4u);
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1072ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 8u);
    i0 = !(i0);
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  if (i0) {
    i0 = l3;
  } else {
    i0 = l1;
    if (i0) {goto block_5;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 20u);
    l1 = i0;
    i0 = !(i0);
  }
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1072ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l2;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF1072MsgBuckets;
      i2 = kF1072MsgBoostBoostUnorderedDetail;
      i3 = 2956u;
      i4 = kF1072MsgGetBucket;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1072ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF1072ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto running_exit;}
      TrapUnreachable();
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 4u);
    l3 = i0;
    i1 = 2u;
    i0 <<= (i1 & 31);
    l5 = i0;
    i1 = l1;
    i0 += i1;
    l4 = i0;
    i0 = l3;
    if (i0) {
      i0 = l1;
      i1 = 0u;
      i2 = l5;
      i0 = FillBytes(ctx, i0, i1, i2);
    }
    i0 = l4;
    i0 = LoadU32(ctx, i0);
    l1 = i0;
    i0 = l4;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 0u;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l1;
    i0 = !(i0);
    a = i0;
    if (i0) {goto block_5;}
    loop_13: 
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = l1;
      i0 = LoadS8(ctx, i0 + 19u);
      i1 = kF1072ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l1;
        i0 = LoadU32(ctx, i0 + 8u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l1;
      DeallocateRawBlock(ctx, i0);
      i0 = a;
      l1 = i0;
      if (i0) {goto loop_13;}
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    goto func_exit;
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    a = i0;
    i0 = HostTempRet0(ctx);
  }
  i0 = l2;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    TrapUnreachable();
  }
  goto func_exit;
  spill_frame:;
  l2 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1072ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l2 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l2;
  i1 = l1;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 8u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
