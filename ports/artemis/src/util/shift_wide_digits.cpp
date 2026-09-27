// shift_wide_digits.cpp -- the wide digit step: the 128-bit pair is shifted left by the byte-derived count and written back as two 64-bit halves..
//
// ShiftWideDigits -- the wide digit step: the 128-bit pair is shifted left by the byte-derived count and written back as two 64-bit halves.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 176164-176216 (f401); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF401ConstFFFFFFC0 = 0xFFFFFFC0u;  // 

}  // namespace

void ShiftWideDigits(EngineContext* ctx, uint32_t dst, uint64_t hi, uint64_t lo, uint32_t bytes) {
  EnterFrame();
  uint64_t l4 = 0;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  uint64_t j0 = 0; uint64_t j1 = 0; uint64_t j2 = 0;
  i0 = bytes;
  i1 = 64u;
  i0 &= i1;
  if (i0) {
    j0 = hi;
    i1 = bytes;
    i2 = kF401ConstFFFFFFC0;
    i1 += i2;
    j1 = (uint64_t)(i1);
    j0 <<= (j1 & 63);
    lo = j0;
    j0 = 0ull;
    hi = j0;
    goto block_0;
  }
  i0 = bytes;
  i0 = !(i0);
  if (i0) {goto block_0;}
  j0 = lo;
  i1 = bytes;
  j1 = (uint64_t)(i1);
  l4 = j1;
  j0 <<= (j1 & 63);
  j1 = hi;
  i2 = 64u;
  i3 = bytes;
  i2 -= i3;
  j2 = (uint64_t)(i2);
  j1 >>= (j2 & 63);
  j0 |= j1;
  lo = j0;
  j0 = hi;
  j1 = l4;
  j0 <<= (j1 & 63);
  hi = j0;
  block_0:;
  i0 = dst;
  j1 = hi;
  StoreU64(ctx, i0, j1);
  i0 = dst;
  j1 = lo;
  StoreU64(ctx, i0 + 8u, j1);
  LeaveFrame();
}
}
