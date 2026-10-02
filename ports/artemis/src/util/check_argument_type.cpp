// check_argument_type.cpp -- validates one argument type and answers false as soon as the tag disagrees..
//
// CheckArgumentType -- validates one argument type and answers false as soon as the tag disagrees.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 134532-134532 (f298); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t QueryValueType(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t FlushOutputRoomIfFull(EngineContext* ctx, uint32_t state, uint32_t limit, uint32_t chunk);
uint32_t FormatArgumentError(EngineContext* ctx, uint32_t state, uint32_t index, uint32_t actual);

namespace {

constexpr uint32_t kF298MsgNoValue = 0x29CF7u;  // no value
constexpr uint32_t kF298MsgSExpectedGotS = 0x29D60u;  // %s expected, got %s
constexpr uint32_t kF298Const2BD80 = 0x2BD80u;  // 
constexpr uint32_t kF298ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF298SaveFrameBytes = 16u;
constexpr uint32_t kF298ResumeWordBytes = 4u;

}  // namespace

void CheckArgumentType(EngineContext* ctx, uint32_t value, uint32_t expected, uint32_t index) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF298SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    value = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    index = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    expected = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF298ResumeWordBytes;
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
    i1 = 16u;
    i0 -= i1;
    l4 = i0;
    GuestStackSp(ctx) = i0;
    i0 = value;
    i1 = expected;
    i0 = QueryValueType(ctx, i0, i1);
    i1 = index;
    i0 = i0 != i1;
    l5 = i0;
  }
  i0 = l5;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = index;
      i1 = kF298ConstFFFFFFFF;
      i0 = i0 == i1;
      if (i0) {
        i0 = kF298MsgNoValue;
      } else {
        i0 = index;
        i1 = 2u;
        i0 <<= (i1 & 31);
        i1 = kF298Const2BD80;
        i0 += i1;
        i0 = LoadU32(ctx, i0);
      }
      index = i0;
      i0 = kF298MsgNoValue;
      l5 = i0;
      i0 = l4;
      i1 = value;
      i2 = expected;
      i1 = QueryValueType(ctx, i1, i2);
      l3 = i1;
      i2 = kF298ConstFFFFFFFF;
      i1 = i1 != i2;
      if (i1) {
        i1 = l3;
        i2 = 2u;
        i1 <<= (i2 & 31);
        i2 = kF298Const2BD80;
        i1 += i2;
        i1 = LoadU32(ctx, i1);
      } else {
        i1 = l5;
      }
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l4;
      i1 = index;
      StoreU32(ctx, i0, i1);
    }
    i0 = l6;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = value;
      i1 = kF298MsgSExpectedGotS;
      i2 = l4;
      i0 = FlushOutputRoomIfFull(ctx, i0, i1, i2);
      l3 = i0;
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l3;
      index = i0;
    }
    i0 = l6;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = value;
      i1 = expected;
      i2 = index;
      i0 = FormatArgumentError(ctx, i0, i1, i2);
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 16u;
    i0 += i1;
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
  i2 = kF298ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = value;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = expected;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = index;
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
