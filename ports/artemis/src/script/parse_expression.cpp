// parse_expression.cpp -- the expression parser front: consumes the operator chain and discharges each operand..
//
// ParseExpression -- the expression parser front: consumes the operator chain and discharges each operand.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 800746-800746 (f1718); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t FormatPushString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
void StepRecordResume(EngineContext* ctx, uint32_t a0);
uint32_t FormatCharValue(EngineContext* ctx, uint32_t a, uint32_t b);
uint32_t EmitOpInstruction(EngineContext* ctx, uint32_t func_state, uint32_t op, uint32_t arg_a, uint32_t arg_b, uint32_t line);
void PatchJumpChain(EngineContext* ctx, uint32_t state, uint32_t instruction);
uint32_t ConcatJumpLists(EngineContext* ctx, uint32_t state);
void ReserveRegisters(EngineContext* ctx, uint32_t state, uint32_t count);
void CloseBlockScope(EngineContext* ctx, uint32_t state);
void PatchComparisonIntoJump(EngineContext* ctx, uint32_t state, uint32_t instruction, uint32_t expression);
uint32_t EmitInstructionABC(EngineContext* ctx, uint32_t state, uint32_t opcode, uint32_t arg_a, uint32_t arg_c);

namespace {

constexpr uint32_t kF1718Const1FFFE = 0x1FFFEu;  // 
constexpr uint32_t kF1718MsgSExpected = 0x2B625u;  // '%s' expected
constexpr uint32_t kF1718ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF1718SaveFrameBytes = 44u;
constexpr uint32_t kF1718ResumeWordBytes = 4u;
constexpr uint32_t kF1718ResumeCloseBlockScope06 = 6u;
constexpr uint32_t kF1718ResumePatchJumpChain09 = 9u;

}  // namespace

void ParseExpression(EngineContext* ctx, uint32_t state, uint32_t token, uint32_t slot, uint32_t depth, uint32_t flag) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l13 = 0u; uint32_t l14 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
      l13 = 0, l14 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1718SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    slot = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 12u);
    depth = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 16u);
    flag = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 24u);
    l7 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 28u);
    l8 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 32u);
    l11 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 36u);
    l10 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 40u);
    l12 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 4u);
    token = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1718ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l9 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    i1 = 32u;
    i0 -= i1;
    l11 = i0;
    GuestStackSp(ctx) = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    l6 = i0;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = 3u;
    i0 += i1;
    l8 = i0;
    i0 = l6;
    i1 = l8;
    StoreU8(ctx, i0 + 50u, i1);
    i0 = l6;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 24u);
    l7 = i0;
    i1 = l6;
    i2 = l8;
    i3 = 255u;
    i2 &= i3;
    i3 = 1u;
    i2 <<= (i3 & 31);
    i1 += i2;
    l8 = i1;
    i1 = LoadU16(ctx, i1 + 166u);
    i2 = 12u;
    i1 *= i2;
    i0 += i1;
    i1 = l6;
    i1 = LoadU32(ctx, i1 + 24u);
    l12 = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l7;
    i1 = l8;
    i1 = LoadU16(ctx, i1 + 168u);
    i2 = 12u;
    i1 *= i2;
    i0 += i1;
    i1 = l12;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l7;
    i1 = l8;
    i1 = LoadU16(ctx, i1 + 170u);
    i2 = 12u;
    i1 *= i2;
    i0 += i1;
    i1 = l12;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = 259u;
    i0 = i0 != i1;
    l10 = i0;
  }
  i0 = l10;
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
      l7 = i0;
    }
    i0 = l9;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = 259u;
      i0 = FormatCharValue(ctx, i0, i1);
      l5 = i0;
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l10 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l11;
      i1 = l10;
      StoreU32(ctx, i0, i1);
    }
    i0 = l9;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l7;
      i1 = kF1718MsgSExpected;
      i2 = l11;
      i0 = FormatPushString(ctx, i0, i1, i2);
      l5 = i0;
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l10 = i0;
    }
    i0 = l9;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l10;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
  }
  i0 = l9;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    StepRecordResume(ctx, i0);
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = flag;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l9;
    i1 = 4u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l6;
      i1 = 32u;
      i2 = token;
      i3 = kF1718Const1FFFE;
      i0 = EmitInstructionABC(ctx, i0, i1, i2, i3);
      l5 = i0;
      i0 = 4u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l10 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_12;}
  }
  i0 = l9;
  i1 = 5u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i0 = ConcatJumpLists(ctx, i0);
    l5 = i0;
    i0 = 5u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l10 = i0;
  }
  block_12:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l11;
    i1 = 0u;
    StoreU8(ctx, i0 + 26u, i1);
    i0 = l11;
    i1 = kF1718ConstFFFFFFFF;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = l6;
    i0 = LoadU8(ctx, i0 + 50u);
    l7 = i0;
    i0 = l11;
    i1 = 0u;
    StoreU8(ctx, i0 + 25u, i1);
    i0 = l11;
    i1 = l7;
    StoreU8(ctx, i0 + 24u, i1);
    i0 = l11;
    i1 = l6;
    i1 = LoadU32(ctx, i1 + 20u);
    StoreU32(ctx, i0 + 16u, i1);
    i0 = l6;
    i1 = l11;
    i2 = 16u;
    i1 += i2;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    l8 = i0;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = depth;
    i0 += i1;
    l7 = i0;
    i0 = l8;
    i1 = l7;
    StoreU8(ctx, i0 + 50u, i1);
    i0 = depth;
    if (i0) {
      i0 = l7;
      i1 = 255u;
      i0 &= i1;
      l12 = i0;
      i0 = l8;
      i0 = LoadU32(ctx, i0 + 24u);
      l5 = i0;
      i0 = l8;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 24u);
      l13 = i0;
      i0 = depth;
      l7 = i0;
      loop_18: 
        i0 = l13;
        i1 = l8;
        i2 = l12;
        i3 = l7;
        i2 -= i3;
        i3 = 1u;
        i2 <<= (i3 & 31);
        i1 += i2;
        i1 = LoadU16(ctx, i1 + 172u);
        i2 = 12u;
        i1 *= i2;
        i0 += i1;
        i1 = l5;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = l7;
        i1 = 1u;
        i0 -= i1;
        l7 = i0;
        if (i0) {goto loop_18;}
    }
  }
  i0 = l9;
  i1 = 6u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = depth;
    ReserveRegisters(ctx, i0, i1);
    i0 = kF1718ResumeCloseBlockScope06;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l9;
  i1 = 7u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    CloseBlockScope(ctx, i0);
    i0 = 7u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i1 = l6;
    i1 = LoadU32(ctx, i1 + 20u);
    l8 = i1;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 20u, i1);
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 12u);
    i0 = LoadU32(ctx, i0 + 48u);
    state = i0;
    i0 = LoadU8(ctx, i0 + 50u);
    l7 = i0;
    i1 = l8;
    i1 = LoadU8(ctx, i1 + 8u);
    l12 = i1;
    i0 = i0 > i1;
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 24u);
      l5 = i0;
      i0 = state;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 24u);
      l13 = i0;
      loop_24: 
        i0 = l13;
        i1 = l7;
        i2 = 1u;
        i1 -= i2;
        l7 = i1;
        i2 = 255u;
        i1 &= i2;
        l14 = i1;
        i2 = 1u;
        i1 <<= (i2 & 31);
        i2 = state;
        i1 += i2;
        i1 = LoadU16(ctx, i1 + 172u);
        i2 = 12u;
        i1 *= i2;
        i0 += i1;
        i1 = l5;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l12;
        i1 = l14;
        i0 = i0 < i1;
        if (i0) {goto loop_24;}
      i0 = state;
      i1 = l12;
      StoreU8(ctx, i0 + 50u, i1);
    }
    i0 = l8;
    i0 = LoadU8(ctx, i0 + 9u);
    state = i0;
  }
  i0 = state;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l9;
    i1 = 8u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l6;
      i1 = 35u;
      i2 = l12;
      i3 = 0u;
      i4 = 0u;
      i0 = EmitOpInstruction(ctx, i0, i1, i2, i3, i4);
      l5 = i0;
      i0 = 8u;
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
    i0 = l6;
    i1 = l6;
    i1 = LoadU8(ctx, i1 + 50u);
    StoreU32(ctx, i0 + 36u, i1);
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 4u);
    state = i0;
  }
  i0 = l9;
  i1 = 9u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = state;
    PatchJumpChain(ctx, i0, i1);
    i0 = kF1718ResumePatchJumpChain09;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l9;
  i1 = 10u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = l10;
    PatchJumpChain(ctx, i0, i1);
    i0 = 10u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = flag;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l9;
    i1 = 11u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l6;
      i1 = 31u;
      i2 = token;
      i3 = kF1718Const1FFFE;
      i0 = EmitInstructionABC(ctx, i0, i1, i2, i3);
      l5 = i0;
      i0 = 11u;
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
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 20u);
      i1 = l6;
      i1 = LoadU32(ctx, i1 + 24u);
      i2 = 2u;
      i1 <<= (i2 & 31);
      i0 += i1;
      i1 = 4u;
      i0 -= i1;
      state = i0;
      i1 = slot;
      StoreU32(ctx, i0, i1);
      goto block_30;
    }
  }
  i0 = l9;
  i1 = 12u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = 33u;
    i2 = token;
    i3 = 0u;
    i4 = depth;
    i0 = EmitOpInstruction(ctx, i0, i1, i2, i3, i4);
    l5 = i0;
    i0 = 12u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    state = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
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
    state = i0;
    i1 = slot;
    StoreU32(ctx, i0, i1);
  }
  i0 = l9;
  i1 = 13u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i0 = ConcatJumpLists(ctx, i0);
    l5 = i0;
    i0 = 13u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l7 = i0;
  }
  block_30:;
  i0 = state;
  i1 = l10;
  i2 = 1u;
  i1 += i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  state = i0;
  i0 = l9;
  i1 = 14u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = l7;
    i2 = state;
    PatchComparisonIntoJump(ctx, i0, i1, i2);
    i0 = 14u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l11;
    i1 = 32u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
  }
  goto func_exit;
  spill_frame:;
  l5 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l5;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1718ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l5 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = token;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l5;
  i1 = slot;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l5;
  i1 = depth;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l5;
  i1 = flag;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l5;
  i1 = l6;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l5;
  i1 = l7;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l5;
  i1 = l8;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l5;
  i1 = l11;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = l5;
  i1 = l10;
  StoreU32(ctx, i0 + 36u, i1);
  i0 = l5;
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
