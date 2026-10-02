// fetch_object_metamethod.cpp -- fetches an object metamethod: the table field is read through the object layer..
//
// FetchObjectMetamethod -- fetches an object metamethod: the table field is read through the object layer.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 143476-143476 (f333); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t LookupMaskedMetamethodSlot(EngineContext* ctx, uint32_t a, uint32_t b);

namespace {

constexpr uint32_t kF333Const2B328 = 0x2B328u;  // 

}  // namespace

uint32_t FetchObjectMetamethod(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t l3 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = b;
  i0 = LoadU32(ctx, i0 + 8u);
  l3 = i0;
  i1 = 5u;
  i0 -= i1;
  switch (i0) {
    case 0: goto block_3;
    case 1: goto spill_frame;
    case 2: goto block_2;
    default: goto spill_frame;
  }
  block_3:;
  i0 = b;
  i0 = LoadU32(ctx, i0);
  i1 = 8u;
  i0 += i1;
  goto block_0;
  block_2:;
  i0 = b;
  i0 = LoadU32(ctx, i0);
  i1 = 8u;
  i0 += i1;
  goto block_0;
  spill_frame:;
  i0 = a;
  i0 = LoadU32(ctx, i0 + 16u);
  i1 = l3;
  i2 = 2u;
  i1 <<= (i2 & 31);
  i0 += i1;
  i1 = 152u;
  i0 += i1;
  block_0:;
  i0 = LoadU32(ctx, i0);
  b = i0;
  i0 = !(i0);
  if (i0) {
    i0 = kF333Const2B328;
    goto func_exit;
  }
  i0 = b;
  i1 = a;
  i1 = LoadU32(ctx, i1 + 16u);
  i2 = c;
  i3 = 2u;
  i2 <<= (i3 & 31);
  i1 += i2;
  i1 = LoadU32(ctx, i1 + 188u);
  i0 = LookupMaskedMetamethodSlot(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
  return i0;
}
}
