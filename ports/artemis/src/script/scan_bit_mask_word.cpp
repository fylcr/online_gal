// scan_bit_mask_word.cpp -- scans a bit-mask word: the first set bit is answered with its index..
//
// ScanBitMaskWord -- scans a bit-mask word: the first set bit is answered with its index.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 543737-543737 (f1325); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t FindStringLength(EngineContext* ctx, uint32_t value, uint32_t literal);
uint32_t FillBytes(EngineContext* ctx, uint32_t dst, uint32_t value, uint32_t length);

namespace {


}  // namespace

uint32_t ScanBitMaskWord(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = GuestStackSp(ctx);
  i1 = 32u;
  i0 -= i1;
  l4 = i0;
  GuestStackSp(ctx) = i0;
  i0 = b;
  i0 = LoadS8(ctx, i0);
  l3 = i0;
  if (i0) {
    i0 = b;
    i0 = LoadU8(ctx, i0 + 1u);
    if (i0) {goto spill_frame;}
  }
  i0 = a;
  i1 = l3;
  i0 = FindStringLength(ctx, i0, i1);
  l3 = i0;
  goto block_0;
  spill_frame:;
  i0 = l4;
  i1 = 0u;
  i2 = 32u;
  i0 = FillBytes(ctx, i0, i1, i2);
  i0 = b;
  i0 = LoadU8(ctx, i0);
  l2 = i0;
  if (i0) {
    loop_4: 
      i0 = l4;
      i1 = l2;
      i2 = 3u;
      i1 >>= (i2 & 31);
      i2 = 28u;
      i1 &= i2;
      i0 += i1;
      l3 = i0;
      i1 = l3;
      i1 = LoadU32(ctx, i1);
      i2 = 1u;
      i3 = l2;
      i2 <<= (i3 & 31);
      i1 |= i2;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i0 = LoadU8(ctx, i0 + 1u);
      l2 = i0;
      i0 = b;
      i1 = 1u;
      i0 += i1;
      b = i0;
      i0 = l2;
      if (i0) {goto loop_4;}
  }
  i0 = a;
  l3 = i0;
  i0 = LoadU8(ctx, i0);
  l2 = i0;
  i0 = !(i0);
  if (i0) {goto block_0;}
  i0 = a;
  b = i0;
  loop_5: 
    i0 = l4;
    i1 = l2;
    i2 = 3u;
    i1 >>= (i2 & 31);
    i2 = 28u;
    i1 &= i2;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    i1 = l2;
    i0 >>= (i1 & 31);
    i1 = 1u;
    i0 &= i1;
    if (i0) {
      i0 = b;
      l3 = i0;
      goto block_0;
    }
    i0 = b;
    i0 = LoadU8(ctx, i0 + 1u);
    l2 = i0;
    i0 = b;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    b = i0;
    i0 = l2;
    if (i0) {goto loop_5;}
  block_0:;
  i0 = l4;
  i1 = 32u;
  i0 += i1;
  GuestStackSp(ctx) = i0;
  i0 = l3;
  i1 = a;
  i0 -= i1;
  LeaveFrame();
  return i0;
}
}
