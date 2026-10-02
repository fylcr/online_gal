// resolve_expr_value.cpp -- resolves the value slot an expression tag names, clamping each family of tags to its own low and high bound and falling back to the shared empty cell when the tag is out of range..
//
// ResolveExprValue -- resolves the value slot an expression tag names, clamping each family of tags to its own low and high bound and falling back to the shared empty cell when the tag is out of range.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 128598-128729 (f264); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF264Const27118 = 0x27118u;  // 
constexpr uint32_t kF264Const2B328 = 0x2B328u;  // 
constexpr uint32_t kF264ConstFFFFD8EE = 0xFFFFD8EEu;  // 
constexpr uint32_t kF264ConstFFFFD8F1 = 0xFFFFD8F1u;  // 

}  // namespace

uint32_t ResolveExprValue(EngineContext* ctx, uint32_t state, uint32_t tag) {
  EnterFrame();
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  i0 = tag;
  i1 = 1u;
  i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 12u);
    i1 = tag;
    i2 = 4u;
    i1 <<= (i2 & 31);
    i0 += i1;
    i1 = 16u;
    i0 -= i1;
    tag = i0;
    i1 = kF264Const2B328;
    i2 = tag;
    i3 = state;
    i3 = LoadU32(ctx, i3 + 8u);
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    goto block_0;
  }
  i0 = tag;
  i1 = kF264ConstFFFFD8F1;
  i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = tag;
    i2 = 4u;
    i1 <<= (i2 & 31);
    i0 += i1;
    goto block_0;
  }
  i0 = tag;
  i1 = 10002u;
  i0 += i1;
  switch (i0) {
    case 0: goto running_exit;
    case 1: goto block_5;
    case 2: goto block_6;
    default: goto block_3;
  }
  block_6:;
  i0 = state;
  i0 = LoadU32(ctx, i0 + 16u);
  i1 = 96u;
  i0 += i1;
  goto block_0;
  block_5:;
  i0 = state;
  i0 = LoadU32(ctx, i0 + 20u);
  i0 = LoadU32(ctx, i0 + 4u);
  i0 = LoadU32(ctx, i0);
  i0 = LoadU32(ctx, i0 + 12u);
  tag = i0;
  i0 = state;
  i1 = 5u;
  StoreU32(ctx, i0 + 96u, i1);
  i0 = state;
  i1 = tag;
  StoreU32(ctx, i0 + 88u, i1);
  i0 = state;
  i1 = 88u;
  i0 += i1;
  goto block_0;
  running_exit:;
  i0 = state;
  i1 = 72u;
  i0 += i1;
  goto block_0;
  block_3:;
  i0 = kF264Const2B328;
  i1 = state;
  i1 = LoadU32(ctx, i1 + 20u);
  i1 = LoadU32(ctx, i1 + 4u);
  i1 = LoadU32(ctx, i1);
  state = i1;
  i2 = 0u;
  i3 = tag;
  i2 -= i3;
  i3 = 4u;
  i2 <<= (i3 & 31);
  i1 += i2;
  i2 = kF264Const27118;
  i1 -= i2;
  i2 = state;
  i2 = LoadU8(ctx, i2 + 7u);
  i3 = kF264ConstFFFFD8EE;
  i4 = tag;
  i3 -= i4;
  i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
  i0 = i2 ? i0 : i1;
  block_0:;
  tag = i0;
  i0 = 0u;
  state = i0;
  i0 = tag;
  i0 = LoadU32(ctx, i0 + 8u);
  i1 = 2u;
  i0 -= i1;
  switch (i0) {
    case 0: goto block_8;
    case 1: goto block_7;
    case 2: goto block_7;
    case 3: goto block_7;
    case 4: goto block_7;
    case 5: goto block_9;
    default: goto block_7;
  }
  block_9:;
  i0 = tag;
  i0 = LoadU32(ctx, i0);
  i1 = 24u;
  i0 += i1;
  goto func_exit;
  block_8:;
  i0 = tag;
  i0 = LoadU32(ctx, i0);
  state = i0;
  block_7:;
  i0 = state;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
