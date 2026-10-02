// parse_assignment.cpp -- parses an assignment, reconciling the target list against the values on the right..
//
// ParseAssignment -- parses an assignment, reconciling the target list against the values on the right.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 801460-801460 (f1719); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void ReleaseExpressionSlots(EngineContext* ctx, uint32_t state, uint32_t instruction, uint32_t count, uint32_t unused);
void CheckMatch(EngineContext* ctx, uint32_t lex_state, uint32_t expected);
void EmitVariableLoad(EngineContext* ctx, uint32_t state, uint32_t variable, uint32_t register_hint);
uint32_t FormatPushString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
void StepRecordResume(EngineContext* ctx, uint32_t a0);
uint32_t CheckStatementLimits(EngineContext* ctx, uint32_t state, uint32_t func_state, uint32_t message);
uint32_t FormatCharValue(EngineContext* ctx, uint32_t a, uint32_t b);
void LexError(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t EmitOpInstruction(EngineContext* ctx, uint32_t func_state, uint32_t op, uint32_t arg_a, uint32_t arg_b, uint32_t line);
void CheckExprComplexity(EngineContext* ctx, uint32_t a, uint32_t b);
void PatchInstructionTarget(EngineContext* ctx, uint32_t state, uint32_t pending);
void ReserveRegisters(EngineContext* ctx, uint32_t state, uint32_t count);

namespace {

constexpr uint32_t kF1719MsgSExpected = 0x2B625u;  // '%s' expected
constexpr uint32_t kF1719MsgMainFunctionHasMore = 0x2B6C0u;  // main function has more than %d %s
constexpr uint32_t kF1719MsgFunctionAtLineD = 0x2B6E2u;  // function at line %d has more than %d %s
constexpr uint32_t kF1719MsgSyntaxError = 0x2B872u;  // syntax error
constexpr uint32_t kF1719MsgVariablesInAssignment = 0x2B87Fu;  // variables in assignment
constexpr uint32_t kF1719SaveFrameBytes = 36u;
constexpr uint32_t kF1719ResumeWordBytes = 4u;

}  // namespace

void ParseAssignment(EngineContext* ctx, uint32_t state, uint32_t target_slot, uint32_t value_slot) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1719SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    value_slot = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 20u);
    l7 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 24u);
    l8 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 28u);
    l9 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 32u);
    l10 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 4u);
    target_slot = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1719ResumeWordBytes;
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
    i1 = 80u;
    i0 -= i1;
    l5 = i0;
    GuestStackSp(ctx) = i0;
    i0 = target_slot;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = 6u;
    i0 -= i1;
    i1 = 4u;
    i0 = i0 >= i1;
    l3 = i0;
  }
  i0 = l3;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l6;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = kF1719MsgSyntaxError;
      RaiseSyntaxError(ctx, i0, i1);
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
    i0 = target_slot;
    i1 = 8u;
    i0 += i1;
    l10 = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = 44u;
    i0 -= i1;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    switch (i0) {
      case 0: goto block_15;
      case 1: goto block_13;
      case 2: goto block_13;
      case 3: goto block_13;
      case 4: goto block_13;
      case 5: goto block_13;
      case 6: goto block_13;
      case 7: goto block_13;
      case 8: goto block_13;
      case 9: goto block_13;
      case 10: goto block_13;
      case 11: goto block_13;
      case 12: goto block_13;
      case 13: goto block_13;
      case 14: goto block_13;
      case 15: goto block_13;
      case 16: goto block_13;
      case 17: goto block_12;
      default: goto block_13;
    }
    block_15:;
  }
  i0 = l6;
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
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = target_slot;
    StoreU32(ctx, i0 + 48u, i1);
    i0 = l5;
    i1 = 56u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l6;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l3;
    CheckMatch(ctx, i0, i1);
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
    i0 = LoadU32(ctx, i0 + 56u);
    i1 = 6u;
    i0 = i0 != i1;
    l3 = i0;
    if (i0) {goto block_10;}
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    l7 = i0;
    i0 = LoadU32(ctx, i0 + 36u);
    l9 = i0;
    loop_20: 
      i0 = target_slot;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = 9u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {
        i0 = l5;
        i0 = LoadU32(ctx, i0 + 64u);
        l3 = i0;
        i1 = target_slot;
        i1 = LoadU32(ctx, i1 + 16u);
        i0 = i0 == i1;
        if (i0) {
          i0 = target_slot;
          i1 = l9;
          StoreU32(ctx, i0 + 16u, i1);
          i0 = 1u;
          l8 = i0;
          i0 = l5;
          i0 = LoadU32(ctx, i0 + 64u);
          l3 = i0;
        }
        i0 = target_slot;
        i0 = LoadU32(ctx, i0 + 20u);
        i1 = l3;
        i0 = i0 == i1;
        if (i0) {goto block_21;}
      }
      i0 = target_slot;
      i0 = LoadU32(ctx, i0);
      target_slot = i0;
      if (i0) {goto loop_20;}
      i0 = l8;
      i0 = !(i0);
      target_slot = i0;
      if (i0) {goto block_10;}
      i0 = l5;
      i0 = LoadU32(ctx, i0 + 64u);
      l3 = i0;
      goto block_11;
      block_21:;
      i0 = target_slot;
      i1 = l9;
      StoreU32(ctx, i0 + 20u, i1);
      i0 = 1u;
      l8 = i0;
      i0 = target_slot;
      i0 = LoadU32(ctx, i0);
      target_slot = i0;
      if (i0) {goto loop_20;}
    goto block_11;
  }
  block_13:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 52u);
    target_slot = i0;
  }
  i0 = l6;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = 61u;
    i0 = FormatCharValue(ctx, i0, i1);
    l4 = i0;
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    StoreU32(ctx, i0, i1);
  }
  i0 = l6;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = target_slot;
    i1 = kF1719MsgSExpected;
    i2 = l5;
    i0 = FormatPushString(ctx, i0, i1, i2);
    l4 = i0;
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    target_slot = i0;
  }
  i0 = l6;
  i1 = 5u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = target_slot;
    RaiseSyntaxError(ctx, i0, i1);
    i0 = 5u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_12:;
  i0 = l6;
  i1 = 6u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    StepRecordResume(ctx, i0);
    i0 = 6u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = target_slot;
  i1 = l5;
  i2 = 48u;
  i1 += i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  target_slot = i0;
  i0 = l6;
  i1 = 7u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = target_slot;
    i2 = 0u;
    i0 = CheckStatementLimits(ctx, i0, i1, i2);
    l4 = i0;
    i0 = 7u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    target_slot = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = 44u;
    i0 = i0 == i1;
    l3 = i0;
    i0 = 1u;
    target_slot = i0;
  }
  i0 = l3;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    loop_33: 
      i0 = l6;
      i1 = 8u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        StepRecordResume(ctx, i0);
        i0 = 8u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        i1 = 48u;
        i0 += i1;
        l7 = i0;
        i0 = state;
        i0 = LoadU32(ctx, i0 + 48u);
        l3 = i0;
      }
      i0 = l6;
      i1 = 9u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l3;
        i1 = l7;
        CheckExprComplexity(ctx, i0, i1);
        i0 = 9u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = l3;
      i1 = l5;
      i2 = 48u;
      i1 += i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      l3 = i0;
      i0 = l6;
      i1 = 10u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = l3;
        i2 = 0u;
        i0 = CheckStatementLimits(ctx, i0, i1, i2);
        l4 = i0;
        i0 = 10u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l4;
        l3 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = target_slot;
        i1 = 1u;
        i0 += i1;
        target_slot = i0;
        i0 = state;
        i0 = LoadU32(ctx, i0 + 16u);
        i1 = 44u;
        i0 = i0 == i1;
        l3 = i0;
        if (i0) {goto loop_33;}
      }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = target_slot;
    i1 = value_slot;
    i0 = i0 != i1;
    l7 = i0;
    i0 = state;
    i1 = 48u;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
  }
  i0 = l7;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l7;
    i1 = l5;
    i2 = 48u;
    i1 += i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l7 = i0;
    i0 = l6;
    i1 = 11u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l3;
      i1 = value_slot;
      i2 = target_slot;
      i3 = l7;
      ReleaseExpressionSlots(ctx, i0, i1, i2, i3);
      i0 = 11u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = target_slot;
      i1 = value_slot;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {goto block_9;}
      i0 = state;
      i0 = LoadU32(ctx, i0 + 48u);
      l3 = i0;
      i0 = LoadU32(ctx, i0 + 36u);
      i1 = value_slot;
      i2 = target_slot;
      i1 -= i2;
      i0 += i1;
      target_slot = i0;
      i0 = l3;
      i1 = target_slot;
      StoreU32(ctx, i0 + 36u, i1);
      goto block_9;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = l5;
    i2 = 48u;
    i1 += i2;
    target_slot = i1;
    PatchInstructionTarget(ctx, i0, i1);
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    state = i0;
    goto block_8;
  }
  block_11:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 36u);
    target_slot = i0;
  }
  i0 = l6;
  i1 = 12u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l7;
    i1 = 0u;
    i2 = target_slot;
    i3 = l3;
    i4 = 0u;
    i0 = EmitOpInstruction(ctx, i0, i1, i2, i3, i4);
    l4 = i0;
    i0 = 12u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    target_slot = i0;
  }
  i0 = l6;
  i1 = 13u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l7;
    i1 = 1u;
    ReserveRegisters(ctx, i0, i1);
    i0 = 13u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_10:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = value_slot;
    i1 = 200u;
    i2 = state;
    i2 = LoadU32(ctx, i2 + 52u);
    i2 = LoadU16(ctx, i2 + 52u);
    i1 -= i2;
    target_slot = i1;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    l3 = i0;
  }
  i0 = l3;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l9;
    } else {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 48u);
      l3 = i0;
      i0 = LoadU32(ctx, i0 + 16u);
      l7 = i0;
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 60u);
      l8 = i0;
      i0 = !(i0);
    }
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        i1 = kF1719MsgVariablesInAssignment;
        StoreU32(ctx, i0 + 20u, i1);
        i0 = l5;
        i1 = target_slot;
        StoreU32(ctx, i0 + 16u, i1);
        i0 = l5;
        i1 = 16u;
        i0 += i1;
        target_slot = i0;
      }
      i0 = l6;
      i1 = 14u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l7;
        i1 = kF1719MsgMainFunctionHasMore;
        i2 = target_slot;
        i0 = FormatPushString(ctx, i0, i1, i2);
        l4 = i0;
        i0 = 14u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l4;
        target_slot = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_50;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i1 = kF1719MsgVariablesInAssignment;
      StoreU32(ctx, i0 + 40u, i1);
      i0 = l5;
      i1 = target_slot;
      StoreU32(ctx, i0 + 36u, i1);
      i0 = l5;
      i1 = l8;
      StoreU32(ctx, i0 + 32u, i1);
      i0 = l5;
      i1 = 32u;
      i0 += i1;
      target_slot = i0;
    }
    i0 = l6;
    i1 = 15u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l7;
      i1 = kF1719MsgFunctionAtLineD;
      i2 = target_slot;
      i0 = FormatPushString(ctx, i0, i1, i2);
      l4 = i0;
      i0 = 15u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      target_slot = i0;
    }
    block_50:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 12u);
      l3 = i0;
    }
    i0 = l6;
    i1 = 16u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l3;
      i1 = target_slot;
      i2 = 0u;
      LexError(ctx, i0, i1, i2);
      i0 = 16u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = value_slot;
    i1 = 1u;
    i0 += i1;
    value_slot = i0;
    i0 = l5;
    i1 = 48u;
    i0 += i1;
    target_slot = i0;
  }
  i0 = l6;
  i1 = 17u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = target_slot;
    i2 = value_slot;
    ParseAssignment(ctx, i0, i1, i2);
    i0 = 17u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_9:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    state = i0;
    i0 = LoadU32(ctx, i0 + 36u);
    target_slot = i0;
    i0 = l5;
    j1 = 18446744073709551615ull;
    StoreU64(ctx, i0 + 64u, j1);
    i0 = l5;
    i1 = 12u;
    StoreU32(ctx, i0 + 48u, i1);
    i0 = l5;
    i1 = target_slot;
    i2 = 1u;
    i1 -= i2;
    target_slot = i1;
    StoreU32(ctx, i0 + 56u, i1);
  }
  block_8:;
  i0 = target_slot;
  i1 = l5;
  i2 = 48u;
  i1 += i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  target_slot = i0;
  i0 = l6;
  i1 = 18u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l10;
    i2 = target_slot;
    EmitVariableLoad(ctx, i0, i1, i2);
    i0 = 18u;
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
  l4 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l4;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1719ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = target_slot;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l4;
  i1 = value_slot;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l4;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l4;
  i1 = l5;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l4;
  i1 = l7;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l4;
  i1 = l8;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l4;
  i1 = l9;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l4;
  i1 = l10;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 36u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
