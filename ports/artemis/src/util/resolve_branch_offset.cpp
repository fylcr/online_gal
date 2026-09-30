// resolve_branch_offset.cpp -- emits a branch and folds its jump offset, raising when the control structure outgrows the offset field..
//
// ResolveBranchOffset -- emits a branch and folds its jump offset, raising when the control structure outgrows the offset field.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 207719-207719 (f520); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
uint32_t EmitInstruction(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

constexpr uint32_t kF520Const1FFFF = 0x1FFFFu;  // 
constexpr uint32_t kF520Const20000 = 0x20000u;  // 
constexpr uint32_t kF520MsgControlStructureTooLong = 0x2A394u;  // control structure too long
constexpr uint32_t kF520Const7FFF8016 = 0x7FFF8016u;  // 
constexpr uint32_t kF520Const7FFFC000 = 0x7FFFC000u;  // 
constexpr uint32_t kF520ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF520SaveFrameBytes = 36u;
constexpr uint32_t kF520ResumeWordBytes = 4u;

}  // namespace

uint32_t ResolveBranchOffset(EngineContext* ctx, uint32_t state, uint32_t opcode, uint32_t operands, uint32_t extra, uint32_t target) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF520SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    operands = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 12u);
    extra = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 16u);
    target = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 24u);
    l7 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 28u);
    l8 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 32u);
    l10 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 4u);
    opcode = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF520ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l9 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = operands;
    i1 = 6u;
    i0 <<= (i1 & 31);
    i1 = opcode;
    i0 |= i1;
    i1 = extra;
    i2 = 23u;
    i1 <<= (i2 & 31);
    i0 |= i1;
    i1 = target;
    i2 = 14u;
    i1 <<= (i2 & 31);
    i0 |= i1;
    opcode = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 12u);
    i0 = LoadU32(ctx, i0 + 8u);
    operands = i0;
  }
  i0 = l9;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = opcode;
    i2 = operands;
    i0 = EmitInstruction(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    opcode = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 32u);
    l6 = i0;
    i0 = state;
    i1 = kF520ConstFFFFFFFF;
    StoreU32(ctx, i0 + 32u, i1);
    i0 = state;
    i0 = LoadU32(ctx, i0 + 12u);
    i0 = LoadU32(ctx, i0 + 8u);
    opcode = i0;
  }
  i0 = l9;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = kF520Const7FFF8016;
    i2 = opcode;
    i0 = EmitInstruction(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    opcode = i0;
  }
  i0 = operands;
  i1 = l6;
  i2 = kF520ConstFFFFFFFF;
  i1 = i1 != i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  operands = i0;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l10;
    i1 = ResumePhase(ctx);
    i1 = !(i1);
    if (i1) {
      i1 = opcode;
      i2 = kF520ConstFFFFFFFF;
      i1 = i1 == i2;
      if (i1) {
        i1 = l6;
        i0 = i1; goto func_exit;
      }
      i1 = state;
      i1 = LoadU32(ctx, i1);
      i1 = LoadU32(ctx, i1 + 12u);
      l7 = i1;
      i1 = opcode;
      target = i1;
      loop_11: 
        i1 = l7;
        i2 = target;
        operands = i2;
        i3 = 2u;
        i2 <<= (i3 & 31);
        i1 += i2;
        l8 = i1;
        i1 = LoadU32(ctx, i1);
        extra = i1;
        i2 = 14u;
        i1 >>= (i2 & 31);
        i2 = kF520Const1FFFF;
        i1 -= i2;
        target = i1;
        i1 = kF520ConstFFFFFFFF;
        i2 = operands;
        i3 = target;
        i2 += i3;
        i3 = 1u;
        i2 += i3;
        i3 = target;
        i4 = kF520ConstFFFFFFFF;
        i3 = i3 == i4;
        i1 = i3 ? i1 : i2;
        target = i1;
        i2 = kF520ConstFFFFFFFF;
        i1 = i1 != i2;
        if (i1) {goto loop_11;}
      i1 = l8;
      l7 = i1;
      i1 = l6;
      i2 = operands;
      i3 = kF520ConstFFFFFFFF;
      i2 ^= i3;
      i1 += i2;
      operands = i1;
      i2 = 31u;
      i1 = (uint32_t)((int32_t)i1 >> (i2 & 31));
      target = i1;
      i2 = operands;
      i3 = target;
      i2 += i3;
      i1 ^= i2;
      i2 = kF520Const20000;
      i1 = (uint32_t)((int32_t)i1 >= (int32_t)i2);
      target = i1;
    }
    i1 = target;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l10 = i0;
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
      i0 = l9;
      i1 = 2u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = kF520MsgControlStructureTooLong;
        RaiseSyntaxError(ctx, i0, i1);
        i0 = 2u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = extra;
      } else {
        i0 = l8;
        i0 = LoadU32(ctx, i0);
      }
      extra = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l7;
      i1 = extra;
      i2 = 16383u;
      i1 &= i2;
      i2 = operands;
      i3 = 14u;
      i2 <<= (i3 & 31);
      i1 |= i2;
      i2 = kF520Const7FFFC000;
      i1 += i2;
      StoreU32(ctx, i0, i1);
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = opcode;
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
  i2 = kF520ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l5 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = opcode;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l5;
  i1 = operands;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l5;
  i1 = extra;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l5;
  i1 = target;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l5;
  i1 = l6;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l5;
  i1 = l7;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l5;
  i1 = l8;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l5;
  i1 = l10;
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
