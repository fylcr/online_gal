// compile_comparison.cpp -- the comparison compiler: constants fold to a constant instruction, everything else takes the metamethod of both operands at the call site..
//
// CompileComparison -- the comparison compiler: constants fold to a constant instruction, everything else takes the metamethod of both operands at the call site.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 787506-787932 (f1702); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void GrowValueStack(EngineContext* ctx, uint32_t a, uint32_t b);
void CallValue(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t FetchMetamethod(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t RawValueEqual(EngineContext* ctx, uint32_t a, uint32_t b);

namespace {

constexpr uint32_t kF1702SaveFrameBytes = 20u;
constexpr uint32_t kF1702ResumeWordBytes = 4u;

}  // namespace

uint32_t CompileComparison(EngineContext* ctx, uint32_t func_state, uint32_t lhs, uint32_t rhs) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  uint64_t j1 = 0;
  double d0 = 0; double d1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1702SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    func_state = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    rhs = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 4u);
    lhs = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1702ResumeWordBytes;
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
    i0 = 1u;
    l5 = i0;
    i0 = lhs;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    switch (i0) {
      case 0: goto block_6;
      case 1: goto running_exit;
      case 2: goto running_exit;
      case 3: goto block_11;
      case 4: goto block_5;
      case 5: goto block_9;
      case 6: goto block_5;
      case 7: goto block_10;
      default: goto block_5;
    }
    block_11:;
    i0 = lhs;
    d0 = LoadF64(ctx, i0);
    i1 = rhs;
    d1 = LoadF64(ctx, i1);
    i0 = d0 == d1;
    goto func_exit;
    block_10:;
    i0 = lhs;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i1 = rhs;
    i1 = LoadU32(ctx, i1);
    l3 = i1;
    i0 = i0 == i1;
    if (i0) {goto block_6;}
    i0 = 0u;
    l5 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    l6 = i0;
    i0 = !(i0);
    if (i0) {goto block_6;}
    i0 = l6;
    i0 = LoadU8(ctx, i0 + 6u);
    i1 = 16u;
    i0 &= i1;
    if (i0) {goto block_6;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i0 = l6;
    i1 = 4u;
    i2 = func_state;
    i2 = LoadU32(ctx, i2 + 16u);
    i2 = LoadU32(ctx, i2 + 204u);
    i0 = FetchMetamethod(ctx, i0, i1, i2);
    l4 = i0;
    i0 = !(i0);
    if (i0) {goto block_6;}
    i0 = l3;
    i1 = l6;
    i0 = i0 == i1;
    if (i0) {goto block_8;}
    i0 = l3;
    i0 = !(i0);
    if (i0) {goto block_6;}
    i0 = l3;
    i0 = LoadU8(ctx, i0 + 6u);
    i1 = 16u;
    i0 &= i1;
    if (i0) {goto block_6;}
    i0 = l3;
    i1 = 4u;
    i2 = func_state;
    i2 = LoadU32(ctx, i2 + 16u);
    i2 = LoadU32(ctx, i2 + 204u);
    i0 = FetchMetamethod(ctx, i0, i1, i2);
    l3 = i0;
    i0 = !(i0);
    if (i0) {goto block_6;}
    i0 = l4;
    i1 = l3;
    i0 = RawValueEqual(ctx, i0, i1);
    if (i0) {goto block_8;}
    goto block_6;
    block_9:;
    i0 = lhs;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i1 = rhs;
    i1 = LoadU32(ctx, i1);
    l3 = i1;
    i0 = i0 == i1;
    if (i0) {goto block_6;}
    i0 = 0u;
    l5 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    l6 = i0;
    i0 = !(i0);
    if (i0) {goto block_6;}
    i0 = l6;
    i0 = LoadU8(ctx, i0 + 6u);
    i1 = 16u;
    i0 &= i1;
    if (i0) {goto block_6;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i0 = l6;
    i1 = 4u;
    i2 = func_state;
    i2 = LoadU32(ctx, i2 + 16u);
    i2 = LoadU32(ctx, i2 + 204u);
    i0 = FetchMetamethod(ctx, i0, i1, i2);
    l4 = i0;
    i0 = !(i0);
    if (i0) {goto block_6;}
    i0 = l3;
    i1 = l6;
    i0 = i0 == i1;
    if (i0) {goto block_8;}
    i0 = l3;
    i0 = !(i0);
    if (i0) {goto block_6;}
    i0 = l3;
    i0 = LoadU8(ctx, i0 + 6u);
    i1 = 16u;
    i0 &= i1;
    if (i0) {goto block_6;}
    i0 = l3;
    i1 = 4u;
    i2 = func_state;
    i2 = LoadU32(ctx, i2 + 16u);
    i2 = LoadU32(ctx, i2 + 204u);
    i0 = FetchMetamethod(ctx, i0, i1, i2);
    l3 = i0;
    i0 = !(i0);
    if (i0) {goto block_6;}
    i0 = l4;
    i1 = l3;
    i0 = RawValueEqual(ctx, i0, i1);
    i0 = !(i0);
    if (i0) {goto block_6;}
    block_8:;
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 32u);
    l3 = i0;
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 8u);
    l5 = i0;
    i1 = l4;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0, j1);
    i0 = l5;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
    i1 = lhs;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0 + 16u, j1);
    i0 = l4;
    i1 = lhs;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 24u, i1);
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 8u);
    lhs = i0;
    i1 = rhs;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0 + 32u, j1);
    i0 = lhs;
    i1 = rhs;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 40u, i1);
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 28u);
    i1 = func_state;
    i1 = LoadU32(ctx, i1 + 8u);
    lhs = i1;
    i0 -= i1;
    i1 = 48u;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    rhs = i0;
  }
  i0 = rhs;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l7;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = func_state;
      i1 = 3u;
      GrowValueStack(ctx, i0, i1);
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = lhs;
    } else {
      i0 = func_state;
      i0 = LoadU32(ctx, i0 + 8u);
    }
    lhs = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = func_state;
    i1 = lhs;
    i2 = 48u;
    i1 += i2;
    rhs = i1;
    StoreU32(ctx, i0 + 8u, i1);
  }
  i0 = l7;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = func_state;
    i1 = lhs;
    i2 = 1u;
    CallValue(ctx, i0, i1, i2);
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = func_state;
    i1 = func_state;
    i1 = LoadU32(ctx, i1 + 8u);
    i2 = 16u;
    i1 -= i2;
    lhs = i1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 32u);
    i1 = l5;
    i2 = l3;
    i1 -= i2;
    i0 += i1;
    rhs = i0;
    i1 = lhs;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0, j1);
    i0 = rhs;
    i1 = lhs;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = 0u;
    l5 = i0;
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 8u);
    func_state = i0;
    i0 = LoadU32(ctx, i0 + 8u);
    lhs = i0;
    switch (i0) {
      case 0: goto block_6;
      case 1: goto block_19;
      default: goto block_18;
    }
    block_19:;
    i0 = func_state;
    i0 = LoadU32(ctx, i0);
    i1 = 0u;
    i0 = i0 != i1;
    goto func_exit;
    block_18:;
    i0 = 1u;
    l5 = i0;
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    goto func_exit;
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = lhs;
    i0 = LoadU32(ctx, i0);
    i1 = rhs;
    i1 = LoadU32(ctx, i1);
    i0 = i0 == i1;
    goto func_exit;
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = lhs;
    i0 = LoadU32(ctx, i0);
    i1 = rhs;
    i1 = LoadU32(ctx, i1);
    i0 = i0 == i1;
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
  i2 = kF1702ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  i1 = func_state;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = lhs;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l4;
  i1 = rhs;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l4;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l4;
  i1 = l5;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 20u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
