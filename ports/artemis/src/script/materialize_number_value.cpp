// materialize_number_value.cpp -- turns a numeric slot into a TValue the arithmetic path can use directly..
//
// MaterializeNumberValue -- turns a numeric slot into a TValue the arithmetic path can use directly.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 347946-347946 (f897); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t CoerceToNumber(EngineContext* ctx, uint32_t state, uint32_t value);

namespace {

constexpr uint32_t kF897SaveFrameBytes = 16u;
constexpr uint32_t kF897ResumeWordBytes = 4u;

}  // namespace

uint32_t MaterializeNumberValue(EngineContext* ctx, uint32_t state, uint32_t slot) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; double l5 = 0;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  double d0 = 0; double d1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF897SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 4u);
    slot = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 12u);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF897ResumeWordBytes;
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
    i0 = GuestStackSp(ctx);
    i1 = 16u;
    i0 -= i1;
    l2 = i0;
    GuestStackSp(ctx) = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = 3u;
    i0 -= i1;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    switch (i0) {
      case 0: goto running_exit;
      case 1: goto block_7;
      default: goto block_5;
    }
    block_7:;
    i0 = l2;
    i1 = 8u;
    i0 += i1;
    l4 = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0);
    i1 = 16u;
    i0 += i1;
    state = i0;
  }
  i0 = l3;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l4;
    i0 = CoerceToNumber(ctx, i0, i1);
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
    i0 = state;
    i0 = !(i0);
    state = i0;
    if (i0) {goto block_5;}
    i0 = l2;
    d0 = LoadF64(ctx, i0 + 8u);
    l5 = d0;
    i0 = slot;
    i1 = 3u;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = slot;
    d1 = l5;
    StoreF64(ctx, i0, d1);
    i0 = slot;
    state = i0;
    goto running_exit;
  }
  block_5:;
  i0 = state;
  i1 = 0u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  state = i0;
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i1 = 16u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
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
  i2 = kF897ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = slot;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = l4;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l3;
  i1 = l2;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 16u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
