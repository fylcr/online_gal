// set_string_bytes.cpp -- replaces the string contents with the given bytes, growing the string block through the replace-and-copy worker..
//
// SetStringBytes -- replaces the string contents with the given bytes, growing the string block through the replace-and-copy worker.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 531541-531541 (f1233); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void CopyStringBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void GrowStringAndReplace(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7);

namespace {

constexpr uint32_t kF1233Const7FFFFFFF = 0x7FFFFFFFu;  // 
constexpr uint32_t kF1233SaveFrameBytes = 32u;
constexpr uint32_t kF1233ResumeWordBytes = 4u;

}  // namespace

uint32_t SetStringBytes(EngineContext* ctx, uint32_t text, uint32_t bytes, uint32_t length) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u; uint32_t i6 = 0u; uint32_t i7 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1233SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    text = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 4u);
    bytes = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    length = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l6 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 20u);
    l7 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 24u);
    l8 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 28u);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1233ResumeWordBytes;
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
    i0 = GuestStackSp(ctx);
    i1 = 16u;
    i0 -= i1;
    l8 = i0;
    GuestStackSp(ctx) = i0;
    i0 = length;
    l6 = i0;
    i0 = l6;
    i1 = text;
    l3 = i1;
    i1 = LoadU8(ctx, i1 + 11u);
    i2 = 7u;
    i1 >>= (i2 & 31);
    if (i1) {
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 8u);
      i2 = kF1233Const7FFFFFFF;
      i1 &= i2;
      i2 = 1u;
      i1 -= i2;
    } else {
      i1 = 10u;
    }
    l7 = i1;
    i0 = i0 <= i1;
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    if (i0) {
      i0 = l3;
      i0 = LoadU8(ctx, i0 + 11u);
      i1 = 7u;
      i0 >>= (i1 & 31);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0);
        goto block_8;
      }
      i0 = l3;
      block_8:;
      l7 = i0;
      i1 = bytes;
      i2 = length;
      CopyStringBytes(ctx, i0, i1, i2);
      i0 = l8;
      i1 = 0u;
      StoreU8(ctx, i0 + 15u, i1);
      i0 = length;
      i1 = l7;
      i0 += i1;
      i1 = l8;
      i1 = LoadU8(ctx, i1 + 15u);
      StoreU8(ctx, i0, i1);
      i0 = l3;
      i0 = LoadU8(ctx, i0 + 11u);
      i1 = 7u;
      i0 >>= (i1 & 31);
      if (i0) {
        i0 = text;
        i1 = length;
        StoreU32(ctx, i0 + 4u, i1);
        goto block_10;
      }
      i0 = text;
      i1 = length;
      StoreU8(ctx, i0 + 11u, i1);
      block_10:;
      goto block_5;
    }
    i0 = l3;
    l6 = i0;
    i0 = length;
    i1 = l7;
    l4 = i1;
    i0 -= i1;
    l7 = i0;
    i0 = l3;
    i0 = LoadU8(ctx, i0 + 11u);
    i1 = 7u;
    i0 >>= (i1 & 31);
    if (i0) {
      i0 = text;
      i0 = LoadU32(ctx, i0 + 4u);
      goto block_12;
    }
    i0 = text;
    i0 = LoadU8(ctx, i0 + 11u);
    block_12:;
    l3 = i0;
  }
  i0 = l5;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = l4;
    i2 = l7;
    i3 = l3;
    i4 = 0u;
    i5 = l3;
    i6 = length;
    i7 = bytes;
    GrowStringAndReplace(ctx, i0, i1, i2, i3, i4, i5, i6, i7);
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l8;
    i1 = 16u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    i0 = text;
    goto func_exit;
  }
  TrapUnreachable();
  spill_frame:;
  l5 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l5;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1233ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l5 = i0;
  i1 = text;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = bytes;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l5;
  i1 = length;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l5;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l5;
  i1 = l6;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l5;
  i1 = l7;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l5;
  i1 = l8;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l5;
  i1 = l4;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 32u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
