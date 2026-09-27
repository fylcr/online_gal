// add_function_constant.cpp -- interns one key/value pair into the function constant table: lookup first, append on miss, and raise on overflow..
//
// AddFunctionConstant -- interns one key/value pair into the function constant table: lookup first, append on miss, and raise on overflow.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 271843-271843 (f708); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t ResolveTableField(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void StoreListEntry(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t GrowArray(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);

namespace {

constexpr uint32_t kF708MsgConstantTableOverflow = 0x2A3AFu;  // constant table overflow
constexpr uint32_t kF708Const3FFFF = 0x3FFFFu;  // 
constexpr uint32_t kF708Const80000000 = 0x80000000u;  // 
constexpr uint32_t kF708SaveFrameBytes = 32u;
constexpr uint32_t kF708ResumeWordBytes = 4u;

}  // namespace

uint32_t AddFunctionConstant(EngineContext* ctx, uint32_t func_state, uint32_t key, uint32_t value) {
  EnterFrame();
  double l10 = 0; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u;
  uint64_t j1 = 0;
  double d0 = 0; double d1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF708SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    func_state = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    value = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 24u);
    l7 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 28u);
    l9 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 4u);
    key = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF708ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l8 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 16u);
    l7 = i0;
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 4u);
    l3 = i0;
  }
  i0 = l8;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l7;
    i1 = l3;
    i2 = key;
    i0 = ResolveTableField(ctx, i0, i1, i2);
    l4 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  if (i0) {
    i0 = l9;
  } else {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = 3u;
    i0 = i0 == i1;
    if (i0) {
      i0 = l3;
      d0 = LoadF64(ctx, i0);
      l10 = d0;
      d0 = wasm_fabs(d0);
      d1 = 2147483648.0;
      i0 = d0 < d1;
      if (i0) {
        d0 = l10;
        i0 = I32_TRUNC_S_F64(d0);
        goto func_exit;
      }
      i0 = kF708Const80000000;
      goto func_exit;
    }
    i0 = func_state;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = LoadU32(ctx, i0 + 40u);
    key = i0;
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 40u);
    l6 = i0;
    i0 = l3;
    i1 = 3u;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = l6;
    d1 = (double)(int32_t)(i1);
    StoreF64(ctx, i0, d1);
    i0 = l5;
    i1 = 40u;
    i0 += i1;
    l6 = i0;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i1 = func_state;
    i1 = LoadU32(ctx, i1 + 40u);
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
  }
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i0 = LoadU32(ctx, i0 + 8u);
      l3 = i0;
    }
    i0 = l8;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l7;
      i1 = l3;
      i2 = l6;
      i3 = 16u;
      i4 = kF708Const3FFFF;
      i5 = kF708MsgConstantTableOverflow;
      i0 = GrowArray(ctx, i0, i1, i2, i3, i4, i5);
      l4 = i0;
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l3;
    } else {
      i0 = l5;
      i1 = l3;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l5;
      i0 = LoadU32(ctx, i0 + 40u);
    }
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    l6 = i0;
    i0 = key;
    i1 = l3;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {
      loop_14: 
        i0 = l6;
        i1 = key;
        i2 = 4u;
        i1 <<= (i2 & 31);
        i0 += i1;
        i1 = 0u;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l3;
        i1 = key;
        i2 = 1u;
        i1 += i2;
        key = i1;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {goto loop_14;}
    }
    i0 = l6;
    i1 = func_state;
    i1 = LoadU32(ctx, i1 + 40u);
    i2 = 4u;
    i1 <<= (i2 & 31);
    i0 += i1;
    key = i0;
    i1 = value;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0, j1);
    i0 = key;
    i1 = value;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = value;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = 4u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {goto block_15;}
    i0 = value;
    i0 = LoadU32(ctx, i0);
    key = i0;
    i0 = LoadU8(ctx, i0 + 5u);
    i1 = 3u;
    i0 &= i1;
    i0 = !(i0);
    if (i0) {goto block_15;}
    i0 = l5;
    i0 = LoadU8(ctx, i0 + 5u);
    i1 = 4u;
    i0 &= i1;
    i0 = !(i0);
    if (i0) {goto block_15;}
    i0 = l7;
    i1 = l5;
    i2 = key;
    StoreListEntry(ctx, i0, i1, i2);
    block_15:;
    i0 = func_state;
    key = i0;
    i0 = LoadU32(ctx, i0 + 40u);
    func_state = i0;
    i1 = 1u;
    i0 += i1;
    value = i0;
    i0 = key;
    i1 = value;
    StoreU32(ctx, i0 + 40u, i1);
    i0 = func_state;
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
  i2 = kF708ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  i1 = func_state;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = key;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l4;
  i1 = value;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l4;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l4;
  i1 = l5;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l4;
  i1 = l6;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l4;
  i1 = l7;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l4;
  i1 = l9;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 32u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
