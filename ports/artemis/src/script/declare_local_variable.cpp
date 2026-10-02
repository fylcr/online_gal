// declare_local_variable.cpp -- reserves a local slot, records its name and resets its compile-time flags..
//
// DeclareLocalVariable -- reserves a local slot, records its name and resets its compile-time flags.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 187356-187356 (f442); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t FormatPushString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void LexError(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void StoreListEntry(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t GrowArray(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f);

namespace {

constexpr uint32_t kF442MsgMainFunctionHasMore = 0x2B6C0u;  // main function has more than %d %s
constexpr uint32_t kF442MsgFunctionAtLineD = 0x2B6E2u;  // function at line %d has more than %d %s
constexpr uint32_t kF442MsgLocalVariables = 0x2B70Fu;  // local variables
constexpr uint32_t kF442MsgTooManyLocalVariables = 0x2B71Fu;  // too many local variables
constexpr uint32_t kF442SaveFrameBytes = 44u;
constexpr uint32_t kF442ResumeWordBytes = 4u;

}  // namespace

void DeclareLocalVariable(EngineContext* ctx, uint32_t state, uint32_t name, uint32_t slot) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u;
      l11 = 0, l12 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF442SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    slot = i0;
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
    l8 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 32u);
    l9 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 36u);
    l10 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 40u);
    l12 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    name = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF442ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l11 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    i1 = 32u;
    i0 -= i1;
    l8 = i0;
    GuestStackSp(ctx) = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    l9 = i0;
    l5 = i0;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = slot;
    i0 += i1;
    i1 = 200u;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    l4 = i0;
  }
  i0 = l4;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l9;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 60u);
      l7 = i0;
      i0 = !(i0);
      l5 = i0;
      i0 = l9;
      i0 = LoadU32(ctx, i0 + 16u);
      l4 = i0;
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
        i0 = l8;
        i1 = kF442MsgLocalVariables;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = l8;
        i1 = 200u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l11;
      i0 = !(i0);
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l4;
        i1 = kF442MsgMainFunctionHasMore;
        i2 = l8;
        i0 = FormatPushString(ctx, i0, i1, i2);
        l3 = i0;
        i0 = 0u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l3;
        l4 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_7;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l8;
      i1 = kF442MsgLocalVariables;
      StoreU32(ctx, i0 + 24u, i1);
      i0 = l8;
      i1 = 200u;
      StoreU32(ctx, i0 + 20u, i1);
      i0 = l8;
      i1 = l7;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l8;
      i1 = 16u;
      i0 += i1;
      l5 = i0;
    }
    i0 = l11;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l4;
      i1 = kF442MsgFunctionAtLineD;
      i2 = l5;
      i0 = FormatPushString(ctx, i0, i1, i2);
      l3 = i0;
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l3;
      l4 = i0;
    }
    block_7:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l9;
      i0 = LoadU32(ctx, i0 + 12u);
      l5 = i0;
    }
    i0 = l11;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l5;
      i1 = l4;
      i2 = 0u;
      LexError(ctx, i0, i1, i2);
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l5;
    } else {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 48u);
    }
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i0 = LoadU32(ctx, i0);
    l7 = i0;
    i0 = LoadU32(ctx, i0 + 56u);
    l4 = i0;
    i1 = l5;
    i1 = LoadS16(ctx, i1 + 48u);
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    if (i0) {
      i0 = l7;
      i0 = LoadU32(ctx, i0 + 24u);
      l6 = i0;
      goto block_17;
    }
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 24u);
    l10 = i0;
    i0 = l7;
    i1 = 56u;
    i0 += i1;
    l12 = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 52u);
    l6 = i0;
  }
  i0 = l11;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = l10;
    i2 = l12;
    i3 = 12u;
    i4 = 32767u;
    i5 = kF442MsgTooManyLocalVariables;
    i0 = GrowArray(ctx, i0, i1, i2, i3, i4, i5);
    l3 = i0;
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    i1 = l6;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 56u);
    l10 = i0;
    i1 = l4;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {goto block_17;}
    loop_22: 
      i0 = l4;
      i1 = 12u;
      i0 *= i1;
      i1 = l6;
      i0 += i1;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l10;
      i1 = l4;
      i2 = 1u;
      i1 += i2;
      l4 = i1;
      i0 = i0 != i1;
      if (i0) {goto loop_22;}
  }
  block_17:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i0 = LoadS16(ctx, i0 + 48u);
    l4 = i0;
    i1 = 12u;
    i0 *= i1;
    i1 = l6;
    i0 += i1;
    i1 = name;
    StoreU32(ctx, i0, i1);
    i0 = name;
    i0 = LoadU8(ctx, i0 + 5u);
    i1 = 3u;
    i0 &= i1;
    i0 = !(i0);
    if (i0) {goto block_24;}
    i0 = l7;
    i0 = LoadU8(ctx, i0 + 5u);
    i1 = 4u;
    i0 &= i1;
    i0 = !(i0);
    if (i0) {goto block_24;}
    i0 = state;
    i0 = LoadU32(ctx, i0 + 52u);
    i1 = l7;
    i2 = name;
    StoreListEntry(ctx, i0, i1, i2);
    i0 = l5;
    i0 = LoadU16(ctx, i0 + 48u);
    l4 = i0;
    block_24:;
    i0 = l5;
    i1 = l4;
    i2 = 1u;
    i1 += i2;
    StoreU16(ctx, i0 + 48u, i1);
    i0 = l9;
    i1 = slot;
    i2 = l9;
    i2 = LoadU8(ctx, i2 + 50u);
    i1 += i2;
    i2 = 1u;
    i1 <<= (i2 & 31);
    i0 += i1;
    i1 = l4;
    StoreU16(ctx, i0 + 172u, i1);
    i0 = l8;
    i1 = 32u;
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
  i2 = kF442ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = name;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = slot;
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
  i1 = l8;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l3;
  i1 = l9;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = l3;
  i1 = l10;
  StoreU32(ctx, i0 + 36u, i1);
  i0 = l3;
  i1 = l12;
  StoreU32(ctx, i0 + 40u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 44u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
