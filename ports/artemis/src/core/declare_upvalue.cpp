// declare_upvalue.cpp -- declares an upvalue, adding the local it closes over to the cell list..
//
// DeclareUpvalue -- declares an upvalue, adding the local it closes over to the cell list.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 780308-780308 (f1700); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void RelocateUpvalueCells(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

constexpr uint32_t kF1700ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF1700SaveFrameBytes = 20u;
constexpr uint32_t kF1700ResumeWordBytes = 4u;

}  // namespace

void DeclareUpvalue(EngineContext* ctx, uint32_t state, uint32_t local) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1700SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    local = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 16u);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1700ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 24u);
    l3 = i0;
    i0 = state;
    i1 = local;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = state;
    i0 = LoadU8(ctx, i0 + 56u);
    l4 = i0;
    i1 = 32u;
    i0 &= i1;
    l2 = i0;
  }
  i0 = l2;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l5;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = 5u;
      i2 = kF1700ConstFFFFFFFF;
      RelocateUpvalueCells(ctx, i0, i1, i2);
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      goto func_exit;
    }
  }
  i0 = l2;
  i1 = l4;
  i2 = 8u;
  i1 &= i2;
  i1 = !(i1);
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l2 = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {goto block_8;}
    i0 = state;
    i0 = LoadU32(ctx, i0 + 64u);
    l2 = i0;
    if (i0) {goto block_8;}
    i0 = state;
    i1 = state;
    i1 = LoadU32(ctx, i1 + 60u);
    l2 = i1;
    StoreU32(ctx, i0 + 64u, i1);
  }
  i0 = l5;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = 3u;
    i2 = kF1700ConstFFFFFFFF;
    RelocateUpvalueCells(ctx, i0, i1, i2);
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_8:;
  i0 = l4;
  i1 = l4;
  i2 = 4u;
  i1 &= i2;
  i1 = !(i1);
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l4 = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    if (i0) {goto block_11;}
    i0 = local;
    i1 = state;
    i1 = LoadU32(ctx, i1 + 20u);
    i1 = LoadU32(ctx, i1 + 4u);
    i1 = LoadU32(ctx, i1);
    i1 = LoadU32(ctx, i1 + 16u);
    l2 = i1;
    i1 = LoadU32(ctx, i1 + 12u);
    l7 = i1;
    i0 -= i1;
    i1 = 2u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    i1 = 1u;
    i0 -= i1;
    l4 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = !(i0);
    if (i0) {
      i0 = 0u;
      l2 = i0;
      i0 = local;
      i1 = l3;
      i0 = i0 <= i1;
      if (i0) {goto block_13;}
      i0 = l4;
      i0 = !(i0);
      if (i0) {goto block_13;}
      goto block_11;
    }
    i0 = l6;
    i1 = l4;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = local;
    i1 = l3;
    i0 = i0 <= i1;
    if (i0) {goto block_13;}
    i0 = l4;
    i0 = !(i0);
    if (i0) {goto block_13;}
    i0 = l2;
    i1 = l6;
    i2 = l3;
    i3 = l7;
    i2 -= i3;
    i1 += i2;
    i2 = 4u;
    i1 -= i2;
    i1 = LoadU32(ctx, i1);
    i0 = i0 == i1;
    if (i0) {goto block_11;}
    block_13:;
  }
  i0 = l5;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = 2u;
    i2 = l2;
    RelocateUpvalueCells(ctx, i0, i1, i2);
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_11:;
  goto func_exit;
  spill_frame:;
  l5 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l5;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1700ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l5 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = local;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l5;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l5;
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l5;
  i1 = l3;
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
