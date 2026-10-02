// copy_text_bytes.cpp -- copies text bytes: the source span is copied into the destination buffer..
//
// CopyTextBytes -- copies text bytes: the source span is copied into the destination buffer.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 1163678-1163678 (f2729); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t StringLength(EngineContext* ctx, uint32_t text);

namespace {


}  // namespace

uint32_t CopyTextBytes(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u;
  i0 = a;
  i0 = StringLength(ctx, i0);
  i1 = a;
  i0 += i1;
  l3 = i0;
  i0 = c;
  i0 = !(i0);
  if (i0) {goto block_0;}
  loop_1: 
    i0 = b;
    i0 = LoadU8(ctx, i0);
    l4 = i0;
    i0 = !(i0);
    if (i0) {goto block_0;}
    i0 = l3;
    i1 = l4;
    StoreU8(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    i0 = b;
    i1 = 1u;
    i0 += i1;
    b = i0;
    i0 = c;
    i1 = 1u;
    i0 -= i1;
    c = i0;
    if (i0) {goto loop_1;}
  block_0:;
  i0 = l3;
  i1 = 0u;
  StoreU8(ctx, i0, i1);
  i0 = a;
  LeaveFrame();
  return i0;
}
}
