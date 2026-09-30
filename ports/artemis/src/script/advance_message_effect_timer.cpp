// advance_message_effect_timer.cpp -- advances the message effect timer: the pending effect step is timed and applied once its interval has elapsed..
//
// AdvanceMessageEffectTimer -- advances the message effect timer: the pending effect step is timed and applied once its interval has elapsed.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 1433347-1433347 (f3192); R1 transcribe-keep.
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

constexpr uint32_t kF3192MsgPx = 0xBA361u;  // px != 0
constexpr uint32_t kF3192MsgBoostBoostSmartPtr = 0xBA369u;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
constexpr uint32_t kF3192MsgOperator = 0xBA3A1u;  // operator->
constexpr uint32_t kF3192ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF3192SaveFrameBytes = 28u;
constexpr uint32_t kF3192ResumeWordBytes = 4u;

}  // namespace

uint32_t AdvanceMessageEffectTimer(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF3192SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 4u);
    b = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 16u);
    l6 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 20u);
    l7 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 24u);
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF3192ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    i1 = 16u;
    i0 -= i1;
    l7 = i0;
    GuestStackSp(ctx) = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 584u);
    l6 = i0;
    i0 = !(i0);
    l8 = i0;
    i0 = 1u;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  i1 = 0u;
  i2 = l8;
  i0 = i2 ? i0 : i1;
  if (i0) {goto block_8;}
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
      i0 = LoadU32(ctx, i0 + 572u);
      i0 = !(i0);
      if (i0) {goto block_9;}
      i0 = l6;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 376u);
      c = i0;
      i0 = a;
      i1 = 604u;
      i0 += i1;
      b = i0;
    }
    i0 = l3;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l6;
      i1 = b;
      i2 = c;
      CallTableT0(ctx, i2, i0, i1);
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
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 572u);
      i2 = 1u;
      i1 -= i2;
      b = i1;
      StoreU32(ctx, i0 + 572u, i1);
      i0 = l7;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 584u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l7;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 588u);
      c = i1;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = c;
      if (i0) {
        i0 = c;
        i1 = c;
        i1 = LoadU32(ctx, i1 + 4u);
        i2 = 1u;
        i1 += i2;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = a;
        i0 = LoadU32(ctx, i0 + 572u);
        b = i0;
      }
      i0 = a;
      i0 = LoadU32(ctx, i0 + 516u);
      i1 = a;
      i1 = LoadU32(ctx, i1 + 528u);
      i2 = b;
      i1 += i2;
      l4 = i1;
      i2 = 170u;
      i1 = DIV_U(i1, i2);
      l6 = i1;
      i2 = 2u;
      i1 <<= (i2 & 31);
      i0 += i1;
      i0 = LoadU32(ctx, i0);
      l9 = i0;
      i0 = a;
      i0 = LoadU32(ctx, i0 + 540u);
      i1 = a;
      i1 = LoadU32(ctx, i1 + 552u);
      i2 = b;
      i1 += i2;
      b = i1;
      i2 = 341u;
      i1 = DIV_U(i1, i2);
      l8 = i1;
      i2 = 2u;
      i1 <<= (i2 & 31);
      i0 += i1;
      i0 = LoadU32(ctx, i0);
      l10 = i0;
      i0 = kF3192ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l7;
      i1 = 8u;
      i0 += i1;
      l5 = i0;
      i0 = l9;
      i1 = l4;
      i2 = l6;
      i3 = 170u;
      i2 *= i3;
      i1 -= i2;
      i2 = 24u;
      i1 *= i2;
      i0 += i1;
      l4 = i0;
      i0 = l10;
      i1 = b;
      i2 = l8;
      i3 = 341u;
      i2 *= i3;
      l6 = i2;
      i1 -= i2;
      i2 = 12u;
      i1 *= i2;
      i0 += i1;
      b = i0;
    }
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3231u;
      i1 = a;
      i2 = l5;
      i3 = l4;
      i4 = b;
      i5 = 0u;
      HostInitResources(ctx, i0, i1, i2, i3, i4, i5);
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3192ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3192ConstD015C;
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
      i0 = b;
      i1 = c;
      i1 = !(i1);
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      b = i0;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        if (i0) {goto block_20;}
        i0 = c;
        i1 = c;
        i1 = LoadU32(ctx, i1 + 4u);
        b = i1;
        i2 = 1u;
        i1 -= i2;
        l4 = i1;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 != i1;
        b = i0;
        if (i0) {goto block_20;}
        i0 = c;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 8u);
        b = i0;
        i0 = kF3192ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l3;
      i1 = 2u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = b;
        i1 = c;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 2u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3192ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3192ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_6;}
        i0 = c;
        i1 = c;
        i1 = LoadU32(ctx, i1 + 8u);
        b = i1;
        i2 = 1u;
        i1 -= i2;
        l4 = i1;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 != i1;
        b = i0;
        if (i0) {goto block_20;}
        i0 = c;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        b = i0;
        i0 = kF3192ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l3;
      i1 = 3u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = b;
        i1 = c;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 3u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3192ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3192ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto block_6;}
      }
      block_20:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 584u);
        b = i0;
        if (i0) {goto block_10;}
        i0 = kF3192ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l3;
      i1 = 4u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3192MsgPx;
        i2 = kF3192MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3192MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 4u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3192ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3192ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_5;}
        goto running_exit;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = c;
      i0 = !(i0);
      if (i0) {goto block_7;}
      i0 = c;
      i1 = c;
      i1 = LoadU32(ctx, i1 + 4u);
      b = i1;
      i2 = 1u;
      i1 -= i2;
      l4 = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      if (i0) {goto block_7;}
      i0 = c;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      b = i0;
      i0 = kF3192ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l3;
    i1 = 5u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = c;
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
      i0 = kF3192ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3192ConstD015C;
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
        i0 = c;
        i1 = c;
        i1 = LoadU32(ctx, i1 + 8u);
        b = i1;
        i2 = 1u;
        i1 -= i2;
        l4 = i1;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 != i1;
        if (i0) {goto block_7;}
        i0 = c;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        b = i0;
        i0 = kF3192ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l3;
      i1 = 6u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = b;
        i1 = c;
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
        i0 = kF3192ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3192ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 != i1;
        b = i0;
        if (i0) {goto block_7;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto running_exit;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    l4 = i0;
    i0 = c;
    i0 = !(i0);
    if (i0) {goto block_8;}
    i0 = 1u;
    l4 = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 572u);
    i1 = a;
    i1 = LoadU32(ctx, i1 + 556u);
    i2 = 1u;
    i1 -= i2;
    i0 = i0 == i1;
    if (i0) {goto block_8;}
    i0 = l6;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 376u);
    c = i0;
    i0 = a;
    i1 = 628u;
    i0 += i1;
    b = i0;
  }
  i0 = l3;
  i1 = 7u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = b;
    i2 = c;
    CallTableT0(ctx, i2, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    i0 = 7u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 572u);
    i2 = 1u;
    i1 += i2;
    b = i1;
    StoreU32(ctx, i0 + 572u, i1);
    i0 = l7;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 584u);
    StoreU32(ctx, i0, i1);
    i0 = l7;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 588u);
    c = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = c;
    if (i0) {
      i0 = c;
      i1 = c;
      i1 = LoadU32(ctx, i1 + 4u);
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = a;
      i0 = LoadU32(ctx, i0 + 572u);
      b = i0;
    }
    i0 = a;
    i0 = LoadU32(ctx, i0 + 516u);
    i1 = a;
    i1 = LoadU32(ctx, i1 + 528u);
    i2 = b;
    i1 += i2;
    l4 = i1;
    i2 = 170u;
    i1 = DIV_U(i1, i2);
    l6 = i1;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l9 = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 540u);
    i1 = a;
    i1 = LoadU32(ctx, i1 + 552u);
    i2 = b;
    i1 += i2;
    b = i1;
    i2 = 341u;
    i1 = DIV_U(i1, i2);
    l8 = i1;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l10 = i0;
    i0 = kF3192ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l9;
    i1 = l4;
    i2 = l6;
    i3 = 170u;
    i2 *= i3;
    i1 -= i2;
    i2 = 24u;
    i1 *= i2;
    i0 += i1;
    l4 = i0;
    i0 = l10;
    i1 = b;
    i2 = l8;
    i3 = 341u;
    i2 *= i3;
    i1 -= i2;
    i2 = 12u;
    i1 *= i2;
    i0 += i1;
    b = i0;
  }
  i0 = l3;
  i1 = 8u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 3231u;
    i1 = a;
    i2 = l7;
    i3 = l4;
    i4 = b;
    i5 = 0u;
    HostInitResources(ctx, i0, i1, i2, i3, i4, i5);
    i0 = 8u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3192ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3192ConstD015C;
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
    i0 = b;
    i1 = c;
    i1 = !(i1);
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    b = i0;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      if (i0) {goto block_45;}
      i0 = c;
      i1 = c;
      i1 = LoadU32(ctx, i1 + 4u);
      b = i1;
      i2 = 1u;
      i1 -= i2;
      l4 = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_45;}
      i0 = c;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      b = i0;
      i0 = kF3192ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l3;
    i1 = 9u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = c;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 9u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3192ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3192ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto running_exit;}
      i0 = c;
      i1 = c;
      i1 = LoadU32(ctx, i1 + 8u);
      b = i1;
      i2 = 1u;
      i1 -= i2;
      l4 = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_45;}
      i0 = c;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      b = i0;
      i0 = kF3192ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l3;
    i1 = 10u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = c;
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
      i0 = kF3192ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3192ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
      if (i0) {goto running_exit;}
    }
    block_45:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 584u);
      b = i0;
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
        i0 = kF3192ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l3;
      i1 = 11u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3192MsgPx;
        i2 = kF3192MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3192MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 11u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3192ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3192ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_5;}
        goto running_exit;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 380u);
      c = i0;
      i0 = a;
      i1 = 616u;
      i0 += i1;
      a = i0;
    }
    i0 = l3;
    i1 = 12u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = a;
      i2 = 1u;
      i3 = c;
      CallTableT5(ctx, i3, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = 12u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_9;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = c;
    i0 = !(i0);
    if (i0) {goto block_7;}
    i0 = c;
    i1 = c;
    i1 = LoadU32(ctx, i1 + 4u);
    b = i1;
    i2 = 1u;
    i1 -= i2;
    l4 = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_7;}
    i0 = c;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
    i0 = kF3192ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l3;
  i1 = 13u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = c;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 13u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3192ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3192ConstD015C;
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
      i0 = c;
      i1 = c;
      i1 = LoadU32(ctx, i1 + 8u);
      b = i1;
      i2 = 1u;
      i1 -= i2;
      l4 = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      if (i0) {goto block_7;}
      i0 = c;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      b = i0;
      i0 = kF3192ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l3;
    i1 = 14u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = c;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 14u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3192ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3192ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_7;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto running_exit;}
  block_10:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 380u);
    c = i0;
    i0 = a;
    i1 = 592u;
    i0 += i1;
    a = i0;
  }
  i0 = l3;
  i1 = 15u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = a;
    i2 = 1u;
    i3 = c;
    CallTableT5(ctx, i3, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    i0 = 15u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_9:;
  i0 = l4;
  i1 = 0u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l4 = i0;
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    i1 = 16u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    i0 = l4;
    goto func_exit;
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    a = i0;
    i0 = HostTempRet0(ctx);
  }
  i0 = l3;
  i1 = 16u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 16u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    TrapUnreachable();
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
  i0 = l3;
  i1 = 17u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 17u;
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
  TrapUnreachable();
  spill_frame:;
  l3 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l3;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF3192ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = c;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l3;
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l3;
  i1 = l6;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l3;
  i1 = l7;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l3;
  i1 = l5;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 28u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
