// find_numeric_key_slot.cpp -- walks a table key to the slot a numeric key would occupy..
//
// FindNumericKeySlot -- walks a table key to the slot a numeric key would occupy.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 794773-794773 (f1707); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF1707Const2B328 = 0x2B328u;  // 
constexpr uint32_t kF1707Const2BBD0 = 0x2BBD0u;  // 
constexpr uint32_t kF1707Const7FFFFFFE = 0x7FFFFFFEu;  // 
constexpr uint32_t kF1707ConstFFFFFFFF = 0xFFFFFFFFu;  // 

}  // namespace

uint32_t FindNumericKeySlot(EngineContext* ctx, uint32_t state) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint64_t l7 = 0; uint64_t l8 = 0; double l9 = 0;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  uint64_t j0 = 0; uint64_t j1 = 0; uint64_t j2 = 0;
  double d0 = 0; double d1 = 0;
  i0 = state;
  i0 = LoadU32(ctx, i0 + 28u);
  l2 = i0;
  i0 = !(i0);
  if (i0) {goto spill_frame;}
  i0 = state;
  i0 = LoadU32(ctx, i0 + 12u);
  l1 = i0;
  i1 = l2;
  i2 = 4u;
  i1 <<= (i2 & 31);
  i0 += i1;
  i1 = 8u;
  i0 -= i1;
  i0 = LoadU32(ctx, i0);
  if (i0) {goto spill_frame;}
  i0 = l2;
  i1 = 2u;
  i0 = i0 < i1;
  if (i0) {goto block_0;}
  loop_2: 
    i0 = l2;
    i1 = l3;
    i0 += i1;
    i1 = 1u;
    i0 >>= (i1 & 31);
    state = i0;
    i1 = 4u;
    i0 <<= (i1 & 31);
    i1 = l1;
    i0 += i1;
    i1 = 8u;
    i0 -= i1;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = l2;
    i1 = state;
    i2 = l5;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    i1 = state;
    i2 = l3;
    i3 = l5;
    i1 = i3 ? i1 : i2;
    l3 = i1;
    i0 -= i1;
    i1 = 1u;
    i0 = i0 > i1;
    if (i0) {goto loop_2;}
  goto block_0;
  spill_frame:;
  i0 = state;
  i0 = LoadU32(ctx, i0 + 16u);
  l5 = i0;
  i1 = kF1707Const2BBD0;
  i0 = i0 == i1;
  if (i0) {
    i0 = l2;
    goto func_exit;
  }
  i0 = l2;
  i1 = 1u;
  i0 += i1;
  l1 = i0;
  i0 = l2;
  l3 = i0;
  loop_4: 
    i0 = l1;
    l6 = i0;
    i0 = l2;
    i1 = l1;
    i2 = 1u;
    i1 -= i2;
    l1 = i1;
    i0 = i0 > i1;
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 12u);
      i1 = l1;
      i2 = 4u;
      i1 <<= (i2 & 31);
      i0 += i1;
      l1 = i0;
      goto block_5;
    }
    i0 = l6;
    d0 = (double)(int32_t)(i0);
    l9 = d0;
    i0 = l5;
    l1 = i0;
    i0 = l6;
    if (i0) {
      d0 = l9;
      j0 = i64_reinterpret_f64(d0);
      l7 = j0;
      j1 = 32ull;
      j0 >>= (j1 & 63);
      l8 = j0;
      i0 = l5;
      j1 = l7;
      j2 = l8;
      j1 += j2;
      i1 = (uint32_t)(j1);
      i2 = kF1707ConstFFFFFFFF;
      i3 = state;
      i3 = LoadU8(ctx, i3 + 7u);
      i2 <<= (i3 & 31);
      i3 = kF1707ConstFFFFFFFF;
      i2 ^= i3;
      i3 = 1u;
      i2 |= i3;
      i1 = REM_U(i1, i2);
      i2 = 5u;
      i1 <<= (i2 & 31);
      i0 += i1;
      l1 = i0;
    }
    dispatch_loop: 
      i0 = l1;
      i0 = LoadU32(ctx, i0 + 24u);
      i1 = 3u;
      i0 = i0 == i1;
      if (i0) {
        i0 = l1;
        d0 = LoadF64(ctx, i0 + 16u);
        d1 = l9;
        i0 = d0 == d1;
        if (i0) {goto block_5;}
      }
      i0 = l1;
      i0 = LoadU32(ctx, i0 + 28u);
      l1 = i0;
      if (i0) {goto dispatch_loop;}
    i0 = kF1707Const2B328;
    l1 = i0;
    block_5:;
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 8u);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = l3;
      i0 -= i1;
      i1 = 1u;
      i0 = i0 <= i1;
      if (i0) {goto block_0;}
      loop_11: 
        i0 = l2;
        i1 = l3;
        i2 = l6;
        i1 += i2;
        i2 = 1u;
        i1 >>= (i2 & 31);
        l4 = i1;
        i2 = 1u;
        i1 -= i2;
        l1 = i1;
        i0 = i0 > i1;
        if (i0) {
          i0 = state;
          i0 = LoadU32(ctx, i0 + 12u);
          i1 = l1;
          i2 = 4u;
          i1 <<= (i2 & 31);
          i0 += i1;
          l1 = i0;
          goto block_12;
        }
        i0 = l4;
        d0 = (double)(int32_t)(i0);
        l9 = d0;
        i0 = l5;
        l1 = i0;
        i0 = l4;
        if (i0) {
          d0 = l9;
          j0 = i64_reinterpret_f64(d0);
          l7 = j0;
          j1 = 32ull;
          j0 >>= (j1 & 63);
          l8 = j0;
          i0 = l5;
          j1 = l7;
          j2 = l8;
          j1 += j2;
          i1 = (uint32_t)(j1);
          i2 = kF1707ConstFFFFFFFF;
          i3 = state;
          i3 = LoadU8(ctx, i3 + 7u);
          i2 <<= (i3 & 31);
          i3 = kF1707ConstFFFFFFFF;
          i2 ^= i3;
          i3 = 1u;
          i2 |= i3;
          i1 = REM_U(i1, i2);
          i2 = 5u;
          i1 <<= (i2 & 31);
          i0 += i1;
          l1 = i0;
        }
        loop_15: 
          i0 = l1;
          i0 = LoadU32(ctx, i0 + 24u);
          i1 = 3u;
          i0 = i0 == i1;
          if (i0) {
            i0 = l1;
            d0 = LoadF64(ctx, i0 + 16u);
            d1 = l9;
            i0 = d0 == d1;
            if (i0) {goto block_12;}
          }
          i0 = l1;
          i0 = LoadU32(ctx, i0 + 28u);
          l1 = i0;
          if (i0) {goto loop_15;}
        i0 = kF1707Const2B328;
        l1 = i0;
        block_12:;
        i0 = l6;
        i1 = l4;
        i2 = l1;
        i2 = LoadU32(ctx, i2 + 8u);
        l1 = i2;
        i0 = i2 ? i0 : i1;
        l6 = i0;
        i1 = l4;
        i2 = l3;
        i3 = l1;
        i1 = i3 ? i1 : i2;
        l3 = i1;
        i0 -= i1;
        i1 = 1u;
        i0 = i0 > i1;
        if (i0) {goto loop_11;}
      goto block_0;
    }
    i0 = 1u;
    l4 = i0;
    i0 = l6;
    l3 = i0;
    i1 = 1u;
    i0 <<= (i1 & 31);
    l1 = i0;
    i1 = kF1707Const7FFFFFFE;
    i0 = i0 < i1;
    if (i0) {goto loop_4;}
  loop_17: 
    i0 = l2;
    i1 = l4;
    i2 = 1u;
    i1 -= i2;
    l3 = i1;
    i0 = i0 > i1;
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 12u);
      i1 = l3;
      i2 = 4u;
      i1 <<= (i2 & 31);
      i0 += i1;
      l1 = i0;
      goto block_18;
    }
    i0 = l4;
    d0 = (double)(int32_t)(i0);
    l9 = d0;
    i0 = l5;
    l1 = i0;
    i0 = l4;
    if (i0) {
      d0 = l9;
      j0 = i64_reinterpret_f64(d0);
      l7 = j0;
      j1 = 32ull;
      j0 >>= (j1 & 63);
      l8 = j0;
      i0 = l5;
      j1 = l7;
      j2 = l8;
      j1 += j2;
      i1 = (uint32_t)(j1);
      i2 = kF1707ConstFFFFFFFF;
      i3 = state;
      i3 = LoadU8(ctx, i3 + 7u);
      i2 <<= (i3 & 31);
      i3 = kF1707ConstFFFFFFFF;
      i2 ^= i3;
      i3 = 1u;
      i2 |= i3;
      i1 = REM_U(i1, i2);
      i2 = 5u;
      i1 <<= (i2 & 31);
      i0 += i1;
      l1 = i0;
    }
    loop_21: 
      i0 = l1;
      i0 = LoadU32(ctx, i0 + 24u);
      i1 = 3u;
      i0 = i0 == i1;
      if (i0) {
        i0 = l1;
        d0 = LoadF64(ctx, i0 + 16u);
        d1 = l9;
        i0 = d0 == d1;
        if (i0) {goto block_18;}
      }
      i0 = l1;
      i0 = LoadU32(ctx, i0 + 28u);
      l1 = i0;
      if (i0) {goto loop_21;}
    i0 = kF1707Const2B328;
    l1 = i0;
    block_18:;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 8u);
    if (i0) {goto loop_17;}
  block_0:;
  i0 = l3;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
