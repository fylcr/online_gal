// fwrite.cpp -- the fwrite shape: size times count bytes are handed to FwriteBuffered and the count of complete size-byte records is answered; the stream state word at +76 is consulted first..
//
// Fwrite -- the fwrite shape: size times count bytes are handed to FwriteBuffered and the count of complete size-byte records is answered; the stream state word at +76 is consulted first.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 161626-161626 (f366); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t FwriteBuffered(EngineContext* ctx, uint32_t src, uint32_t count, uint32_t stream);

namespace {

constexpr uint32_t kF366ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF366SaveFrameBytes = 24u;
constexpr uint32_t kF366ResumeWordBytes = 4u;

}  // namespace

uint32_t Fwrite(EngineContext* ctx, uint32_t ptr, uint32_t size, uint32_t count, uint32_t stream) {
  EnterFrame();
  uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF366SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    ptr = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 4u);
    size = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    count = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 12u);
    stream = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 16u);
    l6 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF366ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l7 = i0;
  }
  i0 = ResumePhase(ctx);
  if (i0) {
    i0 = l4;
  } else {
    i0 = size;
    i1 = count;
    i0 *= i1;
    l6 = i0;
    l5 = i0;
    i0 = stream;
    i0 = LoadU32(ctx, i0 + 76u);
    i1 = kF366ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
  }
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l7;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = ptr;
      i1 = l6;
      i2 = stream;
      i0 = FwriteBuffered(ctx, i0, i1, i2);
      l4 = i0;
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      ptr = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_3;}
  }
  i0 = l7;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = ptr;
    i1 = l6;
    i2 = stream;
    i0 = FwriteBuffered(ctx, i0, i1, i2);
    l4 = i0;
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    ptr = i0;
  }
  block_3:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = ptr;
    i1 = l5;
    i0 = i0 == i1;
    if (i0) {
      i0 = count;
      i1 = 0u;
      i2 = size;
      i0 = i2 ? i0 : i1;
      goto func_exit;
    }
    i0 = ptr;
    i1 = size;
    i0 = DIV_U(i0, i1);
    goto func_exit;
  }
  TrapUnreachable();
  spill_frame:;
  l4 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l4;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF366ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  i1 = ptr;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = size;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l4;
  i1 = count;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l4;
  i1 = stream;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l4;
  i1 = l6;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l4;
  i1 = l5;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 24u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
