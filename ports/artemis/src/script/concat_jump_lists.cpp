// concat_jump_lists.cpp -- emits the branch head and splices the second jump list onto the tail of the first..
//
// ConcatJumpLists -- emits the branch head and splices the second jump list onto the tail of the first.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 234566-234566 (f605); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void RaiseSyntaxError(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t EmitInstruction(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

constexpr uint32_t kF605Const1FFFF = 0x1FFFFu;  // 
constexpr uint32_t kF605Const20000 = 0x20000u;  // 
constexpr uint32_t kF605MsgControlStructureTooLong = 0x2A394u;  // control structure too long
constexpr uint32_t kF605Const7FFF8016 = 0x7FFF8016u;  // 
constexpr uint32_t kF605Const7FFFC000 = 0x7FFFC000u;  // 
constexpr uint32_t kF605ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF605SaveFrameBytes = 36u;
constexpr uint32_t kF605ResumeWordBytes = 4u;

}  // namespace

uint32_t ConcatJumpLists(EngineContext* ctx, uint32_t state) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l10 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u;
      l9 = 0, l10 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF605SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l1 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 16u);
    l6 = i0;
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 24u);
    l7 = i0;
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 28u);
    l8 = i0;
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 32u);
    l9 = i0;
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
    i2 = kF605ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l10 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 32u);
    l6 = i0;
    i0 = state;
    i1 = kF605ConstFFFFFFFF;
    StoreU32(ctx, i0 + 32u, i1);
    i0 = state;
    i0 = LoadU32(ctx, i0 + 12u);
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
  }
  i0 = l10;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = kF605Const7FFF8016;
    i2 = l2;
    i0 = EmitInstruction(ctx, i0, i1, i2);
    l1 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l1;
    l5 = i0;
  }
  i0 = l2;
  i1 = l6;
  i2 = kF605ConstFFFFFFFF;
  i1 = i1 != i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l2 = i0;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l9;
    i1 = ResumePhase(ctx);
    i1 = !(i1);
    if (i1) {
      i1 = l5;
      i2 = kF605ConstFFFFFFFF;
      i1 = i1 == i2;
      if (i1) {
        i1 = l6;
        i0 = i1; goto func_exit;
      }
      i1 = state;
      i1 = LoadU32(ctx, i1);
      i1 = LoadU32(ctx, i1 + 12u);
      l7 = i1;
      i1 = l5;
      l2 = i1;
      loop_9: 
        i1 = l2;
        l3 = i1;
        i1 = kF605ConstFFFFFFFF;
        i2 = l3;
        i3 = l7;
        i4 = l2;
        i5 = 2u;
        i4 <<= (i5 & 31);
        i3 += i4;
        l8 = i3;
        i3 = LoadU32(ctx, i3);
        l4 = i3;
        i4 = 14u;
        i3 >>= (i4 & 31);
        i4 = kF605Const1FFFF;
        i3 -= i4;
        l2 = i3;
        i2 += i3;
        i3 = 1u;
        i2 += i3;
        i3 = l2;
        i4 = kF605ConstFFFFFFFF;
        i3 = i3 == i4;
        i1 = i3 ? i1 : i2;
        l2 = i1;
        i2 = kF605ConstFFFFFFFF;
        i1 = i1 != i2;
        if (i1) {goto loop_9;}
      i1 = l8;
      l7 = i1;
      i1 = l6;
      i2 = l3;
      i3 = kF605ConstFFFFFFFF;
      i2 ^= i3;
      i1 += i2;
      l2 = i1;
      i2 = 31u;
      i1 = (uint32_t)((int32_t)i1 >> (i2 & 31));
      l3 = i1;
      i2 = l2;
      i3 = l3;
      i2 += i3;
      i1 ^= i2;
      i2 = kF605Const20000;
      i1 = (uint32_t)((int32_t)i1 >= (int32_t)i2);
      l3 = i1;
    }
    i1 = l3;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l9 = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = state;
        i0 = LoadU32(ctx, i0 + 12u);
        state = i0;
      }
      i0 = l10;
      i1 = 1u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = kF605MsgControlStructureTooLong;
        RaiseSyntaxError(ctx, i0, i1);
        i0 = 1u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l4;
      } else {
        i0 = l8;
        i0 = LoadU32(ctx, i0);
      }
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l7;
      i1 = l4;
      i2 = 16383u;
      i1 &= i2;
      i2 = l2;
      i3 = 14u;
      i2 <<= (i3 & 31);
      i1 |= i2;
      i2 = kF605Const7FFFC000;
      i1 += i2;
      StoreU32(ctx, i0, i1);
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
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
  i2 = kF605ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l1 = i0;
  i1 = state;
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
  i1 = l6;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l1;
  i1 = l5;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l1;
  i1 = l7;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l1;
  i1 = l8;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l1;
  i1 = l9;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 36u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
