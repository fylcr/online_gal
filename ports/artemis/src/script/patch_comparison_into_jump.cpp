// patch_comparison_into_jump.cpp -- folds a comparison into the branch that tests it, retargeting the jump chain as needed..
//
// PatchComparisonIntoJump -- folds a comparison into the branch that tests it, retargeting the jump chain as needed.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 360750-360750 (f912); R1 transcribe-keep.
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

constexpr uint32_t kF912Const1FFFF = 0x1FFFFu;  // 
constexpr uint32_t kF912Const20000 = 0x20000u;  // 
constexpr uint32_t kF912MsgControlStructureTooLong = 0x2A394u;  // control structure too long
constexpr uint32_t kF912Const2B460 = 0x2B460u;  // 
constexpr uint32_t kF912Const7FC000 = 0x7FC000u;  // 
constexpr uint32_t kF912Const7FFFC000 = 0x7FFFC000u;  // 
constexpr uint32_t kF912ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF912SaveFrameBytes = 36u;
constexpr uint32_t kF912ResumeWordBytes = 4u;

}  // namespace

void PatchComparisonIntoJump(EngineContext* ctx, uint32_t state, uint32_t instruction, uint32_t expression) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u; uint32_t i6 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF912SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 4u);
    instruction = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    expression = i0;
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
    i0 = LoadU32(ctx, i0 + 32u);
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF912ResumeWordBytes;
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
    i0 = state;
    i0 = LoadU32(ctx, i0 + 24u);
    i1 = expression;
    i0 = i0 == i1;
    l3 = i0;
  }
  i0 = l3;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i1 = 2u;
    i0 = i0 == i1;
    i1 = l10;
    i2 = ResumePhase(ctx);
    i2 = !(i2);
    if (i2) {
      i2 = state;
      i3 = expression;
      StoreU32(ctx, i2 + 28u, i3);
      i2 = instruction;
      i3 = kF912ConstFFFFFFFF;
      i2 = i2 == i3;
      if (i2) {goto running_exit;}
      i2 = state;
      i2 = LoadU32(ctx, i2 + 32u);
      expression = i2;
      i3 = kF912ConstFFFFFFFF;
      i2 = i2 == i3;
      if (i2) {goto block_3;}
      i2 = state;
      i2 = LoadU32(ctx, i2);
      i2 = LoadU32(ctx, i2 + 12u);
      l9 = i2;
      loop_10: 
        i2 = expression;
        l7 = i2;
        i2 = kF912ConstFFFFFFFF;
        i3 = l7;
        i4 = l9;
        i5 = expression;
        i6 = 2u;
        i5 <<= (i6 & 31);
        i4 += i5;
        l4 = i4;
        i4 = LoadU32(ctx, i4);
        l3 = i4;
        i5 = 14u;
        i4 >>= (i5 & 31);
        i5 = kF912Const1FFFF;
        i4 -= i5;
        expression = i4;
        i3 += i4;
        i4 = 1u;
        i3 += i4;
        i4 = expression;
        i5 = kF912ConstFFFFFFFF;
        i4 = i4 == i5;
        i2 = i4 ? i2 : i3;
        expression = i2;
        i3 = kF912ConstFFFFFFFF;
        i2 = i2 != i3;
        if (i2) {goto loop_10;}
      i2 = l4;
      l8 = i2;
      i2 = l7;
      i3 = kF912ConstFFFFFFFF;
      i2 ^= i3;
      i3 = instruction;
      i2 += i3;
      instruction = i2;
      i3 = 31u;
      i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
      expression = i2;
      i3 = instruction;
      i4 = expression;
      i3 += i4;
      l7 = i3;
      i2 ^= i3;
      i3 = kF912Const20000;
      i2 = (uint32_t)((int32_t)i2 >= (int32_t)i3);
      expression = i2;
    }
    i2 = expression;
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
      i0 = l6;
      i0 = !(i0);
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = kF912MsgControlStructureTooLong;
        RaiseSyntaxError(ctx, i0, i1);
        i0 = 0u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l3;
      } else {
        i0 = l4;
        i0 = LoadU32(ctx, i0);
      }
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l8;
      i1 = l3;
      i2 = 16383u;
      i1 &= i2;
      i2 = instruction;
      i3 = 14u;
      i2 <<= (i3 & 31);
      i1 |= i2;
      i2 = kF912Const7FFFC000;
      i1 += i2;
      StoreU32(ctx, i0, i1);
      goto func_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = instruction;
    i1 = kF912ConstFFFFFFFF;
    i0 = i0 == i1;
    l3 = i0;
    if (i0) {goto running_exit;}
  }
  loop_17: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      i1 = instruction;
      i2 = 2u;
      i1 <<= (i2 & 31);
      i0 += i1;
      l7 = i0;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i1 = 14u;
      i0 >>= (i1 & 31);
      i1 = kF912Const1FFFF;
      i0 -= i1;
      l4 = i0;
      i1 = kF912ConstFFFFFFFF;
      i0 = i0 == i1;
      l8 = i0;
      i0 = instruction;
      i1 = l4;
      i0 += i1;
      i1 = 1u;
      i0 += i1;
      l10 = i0;
      i0 = instruction;
      i1 = 1u;
      i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
      if (i0) {
        i0 = l7;
        i1 = kF912ResumeWordBytes;
        i0 -= i1;
        l9 = i0;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i1 = 63u;
        i0 &= i1;
        i1 = kF912Const2B460;
        i0 += i1;
        i0 = LoadS8(ctx, i0);
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        if (i0) {goto block_19;}
      }
      i0 = l3;
      l4 = i0;
      i0 = l7;
      l9 = i0;
      block_19:;
      i0 = l4;
      i1 = 63u;
      i0 &= i1;
      i1 = 27u;
      i0 = i0 == i1;
      l5 = i0;
      i0 = kF912ConstFFFFFFFF;
      i1 = l10;
      i2 = l8;
      i0 = i2 ? i0 : i1;
      l8 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      if (i0) {
        i0 = l9;
        i1 = l4;
        i2 = kF912Const7FC000;
        i1 &= i2;
        l5 = i1;
        i2 = l4;
        i3 = 17u;
        i2 >>= (i3 & 31);
        i3 = 32704u;
        i2 &= i3;
        i1 |= i2;
        i2 = 26u;
        i1 |= i2;
        StoreU32(ctx, i0, i1);
        i0 = expression;
        i1 = instruction;
        i2 = kF912ConstFFFFFFFF;
        i1 ^= i2;
        i0 += i1;
        instruction = i0;
        i1 = 31u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        l4 = i0;
        i0 = l4;
        i1 = instruction;
        i2 = l4;
        i1 += i2;
        i0 ^= i1;
        i1 = kF912Const1FFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        l3 = i0;
        if (i0) {goto block_24;}
        goto block_22;
      }
      i0 = expression;
      i1 = instruction;
      i2 = kF912ConstFFFFFFFF;
      i1 ^= i2;
      i0 += i1;
      instruction = i0;
      i1 = 31u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l4 = i0;
      i1 = instruction;
      i2 = l4;
      i1 += i2;
      l9 = i1;
      i0 ^= i1;
      i1 = kF912Const20000;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l4 = i0;
      if (i0) {goto block_21;}
      block_24:;
      i0 = state;
      i0 = LoadU32(ctx, i0 + 12u);
      l3 = i0;
    }
    i0 = l6;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l3;
      i1 = kF912MsgControlStructureTooLong;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    block_22:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l7;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
    }
    block_21:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l7;
      i1 = l3;
      i2 = 16383u;
      i1 &= i2;
      i2 = instruction;
      i3 = 14u;
      i2 <<= (i3 & 31);
      i1 |= i2;
      i2 = kF912Const7FFFC000;
      i1 += i2;
      StoreU32(ctx, i0, i1);
      i0 = l8;
      instruction = i0;
      i1 = kF912ConstFFFFFFFF;
      i0 = i0 != i1;
      l3 = i0;
      if (i0) {goto loop_17;}
    }
  running_exit:;
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
  l6 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l6;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF912ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l6 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l6;
  i1 = instruction;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l6;
  i1 = expression;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l6;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l6;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l6;
  i1 = l7;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l6;
  i1 = l8;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l6;
  i1 = l9;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l6;
  i1 = l5;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 36u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
