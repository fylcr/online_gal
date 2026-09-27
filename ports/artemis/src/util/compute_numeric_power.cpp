// compute_numeric_power.cpp -- the double power operation behind the ^ operator..
//
// ComputeNumericPower -- the double power operation behind the ^ operator.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 184340-184340 (f435); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
double EncodeSignedOrderAsDouble(EngineContext* ctx, double a0, uint32_t a1);

namespace {

constexpr uint32_t kF435Const3988F = 0x3988Fu;  // 
constexpr uint32_t kF435Const80000 = 0x80000u;  // 
constexpr uint32_t kF435ConstBB67A = 0xBB67Au;  // 
constexpr uint32_t kF435ConstBF100 = 0xBF100u;  // 
constexpr uint32_t kF435ConstBF110 = 0xBF110u;  // 
constexpr uint32_t kF435ConstBF120 = 0xBF120u;  // 
constexpr uint32_t kF435ConstFFFFF = 0xFFFFFu;  // 
constexpr uint32_t kF435Const100000 = 0x100000u;  // 
constexpr uint32_t kF435Const20000000 = 0x20000000u;  // 
constexpr uint32_t kF435Const3F6F3400 = 0x3F6F3400u;  // 
constexpr uint32_t kF435Const3FE00000 = 0x3FE00000u;  // 
constexpr uint32_t kF435Const3FE00001 = 0x3FE00001u;  // 
constexpr uint32_t kF435Const3FEFFFFE = 0x3FEFFFFEu;  // 
constexpr uint32_t kF435Const3FEFFFFF = 0x3FEFFFFFu;  // 
constexpr uint32_t kF435Const3FF00000 = 0x3FF00000u;  // 
constexpr uint32_t kF435Const3FF00001 = 0x3FF00001u;  // 
constexpr uint32_t kF435Const3FFFFFFF = 0x3FFFFFFFu;  // 
constexpr uint32_t kF435Const40000000 = 0x40000000u;  // 
constexpr uint32_t kF435Const40900000 = 0x40900000u;  // 
constexpr uint32_t kF435Const4090CC00 = 0x4090CC00u;  // 
constexpr uint32_t kF435Const41400000 = 0x41400000u;  // 
constexpr uint32_t kF435Const41E00001 = 0x41E00001u;  // 
constexpr uint32_t kF435Const433FFFFF = 0x433FFFFFu;  // 
constexpr uint32_t kF435Const43F00001 = 0x43F00001u;  // 
constexpr uint32_t kF435Const7FF00000 = 0x7FF00000u;  // 
constexpr uint32_t kF435Const7FFFFC00 = 0x7FFFFC00u;  // 
constexpr uint32_t kF435Const7FFFFFFF = 0x7FFFFFFFu;  // 
constexpr uint32_t kF435ConstFFF00000 = 0xFFF00000u;  // 
constexpr uint32_t kF435ConstFFFFFBCC = 0xFFFFFBCCu;  // 
constexpr uint32_t kF435ConstFFFFFC01 = 0xFFFFFC01u;  // 
constexpr uint32_t kF435ConstFFFFFFFF = 0xFFFFFFFFu;  // 

}  // namespace

double ComputeNumericPower(EngineContext* ctx, double base, double exponent) {
  EnterFrame();
  uint64_t l10 = 0; uint64_t l11 = 0; double l12 = 0; double l13 = 0; double l14 = 0; double l15 = 0; double l16 = 0; double l17 = 0; double l18 = 0; double l19 = 0; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u; uint32_t i7 = 0u; uint32_t i8 = 0u; uint32_t i9 = 0u;
  uint64_t j0 = 0; uint64_t j1 = 0; uint64_t j2 = 0; uint64_t j3 = 0; uint64_t j7 = 0; uint64_t j8 = 0;
  double d0 = 0; double d1 = 0; double d10 = 0; double d2 = 0; double d3 = 0; double d4 = 0; double d5 = 0; double d6 = 0; double d7 = 0; double d8 = 0; double d9 = 0;
      i9;
      d8, d9, d10;
  d0 = 1.0;
  l12 = d0;
  d0 = exponent;
  j0 = i64_reinterpret_f64(d0);
  l10 = j0;
  j1 = 32ull;
  j0 >>= (j1 & 63);
  i0 = (uint32_t)(j0);
  l3 = i0;
  i1 = kF435Const7FFFFFFF;
  i0 &= i1;
  l2 = i0;
  j1 = l10;
  i1 = (uint32_t)(j1);
  l6 = i1;
  i0 |= i1;
  i0 = !(i0);
  if (i0) {goto block_2;}
  d0 = base;
  j0 = i64_reinterpret_f64(d0);
  l11 = j0;
  j1 = 32ull;
  j0 >>= (j1 & 63);
  i0 = (uint32_t)(j0);
  l5 = i0;
  j0 = l11;
  i0 = (uint32_t)(j0);
  l9 = i0;
  i0 = !(i0);
  i1 = 0u;
  i2 = l5;
  i3 = kF435Const3FF00000;
  i2 = i2 == i3;
  i0 = i2 ? i0 : i1;
  if (i0) {goto block_2;}
  i0 = l5;
  i1 = kF435Const7FFFFFFF;
  i0 &= i1;
  l4 = i0;
  i1 = kF435Const7FF00000;
  i0 = i0 > i1;
  if (i0) {goto running_exit;}
  i0 = l4;
  i1 = kF435Const7FF00000;
  i0 = i0 == i1;
  i1 = l9;
  i2 = 0u;
  i1 = i1 != i2;
  i0 &= i1;
  if (i0) {goto running_exit;}
  i0 = l2;
  i1 = kF435Const7FF00000;
  i0 = i0 > i1;
  if (i0) {goto running_exit;}
  i0 = l6;
  i0 = !(i0);
  if (i0) {goto block_3;}
  i0 = l2;
  i1 = kF435Const7FF00000;
  i0 = i0 != i1;
  if (i0) {goto block_3;}
  running_exit:;
  d0 = base;
  d1 = exponent;
  d0 += d1;
  goto func_exit;
  block_3:;
  i0 = l5;
  i1 = kF435ConstFFFFFFFF;
  i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
  if (i0) {goto block_8;}
  i0 = 2u;
  i1 = l2;
  i2 = kF435Const433FFFFF;
  i1 = i1 > i2;
  if (i1) {goto block_7;}
  i0 = l2;
  i1 = kF435Const3FF00000;
  i0 = i0 < i1;
  if (i0) {goto block_8;}
  i0 = l2;
  i1 = 20u;
  i0 >>= (i1 & 31);
  l7 = i0;
  i0 = l2;
  i1 = kF435Const41400000;
  i0 = i0 >= i1;
  if (i0) {
    i0 = 0u;
    i1 = l6;
    i2 = l6;
    i3 = 1075u;
    i4 = l7;
    i3 -= i4;
    l8 = i3;
    i2 >>= (i3 & 31);
    l7 = i2;
    i3 = l8;
    i2 <<= (i3 & 31);
    i1 = i1 != i2;
    if (i1) {goto block_7;}
    i0 = 2u;
    i1 = l7;
    i2 = 1u;
    i1 &= i2;
    i0 -= i1;
    goto block_7;
  }
  i0 = l6;
  if (i0) {goto block_5;}
  i0 = l2;
  i1 = l2;
  i2 = 1043u;
  i3 = l7;
  i2 -= i3;
  l6 = i2;
  i1 >>= (i2 & 31);
  l7 = i1;
  i2 = l6;
  i1 <<= (i2 & 31);
  i0 = i0 != i1;
  if (i0) {goto block_6;}
  i0 = 2u;
  i1 = l7;
  i2 = 1u;
  i1 &= i2;
  i0 -= i1;
  l8 = i0;
  goto block_6;
  block_8:;
  i0 = 0u;
  block_7:;
  l8 = i0;
  i0 = l6;
  if (i0) {goto block_5;}
  block_6:;
  i0 = l2;
  i1 = kF435Const7FF00000;
  i0 = i0 == i1;
  if (i0) {
    i0 = l9;
    i1 = l4;
    i2 = kF435Const3FF00000;
    i1 -= i2;
    i0 |= i1;
    i0 = !(i0);
    if (i0) {goto block_2;}
    i0 = l4;
    i1 = kF435Const3FF00000;
    i0 = i0 >= i1;
    if (i0) {
      d0 = exponent;
      d1 = 0.0;
      i2 = l3;
      i3 = kF435ConstFFFFFFFF;
      i2 = (uint32_t)((int32_t)i2 > (int32_t)i3);
      d0 = i2 ? d0 : d1;
      goto func_exit;
    }
    d0 = 0.0;
    d1 = exponent;
    d1 = -(d1);
    i2 = l3;
    i3 = kF435ConstFFFFFFFF;
    i2 = (uint32_t)((int32_t)i2 > (int32_t)i3);
    d0 = i2 ? d0 : d1;
    goto func_exit;
  }
  i0 = l2;
  i1 = kF435Const3FF00000;
  i0 = i0 == i1;
  if (i0) {
    i0 = l3;
    i1 = kF435ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {
      d0 = base;
      goto func_exit;
    }
    d0 = 1.0;
    d1 = base;
    d0 /= d1;
    goto func_exit;
  }
  i0 = l3;
  i1 = kF435Const40000000;
  i0 = i0 == i1;
  if (i0) {
    d0 = base;
    d1 = base;
    d0 *= d1;
    goto func_exit;
  }
  i0 = l5;
  i1 = 0u;
  i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
  if (i0) {goto block_5;}
  i0 = l3;
  i1 = kF435Const3FE00000;
  i0 = i0 != i1;
  if (i0) {goto block_5;}
  d0 = base;
  d0 = wasm_sqrt(d0);
  goto func_exit;
  block_5:;
  d0 = base;
  d0 = wasm_fabs(d0);
  l12 = d0;
  i0 = l9;
  if (i0) {goto block_15;}
  i0 = l5;
  i1 = kF435Const3FFFFFFF;
  i0 &= i1;
  i1 = kF435Const3FF00000;
  i0 = i0 != i1;
  i1 = 0u;
  i2 = l4;
  i0 = i2 ? i0 : i1;
  if (i0) {goto block_15;}
  d0 = 1.0;
  d1 = l12;
  d0 /= d1;
  d1 = l12;
  i2 = l3;
  i3 = 0u;
  i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
  d0 = i2 ? d0 : d1;
  l12 = d0;
  i0 = l5;
  i1 = kF435ConstFFFFFFFF;
  i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
  if (i0) {goto block_2;}
  i0 = l4;
  i1 = kF435Const3FF00000;
  i0 -= i1;
  i1 = l8;
  i0 |= i1;
  i0 = !(i0);
  if (i0) {
    d0 = l12;
    d1 = l12;
    d0 -= d1;
    base = d0;
    d1 = base;
    d0 /= d1;
    goto func_exit;
  }
  d0 = l12;
  d0 = -(d0);
  d1 = l12;
  i2 = l8;
  i3 = 1u;
  i2 = i2 == i3;
  d0 = i2 ? d0 : d1;
  goto func_exit;
  block_15:;
  d0 = 1.0;
  l13 = d0;
  i0 = l5;
  i1 = kF435ConstFFFFFFFF;
  i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
  if (i0) {goto block_17;}
  i0 = l8;
  switch (i0) {
    case 0: goto block_19;
    case 1: goto block_18;
    default: goto block_17;
  }
  block_19:;
  d0 = base;
  d1 = base;
  d0 -= d1;
  base = d0;
  d1 = base;
  d0 /= d1;
  goto func_exit;
  block_18:;
  d0 = -1.0;
  l13 = d0;
  block_17:;
  i0 = l2;
  i1 = kF435Const41E00001;
  i0 = i0 >= i1;
  if (i0) {
    i0 = l2;
    i1 = kF435Const43F00001;
    i0 = i0 >= i1;
    if (i0) {
      i0 = l4;
      i1 = kF435Const3FEFFFFF;
      i0 = i0 <= i1;
      if (i0) {
        d0 = INFINITY;
        d1 = 0.0;
        i2 = l3;
        i3 = 0u;
        i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
        d0 = i2 ? d0 : d1;
        goto func_exit;
      }
      d0 = INFINITY;
      d1 = 0.0;
      i2 = l3;
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 > (int32_t)i3);
      d0 = i2 ? d0 : d1;
      goto func_exit;
    }
    i0 = l4;
    i1 = kF435Const3FEFFFFE;
    i0 = i0 <= i1;
    if (i0) {
      d0 = l13;
      d1 = 1.0000000000000001e+300;
      d0 *= d1;
      d1 = 1.0000000000000001e+300;
      d0 *= d1;
      d1 = l13;
      d2 = 1e-300;
      d1 *= d2;
      d2 = 1e-300;
      d1 *= d2;
      i2 = l3;
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      d0 = i2 ? d0 : d1;
      goto func_exit;
    }
    i0 = l4;
    i1 = kF435Const3FF00001;
    i0 = i0 >= i1;
    if (i0) {
      d0 = l13;
      d1 = 1.0000000000000001e+300;
      d0 *= d1;
      d1 = 1.0000000000000001e+300;
      d0 *= d1;
      d1 = l13;
      d2 = 1e-300;
      d1 *= d2;
      d2 = 1e-300;
      d1 *= d2;
      i2 = l3;
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 > (int32_t)i3);
      d0 = i2 ? d0 : d1;
      goto func_exit;
    }
    d0 = l12;
    d1 = -1.0;
    d0 += d1;
    base = d0;
    d1 = 1.4426950216293335;
    d0 *= d1;
    l12 = d0;
    d1 = base;
    d2 = 1.9259629911266175e-08;
    d1 *= d2;
    d2 = base;
    d3 = base;
    d2 *= d3;
    d3 = 0.5;
    d4 = base;
    d5 = base;
    d6 = -0.25;
    d5 *= d6;
    d6 = 0.33333333333333331;
    d5 += d6;
    d4 *= d5;
    d3 -= d4;
    d2 *= d3;
    d3 = -1.4426950408889634;
    d2 *= d3;
    d1 += d2;
    l16 = d1;
    d0 += d1;
    j0 = i64_reinterpret_f64(d0);
    j1 = 18446744069414584320ull;
    j0 &= j1;
    d0 = f64_reinterpret_i64(j0);
    base = d0;
    d1 = l12;
    d0 -= d1;
    goto block_20;
  }
  d0 = l12;
  d1 = 9007199254740992.0;
  d0 *= d1;
  base = d0;
  d1 = l12;
  i2 = l4;
  i3 = kF435Const100000;
  i2 = i2 < i3;
  l2 = i2;
  d0 = i2 ? d0 : d1;
  l12 = d0;
  d0 = base;
  j0 = i64_reinterpret_f64(d0);
  j1 = 32ull;
  j0 >>= (j1 & 63);
  i0 = (uint32_t)(j0);
  i1 = l4;
  i2 = l2;
  i0 = i2 ? i0 : i1;
  l4 = i0;
  i1 = kF435ConstFFFFF;
  i0 &= i1;
  l5 = i0;
  i1 = kF435Const3FF00000;
  i0 |= i1;
  l3 = i0;
  i0 = l4;
  i1 = 20u;
  i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
  i1 = kF435ConstFFFFFBCC;
  i2 = kF435ConstFFFFFC01;
  i3 = l2;
  i1 = i3 ? i1 : i2;
  i0 += i1;
  l4 = i0;
  i0 = 0u;
  l2 = i0;
  i0 = l5;
  i1 = kF435Const3988F;
  i0 = i0 < i1;
  if (i0) {goto block_26;}
  i0 = l5;
  i1 = kF435ConstBB67A;
  i0 = i0 < i1;
  if (i0) {
    i0 = 1u;
    l2 = i0;
    goto block_26;
  }
  i0 = l3;
  i1 = kF435ConstFFF00000;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 1u;
  i0 += i1;
  l4 = i0;
  block_26:;
  i0 = l2;
  i1 = 3u;
  i0 <<= (i1 & 31);
  l5 = i0;
  i1 = kF435ConstBF120;
  i0 += i1;
  d0 = LoadF64(ctx, i0);
  l17 = d0;
  d1 = l12;
  j1 = i64_reinterpret_f64(d1);
  j2 = 4294967295ull;
  j1 &= j2;
  i2 = l3;
  j2 = (uint64_t)(i2);
  j3 = 32ull;
  j2 <<= (j3 & 63);
  j1 |= j2;
  d1 = f64_reinterpret_i64(j1);
  l16 = d1;
  i2 = l5;
  i3 = kF435ConstBF100;
  i2 += i3;
  d2 = LoadF64(ctx, i2);
  l14 = d2;
  d1 -= d2;
  l15 = d1;
  d2 = 1.0;
  d3 = l14;
  d4 = l16;
  d3 += d4;
  d2 /= d3;
  l18 = d2;
  d1 *= d2;
  l12 = d1;
  j1 = i64_reinterpret_f64(d1);
  j2 = 18446744069414584320ull;
  j1 &= j2;
  d1 = f64_reinterpret_i64(j1);
  base = d1;
  d2 = base;
  d3 = base;
  d2 *= d3;
  l19 = d2;
  d3 = 3.0;
  d2 += d3;
  d3 = l12;
  d4 = base;
  d3 += d4;
  d4 = l18;
  d5 = l15;
  d6 = base;
  i7 = l3;
  i8 = 1u;
  i7 = (uint32_t)((int32_t)i7 >> (i8 & 31));
  i8 = kF435Const20000000;
  i7 |= i8;
  i8 = l2;
  i9 = 18u;
  i8 <<= (i9 & 31);
  i7 += i8;
  i8 = kF435Const80000;
  i7 += i8;
  j7 = (uint64_t)(i7);
  j8 = 32ull;
  j7 <<= (j8 & 63);
  d7 = f64_reinterpret_i64(j7);
  l15 = d7;
  d6 *= d7;
  d5 -= d6;
  d6 = base;
  d7 = l16;
  d8 = l15;
  d9 = l14;
  d8 -= d9;
  d7 -= d8;
  d6 *= d7;
  d5 -= d6;
  d4 *= d5;
  l16 = d4;
  d3 *= d4;
  d4 = l12;
  d5 = l12;
  d4 *= d5;
  base = d4;
  d5 = base;
  d4 *= d5;
  d5 = base;
  d6 = base;
  d7 = base;
  d8 = base;
  d9 = base;
  d10 = 0.20697501780033842;
  d9 *= d10;
  d10 = 0.23066074577556175;
  d9 += d10;
  d8 *= d9;
  d9 = 0.27272812380853401;
  d8 += d9;
  d7 *= d8;
  d8 = 0.33333332981837743;
  d7 += d8;
  d6 *= d7;
  d7 = 0.42857142857855018;
  d6 += d7;
  d5 *= d6;
  d6 = 0.59999999999999465;
  d5 += d6;
  d4 *= d5;
  d3 += d4;
  l14 = d3;
  d2 += d3;
  j2 = i64_reinterpret_f64(d2);
  j3 = 18446744069414584320ull;
  j2 &= j3;
  d2 = f64_reinterpret_i64(j2);
  base = d2;
  d1 *= d2;
  l15 = d1;
  d2 = l16;
  d3 = base;
  d2 *= d3;
  d3 = l12;
  d4 = l14;
  d5 = base;
  d6 = -3.0;
  d5 += d6;
  d6 = l19;
  d5 -= d6;
  d4 -= d5;
  d3 *= d4;
  d2 += d3;
  l12 = d2;
  d1 += d2;
  j1 = i64_reinterpret_f64(d1);
  j2 = 18446744069414584320ull;
  j1 &= j2;
  d1 = f64_reinterpret_i64(j1);
  base = d1;
  d2 = 0.96179670095443726;
  d1 *= d2;
  l14 = d1;
  i2 = l5;
  i3 = kF435ConstBF110;
  i2 += i3;
  d2 = LoadF64(ctx, i2);
  d3 = l12;
  d4 = base;
  d5 = l15;
  d4 -= d5;
  d3 -= d4;
  d4 = 0.96179669392597555;
  d3 *= d4;
  d4 = base;
  d5 = -7.0284616509527583e-09;
  d4 *= d5;
  d3 += d4;
  d2 += d3;
  l16 = d2;
  d1 += d2;
  d0 += d1;
  i1 = l4;
  d1 = (double)(int32_t)(i1);
  l12 = d1;
  d0 += d1;
  j0 = i64_reinterpret_f64(d0);
  j1 = 18446744069414584320ull;
  j0 &= j1;
  d0 = f64_reinterpret_i64(j0);
  base = d0;
  d1 = l12;
  d0 -= d1;
  d1 = l17;
  d0 -= d1;
  d1 = l14;
  d0 -= d1;
  block_20:;
  l14 = d0;
  d0 = base;
  j1 = l10;
  j2 = 18446744069414584320ull;
  j1 &= j2;
  d1 = f64_reinterpret_i64(j1);
  l17 = d1;
  d0 *= d1;
  l12 = d0;
  d1 = l16;
  d2 = l14;
  d1 -= d2;
  d2 = exponent;
  d1 *= d2;
  d2 = exponent;
  d3 = l17;
  d2 -= d3;
  d3 = base;
  d2 *= d3;
  d1 += d2;
  base = d1;
  d0 += d1;
  exponent = d0;
  j0 = i64_reinterpret_f64(d0);
  l10 = j0;
  i0 = (uint32_t)(j0);
  l2 = i0;
  j0 = l10;
  j1 = 32ull;
  j0 >>= (j1 & 63);
  i0 = (uint32_t)(j0);
  l3 = i0;
  i1 = kF435Const40900000;
  i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
  if (i0) {
    i0 = l2;
    i1 = l3;
    i2 = kF435Const40900000;
    i1 -= i2;
    i0 |= i1;
    if (i0) {goto spill_frame;}
    d0 = base;
    d1 = 8.0085662595372941e-17;
    d0 += d1;
    d1 = exponent;
    d2 = l12;
    d1 -= d2;
    i0 = d0 > d1;
    i1 = 1u;
    i0 ^= i1;
    if (i0) {goto block_28;}
    goto spill_frame;
  }
  i0 = l3;
  i1 = kF435Const7FFFFC00;
  i0 &= i1;
  i1 = kF435Const4090CC00;
  i0 = i0 < i1;
  if (i0) {goto block_28;}
  i0 = l2;
  i1 = l3;
  i2 = kF435Const3F6F3400;
  i1 += i2;
  i0 |= i1;
  if (i0) {goto block_0;}
  d0 = exponent;
  d1 = l12;
  d0 -= d1;
  d1 = base;
  i0 = d0 >= d1;
  i1 = 1u;
  i0 ^= i1;
  if (i0) {goto block_28;}
  goto block_0;
  block_28:;
  i0 = 0u;
  l2 = i0;
  d0 = l13;
  l14 = d0;
  i0 = l3;
  i1 = kF435Const7FFFFFFF;
  i0 &= i1;
  l4 = i0;
  i1 = kF435Const3FE00001;
  i0 = i0 >= i1;
  if (i0) {
    i0 = 0u;
    i1 = l3;
    i2 = kF435Const100000;
    i3 = l4;
    i4 = 20u;
    i3 >>= (i4 & 31);
    i4 = 1022u;
    i3 -= i4;
    i2 >>= (i3 & 31);
    i1 += i2;
    l4 = i1;
    i2 = kF435ConstFFFFF;
    i1 &= i2;
    i2 = kF435Const100000;
    i1 |= i2;
    i2 = 1043u;
    i3 = l4;
    i4 = 20u;
    i3 >>= (i4 & 31);
    i4 = 2047u;
    i3 &= i4;
    l5 = i3;
    i2 -= i3;
    i1 >>= (i2 & 31);
    l2 = i1;
    i0 -= i1;
    i1 = l2;
    i2 = l3;
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    l2 = i0;
    d0 = base;
    d1 = l12;
    i2 = l4;
    i3 = kF435ConstFFF00000;
    i4 = l5;
    i5 = 1023u;
    i4 -= i5;
    i3 = (uint32_t)((int32_t)i3 >> (i4 & 31));
    i2 &= i3;
    j2 = (uint64_t)(i2);
    j3 = 32ull;
    j2 <<= (j3 & 63);
    d2 = f64_reinterpret_i64(j2);
    d1 -= d2;
    l12 = d1;
    d0 += d1;
    j0 = i64_reinterpret_f64(d0);
  } else {
    j0 = l10;
  }
  j1 = 18446744069414584320ull;
  j0 &= j1;
  d0 = f64_reinterpret_i64(j0);
  exponent = d0;
  d1 = 0.69314718246459961;
  d0 *= d1;
  l13 = d0;
  d1 = base;
  d2 = exponent;
  d3 = l12;
  d2 -= d3;
  d1 -= d2;
  d2 = 0.69314718055994529;
  d1 *= d2;
  d2 = exponent;
  d3 = -1.904654299957768e-09;
  d2 *= d3;
  d1 += d2;
  l12 = d1;
  d0 += d1;
  base = d0;
  d1 = base;
  d0 *= d1;
  exponent = d0;
  d1 = 4.1381367970572385e-08;
  d0 *= d1;
  d1 = -1.6533902205465252e-06;
  d0 += d1;
  l15 = d0;
  d0 = base;
  d1 = base;
  d2 = exponent;
  d3 = exponent;
  d4 = exponent;
  d5 = exponent;
  d6 = l15;
  d5 *= d6;
  d6 = 6.6137563214379344e-05;
  d5 += d6;
  d4 *= d5;
  d5 = -0.0027777777777015593;
  d4 += d5;
  d3 *= d4;
  d4 = 0.16666666666666602;
  d3 += d4;
  d2 *= d3;
  d1 -= d2;
  exponent = d1;
  d0 *= d1;
  d1 = exponent;
  d2 = -2.0;
  d1 += d2;
  d0 /= d1;
  l15 = d0;
  d0 = base;
  d1 = l12;
  d2 = base;
  d3 = l13;
  d2 -= d3;
  d1 -= d2;
  exponent = d1;
  d0 *= d1;
  l13 = d0;
  d0 = l14;
  d1 = base;
  d2 = l15;
  d3 = exponent;
  d4 = l13;
  d3 += d4;
  d2 -= d3;
  d1 -= d2;
  d2 = 1.0;
  d1 += d2;
  base = d1;
  j1 = i64_reinterpret_f64(d1);
  l10 = j1;
  j2 = 32ull;
  j1 >>= (j2 & 63);
  i1 = (uint32_t)(j1);
  i2 = l2;
  i3 = 20u;
  i2 <<= (i3 & 31);
  i1 += i2;
  l3 = i1;
  i2 = kF435ConstFFFFF;
  i1 = (uint32_t)((int32_t)i1 <= (int32_t)i2);
  if (i1) {
    d1 = base;
    i2 = l2;
    d1 = EncodeSignedOrderAsDouble(ctx, d1, i2);
    goto block_31;
  }
  j1 = l10;
  j2 = 4294967295ull;
  j1 &= j2;
  i2 = l3;
  j2 = (uint64_t)(i2);
  j3 = 32ull;
  j2 <<= (j3 & 63);
  j1 |= j2;
  d1 = f64_reinterpret_i64(j1);
  block_31:;
  d0 *= d1;
  l12 = d0;
  block_2:;
  d0 = l12;
  goto func_exit;
  spill_frame:;
  d0 = l13;
  d1 = 1.0000000000000001e+300;
  d0 *= d1;
  d1 = 1.0000000000000001e+300;
  d0 *= d1;
  goto func_exit;
  block_0:;
  d0 = l13;
  d1 = 1e-300;
  d0 *= d1;
  d1 = 1e-300;
  d0 *= d1;
  func_exit:;
  LeaveFrame();
  return d0;
}
}
