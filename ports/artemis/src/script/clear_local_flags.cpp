// clear_local_flags.cpp -- clears the local flags: the local slot flags are reset for the enclosing scope..
//
// ClearLocalFlags -- clears the local flags: the local slot flags are reset for the enclosing scope.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 813440-813440 (f1731); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void ClearFlagsRecursive(EngineContext* ctx, uint32_t state, uint32_t node);

namespace {


}  // namespace

void ClearLocalFlags(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u;
  i0 = a;
  i0 = LoadU32(ctx, i0 + 152u);
  l1 = i0;
  i0 = !(i0);
  if (i0) {goto block_0;}
  i0 = l1;
  i0 = LoadU8(ctx, i0 + 5u);
  i1 = 3u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto block_0;}
  i0 = a;
  i1 = l1;
  ClearFlagsRecursive(ctx, i0, i1);
  block_0:;
  i0 = a;
  i0 = LoadU32(ctx, i0 + 156u);
  l1 = i0;
  i0 = !(i0);
  if (i0) {goto spill_frame;}
  i0 = l1;
  i0 = LoadU8(ctx, i0 + 5u);
  i1 = 3u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto spill_frame;}
  i0 = a;
  i1 = l1;
  ClearFlagsRecursive(ctx, i0, i1);
  spill_frame:;
  i0 = a;
  i0 = LoadU32(ctx, i0 + 160u);
  l1 = i0;
  i0 = !(i0);
  if (i0) {goto block_2;}
  i0 = l1;
  i0 = LoadU8(ctx, i0 + 5u);
  i1 = 3u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto block_2;}
  i0 = a;
  i1 = l1;
  ClearFlagsRecursive(ctx, i0, i1);
  block_2:;
  i0 = a;
  i0 = LoadU32(ctx, i0 + 164u);
  l1 = i0;
  i0 = !(i0);
  if (i0) {goto block_3;}
  i0 = l1;
  i0 = LoadU8(ctx, i0 + 5u);
  i1 = 3u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto block_3;}
  i0 = a;
  i1 = l1;
  ClearFlagsRecursive(ctx, i0, i1);
  block_3:;
  i0 = a;
  i0 = LoadU32(ctx, i0 + 168u);
  l1 = i0;
  i0 = !(i0);
  if (i0) {goto running_exit;}
  i0 = l1;
  i0 = LoadU8(ctx, i0 + 5u);
  i1 = 3u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto running_exit;}
  i0 = a;
  i1 = l1;
  ClearFlagsRecursive(ctx, i0, i1);
  running_exit:;
  i0 = a;
  i0 = LoadU32(ctx, i0 + 172u);
  l1 = i0;
  i0 = !(i0);
  if (i0) {goto block_5;}
  i0 = l1;
  i0 = LoadU8(ctx, i0 + 5u);
  i1 = 3u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto block_5;}
  i0 = a;
  i1 = l1;
  ClearFlagsRecursive(ctx, i0, i1);
  block_5:;
  i0 = a;
  i0 = LoadU32(ctx, i0 + 176u);
  l1 = i0;
  i0 = !(i0);
  if (i0) {goto block_6;}
  i0 = l1;
  i0 = LoadU8(ctx, i0 + 5u);
  i1 = 3u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto block_6;}
  i0 = a;
  i1 = l1;
  ClearFlagsRecursive(ctx, i0, i1);
  block_6:;
  i0 = a;
  i0 = LoadU32(ctx, i0 + 180u);
  l1 = i0;
  i0 = !(i0);
  if (i0) {goto block_7;}
  i0 = l1;
  i0 = LoadU8(ctx, i0 + 5u);
  i1 = 3u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto block_7;}
  i0 = a;
  i1 = l1;
  ClearFlagsRecursive(ctx, i0, i1);
  block_7:;
  i0 = a;
  i0 = LoadU32(ctx, i0 + 184u);
  l1 = i0;
  i0 = !(i0);
  if (i0) {goto block_8;}
  i0 = l1;
  i0 = LoadU8(ctx, i0 + 5u);
  i1 = 3u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto block_8;}
  i0 = a;
  i1 = l1;
  ClearFlagsRecursive(ctx, i0, i1);
  block_8:;
  LeaveFrame();
}
}
