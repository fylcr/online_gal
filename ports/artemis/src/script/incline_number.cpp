// incline_number.cpp -- bumps the line counter on a newline pair, keeping the lookahead consistent..
//
// InclineNumber -- bumps the line counter on a newline pair, keeping the lookahead consistent.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 355751-355751 (f902); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t NextCharSlow(EngineContext* ctx, uint32_t a0);
void LexError(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

constexpr uint32_t kF902MsgChunkHasTooMany = 0x2AD0Au;  // chunk has too many lines
constexpr uint32_t kF902Const7FFFFFFC = 0x7FFFFFFCu;  // 
constexpr uint32_t kF902ResumeWordBytes = 4u;

}  // namespace

void InclineNumber(EngineContext* ctx, uint32_t lex_state) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = 16u;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    lex_state = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 4u);
    l1 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF902ResumeWordBytes;
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
    i0 = lex_state;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = lex_state;
    i0 = LoadU32(ctx, i0 + 56u);
    l1 = i0;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i1 = 1u;
    i0 -= i1;
    l2 = i0;
    i0 = l1;
    i1 = l2;
    StoreU32(ctx, i0, i1);
    i0 = lex_state;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    if (i0) {
      i0 = l1;
      i1 = l1;
      i1 = LoadU32(ctx, i1 + 4u);
      l1 = i1;
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l1;
      i0 = LoadU8(ctx, i0);
      goto running_exit;
    }
  }
  i0 = l5;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l1;
    i0 = NextCharSlow(ctx, i0);
    l3 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
  } else {
    i0 = l1;
  }
  running_exit:;
  l1 = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i1 = l1;
    StoreU32(ctx, i0, i1);
    i0 = l1;
    i1 = 10u;
    i0 -= i1;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    switch (i0) {
      case 0: goto block_11;
      case 1: goto block_9;
      case 2: goto block_9;
      case 3: goto block_11;
      default: goto block_9;
    }
    block_11:;
    i0 = l1;
    i1 = l4;
    i0 = i0 == i1;
    l1 = i0;
    if (i0) {goto block_9;}
    i0 = lex_state;
    i0 = LoadU32(ctx, i0 + 56u);
    l1 = i0;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i1 = 1u;
    i0 -= i1;
    l2 = i0;
    i0 = l1;
    i1 = l2;
    StoreU32(ctx, i0, i1);
    i0 = lex_state;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    if (i0) {
      i0 = l1;
      i1 = l1;
      i1 = LoadU32(ctx, i1 + 4u);
      l1 = i1;
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l1;
      i0 = LoadU8(ctx, i0);
      goto block_12;
    }
  }
  i0 = l5;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l1;
    i0 = NextCharSlow(ctx, i0);
    l3 = i0;
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
  } else {
    i0 = l1;
  }
  block_12:;
  l1 = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i1 = l1;
    StoreU32(ctx, i0, i1);
  }
  block_9:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = lex_state;
    i1 = lex_state;
    i1 = LoadU32(ctx, i1 + 4u);
    l1 = i1;
    i2 = 1u;
    i1 += i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l1;
    i1 = kF902Const7FFFFFFC;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    l1 = i0;
  }
  i0 = l1;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 16u);
      l1 = i0;
    }
    i0 = l5;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = lex_state;
      i1 = kF902MsgChunkHasTooMany;
      i2 = l1;
      LexError(ctx, i0, i1, i2);
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
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
  i2 = kF902ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = lex_state;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = l1;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = l2;
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
