// patch_jump_chain.cpp -- walks a pending jump list and retargets every entry at the chain head..
//
// PatchJumpChain -- walks a pending jump list and retargets every entry at the chain head.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 190209-190209 (f451); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void RaiseSyntaxError(EngineContext* ctx, uint32_t a0, uint32_t a1);

namespace {

constexpr uint32_t kF451Const1FFFF = 0x1FFFFu;  // 
constexpr uint32_t kF451Const20000 = 0x20000u;  // 
constexpr uint32_t kF451MsgControlStructureTooLong = 0x2A394u;  // control structure too long
constexpr uint32_t kF451Const7FFFC000 = 0x7FFFC000u;  // 
constexpr uint32_t kF451ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF451SaveFrameBytes = 24u;
constexpr uint32_t kF451ResumeWordBytes = 4u;

}  // namespace

void PatchJumpChain(EngineContext* ctx, uint32_t state, uint32_t instruction) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u; uint32_t i6 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF451SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    instruction = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l5 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 16u);
    l6 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 20u);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF451ResumeWordBytes;
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
    i0 = state;
    i1 = state;
    i1 = LoadU32(ctx, i1 + 24u);
    StoreU32(ctx, i0 + 28u, i1);
    i0 = instruction;
    i1 = kF451ConstFFFFFFFF;
    i0 = i0 != i1;
    l4 = i0;
  }
  i0 = l4;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i1 = 2u;
    i0 = i0 == i1;
    i1 = l2;
    i2 = ResumePhase(ctx);
    i2 = !(i2);
    if (i2) {
      i2 = state;
      i2 = LoadU32(ctx, i2 + 32u);
      l4 = i2;
      i3 = kF451ConstFFFFFFFF;
      i2 = i2 == i3;
      if (i2) {goto block_3;}
      i2 = state;
      i2 = LoadU32(ctx, i2);
      i2 = LoadU32(ctx, i2 + 12u);
      l2 = i2;
      loop_9: 
        i2 = l4;
        l5 = i2;
        i2 = kF451ConstFFFFFFFF;
        i3 = l5;
        i4 = l2;
        i5 = l4;
        i6 = 2u;
        i5 <<= (i6 & 31);
        i4 += i5;
        l3 = i4;
        i4 = LoadU32(ctx, i4);
        l6 = i4;
        i5 = 14u;
        i4 >>= (i5 & 31);
        i5 = kF451Const1FFFF;
        i4 -= i5;
        l4 = i4;
        i3 += i4;
        i4 = 1u;
        i3 += i4;
        i4 = l4;
        i5 = kF451ConstFFFFFFFF;
        i4 = i4 == i5;
        i2 = i4 ? i2 : i3;
        l4 = i2;
        i3 = kF451ConstFFFFFFFF;
        i2 = i2 != i3;
        if (i2) {goto loop_9;}
      i2 = l3;
      l4 = i2;
      i2 = l5;
      i3 = kF451ConstFFFFFFFF;
      i2 ^= i3;
      i3 = instruction;
      i2 += i3;
      l5 = i2;
      i3 = 31u;
      i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
      instruction = i2;
      i3 = l5;
      i2 += i3;
      l2 = i2;
      i2 = instruction;
      i3 = l2;
      i2 ^= i3;
      i3 = kF451Const20000;
      i2 = (uint32_t)((int32_t)i2 >= (int32_t)i3);
      instruction = i2;
    }
    i2 = instruction;
    i3 = ResumePhase(ctx);
    i1 = i3 ? i1 : i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = state;
        i0 = LoadU32(ctx, i0 + 12u);
        state = i0;
      }
      i0 = l7;
      i0 = !(i0);
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = kF451MsgControlStructureTooLong;
        RaiseSyntaxError(ctx, i0, i1);
        i0 = 0u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l6;
      } else {
        i0 = l3;
        i0 = LoadU32(ctx, i0);
      }
      l6 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = l5;
      i2 = 14u;
      i1 <<= (i2 & 31);
      instruction = i1;
      i2 = l6;
      i3 = 16383u;
      i2 &= i3;
      i1 |= i2;
      i2 = kF451Const7FFFC000;
      i1 += i2;
      state = i1;
      StoreU32(ctx, i0, i1);
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    goto func_exit;
  }
  block_3:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i1 = instruction;
    StoreU32(ctx, i0 + 32u, i1);
  }
  goto func_exit;
  spill_frame:;
  l2 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF451ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l2 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l2;
  i1 = instruction;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l2;
  i1 = l4;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l2;
  i1 = l5;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l2;
  i1 = l6;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l2;
  i1 = l3;
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
