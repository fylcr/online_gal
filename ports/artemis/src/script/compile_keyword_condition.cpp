// compile_keyword_condition.cpp -- compiles a keyword condition: the keyword test is emitted as a conditional jump..
//
// CompileKeywordCondition -- compiles a keyword condition: the keyword test is emitted as a conditional jump.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 2748392-2748392 (f5676); R1 transcribe-keep.
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

namespace {

constexpr uint32_t kF5676SaveFrameBytes = 8u;
constexpr uint32_t kF5676ResumeWordBytes = 4u;

}  // namespace

void CompileKeywordCondition(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF5676SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = b;
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
    i2 = kF5676ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  if (i0) {
    i0 = l2;
  } else {
    i0 = b;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = b;
    i1 = LoadU32(ctx, i1 + 20u);
    i0 = i0 != i1;
  }
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l3;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = b;
      i0 = EmitConditionJump(ctx, i0, i1);
      l2 = i0;
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l2;
      a = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      goto func_exit;
    }
  }
  i0 = l3;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = b;
    DischargeExpression(ctx, i0, i1);
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  goto func_exit;
  spill_frame:;
  l2 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF5676ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l2 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l2;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 8u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
