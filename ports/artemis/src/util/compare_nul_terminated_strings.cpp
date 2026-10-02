// compare_nul_terminated_strings.cpp -- walks two NUL-terminated byte strings and answers the first difference, treating a NUL byte as the end of the shorter side..
//
// CompareNulTerminatedStrings -- walks two NUL-terminated byte strings and answers the first difference, treating a NUL byte as the end of the shorter side.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 113872-113872 (f211); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {


}  // namespace

uint32_t CompareNulTerminatedStrings(EngineContext* ctx, uint32_t left, uint32_t right) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u;
  i0 = right;
  i0 = LoadU8(ctx, i0);
  l2 = i0;
  i0 = left;
  i0 = LoadU8(ctx, i0);
  l3 = i0;
  i0 = !(i0);
  if (i0) {goto block_0;}
  i0 = l2;
  i1 = l3;
  i0 = i0 != i1;
  if (i0) {goto block_0;}
  loop_1: 
    i0 = right;
    i0 = LoadU8(ctx, i0 + 1u);
    l2 = i0;
    i0 = left;
    i0 = LoadU8(ctx, i0 + 1u);
    l3 = i0;
    i0 = !(i0);
    if (i0) {goto block_0;}
    i0 = right;
    i1 = 1u;
    i0 += i1;
    right = i0;
    i0 = left;
    i1 = 1u;
    i0 += i1;
    left = i0;
    i0 = l2;
    i1 = l3;
    i0 = i0 == i1;
    if (i0) {goto loop_1;}
  block_0:;
  i0 = l3;
  i1 = l2;
  i0 -= i1;
  LeaveFrame();
  return i0;
}
}
