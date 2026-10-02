// reset_global_slot_table.cpp -- resets the global slot table: the table is rebuilt empty and its slot count is reset..
//
// ResetGlobalSlotTable -- resets the global slot table: the table is rebuilt empty and its slot count is reset.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 292924-292924 (f761); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF761MsgBuckets = 0x8FAFCu;  // buckets_
constexpr uint32_t kF761MsgBoostBoostUnorderedDetail = 0x8FB05u;  // ../../../../boost_1_66_0/boost/unordered/detail/implementation.hpp
constexpr uint32_t kF761MsgGetBucket = 0x8FB48u;  // get_bucket
constexpr uint32_t kF761ConstCF134 = 0xCF134u;  // 
constexpr uint32_t kF761ConstCF138 = 0xCF138u;  // 
constexpr uint32_t kF761ConstCF144 = 0xCF144u;  // 
constexpr uint32_t kF761ConstCF150 = 0xCF150u;  // 
constexpr uint32_t kF761ConstCF154 = 0xCF154u;  // 
constexpr uint32_t kF761ConstCF160 = 0xCF160u;  // 
constexpr uint32_t kF761Const7FFFFFFF = 0x7FFFFFFFu;  // 
constexpr uint32_t kF761SaveFrameBytes = 12u;
constexpr uint32_t kF761ResumeWordBytes = 4u;

}  // namespace

void ResetGlobalSlotTable(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF761SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 4u);
    l1 = i0;
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
    i2 = kF761ResumeWordBytes;
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
    i0 = a;
    i1 = kF761ConstCF134;
    i1 = LoadU32(ctx, i1);
    i0 = REM_U(i0, i1);
    l2 = i0;
    i0 = kF761ConstCF138;
    i0 = LoadU32(ctx, i0);
    i0 = !(i0);
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    if (i0) {goto block_5;}
    i0 = kF761ConstCF144;
    i0 = LoadU32(ctx, i0);
    l1 = i0;
    i0 = !(i0);
    if (i0) {goto running_exit;}
    i0 = l2;
    i1 = 2u;
    i0 <<= (i1 & 31);
    i1 = l1;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l1 = i0;
    i0 = !(i0);
    if (i0) {goto block_5;}
    i0 = l1;
    i0 = LoadU32(ctx, i0);
    l1 = i0;
    i0 = !(i0);
    if (i0) {goto block_5;}
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = a;
    i0 = i0 != i1;
    if (i0) {
      i0 = l1;
      i0 = LoadU32(ctx, i0 + 4u);
      l3 = i0;
      dispatch_loop: 
        i0 = l2;
        i1 = l3;
        i2 = kF761Const7FFFFFFF;
        i1 &= i2;
        i0 = i0 != i1;
        if (i0) {goto block_5;}
        loop_9: 
          i0 = l1;
          i0 = LoadU32(ctx, i0);
          l1 = i0;
          i0 = !(i0);
          if (i0) {goto block_5;}
          i0 = l1;
          i0 = LoadU32(ctx, i0 + 4u);
          l3 = i0;
          i1 = 0u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          if (i0) {goto loop_9;}
        i0 = l1;
        i0 = LoadU32(ctx, i0 + 8u);
        i1 = a;
        i0 = i0 != i1;
        if (i0) {goto dispatch_loop;}
    }
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 12u);
    l1 = i0;
    i0 = !(i0);
    if (i0) {goto block_5;}
    i0 = l1;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 16u);
    a = i0;
  }
  i0 = l4;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l1;
    i1 = a;
    CallTableT1(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
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
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = kF761ConstCF150;
    i1 = LoadU32(ctx, i1);
    i0 = REM_U(i0, i1);
    l2 = i0;
    i0 = kF761ConstCF154;
    i0 = LoadU32(ctx, i0);
    i0 = !(i0);
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    if (i0) {goto block_14;}
    i0 = kF761ConstCF160;
    i0 = LoadU32(ctx, i0);
    l1 = i0;
    i0 = !(i0);
    if (i0) {goto block_13;}
    i0 = l2;
    i1 = 2u;
    i0 <<= (i1 & 31);
    i1 = l1;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l1 = i0;
    i0 = !(i0);
    if (i0) {goto block_14;}
    i0 = l1;
    i0 = LoadU32(ctx, i0);
    l1 = i0;
    i0 = !(i0);
    if (i0) {goto block_14;}
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = a;
    i0 = i0 != i1;
    if (i0) {
      i0 = l1;
      i0 = LoadU32(ctx, i0 + 4u);
      l3 = i0;
      loop_17: 
        i0 = l2;
        i1 = l3;
        i2 = kF761Const7FFFFFFF;
        i1 &= i2;
        i0 = i0 != i1;
        if (i0) {goto block_14;}
        loop_18: 
          i0 = l1;
          i0 = LoadU32(ctx, i0);
          l1 = i0;
          i0 = !(i0);
          if (i0) {goto block_14;}
          i0 = l1;
          i0 = LoadU32(ctx, i0 + 4u);
          l3 = i0;
          i1 = 0u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          if (i0) {goto loop_18;}
        i0 = l1;
        i0 = LoadU32(ctx, i0 + 8u);
        i1 = a;
        i0 = i0 != i1;
        if (i0) {goto loop_17;}
    }
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 12u);
    a = i0;
    i0 = !(i0);
    if (i0) {goto block_14;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 16u);
    l1 = i0;
  }
  i0 = l4;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = l1;
    CallTableT1(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
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
  block_13:;
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF761MsgBuckets;
    i1 = kF761MsgBoostBoostUnorderedDetail;
    i2 = 2956u;
    i3 = kF761MsgGetBucket;
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
  i2 = kF761ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = l1;
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
