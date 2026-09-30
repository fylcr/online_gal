// fetch_metamethod.cpp -- fetches a metamethod: the operator's metamethod is looked up on the value..
//
// FetchMetamethod -- fetches a metamethod: the operator's metamethod is looked up on the value.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 206785-206785 (f517); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t LookupMaskedMetamethodSlot(EngineContext* ctx, uint32_t a, uint32_t b);

namespace {


}  // namespace

uint32_t FetchMetamethod(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = a;
  i1 = c;
  i0 = LookupMaskedMetamethodSlot(ctx, i0, i1);
  c = i0;
  i0 = LoadU32(ctx, i0 + 8u);
  if (i0) {
    i0 = c;
  } else {
    i0 = a;
    i1 = a;
    i1 = LoadU8(ctx, i1 + 6u);
    i2 = 1u;
    i3 = b;
    i2 <<= (i3 & 31);
    i1 |= i2;
    StoreU8(ctx, i0 + 6u, i1);
    i0 = 0u;
  }
  LeaveFrame();
  return i0;
}
}
