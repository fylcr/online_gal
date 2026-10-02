// coerce_to_number.cpp -- turns a value into a number for arithmetic, falling back through the string path and the metamethod path..
//
// CoerceToNumber -- turns a value into a number for arithmetic, falling back through the string path and the metamethod path.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 166453-166453 (f371); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
double ReadNumericFieldAsDouble(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t ReadPackedI64Value(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

constexpr uint32_t kF371SaveFrameBytes = 36u;
constexpr uint32_t kF371ResumeWordBytes = 4u;

}  // namespace

uint32_t CoerceToNumber(EngineContext* ctx, uint32_t state, uint32_t value) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; double l8 = 0; double l9 = 0;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  double d0 = 0; double d1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF371SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 4u);
    value = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    l5 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l6 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 20u);
    l7 = i0;
    i0 = l4;
    d0 = LoadF64(ctx, i0 + 24u);
    l8 = d0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 32u);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF371ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    i1 = 16u;
    i0 -= i1;
    l6 = i0;
    GuestStackSp(ctx) = i0;
    i0 = l6;
    i1 = 12u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l2;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l3;
    d0 = ReadNumericFieldAsDouble(ctx, i0, i1);
    l9 = d0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    d0 = l9;
    l8 = d0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = value;
    d1 = l8;
    StoreF64(ctx, i0, d1);
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i1 = state;
    i0 = i0 == i1;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  if (i0) {
    i0 = l4;
  } else {
    i0 = l5;
    if (i0) {goto block_6;}
    i0 = l3;
    i0 = LoadU8(ctx, i0);
    l5 = i0;
    i1 = 32u;
    i0 |= i1;
    i1 = 120u;
    i0 = i0 == i1;
  }
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l3;
    i1 = l6;
    i2 = 12u;
    i1 += i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l3 = i0;
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l3;
      i2 = 16u;
      i0 = ReadPackedI64Value(ctx, i0, i1, i2);
      l2 = i0;
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l2;
      state = i0;
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l5;
    } else {
      i0 = value;
      i1 = state;
      d1 = (double)(i1);
      StoreF64(ctx, i0, d1);
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 12u);
      l3 = i0;
      i0 = LoadU8(ctx, i0);
    }
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i0 = !(i0);
    if (i0) {
      i0 = 1u;
      l7 = i0;
      goto block_6;
    }
    i0 = l5;
    i1 = 32u;
    i0 = i0 == i1;
    i1 = l5;
    i2 = 9u;
    i1 -= i2;
    i2 = 5u;
    i1 = i1 < i2;
    i0 |= i1;
    i0 = !(i0);
    if (i0) {goto block_6;}
    loop_13: 
      i0 = l3;
      i0 = LoadU8(ctx, i0 + 1u);
      state = i0;
      i0 = l3;
      i1 = 1u;
      i0 += i1;
      l3 = i0;
      i0 = state;
      i1 = 32u;
      i0 = i0 == i1;
      i1 = state;
      i2 = 9u;
      i1 -= i2;
      i2 = 5u;
      i1 = i1 < i2;
      i0 |= i1;
      if (i0) {goto loop_13;}
    i0 = state;
    i0 = !(i0);
    l7 = i0;
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i1 = 16u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    i0 = l7;
    goto func_exit;
  }
  TrapUnreachable();
  spill_frame:;
  l2 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF371ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l2 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l2;
  i1 = value;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l2;
  i1 = l3;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l2;
  i1 = l5;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l2;
  i1 = l6;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l2;
  i1 = l7;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l2;
  d1 = l8;
  StoreF64(ctx, i0 + 24u, d1);
  i0 = l2;
  i1 = l4;
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
