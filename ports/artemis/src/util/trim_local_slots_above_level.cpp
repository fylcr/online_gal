// trim_local_slots_above_level.cpp -- walks a function prototype from the top and drops the local slots declared above a nesting level..
//
// TrimLocalSlotsAboveLevel -- walks a function prototype from the top and drops the local slots declared above a nesting level.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 166997-166997 (f373); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {


}  // namespace

uint32_t TrimLocalSlotsAboveLevel(EngineContext* ctx, uint32_t proto, uint32_t level, uint32_t result) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  i0 = proto;
  i0 = LoadU32(ctx, i0 + 20u);
  l3 = i0;
  i0 = level;
  i1 = 1u;
  i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
  if (i0) {
    i0 = proto;
    i0 = LoadU32(ctx, i0 + 40u);
    l4 = i0;
    loop_2: 
      i0 = l3;
      i1 = l4;
      i0 = i0 <= i1;
      if (i0) {goto block_0;}
      i0 = level;
      i1 = 1u;
      i0 -= i1;
      level = i0;
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 4u);
      i0 = LoadU32(ctx, i0);
      i0 = LoadU8(ctx, i0 + 6u);
      i0 = !(i0);
      if (i0) {
        i0 = level;
        i1 = l3;
        i1 = LoadU32(ctx, i1 + 20u);
        i0 -= i1;
        level = i0;
      }
      i0 = l3;
      i1 = 24u;
      i0 -= i1;
      l3 = i0;
      i0 = level;
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      if (i0) {goto loop_2;}
  }
  i0 = level;
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = proto;
    i1 = LoadU32(ctx, i1 + 40u);
    proto = i1;
    i0 = i0 <= i1;
    if (i0) {goto block_0;}
    i0 = result;
    i1 = l3;
    i2 = proto;
    i1 -= i2;
    i2 = 24u;
    i1 = I32_DIV_S(i1, i2);
    StoreU32(ctx, i0 + 96u, i1);
    goto running_exit;
  }
  i0 = result;
  i1 = 0u;
  StoreU32(ctx, i0 + 96u, i1);
  running_exit:;
  i0 = 1u;
  l5 = i0;
  block_0:;
  i0 = l5;
  LeaveFrame();
  return i0;
}
}
