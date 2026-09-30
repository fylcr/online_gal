// reset_input_line_state.cpp -- resets the input line state: the pending input line and its cursor are cleared..
//
// ResetInputLineState -- resets the input line state: the pending input line and its cursor are cleared.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 526225-526225 (f1210); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF1210ConstFFFFFFFF = 0xFFFFFFFFu;  // 

}  // namespace

uint32_t ResetInputLineState(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  uint64_t j1 = 0;
  i0 = a;
  i0 = LoadU8(ctx, i0 + 74u);
  l1 = i0;
  i1 = 1u;
  i0 -= i1;
  l2 = i0;
  i0 = a;
  i1 = l1;
  i2 = l2;
  i1 |= i2;
  StoreU8(ctx, i0 + 74u, i1);
  i0 = a;
  i0 = LoadU32(ctx, i0);
  l1 = i0;
  i1 = 8u;
  i0 &= i1;
  if (i0) {
    i0 = a;
    i1 = l1;
    i2 = 32u;
    i1 |= i2;
    StoreU32(ctx, i0, i1);
    i0 = kF1210ConstFFFFFFFF;
    goto func_exit;
  }
  i0 = a;
  j1 = 0ull;
  StoreU64(ctx, i0 + 4u, j1);
  i0 = a;
  i1 = a;
  i1 = LoadU32(ctx, i1 + 44u);
  l1 = i1;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = a;
  i1 = l1;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = a;
  i1 = a;
  i1 = LoadU32(ctx, i1 + 48u);
  i2 = l1;
  i1 += i2;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
