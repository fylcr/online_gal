// unlink_ordered_chain_node.cpp -- unlinks a node from an ordered chain: the neighbours are re-linked and the node's own links are cleared..
//
// UnlinkOrderedChainNode -- unlinks a node from an ordered chain: the neighbours are re-linked and the node's own links are cleared.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 195115-195115 (f471); R1 transcribe-keep.
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

constexpr uint32_t kF471ConstFFFFFFFF = 0xFFFFFFFFu;  // 

}  // namespace

void UnlinkOrderedChainNode(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = a;
  i0 = LoadU32(ctx, i0 + 16u);
  l3 = i0;
  i1 = 341u;
  i0 = DIV_U(i0, i1);
  l1 = i0;
  i0 = a;
  i0 = LoadU32(ctx, i0 + 4u);
  l2 = i0;
  i1 = a;
  i1 = LoadU32(ctx, i1 + 8u);
  l5 = i1;
  i0 = i0 == i1;
  if (i0) {
    i0 = a;
    i1 = 20u;
    i0 += i1;
    l6 = i0;
    goto block_0;
  }
  i0 = a;
  i1 = 20u;
  i0 += i1;
  l6 = i0;
  i0 = l1;
  i1 = 2u;
  i0 <<= (i1 & 31);
  i1 = l2;
  i0 += i1;
  l4 = i0;
  i0 = LoadU32(ctx, i0);
  i1 = l3;
  i2 = l1;
  i3 = 341u;
  i2 *= i3;
  i1 -= i2;
  i2 = 12u;
  i1 *= i2;
  i0 += i1;
  l1 = i0;
  i0 = a;
  i0 = LoadU32(ctx, i0 + 20u);
  i1 = l3;
  i0 += i1;
  l3 = i0;
  i1 = 341u;
  i0 = DIV_U(i0, i1);
  l7 = i0;
  i1 = 2u;
  i0 <<= (i1 & 31);
  i1 = l2;
  i0 += i1;
  i0 = LoadU32(ctx, i0);
  l8 = i0;
  i0 = l8;
  i1 = l3;
  i2 = l7;
  i3 = 341u;
  i2 *= i3;
  i1 -= i2;
  i2 = 12u;
  i1 *= i2;
  i0 += i1;
  l3 = i0;
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto block_0;}
  loop_2: 
    i0 = l1;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF471ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l1;
    i1 = 12u;
    i0 += i1;
    l1 = i0;
    i1 = l4;
    i1 = LoadU32(ctx, i1);
    i0 -= i1;
    i1 = 4092u;
    i0 = i0 == i1;
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 4u);
      l1 = i0;
      i0 = l4;
      i1 = 4u;
      i0 += i1;
      l4 = i0;
    }
    i0 = l1;
    i1 = l3;
    i0 = i0 != i1;
    if (i0) {goto loop_2;}
  i0 = a;
  i0 = LoadU32(ctx, i0 + 4u);
  l2 = i0;
  i0 = a;
  i0 = LoadU32(ctx, i0 + 8u);
  l5 = i0;
  block_0:;
  i0 = l6;
  i1 = 0u;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = l2;
  i0 -= i1;
  i1 = 2u;
  i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
  l1 = i0;
  i1 = 2u;
  i0 = i0 > i1;
  if (i0) {
    loop_6: 
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 4u);
      i2 = 4u;
      i1 += i2;
      l2 = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = a;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = l2;
      i0 -= i1;
      i1 = 2u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l1 = i0;
      i1 = 2u;
      i0 = i0 > i1;
      if (i0) {goto loop_6;}
  }
  i0 = 170u;
  l2 = i0;
  i0 = l1;
  i1 = 1u;
  i0 -= i1;
  switch (i0) {
    case 0: goto block_8;
    case 1: goto block_9;
    default: goto block_7;
  }
  block_9:;
  i0 = 341u;
  l2 = i0;
  block_8:;
  i0 = a;
  i1 = l2;
  StoreU32(ctx, i0 + 16u, i1);
  block_7:;
  LeaveFrame();
}
}
