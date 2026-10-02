// compile_numeric_power.cpp -- compiles a numeric power: the exponent form is folded through the constant path..
//
// CompileNumericPower -- compiles a numeric power: the exponent form is folded through the constant path.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 221453-221453 (f550); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t EmitInstruction(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t ExpressionToRegister(EngineContext* ctx, uint32_t func_state, uint32_t expr);
double ComputeNumericPower(EngineContext* ctx, double base, double exponent);

namespace {

constexpr uint32_t kF550ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF550SaveFrameBytes = 24u;
constexpr uint32_t kF550ResumeWordBytes = 4u;

}  // namespace

void CompileNumericPower(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
  EnterFrame();
  double l10 = 0; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; double l8 = 0; double l9 = 0;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  double d0 = 0; double d1 = 0; double d2 = 0; double d3 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF550SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    d = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = l4;
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
    i2 = kF550ResumeWordBytes;
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
    i0 = c;
    i0 = LoadU32(ctx, i0);
    i1 = 5u;
    i0 = i0 != i1;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    if (i0) {goto block_7;}
    i0 = c;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = kF550ConstFFFFFFFF;
    i0 = i0 != i1;
    if (i0) {goto block_7;}
    i0 = c;
    i0 = LoadU32(ctx, i0 + 20u);
    i1 = kF550ConstFFFFFFFF;
    i0 = i0 != i1;
    if (i0) {goto block_7;}
    i0 = d;
    i0 = LoadU32(ctx, i0);
    i1 = 5u;
    i0 = i0 != i1;
    if (i0) {goto block_7;}
    i0 = d;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = kF550ConstFFFFFFFF;
    i0 = i0 != i1;
    if (i0) {goto block_7;}
    i0 = d;
    i0 = LoadU32(ctx, i0 + 20u);
    i1 = kF550ConstFFFFFFFF;
    i0 = i0 != i1;
    if (i0) {goto block_7;}
    i0 = d;
    d0 = LoadF64(ctx, i0 + 8u);
    l8 = d0;
    i0 = c;
    d0 = LoadF64(ctx, i0 + 8u);
    l9 = d0;
    i0 = b;
    i1 = 12u;
    i0 -= i1;
    l5 = i0;
    switch (i0) {
      case 0: goto block_16;
      case 1: goto block_15;
      case 2: goto block_14;
      case 3: goto block_13;
      case 4: goto block_12;
      case 5: goto block_11;
      case 6: goto block_10;
      case 7: goto block_8;
      case 8: goto running_exit;
      default: goto block_8;
    }
    block_16:;
    d0 = l9;
    d1 = l8;
    d0 += d1;
    goto block_9;
    block_15:;
    d0 = l9;
    d1 = l8;
    d0 -= d1;
    goto block_9;
    block_14:;
    d0 = l9;
    d1 = l8;
    d0 *= d1;
    goto block_9;
    block_13:;
    d0 = l8;
    d1 = 0.0;
    i0 = d0 == d1;
    l5 = i0;
    if (i0) {goto block_6;}
    d0 = l9;
    d1 = l8;
    d0 /= d1;
    goto block_9;
    block_12:;
    d0 = l8;
    d1 = 0.0;
    i0 = d0 == d1;
    l5 = i0;
    if (i0) {goto block_6;}
    d0 = l9;
    d1 = l8;
    d2 = l9;
    d3 = l8;
    d2 /= d3;
    d2 = wasm_floor(d2);
    d1 *= d2;
    d0 -= d1;
    goto block_9;
    block_11:;
    d0 = l9;
    d1 = l8;
    d0 = ComputeNumericPower(ctx, d0, d1);
    goto block_9;
    block_10:;
    d0 = l9;
    d0 = -(d0);
    block_9:;
    l10 = d0;
    d1 = l10;
    i0 = d0 != d1;
    if (i0) {goto block_7;}
    block_8:;
    i0 = c;
    d1 = l10;
    StoreF64(ctx, i0 + 8u, d1);
    goto func_exit;
    block_7:;
    i0 = b;
    i1 = 18u;
    i0 -= i1;
    l5 = i0;
    switch (i0) {
      case 0: goto running_exit;
      case 1: goto block_6;
      case 2: goto running_exit;
      default: goto block_6;
    }
    block_6:;
  }
  i0 = l7;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = d;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    l6 = i0;
  }
  running_exit:;
  i0 = l7;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = c;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l6;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {
      i0 = c;
      i0 = LoadU32(ctx, i0);
      i1 = 12u;
      i0 = i0 != i1;
      if (i0) {goto block_22;}
      i0 = c;
      i0 = LoadU32(ctx, i0 + 8u);
      l4 = i0;
      i1 = 256u;
      i0 &= i1;
      if (i0) {goto block_22;}
      i0 = a;
      i0 = LoadU8(ctx, i0 + 50u);
      i1 = l4;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      if (i0) {goto block_22;}
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 36u);
      i2 = 1u;
      i1 -= i2;
      StoreU32(ctx, i0 + 36u, i1);
      block_22:;
      i0 = d;
      i0 = LoadU32(ctx, i0);
      i1 = 12u;
      i0 = i0 != i1;
      if (i0) {goto block_20;}
      i0 = d;
      i0 = LoadU32(ctx, i0 + 8u);
      d = i0;
      i1 = 256u;
      i0 &= i1;
      if (i0) {goto block_20;}
      i0 = a;
      i0 = LoadU8(ctx, i0 + 50u);
      i1 = d;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      if (i0) {goto block_20;}
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 36u);
      i2 = 1u;
      i1 -= i2;
      StoreU32(ctx, i0 + 36u, i1);
      goto block_20;
    }
    i0 = d;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    if (i0) {goto block_23;}
    i0 = d;
    i0 = LoadU32(ctx, i0 + 8u);
    d = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_23;}
    i0 = a;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = d;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_23;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 36u, i1);
    block_23:;
    i0 = c;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    if (i0) {goto block_20;}
    i0 = c;
    i0 = LoadU32(ctx, i0 + 8u);
    d = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_20;}
    i0 = a;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = d;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_20;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 36u, i1);
    block_20:;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 12u);
    i0 = LoadU32(ctx, i0 + 8u);
    d = i0;
    i0 = l6;
    i1 = 14u;
    i0 <<= (i1 & 31);
    i1 = b;
    i0 |= i1;
    i1 = l5;
    i2 = 23u;
    i1 <<= (i2 & 31);
    i0 |= i1;
    b = i0;
  }
  i0 = l7;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = b;
    i2 = d;
    i0 = EmitInstruction(ctx, i0, i1, i2);
    l4 = i0;
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    a = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i1 = 11u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = a;
    StoreU32(ctx, i0 + 8u, i1);
  }
  goto func_exit;
  spill_frame:;
  l4 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l4;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF550ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l4;
  i1 = c;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l4;
  i1 = d;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l4;
  i1 = l5;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l4;
  i1 = l6;
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
