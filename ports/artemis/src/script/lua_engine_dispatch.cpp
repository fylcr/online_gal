// lua_engine_dispatch.cpp -- the Lua engine dispatch entry: one suspendable step drains the queued tag dispatch work and returns the resume status..
//
// LuaEngine::Dispatch -- the Lua engine dispatch entry: one suspendable step drains the queued tag dispatch work and returns the resume status.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 330574-330574 (f859); R1 transcribe-keep.
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
uint32_t FillBytes(EngineContext* ctx, uint32_t dst, uint32_t value, uint32_t length);
uint32_t StringLength(EngineContext* ctx, uint32_t text);
uint32_t DestroyDispatchObject(EngineContext* ctx, uint32_t a0);

namespace {

constexpr uint32_t kF859Const8AD44 = 0x8AD44u;  // 
constexpr uint32_t kF859Const8B07C = 0x8B07Cu;  // 
constexpr uint32_t kF859Const8E084 = 0x8E084u;  // 
constexpr uint32_t kF859ConstAF4FF = 0xAF4FFu;  // 
constexpr uint32_t kF859MsgIet = 0xB4A71u;  // .iet
constexpr uint32_t kF859MsgScript = 0xB4A76u;  // script
constexpr uint32_t kF859ConstB4A7A = 0xB4A7Au;  // 
constexpr uint32_t kF859MsgSDLuaS = 0xB4A7Du;  // %s(%d): [Lua] %s
constexpr uint32_t kF859MsgSS = 0xB4A8Eu;  // %s%s
constexpr uint32_t kF859MsgAllocatorTAllocateSize = 0xB5BC3u;  // allocator<T>::allocate(size_t n) 'n' exceeds maximum supported size
constexpr uint32_t kF859ConstB5E30 = 0xB5E30u;  // 
constexpr uint32_t kF859MsgPx = 0xB5E31u;  // px != 0
constexpr uint32_t kF859MsgBoostBoostSmartPtr = 0xB5E39u;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
constexpr uint32_t kF859MsgOperator = 0xB5E71u;  // operator->
constexpr uint32_t kF859ConstB5F40 = 0xB5F40u;  // 
constexpr uint32_t kF859MsgOperatorX = 0xB6B10u;  // operator[]
constexpr uint32_t kF859ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF859Const1FFFFFFF = 0x1FFFFFFFu;  // 
constexpr uint32_t kF859Const3FFFFFFF = 0x3FFFFFFFu;  // 
constexpr uint32_t kF859Const40000000 = 0x40000000u;  // 
constexpr uint32_t kF859Const80000000 = 0x80000000u;  // 
constexpr uint32_t kF859ConstFFFFFFC0 = 0xFFFFFFC0u;  // 
constexpr uint32_t kF859ConstFFFFFFF0 = 0xFFFFFFF0u;  // 
constexpr uint32_t kF859ConstFFFFFFFC = 0xFFFFFFFCu;  // 
constexpr uint32_t kF859ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF859SaveFrameBytes = 60u;
constexpr uint32_t kF859ResumeWordBytes = 4u;

}  // namespace

namespace LuaEngine {
uint32_t Dispatch(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l13 = 0u; uint32_t l14 = 0u; uint32_t l15 = 0u; uint32_t l16 = 0u; float l17 = 0; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u; uint32_t i6 = 0u;
  uint64_t j1 = 0;
  float f0 = 0; float f1 = 0;
      l10 = 0, l11 = 0, l12 = 0, l13 = 0, l14 = 0, l15 = 0, l16 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF859SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l8 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
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
    i0 = LoadU32(ctx, i0 + 40u);
    l12 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 44u);
    l13 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 48u);
    l14 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 52u);
    l15 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 56u);
    l16 = i0;
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
    i2 = kF859ResumeWordBytes;
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
    i1 = 176u;
    i0 -= i1;
    l4 = i0;
    GuestStackSp(ctx) = i0;
    i0 = b;
    i0 = LoadU8(ctx, i0 + 11u);
    l5 = i0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    l9 = i0;
    i0 = a;
    i1 = 2688u;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = !(i0);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0);
      l10 = i0;
      i0 = b;
      i0 = LoadU32(ctx, i0 + 4u);
      l14 = i0;
      goto block_7;
    }
    i0 = b;
    i0 = LoadU32(ctx, i0);
    l10 = i0;
    i1 = b;
    i2 = l9;
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    l2 = i2;
    i0 = i2 ? i0 : i1;
    l6 = i0;
    i0 = b;
    i0 = LoadU32(ctx, i0 + 4u);
    l14 = i0;
    i1 = l5;
    i2 = l2;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    loop_9: 
      i0 = l3;
      i0 = LoadU8(ctx, i0 + 27u);
      l11 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l13 = i0;
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 20u);
      i1 = l11;
      i2 = l13;
      i0 = i2 ? i0 : i1;
      l11 = i0;
      i1 = l2;
      i0 = i0 < i1;
      l16 = i0;
      i0 = l11;
      i1 = l2;
      i2 = l16;
      i0 = i2 ? i0 : i1;
      l12 = i0;
      if (i0) {
        i0 = l6;
        i1 = l3;
        i2 = 16u;
        i1 += i2;
        l15 = i1;
        i1 = LoadU32(ctx, i1);
        i2 = l15;
        i3 = l13;
        i1 = i3 ? i1 : i2;
        l13 = i1;
        i2 = l12;
        i0 = CompareBytes(ctx, i0, i1, i2);
        l15 = i0;
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          i1 = l11;
          i0 = i0 >= i1;
          if (i0) {goto block_15;}
          goto block_10;
        }
        i0 = l15;
        i1 = kF859ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {goto block_10;}
        block_15:;
        i0 = l13;
        i1 = l6;
        i2 = l12;
        i0 = CompareBytes(ctx, i0, i1, i2);
        l11 = i0;
        i0 = !(i0);
        l12 = i0;
        if (i0) {goto block_13;}
        i0 = l11;
        i1 = kF859ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {goto block_12;}
        goto block_11;
      }
      i0 = l2;
      i1 = l11;
      i0 = i0 < i1;
      if (i0) {goto block_10;}
      block_13:;
      i0 = l16;
      if (i0) {goto block_11;}
      block_12:;
      i0 = 0u;
      l3 = i0;
      goto block_5;
      block_11:;
      i0 = l3;
      i1 = 4u;
      i0 += i1;
      l3 = i0;
      block_10:;
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      if (i0) {goto loop_9;}
    block_7:;
    i0 = l4;
    i1 = 0u;
    StoreU32(ctx, i0 + 160u, i1);
    i0 = l4;
    j1 = 0ull;
    StoreU64(ctx, i0 + 152u, j1);
    i0 = l14;
    i1 = l5;
    i2 = l9;
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    l11 = i2;
    i0 = i2 ? i0 : i1;
    l3 = i0;
    i1 = 4u;
    i0 += i1;
    l2 = i0;
    i1 = kF859ConstFFFFFFF0;
    i0 = i0 >= i1;
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
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 152u;
      i0 += i1;
      a = i0;
    }
    i0 = l7;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1286u;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_19;}
      goto block_17;
    }
  }
  i0 = l2;
  i1 = l2;
  i2 = 11u;
  i1 = i1 >= i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l2 = i0;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 20u;
      i0 += i1;
      l2 = i0;
      i1 = kF859ConstFFFFFFF0;
      i0 &= i1;
      l5 = i0;
    }
    i0 = l7;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = l5;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l8 = i0;
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      l9 = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l9;
      i1 = 1u;
      i0 = i0 == i1;
      l6 = i0;
      if (i0) {goto block_19;}
      i0 = l4;
      i1 = l2;
      StoreU32(ctx, i0 + 152u, i1);
      i0 = l4;
      i1 = l3;
      StoreU32(ctx, i0 + 156u, i1);
      i0 = l4;
      i1 = l5;
      i2 = kF859Const80000000;
      i1 |= i2;
      l5 = i1;
      StoreU32(ctx, i0 + 160u, i1);
      goto block_25;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l3;
    StoreU8(ctx, i0 + 163u, i1);
    i0 = l4;
    i1 = 152u;
    i0 += i1;
    l2 = i0;
    i0 = l3;
    i0 = !(i0);
    l5 = i0;
    if (i0) {goto block_24;}
  }
  block_25:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i1 = l10;
    i2 = b;
    i3 = l11;
    i1 = i3 ? i1 : i2;
    i2 = l3;
    i0 = CopyBytes(ctx, i0, i1, i2);
    l5 = i0;
  }
  block_24:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i1 = l3;
    i0 += i1;
    i1 = 0u;
    StoreU8(ctx, i0, i1);
    i0 = kF859ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 152u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l7;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 860u;
    i1 = l3;
    i2 = kF859MsgIet;
    i3 = 4u;
    i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
    l8 = i0;
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF859ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF859ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 != i1;
    l3 = i0;
    if (i0) {goto block_18;}
  }
  block_19:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 163u);
    i1 = kF859ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 152u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = a;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_18:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 2688u);
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
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 152u);
      i1 = l4;
      i2 = 152u;
      i1 += i2;
      l6 = i1;
      i2 = l4;
      i2 = LoadU8(ctx, i2 + 163u);
      l2 = i2;
      i3 = 24u;
      i2 <<= (i3 & 31);
      i3 = 24u;
      i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      l11 = i2;
      i0 = i2 ? i0 : i1;
      l5 = i0;
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 156u);
      i1 = l2;
      i2 = l11;
      i0 = i2 ? i0 : i1;
      l2 = i0;
    }
    loop_55: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadU8(ctx, i0 + 27u);
        l11 = i0;
        i1 = 24u;
        i0 <<= (i1 & 31);
        i1 = 24u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l10 = i0;
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 20u);
        l6 = i0;
        i1 = l11;
        i2 = l10;
        i0 = i2 ? i0 : i1;
        l11 = i0;
        i1 = l2;
        i0 = i0 < i1;
        l14 = i0;
        i0 = l11;
        i1 = l2;
        i2 = l14;
        i0 = i2 ? i0 : i1;
        l9 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l9;
        if (i0) {
          i0 = l5;
          i1 = l3;
          i2 = 16u;
          i1 += i2;
          l6 = i1;
          i1 = LoadU32(ctx, i1);
          i2 = l6;
          i3 = l10;
          i1 = i3 ? i1 : i2;
          l10 = i1;
          i2 = l9;
          i0 = CompareBytes(ctx, i0, i1, i2);
          l6 = i0;
          i0 = !(i0);
          l12 = i0;
          if (i0) {
            i0 = l2;
            i1 = l11;
            i0 = i0 >= i1;
            l6 = i0;
            if (i0) {goto block_63;}
            goto block_57;
          }
          i0 = l6;
          i1 = kF859ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          l6 = i0;
          if (i0) {goto block_57;}
          block_63:;
          i0 = l10;
          i1 = l5;
          i2 = l9;
          i0 = CompareBytes(ctx, i0, i1, i2);
          l11 = i0;
          i0 = !(i0);
          l6 = i0;
          if (i0) {goto block_61;}
          i0 = l11;
          i1 = kF859ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          l6 = i0;
          if (i0) {goto block_60;}
          goto block_58;
        }
        i0 = l2;
        i1 = l11;
        i0 = i0 < i1;
        l6 = i0;
        if (i0) {goto block_57;}
        block_61:;
        i0 = l14;
        if (i0) {goto block_58;}
        block_60:;
        i0 = 0u;
        l3 = i0;
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 152u;
        i0 += i1;
        a = i0;
      }
      i0 = l7;
      i1 = 3u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 870u;
        i1 = b;
        i2 = a;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l8 = i0;
        i0 = 3u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l8;
        a = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF859ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_50;}
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        a = i0;
        goto block_37;
      }
      block_58:;
      i0 = l3;
      i1 = l3;
      i2 = 4u;
      i1 += i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      l3 = i0;
      block_57:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        if (i0) {goto loop_55;}
      }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF859ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 88u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l7;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2705u;
    i1 = l3;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF859ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF859ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 != i1;
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
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 88u);
      l3 = i0;
      i0 = !(i0);
      l2 = i0;
    }
    i0 = l2;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF859ConstD015C;
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
        i1 = kF859MsgPx;
        i2 = kF859MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF859MsgOperator;
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
        i0 = kF859ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_17;}
        goto running_exit;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 92u);
      l11 = i0;
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 20u);
      l2 = i0;
      i0 = b;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = b;
      i0 = LoadS8(ctx, i0 + 11u);
      l9 = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = b;
      i2 = l9;
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      l6 = i2;
      i0 = i2 ? i0 : i1;
      l5 = i0;
    }
    i0 = l7;
    i1 = 6u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = l3;
      i2 = l5;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l8 = i0;
      i0 = 6u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_76;}
      i0 = l2;
      i0 = !(i0);
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      if (i0) {goto block_89;}
      i0 = l4;
      i1 = 0u;
      StoreU32(ctx, i0 + 96u, i1);
      i0 = l4;
      j1 = 0ull;
      StoreU64(ctx, i0 + 88u, j1);
      i0 = b;
      i0 = LoadU8(ctx, i0 + 11u);
      l2 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l9 = i0;
      i0 = b;
      i0 = LoadU32(ctx, i0 + 4u);
      i1 = l2;
      i2 = l9;
      i0 = i2 ? i0 : i1;
      l5 = i0;
      i1 = 4u;
      i0 += i1;
      l2 = i0;
      i1 = kF859ConstFFFFFFF0;
      i0 = i0 >= i1;
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
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 88u;
        i0 += i1;
        a = i0;
      }
      i0 = l7;
      i1 = 7u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1286u;
        i1 = a;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 7u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF859ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_17;}
        goto block_91;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0);
      l10 = i0;
      i0 = l2;
      i1 = 11u;
      i0 = i0 >= i1;
      l2 = i0;
    }
    i0 = l2;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 20u;
        i0 += i1;
        l2 = i0;
        i1 = kF859ConstFFFFFFF0;
        i0 &= i1;
        l14 = i0;
      }
      i0 = l7;
      i1 = 8u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1032u;
        i1 = l14;
        i0 = HostFoldLayoutOffset(ctx, i0, i1);
        l8 = i0;
        i0 = 8u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l8;
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF859ConstD015C;
        i0 = LoadU32(ctx, i0);
        l6 = i0;
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l6;
        i1 = 1u;
        i0 = i0 == i1;
        l6 = i0;
        if (i0) {goto block_91;}
        i0 = l4;
        i1 = l2;
        StoreU32(ctx, i0 + 88u, i1);
        i0 = l4;
        i1 = l5;
        StoreU32(ctx, i0 + 92u, i1);
        i0 = l4;
        i1 = l14;
        i2 = kF859Const80000000;
        i1 |= i2;
        l6 = i1;
        StoreU32(ctx, i0 + 96u, i1);
        goto block_99;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = l5;
      StoreU8(ctx, i0 + 99u, i1);
      i0 = l4;
      i1 = 88u;
      i0 += i1;
      l2 = i0;
      i0 = l5;
      i0 = !(i0);
      l6 = i0;
      if (i0) {goto block_98;}
    }
    block_99:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i1 = l10;
      i2 = b;
      i3 = l9;
      i1 = i3 ? i1 : i2;
      i2 = l5;
      i0 = CopyBytes(ctx, i0, i1, i2);
      l6 = i0;
    }
    block_98:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i1 = l5;
      i0 += i1;
      i1 = 0u;
      StoreU8(ctx, i0, i1);
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 88u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l7;
    i1 = 9u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 860u;
      i1 = l2;
      i2 = kF859MsgIet;
      i3 = 4u;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l8 = i0;
      i0 = 9u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_90;}
    }
    block_91:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 99u);
      i1 = kF859ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_38;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 88u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_38;
    }
    block_90:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 20u);
      l2 = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 88u);
      i1 = l4;
      i2 = 88u;
      i1 += i2;
      l6 = i1;
      i2 = l4;
      i2 = LoadS8(ctx, i2 + 99u);
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      l9 = i2;
      i0 = i2 ? i0 : i1;
      l5 = i0;
    }
    i0 = l7;
    i1 = 10u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = l3;
      i2 = l5;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l8 = i0;
      i0 = 10u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF859ConstD015C;
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
        i0 = l2;
        if (i0) {goto block_75;}
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 88u;
        i0 += i1;
        l2 = i0;
      }
      i0 = l7;
      i1 = 11u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 870u;
        i1 = b;
        i2 = l2;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l8 = i0;
        i0 = 11u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l8;
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF859ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 != i1;
        l2 = i0;
        if (i0) {goto block_114;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 99u);
      i1 = kF859ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_38;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 88u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_38;
    }
    block_114:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 99u);
      i1 = kF859ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      l2 = i0;
      if (i0) {goto block_89;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 88u);
      l2 = i0;
      DeallocateRawBlock(ctx, i0);
    }
    block_89:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 36u);
      l2 = i0;
      i0 = kF859ConstD015C;
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
      i0 = l2;
      i1 = l3;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l8 = i0;
      i0 = 12u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l5 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_39;}
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l5;
      StoreU32(ctx, i0 + 148u, i1);
      i0 = l5;
      i1 = 24u;
      i0 += i1;
      i1 = kF859ConstFFFFFFFC;
      i0 &= i1;
      l2 = i0;
    }
    i0 = l7;
    i1 = 13u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = l2;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l8 = i0;
      i0 = 13u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      l9 = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l9;
      i1 = 1u;
      i0 = i0 == i1;
      l6 = i0;
      if (i0) {goto block_74;}
      i0 = l2;
      j1 = 4294967297ull;
      StoreU64(ctx, i0 + 4u, j1);
      i0 = l2;
      i1 = l5;
      i2 = 1u;
      i1 += i2;
      l9 = i1;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = kF859Const8B07C;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 20u;
      i0 += i1;
      l10 = i0;
      i0 = l9;
      if (i0) {
        i0 = l10;
        i1 = 0u;
        i2 = l9;
        i0 = FillBytes(ctx, i0, i1, i2);
      }
      i0 = l4;
      i1 = l2;
      StoreU32(ctx, i0 + 140u, i1);
      i0 = l4;
      i1 = l10;
      StoreU32(ctx, i0 + 136u, i1);
      i0 = l10;
      i1 = 0u;
      i2 = l9;
      i0 = FillBytes(ctx, i0, i1, i2);
      l2 = i0;
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = LoadU32(ctx, i0 + 24u);
      l9 = i0;
      i0 = kF859ConstD015C;
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
      i0 = l9;
      i1 = l3;
      i2 = l2;
      i3 = l5;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l8 = i0;
      i0 = 14u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_41;}
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 128u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l7;
    i1 = 15u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2706u;
      i1 = l3;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 15u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_73;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 136u);
      l3 = i0;
      i0 = !(i0);
      l2 = i0;
    }
    i0 = l2;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 16u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF859MsgPx;
        i2 = kF859MsgBoostBoostSmartPtr;
        i3 = 740u;
        i4 = kF859MsgOperatorX;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 16u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF859ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_17;}
        goto running_exit;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU8(ctx, i0);
      i1 = 65u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_72;}
      i0 = l3;
      i0 = LoadU8(ctx, i0 + 1u);
      i1 = 83u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_72;}
      i0 = l3;
      i0 = LoadU8(ctx, i0 + 2u);
      i1 = 66u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_72;}
      i0 = l3;
      i0 = LoadU8(ctx, i0 + 4u);
      l2 = i0;
      if (i0) {goto block_72;}
      i0 = l4;
      j1 = 4398046511104ull;
      StoreU64(ctx, i0 + 108u, j1);
      i0 = l4;
      j1 = 0ull;
      StoreU64(ctx, i0 + 100u, j1);
      i0 = l4;
      i1 = l4;
      i2 = 88u;
      i1 += i2;
      i2 = 4u;
      i1 |= i2;
      l3 = i1;
      StoreU32(ctx, i0 + 96u, i1);
      i0 = l4;
      i1 = 0u;
      StoreU32(ctx, i0 + 124u, i1);
      i0 = l4;
      j1 = 0ull;
      StoreU64(ctx, i0 + 116u, j1);
      i0 = l4;
      i1 = kF859Const8AD44;
      StoreU32(ctx, i0 + 88u, i1);
      i0 = l4;
      i1 = l3;
      StoreU32(ctx, i0 + 92u, i1);
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 5u;
      StoreU32(ctx, i0 + 32u, i1);
      i0 = l4;
      i1 = 32u;
      i0 += i1;
      l5 = i0;
      i0 = l4;
      i1 = 148u;
      i0 += i1;
      l6 = i0;
      i0 = l4;
      i1 = 80u;
      i0 += i1;
      l3 = i0;
      i0 = l4;
      i1 = 136u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l7;
    i1 = 17u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2707u;
      i1 = l3;
      i2 = l2;
      i3 = l5;
      i4 = l6;
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
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_45;}
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 88u;
      i0 += i1;
      l3 = i0;
      i0 = l4;
      i1 = 80u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l7;
    i1 = 18u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2195u;
      i1 = l3;
      i2 = l2;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 18u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_44;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 84u);
      l3 = i0;
      i0 = !(i0);
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      if (i0) {goto block_144;}
      i0 = l3;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 4u);
      l2 = i1;
      i2 = 1u;
      i1 -= i2;
      l5 = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_144;}
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      l2 = i0;
      i0 = kF859ConstD015C;
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
      i0 = l2;
      i1 = l3;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 19u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_143;}
      i0 = l3;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 8u);
      l2 = i1;
      i2 = 1u;
      i1 -= i2;
      l5 = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_144;}
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      l2 = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 20u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = l3;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 20u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_143;}
    }
    block_144:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 0u;
      StoreU32(ctx, i0 + 72u, i1);
      i0 = l4;
      i1 = 88u;
      i0 += i1;
      l3 = i0;
      i0 = l4;
      i1 = 72u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l7;
    i1 = 21u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1291u;
      i1 = l3;
      i2 = l2;
      i3 = 4u;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 21u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_46;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 72u);
      i1 = 1u;
      i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
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
        i0 = l4;
        i1 = 44u;
        i0 += i1;
        l16 = i0;
        i0 = 0u;
        l14 = i0;
      }
      loop_156: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF859ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = 0u;
          StoreU32(ctx, i0 + 68u, i1);
          i0 = l4;
          i1 = 88u;
          i0 += i1;
          l3 = i0;
          i0 = l4;
          i1 = 68u;
          i0 += i1;
          l2 = i0;
        }
        i0 = l7;
        i1 = 22u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1291u;
          i1 = l3;
          i2 = l2;
          i3 = 4u;
          HostSyncSlot(ctx, i0, i1, i2, i3);
          i0 = 22u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF859ConstD015C;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          i0 = kF859ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 != i1;
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
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = 0u;
            StoreU32(ctx, i0 + 64u, i1);
            i0 = l4;
            i1 = 88u;
            i0 += i1;
            l3 = i0;
            i0 = l4;
            i1 = kF859ConstFFFFFFC0;
            i0 -= i1;
            l2 = i0;
          }
          i0 = l7;
          i1 = 23u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 1291u;
            i1 = l3;
            i2 = l2;
            i3 = 4u;
            HostSyncSlot(ctx, i0, i1, i2, i3);
            i0 = 23u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF859ConstD015C;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 1u;
            i0 = i0 == i1;
            l3 = i0;
            if (i0) {goto block_160;}
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 64u);
            l2 = i0;
            i1 = 24u;
            i0 += i1;
            i1 = kF859ConstFFFFFFFC;
            i0 &= i1;
            l3 = i0;
          }
          i0 = l7;
          i1 = 24u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 1032u;
            i1 = l3;
            i0 = HostFoldLayoutOffset(ctx, i0, i1);
            l8 = i0;
            i0 = 24u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l3 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF859ConstD015C;
            i0 = LoadU32(ctx, i0);
            l5 = i0;
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l5;
            i1 = 1u;
            i0 = i0 == i1;
            l5 = i0;
            if (i0) {goto block_179;}
            i0 = l3;
            j1 = 4294967297ull;
            StoreU64(ctx, i0 + 4u, j1);
            i0 = l3;
            i1 = l2;
            i2 = 1u;
            i1 += i2;
            l2 = i1;
            StoreU32(ctx, i0 + 16u, i1);
            i0 = l3;
            i1 = kF859Const8E084;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 20u;
            i0 += i1;
            l10 = i0;
            i0 = l2;
            if (i0) {
              i0 = l10;
              i1 = 0u;
              i2 = l2;
              i0 = FillBytes(ctx, i0, i1, i2);
            }
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = 88u;
            i0 += i1;
            l5 = i0;
          }
          i0 = l7;
          i1 = 25u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 1291u;
            i1 = l5;
            i2 = l10;
            i3 = l2;
            HostSyncSlot(ctx, i0, i1, i2, i3);
            i0 = 25u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF859ConstD015C;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 == i1;
            l2 = i0;
            if (i0) {goto block_162;}
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 68u);
            l2 = i0;
            switch (i0) {
              case 0: goto block_191;
              case 1: goto block_163;
              default: goto block_159;
            }
            block_191:;
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l7;
          i1 = 26u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 1032u;
            i1 = 44u;
            i0 = HostFoldLayoutOffset(ctx, i0, i1);
            l8 = i0;
            i0 = 26u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l5 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF859ConstD015C;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 == i1;
            l2 = i0;
            if (i0) {goto block_178;}
            i0 = l10;
            i0 = StringLength(ctx, i0);
            l2 = i0;
            i1 = kF859ConstFFFFFFF0;
            i0 = i0 >= i1;
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
              i0 = kF859ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = l4;
              i1 = 32u;
              i0 += i1;
              a = i0;
            }
            i0 = l7;
            i1 = 27u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 1286u;
              i1 = a;
              HostReleaseSlotPair(ctx, i0, i1);
              i0 = 27u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF859ConstD015C;
              i0 = LoadU32(ctx, i0);
              a = i0;
              i0 = kF859ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = a;
              i1 = 1u;
              i0 = i0 != i1;
              a = i0;
              if (i0) {goto block_17;}
              i0 = HostRet0(ctx);
              b = i0;
              i0 = HostTempRet0(ctx);
              a = i0;
              i0 = l5;
              DeallocateRawBlock(ctx, i0);
              goto block_161;
            }
          }
          i0 = l6;
          i1 = l2;
          i2 = 11u;
          i1 = i1 >= i2;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          l6 = i0;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF859ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = l2;
              i1 = 16u;
              i0 += i1;
              i1 = kF859ConstFFFFFFF0;
              i0 &= i1;
              l6 = i0;
            }
            i0 = l7;
            i1 = 28u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 1032u;
              i1 = l6;
              i0 = HostFoldLayoutOffset(ctx, i0, i1);
              l8 = i0;
              i0 = 28u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
              i0 = l8;
              l9 = i0;
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF859ConstD015C;
              i0 = LoadU32(ctx, i0);
              l12 = i0;
              i0 = kF859ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = l12;
              i1 = 1u;
              i0 = i0 == i1;
              l12 = i0;
              if (i0) {goto block_177;}
              i0 = l4;
              i1 = l9;
              StoreU32(ctx, i0 + 32u, i1);
              i0 = l4;
              i1 = l2;
              StoreU32(ctx, i0 + 36u, i1);
              i0 = l4;
              i1 = l6;
              i2 = kF859Const80000000;
              i1 |= i2;
              l6 = i1;
              StoreU32(ctx, i0 + 40u, i1);
              goto block_199;
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l4;
            i1 = l2;
            StoreU8(ctx, i0 + 43u, i1);
            i0 = l4;
            i1 = 32u;
            i0 += i1;
            l9 = i0;
            i0 = l2;
            i0 = !(i0);
            l6 = i0;
            if (i0) {goto block_198;}
          }
          block_199:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l9;
            i1 = l10;
            i2 = l2;
            i0 = CopyBytes(ctx, i0, i1, i2);
            l6 = i0;
          }
          block_198:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l2;
            i1 = l9;
            i0 += i1;
            i1 = 0u;
            StoreU8(ctx, i0, i1);
            i0 = l5;
            i1 = kF859ConstB5F40;
            StoreU32(ctx, i0, i1);
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = 32u;
            i0 += i1;
            l6 = i0;
            i0 = l5;
            i1 = 4u;
            i0 += i1;
            l2 = i0;
          }
          i0 = l7;
          i1 = 29u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 873u;
            i1 = l2;
            i2 = l6;
            i0 = HostOsQuery(ctx, i0, i1, i2);
            l8 = i0;
            i0 = 29u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l2 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF859ConstD015C;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 == i1;
            l2 = i0;
            if (i0) {goto block_176;}
            i0 = l5;
            j1 = 0ull;
            StoreU64(ctx, i0 + 20u, j1);
            i0 = l5;
            j1 = 0ull;
            StoreU64(ctx, i0 + 28u, j1);
            i0 = l5;
            i1 = l5;
            i2 = 20u;
            i1 += i2;
            StoreU32(ctx, i0 + 16u, i1);
            i0 = l5;
            j1 = 0ull;
            StoreU64(ctx, i0 + 36u, j1);
            i0 = l4;
            i0 = LoadS8(ctx, i0 + 43u);
            i1 = kF859ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            if (i0) {
              i0 = l4;
              i0 = LoadU32(ctx, i0 + 32u);
              DeallocateRawBlock(ctx, i0);
            }
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l5;
            i1 = 40u;
            i0 += i1;
            l6 = i0;
            i0 = l4;
            i1 = 88u;
            i0 += i1;
            l2 = i0;
          }
          i0 = l7;
          i1 = 30u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 1291u;
            i1 = l2;
            i2 = l6;
            i3 = 4u;
            HostSyncSlot(ctx, i0, i1, i2, i3);
            i0 = 30u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF859ConstD015C;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 == i1;
            l2 = i0;
            if (i0) {goto block_178;}
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = 0u;
            StoreU32(ctx, i0 + 60u, i1);
            i0 = l4;
            i1 = 60u;
            i0 += i1;
            l6 = i0;
            i0 = l4;
            i1 = 88u;
            i0 += i1;
            l2 = i0;
          }
          i0 = l7;
          i1 = 31u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 1291u;
            i1 = l2;
            i2 = l6;
            i3 = 4u;
            HostSyncSlot(ctx, i0, i1, i2, i3);
            i0 = 31u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF859ConstD015C;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 == i1;
            l2 = i0;
            if (i0) {goto block_175;}
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 60u);
            i1 = 1u;
            i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
            l2 = i0;
          }
          i0 = l2;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = l5;
              i1 = 16u;
              i0 += i1;
              l12 = i0;
              i0 = 0u;
              l9 = i0;
            }
            loop_217: 
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = l4;
                j1 = 0ull;
                StoreU64(ctx, i0 + 48u, j1);
                i0 = l4;
                j1 = 0ull;
                StoreU64(ctx, i0 + 40u, j1);
                i0 = l4;
                j1 = 0ull;
                StoreU64(ctx, i0 + 32u, j1);
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = l4;
                i1 = kF859ConstFFFFFFC0;
                i0 -= i1;
                l6 = i0;
                i0 = l4;
                i1 = 88u;
                i0 += i1;
                l2 = i0;
              }
              i0 = l7;
              i1 = 32u;
              i0 = i0 == i1;
              i1 = 1u;
              i2 = ResumePhase(ctx);
              i0 = i2 ? i0 : i1;
              if (i0) {
                i0 = 1291u;
                i1 = l2;
                i2 = l6;
                i3 = 4u;
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
                i0 = kF859ConstD015C;
                i0 = LoadU32(ctx, i0);
                l2 = i0;
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = l2;
                i1 = 1u;
                i0 = i0 == i1;
                l2 = i0;
                if (i0) {goto block_172;}
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = l4;
                i0 = LoadU32(ctx, i0 + 64u);
                l10 = i0;
                i1 = 24u;
                i0 += i1;
                i1 = kF859ConstFFFFFFFC;
                i0 &= i1;
                l2 = i0;
              }
              i0 = l7;
              i1 = 33u;
              i0 = i0 == i1;
              i1 = 1u;
              i2 = ResumePhase(ctx);
              i0 = i2 ? i0 : i1;
              if (i0) {
                i0 = 1032u;
                i1 = l2;
                i0 = HostFoldLayoutOffset(ctx, i0, i1);
                l8 = i0;
                i0 = 33u;
                i1 = ResumePhase(ctx);
                i2 = 1u;
                i1 = i1 == i2;
                if (i1) {goto spill_frame;}
                i0 = l8;
                l2 = i0;
              }
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = kF859ConstD015C;
                i0 = LoadU32(ctx, i0);
                l6 = i0;
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = l6;
                i1 = 1u;
                i0 = i0 == i1;
                l6 = i0;
                if (i0) {goto block_167;}
                i0 = l2;
                j1 = 4294967297ull;
                StoreU64(ctx, i0 + 4u, j1);
                i0 = l2;
                i1 = l10;
                i2 = 1u;
                i1 += i2;
                l6 = i1;
                StoreU32(ctx, i0 + 16u, i1);
                i0 = l2;
                i1 = kF859Const8E084;
                StoreU32(ctx, i0, i1);
                i0 = l2;
                i1 = 20u;
                i0 += i1;
                l10 = i0;
                i0 = l6;
                if (i0) {
                  i0 = l10;
                  i1 = 0u;
                  i2 = l6;
                  i0 = FillBytes(ctx, i0, i1, i2);
                }
                i0 = l3;
                i1 = l3;
                i1 = LoadU32(ctx, i1 + 4u);
                l6 = i1;
                i2 = 1u;
                i1 -= i2;
                l13 = i1;
                StoreU32(ctx, i0 + 4u, i1);
                i0 = l6;
                i1 = 1u;
                i0 = i0 != i1;
                l6 = i0;
              }
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = l6;
                if (i0) {goto block_224;}
                i0 = l3;
                i0 = LoadU32(ctx, i0);
                i0 = LoadU32(ctx, i0 + 8u);
                l6 = i0;
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
              }
              i0 = l7;
              i1 = 34u;
              i0 = i0 == i1;
              i1 = 1u;
              i2 = ResumePhase(ctx);
              i0 = i2 ? i0 : i1;
              if (i0) {
                i0 = l6;
                i1 = l3;
                HostReleaseSlotPair(ctx, i0, i1);
                i0 = 34u;
                i1 = ResumePhase(ctx);
                i2 = 1u;
                i1 = i1 == i2;
                if (i1) {goto spill_frame;}
              }
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = kF859ConstD015C;
                i0 = LoadU32(ctx, i0);
                l6 = i0;
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = l6;
                i1 = 1u;
                i0 = i0 == i1;
                if (i0) {goto running_exit;}
                i0 = l3;
                i1 = l3;
                i1 = LoadU32(ctx, i1 + 8u);
                l6 = i1;
                i2 = 1u;
                i1 -= i2;
                l13 = i1;
                StoreU32(ctx, i0 + 8u, i1);
                i0 = l6;
                i1 = 1u;
                i0 = i0 != i1;
                l6 = i0;
                if (i0) {goto block_224;}
                i0 = l3;
                i0 = LoadU32(ctx, i0);
                i0 = LoadU32(ctx, i0 + 12u);
                l6 = i0;
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
              }
              i0 = l7;
              i1 = 35u;
              i0 = i0 == i1;
              i1 = 1u;
              i2 = ResumePhase(ctx);
              i0 = i2 ? i0 : i1;
              if (i0) {
                i0 = l6;
                i1 = l3;
                HostReleaseSlotPair(ctx, i0, i1);
                i0 = 35u;
                i1 = ResumePhase(ctx);
                i2 = 1u;
                i1 = i1 == i2;
                if (i1) {goto spill_frame;}
              }
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = kF859ConstD015C;
                i0 = LoadU32(ctx, i0);
                l3 = i0;
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = l3;
                i1 = 1u;
                i0 = i0 == i1;
                l3 = i0;
                if (i0) {goto running_exit;}
              }
              block_224:;
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = l4;
                i0 = LoadU32(ctx, i0 + 64u);
                i1 = 1u;
                i0 += i1;
                l6 = i0;
                i0 = l4;
                i1 = 88u;
                i0 += i1;
                l3 = i0;
              }
              i0 = l7;
              i1 = 36u;
              i0 = i0 == i1;
              i1 = 1u;
              i2 = ResumePhase(ctx);
              i0 = i2 ? i0 : i1;
              if (i0) {
                i0 = 1291u;
                i1 = l3;
                i2 = l10;
                i3 = l6;
                HostSyncSlot(ctx, i0, i1, i2, i3);
                i0 = 36u;
                i1 = ResumePhase(ctx);
                i2 = 1u;
                i1 = i1 == i2;
                if (i1) {goto spill_frame;}
              }
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = kF859ConstD015C;
                i0 = LoadU32(ctx, i0);
                l3 = i0;
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = l3;
                i1 = 1u;
                i0 = i0 == i1;
                l3 = i0;
                if (i0) {goto block_170;}
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = l4;
                i1 = 32u;
                i0 += i1;
                l3 = i0;
              }
              i0 = l7;
              i1 = 37u;
              i0 = i0 == i1;
              i1 = 1u;
              i2 = ResumePhase(ctx);
              i0 = i2 ? i0 : i1;
              if (i0) {
                i0 = 996u;
                i1 = l3;
                i2 = l10;
                i0 = HostOsQuery(ctx, i0, i1, i2);
                l8 = i0;
                i0 = 37u;
                i1 = ResumePhase(ctx);
                i2 = 1u;
                i1 = i1 == i2;
                if (i1) {goto spill_frame;}
                i0 = l8;
                l3 = i0;
              }
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = kF859ConstD015C;
                i0 = LoadU32(ctx, i0);
                l3 = i0;
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = l3;
                i1 = 1u;
                i0 = i0 == i1;
                l3 = i0;
                if (i0) {goto block_171;}
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = l4;
                i1 = kF859ConstFFFFFFC0;
                i0 -= i1;
                l6 = i0;
                i0 = l4;
                i1 = 88u;
                i0 += i1;
                l3 = i0;
              }
              i0 = l7;
              i1 = 38u;
              i0 = i0 == i1;
              i1 = 1u;
              i2 = ResumePhase(ctx);
              i0 = i2 ? i0 : i1;
              if (i0) {
                i0 = 1291u;
                i1 = l3;
                i2 = l6;
                i3 = 4u;
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
                i0 = kF859ConstD015C;
                i0 = LoadU32(ctx, i0);
                l3 = i0;
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = l3;
                i1 = 1u;
                i0 = i0 == i1;
                l3 = i0;
                if (i0) {goto block_168;}
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = l4;
                i0 = LoadU32(ctx, i0 + 64u);
                l10 = i0;
                i1 = 24u;
                i0 += i1;
                i1 = kF859ConstFFFFFFFC;
                i0 &= i1;
                l3 = i0;
              }
              i0 = l7;
              i1 = 39u;
              i0 = i0 == i1;
              i1 = 1u;
              i2 = ResumePhase(ctx);
              i0 = i2 ? i0 : i1;
              if (i0) {
                i0 = 1032u;
                i1 = l3;
                i0 = HostFoldLayoutOffset(ctx, i0, i1);
                l8 = i0;
                i0 = 39u;
                i1 = ResumePhase(ctx);
                i2 = 1u;
                i1 = i1 == i2;
                if (i1) {goto spill_frame;}
                i0 = l8;
                l3 = i0;
              }
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = kF859ConstD015C;
                i0 = LoadU32(ctx, i0);
                l6 = i0;
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = l6;
                i1 = 1u;
                i0 = i0 == i1;
                l6 = i0;
                if (i0) {goto block_166;}
                i0 = l3;
                j1 = 4294967297ull;
                StoreU64(ctx, i0 + 4u, j1);
                i0 = l3;
                i1 = l10;
                i2 = 1u;
                i1 += i2;
                l6 = i1;
                StoreU32(ctx, i0 + 16u, i1);
                i0 = l3;
                i1 = kF859Const8E084;
                StoreU32(ctx, i0, i1);
                i0 = l3;
                i1 = 20u;
                i0 += i1;
                l10 = i0;
                i0 = l6;
                if (i0) {
                  i0 = l10;
                  i1 = 0u;
                  i2 = l6;
                  i0 = FillBytes(ctx, i0, i1, i2);
                }
                i0 = l2;
                i1 = l2;
                i1 = LoadU32(ctx, i1 + 4u);
                l6 = i1;
                i2 = 1u;
                i1 -= i2;
                l13 = i1;
                StoreU32(ctx, i0 + 4u, i1);
                i0 = l6;
                i1 = 1u;
                i0 = i0 != i1;
                l6 = i0;
              }
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = l6;
                if (i0) {goto block_240;}
                i0 = l2;
                i0 = LoadU32(ctx, i0);
                i0 = LoadU32(ctx, i0 + 8u);
                l6 = i0;
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
              }
              i0 = l7;
              i1 = 40u;
              i0 = i0 == i1;
              i1 = 1u;
              i2 = ResumePhase(ctx);
              i0 = i2 ? i0 : i1;
              if (i0) {
                i0 = l6;
                i1 = l2;
                HostReleaseSlotPair(ctx, i0, i1);
                i0 = 40u;
                i1 = ResumePhase(ctx);
                i2 = 1u;
                i1 = i1 == i2;
                if (i1) {goto spill_frame;}
              }
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = kF859ConstD015C;
                i0 = LoadU32(ctx, i0);
                l6 = i0;
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = l6;
                i1 = 1u;
                i0 = i0 == i1;
                if (i0) {goto running_exit;}
                i0 = l2;
                i1 = l2;
                i1 = LoadU32(ctx, i1 + 8u);
                l6 = i1;
                i2 = 1u;
                i1 -= i2;
                l13 = i1;
                StoreU32(ctx, i0 + 8u, i1);
                i0 = l6;
                i1 = 1u;
                i0 = i0 != i1;
                l6 = i0;
                if (i0) {goto block_240;}
                i0 = l2;
                i0 = LoadU32(ctx, i0);
                i0 = LoadU32(ctx, i0 + 12u);
                l6 = i0;
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
              }
              i0 = l7;
              i1 = 41u;
              i0 = i0 == i1;
              i1 = 1u;
              i2 = ResumePhase(ctx);
              i0 = i2 ? i0 : i1;
              if (i0) {
                i0 = l6;
                i1 = l2;
                HostReleaseSlotPair(ctx, i0, i1);
                i0 = 41u;
                i1 = ResumePhase(ctx);
                i2 = 1u;
                i1 = i1 == i2;
                if (i1) {goto spill_frame;}
              }
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = kF859ConstD015C;
                i0 = LoadU32(ctx, i0);
                l2 = i0;
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = l2;
                i1 = 1u;
                i0 = i0 == i1;
                l2 = i0;
                if (i0) {goto running_exit;}
              }
              block_240:;
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = l4;
                i0 = LoadU32(ctx, i0 + 64u);
                i1 = 1u;
                i0 += i1;
                l6 = i0;
                i0 = l4;
                i1 = 88u;
                i0 += i1;
                l2 = i0;
              }
              i0 = l7;
              i1 = 42u;
              i0 = i0 == i1;
              i1 = 1u;
              i2 = ResumePhase(ctx);
              i0 = i2 ? i0 : i1;
              if (i0) {
                i0 = 1291u;
                i1 = l2;
                i2 = l10;
                i3 = l6;
                HostSyncSlot(ctx, i0, i1, i2, i3);
                i0 = 42u;
                i1 = ResumePhase(ctx);
                i2 = 1u;
                i1 = i1 == i2;
                if (i1) {goto spill_frame;}
              }
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = kF859ConstD015C;
                i0 = LoadU32(ctx, i0);
                l2 = i0;
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = l2;
                i1 = 1u;
                i0 = i0 == i1;
                l2 = i0;
                if (i0) {goto block_169;}
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
              }
              i0 = l7;
              i1 = 43u;
              i0 = i0 == i1;
              i1 = 1u;
              i2 = ResumePhase(ctx);
              i0 = i2 ? i0 : i1;
              if (i0) {
                i0 = 996u;
                i1 = l16;
                i2 = l10;
                i0 = HostOsQuery(ctx, i0, i1, i2);
                l8 = i0;
                i0 = 43u;
                i1 = ResumePhase(ctx);
                i2 = 1u;
                i1 = i1 == i2;
                if (i1) {goto spill_frame;}
                i0 = l8;
                l2 = i0;
              }
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = kF859ConstD015C;
                i0 = LoadU32(ctx, i0);
                l2 = i0;
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = l2;
                i1 = 1u;
                i0 = i0 == i1;
                l2 = i0;
                if (i0) {goto block_173;}
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = l4;
                i1 = 32u;
                i0 += i1;
                l6 = i0;
                i0 = l4;
                i1 = 32u;
                i0 += i1;
                l10 = i0;
                i0 = l4;
                i1 = 168u;
                i0 += i1;
                l2 = i0;
              }
              i0 = l7;
              i1 = 44u;
              i0 = i0 == i1;
              i1 = 1u;
              i2 = ResumePhase(ctx);
              i0 = i2 ? i0 : i1;
              if (i0) {
                i0 = 2708u;
                i1 = l2;
                i2 = l12;
                i3 = l6;
                i4 = l10;
                HostFailAssertion(ctx, i0, i1, i2, i3, i4);
                i0 = 44u;
                i1 = ResumePhase(ctx);
                i2 = 1u;
                i1 = i1 == i2;
                if (i1) {goto spill_frame;}
              }
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = kF859ConstD015C;
                i0 = LoadU32(ctx, i0);
                l2 = i0;
                i0 = kF859ConstD015C;
                i1 = 0u;
                StoreU32(ctx, i0, i1);
                i0 = l2;
                i1 = 1u;
                i0 = i0 == i1;
                l2 = i0;
                if (i0) {goto block_165;}
                i0 = l4;
                i0 = LoadS8(ctx, i0 + 55u);
                i1 = kF859ConstFFFFFFFF;
                i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
                if (i0) {
                  i0 = l4;
                  i0 = LoadU32(ctx, i0 + 44u);
                  DeallocateRawBlock(ctx, i0);
                }
                i0 = l4;
                i0 = LoadS8(ctx, i0 + 43u);
                i1 = kF859ConstFFFFFFFF;
                i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
                if (i0) {
                  i0 = l4;
                  i0 = LoadU32(ctx, i0 + 32u);
                  DeallocateRawBlock(ctx, i0);
                }
                i0 = l9;
                i1 = 1u;
                i0 += i1;
                l9 = i0;
                i1 = l4;
                i1 = LoadU32(ctx, i1 + 60u);
                i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
                l2 = i0;
                if (i0) {goto loop_217;}
              }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 128u);
            l9 = i0;
            i0 = !(i0);
            l2 = i0;
          }
          i0 = l2;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF859ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
            }
            i0 = l7;
            i1 = 45u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 857u;
              i1 = kF859MsgPx;
              i2 = kF859MsgBoostBoostSmartPtr;
              i3 = 734u;
              i4 = kF859MsgOperator;
              HostFailAssertion(ctx, i0, i1, i2, i3, i4);
              i0 = 45u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF859ConstD015C;
              i0 = LoadU32(ctx, i0);
              a = i0;
              i0 = kF859ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = a;
              i1 = 1u;
              i0 = i0 != i1;
              a = i0;
              if (i0) {goto block_17;}
              goto running_exit;
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l9;
            i0 = LoadU32(ctx, i0 + 96u);
            l2 = i0;
            i1 = l9;
            i1 = LoadU32(ctx, i1 + 100u);
            i0 = i0 != i1;
            l6 = i0;
            if (i0) {
              i0 = l2;
              i1 = l5;
              StoreU32(ctx, i0, i1);
              i0 = l9;
              i1 = l2;
              i2 = 4u;
              i1 += i2;
              l2 = i1;
              StoreU32(ctx, i0 + 96u, i1);
              goto block_159;
            }
            i0 = l2;
            i1 = l9;
            i2 = 92u;
            i1 += i2;
            l12 = i1;
            i1 = LoadU32(ctx, i1);
            l10 = i1;
            i0 -= i1;
            l6 = i0;
            i1 = 2u;
            i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
            l13 = i0;
            i1 = 1u;
            i0 += i1;
            l2 = i0;
            i1 = kF859Const40000000;
            i0 = i0 >= i1;
            l15 = i0;
          }
          i0 = l15;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF859ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
            }
            i0 = l7;
            i1 = 46u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 1053u;
              i1 = l12;
              HostReleaseSlotPair(ctx, i0, i1);
              i0 = 46u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {goto block_263;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l6;
            i1 = 1u;
            i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
            l12 = i0;
            i1 = l2;
            i0 = i0 < i1;
            l15 = i0;
            i0 = l2;
            i1 = l12;
            i2 = l15;
            i0 = i2 ? i0 : i1;
            i1 = kF859Const3FFFFFFF;
            i2 = l13;
            i3 = kF859Const1FFFFFFF;
            i2 = i2 < i3;
            i0 = i2 ? i0 : i1;
            l12 = i0;
            i0 = !(i0);
            if (i0) {
              i0 = 0u;
              l2 = i0;
              goto block_261;
            }
            i0 = l12;
            i1 = kF859Const40000000;
            i0 = i0 < i1;
            l2 = i0;
            if (i0) {goto block_262;}
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l7;
          i1 = 47u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 1033u;
            i1 = kF859MsgAllocatorTAllocateSize;
            HostReleaseSlotPair(ctx, i0, i1);
            i0 = 47u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          block_263:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF859ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_17;}
            i0 = HostRet0(ctx);
            b = i0;
            i0 = HostTempRet0(ctx);
            a = i0;
            goto block_161;
          }
          block_262:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l12;
            i1 = 2u;
            i0 <<= (i1 & 31);
            l2 = i0;
          }
          i0 = l7;
          i1 = 48u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 1032u;
            i1 = l2;
            i0 = HostFoldLayoutOffset(ctx, i0, i1);
            l8 = i0;
            i0 = 48u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l2 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF859ConstD015C;
            i0 = LoadU32(ctx, i0);
            l15 = i0;
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l15;
            i1 = 1u;
            i0 = i0 == i1;
            l15 = i0;
            if (i0) {goto block_174;}
          }
          block_261:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l13;
            i1 = 2u;
            i0 <<= (i1 & 31);
            i1 = l2;
            i0 += i1;
            l13 = i0;
            i1 = l5;
            StoreU32(ctx, i0, i1);
            i0 = l6;
            i1 = 1u;
            i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
            if (i0) {
              i0 = l2;
              i1 = l10;
              i2 = l6;
              i0 = CopyBytes(ctx, i0, i1, i2);
            }
            i0 = l9;
            i1 = l12;
            i2 = 2u;
            i1 <<= (i2 & 31);
            i2 = l2;
            i1 += i2;
            StoreU32(ctx, i0 + 100u, i1);
            i0 = l9;
            i1 = l13;
            i2 = kF859ResumeWordBytes;
            i1 += i2;
            l5 = i1;
            StoreU32(ctx, i0 + 96u, i1);
            i0 = l9;
            i1 = l2;
            StoreU32(ctx, i0 + 92u, i1);
            i0 = l10;
            i0 = !(i0);
            l2 = i0;
            if (i0) {goto block_159;}
            i0 = l10;
            DeallocateRawBlock(ctx, i0);
            goto block_159;
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_43;
        }
        block_179:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_43;
        }
        block_178:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_161;
        }
        block_177:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          i0 = l5;
          DeallocateRawBlock(ctx, i0);
          goto block_161;
        }
        block_176:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 43u);
          i1 = kF859ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          a = i0;
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 32u);
            a = i0;
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l5;
          DeallocateRawBlock(ctx, i0);
          goto block_161;
        }
        block_175:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_161;
        }
        block_174:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_161;
        }
        block_173:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          goto block_164;
        }
        block_172:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          goto block_164;
        }
        block_171:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = l2;
          l3 = i0;
          i0 = HostTempRet0(ctx);
          goto block_164;
        }
        block_170:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = l2;
          l3 = i0;
          i0 = HostTempRet0(ctx);
          goto block_164;
        }
        block_169:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          goto block_164;
        }
        block_168:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = l2;
          l3 = i0;
          i0 = HostTempRet0(ctx);
          goto block_164;
        }
        block_167:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          goto block_164;
        }
        block_166:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = l2;
          l3 = i0;
          i0 = HostTempRet0(ctx);
          goto block_164;
        }
        block_165:;
        i0 = ResumePhase(ctx);
        if (i0) {
          i0 = a;
        } else {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
        }
        block_164:;
        a = i0;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 55u);
          i1 = kF859ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 44u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 43u);
          i1 = kF859ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_161;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 32u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
          goto block_161;
        }
        block_163:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 128u);
          l5 = i0;
          i0 = !(i0);
          l2 = i0;
        }
        i0 = l2;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l7;
          i1 = 49u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kF859MsgPx;
            i2 = kF859MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF859MsgOperator;
            HostFailAssertion(ctx, i0, i1, i2, i3, i4);
            i0 = 49u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF859ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_17;}
            goto running_exit;
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l5;
          i0 = LoadU32(ctx, i0 + 96u);
          l6 = i0;
          i0 = l5;
          i0 = LoadU32(ctx, i0 + 92u);
          l12 = i0;
          i0 = l10;
          i0 = StringLength(ctx, i0);
          l2 = i0;
          i1 = kF859ConstFFFFFFF0;
          i0 = i0 >= i1;
          l9 = i0;
        }
        i0 = l9;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = 32u;
            i0 += i1;
            a = i0;
          }
          i0 = l7;
          i1 = 50u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 1286u;
            i1 = a;
            HostReleaseSlotPair(ctx, i0, i1);
            i0 = 50u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF859ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_17;}
            i0 = HostRet0(ctx);
            b = i0;
            i0 = HostTempRet0(ctx);
            a = i0;
            goto block_161;
          }
        }
        i0 = l9;
        i1 = l2;
        i2 = 11u;
        i1 = i1 >= i2;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        l9 = i0;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = 16u;
            i0 += i1;
            l9 = i0;
            i1 = kF859ConstFFFFFFF0;
            i0 &= i1;
            l13 = i0;
          }
          i0 = l7;
          i1 = 51u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 1032u;
            i1 = l13;
            i0 = HostFoldLayoutOffset(ctx, i0, i1);
            l8 = i0;
            i0 = 51u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l9 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF859ConstD015C;
            i0 = LoadU32(ctx, i0);
            l15 = i0;
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l15;
            i1 = 1u;
            i0 = i0 == i1;
            l15 = i0;
            if (i0) {goto block_310;}
            i0 = l4;
            i1 = l9;
            StoreU32(ctx, i0 + 32u, i1);
            i0 = l4;
            i1 = l2;
            StoreU32(ctx, i0 + 36u, i1);
            i0 = l4;
            i1 = l13;
            i2 = kF859Const80000000;
            i1 |= i2;
            l13 = i1;
            StoreU32(ctx, i0 + 40u, i1);
            goto block_312;
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i1 = l2;
          StoreU8(ctx, i0 + 43u, i1);
          i0 = l4;
          i1 = 32u;
          i0 += i1;
          l9 = i0;
          i0 = l2;
          i0 = !(i0);
          l13 = i0;
          if (i0) {goto block_311;}
        }
        block_312:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l9;
          i1 = l10;
          i2 = l2;
          i0 = CopyBytes(ctx, i0, i1, i2);
          l10 = i0;
        }
        block_311:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          i1 = l9;
          i0 += i1;
          i1 = 0u;
          StoreU8(ctx, i0, i1);
          i0 = kF859ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l5;
          i1 = 104u;
          i0 += i1;
          l5 = i0;
          i0 = l4;
          i1 = 32u;
          i0 += i1;
          l9 = i0;
          i0 = l4;
          i1 = 168u;
          i0 += i1;
          l2 = i0;
        }
        i0 = l7;
        i1 = 52u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2341u;
          i1 = l2;
          i2 = l5;
          i3 = l9;
          HostSyncSlot(ctx, i0, i1, i2, i3);
          i0 = 52u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF859ConstD015C;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = kF859ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 == i1;
          l2 = i0;
          if (i0) {goto block_309;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 168u);
          i1 = l6;
          i2 = l12;
          i1 -= i2;
          i2 = 2u;
          i1 = (uint32_t)((int32_t)i1 >> (i2 & 31));
          l5 = i1;
          StoreU32(ctx, i0 + 20u, i1);
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 43u);
          i1 = kF859ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          l2 = i0;
          if (i0) {goto block_159;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 32u);
          l2 = i0;
          DeallocateRawBlock(ctx, i0);
          goto block_159;
        }
        block_310:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_161;
        }
        block_309:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 43u);
          i1 = kF859ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_161;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 32u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
          goto block_161;
        }
        block_162:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
        }
        block_161:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i1 = l3;
          i1 = LoadU32(ctx, i1 + 4u);
          a = i1;
          i2 = 1u;
          i1 -= i2;
          l2 = i1;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_43;}
          i0 = l3;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 8u);
          a = i0;
          i0 = kF859ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l7;
        i1 = 53u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = a;
          i1 = l3;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 53u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF859ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF859ConstD015C;
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
            i0 = l3;
            i1 = l3;
            i1 = LoadU32(ctx, i1 + 8u);
            a = i1;
            i2 = 1u;
            i1 -= i2;
            l2 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_43;}
            i0 = l3;
            i0 = LoadU32(ctx, i0);
            i0 = LoadU32(ctx, i0 + 12u);
            a = i0;
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l7;
          i1 = 54u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = a;
            i1 = l3;
            HostReleaseSlotPair(ctx, i0, i1);
            i0 = 54u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF859ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_43;}
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto running_exit;}
        block_160:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_43;
        }
        block_159:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i1 = l3;
          i1 = LoadU32(ctx, i1 + 4u);
          l2 = i1;
          i2 = 1u;
          i1 -= i2;
          l5 = i1;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 != i1;
          l2 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          if (i0) {goto block_335;}
          i0 = l3;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 8u);
          l2 = i0;
          i0 = kF859ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l7;
        i1 = 55u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l2;
          i1 = l3;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 55u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF859ConstD015C;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = kF859ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 != i1;
          l2 = i0;
        }
        i0 = l2;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            i1 = l3;
            i1 = LoadU32(ctx, i1 + 8u);
            l2 = i1;
            i2 = 1u;
            i1 -= i2;
            l5 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 != i1;
            l2 = i0;
            if (i0) {goto block_335;}
            i0 = l3;
            i0 = LoadU32(ctx, i0);
            i0 = LoadU32(ctx, i0 + 12u);
            l2 = i0;
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l7;
          i1 = 56u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l2;
            i1 = l3;
            HostReleaseSlotPair(ctx, i0, i1);
            i0 = 56u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF859ConstD015C;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            i0 = kF859ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 1u;
            i0 = i0 != i1;
            l3 = i0;
            if (i0) {goto block_335;}
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto running_exit;}
        block_335:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l14;
          i1 = 1u;
          i0 += i1;
          l14 = i0;
          i1 = l4;
          i1 = LoadU32(ctx, i1 + 72u);
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          l3 = i0;
          if (i0) {goto loop_156;}
        }
    }
    i0 = l3;
    i1 = l4;
    i2 = 88u;
    i1 += i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l3 = i0;
    i0 = l7;
    i1 = 57u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l3;
      i0 = DestroyDispatchObject(ctx, i0);
      l8 = i0;
      i0 = 57u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_71;}
    block_143:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto running_exit;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_37;
  }
  block_76:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_38;
  }
  block_75:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 99u);
    i1 = kF859ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    a = i0;
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 88u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
    }
    i0 = 1u;
    l3 = i0;
    goto block_70;
  }
  block_74:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_38;
  }
  block_73:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_40;
  }
  block_72:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 128u);
    l2 = i0;
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
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 58u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF859MsgPx;
      i2 = kF859MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF859MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 58u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_17;}
      goto running_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    l5 = i0;
    i0 = kF859ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 59u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l5;
    i1 = l2;
    i2 = l3;
    HostOsSchedule(ctx, i0, i1, i2);
    i0 = 59u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF859ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF859ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 != i1;
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
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 128u);
      l3 = i0;
      i0 = !(i0);
      l2 = i0;
    }
    i0 = l2;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 60u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF859MsgPx;
        i2 = kF859MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF859MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 60u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF859ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_17;}
        goto running_exit;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 20u);
      l2 = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 61u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = l3;
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
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 != i1;
      l3 = i0;
      if (i0) {goto block_71;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_42;
  }
  block_71:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 128u);
    l2 = i0;
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
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 62u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF859MsgPx;
      i2 = kF859MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF859MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 62u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_17;}
      goto running_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 92u);
    l3 = i0;
  }
  loop_381: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 96u);
      i0 = i0 == i1;
      l2 = i0;
    }
    i0 = l2;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i1 = b;
        StoreU32(ctx, i0 + 32u, i1);
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 32u;
        i0 += i1;
        l5 = i0;
        i0 = l4;
        i1 = 168u;
        i0 += i1;
        l6 = i0;
        i0 = l4;
        i1 = 88u;
        i0 += i1;
        l3 = i0;
        i0 = a;
        i1 = 2684u;
        i0 += i1;
        l2 = i0;
      }
      i0 = l7;
      i1 = 63u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 2709u;
        i1 = l3;
        i2 = l2;
        i3 = b;
        i4 = kF859ConstB5E30;
        i5 = l5;
        i6 = l6;
        HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
        i0 = 63u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF859ConstD015C;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 != i1;
        l3 = i0;
        if (i0) {goto block_380;}
        goto block_48;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = LoadU32(ctx, i0 + 8u);
      l6 = i0;
      i0 = l2;
      i0 = LoadU8(ctx, i0 + 15u);
      l5 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l9 = i0;
      i0 = l6;
      i1 = l5;
      i2 = l9;
      i0 = i2 ? i0 : i1;
      i1 = 3u;
      i0 = i0 != i1;
      l5 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      if (i0) {goto block_390;}
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 4u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l7;
    i1 = 64u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = l2;
      i2 = 0u;
      i3 = kF859ConstFFFFFFFF;
      i4 = kF859ConstAF4FF;
      i5 = 3u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l8 = i0;
      i0 = 64u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto running_exit;}
      i0 = l2;
      if (i0) {goto block_390;}
      i0 = a;
      i0 = LoadU32(ctx, i0 + 136u);
      l2 = i0;
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
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 65u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF859MsgPx;
        i2 = kF859MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF859MsgOperator;
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
        i0 = kF859ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_17;}
        goto running_exit;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = l4;
      i1 = 6u;
      StoreU8(ctx, i0 + 99u, i1);
      i0 = l4;
      i1 = kF859MsgScript;
      i1 = LoadU32(ctx, i1);
      l9 = i1;
      StoreU32(ctx, i0 + 88u, i1);
      i0 = l4;
      i1 = kF859ConstB4A7A;
      i1 = LoadU16(ctx, i1);
      l10 = i1;
      StoreU16(ctx, i0 + 92u, i1);
      i0 = l4;
      i1 = 0u;
      StoreU8(ctx, i0 + 94u, i1);
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l4;
      i2 = 88u;
      i1 += i2;
      StoreU32(ctx, i0 + 168u, i1);
      i0 = l4;
      i1 = 32u;
      i0 += i1;
      l6 = i0;
      i0 = l4;
      i1 = 88u;
      i0 += i1;
      l12 = i0;
      i0 = l4;
      i1 = 168u;
      i0 += i1;
      l14 = i0;
      i0 = l4;
      i1 = 72u;
      i0 += i1;
      l13 = i0;
      i0 = l5;
      i1 = 16u;
      i0 += i1;
      l5 = i0;
    }
    i0 = l7;
    i1 = 66u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1959u;
      i1 = l6;
      i2 = l5;
      i3 = l12;
      i4 = kF859ConstB5E30;
      i5 = l14;
      i6 = l13;
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
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF859ConstD015C;
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
      if (i0) {goto block_403;}
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 32u);
      i1 = 28u;
      i0 += i1;
      l5 = i0;
    }
    i0 = l7;
    i1 = 67u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2710u;
      i1 = l2;
      i2 = l5;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l8 = i0;
      i0 = 67u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_403;}
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 99u);
      i1 = kF859ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l5 = i0;
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 88u);
        l5 = i0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l2;
      i0 = !(i0);
      l2 = i0;
      if (i0) {goto block_402;}
      i0 = a;
      i0 = LoadU32(ctx, i0 + 136u);
      l5 = i0;
      i0 = !(i0);
      l2 = i0;
    }
    i0 = l2;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 68u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF859MsgPx;
        i2 = kF859MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF859MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 68u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF859ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_17;}
        goto running_exit;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0);
      i1 = b;
      i2 = b;
      i2 = LoadS8(ctx, i2 + 11u);
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      i0 = i2 ? i0 : i1;
      l14 = i0;
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 40u);
      l6 = i0;
      i0 = l5;
      i1 = 4u;
      i0 += i1;
      l2 = i0;
      i0 = l5;
      i0 = LoadS8(ctx, i0 + 15u);
      i1 = kF859ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l12 = i0;
      if (i0) {
        i0 = l5;
        i0 = LoadU32(ctx, i0 + 4u);
        l2 = i0;
      }
      i0 = l4;
      i1 = l14;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l6;
      StoreU32(ctx, i0 + 20u, i1);
      i0 = l4;
      i1 = l2;
      StoreU32(ctx, i0 + 24u, i1);
      i0 = l4;
      i1 = 16u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l7;
    i1 = 69u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2597u;
      i1 = a;
      i2 = 1u;
      i3 = kF859MsgSDLuaS;
      i4 = l2;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 69u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_402;}
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      a = i0;
      goto block_42;
    }
    block_403:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 99u);
      i1 = kF859ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_42;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 88u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_42;
    }
    block_402:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = l4;
      i1 = 6u;
      StoreU8(ctx, i0 + 99u, i1);
      i0 = l4;
      i1 = l9;
      StoreU32(ctx, i0 + 88u, i1);
      i0 = l4;
      i1 = l10;
      StoreU16(ctx, i0 + 92u, i1);
      i0 = l4;
      i1 = 0u;
      StoreU8(ctx, i0 + 94u, i1);
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l4;
      i2 = 88u;
      i1 += i2;
      StoreU32(ctx, i0 + 168u, i1);
      i0 = l4;
      i1 = 32u;
      i0 += i1;
      l5 = i0;
      i0 = l4;
      i1 = 88u;
      i0 += i1;
      l6 = i0;
      i0 = l4;
      i1 = 168u;
      i0 += i1;
      l9 = i0;
      i0 = l4;
      i1 = 72u;
      i0 += i1;
      l10 = i0;
      i0 = l2;
      i1 = 16u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l7;
    i1 = 70u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1959u;
      i1 = l5;
      i2 = l2;
      i3 = l6;
      i4 = kF859ConstB5E30;
      i5 = l9;
      i6 = l10;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 70u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_389;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 32u);
      l2 = i0;
      i0 = LoadS8(ctx, i0 + 39u);
      i1 = kF859ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 28u);
        i1 = 0u;
        StoreU8(ctx, i0, i1);
        i0 = l2;
        i1 = 0u;
        StoreU32(ctx, i0 + 32u, i1);
        goto block_420;
      }
      i0 = l2;
      i1 = 0u;
      StoreU8(ctx, i0 + 39u, i1);
      i0 = l2;
      i1 = 0u;
      StoreU8(ctx, i0 + 28u, i1);
      block_420:;
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 99u);
      i1 = kF859ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      l2 = i0;
      if (i0) {goto block_390;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 88u);
      l2 = i0;
      DeallocateRawBlock(ctx, i0);
    }
    block_390:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 4u;
      i0 += i1;
      l3 = i0;
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 128u);
      l2 = i0;
      i0 = !(i0);
      l5 = i0;
      if (i0) {goto block_47;}
      goto loop_381;
    }
    block_389:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 99u);
    i1 = kF859ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_42;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 88u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_42;
  }
  block_380:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 128u);
    l3 = i0;
    i0 = !(i0);
    l2 = i0;
  }
  i0 = l2;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 71u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF859MsgPx;
      i2 = kF859MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF859MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 71u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_17;}
      goto running_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 88u);
    l2 = i0;
    i0 = LoadU32(ctx, i0 + 28u);
    l9 = i0;
    i0 = l2;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 92u);
    StoreU32(ctx, i0 + 28u, i1);
    i0 = l3;
    i1 = l9;
    StoreU32(ctx, i0 + 92u, i1);
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 32u);
    l5 = i0;
    i0 = l2;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 96u);
    StoreU32(ctx, i0 + 32u, i1);
    i0 = l3;
    i1 = l5;
    StoreU32(ctx, i0 + 96u, i1);
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 36u);
    l5 = i0;
    i0 = l2;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 100u);
    StoreU32(ctx, i0 + 36u, i1);
    i0 = l3;
    i1 = l5;
    StoreU32(ctx, i0 + 100u, i1);
    i0 = kF859ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 88u;
    i0 += i1;
    l3 = i0;
    i0 = a;
    i1 = 2696u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l7;
  i1 = 72u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2711u;
    i1 = l3;
    i2 = l2;
    i3 = b;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 72u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF859ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF859ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    l3 = i0;
    if (i0) {goto block_48;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 128u);
    l2 = i0;
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
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 73u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF859MsgPx;
      i2 = kF859MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF859MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 73u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_17;}
      goto running_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 88u);
    l3 = i0;
    i0 = LoadU32(ctx, i0 + 40u);
    l9 = i0;
    i0 = l3;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 124u);
    StoreU32(ctx, i0 + 40u, i1);
    i0 = l2;
    i1 = l9;
    StoreU32(ctx, i0 + 124u, i1);
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 24u);
    l5 = i0;
    i0 = l3;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 108u);
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l2;
    i1 = l5;
    StoreU32(ctx, i0 + 108u, i1);
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 28u);
    l5 = i0;
    i0 = l3;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 112u);
    StoreU32(ctx, i0 + 28u, i1);
    i0 = l2;
    i1 = l5;
    StoreU32(ctx, i0 + 112u, i1);
    i0 = l3;
    f0 = LoadF32(ctx, i0 + 32u);
    l17 = f0;
    i0 = l3;
    i1 = l2;
    f1 = LoadF32(ctx, i1 + 116u);
    StoreF32(ctx, i0 + 32u, f1);
    i0 = l2;
    f1 = l17;
    StoreF32(ctx, i0 + 116u, f1);
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 36u);
    l5 = i0;
    i0 = l3;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 120u);
    StoreU32(ctx, i0 + 36u, i1);
    i0 = l2;
    i1 = l5;
    StoreU32(ctx, i0 + 120u, i1);
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 128u);
    l3 = i0;
  }
  loop_439: 
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l2;
    } else {
      i0 = l3;
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 132u);
      i0 = i0 == i1;
    }
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 132u);
        a = i0;
        i0 = !(i0);
        b = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        if (i0) {goto block_443;}
        i0 = a;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 4u);
        b = i1;
        i2 = 1u;
        i1 -= i2;
        l3 = i1;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 != i1;
        b = i0;
        if (i0) {goto block_443;}
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 8u);
        b = i0;
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 74u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = b;
        i1 = a;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 74u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF859ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto running_exit;}
        i0 = a;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 8u);
        b = i1;
        i2 = 1u;
        i1 -= i2;
        l3 = i1;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 != i1;
        b = i0;
        if (i0) {goto block_443;}
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        b = i0;
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 75u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = b;
        i1 = a;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 75u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF859ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 == i1;
        a = i0;
        if (i0) {goto running_exit;}
      }
      block_443:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 140u);
        a = i0;
        i0 = !(i0);
        b = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        if (i0) {goto block_450;}
        i0 = a;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 4u);
        b = i1;
        i2 = 1u;
        i1 -= i2;
        l3 = i1;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 != i1;
        b = i0;
        if (i0) {goto block_450;}
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 8u);
        b = i0;
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 76u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = b;
        i1 = a;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 76u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF859ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto running_exit;}
        i0 = a;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 8u);
        b = i1;
        i2 = 1u;
        i1 -= i2;
        l3 = i1;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 != i1;
        b = i0;
        if (i0) {goto block_450;}
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        b = i0;
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 77u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = b;
        i1 = a;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 77u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF859ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF859ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 == i1;
        a = i0;
        if (i0) {goto running_exit;}
      }
      block_450:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = 0u;
        l3 = i0;
        goto block_70;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0);
      i1 = b;
      i2 = b;
      i2 = LoadS8(ctx, i2 + 11u);
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      i0 = i2 ? i0 : i1;
      l2 = i0;
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 11u);
      i1 = kF859ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0);
        goto block_458;
      }
      i0 = l3;
      block_458:;
      l5 = i0;
      i0 = l4;
      i1 = l2;
      StoreU32(ctx, i0, i1);
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l5;
      StoreU32(ctx, i0 + 4u, i1);
    }
    i0 = l7;
    i1 = 78u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2597u;
      i1 = a;
      i2 = 1u;
      i3 = kF859MsgSS;
      i4 = l4;
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
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      if (i0) {
        i0 = l3;
        i1 = 12u;
        i0 += i1;
        l3 = i0;
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 128u);
        l2 = i0;
        i0 = !(i0);
        if (i0) {goto block_49;}
        goto loop_439;
      }
    }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_42;
  }
  block_70:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l11;
    i0 = !(i0);
    a = i0;
    if (i0) {goto block_50;}
    i0 = l11;
    i1 = l11;
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
    if (i0) {goto block_50;}
    i0 = l11;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    a = i0;
    i0 = kF859ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 79u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = l11;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 79u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF859ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF859ConstD015C;
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
      i0 = l11;
      i1 = l11;
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
      if (i0) {goto block_50;}
      i0 = l11;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      a = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 80u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = l11;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 80u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_50;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto running_exit;}
  block_50:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 163u);
    i1 = kF859ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_5;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 152u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_5;
  }
  block_49:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF859ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 81u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 857u;
    i1 = kF859MsgPx;
    i2 = kF859MsgBoostBoostSmartPtr;
    i3 = 734u;
    i4 = kF859MsgOperator;
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
    i0 = kF859ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF859ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 != i1;
    a = i0;
    if (i0) {goto block_17;}
    goto running_exit;
  }
  block_48:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_42;
  }
  block_47:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF859ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 82u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 857u;
    i1 = kF859MsgPx;
    i2 = kF859MsgBoostBoostSmartPtr;
    i3 = 734u;
    i4 = kF859MsgOperator;
    HostFailAssertion(ctx, i0, i1, i2, i3, i4);
    i0 = 82u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF859ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF859ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 != i1;
    a = i0;
    if (i0) {goto block_17;}
    goto running_exit;
  }
  block_46:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_43;
  }
  block_45:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_43;
  }
  block_44:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 84u);
    a = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_43;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 4u);
    l3 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 != i1;
    l3 = i0;
    if (i0) {goto block_43;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i0 = kF859ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 83u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l3;
    i1 = a;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 83u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF859ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF859ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 != i1;
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
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 8u);
      l3 = i1;
      i2 = 1u;
      i1 -= i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 != i1;
      l3 = i0;
      if (i0) {goto block_43;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      l3 = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 84u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l3;
      i1 = a;
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
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_43;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto running_exit;}
  block_43:;
  i0 = a;
  i1 = l4;
  i2 = 88u;
  i1 += i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  a = i0;
  i0 = l7;
  i1 = 85u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i0 = DestroyDispatchObject(ctx, i0);
    l8 = i0;
    i0 = 85u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    a = i0;
  }
  block_42:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 132u);
    a = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_40;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 4u);
    l3 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 != i1;
    l3 = i0;
    if (i0) {goto block_40;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i0 = kF859ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 86u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l3;
    i1 = a;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 86u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF859ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF859ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 != i1;
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
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 8u);
      l3 = i1;
      i2 = 1u;
      i1 -= i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 != i1;
      l3 = i0;
      if (i0) {goto block_40;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      l3 = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 87u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l3;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 87u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_40;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto running_exit;}
  block_41:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
  }
  block_40:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 140u);
    a = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_38;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 4u);
    l3 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 != i1;
    l3 = i0;
    if (i0) {goto block_38;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i0 = kF859ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 88u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l3;
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
    i0 = kF859ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF859ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 != i1;
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
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 8u);
      l3 = i1;
      i2 = 1u;
      i1 -= i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 != i1;
      l3 = i0;
      if (i0) {goto block_38;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      l3 = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 89u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l3;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 89u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_38;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto running_exit;}
  block_39:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
  }
  block_38:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l11;
    i0 = !(i0);
    a = i0;
    if (i0) {goto block_37;}
    i0 = l11;
    i1 = l11;
    i1 = LoadU32(ctx, i1 + 4u);
    a = i1;
    i2 = 1u;
    i1 -= i2;
    l3 = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 != i1;
    a = i0;
    if (i0) {goto block_37;}
    i0 = l11;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    a = i0;
    i0 = kF859ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 90u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = l11;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 90u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF859ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF859ConstD015C;
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
      i0 = l11;
      i1 = l11;
      i1 = LoadU32(ctx, i1 + 8u);
      a = i1;
      i2 = 1u;
      i1 -= i2;
      l3 = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_37;}
      i0 = l11;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      a = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 91u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = l11;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 91u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF859ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF859ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_37;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto running_exit;}
  block_37:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 163u);
    i1 = kF859ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 152u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = b;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_17:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    TrapUnreachable();
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 176u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    i0 = l3;
    goto func_exit;
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
  i0 = l7;
  i1 = 92u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 92u;
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
  l8 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l8;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF859ResumeWordBytes;
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
  i1 = l2;
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
  i0 = l8;
  i1 = l12;
  StoreU32(ctx, i0 + 40u, i1);
  i0 = l8;
  i1 = l13;
  StoreU32(ctx, i0 + 44u, i1);
  i0 = l8;
  i1 = l14;
  StoreU32(ctx, i0 + 48u, i1);
  i0 = l8;
  i1 = l15;
  StoreU32(ctx, i0 + 52u, i1);
  i0 = l8;
  i1 = l16;
  StoreU32(ctx, i0 + 56u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 60u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
}
