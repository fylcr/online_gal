// fwrite_buffered.cpp -- the buffered write core: count bytes are copied from src into the stream buffer at +20, first opening or growing that buffer through PrepareStreamWrite when it is unset, and the write cursor is advanced..
//
// FwriteBuffered -- the buffered write core: count bytes are copied from src into the stream buffer at +20, first opening or growing that buffer through PrepareStreamWrite when it is unset, and the write cursor is advanced.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 386593-386593 (f962); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t PrepareStreamWrite(EngineContext* ctx, uint32_t stream);
uint32_t CopyBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

constexpr uint32_t kF962SaveFrameBytes = 24u;
constexpr uint32_t kF962ResumeWordBytes = 4u;

}  // namespace

uint32_t FwriteBuffered(EngineContext* ctx, uint32_t src, uint32_t count, uint32_t stream) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF962SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l6 = i0;
    i0 = LoadU32(ctx, i0);
    src = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 4u);
    count = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 8u);
    stream = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF962ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l7 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = stream;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = count;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = !(i0);
    if (i0) {
      i0 = stream;
      i0 = PrepareStreamWrite(ctx, i0);
      if (i0) {goto running_exit;}
      i0 = stream;
      i0 = LoadU32(ctx, i0 + 16u);
      l4 = i0;
    }
    i0 = l5;
    i1 = l4;
    i2 = stream;
    i2 = LoadU32(ctx, i2 + 20u);
    l6 = i2;
    i1 -= i2;
    i0 = i0 > i1;
    l3 = i0;
  }
  i0 = l3;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = stream;
      i0 = LoadU32(ctx, i0 + 36u);
      l3 = i0;
    }
    i0 = l7;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = stream;
      i1 = src;
      i2 = count;
      i3 = l3;
      i0 = CallTableT4(ctx, i3, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l5 = i0;
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      src = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = src;
      goto func_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = stream;
    i0 = LoadS8(ctx, i0 + 75u);
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    if (i0) {
      i0 = 0u;
      l4 = i0;
      goto block_12;
    }
    i0 = count;
    l3 = i0;
    loop_15: 
      i0 = l3;
      l4 = i0;
      i0 = !(i0);
      l3 = i0;
      if (i0) {
        i0 = 0u;
        l4 = i0;
        goto block_12;
      }
      i0 = l4;
      i1 = 1u;
      i0 -= i1;
      l3 = i0;
      i1 = src;
      i0 += i1;
      i0 = LoadU8(ctx, i0);
      i1 = 10u;
      i0 = i0 != i1;
      if (i0) {goto loop_15;}
    i0 = stream;
    i0 = LoadU32(ctx, i0 + 36u);
    l3 = i0;
  }
  i0 = l7;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = stream;
    i1 = src;
    i2 = l4;
    i3 = l3;
    i0 = CallTableT4(ctx, i3, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    l5 = i0;
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = l4;
    i0 = i0 < i1;
    if (i0) {goto running_exit;}
    i0 = count;
    i1 = l4;
    i0 -= i1;
    count = i0;
    i0 = stream;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = src;
    i1 = l4;
    i0 += i1;
    src = i0;
  }
  block_12:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i1 = src;
    i2 = count;
    i0 = CopyBytes(ctx, i0, i1, i2);
    i0 = stream;
    i1 = count;
    i2 = stream;
    i2 = LoadU32(ctx, i2 + 20u);
    i1 += i2;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = count;
    i1 = l4;
    i0 += i1;
    l3 = i0;
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
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
  i2 = kF962ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l5 = i0;
  i1 = src;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = count;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l5;
  i1 = stream;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l5;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l5;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l5;
  i1 = l6;
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
