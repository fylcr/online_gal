// reset_node_chain_flags.cpp -- resets node chain flags: the flags of every node on the chain are cleared..
//
// ResetNodeChainFlags -- resets node chain flags: the flags of every node on the chain are cleared.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 2733082-2733082 (f5624); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void ClearFlagsRecursive(EngineContext* ctx, uint32_t state, uint32_t node);

namespace {


}  // namespace

void ResetNodeChainFlags(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = b;
  i1 = a;
  i1 = LoadU32(ctx, i1 + 16u);
  l2 = i1;
  i1 = LoadU32(ctx, i1 + 28u);
  StoreU32(ctx, i0, i1);
  i0 = l2;
  i1 = b;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = b;
  i0 = LoadU8(ctx, i0 + 5u);
  l3 = i0;
  i1 = 7u;
  i0 &= i1;
  if (i0) {goto block_0;}
  i0 = l2;
  i0 = LoadU8(ctx, i0 + 21u);
  i1 = 1u;
  i0 = i0 == i1;
  if (i0) {
    i0 = b;
    i1 = l3;
    i2 = 4u;
    i1 |= i2;
    StoreU8(ctx, i0 + 5u, i1);
    i0 = b;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = 4u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {goto block_0;}
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU8(ctx, i0 + 5u);
    i1 = 3u;
    i0 &= i1;
    i0 = !(i0);
    if (i0) {goto block_0;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
    a = i0;
    i0 = LoadU8(ctx, i0 + 21u);
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {
      i0 = a;
      i1 = l2;
      ClearFlagsRecursive(ctx, i0, i1);
      goto func_exit;
    }
    i0 = b;
    i1 = a;
    i1 = LoadU8(ctx, i1 + 20u);
    i2 = 3u;
    i1 &= i2;
    i2 = l3;
    i3 = 248u;
    i2 &= i3;
    i1 |= i2;
    StoreU8(ctx, i0 + 5u, i1);
    goto func_exit;
  }
  i0 = b;
  i1 = l2;
  i1 = LoadU8(ctx, i1 + 20u);
  i2 = 3u;
  i1 &= i2;
  i2 = l3;
  i3 = 248u;
  i2 &= i3;
  i1 |= i2;
  StoreU8(ctx, i0 + 5u, i1);
  block_0:;
  func_exit:;
  LeaveFrame();
}
}
