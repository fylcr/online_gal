// persist_session_state.cpp -- saves the session: it sequences the autosave.dat write and drives the onClickWaitIn and onClickWaitOut Lua callbacks..
//
// PersistSessionState -- saves the session: it sequences the autosave.dat write and drives the onClickWaitIn and onClickWaitOut Lua callbacks.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 212336-212336 (f533); R1 transcribe-keep.
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
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
uint32_t CopyBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void ReleaseNodeTree(EngineContext* ctx, uint32_t state, uint32_t node);
uint32_t FormatLuaEventFilterLine(EngineContext* ctx, uint32_t a0, uint32_t a1);
void ApplyEventRecordTable(EngineContext* ctx, uint32_t a);
void FormatLuaTagFilterLine(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3);

namespace {

constexpr uint32_t kF533MsgAutosaveDat = 0xAFAF2u;  // autosave.dat
constexpr uint32_t kF533MsgLuaOnclickwaitoutSS = 0xB4ACBu;  // [Lua] onClickWaitOut %s: %s
constexpr uint32_t kF533MsgLuaOnclickwaitinSS = 0xB4AE7u;  // [Lua] onClickWaitIn %s: %s
constexpr uint32_t kF533MsgPx = 0xB5E31u;  // px != 0
constexpr uint32_t kF533MsgBoostBoostSmartPtr = 0xB5E39u;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
constexpr uint32_t kF533MsgOperator = 0xB5E71u;  // operator->
constexpr uint32_t kF533ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF533Const80000010 = 0x80000010u;  // 
constexpr uint32_t kF533ConstFFFFFFC0 = 0xFFFFFFC0u;  // 
constexpr uint32_t kF533ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF533SaveFrameBytes = 24u;
constexpr uint32_t kF533ResumeWordBytes = 4u;

}  // namespace

void PersistSessionState(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF533SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l6 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
    i0 = l6;
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
    i2 = kF533ResumeWordBytes;
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
    l2 = i0;
    GuestStackSp(ctx) = i0;
    i0 = a;
    i1 = 2876u;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i1 = 13u;
    i0 = i0 == i1;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    if (i0) {goto block_7;}
    i0 = a;
    i1 = b;
    StoreU32(ctx, i0 + 2876u, i1);
    i0 = a;
    i1 = 2880u;
    i0 += i1;
    i1 = l3;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    l4 = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 8u);
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    if (i0) {goto block_16;}
    i0 = a;
    i0 = LoadU8(ctx, i0 + 34u);
    i0 = !(i0);
    l4 = i0;
    if (i0) {goto block_16;}
    i0 = l5;
    if (i0) {goto block_15;}
    i0 = kF533ConstD015C;
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
    i1 = kF533MsgPx;
    i2 = kF533MsgBoostBoostSmartPtr;
    i3 = 734u;
    i4 = kF533MsgOperator;
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
    i0 = kF533ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF533ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 != i1;
    a = i0;
    if (i0) {goto block_6;}
    goto block_5;
  }
  block_16:;
  i0 = l4;
  i1 = l5;
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
      i0 = kF533ConstD015C;
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
      i1 = kF533MsgPx;
      i2 = kF533MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF533MsgOperator;
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
      i0 = kF533ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF533ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_6;}
      goto block_5;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 28u);
    l4 = i0;
  }
  i0 = l7;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l5;
    i1 = 0u;
    i2 = l4;
    CallTableT0(ctx, i2, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    switch (i0) {
      case 0: goto block_27;
      case 1: goto block_14;
      case 2: goto block_7;
      case 3: goto block_13;
      case 4: goto block_7;
      case 5: goto block_9;
      case 6: goto block_12;
      case 7: goto block_7;
      case 8: goto block_7;
      case 9: goto block_7;
      case 10: goto block_7;
      case 11: goto block_7;
      case 12: goto block_7;
      case 13: goto block_7;
      case 14: goto block_11;
      default: goto block_7;
    }
    block_27:;
    i0 = l3;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    if (i0) {goto block_28;}
    i0 = a;
    i1 = 14391u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    b = i0;
    i1 = kF533ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    l3 = i0;
    if (i0) {
      i0 = a;
      i1 = 14384u;
      i0 += i1;
      i0 = LoadU32(ctx, i0);
      goto block_31;
    }
    i0 = b;
    i1 = 255u;
    i0 &= i1;
    block_31:;
    i0 = !(i0);
    b = i0;
    if (i0) {goto block_28;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 136u);
    b = i0;
    i0 = !(i0);
    l3 = i0;
  }
  i0 = l3;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF533ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 3u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF533MsgPx;
      i2 = kF533MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF533MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 3u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF533ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF533ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_6;}
      goto block_5;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    j1 = 0ull;
    StoreU64(ctx, i0 + 68u, j1);
    i0 = kF533ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = l2;
    i2 = kF533ConstFFFFFFC0;
    i1 -= i2;
    i2 = 4u;
    i1 |= i2;
    StoreU32(ctx, i0 + 64u, i1);
    i0 = b;
    l3 = i0;
    i0 = l2;
    i1 = kF533ConstFFFFFFC0;
    i0 -= i1;
    l4 = i0;
    i0 = a;
    i1 = 14380u;
    i0 += i1;
    b = i0;
  }
  i0 = l7;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2595u;
    i1 = l3;
    i2 = b;
    i3 = l4;
    i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
    l6 = i0;
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l6;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF533ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF533ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto running_exit;}
    i0 = l2;
    i1 = kF533ConstFFFFFFC0;
    i0 -= i1;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 68u);
    l4 = i1;
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l5;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_28;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 136u);
    l5 = i0;
    i0 = !(i0);
    l4 = i0;
    i0 = b;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF533ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0);
      goto block_40;
    }
    i0 = b;
    block_40:;
    l3 = i0;
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
      i0 = kF533ConstD015C;
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
      i0 = 857u;
      i1 = kF533MsgPx;
      i2 = kF533MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF533MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 5u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF533ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF533ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_6;}
      goto block_5;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = 4u;
    i0 += i1;
    l4 = i0;
    i0 = l5;
    i0 = LoadS8(ctx, i0 + 15u);
    i1 = kF533ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l5;
      i0 = LoadU32(ctx, i0 + 4u);
      l4 = i0;
    }
    i0 = l2;
    i1 = l4;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = l2;
    i1 = l3;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = l2;
    i1 = 16u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l7;
  i1 = 6u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 1u;
    i2 = kF533MsgLuaOnclickwaitoutSS;
    i3 = l3;
    FormatLuaTagFilterLine(ctx, i0, i1, i2, i3);
    i0 = 6u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadS8(ctx, i0 + 14391u);
    i1 = kF533ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    l3 = i0;
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 14380u);
      i1 = 0u;
      StoreU8(ctx, i0, i1);
      i0 = a;
      i1 = 14384u;
      i0 += i1;
      b = i0;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      goto block_28;
    }
    i0 = b;
    i1 = 0u;
    StoreU8(ctx, i0 + 11u, i1);
    i0 = b;
    i1 = 0u;
    StoreU8(ctx, i0, i1);
  }
  block_28:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = 2768u;
    i0 += i1;
    j1 = 0ull;
    StoreU64(ctx, i0, j1);
    i0 = a;
    i1 = 2792u;
    i0 += i1;
    b = i0;
    i1 = 0u;
    StoreU8(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 7u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    ApplyEventRecordTable(ctx, i0);
    i0 = 7u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_7;}
  block_15:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 28u);
    b = i0;
  }
  i0 = l7;
  i1 = 8u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l5;
    i1 = 1u;
    i2 = b;
    CallTableT0(ctx, i2, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    i0 = 8u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_14:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = 14379u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    b = i0;
    i1 = kF533ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = a;
      i1 = 14372u;
      i0 += i1;
      i0 = LoadU32(ctx, i0);
      goto block_57;
    }
    i0 = b;
    i1 = 255u;
    i0 &= i1;
    block_57:;
    i0 = !(i0);
    b = i0;
    if (i0) {goto block_13;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 136u);
    b = i0;
    i0 = !(i0);
    l3 = i0;
  }
  i0 = l3;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF533ConstD015C;
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
      i1 = kF533MsgPx;
      i2 = kF533MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF533MsgOperator;
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
      i0 = kF533ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF533ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_6;}
      goto block_5;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    j1 = 0ull;
    StoreU64(ctx, i0 + 68u, j1);
    i0 = kF533ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = l2;
    i2 = kF533ConstFFFFFFC0;
    i1 -= i2;
    i2 = 4u;
    i1 |= i2;
    StoreU32(ctx, i0 + 64u, i1);
    i0 = b;
    l3 = i0;
    i0 = l2;
    i1 = kF533ConstFFFFFFC0;
    i0 -= i1;
    l4 = i0;
    i0 = a;
    i1 = 14368u;
    i0 += i1;
    b = i0;
  }
  i0 = l7;
  i1 = 10u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2595u;
    i1 = l3;
    i2 = b;
    i3 = l4;
    i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
    l6 = i0;
    i0 = 10u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l6;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF533ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF533ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto running_exit;}
    i0 = l2;
    i1 = kF533ConstFFFFFFC0;
    i0 -= i1;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 68u);
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l5;
    i0 = !(i0);
    if (i0) {goto block_13;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 136u);
    l5 = i0;
    i0 = !(i0);
    l4 = i0;
    i0 = b;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF533ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0);
      goto block_66;
    }
    i0 = b;
    block_66:;
    l3 = i0;
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
      i0 = kF533ConstD015C;
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
      i0 = 857u;
      i1 = kF533MsgPx;
      i2 = kF533MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF533MsgOperator;
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
      i0 = kF533ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF533ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_6;}
      goto block_5;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = 4u;
    i0 += i1;
    l4 = i0;
    i0 = l2;
    i1 = l5;
    i1 = LoadS8(ctx, i1 + 15u);
    i2 = kF533ConstFFFFFFFF;
    i1 = (uint32_t)((int32_t)i1 <= (int32_t)i2);
    if (i1) {
      i1 = l5;
      i1 = LoadU32(ctx, i1 + 4u);
    } else {
      i1 = l4;
    }
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l2;
    i1 = l3;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 12u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 1u;
    i2 = kF533MsgLuaOnclickwaitinSS;
    i3 = l2;
    FormatLuaTagFilterLine(ctx, i0, i1, i2, i3);
    i0 = 12u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadS8(ctx, i0 + 14379u);
    i1 = kF533ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 14368u);
      i1 = 0u;
      StoreU8(ctx, i0, i1);
      i0 = a;
      i1 = 14372u;
      i0 += i1;
      b = i0;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      goto block_13;
    }
    i0 = b;
    i1 = 0u;
    StoreU8(ctx, i0 + 11u, i1);
    i0 = b;
    i1 = 0u;
    StoreU8(ctx, i0, i1);
  }
  block_13:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU8(ctx, i0 + 48u);
    if (i0) {
      i0 = a;
      i0 = LoadU8(ctx, i0 + 49u);
      if (i0) {goto block_7;}
      i0 = a;
      i0 = LoadU8(ctx, i0 + 50u);
      if (i0) {goto block_7;}
    }
    i0 = a;
    i0 = LoadU32(ctx, i0 + 13212u);
    if (i0) {goto block_7;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 128u);
    l5 = i0;
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
      i0 = kF533ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 13u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF533MsgPx;
      i2 = kF533MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF533MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 13u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF533ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF533ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_6;}
      goto block_5;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 12u;
    i1 = 0u;
    i2 = a;
    i2 = LoadU32(ctx, i2 + 2956u);
    b = i2;
    i3 = 2u;
    i2 = i2 == i3;
    i0 = i2 ? i0 : i1;
    a = i0;
    i0 = b;
    i1 = 2u;
    i0 = i0 == i1;
    b = i0;
  }
  i0 = b;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l7;
    i1 = 14u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 16u;
      i0 = AllocateStringBuffer(ctx, i0);
      l6 = i0;
      i0 = 14u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l6;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i1 = kF533Const80000010;
      StoreU32(ctx, i0 + 56u, i1);
      i0 = l2;
      i1 = b;
      StoreU32(ctx, i0 + 48u, i1);
      i0 = l2;
      i1 = a;
      StoreU32(ctx, i0 + 52u, i1);
      i0 = b;
      i1 = kF533MsgAutosaveDat;
      i2 = a;
      i0 = CopyBytes(ctx, i0, i1, i2);
      goto block_84;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i1 = a;
    StoreU8(ctx, i0 + 59u, i1);
    i0 = l2;
    i1 = 48u;
    i0 += i1;
    b = i0;
  }
  block_84:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = b;
    i0 += i1;
    i1 = 0u;
    StoreU8(ctx, i0, i1);
    i0 = kF533ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 48u;
    i0 += i1;
    a = i0;
  }
  i0 = l7;
  i1 = 15u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2600u;
    i1 = l5;
    i2 = a;
    i3 = 0u;
    i4 = 0u;
    HostFailAssertion(ctx, i0, i1, i2, i3, i4);
    i0 = 15u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF533ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF533ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {
      i0 = l2;
      i0 = LoadS8(ctx, i0 + 59u);
      i1 = kF533ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_7;}
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 48u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_7;
    }
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = l2;
    i0 = LoadS8(ctx, i0 + 59u);
    i1 = kF533ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    b = i0;
    if (i0) {goto block_10;}
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 48u);
    DeallocateRawBlock(ctx, i0);
    i0 = a;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_12:;
  i0 = b;
  i1 = a;
  i2 = 14116u;
  i1 += i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  b = i0;
  i0 = l7;
  i1 = 16u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = b;
    i0 = FormatLuaEventFilterLine(ctx, i0, i1);
    l6 = i0;
    i0 = 16u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l6;
    a = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_7;}
  block_11:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
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
      i0 = kF533ConstD015C;
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
      i1 = kF533MsgPx;
      i2 = kF533MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF533MsgOperator;
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
      i0 = kF533ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF533ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_6;}
      goto block_5;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i1 = 0u;
    StoreU8(ctx, i0 + 32u, i1);
    i0 = l2;
    i1 = 0u;
    StoreU8(ctx, i0 + 43u, i1);
    i0 = kF533ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 32u;
    i0 += i1;
    b = i0;
  }
  i0 = l7;
  i1 = 18u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2600u;
    i1 = a;
    i2 = b;
    i3 = 0u;
    i4 = 1u;
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
    i0 = kF533ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF533ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {
      i0 = l2;
      i0 = LoadS8(ctx, i0 + 43u);
      i1 = kF533ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_7;}
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 32u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_7;
    }
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = l2;
    i0 = LoadS8(ctx, i0 + 43u);
    i1 = kF533ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    b = i0;
    if (i0) {goto block_10;}
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 32u);
    b = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_10:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_9:;
  i0 = b;
  i1 = a;
  i2 = 14028u;
  i1 += i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  b = i0;
  i0 = l7;
  i1 = 19u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = b;
    i0 = FormatLuaEventFilterLine(ctx, i0, i1);
    l6 = i0;
    i0 = 19u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l6;
    a = i0;
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i1 = 80u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    TrapUnreachable();
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    a = i0;
    i0 = HostTempRet0(ctx);
  }
  i0 = l7;
  i1 = 20u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 20u;
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
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = l2;
    i1 = kF533ConstFFFFFFC0;
    i0 -= i1;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 68u);
    ReleaseNodeTree(ctx, i0, i1);
    i0 = a;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  goto func_exit;
  spill_frame:;
  l6 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l6;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF533ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l6 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l6;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l6;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l6;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l6;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l6;
  i1 = l5;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 24u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
