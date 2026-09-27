// skip_whitespace.cpp -- advances past ASCII whitespace (space, tab through carriage return) and answers the first non-space byte pointer..
//
// SkipWhitespace -- advances past ASCII whitespace (space, tab through carriage return) and answers the first non-space byte pointer.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 116302-116302 (f220); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {


}  // namespace

uint32_t SkipWhitespace(EngineContext* ctx, uint32_t text) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  loop_0: 
    i0 = text;
    l1 = i0;
    i1 = 1u;
    i0 += i1;
    text = i0;
    i0 = l1;
    i0 = LoadS8(ctx, i0);
    l2 = i0;
    i1 = 32u;
    i0 = i0 == i1;
    l4 = i0;
    i0 = l4;
    i1 = l2;
    i2 = 9u;
    i1 -= i2;
    i2 = 5u;
    i1 = i1 < i2;
    i0 |= i1;
    if (i0) {goto loop_0;}
  i0 = l1;
  i0 = LoadS8(ctx, i0);
  l2 = i0;
  i1 = 43u;
  i0 -= i1;
  switch (i0) {
    case 0: goto block_2;
    case 1: goto spill_frame;
    case 2: goto block_3;
    default: goto spill_frame;
  }
  block_3:;
  i0 = 1u;
  l5 = i0;
  block_2:;
  i0 = text;
  i0 = LoadS8(ctx, i0);
  l2 = i0;
  i0 = text;
  l1 = i0;
  i0 = l5;
  l6 = i0;
  spill_frame:;
  i0 = l2;
  i1 = 48u;
  i0 -= i1;
  i1 = 10u;
  i0 = i0 < i1;
  if (i0) {
    loop_5: 
      i0 = l3;
      i1 = 10u;
      i0 *= i1;
      i1 = l1;
      i1 = LoadS8(ctx, i1);
      i0 -= i1;
      i1 = 48u;
      i0 += i1;
      l3 = i0;
      i0 = l1;
      i0 = LoadS8(ctx, i0 + 1u);
      text = i0;
      i0 = l1;
      i1 = 1u;
      i0 += i1;
      l1 = i0;
      i0 = text;
      i1 = 48u;
      i0 -= i1;
      i1 = 10u;
      i0 = i0 < i1;
      if (i0) {goto loop_5;}
  }
  i0 = l3;
  i1 = 0u;
  i2 = l3;
  i1 -= i2;
  i2 = l6;
  i0 = i2 ? i0 : i1;
  LeaveFrame();
  return i0;
}
}
