// parse_arguments.cpp -- parses an argument list: the argument expressions are compiled into the parameter slots..
//
// ParseArguments -- parses an argument list: the argument expressions are compiled into the parameter slots.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 797842-797842 (f1714); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void CompileMainBody(EngineContext* ctx, uint32_t a0, uint32_t a1);
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
void StepRecordResume(EngineContext* ctx, uint32_t a0);
uint32_t CheckStatementLimits(EngineContext* ctx, uint32_t state, uint32_t func_state, uint32_t message);
uint32_t EmitOpInstruction(EngineContext* ctx, uint32_t func_state, uint32_t op, uint32_t arg_a, uint32_t arg_b, uint32_t line);
void CheckExprComplexity(EngineContext* ctx, uint32_t a, uint32_t b);
void CheckClosingMatch(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d);
uint32_t EncodeConstant(EngineContext* ctx, uint32_t func_state, uint32_t value);
void AllocateExpressionRegister(EngineContext* ctx, uint32_t state, uint32_t instruction, uint32_t expression);

namespace {

constexpr uint32_t kF1714MsgAmbiguousSyntaxFunctionCall = 0x2B789u;  // ambiguous syntax (function call x new statement)
constexpr uint32_t kF1714MsgFunctionArgumentsExpected = 0x2B7BAu;  // function arguments expected
constexpr uint32_t kF1714ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF1714SaveFrameBytes = 28u;
constexpr uint32_t kF1714ResumeWordBytes = 4u;

}  // namespace

void ParseArguments(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1714SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
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
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1714ResumeWordBytes;
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
    l5 = i0;
    GuestStackSp(ctx) = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
    l2 = i0;
    i1 = 286u;
    i0 = i0 != i1;
    l7 = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 48u);
    l6 = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 4u);
    l8 = i0;
  }
  i0 = l7;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i1 = 123u;
      i0 = i0 == i1;
      l7 = i0;
      if (i0) {goto block_6;}
      i0 = l2;
      i1 = 40u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_7;}
      i0 = l8;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 8u);
      i0 = i0 != i1;
      l2 = i0;
    }
    i0 = l2;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l4;
      i0 = !(i0);
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = kF1714MsgAmbiguousSyntaxFunctionCall;
        RaiseSyntaxError(ctx, i0, i1);
        i0 = 0u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      StepRecordResume(ctx, i0);
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 16u);
      i1 = 41u;
      i0 = i0 == i1;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      if (i0) {
        i0 = l5;
        i1 = 0u;
        StoreU32(ctx, i0 + 8u, i1);
        goto block_15;
      }
      i0 = l5;
      i1 = 8u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l4;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = l2;
      i2 = 0u;
      i0 = CheckStatementLimits(ctx, i0, i1, i2);
      l3 = i0;
      i0 = 2u;
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
      i0 = a;
      i0 = LoadU32(ctx, i0 + 16u);
      i1 = 44u;
      i0 = i0 == i1;
      l2 = i0;
    }
    i0 = l2;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      loop_22: 
        i0 = l4;
        i1 = 3u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = a;
          StepRecordResume(ctx, i0);
          i0 = 3u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l5;
          i1 = 8u;
          i0 += i1;
          l7 = i0;
          i0 = a;
          i0 = LoadU32(ctx, i0 + 48u);
          l2 = i0;
        }
        i0 = l4;
        i1 = 4u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l2;
          i1 = l7;
          CheckExprComplexity(ctx, i0, i1);
          i0 = 4u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = l2;
        i1 = l5;
        i2 = 8u;
        i1 += i2;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        l2 = i0;
        i0 = l4;
        i1 = 5u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = a;
          i1 = l2;
          i2 = 0u;
          i0 = CheckStatementLimits(ctx, i0, i1, i2);
          l3 = i0;
          i0 = 5u;
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
          i0 = a;
          i0 = LoadU32(ctx, i0 + 16u);
          i1 = 44u;
          i0 = i0 == i1;
          l2 = i0;
          if (i0) {goto loop_22;}
        }
    }
    i0 = l2;
    i1 = l5;
    i2 = 8u;
    i1 += i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l2 = i0;
    i0 = l4;
    i1 = 6u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l6;
      i1 = l2;
      i2 = kF1714ConstFFFFFFFF;
      AllocateExpressionRegister(ctx, i0, i1, i2);
      i0 = 6u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    block_15:;
    i0 = l4;
    i1 = 7u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = 41u;
      i2 = 40u;
      i3 = l8;
      CheckClosingMatch(ctx, i0, i1, i2, i3);
      i0 = 7u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_5;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 24u);
    l2 = i0;
  }
  i0 = l4;
  i1 = 8u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = l2;
    i0 = EncodeConstant(ctx, i0, i1);
    l3 = i0;
    i0 = 8u;
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
    i0 = l5;
    j1 = 18446744073709551615ull;
    StoreU64(ctx, i0 + 24u, j1);
    i0 = l5;
    i1 = l2;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = l5;
    i1 = 4u;
    StoreU32(ctx, i0 + 8u, i1);
  }
  i0 = l4;
  i1 = 9u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    StepRecordResume(ctx, i0);
    i0 = 9u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_5;}
  block_7:;
  i0 = l4;
  i1 = 10u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF1714MsgFunctionArgumentsExpected;
    RaiseSyntaxError(ctx, i0, i1);
    i0 = 10u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto running_exit;}
  block_6:;
  i0 = l2;
  i1 = l5;
  i2 = 8u;
  i1 += i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l2 = i0;
  i0 = l4;
  i1 = 11u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = l2;
    CompileMainBody(ctx, i0, i1);
    i0 = 11u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    l2 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    l7 = i0;
    i0 = b;
    i0 = LoadU32(ctx, i0 + 8u);
    a = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    switch (i0) {
      case 0: goto block_38;
      case 1: goto block_40;
      case 2: goto block_40;
      case 3: goto block_40;
      case 4: goto block_40;
      case 5: goto block_40;
      case 6: goto block_40;
      case 7: goto block_40;
      case 8: goto block_40;
      case 9: goto block_40;
      case 10: goto block_40;
      case 11: goto block_40;
      case 12: goto block_40;
      case 13: goto block_37;
      case 14: goto block_37;
      default: goto block_40;
    }
    block_40:;
    i0 = l5;
    i1 = 8u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l4;
  i1 = 12u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = l2;
    CheckExprComplexity(ctx, i0, i1);
    i0 = 12u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_38:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 36u);
    i1 = a;
    i0 -= i1;
    l2 = i0;
  }
  block_37:;
  i0 = l4;
  i1 = 13u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = 28u;
    i2 = a;
    i3 = l2;
    i4 = 2u;
    i0 = EmitOpInstruction(ctx, i0, i1, i2, i3, i4);
    l3 = i0;
    i0 = 13u;
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
    i0 = b;
    j1 = 18446744073709551615ull;
    StoreU64(ctx, i0 + 16u, j1);
    i0 = b;
    i1 = l2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = b;
    i1 = 13u;
    StoreU32(ctx, i0, i1);
    i0 = l6;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 20u);
    i1 = l6;
    i1 = LoadU32(ctx, i1 + 24u);
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    i1 = 4u;
    i0 -= i1;
    i1 = l8;
    StoreU32(ctx, i0, i1);
    i0 = l6;
    i1 = a;
    i2 = 1u;
    i1 += i2;
    StoreU32(ctx, i0 + 36u, i1);
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
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
  i2 = kF1714ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = b;
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
