// convert_value_to_integer.cpp -- narrows a TValue to its integer payload, raising when the tag has no integer form..
//
// ConvertValueToInteger -- narrows a TValue to its integer payload, raising when the tag has no integer form.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 190700-190700 (f453); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t MaterializeNumberValue(EngineContext* ctx, uint32_t state, uint32_t slot);

namespace {

constexpr uint32_t kF453Const27118 = 0x27118u;  // 
constexpr uint32_t kF453Const2B328 = 0x2B328u;  // 
constexpr uint32_t kF453Const80000000 = 0x80000000u;  // 
constexpr uint32_t kF453ConstFFFFD8EE = 0xFFFFD8EEu;  // 
constexpr uint32_t kF453ConstFFFFD8F1 = 0xFFFFD8F1u;  // 
constexpr uint32_t kF453SaveFrameBytes = 8u;
constexpr uint32_t kF453ResumeWordBytes = 4u;

}  // namespace

uint32_t ConvertValueToInteger(EngineContext* ctx, uint32_t state, uint32_t slot) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; double l4 = 0;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u; uint32_t i6 = 0u;
  double d0 = 0; double d1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF453SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0);
    slot = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 4u);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF453ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  i1 = 0u;
  i2 = ResumePhase(ctx);
  if (i2) {
    i2 = state;
  } else {
    i2 = GuestStackSp(ctx);
    i3 = 16u;
    i2 -= i3;
    l2 = i2;
    GuestStackSp(ctx) = i2;
    i2 = slot;
    i3 = 1u;
    i2 = (uint32_t)((int32_t)i2 >= (int32_t)i3);
    if (i2) {
      i2 = state;
      i2 = LoadU32(ctx, i2 + 12u);
      i3 = slot;
      i4 = 4u;
      i3 <<= (i4 & 31);
      i2 += i3;
      i3 = 16u;
      i2 -= i3;
      slot = i2;
      i3 = kF453Const2B328;
      i4 = slot;
      i5 = state;
      i5 = LoadU32(ctx, i5 + 8u);
      i4 = i4 < i5;
      i2 = i4 ? i2 : i3;
      goto block_6;
    }
    i2 = slot;
    i3 = kF453ConstFFFFD8F1;
    i2 = (uint32_t)((int32_t)i2 >= (int32_t)i3);
    if (i2) {
      i2 = state;
      i2 = LoadU32(ctx, i2 + 8u);
      i3 = slot;
      i4 = 4u;
      i3 <<= (i4 & 31);
      i2 += i3;
      goto block_6;
    }
    i2 = slot;
    i3 = 10002u;
    i2 += i3;
    switch (i2) {
      case 0: goto block_10;
      case 1: goto block_11;
      case 2: goto block_12;
      default: goto block_9;
    }
    block_12:;
    i2 = state;
    i2 = LoadU32(ctx, i2 + 16u);
    i3 = 96u;
    i2 += i3;
    goto block_6;
    block_11:;
    i2 = state;
    i2 = LoadU32(ctx, i2 + 20u);
    i2 = LoadU32(ctx, i2 + 4u);
    i2 = LoadU32(ctx, i2);
    i2 = LoadU32(ctx, i2 + 12u);
    slot = i2;
    i2 = state;
    i3 = 5u;
    StoreU32(ctx, i2 + 96u, i3);
    i2 = state;
    i3 = slot;
    StoreU32(ctx, i2 + 88u, i3);
    i2 = state;
    i3 = 88u;
    i2 += i3;
    goto block_6;
    block_10:;
    i2 = state;
    i3 = 72u;
    i2 += i3;
    goto block_6;
    block_9:;
    i2 = kF453Const2B328;
    i3 = state;
    i3 = LoadU32(ctx, i3 + 20u);
    i3 = LoadU32(ctx, i3 + 4u);
    i3 = LoadU32(ctx, i3);
    state = i3;
    i4 = 0u;
    i5 = slot;
    i4 -= i5;
    i5 = 4u;
    i4 <<= (i5 & 31);
    i3 += i4;
    i4 = kF453Const27118;
    i3 -= i4;
    i4 = state;
    i4 = LoadU8(ctx, i4 + 7u);
    i5 = kF453ConstFFFFD8EE;
    i6 = slot;
    i5 -= i6;
    i4 = (uint32_t)((int32_t)i4 < (int32_t)i5);
    i2 = i4 ? i2 : i3;
    block_6:;
    slot = i2;
    i2 = LoadU32(ctx, i2 + 8u);
    i3 = 3u;
    i2 = i2 == i3;
  }
  i0 = i2 ? i0 : i1;
  if (i0) {goto running_exit;}
  i0 = l3;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = slot;
    i1 = l2;
    i0 = MaterializeNumberValue(ctx, i0, i1);
    state = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = state;
    slot = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = slot;
    if (i0) {goto running_exit;}
    i0 = 0u;
    goto block_3;
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  if (i0) {
    i0 = slot;
  } else {
    i0 = slot;
    d0 = LoadF64(ctx, i0);
    l4 = d0;
    d0 = wasm_fabs(d0);
    d1 = 2147483648.0;
    i0 = d0 < d1;
    if (i0) {
      d0 = l4;
      i0 = I32_TRUNC_S_F64(d0);
      goto block_3;
    }
    i0 = kF453Const80000000;
  }
  block_3:;
  slot = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i1 = 16u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    i0 = slot;
    goto func_exit;
  }
  TrapUnreachable();
  spill_frame:;
  state = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF453ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  state = i0;
  i1 = slot;
  StoreU32(ctx, i0, i1);
  i0 = state;
  i1 = l2;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 8u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
