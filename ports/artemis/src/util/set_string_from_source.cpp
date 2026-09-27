// set_string_from_source.cpp -- replaces the string contents from a source string, delegating the grow-and-copy work..
//
// SetStringFromSource -- replaces the string contents from a source string, delegating the grow-and-copy work.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 116989-116989 (f226); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t SetStringBytes(EngineContext* ctx, uint32_t text, uint32_t bytes, uint32_t length);

namespace {

constexpr uint32_t kF226SaveFrameBytes = 12u;
constexpr uint32_t kF226ResumeWordBytes = 4u;

}  // namespace

uint32_t SetStringFromSource(EngineContext* ctx, uint32_t text, uint32_t source) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF226SaveFrameBytes;
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
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF226ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  i1 = l4;
  i2 = l2;
  i3 = text;
  i4 = source;
  i3 = i3 != i4;
  i4 = ResumePhase(ctx);
  i2 = i4 ? i2 : i3;
  l2 = i2;
  i3 = ResumePhase(ctx);
  i1 = i3 ? i1 : i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = text;
      l2 = i0;
      i0 = source;
      i0 = LoadU8(ctx, i0 + 11u);
      i1 = 7u;
      i0 >>= (i1 & 31);
      if (i0) {
        i0 = source;
        i0 = LoadU32(ctx, i0);
        goto block_5;
      }
      i0 = source;
      block_5:;
      text = i0;
      i0 = source;
      i0 = LoadU8(ctx, i0 + 11u);
      i1 = 7u;
      i0 >>= (i1 & 31);
      if (i0) {
        i0 = source;
        i0 = LoadU32(ctx, i0 + 4u);
        goto block_7;
      }
      i0 = source;
      i0 = LoadU8(ctx, i0 + 11u);
      block_7:;
      source = i0;
    }
    i0 = l3;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = text;
      i2 = source;
      i0 = SetStringBytes(ctx, i0, i1, i2);
      l3 = i0;
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l3;
    } else {
      i0 = text;
    }
    text = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
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
  i2 = kF226ResumeWordBytes;
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
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 12u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
