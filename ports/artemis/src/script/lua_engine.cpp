// lua_engine.cpp -- the script call entry: one suspendable step that invokes a function value and answers its result count..
//
// LuaEngine::Call -- the script call entry: one suspendable step that invokes a function value and answers its result count.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 356363-356363 (f904); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t CompactEntryQueue(EngineContext* ctx, uint32_t state, uint32_t queue);
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size);
void ThrowScriptError(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t InternString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t RaiseBlockTooBig(EngineContext* ctx, uint32_t a);
void RaiseScriptError(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t FetchObjectMetamethod(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
void EnsureTableArrayRoom(EngineContext* ctx, uint32_t a);
void RelocateElementArray(EngineContext* ctx, uint32_t state, uint32_t table);
uint32_t AllocateTable(EngineContext* ctx, uint32_t state, uint32_t entries, uint32_t array);
uint32_t GetTableFieldByString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t GetTableFieldByNumber(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void RaiseTypeError(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

namespace {

constexpr uint32_t kF904MsgCall = 0x2A8ABu;  // call
constexpr uint32_t kF904Const2A8B0 = 0x2A8B0u;  // 
constexpr uint32_t kF904MsgStackOverflow = 0x2A8B2u;  // stack overflow
constexpr uint32_t kF904ConstAAAAAAA = 0xAAAAAAAu;  // 
constexpr uint32_t kF904ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF904SaveFrameBytes = 60u;
constexpr uint32_t kF904ResumeWordBytes = 4u;

}  // namespace

namespace LuaEngine {
uint32_t Call(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l13 = 0u; uint32_t l14 = 0u; uint32_t l15 = 0u; uint32_t l16 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  uint64_t j1 = 0;
  double d1 = 0;
      l11 = 0, l12 = 0, l13 = 0, l14 = 0, l15 = 0, l16 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF904SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 24u);
    l7 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 28u);
    l9 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 32u);
    l10 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 36u);
    l11 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 40u);
    l12 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 44u);
    l13 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 48u);
    l14 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 52u);
    l15 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 56u);
    l16 = i0;
    i0 = l5;
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
    i2 = kF904ResumeWordBytes;
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
    i0 = GuestStackSp(ctx);
    i1 = 112u;
    i0 -= i1;
    l11 = i0;
    GuestStackSp(ctx) = i0;
    i0 = b;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = 6u;
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
      i0 = a;
      i1 = b;
      i2 = 16u;
      i0 = FetchObjectMetamethod(ctx, i0, i1, i2);
      l9 = i0;
      i0 = a;
      i0 = LoadU32(ctx, i0 + 32u);
      l7 = i0;
      i0 = l9;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = 6u;
      i0 = i0 != i1;
      l4 = i0;
    }
    i0 = l4;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l8;
      i0 = !(i0);
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = b;
        i2 = kF904MsgCall;
        RaiseTypeError(ctx, i0, i1, i2);
        i0 = 0u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 8u);
      l3 = i0;
      i1 = b;
      i0 = i0 > i1;
      if (i0) {
        loop_12: 
          i0 = l3;
          i1 = l3;
          i2 = 16u;
          i1 -= i2;
          l4 = i1;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0, j1);
          i0 = l3;
          i1 = l4;
          i1 = LoadU32(ctx, i1 + 8u);
          l6 = i1;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l4;
          l3 = i0;
          i1 = b;
          i0 = i0 > i1;
          if (i0) {goto loop_12;}
        i0 = a;
        i0 = LoadU32(ctx, i0 + 8u);
        l3 = i0;
      }
      i0 = a;
      i0 = LoadU32(ctx, i0 + 28u);
      i1 = l3;
      i0 -= i1;
      i1 = 16u;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
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
        i0 = a;
        i0 = LoadU32(ctx, i0 + 44u);
        l4 = i0;
        i1 = 1u;
        i0 += i1;
        l6 = i0;
        i1 = l4;
        i2 = 1u;
        i1 <<= (i2 & 31);
        l3 = i1;
        i2 = l4;
        i3 = 1u;
        i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
        i0 = i2 ? i0 : i1;
        l4 = i0;
      }
      i0 = l8;
      i1 = 1u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l4;
        RelocateElementArray(ctx, i0, i1);
        i0 = 1u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 8u);
      i2 = 16u;
      i1 += i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = a;
      i0 = LoadU32(ctx, i0 + 32u);
      i1 = b;
      i2 = l7;
      i1 -= i2;
      i0 += i1;
      b = i0;
      i1 = l9;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = b;
      i1 = l9;
      i1 = LoadU32(ctx, i1 + 8u);
      l4 = i1;
      StoreU32(ctx, i0 + 8u, i1);
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 32u);
    l4 = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 20u);
    l14 = i0;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 24u);
    l6 = i1;
    StoreU32(ctx, i0 + 12u, i1);
    i0 = b;
    i1 = l4;
    i0 -= i1;
    l16 = i0;
    i0 = l3;
    i0 = LoadU8(ctx, i0 + 6u);
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
      i0 = a;
      i0 = LoadU32(ctx, i0 + 28u);
      i1 = a;
      i1 = LoadU32(ctx, i1 + 8u);
      i0 -= i1;
      l6 = i0;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 16u);
      l12 = i1;
      i1 = LoadU8(ctx, i1 + 75u);
      l4 = i1;
      i2 = 4u;
      i1 <<= (i2 & 31);
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
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
        i0 = l4;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 44u);
        b = i1;
        i0 += i1;
        l6 = i0;
        i1 = b;
        i2 = 1u;
        i1 <<= (i2 & 31);
        l3 = i1;
        i2 = b;
        i3 = l4;
        i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
        i0 = i2 ? i0 : i1;
        b = i0;
      }
      i0 = l8;
      i1 = 2u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = b;
        RelocateElementArray(ctx, i0, i1);
        i0 = 2u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l16;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 32u);
      i0 += i1;
      l4 = i0;
      i0 = l12;
      i0 = LoadU8(ctx, i0 + 74u);
      l7 = i0;
      i0 = !(i0);
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      if (i0) {
        i0 = l4;
        i1 = 16u;
        i0 += i1;
        l3 = i0;
        i1 = l12;
        i1 = LoadU8(ctx, i1 + 73u);
        i2 = 4u;
        i1 <<= (i2 & 31);
        i0 += i1;
        b = i0;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 8u);
        i0 = i0 >= i1;
        l6 = i0;
        if (i0) {goto block_29;}
        i0 = a;
        i1 = b;
        StoreU32(ctx, i0 + 8u, i1);
        goto block_29;
      }
      i0 = a;
      i0 = LoadU32(ctx, i0 + 8u);
      l3 = i0;
      i1 = l4;
      i0 -= i1;
      i1 = 4u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      b = i0;
      i1 = 1u;
      i0 -= i1;
      l4 = i0;
      i0 = b;
      l6 = i0;
      i0 = l6;
      i1 = l12;
      i1 = LoadU8(ctx, i1 + 73u);
      b = i1;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        loop_33: 
          i0 = l3;
          i1 = 0u;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l3;
          i1 = 16u;
          i0 += i1;
          l3 = i0;
          i0 = l4;
          i1 = 1u;
          i0 += i1;
          l4 = i0;
          i1 = b;
          i0 = i0 != i1;
          if (i0) {goto loop_33;}
        i0 = a;
        i1 = l3;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = b;
        l4 = i0;
      }
      i0 = l7;
      i1 = 4u;
      i0 &= i1;
      i0 = !(i0);
      l6 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      if (i0) {goto block_34;}
      i0 = a;
      i0 = LoadU32(ctx, i0 + 16u);
      l7 = i0;
      i0 = LoadU32(ctx, i0 + 68u);
      l14 = i0;
      i0 = l14;
      i1 = l7;
      i1 = LoadU32(ctx, i1 + 64u);
      i0 = i0 >= i1;
      l6 = i0;
    }
    i0 = l6;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l8;
      i1 = 3u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        EnsureTableArrayRoom(ctx, i0);
        i0 = 3u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l3;
      } else {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 8u);
      }
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 28u);
      i1 = l3;
      i0 -= i1;
      l6 = i0;
      i1 = l12;
      i1 = LoadU8(ctx, i1 + 75u);
      l7 = i1;
      i2 = 4u;
      i1 <<= (i2 & 31);
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
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
        i0 = l7;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 44u);
        l3 = i1;
        i0 += i1;
        l14 = i0;
        i1 = l3;
        i2 = 1u;
        i1 <<= (i2 & 31);
        l6 = i1;
        i2 = l3;
        i3 = l7;
        i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
        i0 = i2 ? i0 : i1;
        l3 = i0;
      }
      i0 = l8;
      i1 = 4u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l3;
        RelocateElementArray(ctx, i0, i1);
        i0 = 4u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    i0 = l13;
    i1 = l4;
    i2 = b;
    i1 -= i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l13 = i0;
    i0 = l8;
    i1 = 5u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = l13;
      i2 = 1u;
      i0 = AllocateTable(ctx, i0, i1, i2);
      l5 = i0;
      i0 = 5u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l15 = i0;
    }
    i0 = l3;
    i1 = l13;
    i2 = 1u;
    i1 = (uint32_t)((int32_t)i1 >= (int32_t)i2);
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l3 = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = 0u;
        i1 = l13;
        i0 -= i1;
        l6 = i0;
        i1 = 4u;
        i0 <<= (i1 & 31);
        l14 = i0;
        i0 = 0u;
        l3 = i0;
      }
      loop_48: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i1 = 4u;
          i0 <<= (i1 & 31);
          l10 = i0;
          i0 = a;
          i0 = LoadU32(ctx, i0 + 8u);
          l7 = i0;
          i0 = l3;
          i1 = 1u;
          i0 += i1;
          l3 = i0;
        }
        i0 = l8;
        i1 = 6u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = a;
          i1 = l15;
          i2 = l3;
          i0 = GetTableFieldByNumber(ctx, i0, i1, i2);
          l5 = i0;
          i0 = 6u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l5;
          l9 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l9;
          i1 = l10;
          i2 = l7;
          i3 = l14;
          i2 += i3;
          i1 += i2;
          l7 = i1;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0, j1);
          i0 = l9;
          i1 = l7;
          i1 = LoadU32(ctx, i1 + 8u);
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l3;
          i1 = l13;
          i0 = i0 != i1;
          l6 = i0;
          if (i0) {goto loop_48;}
        }
    }
    i0 = l8;
    i1 = 7u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = kF904Const2A8B0;
      i2 = 1u;
      i0 = InternString(ctx, i0, i1, i2);
      l5 = i0;
      i0 = 7u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l3 = i0;
    }
    i0 = l8;
    i1 = 8u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = l15;
      i2 = l3;
      i0 = GetTableFieldByString(ctx, i0, i1, i2);
      l5 = i0;
      i0 = 8u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 3u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = l13;
      d1 = (double)(int32_t)(i1);
      StoreF64(ctx, i0, d1);
      i0 = a;
      i0 = LoadU32(ctx, i0 + 8u);
      l3 = i0;
    }
    block_34:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i0 = !(i0);
      l6 = i0;
      if (i0) {goto block_56;}
      i0 = a;
      i1 = l3;
      i2 = 16u;
      i1 += i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = l3;
      i2 = l4;
      i3 = 4u;
      i2 <<= (i3 & 31);
      i1 -= i2;
      l7 = i1;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = l3;
      i1 = l3;
      i2 = 0u;
      i3 = l4;
      i2 -= i3;
      i3 = 4u;
      i2 <<= (i3 & 31);
      i1 += i2;
      l4 = i1;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l4;
      i1 = 0u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = 1u;
      l4 = i0;
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      l6 = i0;
      if (i0) {goto block_56;}
      loop_57: 
        i0 = a;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 8u);
        l10 = i1;
        i2 = 16u;
        i1 += i2;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l10;
        i1 = l7;
        i2 = l4;
        i3 = kF904ResumeWordBytes;
        i2 <<= (i3 & 31);
        i1 += i2;
        l9 = i1;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0, j1);
        i0 = l10;
        i1 = l9;
        i1 = LoadU32(ctx, i1 + 8u);
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l9;
        i1 = 0u;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l4;
        i1 = 1u;
        i0 += i1;
        l4 = i0;
        i1 = b;
        i0 = i0 != i1;
        l6 = i0;
        if (i0) {goto loop_57;}
      block_56:;
      i0 = l15;
      if (i0) {
        i0 = a;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 8u);
        b = i1;
        i2 = 16u;
        i1 += i2;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = b;
        i1 = 5u;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = b;
        i1 = l15;
        StoreU32(ctx, i0, i1);
      }
      i0 = l16;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 32u);
      b = i1;
      i0 += i1;
      l4 = i0;
    }
    block_29:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 20u);
      b = i0;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 36u);
      i0 = i0 != i1;
      l6 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      if (i0) {goto block_60;}
      i0 = a;
      i0 = LoadU32(ctx, i0 + 48u);
      l10 = i0;
      i1 = 20001u;
      i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
      if (i0) {goto block_18;}
      i0 = a;
      i0 = LoadU32(ctx, i0 + 40u);
      l9 = i0;
      i0 = l10;
      i1 = 1u;
      i0 <<= (i1 & 31);
      l7 = i0;
      i1 = 1u;
      i0 |= i1;
      i1 = kF904ConstAAAAAAA;
      i0 = i0 <= i1;
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
        i0 = l10;
        i1 = 24u;
        i0 *= i1;
        l6 = i0;
        i0 = l10;
        i1 = 48u;
        i0 *= i1;
        b = i0;
      }
      i0 = l8;
      i1 = 9u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l9;
        i2 = l6;
        i3 = b;
        i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
        l5 = i0;
        i0 = 9u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l5;
        b = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_61;}
    }
    i0 = l8;
    i1 = 10u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i0 = RaiseBlockTooBig(ctx, i0);
      l5 = i0;
      i0 = 10u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      b = i0;
    }
    block_61:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = l7;
      StoreU32(ctx, i0 + 48u, i1);
      i0 = a;
      i1 = b;
      StoreU32(ctx, i0 + 40u, i1);
      i0 = a;
      i1 = l7;
      i2 = 24u;
      i1 *= i2;
      i2 = b;
      i1 += i2;
      i2 = 24u;
      i1 -= i2;
      StoreU32(ctx, i0 + 36u, i1);
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 20u);
      i2 = l9;
      i1 -= i2;
      i2 = 24u;
      i1 = I32_DIV_S(i1, i2);
      i2 = 24u;
      i1 *= i2;
      i2 = b;
      i1 += i2;
      b = i1;
      StoreU32(ctx, i0 + 20u, i1);
      i0 = l10;
      i1 = 10001u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l6 = i0;
      if (i0) {goto block_60;}
    }
    i0 = l8;
    i1 = 11u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = kF904MsgStackOverflow;
      i2 = 0u;
      RaiseScriptError(ctx, i0, i1, i2);
      i0 = 11u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 20u);
      b = i0;
    }
    block_60:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = b;
      i2 = 24u;
      i1 += i2;
      l7 = i1;
      StoreU32(ctx, i0 + 20u, i1);
      i0 = b;
      i1 = l3;
      StoreU32(ctx, i0 + 24u, i1);
      i0 = b;
      i1 = l4;
      StoreU32(ctx, i0 + 28u, i1);
      i0 = a;
      i1 = l3;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = b;
      i1 = l3;
      i2 = l12;
      i2 = LoadU8(ctx, i2 + 75u);
      i3 = 4u;
      i2 <<= (i3 & 31);
      i1 += i2;
      l10 = i1;
      StoreU32(ctx, i0 + 32u, i1);
      i0 = a;
      i1 = l12;
      i1 = LoadU32(ctx, i1 + 12u);
      l4 = i1;
      StoreU32(ctx, i0 + 24u, i1);
      i0 = b;
      i1 = c;
      StoreU32(ctx, i0 + 40u, i1);
      i0 = 0u;
      l9 = i0;
      i0 = b;
      i1 = 0u;
      StoreU32(ctx, i0 + 44u, i1);
      i0 = l10;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 8u);
      l3 = i1;
      i0 = i0 > i1;
      if (i0) {
        loop_73: 
          i0 = l3;
          i1 = 0u;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l10;
          i1 = l3;
          i2 = 16u;
          i1 += i2;
          l3 = i1;
          i0 = i0 > i1;
          if (i0) {goto loop_73;}
      }
      i0 = a;
      i1 = l10;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = a;
      i0 = LoadU8(ctx, i0 + 56u);
      i1 = 1u;
      i0 &= i1;
      i0 = !(i0);
      if (i0) {goto block_19;}
      i0 = a;
      i1 = l4;
      i2 = 4u;
      i1 += i2;
      l3 = i1;
      StoreU32(ctx, i0 + 24u, i1);
      i0 = a;
      i0 = LoadU32(ctx, i0 + 68u);
      c = i0;
      i0 = !(i0);
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      if (i0) {goto block_74;}
      i0 = a;
      i0 = LoadU8(ctx, i0 + 57u);
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_74;}
      i0 = a;
      i0 = LoadU32(ctx, i0 + 32u);
      b = i0;
      i0 = l11;
      i1 = kF904ConstFFFFFFFF;
      StoreU32(ctx, i0 + 28u, i1);
      i0 = l11;
      i1 = 0u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l11;
      i1 = l7;
      i2 = a;
      i2 = LoadU32(ctx, i2 + 40u);
      i1 -= i2;
      i2 = 24u;
      i1 = I32_DIV_S(i1, i2);
      StoreU32(ctx, i0 + 104u, i1);
      i0 = a;
      i0 = LoadU32(ctx, i0 + 28u);
      i1 = l10;
      i0 -= i1;
      i1 = 320u;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
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
        i0 = a;
        i0 = LoadU32(ctx, i0 + 44u);
        l4 = i0;
        i1 = 20u;
        i0 += i1;
        l6 = i0;
        i1 = l4;
        i2 = 1u;
        i1 <<= (i2 & 31);
        l3 = i1;
        i2 = l4;
        i3 = 20u;
        i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
        i0 = i2 ? i0 : i1;
        l4 = i0;
      }
      i0 = l8;
      i1 = 12u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l4;
        RelocateElementArray(ctx, i0, i1);
        i0 = 12u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 20u);
      l3 = i0;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 8u);
      i2 = 320u;
      i1 += i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = a;
      i1 = 0u;
      StoreU8(ctx, i0 + 57u, i1);
      i0 = l11;
      i1 = 8u;
      i0 += i1;
      l4 = i0;
    }
    i0 = l8;
    i1 = 13u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = l4;
      i2 = c;
      CallTableT0(ctx, i2, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = 13u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = 1u;
      StoreU8(ctx, i0 + 57u, i1);
      i0 = a;
      i0 = LoadU32(ctx, i0 + 20u);
      l4 = i0;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 32u);
      c = i1;
      i2 = l10;
      i3 = b;
      i2 -= i3;
      i1 += i2;
      b = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = a;
      i1 = b;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = a;
      i0 = LoadU32(ctx, i0 + 24u);
      l3 = i0;
    }
    block_74:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = l3;
      i2 = 4u;
      i1 -= i2;
      StoreU32(ctx, i0 + 24u, i1);
      goto block_19;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 28u);
    l4 = i0;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 8u);
    i0 -= i1;
    i1 = 320u;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
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
      i0 = LoadU32(ctx, i0 + 44u);
      b = i0;
      i1 = 20u;
      i0 += i1;
      l3 = i0;
      i1 = b;
      i2 = 1u;
      i1 <<= (i2 & 31);
      l4 = i1;
      i2 = b;
      i3 = 20u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      i0 = i2 ? i0 : i1;
      b = i0;
    }
    i0 = l8;
    i1 = 14u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = b;
      RelocateElementArray(ctx, i0, i1);
      i0 = 14u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 20u);
    l3 = i0;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 36u);
    i0 = i0 != i1;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    if (i0) {goto block_90;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 48u);
    l7 = i0;
    i1 = 20001u;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    if (i0) {goto block_17;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 40u);
    l4 = i0;
    i0 = l7;
    i1 = 1u;
    i0 <<= (i1 & 31);
    b = i0;
    i1 = 1u;
    i0 |= i1;
    i1 = kF904ConstAAAAAAA;
    i0 = i0 <= i1;
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
      i0 = l7;
      i1 = 24u;
      i0 *= i1;
      l6 = i0;
      i0 = l7;
      i1 = 48u;
      i0 *= i1;
      l3 = i0;
    }
    i0 = l8;
    i1 = 15u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = l4;
      i2 = l6;
      i3 = l3;
      i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
      l5 = i0;
      i0 = 15u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_91;}
  }
  i0 = l8;
  i1 = 16u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i0 = RaiseBlockTooBig(ctx, i0);
    l5 = i0;
    i0 = 16u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l3 = i0;
  }
  block_91:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = b;
    StoreU32(ctx, i0 + 48u, i1);
    i0 = a;
    i1 = l3;
    StoreU32(ctx, i0 + 40u, i1);
    i0 = a;
    i1 = l3;
    i2 = b;
    i3 = 24u;
    i2 *= i3;
    i1 += i2;
    i2 = 24u;
    i1 -= i2;
    StoreU32(ctx, i0 + 36u, i1);
    i0 = a;
    i1 = l3;
    i2 = a;
    i2 = LoadU32(ctx, i2 + 20u);
    i3 = l4;
    i2 -= i3;
    i3 = 24u;
    i2 = I32_DIV_S(i2, i3);
    i3 = 24u;
    i2 *= i3;
    i1 += i2;
    l3 = i1;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = l7;
    i1 = 10001u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    b = i0;
    if (i0) {goto block_90;}
  }
  i0 = l8;
  i1 = 17u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF904MsgStackOverflow;
    i2 = 0u;
    RaiseScriptError(ctx, i0, i1, i2);
    i0 = 17u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 20u);
    l3 = i0;
  }
  block_90:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = l3;
    i2 = 24u;
    i1 += i2;
    l4 = i1;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = l3;
    i1 = l16;
    i2 = a;
    i2 = LoadU32(ctx, i2 + 32u);
    l7 = i2;
    i1 += i2;
    b = i1;
    StoreU32(ctx, i0 + 28u, i1);
    i0 = l3;
    i1 = b;
    i2 = 16u;
    i1 += i2;
    b = i1;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = a;
    i1 = b;
    StoreU32(ctx, i0 + 12u, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 8u);
    l9 = i0;
    i0 = l3;
    i1 = c;
    StoreU32(ctx, i0 + 40u, i1);
    i0 = l3;
    i1 = l9;
    i2 = 320u;
    i1 += i2;
    c = i1;
    StoreU32(ctx, i0 + 32u, i1);
    i0 = a;
    i0 = LoadU8(ctx, i0 + 56u);
    i1 = 1u;
    i0 &= i1;
    i0 = !(i0);
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    if (i0) {goto block_102;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 68u);
    b = i0;
    i0 = !(i0);
    if (i0) {goto block_102;}
    i0 = a;
    i0 = LoadU8(ctx, i0 + 57u);
    i0 = !(i0);
    if (i0) {goto block_102;}
    i0 = l11;
    i1 = kF904ConstFFFFFFFF;
    StoreU32(ctx, i0 + 28u, i1);
    i0 = l11;
    i1 = 0u;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l11;
    i1 = l4;
    i2 = a;
    i2 = LoadU32(ctx, i2 + 40u);
    i1 -= i2;
    i2 = 24u;
    i1 = I32_DIV_S(i1, i2);
    StoreU32(ctx, i0 + 104u, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 28u);
    i1 = l9;
    i0 -= i1;
    i1 = 320u;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
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
      i0 = a;
      i0 = LoadU32(ctx, i0 + 44u);
      l4 = i0;
      i1 = 20u;
      i0 += i1;
      i1 = l4;
      i2 = 1u;
      i1 <<= (i2 & 31);
      i2 = l4;
      i3 = 20u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      i0 = i2 ? i0 : i1;
      l4 = i0;
    }
    i0 = l8;
    i1 = 18u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = l4;
      RelocateElementArray(ctx, i0, i1);
      i0 = 18u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 20u);
    i1 = a;
    i1 = LoadU32(ctx, i1 + 8u);
    i2 = 320u;
    i1 += i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = a;
    i1 = 0u;
    StoreU8(ctx, i0 + 57u, i1);
    i0 = l11;
    i1 = 8u;
    i0 += i1;
    l4 = i0;
  }
  i0 = l8;
  i1 = 19u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = l4;
    i2 = b;
    CallTableT0(ctx, i2, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    i0 = 19u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = 1u;
    StoreU8(ctx, i0 + 57u, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 20u);
    l4 = i0;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 32u);
    b = i1;
    i2 = c;
    i3 = l7;
    i2 -= i3;
    i1 += i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = a;
    i1 = l9;
    i2 = l7;
    i1 -= i2;
    i2 = b;
    i1 += i2;
    b = i1;
    StoreU32(ctx, i0 + 8u, i1);
  }
  block_102:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 4u);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 16u);
    b = i0;
  }
  i0 = l8;
  i1 = 20u;
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
    l5 = i0;
    i0 = 20u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {
      i0 = 2u;
      l9 = i0;
      goto block_19;
    }
    i0 = a;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = b;
    i2 = 4u;
    i1 <<= (i2 & 31);
    i0 -= i1;
    b = i0;
  }
  i0 = l8;
  i1 = 21u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = b;
    i0 = CompactEntryQueue(ctx, i0, i1);
    l5 = i0;
    i0 = 21u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    a = i0;
  }
  i0 = l9;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l9 = i0;
  block_19:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l11;
    i1 = 112u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    i0 = l9;
    goto func_exit;
  }
  block_18:;
  i0 = l8;
  i1 = 22u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 5u;
    ThrowScriptError(ctx, i0, i1);
    i0 = 22u;
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
  block_17:;
  i0 = l8;
  i1 = 23u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 5u;
    ThrowScriptError(ctx, i0, i1);
    i0 = 23u;
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
  l5 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l5;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF904ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l5 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l5;
  i1 = c;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l5;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l5;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l5;
  i1 = l6;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l5;
  i1 = l7;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l5;
  i1 = l9;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l5;
  i1 = l10;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = l5;
  i1 = l11;
  StoreU32(ctx, i0 + 36u, i1);
  i0 = l5;
  i1 = l12;
  StoreU32(ctx, i0 + 40u, i1);
  i0 = l5;
  i1 = l13;
  StoreU32(ctx, i0 + 44u, i1);
  i0 = l5;
  i1 = l14;
  StoreU32(ctx, i0 + 48u, i1);
  i0 = l5;
  i1 = l15;
  StoreU32(ctx, i0 + 52u, i1);
  i0 = l5;
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
