// reclaim_free_block.cpp -- returns one block to the free list, coalesging with the neighbours when they are free too..
//
// ReclaimFreeBlock -- returns one block to the free list, coalesging with the neighbours when they are free too.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 526678-526678 (f1212); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF1212Const3C000 = 0x3C000u;  // 
constexpr uint32_t kF1212Const7F000 = 0x7F000u;  // 
constexpr uint32_t kF1212ConstD1370 = 0xD1370u;  // 
constexpr uint32_t kF1212ConstD1374 = 0xD1374u;  // 
constexpr uint32_t kF1212ConstD1378 = 0xD1378u;  // 
constexpr uint32_t kF1212ConstD137C = 0xD137Cu;  // 
constexpr uint32_t kF1212ConstD1380 = 0xD1380u;  // 
constexpr uint32_t kF1212ConstD1384 = 0xD1384u;  // 
constexpr uint32_t kF1212ConstD1388 = 0xD1388u;  // 
constexpr uint32_t kF1212ConstD1398 = 0xD1398u;  // 
constexpr uint32_t kF1212ConstD14A0 = 0xD14A0u;  // 
constexpr uint32_t kF1212ConstFFF00 = 0xFFF00u;  // 
constexpr uint32_t kF1212ConstFFFFFF = 0xFFFFFFu;  // 
constexpr uint32_t kF1212ConstFFFFFFF8 = 0xFFFFFFF8u;  // 
constexpr uint32_t kF1212ConstFFFFFFFE = 0xFFFFFFFEu;  // 

}  // namespace

void ReclaimFreeBlock(EngineContext* ctx, uint32_t state, uint32_t block) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  uint64_t j1 = 0;
  i0 = state;
  i1 = block;
  i0 += i1;
  l5 = i0;
  i0 = state;
  i0 = LoadU32(ctx, i0 + 4u);
  l3 = i0;
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto spill_frame;}
  i0 = l3;
  i1 = 3u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto block_0;}
  i0 = state;
  i0 = LoadU32(ctx, i0);
  l2 = i0;
  i1 = block;
  i0 += i1;
  block = i0;
  i0 = state;
  i1 = l2;
  i0 -= i1;
  state = i0;
  i1 = kF1212ConstD1384;
  i1 = LoadU32(ctx, i1);
  i0 = i0 != i1;
  if (i0) {
    i0 = kF1212ConstD1380;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = l2;
    i1 = 255u;
    i0 = i0 <= i1;
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 8u);
      l4 = i0;
      i1 = l2;
      i2 = 3u;
      i1 >>= (i2 & 31);
      l2 = i1;
      i2 = 3u;
      i1 <<= (i2 & 31);
      i2 = kF1212ConstD1398;
      i1 += i2;
      i0 = i0 != i1;
      i0 = state;
      i0 = LoadU32(ctx, i0 + 12u);
      l3 = i0;
      i1 = l4;
      i0 = i0 == i1;
      if (i0) {
        i0 = kF1212ConstD1370;
        i1 = kF1212ConstD1370;
        i1 = LoadU32(ctx, i1);
        i2 = kF1212ConstFFFFFFFE;
        i3 = l2;
        i2 = I32_ROTL(i2, i3);
        i1 &= i2;
        StoreU32(ctx, i0, i1);
        goto spill_frame;
      }
      i0 = l4;
      i1 = l3;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l3;
      i1 = l4;
      StoreU32(ctx, i0 + 8u, i1);
      goto spill_frame;
    }
    i0 = state;
    i0 = LoadU32(ctx, i0 + 24u);
    l6 = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i1 = state;
    i0 = i0 != i1;
    if (i0) {
      i0 = l4;
      i1 = state;
      i1 = LoadU32(ctx, i1 + 8u);
      l2 = i1;
      i0 = i0 <= i1;
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 12u);
      }
      i0 = l2;
      i1 = l3;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l3;
      i1 = l2;
      StoreU32(ctx, i0 + 8u, i1);
      goto block_5;
    }
    i0 = state;
    i1 = 20u;
    i0 += i1;
    l2 = i0;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    if (i0) {goto block_8;}
    i0 = state;
    i1 = 16u;
    i0 += i1;
    l2 = i0;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    if (i0) {goto block_8;}
    i0 = 0u;
    l3 = i0;
    goto block_5;
    block_8:;
    loop_9: 
      i0 = l2;
      l7 = i0;
      i0 = l4;
      l3 = i0;
      i1 = 20u;
      i0 += i1;
      l2 = i0;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      if (i0) {goto loop_9;}
      i0 = l3;
      i1 = 16u;
      i0 += i1;
      l2 = i0;
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 16u);
      l4 = i0;
      if (i0) {goto loop_9;}
    i0 = l7;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    block_5:;
    i0 = l6;
    i0 = !(i0);
    if (i0) {goto spill_frame;}
    i0 = state;
    i0 = LoadU32(ctx, i0 + 28u);
    l2 = i0;
    i1 = 2u;
    i0 <<= (i1 & 31);
    i1 = kF1212ConstD14A0;
    i0 += i1;
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    i1 = state;
    i0 = i0 == i1;
    if (i0) {
      i0 = l4;
      i1 = l3;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      if (i0) {goto block_10;}
      i0 = kF1212ConstD1374;
      i1 = kF1212ConstD1374;
      i1 = LoadU32(ctx, i1);
      i2 = kF1212ConstFFFFFFFE;
      i3 = l2;
      i2 = I32_ROTL(i2, i3);
      i1 &= i2;
      StoreU32(ctx, i0, i1);
      goto spill_frame;
    }
    i0 = l6;
    i1 = 16u;
    i2 = 20u;
    i3 = l6;
    i3 = LoadU32(ctx, i3 + 16u);
    i4 = state;
    i3 = i3 == i4;
    i1 = i3 ? i1 : i2;
    i0 += i1;
    i1 = l3;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i0 = !(i0);
    if (i0) {goto spill_frame;}
    block_10:;
    i0 = l3;
    i1 = l6;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    l2 = i0;
    if (i0) {
      i0 = l3;
      i1 = l2;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = l3;
      StoreU32(ctx, i0 + 24u, i1);
    }
    i0 = state;
    i0 = LoadU32(ctx, i0 + 20u);
    l2 = i0;
    i0 = !(i0);
    if (i0) {goto spill_frame;}
    i0 = l3;
    i1 = l2;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = l2;
    i1 = l3;
    StoreU32(ctx, i0 + 24u, i1);
    goto spill_frame;
  }
  i0 = l5;
  i0 = LoadU32(ctx, i0 + 4u);
  l3 = i0;
  i1 = 3u;
  i0 &= i1;
  i1 = 3u;
  i0 = i0 != i1;
  if (i0) {goto spill_frame;}
  i0 = kF1212ConstD1378;
  i1 = block;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = l3;
  i2 = kF1212ConstFFFFFFFE;
  i1 &= i2;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = state;
  i1 = block;
  i2 = 1u;
  i1 |= i2;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l5;
  i1 = block;
  StoreU32(ctx, i0, i1);
  goto func_exit;
  spill_frame:;
  i0 = l5;
  i0 = LoadU32(ctx, i0 + 4u);
  l3 = i0;
  i1 = 2u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = kF1212ConstD1388;
    i1 = LoadU32(ctx, i1);
    i0 = i0 == i1;
    if (i0) {
      i0 = kF1212ConstD1388;
      i1 = state;
      StoreU32(ctx, i0, i1);
      i0 = kF1212ConstD137C;
      i1 = kF1212ConstD137C;
      i1 = LoadU32(ctx, i1);
      i2 = block;
      i1 += i2;
      block = i1;
      StoreU32(ctx, i0, i1);
      i0 = state;
      i1 = block;
      i2 = 1u;
      i1 |= i2;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = kF1212ConstD1384;
      i0 = LoadU32(ctx, i0);
      i1 = state;
      i0 = i0 != i1;
      if (i0) {goto block_0;}
      i0 = kF1212ConstD1378;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = kF1212ConstD1384;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      goto func_exit;
    }
    i0 = l5;
    i1 = kF1212ConstD1384;
    i1 = LoadU32(ctx, i1);
    i0 = i0 == i1;
    if (i0) {
      i0 = kF1212ConstD1384;
      i1 = state;
      StoreU32(ctx, i0, i1);
      i0 = kF1212ConstD1378;
      i1 = kF1212ConstD1378;
      i1 = LoadU32(ctx, i1);
      i2 = block;
      i1 += i2;
      block = i1;
      StoreU32(ctx, i0, i1);
      i0 = state;
      i1 = block;
      i2 = 1u;
      i1 |= i2;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = state;
      i1 = block;
      i0 += i1;
      i1 = block;
      StoreU32(ctx, i0, i1);
      goto func_exit;
    }
    i0 = kF1212ConstD1380;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = l3;
    i1 = kF1212ConstFFFFFFF8;
    i0 &= i1;
    i1 = block;
    i0 += i1;
    block = i0;
    i0 = l3;
    i1 = 255u;
    i0 = i0 <= i1;
    if (i0) {
      i0 = l5;
      i0 = LoadU32(ctx, i0 + 8u);
      l4 = i0;
      i1 = l3;
      i2 = 3u;
      i1 >>= (i2 & 31);
      l3 = i1;
      i2 = 3u;
      i1 <<= (i2 & 31);
      i2 = kF1212ConstD1398;
      i1 += i2;
      i0 = i0 != i1;
      i0 = l4;
      i1 = l5;
      i1 = LoadU32(ctx, i1 + 12u);
      l2 = i1;
      i0 = i0 == i1;
      if (i0) {
        i0 = kF1212ConstD1370;
        i1 = kF1212ConstD1370;
        i1 = LoadU32(ctx, i1);
        i2 = kF1212ConstFFFFFFFE;
        i3 = l3;
        i2 = I32_ROTL(i2, i3);
        i1 &= i2;
        StoreU32(ctx, i0, i1);
        goto block_17;
      }
      i0 = l4;
      i1 = l2;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = l4;
      StoreU32(ctx, i0 + 8u, i1);
      goto block_17;
    }
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 24u);
    l6 = i0;
    i0 = l5;
    i1 = l5;
    i1 = LoadU32(ctx, i1 + 12u);
    l3 = i1;
    i0 = i0 != i1;
    if (i0) {
      i0 = l2;
      l4 = i0;
      i0 = l4;
      i1 = l5;
      i1 = LoadU32(ctx, i1 + 8u);
      l2 = i1;
      i0 = i0 <= i1;
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 12u);
      }
      i0 = l2;
      i1 = l3;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l3;
      i1 = l2;
      StoreU32(ctx, i0 + 8u, i1);
      goto block_20;
    }
    i0 = l5;
    i1 = 20u;
    i0 += i1;
    l2 = i0;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    if (i0) {goto block_23;}
    i0 = l5;
    i1 = 16u;
    i0 += i1;
    l2 = i0;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    if (i0) {goto block_23;}
    i0 = 0u;
    l3 = i0;
    goto block_20;
    block_23:;
    loop_24: 
      i0 = l2;
      l7 = i0;
      i0 = l4;
      l3 = i0;
      i1 = 20u;
      i0 += i1;
      l2 = i0;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      if (i0) {goto loop_24;}
      i0 = l3;
      i1 = 16u;
      i0 += i1;
      l2 = i0;
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 16u);
      l4 = i0;
      if (i0) {goto loop_24;}
    i0 = l7;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    block_20:;
    i0 = l6;
    i0 = !(i0);
    if (i0) {goto block_17;}
    i0 = l5;
    i1 = l5;
    i1 = LoadU32(ctx, i1 + 28u);
    l2 = i1;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i2 = kF1212ConstD14A0;
    i1 += i2;
    l4 = i1;
    i1 = LoadU32(ctx, i1);
    i0 = i0 == i1;
    if (i0) {
      i0 = l4;
      i1 = l3;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      if (i0) {goto block_25;}
      i0 = kF1212ConstD1374;
      i1 = kF1212ConstD1374;
      i1 = LoadU32(ctx, i1);
      i2 = kF1212ConstFFFFFFFE;
      i3 = l2;
      i2 = I32_ROTL(i2, i3);
      i1 &= i2;
      StoreU32(ctx, i0, i1);
      goto block_17;
    }
    i0 = l6;
    i1 = 16u;
    i2 = 20u;
    i3 = l5;
    i4 = l6;
    i4 = LoadU32(ctx, i4 + 16u);
    i3 = i3 == i4;
    i1 = i3 ? i1 : i2;
    i0 += i1;
    i1 = l3;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i0 = !(i0);
    if (i0) {goto block_17;}
    block_25:;
    i0 = l3;
    i1 = l6;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 16u);
    l2 = i0;
    if (i0) {
      i0 = l3;
      i1 = l2;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = l3;
      StoreU32(ctx, i0 + 24u, i1);
    }
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 20u);
    l2 = i0;
    i0 = !(i0);
    if (i0) {goto block_17;}
    i0 = l3;
    i1 = l2;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = l2;
    i1 = l3;
    StoreU32(ctx, i0 + 24u, i1);
    block_17:;
    i0 = state;
    i1 = block;
    i2 = 1u;
    i1 |= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = state;
    i1 = block;
    i0 += i1;
    i1 = block;
    StoreU32(ctx, i0, i1);
    i0 = kF1212ConstD1384;
    i0 = LoadU32(ctx, i0);
    i1 = state;
    i0 = i0 != i1;
    if (i0) {goto block_13;}
    i0 = kF1212ConstD1378;
    i1 = block;
    StoreU32(ctx, i0, i1);
    goto func_exit;
  }
  i0 = l5;
  i1 = l3;
  i2 = kF1212ConstFFFFFFFE;
  i1 &= i2;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = state;
  i1 = block;
  i2 = 1u;
  i1 |= i2;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = state;
  i1 = block;
  i0 += i1;
  i1 = block;
  StoreU32(ctx, i0, i1);
  block_13:;
  i0 = block;
  i1 = 255u;
  i0 = i0 <= i1;
  if (i0) {
    i0 = block;
    i1 = 3u;
    i0 >>= (i1 & 31);
    l3 = i0;
    i1 = 3u;
    i0 <<= (i1 & 31);
    i1 = kF1212ConstD1398;
    i0 += i1;
    block = i0;
    i0 = kF1212ConstD1370;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i1 = 1u;
    i2 = l3;
    i1 <<= (i2 & 31);
    l3 = i1;
    i0 &= i1;
    i0 = !(i0);
    if (i0) {
      i0 = kF1212ConstD1370;
      i1 = l2;
      i2 = l3;
      i1 |= i2;
      StoreU32(ctx, i0, i1);
      i0 = block;
      goto block_29;
    }
    i0 = block;
    i0 = LoadU32(ctx, i0 + 8u);
    block_29:;
    l2 = i0;
    i0 = block;
    i1 = state;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l2;
    i1 = state;
    StoreU32(ctx, i0 + 12u, i1);
    i0 = state;
    i1 = block;
    StoreU32(ctx, i0 + 12u, i1);
    i0 = state;
    i1 = l2;
    StoreU32(ctx, i0 + 8u, i1);
    goto func_exit;
  }
  i0 = 31u;
  l2 = i0;
  i0 = state;
  j1 = 0ull;
  StoreU64(ctx, i0 + 16u, j1);
  i0 = block;
  i1 = kF1212ConstFFFFFF;
  i0 = i0 <= i1;
  if (i0) {
    i0 = block;
    i1 = 8u;
    i0 >>= (i1 & 31);
    l2 = i0;
    i1 = kF1212ConstFFF00;
    i0 += i1;
    i1 = 16u;
    i0 >>= (i1 & 31);
    i1 = 8u;
    i0 &= i1;
    l3 = i0;
    i0 = l2;
    i1 = l3;
    i0 <<= (i1 & 31);
    l2 = i0;
    i1 = l2;
    i2 = kF1212Const7F000;
    i1 += i2;
    i2 = 16u;
    i1 >>= (i2 & 31);
    i2 = 4u;
    i1 &= i2;
    l2 = i1;
    i0 <<= (i1 & 31);
    l4 = i0;
    i1 = l4;
    i2 = kF1212Const3C000;
    i1 += i2;
    i2 = 16u;
    i1 >>= (i2 & 31);
    i2 = 2u;
    i1 &= i2;
    l4 = i1;
    i0 <<= (i1 & 31);
    i1 = 15u;
    i0 >>= (i1 & 31);
    i1 = l4;
    i2 = l2;
    i3 = l3;
    i2 |= i3;
    i1 |= i2;
    i0 -= i1;
    l3 = i0;
    i1 = 1u;
    i0 <<= (i1 & 31);
    l4 = i0;
    i0 = l4;
    i1 = block;
    i2 = l3;
    i3 = 21u;
    i2 += i3;
    i1 >>= (i2 & 31);
    i2 = 1u;
    i1 &= i2;
    i0 |= i1;
    i1 = 28u;
    i0 += i1;
    l2 = i0;
  }
  i0 = state;
  i1 = l2;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l2;
  i1 = 2u;
  i0 <<= (i1 & 31);
  i1 = kF1212ConstD14A0;
  i0 += i1;
  l3 = i0;
  i0 = kF1212ConstD1374;
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  i1 = 1u;
  i2 = l2;
  i1 <<= (i2 & 31);
  l7 = i1;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {
    i0 = kF1212ConstD1374;
    i1 = l4;
    i2 = l7;
    i1 |= i2;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = state;
    StoreU32(ctx, i0, i1);
    i0 = state;
    i1 = l3;
    StoreU32(ctx, i0 + 24u, i1);
    goto block_33;
  }
  i0 = block;
  i1 = 0u;
  i2 = 25u;
  i3 = l2;
  i4 = 1u;
  i3 >>= (i4 & 31);
  i2 -= i3;
  i3 = l2;
  i4 = 31u;
  i3 = i3 == i4;
  i1 = i3 ? i1 : i2;
  i0 <<= (i1 & 31);
  l2 = i0;
  i0 = l3;
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  loop_35: 
    i0 = l3;
    l4 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    i1 = kF1212ConstFFFFFFF8;
    i0 &= i1;
    i1 = block;
    i0 = i0 == i1;
    if (i0) {goto block_32;}
    i0 = l2;
    i1 = 29u;
    i0 >>= (i1 & 31);
    l3 = i0;
    i0 = l2;
    i1 = 1u;
    i0 <<= (i1 & 31);
    l2 = i0;
    i0 = l3;
    i1 = 4u;
    i0 &= i1;
    i1 = l4;
    i0 += i1;
    l7 = i0;
    i1 = 16u;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    if (i0) {goto loop_35;}
  i0 = l7;
  i1 = state;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = state;
  i1 = l4;
  StoreU32(ctx, i0 + 24u, i1);
  block_33:;
  i0 = state;
  i1 = state;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = state;
  i1 = state;
  StoreU32(ctx, i0 + 8u, i1);
  goto func_exit;
  block_32:;
  i0 = l4;
  i0 = LoadU32(ctx, i0 + 8u);
  block = i0;
  i1 = state;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l4;
  i1 = state;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = state;
  i1 = 0u;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = state;
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = state;
  i1 = block;
  StoreU32(ctx, i0 + 8u, i1);
  block_0:;
  func_exit:;
  LeaveFrame();
}
}
