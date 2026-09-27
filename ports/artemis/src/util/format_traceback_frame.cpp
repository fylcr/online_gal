// format_traceback_frame.cpp -- resolves one traceback frame, walking the locals and the name table for the description..
//
// FormatTracebackFrame -- resolves one traceback frame, walking the locals and the name table for the description.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 206850-206850 (f519); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void GrowValueStack(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t FindPoolEntryByTagByte(EngineContext* ctx, uint32_t pool, uint32_t key);
void FormatSourceId(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t AllocateTable(EngineContext* ctx, uint32_t state, uint32_t entries, uint32_t array);
uint32_t GetTableFieldByNumber(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t ResolveVariableKind(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3);

namespace {

constexpr uint32_t kF519Const2A7C6 = 0x2A7C6u;  // 
constexpr uint32_t kF519MsgTail = 0x2A7C7u;  // tail
constexpr uint32_t kF519MsgTailCall = 0x2A7CCu;  // =(tail call)
constexpr uint32_t kF519MsgC = 0x2A7D9u;  // =[C]
constexpr uint32_t kF519Const2A7DE = 0x2A7DEu;  // 
constexpr uint32_t kF519MsgMain = 0x2A7E0u;  // main
constexpr uint32_t kF519Const2A7E5 = 0x2A7E5u;  // 
constexpr uint32_t kF519ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF519SaveFrameBytes = 36u;
constexpr uint32_t kF519ResumeWordBytes = 4u;

}  // namespace

uint32_t FormatTracebackFrame(EngineContext* ctx, uint32_t state, uint32_t name, uint32_t out) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l13 = 0u; uint32_t l14 = 0u; uint32_t l15 = 0u; uint32_t l16 = 0u; uint32_t l17 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u;
  uint64_t j2 = 0;
      l11 = 0, l12 = 0, l13 = 0, l14 = 0, l15 = 0, l16 = 0, l17 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF519SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l6 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 8u);
    out = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 20u);
    l7 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 24u);
    l8 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 28u);
    l9 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 32u);
    l12 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 4u);
    name = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF519ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l11 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = name;
    i0 = LoadU8(ctx, i0);
    i1 = 62u;
    i0 = i0 == i1;
    l3 = i0;
  }
  i0 = l10;
  i1 = ResumePhase(ctx);
  i1 = !(i1);
  if (i1) {
    i1 = l3;
    if (i1) {
      i1 = state;
      i1 = LoadU32(ctx, i1 + 8u);
      i2 = 16u;
      i1 -= i2;
      l3 = i1;
      i1 = LoadU32(ctx, i1);
      l7 = i1;
      i1 = state;
      i2 = l3;
      StoreU32(ctx, i1 + 8u, i2);
      i1 = name;
      i2 = 1u;
      i1 += i2;
      name = i1;
      goto block_15;
    }
    i1 = out;
    i1 = LoadU32(ctx, i1 + 96u);
    l3 = i1;
    i1 = !(i1);
    if (i1) {goto block_14;}
    i1 = state;
    i1 = LoadU32(ctx, i1 + 40u);
    i2 = l3;
    i3 = 24u;
    i2 *= i3;
    i1 += i2;
    l4 = i1;
    i1 = LoadU32(ctx, i1 + 4u);
    i1 = LoadU32(ctx, i1);
    l7 = i1;
    block_15:;
    i1 = l7;
    i1 = !(i1);
    if (i1) {goto block_14;}
    i1 = name;
    i1 = LoadU8(ctx, i1);
    l5 = i1;
    i1 = !(i1);
    l3 = i1;
    if (i1) {
      i1 = 1u;
      l9 = i1;
      goto block_17;
    }
    i1 = out;
    i2 = 36u;
    i1 += i2;
    l15 = i1;
    i1 = l4;
    i2 = 12u;
    i1 -= i2;
    l13 = i1;
    i1 = l4;
    i2 = 20u;
    i1 -= i2;
    l16 = i1;
    i1 = l4;
    i2 = 24u;
    i1 -= i2;
    l14 = i1;
    i1 = out;
    i2 = kF519ResumeWordBytes;
    i1 += i2;
    l17 = i1;
    i1 = 1u;
    l9 = i1;
    i1 = name;
    l3 = i1;
    loop_19: 
      i1 = l3;
      l8 = i1;
      i1 = l5;
      i2 = 24u;
      i1 <<= (i2 & 31);
      i2 = 24u;
      i1 = (uint32_t)((int32_t)i1 >> (i2 & 31));
      i2 = 76u;
      i1 -= i2;
      switch (i1) {
        case 0: goto block_20;
        case 1: goto block_21;
        case 2: goto block_21;
        case 3: goto block_21;
        case 4: goto block_21;
        case 5: goto block_21;
        case 6: goto block_21;
        case 7: goto block_25;
        case 8: goto block_21;
        case 9: goto block_21;
        case 10: goto block_21;
        case 11: goto block_21;
        case 12: goto block_21;
        case 13: goto block_21;
        case 14: goto block_21;
        case 15: goto block_21;
        case 16: goto block_21;
        case 17: goto block_21;
        case 18: goto block_21;
        case 19: goto block_21;
        case 20: goto block_21;
        case 21: goto block_21;
        case 22: goto block_21;
        case 23: goto block_21;
        case 24: goto block_21;
        case 25: goto block_21;
        case 26: goto block_20;
        case 27: goto block_21;
        case 28: goto block_21;
        case 29: goto block_21;
        case 30: goto block_21;
        case 31: goto block_21;
        case 32: goto block_24;
        case 33: goto block_21;
        case 34: goto block_22;
        case 35: goto block_21;
        case 36: goto block_21;
        case 37: goto block_21;
        case 38: goto block_21;
        case 39: goto block_21;
        case 40: goto block_21;
        case 41: goto block_23;
        default: goto block_21;
      }
      block_25:;
      i1 = l7;
      i1 = LoadU8(ctx, i1 + 6u);
      if (i1) {
        i1 = out;
        i2 = kF519ConstFFFFFFFF;
        StoreU32(ctx, i1 + 28u, i2);
        i1 = kF519MsgC;
        l3 = i1;
        i1 = out;
        i2 = kF519MsgC;
        StoreU32(ctx, i1 + 16u, i2);
        i1 = kF519Const2A7DE;
        l6 = i1;
        i1 = kF519ConstFFFFFFFF;
        goto block_26;
      }
      i1 = out;
      i2 = l7;
      i2 = LoadU32(ctx, i2 + 16u);
      i2 = LoadU32(ctx, i2 + 32u);
      i3 = 16u;
      i2 += i3;
      l3 = i2;
      StoreU32(ctx, i1 + 16u, i2);
      i1 = out;
      i2 = l7;
      i2 = LoadU32(ctx, i2 + 16u);
      i2 = LoadU32(ctx, i2 + 60u);
      l5 = i2;
      StoreU32(ctx, i1 + 28u, i2);
      i1 = kF519Const2A7E5;
      i2 = kF519MsgMain;
      i3 = l5;
      i1 = i3 ? i1 : i2;
      l6 = i1;
      i1 = l7;
      i1 = LoadU32(ctx, i1 + 16u);
      i1 = LoadU32(ctx, i1 + 64u);
      block_26:;
      l5 = i1;
      i1 = out;
      i2 = l6;
      StoreU32(ctx, i1 + 12u, i2);
      i1 = out;
      i2 = l5;
      StoreU32(ctx, i1 + 32u, i2);
      i1 = l15;
      i2 = l3;
      i3 = 60u;
      FormatSourceId(ctx, i1, i2, i3);
      goto block_20;
      block_24:;
      i1 = kF519ConstFFFFFFFF;
      l5 = i1;
      i1 = l4;
      i1 = !(i1);
      if (i1) {goto block_28;}
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 4u);
      l3 = i1;
      i1 = LoadU32(ctx, i1 + 8u);
      i2 = 6u;
      i1 = i1 != i2;
      if (i1) {goto block_28;}
      i1 = l3;
      i1 = LoadU32(ctx, i1);
      l6 = i1;
      i1 = LoadU8(ctx, i1 + 6u);
      if (i1) {goto block_28;}
      i1 = l4;
      i2 = state;
      i2 = LoadU32(ctx, i2 + 20u);
      i1 = i1 != i2;
      if (i1) {
        i1 = l4;
        i1 = LoadU32(ctx, i1 + 12u);
        l10 = i1;
        goto block_29;
      }
      i1 = l4;
      i2 = state;
      i2 = LoadU32(ctx, i2 + 24u);
      l10 = i2;
      StoreU32(ctx, i1 + 12u, i2);
      i1 = l3;
      i1 = LoadU32(ctx, i1);
      l6 = i1;
      block_29:;
      i1 = l10;
      i2 = l6;
      i2 = LoadU32(ctx, i2 + 16u);
      l3 = i2;
      i2 = LoadU32(ctx, i2 + 12u);
      i1 -= i2;
      l6 = i1;
      i2 = 4u;
      i1 = (uint32_t)((int32_t)i1 < (int32_t)i2);
      if (i1) {goto block_28;}
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 20u);
      l3 = i1;
      i1 = !(i1);
      if (i1) {
        i1 = out;
        i2 = 0u;
        StoreU32(ctx, i1 + 20u, i2);
        goto block_20;
      }
      i1 = l6;
      i2 = 2u;
      i1 = (uint32_t)((int32_t)i1 >> (i2 & 31));
      i2 = 1u;
      i1 -= i2;
      i2 = 2u;
      i1 <<= (i2 & 31);
      i2 = l3;
      i1 += i2;
      i1 = LoadU32(ctx, i1);
      l5 = i1;
      block_28:;
      i1 = out;
      i2 = l5;
      StoreU32(ctx, i1 + 20u, i2);
      goto block_20;
      block_23:;
      i1 = out;
      i2 = l7;
      i2 = LoadU8(ctx, i2 + 7u);
      StoreU32(ctx, i1 + 24u, i2);
      goto block_20;
      block_22:;
      i1 = l4;
      i1 = !(i1);
      if (i1) {goto block_32;}
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 4u);
      l3 = i1;
      i1 = LoadU32(ctx, i1 + 8u);
      i2 = 6u;
      i1 = i1 != i2;
      if (i1) {goto block_33;}
      i1 = l3;
      i1 = LoadU32(ctx, i1);
      i1 = LoadU8(ctx, i1 + 6u);
      if (i1) {goto block_33;}
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 20u);
      i2 = 0u;
      i1 = (uint32_t)((int32_t)i1 > (int32_t)i2);
      if (i1) {goto block_32;}
      block_33:;
      i1 = l16;
      i1 = LoadU32(ctx, i1);
      l5 = i1;
      i1 = LoadU32(ctx, i1 + 8u);
      i2 = 6u;
      i1 = i1 != i2;
      if (i1) {goto block_32;}
      i1 = l5;
      i1 = LoadU32(ctx, i1);
      l3 = i1;
      i1 = LoadU8(ctx, i1 + 6u);
      if (i1) {goto block_32;}
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 16u);
      i1 = LoadU32(ctx, i1 + 12u);
      l6 = i1;
      i1 = l14;
      i2 = state;
      i2 = LoadU32(ctx, i2 + 20u);
      i1 = i1 != i2;
      if (i1) {
        i1 = l13;
        i1 = LoadU32(ctx, i1);
        l10 = i1;
        goto block_34;
      }
      i1 = l13;
      i2 = state;
      i2 = LoadU32(ctx, i2 + 24u);
      l10 = i2;
      StoreU32(ctx, i1, i2);
      i1 = l5;
      i1 = LoadU32(ctx, i1);
      l3 = i1;
      block_34:;
      i1 = l6;
      i2 = l10;
      i3 = l3;
      i3 = LoadU32(ctx, i3 + 16u);
      i3 = LoadU32(ctx, i3 + 12u);
      i2 -= i3;
      i1 += i2;
      i2 = 4u;
      i1 -= i2;
      i1 = LoadU32(ctx, i1);
      l3 = i1;
      i2 = 63u;
      i1 &= i2;
      i2 = 28u;
      i1 -= i2;
      l5 = i1;
      i2 = 5u;
      i1 = i1 > i2;
      if (i1) {goto block_32;}
      i1 = 1u;
      i2 = l5;
      i1 <<= (i2 & 31);
      i2 = 35u;
      i1 &= i2;
      i1 = !(i1);
      if (i1) {goto block_32;}
      i1 = out;
      i2 = state;
      i3 = l14;
      i4 = l3;
      i5 = 6u;
      i4 >>= (i5 & 31);
      i5 = 255u;
      i4 &= i5;
      i5 = l17;
      i2 = ResolveVariableKind(ctx, i2, i3, i4, i5);
      l3 = i2;
      StoreU32(ctx, i1 + 8u, i2);
      i1 = l3;
      if (i1) {goto block_20;}
      block_32:;
      i1 = out;
      i2 = 0u;
      StoreU32(ctx, i1 + 4u, i2);
      i1 = out;
      i2 = kF519Const2A7C6;
      StoreU32(ctx, i1 + 8u, i2);
      goto block_20;
      block_21:;
      i1 = 0u;
      l9 = i1;
      block_20:;
      i1 = l8;
      i2 = 1u;
      i1 += i2;
      l3 = i1;
      i1 = l8;
      i1 = LoadU8(ctx, i1 + 1u);
      l5 = i1;
      if (i1) {goto loop_19;}
    block_17:;
    i1 = name;
    i2 = 102u;
    i1 = FindPoolEntryByTagByte(ctx, i1, i2);
    i1 = !(i1);
    out = i1;
    if (i1) {goto block_9;}
    i1 = state;
    i1 = LoadU32(ctx, i1 + 8u);
    out = i1;
    i2 = 6u;
    StoreU32(ctx, i1 + 8u, i2);
    i1 = out;
    i2 = l7;
    StoreU32(ctx, i1, i2);
    i1 = state;
    i1 = LoadU32(ctx, i1 + 8u);
    out = i1;
    i1 = 0u;
    goto block_13;
    block_14:;
    i1 = out;
    j2 = 18446744073709551615ull;
    StoreU64(ctx, i1 + 28u, j2);
    i1 = out;
    i2 = kF519ConstFFFFFFFF;
    StoreU32(ctx, i1 + 20u, i2);
    i1 = out;
    i2 = kF519MsgTail;
    StoreU32(ctx, i1 + 12u, i2);
    i1 = out;
    i2 = kF519Const2A7C6;
    StoreU32(ctx, i1 + 4u, i2);
    i1 = out;
    i2 = kF519Const2A7C6;
    StoreU32(ctx, i1 + 8u, i2);
    i1 = out;
    i2 = kF519MsgTailCall;
    StoreU32(ctx, i1 + 16u, i2);
    i1 = out;
    i2 = 36u;
    i1 += i2;
    l3 = i1;
    i2 = kF519MsgTailCall;
    i3 = 60u;
    FormatSourceId(ctx, i1, i2, i3);
    i1 = 0u;
    l7 = i1;
    i1 = out;
    i2 = 0u;
    StoreU32(ctx, i1 + 24u, i2);
    i1 = name;
    i2 = 102u;
    i1 = FindPoolEntryByTagByte(ctx, i1, i2);
    i1 = !(i1);
    out = i1;
    if (i1) {
      i1 = 1u;
      l9 = i1;
      i1 = 1u;
      l5 = i1;
      goto block_10;
    }
    i1 = state;
    i1 = LoadU32(ctx, i1 + 8u);
    out = i1;
    i2 = 0u;
    StoreU32(ctx, i1 + 8u, i2);
    i1 = 1u;
    l9 = i1;
    i1 = 1u;
    block_13:;
    l5 = i1;
    i1 = state;
    l3 = i1;
    i1 = LoadU32(ctx, i1 + 28u);
    i2 = out;
    i1 -= i2;
    i2 = 16u;
    i1 = (uint32_t)((int32_t)i1 <= (int32_t)i2);
    l8 = i1;
  }
  i1 = l8;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l11;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = 1u;
      GrowValueStack(ctx, i0, i1);
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = out;
    } else {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 8u);
    }
    out = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = out;
    i2 = 16u;
    i1 += i2;
    out = i1;
    StoreU32(ctx, i0 + 8u, i1);
  }
  block_10:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = name;
    i1 = 76u;
    i0 = FindPoolEntryByTagByte(ctx, i0, i1);
    i0 = !(i0);
    name = i0;
    if (i0) {goto running_exit;}
    i0 = l5;
    if (i0) {goto block_7;}
    goto block_8;
  }
  block_9:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = name;
    i1 = 76u;
    i0 = FindPoolEntryByTagByte(ctx, i0, i1);
    i0 = !(i0);
    name = i0;
    if (i0) {goto running_exit;}
  }
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    i0 = LoadU8(ctx, i0 + 6u);
    i0 = !(i0);
    name = i0;
    if (i0) {goto block_6;}
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 8u);
    out = i0;
    i1 = 0u;
    StoreU32(ctx, i0 + 8u, i1);
    goto block_5;
  }
  block_6:;
  i0 = out;
  i1 = 0u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  out = i0;
  i0 = l11;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = 0u;
    i2 = 0u;
    i0 = AllocateTable(ctx, i0, i1, i2);
    l4 = i0;
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    name = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 16u);
    l3 = i0;
    i0 = LoadU32(ctx, i0 + 48u);
    i1 = 1u;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
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
      i0 = LoadU32(ctx, i0 + 20u);
      l3 = i0;
    }
    loop_50: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = out;
        i1 = 2u;
        i0 <<= (i1 & 31);
        i1 = l3;
        i0 += i1;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
      }
      i0 = l11;
      i1 = 2u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = name;
        i2 = l5;
        i0 = GetTableFieldByNumber(ctx, i0, i1, i2);
        l4 = i0;
        i0 = 2u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l4;
        l8 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i1 = 1u;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l8;
        i1 = 1u;
        StoreU32(ctx, i0, i1);
        i0 = out;
        i1 = 1u;
        i0 += i1;
        out = i0;
        i1 = l7;
        i1 = LoadU32(ctx, i1 + 16u);
        i1 = LoadU32(ctx, i1 + 48u);
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l5 = i0;
        if (i0) {goto loop_50;}
      }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 8u);
    out = i0;
    i1 = 5u;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = out;
    i1 = name;
    StoreU32(ctx, i0, i1);
    i0 = state;
    i0 = LoadU32(ctx, i0 + 8u);
    out = i0;
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 28u);
    i1 = out;
    i0 -= i1;
    i1 = 16u;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    l3 = i0;
    i0 = state;
    name = i0;
  }
  i0 = l12;
  i1 = l3;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l12 = i0;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l11;
    i1 = 3u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = 1u;
      GrowValueStack(ctx, i0, i1);
      i0 = 3u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = out;
    } else {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 8u);
    }
    out = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = name;
    i1 = out;
    i2 = 16u;
    i1 += i2;
    StoreU32(ctx, i0 + 8u, i1);
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l9;
    goto func_exit;
  }
  TrapUnreachable();
  spill_frame:;
  l4 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l4;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF519ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = name;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l4;
  i1 = out;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l4;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l4;
  i1 = l5;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l4;
  i1 = l7;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l4;
  i1 = l8;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l4;
  i1 = l9;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l4;
  i1 = l12;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 36u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
