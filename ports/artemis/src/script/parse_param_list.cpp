// parse_param_list.cpp -- parses a parameter list: the parameter names are bound as local slots..
//
// ParseParamList -- parses a parameter list: the parameter names are bound as local slots.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 482643-482643 (f1118); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void PatchUpvalueJumpTarget(EngineContext* ctx, uint32_t a);
uint32_t FormatPushString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
void StepRecordResume(EngineContext* ctx, uint32_t a0);
uint32_t FormatCharValue(EngineContext* ctx, uint32_t a, uint32_t b);
void LexError(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t EmitOpInstruction(EngineContext* ctx, uint32_t func_state, uint32_t op, uint32_t arg_a, uint32_t arg_b, uint32_t line);
void DeclareLocalVariable(EngineContext* ctx, uint32_t state, uint32_t name, uint32_t slot);
void StoreListEntry(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void CheckClosingMatch(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d);
uint32_t BindUpvalueCell(EngineContext* ctx, uint32_t state, uint32_t local, uint32_t upvalue);
void PrepareParameterFrame(EngineContext* ctx, uint32_t a0, uint32_t a1);
void ReserveRegisters(EngineContext* ctx, uint32_t state, uint32_t count);
uint32_t GrowArray(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f);
uint32_t CompileStatement(EngineContext* ctx, uint32_t state);
uint32_t EmitInstructionABC(EngineContext* ctx, uint32_t state, uint32_t opcode, uint32_t arg_a, uint32_t arg_c);

namespace {

constexpr uint32_t kF1118MsgSExpected = 0x2B625u;  // '%s' expected
constexpr uint32_t kF1118MsgChunkHasTooMany = 0x2B633u;  // chunk has too many syntax levels
constexpr uint32_t kF1118MsgSelf = 0x2B70Au;  // self
constexpr uint32_t kF1118Const2B738 = 0x2B738u;  // 
constexpr uint32_t kF1118MsgNameOrExpected = 0x2B73Cu;  // <name> or '...' expected
constexpr uint32_t kF1118MsgConstantTableOverflow = 0x2B755u;  // constant table overflow
constexpr uint32_t kF1118Const3FFFF = 0x3FFFFu;  // 
constexpr uint32_t kF1118Const8010007 = 0x8010007u;  // 
constexpr uint32_t kF1118SaveFrameBytes = 40u;
constexpr uint32_t kF1118ResumeWordBytes = 4u;

}  // namespace

void ParseParamList(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u;
  uint64_t j1 = 0;
      l12 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1118SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 12u);
    d = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
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
    i2 = kF1118ResumeWordBytes;
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
    i1 = 608u;
    i0 -= i1;
    l10 = i0;
    GuestStackSp(ctx) = i0;
    i0 = l10;
    i1 = 32u;
    i0 += i1;
    l4 = i0;
  }
  i0 = l6;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = l4;
    PrepareParameterFrame(ctx, i0, i1);
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l10;
    i0 = LoadU32(ctx, i0 + 32u);
    i1 = d;
    StoreU32(ctx, i0 + 60u, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = 40u;
    i0 = i0 != i1;
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
      i0 = a;
      i0 = LoadU32(ctx, i0 + 52u);
      l8 = i0;
    }
    i0 = l6;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = 40u;
      i0 = FormatCharValue(ctx, i0, i1);
      l5 = i0;
      i0 = 1u;
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
      i0 = l10;
      i1 = l4;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l10;
      i1 = 16u;
      i0 += i1;
      l4 = i0;
    }
    i0 = l6;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l8;
      i1 = kF1118MsgSExpected;
      i2 = l4;
      i0 = FormatPushString(ctx, i0, i1, i2);
      l5 = i0;
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l4 = i0;
    }
    i0 = l6;
    i1 = 3u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = l4;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 3u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
  }
  i0 = l6;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    StepRecordResume(ctx, i0);
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = c;
  i1 = c;
  i1 = !(i1);
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  c = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 48u);
      l8 = i0;
      i0 = LoadU32(ctx, i0);
      l9 = i0;
      goto block_14;
    }
  }
  i0 = l6;
  i1 = 5u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF1118MsgSelf;
    i2 = 4u;
    i0 = BindUpvalueCell(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 5u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    c = i0;
  }
  i0 = l6;
  i1 = 6u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = c;
    i2 = 0u;
    DeclareLocalVariable(ctx, i0, i1, i2);
    i0 = 6u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 48u);
    l8 = i0;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = 1u;
    i0 += i1;
    c = i0;
    i0 = l8;
    i1 = c;
    StoreU8(ctx, i0 + 50u, i1);
    i0 = l8;
    i0 = LoadU32(ctx, i0);
    l9 = i0;
    i0 = LoadU32(ctx, i0 + 24u);
    i1 = l8;
    i2 = c;
    i3 = 255u;
    i2 &= i3;
    i3 = 1u;
    i2 <<= (i3 & 31);
    i1 += i2;
    i1 = LoadU16(ctx, i1 + 170u);
    i2 = 12u;
    i1 *= i2;
    i0 += i1;
    c = i0;
    i1 = l8;
    i1 = LoadU32(ctx, i1 + 24u);
    l4 = i1;
    StoreU32(ctx, i0 + 4u, i1);
  }
  block_14:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l9;
    i1 = 0u;
    StoreU8(ctx, i0 + 74u, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
    l7 = i0;
    i1 = 41u;
    i0 = i0 == i1;
    l11 = i0;
    i0 = 0u;
    l4 = i0;
    i0 = 0u;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  i1 = 0u;
  i2 = l11;
  i0 = i2 ? i0 : i1;
  if (i0) {goto block_21;}
  loop_22: 
    i0 = l4;
    i1 = l7;
    i2 = 279u;
    i1 -= i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l4 = i0;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      switch (i0) {
        case 0: goto block_25;
        case 1: goto block_24;
        case 2: goto block_24;
        case 3: goto block_24;
        case 4: goto block_24;
        case 5: goto block_24;
        case 6: goto block_27;
        default: goto block_24;
      }
      block_27:;
      i0 = a;
      i0 = LoadU32(ctx, i0 + 24u);
      l7 = i0;
    }
    i0 = l6;
    i1 = 7u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      StepRecordResume(ctx, i0);
      i0 = 7u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = l6;
    i1 = 8u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = l7;
      i2 = c;
      DeclareLocalVariable(ctx, i0, i1, i2);
      i0 = 8u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = c;
      i1 = 1u;
      i0 += i1;
      c = i0;
      goto block_23;
    }
    block_25:;
    i0 = l6;
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
    i0 = l6;
    i1 = 10u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = kF1118Const2B738;
      i2 = 3u;
      i0 = BindUpvalueCell(ctx, i0, i1, i2);
      l5 = i0;
      i0 = 10u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l4 = i0;
    }
    i0 = l6;
    i1 = 11u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = l4;
      i2 = c;
      DeclareLocalVariable(ctx, i0, i1, i2);
      i0 = 11u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l9;
      i1 = 7u;
      StoreU8(ctx, i0 + 74u, i1);
      i0 = c;
      i1 = 1u;
      i0 += i1;
      c = i0;
      i0 = 1u;
      l4 = i0;
      goto block_21;
    }
    block_24:;
    i0 = l6;
    i1 = 12u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = kF1118MsgNameOrExpected;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 12u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    block_23:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l9;
      i0 = LoadU8(ctx, i0 + 74u);
      l7 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l7;
      if (i0) {goto block_37;}
      i0 = a;
      i0 = LoadU32(ctx, i0 + 16u);
      i1 = 44u;
      i0 = i0 != i1;
      l4 = i0;
      if (i0) {goto block_37;}
    }
    i0 = l6;
    i1 = 13u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      StepRecordResume(ctx, i0);
      i0 = 13u;
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
      l7 = i0;
      goto loop_22;
    }
    block_37:;
  i0 = l4;
  i1 = l7;
  i2 = 1u;
  i1 &= i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l4 = i0;
  block_21:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 48u);
    l7 = i0;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = c;
    i0 += i1;
    l11 = i0;
    i0 = l7;
    i1 = l11;
    StoreU8(ctx, i0 + 50u, i1);
    i0 = c;
    if (i0) {
      i0 = l11;
      i1 = 255u;
      i0 &= i1;
      l11 = i0;
      i0 = l7;
      i0 = LoadU32(ctx, i0 + 24u);
      l5 = i0;
      i0 = l7;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 24u);
      l12 = i0;
      loop_43: 
        i0 = l12;
        i1 = l11;
        i2 = c;
        i1 -= i2;
        i2 = 1u;
        i1 <<= (i2 & 31);
        i2 = l7;
        i1 += i2;
        i1 = LoadU16(ctx, i1 + 172u);
        i2 = 12u;
        i1 *= i2;
        i0 += i1;
        i1 = l5;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = c;
        i1 = 1u;
        i0 -= i1;
        c = i0;
        if (i0) {goto loop_43;}
    }
    i0 = l9;
    i1 = l8;
    i1 = LoadU8(ctx, i1 + 50u);
    c = i1;
    i2 = l4;
    i1 -= i2;
    l4 = i1;
    StoreU8(ctx, i0 + 73u, i1);
  }
  i0 = l6;
  i1 = 14u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l8;
    i1 = c;
    ReserveRegisters(ctx, i0, i1);
    i0 = 14u;
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
    i0 = i0 != i1;
    c = i0;
  }
  i0 = c;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 52u);
      c = i0;
    }
    i0 = l6;
    i1 = 15u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = 41u;
      i0 = FormatCharValue(ctx, i0, i1);
      l5 = i0;
      i0 = 15u;
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
      i0 = l10;
      i1 = l4;
      StoreU32(ctx, i0, i1);
    }
    i0 = l6;
    i1 = 16u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = kF1118MsgSExpected;
      i2 = l10;
      i0 = FormatPushString(ctx, i0, i1, i2);
      l5 = i0;
      i0 = 16u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      c = i0;
    }
    i0 = l6;
    i1 = 17u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = c;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 17u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
  }
  i0 = l6;
  i1 = 18u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
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
    i0 = a;
    i0 = LoadU32(ctx, i0 + 52u);
    c = i0;
    l4 = i0;
    i1 = c;
    i1 = LoadU16(ctx, i1 + 52u);
    i2 = 1u;
    i1 += i2;
    c = i1;
    StoreU16(ctx, i0 + 52u, i1);
    i0 = c;
    i1 = 65535u;
    i0 &= i1;
    i1 = 201u;
    i0 = i0 >= i1;
    c = i0;
  }
  i0 = c;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l6;
    i1 = 19u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = kF1118MsgChunkHasTooMany;
      i2 = 0u;
      LexError(ctx, i0, i1, i2);
      i0 = 19u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
  }
  loop_58: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 16u);
      i1 = 260u;
      i0 -= i1;
      c = i0;
      i1 = 27u;
      i0 = i0 <= i1;
      l4 = i0;
      i1 = 0u;
      i2 = 1u;
      i3 = c;
      i2 <<= (i3 & 31);
      i3 = kF1118Const8010007;
      i2 &= i3;
      i0 = i2 ? i0 : i1;
      i0 = !(i0);
      c = i0;
    }
    i0 = c;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l6;
      i1 = 20u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i0 = CompileStatement(ctx, i0);
        l5 = i0;
        i0 = 20u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l5;
        c = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 16u);
        i1 = 59u;
        i0 = i0 == i1;
        l4 = i0;
      }
      i0 = l4;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = l6;
        i1 = 21u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = a;
          StepRecordResume(ctx, i0);
          i0 = 21u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 48u);
        l8 = i0;
        i0 = LoadU8(ctx, i0 + 50u);
        l4 = i0;
        i0 = l8;
        i1 = l4;
        StoreU32(ctx, i0 + 36u, i1);
        i0 = c;
        i0 = !(i0);
        c = i0;
        if (i0) {goto loop_58;}
      }
    }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 52u);
    c = i0;
    i0 = LoadU16(ctx, i0 + 52u);
    i1 = 1u;
    i0 -= i1;
    l4 = i0;
    i0 = c;
    i1 = l4;
    StoreU16(ctx, i0 + 52u, i1);
    i0 = l10;
    i0 = LoadU32(ctx, i0 + 32u);
    c = i0;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 4u);
    l4 = i1;
    StoreU32(ctx, i0 + 64u, i1);
  }
  i0 = l6;
  i1 = 22u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 262u;
    i2 = 265u;
    i3 = d;
    CheckClosingMatch(ctx, i0, i1, i2, i3);
    i0 = 22u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l6;
  i1 = 23u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    PatchUpvalueJumpTarget(ctx, i0);
    i0 = 23u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 48u);
    l8 = i0;
    i0 = LoadU32(ctx, i0 + 44u);
    l4 = i0;
    i0 = l4;
    i1 = l8;
    i1 = LoadU32(ctx, i1);
    d = i1;
    i1 = LoadU32(ctx, i1 + 52u);
    c = i1;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    if (i0) {goto block_72;}
    i0 = d;
    i0 = LoadU32(ctx, i0 + 16u);
    l7 = i0;
    i0 = d;
    i1 = 52u;
    i0 += i1;
    l9 = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 52u);
    l4 = i0;
  }
  i0 = l6;
  i1 = 24u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l4;
    i1 = l7;
    i2 = l9;
    i3 = 4u;
    i4 = kF1118Const3FFFF;
    i5 = kF1118MsgConstantTableOverflow;
    i0 = GrowArray(ctx, i0, i1, i2, i3, i4, i5);
    l5 = i0;
    i0 = 24u;
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
    i0 = d;
    i1 = l7;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = d;
    i0 = LoadU32(ctx, i0 + 52u);
    l9 = i0;
    i1 = c;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {goto block_72;}
    i0 = l7;
    i1 = c;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l9;
    i1 = c;
    i2 = 1u;
    i1 += i2;
    c = i1;
    i0 = i0 == i1;
    if (i0) {goto block_72;}
    loop_76: 
      i0 = d;
      i0 = LoadU32(ctx, i0 + 16u);
      i1 = c;
      i2 = 2u;
      i1 <<= (i2 & 31);
      i0 += i1;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l9;
      i1 = c;
      i2 = 1u;
      i1 += i2;
      c = i1;
      i0 = i0 != i1;
      if (i0) {goto loop_76;}
  }
  block_72:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = d;
    i0 = LoadU32(ctx, i0 + 16u);
    l9 = i0;
    i0 = l10;
    i0 = LoadU32(ctx, i0 + 32u);
    l7 = i0;
    i0 = l8;
    i1 = l8;
    i1 = LoadU32(ctx, i1 + 44u);
    l4 = i1;
    i2 = 1u;
    i1 += i2;
    c = i1;
    StoreU32(ctx, i0 + 44u, i1);
    i0 = l9;
    i1 = l4;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    i1 = l7;
    StoreU32(ctx, i0, i1);
    i0 = l10;
    i0 = LoadU32(ctx, i0 + 32u);
    l9 = i0;
    i0 = LoadU8(ctx, i0 + 5u);
    i1 = 3u;
    i0 &= i1;
    i0 = !(i0);
    if (i0) {goto block_78;}
    i0 = d;
    i0 = LoadU8(ctx, i0 + 5u);
    i1 = 4u;
    i0 &= i1;
    i0 = !(i0);
    if (i0) {goto block_78;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 52u);
    i1 = d;
    i2 = l9;
    StoreListEntry(ctx, i0, i1, i2);
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 44u);
    c = i0;
    block_78:;
    i0 = c;
    i1 = 1u;
    i0 -= i1;
    c = i0;
    i0 = 0u;
    a = i0;
  }
  i0 = l6;
  i1 = 25u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l8;
    i1 = 36u;
    i2 = 0u;
    i3 = c;
    i0 = EmitInstructionABC(ctx, i0, i1, i2, i3);
    l5 = i0;
    i0 = 25u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    j1 = 18446744073709551615ull;
    StoreU64(ctx, i0 + 16u, j1);
    i0 = b;
    i1 = c;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = b;
    i1 = 11u;
    StoreU32(ctx, i0, i1);
    i0 = l10;
    i0 = LoadU32(ctx, i0 + 32u);
    i0 = LoadU8(ctx, i0 + 72u);
    b = i0;
  }
  i0 = b;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    loop_83: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l10;
        i1 = 32u;
        i0 += i1;
        i1 = a;
        i2 = 1u;
        i1 <<= (i2 & 31);
        i0 += i1;
        b = i0;
        i0 = LoadU8(ctx, i0 + 51u);
        i1 = 6u;
        i0 = i0 != i1;
        i1 = 2u;
        i0 <<= (i1 & 31);
        c = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 52u);
        b = i0;
      }
      i0 = l6;
      i1 = 26u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l8;
        i1 = c;
        i2 = 0u;
        i3 = b;
        i4 = 0u;
        i0 = EmitOpInstruction(ctx, i0, i1, i2, i3, i4);
        i0 = 26u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i1 = 1u;
        i0 += i1;
        a = i0;
        i1 = l10;
        i1 = LoadU32(ctx, i1 + 32u);
        i1 = LoadU8(ctx, i1 + 72u);
        i0 = i0 < i1;
        b = i0;
        if (i0) {goto loop_83;}
      }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l10;
    i1 = 608u;
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
  i2 = kF1118ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l5 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l5;
  i1 = c;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l5;
  i1 = d;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l5;
  i1 = l4;
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
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 40u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
