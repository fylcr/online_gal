// relocate_element_array.cpp -- moves a table element array into a freshly allocated block and updates the owner pointer..
//
// RelocateElementArray -- moves a table element array into a freshly allocated block and updates the owner pointer.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 189921-189921 (f450); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size);
uint32_t RaiseBlockTooBig(EngineContext* ctx, uint32_t a0);

namespace {

constexpr uint32_t kF450ConstFFFFFFF = 0xFFFFFFFu;  // 
constexpr uint32_t kF450SaveFrameBytes = 24u;
constexpr uint32_t kF450ResumeWordBytes = 4u;

}  // namespace

void RelocateElementArray(EngineContext* ctx, uint32_t state, uint32_t table) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF450SaveFrameBytes;
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
    i0 = LoadU32(ctx, i0 + 4u);
    table = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF450ResumeWordBytes;
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
    i0 = table;
    i1 = 6u;
    i0 += i1;
    l5 = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 32u);
    l4 = i0;
    i0 = table;
    i1 = 7u;
    i0 += i1;
    i1 = kF450ConstFFFFFFF;
    i0 = i0 <= i1;
    l2 = i0;
  }
  i0 = l2;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 44u);
      i1 = 4u;
      i0 <<= (i1 & 31);
      l6 = i0;
      i0 = l5;
      i1 = 4u;
      i0 <<= (i1 & 31);
      l2 = i0;
    }
    i0 = l7;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l4;
      i2 = l6;
      i3 = l2;
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
    if (i0) {goto block_3;}
  }
  i0 = l7;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i0 = RaiseBlockTooBig(ctx, i0);
    l3 = i0;
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    l2 = i0;
  }
  block_3:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i1 = l5;
    StoreU32(ctx, i0 + 44u, i1);
    i0 = state;
    i1 = l2;
    StoreU32(ctx, i0 + 32u, i1);
    i0 = state;
    i1 = l2;
    i2 = table;
    i3 = 4u;
    i2 <<= (i3 & 31);
    i1 += i2;
    StoreU32(ctx, i0 + 28u, i1);
    i0 = state;
    i1 = l2;
    i2 = state;
    i2 = LoadU32(ctx, i2 + 8u);
    i3 = l4;
    i2 -= i3;
    i1 += i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = state;
    i0 = LoadU32(ctx, i0 + 104u);
    table = i0;
    i0 = !(i0);
    if (i0) {goto block_11;}
    i0 = table;
    i1 = table;
    i1 = LoadU32(ctx, i1 + 8u);
    i2 = l4;
    i1 -= i2;
    i2 = l2;
    i1 += i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = table;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = !(i0);
    if (i0) {goto block_11;}
    loop_12: 
      i0 = l2;
      i1 = state;
      i1 = LoadU32(ctx, i1 + 32u);
      i2 = l2;
      i2 = LoadU32(ctx, i2 + 8u);
      i3 = l4;
      i2 -= i3;
      i1 += i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      if (i0) {goto loop_12;}
    block_11:;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 32u);
    table = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 40u);
    l2 = i0;
    i1 = state;
    i1 = LoadU32(ctx, i1 + 20u);
    l5 = i1;
    i0 = i0 <= i1;
    if (i0) {
      loop_14: 
        i0 = l2;
        i1 = l2;
        i1 = LoadU32(ctx, i1 + 8u);
        i2 = l4;
        i1 -= i2;
        i2 = table;
        i1 += i2;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l2;
        i1 = l2;
        i1 = LoadU32(ctx, i1);
        i2 = l4;
        i1 -= i2;
        i2 = table;
        i1 += i2;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = l2;
        i1 = LoadU32(ctx, i1 + 4u);
        i2 = l4;
        i1 -= i2;
        i2 = table;
        i1 += i2;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = l5;
        i1 = l2;
        i2 = 24u;
        i1 += i2;
        l2 = i1;
        i0 = i0 >= i1;
        if (i0) {goto loop_14;}
    }
    i0 = state;
    i1 = state;
    i1 = LoadU32(ctx, i1 + 12u);
    i2 = l4;
    i1 -= i2;
    i2 = table;
    i1 += i2;
    StoreU32(ctx, i0 + 12u, i1);
  }
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
  i2 = kF450ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = table;
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
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 24u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
