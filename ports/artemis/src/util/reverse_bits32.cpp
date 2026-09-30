// reverse_bits32.cpp -- the 32-bit reversal behind the CRC table: each output bit takes the input bit mirrored across the word..
//
// ReverseBits32 -- the 32-bit reversal behind the CRC table: each output bit takes the input bit mirrored across the word.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 1547583-1547583 (f3308); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF3308Const20000 = 0x20000u;  // 
constexpr uint32_t kF3308Const40000 = 0x40000u;  // 
constexpr uint32_t kF3308Const80000 = 0x80000u;  // 
constexpr uint32_t kF3308Const100000 = 0x100000u;  // 
constexpr uint32_t kF3308Const200000 = 0x200000u;  // 
constexpr uint32_t kF3308Const400000 = 0x400000u;  // 
constexpr uint32_t kF3308Const800000 = 0x800000u;  // 
constexpr uint32_t kF3308Const1000000 = 0x1000000u;  // 
constexpr uint32_t kF3308Const2000000 = 0x2000000u;  // 
constexpr uint32_t kF3308Const4000000 = 0x4000000u;  // 
constexpr uint32_t kF3308Const8000000 = 0x8000000u;  // 
constexpr uint32_t kF3308Const10000000 = 0x10000000u;  // 
constexpr uint32_t kF3308Const20000000 = 0x20000000u;  // 
constexpr uint32_t kF3308Const40000000 = 0x40000000u;  // 

}  // namespace

uint32_t ReverseBits32(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i10 = 0u; uint32_t i11 = 0u; uint32_t i12 = 0u; uint32_t i13 = 0u; uint32_t i14 = 0u; uint32_t i15 = 0u; uint32_t i16 = 0u; uint32_t i17 = 0u; uint32_t i18 = 0u; uint32_t i19 = 0u; uint32_t i2 = 0u; uint32_t i20 = 0u; uint32_t i21 = 0u; uint32_t i22 = 0u; uint32_t i23 = 0u; uint32_t i24 = 0u; uint32_t i25 = 0u; uint32_t i26 = 0u; uint32_t i27 = 0u; uint32_t i28 = 0u; uint32_t i29 = 0u; uint32_t i3 = 0u; uint32_t i30 = 0u; uint32_t i31 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u; uint32_t i6 = 0u; uint32_t i7 = 0u; uint32_t i8 = 0u; uint32_t i9 = 0u;
      i8, i9, i10, i11, i12, i13, i14, i15, 
      i16, i17, i18, i19, i20, i21, i22, i23, 
      i24, i25, i26, i27, i28, i29, i30, i31;
  i0 = a;
  i1 = 29u;
  i0 >>= (i1 & 31);
  i1 = 2u;
  i0 &= i1;
  i1 = a;
  i2 = 27u;
  i1 >>= (i2 & 31);
  i2 = 4u;
  i1 &= i2;
  i2 = a;
  i3 = 25u;
  i2 >>= (i3 & 31);
  i3 = 8u;
  i2 &= i3;
  i3 = a;
  i4 = 23u;
  i3 >>= (i4 & 31);
  i4 = 16u;
  i3 &= i4;
  i4 = a;
  i5 = 21u;
  i4 >>= (i5 & 31);
  i5 = 32u;
  i4 &= i5;
  i5 = a;
  i6 = 19u;
  i5 >>= (i6 & 31);
  i6 = 64u;
  i5 &= i6;
  i6 = a;
  i7 = 17u;
  i6 >>= (i7 & 31);
  i7 = 128u;
  i6 &= i7;
  i7 = a;
  i8 = 15u;
  i7 >>= (i8 & 31);
  i8 = 256u;
  i7 &= i8;
  i8 = a;
  i9 = 13u;
  i8 >>= (i9 & 31);
  i9 = 512u;
  i8 &= i9;
  i9 = a;
  i10 = 11u;
  i9 >>= (i10 & 31);
  i10 = 1024u;
  i9 &= i10;
  i10 = a;
  i11 = 9u;
  i10 >>= (i11 & 31);
  i11 = 2048u;
  i10 &= i11;
  i11 = a;
  i12 = 7u;
  i11 >>= (i12 & 31);
  i12 = 4096u;
  i11 &= i12;
  i12 = a;
  i13 = 5u;
  i12 >>= (i13 & 31);
  i13 = 8192u;
  i12 &= i13;
  i13 = a;
  i14 = 3u;
  i13 >>= (i14 & 31);
  i14 = 16384u;
  i13 &= i14;
  i14 = a;
  i15 = 1u;
  i14 >>= (i15 & 31);
  i15 = 32768u;
  i14 &= i15;
  i15 = a;
  i16 = 1u;
  i15 <<= (i16 & 31);
  i16 = 65536u;
  i15 &= i16;
  i16 = a;
  i17 = 3u;
  i16 <<= (i17 & 31);
  i17 = kF3308Const20000;
  i16 &= i17;
  i17 = a;
  i18 = 5u;
  i17 <<= (i18 & 31);
  i18 = kF3308Const40000;
  i17 &= i18;
  i18 = a;
  i19 = 7u;
  i18 <<= (i19 & 31);
  i19 = kF3308Const80000;
  i18 &= i19;
  i19 = a;
  i20 = 9u;
  i19 <<= (i20 & 31);
  i20 = kF3308Const100000;
  i19 &= i20;
  i20 = a;
  i21 = 11u;
  i20 <<= (i21 & 31);
  i21 = kF3308Const200000;
  i20 &= i21;
  i21 = a;
  i22 = 13u;
  i21 <<= (i22 & 31);
  i22 = kF3308Const400000;
  i21 &= i22;
  i22 = a;
  i23 = 15u;
  i22 <<= (i23 & 31);
  i23 = kF3308Const800000;
  i22 &= i23;
  i23 = a;
  i24 = 17u;
  i23 <<= (i24 & 31);
  i24 = kF3308Const1000000;
  i23 &= i24;
  i24 = a;
  i25 = 19u;
  i24 <<= (i25 & 31);
  i25 = kF3308Const2000000;
  i24 &= i25;
  i25 = a;
  i26 = 21u;
  i25 <<= (i26 & 31);
  i26 = kF3308Const4000000;
  i25 &= i26;
  i26 = a;
  i27 = 23u;
  i26 <<= (i27 & 31);
  i27 = kF3308Const8000000;
  i26 &= i27;
  i27 = a;
  i28 = 25u;
  i27 <<= (i28 & 31);
  i28 = kF3308Const10000000;
  i27 &= i28;
  i28 = a;
  i29 = 27u;
  i28 <<= (i29 & 31);
  i29 = kF3308Const20000000;
  i28 &= i29;
  i29 = a;
  i30 = 29u;
  i29 <<= (i30 & 31);
  i30 = kF3308Const40000000;
  i29 &= i30;
  i30 = a;
  i31 = 31u;
  i30 <<= (i31 & 31);
  i29 |= i30;
  i28 |= i29;
  i27 |= i28;
  i26 |= i27;
  i25 |= i26;
  i24 |= i25;
  i23 |= i24;
  i22 |= i23;
  i21 |= i22;
  i20 |= i21;
  i19 |= i20;
  i18 |= i19;
  i17 |= i18;
  i16 |= i17;
  i15 |= i16;
  i14 |= i15;
  i13 |= i14;
  i12 |= i13;
  i11 |= i12;
  i10 |= i11;
  i9 |= i10;
  i8 |= i9;
  i7 |= i8;
  i6 |= i7;
  i5 |= i6;
  i4 |= i5;
  i3 |= i4;
  i2 |= i3;
  i1 |= i2;
  i0 |= i1;
  i1 = a;
  i2 = 31u;
  i1 >>= (i2 & 31);
  i0 |= i1;
  LeaveFrame();
  return i0;
}
}
