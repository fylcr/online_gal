// invoke_meta_field.cpp -- invokes a meta field: the field is resolved and called with the projected arguments..
//
// InvokeMetaField -- invokes a meta field: the field is resolved and called with the projected arguments.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 813851-813851 (f1734); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void CallValue(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t FetchMetamethod(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

namespace {

constexpr uint32_t kF1734SaveFrameBytes = 20u;
constexpr uint32_t kF1734ResumeWordBytes = 4u;

}  // namespace

void InvokeMetaField(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1734SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 4u);
    l2 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    l6 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1734ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l7 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
    l3 = i0;
    i0 = LoadU32(ctx, i0 + 48u);
    l1 = i0;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = l1;
    i1 = l2;
    i0 = i0 == i1;
    if (i0) {
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 48u, i1);
      goto running_exit;
    }
    i0 = l1;
    i1 = l2;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0, i1);
    running_exit:;
    i0 = l2;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 112u);
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 112u);
    i1 = l2;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = l3;
    i1 = LoadU8(ctx, i1 + 20u);
    i2 = 3u;
    i1 &= i2;
    i2 = l2;
    i2 = LoadU8(ctx, i2 + 5u);
    i3 = 248u;
    i2 &= i3;
    i1 |= i2;
    StoreU8(ctx, i0 + 5u, i1);
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 8u);
    l1 = i0;
    i0 = !(i0);
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    if (i0) {goto block_6;}
    i0 = l1;
    i0 = LoadU8(ctx, i0 + 6u);
    i1 = 4u;
    i0 &= i1;
    if (i0) {goto block_6;}
    i0 = l1;
    i1 = 2u;
    i2 = a;
    i2 = LoadU32(ctx, i2 + 16u);
    i2 = LoadU32(ctx, i2 + 196u);
    i0 = FetchMetamethod(ctx, i0, i1, i2);
    l1 = i0;
    i0 = !(i0);
    if (i0) {goto block_6;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 64u);
    l6 = i0;
    i0 = a;
    i0 = LoadU8(ctx, i0 + 57u);
    l4 = i0;
    i0 = a;
    i1 = 0u;
    StoreU8(ctx, i0 + 57u, i1);
    i0 = l3;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 68u);
    i2 = 1u;
    i1 <<= (i2 & 31);
    StoreU32(ctx, i0 + 64u, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 8u);
    l5 = i0;
    i1 = l1;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0, j1);
    i0 = l5;
    i1 = l1;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 8u);
    l1 = i0;
    i1 = l2;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = l1;
    i1 = 7u;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 8u);
    l2 = i1;
    i2 = 32u;
    i1 += i2;
    StoreU32(ctx, i0 + 8u, i1);
  }
  i0 = l7;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = l2;
    i2 = 0u;
    CallValue(ctx, i0, i1, i2);
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = l4;
    StoreU8(ctx, i0 + 57u, i1);
    i0 = l3;
    i1 = l6;
    StoreU32(ctx, i0 + 64u, i1);
  }
  block_6:;
  goto func_exit;
  spill_frame:;
  l1 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l1;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1734ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l1 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l1;
  i1 = l2;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l1;
  i1 = l3;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l1;
  i1 = l6;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l1;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 20u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
