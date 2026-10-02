// bucket_log2.cpp -- the byte length to bucket step: the logarithm is walked eight bits at a time and the base offset of the pool is added back..
//
// BucketLog2 -- the byte length to bucket step: the logarithm is walked eight bits at a time and the base offset of the pool is added back.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 484614-484657 (f1120); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF1120Const2B340 = 0x2B340u;  // 
constexpr uint32_t kF1120ConstFFFFFFFF = 0xFFFFFFFFu;  // 

}  // namespace

uint32_t BucketLog2(EngineContext* ctx, uint32_t value) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  i0 = kF1120ConstFFFFFFFF;
  l1 = i0;
  i0 = value;
  i1 = 256u;
  i0 = i0 < i1;
  if (i0) {
    i0 = value;
    l2 = i0;
    goto block_0;
  }
  loop_2: 
    i0 = l1;
    i1 = 8u;
    i0 += i1;
    l1 = i0;
    i0 = value;
    i1 = 65535u;
    i0 = i0 > i1;
    l3 = i0;
    i0 = value;
    i1 = 8u;
    i0 >>= (i1 & 31);
    l2 = i0;
    value = i0;
    i0 = l3;
    if (i0) {goto loop_2;}
  block_0:;
  i0 = l1;
  i1 = l2;
  i2 = kF1120Const2B340;
  i1 += i2;
  i1 = LoadU8(ctx, i1);
  i0 += i1;
  LeaveFrame();
  return i0;
}
}
