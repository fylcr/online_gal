// release_expression_slots.cpp -- returns expression registers to the free list once the instruction has consumed them..
//
// ReleaseExpressionSlots -- returns expression registers to the free list once the instruction has consumed them.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 482395-482395 (f1117); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void CheckExprComplexity(EngineContext* ctx, uint32_t a, uint32_t b);
void EmitBranchWord(EngineContext* ctx, uint32_t state, uint32_t opcode, uint32_t operands);
void ReserveRegisters(EngineContext* ctx, uint32_t state, uint32_t count);
void AllocateExpressionRegister(EngineContext* ctx, uint32_t state, uint32_t instruction, uint32_t expression);

namespace {

constexpr uint32_t kF1117SaveFrameBytes = 16u;
constexpr uint32_t kF1117ResumeWordBytes = 4u;

}  // namespace

void ReleaseExpressionSlots(EngineContext* ctx, uint32_t state, uint32_t instruction, uint32_t count, uint32_t unused) {
  EnterFrame();
  uint32_t l4 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1117SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    unused = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = unused;
    i0 = LoadU32(ctx, i0 + 4u);
    instruction = i0;
    i0 = unused;
    i0 = LoadU32(ctx, i0 + 8u);
    count = i0;
    i0 = unused;
    i0 = LoadU32(ctx, i0 + 12u);
    unused = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1117ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = instruction;
    i1 = count;
    i0 -= i1;
    instruction = i0;
    i0 = unused;
    i0 = LoadU32(ctx, i0);
    count = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = count;
    switch (i0) {
      case 0: goto block_5;
      case 1: goto block_6;
      case 2: goto block_6;
      case 3: goto block_6;
      case 4: goto block_6;
      case 5: goto block_6;
      case 6: goto block_6;
      case 7: goto block_6;
      case 8: goto block_6;
      case 9: goto block_6;
      case 10: goto block_6;
      case 11: goto block_6;
      case 12: goto block_6;
      case 13: goto block_8;
      case 14: goto block_8;
      default: goto block_6;
    }
    block_8:;
    i0 = 0u;
    i1 = instruction;
    i2 = 1u;
    i1 += i2;
    count = i1;
    i2 = instruction;
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    instruction = i0;
  }
  i0 = l4;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = unused;
    i2 = instruction;
    AllocateExpressionRegister(ctx, i0, i1, i2);
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = instruction;
    i1 = 2u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    count = i0;
    if (i0) {goto running_exit;}
    i0 = instruction;
    i1 = 1u;
    i0 -= i1;
    instruction = i0;
  }
  i0 = l4;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = instruction;
    ReserveRegisters(ctx, i0, i1);
    i0 = 1u;
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
  block_6:;
  i0 = l4;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = unused;
    CheckExprComplexity(ctx, i0, i1);
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = instruction;
    i1 = 1u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {goto running_exit;}
    i0 = state;
    i0 = LoadU32(ctx, i0 + 36u);
    count = i0;
  }
  i0 = l4;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = instruction;
    ReserveRegisters(ctx, i0, i1);
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l4;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = count;
    i2 = instruction;
    EmitBranchWord(ctx, i0, i1, i2);
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  running_exit:;
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
  i2 = kF1117ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = instruction;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l4;
  i1 = count;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l4;
  i1 = unused;
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
