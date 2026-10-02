// expression_to_operand.cpp -- discharges one expression into an operand the emitter can fold into an instruction, spilling constants and registers as the expression demands..
//
// ExpressionToOperand -- discharges one expression into an operand the emitter can fold into an instruction, spilling constants and registers as the expression demands.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 359973-359973 (f910); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t EmitInstruction(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void DischargeExpression(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t AddFunctionConstant(EngineContext* ctx, uint32_t func_state, uint32_t key, uint32_t value);

namespace {

constexpr uint32_t kF910Const7FFFFF = 0x7FFFFFu;  // 
constexpr uint32_t kF910Const800000 = 0x800000u;  // 
constexpr uint32_t kF910ConstFFFFC03F = 0xFFFFC03Fu;  // 
constexpr uint32_t kF910SaveFrameBytes = 24u;
constexpr uint32_t kF910ResumeWordBytes = 4u;

}  // namespace

void ExpressionToOperand(EngineContext* ctx, uint32_t func_state, uint32_t expr, uint32_t operand) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; double l8 = 0;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  double d0 = 0; double d1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF910SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    func_state = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    operand = i0;
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
    i0 = LoadU32(ctx, i0 + 4u);
    expr = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF910ResumeWordBytes;
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
    i0 = GuestStackSp(ctx);
    l3 = i0;
    i1 = 16u;
    i0 -= i1;
    l7 = i0;
    GuestStackSp(ctx) = i0;
  }
  i0 = l6;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = func_state;
    i1 = expr;
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
    i0 = expr;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i1 = 1u;
    i0 -= i1;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    switch (i0) {
      case 0: goto block_15;
      case 1: goto block_14;
      case 2: goto block_14;
      case 3: goto block_12;
      case 4: goto block_11;
      case 5: goto block_6;
      case 6: goto block_6;
      case 7: goto block_6;
      case 8: goto block_6;
      case 9: goto block_6;
      case 10: goto block_10;
      case 11: goto block_9;
      default: goto block_6;
    }
    block_15:;
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 24u);
    l3 = i0;
    i1 = func_state;
    i1 = LoadU32(ctx, i1 + 28u);
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    l4 = i0;
    if (i0) {goto block_8;}
    i0 = l3;
    i0 = !(i0);
    l4 = i0;
    if (i0) {
      i0 = func_state;
      i0 = LoadU8(ctx, i0 + 50u);
      i1 = operand;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l3 = i0;
      if (i0) {goto block_7;}
      goto block_8;
    }
    i0 = func_state;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    i1 = l3;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    i1 = 4u;
    i0 -= i1;
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i1 = 63u;
    i0 &= i1;
    i1 = 3u;
    i0 = i0 != i1;
    if (i0) {goto block_8;}
    i0 = l3;
    i1 = 6u;
    i0 >>= (i1 & 31);
    i1 = 255u;
    i0 &= i1;
    i1 = operand;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_8;}
    i0 = l3;
    i1 = 23u;
    i0 >>= (i1 & 31);
    l5 = i0;
    i1 = 1u;
    i0 += i1;
    i1 = operand;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {goto block_8;}
    i0 = operand;
    i1 = l5;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {goto block_7;}
    i0 = l4;
    i1 = l3;
    i2 = kF910Const7FFFFF;
    i1 &= i2;
    i2 = operand;
    i3 = 23u;
    i2 <<= (i3 & 31);
    i1 |= i2;
    StoreU32(ctx, i0, i1);
    goto block_7;
    block_14:;
    i0 = operand;
    i1 = 6u;
    i0 <<= (i1 & 31);
    i1 = kF910Const800000;
    i2 = 2u;
    i3 = l3;
    i4 = 2u;
    i3 = i3 == i4;
    i1 = i3 ? i1 : i2;
    i0 |= i1;
    i1 = 2u;
    i0 |= i1;
    l3 = i0;
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 12u);
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
  }
  i0 = l6;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = func_state;
    i1 = l3;
    i2 = l4;
    i0 = EmitInstruction(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    func_state = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_7;}
  block_12:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = operand;
    i1 = 6u;
    i0 <<= (i1 & 31);
    i1 = expr;
    i1 = LoadU32(ctx, i1 + 8u);
    i2 = 14u;
    i1 <<= (i2 & 31);
    i0 |= i1;
    i1 = 1u;
    i0 |= i1;
    l3 = i0;
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 12u);
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
  }
  i0 = l6;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = func_state;
    i1 = l3;
    i2 = l4;
    i0 = EmitInstruction(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    func_state = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_7;}
  block_11:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = expr;
    d0 = LoadF64(ctx, i0 + 8u);
    l8 = d0;
    i0 = l7;
    i1 = 3u;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l7;
    d1 = l8;
    StoreF64(ctx, i0, d1);
    i0 = operand;
    i1 = 6u;
    i0 <<= (i1 & 31);
    l3 = i0;
  }
  i0 = l6;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = func_state;
    i1 = l7;
    i2 = l7;
    i0 = AddFunctionConstant(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 14u;
    i0 <<= (i1 & 31);
    i1 = l3;
    i0 |= i1;
    i1 = 1u;
    i0 |= i1;
    l3 = i0;
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 12u);
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
  }
  i0 = l6;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = func_state;
    i1 = l3;
    i2 = l4;
    i0 = EmitInstruction(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    func_state = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_7;}
  block_10:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = func_state;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    i1 = expr;
    i1 = LoadU32(ctx, i1 + 8u);
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    func_state = i0;
    i0 = LoadU32(ctx, i0);
    i1 = kF910ConstFFFFC03F;
    i0 &= i1;
    i1 = operand;
    i2 = 6u;
    i1 <<= (i2 & 31);
    i2 = 16320u;
    i1 &= i2;
    i0 |= i1;
    l3 = i0;
    i0 = func_state;
    i1 = l3;
    StoreU32(ctx, i0, i1);
    goto block_7;
  }
  block_9:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = expr;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i1 = operand;
    i0 = i0 == i1;
    if (i0) {goto block_7;}
    i0 = l3;
    i1 = 23u;
    i0 <<= (i1 & 31);
    i1 = operand;
    i2 = 6u;
    i1 <<= (i2 & 31);
    i0 |= i1;
    l3 = i0;
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 12u);
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
  }
  i0 = l6;
  i1 = 5u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = func_state;
    i1 = l3;
    i2 = l4;
    i0 = EmitInstruction(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 5u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    func_state = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_7;}
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = operand;
    i1 = 23u;
    i0 <<= (i1 & 31);
    i1 = operand;
    i2 = 6u;
    i1 <<= (i2 & 31);
    i0 |= i1;
    i1 = 3u;
    i0 |= i1;
    l3 = i0;
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 12u);
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
  }
  i0 = l6;
  i1 = 6u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = func_state;
    i1 = l3;
    i2 = l4;
    i0 = EmitInstruction(ctx, i0, i1, i2);
    i0 = 6u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = expr;
    i1 = 12u;
    StoreU32(ctx, i0, i1);
    i0 = expr;
    i1 = operand;
    StoreU32(ctx, i0 + 8u, i1);
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    i1 = 16u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
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
  i2 = kF910ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l5 = i0;
  i1 = func_state;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = expr;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l5;
  i1 = operand;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l5;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l5;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l5;
  i1 = l7;
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
