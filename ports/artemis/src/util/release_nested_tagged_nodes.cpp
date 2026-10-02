// release_nested_tagged_nodes.cpp -- releases nested tagged nodes: the tail link's tagged payload is dropped before the nested children are freed..
//
// ReleaseNestedTaggedNodes -- releases nested tagged nodes: the tail link's tagged payload is dropped before the nested children are freed.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 262762-262762 (f682); R1 transcribe-keep.
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

constexpr uint32_t kF682ConstB6A90 = 0xB6A90u;  // 
constexpr uint32_t kF682ConstFFFFFFFF = 0xFFFFFFFFu;  // 

}  // namespace

void ReleaseNestedTaggedNodes(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t i0 = 0u; uint32_t i1 = 0u;
  i0 = b;
  if (i0) {
    i0 = a;
    i1 = b;
    i1 = LoadU32(ctx, i1);
    ReleaseNestedTaggedNodes(ctx, i0, i1);
    i0 = a;
    i1 = b;
    i1 = LoadU32(ctx, i1 + 4u);
    ReleaseNestedTaggedNodes(ctx, i0, i1);
    i0 = b;
    i1 = kF682ConstB6A90;
    StoreU32(ctx, i0 + 28u, i1);
    i0 = b;
    i0 = LoadS8(ctx, i0 + 59u);
    i1 = kF682ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0 + 48u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = b;
    i0 = LoadS8(ctx, i0 + 47u);
    i1 = kF682ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0 + 36u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = b;
    i0 = LoadS8(ctx, i0 + 27u);
    i1 = kF682ConstFFFFFFFF;
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
