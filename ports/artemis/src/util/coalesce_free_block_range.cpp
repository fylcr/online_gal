// coalesce_free_block_range.cpp -- coalesces an adjacent run of free blocks into one and answers the block it produced..
//
// CoalesceFreeBlockRange -- coalesces an adjacent run of free blocks into one and answers the block it produced.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 661647-661647 (f1487); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t FindFreeBlockOnChain(EngineContext* ctx, uint32_t pool, uint32_t node, uint32_t size);
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

constexpr uint32_t kF1487ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF1487ResumeWordBytes = 4u;

}  // namespace

uint32_t CoalesceFreeBlockRange(EngineContext* ctx, uint32_t pool, uint32_t block, uint32_t size, uint32_t node, uint32_t limit) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l13 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
      l13 = 0;
  i0 = pool;
  i1 = kF1487ResumeWordBytes;
  i0 += i1;
  l9 = i0;
  i1 = block;
  i0 = i0 == i1;
  if (i0) {goto block_5;}
  i0 = block;
  i0 = LoadU8(ctx, i0 + 27u);
  l5 = i0;
  i1 = 24u;
  i0 <<= (i1 & 31);
  i1 = 24u;
  i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
  i1 = 0u;
  i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
  l6 = i0;
  i0 = limit;
  i0 = LoadU8(ctx, i0 + 11u);
  l7 = i0;
  i1 = 24u;
  i0 <<= (i1 & 31);
  i1 = 24u;
  i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
  l10 = i0;
  i1 = 0u;
  i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
  l11 = i0;
  i0 = block;
  i0 = LoadU32(ctx, i0 + 20u);
  i1 = l5;
  i2 = l6;
  i0 = i2 ? i0 : i1;
  l5 = i0;
  i1 = limit;
  i1 = LoadU32(ctx, i1 + 4u);
  i2 = l7;
  i3 = l11;
  i1 = i3 ? i1 : i2;
  l8 = i1;
  i0 = i0 < i1;
  l13 = i0;
  i0 = l5;
  i1 = l8;
  i2 = l13;
  i0 = i2 ? i0 : i1;
  l7 = i0;
  if (i0) {
    i0 = limit;
    i0 = LoadU32(ctx, i0);
    i1 = limit;
    i2 = l11;
    i0 = i2 ? i0 : i1;
    l11 = i0;
    i1 = block;
    i2 = 16u;
    i1 += i2;
    l12 = i1;
    i1 = LoadU32(ctx, i1);
    i2 = l12;
    i3 = l6;
    i1 = i3 ? i1 : i2;
    l6 = i1;
    i2 = l7;
    i0 = CompareBytes(ctx, i0, i1, i2);
    l12 = i0;
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i1 = l8;
      i0 = i0 > i1;
      if (i0) {goto block_5;}
      goto running_exit;
    }
    i0 = l12;
    i1 = kF1487ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto running_exit;}
    goto block_5;
  }
  i0 = l5;
  i1 = l8;
  i0 = i0 <= i1;
  if (i0) {goto block_3;}
  block_5:;
  i0 = block;
  i0 = LoadU32(ctx, i0);
  l7 = i0;
  i0 = block;
  node = i0;
  i0 = pool;
  i0 = LoadU32(ctx, i0);
  i1 = block;
  i0 = i0 == i1;
  if (i0) {goto block_9;}
  i0 = l7;
  if (i0) {
    i0 = l7;
    l5 = i0;
    loop_12: 
      i0 = l5;
      node = i0;
      i0 = l5;
      i0 = LoadU32(ctx, i0 + 4u);
      l5 = i0;
      if (i0) {goto loop_12;}
    goto block_10;
  }
  i0 = block;
  i1 = 8u;
  i0 += i1;
  node = i0;
  i0 = block;
  i0 = LoadU32(ctx, i0 + 8u);
  i0 = LoadU32(ctx, i0);
  i1 = block;
  i0 = i0 == i1;
  if (i0) {
    loop_14: 
      i0 = node;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i1 = 8u;
      i0 += i1;
      node = i0;
      i0 = l5;
      i0 = LoadU32(ctx, i0 + 8u);
      i0 = LoadU32(ctx, i0);
      i1 = l5;
      i0 = i0 == i1;
      if (i0) {goto loop_14;}
  }
  i0 = node;
  i0 = LoadU32(ctx, i0);
  node = i0;
  block_10:;
  i0 = limit;
  i0 = LoadU8(ctx, i0 + 11u);
  l5 = i0;
  i1 = 24u;
  i0 <<= (i1 & 31);
  i1 = 24u;
  i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
  i1 = 0u;
  i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
  l6 = i0;
  i0 = node;
  i0 = LoadU8(ctx, i0 + 27u);
  l8 = i0;
  i1 = 24u;
  i0 <<= (i1 & 31);
  i1 = 24u;
  i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
  i1 = 0u;
  i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
  l9 = i0;
  i0 = limit;
  i0 = LoadU32(ctx, i0 + 4u);
  i1 = l5;
  i2 = l6;
  i0 = i2 ? i0 : i1;
  l5 = i0;
  i1 = node;
  i1 = LoadU32(ctx, i1 + 20u);
  i2 = l8;
  i3 = l9;
  i1 = i3 ? i1 : i2;
  l8 = i1;
  i0 = i0 < i1;
  l10 = i0;
  i0 = l5;
  i1 = l8;
  i2 = l10;
  i0 = i2 ? i0 : i1;
  l10 = i0;
  if (i0) {
    i0 = node;
    i1 = 16u;
    i0 += i1;
    l11 = i0;
    i0 = LoadU32(ctx, i0);
    i1 = l11;
    i2 = l9;
    i0 = i2 ? i0 : i1;
    i1 = limit;
    i1 = LoadU32(ctx, i1);
    i2 = limit;
    i3 = l6;
    i1 = i3 ? i1 : i2;
    i2 = l10;
    i0 = CompareBytes(ctx, i0, i1, i2);
    l6 = i0;
    if (i0) {goto block_15;}
  }
  i0 = l5;
  i1 = l8;
  i0 = i0 > i1;
  if (i0) {goto block_9;}
  goto block_8;
  block_15:;
  i0 = l6;
  i1 = kF1487ConstFFFFFFFF;
  i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
  if (i0) {goto block_8;}
  block_9:;
  i0 = l7;
  i0 = !(i0);
  if (i0) {
    i0 = size;
    i1 = block;
    StoreU32(ctx, i0, i1);
    i0 = block;
    goto func_exit;
  }
  i0 = size;
  i1 = node;
  StoreU32(ctx, i0, i1);
  i0 = node;
  i1 = 4u;
  i0 += i1;
  goto func_exit;
  block_8:;
  i0 = pool;
  i1 = size;
  i2 = limit;
  i0 = FindFreeBlockOnChain(ctx, i0, i1, i2);
  goto func_exit;
  running_exit:;
  i0 = l6;
  i1 = l11;
  i2 = l7;
  i0 = CompareBytes(ctx, i0, i1, i2);
  l5 = i0;
  if (i0) {goto block_2;}
  block_3:;
  i0 = l13;
  if (i0) {goto spill_frame;}
  goto block_0;
  block_2:;
  i0 = l5;
  i1 = kF1487ConstFFFFFFFF;
  i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
  if (i0) {goto block_0;}
  spill_frame:;
  i0 = block;
  i0 = LoadU32(ctx, i0 + 4u);
  l7 = i0;
  if (i0) {
    i0 = l7;
    l5 = i0;
    loop_20: 
      i0 = l5;
      node = i0;
      i0 = l5;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      if (i0) {goto loop_20;}
    goto block_18;
  }
  i0 = block;
  i0 = LoadU32(ctx, i0 + 8u);
  node = i0;
  i0 = LoadU32(ctx, i0);
  i1 = block;
  i0 = i0 == i1;
  if (i0) {goto block_18;}
  i0 = block;
  i1 = 8u;
  i0 += i1;
  l5 = i0;
  loop_21: 
    i0 = l5;
    i0 = LoadU32(ctx, i0);
    l6 = i0;
    i1 = 8u;
    i0 += i1;
    l5 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 8u);
    node = i0;
    i0 = LoadU32(ctx, i0);
    i1 = l6;
    i0 = i0 != i1;
    if (i0) {goto loop_21;}
  block_18:;
  i0 = node;
  i1 = l9;
  i0 = i0 == i1;
  if (i0) {goto block_23;}
  i0 = node;
  i0 = LoadU8(ctx, i0 + 27u);
  l5 = i0;
  i1 = 24u;
  i0 <<= (i1 & 31);
  i1 = 24u;
  i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
  i1 = 0u;
  i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
  l6 = i0;
  i0 = l8;
  i1 = node;
  i1 = LoadU32(ctx, i1 + 20u);
  i2 = l5;
  i3 = l6;
  i1 = i3 ? i1 : i2;
  l5 = i1;
  i0 = i0 > i1;
  l9 = i0;
  i0 = l5;
  i1 = l8;
  i2 = l9;
  i0 = i2 ? i0 : i1;
  l9 = i0;
  if (i0) {
    i0 = limit;
    i0 = LoadU32(ctx, i0);
    i1 = limit;
    i2 = l10;
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = node;
    i2 = 16u;
    i1 += i2;
    l10 = i1;
    i1 = LoadU32(ctx, i1);
    i2 = l10;
    i3 = l6;
    i1 = i3 ? i1 : i2;
    i2 = l9;
    i0 = CompareBytes(ctx, i0, i1, i2);
    l6 = i0;
    if (i0) {goto block_24;}
  }
  i0 = l5;
  i1 = l8;
  i0 = i0 > i1;
  if (i0) {goto block_23;}
  goto block_22;
  block_24:;
  i0 = l6;
  i1 = kF1487ConstFFFFFFFF;
  i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
  if (i0) {goto block_22;}
  block_23:;
  i0 = l7;
  i0 = !(i0);
  if (i0) {
    i0 = size;
    i1 = block;
    StoreU32(ctx, i0, i1);
    i0 = block;
    i1 = 4u;
    i0 += i1;
    goto func_exit;
  }
  i0 = size;
  i1 = node;
  StoreU32(ctx, i0, i1);
  i0 = node;
  goto func_exit;
  block_22:;
  i0 = pool;
  i1 = size;
  i2 = limit;
  i0 = FindFreeBlockOnChain(ctx, i0, i1, i2);
  goto func_exit;
  block_0:;
  i0 = size;
  i1 = block;
  StoreU32(ctx, i0, i1);
  i0 = node;
  i1 = block;
  StoreU32(ctx, i0, i1);
  i0 = node;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
