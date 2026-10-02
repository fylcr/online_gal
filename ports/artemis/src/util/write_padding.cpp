// write_padding.cpp -- the field padding of the formatter: the padding character is repeated into a 256 byte stack buffer and flushed in whole blocks until the requested width is reached, and nothing is written when the width already fits or the left-adjust and zero-pad flags are set..
//
// WritePadding -- the field padding of the formatter: the padding character is repeated into a 256 byte stack buffer and flushed in whole blocks until the requested width is reached, and nothing is written when the width already fits or the left-adjust and zero-pad flags are set.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 178729-178729 (f411); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t FillBytes(EngineContext* ctx, uint32_t dst, uint32_t value, uint32_t length);
void InvokeCallbackWhenFlagUnset(EngineContext* ctx, uint32_t state, uint32_t slot, uint32_t callback);

namespace {

constexpr uint32_t kF411SaveFrameBytes = 12u;
constexpr uint32_t kF411ResumeWordBytes = 4u;

}  // namespace

void WritePadding(EngineContext* ctx, uint32_t stream, uint32_t ch, uint32_t width, uint32_t length, uint32_t flags) {
  EnterFrame();
  uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF411SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    stream = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 4u);
    width = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF411ResumeWordBytes;
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
    i1 = 256u;
    i0 -= i1;
    l5 = i0;
    GuestStackSp(ctx) = i0;
    i0 = width;
    i1 = length;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    l7 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    if (i0) {goto running_exit;}
    i0 = flags;
    i1 = 73728u;
    i0 &= i1;
    if (i0) {goto running_exit;}
    i0 = ch;
    i1 = 255u;
    i0 &= i1;
    flags = i0;
    i0 = width;
    i1 = length;
    i0 -= i1;
    width = i0;
    i1 = 256u;
    i0 = i0 < i1;
    ch = i0;
    i0 = l5;
    i1 = flags;
    i2 = width;
    i3 = 256u;
    i4 = ch;
    i2 = i4 ? i2 : i3;
    i0 = FillBytes(ctx, i0, i1, i2);
    i0 = ch;
    i0 = !(i0);
    ch = i0;
  }
  i0 = ch;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    loop_7: 
      i0 = l6;
      i0 = !(i0);
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = stream;
        i1 = l5;
        i2 = 256u;
        InvokeCallbackWhenFlagUnset(ctx, i0, i1, i2);
        i0 = 0u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = width;
        i1 = 256u;
        i0 -= i1;
        width = i0;
        i1 = 255u;
        i0 = i0 > i1;
        if (i0) {goto loop_7;}
      }
  }
  i0 = l6;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = stream;
    i1 = l5;
    i2 = width;
    InvokeCallbackWhenFlagUnset(ctx, i0, i1, i2);
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = 256u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
  }
  goto func_exit;
  spill_frame:;
  ch = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = ch;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF411ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  ch = i0;
  i1 = stream;
  StoreU32(ctx, i0, i1);
  i0 = ch;
  i1 = width;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = ch;
  i1 = l5;
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
