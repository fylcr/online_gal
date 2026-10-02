// clear_flags_recursive.cpp -- clears the low flag bits on one node and walks its typed children, recursing through the branch shapes..
//
// ClearFlagsRecursive -- clears the low flag bits on one node and walks its typed children, recursing through the branch shapes.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 136350-136350 (f303); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {


}  // namespace

void ClearFlagsRecursive(EngineContext* ctx, uint32_t state, uint32_t node) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  i0 = node;
  i0 = LoadU8(ctx, i0 + 5u);
  l2 = i0;
  loop_6: 
    i0 = node;
    i1 = l2;
    i2 = 252u;
    i1 &= i2;
    l2 = i1;
    StoreU8(ctx, i0 + 5u, i1);
    i0 = node;
    i0 = LoadU8(ctx, i0 + 4u);
    l3 = i0;
    i1 = 7u;
    i0 = i0 != i1;
    if (i0) {
      i0 = l3;
      i1 = 5u;
      i0 -= i1;
      switch (i0) {
        case 0: goto running_exit;
        case 1: goto block_5;
        case 2: goto block_0;
        case 3: goto block_3;
        case 4: goto block_2;
        case 5: goto block_7;
        default: goto block_0;
      }
    }
    i0 = node;
    i1 = l2;
    i2 = 4u;
    i1 |= i2;
    StoreU8(ctx, i0 + 5u, i1);
    i0 = node;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = !(i0);
    if (i0) {goto block_9;}
    i0 = l2;
    i0 = LoadU8(ctx, i0 + 5u);
    i1 = 3u;
    i0 &= i1;
    i0 = !(i0);
    if (i0) {goto block_9;}
    i0 = state;
    i1 = l2;
    ClearFlagsRecursive(ctx, i0, i1);
    block_9:;
    i0 = node;
    i0 = LoadU32(ctx, i0 + 12u);
    node = i0;
    i0 = LoadU8(ctx, i0 + 5u);
    l2 = i0;
    i1 = 3u;
    i0 &= i1;
    if (i0) {goto loop_6;}
    goto block_0;
    block_7:;
  i0 = node;
  i0 = LoadU32(ctx, i0 + 8u);
  l2 = i0;
  i0 = LoadU32(ctx, i0 + 8u);
  i1 = 4u;
  i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
  if (i0) {goto block_10;}
  i0 = l2;
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i0 = LoadU8(ctx, i0 + 5u);
  i1 = 3u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto block_10;}
  i0 = state;
  i1 = l3;
  ClearFlagsRecursive(ctx, i0, i1);
  i0 = node;
  i0 = LoadU32(ctx, i0 + 8u);
  l2 = i0;
  block_10:;
  i0 = l2;
  i1 = node;
  i2 = 16u;
  i1 += i2;
  i0 = i0 != i1;
  if (i0) {goto block_0;}
  i0 = node;
  i1 = node;
  i1 = LoadU8(ctx, i1 + 5u);
  i2 = 4u;
  i1 |= i2;
  StoreU8(ctx, i0 + 5u, i1);
  goto func_exit;
  block_5:;
  i0 = node;
  i1 = state;
  i1 = LoadU32(ctx, i1 + 36u);
  StoreU32(ctx, i0 + 8u, i1);
  goto spill_frame;
  running_exit:;
  i0 = node;
  i1 = state;
  i1 = LoadU32(ctx, i1 + 36u);
  StoreU32(ctx, i0 + 24u, i1);
  goto spill_frame;
  block_3:;
  i0 = node;
  i1 = state;
  i1 = LoadU32(ctx, i1 + 36u);
  StoreU32(ctx, i0 + 108u, i1);
  goto spill_frame;
  block_2:;
  i0 = node;
  i1 = state;
  i1 = LoadU32(ctx, i1 + 36u);
  StoreU32(ctx, i0 + 68u, i1);
  spill_frame:;
  i0 = state;
  i1 = node;
  StoreU32(ctx, i0 + 36u, i1);
  block_0:;
  func_exit:;
  LeaveFrame();
}
}
