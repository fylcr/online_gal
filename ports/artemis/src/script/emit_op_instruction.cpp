// emit_op_instruction.cpp -- assembles one instruction word from opcode and operands and hands it to the emitter..
//
// EmitOpInstruction -- assembles one instruction word from opcode and operands and hands it to the emitter.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 179653-179653 (f414); R1 transcribe-keep.
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

constexpr uint32_t kF414SaveFrameBytes = 12u;
constexpr uint32_t kF414ResumeWordBytes = 4u;

}  // namespace

uint32_t EmitOpInstruction(EngineContext* ctx, uint32_t func_state, uint32_t op, uint32_t arg_a, uint32_t arg_b, uint32_t line) {
  EnterFrame();
  uint32_t l5 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF414SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    arg_a = i0;
    i0 = LoadU32(ctx, i0);
    func_state = i0;
    i0 = arg_a;
    i0 = LoadU32(ctx, i0 + 4u);
    op = i0;
    i0 = arg_a;
    i0 = LoadU32(ctx, i0 + 8u);
    arg_a = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF414ResumeWordBytes;
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
    i0 = arg_a;
    i1 = 6u;
    i0 <<= (i1 & 31);
    i1 = op;
    i0 |= i1;
    i1 = arg_b;
    i2 = 23u;
    i1 <<= (i2 & 31);
    i0 |= i1;
    i1 = line;
    i2 = 14u;
    i1 <<= (i2 & 31);
    i0 |= i1;
    op = i0;
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 12u);
    i0 = LoadU32(ctx, i0 + 8u);
    arg_a = i0;
  }
  i0 = l5;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = func_state;
    i1 = op;
    i2 = arg_a;
    i0 = EmitInstruction(ctx, i0, i1, i2);
    arg_b = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = arg_b;
    func_state = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = func_state;
    goto func_exit;
  }
  TrapUnreachable();
  spill_frame:;
  arg_b = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = arg_b;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF414ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  arg_b = i0;
  i1 = func_state;
  StoreU32(ctx, i0, i1);
  i0 = arg_b;
  i1 = op;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = arg_b;
  i1 = arg_a;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 12u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
