// release_linked_node_tree.cpp -- frees a linked node tree: it recurses into both children, hands the tail link to the next slot before releasing it, drops the inline payload when the size byte has the high bit set, and returns the node to the pool..
//
// ReleaseLinkedNodeTree -- frees a linked node tree: it recurses into both children, hands the tail link to the next slot before releasing it, drops the inline payload when the size byte has the high bit set, and returns the node to the pool.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 286218-286218 (f746); R1 transcribe-keep.
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

constexpr uint32_t kF746ConstFFFFFFFF = 0xFFFFFFFFu;  // 

}  // namespace

void ReleaseLinkedNodeTree(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t i0 = 0u; uint32_t i1 = 0u;
  i0 = b;
  if (i0) {
    i0 = a;
    i1 = b;
    i1 = LoadU32(ctx, i1);
    ReleaseLinkedNodeTree(ctx, i0, i1);
    i0 = a;
    i1 = b;
    i1 = LoadU32(ctx, i1 + 4u);
    ReleaseLinkedNodeTree(ctx, i0, i1);
    i0 = b;
    i0 = LoadU32(ctx, i0 + 28u);
    a = i0;
    if (i0) {
      i0 = b;
      i1 = a;
      StoreU32(ctx, i0 + 32u, i1);
      i0 = a;
      DeallocateRawBlock(ctx, i0);
    }
    i0 = b;
    i0 = LoadS8(ctx, i0 + 27u);
    i1 = kF746ConstFFFFFFFF;
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
