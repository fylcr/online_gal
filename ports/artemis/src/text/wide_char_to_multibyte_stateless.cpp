// wide_char_to_multibyte_stateless.cpp -- the stateless face of the conversion: a null destination answers zero, otherwise the stateful encoder is called with a null state argument..
//
// WideCharToMultibyteStateless -- the stateless face of the conversion: a null destination answers zero, otherwise the stateful encoder is called with a null state argument.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 543280-543280 (f1322); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t WideCharToMultibyte(EngineContext* ctx, uint32_t dest, uint32_t wide, uint32_t state);

namespace {


}  // namespace

uint32_t WideCharToMultibyteStateless(EngineContext* ctx, uint32_t dest, uint32_t wide) {
  EnterFrame();
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  i0 = dest;
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    goto func_exit;
  }
  i0 = dest;
  i1 = wide;
  i2 = 0u;
  i0 = WideCharToMultibyte(ctx, i0, i1, i2);
  func_exit:;
  LeaveFrame();
  return i0;
}
}
