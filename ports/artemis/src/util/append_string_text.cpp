// append_string_text.cpp -- the suspendable append front.
//
// AppendStringText -- the suspendable append front. Grows the text by the source string bytes through the checked append worker and answers the updated handle.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 128731-128731 (f265); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void AppendTextBytes(EngineContext* ctx, uint32_t text, uint32_t bytes, uint32_t length);

namespace {

constexpr uint32_t kF265SaveFrameBytes = 16u;
constexpr uint32_t kF265ResumeWordBytes = 4u;

}  // namespace

uint32_t AppendStringText(EngineContext* ctx, uint32_t text, uint32_t source) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF265SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0);
    text = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 4u);
    source = i0;
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
    i2 = kF265ResumeWordBytes;
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
    i0 = source;
    i0 = LoadU8(ctx, i0 + 11u);
    i1 = 7u;
    i0 >>= (i1 & 31);
    i0 = !(i0);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    if (i0) {
      i0 = text;
      i1 = source;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = text;
      i1 = source;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      goto running_exit;
    }
    i0 = source;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = source;
    i0 = LoadU32(ctx, i0 + 4u);
    source = i0;
  }
  i0 = l3;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = text;
    i1 = l4;
    i2 = source;
    AppendTextBytes(ctx, i0, i1, i2);
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i1 = 16u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    i0 = text;
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
  i2 = kF265ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = text;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = source;
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
