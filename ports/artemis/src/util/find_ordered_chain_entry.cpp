// find_ordered_chain_entry.cpp -- finds an entry on an ordered chain: the chain is walked in key order and the matching node is answered..
//
// FindOrderedChainEntry -- finds an entry on an ordered chain: the chain is walked in key order and the matching node is answered.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 123149-123149 (f242); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

constexpr uint32_t kF242ConstFFFFFFFF = 0xFFFFFFFFu;  // 

}  // namespace

uint32_t FindOrderedChainEntry(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = a;
  i1 = 4u;
  i0 += i1;
  l6 = i0;
  i0 = a;
  i0 = LoadU32(ctx, i0 + 4u);
  a = i0;
  i0 = !(i0);
  if (i0) {goto spill_frame;}
  i0 = b;
  i0 = LoadU32(ctx, i0);
  i1 = b;
  i2 = b;
  i2 = LoadU8(ctx, i2 + 11u);
  l3 = i2;
  i3 = 24u;
  i2 <<= (i3 & 31);
  i3 = 24u;
  i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
  i3 = 0u;
  i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
  l2 = i2;
  i0 = i2 ? i0 : i1;
  l7 = i0;
  i0 = b;
  i0 = LoadU32(ctx, i0 + 4u);
  i1 = l3;
  i2 = l2;
  i0 = i2 ? i0 : i1;
  l4 = i0;
  i0 = l6;
  l2 = i0;
  loop_2: 
    i0 = a;
    i0 = LoadU8(ctx, i0 + 27u);
    b = i0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l8 = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 20u);
    i1 = b;
    i2 = l8;
    i0 = i2 ? i0 : i1;
    l9 = i0;
    i1 = l4;
    i0 = i0 > i1;
    l5 = i0;
    i0 = l4;
    i1 = l9;
    i2 = l5;
    i0 = i2 ? i0 : i1;
    l3 = i0;
    if (i0) {
      i0 = a;
      i1 = 16u;
      i0 += i1;
      b = i0;
      i0 = LoadU32(ctx, i0);
      i1 = b;
      i2 = l8;
      i0 = i2 ? i0 : i1;
      i1 = l7;
      i2 = l3;
      i0 = CompareBytes(ctx, i0, i1, i2);
      b = i0;
      if (i0) {goto block_3;}
    }
    i0 = kF242ConstFFFFFFFF;
    i1 = l5;
    i2 = l4;
    i3 = l9;
    i2 = i2 > i3;
    i0 = i2 ? i0 : i1;
    b = i0;
    block_3:;
    i0 = l2;
    i1 = a;
    i2 = b;
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    l2 = i0;
    i0 = b;
    i1 = 29u;
    i0 >>= (i1 & 31);
    i1 = 4u;
    i0 &= i1;
    i1 = a;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    a = i0;
    if (i0) {goto loop_2;}
  i0 = l2;
  i1 = l6;
  i0 = i0 == i1;
  if (i0) {goto spill_frame;}
  i0 = l2;
  i0 = LoadU8(ctx, i0 + 27u);
  a = i0;
  i1 = 24u;
  i0 <<= (i1 & 31);
  i1 = 24u;
  i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
  i1 = 0u;
  i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
  l3 = i0;
  i0 = l2;
  i0 = LoadU32(ctx, i0 + 20u);
  i1 = a;
  i2 = l3;
  i0 = i2 ? i0 : i1;
  l5 = i0;
  i1 = l4;
  i0 = i0 < i1;
  a = i0;
  i0 = l5;
  i1 = l4;
  i2 = a;
  i0 = i2 ? i0 : i1;
  b = i0;
  if (i0) {
    i0 = l7;
    i1 = l2;
    i2 = 16u;
    i1 += i2;
    a = i1;
    i1 = LoadU32(ctx, i1);
    i2 = a;
    i3 = l3;
    i1 = i3 ? i1 : i2;
    i2 = b;
    i0 = CompareBytes(ctx, i0, i1, i2);
    a = i0;
    if (i0) {goto block_5;}
  }
  i0 = l4;
  i1 = l5;
  i0 = i0 < i1;
  if (i0) {goto spill_frame;}
  goto block_0;
  block_5:;
  i0 = a;
  i1 = kF242ConstFFFFFFFF;
  i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
  if (i0) {goto block_0;}
  spill_frame:;
  i0 = l6;
  l2 = i0;
  block_0:;
  i0 = l2;
  LeaveFrame();
  return i0;
}
}
