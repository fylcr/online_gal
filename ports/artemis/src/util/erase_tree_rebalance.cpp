// erase_tree_rebalance.cpp -- the red-black rebalance-for-erase core shared by every tagged-tree erase path..
//
// EraseTreeRebalance -- the red-black rebalance-for-erase core shared by every tagged-tree erase path.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 186545-186545 (f440); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {


}  // namespace

void EraseTreeRebalance(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = b;
  l3 = i0;
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  if (i0) {
    i0 = b;
    i0 = LoadU32(ctx, i0 + 4u);
    l2 = i0;
    i0 = !(i0);
    if (i0) {goto spill_frame;}
    loop_3: 
      i0 = l2;
      l3 = i0;
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      if (i0) {goto loop_3;}
  }
  i0 = l3;
  i0 = LoadU32(ctx, i0 + 4u);
  l4 = i0;
  if (i0) {goto spill_frame;}
  i0 = 0u;
  l4 = i0;
  i0 = 1u;
  goto block_0;
  spill_frame:;
  i0 = l4;
  i1 = l3;
  i1 = LoadU32(ctx, i1 + 8u);
  StoreU32(ctx, i0 + 8u, i1);
  i0 = 0u;
  block_0:;
  l6 = i0;
  i0 = l3;
  i1 = l3;
  i1 = LoadU32(ctx, i1 + 8u);
  l5 = i1;
  i1 = LoadU32(ctx, i1);
  l2 = i1;
  i0 = i0 == i1;
  if (i0) {
    i0 = l5;
    i1 = l4;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = l3;
    i0 = i0 == i1;
    if (i0) {
      i0 = 0u;
      l2 = i0;
      i0 = l4;
      a = i0;
      goto running_exit;
    }
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 4u);
    l2 = i0;
    goto running_exit;
  }
  i0 = l5;
  i1 = l4;
  StoreU32(ctx, i0 + 4u, i1);
  running_exit:;
  i0 = l3;
  i0 = LoadU8(ctx, i0 + 12u);
  l7 = i0;
  i0 = b;
  i1 = l3;
  i0 = i0 != i1;
  if (i0) {
    i0 = l3;
    i1 = b;
    i1 = LoadU32(ctx, i1 + 8u);
    l5 = i1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l5;
    i1 = b;
    i1 = LoadU32(ctx, i1 + 8u);
    i1 = LoadU32(ctx, i1);
    i2 = b;
    i1 = i1 != i2;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    i1 = l3;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = b;
    i1 = LoadU32(ctx, i1);
    l5 = i1;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = l3;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = b;
    i1 = LoadU32(ctx, i1 + 4u);
    l5 = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l5;
    if (i0) {
      i0 = l5;
      i1 = l3;
      StoreU32(ctx, i0 + 8u, i1);
    }
    i0 = l3;
    i1 = b;
    i1 = LoadU8(ctx, i1 + 12u);
    StoreU8(ctx, i0 + 12u, i1);
    i0 = l3;
    i1 = a;
    i2 = a;
    i3 = b;
    i2 = i2 == i3;
    i0 = i2 ? i0 : i1;
    a = i0;
  }
  i0 = l7;
  i0 = !(i0);
  if (i0) {goto block_12;}
  i0 = a;
  i0 = !(i0);
  if (i0) {goto block_12;}
  i0 = l6;
  if (i0) {
    loop_14: 
      i0 = l2;
      i0 = LoadU8(ctx, i0 + 12u);
      b = i0;
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 8u);
      l3 = i0;
      i0 = LoadU32(ctx, i0);
      i1 = l2;
      i0 = i0 != i1;
      if (i0) {
        i0 = b;
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          i1 = 1u;
          StoreU8(ctx, i0 + 12u, i1);
          i0 = l3;
          i1 = 0u;
          StoreU8(ctx, i0 + 12u, i1);
          i0 = l3;
          i1 = l3;
          i1 = LoadU32(ctx, i1 + 4u);
          b = i1;
          i1 = LoadU32(ctx, i1);
          l4 = i1;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = l4;
          if (i0) {
            i0 = l4;
            i1 = l3;
            StoreU32(ctx, i0 + 8u, i1);
          }
          i0 = b;
          i1 = l3;
          i1 = LoadU32(ctx, i1 + 8u);
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 8u);
          l4 = i0;
          i0 = LoadU32(ctx, i0);
          i1 = l3;
          i0 = i0 != i1;
          i1 = 2u;
          i0 <<= (i1 & 31);
          i1 = l4;
          i0 += i1;
          i1 = b;
          StoreU32(ctx, i0, i1);
          i0 = b;
          i1 = l3;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = b;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l2;
          i1 = a;
          i2 = l2;
          i2 = LoadU32(ctx, i2);
          b = i2;
          i3 = a;
          i2 = i2 == i3;
          i0 = i2 ? i0 : i1;
          a = i0;
          i0 = b;
          i0 = LoadU32(ctx, i0 + 4u);
          l2 = i0;
        }
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        if (i0) {
          i0 = l3;
          i0 = LoadU8(ctx, i0 + 12u);
          i0 = !(i0);
          if (i0) {goto block_17;}
        }
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 4u);
        b = i0;
        if (i0) {
          i0 = b;
          i0 = LoadU8(ctx, i0 + 12u);
          i0 = !(i0);
          if (i0) {goto block_11;}
        }
        i0 = l2;
        i1 = 0u;
        StoreU8(ctx, i0 + 12u, i1);
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 8u);
        l2 = i0;
        i1 = a;
        i0 = i0 == i1;
        if (i0) {
          i0 = a;
          l2 = i0;
          goto block_23;
        }
        i0 = l2;
        i0 = LoadU8(ctx, i0 + 12u);
        if (i0) {goto block_15;}
        block_23:;
        i0 = l2;
        i1 = 1u;
        StoreU8(ctx, i0 + 12u, i1);
        goto func_exit;
        block_17:;
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 4u);
        b = i0;
        if (i0) {goto block_11;}
        goto block_10;
      }
      i0 = b;
      if (i0) {
        i0 = l2;
        b = i0;
        goto block_25;
      }
      i0 = l2;
      i1 = 1u;
      StoreU8(ctx, i0 + 12u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 12u, i1);
      i0 = l3;
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 4u);
      b = i1;
      StoreU32(ctx, i0, i1);
      i0 = b;
      if (i0) {
        i0 = b;
        i1 = l3;
        StoreU32(ctx, i0 + 8u, i1);
      }
      i0 = l2;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 8u);
      l4 = i0;
      i0 = LoadU32(ctx, i0);
      i1 = l3;
      i0 = i0 == i1;
      if (i0) {
        i0 = l4;
        i1 = l2;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i0 = LoadU32(ctx, i0);
        b = i0;
        goto block_28;
      }
      i0 = l4;
      i1 = l2;
      StoreU32(ctx, i0 + 4u, i1);
      block_28:;
      i0 = l2;
      i1 = l3;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l3;
      i1 = l2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = a;
      i2 = a;
      i3 = l3;
      i2 = i2 == i3;
      i0 = i2 ? i0 : i1;
      a = i0;
      block_25:;
      i0 = b;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = !(i0);
      if (i0) {goto block_31;}
      i0 = l3;
      i0 = LoadU8(ctx, i0 + 12u);
      if (i0) {goto block_31;}
      i0 = b;
      l2 = i0;
      goto block_30;
      block_31:;
      i0 = b;
      i0 = LoadU32(ctx, i0 + 4u);
      l2 = i0;
      if (i0) {
        i0 = l2;
        i0 = LoadU8(ctx, i0 + 12u);
        i0 = !(i0);
        if (i0) {goto block_32;}
      }
      i0 = b;
      i1 = 0u;
      StoreU8(ctx, i0 + 12u, i1);
      i0 = b;
      i0 = LoadU32(ctx, i0 + 8u);
      l2 = i0;
      i1 = a;
      i0 = i0 != i1;
      if (i0) {
        i0 = l2;
        i0 = LoadU8(ctx, i0 + 12u);
        if (i0) {goto block_15;}
      }
      i0 = l2;
      i1 = 1u;
      StoreU8(ctx, i0 + 12u, i1);
      goto func_exit;
      block_32:;
      i0 = l3;
      if (i0) {
        i0 = l3;
        i0 = LoadU8(ctx, i0 + 12u);
        i0 = !(i0);
        if (i0) {
          i0 = b;
          l2 = i0;
          goto block_30;
        }
        i0 = b;
        i0 = LoadU32(ctx, i0 + 4u);
        l2 = i0;
      }
      i0 = l2;
      i1 = 1u;
      StoreU8(ctx, i0 + 12u, i1);
      i0 = b;
      i1 = 0u;
      StoreU8(ctx, i0 + 12u, i1);
      i0 = b;
      i1 = l2;
      i1 = LoadU32(ctx, i1);
      a = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = a;
      if (i0) {
        i0 = a;
        i1 = b;
        StoreU32(ctx, i0 + 8u, i1);
      }
      i0 = l2;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = b;
      i0 = LoadU32(ctx, i0 + 8u);
      a = i0;
      i0 = LoadU32(ctx, i0);
      i1 = b;
      i0 = i0 != i1;
      i1 = 2u;
      i0 <<= (i1 & 31);
      l3 = i0;
      i0 = a;
      i1 = l3;
      i0 += i1;
      i1 = l2;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = b;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = l2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = b;
      l3 = i0;
      block_30:;
      i0 = l2;
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 8u);
      a = i1;
      i1 = LoadU8(ctx, i1 + 12u);
      StoreU8(ctx, i0 + 12u, i1);
      i0 = a;
      i1 = 1u;
      StoreU8(ctx, i0 + 12u, i1);
      i0 = l3;
      i1 = 1u;
      StoreU8(ctx, i0 + 12u, i1);
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1);
      b = i1;
      i1 = LoadU32(ctx, i1 + 4u);
      l2 = i1;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      if (i0) {
        i0 = l2;
        i1 = a;
        StoreU32(ctx, i0 + 8u, i1);
      }
      i0 = b;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = a;
      i0 = LoadU32(ctx, i0 + 8u);
      l2 = i0;
      i0 = LoadU32(ctx, i0);
      i1 = a;
      i0 = i0 != i1;
      i1 = 2u;
      i0 <<= (i1 & 31);
      l3 = i0;
      i0 = l2;
      i1 = l3;
      i0 += i1;
      i1 = b;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = a;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = a;
      i1 = b;
      StoreU32(ctx, i0 + 8u, i1);
      goto func_exit;
      block_15:;
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 8u);
      b = i0;
      i0 = LoadU32(ctx, i0);
      i1 = l2;
      i0 = i0 == i1;
      i1 = 2u;
      i0 <<= (i1 & 31);
      l2 = i0;
      i0 = b;
      i1 = l2;
      i0 += i1;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      goto loop_14;
    TrapUnreachable();
  }
  i0 = l4;
  i1 = 1u;
  StoreU8(ctx, i0 + 12u, i1);
  block_12:;
  goto func_exit;
  block_11:;
  i0 = b;
  i0 = LoadU8(ctx, i0 + 12u);
  if (i0) {goto block_10;}
  i0 = l2;
  l3 = i0;
  goto block_9;
  block_10:;
  i0 = l3;
  i1 = 1u;
  StoreU8(ctx, i0 + 12u, i1);
  i0 = l2;
  i1 = 0u;
  StoreU8(ctx, i0 + 12u, i1);
  i0 = l2;
  i1 = l3;
  i1 = LoadU32(ctx, i1 + 4u);
  a = i1;
  StoreU32(ctx, i0, i1);
  i0 = a;
  if (i0) {
    i0 = a;
    i1 = l2;
    StoreU32(ctx, i0 + 8u, i1);
  }
  i0 = l3;
  i1 = l2;
  i1 = LoadU32(ctx, i1 + 8u);
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l2;
  i1 = l2;
  i1 = LoadU32(ctx, i1 + 8u);
  a = i1;
  i1 = LoadU32(ctx, i1);
  i0 = i0 != i1;
  i1 = 2u;
  i0 <<= (i1 & 31);
  b = i0;
  i0 = a;
  i1 = b;
  i0 += i1;
  i1 = l3;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = l2;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l2;
  i1 = l3;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l2;
  b = i0;
  block_9:;
  i0 = l3;
  i1 = l3;
  i1 = LoadU32(ctx, i1 + 8u);
  a = i1;
  i1 = LoadU8(ctx, i1 + 12u);
  StoreU8(ctx, i0 + 12u, i1);
  i0 = a;
  i1 = 1u;
  StoreU8(ctx, i0 + 12u, i1);
  i0 = b;
  i1 = 1u;
  StoreU8(ctx, i0 + 12u, i1);
  i0 = a;
  i1 = a;
  i1 = LoadU32(ctx, i1 + 4u);
  b = i1;
  i1 = LoadU32(ctx, i1);
  l2 = i1;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l2;
  if (i0) {
    i0 = l2;
    i1 = a;
    StoreU32(ctx, i0 + 8u, i1);
  }
  i0 = b;
  i1 = a;
  i1 = LoadU32(ctx, i1 + 8u);
  StoreU32(ctx, i0 + 8u, i1);
  i0 = a;
  i0 = LoadU32(ctx, i0 + 8u);
  l2 = i0;
  i0 = LoadU32(ctx, i0);
  i1 = a;
  i0 = i0 != i1;
  i1 = 2u;
  i0 <<= (i1 & 31);
  l3 = i0;
  i0 = l2;
  i1 = l3;
  i0 += i1;
  i1 = b;
  StoreU32(ctx, i0, i1);
  i0 = b;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = a;
  i1 = b;
  StoreU32(ctx, i0 + 8u, i1);
  func_exit:;
  LeaveFrame();
}
}
