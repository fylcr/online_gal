// reshape_tagged_node_chain.cpp -- reshapes a tagged node in place, dispatching on its tag to the matching node reshape..
//
// ReshapeTaggedNodeChain -- reshapes a tagged node in place, dispatching on its tag to the matching node reshape.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 301149-301149 (f783); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size);
void ReshapePowerOfTwoNode(EngineContext* ctx, uint32_t state, uint32_t node);
void ReshapeCountedArrayNode(EngineContext* ctx, uint32_t state, uint32_t node);
void ReshapeHeaderVariantNode(EngineContext* ctx, uint32_t state, uint32_t node);
void ReshapeFieldRecordNode(EngineContext* ctx, uint32_t state, uint32_t node);
void ReshapeInlinePairNode(EngineContext* ctx, uint32_t state, uint32_t node);

namespace {

constexpr uint32_t kF783ConstFFFFFFFD = 0xFFFFFFFDu;  // 
constexpr uint32_t kF783SaveFrameBytes = 32u;
constexpr uint32_t kF783ResumeWordBytes = 4u;

}  // namespace

uint32_t ReshapeTaggedNodeChain(EngineContext* ctx, uint32_t state, uint32_t node, uint32_t tag) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF783SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    tag = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 20u);
    l7 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 24u);
    l8 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 28u);
    l9 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 4u);
    node = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF783ResumeWordBytes;
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
    i0 = node;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = !(i0);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    if (i0) {goto running_exit;}
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    l8 = i0;
    i0 = LoadU8(ctx, i0 + 20u);
    l3 = i0;
    i1 = 3u;
    i0 ^= i1;
    l9 = i0;
  }
  loop_6: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = tag;
      i0 = !(i0);
      if (i0) {goto running_exit;}
      i0 = l4;
      i0 = LoadU8(ctx, i0 + 4u);
      i1 = 8u;
      i0 = i0 == i1;
      l3 = i0;
    }
    i0 = l3;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l3;
      i1 = l4;
      i2 = 104u;
      i1 += i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      l3 = i0;
      i0 = l6;
      i0 = !(i0);
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = l3;
        i2 = kF783ConstFFFFFFFD;
        i0 = ReshapeTaggedNodeChain(ctx, i0, i1, i2);
        l5 = i0;
        i0 = 0u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l5;
      } else {
        i0 = l3;
      }
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l9;
      i1 = l4;
      i1 = LoadU8(ctx, i1 + 5u);
      l3 = i1;
      i2 = 3u;
      i1 ^= i2;
      i0 &= i1;
      l7 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l7;
      if (i0) {
        i0 = l4;
        i1 = l3;
        i2 = 248u;
        i1 &= i2;
        l3 = i1;
        i2 = l8;
        i2 = LoadU8(ctx, i2 + 20u);
        i3 = 3u;
        i2 &= i3;
        i1 |= i2;
        StoreU8(ctx, i0 + 5u, i1);
        i0 = l4;
        node = i0;
        goto block_12;
      }
      i0 = node;
      i1 = l4;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l8;
      i1 = LoadU32(ctx, i1 + 28u);
      i0 = i0 == i1;
      if (i0) {
        i0 = l8;
        i1 = l4;
        i1 = LoadU32(ctx, i1);
        StoreU32(ctx, i0 + 28u, i1);
      }
      i0 = l4;
      i0 = LoadU8(ctx, i0 + 4u);
      i1 = 4u;
      i0 -= i1;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      switch (i0) {
        case 0: goto block_17;
        case 1: goto block_19;
        case 2: goto block_21;
        case 3: goto block_16;
        case 4: goto block_18;
        case 5: goto block_23;
        case 6: goto block_20;
        default: goto block_12;
      }
      block_23:;
    }
    i0 = l6;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l4;
      ReshapeFieldRecordNode(ctx, i0, i1);
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_12;}
    block_21:;
    i0 = l6;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l4;
      ReshapeHeaderVariantNode(ctx, i0, i1);
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_12;}
    block_20:;
    i0 = l6;
    i1 = 3u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l4;
      ReshapeInlinePairNode(ctx, i0, i1);
      i0 = 3u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_12;}
    block_19:;
    i0 = l6;
    i1 = 4u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l4;
      ReshapePowerOfTwoNode(ctx, i0, i1);
      i0 = 4u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_12;}
    block_18:;
    i0 = l6;
    i1 = 5u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l4;
      ReshapeCountedArrayNode(ctx, i0, i1);
      i0 = 5u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_12;}
    block_17:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 16u);
      l3 = i0;
      i0 = LoadU32(ctx, i0 + 4u);
      i1 = 1u;
      i0 -= i1;
      l7 = i0;
      i0 = l3;
      i1 = l7;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 12u);
      i1 = 17u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l6;
    i1 = 6u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l4;
      i2 = l3;
      i3 = 0u;
      i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
      l5 = i0;
      i0 = 6u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_12;}
    block_16:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 16u);
      i1 = 24u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l6;
    i1 = 7u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l4;
      i2 = l3;
      i3 = 0u;
      i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
      i0 = 7u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    block_12:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = tag;
      i1 = 1u;
      i0 -= i1;
      tag = i0;
      i0 = node;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      if (i0) {goto loop_6;}
    }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = node;
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
  i2 = kF783ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l5 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = node;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l5;
  i1 = tag;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l5;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l5;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l5;
  i1 = l7;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l5;
  i1 = l8;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l5;
  i1 = l9;
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
