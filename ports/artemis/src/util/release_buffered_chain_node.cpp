// release_buffered_chain_node.cpp -- unlinks one buffered-stream node from the global chain, flushes its pending bytes, runs the virtual teardown hook and frees the node..
//
// ReleaseBufferedChainNode -- unlinks one buffered-stream node from the global chain, flushes its pending bytes, runs the virtual teardown hook and frees the node.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 153678-153678 (f360); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
uint32_t FlushBufferedChain(EngineContext* ctx, uint32_t a);

namespace {

constexpr uint32_t kF360ConstCF8CC = 0xCF8CCu;  // 
constexpr uint32_t kF360SaveFrameBytes = 20u;
constexpr uint32_t kF360ResumeWordBytes = 4u;

}  // namespace

uint32_t ReleaseBufferedChainNode(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF360SaveFrameBytes;
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
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
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
    i2 = kF360ResumeWordBytes;
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
    i0 = a;
    i0 = LoadU32(ctx, i0 + 76u);
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    l4 = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i1 = 1u;
    i0 &= i1;
    l5 = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 52u);
      l3 = i0;
      if (i0) {
        i0 = l3;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 56u);
        StoreU32(ctx, i0 + 56u, i1);
      }
      i0 = a;
      i0 = LoadU32(ctx, i0 + 56u);
      l2 = i0;
      if (i0) {
        i0 = l2;
        i1 = l3;
        StoreU32(ctx, i0 + 52u, i1);
      }
      i0 = kF360ConstCF8CC;
      i0 = LoadU32(ctx, i0);
      i1 = a;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {
        i0 = kF360ConstCF8CC;
        i1 = l2;
        StoreU32(ctx, i0, i1);
      }
    }
  }
  i0 = l6;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i0 = FlushBufferedChain(ctx, i0);
    l1 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l1;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 12u);
    l2 = i0;
  }
  i0 = l6;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = l2;
    i0 = CallTableT2(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    l1 = i0;
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l1;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 96u);
    l1 = i0;
    if (i0) {
      i0 = l1;
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l5;
    i0 = !(i0);
    if (i0) {
      i0 = a;
      DeallocateRawBlock(ctx, i0);
      goto block_13;
    }
    i0 = l4;
    i0 = !(i0);
    if (i0) {goto block_13;}
    block_13:;
    i0 = l2;
    i1 = l3;
    i0 |= i1;
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
  i2 = kF360ResumeWordBytes;
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
  i0 = l1;
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l1;
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
