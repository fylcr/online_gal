// clear_upvalue_flags.cpp -- clears the upvalue flags: every upvalue's cached flag is reset..
//
// ClearUpvalueFlags -- clears the upvalue flags: every upvalue's cached flag is reset.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 813595-813595 (f1732); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void ClearLocalFlags(EngineContext* ctx, uint32_t a);
void ClearFlagsRecursive(EngineContext* ctx, uint32_t state, uint32_t node);

namespace {


}  // namespace

void ClearUpvalueFlags(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u;
  uint64_t j1 = 0;
  i0 = a;
  i0 = LoadU32(ctx, i0 + 16u);
  l1 = i0;
  i1 = 0u;
  StoreU32(ctx, i0 + 44u, i1);
  i0 = l1;
  j1 = 0ull;
  StoreU64(ctx, i0 + 36u, j1);
  i0 = l1;
  i0 = LoadU32(ctx, i0 + 112u);
  l2 = i0;
  i0 = LoadU8(ctx, i0 + 5u);
  i1 = 3u;
  i0 &= i1;
  if (i0) {
    i0 = l1;
    i1 = l2;
    ClearFlagsRecursive(ctx, i0, i1);
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 112u);
    l2 = i0;
  }
  i0 = l2;
  i0 = LoadU32(ctx, i0 + 80u);
  i1 = 4u;
  i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
  if (i0) {goto spill_frame;}
  i0 = l2;
  i0 = LoadU32(ctx, i0 + 72u);
  l2 = i0;
  i0 = LoadU8(ctx, i0 + 5u);
  i1 = 3u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto spill_frame;}
  i0 = l1;
  i1 = l2;
  ClearFlagsRecursive(ctx, i0, i1);
  spill_frame:;
  i0 = a;
  i0 = LoadU32(ctx, i0 + 16u);
  a = i0;
  i0 = LoadU32(ctx, i0 + 104u);
  i1 = 4u;
  i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
  if (i0) {goto block_2;}
  i0 = a;
  i0 = LoadU32(ctx, i0 + 96u);
  a = i0;
  i0 = LoadU8(ctx, i0 + 5u);
  i1 = 3u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto block_2;}
  i0 = l1;
  i1 = a;
  ClearFlagsRecursive(ctx, i0, i1);
  block_2:;
  i0 = l1;
  ClearLocalFlags(ctx, i0);
  i0 = l1;
  i1 = 1u;
  StoreU8(ctx, i0 + 21u, i1);
  LeaveFrame();
}
}
