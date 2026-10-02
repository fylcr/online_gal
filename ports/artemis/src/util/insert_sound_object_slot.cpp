// insert_sound_object_slot.cpp -- allocates one unordered node, constructs the sound-object value in place and links it into the bucket chain..
//
// InsertSoundObjectSlot -- allocates one unordered node, constructs the sound-object value in place and links it into the bucket chain.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 444737-444737 (f1058); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
uint32_t DestroySoundObject(EngineContext* ctx, uint32_t a);

namespace {

constexpr uint32_t kF1058Const90E80 = 0x90E80u;  // 
constexpr uint32_t kF1058MsgBuckets = 0x90EFCu;  // buckets_
constexpr uint32_t kF1058MsgBoostBoostUnorderedDetail = 0x90F05u;  // ../../../../boost_1_66_0/boost/unordered/detail/implementation.hpp
constexpr uint32_t kF1058MsgGetBucket = 0x90F48u;  // get_bucket
constexpr uint32_t kF1058ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF1058Const7FFFFFFF = 0x7FFFFFFFu;  // 
constexpr uint32_t kF1058SaveFrameBytes = 28u;
constexpr uint32_t kF1058ResumeWordBytes = 4u;

}  // namespace

void InsertSoundObjectSlot(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1058SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 24u);
    l7 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 4u);
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1058ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l8 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i1 = b;
    i1 = LoadU32(ctx, i1 + 4u);
    i0 = REM_U(i0, i1);
    l6 = i0;
    i0 = b;
    i0 = LoadU32(ctx, i0 + 8u);
    l7 = i0;
    i0 = !(i0);
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    if (i0) {goto block_6;}
    i0 = b;
    i0 = LoadU32(ctx, i0 + 20u);
    c = i0;
    if (i0) {
      i0 = l6;
      i1 = 2u;
      i0 <<= (i1 & 31);
      i1 = c;
      i0 += i1;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = !(i0);
      l3 = i0;
      if (i0) {goto block_6;}
      i0 = c;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = !(i0);
      l3 = i0;
      if (i0) {goto block_6;}
      i0 = l5;
      i1 = c;
      i1 = LoadU32(ctx, i1 + 8u);
      i0 = i0 != i1;
      if (i0) {
        i0 = c;
        i0 = LoadU32(ctx, i0 + 4u);
        l3 = i0;
        loop_9: 
          i0 = l6;
          i1 = l3;
          i2 = kF1058Const7FFFFFFF;
          i1 &= i2;
          i0 = i0 != i1;
          l3 = i0;
          if (i0) {goto block_6;}
          loop_10: 
            i0 = c;
            i0 = LoadU32(ctx, i0);
            c = i0;
            i0 = !(i0);
            l3 = i0;
            if (i0) {goto block_6;}
            i0 = c;
            i0 = LoadU32(ctx, i0 + 4u);
            l3 = i0;
            i1 = 0u;
            i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
            if (i0) {goto loop_10;}
          i0 = c;
          i0 = LoadU32(ctx, i0 + 8u);
          i1 = l5;
          i0 = i0 != i1;
          if (i0) {goto loop_9;}
      }
      i0 = a;
      i1 = c;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 0u;
      StoreU8(ctx, i0 + 4u, i1);
      goto func_exit;
    }
    goto running_exit;
    block_6:;
  }
  i0 = l8;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 52u;
    i0 = AllocateStringBuffer(ctx, i0);
    l4 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    j1 = 0ull;
    StoreU64(ctx, i0 + 32u, j1);
    i0 = c;
    i1 = 0u;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = c;
    j1 = 0ull;
    StoreU64(ctx, i0 + 16u, j1);
    i0 = c;
    i1 = kF1058Const90E80;
    StoreU32(ctx, i0 + 12u, i1);
    i0 = c;
    i1 = l5;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = c;
    j1 = 0ull;
    StoreU64(ctx, i0, j1);
    i0 = c;
    j1 = 0ull;
    StoreU64(ctx, i0 + 44u, j1);
    i0 = c;
    i1 = c;
    i2 = 32u;
    i1 += i2;
    StoreU32(ctx, i0 + 28u, i1);
    i0 = c;
    i1 = c;
    i2 = 44u;
    i1 += i2;
    StoreU32(ctx, i0 + 40u, i1);
    i0 = kF1058ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l7;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l8;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2141u;
    i1 = b;
    i2 = l3;
    HostOsSchedule(ctx, i0, i1, i2);
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1058ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF1058ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 != i1;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    if (i0) {
      i0 = l5;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 4u);
      l7 = i1;
      i0 = REM_U(i0, i1);
      l3 = i0;
      i0 = b;
      i0 = LoadU32(ctx, i0 + 20u);
      l5 = i0;
      i0 = !(i0);
      if (i0) {goto block_15;}
      i0 = c;
      i1 = l3;
      i2 = kF1058Const7FFFFFFF;
      i1 &= i2;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l5;
      i1 = l3;
      i2 = 2u;
      i1 <<= (i2 & 31);
      i0 += i1;
      l6 = i0;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = l5;
      i1 = l7;
      i2 = 2u;
      i1 <<= (i2 & 31);
      i0 += i1;
      l3 = i0;
      i0 = LoadU32(ctx, i0);
      l7 = i0;
      if (i0) {
        i0 = l7;
        i0 = LoadU32(ctx, i0 + 4u);
        i1 = 2u;
        i0 <<= (i1 & 31);
        i1 = l5;
        i0 += i1;
        i1 = c;
        StoreU32(ctx, i0, i1);
      }
      i0 = l6;
      i1 = l3;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = l3;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0, i1);
      goto block_16;
    }
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = c;
    i1 = 12u;
    i0 += i1;
    b = i0;
  }
  i0 = l8;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i0 = DestroySoundObject(ctx, i0);
    l4 = i0;
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    DeallocateRawBlock(ctx, i0);
    i0 = a;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_17:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i1 = l3;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0, i1);
    i0 = l6;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
  }
  block_16:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = c;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = b;
    i1 = LoadU32(ctx, i1 + 8u);
    i2 = 1u;
    i1 += i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = a;
    i1 = c;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    StoreU8(ctx, i0 + 4u, i1);
    goto func_exit;
  }
  block_15:;
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1058MsgBuckets;
    i1 = kF1058MsgBoostBoostUnorderedDetail;
    i2 = 2956u;
    i3 = kF1058MsgGetBucket;
    HostAssertFail(ctx, i0, i1, i2, i3);
    TrapUnreachable();
  }
  goto func_exit;
  spill_frame:;
  l4 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l4;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1058ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l4;
  i1 = c;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l4;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l4;
  i1 = l5;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l4;
  i1 = l6;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l4;
  i1 = l7;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 28u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
