// check_statement_limits.cpp -- the parser semantic and depth check: the syntax-level counter behind the "chunk has too many syntax levels" trap, the vararg guard, and the statement/condition/expression emit gates it drives..
//
// CheckStatementLimits -- the parser semantic and depth check: the syntax-level counter behind the "chunk has too many syntax levels" trap, the vararg guard, and the statement/condition/expression emit gates it drives.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 149382-150325 (f346); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void ParseParamList(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d);
void CheckMatch(EngineContext* ctx, uint32_t lex_state, uint32_t expected);
void CompileMainBody(EngineContext* ctx, uint32_t a0, uint32_t a1);
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
void StepRecordResume(EngineContext* ctx, uint32_t a0);
void LexError(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t EmitOpInstruction(EngineContext* ctx, uint32_t func_state, uint32_t op, uint32_t arg_a, uint32_t arg_b, uint32_t line);
void EmitExpression(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d);
void EmitCondition(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
void EmitStatement(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t EncodeConstant(EngineContext* ctx, uint32_t func_state, uint32_t value);

namespace {

constexpr uint32_t kF346MsgChunkHasTooMany = 0x2B633u;  // chunk has too many syntax levels
constexpr uint32_t kF346Const2B660 = 0x2B660u;  // 
constexpr uint32_t kF346Const2B661 = 0x2B661u;  // 
constexpr uint32_t kF346MsgCannotUseOutsideA = 0x2B67Eu;  // cannot use '...' outside a vararg function
constexpr uint32_t kF346SaveFrameBytes = 32u;
constexpr uint32_t kF346ResumeWordBytes = 4u;

}  // namespace

uint32_t CheckStatementLimits(EngineContext* ctx, uint32_t state, uint32_t func_state, uint32_t message) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  uint64_t j1 = 0;
  double d1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF346SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    message = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 16u);
    l3 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 24u);
    l8 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 28u);
    l9 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 4u);
    func_state = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF346ResumeWordBytes;
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
    i1 = 32u;
    i0 -= i1;
    l9 = i0;
    GuestStackSp(ctx) = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 52u);
    l4 = i0;
    l6 = i0;
    i1 = l4;
    i1 = LoadU16(ctx, i1 + 52u);
    i2 = 1u;
    i1 += i2;
    l4 = i1;
    StoreU16(ctx, i0 + 52u, i1);
    i0 = l4;
    i1 = 65535u;
    i0 &= i1;
    i1 = 201u;
    i0 = i0 >= i1;
    l4 = i0;
    i0 = 1u;
    l3 = i0;
  }
  i0 = l4;
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
      i0 = state;
      i1 = kF346MsgChunkHasTooMany;
      i2 = 0u;
      LexError(ctx, i0, i1, i2);
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i1 = 263u;
    i0 -= i1;
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    switch (i0) {
      case 0: goto block_15;
      case 1: goto block_11;
      case 2: goto block_13;
      case 3: goto block_11;
      case 4: goto block_11;
      case 5: goto block_11;
      case 6: goto block_17;
      case 7: goto block_20;
      case 8: goto block_11;
      case 9: goto block_11;
      case 10: goto block_11;
      case 11: goto block_11;
      case 12: goto block_16;
      case 13: goto block_11;
      case 14: goto block_11;
      case 15: goto block_11;
      case 16: goto block_14;
      case 17: goto block_11;
      case 18: goto block_11;
      case 19: goto block_11;
      case 20: goto block_11;
      case 21: goto block_10;
      case 22: goto block_11;
      case 23: goto block_18;
      default: goto block_21;
    }
    block_21:;
    i0 = l4;
    i1 = 35u;
    i0 -= i1;
    l3 = i0;
    switch (i0) {
      case 0: goto block_22;
      case 1: goto block_11;
      case 2: goto block_11;
      case 3: goto block_11;
      case 4: goto block_11;
      case 5: goto block_11;
      case 6: goto block_11;
      case 7: goto block_11;
      case 8: goto block_11;
      case 9: goto block_11;
      case 10: goto block_23;
      default: goto block_12;
    }
    block_23:;
    i0 = 0u;
    l3 = i0;
    goto block_20;
    block_22:;
    i0 = 2u;
    l3 = i0;
    block_20:;
  }
  i0 = l7;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    StepRecordResume(ctx, i0);
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l7;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = func_state;
    i2 = 8u;
    i0 = CheckStatementLimits(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    l4 = i0;
  }
  i0 = l7;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l4;
    i1 = l3;
    i2 = func_state;
    EmitStatement(ctx, i0, i1, i2);
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_7;}
  block_18:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 24u);
    l4 = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    l3 = i0;
  }
  i0 = l7;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l3;
    i1 = l4;
    i0 = EncodeConstant(ctx, i0, i1);
    l5 = i0;
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = func_state;
    j1 = 18446744073709551615ull;
    StoreU64(ctx, i0 + 16u, j1);
    i0 = func_state;
    i1 = l3;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = func_state;
    i1 = 4u;
    StoreU32(ctx, i0, i1);
    goto block_9;
  }
  block_17:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = func_state;
    j1 = 18446744073709551615ull;
    StoreU64(ctx, i0 + 16u, j1);
    i0 = func_state;
    i1 = 0u;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = func_state;
    i1 = 1u;
    StoreU32(ctx, i0, i1);
    goto block_9;
  }
  block_16:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = func_state;
    j1 = 18446744073709551615ull;
    StoreU64(ctx, i0 + 16u, j1);
    i0 = func_state;
    i1 = 0u;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = func_state;
    i1 = 2u;
    StoreU32(ctx, i0, i1);
    goto block_9;
  }
  block_15:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = func_state;
    j1 = 18446744073709551615ull;
    StoreU64(ctx, i0 + 16u, j1);
    i0 = func_state;
    i1 = 0u;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = func_state;
    i1 = 3u;
    StoreU32(ctx, i0, i1);
    goto block_9;
  }
  block_14:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    l6 = i0;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU8(ctx, i0 + 74u);
    l4 = i0;
    i0 = !(i0);
    l8 = i0;
  }
  i0 = l8;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l7;
    i1 = 5u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = kF346MsgCannotUseOutsideA;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 5u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l4;
    } else {
      i0 = l6;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = LoadU8(ctx, i0 + 74u);
    }
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = l4;
    i2 = 251u;
    i1 &= i2;
    l4 = i1;
    StoreU8(ctx, i0 + 74u, i1);
  }
  i0 = l7;
  i1 = 6u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = 37u;
    i2 = 0u;
    i3 = 1u;
    i4 = 0u;
    i0 = EmitOpInstruction(ctx, i0, i1, i2, i3, i4);
    l5 = i0;
    i0 = 6u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = func_state;
    j1 = 18446744073709551615ull;
    StoreU64(ctx, i0 + 16u, j1);
    i0 = func_state;
    i1 = l3;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = func_state;
    i1 = 14u;
    StoreU32(ctx, i0, i1);
    goto block_9;
  }
  block_13:;
  i0 = l7;
  i1 = 7u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    StepRecordResume(ctx, i0);
    i0 = 7u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 4u);
    l4 = i0;
  }
  i0 = l7;
  i1 = 8u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = func_state;
    i2 = 0u;
    i3 = l4;
    ParseParamList(ctx, i0, i1, i2, i3);
    i0 = 8u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_7;}
  block_12:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 123u;
    i0 = i0 == i1;
    l4 = i0;
    if (i0) {goto block_8;}
  }
  block_11:;
  i0 = l7;
  i1 = 9u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = func_state;
    CheckMatch(ctx, i0, i1);
    i0 = 9u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_7;}
  block_10:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = func_state;
    j1 = 18446744073709551615ull;
    StoreU64(ctx, i0 + 16u, j1);
    i0 = func_state;
    i1 = 0u;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = func_state;
    i1 = 5u;
    StoreU32(ctx, i0, i1);
    i0 = func_state;
    i1 = state;
    d1 = LoadF64(ctx, i1 + 24u);
    StoreF64(ctx, i0 + 8u, d1);
  }
  block_9:;
  i0 = l7;
  i1 = 10u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    StepRecordResume(ctx, i0);
    i0 = 10u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_7;}
  block_8:;
  i0 = l7;
  i1 = 11u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = func_state;
    CompileMainBody(ctx, i0, i1);
    i0 = 11u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 15u;
    l4 = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    l6 = i0;
    i1 = 37u;
    i0 -= i1;
    l8 = i0;
    i0 = 0u;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l8;
    switch (i0) {
      case 0: goto block_64;
      case 1: goto block_51;
      case 2: goto block_51;
      case 3: goto block_51;
      case 4: goto block_51;
      case 5: goto block_66;
      case 6: goto block_53;
      case 7: goto block_51;
      case 8: goto block_67;
      case 9: goto block_51;
      case 10: goto block_65;
      case 11: goto block_51;
      case 12: goto block_51;
      case 13: goto block_51;
      case 14: goto block_51;
      case 15: goto block_51;
      case 16: goto block_51;
      case 17: goto block_51;
      case 18: goto block_51;
      case 19: goto block_51;
      case 20: goto block_51;
      case 21: goto block_51;
      case 22: goto block_51;
      case 23: goto block_59;
      case 24: goto block_51;
      case 25: goto block_57;
      case 26: goto block_51;
      case 27: goto block_51;
      case 28: goto block_51;
      case 29: goto block_51;
      case 30: goto block_51;
      case 31: goto block_51;
      case 32: goto block_51;
      case 33: goto block_51;
      case 34: goto block_51;
      case 35: goto block_51;
      case 36: goto block_51;
      case 37: goto block_51;
      case 38: goto block_51;
      case 39: goto block_51;
      case 40: goto block_51;
      case 41: goto block_51;
      case 42: goto block_51;
      case 43: goto block_51;
      case 44: goto block_51;
      case 45: goto block_51;
      case 46: goto block_51;
      case 47: goto block_51;
      case 48: goto block_51;
      case 49: goto block_51;
      case 50: goto block_51;
      case 51: goto block_51;
      case 52: goto block_51;
      case 53: goto block_51;
      case 54: goto block_51;
      case 55: goto block_51;
      case 56: goto block_51;
      case 57: goto block_63;
      default: goto block_68;
    }
    block_68:;
    i0 = l6;
    i1 = 257u;
    i0 -= i1;
    switch (i0) {
      case 0: goto block_55;
      case 1: goto block_51;
      case 2: goto block_51;
      case 3: goto block_51;
      case 4: goto block_51;
      case 5: goto block_51;
      case 6: goto block_51;
      case 7: goto block_51;
      case 8: goto block_51;
      case 9: goto block_51;
      case 10: goto block_51;
      case 11: goto block_51;
      case 12: goto block_51;
      case 13: goto block_51;
      case 14: goto block_54;
      case 15: goto block_51;
      case 16: goto block_51;
      case 17: goto block_51;
      case 18: goto block_51;
      case 19: goto block_51;
      case 20: goto block_51;
      case 21: goto block_62;
      case 22: goto block_51;
      case 23: goto block_60;
      case 24: goto block_56;
      case 25: goto block_58;
      case 26: goto block_61;
      default: goto block_51;
    }
    block_67:;
    i0 = 1u;
    l3 = i0;
    goto block_53;
    block_66:;
    i0 = 2u;
    l3 = i0;
    goto block_53;
    block_65:;
    i0 = 3u;
    l3 = i0;
    goto block_53;
    block_64:;
    i0 = 4u;
    l3 = i0;
    goto block_53;
    block_63:;
    i0 = 5u;
    l3 = i0;
    goto block_53;
    block_62:;
    i0 = 6u;
    l3 = i0;
    goto block_53;
    block_61:;
    i0 = 7u;
    l3 = i0;
    goto block_53;
    block_60:;
    i0 = 8u;
    l3 = i0;
    goto block_53;
    block_59:;
    i0 = 9u;
    l3 = i0;
    goto block_53;
    block_58:;
    i0 = 10u;
    l3 = i0;
    goto block_53;
    block_57:;
    i0 = 11u;
    l3 = i0;
    goto block_53;
    block_56:;
    i0 = 12u;
    l3 = i0;
    goto block_53;
    block_55:;
    i0 = 13u;
    l3 = i0;
    goto block_53;
    block_54:;
    i0 = 14u;
    l3 = i0;
    block_53:;
  }
  loop_69: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 1u;
      i0 <<= (i1 & 31);
      l4 = i0;
      i1 = kF346Const2B660;
      i0 += i1;
      i0 = LoadU8(ctx, i0);
      i1 = message;
      i0 = i0 <= i1;
      l6 = i0;
      if (i0) {
        i0 = l3;
        l4 = i0;
        goto block_51;
      }
    }
    i0 = l7;
    i1 = 12u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      StepRecordResume(ctx, i0);
      i0 = 12u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 48u);
      l6 = i0;
    }
    i0 = l7;
    i1 = 13u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l6;
      i1 = l3;
      i2 = func_state;
      EmitCondition(ctx, i0, i1, i2);
      i0 = 13u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = kF346Const2B661;
      i0 += i1;
      i0 = LoadU8(ctx, i0);
      l4 = i0;
      i0 = l9;
      i1 = 8u;
      i0 += i1;
      l6 = i0;
    }
    i0 = l7;
    i1 = 14u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l6;
      i2 = l4;
      i0 = CheckStatementLimits(ctx, i0, i1, i2);
      l5 = i0;
      i0 = 14u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l6 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 48u);
      l8 = i0;
      i0 = l9;
      i1 = 8u;
      i0 += i1;
      l4 = i0;
    }
    i0 = l7;
    i1 = 15u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l8;
      i1 = l3;
      i2 = func_state;
      i3 = l4;
      EmitExpression(ctx, i0, i1, i2, i3);
      i0 = 15u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = 15u;
      l4 = i0;
      i0 = l6;
      l3 = i0;
      i1 = 15u;
      i0 = i0 != i1;
      l6 = i0;
      if (i0) {goto loop_69;}
    }
  block_51:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 52u);
    state = i0;
    i0 = LoadU16(ctx, i0 + 52u);
    i1 = 1u;
    i0 -= i1;
    func_state = i0;
    i0 = state;
    i1 = func_state;
    StoreU16(ctx, i0 + 52u, i1);
    i0 = l9;
    i1 = 32u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    i0 = l4;
    goto func_exit;
  }
  TrapUnreachable();
  spill_frame:;
  l5 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l5;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF346ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l5 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = func_state;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l5;
  i1 = message;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l5;
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l5;
  i1 = l3;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l5;
  i1 = l6;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l5;
  i1 = l8;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l5;
  i1 = l9;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 32u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
