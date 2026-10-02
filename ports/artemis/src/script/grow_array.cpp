// grow_array.cpp -- grows an array: the storage is reallocated to the requested capacity and the elements are moved..
//
// GrowArray -- grows an array: the storage is reallocated to the requested capacity and the elements are moved.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 268861-268861 (f704); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void ThrowScriptError(EngineContext* ctx, uint32_t a0, uint32_t a1);
void RaiseScriptError(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

namespace {

constexpr uint32_t kF704MsgMemoryAllocationErrorBlock = 0x2AF6Eu;  // memory allocation error: block too big
constexpr uint32_t kF704ConstFFFFFFFD = 0xFFFFFFFDu;  // 
constexpr uint32_t kF704SaveFrameBytes = 36u;
constexpr uint32_t kF704ResumeWordBytes = 4u;

}  // namespace

uint32_t GrowArray(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF704SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l6 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 12u);
    d = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 16u);
    e = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 20u);
    f = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 24u);
    l7 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 28u);
    l8 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 32u);
    l10 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 4u);
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF704ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l9 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i0 = LoadU32(ctx, i0);
    l8 = i0;
    i1 = e;
    i2 = 2u;
    i1 = I32_DIV_S(i1, i2);
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    l7 = i0;
  }
  i0 = l7;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = e;
      i1 = l8;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      l7 = i0;
      if (i0) {goto block_3;}
    }
    i0 = l9;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = f;
      i2 = 0u;
      RaiseScriptError(ctx, i0, i1, i2);
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_3;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l8;
    i1 = 1u;
    i0 <<= (i1 & 31);
    e = i0;
    i1 = 4u;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    f = i0;
    i0 = e;
    i1 = 4u;
    i2 = f;
    i0 = i2 ? i0 : i1;
    e = i0;
  }
  block_3:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF704ConstFFFFFFFD;
    i1 = d;
    i0 = DIV_U(i0, i1);
    l7 = i0;
    i1 = e;
    i2 = 1u;
    i1 += i2;
    i0 = i0 >= i1;
    f = i0;
  }
  i0 = f;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 16u);
      f = i0;
      i0 = LoadU32(ctx, i0 + 16u);
      l10 = i0;
      i0 = b;
      l7 = i0;
      i0 = d;
      i1 = c;
      i1 = LoadU32(ctx, i1);
      i0 *= i1;
      l8 = i0;
      i0 = d;
      i1 = e;
      i0 *= i1;
      b = i0;
      i0 = f;
      i0 = LoadU32(ctx, i0 + 12u);
      d = i0;
    }
    i0 = l9;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l10;
      i1 = l7;
      i2 = l8;
      i3 = b;
      i4 = d;
      i0 = CallTableT7(ctx, i4, i0, i1, i2, i3);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l6 = i0;
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l6;
      d = i0;
    }
    i0 = l7;
    i1 = b;
    i1 = !(i1);
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l7 = i0;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l7;
      if (i0) {goto block_15;}
      i0 = d;
      if (i0) {goto block_15;}
    }
    i0 = l9;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = 4u;
      ThrowScriptError(ctx, i0, i1);
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    block_15:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = f;
      i1 = f;
      i1 = LoadU32(ctx, i1 + 68u);
      i2 = b;
      i3 = l8;
      i2 -= i3;
      i1 += i2;
      StoreU32(ctx, i0 + 68u, i1);
      i0 = c;
      i1 = e;
      StoreU32(ctx, i0, i1);
      i0 = d;
      goto func_exit;
    }
  }
  i0 = l9;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF704MsgMemoryAllocationErrorBlock;
    i2 = 0u;
    RaiseScriptError(ctx, i0, i1, i2);
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i1 = e;
    StoreU32(ctx, i0, i1);
    i0 = 0u;
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
  i2 = kF704ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l6 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l6;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l6;
  i1 = c;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l6;
  i1 = d;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l6;
  i1 = e;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l6;
  i1 = f;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l6;
  i1 = l7;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l6;
  i1 = l8;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l6;
  i1 = l10;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 36u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
