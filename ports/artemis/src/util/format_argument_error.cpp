// format_argument_error.cpp -- formats the bad argument #%d message into the pending message buffer..
//
// FormatArgumentError -- formats the bad argument #%d message into the pending message buffer.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 136486-136486 (f304); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t CompareNulTerminatedStrings(EngineContext* ctx, uint32_t left, uint32_t right);
uint32_t RaiseTraceback(EngineContext* ctx, uint32_t value, uint32_t message, uint32_t tag);
uint32_t TrimLocalSlotsAboveLevel(EngineContext* ctx, uint32_t proto, uint32_t level, uint32_t result);
uint32_t FormatTracebackFrame(EngineContext* ctx, uint32_t state, uint32_t name, uint32_t out);

namespace {

constexpr uint32_t kF304MsgBadArgumentDS = 0x29D03u;  // bad argument #%d (%s)
constexpr uint32_t kF304Const29D19 = 0x29D19u;  // 
constexpr uint32_t kF304MsgMethod = 0x29D1Bu;  // method
constexpr uint32_t kF304MsgCallingSOnBad = 0x29D22u;  // calling '%s' on bad self (%s)
constexpr uint32_t kF304Const29D40 = 0x29D40u;  // 
constexpr uint32_t kF304MsgBadArgumentDTo = 0x29D42u;  // bad argument #%d to '%s' (%s)
constexpr uint32_t kF304SaveFrameBytes = 20u;
constexpr uint32_t kF304ResumeWordBytes = 4u;

}  // namespace

uint32_t FormatArgumentError(EngineContext* ctx, uint32_t state, uint32_t index, uint32_t actual) {
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
    i2 = kF304SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    actual = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    index = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF304ResumeWordBytes;
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
    i0 = GuestStackSp(ctx);
    i1 = 160u;
    i0 -= i1;
    l4 = i0;
    GuestStackSp(ctx) = i0;
    i0 = state;
    i1 = 0u;
    i2 = l4;
    i3 = 56u;
    i2 += i3;
    i0 = TrimLocalSlotsAboveLevel(ctx, i0, i1, i2);
    i0 = !(i0);
    l5 = i0;
  }
  i0 = l5;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = actual;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l4;
      i1 = index;
      StoreU32(ctx, i0, i1);
    }
    i0 = l6;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = kF304MsgBadArgumentDS;
      i2 = l4;
      i0 = RaiseTraceback(ctx, i0, i1, i2);
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
    if (i0) {goto running_exit;}
  }
  i0 = l5;
  i1 = l4;
  i2 = 56u;
  i1 += i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l5 = i0;
  i0 = l6;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = kF304Const29D19;
    i2 = l5;
    i0 = FormatTracebackFrame(ctx, i0, i1, i2);
    l3 = i0;
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 64u);
    i1 = kF304MsgMethod;
    i0 = CompareNulTerminatedStrings(ctx, i0, i1);
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    if (i0) {goto block_10;}
    i0 = index;
    i1 = 1u;
    i0 -= i1;
    index = i0;
    if (i0) {goto block_10;}
    i0 = l4;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 60u);
    StoreU32(ctx, i0 + 16u, i1);
    i0 = l4;
    i1 = actual;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = l4;
    i1 = 16u;
    i0 += i1;
    index = i0;
  }
  i0 = l6;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = kF304MsgCallingSOnBad;
    i2 = index;
    i0 = RaiseTraceback(ctx, i0, i1, i2);
    l3 = i0;
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    state = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto running_exit;}
  block_10:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 60u);
    l5 = i0;
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = kF304Const29D40;
      StoreU32(ctx, i0 + 60u, i1);
      i0 = kF304Const29D40;
      l5 = i0;
    }
    i0 = l4;
    i1 = actual;
    StoreU32(ctx, i0 + 40u, i1);
    i0 = l4;
    i1 = l5;
    StoreU32(ctx, i0 + 36u, i1);
    i0 = l4;
    i1 = index;
    StoreU32(ctx, i0 + 32u, i1);
    i0 = l4;
    i1 = 32u;
    i0 += i1;
    index = i0;
  }
  i0 = l6;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = kF304MsgBadArgumentDTo;
    i2 = index;
    i0 = RaiseTraceback(ctx, i0, i1, i2);
    l3 = i0;
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    state = i0;
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 160u;
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
  i2 = kF304ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = index;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = actual;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l3;
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l3;
  i1 = l5;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 20u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
