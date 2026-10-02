// expression_to_register.cpp -- discharges one expression into its next free register, emitting whatever moves the expression shape demands..
//
// ExpressionToRegister -- discharges one expression into its next free register, emitting whatever moves the expression shape demands.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 161104-161104 (f364); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void DischargeExpression(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t EmitConditionJump(EngineContext* ctx, uint32_t func_state, uint32_t condition);
uint32_t AddFunctionConstant(EngineContext* ctx, uint32_t func_state, uint32_t key, uint32_t value);

namespace {

constexpr uint32_t kF364SaveFrameBytes = 20u;
constexpr uint32_t kF364ResumeWordBytes = 4u;

}  // namespace

uint32_t ExpressionToRegister(EngineContext* ctx, uint32_t func_state, uint32_t expr) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; double l7 = 0;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  double d0 = 0; double d1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF364SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0);
    func_state = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = l2;
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
    i2 = kF364ResumeWordBytes;
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
    i1 = 32u;
    i0 -= i1;
    l4 = i0;
    GuestStackSp(ctx) = i0;
    i0 = expr;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
    i1 = expr;
    i1 = LoadU32(ctx, i1 + 16u);
    i0 = i0 != i1;
    l3 = i0;
  }
  i0 = l3;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l6;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = func_state;
      i1 = expr;
      i0 = EmitConditionJump(ctx, i0, i1);
      l2 = i0;
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l2;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_3;}
  }
  i0 = l6;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = func_state;
    i1 = expr;
    DischargeExpression(ctx, i0, i1);
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_3:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = expr;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i1 = 1u;
    i0 -= i1;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    switch (i0) {
      case 0: goto block_14;
      case 1: goto block_14;
      case 2: goto block_14;
      case 3: goto block_12;
      case 4: goto block_14;
      default: goto block_11;
    }
    block_14:;
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 40u);
    i1 = 255u;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_11;}
    i0 = l3;
    i1 = 1u;
    i0 -= i1;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    switch (i0) {
      case 0: goto block_19;
      case 1: goto block_16;
      case 2: goto block_16;
      case 3: goto block_16;
      case 4: goto block_17;
      default: goto block_16;
    }
    block_19:;
    i0 = l4;
    i1 = 0u;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 4u);
    l3 = i0;
    i0 = l4;
    i1 = 5u;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l4;
    i1 = l3;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = l4;
    i1 = 16u;
    i0 += i1;
    l3 = i0;
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
    i0 = AddFunctionConstant(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    func_state = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_15;}
  block_17:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = expr;
    d0 = LoadF64(ctx, i0 + 8u);
    l7 = d0;
    i0 = l4;
    i1 = 3u;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l4;
    d1 = l7;
    StoreF64(ctx, i0 + 16u, d1);
    i0 = l4;
    i1 = 16u;
    i0 += i1;
    l5 = i0;
    i0 = l4;
    i1 = 16u;
    i0 += i1;
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
    i1 = l3;
    i2 = l5;
    i0 = AddFunctionConstant(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    func_state = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_15;}
  block_16:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 1u;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l4;
    i1 = l3;
    i2 = 2u;
    i1 = i1 == i2;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = l4;
    i1 = 16u;
    i0 += i1;
    l5 = i0;
    i0 = l4;
    i1 = 16u;
    i0 += i1;
    l3 = i0;
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
    i2 = l5;
    i0 = AddFunctionConstant(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    func_state = i0;
  }
  block_15:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = expr;
    i1 = 4u;
    StoreU32(ctx, i0, i1);
    i0 = expr;
    i1 = func_state;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = func_state;
    i1 = 256u;
    i0 |= i1;
    goto block_10;
  }
  block_12:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = expr;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i1 = 255u;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_11;}
    i0 = l3;
    i1 = 256u;
    i0 |= i1;
    goto block_10;
  }
  block_11:;
  i0 = l6;
  i1 = 5u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = func_state;
    i1 = expr;
    i0 = EmitConditionJump(ctx, i0, i1);
    l2 = i0;
    i0 = 5u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
  } else {
    i0 = expr;
  }
  block_10:;
  expr = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 32u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    i0 = expr;
    goto func_exit;
  }
  TrapUnreachable();
  spill_frame:;
  l2 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF364ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l2 = i0;
  i1 = func_state;
  StoreU32(ctx, i0, i1);
  i0 = l2;
  i1 = expr;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l2;
  i1 = l3;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l2;
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l2;
  i1 = l5;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 20u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
