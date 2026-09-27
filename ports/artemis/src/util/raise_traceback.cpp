// raise_traceback.cpp -- prefixes an error message with the %s:%d traceback source position..
//
// RaiseTraceback -- prefixes an error message with the %s:%d traceback source position.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 122216-122216 (f238); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t FlushOutputRoomIfFull(EngineContext* ctx, uint32_t state, uint32_t limit, uint32_t chunk);
void InternKeyIntoTableSlot(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void InsertFormattedKeyEntry(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t StepCallValueFrame(EngineContext* ctx, uint32_t a0);
uint32_t TrimLocalSlotsAboveLevel(EngineContext* ctx, uint32_t proto, uint32_t level, uint32_t result);
uint32_t FormatTracebackFrame(EngineContext* ctx, uint32_t state, uint32_t name, uint32_t out);
void FormatTracebackSlotEntry(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

constexpr uint32_t kF238Const29D74 = 0x29D74u;  // 
constexpr uint32_t kF238MsgSD = 0x29D77u;  // %s:%d: 
constexpr uint32_t kF238Const29D7F = 0x29D7Fu;  // 
constexpr uint32_t kF238SaveFrameBytes = 16u;
constexpr uint32_t kF238ResumeWordBytes = 4u;

}  // namespace

uint32_t RaiseTraceback(EngineContext* ctx, uint32_t value, uint32_t message, uint32_t tag) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF238SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    value = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    tag = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    message = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF238ResumeWordBytes;
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
    i0 = GuestStackSp(ctx);
    i1 = 128u;
    i0 -= i1;
    l4 = i0;
    GuestStackSp(ctx) = i0;
    i0 = l4;
    i1 = tag;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = value;
    i1 = 1u;
    i2 = l4;
    i3 = 24u;
    i2 += i3;
    i0 = TrimLocalSlotsAboveLevel(ctx, i0, i1, i2);
    i0 = !(i0);
    tag = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = tag;
    if (i0) {goto block_5;}
    i0 = l4;
    i1 = 24u;
    i0 += i1;
    tag = i0;
  }
  i0 = l5;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = value;
    i1 = kF238Const29D74;
    i2 = tag;
    i0 = FormatTracebackFrame(ctx, i0, i1, i2);
    l3 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    tag = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 44u);
    tag = i0;
    i1 = 1u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {goto block_5;}
    i0 = l4;
    i1 = tag;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l4;
    i1 = l4;
    i2 = 60u;
    i1 += i2;
    tag = i1;
    StoreU32(ctx, i0, i1);
  }
  i0 = l5;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = value;
    i1 = kF238MsgSD;
    i2 = l4;
    i0 = FlushOutputRoomIfFull(ctx, i0, i1, i2);
    l3 = i0;
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    tag = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto running_exit;}
  block_5:;
  i0 = l5;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = value;
    i1 = kF238Const29D7F;
    i2 = 0u;
    InternKeyIntoTableSlot(ctx, i0, i1, i2);
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 20u);
    tag = i0;
  }
  i0 = l5;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = value;
    i1 = message;
    i2 = tag;
    FormatTracebackSlotEntry(ctx, i0, i1, i2);
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l5;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = value;
    i1 = 2u;
    InsertFormattedKeyEntry(ctx, i0, i1);
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l5;
  i1 = 5u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = value;
    i0 = StepCallValueFrame(ctx, i0);
    l3 = i0;
    i0 = 5u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    value = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 128u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    i0 = value;
    goto func_exit;
  }
  TrapUnreachable();
  spill_frame:;
  l3 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l3;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF238ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = value;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = message;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = tag;
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
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
