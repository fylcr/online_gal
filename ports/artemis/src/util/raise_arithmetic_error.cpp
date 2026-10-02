// raise_arithmetic_error.cpp -- raises the attempt to perform arithmetic on error for a bad operand..
//
// RaiseArithmeticError -- raises the attempt to perform arithmetic on error for a bad operand.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 252605-252605 (f644); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void RaiseScriptError(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t MaterializeNumberValue(EngineContext* ctx, uint32_t state, uint32_t slot);
uint32_t ResolveVariableKind(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3);

namespace {

constexpr uint32_t kF644MsgAttemptToSS = 0x2A718u;  // attempt to %s %s '%s' (a %s value)
constexpr uint32_t kF644MsgAttemptToSA = 0x2A73Bu;  // attempt to %s a %s value
constexpr uint32_t kF644MsgPerformArithmeticOn = 0x2A760u;  // perform arithmetic on
constexpr uint32_t kF644Const2BD80 = 0x2BD80u;  // 
constexpr uint32_t kF644ConstFFFFFFC0 = 0xFFFFFFC0u;  // 
constexpr uint32_t kF644SaveFrameBytes = 32u;
constexpr uint32_t kF644ResumeWordBytes = 4u;

}  // namespace

void RaiseArithmeticError(EngineContext* ctx, uint32_t state, uint32_t value, uint32_t meta) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF644SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    meta = i0;
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
    l7 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 28u);
    l9 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    value = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF644ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l8 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    i1 = kF644ConstFFFFFFC0;
    i0 += i1;
    l4 = i0;
    GuestStackSp(ctx) = i0;
    i0 = l4;
    i1 = 40u;
    i0 += i1;
    l5 = i0;
  }
  i0 = l8;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = value;
    i1 = l5;
    i0 = MaterializeNumberValue(ctx, i0, i1);
    l3 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 0u;
    StoreU32(ctx, i0 + 60u, i1);
    i0 = meta;
    i1 = value;
    i2 = l5;
    i0 = i2 ? i0 : i1;
    meta = i0;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = 2u;
    i0 <<= (i1 & 31);
    i1 = kF644Const2BD80;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = LoadU32(ctx, i0);
    value = i0;
    i1 = l6;
    i1 = LoadU32(ctx, i1 + 8u);
    l9 = i1;
    i0 = i0 >= i1;
    l7 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    if (i0) {goto block_7;}
    loop_9: 
      i0 = value;
      i1 = meta;
      i0 = i0 != i1;
      if (i0) {
        i0 = l9;
        i1 = value;
        i2 = 16u;
        i1 += i2;
        value = i1;
        i0 = i0 > i1;
        if (i0) {goto loop_9;}
        goto block_7;
      }
    i0 = state;
    i1 = l6;
    i2 = meta;
    i3 = state;
    i3 = LoadU32(ctx, i3 + 12u);
    i2 -= i3;
    i3 = 4u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = l4;
    i4 = 60u;
    i3 += i4;
    i0 = ResolveVariableKind(ctx, i0, i1, i2, i3);
    value = i0;
    i0 = !(i0);
    if (i0) {goto block_7;}
    i0 = l4;
    i1 = kF644MsgPerformArithmeticOn;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = l4;
    i1 = value;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = l4;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 60u);
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l4;
    i1 = l5;
    StoreU32(ctx, i0 + 28u, i1);
    i0 = l4;
    i1 = 16u;
    i0 += i1;
    value = i0;
  }
  i0 = l8;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = kF644MsgAttemptToSS;
    i2 = value;
    RaiseScriptError(ctx, i0, i1, i2);
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_6;}
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l5;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l4;
    i1 = kF644MsgPerformArithmeticOn;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = kF644MsgAttemptToSA;
    i2 = l4;
    RaiseScriptError(ctx, i0, i1, i2);
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = kF644ConstFFFFFFC0;
    i0 -= i1;
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
  i2 = kF644ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = value;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = meta;
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
  i1 = l7;
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
