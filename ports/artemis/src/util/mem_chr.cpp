// mem_chr.cpp -- the libc memchr: a byte scan up to the alignment boundary, then the word-at-a-time haszero probe, and the address of the first matching byte, or zero when the range runs out..
//
// MemChr -- the libc memchr: a byte scan up to the alignment boundary, then the word-at-a-time haszero probe, and the address of the first matching byte, or zero when the range runs out.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 122752-122752 (f240); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF240Const1010101 = 0x1010101u;  // 
constexpr uint32_t kF240Const80808080 = 0x80808080u;  // 
constexpr uint32_t kF240ConstFFFFFFFF = 0xFFFFFFFFu;  // 

}  // namespace

uint32_t MemChr(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  i0 = c;
  i1 = 0u;
  i0 = i0 != i1;
  l3 = i0;
  i0 = c;
  i0 = !(i0);
  if (i0) {goto block_2;}
  i0 = a;
  i1 = 3u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto block_2;}
  i0 = b;
  i1 = 255u;
  i0 &= i1;
  l4 = i0;
  loop_3: 
    i0 = l4;
    i1 = a;
    i1 = LoadU8(ctx, i1);
    i0 = i0 == i1;
    if (i0) {goto spill_frame;}
    i0 = a;
    i1 = 1u;
    i0 += i1;
    a = i0;
    i0 = c;
    i1 = 1u;
    i0 -= i1;
    c = i0;
    i1 = 0u;
    i0 = i0 != i1;
    l3 = i0;
    i0 = c;
    i0 = !(i0);
    if (i0) {goto block_2;}
    i0 = a;
    i1 = 3u;
    i0 &= i1;
    if (i0) {goto loop_3;}
  block_2:;
  i0 = l3;
  i0 = !(i0);
  if (i0) {goto block_0;}
  spill_frame:;
  i0 = a;
  i0 = LoadU8(ctx, i0);
  i1 = b;
  i2 = 255u;
  i1 &= i2;
  i0 = i0 == i1;
  if (i0) {goto running_exit;}
  i0 = c;
  i1 = 4u;
  i0 = i0 < i1;
  if (i0) {goto running_exit;}
  i0 = b;
  i1 = 255u;
  i0 &= i1;
  i1 = kF240Const1010101;
  i0 *= i1;
  l3 = i0;
  loop_5: 
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i1 = l3;
    i0 ^= i1;
    l4 = i0;
    i1 = kF240ConstFFFFFFFF;
    i0 ^= i1;
    l5 = i0;
    i0 = l5;
    i1 = l4;
    i2 = kF240Const1010101;
    i1 -= i2;
    i0 &= i1;
    i1 = kF240Const80808080;
    i0 &= i1;
    if (i0) {goto running_exit;}
    i0 = a;
    i1 = 4u;
    i0 += i1;
    a = i0;
    i0 = c;
    i1 = 4u;
    i0 -= i1;
    c = i0;
    i1 = 3u;
    i0 = i0 > i1;
    if (i0) {goto loop_5;}
  running_exit:;
  i0 = c;
  i0 = !(i0);
  if (i0) {goto block_0;}
  i0 = b;
  i1 = 255u;
  i0 &= i1;
  b = i0;
  loop_6: 
    i0 = a;
    i0 = LoadU8(ctx, i0);
    i1 = b;
    i0 = i0 == i1;
    if (i0) {
      i0 = a;
      goto func_exit;
    }
    i0 = a;
    i1 = 1u;
    i0 += i1;
    a = i0;
    i0 = c;
    i1 = 1u;
    i0 -= i1;
    c = i0;
    if (i0) {goto loop_6;}
  block_0:;
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
