// find_or_insert_sound_object_slot.cpp -- hashes the key into the sound table, emplaces the value on a miss and asserts the shared pointer reachable through the node..
//
// FindOrInsertSoundObjectSlot -- hashes the key into the sound table, emplaces the value on a miss and asserts the shared pointer reachable through the node.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 632567-632567 (f1469); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void InsertSoundObjectSlot(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
void Panic(EngineContext* ctx, uint32_t a0);

namespace {

constexpr uint32_t kF1469MsgBuckets = 0xB5B6Cu;  // buckets_
constexpr uint32_t kF1469MsgBoostBoostUnorderedDetail = 0xB5B75u;  // ../../../../boost_1_66_0/boost/unordered/detail/implementation.hpp
constexpr uint32_t kF1469MsgGetBucket = 0xB5BB8u;  // get_bucket
constexpr uint32_t kF1469MsgPx = 0xB5E31u;  // px != 0
constexpr uint32_t kF1469MsgBoostBoostSmartPtr = 0xB5E39u;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
constexpr uint32_t kF1469MsgOperator = 0xB5E71u;  // operator->
constexpr uint32_t kF1469ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF1469Const7FFFFFFF = 0x7FFFFFFFu;  // 
constexpr uint32_t kF1469SaveFrameBytes = 16u;
constexpr uint32_t kF1469ResumeWordBytes = 4u;

}  // namespace

void FindOrInsertSoundObjectSlot(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1469SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    b = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
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
    i2 = kF1469ResumeWordBytes;
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
    i0 = GuestStackSp(ctx);
    i1 = 16u;
    i0 -= i1;
    l3 = i0;
    GuestStackSp(ctx) = i0;
    i0 = l3;
    i1 = b;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = b;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 24u);
    i0 = REM_U(i0, i1);
    l5 = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 28u);
    i0 = !(i0);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {goto block_6;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 40u);
    l2 = i0;
    i0 = !(i0);
    if (i0) {goto block_5;}
    i0 = l5;
    i1 = 2u;
    i0 <<= (i1 & 31);
    i1 = l2;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = !(i0);
    if (i0) {goto block_6;}
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = !(i0);
    if (i0) {goto block_6;}
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = b;
    i0 = i0 != i1;
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 4u);
      l6 = i0;
      loop_9: 
        i0 = l5;
        i1 = l6;
        i2 = kF1469Const7FFFFFFF;
        i1 &= i2;
        i0 = i0 != i1;
        if (i0) {goto block_6;}
        loop_10: 
          i0 = l2;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = !(i0);
          if (i0) {goto block_6;}
          i0 = l2;
          i0 = LoadU32(ctx, i0 + 4u);
          l6 = i0;
          i1 = 0u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          if (i0) {goto loop_10;}
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 8u);
        i1 = b;
        i0 = i0 != i1;
        if (i0) {goto loop_9;}
    }
    i0 = l3;
    i1 = 8u;
    i0 += i1;
    b = i0;
    i0 = l3;
    i1 = 4u;
    i0 += i1;
    l2 = i0;
    i0 = a;
    i1 = 20u;
    i0 += i1;
    a = i0;
  }
  i0 = l4;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = a;
    i2 = l2;
    InsertSoundObjectSlot(ctx, i0, i1, i2);
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    i0 = LoadU32(ctx, i0 + 20u);
    a = i0;
    i0 = !(i0);
    b = i0;
  }
  i0 = b;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1469ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF1469MsgPx;
      i2 = kF1469MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF1469MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1469ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF1469ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      if (i0) {goto running_exit;}
      i0 = 0u;
      i0 = FindMatchingCatch(ctx, i0);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
    }
    i0 = l4;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      Panic(ctx, i0);
      i0 = 2u;
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
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 60u);
    b = i0;
  }
  i0 = l4;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = b;
    CallTableT1(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 16u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1469MsgBuckets;
    i1 = kF1469MsgBoostBoostUnorderedDetail;
    i2 = 2956u;
    i3 = kF1469MsgGetBucket;
    HostAssertFail(ctx, i0, i1, i2, i3);
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
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
  i2 = kF1469ResumeWordBytes;
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
  i1 = l2;
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
  func_exit:;
  LeaveFrame();
}
}
