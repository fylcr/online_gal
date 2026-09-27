// emit_variable_load.cpp -- emits the load for one variable kind into the register the caller reserved..
//
// EmitVariableLoad -- emits the load for one variable kind into the register the caller reserved.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 493216-493216 (f1132); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t EmitInstruction(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t ExpressionToRegister(EngineContext* ctx, uint32_t func_state, uint32_t expr);
uint32_t EmitConditionJump(EngineContext* ctx, uint32_t func_state, uint32_t condition);
void CompileExpression(EngineContext* ctx, uint32_t func_state, uint32_t code, uint32_t line);

namespace {

constexpr uint32_t kF1132SaveFrameBytes = 24u;
constexpr uint32_t kF1132ResumeWordBytes = 4u;

}  // namespace

void EmitVariableLoad(EngineContext* ctx, uint32_t state, uint32_t variable, uint32_t register_hint) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1132SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    variable = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    register_hint = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
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
    i2 = kF1132ResumeWordBytes;
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
    i0 = variable;
    i0 = LoadU32(ctx, i0);
    i1 = 6u;
    i0 -= i1;
    l4 = i0;
    i0 = state;
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    switch (i0) {
      case 0: goto block_10;
      case 1: goto block_8;
      case 2: goto block_7;
      case 3: goto block_6;
      default: goto running_exit;
    }
    block_10:;
    i0 = register_hint;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    l6 = i0;
    if (i0) {goto block_11;}
    i0 = register_hint;
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
    i1 = 256u;
    i0 &= i1;
    l6 = i0;
    if (i0) {goto block_11;}
    i0 = l4;
    i1 = state;
    i1 = LoadU8(ctx, i1 + 50u);
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l6 = i0;
    if (i0) {goto block_11;}
    i0 = state;
    i1 = state;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    l6 = i1;
    StoreU32(ctx, i0 + 36u, i1);
    block_11:;
    i0 = variable;
    i0 = LoadU32(ctx, i0 + 8u);
    variable = i0;
  }
  i0 = l7;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = register_hint;
    i2 = variable;
    CompileExpression(ctx, i0, i1, i2);
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
  block_8:;
  i0 = l4;
  i1 = 8u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l4 = i0;
  i0 = l7;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = register_hint;
    i0 = EmitConditionJump(ctx, i0, i1);
    l5 = i0;
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = variable;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = 23u;
    i0 <<= (i1 & 31);
    i1 = l3;
    i2 = 6u;
    i1 <<= (i2 & 31);
    i0 |= i1;
    goto block_5;
  }
  block_7:;
  i0 = l4;
  i1 = 7u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l4 = i0;
  i0 = l7;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = register_hint;
    i0 = EmitConditionJump(ctx, i0, i1);
    l5 = i0;
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = variable;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = 14u;
    i0 <<= (i1 & 31);
    i1 = l3;
    i2 = 6u;
    i1 <<= (i2 & 31);
    i0 |= i1;
    goto block_5;
  }
  block_6:;
  i0 = l4;
  i1 = 9u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l4 = i0;
  i0 = l7;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = register_hint;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l5 = i0;
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  if (i0) {
    i0 = variable;
  } else {
    i0 = l3;
    i1 = 14u;
    i0 <<= (i1 & 31);
    i1 = variable;
    i1 = LoadU32(ctx, i1 + 8u);
    i2 = 6u;
    i1 <<= (i2 & 31);
    i0 |= i1;
    i1 = variable;
    i1 = LoadU32(ctx, i1 + 12u);
    i2 = 23u;
    i1 <<= (i2 & 31);
    i0 |= i1;
  }
  block_5:;
  variable = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = variable;
    i1 = l4;
    i0 |= i1;
    variable = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 12u);
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
  }
  i0 = l7;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = variable;
    i2 = l4;
    i0 = EmitInstruction(ctx, i0, i1, i2);
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = register_hint;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    if (i0) {goto block_23;}
    i0 = register_hint;
    i0 = LoadU32(ctx, i0 + 8u);
    variable = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_23;}
    i0 = state;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = variable;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_23;}
    i0 = state;
    i1 = state;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 36u, i1);
    block_23:;
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
  i2 = kF1132ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l5 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = variable;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l5;
  i1 = register_hint;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l5;
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l5;
  i1 = l6;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l5;
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
