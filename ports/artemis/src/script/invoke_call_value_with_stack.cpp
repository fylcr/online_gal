// invoke_call_value_with_stack.cpp -- invokes a callable with the stack window: the arguments come from the value stack and the call is entered..
//
// InvokeCallValueWithStack -- invokes a callable with the stack window: the arguments come from the value stack and the call is entered.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 820175-820175 (f1742); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void ThrowScriptError(EngineContext* ctx, uint32_t a0, uint32_t a1);
void GrowValueStack(EngineContext* ctx, uint32_t a, uint32_t b);
void CallValue(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

namespace {

constexpr uint32_t kF1742SaveFrameBytes = 12u;
constexpr uint32_t kF1742ResumeWordBytes = 4u;

}  // namespace

void InvokeCallValueWithStack(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint64_t l5 = 0;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  uint64_t j0 = 0; uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1742SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 4u);
    l1 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1742ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 116u);
    l1 = i0;
  }
  i0 = l1;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l1;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 32u);
      i0 += i1;
      l1 = i0;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = 6u;
      i0 = i0 != i1;
      l2 = i0;
    }
    i0 = l2;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l3;
      i0 = !(i0);
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = 5u;
        ThrowScriptError(ctx, i0, i1);
        i0 = 0u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l2;
    } else {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 8u);
      l2 = i0;
      i1 = 16u;
      i0 -= i1;
      l4 = i0;
      j0 = LoadU64(ctx, i0);
      l5 = j0;
      i0 = l2;
      j1 = l5;
      StoreU64(ctx, i0, j1);
      i0 = l2;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = a;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = 16u;
      i0 -= i1;
      l2 = i0;
      i1 = l1;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = l2;
      i1 = l1;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = a;
      i0 = LoadU32(ctx, i0 + 28u);
      i1 = a;
      i1 = LoadU32(ctx, i1 + 8u);
      l1 = i1;
      i0 -= i1;
      i1 = 16u;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    }
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = 1u;
        GrowValueStack(ctx, i0, i1);
        i0 = 1u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l1;
      } else {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 8u);
      }
      l1 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = l1;
      i2 = 16u;
      i1 += i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l1;
      i1 = 16u;
      i0 -= i1;
      l1 = i0;
    }
    i0 = l3;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = l1;
      i2 = 1u;
      CallValue(ctx, i0, i1, i2);
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
  }
  i0 = l3;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 2u;
    ThrowScriptError(ctx, i0, i1);
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  goto func_exit;
  spill_frame:;
  l3 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l3;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1742ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = l1;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 12u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
