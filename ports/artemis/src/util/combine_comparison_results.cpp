// combine_comparison_results.cpp -- merges the two operand results of a three-way comparison into one value..
//
// CombineComparisonResults -- merges the two operand results of a three-way comparison into one value.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 123668-123668 (f246); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t FillBytes(EngineContext* ctx, uint32_t dst, uint32_t value, uint32_t length);
uint32_t MatchCStringPrefix(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

namespace {

constexpr uint32_t kF246ConstFFFFFFC0 = 0xFFFFFFC0u;  // 
constexpr uint32_t kF246SaveFrameBytes = 24u;
constexpr uint32_t kF246ResumeWordBytes = 4u;

}  // namespace

uint32_t CombineComparisonResults(EngineContext* ctx, uint32_t left, uint32_t right, uint32_t result, uint32_t box) {
  EnterFrame();
  uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u; uint32_t i6 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF246SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    left = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 4u);
    right = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    result = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 12u);
    box = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF246ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l7 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    i1 = kF246ConstFFFFFFC0;
    i0 += i1;
    l4 = i0;
    GuestStackSp(ctx) = i0;
    i0 = left;
    i0 = LoadU32(ctx, i0);
    l6 = i0;
    i1 = 4u;
    i0 -= i1;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = l6;
    i1 = 8u;
    i0 -= i1;
    i0 = LoadU32(ctx, i0);
    l6 = i0;
    i0 = l4;
    i1 = box;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = l4;
    i1 = right;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = l4;
    i1 = left;
    StoreU32(ctx, i0 + 12u, i1);
    i0 = l4;
    i1 = result;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = 0u;
    right = i0;
    i0 = l4;
    i1 = 24u;
    i0 += i1;
    i1 = 0u;
    i2 = 39u;
    i0 = FillBytes(ctx, i0, i1, i2);
    box = i0;
    i0 = l5;
    i1 = result;
    i2 = 0u;
    i0 = MatchCStringPrefix(ctx, i0, i1, i2);
    result = i0;
    i0 = left;
    i1 = l6;
    i0 += i1;
    left = i0;
  }
  i0 = result;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = 1u;
      StoreU32(ctx, i0 + 56u, i1);
      i0 = l5;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 20u);
      result = i0;
      i0 = l4;
      i1 = 8u;
      i0 += i1;
      right = i0;
    }
    i0 = l7;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l5;
      i1 = right;
      i2 = left;
      i3 = left;
      i4 = 1u;
      i5 = 0u;
      i6 = result;
      CallTableT15(ctx, i6, i0, i1, i2, i3, i4, i5);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = left;
      i1 = 0u;
      i2 = l4;
      i2 = LoadU32(ctx, i2 + 32u);
      i3 = 1u;
      i2 = i2 == i3;
      i0 = i2 ? i0 : i1;
      right = i0;
      goto running_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 24u);
    box = i0;
    i0 = l4;
    i1 = 8u;
    i0 += i1;
    result = i0;
  }
  i0 = l7;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l5;
    i1 = result;
    i2 = left;
    i3 = 1u;
    i4 = 0u;
    i5 = box;
    CallTableT11(ctx, i5, i0, i1, i2, i3, i4);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 44u);
    switch (i0) {
      case 0: goto block_13;
      case 1: goto block_12;
      default: goto running_exit;
    }
    block_13:;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 28u);
    i1 = 0u;
    i2 = l4;
    i2 = LoadU32(ctx, i2 + 40u);
    i3 = 1u;
    i2 = i2 == i3;
    i0 = i2 ? i0 : i1;
    i1 = 0u;
    i2 = l4;
    i2 = LoadU32(ctx, i2 + 36u);
    i3 = 1u;
    i2 = i2 == i3;
    i0 = i2 ? i0 : i1;
    i1 = 0u;
    i2 = l4;
    i2 = LoadU32(ctx, i2 + 48u);
    i3 = 1u;
    i2 = i2 == i3;
    i0 = i2 ? i0 : i1;
    right = i0;
    goto running_exit;
    block_12:;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 32u);
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 48u);
      if (i0) {goto running_exit;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 36u);
      i1 = 1u;
      i0 = i0 != i1;
      if (i0) {goto running_exit;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 40u);
      i1 = 1u;
      i0 = i0 != i1;
      if (i0) {goto running_exit;}
    }
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 24u);
    right = i0;
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = kF246ConstFFFFFFC0;
    i0 -= i1;
    GuestStackSp(ctx) = i0;
    i0 = right;
    goto func_exit;
  }
  TrapUnreachable();
  spill_frame:;
  l6 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l6;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF246ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l6 = i0;
  i1 = left;
  StoreU32(ctx, i0, i1);
  i0 = l6;
  i1 = right;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l6;
  i1 = result;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l6;
  i1 = box;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l6;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l6;
  i1 = l5;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 24u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
