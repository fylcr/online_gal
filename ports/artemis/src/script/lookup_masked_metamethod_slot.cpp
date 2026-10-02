// lookup_masked_metamethod_slot.cpp -- looks up a masked metamethod slot: the slot word's mask selects the metamethod..
//
// LookupMaskedMetamethodSlot -- looks up a masked metamethod slot: the slot word's mask selects the metamethod.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 795114-795114 (f1708); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF1708Const2B328 = 0x2B328u;  // 
constexpr uint32_t kF1708ConstFFFFFFFF = 0xFFFFFFFFu;  // 

}  // namespace

uint32_t LookupMaskedMetamethodSlot(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = a;
  i0 = LoadU32(ctx, i0 + 16u);
  i1 = b;
  i1 = LoadU32(ctx, i1 + 8u);
  i2 = kF1708ConstFFFFFFFF;
  i3 = a;
  i3 = LoadU8(ctx, i3 + 7u);
  i2 <<= (i3 & 31);
  i3 = kF1708ConstFFFFFFFF;
  i2 ^= i3;
  i1 &= i2;
  i2 = 5u;
  i1 <<= (i2 & 31);
  i0 += i1;
  a = i0;
  loop_1: 
    i0 = a;
    i0 = LoadU32(ctx, i0 + 24u);
    i1 = 4u;
    i0 = i0 == i1;
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 16u);
      i1 = b;
      i0 = i0 == i1;
      if (i0) {goto block_0;}
    }
    i0 = a;
    i0 = LoadU32(ctx, i0 + 28u);
    a = i0;
    if (i0) {goto loop_1;}
  i0 = kF1708Const2B328;
  a = i0;
  block_0:;
  i0 = a;
  LeaveFrame();
  return i0;
}
}
