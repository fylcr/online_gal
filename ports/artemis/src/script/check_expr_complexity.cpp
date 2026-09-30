// check_expr_complexity.cpp -- checks the expression complexity: the register and depth budgets are verified before the expression is emitted..
//
// CheckExprComplexity -- checks the expression complexity: the register and depth budgets are verified before the expression is emitted.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 183127-183127 (f427); R1 transcribe-keep.
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
void CompileExpression(EngineContext* ctx, uint32_t func_state, uint32_t code, uint32_t line);

namespace {

constexpr uint32_t kF427MsgFunctionOrExpressionToo = 0x2A371u;  // function or expression too complex
constexpr uint32_t kF427SaveFrameBytes = 24u;
constexpr uint32_t kF427ResumeWordBytes = 4u;

}  // namespace

void CheckExprComplexity(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF427SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    b = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
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
    i2 = kF427ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
  }
  i0 = l4;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = b;
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
    i0 = b;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    if (i0) {goto block_6;}
    i0 = b;
    i0 = LoadU32(ctx, i0 + 8u);
    l5 = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_6;}
    i0 = l5;
    i1 = a;
    i1 = LoadU8(ctx, i1 + 50u);
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {goto block_6;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 36u, i1);
    block_6:;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 36u);
    l6 = i0;
    i1 = 1u;
    i0 += i1;
    l5 = i0;
    i0 = l6;
    i1 = a;
    i1 = LoadU32(ctx, i1);
    l3 = i1;
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
      goto block_7;
    }
    i0 = l6;
    i1 = 249u;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    l6 = i0;
    i0 = l5;
    l2 = i0;
  }
  i0 = l6;
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
      l2 = i0;
    }
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = kF427MsgFunctionOrExpressionToo;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l2;
    } else {
      i0 = a;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = a;
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
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = l2;
    StoreU32(ctx, i0 + 36u, i1);
    i0 = l2;
    i1 = 1u;
    i0 -= i1;
    l2 = i0;
  }
  i0 = l4;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = b;
    i2 = l2;
    CompileExpression(ctx, i0, i1, i2);
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
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
  i2 = kF427ResumeWordBytes;
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
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l4;
  i1 = l5;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l4;
  i1 = l6;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l4;
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
