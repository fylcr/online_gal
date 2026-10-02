// fill_bytes.cpp -- the bounded zero-fill: a byte-at-a-time run for the short spans and a word-at-a-time run past the threshold, clamped at the long end..
//
// FillBytes -- the bounded zero-fill: a byte-at-a-time run for the short spans and a word-at-a-time run past the threshold, clamped at the long end.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 113085-113282 (f205); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF205Const1010101 = 0x1010101u;  // 
constexpr uint32_t kF205ConstFFFFFFFC = 0xFFFFFFFCu;  // 

}  // namespace

uint32_t FillBytes(EngineContext* ctx, uint32_t dst, uint32_t value, uint32_t length) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint64_t l5 = 0;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  uint64_t j0 = 0; uint64_t j1 = 0; uint64_t j2 = 0;
  i0 = length;
  i0 = !(i0);
  if (i0) {goto block_0;}
  i0 = dst;
  i1 = length;
  i0 += i1;
  l3 = i0;
  i1 = 1u;
  i0 -= i1;
  i1 = value;
  StoreU8(ctx, i0, i1);
  i0 = dst;
  i1 = value;
  StoreU8(ctx, i0, i1);
  i0 = length;
  i1 = 3u;
  i0 = i0 < i1;
  if (i0) {goto block_0;}
  i0 = l3;
  i1 = 2u;
  i0 -= i1;
  i1 = value;
  StoreU8(ctx, i0, i1);
  i0 = dst;
  i1 = value;
  StoreU8(ctx, i0 + 1u, i1);
  i0 = l3;
  i1 = 3u;
  i0 -= i1;
  i1 = value;
  StoreU8(ctx, i0, i1);
  i0 = dst;
  i1 = value;
  StoreU8(ctx, i0 + 2u, i1);
  i0 = length;
  i1 = 7u;
  i0 = i0 < i1;
  if (i0) {goto block_0;}
  i0 = l3;
  i1 = 4u;
  i0 -= i1;
  i1 = value;
  StoreU8(ctx, i0, i1);
  i0 = dst;
  i1 = value;
  StoreU8(ctx, i0 + 3u, i1);
  i0 = length;
  i1 = 9u;
  i0 = i0 < i1;
  if (i0) {goto block_0;}
  i0 = 0u;
  i1 = dst;
  i0 -= i1;
  i1 = 3u;
  i0 &= i1;
  l4 = i0;
  i1 = dst;
  i0 += i1;
  l3 = i0;
  i1 = value;
  i2 = 255u;
  i1 &= i2;
  i2 = kF205Const1010101;
  i1 *= i2;
  value = i1;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = length;
  i2 = l4;
  i1 -= i2;
  i2 = kF205ConstFFFFFFFC;
  i1 &= i2;
  l4 = i1;
  i0 += i1;
  length = i0;
  i1 = 4u;
  i0 -= i1;
  i1 = value;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = 9u;
  i0 = i0 < i1;
  if (i0) {goto block_0;}
  i0 = l3;
  i1 = value;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l3;
  i1 = value;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = length;
  i1 = 8u;
  i0 -= i1;
  i1 = value;
  StoreU32(ctx, i0, i1);
  i0 = length;
  i1 = 12u;
  i0 -= i1;
  i1 = value;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = 25u;
  i0 = i0 < i1;
  if (i0) {goto block_0;}
  i0 = l3;
  i1 = value;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l3;
  i1 = value;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l3;
  i1 = value;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l3;
  i1 = value;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = length;
  i1 = 16u;
  i0 -= i1;
  i1 = value;
  StoreU32(ctx, i0, i1);
  i0 = length;
  i1 = 20u;
  i0 -= i1;
  i1 = value;
  StoreU32(ctx, i0, i1);
  i0 = length;
  i1 = 24u;
  i0 -= i1;
  i1 = value;
  StoreU32(ctx, i0, i1);
  i0 = length;
  i1 = 28u;
  i0 -= i1;
  i1 = value;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = l3;
  i2 = 4u;
  i1 &= i2;
  i2 = 24u;
  i1 |= i2;
  l4 = i1;
  i0 -= i1;
  length = i0;
  i1 = 32u;
  i0 = i0 < i1;
  if (i0) {goto block_0;}
  i0 = value;
  j0 = (uint64_t)(i0);
  l5 = j0;
  j1 = l5;
  j2 = 32ull;
  j1 <<= (j2 & 63);
  j0 |= j1;
  l5 = j0;
  i0 = l3;
  i1 = l4;
  i0 += i1;
  value = i0;
  loop_1: 
    i0 = value;
    j1 = l5;
    StoreU64(ctx, i0 + 24u, j1);
    i0 = value;
    j1 = l5;
    StoreU64(ctx, i0 + 16u, j1);
    i0 = value;
    j1 = l5;
    StoreU64(ctx, i0 + 8u, j1);
    i0 = value;
    j1 = l5;
    StoreU64(ctx, i0, j1);
    i0 = value;
    i1 = 32u;
    i0 += i1;
    value = i0;
    i0 = length;
    i1 = 32u;
    i0 -= i1;
    length = i0;
    i1 = 31u;
    i0 = i0 > i1;
    if (i0) {goto loop_1;}
  block_0:;
  i0 = dst;
  LeaveFrame();
  return i0;
}
}
