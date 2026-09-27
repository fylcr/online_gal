// prepare_string_buffer.cpp -- makes the string buffer ready for the given size, switching to the external block past the inline threshold..
//
// PrepareStringBuffer -- makes the string buffer ready for the given size, switching to the external block past the inline threshold.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 116393-116393 (f221); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF221Const18AD0 = 0x18AD0u;  // 
constexpr uint32_t kF221ConstFFFFFFFF = 0xFFFFFFFFu;  // 

}  // namespace

uint32_t PrepareStringBuffer(EngineContext* ctx, uint32_t buffer, uint32_t size) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = size;
  i1 = 33u;
  i0 = i0 >= i1;
  if (i0) {
    i0 = buffer;
    i0 = LoadU32(ctx, i0 + 16u);
    l3 = i0;
    goto block_0;
  }
  i0 = buffer;
  i0 = LoadU32(ctx, i0 + 4u);
  l4 = i0;
  i1 = size;
  i0 += i1;
  l2 = i0;
  i0 = buffer;
  i0 = LoadU32(ctx, i0);
  l5 = i0;
  i1 = buffer;
  i1 = LoadU32(ctx, i1 + 16u);
  l3 = i1;
  i2 = 4u;
  i1 -= i2;
  i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
  if (i0) {goto block_2;}
  i0 = l5;
  i1 = l3;
  i2 = l2;
  i3 = 7u;
  i2 += i3;
  i3 = 3u;
  i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
  i1 -= i2;
  i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
  if (i0) {goto block_0;}
  i0 = l2;
  if (i0) {goto block_2;}
  i0 = 0u;
  goto func_exit;
  block_2:;
  i0 = size;
  i1 = 2u;
  i0 <<= (i1 & 31);
  i1 = kF221Const18AD0;
  i0 += i1;
  i0 = LoadU32(ctx, i0);
  l6 = i0;
  i0 = buffer;
  i0 = LoadU32(ctx, i0 + 12u);
  l3 = i0;
  i0 = LoadU8(ctx, i0);
  i1 = l4;
  i0 >>= (i1 & 31);
  size = i0;
  i0 = l2;
  i1 = 9u;
  i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
  if (i0) {goto block_3;}
  i0 = l3;
  i0 = LoadU8(ctx, i0 + 1u);
  i1 = 8u;
  i2 = l4;
  i1 -= i2;
  i0 <<= (i1 & 31);
  i1 = size;
  i0 |= i1;
  size = i0;
  i0 = l2;
  i1 = 17u;
  i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
  if (i0) {goto block_3;}
  i0 = l3;
  i0 = LoadU8(ctx, i0 + 2u);
  i1 = 16u;
  i2 = l4;
  i1 -= i2;
  i0 <<= (i1 & 31);
  i1 = size;
  i0 |= i1;
  size = i0;
  i0 = l2;
  i1 = 25u;
  i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
  if (i0) {goto block_3;}
  i0 = l3;
  i0 = LoadU8(ctx, i0 + 3u);
  i1 = 24u;
  i2 = l4;
  i1 -= i2;
  i0 <<= (i1 & 31);
  i1 = size;
  i0 |= i1;
  size = i0;
  i0 = l4;
  i0 = !(i0);
  if (i0) {goto block_3;}
  i0 = l2;
  i1 = 33u;
  i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
  if (i0) {goto block_3;}
  i0 = l3;
  i0 = LoadU8(ctx, i0 + 4u);
  i1 = 32u;
  i2 = l4;
  i1 -= i2;
  i0 <<= (i1 & 31);
  i1 = size;
  i0 |= i1;
  size = i0;
  block_3:;
  i0 = buffer;
  i1 = l2;
  i2 = 7u;
  i1 &= i2;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = buffer;
  i1 = l5;
  i2 = l2;
  i3 = 8u;
  i2 = I32_DIV_S(i2, i3);
  l2 = i2;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = buffer;
  i1 = l2;
  i2 = l3;
  i1 += i2;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = size;
  i1 = l6;
  i0 &= i1;
  goto func_exit;
  block_0:;
  i0 = buffer;
  i1 = 1u;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = buffer;
  i1 = l3;
  StoreU32(ctx, i0, i1);
  i0 = buffer;
  i1 = 0u;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = kF221ConstFFFFFFFF;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
