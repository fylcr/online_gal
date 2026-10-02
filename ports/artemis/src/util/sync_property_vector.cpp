// sync_property_vector.cpp -- compares the cached property vector against the incoming one, takes the equality adapter for the pair when it drifts, and folds the dirty bits back afterwards..
//
// SyncPropertyVector -- compares the cached property vector against the incoming one, takes the equality adapter for the pair when it drifts, and folds the dirty bits back afterwards.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 2581536-2581947 (f4715); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF4715ResumeWordBytes = 4u;

}  // namespace

void SyncPropertyVector(EngineContext* ctx, uint32_t state, uint32_t index, float scale, uint32_t flag) {
  EnterFrame();
  uint32_t l4 = 0u; uint32_t l5 = 0u; float l6 = 0; float l7 = 0; float l8 = 0; float l9 = 0;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  float f0 = 0; float f1 = 0; float f2 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = 20u;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 4u);
    index = i0;
    i0 = l4;
    f0 = LoadF32(ctx, i0 + 8u);
    scale = f0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    flag = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF4715ResumeWordBytes;
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
    i0 = state;
    f0 = LoadF32(ctx, i0 + 24u);
    f1 = 0.0f;
    i0 = f0 <= f1;
    if (i0) {goto block_3;}
    i0 = state;
    f0 = LoadF32(ctx, i0 + 28u);
    f1 = 0.0f;
    i0 = f0 <= f1;
    if (i0) {goto block_3;}
    i0 = index;
    f0 = LoadF32(ctx, i0 + 8u);
    l6 = f0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 40u);
    l4 = i0;
    i0 = index;
    f0 = LoadF32(ctx, i0 + 4u);
    l9 = f0;
    i1 = state;
    f1 = LoadF32(ctx, i1 + 88u);
    f0 -= f1;
    f0 = wasm_fabsf(f0);
    f1 = 1.1920929e-07;
    i0 = f0 >= f1;
    if (i0) {goto block_8;}
    f0 = l6;
    i1 = state;
    f1 = LoadF32(ctx, i1 + 92u);
    f0 -= f1;
    f0 = wasm_fabsf(f0);
    f1 = 1.1920929e-07;
    i0 = f0 >= f1;
    if (i0) {goto block_8;}
    i0 = index;
    f0 = LoadF32(ctx, i0 + 12u);
    i1 = state;
    f1 = LoadF32(ctx, i1 + 96u);
    f0 -= f1;
    f0 = wasm_fabsf(f0);
    f1 = 1.1920929e-07;
    i0 = f0 >= f1;
    if (i0) {goto block_9;}
    i0 = index;
    f0 = LoadF32(ctx, i0 + 16u);
    i1 = state;
    f1 = LoadF32(ctx, i1 + 100u);
    f0 -= f1;
    f0 = wasm_fabsf(f0);
    f1 = 1.1920929e-07;
    i0 = f0 >= f1;
    if (i0) {goto block_9;}
    i0 = index;
    f0 = LoadF32(ctx, i0 + 20u);
    i1 = state;
    f1 = LoadF32(ctx, i1 + 104u);
    f0 -= f1;
    f0 = wasm_fabsf(f0);
    f1 = 1.1920929e-07;
    i0 = f0 >= f1;
    if (i0) {goto block_9;}
    i0 = index;
    f0 = LoadF32(ctx, i0 + 24u);
    i1 = state;
    f1 = LoadF32(ctx, i1 + 108u);
    f0 -= f1;
    f0 = wasm_fabsf(f0);
    f1 = 1.1920929e-07;
    i0 = f0 >= f1;
    i1 = 1u;
    i0 ^= i1;
    if (i0) {goto block_6;}
    block_9:;
    i0 = index;
    f0 = LoadF32(ctx, i0 + 16u);
    l7 = f0;
    i1 = state;
    f1 = LoadF32(ctx, i1 + 100u);
    f0 -= f1;
    f0 = wasm_fabsf(f0);
    f1 = 1.1920929e-07;
    i0 = f0 >= f1;
    if (i0) {goto block_8;}
    i0 = index;
    f0 = LoadF32(ctx, i0 + 20u);
    l8 = f0;
    i1 = state;
    f1 = LoadF32(ctx, i1 + 104u);
    f0 -= f1;
    f0 = wasm_fabsf(f0);
    f1 = 1.1920929e-07;
    i0 = f0 >= f1;
    i1 = 1u;
    i0 ^= i1;
    i0 = !(i0);
    if (i0) {goto block_8;}
    i0 = l4;
    i1 = 1u;
    i0 |= i1;
    goto block_7;
    block_8:;
    i0 = index;
    f0 = LoadF32(ctx, i0 + 20u);
    l8 = f0;
    i0 = index;
    f0 = LoadF32(ctx, i0 + 16u);
    l7 = f0;
    i0 = l4;
    i1 = 5u;
    i0 |= i1;
    block_7:;
    l4 = i0;
    i0 = state;
    f1 = l9;
    StoreF32(ctx, i0 + 88u, f1);
    i0 = state;
    f1 = l6;
    StoreF32(ctx, i0 + 92u, f1);
    i0 = index;
    f0 = LoadF32(ctx, i0 + 12u);
    l6 = f0;
    i0 = state;
    f1 = l8;
    StoreF32(ctx, i0 + 104u, f1);
    i0 = state;
    f1 = l7;
    StoreF32(ctx, i0 + 100u, f1);
    i0 = state;
    f1 = l6;
    StoreF32(ctx, i0 + 96u, f1);
    i0 = state;
    i1 = index;
    f1 = LoadF32(ctx, i1 + 24u);
    StoreF32(ctx, i0 + 108u, f1);
    block_6:;
    i0 = l4;
    i1 = 1u;
    i0 &= i1;
    index = i0;
  }
  i0 = index;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 152u);
      index = i0;
    }
    i0 = l5;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = index;
      CallTableT1(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i1 = state;
    i1 = LoadU32(ctx, i1 + 52u);
    i2 = 8u;
    i1 |= i2;
    index = i1;
    StoreU32(ctx, i0 + 52u, i1);
    i0 = state;
    i0 = LoadU8(ctx, i0 + 38u);
    i0 = !(i0);
    if (i0) {goto block_3;}
    i0 = flag;
    i0 = !(i0);
    flag = i0;
    i0 = state;
    f0 = LoadF32(ctx, i0 + 12u);
    f1 = scale;
    f0 *= f1;
    scale = f0;
  }
  i0 = flag;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i1 = 56u;
      i0 += i1;
      flag = i0;
      i0 = state;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 156u);
      index = i0;
    }
    i0 = l5;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = flag;
      f2 = scale;
      i3 = l4;
      i4 = index;
      CallTableT16(ctx, i4, i0, i1, f2, i3);
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
      goto func_exit;
    }
  }
  i0 = flag;
  f1 = scale;
  f2 = 1.1920929e-07;
  i1 = f1 >= f2;
  i2 = 1u;
  i1 ^= i2;
  i1 = !(i1);
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  flag = i0;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i1 = 56u;
      i0 += i1;
      flag = i0;
      i0 = state;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 160u);
      index = i0;
    }
    i0 = l5;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = flag;
      f2 = scale;
      i3 = l4;
      i4 = index;
      CallTableT16(ctx, i4, i0, i1, f2, i3);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = index;
    } else {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 44u);
      i1 = state;
      i1 = LoadU32(ctx, i1 + 52u);
      i0 |= i1;
    }
    index = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i1 = state;
    i1 = LoadU32(ctx, i1 + 48u);
    i2 = index;
    i1 |= i2;
    StoreU32(ctx, i0 + 52u, i1);
  }
  block_3:;
  goto func_exit;
  spill_frame:;
  l5 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l5;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF4715ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l5 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = index;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l5;
  f1 = scale;
  StoreF32(ctx, i0 + 8u, f1);
  i0 = l5;
  i1 = flag;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l5;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 20u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
