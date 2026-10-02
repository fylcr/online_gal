// release_shallow_node_pair.cpp -- releases a shallow node pair: only the two node headers are freed, their payload being owned elsewhere..
//
// ReleaseShallowNodePair -- releases a shallow node pair: only the two node headers are freed, their payload being owned elsewhere.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 199876-199876 (f491); R1 transcribe-keep.
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

constexpr uint32_t kF491ConstB5F40 = 0xB5F40u;  // 
constexpr uint32_t kF491ConstFFFFFFFF = 0xFFFFFFFFu;  // 

}  // namespace

void ReleaseShallowNodePair(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t i0 = 0u; uint32_t i1 = 0u;
  i0 = b;
  if (i0) {
    i0 = a;
    i1 = b;
    i1 = LoadU32(ctx, i1);
    ReleaseShallowNodePair(ctx, i0, i1);
    i0 = a;
    i1 = b;
    i1 = LoadU32(ctx, i1 + 4u);
    ReleaseShallowNodePair(ctx, i0, i1);
    i0 = b;
    i1 = kF491ConstB5F40;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = b;
    i0 = LoadS8(ctx, i0 + 59u);
    i1 = kF491ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0 + 48u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = b;
    i1 = 36u;
    i0 += i1;
    i1 = b;
    i1 = LoadU32(ctx, i1 + 40u);
    ReleaseNodeTree(ctx, i0, i1);
    i0 = b;
    i0 = LoadS8(ctx, i0 + 35u);
    i1 = kF491ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0 + 24u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = b;
    DeallocateRawBlock(ctx, i0);
  }
  LeaveFrame();
}
}
