// raise_type_error.cpp -- raises a type error: the expected and the actual type are formatted into the message..
//
// RaiseTypeError -- raises a type error: the expected and the actual type are formatted into the message.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 358737-358737 (f907); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void RaiseScriptError(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t ResolveVariableKind(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3);

namespace {

constexpr uint32_t kF907MsgAttemptToSS = 0x2A718u;  // attempt to %s %s '%s' (a %s value)
constexpr uint32_t kF907MsgAttemptToSA = 0x2A73Bu;  // attempt to %s a %s value
constexpr uint32_t kF907Const2BD80 = 0x2BD80u;  // 
constexpr uint32_t kF907SaveFrameBytes = 20u;
constexpr uint32_t kF907ResumeWordBytes = 4u;

}  // namespace

void RaiseTypeError(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
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
    i2 = kF907SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 4u);
    b = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF907ResumeWordBytes;
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
    i1 = 48u;
    i0 -= i1;
    l3 = i0;
    GuestStackSp(ctx) = i0;
    i0 = l3;
    i1 = 0u;
    StoreU32(ctx, i0 + 44u, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 20u);
    l7 = i0;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i1 = l7;
    i1 = LoadU32(ctx, i1 + 8u);
    l8 = i1;
    i0 = i0 >= i1;
    l9 = i0;
    i0 = b;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = 2u;
    i0 <<= (i1 & 31);
    i1 = kF907Const2BD80;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l9;
    if (i0) {goto block_5;}
    loop_7: 
      i0 = b;
      i1 = l5;
      i0 = i0 != i1;
      if (i0) {
        i0 = l8;
        i1 = l5;
        i2 = 16u;
        i1 += i2;
        l5 = i1;
        i0 = i0 > i1;
        if (i0) {goto loop_7;}
        goto block_5;
      }
    i0 = a;
    i1 = l7;
    i2 = b;
    i3 = a;
    i3 = LoadU32(ctx, i3 + 12u);
    i2 -= i3;
    i3 = 4u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = l3;
    i4 = 44u;
    i3 += i4;
    i0 = ResolveVariableKind(ctx, i0, i1, i2, i3);
    b = i0;
    i0 = !(i0);
    if (i0) {goto block_5;}
    i0 = l3;
    i1 = c;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = l3;
    i1 = b;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = l3;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 44u);
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l3;
    i1 = l4;
    StoreU32(ctx, i0 + 28u, i1);
    i0 = l3;
    i1 = 16u;
    i0 += i1;
    b = i0;
  }
  i0 = l6;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF907MsgAttemptToSS;
    i2 = b;
    RaiseScriptError(ctx, i0, i1, i2);
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto running_exit;}
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = l4;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l3;
    i1 = c;
    StoreU32(ctx, i0, i1);
  }
  i0 = l6;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF907MsgAttemptToSA;
    i2 = l3;
    RaiseScriptError(ctx, i0, i1, i2);
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 48u;
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
  i2 = kF907ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l5 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l5;
  i1 = c;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l5;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l5;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 20u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
