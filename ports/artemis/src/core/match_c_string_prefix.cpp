// match_c_string_prefix.cpp -- matches a C string prefix: the prefix is compared without running past the end of the haystack..
//
// MatchCStringPrefix -- matches a C string prefix: the prefix is compared without running past the end of the haystack.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 150982-150982 (f351); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t CompareNulTerminatedStrings(EngineContext* ctx, uint32_t left, uint32_t right);

namespace {


}  // namespace

uint32_t MatchCStringPrefix(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t i0 = 0u; uint32_t i1 = 0u;
  i0 = c;
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 4u);
    i1 = b;
    i1 = LoadU32(ctx, i1 + 4u);
    i0 = i0 == i1;
    goto func_exit;
  }
  i0 = a;
  i1 = b;
  i0 = i0 == i1;
  if (i0) {
    i0 = 1u;
    goto func_exit;
  }
  i0 = a;
  i0 = LoadU32(ctx, i0 + 4u);
  i1 = b;
  i1 = LoadU32(ctx, i1 + 4u);
  i0 = CompareNulTerminatedStrings(ctx, i0, i1);
  i0 = !(i0);
  func_exit:;
  LeaveFrame();
  return i0;
}
}
