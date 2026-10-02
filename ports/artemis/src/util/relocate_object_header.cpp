// relocate_object_header.cpp -- moves a header down the slab, repacking the tag byte and the two flag bytes into the new position..
//
// RelocateObjectHeader -- moves a header down the slab, repacking the tag byte and the two flag bytes into the new position.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 234536-234564 (f604); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {


}  // namespace

void RelocateObjectHeader(EngineContext* ctx, uint32_t state, uint32_t source, uint32_t tag) {
  EnterFrame();
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  i0 = source;
  i1 = state;
  i1 = LoadU32(ctx, i1 + 16u);
  state = i1;
  i1 = LoadU32(ctx, i1 + 28u);
  StoreU32(ctx, i0, i1);
  i0 = state;
  i1 = source;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = state;
  i0 = LoadU8(ctx, i0 + 20u);
  state = i0;
  i0 = source;
  i1 = tag;
  StoreU8(ctx, i0 + 4u, i1);
  i0 = source;
  i1 = state;
  i2 = 3u;
  i1 &= i2;
  StoreU8(ctx, i0 + 5u, i1);
  LeaveFrame();
}
}
