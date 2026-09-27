// compare_numeric_literal.cpp -- scans both number literals past their digit runs, skips the whitespace between them and compares the byte ranges that remain..
//
// CompareNumericLiteral -- scans both number literals past their digit runs, skips the whitespace between them and compares the byte ranges that remain.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 199941-200397 (f493); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t SkipWhitespace(EngineContext* ctx, uint32_t text);

namespace {

constexpr uint32_t kF493ConstFFFFFFFF = 0xFFFFFFFFu;  // 

}  // namespace

uint32_t CompareNumericLiteral(EngineContext* ctx, uint32_t state, uint32_t lhs, uint32_t rhs) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
      l11 = 0, l12 = 0;
  i0 = lhs;
  i0 = LoadU32(ctx, i0 + 4u);
  i1 = lhs;
  i1 = LoadU8(ctx, i1 + 11u);
  state = i1;
  i2 = state;
  i3 = 24u;
  i2 <<= (i3 & 31);
  i3 = 24u;
  i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
  l9 = i2;
  i3 = 0u;
  i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
  state = i2;
  i0 = i2 ? i0 : i1;
  l6 = i0;
  i0 = !(i0);
  if (i0) {goto spill_frame;}
  i0 = lhs;
  i0 = LoadU32(ctx, i0);
  l7 = i0;
  i1 = lhs;
  i2 = state;
  i0 = i2 ? i0 : i1;
  l5 = i0;
  i0 = l7;
  i1 = 255u;
  i0 &= i1;
  i1 = 45u;
  i0 = i0 != i1;
  if (i0) {
    i0 = 0u;
    state = i0;
    i0 = l9;
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    if (i0) {goto running_exit;}
    i0 = 1u;
    l3 = i0;
    loop_6: 
      i0 = state;
      if (i0) {
        i0 = state;
        i1 = l5;
        i0 += i1;
        i0 = LoadU8(ctx, i0);
        l4 = i0;
        goto block_8;
      }
      i0 = l7;
      i0 = LoadU8(ctx, i0);
      l4 = i0;
      i1 = 45u;
      i0 = i0 == i1;
      if (i0) {goto block_7;}
      block_8:;
      i0 = l4;
      i1 = 48u;
      i0 -= i1;
      i1 = 9u;
      i0 = i0 > i1;
      if (i0) {goto block_3;}
      block_7:;
      i0 = l6;
      i1 = state;
      i2 = 1u;
      i1 += i2;
      state = i1;
      i0 = i0 > i1;
      l3 = i0;
      i0 = state;
      i1 = l6;
      i0 = i0 != i1;
      if (i0) {goto loop_6;}
    goto block_3;
  }
  i0 = 0u;
  state = i0;
  i0 = 1u;
  l3 = i0;
  i0 = l9;
  i1 = kF493ConstFFFFFFFF;
  i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
  if (i0) {
    loop_11: 
      i0 = state;
      if (i0) {
        i0 = state;
        i1 = l5;
        i0 += i1;
        i0 = LoadU8(ctx, i0);
        l4 = i0;
        goto block_13;
      }
      i0 = l7;
      i0 = LoadU8(ctx, i0);
      l4 = i0;
      i1 = 45u;
      i0 = i0 == i1;
      if (i0) {goto block_12;}
      block_13:;
      i0 = l4;
      i1 = 48u;
      i0 -= i1;
      i1 = 9u;
      i0 = i0 > i1;
      if (i0) {goto block_3;}
      block_12:;
      i0 = l6;
      i1 = state;
      i2 = 1u;
      i1 += i2;
      state = i1;
      i0 = i0 > i1;
      l3 = i0;
      i0 = state;
      i1 = l6;
      i0 = i0 != i1;
      if (i0) {goto loop_11;}
    goto block_3;
  }
  loop_15: 
    i0 = state;
    if (i0) {
      i0 = state;
      i1 = l5;
      i0 += i1;
      i0 = LoadU8(ctx, i0);
      i1 = 48u;
      i0 -= i1;
      i1 = 9u;
      i0 = i0 > i1;
      if (i0) {goto block_3;}
    }
    i0 = l6;
    i1 = state;
    i2 = 1u;
    i1 += i2;
    state = i1;
    i0 = i0 > i1;
    l3 = i0;
    i0 = state;
    i1 = l6;
    i0 = i0 != i1;
    if (i0) {goto loop_15;}
  goto block_3;
  running_exit:;
  i0 = lhs;
  i0 = LoadU8(ctx, i0);
  i1 = 48u;
  i0 -= i1;
  i1 = 10u;
  i0 = i0 >= i1;
  if (i0) {goto block_2;}
  loop_17: 
    i0 = l6;
    i1 = state;
    i2 = 1u;
    i1 += i2;
    state = i1;
    i0 = i0 != i1;
    if (i0) {
      i0 = state;
      i1 = l5;
      i0 += i1;
      i0 = LoadU8(ctx, i0);
      i1 = 48u;
      i0 -= i1;
      i1 = 9u;
      i0 = i0 <= i1;
      if (i0) {goto loop_17;}
    }
  i0 = state;
  i1 = l6;
  i0 = i0 < i1;
  l3 = i0;
  block_3:;
  i0 = l3;
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto spill_frame;}
  block_2:;
  i0 = rhs;
  i0 = LoadU32(ctx, i0 + 4u);
  l12 = i0;
  i0 = rhs;
  i0 = LoadU8(ctx, i0 + 11u);
  l11 = i0;
  l5 = i0;
  goto block_0;
  spill_frame:;
  i0 = rhs;
  i0 = LoadU32(ctx, i0);
  l7 = i0;
  i0 = rhs;
  i0 = LoadU8(ctx, i0 + 11u);
  l11 = i0;
  i1 = 24u;
  i0 <<= (i1 & 31);
  i1 = 24u;
  i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
  l5 = i0;
  i1 = 0u;
  i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
  state = i0;
  i0 = rhs;
  i0 = LoadU32(ctx, i0 + 4u);
  l12 = i0;
  i1 = l11;
  i2 = state;
  i0 = i2 ? i0 : i1;
  l3 = i0;
  if (i0) {
    i0 = 0u;
    state = i0;
    i0 = l7;
    i1 = rhs;
    i2 = l5;
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    l4 = i2;
    i0 = i2 ? i0 : i1;
    l10 = i0;
    i0 = l7;
    i1 = 255u;
    i0 &= i1;
    i1 = 45u;
    i0 = i0 != i1;
    if (i0) {
      i0 = l4;
      i0 = !(i0);
      if (i0) {goto block_21;}
      i0 = 1u;
      l4 = i0;
      loop_23: 
        i0 = state;
        if (i0) {
          i0 = state;
          i1 = l10;
          i0 += i1;
          i0 = LoadU8(ctx, i0);
          l8 = i0;
          goto block_25;
        }
        i0 = l7;
        i0 = LoadU8(ctx, i0);
        l8 = i0;
        i1 = 45u;
        i0 = i0 == i1;
        if (i0) {goto block_24;}
        block_25:;
        i0 = l8;
        i1 = 48u;
        i0 -= i1;
        i1 = 9u;
        i0 = i0 > i1;
        if (i0) {goto block_20;}
        block_24:;
        i0 = l3;
        i1 = state;
        i2 = 1u;
        i1 += i2;
        state = i1;
        i0 = i0 > i1;
        l4 = i0;
        i0 = state;
        i1 = l3;
        i0 = i0 != i1;
        if (i0) {goto loop_23;}
      goto block_20;
    }
    i0 = 1u;
    l4 = i0;
    i0 = l5;
    i1 = kF493ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      loop_28: 
        i0 = state;
        if (i0) {
          i0 = state;
          i1 = l10;
          i0 += i1;
          i0 = LoadU8(ctx, i0);
          l8 = i0;
          goto block_30;
        }
        i0 = l7;
        i0 = LoadU8(ctx, i0);
        l8 = i0;
        i1 = 45u;
        i0 = i0 == i1;
        if (i0) {goto block_29;}
        block_30:;
        i0 = l8;
        i1 = 48u;
        i0 -= i1;
        i1 = 9u;
        i0 = i0 > i1;
        if (i0) {goto block_20;}
        block_29:;
        i0 = l3;
        i1 = state;
        i2 = 1u;
        i1 += i2;
        state = i1;
        i0 = i0 > i1;
        l4 = i0;
        i0 = state;
        i1 = l3;
        i0 = i0 != i1;
        if (i0) {goto loop_28;}
      goto block_20;
    }
    loop_32: 
      i0 = state;
      if (i0) {
        i0 = state;
        i1 = l10;
        i0 += i1;
        i0 = LoadU8(ctx, i0);
        i1 = 48u;
        i0 -= i1;
        i1 = 9u;
        i0 = i0 > i1;
        if (i0) {goto block_20;}
      }
      i0 = l3;
      i1 = state;
      i2 = 1u;
      i1 += i2;
      state = i1;
      i0 = i0 > i1;
      l4 = i0;
      i0 = state;
      i1 = l3;
      i0 = i0 != i1;
      if (i0) {goto loop_32;}
    goto block_20;
    block_21:;
    i0 = rhs;
    i0 = LoadU8(ctx, i0);
    i1 = 48u;
    i0 -= i1;
    i1 = 10u;
    i0 = i0 >= i1;
    if (i0) {goto block_0;}
    loop_34: 
      i0 = l3;
      i1 = state;
      i2 = 1u;
      i1 += i2;
      state = i1;
      i0 = i0 != i1;
      if (i0) {
        i0 = state;
        i1 = l10;
        i0 += i1;
        i0 = LoadU8(ctx, i0);
        i1 = 48u;
        i0 -= i1;
        i1 = 9u;
        i0 = i0 <= i1;
        if (i0) {goto loop_34;}
      }
    i0 = state;
    i1 = l3;
    i0 = i0 < i1;
    l4 = i0;
    block_20:;
    i0 = l4;
    if (i0) {goto block_0;}
  }
  i0 = lhs;
  i0 = LoadU32(ctx, i0);
  i1 = lhs;
  i2 = l9;
  i3 = 0u;
  i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
  i0 = i2 ? i0 : i1;
  i0 = SkipWhitespace(ctx, i0);
  i1 = l7;
  i2 = rhs;
  i3 = l5;
  i4 = 0u;
  i3 = (uint32_t)((int32_t)i3 < (int32_t)i4);
  i1 = i3 ? i1 : i2;
  i1 = SkipWhitespace(ctx, i1);
  i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
  goto func_exit;
  block_0:;
  i0 = l6;
  i1 = l12;
  i2 = l11;
  i3 = l5;
  i4 = 24u;
  i3 <<= (i4 & 31);
  i4 = 24u;
  i3 = (uint32_t)((int32_t)i3 >> (i4 & 31));
  i4 = 0u;
  i3 = (uint32_t)((int32_t)i3 < (int32_t)i4);
  l3 = i3;
  i1 = i3 ? i1 : i2;
  state = i1;
  i0 = i0 > i1;
  l4 = i0;
  i0 = state;
  i1 = l6;
  i2 = l4;
  i0 = i2 ? i0 : i1;
  l5 = i0;
  if (i0) {
    i0 = lhs;
    i0 = LoadU32(ctx, i0);
    i1 = lhs;
    i2 = l9;
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = rhs;
    i1 = LoadU32(ctx, i1);
    i2 = rhs;
    i3 = l3;
    i1 = i3 ? i1 : i2;
    i2 = l5;
    i0 = CompareBytes(ctx, i0, i1, i2);
    l3 = i0;
    if (i0) {goto block_36;}
  }
  i0 = kF493ConstFFFFFFFF;
  i1 = l4;
  i2 = state;
  i3 = l6;
  i2 = i2 > i3;
  i0 = i2 ? i0 : i1;
  l3 = i0;
  block_36:;
  i0 = l3;
  i1 = 31u;
  i0 >>= (i1 & 31);
  func_exit:;
  LeaveFrame();
  return i0;
}
}
