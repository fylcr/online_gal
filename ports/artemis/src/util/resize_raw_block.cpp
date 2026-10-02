// resize_raw_block.cpp -- the resize shim the allocator layer drives: a zero new size deallocates the block, anything else hands it to the raw reallocator..
//
// ResizeRawBlock -- the resize shim the allocator layer drives: a zero new size deallocates the block, anything else hands it to the raw reallocator.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 2756784-2756784 (f5721); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
uint32_t ReallocateRawBlock(EngineContext* ctx, uint32_t block, uint32_t size);

namespace {


}  // namespace

uint32_t ResizeRawBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size) {
  EnterFrame();
  uint32_t i0 = 0u; uint32_t i1 = 0u;
  i0 = new_size;
  i0 = !(i0);
  if (i0) {
    i0 = block;
    DeallocateRawBlock(ctx, i0);
    i0 = 0u;
    goto func_exit;
  }
  i0 = block;
  i1 = new_size;
  i0 = ReallocateRawBlock(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
  return i0;
}
}
