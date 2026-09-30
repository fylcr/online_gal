// ensure_table_array_room.cpp -- decides the next capacity of a table array part (1024-element steps plus a spent-credit percent fallback)..
//
// EnsureTableArrayRoom -- decides the next capacity of a table array part (1024-element steps plus a spent-credit percent fallback).
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 160907-160907 (f363); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t CloseFunctionScope(EngineContext* ctx, uint32_t state);

namespace {

constexpr uint32_t kF363Const7FFFFFFE = 0x7FFFFFFEu;  // 
constexpr uint32_t kF363SaveFrameBytes = 20u;
constexpr uint32_t kF363ResumeWordBytes = 4u;

}  // namespace

void EnsureTableArrayRoom(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF363SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 4u);
    l1 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF363ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
    l1 = i0;
    i0 = LoadU32(ctx, i0 + 76u);
    l3 = i0;
    i0 = l1;
    i1 = l1;
    i1 = LoadU32(ctx, i1 + 68u);
    i2 = l1;
    i2 = LoadU32(ctx, i2 + 64u);
    l4 = i2;
    i1 -= i2;
    l5 = i1;
    i2 = l3;
    i1 += i2;
    StoreU32(ctx, i0 + 76u, i1);
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 84u);
    i1 = 10u;
    i0 *= i1;
    l3 = i0;
    i1 = kF363Const7FFFFFFE;
    i2 = l3;
    i0 = i2 ? i0 : i1;
    l3 = i0;
  }
  loop_4: 
    i0 = l6;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i0 = CloseFunctionScope(ctx, i0);
      l2 = i0;
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l2;
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l1;
      i0 = LoadU8(ctx, i0 + 21u);
      l5 = i0;
      i0 = !(i0);
      if (i0) {goto block_5;}
      i0 = l3;
      i1 = l4;
      i0 -= i1;
      l3 = i0;
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      l4 = i0;
      if (i0) {goto loop_4;}
    }
    block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    if (i0) {
      i0 = l1;
      i0 = LoadU32(ctx, i0 + 76u);
      a = i0;
      i1 = 1023u;
      i0 = i0 <= i1;
      if (i0) {
        i0 = l1;
        i1 = l1;
        i1 = LoadU32(ctx, i1 + 68u);
        i2 = 1024u;
        i1 += i2;
        StoreU32(ctx, i0 + 64u, i1);
        goto func_exit;
      }
      i0 = l1;
      i1 = a;
      i2 = 1024u;
      i1 -= i2;
      StoreU32(ctx, i0 + 76u, i1);
      i0 = l1;
      i1 = l1;
      i1 = LoadU32(ctx, i1 + 68u);
      StoreU32(ctx, i0 + 64u, i1);
      goto func_exit;
    }
    i0 = l1;
    i1 = l1;
    i1 = LoadU32(ctx, i1 + 80u);
    i2 = l1;
    i2 = LoadU32(ctx, i2 + 72u);
    i3 = 100u;
    i2 = DIV_U(i2, i3);
    i1 *= i2;
    StoreU32(ctx, i0 + 64u, i1);
  }
  goto func_exit;
  spill_frame:;
  l2 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF363ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l2 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l2;
  i1 = l1;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l2;
  i1 = l3;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l2;
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l2;
  i1 = l5;
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
