// encode_signed_order_as_double.cpp -- encodes a signed order as a double: the order key is folded into a sortable float..
//
// EncodeSignedOrderAsDouble -- encodes a signed order as a double: the order key is folded into a sortable float.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 197349-197349 (f479); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF479ConstFFFFF406 = 0xFFFFF406u;  // 
constexpr uint32_t kF479ConstFFFFF803 = 0xFFFFF803u;  // 
constexpr uint32_t kF479ConstFFFFFC01 = 0xFFFFFC01u;  // 

}  // namespace

double EncodeSignedOrderAsDouble(EngineContext* ctx, double a, uint32_t b) {
  EnterFrame();
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  uint64_t j1 = 0; uint64_t j2 = 0;
  double d0 = 0; double d1 = 0;
  i0 = b;
  i1 = 1024u;
  i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
  if (i0) {
    d0 = a;
    d1 = 8.9884656743115795e+307;
    d0 *= d1;
    a = d0;
    i0 = b;
    i1 = 2047u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {
      i0 = b;
      i1 = 1023u;
      i0 -= i1;
      b = i0;
      goto block_0;
    }
    d0 = a;
    d1 = 8.9884656743115795e+307;
    d0 *= d1;
    a = d0;
    i0 = b;
    i1 = 3069u;
    i2 = b;
    i3 = 3069u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 2046u;
    i0 -= i1;
    b = i0;
    goto block_0;
  }
  i0 = b;
  i1 = kF479ConstFFFFFC01;
  i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
  if (i0) {goto block_0;}
  d0 = a;
  d1 = 2.2250738585072014e-308;
  d0 *= d1;
  a = d0;
  i0 = b;
  i1 = kF479ConstFFFFF803;
  i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
  if (i0) {
    i0 = b;
    i1 = 1022u;
    i0 += i1;
    b = i0;
    goto block_0;
  }
  d0 = a;
  d1 = 2.2250738585072014e-308;
  d0 *= d1;
  a = d0;
  i0 = b;
  i1 = kF479ConstFFFFF406;
  i2 = b;
  i3 = kF479ConstFFFFF406;
  i2 = (uint32_t)((int32_t)i2 > (int32_t)i3);
  i0 = i2 ? i0 : i1;
  i1 = 2044u;
  i0 += i1;
  b = i0;
  block_0:;
  d0 = a;
  i1 = b;
  i2 = 1023u;
  i1 += i2;
  j1 = (uint64_t)(i1);
  j2 = 52ull;
  j1 <<= (j2 & 63);
  d1 = f64_reinterpret_i64(j1);
  d0 *= d1;
  LeaveFrame();
  return d0;
}
}
