// check_match.cpp -- the luaY_check-shaped syntax match check: on a mismatch it names what was expected, the token that showed up instead and the position, then raises.
//
// CheckMatch -- the luaY_check-shaped syntax match check: on a mismatch it names what was expected, the token that showed up instead and the position, then raises. Calls RaiseSyntaxError(lex_state, message) and FormatPushString.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 483874-484612 (f1119); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void ExpectToken(EngineContext* ctx, uint32_t a, uint32_t b);
uint32_t AppendWithLimit(EngineContext* ctx, uint32_t list, uint32_t entry, uint32_t limit, uint32_t what);
void MatchKeyword(EngineContext* ctx, uint32_t a, uint32_t b);
void ParseArguments(EngineContext* ctx, uint32_t a, uint32_t b);
void EmitMoveSlot(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t FormatPushString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
void StepRecordResume(EngineContext* ctx, uint32_t a0);
uint32_t CheckStatementLimits(EngineContext* ctx, uint32_t state, uint32_t func_state, uint32_t message);
uint32_t FormatCharValue(EngineContext* ctx, uint32_t a, uint32_t b);
void CheckExprComplexity(EngineContext* ctx, uint32_t a, uint32_t b);
void DischargeExpression(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t EmitConditionJump(EngineContext* ctx, uint32_t func_state, uint32_t condition);
void CheckClosingMatch(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d);
void EmitJumpIf(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t EncodeConstant(EngineContext* ctx, uint32_t func_state, uint32_t value);

namespace {

constexpr uint32_t kF1119MsgSExpected = 0x2B625u;  // '%s' expected
constexpr uint32_t kF1119MsgUnexpectedSymbol = 0x2B76Du;  // unexpected symbol
constexpr uint32_t kF1119SaveFrameBytes = 28u;
constexpr uint32_t kF1119ResumeWordBytes = 4u;

}  // namespace

void CheckMatch(EngineContext* ctx, uint32_t lex_state, uint32_t expected) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1119SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    lex_state = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l5 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 16u);
    l6 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 20u);
    l7 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 24u);
    l8 = i0;
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
    i2 = kF1119ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    i1 = 32u;
    i0 -= i1;
    l6 = i0;
    GuestStackSp(ctx) = i0;
    i0 = lex_state;
    i0 = LoadU32(ctx, i0 + 48u);
    l7 = i0;
    i0 = lex_state;
    i0 = LoadU32(ctx, i0 + 16u);
    l2 = i0;
    i1 = 285u;
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
      i0 = l2;
      i1 = 40u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto running_exit;}
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 4u);
      l2 = i0;
    }
    i0 = l4;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = lex_state;
      StepRecordResume(ctx, i0);
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = lex_state;
      i1 = expected;
      i2 = 0u;
      i0 = CheckStatementLimits(ctx, i0, i1, i2);
      l3 = i0;
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l3;
      l5 = i0;
    }
    i0 = l4;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = lex_state;
      i1 = 41u;
      i2 = 40u;
      i3 = l2;
      CheckClosingMatch(ctx, i0, i1, i2, i3);
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 48u);
      l2 = i0;
    }
    i0 = l4;
    i1 = 3u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = expected;
      DischargeExpression(ctx, i0, i1);
      i0 = 3u;
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
    i0 = lex_state;
    i0 = LoadU32(ctx, i0 + 24u);
    l2 = i0;
  }
  i0 = l4;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = lex_state;
    StepRecordResume(ctx, i0);
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = lex_state;
    i0 = LoadU32(ctx, i0 + 48u);
    l5 = i0;
  }
  i0 = l4;
  i1 = 5u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l5;
    i1 = l2;
    i2 = expected;
    i3 = 1u;
    i0 = AppendWithLimit(ctx, i0, i1, i2, i3);
    l3 = i0;
    i0 = 5u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    l8 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  i1 = 0u;
  i2 = l8;
  i3 = 8u;
  i2 = i2 != i3;
  i0 = i2 ? i0 : i1;
  if (i0) {goto block_3;}
  i0 = l4;
  i1 = 6u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l5;
    i1 = l2;
    i0 = EncodeConstant(ctx, i0, i1);
    l3 = i0;
    i0 = 6u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = expected;
    i1 = l2;
    StoreU32(ctx, i0 + 8u, i1);
    goto block_3;
  }
  running_exit:;
  i0 = l4;
  i1 = 7u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = lex_state;
    i1 = kF1119MsgUnexpectedSymbol;
    RaiseSyntaxError(ctx, i0, i1);
    i0 = 7u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_3:;
  loop_21: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 16u);
      l2 = i0;
      i1 = 90u;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l5 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      if (i0) {
        i0 = l2;
        i1 = 40u;
        i0 -= i1;
        l2 = i0;
        switch (i0) {
          case 0: goto block_28;
          case 1: goto block_22;
          case 2: goto block_22;
          case 3: goto block_22;
          case 4: goto block_22;
          case 5: goto block_22;
          case 6: goto block_26;
          case 7: goto block_22;
          case 8: goto block_22;
          case 9: goto block_22;
          case 10: goto block_22;
          case 11: goto block_22;
          case 12: goto block_22;
          case 13: goto block_22;
          case 14: goto block_22;
          case 15: goto block_22;
          case 16: goto block_22;
          case 17: goto block_22;
          case 18: goto block_24;
          default: goto block_22;
        }
      }
      i0 = l2;
      i1 = 91u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_25;}
      i0 = l2;
      i1 = 123u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_28;}
      i0 = l2;
      i1 = 286u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_22;}
      block_28:;
    }
    i0 = l4;
    i1 = 8u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l7;
      i1 = expected;
      CheckExprComplexity(ctx, i0, i1);
      i0 = 8u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = l4;
    i1 = 9u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = lex_state;
      i1 = expected;
      ParseArguments(ctx, i0, i1);
      i0 = 9u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto loop_21;}
    block_26:;
    i0 = l4;
    i1 = 10u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = lex_state;
      i1 = expected;
      ExpectToken(ctx, i0, i1);
      i0 = 10u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto loop_21;}
    block_25:;
    i0 = l4;
    i1 = 11u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l7;
      i1 = expected;
      i0 = EmitConditionJump(ctx, i0, i1);
      l3 = i0;
      i0 = 11u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l3;
    } else {
      i0 = l2;
    }
    i1 = l6;
    i2 = 8u;
    i1 += i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l2 = i0;
    i0 = l4;
    i1 = 12u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = lex_state;
      i1 = l2;
      MatchKeyword(ctx, i0, i1);
      i0 = 12u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = l2;
    i1 = l6;
    i2 = 8u;
    i1 += i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l2 = i0;
    i0 = l4;
    i1 = 13u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l7;
      i1 = expected;
      i2 = l2;
      EmitMoveSlot(ctx, i0, i1, i2);
      i0 = 13u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto loop_21;}
    block_24:;
    i0 = l4;
    i1 = 14u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = lex_state;
      StepRecordResume(ctx, i0);
      i0 = 14u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 16u);
      i1 = 285u;
      i0 = i0 != i1;
      l2 = i0;
    }
    i0 = l2;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = lex_state;
        i0 = LoadU32(ctx, i0 + 52u);
        l2 = i0;
      }
      i0 = l4;
      i1 = 15u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = lex_state;
        i1 = 285u;
        i0 = FormatCharValue(ctx, i0, i1);
        l3 = i0;
        i0 = 15u;
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
        i0 = l6;
        i1 = l5;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 16u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = kF1119MsgSExpected;
        i2 = l6;
        i0 = FormatPushString(ctx, i0, i1, i2);
        l3 = i0;
        i0 = 16u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l3;
        l2 = i0;
      }
      i0 = l4;
      i1 = 17u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = lex_state;
        i1 = l2;
        RaiseSyntaxError(ctx, i0, i1);
        i0 = 17u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 24u);
      l2 = i0;
    }
    i0 = l4;
    i1 = 18u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = lex_state;
      StepRecordResume(ctx, i0);
      i0 = 18u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = lex_state;
      i0 = LoadU32(ctx, i0 + 48u);
      l5 = i0;
    }
    i0 = l4;
    i1 = 19u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l5;
      i1 = l2;
      i0 = EncodeConstant(ctx, i0, i1);
      l3 = i0;
      i0 = 19u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l3;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      j1 = 18446744073709551615ull;
      StoreU64(ctx, i0 + 24u, j1);
      i0 = l6;
      i1 = l2;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l6;
      i1 = 4u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l6;
      i1 = 8u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l4;
    i1 = 20u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l7;
      i1 = expected;
      i2 = l2;
      EmitJumpIf(ctx, i0, i1, i2);
      i0 = 20u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = l4;
    i1 = 21u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = lex_state;
      i1 = expected;
      ParseArguments(ctx, i0, i1);
      i0 = 21u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto loop_21;}
    block_22:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
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
  i2 = kF1119ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = lex_state;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = expected;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l3;
  i1 = l5;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l3;
  i1 = l6;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l3;
  i1 = l7;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l3;
  i1 = l8;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 28u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
