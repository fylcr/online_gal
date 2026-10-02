// erase_tagged_tree_entry.cpp -- unlinks one tagged-tree node, rebalances, destroys the CScriptBlock value it held and answers the successor..
//
// EraseTaggedTreeEntry -- unlinks one tagged-tree node, rebalances, destroys the CScriptBlock value it held and answers the successor.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 328510-328510 (f852); R1 transcribe-keep.
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
void EraseTreeRebalance(EngineContext* ctx, uint32_t a, uint32_t b);

namespace {

constexpr uint32_t kF852ConstB5F40 = 0xB5F40u;  // 
constexpr uint32_t kF852ConstFFFFFFFF = 0xFFFFFFFFu;  // 

}  // namespace

uint32_t EraseTaggedTreeEntry(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  i0 = b;
  i0 = LoadU32(ctx, i0 + 4u);
  l2 = i0;
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    i1 = b;
    i0 = i0 == i1;
    if (i0) {goto block_0;}
    i0 = b;
    i1 = 8u;
    i0 += i1;
    l4 = i0;
    loop_2: 
      i0 = l4;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i1 = 8u;
      i0 += i1;
      l4 = i0;
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 8u);
      l3 = i0;
      i0 = LoadU32(ctx, i0);
      i1 = l2;
      i0 = i0 != i1;
      if (i0) {goto loop_2;}
    goto block_0;
  }
  loop_3: 
    i0 = l2;
    l3 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    if (i0) {goto loop_3;}
  block_0:;
  i0 = a;
  i0 = LoadU32(ctx, i0);
  i1 = b;
  i0 = i0 == i1;
  if (i0) {
    i0 = a;
    i1 = l3;
    StoreU32(ctx, i0, i1);
  }
  i0 = a;
  i1 = a;
  i1 = LoadU32(ctx, i1 + 8u);
  i2 = 1u;
  i1 -= i2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = a;
  i0 = LoadU32(ctx, i0 + 4u);
  i1 = b;
  EraseTreeRebalance(ctx, i0, i1);
  i0 = b;
  i1 = kF852ConstB5F40;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = b;
  i0 = LoadS8(ctx, i0 + 59u);
  i1 = kF852ConstFFFFFFFF;
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
  i1 = kF852ConstFFFFFFFF;
  i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
  if (i0) {
    i0 = b;
    i0 = LoadU32(ctx, i0 + 24u);
    DeallocateRawBlock(ctx, i0);
  }
  i0 = b;
  DeallocateRawBlock(ctx, i0);
  i0 = l3;
  LeaveFrame();
  return i0;
}
}
