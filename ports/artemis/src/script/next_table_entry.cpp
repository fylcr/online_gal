// next_table_entry.cpp -- the next-pair walk: the successor of the current entry is read and the missing key raises the next error..
//
// NextTableEntry -- the next-pair walk: the successor of the current entry is read and the missing key raises the next error.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 2696967-2697395 (f5511); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void RaiseScriptError(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t RawValueEqual(EngineContext* ctx, uint32_t a, uint32_t b);

namespace {

constexpr uint32_t kF5511MsgInvalidKeyToNext = 0x2BBB9u;  // invalid key to 'next'
constexpr uint32_t kF5511Const80000000 = 0x80000000u;  // 
constexpr uint32_t kF5511ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF5511SaveFrameBytes = 16u;
constexpr uint32_t kF5511ResumeWordBytes = 4u;

}  // namespace

uint32_t NextTableEntry(EngineContext* ctx, uint32_t state, uint32_t table, uint32_t key) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint64_t l6 = 0; double l7 = 0;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  uint64_t j1 = 0; uint64_t j2 = 0; uint64_t j3 = 0;
  double d0 = 0; double d1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF5511SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    table = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    key = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF5511ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = key;
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
    i0 = kF5511ConstFFFFFFFF;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    switch (i0) {
      case 0: goto running_exit;
      case 1: goto block_9;
      case 2: goto block_8;
      case 3: goto block_11;
      case 4: goto block_10;
      default: goto block_7;
    }
    block_11:;
    i0 = key;
    d0 = LoadF64(ctx, i0);
    l7 = d0;
    d0 = wasm_fabs(d0);
    d1 = 2147483648.0;
    i0 = d0 < d1;
    if (i0) {
      d0 = l7;
      i0 = I32_TRUNC_S_F64(d0);
      goto block_13;
    }
    i0 = kF5511Const80000000;
    block_13:;
    l3 = i0;
    i1 = 1u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {goto block_12;}
    i0 = l3;
    d0 = (double)(int32_t)(i0);
    d1 = l7;
    i0 = d0 != d1;
    if (i0) {goto block_12;}
    i0 = l3;
    i1 = table;
    i1 = LoadU32(ctx, i1 + 28u);
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_12;}
    i0 = l3;
    i1 = 1u;
    i0 -= i1;
    l3 = i0;
    goto running_exit;
    block_12:;
    d0 = l7;
    d1 = 0.0;
    i0 = d0 == d1;
    if (i0) {
      i0 = table;
      i0 = LoadU32(ctx, i0 + 16u);
      goto block_6;
    }
    i0 = table;
    i0 = LoadU32(ctx, i0 + 16u);
    d1 = l7;
    j1 = i64_reinterpret_f64(d1);
    l6 = j1;
    j2 = l6;
    j3 = 32ull;
    j2 >>= (j3 & 63);
    j1 += j2;
    i1 = (uint32_t)(j1);
    i2 = kF5511ConstFFFFFFFF;
    i3 = table;
    i3 = LoadU8(ctx, i3 + 7u);
    i2 <<= (i3 & 31);
    i3 = kF5511ConstFFFFFFFF;
    i2 ^= i3;
    i3 = 1u;
    i2 |= i3;
    i1 = REM_U(i1, i2);
    i2 = 5u;
    i1 <<= (i2 & 31);
    i0 += i1;
    goto block_6;
    block_10:;
    i0 = table;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = key;
    i1 = LoadU32(ctx, i1);
    i1 = LoadU32(ctx, i1 + 8u);
    i2 = kF5511ConstFFFFFFFF;
    i3 = table;
    i3 = LoadU8(ctx, i3 + 7u);
    i2 <<= (i3 & 31);
    i3 = kF5511ConstFFFFFFFF;
    i2 ^= i3;
    i1 &= i2;
    i2 = 5u;
    i1 <<= (i2 & 31);
    i0 += i1;
    goto block_6;
    block_9:;
    i0 = table;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = key;
    i1 = LoadU32(ctx, i1);
    i2 = kF5511ConstFFFFFFFF;
    i3 = table;
    i3 = LoadU8(ctx, i3 + 7u);
    i2 <<= (i3 & 31);
    i3 = kF5511ConstFFFFFFFF;
    i2 ^= i3;
    i1 &= i2;
    i2 = 5u;
    i1 <<= (i2 & 31);
    i0 += i1;
    goto block_6;
    block_8:;
    i0 = table;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = key;
    i1 = LoadU32(ctx, i1);
    i2 = kF5511ConstFFFFFFFF;
    i3 = table;
    i3 = LoadU8(ctx, i3 + 7u);
    i2 <<= (i3 & 31);
    i3 = kF5511ConstFFFFFFFF;
    i2 ^= i3;
    i3 = 1u;
    i2 |= i3;
    i1 = REM_U(i1, i2);
    i2 = 5u;
    i1 <<= (i2 & 31);
    i0 += i1;
    goto block_6;
    block_7:;
    i0 = table;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = key;
    i1 = LoadU32(ctx, i1);
    i2 = kF5511ConstFFFFFFFF;
    i3 = table;
    i3 = LoadU8(ctx, i3 + 7u);
    i2 <<= (i3 & 31);
    i3 = kF5511ConstFFFFFFFF;
    i2 ^= i3;
    i3 = 1u;
    i2 |= i3;
    i1 = REM_U(i1, i2);
    i2 = 5u;
    i1 <<= (i2 & 31);
    i0 += i1;
    block_6:;
    l3 = i0;
    loop_16: 
      i0 = l3;
      i1 = 16u;
      i0 += i1;
      i1 = key;
      i0 = RawValueEqual(ctx, i0, i1);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 24u);
        i1 = 11u;
        i0 = i0 != i1;
        if (i0) {goto block_17;}
        i0 = key;
        i0 = LoadU32(ctx, i0 + 8u);
        i1 = 4u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        if (i0) {goto block_17;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 16u);
        i1 = key;
        i1 = LoadU32(ctx, i1);
        i0 = i0 != i1;
        if (i0) {goto block_17;}
      }
      i0 = table;
      i0 = LoadU32(ctx, i0 + 28u);
      i1 = l3;
      i2 = table;
      i2 = LoadU32(ctx, i2 + 16u);
      i1 -= i2;
      i2 = 5u;
      i1 = (uint32_t)((int32_t)i1 >> (i2 & 31));
      i0 += i1;
      l3 = i0;
      goto running_exit;
      block_17:;
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 28u);
      l3 = i0;
      if (i0) {goto loop_16;}
    i0 = 0u;
    l3 = i0;
  }
  i0 = l5;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = kF5511MsgInvalidKeyToNext;
    i2 = 0u;
    RaiseScriptError(ctx, i0, i1, i2);
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l5 = i0;
    i0 = table;
    i0 = LoadU32(ctx, i0 + 28u);
    l4 = i0;
    loop_23: 
      i0 = l3;
      state = i0;
      i0 = l3;
      i1 = 1u;
      i0 += i1;
      l3 = i0;
      i1 = l4;
      i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
      if (i0) {goto block_22;}
      i0 = table;
      i0 = LoadU32(ctx, i0 + 12u);
      i1 = l3;
      i2 = 4u;
      i1 <<= (i2 & 31);
      i0 += i1;
      i0 = LoadU32(ctx, i0 + 8u);
      i0 = !(i0);
      if (i0) {goto loop_23;}
    i0 = key;
    i1 = 3u;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = key;
    i1 = state;
    i2 = 2u;
    i1 += i2;
    d1 = (double)(int32_t)(i1);
    StoreF64(ctx, i0, d1);
    i0 = key;
    i1 = table;
    i1 = LoadU32(ctx, i1 + 12u);
    i2 = l3;
    i3 = 4u;
    i2 <<= (i3 & 31);
    i1 += i2;
    state = i1;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0 + 16u, j1);
    goto block_21;
    block_22:;
    i0 = 0u;
    i1 = l5;
    i2 = l4;
    i1 -= i2;
    i2 = l4;
    i3 = l5;
    i2 = (uint32_t)((int32_t)i2 > (int32_t)i3);
    i0 = i2 ? i0 : i1;
    l3 = i0;
    i0 = l3;
    i1 = 1u;
    i2 = table;
    i2 = LoadU8(ctx, i2 + 7u);
    i1 <<= (i2 & 31);
    l4 = i1;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {
      i0 = table;
      i0 = LoadU32(ctx, i0 + 16u);
      state = i0;
      loop_25: 
        i0 = l3;
        i1 = 5u;
        i0 <<= (i1 & 31);
        i1 = state;
        i0 += i1;
        i0 = LoadU32(ctx, i0 + 8u);
        if (i0) {
          i0 = key;
          i1 = l3;
          i2 = 5u;
          i1 <<= (i2 & 31);
          l3 = i1;
          i2 = state;
          i1 += i2;
          state = i1;
          j1 = LoadU64(ctx, i1 + 16u);
          StoreU64(ctx, i0, j1);
          i0 = key;
          i1 = state;
          i1 = LoadU32(ctx, i1 + 24u);
          StoreU32(ctx, i0 + 8u, i1);
          i0 = key;
          i1 = l3;
          i2 = table;
          i2 = LoadU32(ctx, i2 + 16u);
          i1 += i2;
          state = i1;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0 + 16u, j1);
          goto block_21;
        }
        i0 = l3;
        i1 = 1u;
        i0 += i1;
        l3 = i0;
        i1 = l4;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        if (i0) {goto loop_25;}
    }
    i0 = 0u;
    goto func_exit;
    block_21:;
    i0 = key;
    i1 = state;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 24u, i1);
    i0 = 1u;
    goto func_exit;
  }
  TrapUnreachable();
  spill_frame:;
  l4 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l4;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF5511ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = table;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l4;
  i1 = key;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l4;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 16u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
