// unlink_list_node.cpp -- splices a node out of a doubly linked chain in either order, walking back through the marking bytes the list keeps beside every link until the survivor is the node it started from..
//
// UnlinkListNode -- splices a node out of a doubly linked chain in either order, walking back through the marking bytes the list keeps beside every link until the survivor is the node it started from.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 115482-115743 (f215); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {


}  // namespace

void UnlinkListNode(EngineContext* ctx, uint32_t node, uint32_t victim) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  i0 = victim;
  i1 = node;
  i2 = victim;
  i1 = i1 == i2;
  l3 = i1;
  StoreU8(ctx, i0 + 12u, i1);
  i0 = l3;
  if (i0) {goto block_0;}
  loop_1: 
    i0 = victim;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i0 = LoadU8(ctx, i0 + 12u);
    if (i0) {goto block_0;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i1 = l3;
    i0 = i0 == i1;
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 4u);
      l4 = i0;
      i0 = !(i0);
      if (i0) {goto running_exit;}
      i0 = l4;
      i0 = LoadU8(ctx, i0 + 12u);
      if (i0) {goto running_exit;}
      goto block_2;
      running_exit:;
      i0 = victim;
      i1 = l3;
      i1 = LoadU32(ctx, i1);
      i0 = i0 == i1;
      if (i0) {
        i0 = l3;
        victim = i0;
        goto block_5;
      }
      i0 = l3;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 4u);
      victim = i1;
      i1 = LoadU32(ctx, i1);
      node = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = victim;
      i1 = node;
      if (i1) {
        i1 = node;
        i2 = l3;
        StoreU32(ctx, i1 + 8u, i2);
        i1 = l3;
        i1 = LoadU32(ctx, i1 + 8u);
      } else {
        i1 = l2;
      }
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 8u);
      node = i1;
      i1 = LoadU32(ctx, i1);
      i0 = i0 != i1;
      i1 = 2u;
      i0 <<= (i1 & 31);
      l2 = i0;
      i0 = node;
      i1 = l2;
      i0 += i1;
      i1 = victim;
      StoreU32(ctx, i0, i1);
      i0 = victim;
      i1 = l3;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = victim;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = victim;
      i0 = LoadU32(ctx, i0 + 8u);
      l2 = i0;
      block_5:;
      i0 = victim;
      i1 = 1u;
      StoreU8(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = 0u;
      StoreU8(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = l2;
      i1 = LoadU32(ctx, i1);
      node = i1;
      i1 = LoadU32(ctx, i1 + 4u);
      victim = i1;
      StoreU32(ctx, i0, i1);
      i0 = victim;
      if (i0) {
        i0 = victim;
        i1 = l2;
        StoreU32(ctx, i0 + 8u, i1);
      }
      i0 = node;
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 8u);
      victim = i0;
      i0 = LoadU32(ctx, i0);
      i1 = l2;
      i0 = i0 != i1;
      i1 = 2u;
      i0 <<= (i1 & 31);
      l3 = i0;
      i0 = victim;
      i1 = l3;
      i0 += i1;
      i1 = node;
      StoreU32(ctx, i0, i1);
      i0 = node;
      i1 = l2;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = node;
      StoreU32(ctx, i0 + 8u, i1);
      goto func_exit;
    }
    i0 = l4;
    i0 = !(i0);
    if (i0) {goto block_9;}
    i0 = l4;
    i0 = LoadU8(ctx, i0 + 12u);
    if (i0) {goto block_9;}
    goto block_2;
    block_9:;
    i0 = victim;
    i1 = l3;
    i1 = LoadU32(ctx, i1);
    i0 = i0 != i1;
    if (i0) {
      i0 = l3;
      victim = i0;
      goto block_10;
    }
    i0 = l3;
    i1 = victim;
    i1 = LoadU32(ctx, i1 + 4u);
    node = i1;
    StoreU32(ctx, i0, i1);
    i0 = victim;
    i1 = node;
    if (i1) {
      i1 = node;
      i2 = l3;
      StoreU32(ctx, i1 + 8u, i2);
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 8u);
    } else {
      i1 = l2;
    }
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 8u);
    node = i1;
    i1 = LoadU32(ctx, i1);
    i0 = i0 != i1;
    i1 = 2u;
    i0 <<= (i1 & 31);
    l2 = i0;
    i0 = node;
    i1 = l2;
    i0 += i1;
    i1 = victim;
    StoreU32(ctx, i0, i1);
    i0 = victim;
    i1 = l3;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l3;
    i1 = victim;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = victim;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    block_10:;
    i0 = victim;
    i1 = 1u;
    StoreU8(ctx, i0 + 12u, i1);
    i0 = l2;
    i1 = 0u;
    StoreU8(ctx, i0 + 12u, i1);
    i0 = l2;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 4u);
    node = i1;
    i1 = LoadU32(ctx, i1);
    victim = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = victim;
    if (i0) {
      i0 = victim;
      i1 = l2;
      StoreU32(ctx, i0 + 8u, i1);
    }
    i0 = node;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 8u);
    victim = i0;
    i0 = LoadU32(ctx, i0);
    i1 = l2;
    i0 = i0 != i1;
    i1 = 2u;
    i0 <<= (i1 & 31);
    l3 = i0;
    i0 = victim;
    i1 = l3;
    i0 += i1;
    i1 = node;
    StoreU32(ctx, i0, i1);
    i0 = node;
    i1 = l2;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = node;
    StoreU32(ctx, i0 + 8u, i1);
    goto block_0;
    block_2:;
    i0 = l3;
    i1 = 1u;
    StoreU8(ctx, i0 + 12u, i1);
    i0 = l2;
    i1 = node;
    i2 = l2;
    i1 = i1 == i2;
    StoreU8(ctx, i0 + 12u, i1);
    i0 = l4;
    i1 = 12u;
    i0 += i1;
    i1 = 1u;
    StoreU8(ctx, i0, i1);
    i0 = l2;
    victim = i0;
    i1 = node;
    i0 = i0 != i1;
    if (i0) {goto loop_1;}
  block_0:;
  func_exit:;
  LeaveFrame();
}
}
