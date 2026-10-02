// find_last_byte.cpp -- scans a byte range backwards for one byte and answers its index, or minus one..
//
// FindLastByte -- scans a byte range backwards for one byte and answers its index, or minus one.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 1020998-1020998 (f2269); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF2269ConstFFFFFFFF = 0xFFFFFFFFu;  // 

}  // namespace

uint32_t FindLastByte(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t l3 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u;
  i0 = b;
  i0 = !(i0);
  if (i0) {
    i0 = kF2269ConstFFFFFFFF;
    goto func_exit;
  }
  i0 = a;
  i1 = b;
  i0 += i1;
  b = i0;
  i0 = kF2269ConstFFFFFFFF;
  l3 = i0;
  loop_2: 
    i0 = a;
    i1 = b;
    i0 = i0 == i1;
    if (i0) {goto spill_frame;}
    i0 = b;
    i1 = 1u;
    i0 -= i1;
    b = i0;
    i0 = LoadS8(ctx, i0);
    i1 = c;
    i0 = i0 != i1;
    if (i0) {goto loop_2;}
  i0 = b;
  i1 = a;
  i0 -= i1;
  l3 = i0;
  spill_frame:;
  i0 = l3;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
