// set_stack_top.cpp -- sets the stack top to the given slot index, filling newly exposed slots with the shared nil record..
//
// SetStackTop -- sets the stack top to the given slot index, filling newly exposed slots with the shared nil record.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 113626-113626 (f209); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF209Const27118 = 0x27118u;  // 
constexpr uint32_t kF209Const2B328 = 0x2B328u;  // 
constexpr uint32_t kF209ConstFFFFD8EE = 0xFFFFD8EEu;  // 
constexpr uint32_t kF209ConstFFFFD8F1 = 0xFFFFD8F1u;  // 

}  // namespace

void SetStackTop(EngineContext* ctx, uint32_t stack, uint32_t index) {
  EnterFrame();
  uint32_t l2 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  uint64_t j1 = 0;
  i0 = index;
  i1 = 1u;
  i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
  if (i0) {
    i0 = stack;
    i0 = LoadU32(ctx, i0 + 12u);
    i1 = index;
    i2 = 4u;
    i1 <<= (i2 & 31);
    i0 += i1;
    i1 = 16u;
    i0 -= i1;
    index = i0;
    i1 = kF209Const2B328;
    i2 = stack;
    i2 = LoadU32(ctx, i2 + 8u);
    i3 = index;
    i2 = i2 > i3;
    i0 = i2 ? i0 : i1;
    goto block_0;
  }
  i0 = index;
  i1 = kF209ConstFFFFD8F1;
  i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
  if (i0) {
    i0 = stack;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = index;
    i2 = 4u;
    i1 <<= (i2 & 31);
    i0 += i1;
    goto block_0;
  }
  i0 = index;
  i1 = 10002u;
  i0 += i1;
  switch (i0) {
    case 0: goto running_exit;
    case 1: goto block_5;
    case 2: goto block_6;
    default: goto block_3;
  }
  block_6:;
  i0 = stack;
  i0 = LoadU32(ctx, i0 + 16u);
  i1 = 96u;
  i0 += i1;
  goto block_0;
  block_5:;
  i0 = stack;
  i0 = LoadU32(ctx, i0 + 20u);
  i0 = LoadU32(ctx, i0 + 4u);
  i0 = LoadU32(ctx, i0);
  i0 = LoadU32(ctx, i0 + 12u);
  index = i0;
  i0 = stack;
  i1 = 5u;
  StoreU32(ctx, i0 + 96u, i1);
  i0 = stack;
  i1 = index;
  StoreU32(ctx, i0 + 88u, i1);
  i0 = stack;
  i1 = 88u;
  i0 += i1;
  goto block_0;
  running_exit:;
  i0 = stack;
  i1 = 72u;
  i0 += i1;
  goto block_0;
  block_3:;
  i0 = kF209Const2B328;
  i1 = stack;
  i1 = LoadU32(ctx, i1 + 20u);
  i1 = LoadU32(ctx, i1 + 4u);
  i1 = LoadU32(ctx, i1);
  l2 = i1;
  i2 = 0u;
  i3 = index;
  i2 -= i3;
  i3 = 4u;
  i2 <<= (i3 & 31);
  i1 += i2;
  i2 = kF209Const27118;
  i1 -= i2;
  i2 = l2;
  i2 = LoadU8(ctx, i2 + 7u);
  i3 = kF209ConstFFFFD8EE;
  i4 = index;
  i3 -= i4;
  i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
  i0 = i2 ? i0 : i1;
  block_0:;
  index = i0;
  i0 = stack;
  i0 = LoadU32(ctx, i0 + 8u);
  l2 = i0;
  i1 = index;
  j1 = LoadU64(ctx, i1);
  StoreU64(ctx, i0, j1);
  i0 = l2;
  i1 = index;
  i1 = LoadU32(ctx, i1 + 8u);
  StoreU32(ctx, i0 + 8u, i1);
  i0 = stack;
  i1 = stack;
  i1 = LoadU32(ctx, i1 + 8u);
  i2 = 16u;
  i1 += i2;
  StoreU32(ctx, i0 + 8u, i1);
  LeaveFrame();
}
}
