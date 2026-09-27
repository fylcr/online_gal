// compile_statement.cpp -- the statement compiler proper: dispatches on the statement kind and emits the code for it..
//
// CompileStatement -- the statement compiler proper: dispatches on the statement kind and emits the code for it.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 348345-348345 (f899); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void ExpectToken(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t AppendWithLimit(EngineContext* ctx, uint32_t list, uint32_t entry, uint32_t limit, uint32_t what);
void ReleaseExpressionSlots(EngineContext* ctx, uint32_t state, uint32_t instruction, uint32_t count, uint32_t unused);
void ParseParamList(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3);
void CheckMatch(EngineContext* ctx, uint32_t lex_state, uint32_t expected);
void EmitVariableLoad(EngineContext* ctx, uint32_t state, uint32_t variable, uint32_t register_hint);
void ParseExpression(EngineContext* ctx, uint32_t state, uint32_t token, uint32_t slot, uint32_t depth, uint32_t flag);
void ParseAssignment(EngineContext* ctx, uint32_t state, uint32_t target_slot, uint32_t value_slot);
uint32_t ParseSimpleAssignment(EngineContext* ctx, uint32_t state);
void EmitJumpInstruction(EngineContext* ctx, uint32_t state, uint32_t register_hint, uint32_t target);
uint32_t FormatPushString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void RaiseSyntaxError(EngineContext* ctx, uint32_t a0, uint32_t a1);
void StepRecordResume(EngineContext* ctx, uint32_t a0);
uint32_t CheckStatementLimits(EngineContext* ctx, uint32_t state, uint32_t func_state, uint32_t message);
uint32_t FormatCharValue(EngineContext* ctx, uint32_t a0, uint32_t a1);
void LexError(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t EmitOpInstruction(EngineContext* ctx, uint32_t func_state, uint32_t op, uint32_t arg_a, uint32_t arg_b, uint32_t line);
void CheckExprComplexity(EngineContext* ctx, uint32_t a0, uint32_t a1);
void DeclareLocalVariable(EngineContext* ctx, uint32_t state, uint32_t name, uint32_t slot);
void PatchJumpChain(EngineContext* ctx, uint32_t state, uint32_t instruction);
uint32_t EmitConditionJump(EngineContext* ctx, uint32_t func_state, uint32_t condition);
void CheckClosingMatch(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3);
uint32_t BindUpvalueCell(EngineContext* ctx, uint32_t state, uint32_t local, uint32_t upvalue);
uint32_t AddOneConstant(EngineContext* ctx, uint32_t state);
void ReserveThreeRegisters(EngineContext* ctx, uint32_t state);
uint32_t ConcatJumpLists(EngineContext* ctx, uint32_t state);
uint32_t EncodeConstant(EngineContext* ctx, uint32_t func_state, uint32_t value);
void ReserveRegisters(EngineContext* ctx, uint32_t state, uint32_t count);
void CloseBlockScope(EngineContext* ctx, uint32_t state);
void FixPendingJumpTargets(EngineContext* ctx, uint32_t state, uint32_t instruction, uint32_t target);
void EmitVariableStore(EngineContext* ctx, uint32_t state, uint32_t variable);
void AllocateExpressionRegister(EngineContext* ctx, uint32_t state, uint32_t instruction, uint32_t expression);
void PatchComparisonIntoJump(EngineContext* ctx, uint32_t state, uint32_t instruction, uint32_t expression);
uint32_t EmitInstructionABC(EngineContext* ctx, uint32_t state, uint32_t opcode, uint32_t arg_a, uint32_t arg_c);

namespace {

constexpr uint32_t kF899MsgSExpected = 0x2B625u;  // '%s' expected
constexpr uint32_t kF899MsgChunkHasTooMany = 0x2B633u;  // chunk has too many syntax levels
constexpr uint32_t kF899MsgOrInExpected = 0x2B7FFu;  // '=' or 'in' expected
constexpr uint32_t kF899MsgForIndex = 0x2B814u;  // (for index)
constexpr uint32_t kF899MsgForLimit = 0x2B820u;  // (for limit)
constexpr uint32_t kF899MsgForStep = 0x2B82Cu;  // (for step)
constexpr uint32_t kF899MsgForGenerator = 0x2B837u;  // (for generator)
constexpr uint32_t kF899MsgForState = 0x2B847u;  // (for state)
constexpr uint32_t kF899MsgForControl = 0x2B853u;  // (for control)
constexpr uint32_t kF899MsgNoLoopToBreak = 0x2B861u;  // no loop to break
constexpr uint32_t kF899Const8010007 = 0x8010007u;  // 
constexpr uint32_t kF899ConstFF803FFF = 0xFF803FFFu;  // 
constexpr uint32_t kF899ConstFFFFFFC0 = 0xFFFFFFC0u;  // 
constexpr uint32_t kF899ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF899SaveFrameBytes = 56u;
constexpr uint32_t kF899ResumeWordBytes = 4u;

}  // namespace

uint32_t CompileStatement(EngineContext* ctx, uint32_t state) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l13 = 0u; uint32_t l14 = 0u; uint32_t l15 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  uint64_t j1 = 0;
      l9 = 0, l10 = 0, l11 = 0, l12 = 0, l13 = 0, l14 = 0, l15 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF899SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 16u);
    l6 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 20u);
    l7 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 24u);
    l8 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 28u);
    l9 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 32u);
    l10 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 36u);
    l11 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 40u);
    l12 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 44u);
    l13 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 48u);
    l14 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 52u);
    l15 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 4u);
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF899ResumeWordBytes;
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
    i1 = 208u;
    i0 -= i1;
    l6 = i0;
    GuestStackSp(ctx) = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 4u);
    l9 = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = 258u;
    i0 -= i1;
    l7 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    switch (i0) {
      case 0: goto block_13;
      case 1: goto block_7;
      case 2: goto block_14;
      case 3: goto block_14;
      case 4: goto block_14;
      case 5: goto block_14;
      case 6: goto block_8;
      case 7: goto block_10;
      case 8: goto block_16;
      case 9: goto block_14;
      case 10: goto block_11;
      case 11: goto block_14;
      case 12: goto block_14;
      case 13: goto block_14;
      case 14: goto block_9;
      case 15: goto block_12;
      case 16: goto block_14;
      case 17: goto block_14;
      case 18: goto block_14;
      case 19: goto block_6;
      default: goto block_14;
    }
    block_16:;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    l1 = i0;
    i0 = l6;
    i1 = kF899ConstFFFFFFFF;
    StoreU32(ctx, i0 + 176u, i1);
  }
  i0 = l4;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i0 = ParseSimpleAssignment(ctx, i0);
    l5 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l3 = i0;
  }
  loop_18: 
    i0 = l14;
    i1 = ResumePhase(ctx);
    i1 = !(i1);
    if (i1) {
      i1 = state;
      i1 = LoadU32(ctx, i1 + 16u);
      l2 = i1;
      i2 = 261u;
      i1 = i1 != i2;
      l7 = i1;
    }
    i1 = l7;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l14 = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l2;
      i1 = l2;
      i2 = 260u;
      i1 = i1 == i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      l2 = i0;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = l2;
        i1 = l6;
        i2 = 176u;
        i1 += i2;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        l2 = i0;
        i0 = l4;
        i1 = 1u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l1;
          i0 = ConcatJumpLists(ctx, i0);
          l5 = i0;
          i0 = 1u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l5;
          l7 = i0;
        }
        i0 = l4;
        i1 = 2u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l1;
          i1 = l2;
          i2 = l7;
          FixPendingJumpTargets(ctx, i0, i1, i2);
          i0 = 2u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = l4;
        i1 = 3u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l1;
          i1 = l3;
          PatchJumpChain(ctx, i0, i1);
          i0 = 3u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = l4;
        i1 = 4u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = state;
          StepRecordResume(ctx, i0);
          i0 = 4u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = l4;
        i1 = 5u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = state;
          CloseBlockScope(ctx, i0);
          i0 = 5u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto block_22;}
      }
      i0 = l2;
      i1 = l6;
      i2 = 176u;
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
        i0 = l1;
        i1 = l2;
        i2 = l3;
        FixPendingJumpTargets(ctx, i0, i1, i2);
        i0 = 6u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      block_22:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        i0 = LoadU32(ctx, i0 + 176u);
        l2 = i0;
      }
      i0 = l4;
      i1 = 7u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l1;
        i1 = l2;
        PatchJumpChain(ctx, i0, i1);
        i0 = 7u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = l4;
      i1 = 8u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = 262u;
        i2 = 266u;
        i3 = l9;
        CheckClosingMatch(ctx, i0, i1, i2, i3);
        i0 = 8u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_5;}
    }
    i0 = l14;
    i0 = !(i0);
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l2;
      i1 = l6;
      i2 = 176u;
      i1 += i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      l2 = i0;
      i0 = l4;
      i1 = 9u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l1;
        i0 = ConcatJumpLists(ctx, i0);
        l5 = i0;
        i0 = 9u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l5;
        l7 = i0;
      }
      i0 = l4;
      i1 = 10u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l1;
        i1 = l2;
        i2 = l7;
        FixPendingJumpTargets(ctx, i0, i1, i2);
        i0 = 10u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = l4;
      i1 = 11u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l1;
        i1 = l3;
        PatchJumpChain(ctx, i0, i1);
        i0 = 11u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = l4;
      i1 = 12u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i0 = ParseSimpleAssignment(ctx, i0);
        l5 = i0;
        i0 = 12u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l5;
        l3 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto loop_18;}
    }
  block_14:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i1 = 184u;
    i0 += i1;
    l2 = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    l1 = i0;
  }
  i0 = l4;
  i1 = 13u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l2;
    CheckMatch(ctx, i0, i1);
    i0 = 13u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 184u);
    i1 = 13u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      i1 = l6;
      i1 = LoadU32(ctx, i1 + 192u);
      i2 = 2u;
      i1 <<= (i2 & 31);
      i0 += i1;
      state = i0;
      i0 = LoadU32(ctx, i0);
      i1 = kF899ConstFF803FFF;
      i0 &= i1;
      i1 = 16384u;
      i0 |= i1;
      l1 = i0;
      i0 = state;
      i1 = l1;
      StoreU32(ctx, i0, i1);
      goto block_5;
    }
    i0 = l6;
    i1 = 0u;
    StoreU32(ctx, i0 + 176u, i1);
    i0 = l6;
    i1 = 176u;
    i0 += i1;
    l1 = i0;
  }
  i0 = l4;
  i1 = 14u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l1;
    i2 = 1u;
    ParseAssignment(ctx, i0, i1, i2);
    i0 = 14u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_5;}
  block_13:;
  i0 = l4;
  i1 = 15u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    StepRecordResume(ctx, i0);
    i0 = 15u;
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
    l7 = i0;
    i0 = LoadU32(ctx, i0 + 20u);
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    if (i0) {
      loop_48: 
        i0 = l1;
        i0 = LoadU8(ctx, i0 + 10u);
        l8 = i0;
        if (i0) {
          i0 = l1;
          l2 = i0;
          goto block_45;
        }
        i0 = l1;
        i0 = LoadU8(ctx, i0 + 9u);
        l8 = i0;
        i1 = l3;
        i0 |= i1;
        l3 = i0;
        i0 = l1;
        i0 = LoadU32(ctx, i0);
        l1 = i0;
        if (i0) {goto loop_48;}
    }
  }
  i0 = l4;
  i1 = 16u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = kF899MsgNoLoopToBreak;
    RaiseSyntaxError(ctx, i0, i1);
    i0 = 16u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_45:;
  i0 = l3;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i0 = LoadU8(ctx, i0 + 8u);
      state = i0;
    }
    i0 = l4;
    i1 = 17u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l7;
      i1 = 35u;
      i2 = state;
      i3 = 0u;
      i4 = 0u;
      i0 = EmitOpInstruction(ctx, i0, i1, i2, i3, i4);
      l5 = i0;
      i0 = 17u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
    } else {
      i0 = state;
    }
  } else {
    i0 = state;
  }
  i1 = l2;
  i2 = 4u;
  i1 += i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  state = i0;
  i0 = l4;
  i1 = 18u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l7;
    i0 = ConcatJumpLists(ctx, i0);
    l5 = i0;
    i0 = 18u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l1 = i0;
  }
  i0 = l4;
  i1 = 19u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l7;
    i1 = state;
    i2 = l1;
    FixPendingJumpTargets(ctx, i0, i1, i2);
    i0 = 19u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = 1u;
  i1 = ResumePhase(ctx);
  i1 = !(i1);
  if (i1) {goto running_exit;}
  block_12:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    l2 = i0;
  }
  i0 = l4;
  i1 = 20u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    StepRecordResume(ctx, i0);
    i0 = 20u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    l3 = i0;
    i1 = 260u;
    i0 -= i1;
    l1 = i0;
    i1 = 27u;
    i0 = i0 > i1;
    l7 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    if (i0) {goto block_61;}
    i0 = 1u;
    i1 = l1;
    i0 <<= (i1 & 31);
    i1 = kF899Const8010007;
    i0 &= i1;
    i0 = !(i0);
    if (i0) {goto block_61;}
    i0 = 0u;
    l1 = i0;
    goto block_59;
    block_61:;
    i0 = 0u;
    l1 = i0;
    i0 = l3;
    i1 = 59u;
    i0 = i0 == i1;
    l3 = i0;
    if (i0) {goto block_59;}
    i0 = l6;
    i1 = 176u;
    i0 += i1;
    l1 = i0;
  }
  i0 = l4;
  i1 = 21u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l1;
    i2 = 0u;
    i0 = CheckStatementLimits(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 21u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = 44u;
    i0 = i0 == i1;
    l1 = i0;
  }
  i0 = l1;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l1 = i0;
    loop_70: 
      i0 = l4;
      i1 = 22u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        StepRecordResume(ctx, i0);
        i0 = 22u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        i1 = 176u;
        i0 += i1;
        l7 = i0;
        i0 = state;
        i0 = LoadU32(ctx, i0 + 48u);
        l3 = i0;
      }
      i0 = l4;
      i1 = 23u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l3;
        i1 = l7;
        CheckExprComplexity(ctx, i0, i1);
        i0 = 23u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = l3;
      i1 = l6;
      i2 = 176u;
      i1 += i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      l3 = i0;
      i0 = l4;
      i1 = 24u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = l3;
        i2 = 0u;
        i0 = CheckStatementLimits(ctx, i0, i1, i2);
        l5 = i0;
        i0 = 24u;
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
        i0 = l1;
        i1 = 1u;
        i0 += i1;
        l1 = i0;
        i0 = state;
        i0 = LoadU32(ctx, i0 + 16u);
        i1 = 44u;
        i0 = i0 == i1;
        l3 = i0;
        if (i0) {goto loop_70;}
      }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 176u);
      i1 = 13u;
      i0 -= i1;
      i1 = 2u;
      i0 = i0 >= i1;
      state = i0;
      if (i0) {goto block_64;}
      i0 = l1;
      state = i0;
      goto block_66;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 1u;
    state = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 176u);
    i1 = 13u;
    i0 -= i1;
    i1 = 1u;
    i0 = i0 > i1;
    l1 = i0;
    if (i0) {goto block_65;}
  }
  block_66:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i1 = 176u;
    i0 += i1;
    l3 = i0;
    i0 = kF899ConstFFFFFFFF;
    l1 = i0;
  }
  i0 = l4;
  i1 = 25u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = l3;
    i2 = kF899ConstFFFFFFFF;
    AllocateExpressionRegister(ctx, i0, i1, i2);
    i0 = 25u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i1 = 1u;
    i0 = i0 != i1;
    state = i0;
    if (i0) {goto block_63;}
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 176u);
    i1 = 13u;
    i0 = i0 != i1;
    state = i0;
    if (i0) {goto block_63;}
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    i1 = l6;
    i1 = LoadU32(ctx, i1 + 184u);
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    state = i0;
    i0 = LoadU32(ctx, i0);
    i1 = kF899ConstFFFFFFC0;
    i0 &= i1;
    i1 = 29u;
    i0 |= i1;
    l3 = i0;
    i0 = state;
    i1 = l3;
    StoreU32(ctx, i0, i1);
    goto block_63;
  }
  block_65:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 1u;
    l1 = i0;
    i0 = l6;
    i1 = 176u;
    i0 += i1;
    state = i0;
  }
  i0 = l4;
  i1 = 26u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = state;
    i0 = EmitConditionJump(ctx, i0, i1);
    l5 = i0;
    i0 = 26u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l8 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_59;}
  block_64:;
  i0 = state;
  i1 = l6;
  i2 = 176u;
  i1 += i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  state = i0;
  i0 = l4;
  i1 = 27u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = state;
    CheckExprComplexity(ctx, i0, i1);
    i0 = 27u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_63:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i0 = LoadU8(ctx, i0 + 50u);
    l8 = i0;
  }
  block_59:;
  i0 = l4;
  i1 = 28u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = l8;
    i2 = l1;
    EmitJumpInstruction(ctx, i0, i1, i2);
    i0 = 28u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = 1u;
  i1 = ResumePhase(ctx);
  i1 = !(i1);
  if (i1) {goto running_exit;}
  block_11:;
  i0 = l4;
  i1 = 29u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    StepRecordResume(ctx, i0);
    i0 = 29u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    l3 = i0;
    i1 = 265u;
    i0 = i0 == i1;
    l2 = i0;
  }
  i0 = l2;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l4;
    i1 = 30u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      StepRecordResume(ctx, i0);
      i0 = 30u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 16u);
      i1 = 285u;
      i0 = i0 != i1;
      l2 = i0;
      i0 = state;
      i0 = LoadU32(ctx, i0 + 48u);
      l1 = i0;
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
        i0 = state;
        i0 = LoadU32(ctx, i0 + 52u);
        l2 = i0;
      }
      i0 = l4;
      i1 = 31u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = 285u;
        i0 = FormatCharValue(ctx, i0, i1);
        l5 = i0;
        i0 = 31u;
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
        i0 = l6;
        i1 = l3;
        StoreU32(ctx, i0 + 112u, i1);
        i0 = l6;
        i1 = 112u;
        i0 += i1;
        l3 = i0;
      }
      i0 = l4;
      i1 = 32u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = kF899MsgSExpected;
        i2 = l3;
        i0 = FormatPushString(ctx, i0, i1, i2);
        l5 = i0;
        i0 = 32u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l5;
        l2 = i0;
      }
      i0 = l4;
      i1 = 33u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = l2;
        RaiseSyntaxError(ctx, i0, i1);
        i0 = 33u;
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
      i0 = LoadU32(ctx, i0 + 24u);
      l2 = i0;
    }
    i0 = l4;
    i1 = 34u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      StepRecordResume(ctx, i0);
      i0 = 34u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = l4;
    i1 = 35u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l2;
      i2 = 0u;
      DeclareLocalVariable(ctx, i0, i1, i2);
      i0 = 35u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l1;
      i0 = LoadU32(ctx, i0 + 36u);
      l2 = i0;
      i0 = l6;
      j1 = 18446744073709551615ull;
      StoreU64(ctx, i0 + 192u, j1);
      i0 = l6;
      i1 = l2;
      StoreU32(ctx, i0 + 184u, i1);
      i0 = l6;
      i1 = 6u;
      StoreU32(ctx, i0 + 176u, i1);
    }
    i0 = l4;
    i1 = 36u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l1;
      i1 = 1u;
      ReserveRegisters(ctx, i0, i1);
      i0 = 36u;
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
      l2 = i0;
      i0 = LoadU8(ctx, i0 + 50u);
      i1 = 1u;
      i0 += i1;
      l3 = i0;
      i0 = l2;
      i1 = l3;
      StoreU8(ctx, i0 + 50u, i1);
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 24u);
      l7 = i0;
      i1 = l3;
      i2 = 255u;
      i1 &= i2;
      i2 = 1u;
      i1 <<= (i2 & 31);
      i2 = l2;
      i1 += i2;
      i1 = LoadU16(ctx, i1 + 170u);
      i2 = 12u;
      i1 *= i2;
      i0 += i1;
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 24u);
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l6;
      i1 = 152u;
      i0 += i1;
      l2 = i0;
      i0 = state;
      i0 = LoadU32(ctx, i0 + 4u);
      l3 = i0;
    }
    i0 = l4;
    i1 = 37u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l2;
      i2 = 0u;
      i3 = l3;
      ParseParamList(ctx, i0, i1, i2, i3);
      i0 = 37u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = 152u;
      i0 += i1;
      l2 = i0;
      i0 = l6;
      i1 = 176u;
      i0 += i1;
      state = i0;
    }
    i0 = l4;
    i1 = 38u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l1;
      i1 = state;
      i2 = l2;
      EmitVariableLoad(ctx, i0, i1, i2);
      i0 = 38u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 24u);
      i1 = l1;
      i1 = LoadU8(ctx, i1 + 50u);
      i2 = 1u;
      i1 <<= (i2 & 31);
      i2 = l1;
      i1 += i2;
      i1 = LoadU16(ctx, i1 + 170u);
      i2 = 12u;
      i1 *= i2;
      i0 += i1;
      i1 = l1;
      i1 = LoadU32(ctx, i1 + 24u);
      StoreU32(ctx, i0 + 4u, i1);
      i0 = 0u;
      goto running_exit;
    }
  }
  loop_108: 
    i0 = l2;
    i1 = l3;
    i2 = 285u;
    i1 = i1 != i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l2 = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = state;
        i0 = LoadU32(ctx, i0 + 52u);
        l2 = i0;
      }
      i0 = l4;
      i1 = 39u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = 285u;
        i0 = FormatCharValue(ctx, i0, i1);
        l5 = i0;
        i0 = 39u;
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
        i0 = l6;
        i1 = l3;
        StoreU32(ctx, i0 + 128u, i1);
        i0 = l6;
        i1 = 128u;
        i0 += i1;
        l3 = i0;
      }
      i0 = l4;
      i1 = 40u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = kF899MsgSExpected;
        i2 = l3;
        i0 = FormatPushString(ctx, i0, i1, i2);
        l5 = i0;
        i0 = 40u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l5;
        l2 = i0;
      }
      i0 = l4;
      i1 = 41u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = l2;
        RaiseSyntaxError(ctx, i0, i1);
        i0 = 41u;
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
      i0 = LoadU32(ctx, i0 + 24u);
      l2 = i0;
    }
    i0 = l4;
    i1 = 42u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      StepRecordResume(ctx, i0);
      i0 = 42u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = l4;
    i1 = 43u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l2;
      i2 = l1;
      DeclareLocalVariable(ctx, i0, i1, i2);
      i0 = 43u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 16u);
      l2 = i0;
      i1 = 44u;
      i0 = i0 == i1;
      l3 = i0;
      i0 = l1;
      i1 = 1u;
      i0 += i1;
      l1 = i0;
    }
    i0 = l3;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l4;
      i1 = 44u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        StepRecordResume(ctx, i0);
        i0 = 44u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = state;
        i0 = LoadU32(ctx, i0 + 16u);
        l3 = i0;
        goto loop_108;
      }
    }
  i0 = l2;
  i1 = l2;
  i2 = 61u;
  i1 = i1 == i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l2 = i0;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l4;
    i1 = 45u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      StepRecordResume(ctx, i0);
      i0 = 45u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = l2;
    i1 = l6;
    i2 = 176u;
    i1 += i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l2 = i0;
    i0 = l4;
    i1 = 46u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l2;
      i2 = 0u;
      i0 = CheckStatementLimits(ctx, i0, i1, i2);
      l5 = i0;
      i0 = 46u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = 1u;
      l3 = i0;
      i0 = state;
      i0 = LoadU32(ctx, i0 + 16u);
      i1 = 44u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_122;}
    }
    loop_127: 
      i0 = l4;
      i1 = 47u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        StepRecordResume(ctx, i0);
        i0 = 47u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        i1 = 176u;
        i0 += i1;
        l7 = i0;
        i0 = state;
        i0 = LoadU32(ctx, i0 + 48u);
        l2 = i0;
      }
      i0 = l4;
      i1 = 48u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l7;
        CheckExprComplexity(ctx, i0, i1);
        i0 = 48u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = l2;
      i1 = l6;
      i2 = 176u;
      i1 += i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      l2 = i0;
      i0 = l4;
      i1 = 49u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = l2;
        i2 = 0u;
        i0 = CheckStatementLimits(ctx, i0, i1, i2);
        l5 = i0;
        i0 = 49u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l5;
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i1 = 1u;
        i0 += i1;
        l3 = i0;
        i0 = state;
        i0 = LoadU32(ctx, i0 + 16u);
        i1 = 44u;
        i0 = i0 == i1;
        l2 = i0;
        if (i0) {goto loop_127;}
      }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_122;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i1 = 0u;
    StoreU32(ctx, i0 + 176u, i1);
    i0 = 0u;
    l3 = i0;
  }
  block_122:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i1 = 176u;
    i0 += i1;
    l7 = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    l2 = i0;
  }
  i0 = l4;
  i1 = 50u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = l1;
    i2 = l3;
    i3 = l7;
    ReleaseExpressionSlots(ctx, i0, i1, i2, i3);
    i0 = 50u;
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
    state = i0;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = l1;
    i0 += i1;
    l2 = i0;
    i0 = state;
    i1 = l2;
    StoreU8(ctx, i0 + 50u, i1);
    i0 = l2;
    i1 = 255u;
    i0 &= i1;
    l2 = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 24u);
    l3 = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 24u);
    l7 = i0;
    loop_137: 
      i0 = l2;
      i1 = l1;
      i0 -= i1;
      i1 = 1u;
      i0 <<= (i1 & 31);
      i1 = state;
      i0 += i1;
      i0 = LoadU16(ctx, i0 + 172u);
      i1 = 12u;
      i0 *= i1;
      i1 = l7;
      i0 += i1;
      i1 = l3;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l1;
      i1 = 1u;
      i0 -= i1;
      l1 = i0;
      if (i0) {goto loop_137;}
    goto block_5;
  }
  block_10:;
  i0 = l4;
  i1 = 51u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    StepRecordResume(ctx, i0);
    i0 = 51u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = 285u;
    i0 = i0 != i1;
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
      i0 = state;
      i0 = LoadU32(ctx, i0 + 52u);
      l1 = i0;
    }
    i0 = l4;
    i1 = 52u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = 285u;
      i0 = FormatCharValue(ctx, i0, i1);
      l5 = i0;
      i0 = 52u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = l2;
      StoreU32(ctx, i0 + 96u, i1);
      i0 = l6;
      i1 = 96u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l4;
    i1 = 53u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l1;
      i1 = kF899MsgSExpected;
      i2 = l2;
      i0 = FormatPushString(ctx, i0, i1, i2);
      l5 = i0;
      i0 = 53u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l1 = i0;
    }
    i0 = l4;
    i1 = 54u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l1;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 54u;
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
    i0 = LoadU32(ctx, i0 + 24u);
    l1 = i0;
  }
  i0 = l4;
  i1 = 55u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    StepRecordResume(ctx, i0);
    i0 = 55u;
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
    l2 = i0;
    i0 = l6;
    i1 = 176u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l4;
  i1 = 56u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = l1;
    i2 = l3;
    i3 = 1u;
    i0 = AppendWithLimit(ctx, i0, i1, i2, i3);
    l5 = i0;
    i0 = 56u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l3 = i0;
  }
  i0 = l3;
  i1 = l3;
  i2 = 8u;
  i1 = i1 == i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l3 = i0;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l4;
    i1 = 57u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = l1;
      i0 = EncodeConstant(ctx, i0, i1);
      l5 = i0;
      i0 = 57u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l1 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = l1;
      StoreU32(ctx, i0 + 184u, i1);
    }
  }
  loop_155: 
    i0 = l15;
    i1 = ResumePhase(ctx);
    i1 = !(i1);
    if (i1) {
      i1 = state;
      i1 = LoadU32(ctx, i1 + 16u);
      l1 = i1;
      i2 = 46u;
      i1 = i1 != i2;
      l2 = i1;
    }
    i1 = l2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l15 = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l13;
      i1 = ResumePhase(ctx);
      i1 = !(i1);
      if (i1) {
        i1 = state;
        l2 = i1;
        i1 = l6;
        i2 = 152u;
        i1 += i2;
        l3 = i1;
        i1 = l1;
        i2 = 58u;
        i1 = i1 == i2;
        l1 = i1;
      }
      i1 = l1;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      l13 = i0;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = l1;
        i1 = l6;
        i2 = 176u;
        i1 += i2;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        l1 = i0;
        i0 = l4;
        i1 = 58u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = state;
          i1 = l1;
          ExpectToken(ctx, i0, i1);
          i0 = 58u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = l1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        l1 = i0;
      }
      i0 = l1;
      i1 = 0u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      i1 = l1;
      i2 = l13;
      i2 = !(i2);
      i3 = ResumePhase(ctx);
      i4 = 2u;
      i3 = i3 == i4;
      i2 |= i3;
      i0 = i2 ? i0 : i1;
      l1 = i0;
      i0 = l4;
      i1 = 59u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l3;
        i2 = l1;
        i3 = l9;
        ParseParamList(ctx, i0, i1, i2, i3);
        i0 = 59u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        i1 = 176u;
        i0 += i1;
        l2 = i0;
        i0 = l6;
        i1 = 152u;
        i0 += i1;
        l3 = i0;
        i0 = state;
        i0 = LoadU32(ctx, i0 + 48u);
        l1 = i0;
      }
      i0 = l4;
      i1 = 60u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l1;
        i1 = l2;
        i2 = l3;
        EmitVariableLoad(ctx, i0, i1, i2);
        i0 = 60u;
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
        state = i0;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 20u);
        l1 = i0;
        i0 = l1;
        i1 = state;
        i1 = LoadU32(ctx, i1 + 24u);
        i2 = 2u;
        i1 <<= (i2 & 31);
        i0 += i1;
        i1 = kF899ResumeWordBytes;
        i0 -= i1;
        state = i0;
        i1 = l9;
        StoreU32(ctx, i0, i1);
        goto block_5;
      }
    }
    i0 = l15;
    i0 = !(i0);
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l1;
      i1 = l6;
      i2 = 176u;
      i1 += i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      l1 = i0;
      i0 = l4;
      i1 = 61u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = l1;
        ExpectToken(ctx, i0, i1);
        i0 = 61u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto loop_155;}
    }
  block_9:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    l7 = i0;
    i0 = LoadU32(ctx, i0 + 24u);
    l1 = i0;
    i0 = l7;
    i1 = l1;
    StoreU32(ctx, i0 + 28u, i1);
    i0 = l1;
    l8 = i0;
    i0 = l6;
    i1 = kF899ConstFFFFFFFF;
    StoreU32(ctx, i0 + 156u, i1);
    i0 = l6;
    i1 = 1u;
    StoreU8(ctx, i0 + 162u, i1);
    i0 = l7;
    i0 = LoadU8(ctx, i0 + 50u);
    l1 = i0;
    i0 = l6;
    i1 = 0u;
    StoreU8(ctx, i0 + 161u, i1);
    i0 = l6;
    i1 = l1;
    StoreU8(ctx, i0 + 160u, i1);
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 20u);
    l2 = i0;
    i0 = l6;
    i1 = kF899ConstFFFFFFFF;
    StoreU32(ctx, i0 + 140u, i1);
    i0 = l6;
    i1 = l2;
    StoreU32(ctx, i0 + 152u, i1);
    i0 = l6;
    i1 = 0u;
    StoreU16(ctx, i0 + 145u, i1);
    i0 = l6;
    i1 = l1;
    StoreU8(ctx, i0 + 144u, i1);
    i0 = l6;
    i1 = l6;
    i2 = 152u;
    i1 += i2;
    StoreU32(ctx, i0 + 136u, i1);
    i0 = l7;
    i1 = l6;
    i2 = 136u;
    i1 += i2;
    l1 = i1;
    StoreU32(ctx, i0 + 20u, i1);
  }
  i0 = l4;
  i1 = 62u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    StepRecordResume(ctx, i0);
    i0 = 62u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 52u);
    l1 = i0;
    l2 = i0;
    i1 = l1;
    i1 = LoadU16(ctx, i1 + 52u);
    i2 = 1u;
    i1 += i2;
    l1 = i1;
    StoreU16(ctx, i0 + 52u, i1);
    i0 = l1;
    i1 = 65535u;
    i0 &= i1;
    i1 = 201u;
    i0 = i0 >= i1;
    l1 = i0;
  }
  i0 = l1;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l4;
    i1 = 63u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = kF899MsgChunkHasTooMany;
      i2 = 0u;
      LexError(ctx, i0, i1, i2);
      i0 = 63u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
  }
  loop_176: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 16u);
      i1 = 260u;
      i0 -= i1;
      l1 = i0;
      i1 = 27u;
      i0 = i0 <= i1;
      l2 = i0;
      i1 = 0u;
      i2 = 1u;
      i3 = l1;
      i2 <<= (i3 & 31);
      i3 = kF899Const8010007;
      i2 &= i3;
      i0 = i2 ? i0 : i1;
      i0 = !(i0);
      l1 = i0;
    }
    i0 = l1;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l4;
      i1 = 64u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i0 = CompileStatement(ctx, i0);
        l5 = i0;
        i0 = 64u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l5;
        l1 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = state;
        i0 = LoadU32(ctx, i0 + 16u);
        i1 = 59u;
        i0 = i0 == i1;
        l2 = i0;
      }
      i0 = l2;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = l4;
        i1 = 65u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = state;
          StepRecordResume(ctx, i0);
          i0 = 65u;
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
        i0 = LoadU32(ctx, i0 + 48u);
        l2 = i0;
        i0 = LoadU8(ctx, i0 + 50u);
        l10 = i0;
        i0 = l2;
        i1 = l10;
        StoreU32(ctx, i0 + 36u, i1);
        i0 = l1;
        i0 = !(i0);
        l1 = i0;
        if (i0) {goto loop_176;}
      }
    }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 52u);
    l1 = i0;
    i0 = LoadU16(ctx, i0 + 52u);
    i1 = 1u;
    i0 -= i1;
    l2 = i0;
    i0 = l1;
    i1 = l2;
    StoreU16(ctx, i0 + 52u, i1);
  }
  i0 = l4;
  i1 = 66u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = 276u;
    i2 = 272u;
    i3 = l9;
    CheckClosingMatch(ctx, i0, i1, i2, i3);
    i0 = 66u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l1;
  i1 = l6;
  i2 = 176u;
  i1 += i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l1 = i0;
  i0 = l4;
  i1 = 67u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l1;
    i2 = 0u;
    i0 = CheckStatementLimits(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 67u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 176u);
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {
      i0 = l6;
      i1 = 3u;
      StoreU32(ctx, i0 + 176u, i1);
    }
    i0 = l6;
    i1 = 176u;
    i0 += i1;
    l2 = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    l1 = i0;
  }
  i0 = l4;
  i1 = 68u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l1;
    i1 = l2;
    EmitVariableStore(ctx, i0, i1);
    i0 = 68u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 196u);
    l10 = i0;
    i0 = l6;
    i0 = LoadU8(ctx, i0 + 145u);
    i0 = !(i0);
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
      i0 = l7;
      i1 = l7;
      i1 = LoadU32(ctx, i1 + 20u);
      l3 = i1;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0 + 20u, i1);
      i0 = l7;
      i0 = LoadU32(ctx, i0 + 12u);
      i0 = LoadU32(ctx, i0 + 48u);
      l2 = i0;
      i0 = LoadU8(ctx, i0 + 50u);
      l1 = i0;
      i1 = l3;
      i1 = LoadU8(ctx, i1 + 8u);
      l9 = i1;
      i0 = i0 > i1;
      l11 = i0;
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 24u);
        l11 = i0;
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 24u);
        l12 = i0;
        loop_199: 
          i0 = l1;
          i1 = 1u;
          i0 -= i1;
          l1 = i0;
          i1 = 255u;
          i0 &= i1;
          l13 = i0;
          i1 = 1u;
          i0 <<= (i1 & 31);
          i1 = l2;
          i0 += i1;
          i0 = LoadU16(ctx, i0 + 172u);
          i1 = 12u;
          i0 *= i1;
          i1 = l12;
          i0 += i1;
          i1 = l11;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l9;
          i1 = l13;
          i0 = i0 < i1;
          if (i0) {goto loop_199;}
        i0 = l2;
        i1 = l9;
        StoreU8(ctx, i0 + 50u, i1);
      }
      i0 = l3;
      i0 = LoadU8(ctx, i0 + 9u);
      l1 = i0;
    }
    i0 = l1;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l4;
      i1 = 69u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l7;
        i1 = 35u;
        i2 = l9;
        i3 = 0u;
        i4 = 0u;
        i0 = EmitOpInstruction(ctx, i0, i1, i2, i3, i4);
        l5 = i0;
        i0 = 69u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l5;
        l1 = i0;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l7;
      i1 = l7;
      i1 = LoadU8(ctx, i1 + 50u);
      StoreU32(ctx, i0 + 36u, i1);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 4u);
      l1 = i0;
    }
    i0 = l4;
    i1 = 70u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l7;
      i1 = l1;
      PatchJumpChain(ctx, i0, i1);
      i0 = 70u;
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
      state = i0;
    }
    i0 = l4;
    i1 = 71u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l10;
      i2 = l8;
      PatchComparisonIntoJump(ctx, i0, i1, i2);
      i0 = 71u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_192;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    l2 = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    l9 = i0;
    i0 = LoadU32(ctx, i0 + 20u);
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    if (i0) {
      loop_210: 
        i0 = l1;
        i0 = LoadU8(ctx, i0 + 10u);
        l11 = i0;
        if (i0) {
          i0 = l1;
          l2 = i0;
          goto block_207;
        }
        i0 = l1;
        i0 = LoadU8(ctx, i0 + 9u);
        l11 = i0;
        i1 = l3;
        i0 |= i1;
        l3 = i0;
        i0 = l1;
        i0 = LoadU32(ctx, i0);
        l1 = i0;
        if (i0) {goto loop_210;}
    }
  }
  i0 = l4;
  i1 = 72u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = kF899MsgNoLoopToBreak;
    RaiseSyntaxError(ctx, i0, i1);
    i0 = 72u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_207:;
  i0 = l3;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i0 = LoadU8(ctx, i0 + 8u);
      l1 = i0;
    }
    i0 = l4;
    i1 = 73u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l9;
      i1 = 35u;
      i2 = l1;
      i3 = 0u;
      i4 = 0u;
      i0 = EmitOpInstruction(ctx, i0, i1, i2, i3, i4);
      l5 = i0;
      i0 = 73u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
    } else {
      i0 = l1;
    }
  } else {
    i0 = l1;
  }
  i1 = l2;
  i2 = 4u;
  i1 += i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l1 = i0;
  i0 = l4;
  i1 = 74u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l9;
    i0 = ConcatJumpLists(ctx, i0);
    l5 = i0;
    i0 = 74u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l2 = i0;
  }
  i0 = l4;
  i1 = 75u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l9;
    i1 = l1;
    i2 = l2;
    FixPendingJumpTargets(ctx, i0, i1, i2);
    i0 = 75u;
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
    l1 = i0;
  }
  i0 = l4;
  i1 = 76u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l1;
    i1 = l10;
    PatchJumpChain(ctx, i0, i1);
    i0 = 76u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    i1 = l7;
    i1 = LoadU32(ctx, i1 + 20u);
    l3 = i1;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 20u, i1);
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 12u);
    i0 = LoadU32(ctx, i0 + 48u);
    l2 = i0;
    i0 = LoadU8(ctx, i0 + 50u);
    l1 = i0;
    i1 = l3;
    i1 = LoadU8(ctx, i1 + 8u);
    l9 = i1;
    i0 = i0 > i1;
    l10 = i0;
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 24u);
      l10 = i0;
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 24u);
      l11 = i0;
      loop_223: 
        i0 = l1;
        i1 = 1u;
        i0 -= i1;
        l1 = i0;
        i1 = 255u;
        i0 &= i1;
        l12 = i0;
        i1 = 1u;
        i0 <<= (i1 & 31);
        i1 = l2;
        i0 += i1;
        i0 = LoadU16(ctx, i0 + 172u);
        i1 = 12u;
        i0 *= i1;
        i1 = l11;
        i0 += i1;
        i1 = l10;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l9;
        i1 = l12;
        i0 = i0 < i1;
        l12 = i0;
        if (i0) {goto loop_223;}
      i0 = l2;
      i1 = l9;
      StoreU8(ctx, i0 + 50u, i1);
    }
    i0 = l3;
    i0 = LoadU8(ctx, i0 + 9u);
    l1 = i0;
  }
  i0 = l1;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l4;
    i1 = 77u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l7;
      i1 = 35u;
      i2 = l9;
      i3 = 0u;
      i4 = 0u;
      i0 = EmitOpInstruction(ctx, i0, i1, i2, i3, i4);
      l5 = i0;
      i0 = 77u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l1 = i0;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    i1 = l7;
    i1 = LoadU8(ctx, i1 + 50u);
    StoreU32(ctx, i0 + 36u, i1);
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    l1 = i0;
  }
  i0 = l4;
  i1 = 78u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l7;
    i1 = l1;
    PatchJumpChain(ctx, i0, i1);
    i0 = 78u;
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
    state = i0;
  }
  i0 = l4;
  i1 = 79u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l7;
    i0 = ConcatJumpLists(ctx, i0);
    l5 = i0;
    i0 = 79u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l1 = i0;
  }
  i0 = l4;
  i1 = 80u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l1;
    i2 = l8;
    PatchComparisonIntoJump(ctx, i0, i1, i2);
    i0 = 80u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_192:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    i1 = l7;
    i1 = LoadU32(ctx, i1 + 20u);
    l2 = i1;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 20u, i1);
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 12u);
    i0 = LoadU32(ctx, i0 + 48u);
    l1 = i0;
    i0 = LoadU8(ctx, i0 + 50u);
    state = i0;
    i1 = l2;
    i1 = LoadU8(ctx, i1 + 8u);
    l3 = i1;
    i0 = i0 > i1;
    l8 = i0;
    if (i0) {
      i0 = l1;
      i0 = LoadU32(ctx, i0 + 24u);
      l9 = i0;
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 24u);
      l8 = i0;
      loop_234: 
        i0 = state;
        i1 = 1u;
        i0 -= i1;
        state = i0;
        i1 = 255u;
        i0 &= i1;
        l10 = i0;
        i1 = 1u;
        i0 <<= (i1 & 31);
        i1 = l1;
        i0 += i1;
        i0 = LoadU16(ctx, i0 + 172u);
        i1 = 12u;
        i0 *= i1;
        i1 = l8;
        i0 += i1;
        l11 = i0;
        i1 = l9;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l3;
        i1 = l10;
        i0 = i0 < i1;
        l10 = i0;
        if (i0) {goto loop_234;}
      i0 = l1;
      i1 = l3;
      StoreU8(ctx, i0 + 50u, i1);
    }
    i0 = l2;
    i0 = LoadU8(ctx, i0 + 9u);
    state = i0;
  }
  i0 = state;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l4;
    i1 = 81u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l7;
      i1 = 35u;
      i2 = l3;
      i3 = 0u;
      i4 = 0u;
      i0 = EmitOpInstruction(ctx, i0, i1, i2, i3, i4);
      l5 = i0;
      i0 = 81u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      state = i0;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    i1 = l7;
    i1 = LoadU8(ctx, i1 + 50u);
    StoreU32(ctx, i0 + 36u, i1);
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 4u);
    state = i0;
  }
  i0 = l4;
  i1 = 82u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l7;
    i1 = state;
    PatchJumpChain(ctx, i0, i1);
    i0 = 82u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_5;}
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    l7 = i0;
    i0 = l6;
    i1 = 1u;
    StoreU8(ctx, i0 + 162u, i1);
    i0 = l6;
    i1 = kF899ConstFFFFFFFF;
    StoreU32(ctx, i0 + 156u, i1);
    i0 = l7;
    i0 = LoadU8(ctx, i0 + 50u);
    l1 = i0;
    i0 = l6;
    i1 = 0u;
    StoreU8(ctx, i0 + 161u, i1);
    i0 = l6;
    i1 = l1;
    StoreU8(ctx, i0 + 160u, i1);
    i0 = l6;
    i1 = l7;
    i1 = LoadU32(ctx, i1 + 20u);
    StoreU32(ctx, i0 + 152u, i1);
    i0 = l7;
    i1 = l6;
    i2 = 152u;
    i1 += i2;
    l1 = i1;
    StoreU32(ctx, i0 + 20u, i1);
  }
  i0 = l4;
  i1 = 83u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    StepRecordResume(ctx, i0);
    i0 = 83u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = 285u;
    i0 = i0 != i1;
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
      i0 = state;
      i0 = LoadU32(ctx, i0 + 52u);
      l1 = i0;
    }
    i0 = l4;
    i1 = 84u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = 285u;
      i0 = FormatCharValue(ctx, i0, i1);
      l5 = i0;
      i0 = 84u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = l2;
      StoreU32(ctx, i0 + 80u, i1);
      i0 = l6;
      i1 = 80u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l4;
    i1 = 85u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l1;
      i1 = kF899MsgSExpected;
      i2 = l2;
      i0 = FormatPushString(ctx, i0, i1, i2);
      l5 = i0;
      i0 = 85u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l1 = i0;
    }
    i0 = l4;
    i1 = 86u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l1;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 86u;
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
    i0 = LoadU32(ctx, i0 + 24u);
    l3 = i0;
  }
  i0 = l4;
  i1 = 87u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    StepRecordResume(ctx, i0);
    i0 = 87u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    l1 = i0;
    i1 = 44u;
    i0 -= i1;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    switch (i0) {
      case 0: goto block_253;
      case 1: goto block_254;
      case 2: goto block_254;
      case 3: goto block_254;
      case 4: goto block_254;
      case 5: goto block_254;
      case 6: goto block_254;
      case 7: goto block_254;
      case 8: goto block_254;
      case 9: goto block_254;
      case 10: goto block_254;
      case 11: goto block_254;
      case 12: goto block_254;
      case 13: goto block_254;
      case 14: goto block_254;
      case 15: goto block_254;
      case 16: goto block_254;
      case 17: goto block_257;
      default: goto block_255;
    }
    block_257:;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    l1 = i0;
    i0 = LoadU32(ctx, i0 + 36u);
    l2 = i0;
  }
  i0 = l4;
  i1 = 88u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = kF899MsgForIndex;
    i2 = 11u;
    i0 = BindUpvalueCell(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 88u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l8 = i0;
  }
  i0 = l4;
  i1 = 89u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l8;
    i2 = 0u;
    DeclareLocalVariable(ctx, i0, i1, i2);
    i0 = 89u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l4;
  i1 = 90u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = kF899MsgForLimit;
    i2 = 11u;
    i0 = BindUpvalueCell(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 90u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l8 = i0;
  }
  i0 = l4;
  i1 = 91u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l8;
    i2 = 1u;
    DeclareLocalVariable(ctx, i0, i1, i2);
    i0 = 91u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l4;
  i1 = 92u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = kF899MsgForStep;
    i2 = 10u;
    i0 = BindUpvalueCell(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 92u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l8 = i0;
  }
  i0 = l4;
  i1 = 93u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l8;
    i2 = 2u;
    DeclareLocalVariable(ctx, i0, i1, i2);
    i0 = 93u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l4;
  i1 = 94u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l3;
    i2 = 3u;
    DeclareLocalVariable(ctx, i0, i1, i2);
    i0 = 94u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = 61u;
    i0 = i0 != i1;
    l3 = i0;
  }
  i0 = l3;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 52u);
      l3 = i0;
    }
    i0 = l4;
    i1 = 95u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = 61u;
      i0 = FormatCharValue(ctx, i0, i1);
      l5 = i0;
      i0 = 95u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l8 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = l8;
      StoreU32(ctx, i0 + 32u, i1);
      i0 = l6;
      i1 = 32u;
      i0 += i1;
      l8 = i0;
    }
    i0 = l4;
    i1 = 96u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l3;
      i1 = kF899MsgSExpected;
      i2 = l8;
      i0 = FormatPushString(ctx, i0, i1, i2);
      l5 = i0;
      i0 = 96u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l3 = i0;
    }
    i0 = l4;
    i1 = 97u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l3;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 97u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
  }
  i0 = l4;
  i1 = 98u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    StepRecordResume(ctx, i0);
    i0 = 98u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l3;
  i1 = l6;
  i2 = 176u;
  i1 += i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l3 = i0;
  i0 = l4;
  i1 = 99u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l3;
    i2 = 0u;
    i0 = CheckStatementLimits(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 99u;
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
    i0 = l6;
    i1 = 176u;
    i0 += i1;
    l8 = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    l3 = i0;
  }
  i0 = l4;
  i1 = 100u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l3;
    i1 = l8;
    CheckExprComplexity(ctx, i0, i1);
    i0 = 100u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = 44u;
    i0 = i0 != i1;
    l3 = i0;
  }
  i0 = l3;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 52u);
      l3 = i0;
    }
    i0 = l4;
    i1 = 101u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = 44u;
      i0 = FormatCharValue(ctx, i0, i1);
      l5 = i0;
      i0 = 101u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l8 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = l8;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l6;
      i1 = 16u;
      i0 += i1;
      l8 = i0;
    }
    i0 = l4;
    i1 = 102u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l3;
      i1 = kF899MsgSExpected;
      i2 = l8;
      i0 = FormatPushString(ctx, i0, i1, i2);
      l5 = i0;
      i0 = 102u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l3 = i0;
    }
    i0 = l4;
    i1 = 103u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l3;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 103u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
  }
  i0 = l4;
  i1 = 104u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    StepRecordResume(ctx, i0);
    i0 = 104u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l3;
  i1 = l6;
  i2 = 176u;
  i1 += i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l3 = i0;
  i0 = l4;
  i1 = 105u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l3;
    i2 = 0u;
    i0 = CheckStatementLimits(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 105u;
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
    i0 = l6;
    i1 = 176u;
    i0 += i1;
    l8 = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    l3 = i0;
  }
  i0 = l4;
  i1 = 106u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l3;
    i1 = l8;
    CheckExprComplexity(ctx, i0, i1);
    i0 = 106u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = 44u;
    i0 = i0 == i1;
    l3 = i0;
  }
  i0 = l3;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l4;
    i1 = 107u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      StepRecordResume(ctx, i0);
      i0 = 107u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = l1;
    i1 = l6;
    i2 = 176u;
    i1 += i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l1 = i0;
    i0 = l4;
    i1 = 108u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l1;
      i2 = 0u;
      i0 = CheckStatementLimits(ctx, i0, i1, i2);
      l5 = i0;
      i0 = 108u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l1 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = 176u;
      i0 += i1;
      l3 = i0;
      i0 = state;
      i0 = LoadU32(ctx, i0 + 48u);
      l1 = i0;
    }
    i0 = l4;
    i1 = 109u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l1;
      i1 = l3;
      CheckExprComplexity(ctx, i0, i1);
      i0 = 109u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_289;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 36u);
    l3 = i0;
  }
  i0 = l4;
  i1 = 110u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l1;
    i0 = AddOneConstant(ctx, i0);
    l5 = i0;
    i0 = 110u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l8 = i0;
  }
  i0 = l4;
  i1 = 111u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l1;
    i1 = 1u;
    i2 = l3;
    i3 = l8;
    i0 = EmitInstructionABC(ctx, i0, i1, i2, i3);
    l5 = i0;
    i0 = 111u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l3 = i0;
  }
  i0 = l4;
  i1 = 112u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l1;
    i1 = 1u;
    ReserveRegisters(ctx, i0, i1);
    i0 = 112u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_289:;
  i0 = l4;
  i1 = 113u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l2;
    i2 = l9;
    i3 = 1u;
    i4 = 1u;
    ParseExpression(ctx, i0, i1, i2, i3, i4);
    i0 = 113u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_252;}
  block_255:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i1 = 267u;
    i0 = i0 == i1;
    l1 = i0;
    if (i0) {goto block_253;}
  }
  block_254:;
  i0 = l4;
  i1 = 114u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = kF899MsgOrInExpected;
    RaiseSyntaxError(ctx, i0, i1);
    i0 = 114u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_252;}
  block_253:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    l10 = i0;
    i0 = LoadU32(ctx, i0 + 36u);
    l11 = i0;
  }
  i0 = l4;
  i1 = 115u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = kF899MsgForGenerator;
    i2 = 15u;
    i0 = BindUpvalueCell(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 115u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l1 = i0;
  }
  i0 = l4;
  i1 = 116u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l1;
    i2 = 0u;
    DeclareLocalVariable(ctx, i0, i1, i2);
    i0 = 116u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l2;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l2 = i0;
  i0 = l4;
  i1 = 117u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = kF899MsgForState;
    i2 = 11u;
    i0 = BindUpvalueCell(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 117u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l1 = i0;
  }
  i0 = l4;
  i1 = 118u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l1;
    i2 = 1u;
    DeclareLocalVariable(ctx, i0, i1, i2);
    i0 = 118u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l4;
  i1 = 119u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = kF899MsgForControl;
    i2 = 13u;
    i0 = BindUpvalueCell(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 119u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l1 = i0;
  }
  i0 = l4;
  i1 = 120u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l1;
    i2 = 2u;
    DeclareLocalVariable(ctx, i0, i1, i2);
    i0 = 120u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l4;
  i1 = 121u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l3;
    i2 = 3u;
    DeclareLocalVariable(ctx, i0, i1, i2);
    i0 = 121u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    l8 = i0;
    i1 = 44u;
    i0 = i0 == i1;
    l1 = i0;
  }
  i0 = l1;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l3;
    i1 = 4u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l3 = i0;
    loop_315: 
      i0 = l1;
      i1 = l3;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      l1 = i0;
      i0 = l4;
      i1 = 122u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        StepRecordResume(ctx, i0);
        i0 = 122u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = state;
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
          i0 = state;
          i0 = LoadU32(ctx, i0 + 52u);
          l2 = i0;
        }
        i0 = l4;
        i1 = 123u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = state;
          i1 = 285u;
          i0 = FormatCharValue(ctx, i0, i1);
          l5 = i0;
          i0 = 123u;
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
          i0 = l6;
          i1 = l3;
          StoreU32(ctx, i0 + 64u, i1);
          i0 = l6;
          i1 = kF899ConstFFFFFFC0;
          i0 -= i1;
          l3 = i0;
        }
        i0 = l4;
        i1 = 124u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l2;
          i1 = kF899MsgSExpected;
          i2 = l3;
          i0 = FormatPushString(ctx, i0, i1, i2);
          l5 = i0;
          i0 = 124u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l5;
          l2 = i0;
        }
        i0 = l4;
        i1 = 125u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = state;
          i1 = l2;
          RaiseSyntaxError(ctx, i0, i1);
          i0 = 125u;
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
        i0 = LoadU32(ctx, i0 + 24u);
        l2 = i0;
      }
      i0 = l4;
      i1 = 126u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        StepRecordResume(ctx, i0);
        i0 = 126u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = l4;
      i1 = 127u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = l2;
        i2 = l1;
        DeclareLocalVariable(ctx, i0, i1, i2);
        i0 = 127u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l1;
        i1 = 1u;
        i0 += i1;
        l3 = i0;
        i0 = state;
        i0 = LoadU32(ctx, i0 + 16u);
        l8 = i0;
        i1 = 44u;
        i0 = i0 == i1;
        l2 = i0;
        if (i0) {goto loop_315;}
      }
    i0 = l2;
    i1 = l1;
    i2 = 2u;
    i1 -= i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l2 = i0;
  }
  i0 = l1;
  i1 = l8;
  i2 = 267u;
  i1 = i1 != i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l1 = i0;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 52u);
      l1 = i0;
    }
    i0 = l4;
    i1 = 128u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = 267u;
      i0 = FormatCharValue(ctx, i0, i1);
      l5 = i0;
      i0 = 128u;
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
      i0 = l6;
      i1 = l3;
      StoreU32(ctx, i0 + 48u, i1);
      i0 = l6;
      i1 = 48u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l4;
    i1 = 129u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l1;
      i1 = kF899MsgSExpected;
      i2 = l3;
      i0 = FormatPushString(ctx, i0, i1, i2);
      l5 = i0;
      i0 = 129u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l1 = i0;
    }
    i0 = l4;
    i1 = 130u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l1;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 130u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
  }
  i0 = l4;
  i1 = 131u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    StepRecordResume(ctx, i0);
    i0 = 131u;
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
    l3 = i0;
    i0 = l6;
    i1 = 176u;
    i0 += i1;
    l1 = i0;
  }
  i0 = l4;
  i1 = 132u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l1;
    i2 = 0u;
    i0 = CheckStatementLimits(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 132u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = 44u;
    i0 = i0 == i1;
    l8 = i0;
    i0 = 1u;
    l1 = i0;
  }
  i0 = l8;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    loop_340: 
      i0 = l4;
      i1 = 133u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        StepRecordResume(ctx, i0);
        i0 = 133u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        i1 = 176u;
        i0 += i1;
        l12 = i0;
        i0 = state;
        i0 = LoadU32(ctx, i0 + 48u);
        l8 = i0;
      }
      i0 = l4;
      i1 = 134u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l8;
        i1 = l12;
        CheckExprComplexity(ctx, i0, i1);
        i0 = 134u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = l8;
      i1 = l6;
      i2 = 176u;
      i1 += i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      l8 = i0;
      i0 = l4;
      i1 = 135u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = l8;
        i2 = 0u;
        i0 = CheckStatementLimits(ctx, i0, i1, i2);
        l5 = i0;
        i0 = 135u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l5;
        l8 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l1;
        i1 = 1u;
        i0 += i1;
        l1 = i0;
        i0 = state;
        i0 = LoadU32(ctx, i0 + 16u);
        i1 = 44u;
        i0 = i0 == i1;
        l8 = i0;
        if (i0) {goto loop_340;}
      }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i1 = 176u;
    i0 += i1;
    l12 = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    l8 = i0;
  }
  i0 = l4;
  i1 = 136u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l8;
    i1 = 3u;
    i2 = l1;
    i3 = l12;
    ReleaseExpressionSlots(ctx, i0, i1, i2, i3);
    i0 = 136u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l4;
  i1 = 137u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l10;
    ReserveThreeRegisters(ctx, i0);
    i0 = 137u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l4;
  i1 = 138u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l11;
    i2 = l3;
    i3 = l2;
    i4 = 0u;
    ParseExpression(ctx, i0, i1, i2, i3, i4);
    i0 = 138u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_252:;
  i0 = l4;
  i1 = 139u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = 262u;
    i2 = 264u;
    i3 = l9;
    CheckClosingMatch(ctx, i0, i1, i2, i3);
    i0 = 139u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    i1 = l7;
    i1 = LoadU32(ctx, i1 + 20u);
    l2 = i1;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 20u, i1);
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 12u);
    i0 = LoadU32(ctx, i0 + 48u);
    l1 = i0;
    i0 = LoadU8(ctx, i0 + 50u);
    state = i0;
    i1 = l2;
    i1 = LoadU8(ctx, i1 + 8u);
    l3 = i1;
    i0 = i0 > i1;
    l8 = i0;
    if (i0) {
      i0 = l1;
      i0 = LoadU32(ctx, i0 + 24u);
      l9 = i0;
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 24u);
      l8 = i0;
      loop_354: 
        i0 = state;
        i1 = 1u;
        i0 -= i1;
        state = i0;
        i1 = 255u;
        i0 &= i1;
        l10 = i0;
        i1 = 1u;
        i0 <<= (i1 & 31);
        i1 = l1;
        i0 += i1;
        i0 = LoadU16(ctx, i0 + 172u);
        i1 = 12u;
        i0 *= i1;
        i1 = l8;
        i0 += i1;
        i1 = l9;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l3;
        i1 = l10;
        i0 = i0 < i1;
        if (i0) {goto loop_354;}
      i0 = l1;
      i1 = l3;
      StoreU8(ctx, i0 + 50u, i1);
    }
    i0 = l2;
    i0 = LoadU8(ctx, i0 + 9u);
    state = i0;
  }
  i0 = state;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l4;
    i1 = 140u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l7;
      i1 = 35u;
      i2 = l3;
      i3 = 0u;
      i4 = 0u;
      i0 = EmitOpInstruction(ctx, i0, i1, i2, i3, i4);
      l5 = i0;
      i0 = 140u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      state = i0;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    i1 = l7;
    i1 = LoadU8(ctx, i1 + 50u);
    StoreU32(ctx, i0 + 36u, i1);
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 4u);
    state = i0;
  }
  i0 = l4;
  i1 = 141u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l7;
    i1 = state;
    PatchJumpChain(ctx, i0, i1);
    i0 = 141u;
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
  i1 = 142u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    StepRecordResume(ctx, i0);
    i0 = 142u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l4;
  i1 = 143u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    CloseBlockScope(ctx, i0);
    i0 = 143u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l4;
  i1 = 144u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = 262u;
    i2 = 259u;
    i3 = l9;
    CheckClosingMatch(ctx, i0, i1, i2, i3);
    i0 = 144u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_5;}
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    l1 = i0;
  }
  i0 = l4;
  i1 = 145u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    StepRecordResume(ctx, i0);
    i0 = 145u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i1 = l1;
    i1 = LoadU32(ctx, i1 + 24u);
    l2 = i1;
    StoreU32(ctx, i0 + 28u, i1);
    i0 = l6;
    i1 = 176u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l4;
  i1 = 146u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l3;
    i2 = 0u;
    i0 = CheckStatementLimits(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 146u;
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
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 176u);
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {
      i0 = l6;
      i1 = 3u;
      StoreU32(ctx, i0 + 176u, i1);
    }
    i0 = l6;
    i1 = 176u;
    i0 += i1;
    l7 = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    l3 = i0;
  }
  i0 = l4;
  i1 = 147u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l3;
    i1 = l7;
    EmitVariableStore(ctx, i0, i1);
    i0 = 147u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i1 = 1u;
    StoreU8(ctx, i0 + 162u, i1);
    i0 = l6;
    i1 = kF899ConstFFFFFFFF;
    StoreU32(ctx, i0 + 156u, i1);
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 196u);
    l8 = i0;
    i0 = l1;
    i0 = LoadU8(ctx, i0 + 50u);
    l3 = i0;
    i0 = l6;
    i1 = 0u;
    StoreU8(ctx, i0 + 161u, i1);
    i0 = l6;
    i1 = l3;
    StoreU8(ctx, i0 + 160u, i1);
    i0 = l6;
    i1 = l1;
    i1 = LoadU32(ctx, i1 + 20u);
    StoreU32(ctx, i0 + 152u, i1);
    i0 = l1;
    i1 = l6;
    i2 = 152u;
    i1 += i2;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = 259u;
    i0 = i0 != i1;
    l3 = i0;
  }
  i0 = l3;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 52u);
      l3 = i0;
    }
    i0 = l4;
    i1 = 148u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = 259u;
      i0 = FormatCharValue(ctx, i0, i1);
      l5 = i0;
      i0 = 148u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l7 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = l7;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 149u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l3;
      i1 = kF899MsgSExpected;
      i2 = l6;
      i0 = FormatPushString(ctx, i0, i1, i2);
      l5 = i0;
      i0 = 149u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l3 = i0;
    }
    i0 = l4;
    i1 = 150u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l3;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 150u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
  }
  i0 = l4;
  i1 = 151u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    StepRecordResume(ctx, i0);
    i0 = 151u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l4;
  i1 = 152u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    CloseBlockScope(ctx, i0);
    i0 = 152u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l4;
  i1 = 153u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l1;
    i0 = ConcatJumpLists(ctx, i0);
    l5 = i0;
    i0 = 153u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l3 = i0;
  }
  i0 = l4;
  i1 = 154u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l1;
    i1 = l3;
    i2 = l2;
    PatchComparisonIntoJump(ctx, i0, i1, i2);
    i0 = 154u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l4;
  i1 = 155u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = 262u;
    i2 = 277u;
    i3 = l9;
    CheckClosingMatch(ctx, i0, i1, i2, i3);
    i0 = 155u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i1 = l1;
    i1 = LoadU32(ctx, i1 + 20u);
    l3 = i1;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 20u, i1);
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 12u);
    i0 = LoadU32(ctx, i0 + 48u);
    l2 = i0;
    i0 = LoadU8(ctx, i0 + 50u);
    state = i0;
    i1 = l3;
    i1 = LoadU8(ctx, i1 + 8u);
    l7 = i1;
    i0 = i0 > i1;
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 24u);
      l9 = i0;
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 24u);
      l10 = i0;
      loop_385: 
        i0 = state;
        i1 = 1u;
        i0 -= i1;
        state = i0;
        i1 = 255u;
        i0 &= i1;
        l11 = i0;
        i1 = 1u;
        i0 <<= (i1 & 31);
        i1 = l2;
        i0 += i1;
        i0 = LoadU16(ctx, i0 + 172u);
        i1 = 12u;
        i0 *= i1;
        i1 = l10;
        i0 += i1;
        i1 = l9;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l7;
        i1 = l11;
        i0 = i0 < i1;
        if (i0) {goto loop_385;}
      i0 = l2;
      i1 = l7;
      StoreU8(ctx, i0 + 50u, i1);
    }
    i0 = l3;
    i0 = LoadU8(ctx, i0 + 9u);
    state = i0;
  }
  i0 = state;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l4;
    i1 = 156u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l1;
      i1 = 35u;
      i2 = l7;
      i3 = 0u;
      i4 = 0u;
      i0 = EmitOpInstruction(ctx, i0, i1, i2, i3, i4);
      l5 = i0;
      i0 = 156u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      state = i0;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i1 = l1;
    i1 = LoadU8(ctx, i1 + 50u);
    StoreU32(ctx, i0 + 36u, i1);
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    state = i0;
  }
  i0 = l4;
  i1 = 157u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l1;
    i1 = state;
    PatchJumpChain(ctx, i0, i1);
    i0 = 157u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l4;
  i1 = 158u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l1;
    i1 = l8;
    PatchJumpChain(ctx, i0, i1);
    i0 = 158u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_5:;
  i0 = state;
  i1 = 0u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  running_exit:;
  state = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i1 = 208u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    i0 = state;
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
  i2 = kF899ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l5 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = l1;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l5;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l5;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l5;
  i1 = l6;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l5;
  i1 = l7;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l5;
  i1 = l8;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l5;
  i1 = l9;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l5;
  i1 = l10;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = l5;
  i1 = l11;
  StoreU32(ctx, i0 + 36u, i1);
  i0 = l5;
  i1 = l12;
  StoreU32(ctx, i0 + 40u, i1);
  i0 = l5;
  i1 = l13;
  StoreU32(ctx, i0 + 44u, i1);
  i0 = l5;
  i1 = l14;
  StoreU32(ctx, i0 + 48u, i1);
  i0 = l5;
  i1 = l15;
  StoreU32(ctx, i0 + 52u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 56u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
