// release_node_tree.cpp -- releases a two-child node tree depth-first and frees the owned block behind the tail field when the flag byte marks it owned..
//
// ReleaseNodeTree -- releases a two-child node tree depth-first and frees the owned block behind the tail field when the flag byte marks it owned.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 113039-113039 (f204); R1 transcribe-keep.
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

constexpr uint32_t kF204ConstFFFFFFFF = 0xFFFFFFFFu;  // 

}  // namespace

void ReleaseNodeTree(EngineContext* ctx, uint32_t state, uint32_t node) {
  EnterFrame();
  uint32_t i0 = 0u; uint32_t i1 = 0u;
  i0 = node;
  if (i0) {
    i0 = state;
    i1 = node;
    i1 = LoadU32(ctx, i1);
    ReleaseNodeTree(ctx, i0, i1);
    i0 = state;
    i1 = node;
    i1 = LoadU32(ctx, i1 + 4u);
    ReleaseNodeTree(ctx, i0, i1);
    i0 = node;
    i0 = LoadS8(ctx, i0 + 39u);
    i1 = kF204ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = node;
      i0 = LoadU32(ctx, i0 + 28u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = node;
    i0 = LoadS8(ctx, i0 + 27u);
    i1 = kF204ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = node;
      i0 = LoadU32(ctx, i0 + 16u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = node;
    DeallocateRawBlock(ctx, i0);
  }
  LeaveFrame();
}
}
