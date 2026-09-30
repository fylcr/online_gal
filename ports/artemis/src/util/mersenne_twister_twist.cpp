// mersenne_twister_twist.cpp -- std::mt19937's state twist: the 624-word array is regenerated in place with the 0x9908B0DF matrix constant, the wrapped middle words are patched, and the index goes back to zero..
//
// MersenneTwisterTwist -- std::mt19937's state twist: the 624-word array is regenerated in place with the 0x9908B0DF matrix constant, the wrapped middle words are patched, and the index goes back to zero.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 330133-330133 (f857); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF857Const7FFFFFFE = 0x7FFFFFFEu;  // 
constexpr uint32_t kF857Const80000000 = 0x80000000u;  // 
constexpr uint32_t kF857Const9908B0DF = 0x9908B0DFu;  // 

}  // namespace

void MersenneTwisterTwist(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  i0 = a;
  i0 = LoadU32(ctx, i0);
  l2 = i0;
  loop_0: 
    i0 = l2;
    i1 = kF857Const80000000;
    i0 &= i1;
    l3 = i0;
    i0 = l1;
    i1 = 2u;
    i0 <<= (i1 & 31);
    i1 = a;
    i0 += i1;
    l2 = i0;
    l4 = i0;
    i0 = l2;
    i1 = 1588u;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    i1 = l1;
    i2 = 1u;
    i1 += i2;
    l1 = i1;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i2 = a;
    i1 += i2;
    i1 = LoadU32(ctx, i1);
    l2 = i1;
    i2 = 1u;
    i1 &= i2;
    i2 = kF857Const9908B0DF;
    i1 *= i2;
    i0 ^= i1;
    l5 = i0;
    i0 = l4;
    i1 = l5;
    i2 = l2;
    i3 = kF857Const7FFFFFFE;
    i2 &= i3;
    i3 = l3;
    i2 |= i3;
    i3 = 1u;
    i2 >>= (i3 & 31);
    i1 ^= i2;
    StoreU32(ctx, i0, i1);
    i0 = l1;
    i1 = 222u;
    i0 = i0 != i1;
    if (i0) {goto loop_0;}
  i0 = a;
  i1 = a;
  i1 = LoadU32(ctx, i1 + 2476u);
  i2 = a;
  i2 = LoadU32(ctx, i2 + 892u);
  l1 = i2;
  i3 = 1u;
  i2 &= i3;
  i3 = kF857Const9908B0DF;
  i2 *= i3;
  i1 ^= i2;
  i2 = l1;
  i3 = kF857Const7FFFFFFE;
  i2 &= i3;
  i3 = a;
  i3 = LoadU32(ctx, i3 + 888u);
  i4 = kF857Const80000000;
  i3 &= i4;
  i2 |= i3;
  i3 = 1u;
  i2 >>= (i3 & 31);
  i1 ^= i2;
  StoreU32(ctx, i0 + 888u, i1);
  i0 = a;
  i1 = a;
  i1 = LoadU32(ctx, i1 + 2484u);
  i2 = a;
  i2 = LoadU32(ctx, i2 + 900u);
  l2 = i2;
  i3 = 1u;
  i2 &= i3;
  i3 = kF857Const9908B0DF;
  i2 *= i3;
  i1 ^= i2;
  i2 = l2;
  i3 = kF857Const7FFFFFFE;
  i2 &= i3;
  i3 = a;
  i3 = LoadU32(ctx, i3 + 896u);
  l3 = i3;
  i4 = kF857Const80000000;
  i3 &= i4;
  i2 |= i3;
  i3 = 1u;
  i2 >>= (i3 & 31);
  i1 ^= i2;
  StoreU32(ctx, i0 + 896u, i1);
  i0 = a;
  i1 = a;
  i1 = LoadU32(ctx, i1 + 2480u);
  i2 = l3;
  i3 = 1u;
  i2 &= i3;
  i3 = kF857Const9908B0DF;
  i2 *= i3;
  i1 ^= i2;
  i2 = l3;
  i3 = kF857Const7FFFFFFE;
  i2 &= i3;
  i3 = l1;
  i4 = kF857Const80000000;
  i3 &= i4;
  i2 |= i3;
  i3 = 1u;
  i2 >>= (i3 & 31);
  i1 ^= i2;
  StoreU32(ctx, i0 + 892u, i1);
  i0 = a;
  i0 = LoadU32(ctx, i0 + 2488u);
  i1 = a;
  i1 = LoadU32(ctx, i1 + 904u);
  l1 = i1;
  i2 = 1u;
  i1 &= i2;
  i2 = kF857Const9908B0DF;
  i1 *= i2;
  i0 ^= i1;
  l4 = i0;
  i0 = a;
  i1 = l4;
  i2 = l2;
  i3 = kF857Const80000000;
  i2 &= i3;
  i3 = l1;
  i4 = kF857Const7FFFFFFE;
  i3 &= i4;
  i2 |= i3;
  i3 = 1u;
  i2 >>= (i3 & 31);
  i1 ^= i2;
  StoreU32(ctx, i0 + 900u, i1);
  i0 = a;
  i1 = a;
  i1 = LoadU32(ctx, i1 + 2492u);
  i2 = a;
  i2 = LoadU32(ctx, i2 + 908u);
  l2 = i2;
  i3 = 1u;
  i2 &= i3;
  i3 = kF857Const9908B0DF;
  i2 *= i3;
  i1 ^= i2;
  i2 = l2;
  i3 = kF857Const7FFFFFFE;
  i2 &= i3;
  i3 = l1;
  i4 = kF857Const80000000;
  i3 &= i4;
  i2 |= i3;
  i3 = 1u;
  i2 >>= (i3 & 31);
  i1 ^= i2;
  StoreU32(ctx, i0 + 904u, i1);
  i0 = 227u;
  l1 = i0;
  loop_1: 
    i0 = l2;
    i1 = kF857Const80000000;
    i0 &= i1;
    l3 = i0;
    i0 = l1;
    i1 = 2u;
    i0 <<= (i1 & 31);
    i1 = a;
    i0 += i1;
    l2 = i0;
    l4 = i0;
    i0 = l2;
    i1 = 908u;
    i0 -= i1;
    i0 = LoadU32(ctx, i0);
    i1 = l1;
    i2 = 1u;
    i1 += i2;
    l1 = i1;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i2 = a;
    i1 += i2;
    i1 = LoadU32(ctx, i1);
    l2 = i1;
    i2 = 1u;
    i1 &= i2;
    i2 = kF857Const9908B0DF;
    i1 *= i2;
    i0 ^= i1;
    l5 = i0;
    i0 = l4;
    i1 = l5;
    i2 = l2;
    i3 = kF857Const7FFFFFFE;
    i2 &= i3;
    i3 = l3;
    i2 |= i3;
    i3 = 1u;
    i2 >>= (i3 & 31);
    i1 ^= i2;
    StoreU32(ctx, i0, i1);
    i0 = l1;
    i1 = 623u;
    i0 = i0 != i1;
    if (i0) {goto loop_1;}
  i0 = a;
  i1 = 0u;
  StoreU32(ctx, i0 + 2496u, i1);
  i0 = a;
  i1 = a;
  i1 = LoadU32(ctx, i1 + 1584u);
  i2 = a;
  i2 = LoadU32(ctx, i2);
  l1 = i2;
  i3 = 1u;
  i2 &= i3;
  i3 = kF857Const9908B0DF;
  i2 *= i3;
  i1 ^= i2;
  i2 = l1;
  i3 = kF857Const7FFFFFFE;
  i2 &= i3;
  i3 = a;
  i3 = LoadU32(ctx, i3 + 2492u);
  i4 = kF857Const80000000;
  i3 &= i4;
  i2 |= i3;
  i3 = 1u;
  i2 >>= (i3 & 31);
  i1 ^= i2;
  StoreU32(ctx, i0 + 2492u, i1);
  LeaveFrame();
}
}
