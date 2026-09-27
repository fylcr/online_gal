// invoke_callback_when_flag_unset.cpp -- runs the pending callback while the guard flag on the slot is still clear..
//
// InvokeCallbackWhenFlagUnset -- runs the pending callback while the guard flag on the slot is still clear.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 167848-167848 (f377); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t ReadInputLine(EngineContext* ctx, uint32_t state, uint32_t stream, uint32_t line);

namespace {

constexpr uint32_t kF377SaveFrameBytes = 12u;
constexpr uint32_t kF377ResumeWordBytes = 4u;

}  // namespace

void InvokeCallbackWhenFlagUnset(EngineContext* ctx, uint32_t state, uint32_t slot, uint32_t callback) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF377SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    callback = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = callback;
    i0 = LoadU32(ctx, i0 + 4u);
    slot = i0;
    i0 = callback;
    i0 = LoadU32(ctx, i0 + 8u);
    callback = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF377ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  if (i0) {
    i0 = l4;
  } else {
    i0 = state;
    i0 = LoadU8(ctx, i0);
    i1 = 32u;
    i0 &= i1;
    i0 = !(i0);
  }
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l3;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = slot;
      i1 = callback;
      i2 = state;
      i0 = ReadInputLine(ctx, i0, i1, i2);
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
  }
  goto func_exit;
  spill_frame:;
  l3 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l3;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF377ResumeWordBytes;
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
  i1 = callback;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 12u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
