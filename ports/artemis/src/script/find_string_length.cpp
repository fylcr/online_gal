// find_string_length.cpp -- walks a NUL-terminated literal to find its length..
//
// FindStringLength -- walks a NUL-terminated literal to find its length.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 397754-397754 (f1005); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t StringLength(EngineContext* ctx, uint32_t text);

namespace {

constexpr uint32_t kF1005Const1010101 = 0x1010101u;  // 
constexpr uint32_t kF1005Const80808080 = 0x80808080u;  // 
constexpr uint32_t kF1005ConstFFFFFFFF = 0xFFFFFFFFu;  // 

}  // namespace

uint32_t FindStringLength(EngineContext* ctx, uint32_t value, uint32_t literal) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  i0 = literal;
  i1 = 255u;
  i0 &= i1;
  l3 = i0;
  if (i0) {
    i0 = value;
    i1 = 3u;
    i0 &= i1;
    if (i0) {
      loop_3: 
        i0 = value;
        i0 = LoadU8(ctx, i0);
        l2 = i0;
        i0 = !(i0);
        if (i0) {goto block_0;}
        i0 = literal;
        i1 = 255u;
        i0 &= i1;
        i1 = l2;
        i0 = i0 == i1;
        if (i0) {goto block_0;}
        i0 = value;
        i1 = 1u;
        i0 += i1;
        value = i0;
        i1 = 3u;
        i0 &= i1;
        if (i0) {goto loop_3;}
    }
    i0 = value;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i1 = kF1005ConstFFFFFFFF;
    i0 ^= i1;
    i1 = l2;
    i2 = kF1005Const1010101;
    i1 -= i2;
    i0 &= i1;
    i1 = kF1005Const80808080;
    i0 &= i1;
    if (i0) {goto running_exit;}
    i0 = l3;
    i1 = kF1005Const1010101;
    i0 *= i1;
    l3 = i0;
    loop_5: 
      i0 = l2;
      i1 = l3;
      i0 ^= i1;
      l2 = i0;
      i1 = kF1005ConstFFFFFFFF;
      i0 ^= i1;
      l4 = i0;
      i0 = l4;
      i1 = l2;
      i2 = kF1005Const1010101;
      i1 -= i2;
      i0 &= i1;
      i1 = kF1005Const80808080;
      i0 &= i1;
      if (i0) {goto running_exit;}
      i0 = value;
      i0 = LoadU32(ctx, i0 + 4u);
      l2 = i0;
      i0 = value;
      i1 = 4u;
      i0 += i1;
      value = i0;
      i0 = l2;
      i1 = kF1005Const1010101;
      i0 -= i1;
      i1 = l2;
      i2 = kF1005ConstFFFFFFFF;
      i1 ^= i2;
      i0 &= i1;
      i1 = kF1005Const80808080;
      i0 &= i1;
      i0 = !(i0);
      if (i0) {goto loop_5;}
    running_exit:;
    loop_6: 
      i0 = value;
      l2 = i0;
      i0 = LoadU8(ctx, i0);
      l3 = i0;
      if (i0) {
        i0 = l2;
        i1 = 1u;
        i0 += i1;
        value = i0;
        i0 = l3;
        i1 = literal;
        i2 = 255u;
        i1 &= i2;
        i0 = i0 != i1;
        if (i0) {goto loop_6;}
      }
    i0 = l2;
    goto func_exit;
  }
  i0 = value;
  i0 = StringLength(ctx, i0);
  i1 = value;
  i0 += i1;
  goto func_exit;
  block_0:;
  i0 = value;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
