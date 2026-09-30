// compositor_tween.cpp -- the compositor tween dispatch: one suspendable step that carries a tween through the compositor tween table..
//
// Compositor::TweenDispatch -- the compositor tween dispatch: one suspendable step that carries a tween through the compositor tween table.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 1843846-1843846 (f3655); R1 transcribe-keep.
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
void ReleaseNodeTree(EngineContext* ctx, uint32_t state, uint32_t node);
uint32_t FindOrderedChainEntry(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t CombineComparisonResults(EngineContext* ctx, uint32_t left, uint32_t right, uint32_t result, uint32_t box);

namespace {

constexpr uint32_t kF3655Const8C564 = 0x8C564u;  // 
constexpr uint32_t kF3655MsgLytween = 0xAF675u;  // lytween
constexpr uint32_t kF3655ConstAF678 = 0xAF678u;  // 
constexpr uint32_t kF3655ConstB5265 = 0xB5265u;  // 
constexpr uint32_t kF3655ConstB5E30 = 0xB5E30u;  // 
constexpr uint32_t kF3655MsgPx = 0xB5E31u;  // px != 0
constexpr uint32_t kF3655MsgBoostBoostSmartPtr = 0xB5E39u;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
constexpr uint32_t kF3655MsgOperator = 0xB5E71u;  // operator->
constexpr uint32_t kF3655ConstB8F8C = 0xB8F8Cu;  // 
constexpr uint32_t kF3655ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF3655ConstFFFFFFC0 = 0xFFFFFFC0u;  // 
constexpr uint32_t kF3655ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF3655SaveFrameBytes = 40u;
constexpr uint32_t kF3655ResumeWordBytes = 4u;

}  // namespace

namespace Compositor {
void TweenDispatch(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u; uint32_t i6 = 0u;
  uint64_t j1 = 0;
      l11 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF3655SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l8 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 24u);
    l6 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 28u);
    l9 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 32u);
    l10 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 36u);
    l11 = i0;
    i0 = l8;
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
    i2 = kF3655ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l7 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    i1 = 80u;
    i0 -= i1;
    l3 = i0;
    GuestStackSp(ctx) = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = !(i0);
    l6 = i0;
  }
  i0 = l6;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3655ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3655MsgPx;
      i2 = kF3655MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3655MsgOperator;
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
      i0 = kF3655ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3655ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto running_exit;}
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
    l6 = i0;
  }
  i0 = l6;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3655ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3655MsgPx;
      i2 = kF3655MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3655MsgOperator;
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
      i0 = kF3655ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3655ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto running_exit;}
      goto block_3;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 66u, i1);
    i0 = l3;
    i1 = 25705u;
    StoreU16(ctx, i0 + 64u, i1);
    i0 = l3;
    i1 = 2u;
    StoreU8(ctx, i0 + 75u, i1);
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    i2 = kF3655ConstFFFFFFC0;
    i1 -= i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 48u;
    i0 += i1;
    l6 = i0;
    i0 = b;
    i1 = 16u;
    i0 += i1;
    l10 = i0;
    i0 = l3;
    i1 = 8u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = 40u;
    i0 += i1;
    l9 = i0;
    i0 = l3;
    i1 = kF3655ConstFFFFFFC0;
    i0 -= i1;
    b = i0;
  }
  i0 = l7;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = l6;
    i2 = l10;
    i3 = b;
    i4 = kF3655ConstB5E30;
    i5 = l5;
    i6 = l9;
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
    i0 = kF3655ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    if (i0) {goto block_33;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 48u);
    b = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 304u);
    l6 = i0;
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 24u;
    i0 += i1;
    l5 = i0;
    i0 = b;
    i1 = 28u;
    i0 += i1;
    b = i0;
  }
  i0 = l7;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = l5;
    i2 = l4;
    i3 = b;
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
    i0 = kF3655ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    b = i0;
    if (i0) {goto block_33;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 24u);
    b = i0;
    i0 = !(i0);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    if (i0) {
      i0 = 0u;
      b = i0;
      i0 = 0u;
      l4 = i0;
      goto block_37;
    }
    i0 = 0u;
    l4 = i0;
  }
  i0 = l7;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = kF3655Const8C564;
    i2 = kF3655ConstB8F8C;
    i3 = 0u;
    i0 = CombineComparisonResults(ctx, i0, i1, i2, i3);
    l8 = i0;
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i0 = !(i0);
    l6 = i0;
    if (i0) {
      i0 = 0u;
      b = i0;
      goto block_37;
    }
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 28u);
    l6 = i0;
    i0 = !(i0);
    l5 = i0;
    if (i0) {goto block_37;}
    i0 = l6;
    i1 = l6;
    i1 = LoadU32(ctx, i1 + 4u);
    i2 = 1u;
    i1 += i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l6;
    l4 = i0;
  }
  block_37:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 28u);
    l6 = i0;
    i0 = !(i0);
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    if (i0) {goto block_44;}
    i0 = l6;
    i1 = l6;
    i1 = LoadU32(ctx, i1 + 4u);
    l5 = i1;
    i2 = 1u;
    i1 -= i2;
    l9 = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 != i1;
    l5 = i0;
    if (i0) {goto block_44;}
    i0 = l6;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    l5 = i0;
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 5u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l5;
    i1 = l6;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 5u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3655ConstD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_3;}
    i0 = l6;
    i1 = l6;
    i1 = LoadU32(ctx, i1 + 8u);
    l5 = i1;
    i2 = 1u;
    i1 -= i2;
    l9 = i1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 != i1;
    l5 = i0;
    if (i0) {goto block_44;}
    i0 = l6;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    l5 = i0;
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 6u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l5;
    i1 = l6;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 6u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3655ConstD015C;
    i0 = LoadU32(ctx, i0);
    l6 = i0;
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l6;
    i1 = 1u;
    i0 = i0 == i1;
    l6 = i0;
    if (i0) {goto block_3;}
  }
  block_44:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 75u);
    i1 = kF3655ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 64u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = b;
    if (i0) {
      i0 = l4;
      l6 = i0;
      goto block_30;
    }
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 66u, i1);
    i0 = l3;
    i1 = 25705u;
    StoreU16(ctx, i0 + 64u, i1);
    i0 = l3;
    i1 = 2u;
    StoreU8(ctx, i0 + 75u, i1);
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    i2 = kF3655ConstFFFFFFC0;
    i1 -= i2;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l3;
    i1 = 48u;
    i0 += i1;
    l6 = i0;
    i0 = l3;
    i1 = kF3655ConstFFFFFFC0;
    i0 -= i1;
    l5 = i0;
    i0 = l3;
    i1 = 24u;
    i0 += i1;
    l9 = i0;
    i0 = l3;
    i1 = 8u;
    i0 += i1;
    l11 = i0;
    i0 = 0u;
    b = i0;
  }
  i0 = l7;
  i1 = 7u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = l6;
    i2 = l10;
    i3 = l5;
    i4 = kF3655ConstB5E30;
    i5 = l9;
    i6 = l11;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 7u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3655ConstD015C;
    i0 = LoadU32(ctx, i0);
    l6 = i0;
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l6;
    i1 = 1u;
    i0 = i0 == i1;
    l6 = i0;
    if (i0) {goto block_32;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 48u);
    l6 = i0;
    i0 = LoadU32(ctx, i0 + 32u);
    l9 = i0;
    i0 = l6;
    i0 = LoadU8(ctx, i0 + 39u);
    l5 = i0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l11 = i0;
    i0 = l9;
    i1 = l5;
    i2 = l11;
    i0 = i2 ? i0 : i1;
    i1 = 1u;
    i0 = i0 == i1;
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
      i0 = kF3655ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 28u;
      i0 += i1;
      b = i0;
    }
    i0 = l7;
    i1 = 8u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = b;
      i2 = 0u;
      i3 = kF3655ConstFFFFFFFF;
      i4 = kF3655ConstB5265;
      i5 = 1u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l8 = i0;
      i0 = 8u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = b;
    } else {
      i0 = kF3655ConstD015C;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = kF3655ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 == i1;
      l6 = i0;
      if (i0) {goto block_3;}
      i0 = b;
      i0 = !(i0);
    }
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 75u);
    i1 = kF3655ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 64u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = b;
    i0 = !(i0);
    b = i0;
    if (i0) {
      i0 = l4;
      l6 = i0;
      goto block_29;
    }
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
    l6 = i0;
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
      i0 = kF3655ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 9u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3655MsgPx;
      i2 = kF3655MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3655MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 9u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3655ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3655ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto running_exit;}
      goto block_3;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 24u);
    b = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 28u);
    l6 = i0;
    if (i0) {
      i0 = l6;
      i1 = l6;
      i1 = LoadU32(ctx, i1 + 4u);
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 4u, i1);
    }
    i0 = l4;
    i0 = !(i0);
    l5 = i0;
    if (i0) {goto block_31;}
    i0 = l4;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 4u);
    l5 = i1;
    i2 = 1u;
    i1 -= i2;
    l9 = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 != i1;
    l5 = i0;
    if (i0) {goto block_31;}
    i0 = l4;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    l5 = i0;
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 10u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l5;
    i1 = l4;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 10u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3655ConstD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF3655ConstD015C;
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
      i0 = l4;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 8u);
      l5 = i1;
      i2 = 1u;
      i1 -= i2;
      l9 = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 != i1;
      l5 = i0;
      if (i0) {goto block_31;}
      i0 = l4;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      l5 = i0;
      i0 = kF3655ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 11u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l5;
      i1 = l4;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 11u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3655ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3655ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 != i1;
      l4 = i0;
      if (i0) {goto block_31;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_3;}
  block_33:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 75u);
    i1 = kF3655ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_20;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 64u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_20;
  }
  block_32:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 75u);
    i1 = kF3655ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_21;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 64u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_21;
  }
  block_31:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i0 = !(i0);
    l4 = i0;
    if (i0) {goto block_29;}
  }
  block_30:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 388u);
    l4 = i0;
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 12u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l4;
    i1 = b;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 12u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3655ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_28;}
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_22;
  }
  block_29:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 66u, i1);
    i0 = l3;
    i1 = 25705u;
    StoreU16(ctx, i0 + 64u, i1);
    i0 = l3;
    i1 = 2u;
    StoreU8(ctx, i0 + 75u, i1);
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    i2 = kF3655ConstFFFFFFC0;
    i1 -= i2;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l3;
    i1 = kF3655ConstFFFFFFC0;
    i0 -= i1;
    l4 = i0;
    i0 = l3;
    i1 = 24u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = 8u;
    i0 += i1;
    l9 = i0;
    i0 = l3;
    i1 = 48u;
    i0 += i1;
    b = i0;
  }
  i0 = l7;
  i1 = 13u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = b;
    i2 = l10;
    i3 = l4;
    i4 = kF3655ConstB5E30;
    i5 = l5;
    i6 = l9;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 13u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3655ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    b = i0;
    if (i0) {goto block_23;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 48u);
    l4 = i0;
    i1 = 28u;
    i0 += i1;
    l5 = i0;
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 39u);
    i1 = kF3655ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 28u);
      l5 = i0;
    }
    i0 = l5;
    i0 = LoadU8(ctx, i0);
    b = i0;
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 75u);
    i1 = kF3655ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    l4 = i0;
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 64u);
      l4 = i0;
      DeallocateRawBlock(ctx, i0);
    }
    i0 = b;
    i1 = 126u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_28;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
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
      i0 = kF3655ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 14u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3655MsgPx;
      i2 = kF3655MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3655MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 14u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3655ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3655ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto running_exit;}
      goto block_3;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 50u, i1);
    i0 = l3;
    i1 = 25705u;
    StoreU16(ctx, i0 + 48u, i1);
    i0 = l3;
    i1 = 2u;
    StoreU8(ctx, i0 + 59u, i1);
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    i2 = 48u;
    i1 += i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 48u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = 8u;
    i0 += i1;
    l9 = i0;
    i0 = l3;
    i1 = 40u;
    i0 += i1;
    l11 = i0;
    i0 = l3;
    i1 = 24u;
    i0 += i1;
    l4 = i0;
  }
  i0 = l7;
  i1 = 15u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = l4;
    i2 = l10;
    i3 = l5;
    i4 = kF3655ConstB5E30;
    i5 = l9;
    i6 = l11;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 15u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3655ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    l4 = i0;
    if (i0) {goto block_25;}
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = kF3655ConstFFFFFFC0;
    i0 -= i1;
    l5 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 24u);
    i1 = 28u;
    i0 += i1;
    l4 = i0;
  }
  i0 = l7;
  i1 = 16u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 854u;
    i1 = l5;
    i2 = l4;
    i3 = 1u;
    i4 = kF3655ConstFFFFFFFF;
    i5 = l4;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l8 = i0;
    i0 = 16u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3655ConstD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_25;}
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 88u;
    i0 += i1;
    i1 = l4;
    i0 = FindOrderedChainEntry(ctx, i0, i1);
    l5 = i0;
    i0 = kF3655ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    b = i0;
    if (i0) {goto block_26;}
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF3655ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 59u);
    i1 = kF3655ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 48u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
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
      i0 = kF3655ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 17u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3655MsgPx;
      i2 = kF3655MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3655MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 17u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3655ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3655ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto running_exit;}
      goto block_3;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = b;
    i2 = 92u;
    i1 += i2;
    i0 = i0 == i1;
    b = i0;
    if (i0) {goto block_28;}
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 28u);
    b = i0;
    i1 = l5;
    i2 = 32u;
    i1 += i2;
    l9 = i1;
    i0 = i0 == i1;
    l4 = i0;
    if (i0) {goto block_28;}
  }
  loop_103: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      l5 = i0;
      i0 = b;
      i0 = LoadU32(ctx, i0 + 16u);
      b = i0;
      i0 = kF3655ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 18u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1776u;
      i1 = b;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 18u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3655ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3655ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
      if (i0) {goto block_27;}
      i0 = l5;
      i0 = LoadU32(ctx, i0 + 4u);
      l4 = i0;
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        i1 = l5;
        i1 = LoadU32(ctx, i1 + 8u);
        b = i1;
        i1 = LoadU32(ctx, i1);
        i0 = i0 == i1;
        if (i0) {goto block_107;}
        i0 = l5;
        i1 = 8u;
        i0 += i1;
        l4 = i0;
        loop_109: 
          i0 = l4;
          i0 = LoadU32(ctx, i0);
          l5 = i0;
          i1 = 8u;
          i0 += i1;
          l4 = i0;
          i0 = l5;
          i0 = LoadU32(ctx, i0 + 8u);
          b = i0;
          i0 = LoadU32(ctx, i0);
          i1 = l5;
          i0 = i0 != i1;
          l5 = i0;
          if (i0) {goto loop_109;}
        goto block_107;
      }
      loop_110: 
        i0 = l4;
        b = i0;
        i0 = l4;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        if (i0) {goto loop_110;}
      block_107:;
      i0 = b;
      i1 = l9;
      i0 = i0 != i1;
      l4 = i0;
      if (i0) {goto loop_103;}
    }
  block_28:;
  i0 = l4;
  i1 = l6;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l4 = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    if (i0) {goto block_114;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 128u);
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
      i0 = kF3655ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 19u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3655MsgPx;
      i2 = kF3655MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3655MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 19u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3655ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3655ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto running_exit;}
      goto block_3;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 26u, i1);
    i0 = l3;
    i1 = 25705u;
    StoreU16(ctx, i0 + 24u, i1);
    i0 = l3;
    i1 = 2u;
    StoreU8(ctx, i0 + 35u, i1);
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    i2 = 24u;
    i1 += i2;
    StoreU32(ctx, i0 + 48u, i1);
    i0 = l3;
    i1 = 24u;
    i0 += i1;
    c = i0;
    i0 = l3;
    i1 = 48u;
    i0 += i1;
    l6 = i0;
    i0 = l3;
    i1 = 8u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = kF3655ConstFFFFFFC0;
    i0 -= i1;
    b = i0;
  }
  i0 = l7;
  i1 = 20u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = b;
    i2 = l10;
    i3 = c;
    i4 = kF3655ConstB5E30;
    i5 = l6;
    i6 = l5;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 20u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3655ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    b = i0;
    if (i0) {goto block_113;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 64u);
    b = i0;
    i0 = l3;
    i1 = 7u;
    StoreU8(ctx, i0 + 19u, i1);
    i0 = l3;
    i1 = kF3655MsgLytween;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = kF3655ConstAF678;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 11u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 15u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 52u, j1);
    i0 = l3;
    i1 = l3;
    i2 = 48u;
    i1 += i2;
    i2 = 4u;
    i1 |= i2;
    StoreU32(ctx, i0 + 48u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 68u, j1);
    i0 = l3;
    i1 = l3;
    i2 = kF3655ConstFFFFFFC0;
    i1 -= i2;
    i2 = 4u;
    i1 |= i2;
    StoreU32(ctx, i0 + 64u, i1);
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 8u;
    i0 += i1;
    c = i0;
    i0 = l3;
    i1 = 48u;
    i0 += i1;
    l6 = i0;
    i0 = b;
    i1 = 28u;
    i0 += i1;
    b = i0;
  }
  i0 = l7;
  i1 = 21u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2651u;
    i1 = a;
    i2 = b;
    i3 = c;
    i4 = l6;
    i5 = 1u;
    HostInitResources(ctx, i0, i1, i2, i3, i4, i5);
    i0 = 21u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3655ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      i0 = l3;
      i1 = 48u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 52u);
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l3;
      i1 = kF3655ConstFFFFFFC0;
      i0 -= i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 68u);
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 19u);
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
      a = i0;
      if (i0) {goto block_112;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 8u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_112;
    }
    i0 = l3;
    i1 = 48u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 52u);
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l3;
    i1 = kF3655ConstFFFFFFC0;
    i0 -= i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 68u);
    b = i1;
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 19u);
    i1 = kF3655ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 8u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 35u);
    i1 = kF3655ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_114;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 24u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_114:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = !(i0);
    a = i0;
    if (i0) {goto block_111;}
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
    if (i0) {goto block_111;}
    i0 = l4;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    a = i0;
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 22u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = l4;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 22u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3655ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF3655ConstD015C;
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
      if (i0) {goto block_111;}
      i0 = l4;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      a = i0;
      i0 = kF3655ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 23u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = l4;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 23u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3655ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3655ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_111;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_3;}
  block_113:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
  }
  block_112:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 35u);
    i1 = kF3655ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_21;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 24u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_21;
  }
  block_111:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 80u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_27:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_22;
  }
  block_26:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF3655ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_24;}
    i0 = l4;
    i0 = LoadU32(ctx, i0);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_24;
  }
  block_25:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
  }
  block_24:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 59u);
    i1 = kF3655ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_22;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 48u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_22;
  }
  block_23:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 75u);
    i1 = kF3655ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_22;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 64u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_22:;
  i0 = l4;
  i1 = l6;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l4 = i0;
  block_21:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = !(i0);
    a = i0;
    if (i0) {goto block_20;}
    i0 = l4;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 4u);
    a = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 != i1;
    a = i0;
    if (i0) {goto block_20;}
    i0 = l4;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    a = i0;
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 24u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = l4;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 24u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3655ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_3;}
    i0 = l4;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 8u);
    a = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 != i1;
    a = i0;
    if (i0) {goto block_20;}
    i0 = l4;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    a = i0;
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 25u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = l4;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 25u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3655ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF3655ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_3;}
  }
  block_20:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    HostOsRelease(ctx, i0);
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    TrapUnreachable();
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
  i0 = l7;
  i1 = 26u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 26u;
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
  l8 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l8;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF3655ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l8 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l8;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l8;
  i1 = c;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l8;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l8;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l8;
  i1 = l5;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l8;
  i1 = l6;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l8;
  i1 = l9;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l8;
  i1 = l10;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = l8;
  i1 = l11;
  StoreU32(ctx, i0 + 36u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 40u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
}
