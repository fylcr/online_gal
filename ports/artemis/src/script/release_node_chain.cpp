// release_node_chain.cpp -- unlinks a node from its chain and hands the block back to the allocator..
//
// ReleaseNodeChain -- unlinks a node from its chain and hands the block back to the allocator.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 221210-221210 (f549); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size);
void ResetNodeChainFlags(EngineContext* ctx, uint32_t a0, uint32_t a1);

namespace {

constexpr uint32_t kF549ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF549SaveFrameBytes = 28u;
constexpr uint32_t kF549ResumeWordBytes = 4u;

}  // namespace

void ReleaseNodeChain(EngineContext* ctx, uint32_t state, uint32_t chain) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF549SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 24u);
    l7 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    chain = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF549ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l8 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 104u);
    l2 = i0;
    i0 = !(i0);
    l4 = i0;
    if (i0) {goto block_3;}
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    l7 = i0;
  }
  loop_5: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 8u);
      l5 = i0;
      i1 = chain;
      i0 = i0 < i1;
      if (i0) {goto block_3;}
      i0 = state;
      i1 = l2;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0 + 104u, i1);
      i0 = l2;
      i1 = 16u;
      i0 += i1;
      l4 = i0;
      i0 = l2;
      i0 = LoadU8(ctx, i0 + 5u);
      i1 = l7;
      i1 = LoadU8(ctx, i1 + 20u);
      i2 = kF549ConstFFFFFFFF;
      i1 ^= i2;
      i0 &= i1;
      i1 = 3u;
      i0 &= i1;
      l6 = i0;
    }
    i0 = l6;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i1 = l5;
        i0 = i0 != i1;
        l5 = i0;
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 4u);
          i1 = l4;
          i1 = LoadU32(ctx, i1);
          l6 = i1;
          StoreU32(ctx, i0 + 16u, i1);
          i0 = l4;
          i0 = LoadU32(ctx, i0);
          l5 = i0;
          i1 = l4;
          i1 = LoadU32(ctx, i1 + 4u);
          l4 = i1;
          StoreU32(ctx, i0 + 20u, i1);
        }
      }
      i0 = l8;
      i0 = !(i0);
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = l2;
        i2 = 32u;
        i3 = 0u;
        i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
        l3 = i0;
        i0 = 0u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l3;
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_6;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 20u);
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 16u);
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 16u);
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 20u);
      l6 = i1;
      StoreU32(ctx, i0 + 20u, i1);
      i0 = l2;
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 8u);
      l5 = i1;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0 + 16u, j1);
      i0 = l2;
      i1 = l5;
      i1 = LoadU32(ctx, i1 + 8u);
      l5 = i1;
      StoreU32(ctx, i0 + 24u, i1);
      i0 = l2;
      i1 = l4;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = state;
      i1 = l2;
      ResetNodeChainFlags(ctx, i0, i1);
    }
    block_6:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 104u);
      l2 = i0;
      if (i0) {goto loop_5;}
    }
  block_3:;
  goto func_exit;
  spill_frame:;
  l3 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l3;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF549ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = chain;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l3;
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l3;
  i1 = l5;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l3;
  i1 = l6;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l3;
  i1 = l7;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 28u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
