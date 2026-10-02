// init_crc32_table.cpp -- the lazy CRC-32 table builder: every one of the 256 entries reflects its own index byte, runs eight rounds of the 0x04C11DB7 polynomial and stores the reversed word, once, behind the initialised flag..
//
// InitCrc32Table -- the lazy CRC-32 table builder: every one of the 256 entries reflects its own index byte, runs eight rounds of the 0x04C11DB7 polynomial and stores the reversed word, once, behind the initialised flag.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 628707-628707 (f1466); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t ReverseBits32(EngineContext* ctx, uint32_t a);

namespace {

constexpr uint32_t kF1466ConstCF224 = 0xCF224u;  // 
constexpr uint32_t kF1466ConstCF230 = 0xCF230u;  // 
constexpr uint32_t kF1466Const4C11DB7 = 0x4C11DB7u;  // 
constexpr uint32_t kF1466Const9823B6E = 0x9823B6Eu;  // 
constexpr uint32_t kF1466Const130476DC = 0x130476DCu;  // 
constexpr uint32_t kF1466Const2608EDB8 = 0x2608EDB8u;  // 
constexpr uint32_t kF1466Const80000000 = 0x80000000u;  // 
constexpr uint32_t kF1466ConstFFFFFFFF = 0xFFFFFFFFu;  // 

}  // namespace

void InitCrc32Table(EngineContext* ctx) {
  EnterFrame();
  uint32_t l0 = 0u; uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u;
  i0 = kF1466ConstCF224;
  i0 = LoadU8(ctx, i0);
  i0 = !(i0);
  if (i0) {
    loop_1: 
      i0 = l1;
      i1 = 1u;
      i0 <<= (i1 & 31);
      i1 = 16u;
      i0 &= i1;
      i1 = l1;
      i2 = 3u;
      i1 <<= (i2 & 31);
      i2 = 32u;
      i1 &= i2;
      i2 = l1;
      i3 = 5u;
      i2 <<= (i3 & 31);
      i3 = 64u;
      i2 &= i3;
      i3 = l1;
      i4 = 7u;
      i3 <<= (i4 & 31);
      i4 = l1;
      i5 = 255u;
      i4 &= i5;
      l4 = i4;
      i5 = 7u;
      i4 >>= (i5 & 31);
      i3 |= i4;
      i2 |= i3;
      i1 |= i2;
      i0 |= i1;
      i1 = l4;
      i2 = 1u;
      i1 >>= (i2 & 31);
      i2 = 8u;
      i1 &= i2;
      i0 |= i1;
      i1 = l4;
      i2 = 3u;
      i1 >>= (i2 & 31);
      i2 = 4u;
      i1 &= i2;
      i0 |= i1;
      i1 = l4;
      i2 = 5u;
      i1 >>= (i2 & 31);
      i2 = 2u;
      i1 &= i2;
      i0 |= i1;
      i1 = 255u;
      i0 &= i1;
      i1 = 2u;
      i0 <<= (i1 & 31);
      i1 = kF1466ConstCF230;
      i0 += i1;
      l5 = i0;
      i0 = 0u;
      i1 = kF1466Const2608EDB8;
      i2 = l1;
      i3 = 24u;
      i2 <<= (i3 & 31);
      i3 = 24u;
      i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
      i3 = kF1466ConstFFFFFFFF;
      i2 = (uint32_t)((int32_t)i2 > (int32_t)i3);
      i0 = i2 ? i0 : i1;
      l0 = i0;
      i1 = kF1466Const130476DC;
      i0 ^= i1;
      i1 = l0;
      i2 = l1;
      i3 = 64u;
      i2 &= i3;
      i0 = i2 ? i0 : i1;
      l0 = i0;
      i1 = kF1466Const9823B6E;
      i0 ^= i1;
      i1 = l0;
      i2 = l1;
      i3 = 32u;
      i2 &= i3;
      i0 = i2 ? i0 : i1;
      l0 = i0;
      i1 = kF1466Const4C11DB7;
      i0 ^= i1;
      i1 = l0;
      i2 = l1;
      i3 = 16u;
      i2 &= i3;
      i0 = i2 ? i0 : i1;
      l0 = i0;
      i1 = kF1466Const80000000;
      i0 ^= i1;
      i1 = l0;
      i2 = l1;
      i3 = 8u;
      i2 &= i3;
      i0 = i2 ? i0 : i1;
      l0 = i0;
      i1 = 1u;
      i0 <<= (i1 & 31);
      l2 = i0;
      i1 = kF1466Const4C11DB7;
      i0 ^= i1;
      l3 = i0;
      i0 = l2;
      i1 = l3;
      i2 = l0;
      i3 = kF1466ConstFFFFFFFF;
      i2 = (uint32_t)((int32_t)i2 > (int32_t)i3);
      i0 = i2 ? i0 : i1;
      l0 = i0;
      i1 = kF1466Const80000000;
      i0 ^= i1;
      i1 = l0;
      i2 = l1;
      i3 = 4u;
      i2 &= i3;
      i0 = i2 ? i0 : i1;
      l0 = i0;
      i1 = 1u;
      i0 <<= (i1 & 31);
      l2 = i0;
      i1 = kF1466Const4C11DB7;
      i0 ^= i1;
      l3 = i0;
      i0 = l2;
      i1 = l3;
      i2 = l0;
      i3 = kF1466ConstFFFFFFFF;
      i2 = (uint32_t)((int32_t)i2 > (int32_t)i3);
      i0 = i2 ? i0 : i1;
      l0 = i0;
      i1 = kF1466Const80000000;
      i0 ^= i1;
      i1 = l0;
      i2 = l1;
      i3 = 2u;
      i2 &= i3;
      i0 = i2 ? i0 : i1;
      l0 = i0;
      i1 = 1u;
      i0 <<= (i1 & 31);
      l2 = i0;
      i1 = kF1466Const4C11DB7;
      i0 ^= i1;
      l3 = i0;
      i0 = l2;
      i1 = l3;
      i2 = l0;
      i3 = kF1466ConstFFFFFFFF;
      i2 = (uint32_t)((int32_t)i2 > (int32_t)i3);
      i0 = i2 ? i0 : i1;
      l0 = i0;
      i1 = kF1466Const80000000;
      i0 ^= i1;
      i1 = l0;
      i2 = l1;
      i3 = 1u;
      i2 &= i3;
      i0 = i2 ? i0 : i1;
      l1 = i0;
      i1 = 1u;
      i0 <<= (i1 & 31);
      l0 = i0;
      i1 = kF1466Const4C11DB7;
      i0 ^= i1;
      l2 = i0;
      i0 = l5;
      i1 = l0;
      i2 = l2;
      i3 = l1;
      i4 = kF1466ConstFFFFFFFF;
      i3 = (uint32_t)((int32_t)i3 > (int32_t)i4);
      i1 = i3 ? i1 : i2;
      i1 = ReverseBits32(ctx, i1);
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l1 = i0;
      i1 = 255u;
      i0 &= i1;
      i1 = l1;
      i0 = i0 == i1;
      if (i0) {goto loop_1;}
    i0 = kF1466ConstCF224;
    i1 = 1u;
    StoreU8(ctx, i0, i1);
  }
  LeaveFrame();
}
}
