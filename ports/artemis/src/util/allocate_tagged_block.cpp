// allocate_tagged_block.cpp -- allocates a tagged block of the requested size and stamps its header..
//
// AllocateTaggedBlock -- allocates a tagged block of the requested size and stamps its header.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 1013088-1013088 (f2207); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void ReclaimFreeBlock(EngineContext* ctx, uint32_t state, uint32_t block);

namespace {

constexpr uint32_t kF2207ConstD1370 = 0xD1370u;  // 
constexpr uint32_t kF2207ConstD1374 = 0xD1374u;  // 
constexpr uint32_t kF2207ConstD1378 = 0xD1378u;  // 
constexpr uint32_t kF2207ConstD137C = 0xD137Cu;  // 
constexpr uint32_t kF2207ConstD1380 = 0xD1380u;  // 
constexpr uint32_t kF2207ConstD1384 = 0xD1384u;  // 
constexpr uint32_t kF2207ConstD1388 = 0xD1388u;  // 
constexpr uint32_t kF2207ConstD1398 = 0xD1398u;  // 
constexpr uint32_t kF2207ConstD14A0 = 0xD14A0u;  // 
constexpr uint32_t kF2207ConstD1550 = 0xD1550u;  // 
constexpr uint32_t kF2207ConstFFFFFFF8 = 0xFFFFFFF8u;  // 
constexpr uint32_t kF2207ConstFFFFFFFE = 0xFFFFFFFEu;  // 

}  // namespace

uint32_t AllocateTaggedBlock(EngineContext* ctx, uint32_t header, uint32_t size) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
      l10 = 0;
  i0 = header;
  i0 = LoadU32(ctx, i0 + 4u);
  l7 = i0;
  i1 = 3u;
  i0 &= i1;
  l2 = i0;
  i0 = l7;
  i1 = kF2207ConstFFFFFFF8;
  i0 &= i1;
  l6 = i0;
  i1 = header;
  i0 += i1;
  l5 = i0;
  i0 = kF2207ConstD1380;
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = header;
  i0 = i0 > i1;
  if (i0) {goto block_0;}
  i0 = l2;
  i1 = 1u;
  i0 = i0 == i1;
  if (i0) {goto block_0;}
  block_0:;
  i0 = l2;
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    l2 = i0;
    i0 = size;
    i1 = 256u;
    i0 = i0 < i1;
    if (i0) {goto spill_frame;}
    i0 = l6;
    i1 = size;
    i2 = 4u;
    i1 += i2;
    i0 = i0 >= i1;
    if (i0) {
      i0 = header;
      l2 = i0;
      i0 = l6;
      i1 = size;
      i0 -= i1;
      i1 = kF2207ConstD1550;
      i1 = LoadU32(ctx, i1);
      i2 = 1u;
      i1 <<= (i2 & 31);
      i0 = i0 <= i1;
      if (i0) {goto spill_frame;}
    }
    i0 = 0u;
    goto func_exit;
  }
  i0 = size;
  i1 = l6;
  i0 = i0 <= i1;
  if (i0) {
    i0 = l6;
    i1 = size;
    i0 -= i1;
    l2 = i0;
    i1 = 16u;
    i0 = i0 < i1;
    if (i0) {goto running_exit;}
    i0 = header;
    i1 = l7;
    i2 = 1u;
    i1 &= i2;
    i2 = size;
    i1 |= i2;
    i2 = 2u;
    i1 |= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = header;
    i1 = size;
    i0 += i1;
    size = i0;
    i1 = l2;
    i2 = 3u;
    i1 |= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l5;
    i1 = l5;
    i1 = LoadU32(ctx, i1 + 4u);
    i2 = 1u;
    i1 |= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = size;
    i1 = l2;
    ReclaimFreeBlock(ctx, i0, i1);
    goto running_exit;
  }
  i0 = 0u;
  l2 = i0;
  i0 = l5;
  i1 = kF2207ConstD1388;
  i1 = LoadU32(ctx, i1);
  i0 = i0 == i1;
  if (i0) {
    i0 = l6;
    i1 = kF2207ConstD137C;
    i1 = LoadU32(ctx, i1);
    i0 += i1;
    l3 = i0;
    i1 = size;
    i0 = i0 <= i1;
    if (i0) {goto spill_frame;}
    i0 = header;
    i1 = l7;
    i2 = 1u;
    i1 &= i2;
    i2 = size;
    i1 |= i2;
    i2 = 2u;
    i1 |= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = header;
    i1 = size;
    i0 += i1;
    l2 = i0;
    i1 = l3;
    i2 = size;
    i1 -= i2;
    size = i1;
    i2 = 1u;
    i1 |= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = kF2207ConstD137C;
    i1 = size;
    StoreU32(ctx, i0, i1);
    i0 = kF2207ConstD1388;
    i1 = l2;
    StoreU32(ctx, i0, i1);
    goto running_exit;
  }
  i0 = l5;
  i1 = kF2207ConstD1384;
  i1 = LoadU32(ctx, i1);
  i0 = i0 == i1;
  if (i0) {
    i0 = l6;
    i1 = kF2207ConstD1378;
    i1 = LoadU32(ctx, i1);
    i0 += i1;
    l3 = i0;
    i1 = size;
    i0 = i0 < i1;
    if (i0) {goto spill_frame;}
    i0 = l3;
    i1 = size;
    i0 -= i1;
    l2 = i0;
    i1 = 16u;
    i0 = i0 >= i1;
    if (i0) {
      i0 = header;
      i1 = l7;
      i2 = 1u;
      i1 &= i2;
      i2 = size;
      i1 |= i2;
      i2 = 2u;
      i1 |= i2;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = header;
      i1 = size;
      i0 += i1;
      size = i0;
      i1 = l2;
      i2 = 1u;
      i1 |= i2;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = header;
      i1 = l3;
      i0 += i1;
      l3 = i0;
      i1 = l2;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 4u);
      i2 = kF2207ConstFFFFFFFE;
      i1 &= i2;
      StoreU32(ctx, i0 + 4u, i1);
      goto block_8;
    }
    i0 = header;
    i1 = l3;
    i2 = l7;
    i3 = 1u;
    i2 &= i3;
    i1 |= i2;
    i2 = 2u;
    i1 |= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = header;
    i1 = l3;
    i0 += i1;
    size = i0;
    i0 = LoadU32(ctx, i0 + 4u);
    i1 = 1u;
    i0 |= i1;
    l2 = i0;
    i0 = size;
    i1 = l2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = 0u;
    l2 = i0;
    i0 = 0u;
    size = i0;
    block_8:;
    i0 = kF2207ConstD1384;
    i1 = size;
    StoreU32(ctx, i0, i1);
    i0 = kF2207ConstD1378;
    i1 = l2;
    StoreU32(ctx, i0, i1);
    goto running_exit;
  }
  i0 = l5;
  i0 = LoadU32(ctx, i0 + 4u);
  l4 = i0;
  i1 = 2u;
  i0 &= i1;
  if (i0) {goto spill_frame;}
  i0 = l4;
  i1 = kF2207ConstFFFFFFF8;
  i0 &= i1;
  i1 = l6;
  i0 += i1;
  l9 = i0;
  i1 = size;
  i0 = i0 < i1;
  if (i0) {goto spill_frame;}
  i0 = l9;
  i1 = size;
  i0 -= i1;
  l10 = i0;
  i0 = l4;
  i1 = 255u;
  i0 = i0 <= i1;
  if (i0) {
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    l6 = i0;
    i1 = l4;
    i2 = 3u;
    i1 >>= (i2 & 31);
    l3 = i1;
    i2 = 3u;
    i1 <<= (i2 & 31);
    i2 = kF2207ConstD1398;
    i1 += i2;
    i0 = i0 != i1;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 12u);
    l8 = i0;
    i1 = l6;
    i0 = i0 == i1;
    if (i0) {
      i0 = kF2207ConstD1370;
      i1 = kF2207ConstD1370;
      i1 = LoadU32(ctx, i1);
      i2 = kF2207ConstFFFFFFFE;
      i3 = l3;
      i2 = I32_ROTL(i2, i3);
      i1 &= i2;
      StoreU32(ctx, i0, i1);
      goto block_10;
    }
    i0 = l6;
    i1 = l8;
    StoreU32(ctx, i0 + 12u, i1);
    i0 = l8;
    i1 = l6;
    StoreU32(ctx, i0 + 8u, i1);
    goto block_10;
  }
  i0 = l5;
  i0 = LoadU32(ctx, i0 + 24u);
  l8 = i0;
  i0 = l5;
  i1 = l5;
  i1 = LoadU32(ctx, i1 + 12u);
  l4 = i1;
  i0 = i0 != i1;
  if (i0) {
    i0 = l3;
    i1 = l5;
    i1 = LoadU32(ctx, i1 + 8u);
    l2 = i1;
    i0 = i0 <= i1;
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 12u);
    }
    i0 = l2;
    i1 = l4;
    StoreU32(ctx, i0 + 12u, i1);
    i0 = l4;
    i1 = l2;
    StoreU32(ctx, i0 + 8u, i1);
    goto block_13;
  }
  i0 = l5;
  i1 = 20u;
  i0 += i1;
  l2 = i0;
  i0 = LoadU32(ctx, i0);
  l6 = i0;
  if (i0) {goto block_16;}
  i0 = l5;
  i1 = 16u;
  i0 += i1;
  l2 = i0;
  i0 = LoadU32(ctx, i0);
  l6 = i0;
  if (i0) {goto block_16;}
  i0 = 0u;
  l4 = i0;
  goto block_13;
  block_16:;
  loop_17: 
    i0 = l2;
    l3 = i0;
    i0 = l6;
    l4 = i0;
    i1 = 20u;
    i0 += i1;
    l2 = i0;
    i0 = LoadU32(ctx, i0);
    l6 = i0;
    if (i0) {goto loop_17;}
    i0 = l4;
    i1 = 16u;
    i0 += i1;
    l2 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l6 = i0;
    if (i0) {goto loop_17;}
  i0 = l3;
  i1 = 0u;
  StoreU32(ctx, i0, i1);
  block_13:;
  i0 = l8;
  i0 = !(i0);
  if (i0) {goto block_10;}
  i0 = l5;
  i1 = l5;
  i1 = LoadU32(ctx, i1 + 28u);
  l3 = i1;
  i2 = 2u;
  i1 <<= (i2 & 31);
  i2 = kF2207ConstD14A0;
  i1 += i2;
  l2 = i1;
  i1 = LoadU32(ctx, i1);
  i0 = i0 == i1;
  if (i0) {
    i0 = l2;
    i1 = l4;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    if (i0) {goto block_18;}
    i0 = kF2207ConstD1374;
    i1 = kF2207ConstD1374;
    i1 = LoadU32(ctx, i1);
    i2 = kF2207ConstFFFFFFFE;
    i3 = l3;
    i2 = I32_ROTL(i2, i3);
    i1 &= i2;
    StoreU32(ctx, i0, i1);
    goto block_10;
  }
  i0 = l8;
  i1 = 16u;
  i2 = 20u;
  i3 = l5;
  i4 = l8;
  i4 = LoadU32(ctx, i4 + 16u);
  i3 = i3 == i4;
  i1 = i3 ? i1 : i2;
  i0 += i1;
  i1 = l4;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i0 = !(i0);
  if (i0) {goto block_10;}
  block_18:;
  i0 = l4;
  i1 = l8;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l5;
  i0 = LoadU32(ctx, i0 + 16u);
  l2 = i0;
  if (i0) {
    i0 = l4;
    i1 = l2;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = l2;
    i1 = l4;
    StoreU32(ctx, i0 + 24u, i1);
  }
  i0 = l5;
  i0 = LoadU32(ctx, i0 + 20u);
  l2 = i0;
  i0 = !(i0);
  if (i0) {goto block_10;}
  i0 = l4;
  i1 = l2;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l2;
  i1 = l4;
  StoreU32(ctx, i0 + 24u, i1);
  block_10:;
  i0 = l10;
  i1 = 15u;
  i0 = i0 <= i1;
  if (i0) {
    i0 = header;
    i1 = l9;
    i2 = l7;
    i3 = 1u;
    i2 &= i3;
    i1 |= i2;
    i2 = 2u;
    i1 |= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = header;
    i1 = l9;
    i0 += i1;
    size = i0;
    i0 = LoadU32(ctx, i0 + 4u);
    i1 = 1u;
    i0 |= i1;
    l2 = i0;
    i0 = size;
    i1 = l2;
    StoreU32(ctx, i0 + 4u, i1);
    goto running_exit;
  }
  i0 = header;
  i1 = l7;
  i2 = 1u;
  i1 &= i2;
  i2 = size;
  i1 |= i2;
  i2 = 2u;
  i1 |= i2;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = header;
  i1 = size;
  i0 += i1;
  l2 = i0;
  i1 = l10;
  i2 = 3u;
  i1 |= i2;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = header;
  i1 = l9;
  i0 += i1;
  size = i0;
  i0 = LoadU32(ctx, i0 + 4u);
  i1 = 1u;
  i0 |= i1;
  l3 = i0;
  i0 = size;
  i1 = l3;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l2;
  i1 = l10;
  ReclaimFreeBlock(ctx, i0, i1);
  running_exit:;
  i0 = header;
  l2 = i0;
  spill_frame:;
  i0 = l2;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
