// release_map_field_subtree.cpp -- releases the map subtree rooted in the field at offset four of the owning object..
//
// ReleaseMapFieldSubtree -- releases the map subtree rooted in the field at offset four of the owning object.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 680641-680641 (f1527); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void ReleaseNodeSubtreeB(EngineContext* ctx, uint32_t a, uint32_t b);

namespace {


}  // namespace

void ReleaseMapFieldSubtree(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t i0 = 0u; uint32_t i1 = 0u;
  i0 = a;
  i1 = a;
  i1 = LoadU32(ctx, i1 + 4u);
  ReleaseNodeSubtreeB(ctx, i0, i1);
  LeaveFrame();
}
}
