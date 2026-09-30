// layer_events.cpp -- the layer event registration dispatch: one suspendable step that carries a layer event through the compositor registration table..
//
// Compositor::LyEventDispatch -- the layer event registration dispatch: one suspendable step that carries a layer event through the compositor registration table.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 1871065-1871065 (f3661); R1 transcribe-keep.
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
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t CopyBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void ReleaseNodeTree(EngineContext* ctx, uint32_t state, uint32_t node);
uint32_t CombineComparisonResults(EngineContext* ctx, uint32_t left, uint32_t right, uint32_t result, uint32_t box);
void ReleaseLayerNodeTree(EngineContext* ctx, uint32_t a, uint32_t b);

namespace {

constexpr uint32_t kF3661Const8ADC0 = 0x8ADC0u;  // 
constexpr uint32_t kF3661Const8AE54 = 0x8AE54u;  // 
constexpr uint32_t kF3661MsgBuckets = 0x8AE58u;  // buckets_
constexpr uint32_t kF3661Const8C564 = 0x8C564u;  // 
constexpr uint32_t kF3661MsgReset = 0xAF57Cu;  // reset
constexpr uint32_t kF3661MsgLyevent = 0xAF658u;  // lyevent
constexpr uint32_t kF3661ConstAF65B = 0xAF65Bu;  // 
constexpr uint32_t kF3661ConstB4B29 = 0xB4B29u;  // 
constexpr uint32_t kF3661MsgInit = 0xB4D6Eu;  // init
constexpr uint32_t kF3661MsgClick = 0xB530Eu;  // click
constexpr uint32_t kF3661MsgRollover = 0xB5314u;  // rollover
constexpr uint32_t kF3661MsgRollout = 0xB531Du;  // rollout
constexpr uint32_t kF3661MsgDragin = 0xB5325u;  // dragin
constexpr uint32_t kF3661MsgDrag = 0xB532Cu;  // drag
constexpr uint32_t kF3661MsgDragout = 0xB5331u;  // dragout
constexpr uint32_t kF3661MsgPenetration = 0xB5339u;  // penetration
constexpr uint32_t kF3661ConstB5340 = 0xB5340u;  // 
constexpr uint32_t kF3661MsgEnable = 0xB5345u;  // enable
constexpr uint32_t kF3661MsgDisable = 0xB534Cu;  // disable
constexpr uint32_t kF3661ConstB5354 = 0xB5354u;  // 
constexpr uint32_t kF3661MsgBucketsX = 0xB5B6Cu;  // buckets_
constexpr uint32_t kF3661MsgBoostBoostUnorderedDetail = 0xB5B75u;  // ../../../../boost_1_66_0/boost/unordered/detail/implementation.hpp
constexpr uint32_t kF3661MsgGetBucket = 0xB5BB8u;  // get_bucket
constexpr uint32_t kF3661ConstB5E30 = 0xB5E30u;  // 
constexpr uint32_t kF3661MsgPx = 0xB5E31u;  // px != 0
constexpr uint32_t kF3661MsgBoostBoostSmartPtr = 0xB5E39u;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
constexpr uint32_t kF3661MsgOperator = 0xB5E71u;  // operator->
constexpr uint32_t kF3661ConstB5F40 = 0xB5F40u;  // 
constexpr uint32_t kF3661ConstB8F8C = 0xB8F8Cu;  // 
constexpr uint32_t kF3661ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF3661Const65646F6D = 0x65646F6Du;  // 
constexpr uint32_t kF3661Const65707974 = 0x65707974u;  // 
constexpr uint32_t kF3661Const80000000 = 0x80000000u;  // 
constexpr uint32_t kF3661ConstFFFFFFC0 = 0xFFFFFFC0u;  // 
constexpr uint32_t kF3661ConstFFFFFFF0 = 0xFFFFFFF0u;  // 
constexpr uint32_t kF3661ConstFFFFFFFE = 0xFFFFFFFEu;  // 
constexpr uint32_t kF3661ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF3661SaveFrameBytes = 56u;
constexpr uint32_t kF3661ResumeWordBytes = 4u;
constexpr uint32_t kF3661ResumeDeallocateRawBlock01 = 1u;
constexpr uint32_t kF3661ResumeDeallocateRawBlock41 = 41u;
constexpr uint32_t kF3661ResumeDeallocateRawBlock51 = 51u;
constexpr uint32_t kF3661ResumeDeallocateRawBlock54 = 54u;
constexpr uint32_t kF3661ResumeDeallocateRawBlock118 = 118u;

}  // namespace

namespace Compositor {
void LyEventDispatch(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l13 = 0u; uint32_t l14 = 0u; uint32_t l15 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u; uint32_t i6 = 0u;
  uint64_t j1 = 0; uint64_t j3 = 0;
      l11 = 0, l12 = 0, l13 = 0, l14 = 0, l15 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF3661SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l9 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 24u);
    l7 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 28u);
    l8 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 32u);
    l10 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 36u);
    l11 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 40u);
    l12 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 44u);
    l13 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 48u);
    l14 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 52u);
    l15 = i0;
    i0 = l9;
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
    i2 = kF3661ResumeWordBytes;
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
    i0 = GuestStackSp(ctx);
    i1 = 528u;
    i0 -= i1;
    l3 = i0;
    GuestStackSp(ctx) = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = !(i0);
    l5 = i0;
  }
  i0 = l5;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l6;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3661MsgPx;
      i2 = kF3661MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3661MsgOperator;
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
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_11;}
      goto block_3;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 24u);
    l4 = i0;
    i0 = !(i0);
    l5 = i0;
  }
  i0 = l5;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l6;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3661MsgPx;
      i2 = kF3661MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3661MsgOperator;
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
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_11;}
      goto block_3;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 98u, i1);
    i0 = l3;
    i1 = 25705u;
    StoreU16(ctx, i0 + 96u, i1);
    i0 = l3;
    i1 = 2u;
    StoreU8(ctx, i0 + 107u, i1);
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    i2 = 96u;
    i1 += i2;
    StoreU32(ctx, i0 + 200u, i1);
    i0 = b;
    i1 = 16u;
    i0 += i1;
    l13 = i0;
    i0 = l3;
    i1 = 96u;
    i0 += i1;
    l7 = i0;
    i0 = l3;
    i1 = 200u;
    i0 += i1;
    l10 = i0;
    i0 = l3;
    i1 = 184u;
    i0 += i1;
    l11 = i0;
    i0 = l3;
    i1 = 216u;
    i0 += i1;
    l5 = i0;
  }
  i0 = l6;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = l5;
    i2 = l13;
    i3 = l7;
    i4 = kF3661ConstB5E30;
    i5 = l10;
    i6 = l11;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 == i1;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    if (i0) {goto block_28;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 216u);
    l5 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 304u);
    l7 = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 504u;
    i0 += i1;
    l10 = i0;
    i0 = l5;
    i1 = 28u;
    i0 += i1;
    l5 = i0;
  }
  i0 = l6;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l7;
    i1 = l10;
    i2 = l4;
    i3 = l5;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    l4 = i0;
    if (i0) {goto block_28;}
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 107u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 96u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 504u);
    l4 = i0;
    if (i0) {goto block_27;}
    goto running_exit;
  }
  block_28:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 107u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_5;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 96u);
    DeallocateRawBlock(ctx, i0);
    i0 = c;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_27:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 100u, i1);
    i0 = l3;
    i1 = kF3661Const65707974;
    StoreU32(ctx, i0 + 96u, i1);
    i0 = l3;
    i1 = 4u;
    StoreU8(ctx, i0 + 107u, i1);
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    i2 = 96u;
    i1 += i2;
    StoreU32(ctx, i0 + 200u, i1);
    i0 = l3;
    i1 = 216u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = 96u;
    i0 += i1;
    l7 = i0;
    i0 = l3;
    i1 = 200u;
    i0 += i1;
    l10 = i0;
    i0 = l3;
    i1 = 184u;
    i0 += i1;
    l11 = i0;
    i0 = 0u;
    l4 = i0;
  }
  i0 = l6;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = l5;
    i2 = l13;
    i3 = l7;
    i4 = kF3661ConstB5E30;
    i5 = l10;
    i6 = l11;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 != i1;
    l5 = i0;
  }
  i0 = l5;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 216u);
      l5 = i0;
      i0 = LoadU32(ctx, i0 + 32u);
      l10 = i0;
      i0 = l5;
      i0 = LoadU8(ctx, i0 + 39u);
      l7 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l11 = i0;
      i0 = l10;
      i1 = l7;
      i2 = l11;
      i0 = i2 ? i0 : i1;
      i1 = 5u;
      i0 = i0 == i1;
      l7 = i0;
    }
    i0 = l7;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 28u;
        i0 += i1;
        l4 = i0;
      }
      i0 = l6;
      i1 = 5u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1354u;
        i1 = l4;
        i2 = 0u;
        i3 = kF3661ConstFFFFFFFF;
        i4 = kF3661MsgClick;
        i5 = 5u;
        i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
        l9 = i0;
        i0 = 5u;
        i1 = ResumePhase(ctx);
        i2 = kF3661ResumeDeallocateRawBlock01;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l9;
        l4 = i0;
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l4;
      } else {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 1u;
        i0 = i0 == i1;
        l5 = i0;
        if (i0) {goto block_3;}
        i0 = l4;
        i0 = !(i0);
      }
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 107u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l5 = i0;
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 96u);
        l5 = i0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = 1u;
      i1 = l4;
      if (i1) {goto block_36;}
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 100u, i1);
      i0 = l3;
      i1 = kF3661Const65707974;
      StoreU32(ctx, i0 + 96u, i1);
      i0 = l3;
      i1 = 4u;
      StoreU8(ctx, i0 + 107u, i1);
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = l3;
      i2 = 96u;
      i1 += i2;
      StoreU32(ctx, i0 + 200u, i1);
      i0 = l3;
      i1 = 216u;
      i0 += i1;
      l5 = i0;
      i0 = l3;
      i1 = 96u;
      i0 += i1;
      l7 = i0;
      i0 = l3;
      i1 = 200u;
      i0 += i1;
      l10 = i0;
      i0 = l3;
      i1 = 184u;
      i0 += i1;
      l11 = i0;
      i0 = 0u;
      l4 = i0;
    }
    i0 = l6;
    i1 = 6u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1959u;
      i1 = l5;
      i2 = l13;
      i3 = l7;
      i4 = kF3661ConstB5E30;
      i5 = l10;
      i6 = l11;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 6u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_38;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 216u);
      l5 = i0;
      i0 = LoadU32(ctx, i0 + 32u);
      l7 = i0;
      i0 = l5;
      i0 = LoadU8(ctx, i0 + 39u);
      l12 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l10 = i0;
      i0 = l7;
      i1 = l12;
      i2 = l10;
      i0 = i2 ? i0 : i1;
      i1 = 8u;
      i0 = i0 == i1;
      l7 = i0;
    }
    i0 = l7;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 28u;
        i0 += i1;
        l4 = i0;
      }
      i0 = l6;
      i1 = 7u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1354u;
        i1 = l4;
        i2 = 0u;
        i3 = kF3661ConstFFFFFFFF;
        i4 = kF3661MsgRollover;
        i5 = 8u;
        i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
        l9 = i0;
        i0 = 7u;
        i1 = ResumePhase(ctx);
        i2 = kF3661ResumeDeallocateRawBlock01;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l9;
        l4 = i0;
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l4;
      } else {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 1u;
        i0 = i0 == i1;
        l5 = i0;
        if (i0) {goto block_3;}
        i0 = l4;
        i0 = !(i0);
      }
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 107u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l5 = i0;
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 96u);
        l5 = i0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l4;
      i0 = !(i0);
      l4 = i0;
      if (i0) {goto block_37;}
      i0 = 1u;
      l8 = i0;
      i0 = 1u;
      l12 = i0;
      i0 = 0u;
      goto block_36;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 107u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_6;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 96u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_6;
  }
  block_38:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 107u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_6;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 96u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_6;
  }
  block_37:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 100u, i1);
    i0 = l3;
    i1 = kF3661Const65707974;
    StoreU32(ctx, i0 + 96u, i1);
    i0 = l3;
    i1 = 4u;
    StoreU8(ctx, i0 + 107u, i1);
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    i2 = 96u;
    i1 += i2;
    StoreU32(ctx, i0 + 200u, i1);
    i0 = l3;
    i1 = 216u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = 96u;
    i0 += i1;
    l7 = i0;
    i0 = l3;
    i1 = 200u;
    i0 += i1;
    l10 = i0;
    i0 = l3;
    i1 = 184u;
    i0 += i1;
    l11 = i0;
    i0 = 0u;
    l4 = i0;
  }
  i0 = l6;
  i1 = 8u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = l5;
    i2 = l13;
    i3 = l7;
    i4 = kF3661ConstB5E30;
    i5 = l10;
    i6 = l11;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 8u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 != i1;
    l5 = i0;
  }
  i0 = l5;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 216u);
      l5 = i0;
      i0 = LoadU32(ctx, i0 + 32u);
      l7 = i0;
      i0 = l5;
      i0 = LoadU8(ctx, i0 + 39u);
      l12 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l10 = i0;
      i0 = l7;
      i1 = l12;
      i2 = l10;
      i0 = i2 ? i0 : i1;
      i1 = 7u;
      i0 = i0 == i1;
      l7 = i0;
    }
    i0 = l7;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 28u;
        i0 += i1;
        l4 = i0;
      }
      i0 = l6;
      i1 = 9u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1354u;
        i1 = l4;
        i2 = 0u;
        i3 = kF3661ConstFFFFFFFF;
        i4 = kF3661MsgRollout;
        i5 = 7u;
        i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
        l9 = i0;
        i0 = 9u;
        i1 = ResumePhase(ctx);
        i2 = kF3661ResumeDeallocateRawBlock01;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l9;
        l4 = i0;
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l4;
      } else {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 1u;
        i0 = i0 == i1;
        l5 = i0;
        if (i0) {goto block_3;}
        i0 = l4;
        i0 = !(i0);
      }
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 107u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l5 = i0;
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 96u);
        l5 = i0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l4;
      i0 = !(i0);
      l4 = i0;
      if (i0) {goto block_64;}
      i0 = 2u;
      l12 = i0;
      goto block_63;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 107u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_6;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 96u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_6;
  }
  block_64:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 100u, i1);
    i0 = l3;
    i1 = kF3661Const65707974;
    StoreU32(ctx, i0 + 96u, i1);
    i0 = l3;
    i1 = 4u;
    StoreU8(ctx, i0 + 107u, i1);
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    i2 = 96u;
    i1 += i2;
    StoreU32(ctx, i0 + 200u, i1);
    i0 = l3;
    i1 = 216u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = 96u;
    i0 += i1;
    l7 = i0;
    i0 = l3;
    i1 = 200u;
    i0 += i1;
    l10 = i0;
    i0 = l3;
    i1 = 184u;
    i0 += i1;
    l11 = i0;
    i0 = 0u;
    l4 = i0;
  }
  i0 = l6;
  i1 = 10u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = l5;
    i2 = l13;
    i3 = l7;
    i4 = kF3661ConstB5E30;
    i5 = l10;
    i6 = l11;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 10u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 != i1;
    l5 = i0;
  }
  i0 = l5;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 216u);
      l5 = i0;
      i0 = LoadU32(ctx, i0 + 32u);
      l7 = i0;
      i0 = l5;
      i0 = LoadU8(ctx, i0 + 39u);
      l12 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l10 = i0;
      i0 = l7;
      i1 = l12;
      i2 = l10;
      i0 = i2 ? i0 : i1;
      i1 = 6u;
      i0 = i0 == i1;
      l7 = i0;
    }
    i0 = l7;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 28u;
        i0 += i1;
        l4 = i0;
      }
      i0 = l6;
      i1 = 11u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1354u;
        i1 = l4;
        i2 = 0u;
        i3 = kF3661ConstFFFFFFFF;
        i4 = kF3661MsgDragin;
        i5 = 6u;
        i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
        l9 = i0;
        i0 = 11u;
        i1 = ResumePhase(ctx);
        i2 = kF3661ResumeDeallocateRawBlock01;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l9;
        l4 = i0;
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l4;
      } else {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 1u;
        i0 = i0 == i1;
        l5 = i0;
        if (i0) {goto block_3;}
        i0 = l4;
        i0 = !(i0);
      }
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 107u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l5 = i0;
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 96u);
        l5 = i0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l4;
      i0 = !(i0);
      l4 = i0;
      if (i0) {goto block_79;}
      i0 = 3u;
      l12 = i0;
      goto block_63;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 107u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_6;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 96u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_6;
  }
  block_79:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 100u, i1);
    i0 = l3;
    i1 = kF3661Const65707974;
    StoreU32(ctx, i0 + 96u, i1);
    i0 = l3;
    i1 = 4u;
    StoreU8(ctx, i0 + 107u, i1);
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    i2 = 96u;
    i1 += i2;
    StoreU32(ctx, i0 + 200u, i1);
    i0 = l3;
    i1 = 216u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = 96u;
    i0 += i1;
    l7 = i0;
    i0 = l3;
    i1 = 200u;
    i0 += i1;
    l10 = i0;
    i0 = l3;
    i1 = 184u;
    i0 += i1;
    l11 = i0;
    i0 = 0u;
    l4 = i0;
  }
  i0 = l6;
  i1 = 12u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = l5;
    i2 = l13;
    i3 = l7;
    i4 = kF3661ConstB5E30;
    i5 = l10;
    i6 = l11;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 12u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 != i1;
    l5 = i0;
  }
  i0 = l5;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 216u);
      l5 = i0;
      i0 = LoadU32(ctx, i0 + 32u);
      l7 = i0;
      i0 = l5;
      i0 = LoadU8(ctx, i0 + 39u);
      l12 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l10 = i0;
      i0 = l7;
      i1 = l12;
      i2 = l10;
      i0 = i2 ? i0 : i1;
      i1 = 4u;
      i0 = i0 == i1;
      l7 = i0;
    }
    i0 = l7;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 28u;
        i0 += i1;
        l4 = i0;
      }
      i0 = l6;
      i1 = 13u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1354u;
        i1 = l4;
        i2 = 0u;
        i3 = kF3661ConstFFFFFFFF;
        i4 = kF3661MsgDrag;
        i5 = kF3661ResumeWordBytes;
        i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
        l9 = i0;
        i0 = 13u;
        i1 = ResumePhase(ctx);
        i2 = kF3661ResumeDeallocateRawBlock01;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l9;
        l4 = i0;
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l4;
      } else {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 1u;
        i0 = i0 == i1;
        l5 = i0;
        if (i0) {goto block_3;}
        i0 = l4;
        i0 = !(i0);
      }
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 107u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l5 = i0;
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 96u);
        l5 = i0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l4;
      i0 = !(i0);
      l4 = i0;
      if (i0) {goto block_94;}
      i0 = 4u;
      l12 = i0;
      goto block_63;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 107u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_6;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 96u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_6;
  }
  block_94:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 100u, i1);
    i0 = l3;
    i1 = kF3661Const65707974;
    StoreU32(ctx, i0 + 96u, i1);
    i0 = l3;
    i1 = 4u;
    StoreU8(ctx, i0 + 107u, i1);
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    i2 = 96u;
    i1 += i2;
    StoreU32(ctx, i0 + 200u, i1);
    i0 = l3;
    i1 = 216u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = 96u;
    i0 += i1;
    l7 = i0;
    i0 = l3;
    i1 = 200u;
    i0 += i1;
    l10 = i0;
    i0 = l3;
    i1 = 184u;
    i0 += i1;
    l11 = i0;
    i0 = 0u;
    l4 = i0;
  }
  i0 = l6;
  i1 = 14u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = l5;
    i2 = l13;
    i3 = l7;
    i4 = kF3661ConstB5E30;
    i5 = l10;
    i6 = l11;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 14u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 != i1;
    l5 = i0;
  }
  i0 = l5;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 216u);
      l5 = i0;
      i0 = LoadU32(ctx, i0 + 32u);
      l7 = i0;
      i0 = l5;
      i0 = LoadU8(ctx, i0 + 39u);
      l12 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l10 = i0;
      i0 = l7;
      i1 = l12;
      i2 = l10;
      i0 = i2 ? i0 : i1;
      i1 = 7u;
      i0 = i0 == i1;
      l7 = i0;
    }
    i0 = l7;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 28u;
        i0 += i1;
        l4 = i0;
      }
      i0 = l6;
      i1 = 15u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1354u;
        i1 = l4;
        i2 = 0u;
        i3 = kF3661ConstFFFFFFFF;
        i4 = kF3661MsgDragout;
        i5 = 7u;
        i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
        l9 = i0;
        i0 = 15u;
        i1 = ResumePhase(ctx);
        i2 = kF3661ResumeDeallocateRawBlock01;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l9;
        l4 = i0;
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l4;
      } else {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 1u;
        i0 = i0 == i1;
        l5 = i0;
        if (i0) {goto block_3;}
        i0 = l4;
        i0 = !(i0);
      }
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 107u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l5 = i0;
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 96u);
        l5 = i0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l4;
      i0 = !(i0);
      l4 = i0;
      if (i0) {goto running_exit;}
      i0 = 5u;
      l12 = i0;
      goto block_63;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 107u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_6;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 96u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_6;
  }
  block_63:;
  i0 = l10;
  i1 = 0u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  block_36:;
  l10 = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 100u, i1);
    i0 = l3;
    i1 = kF3661Const65646F6D;
    StoreU32(ctx, i0 + 96u, i1);
    i0 = l3;
    i1 = 4u;
    StoreU8(ctx, i0 + 107u, i1);
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    i2 = 96u;
    i1 += i2;
    StoreU32(ctx, i0 + 200u, i1);
    i0 = l3;
    i1 = 96u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = 200u;
    i0 += i1;
    l7 = i0;
    i0 = l3;
    i1 = 184u;
    i0 += i1;
    l11 = i0;
    i0 = l3;
    i1 = 216u;
    i0 += i1;
    l4 = i0;
  }
  i0 = l6;
  i1 = 16u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = l4;
    i2 = l13;
    i3 = l5;
    i4 = kF3661ConstB5E30;
    i5 = l7;
    i6 = l11;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 16u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_7;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 216u);
    l4 = i0;
    i0 = LoadU32(ctx, i0 + 32u);
    l11 = i0;
    i0 = l4;
    i0 = LoadU8(ctx, i0 + 39u);
    l7 = i0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l14 = i0;
    i0 = l11;
    i1 = l7;
    i2 = l14;
    i0 = i2 ? i0 : i1;
    i1 = 4u;
    i0 = i0 == i1;
    l7 = i0;
    i0 = 0u;
    l5 = i0;
  }
  i0 = l7;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 28u;
      i0 += i1;
      l4 = i0;
    }
    i0 = l6;
    i1 = 17u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = l4;
      i2 = 0u;
      i3 = kF3661ConstFFFFFFFF;
      i4 = kF3661MsgInit;
      i5 = kF3661ResumeWordBytes;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l9 = i0;
      i0 = 17u;
      i1 = ResumePhase(ctx);
      i2 = kF3661ResumeDeallocateRawBlock01;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l9;
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l5;
    } else {
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_3;}
      i0 = l4;
      i0 = !(i0);
    }
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 107u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    l4 = i0;
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 96u);
      l4 = i0;
      DeallocateRawBlock(ctx, i0);
    }
  }
  i0 = l5;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 504u);
      l7 = i0;
      i0 = l8;
      i1 = l10;
      i0 |= i1;
      i1 = 1u;
      i0 = i0 != i1;
      l4 = i0;
      if (i0) {goto block_138;}
      i0 = l7;
      i0 = !(i0);
      l4 = i0;
    }
    i0 = l4;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l6;
      i1 = 18u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3661MsgPx;
        i2 = kF3661MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3661MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 18u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_11;}
        goto block_3;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = 0u;
      l4 = i0;
    }
    i0 = l6;
    i1 = 19u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 16u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l9 = i0;
      i0 = 19u;
      i1 = ResumePhase(ctx);
      i2 = kF3661ResumeDeallocateRawBlock01;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l9;
      l8 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = 1u;
      l10 = i0;
      i0 = l5;
      i1 = 1u;
      i0 = i0 != i1;
      l5 = i0;
    }
    i0 = l5;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      i1 = 0u;
      i2 = ResumePhase(ctx);
      i2 = !(i2);
      if (i2) {
        i2 = l8;
        i3 = kF3661ConstB5340;
        i3 = LoadU32(ctx, i3);
        StoreU32(ctx, i2 + 7u, i3);
        i2 = l8;
        i3 = kF3661MsgPenetration;
        j3 = LoadU64(ctx, i3);
        StoreU64(ctx, i2, j3);
        i2 = l8;
        i3 = 0u;
        StoreU8(ctx, i2 + 11u, i3);
        i2 = b;
        i2 = LoadU32(ctx, i2 + 20u);
        l5 = i2;
        i2 = !(i2);
        l11 = i2;
      }
      i2 = l11;
      i0 = i2 ? i0 : i1;
      if (i0) {goto block_161;}
      loop_165: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l5;
          i0 = LoadU8(ctx, i0 + 27u);
          l4 = i0;
          i1 = 24u;
          i0 <<= (i1 & 31);
          i1 = 24u;
          i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
          i1 = 0u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          l14 = i0;
          i0 = l5;
          i0 = LoadU32(ctx, i0 + 20u);
          i1 = l4;
          i2 = l14;
          i0 = i2 ? i0 : i1;
          l4 = i0;
          i1 = 11u;
          i0 = i0 < i1;
          l15 = i0;
          i0 = l4;
          i1 = 11u;
          i2 = l15;
          i0 = i2 ? i0 : i1;
          l11 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l11;
          if (i0) {
            i0 = l8;
            i1 = l5;
            i2 = 16u;
            i1 += i2;
            l9 = i1;
            i1 = LoadU32(ctx, i1);
            i2 = l9;
            i3 = l14;
            i1 = i3 ? i1 : i2;
            l14 = i1;
            i2 = l11;
            i0 = CompareBytes(ctx, i0, i1, i2);
            l9 = i0;
            i0 = !(i0);
            if (i0) {
              i0 = l4;
              i1 = 11u;
              i0 = i0 <= i1;
              l4 = i0;
              if (i0) {goto block_173;}
              goto block_167;
            }
            i0 = l9;
            i1 = kF3661ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            l4 = i0;
            if (i0) {goto block_167;}
            block_173:;
            i0 = l14;
            i1 = l8;
            i2 = l11;
            i0 = CompareBytes(ctx, i0, i1, i2);
            l4 = i0;
            i0 = !(i0);
            l11 = i0;
            if (i0) {goto block_171;}
            i0 = l4;
            i1 = kF3661ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            l4 = i0;
            if (i0) {goto block_170;}
            goto block_168;
          }
          i0 = l4;
          i1 = 11u;
          i0 = i0 > i1;
          l4 = i0;
          if (i0) {goto block_167;}
          block_171:;
          i0 = l15;
          if (i0) {goto block_168;}
          block_170:;
          i0 = kF3661ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l6;
        i1 = 20u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1032u;
          i1 = 16u;
          i0 = HostFoldLayoutOffset(ctx, i0, i1);
          l9 = i0;
          i0 = 20u;
          i1 = ResumePhase(ctx);
          i2 = kF3661ResumeDeallocateRawBlock01;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l9;
          l4 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3661ConstD015C;
          i0 = LoadU32(ctx, i0);
          l5 = i0;
          i0 = kF3661ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l5;
          i1 = 1u;
          i0 = i0 == i1;
          l5 = i0;
          if (i0) {goto block_160;}
          i0 = l3;
          i1 = l4;
          StoreU32(ctx, i0 + 96u, i1);
          i0 = l3;
          j1 = 9223372105574252555ull;
          StoreU64(ctx, i0 + 100u, j1);
          i0 = l4;
          i1 = 0u;
          StoreU8(ctx, i0 + 11u, i1);
          i0 = l4;
          i1 = kF3661ConstB5340;
          i1 = LoadU32(ctx, i1);
          StoreU32(ctx, i0 + 7u, i1);
          i0 = l4;
          i1 = kF3661MsgPenetration;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0, j1);
          i0 = kF3661ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = l3;
          i2 = 96u;
          i1 += i2;
          StoreU32(ctx, i0 + 200u, i1);
          i0 = l3;
          i1 = 96u;
          i0 += i1;
          l5 = i0;
          i0 = l3;
          i1 = 200u;
          i0 += i1;
          l10 = i0;
          i0 = l3;
          i1 = 184u;
          i0 += i1;
          l11 = i0;
          i0 = l3;
          i1 = 216u;
          i0 += i1;
          l4 = i0;
        }
        i0 = l6;
        i1 = 21u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1959u;
          i1 = l4;
          i2 = l13;
          i3 = l5;
          i4 = kF3661ConstB5E30;
          i5 = l10;
          i6 = l11;
          HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
          i0 = 21u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3661ConstD015C;
          i0 = LoadU32(ctx, i0);
          l4 = i0;
          i0 = kF3661ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = 1u;
          i0 = i0 != i1;
          l4 = i0;
          if (i0) {goto block_164;}
          i0 = HostRet0(ctx);
          c = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_153;
        }
        block_168:;
        i0 = l5;
        i1 = l5;
        i2 = 4u;
        i1 += i2;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        l5 = i0;
        block_167:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l5;
          i0 = LoadU32(ctx, i0);
          l5 = i0;
          if (i0) {goto loop_165;}
        }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = 0u;
        l4 = i0;
        goto block_161;
      }
      block_164:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = 0u;
        l10 = i0;
        i0 = 1u;
        l4 = i0;
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 216u);
        l5 = i0;
        i0 = LoadU32(ctx, i0 + 32u);
        l14 = i0;
        i0 = l5;
        i0 = LoadU8(ctx, i0 + 39u);
        l11 = i0;
        i1 = 24u;
        i0 <<= (i1 & 31);
        i1 = 24u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l9 = i0;
        i0 = l14;
        i1 = l11;
        i2 = l9;
        i0 = i2 ? i0 : i1;
        i1 = 1u;
        i0 = i0 != i1;
        l11 = i0;
        if (i0) {goto block_161;}
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 28u;
        i0 += i1;
        l4 = i0;
      }
      i0 = l6;
      i1 = 22u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1354u;
        i1 = l4;
        i2 = 0u;
        i3 = kF3661ConstFFFFFFFF;
        i4 = kF3661ConstB4B29;
        i5 = 1u;
        i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
        l9 = i0;
        i0 = 22u;
        i1 = ResumePhase(ctx);
        i2 = kF3661ResumeDeallocateRawBlock01;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l9;
        l4 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 1u;
        i0 = i0 == i1;
        l5 = i0;
        if (i0) {goto block_3;}
        i0 = l4;
        i1 = 0u;
        i0 = i0 != i1;
        l4 = i0;
      }
      block_161:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 376u;
        i0 += i1;
        l5 = i0;
      }
      i0 = l6;
      i1 = 23u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 2591u;
        i1 = l5;
        i2 = b;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l9 = i0;
        i0 = 23u;
        i1 = ResumePhase(ctx);
        i2 = kF3661ResumeDeallocateRawBlock01;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l9;
        l5 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        l11 = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l11;
        i1 = 1u;
        i0 = i0 == i1;
        l11 = i0;
        if (i0) {goto block_159;}
        i0 = l3;
        i1 = 0u;
        StoreU32(ctx, i0 + 372u, i1);
        i0 = l3;
        i1 = 2648u;
        StoreU32(ctx, i0 + 368u, i1);
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = l3;
        j1 = LoadU64(ctx, i1 + 368u);
        StoreU64(ctx, i0, j1);
        i0 = l3;
        i1 = 424u;
        i0 += i1;
        l11 = i0;
      }
      i0 = l6;
      i1 = 24u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 2649u;
        i1 = l11;
        i2 = l3;
        i3 = a;
        i4 = l12;
        i5 = l4;
        i6 = l5;
        HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
        i0 = 24u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 1u;
        i0 = i0 == i1;
        l4 = i0;
        if (i0) {goto block_158;}
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 424u;
        i0 += i1;
        l11 = i0;
        i0 = l3;
        i1 = 488u;
        i0 += i1;
        l4 = i0;
      }
      i0 = l6;
      i1 = 25u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 2650u;
        i1 = l4;
        i2 = l11;
        i3 = 0u;
        i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
        l9 = i0;
        i0 = 25u;
        i1 = ResumePhase(ctx);
        i2 = kF3661ResumeDeallocateRawBlock01;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l9;
        l4 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        l11 = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l11;
        i1 = 1u;
        i0 = i0 == i1;
        l11 = i0;
        if (i0) {goto block_156;}
        i0 = l7;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 272u);
        l11 = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l6;
      i1 = 26u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l11;
        i1 = l7;
        i2 = l12;
        i3 = l4;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 26u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        l7 = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l7;
        i1 = 1u;
        i0 = i0 == i1;
        l7 = i0;
        if (i0) {goto block_157;}
        i0 = l4;
        i0 = LoadU32(ctx, i0);
        l7 = i0;
      }
      i0 = l7;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = l11;
        i1 = l7;
        i2 = 1u;
        i1 &= i2;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        l11 = i0;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l11;
          if (i0) {goto block_195;}
          i0 = l7;
          i1 = kF3661ConstFFFFFFFE;
          i0 &= i1;
          i0 = LoadU32(ctx, i0);
          l7 = i0;
          i0 = !(i0);
          l11 = i0;
          if (i0) {goto block_195;}
          i0 = kF3661ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l7;
          l11 = i0;
          i0 = l4;
          i1 = 4u;
          i0 += i1;
          l7 = i0;
        }
        i0 = l6;
        i1 = 27u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l11;
          i1 = l7;
          i2 = l7;
          i3 = 2u;
          HostSyncSlot(ctx, i0, i1, i2, i3);
          i0 = 27u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3661ConstD015C;
          i0 = LoadU32(ctx, i0);
          l7 = i0;
          i0 = kF3661ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l7;
          i1 = 1u;
          i0 = i0 == i1;
          l7 = i0;
          if (i0) {goto block_151;}
        }
        block_195:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i1 = kF3661ConstB5F40;
        StoreU32(ctx, i0 + 444u, i1);
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 483u);
        i1 = kF3661ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 472u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l3;
        i1 = 460u;
        i0 += i1;
        i1 = l3;
        i1 = LoadU32(ctx, i1 + 464u);
        ReleaseNodeTree(ctx, i0, i1);
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 459u);
        i1 = kF3661ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 448u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l5;
        i1 = kF3661ConstB5F40;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i0 = LoadS8(ctx, i0 + 39u);
        i1 = kF3661ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l5;
          i0 = LoadU32(ctx, i0 + 28u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l5;
        i1 = 16u;
        i0 += i1;
        i1 = l5;
        i1 = LoadU32(ctx, i1 + 20u);
        l7 = i1;
        ReleaseNodeTree(ctx, i0, i1);
        i0 = l5;
        i0 = LoadS8(ctx, i0 + 15u);
        i1 = kF3661ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        l4 = i0;
        if (i0) {
          i0 = l5;
          i0 = LoadU32(ctx, i0 + 4u);
          l4 = i0;
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l10;
        if (i0) {goto block_205;}
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 107u);
        i1 = kF3661ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        l4 = i0;
        if (i0) {goto block_205;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 96u);
        l4 = i0;
        DeallocateRawBlock(ctx, i0);
        block_205:;
        i0 = l8;
        DeallocateRawBlock(ctx, i0);
        goto block_137;
      }
      block_160:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        c = i0;
        i0 = HostTempRet0(ctx);
        a = i0;
        i0 = l8;
        DeallocateRawBlock(ctx, i0);
        goto block_6;
      }
      block_159:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        c = i0;
        i0 = HostTempRet0(ctx);
        i0 = l10;
        i0 = !(i0);
        a = i0;
        if (i0) {goto block_153;}
        goto block_152;
      }
      block_158:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        c = i0;
        i0 = HostTempRet0(ctx);
        a = i0;
        goto block_154;
      }
      block_157:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        c = i0;
        i0 = HostTempRet0(ctx);
        i0 = l4;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = !(i0);
        b = i0;
        if (i0) {goto block_155;}
        i0 = a;
        i1 = 1u;
        i0 &= i1;
        b = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        if (i0) {goto block_211;}
        i0 = a;
        i1 = kF3661ConstFFFFFFFE;
        i0 &= i1;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = !(i0);
        b = i0;
        if (i0) {goto block_211;}
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        b = i0;
        i0 = l4;
        i1 = 4u;
        i0 += i1;
        a = i0;
      }
      i0 = l6;
      i1 = 28u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = b;
        i1 = a;
        i2 = a;
        i3 = 2u;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 28u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 == i1;
        a = i0;
        if (i0) {goto block_210;}
      }
      block_211:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        goto block_155;
      }
      block_210:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_3;}
      block_156:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        c = i0;
        i0 = HostTempRet0(ctx);
        a = i0;
      }
      block_155:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i1 = kF3661ConstB5F40;
        StoreU32(ctx, i0 + 444u, i1);
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 483u);
        i1 = kF3661ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 472u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l3;
        i1 = 460u;
        i0 += i1;
        i1 = l3;
        i1 = LoadU32(ctx, i1 + 464u);
        b = i1;
        ReleaseNodeTree(ctx, i0, i1);
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 459u);
        i1 = kF3661ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        a = i0;
        if (i0) {goto block_154;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 448u);
        a = i0;
        DeallocateRawBlock(ctx, i0);
      }
      block_154:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        i1 = kF3661ConstB5F40;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i0 = LoadS8(ctx, i0 + 39u);
        i1 = kF3661ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l5;
          i0 = LoadU32(ctx, i0 + 28u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l5;
        i1 = 16u;
        i0 += i1;
        i1 = l5;
        i1 = LoadU32(ctx, i1 + 20u);
        b = i1;
        ReleaseNodeTree(ctx, i0, i1);
        i0 = l5;
        i0 = LoadS8(ctx, i0 + 15u);
        i1 = kF3661ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        a = i0;
        if (i0) {
          i0 = l5;
          i0 = LoadU32(ctx, i0 + 4u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l10;
        if (i0) {goto block_152;}
      }
      block_153:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 107u);
        i1 = kF3661ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        a = i0;
        if (i0) {goto block_152;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 96u);
        a = i0;
        DeallocateRawBlock(ctx, i0);
      }
      block_152:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        DeallocateRawBlock(ctx, i0);
        goto block_6;
      }
      block_151:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_3;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      c = i0;
      i0 = HostTempRet0(ctx);
      a = i0;
      goto block_6;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    l5 = i0;
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 100u, i1);
    i0 = l3;
    i1 = kF3661Const65646F6D;
    StoreU32(ctx, i0 + 96u, i1);
    i0 = l3;
    i1 = 4u;
    StoreU8(ctx, i0 + 107u, i1);
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    i2 = 96u;
    i1 += i2;
    StoreU32(ctx, i0 + 200u, i1);
    i0 = l3;
    i1 = 96u;
    i0 += i1;
    l8 = i0;
    i0 = l3;
    i1 = 200u;
    i0 += i1;
    l7 = i0;
    i0 = l3;
    i1 = 184u;
    i0 += i1;
    l10 = i0;
    i0 = l3;
    i1 = 216u;
    i0 += i1;
    l4 = i0;
  }
  i0 = l6;
  i1 = 29u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = l4;
    i2 = l13;
    i3 = l8;
    i4 = kF3661ConstB5E30;
    i5 = l7;
    i6 = l10;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 29u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
    l4 = i0;
  }
  i0 = l4;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 216u);
      l4 = i0;
      i0 = LoadU32(ctx, i0 + 32u);
      l7 = i0;
      i0 = l4;
      i0 = LoadU8(ctx, i0 + 39u);
      l8 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l10 = i0;
      i0 = l7;
      i1 = l8;
      i2 = l10;
      i0 = i2 ? i0 : i1;
      i1 = 5u;
      i0 = i0 == i1;
      l8 = i0;
    }
    i0 = l8;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 28u;
        i0 += i1;
        l4 = i0;
      }
      i0 = l6;
      i1 = 30u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1354u;
        i1 = l4;
        i2 = 0u;
        i3 = kF3661ConstFFFFFFFF;
        i4 = kF3661MsgReset;
        i5 = 5u;
        i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
        l9 = i0;
        i0 = 30u;
        i1 = ResumePhase(ctx);
        i2 = kF3661ResumeDeallocateRawBlock01;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l9;
        l4 = i0;
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l5;
      } else {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_3;}
        i0 = l4;
        i0 = !(i0);
      }
      l5 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 107u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 96u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l5;
      i0 = !(i0);
      l4 = i0;
      if (i0) {goto block_230;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 504u);
      b = i0;
      i0 = !(i0);
      l4 = i0;
    }
    i0 = l4;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l6;
      i1 = 31u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3661MsgPx;
        i2 = kF3661MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3661MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 31u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_11;}
        goto block_3;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 152u, i1);
      i0 = b;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 272u);
      l4 = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 152u;
      i0 += i1;
      l5 = i0;
    }
    i0 = l6;
    i1 = 32u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l4;
      i1 = b;
      i2 = l12;
      i3 = l5;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 32u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
      if (i0) {goto block_232;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 152u);
      b = i0;
    }
    i0 = b;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l4;
      i1 = b;
      i2 = 1u;
      i1 &= i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      l4 = i0;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        if (i0) {goto block_253;}
        i0 = b;
        i1 = kF3661ConstFFFFFFFE;
        i0 &= i1;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = !(i0);
        l4 = i0;
        if (i0) {goto block_253;}
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        l4 = i0;
        i0 = l3;
        i1 = 152u;
        i0 += i1;
        i1 = 4u;
        i0 |= i1;
        b = i0;
      }
      i0 = l6;
      i1 = 33u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l4;
        i1 = b;
        i2 = b;
        i3 = 2u;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 33u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto block_3;}
      }
      block_253:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i1 = 0u;
        StoreU32(ctx, i0 + 152u, i1);
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 504u);
      b = i0;
      i0 = !(i0);
      l4 = i0;
    }
    i0 = l4;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l6;
      i1 = 34u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3661MsgPx;
        i2 = kF3661MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3661MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 34u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_11;}
        goto block_3;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 292u);
      l4 = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l6;
    i1 = 35u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l4;
      i1 = b;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l9 = i0;
      i0 = 35u;
      i1 = ResumePhase(ctx);
      i2 = kF3661ResumeDeallocateRawBlock01;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l9;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      l4 = i0;
      if (i0) {goto block_130;}
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_231;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 504u);
      b = i0;
      i0 = !(i0);
      l4 = i0;
    }
    i0 = l4;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l6;
      i1 = 36u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3661MsgPx;
        i2 = kF3661MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3661MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 36u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_11;}
        goto block_3;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 276u);
      l4 = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l6;
    i1 = 37u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l4;
      i1 = b;
      i2 = l12;
      i3 = 1u;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 37u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_231;}
      goto block_130;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 107u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_6;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 96u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_6;
  }
  block_232:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 152u);
    a = i0;
    i0 = !(i0);
    b = i0;
    if (i0) {goto block_6;}
    i0 = a;
    i1 = 1u;
    i0 &= i1;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    if (i0) {goto block_278;}
    i0 = a;
    i1 = kF3661ConstFFFFFFFE;
    i0 &= i1;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = !(i0);
    b = i0;
    if (i0) {goto block_278;}
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    b = i0;
    i0 = l3;
    i1 = 152u;
    i0 += i1;
    i1 = 4u;
    i0 |= i1;
    a = i0;
  }
  i0 = l6;
  i1 = 38u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = a;
    i2 = a;
    i3 = 2u;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 38u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_277;}
  }
  block_278:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 0u;
    StoreU32(ctx, i0 + 152u, i1);
    goto block_6;
  }
  block_277:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_3;}
  block_231:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    if (i0) {goto running_exit;}
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 100u, j1);
    i0 = l3;
    i1 = l3;
    i2 = 96u;
    i1 += i2;
    i2 = 4u;
    i1 |= i2;
    b = i1;
    StoreU32(ctx, i0 + 96u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 220u, i1);
    i0 = l3;
    i1 = kF3661Const65707974;
    StoreU32(ctx, i0 + 216u, i1);
    i0 = l3;
    i1 = 4u;
    StoreU8(ctx, i0 + 227u, i1);
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    i2 = 216u;
    i1 += i2;
    StoreU32(ctx, i0 + 184u, i1);
    i0 = l3;
    i1 = 216u;
    i0 += i1;
    l4 = i0;
    i0 = l3;
    i1 = 184u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = 80u;
    i0 += i1;
    l8 = i0;
    i0 = l3;
    i1 = 200u;
    i0 += i1;
    c = i0;
  }
  i0 = l6;
  i1 = 39u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = c;
    i2 = l13;
    i3 = l4;
    i4 = kF3661ConstB5E30;
    i5 = l5;
    i6 = l8;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 39u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i0 = LoadU32(ctx, i0);
    c = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = 1u;
    i0 = i0 != i1;
    c = i0;
  }
  i0 = c;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 200u);
      c = i0;
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 204u, i1);
      i0 = l3;
      i1 = kF3661Const65707974;
      StoreU32(ctx, i0 + 200u, i1);
      i0 = l3;
      i1 = 4u;
      StoreU8(ctx, i0 + 211u, i1);
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = l3;
      i2 = 200u;
      i1 += i2;
      StoreU32(ctx, i0 + 80u, i1);
      i0 = l3;
      i1 = 96u;
      i0 += i1;
      l5 = i0;
      i0 = l3;
      i1 = 200u;
      i0 += i1;
      l8 = i0;
      i0 = l3;
      i1 = 80u;
      i0 += i1;
      l7 = i0;
      i0 = l3;
      i1 = kF3661ConstFFFFFFC0;
      i0 -= i1;
      l10 = i0;
      i0 = l3;
      i1 = 184u;
      i0 += i1;
      l4 = i0;
    }
    i0 = l6;
    i1 = 40u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1959u;
      i1 = l4;
      i2 = l5;
      i3 = l8;
      i4 = kF3661ConstB5E30;
      i5 = l7;
      i6 = l10;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 40u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      l4 = i0;
      if (i0) {goto block_288;}
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 184u);
      i1 = 28u;
      i0 += i1;
      l4 = i0;
      i0 = c;
      i1 = 28u;
      i0 += i1;
      c = i0;
    }
    i0 = l6;
    i1 = 41u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 870u;
      i1 = l4;
      i2 = c;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l9 = i0;
      i0 = kF3661ResumeDeallocateRawBlock41;
      i1 = ResumePhase(ctx);
      i2 = kF3661ResumeDeallocateRawBlock01;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l9;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 == i1;
      c = i0;
      if (i0) {goto block_288;}
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 211u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 200u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 227u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      c = i0;
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 216u);
        c = i0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = a;
      i0 = LoadU32(ctx, i0 + 128u);
      l12 = i0;
      i0 = !(i0);
      a = i0;
    }
    i0 = a;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l6;
      i1 = 42u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3661MsgPx;
        i2 = kF3661MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3661MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 42u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_11;}
        goto block_3;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 218u, i1);
      i0 = l3;
      i1 = 25705u;
      StoreU16(ctx, i0 + 216u, i1);
      i0 = l3;
      i1 = 2u;
      StoreU8(ctx, i0 + 227u, i1);
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = l3;
      i2 = 216u;
      i1 += i2;
      StoreU32(ctx, i0 + 184u, i1);
      i0 = l3;
      i1 = 216u;
      i0 += i1;
      c = i0;
      i0 = l3;
      i1 = 184u;
      i0 += i1;
      l4 = i0;
      i0 = l3;
      i1 = 80u;
      i0 += i1;
      l5 = i0;
      i0 = l3;
      i1 = 200u;
      i0 += i1;
      a = i0;
    }
    i0 = l6;
    i1 = 43u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1959u;
      i1 = a;
      i2 = l13;
      i3 = c;
      i4 = kF3661ConstB5E30;
      i5 = l4;
      i6 = l5;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 43u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_286;}
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 140u, j1);
      i0 = l3;
      i1 = l3;
      i2 = 136u;
      i1 += i2;
      i2 = 4u;
      i1 |= i2;
      l13 = i1;
      StoreU32(ctx, i0 + 136u, i1);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 200u);
      l8 = i0;
      i0 = l3;
      i1 = 7u;
      StoreU8(ctx, i0 + 211u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 207u, i1);
      i0 = l3;
      i1 = kF3661MsgLyevent;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0 + 200u, i1);
      i0 = l3;
      i1 = kF3661ConstAF65B;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0 + 203u, i1);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 96u);
      l4 = i0;
      i1 = b;
      i0 = i0 != i1;
      a = i0;
    }
    i0 = a;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      loop_308: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3661ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = 136u;
          i0 += i1;
          l5 = i0;
          i0 = l4;
          a = i0;
          i1 = 16u;
          i0 += i1;
          c = i0;
        }
        i0 = l6;
        i1 = 44u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2364u;
          i1 = l5;
          i2 = l13;
          i3 = c;
          i4 = c;
          i0 = HostInvokeIiiii(ctx, i0, i1, i2, i3, i4);
          l9 = i0;
          i0 = 44u;
          i1 = ResumePhase(ctx);
          i2 = kF3661ResumeDeallocateRawBlock01;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l9;
          c = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3661ConstD015C;
          i0 = LoadU32(ctx, i0);
          c = i0;
          i0 = kF3661ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = c;
          i1 = 1u;
          i0 = i0 == i1;
          c = i0;
          if (i0) {goto block_229;}
          i0 = a;
          i0 = LoadU32(ctx, i0 + 4u);
          l5 = i0;
          i0 = !(i0);
          c = i0;
          if (i0) {
            i0 = a;
            i0 = LoadU32(ctx, i0 + 8u);
            l4 = i0;
            i0 = LoadU32(ctx, i0);
            i1 = a;
            i0 = i0 == i1;
            c = i0;
            if (i0) {goto block_312;}
            i0 = a;
            i1 = 8u;
            i0 += i1;
            c = i0;
            loop_314: 
              i0 = c;
              i0 = LoadU32(ctx, i0);
              a = i0;
              i1 = 8u;
              i0 += i1;
              c = i0;
              i0 = a;
              i0 = LoadU32(ctx, i0 + 8u);
              l4 = i0;
              i0 = LoadU32(ctx, i0);
              l5 = i0;
              i1 = a;
              i0 = i0 != i1;
              if (i0) {goto loop_314;}
            goto block_312;
          }
          loop_315: 
            i0 = l5;
            l4 = i0;
            i0 = LoadU32(ctx, i0);
            l5 = i0;
            if (i0) {goto loop_315;}
          block_312:;
          i0 = b;
          i1 = l4;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto loop_308;}
        }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 200u;
      i0 += i1;
      b = i0;
      i0 = l3;
      i1 = 136u;
      i0 += i1;
      c = i0;
      i0 = l8;
      i1 = 28u;
      i0 += i1;
      a = i0;
    }
    i0 = l6;
    i1 = 45u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2651u;
      i1 = l12;
      i2 = a;
      i3 = b;
      i4 = c;
      i5 = 1u;
      HostInitResources(ctx, i0, i1, i2, i3, i4, i5);
      i0 = 45u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_285;}
      i0 = l3;
      i1 = 136u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 140u);
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 211u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 200u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 227u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 216u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l3;
      i1 = 96u;
      i0 += i1;
      a = i0;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 100u);
      b = i1;
      ReleaseNodeTree(ctx, i0, i1);
      goto running_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_287;
  }
  block_288:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 211u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_287;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 200u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_287:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 227u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_227;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 216u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_227;
  }
  block_286:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_228;
  }
  block_285:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_229;}
  block_230:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    l5 = i0;
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 100u, i1);
    i0 = l3;
    i1 = kF3661Const65646F6D;
    StoreU32(ctx, i0 + 96u, i1);
    i0 = l3;
    i1 = 4u;
    StoreU8(ctx, i0 + 107u, i1);
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    i2 = 96u;
    i1 += i2;
    StoreU32(ctx, i0 + 200u, i1);
    i0 = l3;
    i1 = 96u;
    i0 += i1;
    l8 = i0;
    i0 = l3;
    i1 = 200u;
    i0 += i1;
    l7 = i0;
    i0 = l3;
    i1 = 184u;
    i0 += i1;
    l10 = i0;
    i0 = l3;
    i1 = 216u;
    i0 += i1;
    l4 = i0;
  }
  i0 = l6;
  i1 = 46u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = l4;
    i2 = l13;
    i3 = l8;
    i4 = kF3661ConstB5E30;
    i5 = l7;
    i6 = l10;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 46u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
    l4 = i0;
  }
  i0 = l4;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 216u);
      l4 = i0;
      i0 = LoadU32(ctx, i0 + 32u);
      l7 = i0;
      i0 = l4;
      i0 = LoadU8(ctx, i0 + 39u);
      l8 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l10 = i0;
      i0 = l7;
      i1 = l8;
      i2 = l10;
      i0 = i2 ? i0 : i1;
      i1 = 6u;
      i0 = i0 == i1;
      l8 = i0;
    }
    i0 = l8;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 28u;
        i0 += i1;
        l4 = i0;
      }
      i0 = l6;
      i1 = 47u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1354u;
        i1 = l4;
        i2 = 0u;
        i3 = kF3661ConstFFFFFFFF;
        i4 = kF3661MsgEnable;
        i5 = 6u;
        i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
        l9 = i0;
        i0 = 47u;
        i1 = ResumePhase(ctx);
        i2 = kF3661ResumeDeallocateRawBlock01;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l9;
        l4 = i0;
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l5;
      } else {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_3;}
        i0 = l4;
        i0 = !(i0);
      }
      l5 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 107u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l4 = i0;
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 96u);
        l4 = i0;
        DeallocateRawBlock(ctx, i0);
      }
    }
    i0 = l5;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 504u);
        b = i0;
        i0 = !(i0);
        l4 = i0;
      }
      i0 = l4;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3661ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l6;
        i1 = 48u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3661MsgPx;
          i2 = kF3661MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3661MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 48u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3661ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3661ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_11;}
          goto block_3;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 276u);
        l4 = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l6;
      i1 = 49u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l4;
        i1 = b;
        i2 = l12;
        i3 = 1u;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 49u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto block_130;}
        i0 = c;
        if (i0) {goto running_exit;}
        i0 = l3;
        j1 = 0ull;
        StoreU64(ctx, i0 + 220u, j1);
        i0 = l3;
        i1 = l3;
        i2 = 216u;
        i1 += i2;
        i2 = 4u;
        i1 |= i2;
        l12 = i1;
        StoreU32(ctx, i0 + 216u, i1);
        i0 = l3;
        i1 = 0u;
        StoreU8(ctx, i0 + 100u, i1);
        i0 = l3;
        i1 = kF3661Const65646F6D;
        StoreU32(ctx, i0 + 96u, i1);
        i0 = l3;
        i1 = 4u;
        StoreU8(ctx, i0 + 107u, i1);
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = l3;
        i2 = 96u;
        i1 += i2;
        StoreU32(ctx, i0 + 184u, i1);
        i0 = l3;
        i1 = 216u;
        i0 += i1;
        c = i0;
        i0 = l3;
        i1 = 96u;
        i0 += i1;
        l4 = i0;
        i0 = l3;
        i1 = 184u;
        i0 += i1;
        l5 = i0;
        i0 = l3;
        i1 = 80u;
        i0 += i1;
        l8 = i0;
        i0 = l3;
        i1 = 200u;
        i0 += i1;
        b = i0;
      }
      i0 = l6;
      i1 = 50u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1959u;
        i1 = b;
        i2 = c;
        i3 = l4;
        i4 = kF3661ConstB5E30;
        i5 = l5;
        i6 = l8;
        HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
        i0 = 50u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto block_340;}
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 200u);
        i1 = 28u;
        i0 += i1;
        b = i0;
      }
      i0 = l6;
      i1 = 51u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 996u;
        i1 = b;
        i2 = kF3661MsgDisable;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l9 = i0;
        i0 = kF3661ResumeDeallocateRawBlock51;
        i1 = ResumePhase(ctx);
        i2 = kF3661ResumeDeallocateRawBlock01;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l9;
        b = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto block_340;}
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 107u);
        i1 = kF3661ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 96u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l3;
        i1 = 0u;
        StoreU8(ctx, i0 + 100u, i1);
        i0 = l3;
        i1 = kF3661Const65707974;
        StoreU32(ctx, i0 + 96u, i1);
        i0 = l3;
        i1 = 4u;
        StoreU8(ctx, i0 + 107u, i1);
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = l3;
        i2 = 96u;
        i1 += i2;
        StoreU32(ctx, i0 + 184u, i1);
        i0 = l3;
        i1 = 96u;
        i0 += i1;
        c = i0;
        i0 = l3;
        i1 = 184u;
        i0 += i1;
        l4 = i0;
        i0 = l3;
        i1 = 80u;
        i0 += i1;
        l5 = i0;
        i0 = l3;
        i1 = 200u;
        i0 += i1;
        b = i0;
      }
      i0 = l6;
      i1 = 52u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1959u;
        i1 = b;
        i2 = l13;
        i3 = c;
        i4 = kF3661ConstB5E30;
        i5 = l4;
        i6 = l5;
        HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
        i0 = 52u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto block_342;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 200u);
        b = i0;
        i0 = l3;
        i1 = 0u;
        StoreU8(ctx, i0 + 204u, i1);
        i0 = l3;
        i1 = kF3661Const65707974;
        StoreU32(ctx, i0 + 200u, i1);
        i0 = l3;
        i1 = 4u;
        StoreU8(ctx, i0 + 211u, i1);
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = l3;
        i2 = 200u;
        i1 += i2;
        StoreU32(ctx, i0 + 80u, i1);
        i0 = l3;
        i1 = 216u;
        i0 += i1;
        l4 = i0;
        i0 = l3;
        i1 = 200u;
        i0 += i1;
        l5 = i0;
        i0 = l3;
        i1 = 80u;
        i0 += i1;
        l8 = i0;
        i0 = l3;
        i1 = kF3661ConstFFFFFFC0;
        i0 -= i1;
        l7 = i0;
        i0 = l3;
        i1 = 184u;
        i0 += i1;
        c = i0;
      }
      i0 = l6;
      i1 = 53u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1959u;
        i1 = c;
        i2 = l4;
        i3 = l5;
        i4 = kF3661ConstB5E30;
        i5 = l8;
        i6 = l7;
        HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
        i0 = 53u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        c = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = c;
        i1 = 1u;
        i0 = i0 == i1;
        c = i0;
        if (i0) {goto block_343;}
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 184u);
        i1 = 28u;
        i0 += i1;
        c = i0;
        i0 = b;
        i1 = 28u;
        i0 += i1;
        b = i0;
      }
      i0 = l6;
      i1 = 54u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 870u;
        i1 = c;
        i2 = b;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l9 = i0;
        i0 = kF3661ResumeDeallocateRawBlock54;
        i1 = ResumePhase(ctx);
        i2 = kF3661ResumeDeallocateRawBlock01;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l9;
        b = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto block_343;}
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 211u);
        i1 = kF3661ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 200u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 107u);
        i1 = kF3661ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 96u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = a;
        i0 = LoadU32(ctx, i0 + 128u);
        l8 = i0;
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
          i0 = kF3661ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l6;
        i1 = 55u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3661MsgPx;
          i2 = kF3661MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3661MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 55u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3661ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3661ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_11;}
          goto block_3;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i1 = 0u;
        StoreU8(ctx, i0 + 98u, i1);
        i0 = l3;
        i1 = 25705u;
        StoreU16(ctx, i0 + 96u, i1);
        i0 = l3;
        i1 = 2u;
        StoreU8(ctx, i0 + 107u, i1);
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = l3;
        i2 = 96u;
        i1 += i2;
        StoreU32(ctx, i0 + 184u, i1);
        i0 = l3;
        i1 = 96u;
        i0 += i1;
        c = i0;
        i0 = l3;
        i1 = 184u;
        i0 += i1;
        l4 = i0;
        i0 = l3;
        i1 = 80u;
        i0 += i1;
        l5 = i0;
        i0 = l3;
        i1 = 200u;
        i0 += i1;
        b = i0;
      }
      i0 = l6;
      i1 = 56u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1959u;
        i1 = b;
        i2 = l13;
        i3 = c;
        i4 = kF3661ConstB5E30;
        i5 = l4;
        i6 = l5;
        HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
        i0 = 56u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto block_345;}
        i0 = l3;
        j1 = 0ull;
        StoreU64(ctx, i0 + 124u, j1);
        i0 = l3;
        i1 = l3;
        i2 = 120u;
        i1 += i2;
        i2 = 4u;
        i1 |= i2;
        l7 = i1;
        StoreU32(ctx, i0 + 120u, i1);
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 200u);
        l10 = i0;
        i0 = l3;
        i1 = 7u;
        StoreU8(ctx, i0 + 211u, i1);
        i0 = l3;
        i1 = 0u;
        StoreU8(ctx, i0 + 207u, i1);
        i0 = l3;
        i1 = kF3661MsgLyevent;
        i1 = LoadU32(ctx, i1);
        StoreU32(ctx, i0 + 200u, i1);
        i0 = l3;
        i1 = kF3661ConstAF65B;
        i1 = LoadU32(ctx, i1);
        StoreU32(ctx, i0 + 203u, i1);
        i0 = l12;
        i1 = l3;
        i1 = LoadU32(ctx, i1 + 216u);
        l4 = i1;
        i0 = i0 != i1;
        b = i0;
      }
      i0 = b;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        loop_384: 
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 120u;
            i0 += i1;
            l5 = i0;
            i0 = l4;
            i1 = 16u;
            i0 += i1;
            c = i0;
            i0 = l4;
            b = i0;
          }
          i0 = l6;
          i1 = 57u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 2364u;
            i1 = l5;
            i2 = l7;
            i3 = c;
            i4 = c;
            i0 = HostInvokeIiiii(ctx, i0, i1, i2, i3, i4);
            l9 = i0;
            i0 = 57u;
            i1 = ResumePhase(ctx);
            i2 = kF3661ResumeDeallocateRawBlock01;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l9;
            c = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3661ConstD015C;
            i0 = LoadU32(ctx, i0);
            c = i0;
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = c;
            i1 = 1u;
            i0 = i0 == i1;
            c = i0;
            if (i0) {goto block_346;}
            i0 = b;
            i0 = LoadU32(ctx, i0 + 4u);
            l5 = i0;
            i0 = !(i0);
            c = i0;
            if (i0) {
              i0 = b;
              i0 = LoadU32(ctx, i0 + 8u);
              l4 = i0;
              i0 = LoadU32(ctx, i0);
              i1 = b;
              i0 = i0 == i1;
              c = i0;
              if (i0) {goto block_388;}
              i0 = b;
              i1 = 8u;
              i0 += i1;
              c = i0;
              loop_390: 
                i0 = c;
                i0 = LoadU32(ctx, i0);
                b = i0;
                i1 = 8u;
                i0 += i1;
                c = i0;
                i0 = b;
                i0 = LoadU32(ctx, i0 + 8u);
                l4 = i0;
                i0 = LoadU32(ctx, i0);
                l5 = i0;
                i1 = b;
                i0 = i0 != i1;
                if (i0) {goto loop_390;}
              goto block_388;
            }
            loop_391: 
              i0 = l5;
              l4 = i0;
              i0 = LoadU32(ctx, i0);
              l5 = i0;
              if (i0) {goto loop_391;}
            block_388:;
            i0 = l4;
            i1 = l12;
            i0 = i0 != i1;
            b = i0;
            if (i0) {goto loop_384;}
          }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 200u;
        i0 += i1;
        c = i0;
        i0 = l3;
        i1 = 120u;
        i0 += i1;
        l4 = i0;
        i0 = l10;
        i1 = 28u;
        i0 += i1;
        b = i0;
      }
      i0 = l6;
      i1 = 58u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 2651u;
        i1 = l8;
        i2 = b;
        i3 = c;
        i4 = l4;
        i5 = 1u;
        HostInitResources(ctx, i0, i1, i2, i3, i4, i5);
        i0 = 58u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto block_346;}
        i0 = l3;
        i1 = 120u;
        i0 += i1;
        i1 = l3;
        i1 = LoadU32(ctx, i1 + 124u);
        ReleaseNodeTree(ctx, i0, i1);
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 211u);
        i1 = kF3661ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 200u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 107u);
        i1 = kF3661ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 96u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l3;
        i1 = 0u;
        StoreU8(ctx, i0 + 96u, i1);
        i0 = 38u;
        l4 = i0;
        i0 = kF3661Const8ADC0;
        l5 = i0;
        loop_397: 
          i0 = l5;
          i1 = l4;
          i2 = 1u;
          i1 >>= (i2 & 31);
          b = i1;
          i2 = 2u;
          i1 <<= (i2 & 31);
          i0 += i1;
          c = i0;
          i1 = 4u;
          i0 += i1;
          i1 = l5;
          i2 = c;
          i2 = LoadU32(ctx, i2);
          i3 = 11u;
          i2 = i2 < i3;
          c = i2;
          i0 = i2 ? i0 : i1;
          l5 = i0;
          i0 = b;
          i1 = kF3661ConstFFFFFFFF;
          i0 ^= i1;
          l8 = i0;
          i1 = l4;
          i0 += i1;
          i1 = b;
          i2 = c;
          i0 = i2 ? i0 : i1;
          l4 = i0;
          if (i0) {goto loop_397;}
        i0 = kF3661Const8AE54;
        i1 = l5;
        i2 = l5;
        i3 = kF3661MsgBuckets;
        i2 = i2 == i3;
        i0 = i2 ? i0 : i1;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = l3;
        j1 = 0ull;
        StoreU64(ctx, i0 + 112u, j1);
        i0 = l3;
        j1 = 4575657221408423936ull;
        StoreU64(ctx, i0 + 104u, j1);
        i0 = l3;
        i1 = b;
        StoreU32(ctx, i0 + 100u, i1);
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 504u);
        b = i0;
        i0 = !(i0);
        c = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      i1 = 0u;
      i2 = c;
      i0 = i2 ? i0 : i1;
      if (i0) {goto block_399;}
      i0 = l6;
      i1 = 59u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = b;
        i1 = kF3661Const8C564;
        i2 = kF3661ConstB8F8C;
        i3 = 0u;
        i0 = CombineComparisonResults(ctx, i0, i1, i2, i3);
        l9 = i0;
        i0 = 59u;
        i1 = ResumePhase(ctx);
        i2 = kF3661ResumeDeallocateRawBlock01;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l9;
        c = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = c;
        i0 = !(i0);
        b = i0;
        if (i0) {goto block_399;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 508u);
        b = i0;
        if (i0) {
          i0 = b;
          i1 = b;
          i1 = LoadU32(ctx, i1 + 4u);
          i2 = 1u;
          i1 += i2;
          StoreU32(ctx, i0 + 4u, i1);
        }
        i0 = l3;
        i1 = 0u;
        StoreU8(ctx, i0 + 200u, i1);
        i0 = l3;
        i1 = 0u;
        StoreU8(ctx, i0 + 211u, i1);
        i0 = c;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 368u);
        l4 = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 96u;
        i0 += i1;
        l8 = i0;
        i0 = l3;
        i1 = 200u;
        i0 += i1;
        l5 = i0;
      }
      i0 = l6;
      i1 = 60u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l4;
        i1 = c;
        i2 = l5;
        i3 = l8;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 60u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        c = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = c;
        i1 = 1u;
        i0 = i0 == i1;
        c = i0;
        if (i0) {goto block_348;}
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 211u);
        i1 = kF3661ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 200u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = b;
        i0 = !(i0);
        c = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = c;
        if (i0) {goto block_406;}
        i0 = b;
        i1 = b;
        i1 = LoadU32(ctx, i1 + 4u);
        c = i1;
        i2 = 1u;
        i1 -= i2;
        l4 = i1;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = c;
        i1 = 1u;
        i0 = i0 != i1;
        c = i0;
        if (i0) {goto block_406;}
        i0 = b;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 8u);
        c = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l6;
      i1 = 61u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = c;
        i1 = b;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 61u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        c = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = c;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_3;}
        i0 = b;
        i1 = b;
        i1 = LoadU32(ctx, i1 + 8u);
        c = i1;
        i2 = 1u;
        i1 -= i2;
        l4 = i1;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = c;
        i1 = 1u;
        i0 = i0 != i1;
        c = i0;
        if (i0) {goto block_406;}
        i0 = b;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        c = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l6;
      i1 = 62u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = c;
        i1 = b;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 62u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto block_3;}
      }
      block_406:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 104u);
        i0 = !(i0);
        b = i0;
        if (i0) {goto block_327;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 116u);
        b = i0;
        if (i0) {goto block_398;}
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l6;
      i1 = 63u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3661MsgBucketsX;
        i2 = kF3661MsgBoostBoostUnorderedDetail;
        i3 = 2956u;
        i4 = kF3661MsgGetBucket;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 63u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_11;}
        goto block_3;
      }
      block_399:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l6;
      i1 = 64u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3661MsgPx;
        i2 = kF3661MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3661MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 64u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_11;}
        goto block_3;
      }
      block_398:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 100u);
        i1 = 2u;
        i0 <<= (i1 & 31);
        i1 = b;
        i0 += i1;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = !(i0);
        c = i0;
        if (i0) {goto block_327;}
        i0 = l3;
        i1 = 48u;
        i0 += i1;
        c = i0;
        i1 = 4u;
        i0 |= i1;
        l8 = i0;
      }
      loop_421: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = a;
          i0 = LoadU32(ctx, i0 + 128u);
          l7 = i0;
          i0 = !(i0);
          c = i0;
        }
        i0 = c;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l6;
          i1 = 65u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kF3661MsgPx;
            i2 = kF3661MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF3661MsgOperator;
            HostFailAssertion(ctx, i0, i1, i2, i3, i4);
            i0 = 65u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3661ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_11;}
            goto block_3;
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i1 = 0u;
          StoreU8(ctx, i0 + 82u, i1);
          i0 = l3;
          i1 = 25705u;
          StoreU16(ctx, i0 + 80u, i1);
          i0 = l3;
          i1 = 2u;
          StoreU8(ctx, i0 + 91u, i1);
          i0 = kF3661ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = l3;
          i2 = 80u;
          i1 += i2;
          StoreU32(ctx, i0 + 520u, i1);
          i0 = l3;
          i1 = 80u;
          i0 += i1;
          l4 = i0;
          i0 = l3;
          i1 = 520u;
          i0 += i1;
          l5 = i0;
          i0 = l3;
          i1 = 512u;
          i0 += i1;
          l10 = i0;
          i0 = l3;
          i1 = kF3661ConstFFFFFFC0;
          i0 -= i1;
          c = i0;
        }
        i0 = l6;
        i1 = 66u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1959u;
          i1 = c;
          i2 = l13;
          i3 = l4;
          i4 = kF3661ConstB5E30;
          i5 = l5;
          i6 = l10;
          HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
          i0 = 66u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3661ConstD015C;
          i0 = LoadU32(ctx, i0);
          c = i0;
          i0 = kF3661ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = c;
          i1 = 1u;
          i0 = i0 != i1;
          c = i0;
        }
        i0 = c;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            i1 = 0u;
            StoreU32(ctx, i0 + 192u, i1);
            i0 = l3;
            j1 = 0ull;
            StoreU64(ctx, i0 + 184u, j1);
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 64u);
            l4 = i0;
            i0 = LoadS8(ctx, i0 + 39u);
            l5 = i0;
            i1 = kF3661ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            if (i0) {
              i0 = l4;
              i0 = LoadU32(ctx, i0 + 32u);
              l5 = i0;
              i0 = l4;
              i0 = LoadU32(ctx, i0 + 28u);
              goto block_437;
            }
            i0 = l5;
            i1 = 255u;
            i0 &= i1;
            l5 = i0;
            i0 = l4;
            i1 = 28u;
            i0 += i1;
            block_437:;
            c = i0;
            i0 = l5;
            i1 = 1u;
            i0 += i1;
            l4 = i0;
            i1 = kF3661ConstFFFFFFF0;
            i0 = i0 >= i1;
            l10 = i0;
          }
          i0 = l10;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3661ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = l3;
              i1 = 184u;
              i0 += i1;
              a = i0;
            }
            i0 = l6;
            i1 = 67u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 1286u;
              i1 = a;
              HostReleaseSlotPair(ctx, i0, i1);
              i0 = 67u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3661ConstD015C;
              i0 = LoadU32(ctx, i0);
              a = i0;
              i0 = kF3661ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = a;
              i1 = 1u;
              i0 = i0 != i1;
              a = i0;
              if (i0) {goto block_11;}
              goto block_434;
            }
          }
          i0 = l4;
          i1 = l4;
          i2 = 11u;
          i1 = i1 >= i2;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          l4 = i0;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3661ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = l5;
              i1 = 17u;
              i0 += i1;
              l4 = i0;
              i1 = kF3661ConstFFFFFFF0;
              i0 &= i1;
              l10 = i0;
            }
            i0 = l6;
            i1 = 68u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 1032u;
              i1 = l10;
              i0 = HostFoldLayoutOffset(ctx, i0, i1);
              l9 = i0;
              i0 = 68u;
              i1 = ResumePhase(ctx);
              i2 = kF3661ResumeDeallocateRawBlock01;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
              i0 = l9;
              l4 = i0;
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3661ConstD015C;
              i0 = LoadU32(ctx, i0);
              l11 = i0;
              i0 = kF3661ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = l11;
              i1 = 1u;
              i0 = i0 == i1;
              l11 = i0;
              if (i0) {goto block_443;}
              i0 = l3;
              i1 = l4;
              StoreU32(ctx, i0 + 184u, i1);
              i0 = l3;
              i1 = l5;
              StoreU32(ctx, i0 + 188u, i1);
              i0 = l3;
              i1 = l10;
              i2 = kF3661Const80000000;
              i1 |= i2;
              l10 = i1;
              StoreU32(ctx, i0 + 192u, i1);
              goto block_445;
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            i1 = l5;
            StoreU8(ctx, i0 + 195u, i1);
            i0 = l3;
            i1 = 184u;
            i0 += i1;
            l4 = i0;
            i0 = l5;
            i0 = !(i0);
            l10 = i0;
            if (i0) {goto block_444;}
          }
          block_445:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l4;
            i1 = c;
            i2 = l5;
            i0 = CopyBytes(ctx, i0, i1, i2);
            c = i0;
          }
          block_444:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l4;
            i1 = l5;
            i0 += i1;
            i1 = 0u;
            StoreU8(ctx, i0, i1);
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 184u;
            i0 += i1;
            c = i0;
          }
          i0 = l6;
          i1 = 69u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 860u;
            i1 = c;
            i2 = kF3661ConstB5354;
            i3 = 1u;
            i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
            l9 = i0;
            i0 = 69u;
            i1 = ResumePhase(ctx);
            i2 = kF3661ResumeDeallocateRawBlock01;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l9;
            c = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3661ConstD015C;
            i0 = LoadU32(ctx, i0);
            c = i0;
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = c;
            i1 = 1u;
            i0 = i0 != i1;
            c = i0;
            if (i0) {goto block_433;}
          }
          block_443:;
          block_434:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = HostRet0(ctx);
            c = i0;
            i0 = HostTempRet0(ctx);
            i0 = l3;
            i0 = LoadS8(ctx, i0 + 195u);
            i1 = kF3661ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            a = i0;
            if (i0) {goto block_349;}
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 184u);
            a = i0;
            DeallocateRawBlock(ctx, i0);
            goto block_349;
          }
          block_433:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = b;
            i0 = LoadU32(ctx, i0 + 12u);
            l4 = i0;
            i0 = b;
            i0 = LoadU8(ctx, i0 + 19u);
            c = i0;
            i0 = b;
            i0 = LoadU32(ctx, i0 + 8u);
            l5 = i0;
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 184u;
            i0 += i1;
            l11 = i0;
            i0 = l5;
            i1 = b;
            i2 = 8u;
            i1 += i2;
            i2 = c;
            i3 = 24u;
            i2 <<= (i3 & 31);
            i3 = 24u;
            i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
            i3 = 0u;
            i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
            l5 = i2;
            i0 = i2 ? i0 : i1;
            l10 = i0;
            i0 = l4;
            i1 = c;
            i2 = l5;
            i0 = i2 ? i0 : i1;
            c = i0;
          }
          i0 = l6;
          i1 = 70u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 860u;
            i1 = l11;
            i2 = l10;
            i3 = c;
            i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
            l9 = i0;
            i0 = 70u;
            i1 = ResumePhase(ctx);
            i2 = kF3661ResumeDeallocateRawBlock01;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l9;
            c = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3661ConstD015C;
            i0 = LoadU32(ctx, i0);
            l4 = i0;
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = 1u;
            i0 = i0 == i1;
            l4 = i0;
            if (i0) {goto block_420;}
            i0 = l3;
            i1 = c;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 208u, i1);
            i0 = l3;
            i1 = c;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 200u, j1);
            i0 = c;
            j1 = 0ull;
            StoreU64(ctx, i0, j1);
            i0 = c;
            i1 = 0u;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = l3;
            j1 = 0ull;
            StoreU64(ctx, i0 + 52u, j1);
            i0 = l3;
            i1 = l8;
            StoreU32(ctx, i0 + 48u, i1);
            i0 = l3;
            i1 = 7u;
            StoreU8(ctx, i0 + 75u, i1);
            i0 = l3;
            i1 = kF3661MsgLyevent;
            i1 = LoadU32(ctx, i1);
            StoreU32(ctx, i0 + 64u, i1);
            i0 = l3;
            i1 = kF3661ConstAF65B;
            i1 = LoadU32(ctx, i1);
            StoreU32(ctx, i0 + 67u, i1);
            i0 = l3;
            i1 = 0u;
            StoreU8(ctx, i0 + 71u, i1);
            i0 = l12;
            i1 = l3;
            i1 = LoadU32(ctx, i1 + 216u);
            l4 = i1;
            i0 = i0 != i1;
            c = i0;
          }
          i0 = c;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            loop_461: 
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = kF3661ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = l3;
                i1 = 48u;
                i0 += i1;
                l5 = i0;
                i0 = l4;
                c = i0;
                i0 = l4;
                i1 = 16u;
                i0 += i1;
                l4 = i0;
              }
              i0 = l6;
              i1 = 71u;
              i0 = i0 == i1;
              i1 = 1u;
              i2 = ResumePhase(ctx);
              i0 = i2 ? i0 : i1;
              if (i0) {
                i0 = 2364u;
                i1 = l5;
                i2 = l8;
                i3 = l4;
                i4 = l4;
                i0 = HostInvokeIiiii(ctx, i0, i1, i2, i3, i4);
                l9 = i0;
                i0 = 71u;
                i1 = ResumePhase(ctx);
                i2 = kF3661ResumeDeallocateRawBlock01;
                i1 = i1 == i2;
                if (i1) {goto spill_frame;}
                i0 = l9;
                l4 = i0;
              }
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = kF3661ConstD015C;
                i0 = LoadU32(ctx, i0);
                l4 = i0;
                i0 = kF3661ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = l4;
                i1 = 1u;
                i0 = i0 == i1;
                l4 = i0;
                if (i0) {goto block_351;}
                i0 = c;
                i0 = LoadU32(ctx, i0 + 4u);
                l5 = i0;
                i0 = !(i0);
                if (i0) {
                  i0 = c;
                  i0 = LoadU32(ctx, i0 + 8u);
                  l4 = i0;
                  i0 = LoadU32(ctx, i0);
                  i1 = c;
                  i0 = i0 == i1;
                  l5 = i0;
                  if (i0) {goto block_465;}
                  i0 = c;
                  i1 = 8u;
                  i0 += i1;
                  c = i0;
                  loop_467: 
                    i0 = c;
                    i0 = LoadU32(ctx, i0);
                    l5 = i0;
                    i1 = 8u;
                    i0 += i1;
                    c = i0;
                    i0 = l5;
                    i0 = LoadU32(ctx, i0 + 8u);
                    l4 = i0;
                    i0 = LoadU32(ctx, i0);
                    l10 = i0;
                    i1 = l5;
                    i0 = i0 != i1;
                    l5 = i0;
                    if (i0) {goto loop_467;}
                  goto block_465;
                }
                loop_468: 
                  i0 = l5;
                  l4 = i0;
                  i0 = LoadU32(ctx, i0);
                  l5 = i0;
                  if (i0) {goto loop_468;}
                block_465:;
                i0 = l4;
                i1 = l12;
                i0 = i0 != i1;
                c = i0;
                if (i0) {goto loop_461;}
              }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = kF3661ConstFFFFFFC0;
            i0 -= i1;
            l4 = i0;
            i0 = l3;
            i1 = 48u;
            i0 += i1;
            l5 = i0;
            i0 = l3;
            i1 = 200u;
            i0 += i1;
            c = i0;
          }
          i0 = l6;
          i1 = 72u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 2651u;
            i1 = l7;
            i2 = c;
            i3 = l4;
            i4 = l5;
            i5 = 1u;
            HostInitResources(ctx, i0, i1, i2, i3, i4, i5);
            i0 = 72u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3661ConstD015C;
            i0 = LoadU32(ctx, i0);
            c = i0;
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = c;
            i1 = 1u;
            i0 = i0 == i1;
            c = i0;
            if (i0) {goto block_419;}
            i0 = l3;
            i1 = 48u;
            i0 += i1;
            i1 = l3;
            i1 = LoadU32(ctx, i1 + 52u);
            l4 = i1;
            ReleaseNodeTree(ctx, i0, i1);
            i0 = l3;
            i0 = LoadS8(ctx, i0 + 75u);
            i1 = kF3661ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            if (i0) {
              i0 = l3;
              i0 = LoadU32(ctx, i0 + 64u);
              DeallocateRawBlock(ctx, i0);
            }
            i0 = l3;
            i0 = LoadS8(ctx, i0 + 211u);
            i1 = kF3661ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            if (i0) {
              i0 = l3;
              i0 = LoadU32(ctx, i0 + 200u);
              DeallocateRawBlock(ctx, i0);
            }
            i0 = l3;
            i0 = LoadS8(ctx, i0 + 195u);
            i1 = kF3661ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            if (i0) {
              i0 = l3;
              i0 = LoadU32(ctx, i0 + 184u);
              DeallocateRawBlock(ctx, i0);
            }
            i0 = l3;
            i0 = LoadS8(ctx, i0 + 91u);
            i1 = kF3661ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            c = i0;
            if (i0) {
              i0 = l3;
              i0 = LoadU32(ctx, i0 + 80u);
              c = i0;
              DeallocateRawBlock(ctx, i0);
            }
            i0 = b;
            i0 = LoadU32(ctx, i0);
            b = i0;
            if (i0) {goto loop_421;}
            goto block_327;
          }
        }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        c = i0;
        i0 = HostTempRet0(ctx);
        a = i0;
        goto block_349;
      }
      block_420:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        c = i0;
        i0 = HostTempRet0(ctx);
        a = i0;
        goto block_350;
      }
      block_419:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_351;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = 0u;
      l5 = i0;
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 100u, i1);
      i0 = l3;
      i1 = kF3661Const65646F6D;
      StoreU32(ctx, i0 + 96u, i1);
      i0 = l3;
      i1 = 4u;
      StoreU8(ctx, i0 + 107u, i1);
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = l3;
      i2 = 96u;
      i1 += i2;
      StoreU32(ctx, i0 + 200u, i1);
      i0 = l3;
      i1 = 96u;
      i0 += i1;
      l8 = i0;
      i0 = l3;
      i1 = 200u;
      i0 += i1;
      l7 = i0;
      i0 = l3;
      i1 = 184u;
      i0 += i1;
      l10 = i0;
      i0 = l3;
      i1 = 216u;
      i0 += i1;
      l4 = i0;
    }
    i0 = l6;
    i1 = 73u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1959u;
      i1 = l4;
      i2 = l13;
      i3 = l8;
      i4 = kF3661ConstB5E30;
      i5 = l7;
      i6 = l10;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 73u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 != i1;
      l4 = i0;
    }
    i0 = l4;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 216u);
        l4 = i0;
        i0 = LoadU32(ctx, i0 + 32u);
        l7 = i0;
        i0 = l4;
        i0 = LoadU8(ctx, i0 + 39u);
        l8 = i0;
        i1 = 24u;
        i0 <<= (i1 & 31);
        i1 = 24u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l10 = i0;
        i0 = l7;
        i1 = l8;
        i2 = l10;
        i0 = i2 ? i0 : i1;
        i1 = 7u;
        i0 = i0 == i1;
        l8 = i0;
      }
      i0 = l8;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3661ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = 28u;
          i0 += i1;
          l4 = i0;
        }
        i0 = l6;
        i1 = 74u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1354u;
          i1 = l4;
          i2 = 0u;
          i3 = kF3661ConstFFFFFFFF;
          i4 = kF3661MsgDisable;
          i5 = 7u;
          i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
          l9 = i0;
          i0 = 74u;
          i1 = ResumePhase(ctx);
          i2 = kF3661ResumeDeallocateRawBlock01;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l9;
          l4 = i0;
        }
        i0 = ResumePhase(ctx);
        if (i0) {
          i0 = l5;
        } else {
          i0 = kF3661ConstD015C;
          i0 = LoadU32(ctx, i0);
          l5 = i0;
          i0 = kF3661ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l5;
          i1 = 1u;
          i0 = i0 == i1;
          if (i0) {goto block_3;}
          i0 = l4;
          i0 = !(i0);
        }
        l5 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 107u);
        i1 = kF3661ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 96u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l5;
        i0 = !(i0);
        if (i0) {goto running_exit;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 504u);
        l4 = i0;
        i0 = !(i0);
        l5 = i0;
      }
      i0 = l5;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3661ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l6;
        i1 = 75u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3661MsgPx;
          i2 = kF3661MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3661MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 75u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3661ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3661ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_11;}
          goto block_3;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 276u);
        l5 = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l6;
      i1 = 76u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l5;
        i1 = l4;
        i2 = l12;
        i3 = 0u;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 76u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_130;}
        i0 = c;
        if (i0) {goto running_exit;}
        i0 = l3;
        j1 = 0ull;
        StoreU64(ctx, i0 + 220u, j1);
        i0 = l3;
        i1 = l3;
        i2 = 216u;
        i1 += i2;
        i2 = 4u;
        i1 |= i2;
        l12 = i1;
        StoreU32(ctx, i0 + 216u, i1);
        i0 = l3;
        i1 = 0u;
        StoreU8(ctx, i0 + 100u, i1);
        i0 = l3;
        i1 = kF3661Const65646F6D;
        StoreU32(ctx, i0 + 96u, i1);
        i0 = l3;
        i1 = 4u;
        StoreU8(ctx, i0 + 107u, i1);
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 216u;
        i0 += i1;
        l4 = i0;
        i0 = l3;
        i1 = 96u;
        i0 += i1;
        l5 = i0;
        i0 = l3;
        i1 = 96u;
        i0 += i1;
        l8 = i0;
        i0 = l3;
        i1 = 200u;
        i0 += i1;
        c = i0;
      }
      i0 = l6;
      i1 = 77u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 2652u;
        i1 = c;
        i2 = l4;
        i3 = l5;
        i4 = l8;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 77u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        c = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = c;
        i1 = 1u;
        i0 = i0 != i1;
        c = i0;
      }
      i0 = c;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i0 = LoadS8(ctx, i0 + 107u);
          i1 = kF3661ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 96u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l3;
          i1 = 0u;
          StoreU8(ctx, i0 + 100u, i1);
          i0 = l3;
          i1 = kF3661Const65707974;
          StoreU32(ctx, i0 + 96u, i1);
          i0 = l3;
          i1 = 4u;
          StoreU8(ctx, i0 + 107u, i1);
          i0 = kF3661ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = 216u;
          i0 += i1;
          l4 = i0;
          i0 = l3;
          i1 = 96u;
          i0 += i1;
          l5 = i0;
          i0 = l3;
          i1 = 96u;
          i0 += i1;
          l8 = i0;
          i0 = l3;
          i1 = 200u;
          i0 += i1;
          c = i0;
        }
        i0 = l6;
        i1 = 78u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2652u;
          i1 = c;
          i2 = l4;
          i3 = l5;
          i4 = l8;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 78u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3661ConstD015C;
          i0 = LoadU32(ctx, i0);
          c = i0;
          i0 = kF3661ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = c;
          i1 = 1u;
          i0 = i0 != i1;
          c = i0;
        }
        i0 = c;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            i0 = LoadS8(ctx, i0 + 107u);
            i1 = kF3661ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            if (i0) {
              i0 = l3;
              i0 = LoadU32(ctx, i0 + 96u);
              DeallocateRawBlock(ctx, i0);
            }
            i0 = a;
            i0 = LoadU32(ctx, i0 + 128u);
            l8 = i0;
            i0 = !(i0);
            c = i0;
          }
          i0 = c;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3661ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
            }
            i0 = l6;
            i1 = 79u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 857u;
              i1 = kF3661MsgPx;
              i2 = kF3661MsgBoostBoostSmartPtr;
              i3 = 734u;
              i4 = kF3661MsgOperator;
              HostFailAssertion(ctx, i0, i1, i2, i3, i4);
              i0 = 79u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3661ConstD015C;
              i0 = LoadU32(ctx, i0);
              a = i0;
              i0 = kF3661ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = a;
              i1 = 1u;
              i0 = i0 != i1;
              a = i0;
              if (i0) {goto block_11;}
              goto block_3;
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            j1 = 0ull;
            StoreU64(ctx, i0 + 36u, j1);
            i0 = l3;
            i1 = l3;
            i2 = 32u;
            i1 += i2;
            i2 = 4u;
            i1 |= i2;
            l7 = i1;
            StoreU32(ctx, i0 + 32u, i1);
            i0 = l12;
            i1 = l3;
            i1 = LoadU32(ctx, i1 + 216u);
            l4 = i1;
            i0 = i0 != i1;
            c = i0;
          }
          i0 = c;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            loop_527: 
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = kF3661ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = l3;
                i1 = 32u;
                i0 += i1;
                l5 = i0;
                i0 = l4;
                c = i0;
                i0 = l4;
                i1 = 16u;
                i0 += i1;
                l4 = i0;
              }
              i0 = l6;
              i1 = 80u;
              i0 = i0 == i1;
              i1 = 1u;
              i2 = ResumePhase(ctx);
              i0 = i2 ? i0 : i1;
              if (i0) {
                i0 = 2386u;
                i1 = l5;
                i2 = l7;
                i3 = l4;
                i4 = l4;
                i0 = HostInvokeIiiii(ctx, i0, i1, i2, i3, i4);
                l9 = i0;
                i0 = 80u;
                i1 = ResumePhase(ctx);
                i2 = kF3661ResumeDeallocateRawBlock01;
                i1 = i1 == i2;
                if (i1) {goto spill_frame;}
                i0 = l9;
                l4 = i0;
              }
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = kF3661ConstD015C;
                i0 = LoadU32(ctx, i0);
                l4 = i0;
                i0 = kF3661ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = l4;
                i1 = 1u;
                i0 = i0 == i1;
                l4 = i0;
                if (i0) {goto block_523;}
                i0 = c;
                i0 = LoadU32(ctx, i0 + 4u);
                l5 = i0;
                i0 = !(i0);
                if (i0) {
                  i0 = c;
                  i0 = LoadU32(ctx, i0 + 8u);
                  l4 = i0;
                  i0 = LoadU32(ctx, i0);
                  i1 = c;
                  i0 = i0 == i1;
                  l5 = i0;
                  if (i0) {goto block_531;}
                  i0 = c;
                  i1 = 8u;
                  i0 += i1;
                  c = i0;
                  loop_533: 
                    i0 = c;
                    i0 = LoadU32(ctx, i0);
                    l5 = i0;
                    i1 = 8u;
                    i0 += i1;
                    c = i0;
                    i0 = l5;
                    i0 = LoadU32(ctx, i0 + 8u);
                    l4 = i0;
                    i0 = LoadU32(ctx, i0);
                    l10 = i0;
                    i1 = l5;
                    i0 = i0 != i1;
                    l5 = i0;
                    if (i0) {goto loop_533;}
                  goto block_531;
                }
                loop_534: 
                  i0 = l5;
                  l4 = i0;
                  i0 = LoadU32(ctx, i0);
                  l5 = i0;
                  if (i0) {goto loop_534;}
                block_531:;
                i0 = l4;
                i1 = l12;
                i0 = i0 != i1;
                c = i0;
                if (i0) {goto loop_527;}
              }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 32u;
            i0 += i1;
            c = i0;
          }
          i0 = l6;
          i1 = 81u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 2645u;
            i1 = l8;
            i2 = b;
            i3 = c;
            i4 = 1u;
            HostFailAssertion(ctx, i0, i1, i2, i3, i4);
            i0 = 81u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3661ConstD015C;
            i0 = LoadU32(ctx, i0);
            c = i0;
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = c;
            i1 = 1u;
            i0 = i0 == i1;
            c = i0;
            if (i0) {goto block_516;}
            i0 = l3;
            i1 = 32u;
            i0 += i1;
            i1 = l3;
            i1 = LoadU32(ctx, i1 + 36u);
            ReleaseLayerNodeTree(ctx, i0, i1);
            i0 = l3;
            i1 = 0u;
            StoreU8(ctx, i0 + 96u, i1);
            i0 = 38u;
            l4 = i0;
            i0 = kF3661Const8ADC0;
            l5 = i0;
            loop_538: 
              i0 = l4;
              i1 = 1u;
              i0 >>= (i1 & 31);
              c = i0;
              i1 = 2u;
              i0 <<= (i1 & 31);
              i1 = l5;
              i0 += i1;
              l8 = i0;
              i1 = 4u;
              i0 += i1;
              i1 = l5;
              i2 = l8;
              i2 = LoadU32(ctx, i2);
              i3 = 11u;
              i2 = i2 < i3;
              l8 = i2;
              i0 = i2 ? i0 : i1;
              l5 = i0;
              i0 = c;
              i1 = kF3661ConstFFFFFFFF;
              i0 ^= i1;
              l7 = i0;
              i1 = l4;
              i0 += i1;
              i1 = c;
              i2 = l8;
              i0 = i2 ? i0 : i1;
              l4 = i0;
              if (i0) {goto loop_538;}
            i0 = kF3661Const8AE54;
            i1 = l5;
            i2 = l5;
            i3 = kF3661MsgBuckets;
            i2 = i2 == i3;
            i0 = i2 ? i0 : i1;
            i0 = LoadU32(ctx, i0);
            c = i0;
            i0 = l3;
            j1 = 0ull;
            StoreU64(ctx, i0 + 112u, j1);
            i0 = l3;
            j1 = 4575657221408423936ull;
            StoreU64(ctx, i0 + 104u, j1);
            i0 = l3;
            i1 = c;
            StoreU32(ctx, i0 + 100u, i1);
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 504u);
            c = i0;
            i0 = !(i0);
            l4 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          i1 = 0u;
          i2 = l4;
          i0 = i2 ? i0 : i1;
          if (i0) {goto block_541;}
          i0 = l6;
          i1 = 82u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = c;
            i1 = kF3661Const8C564;
            i2 = kF3661ConstB8F8C;
            i3 = 0u;
            i0 = CombineComparisonResults(ctx, i0, i1, i2, i3);
            l9 = i0;
            i0 = 82u;
            i1 = ResumePhase(ctx);
            i2 = kF3661ResumeDeallocateRawBlock01;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l9;
            c = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = c;
            i0 = !(i0);
            l4 = i0;
            if (i0) {goto block_541;}
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 508u);
            l4 = i0;
            if (i0) {
              i0 = l4;
              i1 = l4;
              i1 = LoadU32(ctx, i1 + 4u);
              i2 = 1u;
              i1 += i2;
              StoreU32(ctx, i0 + 4u, i1);
            }
            i0 = l3;
            i1 = 0u;
            StoreU8(ctx, i0 + 200u, i1);
            i0 = l3;
            i1 = 0u;
            StoreU8(ctx, i0 + 211u, i1);
            i0 = c;
            i0 = LoadU32(ctx, i0);
            i0 = LoadU32(ctx, i0 + 368u);
            l5 = i0;
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 200u;
            i0 += i1;
            l8 = i0;
            i0 = l3;
            i1 = 96u;
            i0 += i1;
            l7 = i0;
          }
          i0 = l6;
          i1 = 83u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l5;
            i1 = c;
            i2 = l8;
            i3 = l7;
            HostSyncSlot(ctx, i0, i1, i2, i3);
            i0 = 83u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3661ConstD015C;
            i0 = LoadU32(ctx, i0);
            c = i0;
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = c;
            i1 = 1u;
            i0 = i0 == i1;
            c = i0;
            if (i0) {goto block_518;}
            i0 = l3;
            i0 = LoadS8(ctx, i0 + 211u);
            i1 = kF3661ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            if (i0) {
              i0 = l3;
              i0 = LoadU32(ctx, i0 + 200u);
              DeallocateRawBlock(ctx, i0);
            }
            i0 = l4;
            i0 = !(i0);
            c = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = c;
            if (i0) {goto block_548;}
            i0 = l4;
            i1 = l4;
            i1 = LoadU32(ctx, i1 + 4u);
            c = i1;
            i2 = 1u;
            i1 -= i2;
            l5 = i1;
            StoreU32(ctx, i0 + 4u, i1);
            i0 = c;
            i1 = 1u;
            i0 = i0 != i1;
            c = i0;
            if (i0) {goto block_548;}
            i0 = l4;
            i0 = LoadU32(ctx, i0);
            i0 = LoadU32(ctx, i0 + 8u);
            c = i0;
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l6;
          i1 = 84u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = c;
            i1 = l4;
            HostReleaseSlotPair(ctx, i0, i1);
            i0 = 84u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3661ConstD015C;
            i0 = LoadU32(ctx, i0);
            c = i0;
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = c;
            i1 = 1u;
            i0 = i0 == i1;
            if (i0) {goto block_3;}
            i0 = l4;
            i1 = l4;
            i1 = LoadU32(ctx, i1 + 8u);
            c = i1;
            i2 = 1u;
            i1 -= i2;
            l5 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = c;
            i1 = 1u;
            i0 = i0 != i1;
            c = i0;
            if (i0) {goto block_548;}
            i0 = l4;
            i0 = LoadU32(ctx, i0);
            i0 = LoadU32(ctx, i0 + 12u);
            c = i0;
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l6;
          i1 = 85u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = c;
            i1 = l4;
            HostReleaseSlotPair(ctx, i0, i1);
            i0 = 85u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3661ConstD015C;
            i0 = LoadU32(ctx, i0);
            c = i0;
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = c;
            i1 = 1u;
            i0 = i0 == i1;
            c = i0;
            if (i0) {goto block_3;}
          }
          block_548:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            i1 = 0u;
            StoreU8(ctx, i0 + 186u, i1);
            i0 = l3;
            i1 = 25705u;
            StoreU16(ctx, i0 + 184u, i1);
            i0 = l3;
            i1 = 2u;
            StoreU8(ctx, i0 + 195u, i1);
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = l3;
            i2 = 184u;
            i1 += i2;
            StoreU32(ctx, i0 + 64u, i1);
            i0 = l3;
            i1 = 184u;
            i0 += i1;
            l4 = i0;
            i0 = l3;
            i1 = kF3661ConstFFFFFFC0;
            i0 -= i1;
            l5 = i0;
            i0 = l3;
            i1 = 520u;
            i0 += i1;
            l8 = i0;
            i0 = l3;
            i1 = 80u;
            i0 += i1;
            c = i0;
          }
          i0 = l6;
          i1 = 86u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 1959u;
            i1 = c;
            i2 = l13;
            i3 = l4;
            i4 = kF3661ConstB5E30;
            i5 = l5;
            i6 = l8;
            HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
            i0 = 86u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3661ConstD015C;
            i0 = LoadU32(ctx, i0);
            c = i0;
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = c;
            i1 = 1u;
            i0 = i0 == i1;
            c = i0;
            if (i0) {goto block_520;}
            i0 = l3;
            i1 = 0u;
            StoreU32(ctx, i0 + 208u, i1);
            i0 = l3;
            j1 = 0ull;
            StoreU64(ctx, i0 + 200u, j1);
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 80u);
            l4 = i0;
            i0 = LoadS8(ctx, i0 + 39u);
            l5 = i0;
            i1 = kF3661ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            c = i0;
            if (i0) {goto block_540;}
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 32u);
            l5 = i0;
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 28u);
            goto block_539;
          }
          block_541:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l6;
          i1 = 87u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kF3661MsgPx;
            i2 = kF3661MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF3661MsgOperator;
            HostFailAssertion(ctx, i0, i1, i2, i3, i4);
            i0 = 87u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3661ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_11;}
            goto block_3;
          }
          block_540:;
          i0 = ResumePhase(ctx);
          if (i0) {
            i0 = c;
          } else {
            i0 = l5;
            i1 = 255u;
            i0 &= i1;
            l5 = i0;
            i0 = l4;
            i1 = 28u;
            i0 += i1;
          }
          block_539:;
          c = i0;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l5;
            i1 = 1u;
            i0 += i1;
            l4 = i0;
            i1 = kF3661ConstFFFFFFF0;
            i0 = i0 >= i1;
            l8 = i0;
          }
          i0 = l8;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3661ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = l3;
              i1 = 200u;
              i0 += i1;
              a = i0;
            }
            i0 = l6;
            i1 = 88u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 1286u;
              i1 = a;
              HostReleaseSlotPair(ctx, i0, i1);
              i0 = 88u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3661ConstD015C;
              i0 = LoadU32(ctx, i0);
              a = i0;
              i0 = kF3661ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = a;
              i1 = 1u;
              i0 = i0 == i1;
              a = i0;
              if (i0) {goto block_562;}
              goto block_11;
            }
          }
          i0 = l4;
          i1 = l4;
          i2 = 11u;
          i1 = i1 >= i2;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          l4 = i0;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3661ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = l5;
              i1 = 17u;
              i0 += i1;
              l4 = i0;
              i1 = kF3661ConstFFFFFFF0;
              i0 &= i1;
              l8 = i0;
            }
            i0 = l6;
            i1 = 89u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 1032u;
              i1 = l8;
              i0 = HostFoldLayoutOffset(ctx, i0, i1);
              l9 = i0;
              i0 = 89u;
              i1 = ResumePhase(ctx);
              i2 = kF3661ResumeDeallocateRawBlock01;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
              i0 = l9;
              l4 = i0;
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3661ConstD015C;
              i0 = LoadU32(ctx, i0);
              l7 = i0;
              i0 = kF3661ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = l7;
              i1 = 1u;
              i0 = i0 == i1;
              l7 = i0;
              if (i0) {goto block_562;}
              i0 = l3;
              i1 = l4;
              StoreU32(ctx, i0 + 200u, i1);
              i0 = l3;
              i1 = l5;
              StoreU32(ctx, i0 + 204u, i1);
              i0 = l3;
              i1 = l8;
              i2 = kF3661Const80000000;
              i1 |= i2;
              l8 = i1;
              StoreU32(ctx, i0 + 208u, i1);
              goto block_570;
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            i1 = l5;
            StoreU8(ctx, i0 + 211u, i1);
            i0 = l3;
            i1 = 200u;
            i0 += i1;
            l4 = i0;
            i0 = l5;
            i0 = !(i0);
            l8 = i0;
            if (i0) {goto block_569;}
          }
          block_570:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l4;
            i1 = c;
            i2 = l5;
            i0 = CopyBytes(ctx, i0, i1, i2);
            c = i0;
          }
          block_569:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l4;
            i1 = l5;
            i0 += i1;
            i1 = 0u;
            StoreU8(ctx, i0, i1);
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 200u;
            i0 += i1;
            c = i0;
          }
          i0 = l6;
          i1 = 90u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 860u;
            i1 = c;
            i2 = kF3661ConstB5354;
            i3 = 1u;
            i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
            l9 = i0;
            i0 = 90u;
            i1 = ResumePhase(ctx);
            i2 = kF3661ResumeDeallocateRawBlock01;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l9;
            c = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3661ConstD015C;
            i0 = LoadU32(ctx, i0);
            c = i0;
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = c;
            i1 = 1u;
            i0 = i0 != i1;
            c = i0;
            if (i0) {goto block_561;}
          }
          block_562:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = HostRet0(ctx);
            c = i0;
            i0 = HostTempRet0(ctx);
            i0 = l3;
            i0 = LoadS8(ctx, i0 + 211u);
            i1 = kF3661ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            a = i0;
            if (i0) {goto block_519;}
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 200u);
            a = i0;
            DeallocateRawBlock(ctx, i0);
            goto block_519;
          }
          block_561:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            i0 = LoadS8(ctx, i0 + 195u);
            i1 = kF3661ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            if (i0) {
              i0 = l3;
              i0 = LoadU32(ctx, i0 + 184u);
              DeallocateRawBlock(ctx, i0);
            }
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 104u);
            i0 = !(i0);
            c = i0;
            if (i0) {goto block_480;}
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 116u);
            c = i0;
            i0 = !(i0);
            l4 = i0;
          }
          i0 = l4;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3661ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
            }
            i0 = l6;
            i1 = 91u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 857u;
              i1 = kF3661MsgBucketsX;
              i2 = kF3661MsgBoostBoostUnorderedDetail;
              i3 = 2956u;
              i4 = kF3661MsgGetBucket;
              HostFailAssertion(ctx, i0, i1, i2, i3, i4);
              i0 = 91u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3661ConstD015C;
              i0 = LoadU32(ctx, i0);
              a = i0;
              i0 = kF3661ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = a;
              i1 = 1u;
              i0 = i0 != i1;
              a = i0;
              if (i0) {goto block_11;}
              goto block_3;
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 100u);
            i1 = 2u;
            i0 <<= (i1 & 31);
            l4 = i0;
            i1 = c;
            i0 += i1;
            i0 = LoadU32(ctx, i0);
            l8 = i0;
            i0 = !(i0);
            c = i0;
            if (i0) {goto block_480;}
            i0 = l3;
            i1 = 16u;
            i0 += i1;
            c = i0;
            i1 = 4u;
            i0 |= i1;
            l7 = i0;
          }
          loop_590: 
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3661ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = l3;
              i1 = 200u;
              i0 += i1;
              l4 = i0;
              i0 = l8;
              i1 = 8u;
              i0 += i1;
              l5 = i0;
              i0 = l3;
              i1 = 184u;
              i0 += i1;
              c = i0;
            }
            i0 = l6;
            i1 = 92u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 2653u;
              i1 = c;
              i2 = l4;
              i3 = l5;
              HostSyncSlot(ctx, i0, i1, i2, i3);
              i0 = 92u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3661ConstD015C;
              i0 = LoadU32(ctx, i0);
              c = i0;
              i0 = kF3661ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = c;
              i1 = 1u;
              i0 = i0 == i1;
              c = i0;
              if (i0) {goto block_591;}
              i0 = l3;
              i1 = 0u;
              StoreU8(ctx, i0 + 82u, i1);
              i0 = l3;
              i1 = 25705u;
              StoreU16(ctx, i0 + 80u, i1);
              i0 = l3;
              i1 = 2u;
              StoreU8(ctx, i0 + 91u, i1);
              i0 = kF3661ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = l3;
              i1 = l3;
              i2 = 80u;
              i1 += i2;
              StoreU32(ctx, i0 + 520u, i1);
              i0 = l3;
              i1 = 80u;
              i0 += i1;
              l4 = i0;
              i0 = l3;
              i1 = 520u;
              i0 += i1;
              l5 = i0;
              i0 = l3;
              i1 = 512u;
              i0 += i1;
              l10 = i0;
              i0 = l3;
              i1 = kF3661ConstFFFFFFC0;
              i0 -= i1;
              c = i0;
            }
            i0 = l6;
            i1 = 93u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 1959u;
              i1 = c;
              i2 = l13;
              i3 = l4;
              i4 = kF3661ConstB5E30;
              i5 = l5;
              i6 = l10;
              HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
              i0 = 93u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3661ConstD015C;
              i0 = LoadU32(ctx, i0);
              c = i0;
              i0 = kF3661ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = c;
              i1 = 1u;
              i0 = i0 == i1;
              c = i0;
              if (i0) {goto block_589;}
              i0 = l3;
              i0 = LoadU32(ctx, i0 + 64u);
              l4 = i0;
              c = i0;
              i0 = l4;
              i0 = LoadS8(ctx, i0 + 39u);
              i1 = kF3661ConstFFFFFFFF;
              i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
              if (i0) {
                i0 = c;
                i0 = LoadU32(ctx, i0 + 28u);
                DeallocateRawBlock(ctx, i0);
              }
              i0 = c;
              i1 = l3;
              j1 = LoadU64(ctx, i1 + 184u);
              StoreU64(ctx, i0 + 28u, j1);
              i0 = c;
              i1 = l3;
              i1 = LoadU32(ctx, i1 + 192u);
              l4 = i1;
              StoreU32(ctx, i0 + 36u, i1);
              i0 = l3;
              i1 = 0u;
              StoreU8(ctx, i0 + 195u, i1);
              i0 = l3;
              i1 = 0u;
              StoreU8(ctx, i0 + 184u, i1);
              i0 = l3;
              i0 = LoadS8(ctx, i0 + 91u);
              i1 = 0u;
              i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
              if (i0) {goto block_598;}
              i0 = l3;
              i0 = LoadU32(ctx, i0 + 80u);
              DeallocateRawBlock(ctx, i0);
              i0 = l3;
              i0 = LoadS8(ctx, i0 + 195u);
              i1 = kF3661ConstFFFFFFFF;
              i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
              if (i0) {goto block_598;}
              i0 = l3;
              i0 = LoadU32(ctx, i0 + 184u);
              DeallocateRawBlock(ctx, i0);
              block_598:;
              i0 = a;
              i0 = LoadU32(ctx, i0 + 128u);
              l10 = i0;
              i0 = !(i0);
              c = i0;
            }
            i0 = c;
            i1 = ResumePhase(ctx);
            i2 = 2u;
            i1 = i1 == i2;
            i0 |= i1;
            if (i0) {
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = kF3661ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
              }
              i0 = l6;
              i1 = 94u;
              i0 = i0 == i1;
              i1 = 1u;
              i2 = ResumePhase(ctx);
              i0 = i2 ? i0 : i1;
              if (i0) {
                i0 = 857u;
                i1 = kF3661MsgPx;
                i2 = kF3661MsgBoostBoostSmartPtr;
                i3 = 734u;
                i4 = kF3661MsgOperator;
                HostFailAssertion(ctx, i0, i1, i2, i3, i4);
                i0 = 94u;
                i1 = ResumePhase(ctx);
                i2 = 1u;
                i1 = i1 == i2;
                if (i1) {goto spill_frame;}
              }
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = kF3661ConstD015C;
                i0 = LoadU32(ctx, i0);
                a = i0;
                i0 = kF3661ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = a;
                i1 = 1u;
                i0 = i0 != i1;
                a = i0;
                if (i0) {goto block_11;}
                goto block_3;
              }
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = l3;
              j1 = 0ull;
              StoreU64(ctx, i0 + 20u, j1);
              i0 = l3;
              i1 = l7;
              StoreU32(ctx, i0 + 16u, i1);
              i0 = l12;
              i1 = l3;
              i1 = LoadU32(ctx, i1 + 216u);
              l4 = i1;
              i0 = i0 != i1;
              c = i0;
            }
            i0 = c;
            i1 = ResumePhase(ctx);
            i2 = 2u;
            i1 = i1 == i2;
            i0 |= i1;
            if (i0) {
              loop_606: 
                i0 = ResumePhase(ctx);
                i0 = !(i0);
                if (i0) {
                  i0 = kF3661ConstD015C;
                  i1 = 0u;
                  StoreU32(ctx, i0, i1);
                  i0 = l3;
                  i1 = 16u;
                  i0 += i1;
                  l5 = i0;
                  i0 = l4;
                  c = i0;
                  i0 = l4;
                  i1 = 16u;
                  i0 += i1;
                  l4 = i0;
                }
                i0 = l6;
                i1 = 95u;
                i0 = i0 == i1;
                i1 = 1u;
                i2 = ResumePhase(ctx);
                i0 = i2 ? i0 : i1;
                if (i0) {
                  i0 = 2386u;
                  i1 = l5;
                  i2 = l7;
                  i3 = l4;
                  i4 = l4;
                  i0 = HostInvokeIiiii(ctx, i0, i1, i2, i3, i4);
                  l9 = i0;
                  i0 = 95u;
                  i1 = ResumePhase(ctx);
                  i2 = kF3661ResumeDeallocateRawBlock01;
                  i1 = i1 == i2;
                  if (i1) {goto spill_frame;}
                  i0 = l9;
                  l4 = i0;
                }
                i0 = ResumePhase(ctx);
                i0 = !(i0);
                if (i0) {
                  i0 = kF3661ConstD015C;
                  i0 = LoadU32(ctx, i0);
                  l4 = i0;
                  i0 = kF3661ConstD015C;
                  i1 = 0u;
                  StoreU32(ctx, i0, i1);
                  i0 = l4;
                  i1 = 1u;
                  i0 = i0 == i1;
                  l4 = i0;
                  if (i0) {goto block_522;}
                  i0 = c;
                  i0 = LoadU32(ctx, i0 + 4u);
                  l5 = i0;
                  i0 = !(i0);
                  if (i0) {
                    i0 = c;
                    i0 = LoadU32(ctx, i0 + 8u);
                    l4 = i0;
                    i0 = LoadU32(ctx, i0);
                    i1 = c;
                    i0 = i0 == i1;
                    l5 = i0;
                    if (i0) {goto block_610;}
                    i0 = c;
                    i1 = 8u;
                    i0 += i1;
                    c = i0;
                    loop_612: 
                      i0 = c;
                      i0 = LoadU32(ctx, i0);
                      l5 = i0;
                      i1 = 8u;
                      i0 += i1;
                      c = i0;
                      i0 = l5;
                      i0 = LoadU32(ctx, i0 + 8u);
                      l4 = i0;
                      i0 = LoadU32(ctx, i0);
                      i1 = l5;
                      i0 = i0 != i1;
                      l5 = i0;
                      if (i0) {goto loop_612;}
                    goto block_610;
                  }
                  loop_613: 
                    i0 = l5;
                    l4 = i0;
                    i0 = LoadU32(ctx, i0);
                    l5 = i0;
                    if (i0) {goto loop_613;}
                  block_610:;
                  i0 = l4;
                  i1 = l12;
                  i0 = i0 != i1;
                  c = i0;
                  if (i0) {goto loop_606;}
                }
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3661ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = l3;
              i1 = 16u;
              i0 += i1;
              c = i0;
            }
            i0 = l6;
            i1 = 96u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 2645u;
              i1 = l10;
              i2 = b;
              i3 = c;
              i4 = 1u;
              HostFailAssertion(ctx, i0, i1, i2, i3, i4);
              i0 = 96u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3661ConstD015C;
              i0 = LoadU32(ctx, i0);
              c = i0;
              i0 = kF3661ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = c;
              i1 = 1u;
              i0 = i0 == i1;
              c = i0;
              if (i0) {goto block_588;}
              i0 = l3;
              i1 = 16u;
              i0 += i1;
              c = i0;
              i1 = l3;
              i1 = LoadU32(ctx, i1 + 20u);
              l4 = i1;
              ReleaseLayerNodeTree(ctx, i0, i1);
              i0 = l8;
              i0 = LoadU32(ctx, i0);
              l8 = i0;
              if (i0) {goto loop_590;}
              goto block_480;
            }
            block_591:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = HostRet0(ctx);
            c = i0;
            i0 = HostTempRet0(ctx);
            a = i0;
            goto block_521;
          }
          block_589:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = HostRet0(ctx);
            c = i0;
            i0 = HostTempRet0(ctx);
            i0 = l3;
            i0 = LoadS8(ctx, i0 + 91u);
            i1 = kF3661ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            if (i0) {
              i0 = l3;
              i0 = LoadU32(ctx, i0 + 80u);
              DeallocateRawBlock(ctx, i0);
            }
            i0 = l3;
            i0 = LoadS8(ctx, i0 + 195u);
            i1 = kF3661ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            a = i0;
            if (i0) {goto block_521;}
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 184u);
            a = i0;
            DeallocateRawBlock(ctx, i0);
            goto block_521;
          }
          block_588:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = HostRet0(ctx);
            c = i0;
            i0 = HostTempRet0(ctx);
            i0 = l3;
            i1 = 16u;
            i0 += i1;
            a = i0;
            i1 = l3;
            i1 = LoadU32(ctx, i1 + 20u);
            b = i1;
            ReleaseLayerNodeTree(ctx, i0, i1);
            goto block_521;
          }
          block_523:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = HostRet0(ctx);
            c = i0;
            i0 = HostTempRet0(ctx);
            i0 = l3;
            i1 = 32u;
            i0 += i1;
            a = i0;
            i1 = l3;
            i1 = LoadU32(ctx, i1 + 36u);
            b = i1;
            ReleaseLayerNodeTree(ctx, i0, i1);
            goto block_500;
          }
          block_522:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = HostRet0(ctx);
            c = i0;
            i0 = HostTempRet0(ctx);
            i0 = l3;
            i1 = 16u;
            i0 += i1;
            a = i0;
            i1 = l3;
            i1 = LoadU32(ctx, i1 + 20u);
            b = i1;
            ReleaseLayerNodeTree(ctx, i0, i1);
          }
          block_521:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            i0 = LoadS8(ctx, i0 + 211u);
            i1 = kF3661ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            a = i0;
            if (i0) {goto block_517;}
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 200u);
            a = i0;
            DeallocateRawBlock(ctx, i0);
            goto block_517;
          }
          block_520:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = HostRet0(ctx);
            c = i0;
            i0 = HostTempRet0(ctx);
            a = i0;
          }
          block_519:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            i0 = LoadS8(ctx, i0 + 195u);
            i1 = kF3661ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            a = i0;
            if (i0) {goto block_517;}
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 184u);
            a = i0;
            DeallocateRawBlock(ctx, i0);
            goto block_517;
          }
          block_518:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = HostRet0(ctx);
            c = i0;
            i0 = HostTempRet0(ctx);
            i0 = l3;
            i0 = LoadS8(ctx, i0 + 211u);
            i1 = kF3661ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            if (i0) {
              i0 = l3;
              i0 = LoadU32(ctx, i0 + 200u);
              DeallocateRawBlock(ctx, i0);
            }
            i0 = l4;
            i0 = !(i0);
            a = i0;
            if (i0) {goto block_517;}
            i0 = l4;
            i1 = l4;
            i1 = LoadU32(ctx, i1 + 4u);
            a = i1;
            i2 = 1u;
            i1 -= i2;
            b = i1;
            StoreU32(ctx, i0 + 4u, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_517;}
            i0 = l4;
            i0 = LoadU32(ctx, i0);
            i0 = LoadU32(ctx, i0 + 8u);
            a = i0;
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l6;
          i1 = 97u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = a;
            i1 = l4;
            HostReleaseSlotPair(ctx, i0, i1);
            i0 = 97u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3661ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
          }
          i0 = a;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = l4;
              i1 = l4;
              i1 = LoadU32(ctx, i1 + 8u);
              a = i1;
              i2 = 1u;
              i1 -= i2;
              b = i1;
              StoreU32(ctx, i0 + 8u, i1);
              i0 = a;
              i1 = 1u;
              i0 = i0 != i1;
              a = i0;
              if (i0) {goto block_517;}
              i0 = l4;
              i0 = LoadU32(ctx, i0);
              i0 = LoadU32(ctx, i0 + 12u);
              a = i0;
              i0 = kF3661ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
            }
            i0 = l6;
            i1 = 98u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = a;
              i1 = l4;
              HostReleaseSlotPair(ctx, i0, i1);
              i0 = 98u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3661ConstD015C;
              i0 = LoadU32(ctx, i0);
              a = i0;
              i0 = kF3661ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = a;
              i1 = 1u;
              i0 = i0 != i1;
              a = i0;
              if (i0) {goto block_517;}
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {goto block_3;}
          block_517:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 96u;
            i0 += i1;
            a = i0;
          }
          i0 = l6;
          i1 = 99u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 2654u;
            i1 = a;
            HostReleaseSlotPair(ctx, i0, i1);
            i0 = 99u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3661ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3661ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_500;}
            goto block_3;
          }
          block_516:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = HostRet0(ctx);
            c = i0;
            i0 = HostTempRet0(ctx);
            i0 = l3;
            i1 = 32u;
            i0 += i1;
            a = i0;
            i1 = l3;
            i1 = LoadU32(ctx, i1 + 36u);
            b = i1;
            ReleaseLayerNodeTree(ctx, i0, i1);
            goto block_500;
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          c = i0;
          i0 = HostTempRet0(ctx);
          i0 = l3;
          i0 = LoadS8(ctx, i0 + 107u);
          i1 = kF3661ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_500;}
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 96u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
          goto block_500;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        c = i0;
        i0 = HostTempRet0(ctx);
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 107u);
        i1 = kF3661ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        a = i0;
        if (i0) {goto block_500;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 96u);
        a = i0;
        DeallocateRawBlock(ctx, i0);
      }
      block_500:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i1 = 216u;
        i0 += i1;
        a = i0;
        i1 = l3;
        i1 = LoadU32(ctx, i1 + 220u);
        b = i1;
        ReleaseLayerNodeTree(ctx, i0, i1);
        goto block_6;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      c = i0;
      i0 = HostTempRet0(ctx);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 107u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_6;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 96u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_6;
    }
    block_480:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 211u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 200u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 96u;
      i0 += i1;
      a = i0;
    }
    i0 = l6;
    i1 = 100u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2654u;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 100u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {
        i0 = l3;
        i1 = 216u;
        i0 += i1;
        a = i0;
        i1 = l3;
        i1 = LoadU32(ctx, i1 + 220u);
        b = i1;
        ReleaseLayerNodeTree(ctx, i0, i1);
        goto running_exit;
      }
      goto block_3;
    }
    block_351:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      c = i0;
      i0 = HostTempRet0(ctx);
      i0 = l3;
      i1 = 48u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 52u);
      b = i1;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 75u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 64u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 211u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_350;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 200u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
    }
    block_350:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 195u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_349;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 184u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
    }
    block_349:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 91u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_347;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 80u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_347;
    }
    block_348:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      c = i0;
      i0 = HostTempRet0(ctx);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 211u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 200u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = b;
      i0 = !(i0);
      a = i0;
      if (i0) {goto block_347;}
      i0 = b;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 4u);
      a = i1;
      i2 = 1u;
      i1 -= i2;
      l4 = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_347;}
      i0 = b;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      a = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l6;
    i1 = 101u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = b;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 101u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
    }
    i0 = a;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i1 = b;
        i1 = LoadU32(ctx, i1 + 8u);
        a = i1;
        i2 = 1u;
        i1 -= i2;
        l4 = i1;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_347;}
        i0 = b;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        a = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l6;
      i1 = 102u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = b;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 102u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_347;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_3;}
    block_347:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 96u;
      i0 += i1;
      a = i0;
    }
    i0 = l6;
    i1 = 103u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2654u;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 103u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_339;}
      goto block_3;
    }
    block_346:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      c = i0;
      i0 = HostTempRet0(ctx);
      i0 = l3;
      i1 = 120u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 124u);
      b = i1;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 211u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_344;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 200u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_344;
    }
    block_345:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      c = i0;
      i0 = HostTempRet0(ctx);
      a = i0;
    }
    block_344:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 107u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_339;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 96u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_339;
    }
    block_343:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      c = i0;
      i0 = HostTempRet0(ctx);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 211u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_341;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 200u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_341;
    }
    block_342:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      c = i0;
      i0 = HostTempRet0(ctx);
      a = i0;
    }
    block_341:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 107u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_339;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 96u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_339;
    }
    block_340:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      c = i0;
      i0 = HostTempRet0(ctx);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 107u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_339;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 96u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
    }
    block_339:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 216u;
      i0 += i1;
      a = i0;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 220u);
      b = i1;
      ReleaseNodeTree(ctx, i0, i1);
      goto block_6;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 107u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_6;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 96u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_6;
  }
  block_327:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 96u;
    i0 += i1;
    a = i0;
  }
  i0 = l6;
  i1 = 104u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2654u;
    i1 = a;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 104u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 != i1;
    a = i0;
    if (i0) {
      i0 = l3;
      i1 = 216u;
      i0 += i1;
      a = i0;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 220u);
      b = i1;
      ReleaseNodeTree(ctx, i0, i1);
      goto running_exit;
    }
    goto block_3;
  }
  block_229:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i1 = 136u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 140u);
    b = i1;
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 211u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_228;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 200u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_228:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 227u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_227;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 216u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_227:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 96u;
    i0 += i1;
    a = i0;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 100u);
    b = i1;
    ReleaseNodeTree(ctx, i0, i1);
    goto block_6;
  }
  block_138:;
  i0 = l4;
  i1 = l7;
  i1 = !(i1);
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l4 = i0;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l6;
    i1 = 105u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3661MsgPx;
      i2 = kF3661MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3661MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 105u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_11;}
      goto block_3;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
  }
  i0 = l6;
  i1 = 106u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2591u;
    i1 = l4;
    i2 = b;
    i0 = HostOsQuery(ctx, i0, i1, i2);
    l9 = i0;
    i0 = 106u;
    i1 = ResumePhase(ctx);
    i2 = kF3661ResumeDeallocateRawBlock01;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l9;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_130;}
    i0 = l3;
    i1 = 0u;
    StoreU32(ctx, i0 + 236u, i1);
    i0 = l3;
    i1 = 2648u;
    StoreU32(ctx, i0 + 232u, i1);
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    j1 = LoadU64(ctx, i1 + 232u);
    StoreU64(ctx, i0 + 8u, j1);
    i0 = l3;
    i1 = 8u;
    i0 += i1;
    l8 = i0;
    i0 = l3;
    i1 = 288u;
    i0 += i1;
    l5 = i0;
  }
  i0 = l6;
  i1 = 107u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2649u;
    i1 = l5;
    i2 = l8;
    i3 = a;
    i4 = l12;
    i5 = 1u;
    i6 = l4;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 107u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 == i1;
    l5 = i0;
    if (i0) {goto block_136;}
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 288u;
    i0 += i1;
    l8 = i0;
    i0 = l3;
    i1 = 352u;
    i0 += i1;
    l5 = i0;
  }
  i0 = l6;
  i1 = 108u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2650u;
    i1 = l5;
    i2 = l8;
    i3 = 0u;
    i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
    l9 = i0;
    i0 = 108u;
    i1 = ResumePhase(ctx);
    i2 = kF3661ResumeDeallocateRawBlock01;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l9;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i0 = LoadU32(ctx, i0);
    l8 = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l8;
    i1 = 1u;
    i0 = i0 == i1;
    l8 = i0;
    if (i0) {goto block_134;}
    i0 = l7;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 272u);
    l8 = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l6;
  i1 = 109u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l8;
    i1 = l7;
    i2 = l12;
    i3 = l5;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 109u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i0 = LoadU32(ctx, i0);
    l8 = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l8;
    i1 = 1u;
    i0 = i0 == i1;
    l8 = i0;
    if (i0) {goto block_135;}
    i0 = l5;
    i0 = LoadU32(ctx, i0);
    l8 = i0;
  }
  i0 = l8;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l7;
    i1 = l8;
    i2 = 1u;
    i1 &= i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l7 = i0;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l7;
      if (i0) {goto block_695;}
      i0 = l8;
      i1 = kF3661ConstFFFFFFFE;
      i0 &= i1;
      i0 = LoadU32(ctx, i0);
      l8 = i0;
      i0 = !(i0);
      l7 = i0;
      if (i0) {goto block_695;}
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l8;
      l7 = i0;
      i0 = l5;
      i1 = 4u;
      i0 += i1;
      l8 = i0;
    }
    i0 = l6;
    i1 = 110u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l7;
      i1 = l8;
      i2 = l8;
      i3 = 2u;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 110u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      l8 = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l8;
      i1 = 1u;
      i0 = i0 == i1;
      l8 = i0;
      if (i0) {goto block_3;}
    }
    block_695:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = kF3661ConstB5F40;
    StoreU32(ctx, i0 + 308u, i1);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 347u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 336u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i1 = 324u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 328u);
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 323u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 312u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l4;
    i1 = kF3661ConstB5F40;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 39u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 28u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l4;
    i1 = 16u;
    i0 += i1;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 20u);
    l8 = i1;
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 15u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    l5 = i0;
    if (i0) {goto block_137;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 4u);
    l4 = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_137:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 504u);
    l4 = i0;
    if (i0) {goto block_131;}
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l6;
  i1 = 111u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 857u;
    i1 = kF3661MsgPx;
    i2 = kF3661MsgBoostBoostSmartPtr;
    i3 = 734u;
    i4 = kF3661MsgOperator;
    HostFailAssertion(ctx, i0, i1, i2, i3, i4);
    i0 = 111u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 != i1;
    a = i0;
    if (i0) {goto block_11;}
    goto block_3;
  }
  block_136:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_132;
  }
  block_135:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l5;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = !(i0);
    b = i0;
    if (i0) {goto block_133;}
    i0 = a;
    i1 = 1u;
    i0 &= i1;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    if (i0) {goto block_710;}
    i0 = a;
    i1 = kF3661ConstFFFFFFFE;
    i0 &= i1;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = !(i0);
    b = i0;
    if (i0) {goto block_710;}
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    b = i0;
    i0 = l5;
    i1 = 4u;
    i0 += i1;
    a = i0;
  }
  i0 = l6;
  i1 = 112u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = a;
    i2 = a;
    i3 = 2u;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 112u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_709;}
  }
  block_710:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    goto block_133;
  }
  block_709:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_3;}
  block_134:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
  }
  block_133:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = kF3661ConstB5F40;
    StoreU32(ctx, i0 + 308u, i1);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 347u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 336u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i1 = 324u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 328u);
    b = i1;
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 323u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_132;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 312u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_132:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = kF3661ConstB5F40;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 39u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 28u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l4;
    i1 = 16u;
    i0 += i1;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 20u);
    b = i1;
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 15u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_6;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 4u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_6;
  }
  block_131:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 292u);
    l5 = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l6;
  i1 = 113u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l5;
    i1 = l4;
    i0 = HostFoldLayoutOffset(ctx, i0, i1);
    l9 = i0;
    i0 = 113u;
    i1 = ResumePhase(ctx);
    i2 = kF3661ResumeDeallocateRawBlock01;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l9;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 == i1;
    l5 = i0;
    if (i0) {goto block_130;}
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    l4 = i0;
  }
  i0 = l4;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 504u);
      l4 = i0;
      i0 = !(i0);
      l5 = i0;
    }
    i0 = l5;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l6;
      i1 = 114u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3661MsgPx;
        i2 = kF3661MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3661MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 114u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_11;}
        goto block_3;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 276u);
      l5 = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l6;
    i1 = 115u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l5;
      i1 = l4;
      i2 = l12;
      i3 = 1u;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 115u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      l4 = i0;
      if (i0) {goto block_130;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    if (i0) {goto running_exit;}
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 220u, j1);
    i0 = l3;
    i1 = l3;
    i2 = 216u;
    i1 += i2;
    i2 = 4u;
    i1 |= i2;
    l12 = i1;
    StoreU32(ctx, i0 + 216u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 204u, i1);
    i0 = l3;
    i1 = kF3661Const65707974;
    StoreU32(ctx, i0 + 200u, i1);
    i0 = l3;
    i1 = 4u;
    StoreU8(ctx, i0 + 211u, i1);
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    i2 = 200u;
    i1 += i2;
    StoreU32(ctx, i0 + 80u, i1);
    i0 = l3;
    i1 = 200u;
    i0 += i1;
    l4 = i0;
    i0 = l3;
    i1 = 80u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = kF3661ConstFFFFFFC0;
    i0 -= i1;
    l8 = i0;
    i0 = l3;
    i1 = 184u;
    i0 += i1;
    c = i0;
  }
  i0 = l6;
  i1 = 116u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = c;
    i2 = l13;
    i3 = l4;
    i4 = kF3661ConstB5E30;
    i5 = l5;
    i6 = l8;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 116u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i0 = LoadU32(ctx, i0);
    c = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = 1u;
    i0 = i0 != i1;
    c = i0;
  }
  i0 = c;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 184u);
      c = i0;
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 188u, i1);
      i0 = l3;
      i1 = kF3661Const65707974;
      StoreU32(ctx, i0 + 184u, i1);
      i0 = l3;
      i1 = 4u;
      StoreU8(ctx, i0 + 195u, i1);
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = l3;
      i2 = 184u;
      i1 += i2;
      StoreU32(ctx, i0 + 64u, i1);
      i0 = l3;
      i1 = 216u;
      i0 += i1;
      l5 = i0;
      i0 = l3;
      i1 = 184u;
      i0 += i1;
      l8 = i0;
      i0 = l3;
      i1 = kF3661ConstFFFFFFC0;
      i0 -= i1;
      l7 = i0;
      i0 = l3;
      i1 = 520u;
      i0 += i1;
      l10 = i0;
      i0 = l3;
      i1 = 80u;
      i0 += i1;
      l4 = i0;
    }
    i0 = l6;
    i1 = 117u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1959u;
      i1 = l4;
      i2 = l5;
      i3 = l8;
      i4 = kF3661ConstB5E30;
      i5 = l7;
      i6 = l10;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 117u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_740;}
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 80u);
      i1 = 28u;
      i0 += i1;
      l4 = i0;
      i0 = c;
      i1 = 28u;
      i0 += i1;
      c = i0;
    }
    i0 = l6;
    i1 = 118u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 870u;
      i1 = l4;
      i2 = c;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l9 = i0;
      i0 = kF3661ResumeDeallocateRawBlock118;
      i1 = ResumePhase(ctx);
      i2 = kF3661ResumeDeallocateRawBlock01;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l9;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 == i1;
      c = i0;
      if (i0) {goto block_740;}
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 195u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 184u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 211u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 200u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = a;
      i0 = LoadU32(ctx, i0 + 128u);
      l8 = i0;
      i0 = !(i0);
      c = i0;
    }
    i0 = c;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l6;
      i1 = 119u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3661MsgPx;
        i2 = kF3661MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3661MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 119u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_11;}
        goto block_3;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 202u, i1);
      i0 = l3;
      i1 = 25705u;
      StoreU16(ctx, i0 + 200u, i1);
      i0 = l3;
      i1 = 2u;
      StoreU8(ctx, i0 + 211u, i1);
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = l3;
      i2 = 200u;
      i1 += i2;
      StoreU32(ctx, i0 + 80u, i1);
      i0 = l3;
      i1 = 200u;
      i0 += i1;
      l4 = i0;
      i0 = l3;
      i1 = 80u;
      i0 += i1;
      l5 = i0;
      i0 = l3;
      i1 = kF3661ConstFFFFFFC0;
      i0 -= i1;
      l7 = i0;
      i0 = l3;
      i1 = 184u;
      i0 += i1;
      c = i0;
    }
    i0 = l6;
    i1 = 120u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1959u;
      i1 = c;
      i2 = l13;
      i3 = l4;
      i4 = kF3661ConstB5E30;
      i5 = l5;
      i6 = l7;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 120u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 == i1;
      c = i0;
      if (i0) {goto block_738;}
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 172u, j1);
      i0 = l3;
      i1 = l3;
      i2 = 168u;
      i1 += i2;
      i2 = 4u;
      i1 |= i2;
      l7 = i1;
      StoreU32(ctx, i0 + 168u, i1);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 184u);
      l10 = i0;
      i0 = l3;
      i1 = 7u;
      StoreU8(ctx, i0 + 195u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 191u, i1);
      i0 = l3;
      i1 = kF3661MsgLyevent;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0 + 184u, i1);
      i0 = l3;
      i1 = kF3661ConstAF65B;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0 + 187u, i1);
      i0 = l12;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 216u);
      l4 = i1;
      i0 = i0 != i1;
      c = i0;
    }
    i0 = c;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      loop_760: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3661ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = 168u;
          i0 += i1;
          l5 = i0;
          i0 = l4;
          c = i0;
          i0 = l4;
          i1 = 16u;
          i0 += i1;
          l4 = i0;
        }
        i0 = l6;
        i1 = 121u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2364u;
          i1 = l5;
          i2 = l7;
          i3 = l4;
          i4 = l4;
          i0 = HostInvokeIiiii(ctx, i0, i1, i2, i3, i4);
          l9 = i0;
          i0 = 121u;
          i1 = ResumePhase(ctx);
          i2 = kF3661ResumeDeallocateRawBlock01;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l9;
          l4 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3661ConstD015C;
          i0 = LoadU32(ctx, i0);
          l4 = i0;
          i0 = kF3661ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = 1u;
          i0 = i0 == i1;
          if (i0) {goto block_10;}
          i0 = c;
          i0 = LoadU32(ctx, i0 + 4u);
          l5 = i0;
          i0 = !(i0);
          if (i0) {
            i0 = c;
            i0 = LoadU32(ctx, i0 + 8u);
            l4 = i0;
            i0 = LoadU32(ctx, i0);
            i1 = c;
            i0 = i0 == i1;
            l5 = i0;
            if (i0) {goto block_764;}
            i0 = c;
            i1 = 8u;
            i0 += i1;
            c = i0;
            loop_766: 
              i0 = c;
              i0 = LoadU32(ctx, i0);
              l5 = i0;
              i1 = 8u;
              i0 += i1;
              c = i0;
              i0 = l5;
              i0 = LoadU32(ctx, i0 + 8u);
              l4 = i0;
              i0 = LoadU32(ctx, i0);
              i1 = l5;
              i0 = i0 != i1;
              l5 = i0;
              if (i0) {goto loop_766;}
            goto block_764;
          }
          loop_767: 
            i0 = l5;
            l4 = i0;
            i0 = LoadU32(ctx, i0);
            l5 = i0;
            if (i0) {goto loop_767;}
          block_764:;
          i0 = l4;
          i1 = l12;
          i0 = i0 != i1;
          c = i0;
          if (i0) {goto loop_760;}
        }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 184u;
      i0 += i1;
      l4 = i0;
      i0 = l3;
      i1 = 168u;
      i0 += i1;
      l5 = i0;
      i0 = l10;
      i1 = 28u;
      i0 += i1;
      c = i0;
    }
    i0 = l6;
    i1 = 122u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2651u;
      i1 = l8;
      i2 = c;
      i3 = l4;
      i4 = l5;
      i5 = 1u;
      HostInitResources(ctx, i0, i1, i2, i3, i4, i5);
      i0 = 122u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 == i1;
      c = i0;
      if (i0) {goto block_10;}
      i0 = l3;
      i1 = 168u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 172u);
      l4 = i1;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 195u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 184u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 211u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 200u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = a;
      i0 = LoadU32(ctx, i0 + 128u);
      a = i0;
      i0 = !(i0);
      c = i0;
    }
    i0 = c;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l6;
      i1 = 123u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3661MsgPx;
        i2 = kF3661MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3661MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 123u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_11;}
        goto block_3;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 202u, i1);
      i0 = l3;
      i1 = 25705u;
      StoreU16(ctx, i0 + 200u, i1);
      i0 = l3;
      i1 = 2u;
      StoreU8(ctx, i0 + 211u, i1);
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = l3;
      i2 = 200u;
      i1 += i2;
      StoreU32(ctx, i0 + 80u, i1);
      i0 = l3;
      i1 = 200u;
      i0 += i1;
      l4 = i0;
      i0 = l3;
      i1 = 80u;
      i0 += i1;
      l5 = i0;
      i0 = l3;
      i1 = kF3661ConstFFFFFFC0;
      i0 -= i1;
      l8 = i0;
      i0 = l3;
      i1 = 184u;
      i0 += i1;
      c = i0;
    }
    i0 = l6;
    i1 = 124u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1959u;
      i1 = c;
      i2 = l13;
      i3 = l4;
      i4 = kF3661ConstB5E30;
      i5 = l5;
      i6 = l8;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 124u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 == i1;
      c = i0;
      if (i0) {goto block_737;}
      i0 = l3;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 184u);
      i2 = 28u;
      i1 += i2;
      c = i1;
      StoreU32(ctx, i0 + 80u, i1);
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 184u;
      i0 += i1;
      l4 = i0;
      i0 = l3;
      i1 = 80u;
      i0 += i1;
      l5 = i0;
      i0 = l3;
      i1 = kF3661ConstFFFFFFC0;
      i0 -= i1;
      l8 = i0;
      i0 = a;
      i1 = 76u;
      i0 += i1;
      a = i0;
    }
    i0 = l6;
    i1 = 125u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2613u;
      i1 = l4;
      i2 = a;
      i3 = c;
      i4 = kF3661ConstB5E30;
      i5 = l5;
      i6 = l8;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 125u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_737;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 184u);
      a = i0;
      i0 = LoadU32(ctx, i0 + 32u);
      l4 = i0;
      i0 = l4;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 36u);
      i0 = i0 != i1;
      c = i0;
    }
    i0 = c;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l6;
      i1 = 126u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 2591u;
        i1 = l4;
        i2 = b;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l9 = i0;
        i0 = 126u;
        i1 = ResumePhase(ctx);
        i2 = kF3661ResumeDeallocateRawBlock01;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l9;
        b = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3661ConstD015C;
        i0 = LoadU32(ctx, i0);
        c = i0;
        i0 = kF3661ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = c;
        i1 = 1u;
        i0 = i0 == i1;
        c = i0;
        if (i0) {
          i0 = HostRet0(ctx);
          c = i0;
          i0 = HostTempRet0(ctx);
          b = i0;
          i0 = a;
          i1 = l4;
          StoreU32(ctx, i0 + 32u, i1);
          goto block_736;
        }
        i0 = a;
        i1 = b;
        i2 = 44u;
        i1 += i2;
        b = i1;
        StoreU32(ctx, i0 + 32u, i1);
        goto block_781;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 28u;
      i0 += i1;
      a = i0;
    }
    i0 = l6;
    i1 = 127u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2614u;
      i1 = a;
      i2 = b;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 127u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_737;}
    }
    block_781:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 211u);
      i1 = kF3661ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 200u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l3;
      i1 = 216u;
      i0 += i1;
      a = i0;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 220u);
      b = i1;
      ReleaseNodeTree(ctx, i0, i1);
      goto running_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_739;
  }
  block_740:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 195u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_739;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 184u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_739:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 211u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_8;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 200u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_8;
  }
  block_738:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_9;
  }
  block_737:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
  }
  block_736:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 211u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_8;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 200u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_8;
  }
  block_130:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_6;
  }
  block_11:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    TrapUnreachable();
  }
  block_10:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i1 = 168u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 172u);
    b = i1;
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 195u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_9;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 184u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_9:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 211u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_8;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 200u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 216u;
    i0 += i1;
    a = i0;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 220u);
    b = i1;
    ReleaseNodeTree(ctx, i0, i1);
    goto block_6;
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 107u);
    i1 = kF3661ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_6;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 96u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 508u);
    a = i0;
    i0 = !(i0);
    b = i0;
    if (i0) {goto block_5;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 4u);
    b = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_5;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l6;
  i1 = 128u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = a;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 128u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_3;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 8u);
    b = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_5;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    b = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l6;
  i1 = 129u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = a;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 129u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_3;}
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 508u);
    a = i0;
    i0 = !(i0);
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    if (i0) {goto block_813;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 4u);
    b = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_813;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l6;
  i1 = 130u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = a;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 130u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3661ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3661ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
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
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 8u);
      b = i1;
      i2 = 1u;
      i1 -= i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      if (i0) {goto block_813;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      b = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l6;
    i1 = 131u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 131u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3661ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3661ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_813;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_3;}
  block_813:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 528u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_3:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    a = i0;
    i0 = HostTempRet0(ctx);
  }
  i0 = l6;
  i1 = 132u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 132u;
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
  l9 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l9;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF3661ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l9 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l9;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l9;
  i1 = c;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l9;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l9;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l9;
  i1 = l5;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l9;
  i1 = l7;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l9;
  i1 = l8;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l9;
  i1 = l10;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = l9;
  i1 = l11;
  StoreU32(ctx, i0 + 36u, i1);
  i0 = l9;
  i1 = l12;
  StoreU32(ctx, i0 + 40u, i1);
  i0 = l9;
  i1 = l13;
  StoreU32(ctx, i0 + 44u, i1);
  i0 = l9;
  i1 = l14;
  StoreU32(ctx, i0 + 48u, i1);
  i0 = l9;
  i1 = l15;
  StoreU32(ctx, i0 + 52u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 56u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
}
