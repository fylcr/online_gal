// format_unsigned_decimal.cpp -- the decimal digit writer: the high digits are peeled off while the value does not fit in thirty-two bits and the remaining digits are written backwards from the end of the buffer, answering the start of the digit run..
//
// FormatUnsignedDecimal -- the decimal digit writer: the high digits are peeled off while the value does not fit in thirty-two bits and the remaining digits are written backwards from the end of the buffer, answering the start of the digit run.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 259113-259113 (f672); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {


}  // namespace

uint32_t FormatUnsignedDecimal(EngineContext* ctx, uint64_t value, uint32_t dest) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint64_t l5 = 0;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  uint64_t j0 = 0; uint64_t j1 = 0; uint64_t j2 = 0; uint64_t j3 = 0;
  j0 = value;
  j1 = 4294967296ull;
  i0 = j0 < j1;
  if (i0) {
    j0 = value;
    l5 = j0;
    goto block_0;
  }
  loop_2: 
    i0 = dest;
    i1 = 1u;
    i0 -= i1;
    dest = i0;
    j1 = value;
    j2 = value;
    j3 = 10ull;
    j2 = DIV_U(j2, j3);
    l5 = j2;
    j3 = 10ull;
    j2 *= j3;
    j1 -= j2;
    i1 = (uint32_t)(j1);
    i2 = 48u;
    i1 |= i2;
    StoreU8(ctx, i0, i1);
    j0 = value;
    j1 = 42949672959ull;
    i0 = j0 > j1;
    l2 = i0;
    j0 = l5;
    value = j0;
    i0 = l2;
    if (i0) {goto loop_2;}
  block_0:;
  j0 = l5;
  i0 = (uint32_t)(j0);
  l2 = i0;
  if (i0) {
    loop_4: 
      i0 = dest;
      i1 = 1u;
      i0 -= i1;
      dest = i0;
      i1 = l2;
      i2 = l2;
      i3 = 10u;
      i2 = DIV_U(i2, i3);
      l3 = i2;
      i3 = 10u;
      i2 *= i3;
      i1 -= i2;
      i2 = 48u;
      i1 |= i2;
      StoreU8(ctx, i0, i1);
      i0 = l2;
      i1 = 9u;
      i0 = i0 > i1;
      l4 = i0;
      i0 = l3;
      l2 = i0;
      i0 = l4;
      if (i0) {goto loop_4;}
  }
  i0 = dest;
  LeaveFrame();
  return i0;
}
}
