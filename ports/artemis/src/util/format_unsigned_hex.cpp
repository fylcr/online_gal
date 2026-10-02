// format_unsigned_hex.cpp -- the hexadecimal digit writer: digits are indexed out of the uppercase table and written backwards from the end of the buffer, four bits at a time, with the caller's case and prefix mask or-ed into every byte..
//
// FormatUnsignedHex -- the hexadecimal digit writer: digits are indexed out of the uppercase table and written backwards from the end of the buffer, four bits at a time, with the caller's case and prefix mask or-ed into every byte.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 1174027-1174027 (f2753); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF2753MsgAbcdefXXX = 0xBFE60u;  // 0123456789ABCDEF-0X+0X 0X-0x+0x 0x

}  // namespace

uint32_t FormatUnsignedHex(EngineContext* ctx, uint64_t value, uint32_t dest, uint32_t or_mask) {
  EnterFrame();
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  uint64_t j0 = 0; uint64_t j1 = 0;
  j0 = value;
  i0 = !(j0);
  i0 = !(i0);
  if (i0) {
    loop_1: 
      i0 = dest;
      i1 = 1u;
      i0 -= i1;
      dest = i0;
      j1 = value;
      i1 = (uint32_t)(j1);
      i2 = 15u;
      i1 &= i2;
      i2 = kF2753MsgAbcdefXXX;
      i1 += i2;
      i1 = LoadU8(ctx, i1);
      i2 = or_mask;
      i1 |= i2;
      StoreU8(ctx, i0, i1);
      j0 = value;
      j1 = 4ull;
      j0 >>= (j1 & 63);
      value = j0;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto loop_1;}
  }
  i0 = dest;
  LeaveFrame();
  return i0;
}
}
