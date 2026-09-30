// release_tagged_node_triple.cpp -- releases a tagged node triple: the three node slots are freed through the node release path in order..
//
// ReleaseTaggedNodeTriple -- releases a tagged node triple: the three node slots are freed through the node release path in order.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 213796-213796 (f534); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);

namespace {

constexpr uint32_t kF534ConstB6AD4 = 0xB6AD4u;  // 
constexpr uint32_t kF534ConstFFFFFFFF = 0xFFFFFFFFu;  // 

}  // namespace

void ReleaseTaggedNodeTriple(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t i0 = 0u; uint32_t i1 = 0u;
  i0 = b;
  if (i0) {
    i0 = a;
    i1 = b;
    i1 = LoadU32(ctx, i1);
    ReleaseTaggedNodeTriple(ctx, i0, i1);
    i0 = a;
    i1 = b;
    i1 = LoadU32(ctx, i1 + 4u);
    ReleaseTaggedNodeTriple(ctx, i0, i1);
    i0 = b;
    i1 = kF534ConstB6AD4;
    StoreU32(ctx, i0 + 28u, i1);
    i0 = b;
    i0 = LoadS8(ctx, i0 + 47u);
    i1 = kF534ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0 + 36u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = b;
    i0 = LoadS8(ctx, i0 + 27u);
    i1 = kF534ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0 + 16u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = b;
    DeallocateRawBlock(ctx, i0);
  }
  LeaveFrame();
}
}
