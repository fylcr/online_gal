// read_live_node_payload.cpp -- the liveness guard on the payload read: a node of the live kind with its flag unset answers the payload address, anything else answers zero..
//
// ReadLiveNodePayload -- the liveness guard on the payload read: a node of the live kind with its flag unset answers the payload address, anything else answers zero.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 236091-236124 (f611); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {


}  // namespace

uint32_t ReadLiveNodePayload(EngineContext* ctx, uint32_t node) {
  EnterFrame();
  uint32_t l1 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u;
  i0 = node;
  i0 = LoadU32(ctx, i0 + 8u);
  i1 = 16u;
  i0 -= i1;
  l1 = i0;
  i0 = 0u;
  node = i0;
  i0 = l1;
  i0 = LoadU32(ctx, i0 + 8u);
  i1 = 6u;
  i0 = i0 != i1;
  if (i0) {goto block_0;}
  i0 = l1;
  i0 = LoadU32(ctx, i0);
  l1 = i0;
  i0 = LoadU8(ctx, i0 + 6u);
  i0 = !(i0);
  if (i0) {goto block_0;}
  i0 = l1;
  i0 = LoadU32(ctx, i0 + 16u);
  node = i0;
  block_0:;
  i0 = node;
  LeaveFrame();
  return i0;
}
}
