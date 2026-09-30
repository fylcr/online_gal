// emit_variable_store.cpp -- emits the store for one variable kind, from a local slot up to an upvalue cell..
//
// EmitVariableStore -- emits the store for one variable kind, from a local slot up to an upvalue cell.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 359188-359188 (f909); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
void DischargeExpression(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t ResolveBranchOffset(EngineContext* ctx, uint32_t state, uint32_t opcode, uint32_t operands, uint32_t extra, uint32_t target);
void ExpressionToOperand(EngineContext* ctx, uint32_t func_state, uint32_t expr, uint32_t operand);

namespace {

constexpr uint32_t kF909Const1FFFF = 0x1FFFFu;  // 
constexpr uint32_t kF909Const20000 = 0x20000u;  // 
constexpr uint32_t kF909MsgFunctionOrExpressionToo = 0x2A371u;  // function or expression too complex
constexpr uint32_t kF909MsgControlStructureTooLong = 0x2A394u;  // control structure too long
constexpr uint32_t kF909Const2B460 = 0x2B460u;  // 
constexpr uint32_t kF909Const7FFFC000 = 0x7FFFC000u;  // 
constexpr uint32_t kF909ConstFFFFC03F = 0xFFFFC03Fu;  // 
constexpr uint32_t kF909ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF909SaveFrameBytes = 44u;
constexpr uint32_t kF909ResumeWordBytes = 4u;

}  // namespace

void EmitVariableStore(EngineContext* ctx, uint32_t state, uint32_t variable) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
      l10 = 0, l11 = 0, l12 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF909SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 24u);
    l7 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 28u);
    l8 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 32u);
    l10 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 36u);
    l11 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 40u);
    l12 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 4u);
    variable = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF909ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l9 = i0;
  }
  i0 = l9;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = variable;
    DischargeExpression(ctx, i0, i1);
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = variable;
    i0 = LoadU32(ctx, i0);
    i1 = 2u;
    i0 -= i1;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    switch (i0) {
      case 0: goto block_6;
      case 1: goto block_11;
      case 2: goto block_6;
      case 3: goto block_6;
      case 4: goto block_11;
      case 5: goto block_11;
      case 6: goto block_11;
      case 7: goto block_11;
      case 8: goto block_14;
      case 9: goto block_13;
      case 10: goto block_9;
      default: goto block_11;
    }
    block_14:;
    i0 = state;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    i1 = variable;
    i1 = LoadU32(ctx, i1 + 8u);
    l3 = i1;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    l2 = i0;
    i0 = l3;
    i1 = 1u;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i1 = 4u;
      i0 -= i1;
      l5 = i0;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i1 = 63u;
      i0 &= i1;
      i1 = kF909Const2B460;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l6 = i0;
      if (i0) {goto block_15;}
    }
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = l2;
    l5 = i0;
    block_15:;
    i0 = l5;
    i1 = l3;
    i2 = kF909ConstFFFFC03F;
    i1 &= i2;
    i2 = l3;
    i3 = 16320u;
    i2 &= i3;
    i2 = !(i2);
    i3 = 6u;
    i2 <<= (i3 & 31);
    l3 = i2;
    i1 |= i2;
    l2 = i1;
    StoreU32(ctx, i0, i1);
    i0 = variable;
    i0 = LoadU32(ctx, i0 + 8u);
    l5 = i0;
    goto block_7;
    block_13:;
    i0 = state;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0 + 12u);
    i1 = variable;
    i1 = LoadU32(ctx, i1 + 8u);
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i1 = 63u;
    i0 &= i1;
    i1 = 19u;
    i0 = i0 != i1;
    l5 = i0;
    if (i0) {goto block_10;}
    i0 = state;
    i1 = state;
    i1 = LoadU32(ctx, i1 + 24u);
    i2 = 1u;
    i1 -= i2;
    l5 = i1;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l2;
    i1 = 23u;
    i0 >>= (i1 & 31);
    l2 = i0;
  }
  i0 = l9;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = 26u;
    i2 = l2;
    i3 = 0u;
    i4 = 1u;
    i0 = ResolveBranchOffset(ctx, i0, i1, i2, i3, i4);
    l4 = i0;
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_7;}
  block_11:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
  }
  block_10:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 36u);
    l6 = i0;
    i1 = 1u;
    i0 += i1;
    l5 = i0;
    i0 = l6;
    i1 = l3;
    i1 = LoadU8(ctx, i1 + 75u);
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {
      i0 = l5;
      l2 = i0;
      goto block_20;
    }
    i0 = l6;
    i1 = 249u;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    l6 = i0;
    i0 = l5;
    l2 = i0;
  }
  i0 = l10;
  i1 = l6;
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
      l2 = i0;
    }
    i0 = l9;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = kF909MsgFunctionOrExpressionToo;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l2;
    } else {
      i0 = state;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = state;
      i0 = LoadU32(ctx, i0 + 36u);
      i1 = 1u;
      i0 += i1;
    }
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = l5;
    StoreU8(ctx, i0 + 75u, i1);
  }
  block_20:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i1 = l2;
    StoreU32(ctx, i0 + 36u, i1);
    i0 = l2;
    i1 = 1u;
    i0 -= i1;
    l2 = i0;
  }
  i0 = l9;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = variable;
    i2 = l2;
    ExpressionToOperand(ctx, i0, i1, i2);
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = variable;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    l2 = i0;
    if (i0) {goto block_8;}
  }
  block_9:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = variable;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i1 = 256u;
    i0 &= i1;
    l2 = i0;
    if (i0) {goto block_8;}
    i0 = l3;
    i1 = state;
    i1 = LoadU8(ctx, i1 + 50u);
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l2 = i0;
    if (i0) {goto block_8;}
    i0 = state;
    i1 = state;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    l2 = i1;
    StoreU32(ctx, i0 + 36u, i1);
  }
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = variable;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
  }
  i0 = l9;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = 27u;
    i2 = 255u;
    i3 = l2;
    i4 = 0u;
    i0 = ResolveBranchOffset(ctx, i0, i1, i2, i3, i4);
    l4 = i0;
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    l5 = i0;
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = kF909ConstFFFFFFFF;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_6;}
    i0 = variable;
    i0 = LoadU32(ctx, i0 + 20u);
    l3 = i0;
    i1 = kF909ConstFFFFFFFF;
    i0 = i0 != i1;
    l2 = i0;
  }
  i0 = l2;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l11;
    i1 = ResumePhase(ctx);
    i1 = !(i1);
    if (i1) {
      i1 = state;
      i1 = LoadU32(ctx, i1);
      i1 = LoadU32(ctx, i1 + 12u);
      l7 = i1;
      loop_39: 
        i1 = l7;
        i2 = l3;
        l2 = i2;
        i3 = 2u;
        i2 <<= (i3 & 31);
        i1 += i2;
        l8 = i1;
        i1 = LoadU32(ctx, i1);
        l6 = i1;
        i2 = 14u;
        i1 >>= (i2 & 31);
        i2 = kF909Const1FFFF;
        i1 -= i2;
        l3 = i1;
        i1 = kF909ConstFFFFFFFF;
        i2 = l2;
        i3 = l3;
        i2 += i3;
        i3 = 1u;
        i2 += i3;
        i3 = l3;
        i4 = kF909ConstFFFFFFFF;
        i3 = i3 == i4;
        i1 = i3 ? i1 : i2;
        l3 = i1;
        i2 = kF909ConstFFFFFFFF;
        i1 = i1 != i2;
        if (i1) {goto loop_39;}
      i1 = l8;
      l7 = i1;
      i1 = l5;
      i2 = l2;
      i3 = kF909ConstFFFFFFFF;
      i2 ^= i3;
      i1 += i2;
      l3 = i1;
      i2 = 31u;
      i1 = (uint32_t)((int32_t)i1 >> (i2 & 31));
      l2 = i1;
      i2 = l3;
      i1 += i2;
      l5 = i1;
      i1 = l2;
      i2 = l5;
      i1 ^= i2;
      i2 = kF909Const20000;
      i1 = (uint32_t)((int32_t)i1 >= (int32_t)i2);
      l2 = i1;
    }
    i1 = l2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l11 = i0;
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
        l2 = i0;
      }
      i0 = l9;
      i1 = 5u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = kF909MsgControlStructureTooLong;
        RaiseSyntaxError(ctx, i0, i1);
        i0 = 5u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l6;
      } else {
        i0 = l8;
        i0 = LoadU32(ctx, i0);
      }
      l6 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l7;
      i1 = l3;
      i2 = 14u;
      i1 <<= (i2 & 31);
      l5 = i1;
      i2 = l6;
      i3 = 16383u;
      i2 &= i3;
      i1 |= i2;
      i2 = kF909Const7FFFC000;
      i1 += i2;
      l2 = i1;
      StoreU32(ctx, i0, i1);
      goto block_6;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = variable;
    i1 = l5;
    StoreU32(ctx, i0 + 20u, i1);
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = variable;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = state;
    i1 = state;
    i1 = LoadU32(ctx, i1 + 24u);
    StoreU32(ctx, i0 + 28u, i1);
    i0 = l5;
    i1 = kF909ConstFFFFFFFF;
    i0 = i0 != i1;
    l2 = i0;
  }
  i0 = l2;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l12;
    i1 = ResumePhase(ctx);
    i1 = !(i1);
    if (i1) {
      i1 = state;
      i1 = LoadU32(ctx, i1 + 32u);
      l3 = i1;
      i2 = kF909ConstFFFFFFFF;
      i1 = i1 == i2;
      if (i1) {goto block_46;}
      i1 = state;
      i1 = LoadU32(ctx, i1);
      i1 = LoadU32(ctx, i1 + 12u);
      l7 = i1;
      loop_52: 
        i1 = l7;
        i2 = l3;
        l2 = i2;
        i3 = 2u;
        i2 <<= (i3 & 31);
        i1 += i2;
        l8 = i1;
        i1 = LoadU32(ctx, i1);
        l6 = i1;
        i2 = 14u;
        i1 >>= (i2 & 31);
        i2 = kF909Const1FFFF;
        i1 -= i2;
        l3 = i1;
        i1 = kF909ConstFFFFFFFF;
        i2 = l2;
        i3 = l3;
        i2 += i3;
        i3 = 1u;
        i2 += i3;
        i3 = l3;
        i4 = kF909ConstFFFFFFFF;
        i3 = i3 == i4;
        i1 = i3 ? i1 : i2;
        l3 = i1;
        i2 = kF909ConstFFFFFFFF;
        i1 = i1 != i2;
        if (i1) {goto loop_52;}
      i1 = l8;
      l7 = i1;
      i1 = l5;
      i2 = l2;
      i3 = kF909ConstFFFFFFFF;
      i2 ^= i3;
      i1 += i2;
      l3 = i1;
      i2 = 31u;
      i1 = (uint32_t)((int32_t)i1 >> (i2 & 31));
      l2 = i1;
      i2 = l3;
      i1 += i2;
      l5 = i1;
      i1 = l2;
      i2 = l5;
      i1 ^= i2;
      i2 = kF909Const20000;
      i1 = (uint32_t)((int32_t)i1 >= (int32_t)i2);
      l2 = i1;
    }
    i1 = l2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l12 = i0;
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
      i1 = 6u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = kF909MsgControlStructureTooLong;
        RaiseSyntaxError(ctx, i0, i1);
        i0 = 6u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l6;
      } else {
        i0 = l8;
        i0 = LoadU32(ctx, i0);
      }
      l6 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l7;
      i1 = l6;
      i2 = 16383u;
      i1 &= i2;
      i2 = l3;
      i3 = 14u;
      i2 <<= (i3 & 31);
      i1 |= i2;
      i2 = kF909Const7FFFC000;
      i1 += i2;
      state = i1;
      StoreU32(ctx, i0, i1);
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = variable;
    i1 = kF909ConstFFFFFFFF;
    StoreU32(ctx, i0 + 16u, i1);
    goto func_exit;
  }
  block_46:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i1 = l5;
    StoreU32(ctx, i0 + 32u, i1);
    i0 = variable;
    i1 = kF909ConstFFFFFFFF;
    StoreU32(ctx, i0 + 16u, i1);
  }
  goto func_exit;
  spill_frame:;
  l4 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l4;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF909ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = variable;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l4;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l4;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l4;
  i1 = l5;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l4;
  i1 = l6;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l4;
  i1 = l7;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l4;
  i1 = l8;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l4;
  i1 = l10;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = l4;
  i1 = l11;
  StoreU32(ctx, i0 + 36u, i1);
  i0 = l4;
  i1 = l12;
  StoreU32(ctx, i0 + 40u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 44u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
