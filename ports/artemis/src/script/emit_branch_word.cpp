// emit_branch_word.cpp -- emits one branch word, choosing the short and the long encoding..
//
// EmitBranchWord -- emits one branch word, choosing the short and the long encoding.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 2748861-2748861 (f5680); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t EmitInstruction(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

constexpr uint32_t kF5680Const7FFFFF = 0x7FFFFFu;  // 
constexpr uint32_t kF5680Const800000 = 0x800000u;  // 
constexpr uint32_t kF5680SaveFrameBytes = 12u;
constexpr uint32_t kF5680ResumeWordBytes = 4u;

}  // namespace

void EmitBranchWord(EngineContext* ctx, uint32_t state, uint32_t opcode, uint32_t operands) {
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
    i2 = kF5680SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    operands = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = operands;
    i0 = LoadU32(ctx, i0 + 4u);
    opcode = i0;
    i0 = operands;
    i0 = LoadU32(ctx, i0 + 8u);
    operands = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF5680ResumeWordBytes;
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
    i0 = state;
    i0 = LoadU32(ctx, i0 + 24u);
    l3 = i0;
    i1 = state;
    i1 = LoadU32(ctx, i1 + 28u);
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {goto running_exit;}
    i0 = l3;
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i0 = LoadU8(ctx, i0 + 50u);
      i1 = opcode;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {goto block_5;}
      goto running_exit;
    }
    i0 = state;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    i1 = l3;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    i1 = 4u;
    i0 -= i1;
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i1 = 63u;
    i0 &= i1;
    i1 = 3u;
    i0 = i0 != i1;
    if (i0) {goto running_exit;}
    i0 = l3;
    i1 = 6u;
    i0 >>= (i1 & 31);
    i1 = 255u;
    i0 &= i1;
    i1 = opcode;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto running_exit;}
    i0 = l3;
    i1 = 23u;
    i0 >>= (i1 & 31);
    l6 = i0;
    i1 = 1u;
    i0 += i1;
    i1 = opcode;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {goto running_exit;}
    i0 = l6;
    i1 = opcode;
    i2 = operands;
    i1 += i2;
    i2 = 1u;
    i1 -= i2;
    state = i1;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    if (i0) {goto block_5;}
    i0 = l5;
    i1 = l3;
    i2 = kF5680Const7FFFFF;
    i1 &= i2;
    i2 = state;
    i3 = 23u;
    i2 <<= (i3 & 31);
    i1 |= i2;
    StoreU32(ctx, i0, i1);
    block_5:;
    goto func_exit;
    running_exit:;
    i0 = opcode;
    i1 = operands;
    i0 += i1;
    i1 = 23u;
    i0 <<= (i1 & 31);
    i1 = kF5680Const800000;
    i0 -= i1;
    i1 = opcode;
    i2 = 6u;
    i1 <<= (i2 & 31);
    i0 |= i1;
    i1 = 3u;
    i0 |= i1;
    opcode = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 12u);
    i0 = LoadU32(ctx, i0 + 8u);
    operands = i0;
  }
  i0 = l4;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = opcode;
    i2 = operands;
    i0 = EmitInstruction(ctx, i0, i1, i2);
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
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
  i2 = kF5680ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = opcode;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = operands;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 12u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
