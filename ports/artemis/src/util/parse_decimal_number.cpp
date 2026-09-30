// parse_decimal_number.cpp -- the positional-argument reader of the formatter: the digit run behind the character cursor is accumulated in base ten and the cursor is advanced past it, answering the parsed number..
//
// ParseDecimalNumber -- the positional-argument reader of the formatter: the digit run behind the character cursor is accumulated in base ten and the cursor is advanced past it, answering the parsed number.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 551552-551552 (f1344); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {


}  // namespace

uint32_t ParseDecimalNumber(EngineContext* ctx, uint32_t cursor) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  i0 = cursor;
  i0 = LoadU32(ctx, i0);
  i0 = LoadS8(ctx, i0);
  i1 = 48u;
  i0 -= i1;
  i1 = 10u;
  i0 = i0 < i1;
  if (i0) {
    loop_1: 
      i0 = cursor;
      i0 = LoadU32(ctx, i0);
      l1 = i0;
      i0 = LoadS8(ctx, i0);
      l3 = i0;
      i0 = cursor;
      i1 = l1;
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = l2;
      i2 = 10u;
      i1 *= i2;
      i0 += i1;
      i1 = 48u;
      i0 -= i1;
      l2 = i0;
      i0 = l1;
      i0 = LoadS8(ctx, i0 + 1u);
      i1 = 48u;
      i0 -= i1;
      i1 = 10u;
      i0 = i0 < i1;
      if (i0) {goto loop_1;}
  }
  i0 = l2;
  LeaveFrame();
  return i0;
}
}
