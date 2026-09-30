// emit_statement.cpp -- emits a statement: the statement kind selects the emit path and the register window is closed..
//
// EmitStatement -- emits a statement: the statement kind selects the emit path and the register window is closed.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 2747130-2747130 (f5673); R1 transcribe-keep.
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
void DischargeExpression(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t EmitConditionJump(EngineContext* ctx, uint32_t func_state, uint32_t condition);
void CompileNumericPower(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d);
void ExpressionToOperand(EngineContext* ctx, uint32_t func_state, uint32_t expr, uint32_t operand);

namespace {

constexpr uint32_t kF5673Const1FFFF = 0x1FFFFu;  // 
constexpr uint32_t kF5673MsgFunctionOrExpressionToo = 0x2A371u;  // function or expression too complex
constexpr uint32_t kF5673Const2B460 = 0x2B460u;  // 
constexpr uint32_t kF5673Const7FC000 = 0x7FC000u;  // 
constexpr uint32_t kF5673ConstFFFFC03F = 0xFFFFC03Fu;  // 
constexpr uint32_t kF5673ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF5673SaveFrameBytes = 32u;
constexpr uint32_t kF5673ResumeWordBytes = 4u;

}  // namespace

void EmitStatement(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF5673SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
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
    i0 = LoadU32(ctx, i0 + 24u);
    l8 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 28u);
    l9 = i0;
    i0 = l3;
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
    i2 = kF5673ResumeWordBytes;
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
    i0 = GuestStackSp(ctx);
    l4 = i0;
    i1 = 32u;
    i0 -= i1;
    l8 = i0;
    GuestStackSp(ctx) = i0;
    i0 = l8;
    j1 = 18446744073709551615ull;
    StoreU64(ctx, i0 + 24u, j1);
    i0 = l8;
    j1 = 0ull;
    StoreU64(ctx, i0 + 16u, j1);
    i0 = l8;
    i1 = 5u;
    StoreU32(ctx, i0 + 8u, i1);
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    switch (i0) {
      case 0: goto block_8;
      case 1: goto block_6;
      case 2: goto block_5;
      default: goto running_exit;
    }
    block_8:;
    i0 = c;
    i0 = LoadU32(ctx, i0);
    i1 = 5u;
    i0 = i0 != i1;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    if (i0) {goto block_11;}
    i0 = c;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = kF5673ConstFFFFFFFF;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_11;}
    i0 = c;
    i0 = LoadU32(ctx, i0 + 20u);
    i1 = kF5673ConstFFFFFFFF;
    i0 = i0 == i1;
    b = i0;
    if (i0) {goto block_9;}
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
    i0 = EmitConditionJump(ctx, i0, i1);
    l3 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    b = i0;
  }
  block_9:;
  i0 = b;
  i1 = l8;
  i2 = 8u;
  i1 += i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  b = i0;
  i0 = l7;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 18u;
    i2 = c;
    i3 = b;
    CompileNumericPower(ctx, i0, i1, i2, i3);
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto running_exit;}
  block_6:;
  i0 = l7;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = c;
    DischargeExpression(ctx, i0, i1);
    i0 = 2u;
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
      case 0: goto block_23;
      case 1: goto block_22;
      case 2: goto block_23;
      case 3: goto block_22;
      case 4: goto block_22;
      case 5: goto block_16;
      case 6: goto block_16;
      case 7: goto block_16;
      case 8: goto block_16;
      case 9: goto block_21;
      case 10: goto block_20;
      case 11: goto block_18;
      default: goto block_16;
    }
    block_23:;
    i0 = c;
    i1 = 2u;
    StoreU32(ctx, i0, i1);
    goto block_16;
    block_22:;
    i0 = c;
    i1 = 3u;
    StoreU32(ctx, i0, i1);
    goto block_16;
    block_21:;
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    i1 = c;
    i1 = LoadU32(ctx, i1 + 8u);
    b = i1;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    l4 = i0;
    i0 = b;
    i1 = 1u;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    if (i0) {
      i0 = l4;
      i1 = 4u;
      i0 -= i1;
      l5 = i0;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i1 = 63u;
      i0 &= i1;
      i1 = kF5673Const2B460;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      if (i0) {goto block_24;}
    }
    i0 = l4;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = l4;
    l5 = i0;
    block_24:;
    i0 = l5;
    i1 = b;
    i2 = kF5673ConstFFFFC03F;
    i1 &= i2;
    i2 = b;
    i3 = 16320u;
    i2 &= i3;
    i2 = !(i2);
    i3 = 6u;
    i2 <<= (i3 & 31);
    i1 |= i2;
    b = i1;
    StoreU32(ctx, i0, i1);
    goto block_16;
    block_20:;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 36u);
    l6 = i0;
    i1 = 1u;
    i0 += i1;
    l5 = i0;
    i0 = l6;
    i1 = a;
    i1 = LoadU32(ctx, i1);
    l4 = i1;
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
      goto block_26;
    }
    i0 = l6;
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
    i0 = l7;
    i1 = 3u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = kF5673MsgFunctionOrExpressionToo;
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
      i0 = LoadU32(ctx, i0);
      l4 = i0;
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
    i0 = l4;
    i1 = l5;
    StoreU8(ctx, i0 + 75u, i1);
  }
  block_26:;
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
    l4 = i0;
    if (i0) {goto block_17;}
    i0 = a;
    i0 = LoadU8(ctx, i0 + 50u);
    l4 = i0;
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
    i1 = 23u;
    i0 <<= (i1 & 31);
    i1 = 19u;
    i0 |= i1;
    l4 = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 12u);
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
    i1 = l4;
    i2 = b;
    i0 = EmitInstruction(ctx, i0, i1, i2);
    l3 = i0;
    i0 = 5u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i1 = 11u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = b;
    StoreU32(ctx, i0 + 8u, i1);
  }
  block_16:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i0 = LoadU32(ctx, i0 + 20u);
    b = i0;
    i0 = c;
    i1 = c;
    i1 = LoadU32(ctx, i1 + 16u);
    l6 = i1;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = c;
    i1 = b;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = l6;
    i1 = kF5673ConstFFFFFFFF;
    i0 = i0 != i1;
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      l9 = i0;
      loop_43: 
        i0 = l9;
        i1 = l6;
        i2 = 2u;
        i1 <<= (i2 & 31);
        i0 += i1;
        b = i0;
        i0 = l6;
        i1 = 1u;
        i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
        if (i0) {
          i0 = b;
          i1 = 4u;
          i0 -= i1;
          l4 = i0;
          i0 = LoadU32(ctx, i0);
          l5 = i0;
          i1 = 63u;
          i0 &= i1;
          i1 = kF5673Const2B460;
          i0 += i1;
          i0 = LoadS8(ctx, i0);
          i1 = 0u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          if (i0) {goto block_44;}
        }
        i0 = b;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i0 = b;
        l4 = i0;
        block_44:;
        i0 = l5;
        i1 = 63u;
        i0 &= i1;
        i1 = 27u;
        i0 = i0 == i1;
        if (i0) {
          i0 = l4;
          i1 = l5;
          i2 = 17u;
          i1 >>= (i2 & 31);
          i2 = 32704u;
          i1 &= i2;
          i2 = l5;
          i3 = kF5673Const7FC000;
          i2 &= i3;
          i1 |= i2;
          i2 = 26u;
          i1 |= i2;
          StoreU32(ctx, i0, i1);
        }
        i0 = kF5673ConstFFFFFFFF;
        i1 = l6;
        i2 = b;
        i2 = LoadU32(ctx, i2);
        i3 = 14u;
        i2 >>= (i3 & 31);
        i3 = kF5673Const1FFFF;
        i2 -= i3;
        b = i2;
        i1 += i2;
        i2 = 1u;
        i1 += i2;
        i2 = b;
        i3 = kF5673ConstFFFFFFFF;
        i2 = i2 == i3;
        i0 = i2 ? i0 : i1;
        l6 = i0;
        i1 = kF5673ConstFFFFFFFF;
        i0 = i0 != i1;
        if (i0) {goto loop_43;}
      i0 = c;
      i0 = LoadU32(ctx, i0 + 16u);
      b = i0;
    }
    i0 = b;
    i1 = kF5673ConstFFFFFFFF;
    i0 = i0 == i1;
    if (i0) {goto running_exit;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    c = i0;
    loop_47: 
      i0 = c;
      i1 = b;
      i2 = 2u;
      i1 <<= (i2 & 31);
      i0 += i1;
      a = i0;
      i0 = b;
      i1 = 1u;
      i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
      if (i0) {
        i0 = a;
        i1 = 4u;
        i0 -= i1;
        l4 = i0;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i1 = 63u;
        i0 &= i1;
        i1 = kF5673Const2B460;
        i0 += i1;
        i0 = LoadS8(ctx, i0);
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        if (i0) {goto block_48;}
      }
      i0 = a;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = a;
      l4 = i0;
      block_48:;
      i0 = l5;
      i1 = 63u;
      i0 &= i1;
      i1 = 27u;
      i0 = i0 == i1;
      if (i0) {
        i0 = l4;
        i1 = l5;
        i2 = 17u;
        i1 >>= (i2 & 31);
        i2 = 32704u;
        i1 &= i2;
        i2 = l5;
        i3 = kF5673Const7FC000;
        i2 &= i3;
        i1 |= i2;
        i2 = 26u;
        i1 |= i2;
        StoreU32(ctx, i0, i1);
      }
      i0 = kF5673ConstFFFFFFFF;
      i1 = b;
      i2 = a;
      i2 = LoadU32(ctx, i2);
      i3 = 14u;
      i2 >>= (i3 & 31);
      i3 = kF5673Const1FFFF;
      i2 -= i3;
      a = i2;
      i1 += i2;
      i2 = 1u;
      i1 += i2;
      i2 = a;
      i3 = kF5673ConstFFFFFFFF;
      i2 = i2 == i3;
      i0 = i2 ? i0 : i1;
      b = i0;
      i1 = kF5673ConstFFFFFFFF;
      i0 = i0 != i1;
      if (i0) {goto loop_47;}
    goto running_exit;
  }
  block_5:;
  i0 = l7;
  i1 = 6u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = c;
    i0 = EmitConditionJump(ctx, i0, i1);
    l3 = i0;
    i0 = 6u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
  } else {
    i0 = b;
  }
  i1 = l8;
  i2 = 8u;
  i1 += i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  b = i0;
  i0 = l7;
  i1 = 7u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 20u;
    i2 = c;
    i3 = b;
    CompileNumericPower(ctx, i0, i1, i2, i3);
    i0 = 7u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l8;
    i1 = 32u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
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
  i2 = kF5673ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = c;
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
  i0 = l3;
  i1 = l8;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l3;
  i1 = l9;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 32u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
