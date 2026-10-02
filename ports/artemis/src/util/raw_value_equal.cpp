// raw_value_equal.cpp -- the four-way raw comparison: the pointer test first, then the tag-selected compare, with the double slot the last of the cases..
//
// RawValueEqual -- the four-way raw comparison: the pointer test first, then the tag-selected compare, with the double slot the last of the cases.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 234482-234534 (f603); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {


}  // namespace

uint32_t RawValueEqual(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u;
  double d0 = 0; double d1 = 0;
  i0 = a;
  i0 = LoadU32(ctx, i0 + 8u);
  l3 = i0;
  i1 = b;
  i1 = LoadU32(ctx, i1 + 8u);
  i0 = i0 != i1;
  if (i0) {goto spill_frame;}
  i0 = 1u;
  l2 = i0;
  i0 = l3;
  switch (i0) {
    case 0: goto spill_frame;
    case 1: goto block_0;
    case 2: goto block_0;
    case 3: goto block_3;
    default: goto block_2;
  }
  block_3:;
  i0 = a;
  d0 = LoadF64(ctx, i0);
  i1 = b;
  d1 = LoadF64(ctx, i1);
  i0 = d0 == d1;
  goto func_exit;
  block_2:;
  i0 = a;
  i0 = LoadU32(ctx, i0);
  i1 = b;
  i1 = LoadU32(ctx, i1);
  i0 = i0 == i1;
  l2 = i0;
  spill_frame:;
  i0 = l2;
  goto func_exit;
  block_0:;
  i0 = a;
  i0 = LoadU32(ctx, i0);
  i1 = b;
  i1 = LoadU32(ctx, i1);
  i0 = i0 == i1;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
