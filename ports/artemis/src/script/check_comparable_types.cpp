// check_comparable_types.cpp -- rejects operand types that cannot be compared, before the comparison is emitted..
//
// CheckComparableTypes -- rejects operand types that cannot be compared, before the comparison is emitted.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 819983-819983 (f1741); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void RaiseScriptError(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

constexpr uint32_t kF1741MsgAttemptToCompareTwo = 0x2A776u;  // attempt to compare two %s values
constexpr uint32_t kF1741MsgAttemptToCompareS = 0x2A797u;  // attempt to compare %s with %s
constexpr uint32_t kF1741Const2BD80 = 0x2BD80u;  // 
constexpr uint32_t kF1741SaveFrameBytes = 16u;
constexpr uint32_t kF1741ResumeWordBytes = 4u;

}  // namespace

uint32_t CheckComparableTypes(EngineContext* ctx, uint32_t state, uint32_t left, uint32_t right) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1741SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    left = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    right = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1741ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  if (i0) {
    i0 = l5;
  } else {
    i0 = GuestStackSp(ctx);
    i1 = 32u;
    i0 -= i1;
    l3 = i0;
    GuestStackSp(ctx) = i0;
    i0 = left;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = 2u;
    i0 <<= (i1 & 31);
    i1 = kF1741Const2BD80;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    left = i0;
    i0 = LoadU8(ctx, i0 + 2u);
    i1 = right;
    i1 = LoadU32(ctx, i1 + 8u);
    i2 = 2u;
    i1 <<= (i2 & 31);
    i2 = kF1741Const2BD80;
    i1 += i2;
    i1 = LoadU32(ctx, i1);
    right = i1;
    i1 = LoadU8(ctx, i1 + 2u);
    i0 = i0 == i1;
  }
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = left;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = kF1741MsgAttemptToCompareTwo;
      i2 = l3;
      RaiseScriptError(ctx, i0, i1, i2);
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_3;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = right;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = l3;
    i1 = left;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = l3;
    i1 = 16u;
    i0 += i1;
    left = i0;
  }
  i0 = l4;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = kF1741MsgAttemptToCompareS;
    i2 = left;
    RaiseScriptError(ctx, i0, i1, i2);
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_3:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 32u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    i0 = 0u;
    goto func_exit;
  }
  TrapUnreachable();
  spill_frame:;
  l4 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l4;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1741ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = left;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l4;
  i1 = right;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l4;
  i1 = l3;
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
