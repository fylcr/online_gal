// format_unsigned_octal.cpp -- the octal digit writer: digits are written backwards from the end of the buffer, three bits at a time, and the new start of the digit run is answered..
//
// FormatUnsignedOctal -- the octal digit writer: digits are written backwards from the end of the buffer, three bits at a time, and the new start of the digit run is answered.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 1173990-1173990 (f2752); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {


}  // namespace

uint32_t FormatUnsignedOctal(EngineContext* ctx, uint64_t value, uint32_t dest) {
  EnterFrame();
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  uint64_t j0 = 0; uint64_t j1 = 0;
  j0 = value;
  i0 = !(j0);
  i0 = !(i0);
  if (i0) {
    loop_1: 
      i0 = dest;
      i1 = 1u;
      i0 -= i1;
      dest = i0;
      j1 = value;
      i1 = (uint32_t)(j1);
      i2 = 7u;
      i1 &= i2;
      i2 = 48u;
      i1 |= i2;
      StoreU8(ctx, i0, i1);
      j0 = value;
      j1 = 3ull;
      j0 >>= (j1 & 63);
      value = j0;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto loop_1;}
  }
  i0 = dest;
  LeaveFrame();
  return i0;
}
}
