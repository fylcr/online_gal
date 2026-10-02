// find_unordered_value_slot.cpp -- hash table read: buckets the given hash, walks the bucket chain past the marked nodes and returns the stored value slot..
//
// FindUnorderedValueSlot -- hash table read: buckets the given hash, walks the bucket chain past the marked nodes and returns the stored value slot.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 327606-327606 (f850); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void Panic(EngineContext* ctx, uint32_t a0);

namespace {

constexpr uint32_t kF850MsgBuckets = 0xB5B6Cu;  // buckets_
constexpr uint32_t kF850MsgBoostBoostUnorderedDetail = 0xB5B75u;  // ../../../../boost_1_66_0/boost/unordered/detail/implementation.hpp
constexpr uint32_t kF850MsgGetBucket = 0xB5BB8u;  // get_bucket
constexpr uint32_t kF850MsgPx = 0xB5E31u;  // px != 0
constexpr uint32_t kF850MsgBoostBoostSmartPtr = 0xB5E39u;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
constexpr uint32_t kF850MsgOperator = 0xB5E71u;  // operator->
constexpr uint32_t kF850ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF850Const7FFFFFFF = 0x7FFFFFFFu;  // 
constexpr uint32_t kF850SaveFrameBytes = 12u;
constexpr uint32_t kF850ResumeWordBytes = 4u;

}  // namespace

uint32_t FindUnorderedValueSlot(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF850SaveFrameBytes;
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
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF850ResumeWordBytes;
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
    a = i0;
    i0 = !(i0);
    if (i0) {goto block_5;}
    i0 = l5;
    i1 = 2u;
    i0 <<= (i1 & 31);
    i1 = a;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = !(i0);
    if (i0) {goto block_6;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = !(i0);
    if (i0) {goto block_6;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = b;
    i0 = i0 != i1;
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 4u);
      l2 = i0;
      loop_10: 
        i0 = l5;
        i1 = l2;
        i2 = kF850Const7FFFFFFF;
        i1 &= i2;
        i0 = i0 != i1;
        if (i0) {goto block_6;}
        loop_11: 
          i0 = a;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = !(i0);
          if (i0) {goto block_6;}
          i0 = a;
          i0 = LoadU32(ctx, i0 + 4u);
          l2 = i0;
          i1 = 0u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          if (i0) {goto loop_11;}
        i0 = a;
        i0 = LoadU32(ctx, i0 + 8u);
        i1 = b;
        i0 = i0 != i1;
        if (i0) {goto loop_10;}
    }
    i0 = a;
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
      i0 = kF850ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF850MsgPx;
      i2 = kF850MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF850MsgOperator;
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
      i0 = kF850ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF850ConstD015C;
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
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 28u);
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
    i1 = b;
    i0 = CallTableT2(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    l2 = i0;
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l3 = i0;
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    goto func_exit;
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF850MsgBuckets;
    i1 = kF850MsgBoostBoostUnorderedDetail;
    i2 = 2956u;
    i3 = kF850MsgGetBucket;
    HostAssertFail(ctx, i0, i1, i2, i3);
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    TrapUnreachable();
  }
  TrapUnreachable();
  spill_frame:;
  l2 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF850ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l2 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l2;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l2;
  i1 = l3;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 12u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
