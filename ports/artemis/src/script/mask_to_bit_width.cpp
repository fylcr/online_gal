// mask_to_bit_width.cpp -- converts a mask to a bit width: the highest set bit is counted and the width is answered..
//
// MaskToBitWidth -- converts a mask to a bit width: the highest set bit is counted and the width is answered.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 803530-803530 (f1723); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {


}  // namespace

uint32_t MaskToBitWidth(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = a;
  i1 = 16u;
  i0 = i0 < i1;
  if (i0) {
    i0 = 8u;
  } else {
    i0 = 1u;
    l1 = i0;
    loop_1: 
      i0 = l1;
      i1 = 1u;
      i0 += i1;
      l1 = i0;
      i0 = a;
      i1 = 1u;
      i0 += i1;
      l2 = i0;
      i1 = 1u;
      i0 >>= (i1 & 31);
      a = i0;
      i0 = l2;
      i1 = 31u;
      i0 = i0 > i1;
      if (i0) {goto loop_1;}
    i0 = l1;
    i1 = 3u;
    i0 <<= (i1 & 31);
  }
  l1 = i0;
  i0 = a;
  i1 = a;
  i2 = 8u;
  i1 -= i2;
  i2 = l1;
  i1 |= i2;
  i2 = a;
  i3 = 8u;
  i2 = i2 < i3;
  i0 = i2 ? i0 : i1;
  LeaveFrame();
  return i0;
}
}
