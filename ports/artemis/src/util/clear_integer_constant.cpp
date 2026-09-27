// clear_integer_constant.cpp -- the inline-constant probe: the small tag range and the sentinel value clear the slot and answer zero, every other shape answers whether it survived..
//
// ClearIntegerConstant -- the inline-constant probe: the small tag range and the sentinel value clear the slot and answer zero, every other shape answers whether it survived.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 662764-662801 (f1493); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF1493ConstFFFFFFFE = 0xFFFFFFFEu;  // 

}  // namespace

uint32_t ClearIntegerConstant(EngineContext* ctx, uint32_t slot, uint32_t tag) {
  EnterFrame();
  uint32_t l2 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u;
  i0 = tag;
  i1 = 2u;
  i0 <<= (i1 & 31);
  i1 = slot;
  i0 += i1;
  l2 = i0;
  i1 = 20u;
  i0 += i1;
  i0 = LoadU32(ctx, i0);
  slot = i0;
  i0 = tag;
  i1 = kF1493ConstFFFFFFFE;
  i0 &= i1;
  i1 = 136u;
  i0 = i0 != i1;
  if (i0) {goto block_0;}
  i0 = slot;
  i1 = 1u;
  i0 = i0 != i1;
  if (i0) {goto block_0;}
  i0 = l2;
  i1 = 0u;
  StoreU32(ctx, i0 + 20u, i1);
  block_0:;
  i0 = slot;
  i1 = 1u;
  i0 = i0 == i1;
  LeaveFrame();
  return i0;
}
}
