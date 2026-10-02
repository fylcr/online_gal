// fix_pending_jump_targets.cpp -- re-encodes every pending jump target of an instruction and seeds a new entry for a fresh line..
//
// FixPendingJumpTargets -- re-encodes every pending jump target of an instruction and seeds a new entry for a fresh line.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 301598-301598 (f784); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);

namespace {

constexpr uint32_t kF784Const1FFFF = 0x1FFFFu;  // 
constexpr uint32_t kF784Const20000 = 0x20000u;  // 
constexpr uint32_t kF784MsgControlStructureTooLong = 0x2A394u;  // control structure too long
constexpr uint32_t kF784Const7FFFC000 = 0x7FFFC000u;  // 
constexpr uint32_t kF784ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF784SaveFrameBytes = 24u;
constexpr uint32_t kF784ResumeWordBytes = 4u;

}  // namespace

void FixPendingJumpTargets(EngineContext* ctx, uint32_t state, uint32_t instruction, uint32_t target) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF784SaveFrameBytes;
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
    target = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
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
    i2 = kF784ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
  }
  i0 = l4;
  i1 = target;
  i2 = kF784ConstFFFFFFFF;
  i1 = i1 != i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l4 = i0;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i1 = 2u;
    i0 = i0 == i1;
    i1 = l7;
    i2 = ResumePhase(ctx);
    i2 = !(i2);
    if (i2) {
      i2 = instruction;
      i2 = LoadU32(ctx, i2);
      l4 = i2;
      i3 = kF784ConstFFFFFFFF;
      i2 = i2 == i3;
      if (i2) {goto block_3;}
      i2 = state;
      i2 = LoadU32(ctx, i2);
      i2 = LoadU32(ctx, i2 + 12u);
      l7 = i2;
      loop_7: 
        i2 = l7;
        i3 = l4;
        instruction = i3;
        i4 = 2u;
        i3 <<= (i4 & 31);
        i2 += i3;
        l3 = i2;
        i2 = LoadU32(ctx, i2);
        l6 = i2;
        i3 = 14u;
        i2 >>= (i3 & 31);
        i3 = kF784Const1FFFF;
        i2 -= i3;
        l4 = i2;
        i2 = kF784ConstFFFFFFFF;
        i3 = instruction;
        i4 = l4;
        i3 += i4;
        i4 = 1u;
        i3 += i4;
        i4 = l4;
        i5 = kF784ConstFFFFFFFF;
        i4 = i4 == i5;
        i2 = i4 ? i2 : i3;
        l4 = i2;
        i3 = kF784ConstFFFFFFFF;
        i2 = i2 != i3;
        if (i2) {goto loop_7;}
      i2 = l3;
      l4 = i2;
      i2 = target;
      i3 = instruction;
      i4 = kF784ConstFFFFFFFF;
      i3 ^= i4;
      i2 += i3;
      instruction = i2;
      i3 = 31u;
      i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
      target = i2;
      i3 = instruction;
      i4 = target;
      i3 += i4;
      i2 ^= i3;
      i3 = kF784Const20000;
      i2 = (uint32_t)((int32_t)i2 >= (int32_t)i3);
      target = i2;
    }
    i2 = target;
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
      i0 = l5;
      i0 = !(i0);
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = kF784MsgControlStructureTooLong;
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
      i1 = instruction;
      i2 = 14u;
      i1 <<= (i2 & 31);
      instruction = i1;
      i2 = l6;
      i3 = 16383u;
      i2 &= i3;
      i1 |= i2;
      i2 = kF784Const7FFFC000;
      i1 += i2;
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
    i0 = instruction;
    i1 = target;
    StoreU32(ctx, i0, i1);
  }
  goto func_exit;
  spill_frame:;
  l5 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l5;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF784ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l5 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = instruction;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l5;
  i1 = target;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l5;
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l5;
  i1 = l6;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l5;
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
