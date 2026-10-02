// compare_constant_operands.cpp -- compares two constants at compile time, folding the comparison when both sides are known..
//
// CompareConstantOperands -- compares two constants at compile time, folding the comparison when both sides are known.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 787934-787934 (f1703); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t CheckComparableTypes(EngineContext* ctx, uint32_t state, uint32_t left, uint32_t right);
uint32_t StringLength(EngineContext* ctx, uint32_t text);
void GrowValueStack(EngineContext* ctx, uint32_t a, uint32_t b);
uint32_t FetchObjectMetamethod(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
void CallValue(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t RawValueEqual(EngineContext* ctx, uint32_t a, uint32_t b);
uint32_t CompareScriptStrings(EngineContext* ctx, uint32_t left, uint32_t right);

namespace {

constexpr uint32_t kF1703ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF1703SaveFrameBytes = 20u;
constexpr uint32_t kF1703ResumeWordBytes = 4u;

}  // namespace

uint32_t CompareConstantOperands(EngineContext* ctx, uint32_t state, uint32_t left, uint32_t right) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  uint64_t j1 = 0;
  double d0 = 0; double d1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1703SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 4u);
    left = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    right = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1703ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = left;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i1 = right;
    i1 = LoadU32(ctx, i1 + 8u);
    i0 = i0 != i1;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    if (i0) {goto block_5;}
    i0 = l3;
    i1 = 3u;
    i0 -= i1;
    switch (i0) {
      case 0: goto block_9;
      case 1: goto block_8;
      default: goto block_7;
    }
    block_9:;
    i0 = left;
    d0 = LoadF64(ctx, i0);
    i1 = right;
    d1 = LoadF64(ctx, i1);
    i0 = d0 < d1;
    goto func_exit;
    block_8:;
    i0 = left;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i1 = 16u;
    i0 += i1;
    state = i0;
    i1 = right;
    i1 = LoadU32(ctx, i1);
    right = i1;
    i2 = 16u;
    i1 += i2;
    l4 = i1;
    i0 = CompareScriptStrings(ctx, i0, i1);
    left = i0;
    if (i0) {goto block_10;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = right;
    i0 = LoadU32(ctx, i0 + 12u);
    right = i0;
    loop_11: 
      i0 = right;
      i1 = state;
      i1 = StringLength(ctx, i1);
      left = i1;
      i0 = i0 == i1;
      if (i0) {
        i0 = 0u;
        left = i0;
        goto block_10;
      }
      i0 = left;
      i1 = l3;
      i0 = i0 == i1;
      if (i0) {
        i0 = kF1703ConstFFFFFFFF;
        left = i0;
        goto block_10;
      }
      i0 = right;
      i1 = left;
      i2 = 1u;
      i1 += i2;
      left = i1;
      i0 -= i1;
      right = i0;
      i0 = l3;
      i1 = left;
      i0 -= i1;
      l3 = i0;
      i0 = state;
      i1 = left;
      i0 += i1;
      state = i0;
      i1 = left;
      i2 = l4;
      i1 += i2;
      l4 = i1;
      i0 = CompareScriptStrings(ctx, i0, i1);
      left = i0;
      i0 = !(i0);
      if (i0) {goto loop_11;}
    block_10:;
    i0 = left;
    i1 = 31u;
    i0 >>= (i1 & 31);
    goto func_exit;
    block_7:;
    i0 = state;
    i1 = left;
    i2 = 13u;
    i0 = FetchObjectMetamethod(ctx, i0, i1, i2);
    l3 = i0;
    i0 = LoadU32(ctx, i0 + 8u);
    i0 = !(i0);
    if (i0) {goto block_5;}
    i0 = l3;
    i1 = state;
    i2 = right;
    i3 = 13u;
    i1 = FetchObjectMetamethod(ctx, i1, i2, i3);
    i0 = RawValueEqual(ctx, i0, i1);
    i0 = !(i0);
    if (i0) {goto block_5;}
    i0 = state;
    i0 = LoadU32(ctx, i0 + 32u);
    l5 = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
    i1 = l3;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0, j1);
    i0 = l4;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = state;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i1 = left;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0 + 16u, j1);
    i0 = l3;
    i1 = left;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 24u, i1);
    i0 = state;
    i0 = LoadU32(ctx, i0 + 8u);
    left = i0;
    i1 = right;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0 + 32u, j1);
    i0 = left;
    i1 = right;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 40u, i1);
    i0 = state;
    i0 = LoadU32(ctx, i0 + 28u);
    i1 = state;
    i1 = LoadU32(ctx, i1 + 8u);
    left = i1;
    i0 -= i1;
    i1 = 48u;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    right = i0;
  }
  i0 = right;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l6;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = 3u;
      GrowValueStack(ctx, i0, i1);
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = left;
    } else {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 8u);
    }
    left = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i1 = left;
    i2 = 48u;
    i1 += i2;
    right = i1;
    StoreU32(ctx, i0 + 8u, i1);
  }
  i0 = l6;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = left;
    i2 = 1u;
    CallValue(ctx, i0, i1, i2);
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i1 = state;
    i1 = LoadU32(ctx, i1 + 8u);
    i2 = 16u;
    i1 -= i2;
    left = i1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = state;
    i0 = LoadU32(ctx, i0 + 32u);
    i1 = l4;
    i2 = l5;
    i1 -= i2;
    i0 += i1;
    right = i0;
    i1 = left;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0, j1);
    i0 = right;
    i1 = left;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = state;
    i0 = LoadU32(ctx, i0 + 8u);
    state = i0;
    i0 = LoadU32(ctx, i0 + 8u);
    left = i0;
    switch (i0) {
      case 0: goto running_exit;
      case 1: goto block_21;
      default: goto block_20;
    }
    block_21:;
    i0 = state;
    i0 = LoadU32(ctx, i0);
    i1 = 0u;
    i0 = i0 != i1;
    goto func_exit;
    block_20:;
    i0 = 1u;
    goto func_exit;
  }
  block_5:;
  i0 = l6;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = left;
    i2 = right;
    i0 = CheckComparableTypes(ctx, i0, i1, i2);
    l3 = i0;
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    left = i0;
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = left;
    goto func_exit;
  }
  TrapUnreachable();
  spill_frame:;
  l3 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l3;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1703ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = left;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = right;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l3;
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l3;
  i1 = l5;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 20u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
