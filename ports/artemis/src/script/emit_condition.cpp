// emit_condition.cpp -- emits a condition: the test becomes a comparison plus a jump..
//
// EmitCondition -- emits a condition: the test becomes a comparison plus a jump.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 2746118-2746118 (f5672); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
uint32_t ExpressionToRegister(EngineContext* ctx, uint32_t func_state, uint32_t expr);
void DischargeExpression(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t ResolveBranchOffset(EngineContext* ctx, uint32_t state, uint32_t opcode, uint32_t operands, uint32_t extra, uint32_t target);
void CompileExpression(EngineContext* ctx, uint32_t func_state, uint32_t code, uint32_t line);
void EmitVariableStore(EngineContext* ctx, uint32_t state, uint32_t variable);
void ExpressionToOperand(EngineContext* ctx, uint32_t func_state, uint32_t expr, uint32_t operand);

namespace {

constexpr uint32_t kF5672Const1FFFF = 0x1FFFFu;  // 
constexpr uint32_t kF5672Const20000 = 0x20000u;  // 
constexpr uint32_t kF5672MsgFunctionOrExpressionToo = 0x2A371u;  // function or expression too complex
constexpr uint32_t kF5672MsgControlStructureTooLong = 0x2A394u;  // control structure too long
constexpr uint32_t kF5672Const7FFFC000 = 0x7FFFC000u;  // 
constexpr uint32_t kF5672ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF5672SaveFrameBytes = 44u;
constexpr uint32_t kF5672ResumeWordBytes = 4u;

}  // namespace

void EmitCondition(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u;
      l11 = 0, l12 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF5672SaveFrameBytes;
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
    l3 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
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
    i2 = kF5672ResumeWordBytes;
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
    i0 = b;
    switch (i0) {
      case 0: goto block_7;
      case 1: goto block_7;
      case 2: goto block_7;
      case 3: goto block_7;
      case 4: goto block_7;
      case 5: goto block_7;
      case 6: goto block_8;
      case 7: goto block_6;
      case 8: goto block_6;
      case 9: goto block_6;
      case 10: goto block_6;
      case 11: goto block_6;
      case 12: goto block_6;
      case 13: goto block_11;
      case 14: goto block_9;
      default: goto block_6;
    }
    block_11:;
  }
  i0 = l7;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = c;
    EmitVariableStore(ctx, i0, i1);
    i0 = 0u;
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
  block_9:;
  i0 = l7;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = c;
    DischargeExpression(ctx, i0, i1);
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i0 = LoadU32(ctx, i0);
    i1 = 1u;
    i0 -= i1;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    switch (i0) {
      case 0: goto block_3;
      case 1: goto block_20;
      case 2: goto block_3;
      case 3: goto block_20;
      case 4: goto block_20;
      case 5: goto block_20;
      case 6: goto block_20;
      case 7: goto block_20;
      case 8: goto block_20;
      case 9: goto block_23;
      case 10: goto block_22;
      case 11: goto block_18;
      default: goto block_20;
    }
    block_23:;
    i0 = c;
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
    goto block_16;
    block_22:;
    i0 = a;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = LoadU32(ctx, i0 + 12u);
    i1 = c;
    i1 = LoadU32(ctx, i1 + 8u);
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i1 = 63u;
    i0 &= i1;
    i1 = 19u;
    i0 = i0 != i1;
    l5 = i0;
    if (i0) {goto block_19;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 24u);
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l3;
    i1 = 23u;
    i0 >>= (i1 & 31);
    b = i0;
  }
  i0 = l7;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 26u;
    i2 = b;
    i3 = 0u;
    i4 = 0u;
    i0 = ResolveBranchOffset(ctx, i0, i1, i2, i3, i4);
    l4 = i0;
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_16;}
  block_20:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0);
    b = i0;
  }
  block_19:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 36u);
    l6 = i0;
    i1 = 1u;
    i0 += i1;
    l5 = i0;
    i0 = l6;
    i1 = b;
    i1 = LoadU8(ctx, i1 + 75u);
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    if (i0) {
      i0 = l5;
      l3 = i0;
      goto block_27;
    }
    i0 = l6;
    i1 = 249u;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    l8 = i0;
    i0 = l5;
    l3 = i0;
  }
  i0 = l10;
  i1 = l8;
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
      i0 = a;
      i0 = LoadU32(ctx, i0 + 12u);
      b = i0;
    }
    i0 = l7;
    i1 = 3u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = kF5672MsgFunctionOrExpressionToo;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 3u;
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
      i0 = LoadU32(ctx, i0 + 36u);
      i1 = 1u;
      i0 += i1;
      l3 = i0;
      i0 = a;
      i0 = LoadU32(ctx, i0);
    }
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i1 = l5;
    StoreU8(ctx, i0 + 75u, i1);
  }
  block_27:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = l3;
    StoreU32(ctx, i0 + 36u, i1);
    i0 = l3;
    i1 = 1u;
    i0 -= i1;
    b = i0;
  }
  i0 = l7;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = c;
    i2 = b;
    ExpressionToOperand(ctx, i0, i1, i2);
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_17;}
  }
  block_18:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
    i1 = 256u;
    i0 &= i1;
    l3 = i0;
    if (i0) {goto block_17;}
    i0 = a;
    i0 = LoadU8(ctx, i0 + 50u);
    l3 = i0;
    i1 = b;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    b = i0;
    if (i0) {goto block_17;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    b = i1;
    StoreU32(ctx, i0 + 36u, i1);
  }
  block_17:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
  }
  i0 = l7;
  i1 = 5u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 27u;
    i2 = 255u;
    i3 = b;
    i4 = 1u;
    i0 = ResolveBranchOffset(ctx, i0, i1, i2, i3, i4);
    l4 = i0;
    i0 = 5u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    b = i0;
  }
  block_16:;
  i0 = l11;
  i1 = ResumePhase(ctx);
  i1 = !(i1);
  if (i1) {
    i1 = b;
    l6 = i1;
    i1 = b;
    i2 = kF5672ConstFFFFFFFF;
    i1 = i1 == i2;
    b = i1;
    if (i1) {goto block_3;}
    i1 = c;
    i1 = LoadU32(ctx, i1 + 16u);
    b = i1;
    i2 = kF5672ConstFFFFFFFF;
    i1 = i1 == i2;
    l3 = i1;
    if (i1) {goto running_exit;}
    i1 = a;
    i1 = LoadU32(ctx, i1);
    i1 = LoadU32(ctx, i1 + 12u);
    l9 = i1;
    loop_43: 
      i1 = b;
      l3 = i1;
      i1 = kF5672ConstFFFFFFFF;
      i2 = l3;
      i3 = l9;
      i4 = b;
      i5 = 2u;
      i4 <<= (i5 & 31);
      i3 += i4;
      l8 = i3;
      i3 = LoadU32(ctx, i3);
      l5 = i3;
      i4 = 14u;
      i3 >>= (i4 & 31);
      i4 = kF5672Const1FFFF;
      i3 -= i4;
      b = i3;
      i2 += i3;
      i3 = 1u;
      i2 += i3;
      i3 = b;
      i4 = kF5672ConstFFFFFFFF;
      i3 = i3 == i4;
      i1 = i3 ? i1 : i2;
      b = i1;
      i2 = kF5672ConstFFFFFFFF;
      i1 = i1 != i2;
      if (i1) {goto loop_43;}
    i1 = l8;
    l9 = i1;
    i1 = l6;
    i2 = l3;
    i3 = kF5672ConstFFFFFFFF;
    i2 ^= i3;
    i1 += i2;
    b = i1;
    i2 = 31u;
    i1 = (uint32_t)((int32_t)i1 >> (i2 & 31));
    l3 = i1;
    i2 = b;
    i3 = l3;
    i2 += i3;
    l6 = i2;
    i1 ^= i2;
    i2 = kF5672Const20000;
    i1 = (uint32_t)((int32_t)i1 >= (int32_t)i2);
    l3 = i1;
  }
  i1 = l3;
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
      l3 = i0;
    }
    i0 = l7;
    i1 = 6u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l3;
      i1 = kF5672MsgControlStructureTooLong;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 6u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l5;
    } else {
      i0 = l8;
      i0 = LoadU32(ctx, i0);
    }
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l9;
    i1 = l5;
    i2 = 16383u;
    i1 &= i2;
    l3 = i1;
    i2 = b;
    i3 = 14u;
    i2 <<= (i3 & 31);
    i1 |= i2;
    i2 = kF5672Const7FFFC000;
    i1 += i2;
    b = i1;
    StoreU32(ctx, i0, i1);
    goto block_3;
  }
  block_8:;
  i0 = l7;
  i1 = 7u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = c;
    DischargeExpression(ctx, i0, i1);
    i0 = 7u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    if (i0) {goto block_51;}
    i0 = c;
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_51;}
    i0 = a;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = b;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_51;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 36u, i1);
    block_51:;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 36u);
    l6 = i0;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    i0 = l6;
    i1 = a;
    i1 = LoadU32(ctx, i1);
    l5 = i1;
    i1 = LoadU8(ctx, i1 + 75u);
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    if (i0) {
      i0 = l3;
      b = i0;
      goto block_52;
    }
    i0 = l6;
    i1 = 249u;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    l8 = i0;
    i0 = l3;
    b = i0;
  }
  i0 = l8;
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
    i0 = l7;
    i1 = 8u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = kF5672MsgFunctionOrExpressionToo;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 8u;
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
      l5 = i0;
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
    i0 = l5;
    i1 = l3;
    StoreU8(ctx, i0 + 75u, i1);
  }
  block_52:;
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
  i0 = l7;
  i1 = 9u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = c;
    i2 = b;
    CompileExpression(ctx, i0, i1, i2);
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
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i0 = LoadU32(ctx, i0);
    i1 = 5u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_64;}
    i0 = c;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = kF5672ConstFFFFFFFF;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_64;}
    i0 = c;
    i0 = LoadU32(ctx, i0 + 20u);
    i1 = kF5672ConstFFFFFFFF;
    i0 = i0 == i1;
    b = i0;
    if (i0) {goto block_5;}
    block_64:;
  }
  i0 = l7;
  i1 = 10u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = c;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 10u;
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
    goto func_exit;
  }
  block_6:;
  i0 = l7;
  i1 = 11u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = c;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 11u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    a = i0;
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    goto func_exit;
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i1 = l6;
    StoreU32(ctx, i0 + 16u, i1);
  }
  block_3:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 24u);
    StoreU32(ctx, i0 + 28u, i1);
    i0 = l6;
    i1 = kF5672ConstFFFFFFFF;
    i0 = i0 == i1;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    if (i0) {goto block_71;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 32u);
    b = i0;
    i1 = kF5672ConstFFFFFFFF;
    i0 = i0 != i1;
    l3 = i0;
  }
  i0 = l3;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l12;
    i1 = ResumePhase(ctx);
    i1 = !(i1);
    if (i1) {
      i1 = a;
      i1 = LoadU32(ctx, i1);
      i1 = LoadU32(ctx, i1 + 12u);
      l9 = i1;
      loop_77: 
        i1 = b;
        l3 = i1;
        i1 = kF5672ConstFFFFFFFF;
        i2 = l3;
        i3 = l9;
        i4 = b;
        i5 = 2u;
        i4 <<= (i5 & 31);
        i3 += i4;
        l8 = i3;
        i3 = LoadU32(ctx, i3);
        l5 = i3;
        i4 = 14u;
        i3 >>= (i4 & 31);
        i4 = kF5672Const1FFFF;
        i3 -= i4;
        b = i3;
        i2 += i3;
        i3 = 1u;
        i2 += i3;
        i3 = b;
        i4 = kF5672ConstFFFFFFFF;
        i3 = i3 == i4;
        i1 = i3 ? i1 : i2;
        b = i1;
        i2 = kF5672ConstFFFFFFFF;
        i1 = i1 != i2;
        if (i1) {goto loop_77;}
      i1 = l8;
      l9 = i1;
      i1 = l6;
      i2 = l3;
      i3 = kF5672ConstFFFFFFFF;
      i2 ^= i3;
      i1 += i2;
      b = i1;
      i2 = 31u;
      i1 = (uint32_t)((int32_t)i1 >> (i2 & 31));
      l3 = i1;
      i2 = b;
      i3 = l3;
      i2 += i3;
      l6 = i2;
      i1 ^= i2;
      i2 = kF5672Const20000;
      i1 = (uint32_t)((int32_t)i1 >= (int32_t)i2);
      l3 = i1;
    }
    i1 = l3;
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
      i0 = l7;
      i1 = 12u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = kF5672MsgControlStructureTooLong;
        RaiseSyntaxError(ctx, i0, i1);
        i0 = 12u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l5;
      } else {
        i0 = l8;
        i0 = LoadU32(ctx, i0);
      }
      l5 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l9;
      i1 = l5;
      i2 = 16383u;
      i1 &= i2;
      i2 = b;
      i3 = 14u;
      i2 <<= (i3 & 31);
      i1 |= i2;
      i2 = kF5672Const7FFFC000;
      i1 += i2;
      StoreU32(ctx, i0, i1);
      goto block_71;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = l6;
    StoreU32(ctx, i0 + 32u, i1);
  }
  block_71:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i1 = kF5672ConstFFFFFFFF;
    StoreU32(ctx, i0 + 20u, i1);
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
  i2 = kF5672ResumeWordBytes;
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
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l4;
  i1 = l5;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l4;
  i1 = l6;
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
