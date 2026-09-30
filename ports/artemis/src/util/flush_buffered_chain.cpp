// flush_buffered_chain.cpp -- flushes the object's own write buffer and then walks the global chain flushing every node that still has pending bytes..
//
// FlushBufferedChain -- flushes the object's own write buffer and then walks the global chain flushing every node that still has pending bytes.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 259191-259191 (f673); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t FlushOutputBuffer(EngineContext* ctx, uint32_t a);

namespace {

constexpr uint32_t kF673ConstC8710 = 0xC8710u;  // 
constexpr uint32_t kF673ConstCF8CC = 0xCF8CCu;  // 
constexpr uint32_t kF673ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF673SaveFrameBytes = 12u;
constexpr uint32_t kF673ResumeWordBytes = 4u;

}  // namespace

uint32_t FlushBufferedChain(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF673SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l1 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 4u);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF673ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
  }
  i0 = a;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 76u);
      i1 = kF673ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l2 = i0;
    }
    i0 = l2;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l4;
      i0 = !(i0);
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i0 = FlushOutputBuffer(ctx, i0);
        l1 = i0;
        i0 = 0u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l1;
        a = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        goto func_exit;
      }
    }
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i0 = FlushOutputBuffer(ctx, i0);
      l1 = i0;
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l1;
      a = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      goto func_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF673ConstC8710;
    i0 = LoadU32(ctx, i0);
    a = i0;
  }
  i0 = a;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF673ConstC8710;
      i0 = LoadU32(ctx, i0);
      a = i0;
    }
    i0 = l4;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i0 = FlushBufferedChain(ctx, i0);
      l1 = i0;
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l1;
    } else {
      i0 = l2;
    }
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF673ConstCF8CC;
    i0 = LoadU32(ctx, i0);
    a = i0;
  }
  i0 = a;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    loop_19: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 76u);
        i0 = a;
        i0 = LoadU32(ctx, i0 + 20u);
        i1 = a;
        i1 = LoadU32(ctx, i1 + 28u);
        i0 = i0 > i1;
        l3 = i0;
      }
      i0 = l3;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = l2;
        i1 = l4;
        i2 = 3u;
        i1 = i1 == i2;
        i2 = 1u;
        i3 = ResumePhase(ctx);
        i1 = i3 ? i1 : i2;
        if (i1) {
          i1 = a;
          i1 = FlushOutputBuffer(ctx, i1);
          l1 = i1;
          i1 = 3u;
          i2 = ResumePhase(ctx);
          i3 = 1u;
          i2 = i2 == i3;
          if (i2) {i0 = i1; goto spill_frame;}
          i1 = l1;
          l3 = i1;
        }
        i1 = l2;
        i2 = l3;
        i1 |= i2;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 56u);
        a = i0;
        if (i0) {goto loop_19;}
      }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    goto func_exit;
  }
  TrapUnreachable();
  spill_frame:;
  l1 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l1;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF673ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l1 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l1;
  i1 = l2;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l1;
  i1 = l3;
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
