// emit_expression.cpp -- emits an expression: the operand tree is walked and the value lands in a register..
//
// EmitExpression -- emits an expression: the operand tree is walked and the value lands in a register.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 2744407-2744407 (f5671); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
uint32_t ExpressionToRegister(EngineContext* ctx, uint32_t func_state, uint32_t expr);
void DischargeExpression(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t EmitConditionJump(EngineContext* ctx, uint32_t func_state, uint32_t condition);
uint32_t ResolveBranchOffset(EngineContext* ctx, uint32_t state, uint32_t opcode, uint32_t operands, uint32_t extra, uint32_t target);
void CompileNumericPower(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d);
void CompileExpression(EngineContext* ctx, uint32_t func_state, uint32_t code, uint32_t line);

namespace {

constexpr uint32_t kF5671Const1FFFF = 0x1FFFFu;  // 
constexpr uint32_t kF5671Const20000 = 0x20000u;  // 
constexpr uint32_t kF5671MsgFunctionOrExpressionToo = 0x2A371u;  // function or expression too complex
constexpr uint32_t kF5671MsgControlStructureTooLong = 0x2A394u;  // control structure too long
constexpr uint32_t kF5671Const7FFFFF = 0x7FFFFFu;  // 
constexpr uint32_t kF5671Const7FFFC000 = 0x7FFFC000u;  // 
constexpr uint32_t kF5671ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF5671SaveFrameBytes = 44u;
constexpr uint32_t kF5671ResumeWordBytes = 4u;

}  // namespace

void EmitExpression(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u;
  uint64_t j1 = 0;
      l12 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF5671SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    d = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 20u);
    l7 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 24u);
    l8 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 28u);
    l9 = i0;
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
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF5671ResumeWordBytes;
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
    i0 = b;
    switch (i0) {
      case 0: goto block_20;
      case 1: goto block_19;
      case 2: goto block_18;
      case 3: goto block_17;
      case 4: goto block_16;
      case 5: goto block_15;
      case 6: goto block_21;
      case 7: goto block_13;
      case 8: goto block_14;
      case 9: goto block_12;
      case 10: goto block_11;
      case 11: goto block_10;
      case 12: goto block_9;
      case 13: goto block_24;
      case 14: goto block_22;
      default: goto block_8;
    }
    block_24:;
  }
  i0 = l6;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = d;
    DischargeExpression(ctx, i0, i1);
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l11;
  i1 = ResumePhase(ctx);
  i1 = !(i1);
  if (i1) {
    i1 = c;
    i1 = LoadU32(ctx, i1 + 20u);
    l8 = i1;
    i2 = kF5671ConstFFFFFFFF;
    i1 = i1 == i2;
    if (i1) {goto running_exit;}
    i1 = d;
    i1 = LoadU32(ctx, i1 + 20u);
    b = i1;
    i2 = kF5671ConstFFFFFFFF;
    i1 = i1 == i2;
    if (i1) {goto block_7;}
    i1 = a;
    i1 = LoadU32(ctx, i1);
    i1 = LoadU32(ctx, i1 + 12u);
    l10 = i1;
    loop_28: 
      i1 = b;
      l5 = i1;
      i1 = kF5671ConstFFFFFFFF;
      i2 = l5;
      i3 = l10;
      i4 = b;
      i5 = 2u;
      i4 <<= (i5 & 31);
      i3 += i4;
      l9 = i3;
      i3 = LoadU32(ctx, i3);
      l7 = i3;
      i4 = 14u;
      i3 >>= (i4 & 31);
      i4 = kF5671Const1FFFF;
      i3 -= i4;
      b = i3;
      i2 += i3;
      i3 = 1u;
      i2 += i3;
      i3 = b;
      i4 = kF5671ConstFFFFFFFF;
      i3 = i3 == i4;
      i1 = i3 ? i1 : i2;
      b = i1;
      i2 = kF5671ConstFFFFFFFF;
      i1 = i1 != i2;
      if (i1) {goto loop_28;}
    i1 = l9;
    l10 = i1;
    i1 = l8;
    i2 = l5;
    i3 = kF5671ConstFFFFFFFF;
    i2 ^= i3;
    i1 += i2;
    b = i1;
    i2 = 31u;
    i1 = (uint32_t)((int32_t)i1 >> (i2 & 31));
    l5 = i1;
    i2 = b;
    i3 = l5;
    i2 += i3;
    l8 = i2;
    i1 ^= i2;
    i2 = kF5671Const20000;
    i1 = (uint32_t)((int32_t)i1 >= (int32_t)i2);
    l5 = i1;
  }
  i1 = l5;
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
      i0 = a;
      i0 = LoadU32(ctx, i0 + 12u);
      a = i0;
    }
    i0 = l6;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = kF5671MsgControlStructureTooLong;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l7;
    } else {
      i0 = l9;
      i0 = LoadU32(ctx, i0);
    }
    l7 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l10;
    i1 = l7;
    i2 = 16383u;
    i1 &= i2;
    i2 = b;
    i3 = 14u;
    i2 <<= (i3 & 31);
    i1 |= i2;
    i2 = kF5671Const7FFFC000;
    i1 += i2;
    a = i1;
    StoreU32(ctx, i0, i1);
    goto running_exit;
  }
  block_22:;
  i0 = l6;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = d;
    DischargeExpression(ctx, i0, i1);
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l12;
  i1 = ResumePhase(ctx);
  i1 = !(i1);
  if (i1) {
    i1 = c;
    i1 = LoadU32(ctx, i1 + 16u);
    l8 = i1;
    i2 = kF5671ConstFFFFFFFF;
    i1 = i1 == i2;
    if (i1) {goto block_5;}
    i1 = d;
    i1 = LoadU32(ctx, i1 + 16u);
    b = i1;
    i2 = kF5671ConstFFFFFFFF;
    i1 = i1 == i2;
    if (i1) {goto block_6;}
    i1 = a;
    i1 = LoadU32(ctx, i1);
    i1 = LoadU32(ctx, i1 + 12u);
    l10 = i1;
    loop_37: 
      i1 = b;
      l5 = i1;
      i1 = kF5671ConstFFFFFFFF;
      i2 = l5;
      i3 = l10;
      i4 = b;
      i5 = 2u;
      i4 <<= (i5 & 31);
      i3 += i4;
      l9 = i3;
      i3 = LoadU32(ctx, i3);
      l7 = i3;
      i4 = 14u;
      i3 >>= (i4 & 31);
      i4 = kF5671Const1FFFF;
      i3 -= i4;
      b = i3;
      i2 += i3;
      i3 = 1u;
      i2 += i3;
      i3 = b;
      i4 = kF5671ConstFFFFFFFF;
      i3 = i3 == i4;
      i1 = i3 ? i1 : i2;
      b = i1;
      i2 = kF5671ConstFFFFFFFF;
      i1 = i1 != i2;
      if (i1) {goto loop_37;}
    i1 = l9;
    l10 = i1;
    i1 = l8;
    i2 = l5;
    i3 = kF5671ConstFFFFFFFF;
    i2 ^= i3;
    i1 += i2;
    b = i1;
    i2 = 31u;
    i1 = (uint32_t)((int32_t)i1 >> (i2 & 31));
    l5 = i1;
    i2 = b;
    i3 = l5;
    i2 += i3;
    l8 = i2;
    i1 ^= i2;
    i2 = kF5671Const20000;
    i1 = (uint32_t)((int32_t)i1 >= (int32_t)i2);
    l5 = i1;
  }
  i1 = l5;
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
      i0 = a;
      i0 = LoadU32(ctx, i0 + 12u);
      a = i0;
    }
    i0 = l6;
    i1 = 3u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = kF5671MsgControlStructureTooLong;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 3u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l7;
    } else {
      i0 = l9;
      i0 = LoadU32(ctx, i0);
    }
    l7 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l10;
    i1 = l7;
    i2 = 16383u;
    i1 &= i2;
    i2 = b;
    i3 = 14u;
    i2 <<= (i3 & 31);
    i1 |= i2;
    i2 = kF5671Const7FFFC000;
    i1 += i2;
    a = i1;
    StoreU32(ctx, i0, i1);
    goto block_5;
  }
  block_21:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = d;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
    i1 = d;
    i1 = LoadU32(ctx, i1 + 16u);
    i0 = i0 != i1;
    b = i0;
  }
  i0 = b;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l6;
    i1 = 4u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = d;
      i0 = EmitConditionJump(ctx, i0, i1);
      l4 = i0;
      i0 = 4u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_43;}
  }
  i0 = l6;
  i1 = 5u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = d;
    DischargeExpression(ctx, i0, i1);
    i0 = 5u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_43:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = d;
    i0 = LoadU32(ctx, i0);
    i1 = 11u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_50;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    b = i0;
    i1 = d;
    i1 = LoadU32(ctx, i1 + 8u);
    l5 = i1;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l7 = i0;
    i1 = 63u;
    i0 &= i1;
    i1 = 21u;
    i0 = i0 != i1;
    l9 = i0;
    if (i0) {goto block_50;}
    i0 = c;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    if (i0) {goto block_51;}
    i0 = c;
    i0 = LoadU32(ctx, i0 + 8u);
    l8 = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_51;}
    i0 = l8;
    i1 = a;
    i1 = LoadU8(ctx, i1 + 50u);
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {goto block_51;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 36u, i1);
    i0 = b;
    i1 = d;
    i1 = LoadU32(ctx, i1 + 8u);
    l5 = i1;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l7 = i0;
    block_51:;
    i0 = b;
    i1 = l5;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    i1 = l7;
    i2 = kF5671Const7FFFFF;
    i1 &= i2;
    i2 = c;
    i2 = LoadU32(ctx, i2 + 8u);
    i3 = 23u;
    i2 <<= (i3 & 31);
    i1 |= i2;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = 11u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = d;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 8u, i1);
    goto func_exit;
    block_50:;
  }
  i0 = l6;
  i1 = 6u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = d;
    DischargeExpression(ctx, i0, i1);
    i0 = 6u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = d;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    if (i0) {goto block_54;}
    i0 = d;
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_54;}
    i0 = a;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = b;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_54;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 36u, i1);
    block_54:;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 36u);
    l8 = i0;
    i1 = 1u;
    i0 += i1;
    l5 = i0;
    i0 = l8;
    i1 = a;
    i1 = LoadU32(ctx, i1);
    l7 = i1;
    i1 = LoadU8(ctx, i1 + 75u);
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    if (i0) {
      i0 = l5;
      b = i0;
      goto block_55;
    }
    i0 = l8;
    i1 = 249u;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    l9 = i0;
    i0 = l5;
    b = i0;
  }
  i0 = l9;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 12u);
      b = i0;
    }
    i0 = l6;
    i1 = 7u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = kF5671MsgFunctionOrExpressionToo;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 7u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = b;
    } else {
      i0 = a;
      i0 = LoadU32(ctx, i0);
      l7 = i0;
      i0 = a;
      i0 = LoadU32(ctx, i0 + 36u);
      i1 = 1u;
      i0 += i1;
    }
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    i1 = l5;
    StoreU8(ctx, i0 + 75u, i1);
  }
  block_55:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = b;
    StoreU32(ctx, i0 + 36u, i1);
    i0 = b;
    i1 = 1u;
    i0 -= i1;
    b = i0;
  }
  i0 = l6;
  i1 = 8u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = d;
    i2 = b;
    CompileExpression(ctx, i0, i1, i2);
    i0 = 8u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l6;
  i1 = 9u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 21u;
    i2 = c;
    i3 = d;
    CompileNumericPower(ctx, i0, i1, i2, i3);
    i0 = 9u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    goto func_exit;
  }
  block_20:;
  i0 = l6;
  i1 = 10u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 12u;
    i2 = c;
    i3 = d;
    CompileNumericPower(ctx, i0, i1, i2, i3);
    i0 = 10u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    goto func_exit;
  }
  block_19:;
  i0 = l6;
  i1 = 11u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 13u;
    i2 = c;
    i3 = d;
    CompileNumericPower(ctx, i0, i1, i2, i3);
    i0 = 11u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    goto func_exit;
  }
  block_18:;
  i0 = l6;
  i1 = 12u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 14u;
    i2 = c;
    i3 = d;
    CompileNumericPower(ctx, i0, i1, i2, i3);
    i0 = 12u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    goto func_exit;
  }
  block_17:;
  i0 = l6;
  i1 = 13u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 15u;
    i2 = c;
    i3 = d;
    CompileNumericPower(ctx, i0, i1, i2, i3);
    i0 = 13u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    goto func_exit;
  }
  block_16:;
  i0 = l6;
  i1 = 14u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 16u;
    i2 = c;
    i3 = d;
    CompileNumericPower(ctx, i0, i1, i2, i3);
    i0 = 14u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    goto func_exit;
  }
  block_15:;
  i0 = l6;
  i1 = 15u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 17u;
    i2 = c;
    i3 = d;
    CompileNumericPower(ctx, i0, i1, i2, i3);
    i0 = 15u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    goto func_exit;
  }
  block_14:;
  i0 = l6;
  i1 = 16u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = c;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 16u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    b = i0;
  }
  i0 = l6;
  i1 = 17u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = d;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 17u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = d;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    if (i0) {goto block_82;}
    i0 = d;
    i0 = LoadU32(ctx, i0 + 8u);
    d = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_82;}
    i0 = a;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = d;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_82;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 36u, i1);
    block_82:;
    i0 = c;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    d = i0;
    if (i0) {goto block_83;}
    i0 = c;
    i0 = LoadU32(ctx, i0 + 8u);
    d = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_83;}
    i0 = a;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = d;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    d = i0;
    if (i0) {goto block_83;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    d = i1;
    StoreU32(ctx, i0 + 36u, i1);
    block_83:;
  }
  i0 = l6;
  i1 = 18u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 23u;
    i2 = 1u;
    i3 = b;
    i4 = l5;
    i0 = ResolveBranchOffset(ctx, i0, i1, i2, i3, i4);
    l4 = i0;
    i0 = 18u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    a = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_3;}
  block_13:;
  i0 = l6;
  i1 = 19u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = c;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 19u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    b = i0;
  }
  i0 = l6;
  i1 = 20u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = d;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 20u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = d;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    if (i0) {goto block_88;}
    i0 = d;
    i0 = LoadU32(ctx, i0 + 8u);
    d = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_88;}
    i0 = a;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = d;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_88;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 36u, i1);
    block_88:;
    i0 = c;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    d = i0;
    if (i0) {goto block_89;}
    i0 = c;
    i0 = LoadU32(ctx, i0 + 8u);
    d = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_89;}
    i0 = a;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = d;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    d = i0;
    if (i0) {goto block_89;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    d = i1;
    StoreU32(ctx, i0 + 36u, i1);
    block_89:;
  }
  i0 = l6;
  i1 = 21u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 23u;
    i2 = 0u;
    i3 = b;
    i4 = l5;
    i0 = ResolveBranchOffset(ctx, i0, i1, i2, i3, i4);
    l4 = i0;
    i0 = 21u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    a = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_3;}
  block_12:;
  i0 = l6;
  i1 = 22u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = c;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 22u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    b = i0;
  }
  i0 = l6;
  i1 = 23u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = d;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 23u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = d;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    if (i0) {goto block_94;}
    i0 = d;
    i0 = LoadU32(ctx, i0 + 8u);
    d = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_94;}
    i0 = a;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = d;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_94;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 36u, i1);
    block_94:;
    i0 = c;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    d = i0;
    if (i0) {goto block_95;}
    i0 = c;
    i0 = LoadU32(ctx, i0 + 8u);
    d = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_95;}
    i0 = a;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = d;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    d = i0;
    if (i0) {goto block_95;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    d = i1;
    StoreU32(ctx, i0 + 36u, i1);
    block_95:;
  }
  i0 = l6;
  i1 = 24u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 24u;
    i2 = 1u;
    i3 = b;
    i4 = l5;
    i0 = ResolveBranchOffset(ctx, i0, i1, i2, i3, i4);
    l4 = i0;
    i0 = 24u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    a = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_3;}
  block_11:;
  i0 = l6;
  i1 = 25u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = c;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 25u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    b = i0;
  }
  i0 = l6;
  i1 = 26u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = d;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 26u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = d;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    if (i0) {goto block_100;}
    i0 = d;
    i0 = LoadU32(ctx, i0 + 8u);
    d = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_100;}
    i0 = a;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = d;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_100;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 36u, i1);
    block_100:;
    i0 = c;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    d = i0;
    if (i0) {goto block_101;}
    i0 = c;
    i0 = LoadU32(ctx, i0 + 8u);
    d = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_101;}
    i0 = a;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = d;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    d = i0;
    if (i0) {goto block_101;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    d = i1;
    StoreU32(ctx, i0 + 36u, i1);
    block_101:;
  }
  i0 = l6;
  i1 = 27u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 25u;
    i2 = 1u;
    i3 = b;
    i4 = l5;
    i0 = ResolveBranchOffset(ctx, i0, i1, i2, i3, i4);
    l4 = i0;
    i0 = 27u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    a = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_3;}
  block_10:;
  i0 = l6;
  i1 = 28u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = c;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 28u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    b = i0;
  }
  i0 = l6;
  i1 = 29u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = d;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 29u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = d;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    if (i0) {goto block_106;}
    i0 = d;
    i0 = LoadU32(ctx, i0 + 8u);
    d = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_106;}
    i0 = a;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = d;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_106;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 36u, i1);
    block_106:;
    i0 = c;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    d = i0;
    if (i0) {goto block_107;}
    i0 = c;
    i0 = LoadU32(ctx, i0 + 8u);
    d = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_107;}
    i0 = a;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = d;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    d = i0;
    if (i0) {goto block_107;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    d = i1;
    StoreU32(ctx, i0 + 36u, i1);
    block_107:;
  }
  i0 = l6;
  i1 = 30u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 24u;
    i2 = 1u;
    i3 = l5;
    i4 = b;
    i0 = ResolveBranchOffset(ctx, i0, i1, i2, i3, i4);
    l4 = i0;
    i0 = 30u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    a = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_3;}
  block_9:;
  i0 = l6;
  i1 = 31u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = c;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 31u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    b = i0;
  }
  i0 = l6;
  i1 = 32u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = d;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 32u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = d;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    if (i0) {goto block_112;}
    i0 = d;
    i0 = LoadU32(ctx, i0 + 8u);
    d = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_112;}
    i0 = a;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = d;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_112;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 36u, i1);
    block_112:;
    i0 = c;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    d = i0;
    if (i0) {goto block_113;}
    i0 = c;
    i0 = LoadU32(ctx, i0 + 8u);
    d = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_113;}
    i0 = a;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = d;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    d = i0;
    if (i0) {goto block_113;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    d = i1;
    StoreU32(ctx, i0 + 36u, i1);
    block_113:;
  }
  i0 = l6;
  i1 = 33u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 25u;
    i2 = 1u;
    i3 = l5;
    i4 = b;
    i0 = ResolveBranchOffset(ctx, i0, i1, i2, i3, i4);
    l4 = i0;
    i0 = 33u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    a = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i1 = 10u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = a;
    StoreU32(ctx, i0 + 8u, i1);
  }
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    goto func_exit;
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = d;
    i1 = l8;
    StoreU32(ctx, i0 + 20u, i1);
    goto running_exit;
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = d;
    i1 = l8;
    StoreU32(ctx, i0 + 16u, i1);
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i1 = d;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0, j1);
    i0 = c;
    i1 = d;
    j1 = LoadU64(ctx, i1 + 16u);
    StoreU64(ctx, i0 + 16u, j1);
    i0 = c;
    i1 = d;
    j1 = LoadU64(ctx, i1 + 8u);
    StoreU64(ctx, i0 + 8u, j1);
    goto func_exit;
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i1 = d;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0, j1);
    i0 = c;
    i1 = d;
    j1 = LoadU64(ctx, i1 + 16u);
    StoreU64(ctx, i0 + 16u, j1);
    i0 = c;
    i1 = d;
    j1 = LoadU64(ctx, i1 + 8u);
    StoreU64(ctx, i0 + 8u, j1);
    goto func_exit;
  }
  block_3:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i1 = 10u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = a;
    StoreU32(ctx, i0 + 8u, i1);
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
  i2 = kF5671ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l4;
  i1 = c;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l4;
  i1 = d;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l4;
  i1 = l5;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l4;
  i1 = l7;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l4;
  i1 = l8;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l4;
  i1 = l9;
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
