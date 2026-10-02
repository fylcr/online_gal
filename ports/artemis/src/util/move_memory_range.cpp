// move_memory_range.cpp -- moves a memory range: the overlapping case is ordered so the copy survives a destination that sits inside the source..
//
// MoveMemoryRange -- moves a memory range: the overlapping case is ordered so the copy survives a destination that sits inside the source.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 129655-129655 (f268); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t CopyBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {


}  // namespace

uint32_t MoveMemoryRange(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t l3 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = a;
  i1 = b;
  i0 = i0 == i1;
  if (i0) {goto block_0;}
  i0 = b;
  i1 = a;
  i0 -= i1;
  i1 = c;
  i0 -= i1;
  i1 = 0u;
  i2 = c;
  i3 = 1u;
  i2 <<= (i3 & 31);
  i1 -= i2;
  i0 = i0 <= i1;
  if (i0) {
    i0 = a;
    i1 = b;
    i2 = c;
    i0 = CopyBytes(ctx, i0, i1, i2);
    goto func_exit;
  }
  i0 = a;
  i1 = b;
  i0 ^= i1;
  i1 = 3u;
  i0 &= i1;
  l3 = i0;
  i0 = a;
  i1 = b;
  i0 = i0 < i1;
  if (i0) {
    i0 = l3;
    if (i0) {
      i0 = a;
      l3 = i0;
      goto block_2;
    }
    i0 = a;
    i1 = 3u;
    i0 &= i1;
    i0 = !(i0);
    if (i0) {
      i0 = a;
      l3 = i0;
      goto block_3;
    }
    i0 = a;
    l3 = i0;
    loop_7: 
      i0 = c;
      i0 = !(i0);
      if (i0) {goto block_0;}
      i0 = l3;
      i1 = b;
      i1 = LoadU8(ctx, i1);
      StoreU8(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 += i1;
      b = i0;
      i0 = c;
      i1 = 1u;
      i0 -= i1;
      c = i0;
      i0 = l3;
      i1 = 1u;
      i0 += i1;
      l3 = i0;
      i1 = 3u;
      i0 &= i1;
      if (i0) {goto loop_7;}
    goto block_3;
  }
  i0 = l3;
  if (i0) {goto block_8;}
  i0 = a;
  i1 = c;
  i0 += i1;
  i1 = 3u;
  i0 &= i1;
  if (i0) {
    loop_10: 
      i0 = c;
      i0 = !(i0);
      if (i0) {goto block_0;}
      i0 = c;
      i1 = 1u;
      i0 -= i1;
      c = i0;
      i1 = a;
      i0 += i1;
      l3 = i0;
      i1 = b;
      i2 = c;
      i1 += i2;
      i1 = LoadU8(ctx, i1);
      StoreU8(ctx, i0, i1);
      i0 = l3;
      i1 = 3u;
      i0 &= i1;
      if (i0) {goto loop_10;}
  }
  i0 = c;
  i1 = 3u;
  i0 = i0 <= i1;
  if (i0) {goto block_8;}
  loop_11: 
    i0 = c;
    i1 = 4u;
    i0 -= i1;
    c = i0;
    i1 = a;
    i0 += i1;
    i1 = b;
    i2 = c;
    i1 += i2;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = 3u;
    i0 = i0 > i1;
    if (i0) {goto loop_11;}
  block_8:;
  i0 = c;
  i0 = !(i0);
  if (i0) {goto block_0;}
  loop_12: 
    i0 = c;
    i1 = 1u;
    i0 -= i1;
    c = i0;
    i1 = a;
    i0 += i1;
    i1 = b;
    i2 = c;
    i1 += i2;
    i1 = LoadU8(ctx, i1);
    StoreU8(ctx, i0, i1);
    i0 = c;
    if (i0) {goto loop_12;}
  goto block_0;
  block_3:;
  i0 = c;
  i1 = 3u;
  i0 = i0 <= i1;
  if (i0) {goto block_2;}
  loop_13: 
    i0 = l3;
    i1 = b;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 4u;
    i0 += i1;
    b = i0;
    i0 = l3;
    i1 = 4u;
    i0 += i1;
    l3 = i0;
    i0 = c;
    i1 = 4u;
    i0 -= i1;
    c = i0;
    i1 = 3u;
    i0 = i0 > i1;
    if (i0) {goto loop_13;}
  block_2:;
  i0 = c;
  i0 = !(i0);
  if (i0) {goto block_0;}
  loop_14: 
    i0 = l3;
    i1 = b;
    i1 = LoadU8(ctx, i1);
    StoreU8(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    i0 = b;
    i1 = 1u;
    i0 += i1;
    b = i0;
    i0 = c;
    i1 = 1u;
    i0 -= i1;
    c = i0;
    if (i0) {goto loop_14;}
  block_0:;
  i0 = a;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
