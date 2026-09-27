// find_free_block_on_chain.cpp -- finds a free block of at least size bytes on the pool free chain..
//
// FindFreeBlockOnChain -- finds a free block of at least size bytes on the pool free chain.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 661490-661490 (f1486); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

constexpr uint32_t kF1486ConstFFFFFFFF = 0xFFFFFFFFu;  // 

}  // namespace

uint32_t FindFreeBlockOnChain(EngineContext* ctx, uint32_t pool, uint32_t node, uint32_t size) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = pool;
  i1 = 4u;
  i0 += i1;
  l3 = i0;
  i0 = pool;
  i0 = LoadU32(ctx, i0 + 4u);
  pool = i0;
  if (i0) {
    i0 = size;
    i0 = LoadU32(ctx, i0);
    i1 = size;
    i2 = size;
    i2 = LoadU8(ctx, i2 + 11u);
    l4 = i2;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    l5 = i2;
    i0 = i2 ? i0 : i1;
    l8 = i0;
    i0 = size;
    i0 = LoadU32(ctx, i0 + 4u);
    i1 = l4;
    i2 = l5;
    i0 = i2 ? i0 : i1;
    l4 = i0;
    loop_2: 
      i0 = pool;
      i0 = LoadU8(ctx, i0 + 27u);
      size = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l6 = i0;
      i0 = l4;
      i1 = pool;
      i1 = LoadU32(ctx, i1 + 20u);
      i2 = size;
      i3 = l6;
      i1 = i3 ? i1 : i2;
      size = i1;
      i0 = i0 > i1;
      l9 = i0;
      i0 = size;
      i1 = l4;
      i2 = l9;
      i0 = i2 ? i0 : i1;
      l5 = i0;
      if (i0) {
        i0 = l8;
        i1 = pool;
        i2 = 16u;
        i1 += i2;
        l7 = i1;
        i1 = LoadU32(ctx, i1);
        i2 = l7;
        i3 = l6;
        i1 = i3 ? i1 : i2;
        l6 = i1;
        i2 = l5;
        i0 = CompareBytes(ctx, i0, i1, i2);
        l7 = i0;
        i0 = !(i0);
        if (i0) {
          i0 = size;
          i1 = l4;
          i0 = i0 > i1;
          if (i0) {goto block_8;}
          goto block_7;
        }
        i0 = l7;
        i1 = kF1486ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {goto block_7;}
        goto block_8;
      }
      i0 = size;
      i1 = l4;
      i0 = i0 <= i1;
      if (i0) {goto block_6;}
      block_8:;
      i0 = pool;
      i0 = LoadU32(ctx, i0);
      size = i0;
      if (i0) {goto block_3;}
      i0 = node;
      i1 = pool;
      StoreU32(ctx, i0, i1);
      i0 = pool;
      goto func_exit;
      block_7:;
      i0 = l6;
      i1 = l8;
      i2 = l5;
      i0 = CompareBytes(ctx, i0, i1, i2);
      size = i0;
      if (i0) {goto block_5;}
      block_6:;
      i0 = l9;
      if (i0) {goto running_exit;}
      goto block_0;
      block_5:;
      i0 = size;
      i1 = kF1486ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      if (i0) {goto block_0;}
      running_exit:;
      i0 = pool;
      i1 = 4u;
      i0 += i1;
      l3 = i0;
      i0 = pool;
      i0 = LoadU32(ctx, i0 + 4u);
      size = i0;
      i0 = !(i0);
      if (i0) {goto block_0;}
      i0 = l3;
      pool = i0;
      block_3:;
      i0 = pool;
      l3 = i0;
      i0 = size;
      pool = i0;
      goto loop_2;
    TrapUnreachable();
  }
  i0 = node;
  i1 = l3;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  goto func_exit;
  block_0:;
  i0 = node;
  i1 = pool;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
