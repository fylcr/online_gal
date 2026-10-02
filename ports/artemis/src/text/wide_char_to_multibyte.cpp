// wide_char_to_multibyte.cpp -- musl wcrtomb: the wide character is encoded into at most four UTF-8 bytes (ASCII, two byte, three byte with the surrogate range refused, four byte up to 0x10FFFF), a null destination only asks for the byte count, and an unencodable character sets errno to EILSEQ and answers (uint32_t)-1..
//
// WideCharToMultibyte -- musl wcrtomb: the wide character is encoded into at most four UTF-8 bytes (ASCII, two byte, three byte with the surrogate range refused, four byte up to 0x10FFFF), a null destination only asks for the byte count, and an unencodable character sets errno to EILSEQ and answers (uint32_t)-1.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 224788-224788 (f572); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF572ConstC8524 = 0xC8524u;  // 
constexpr uint32_t kF572ConstCF7E0 = 0xCF7E0u;  // 
constexpr uint32_t kF572ConstFFFFF = 0xFFFFFu;  // 
constexpr uint32_t kF572ConstFFFFE000 = 0xFFFFE000u;  // 
constexpr uint32_t kF572ConstFFFFFF80 = 0xFFFFFF80u;  // 
constexpr uint32_t kF572ConstFFFFFFFF = 0xFFFFFFFFu;  // 

}  // namespace

uint32_t WideCharToMultibyte(EngineContext* ctx, uint32_t dest, uint32_t wide, uint32_t state) {
  EnterFrame();
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = dest;
  if (i0) {
    i0 = wide;
    i1 = 127u;
    i0 = i0 <= i1;
    if (i0) {goto block_0;}
    i0 = kF572ConstC8524;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    i0 = !(i0);
    if (i0) {
      i0 = wide;
      i1 = kF572ConstFFFFFF80;
      i0 &= i1;
      i1 = 57216u;
      i0 = i0 == i1;
      if (i0) {goto block_0;}
      goto block_2;
    }
    i0 = wide;
    i1 = 2047u;
    i0 = i0 <= i1;
    if (i0) {
      i0 = dest;
      i1 = wide;
      i2 = 63u;
      i1 &= i2;
      i2 = 128u;
      i1 |= i2;
      StoreU8(ctx, i0 + 1u, i1);
      i0 = dest;
      i1 = wide;
      i2 = 6u;
      i1 >>= (i2 & 31);
      i2 = 192u;
      i1 |= i2;
      StoreU8(ctx, i0, i1);
      i0 = 2u;
      goto func_exit;
    }
    i0 = wide;
    i1 = 55296u;
    i0 = i0 >= i1;
    i1 = 0u;
    i2 = wide;
    i3 = kF572ConstFFFFE000;
    i2 &= i3;
    i3 = 57344u;
    i2 = i2 != i3;
    i0 = i2 ? i0 : i1;
    i0 = !(i0);
    if (i0) {
      i0 = dest;
      i1 = wide;
      i2 = 63u;
      i1 &= i2;
      i2 = 128u;
      i1 |= i2;
      StoreU8(ctx, i0 + 2u, i1);
      i0 = dest;
      i1 = wide;
      i2 = 12u;
      i1 >>= (i2 & 31);
      i2 = 224u;
      i1 |= i2;
      StoreU8(ctx, i0, i1);
      i0 = dest;
      i1 = wide;
      i2 = 6u;
      i1 >>= (i2 & 31);
      i2 = 63u;
      i1 &= i2;
      i2 = 128u;
      i1 |= i2;
      StoreU8(ctx, i0 + 1u, i1);
      i0 = 3u;
      goto func_exit;
    }
    i0 = wide;
    i1 = 65536u;
    i0 -= i1;
    i1 = kF572ConstFFFFF;
    i0 = i0 <= i1;
    if (i0) {
      i0 = dest;
      i1 = wide;
      i2 = 63u;
      i1 &= i2;
      i2 = 128u;
      i1 |= i2;
      StoreU8(ctx, i0 + 3u, i1);
      i0 = dest;
      i1 = wide;
      i2 = 18u;
      i1 >>= (i2 & 31);
      i2 = 240u;
      i1 |= i2;
      StoreU8(ctx, i0, i1);
      i0 = dest;
      i1 = wide;
      i2 = 6u;
      i1 >>= (i2 & 31);
      i2 = 63u;
      i1 &= i2;
      i2 = 128u;
      i1 |= i2;
      StoreU8(ctx, i0 + 2u, i1);
      i0 = dest;
      i1 = wide;
      i2 = 12u;
      i1 >>= (i2 & 31);
      i2 = 63u;
      i1 &= i2;
      i2 = 128u;
      i1 |= i2;
      StoreU8(ctx, i0 + 1u, i1);
      i0 = 4u;
      goto func_exit;
    }
    block_2:;
    i0 = kF572ConstCF7E0;
    i1 = 25u;
    StoreU32(ctx, i0, i1);
    i0 = kF572ConstFFFFFFFF;
  } else {
    i0 = 1u;
  }
  goto func_exit;
  block_0:;
  i0 = dest;
  i1 = wide;
  StoreU8(ctx, i0, i1);
  i0 = 1u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
