// fill_words.cpp -- the word-granular fill: one value is written across the requested number of words and the destination is answered; a zero count writes nothing..
//
// FillWords -- the word-granular fill: one value is written across the requested number of words and the destination is answered; a zero count writes nothing.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 182151-182151 (f420); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {


}  // namespace

uint32_t FillWords(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t l3 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u;
  i0 = c;
  if (i0) {
    i0 = a;
    l3 = i0;
    loop_1: 
      i0 = l3;
      i1 = b;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 4u;
      i0 += i1;
      l3 = i0;
      i0 = c;
      i1 = 1u;
      i0 -= i1;
      c = i0;
      if (i0) {goto loop_1;}
  }
  i0 = a;
  LeaveFrame();
  return i0;
}
}
