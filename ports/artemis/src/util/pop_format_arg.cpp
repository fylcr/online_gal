// pop_format_arg.cpp -- the argument-slot reader of the formatter (musl pop_arg): the type code selects one of ten widths (the 32, 16 and 8 bit signed and unsigned values widened into the 64 bit slot, plus the aligned 64 bit and floating cases) whose value is read out of the argument cursor and stored into the result slot..
//
// PopVarArg -- the argument-slot reader of the formatter (musl pop_arg): the type code selects one of ten widths (the 32, 16 and 8 bit signed and unsigned values widened into the 64 bit slot, plus the aligned 64 bit and floating cases) whose value is read out of the argument cursor and stored into the result slot.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 551300-551300 (f1343); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF1343ConstFFFFFFF8 = 0xFFFFFFF8u;  // 
constexpr uint32_t kF1343SaveFrameBytes = 12u;
constexpr uint32_t kF1343ResumeWordBytes = 4u;

}  // namespace

void PopVarArg(EngineContext* ctx, uint32_t out, uint32_t type_code, uint32_t ap, uint32_t scratch) {
  EnterFrame();
  uint32_t l4 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  uint64_t j1 = 0;
  double d1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1343SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    scratch = i0;
    i0 = LoadU32(ctx, i0);
    out = i0;
    i0 = scratch;
    i0 = LoadU32(ctx, i0 + 4u);
    ap = i0;
    i0 = scratch;
    i0 = LoadU32(ctx, i0 + 8u);
    scratch = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1343ResumeWordBytes;
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
    i0 = type_code;
    i1 = 20u;
    i0 = i0 > i1;
    if (i0) {goto block_3;}
    i0 = type_code;
    i1 = 9u;
    i0 -= i1;
    switch (i0) {
      case 0: goto block_14;
      case 1: goto block_13;
      case 2: goto block_12;
      case 3: goto block_11;
      case 4: goto block_10;
      case 5: goto block_9;
      case 6: goto block_8;
      case 7: goto block_7;
      case 8: goto block_6;
      case 9: goto block_5;
      default: goto block_3;
    }
    block_14:;
    i0 = ap;
    i1 = ap;
    i1 = LoadU32(ctx, i1);
    type_code = i1;
    i2 = 4u;
    i1 += i2;
    StoreU32(ctx, i0, i1);
    i0 = out;
    i1 = type_code;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0, i1);
    goto func_exit;
    block_13:;
    i0 = ap;
    i1 = ap;
    i1 = LoadU32(ctx, i1);
    type_code = i1;
    i2 = 4u;
    i1 += i2;
    StoreU32(ctx, i0, i1);
    i0 = out;
    i1 = type_code;
    j1 = (uint64_t)(int64_t)(int32_t)LoadU32(ctx, i1);
    StoreU64(ctx, i0, j1);
    goto func_exit;
    block_12:;
    i0 = ap;
    i1 = ap;
    i1 = LoadU32(ctx, i1);
    type_code = i1;
    i2 = 4u;
    i1 += i2;
    StoreU32(ctx, i0, i1);
    i0 = out;
    i1 = type_code;
    j1 = (uint64_t)LoadU32(ctx, i1);
    StoreU64(ctx, i0, j1);
    goto func_exit;
    block_11:;
    i0 = ap;
    i1 = ap;
    i1 = LoadU32(ctx, i1);
    i2 = 7u;
    i1 += i2;
    i2 = kF1343ConstFFFFFFF8;
    i1 &= i2;
    type_code = i1;
    i2 = 8u;
    i1 += i2;
    StoreU32(ctx, i0, i1);
    i0 = out;
    i1 = type_code;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0, j1);
    goto func_exit;
    block_10:;
    i0 = ap;
    i1 = ap;
    i1 = LoadU32(ctx, i1);
    type_code = i1;
    i2 = 4u;
    i1 += i2;
    StoreU32(ctx, i0, i1);
    i0 = out;
    i1 = type_code;
    j1 = (uint64_t)(int64_t)LoadS16(ctx, i1);
    StoreU64(ctx, i0, j1);
    goto func_exit;
    block_9:;
    i0 = ap;
    i1 = ap;
    i1 = LoadU32(ctx, i1);
    type_code = i1;
    i2 = 4u;
    i1 += i2;
    StoreU32(ctx, i0, i1);
    i0 = out;
    i1 = type_code;
    j1 = (uint64_t)LoadU16(ctx, i1);
    StoreU64(ctx, i0, j1);
    goto func_exit;
    block_8:;
    i0 = ap;
    i1 = ap;
    i1 = LoadU32(ctx, i1);
    type_code = i1;
    i2 = 4u;
    i1 += i2;
    StoreU32(ctx, i0, i1);
    i0 = out;
    i1 = type_code;
    j1 = (uint64_t)(int64_t)LoadS8(ctx, i1);
    StoreU64(ctx, i0, j1);
    goto func_exit;
    block_7:;
    i0 = ap;
    i1 = ap;
    i1 = LoadU32(ctx, i1);
    type_code = i1;
    i2 = 4u;
    i1 += i2;
    StoreU32(ctx, i0, i1);
    i0 = out;
    i1 = type_code;
    j1 = (uint64_t)LoadU8(ctx, i1);
    StoreU64(ctx, i0, j1);
    goto func_exit;
    block_6:;
    i0 = ap;
    i1 = ap;
    i1 = LoadU32(ctx, i1);
    i2 = 7u;
    i1 += i2;
    i2 = kF1343ConstFFFFFFF8;
    i1 &= i2;
    type_code = i1;
    i2 = 8u;
    i1 += i2;
    StoreU32(ctx, i0, i1);
    i0 = out;
    i1 = type_code;
    d1 = LoadF64(ctx, i1);
    StoreF64(ctx, i0, d1);
    goto func_exit;
    block_5:;
  }
  i0 = l4;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = out;
    i1 = ap;
    i2 = scratch;
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
  block_3:;
  goto func_exit;
  spill_frame:;
  type_code = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = type_code;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1343ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  type_code = i0;
  i1 = out;
  StoreU32(ctx, i0, i1);
  i0 = type_code;
  i1 = ap;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = type_code;
  i1 = scratch;
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
