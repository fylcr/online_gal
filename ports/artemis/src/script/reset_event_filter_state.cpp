// reset_event_filter_state.cpp -- resets the event filter state: the latched event filters are cleared..
//
// ResetEventFilterState -- resets the event filter state: the latched event filters are cleared.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 199253-199253 (f490); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void Panic(EngineContext* ctx, uint32_t a0);
uint32_t FormatLuaEventFilterLine(EngineContext* ctx, uint32_t a0, uint32_t a1);
void InitNodeStorage(EngineContext* ctx, uint32_t a);

namespace {

constexpr uint32_t kF490MsgPx = 0xB5E31u;  // px != 0
constexpr uint32_t kF490MsgBoostBoostSmartPtr = 0xB5E39u;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
constexpr uint32_t kF490MsgOperator = 0xB5E71u;  // operator->
constexpr uint32_t kF490ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF490SaveFrameBytes = 40u;
constexpr uint32_t kF490ResumeWordBytes = 4u;

}  // namespace

void ResetEventFilterState(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
      l9 = 0, l10 = 0, l11 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF490SaveFrameBytes;
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
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 16u);
    l6 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 20u);
    l7 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 24u);
    l8 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 28u);
    l10 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 32u);
    l9 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 36u);
    l11 = i0;
    i0 = l2;
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
    i2 = kF490ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = 2876u;
    i0 += i1;
    l1 = i0;
    i0 = LoadU32(ctx, i0);
    l11 = i0;
    i0 = l1;
    i1 = a;
    i2 = 2880u;
    i1 += i2;
    i1 = LoadU32(ctx, i1);
    l1 = i1;
    StoreU32(ctx, i0, i1);
    i0 = l1;
    i1 = 1u;
    i0 = i0 != i1;
    l1 = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    if (i0) {goto block_7;}
    i0 = a;
    i0 = LoadU8(ctx, i0 + 34u);
    i0 = !(i0);
    l1 = i0;
    if (i0) {goto block_7;}
    i0 = l3;
    i0 = !(i0);
    l1 = i0;
  }
  i0 = l1;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF490ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF490MsgPx;
      i2 = kF490MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF490MsgOperator;
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
      i0 = kF490ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF490ConstD015C;
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
    i0 = l3;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 28u);
    l1 = i0;
  }
  i0 = l5;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l3;
    i1 = 1u;
    i2 = l1;
    CallTableT0(ctx, i2, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_6;}
  block_7:;
  i0 = l1;
  i1 = l3;
  i1 = !(i1);
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l1 = i0;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF490ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF490MsgPx;
      i2 = kF490MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF490MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF490ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF490ConstD015C;
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
    i0 = l3;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 28u);
    l1 = i0;
  }
  i0 = l5;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l3;
    i1 = 0u;
    i2 = l1;
    CallTableT0(ctx, i2, i0, i1);
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
    i0 = a;
    i1 = 2912u;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l1 = i0;
    i1 = a;
    i2 = 2924u;
    i1 += i2;
    i1 = LoadU32(ctx, i1);
    l7 = i1;
    i2 = a;
    i3 = 2928u;
    i2 += i3;
    i2 = LoadU32(ctx, i2);
    i1 += i2;
    l2 = i1;
    i2 = 93u;
    i1 = DIV_U(i1, i2);
    l3 = i1;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    l6 = i0;
    i0 = 0u;
    l4 = i0;
    i0 = a;
    l10 = i0;
    i0 = l1;
    i1 = a;
    i2 = 2916u;
    i1 += i2;
    i1 = LoadU32(ctx, i1);
    l8 = i1;
    i0 = i0 != i1;
    if (i0) {
      i0 = l6;
      i0 = LoadU32(ctx, i0);
      i1 = l2;
      i2 = l3;
      i3 = 93u;
      i2 *= i3;
      i1 -= i2;
      i2 = 44u;
      i1 *= i2;
      i0 += i1;
      l4 = i0;
    }
    i0 = l4;
    l3 = i0;
  }
  loop_27: 
    i0 = l9;
    i1 = ResumePhase(ctx);
    i1 = !(i1);
    if (i1) {
      i1 = l1;
      i2 = l8;
      i1 = i1 == i2;
      l4 = i1;
      if (i1) {
        i1 = 0u;
      } else {
        i1 = l7;
        i2 = 93u;
        i1 = DIV_U(i1, i2);
        l2 = i1;
        i2 = 2u;
        i1 <<= (i2 & 31);
        i2 = l1;
        i1 += i2;
        i1 = LoadU32(ctx, i1);
        l4 = i1;
        i1 = l4;
        i2 = l7;
        i3 = l2;
        i4 = 93u;
        i3 *= i4;
        i2 -= i3;
        i3 = 44u;
        i2 *= i3;
        i1 += i2;
      }
      i2 = l3;
      i1 = i1 == i2;
      l1 = i1;
    }
    i1 = l1;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l9 = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l1;
      i1 = a;
      i2 = 2908u;
      i1 += i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      l1 = i0;
      i0 = l5;
      i1 = 4u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l1;
        InitNodeStorage(ctx, i0);
        i0 = 4u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l11;
        i1 = 5u;
        i0 -= i1;
        l1 = i0;
        switch (i0) {
          case 0: goto block_32;
          case 1: goto block_26;
          default: goto block_24;
        }
      }
      block_32:;
    }
    i0 = l9;
    i0 = !(i0);
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        l4 = i0;
        i0 = l3;
        l1 = i0;
        i1 = l6;
        i1 = LoadU32(ctx, i1);
        i0 = i0 == i1;
        if (i0) {
          i0 = l6;
          i1 = 4u;
          i0 -= i1;
          i0 = LoadU32(ctx, i0);
          i1 = 4092u;
          i0 += i1;
        } else {
          i0 = l1;
        }
        i1 = 44u;
        i0 -= i1;
        l1 = i0;
      }
      i0 = l5;
      i1 = 5u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l4;
        i1 = l1;
        i0 = FormatLuaEventFilterLine(ctx, i0, i1);
        i0 = 5u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i1 = l6;
        i1 = LoadU32(ctx, i1);
        i0 = i0 == i1;
        if (i0) {
          i0 = l6;
          i1 = 4u;
          i0 -= i1;
          l6 = i0;
          i0 = LoadU32(ctx, i0);
          i1 = 4092u;
          i0 += i1;
        } else {
          i0 = l3;
        }
        i1 = 44u;
        i0 -= i1;
        l3 = i0;
        i0 = a;
        i0 = LoadU32(ctx, i0 + 2916u);
        l8 = i0;
        i0 = a;
        i0 = LoadU32(ctx, i0 + 2924u);
        l7 = i0;
        i0 = a;
        i0 = LoadU32(ctx, i0 + 2912u);
        l1 = i0;
        goto loop_27;
      }
    }
  i0 = a;
  i1 = 14072u;
  i0 += i1;
  i1 = ResumePhase(ctx);
  i1 = !(i1);
  if (i1) {goto block_25;}
  block_26:;
  i0 = a;
  i1 = a;
  i2 = 14160u;
  i1 += i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  block_25:;
  a = i0;
  i0 = l5;
  i1 = 6u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l10;
    i1 = a;
    i0 = FormatLuaEventFilterLine(ctx, i0, i1);
    l2 = i0;
    i0 = 6u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    a = i0;
  }
  block_24:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    goto func_exit;
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
  i0 = l5;
  i1 = 7u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 7u;
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
  i2 = kF490ResumeWordBytes;
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
  i0 = l2;
  i1 = l3;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l2;
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l2;
  i1 = l6;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l2;
  i1 = l7;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l2;
  i1 = l8;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l2;
  i1 = l10;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l2;
  i1 = l9;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = l2;
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
