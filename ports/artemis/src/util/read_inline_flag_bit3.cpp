// read_inline_flag_bit3.cpp -- reads bit 3 of the inline flag byte at an index and answers it as a flag..
//
// ReadInlineFlagBit3 -- reads bit 3 of the inline flag byte at an index and answers it as a flag.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 1607674-1607674 (f3516); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t ClearIntegerConstant(EngineContext* ctx, uint32_t slot, uint32_t tag);

namespace {


}  // namespace

uint32_t ReadInlineFlagBit3(EngineContext* ctx, uint32_t state, uint32_t index) {
  EnterFrame();
  uint32_t l2 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u;
  i0 = index;
  i1 = 319u;
  i0 = i0 <= i1;
  if (i0) {
    i0 = index;
    i1 = 2u;
    i0 <<= (i1 & 31);
    i1 = state;
    i0 += i1;
    i1 = 8516u;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i1 = 63u;
    i0 &= i1;
    if (i0) {goto block_0;}
  }
  i0 = state;
  i1 = index;
  i0 = ClearIntegerConstant(ctx, i0, i1);
  goto func_exit;
  block_0:;
  i0 = l2;
  i1 = 8u;
  i0 &= i1;
  i1 = 3u;
  i0 >>= (i1 & 31);
  func_exit:;
  LeaveFrame();
  return i0;
}
}
