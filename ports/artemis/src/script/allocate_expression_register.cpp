// allocate_expression_register.cpp -- allocates the next free register for an expression and records it on the instruction..
//
// AllocateExpressionRegister -- allocates the next free register for an expression and records it on the instruction.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 360494-360494 (f911); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);

namespace {

constexpr uint32_t kF911MsgFunctionOrExpressionToo = 0x2A371u;  // function or expression too complex
constexpr uint32_t kF911Const7FC000 = 0x7FC000u;  // 
constexpr uint32_t kF911Const7FFFFF = 0x7FFFFFu;  // 
constexpr uint32_t kF911Const800000 = 0x800000u;  // 
constexpr uint32_t kF911ConstFF803FFF = 0xFF803FFFu;  // 
constexpr uint32_t kF911ConstFFFFC03F = 0xFFFFC03Fu;  // 
constexpr uint32_t kF911SaveFrameBytes = 16u;
constexpr uint32_t kF911ResumeWordBytes = 4u;

}  // namespace

void AllocateExpressionRegister(EngineContext* ctx, uint32_t state, uint32_t instruction, uint32_t expression) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF911SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 4u);
    instruction = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    expression = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF911ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = instruction;
    i0 = LoadU32(ctx, i0);
    i1 = 13u;
    i0 -= i1;
    switch (i0) {
      case 0: goto block_6;
      case 1: goto block_5;
      default: goto block_3;
    }
    block_6:;
    i0 = state;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    i1 = instruction;
    i1 = LoadU32(ctx, i1 + 8u);
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    state = i0;
    i0 = LoadU32(ctx, i0);
    i1 = kF911ConstFF803FFF;
    i0 &= i1;
    i1 = expression;
    i2 = 14u;
    i1 <<= (i2 & 31);
    i2 = 16384u;
    i1 += i2;
    i2 = kF911Const7FC000;
    i1 &= i2;
    i0 |= i1;
    instruction = i0;
    i0 = state;
    i1 = instruction;
    StoreU32(ctx, i0, i1);
    goto func_exit;
    block_5:;
    i0 = state;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i1 = instruction;
    i1 = LoadU32(ctx, i1 + 8u);
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    l6 = i0;
    i0 = LoadU32(ctx, i0);
    i1 = kF911Const7FFFFF;
    i0 &= i1;
    i1 = expression;
    i2 = 23u;
    i1 <<= (i2 & 31);
    i0 |= i1;
    i1 = kF911Const800000;
    i0 += i1;
    expression = i0;
    i0 = l6;
    i1 = expression;
    StoreU32(ctx, i0, i1);
    i0 = state;
    i0 = LoadU32(ctx, i0 + 36u);
    i1 = 6u;
    i0 <<= (i1 & 31);
    i1 = 16320u;
    i0 &= i1;
    i1 = l3;
    i2 = instruction;
    i2 = LoadU32(ctx, i2 + 8u);
    i3 = 2u;
    i2 <<= (i3 & 31);
    i1 += i2;
    instruction = i1;
    i1 = LoadU32(ctx, i1);
    i2 = kF911ConstFFFFC03F;
    i1 &= i2;
    i0 |= i1;
    expression = i0;
    i0 = instruction;
    i1 = expression;
    StoreU32(ctx, i0, i1);
    i0 = state;
    i0 = LoadU32(ctx, i0 + 36u);
    l3 = i0;
    i1 = 1u;
    i0 += i1;
    instruction = i0;
    i0 = l3;
    i1 = l4;
    i1 = LoadU8(ctx, i1 + 75u);
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    expression = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = expression;
    if (i0) {
      i0 = instruction;
      expression = i0;
      goto block_7;
    }
    i0 = l3;
    i1 = 249u;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    l3 = i0;
    i0 = instruction;
    expression = i0;
  }
  i0 = l3;
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
      expression = i0;
    }
    i0 = l5;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = expression;
      i1 = kF911MsgFunctionOrExpressionToo;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = expression;
    } else {
      i0 = state;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = state;
      i0 = LoadU32(ctx, i0 + 36u);
      i1 = 1u;
      i0 += i1;
    }
    expression = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = instruction;
    StoreU8(ctx, i0 + 75u, i1);
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i1 = expression;
    StoreU32(ctx, i0 + 36u, i1);
  }
  block_3:;
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
  i2 = kF911ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = instruction;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = expression;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l3;
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 16u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
