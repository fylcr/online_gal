// patch_instruction_target.cpp -- retargets one pending jump in an instruction word and settles the pending list..
//
// PatchInstructionTarget -- retargets one pending jump in an instruction word and settles the pending list.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 2748522-2748522 (f5677); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF5677Const7FFFFF = 0x7FFFFFu;  // 
constexpr uint32_t kF5677Const1000000 = 0x1000000u;  // 

}  // namespace

void PatchInstructionTarget(EngineContext* ctx, uint32_t state, uint32_t pending) {
  EnterFrame();
  uint32_t l2 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = pending;
  i0 = LoadU32(ctx, i0);
  i1 = 13u;
  i0 -= i1;
  switch (i0) {
    case 0: goto block_2;
    case 1: goto spill_frame;
    default: goto block_0;
  }
  block_2:;
  i0 = pending;
  i1 = 12u;
  StoreU32(ctx, i0, i1);
  i0 = pending;
  i1 = state;
  i1 = LoadU32(ctx, i1);
  i1 = LoadU32(ctx, i1 + 12u);
  i2 = pending;
  i2 = LoadU32(ctx, i2 + 8u);
  i3 = 2u;
  i2 <<= (i3 & 31);
  i1 += i2;
  i1 = LoadU32(ctx, i1);
  i2 = 6u;
  i1 >>= (i2 & 31);
  i2 = 255u;
  i1 &= i2;
  StoreU32(ctx, i0 + 8u, i1);
  goto func_exit;
  spill_frame:;
  i0 = state;
  i0 = LoadU32(ctx, i0);
  i0 = LoadU32(ctx, i0 + 12u);
  i1 = pending;
  i1 = LoadU32(ctx, i1 + 8u);
  i2 = 2u;
  i1 <<= (i2 & 31);
  i0 += i1;
  state = i0;
  i0 = LoadU32(ctx, i0);
  i1 = kF5677Const7FFFFF;
  i0 &= i1;
  i1 = kF5677Const1000000;
  i0 |= i1;
  l2 = i0;
  i0 = state;
  i1 = l2;
  StoreU32(ctx, i0, i1);
  i0 = pending;
  i1 = 11u;
  StoreU32(ctx, i0, i1);
  block_0:;
  func_exit:;
  LeaveFrame();
}
}
