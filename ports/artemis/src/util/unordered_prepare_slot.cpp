// unordered_prepare_slot.cpp -- prepares an unordered-map slot for insertion, growing the buckets when the load factor demands it..
//
// UnorderedPrepareSlot -- prepares an unordered-map slot for insertion, growing the buckets when the load factor demands it.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 459903-459903 (f1071); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);

namespace {

constexpr uint32_t kF1071MsgBuckets = 0x8FAFCu;  // buckets_
constexpr uint32_t kF1071MsgBoostBoostUnorderedDetail = 0x8FB05u;  // ../../../../boost_1_66_0/boost/unordered/detail/implementation.hpp
constexpr uint32_t kF1071MsgGetBucket = 0x8FB48u;  // get_bucket
constexpr uint32_t kF1071ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF1071Const7FFFFFFF = 0x7FFFFFFFu;  // 
constexpr uint32_t kF1071ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF1071SaveFrameBytes = 16u;
constexpr uint32_t kF1071ResumeWordBytes = 4u;

}  // namespace

uint32_t UnorderedPrepareSlot(EngineContext* ctx, uint32_t map, uint32_t key, uint32_t hash) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1071SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    map = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    key = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    hash = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1071ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = map;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i0 = kF1071ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l4;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1784u;
    i1 = map;
    i2 = l3;
    HostOsSchedule(ctx, i0, i1, i2);
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1071ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF1071ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {
      i0 = hash;
      i1 = map;
      i1 = LoadU32(ctx, i1 + 4u);
      l4 = i1;
      i0 = REM_U(i0, i1);
      l3 = i0;
      i0 = map;
      i0 = LoadU32(ctx, i0 + 20u);
      hash = i0;
      i0 = !(i0);
      if (i0) {goto block_6;}
      i0 = key;
      i1 = l3;
      i2 = kF1071Const7FFFFFFF;
      i1 &= i2;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l3;
      i1 = 2u;
      i0 <<= (i1 & 31);
      i1 = hash;
      i0 += i1;
      l5 = i0;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i1 = 2u;
        i0 <<= (i1 & 31);
        i1 = hash;
        i0 += i1;
        l3 = i0;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 4u);
          i1 = 2u;
          i0 <<= (i1 & 31);
          i1 = hash;
          i0 += i1;
          i1 = key;
          StoreU32(ctx, i0, i1);
        }
        i0 = l5;
        i1 = l3;
        StoreU32(ctx, i0, i1);
        i0 = key;
        i1 = l3;
        i1 = LoadU32(ctx, i1);
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = key;
        StoreU32(ctx, i0, i1);
        goto block_8;
      }
      i0 = key;
      i1 = l3;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i0 = LoadU32(ctx, i0);
      i1 = key;
      StoreU32(ctx, i0, i1);
      block_8:;
      i0 = map;
      i1 = map;
      i1 = LoadU32(ctx, i1 + 8u);
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = key;
      goto func_exit;
    }
    i0 = HostRet0(ctx);
    map = i0;
    i0 = HostTempRet0(ctx);
    i0 = key;
    if (i0) {
      i0 = key;
      i0 = LoadS8(ctx, i0 + 31u);
      i1 = kF1071ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = key;
        i0 = LoadU32(ctx, i0 + 20u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = key;
      i0 = LoadS8(ctx, i0 + 19u);
      i1 = kF1071ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = key;
        i0 = LoadU32(ctx, i0 + 8u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = key;
      DeallocateRawBlock(ctx, i0);
    }
    i0 = map;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
    block_6:;
    i0 = kF1071MsgBuckets;
    i1 = kF1071MsgBoostBoostUnorderedDetail;
    i2 = 2956u;
    i3 = kF1071MsgGetBucket;
    HostAssertFail(ctx, i0, i1, i2, i3);
    TrapUnreachable();
  }
  TrapUnreachable();
  spill_frame:;
  l4 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l4;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1071ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  i1 = map;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = key;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l4;
  i1 = hash;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l4;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 16u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
