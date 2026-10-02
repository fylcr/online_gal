// find_last_char_in_string.cpp -- string wrapper over the backwards byte scan: reads the base pointer and the short-string length out of the member string..
//
// FindLastCharInString -- string wrapper over the backwards byte scan: reads the base pointer and the short-string length out of the member string.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 277050-277050 (f722); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t FindLastByte(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

namespace {


}  // namespace

uint32_t FindLastCharInString(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  i0 = a;
  i0 = LoadU8(ctx, i0 + 11u);
  i1 = 7u;
  i0 >>= (i1 & 31);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0);
    goto block_0;
  }
  i0 = a;
  block_0:;
  i1 = a;
  i1 = LoadU8(ctx, i1 + 11u);
  i2 = 7u;
  i1 >>= (i2 & 31);
  if (i1) {
    i1 = a;
    i1 = LoadU32(ctx, i1 + 4u);
    goto block_2;
  }
  i1 = a;
  i1 = LoadU8(ctx, i1 + 11u);
  block_2:;
  i2 = b;
  i0 = FindLastByte(ctx, i0, i1, i2);
  LeaveFrame();
  return i0;
}
}
