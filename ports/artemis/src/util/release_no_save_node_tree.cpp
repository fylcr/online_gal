// release_no_save_node_tree.cpp -- releases a node tree marked no-save: the save-eligible payload is left where it is and only the node and its children are freed..
//
// ReleaseNoSaveNodeTree -- releases a node tree marked no-save: the save-eligible payload is left where it is and only the node and its children are freed.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 171389-171389 (f391); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void ReleaseNodeTree(EngineContext* ctx, uint32_t state, uint32_t node);

namespace {

constexpr uint32_t kF391ConstB5F40 = 0xB5F40u;  // 
constexpr uint32_t kF391ConstFFFFFFFF = 0xFFFFFFFFu;  // 

}  // namespace

void ReleaseNoSaveNodeTree(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t i0 = 0u; uint32_t i1 = 0u;
  i0 = b;
  if (i0) {
    i0 = a;
    i1 = b;
    i1 = LoadU32(ctx, i1);
    ReleaseNoSaveNodeTree(ctx, i0, i1);
    i0 = a;
    i1 = b;
    i1 = LoadU32(ctx, i1 + 4u);
    ReleaseNoSaveNodeTree(ctx, i0, i1);
    i0 = b;
    i1 = kF391ConstB5F40;
    StoreU32(ctx, i0 + 28u, i1);
    i0 = b;
    i0 = LoadS8(ctx, i0 + 67u);
    i1 = kF391ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0 + 56u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = b;
    i1 = 44u;
    i0 += i1;
    i1 = b;
    i1 = LoadU32(ctx, i1 + 48u);
    ReleaseNodeTree(ctx, i0, i1);
    i0 = b;
    i0 = LoadS8(ctx, i0 + 43u);
    i1 = kF391ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0 + 32u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = b;
    i0 = LoadS8(ctx, i0 + 27u);
    i1 = kF391ConstFFFFFFFF;
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
