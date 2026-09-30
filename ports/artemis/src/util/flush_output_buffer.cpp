// flush_output_buffer.cpp -- hands the pending bytes of one output buffer to its sink callback, resets the counters and reports whether anything was pending..
//
// FlushOutputBuffer -- hands the pending bytes of one output buffer to its sink callback, resets the counters and reports whether anything was pending.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 401607-401607 (f1016); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF1016ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF1016SaveFrameBytes = 24u;
constexpr uint32_t kF1016ResumeWordBytes = 4u;

}  // namespace

uint32_t FlushOutputBuffer(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint64_t l6 = 0;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  uint64_t j0 = 0; uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1016SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i0 = l2;
    j0 = LoadU64(ctx, i0 + 12u);
    l6 = j0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 4u);
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1016ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 28u);
    l3 = i0;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 20u);
    i0 = i0 >= i1;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    if (i0) {goto running_exit;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 36u);
    l1 = i0;
  }
  i0 = l4;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 0u;
    i2 = 0u;
    i3 = l1;
    i0 = CallTableT4(ctx, i3, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    l2 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 20u);
    l1 = i0;
    if (i0) {goto running_exit;}
    i0 = kF1016ConstFFFFFFFF;
    goto func_exit;
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  if (i0) {
    i0 = l5;
  } else {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 4u);
    l1 = i0;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 8u);
    l3 = i1;
    i0 = i0 < i1;
  }
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l1;
      i1 = l3;
      i0 -= i1;
      j0 = (uint64_t)(int64_t)(int32_t)(i0);
      l6 = j0;
      i0 = a;
      i0 = LoadU32(ctx, i0 + 40u);
      l1 = i0;
    }
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      j1 = l6;
      i2 = 1u;
      i3 = l1;
      j0 = CallTableT31(ctx, i3, i0, j1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = 0u;
    StoreU32(ctx, i0 + 28u, i1);
    i0 = a;
    j1 = 0ull;
    StoreU64(ctx, i0 + 16u, j1);
    i0 = a;
    j1 = 0ull;
    StoreU64(ctx, i0 + 4u, j1);
    i0 = 0u;
    goto func_exit;
  }
  TrapUnreachable();
  spill_frame:;
  l2 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1016ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l2 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l2;
  i1 = l1;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l2;
  i1 = l3;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l2;
  j1 = l6;
  StoreU64(ctx, i0 + 12u, j1);
  i0 = l2;
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
