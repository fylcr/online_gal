// reallocate_format_buffer.cpp -- reallocates the format buffer: the buffer grows to hold the text being formatted..
//
// ReallocateFormatBuffer -- reallocates the format buffer: the buffer grows to hold the text being formatted.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 298377-298377 (f776); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size);
uint32_t RaiseBlockTooBig(EngineContext* ctx, uint32_t a);

namespace {

constexpr uint32_t kF776ConstFFFFFFFD = 0xFFFFFFFDu;  // 
constexpr uint32_t kF776SaveFrameBytes = 20u;
constexpr uint32_t kF776ResumeWordBytes = 4u;

}  // namespace

uint32_t ReallocateFormatBuffer(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
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
    i2 = kF776SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF776ResumeWordBytes;
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
    i0 = b;
    i0 = LoadU32(ctx, i0 + 8u);
    l5 = i0;
    i1 = c;
    i0 = i0 >= i1;
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0);
      goto func_exit;
    }
    i0 = c;
    i1 = 32u;
    i2 = c;
    i3 = 32u;
    i2 = i2 > i3;
    i0 = i2 ? i0 : i1;
    l4 = i0;
    i1 = 1u;
    i0 += i1;
    i1 = kF776ConstFFFFFFFD;
    i0 = i0 <= i1;
    c = i0;
  }
  i0 = c;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0);
      c = i0;
    }
    i0 = l6;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = c;
      i2 = l5;
      i3 = l4;
      i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
      l3 = i0;
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l3;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_3;}
  }
  i0 = l6;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i0 = RaiseBlockTooBig(ctx, i0);
    l3 = i0;
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    c = i0;
  }
  block_3:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i1 = l4;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = b;
    i1 = c;
    StoreU32(ctx, i0, i1);
    i0 = c;
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
  i2 = kF776ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = c;
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
