// raise_script_error.cpp -- raises a script error: the formatted message is handed to the error route together with the traceback frame..
//
// RaiseScriptError -- raises a script error: the formatted message is handed to the error route together with the traceback frame.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 138249-138249 (f313); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void InvokeCallValueWithStack(EngineContext* ctx, uint32_t a);
uint32_t FormatPushString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void FormatSourceId(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t FormatNumber(EngineContext* ctx, uint32_t state, uint32_t value, uint32_t options);

namespace {

constexpr uint32_t kF313MsgSDS = 0x2A80Du;  // %s:%d: %s
constexpr uint32_t kF313ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF313SaveFrameBytes = 24u;
constexpr uint32_t kF313ResumeWordBytes = 4u;

}  // namespace

void RaiseScriptError(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
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
    i2 = kF313SaveFrameBytes;
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
    c = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
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
    i2 = kF313ResumeWordBytes;
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
    i0 = GuestStackSp(ctx);
    l4 = i0;
    i1 = 80u;
    i0 -= i1;
    l5 = i0;
    GuestStackSp(ctx) = i0;
    i0 = l5;
    i1 = c;
    StoreU32(ctx, i0 + 12u, i1);
  }
  i0 = l7;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = b;
    i2 = c;
    i0 = FormatNumber(ctx, i0, i1, i2);
    l6 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l6;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 20u);
    b = i0;
    i0 = LoadU32(ctx, i0 + 4u);
    c = i0;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = 6u;
    i0 = i0 != i1;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    if (i0) {goto block_6;}
    i0 = c;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU8(ctx, i0 + 6u);
    if (i0) {goto block_6;}
    i0 = b;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 24u);
    l4 = i1;
    StoreU32(ctx, i0 + 12u, i1);
    i0 = kF313ConstFFFFFFFF;
    i1 = l4;
    i2 = c;
    i2 = LoadU32(ctx, i2);
    i2 = LoadU32(ctx, i2 + 16u);
    c = i2;
    i2 = LoadU32(ctx, i2 + 12u);
    i1 -= i2;
    l4 = i1;
    i2 = 4u;
    i1 = (uint32_t)((int32_t)i1 < (int32_t)i2);
    if (i1) {goto block_8;}
    i0 = 0u;
    i1 = c;
    i1 = LoadU32(ctx, i1 + 20u);
    b = i1;
    i1 = !(i1);
    if (i1) {goto block_8;}
    i0 = l4;
    i1 = 2u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    i1 = 1u;
    i0 -= i1;
    i1 = 2u;
    i0 <<= (i1 & 31);
    i1 = b;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    block_8:;
    l4 = i0;
    i0 = l5;
    i1 = 16u;
    i0 += i1;
    i1 = c;
    i1 = LoadU32(ctx, i1 + 32u);
    i2 = 16u;
    i1 += i2;
    i2 = 60u;
    FormatSourceId(ctx, i0, i1, i2);
    i0 = l5;
    i1 = l3;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l5;
    i1 = l4;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l5;
    i1 = l5;
    i2 = 16u;
    i1 += i2;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF313MsgSDS;
    i2 = l5;
    i0 = FormatPushString(ctx, i0, i1, i2);
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_6:;
  i0 = l7;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    InvokeCallValueWithStack(ctx, i0);
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = 80u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
  }
  goto func_exit;
  spill_frame:;
  l6 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l6;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF313ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l6 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l6;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l6;
  i1 = c;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l6;
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l6;
  i1 = l5;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l6;
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
