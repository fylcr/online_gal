// reserve_int_vector.cpp -- reserves an integer vector, reallocating and re-stamping the header on the way..
//
// ReserveIntVector -- reserves an integer vector, reallocating and re-stamping the header on the way.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 814082-814082 (f1735); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t FillBytes(EngineContext* ctx, uint32_t dst, uint32_t value, uint32_t length);
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size);
void RelocateObjectHeader(EngineContext* ctx, uint32_t state, uint32_t source, uint32_t tag);

namespace {

constexpr uint32_t kF1735SaveFrameBytes = 24u;
constexpr uint32_t kF1735ResumeWordBytes = 4u;

}  // namespace

uint32_t ReserveIntVector(EngineContext* ctx, uint32_t state, uint32_t vector, uint32_t size) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1735SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 4u);
    vector = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    size = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    l5 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l6 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 20u);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1735ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = vector;
    i1 = 2u;
    i0 <<= (i1 & 31);
    l6 = i0;
    i1 = 20u;
    i0 += i1;
    l4 = i0;
    i0 = state;
    l5 = i0;
  }
  i0 = l3;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = 0u;
    i2 = 0u;
    i3 = l4;
    i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
    l3 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    state = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = state;
    i2 = 6u;
    RelocateObjectHeader(ctx, i0, i1, i2);
    i0 = state;
    i1 = size;
    StoreU32(ctx, i0 + 12u, i1);
    i0 = state;
    i1 = 0u;
    StoreU8(ctx, i0 + 6u, i1);
    i0 = state;
    i1 = vector;
    StoreU8(ctx, i0 + 7u, i1);
    i0 = vector;
    if (i0) {
      i0 = state;
      i1 = 20u;
      i0 += i1;
      i1 = 0u;
      i2 = l6;
      i0 = FillBytes(ctx, i0, i1, i2);
    }
    i0 = state;
    goto func_exit;
  }
  TrapUnreachable();
  spill_frame:;
  l3 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l3;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1735ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = vector;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = size;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l3;
  i1 = l5;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l3;
  i1 = l6;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l3;
  i1 = l4;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 24u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
