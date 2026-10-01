#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
uint32_t ExpressionToRegister(EngineContext* ctx, uint32_t func_state, uint32_t expr);
void DischargeExpression(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t EmitConditionJump(EngineContext* ctx, uint32_t func_state, uint32_t condition);
uint32_t ResolveBranchOffset(EngineContext* ctx, uint32_t state, uint32_t opcode, uint32_t operands, uint32_t extra, uint32_t target);
void CompileNumericPower(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d);
void CompileExpression(EngineContext* ctx, uint32_t func_state, uint32_t code, uint32_t line);

namespace {

constexpr uint32_t kFunctionOrExpressionTooComplexMessage = 0x2A371u;  // function or expression too complex
constexpr uint32_t kData7FFFFF = 0x7FFFFFu;
constexpr uint32_t kEmitExpressionSaveFrameBytes = 44u;
constexpr uint32_t kEmitExpressionResumeWordBytes = 4u;

}  // namespace

// Emits one expression: it walks the operand tree, spends the syntax level the
// statement gave it and leaves the value in a register.  The step is suspendable
// at every call it makes.
void EmitExpression(EngineContext* ctx, uint32_t holder, uint32_t level, uint32_t func_state, uint32_t carry) {
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
    i2 = kEmitExpressionSaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    holder = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    func_state = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    carry = i0;
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
    i0 = LoadU32(ctx, i0 + 36u);
    l11 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 40u);
    l12 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 4u);
    level = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kEmitExpressionResumeWordBytes;
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
    i0 = level;
    switch (i0) {
      case 0: goto block_20;
      case 1: goto block_19;
      case 2: goto block_18;
      case 3: goto block_17;
      case 4: goto block_16;
      case 5: goto block_15;
      case 6: goto block_21;
      case 7: goto block_13;
      case 8: goto block_14;
      case 9: goto block_12;
      case 10: goto block_11;
      case 11: goto block_10;
      case 12: goto block_9;
      case 13: goto block_24;
      case 14: goto block_22;
      default: goto block_8;
    }
    block_24:;
  }
  i0 = l6;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = carry;
    DischargeExpression(ctx, i0, i1);
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l11;
  i1 = ResumePhase(ctx);
  i1 = !(i1);
  if (i1) {
    i1 = func_state;
    i1 = LoadU32(ctx, i1 + 20u);
    l8 = i1;
    i2 = kDataFFFFFFFF;
    i1 = i1 == i2;
    if (i1) {goto running_exit;}
    i1 = carry;
    i1 = LoadU32(ctx, i1 + 20u);
    level = i1;
    i2 = kDataFFFFFFFF;
    i1 = i1 == i2;
    if (i1) {goto block_7;}
    i1 = holder;
    i1 = LoadU32(ctx, i1);
    i1 = LoadU32(ctx, i1 + 12u);
    l10 = i1;
    loop_28: 
      i1 = level;
      l5 = i1;
      i1 = kDataFFFFFFFF;
      i2 = l5;
      i3 = l10;
      i4 = level;
      i5 = 2u;
      i4 <<= (i5 & 31);
      i3 += i4;
      l9 = i3;
      i3 = LoadU32(ctx, i3);
      l7 = i3;
      i4 = 14u;
      i3 >>= (i4 & 31);
      i4 = kData1FFFF;
      i3 -= i4;
      level = i3;
      i2 += i3;
      i3 = 1u;
      i2 += i3;
      i3 = level;
      i4 = kDataFFFFFFFF;
      i3 = i3 == i4;
      i1 = i3 ? i1 : i2;
      level = i1;
      i2 = kDataFFFFFFFF;
      i1 = i1 != i2;
      if (i1) {goto loop_28;}
    i1 = l9;
    l10 = i1;
    i1 = l8;
    i2 = l5;
    i3 = kDataFFFFFFFF;
    i2 ^= i3;
    i1 += i2;
    level = i1;
    i2 = 31u;
    i1 = (uint32_t)((int32_t)i1 >> (i2 & 31));
    l5 = i1;
    i2 = level;
    i3 = l5;
    i2 += i3;
    l8 = i2;
    i1 ^= i2;
    i2 = kData20000;
    i1 = (uint32_t)((int32_t)i1 >= (int32_t)i2);
    l5 = i1;
  }
  i1 = l5;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l11 = i0;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = holder;
      i0 = LoadU32(ctx, i0 + 12u);
      holder = i0;
    }
    i0 = l6;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = holder;
      i1 = kControlStructureTooLongMessage;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l7;
    } else {
      i0 = l9;
      i0 = LoadU32(ctx, i0);
    }
    l7 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l10;
    i1 = l7;
    i2 = 16383u;
    i1 &= i2;
    i2 = level;
    i3 = 14u;
    i2 <<= (i3 & 31);
    i1 |= i2;
    i2 = kData7FFFC000;
    i1 += i2;
    holder = i1;
    StoreU32(ctx, i0, i1);
    goto running_exit;
  }
  block_22:;
  i0 = l6;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = carry;
    DischargeExpression(ctx, i0, i1);
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l12;
  i1 = ResumePhase(ctx);
  i1 = !(i1);
  if (i1) {
    i1 = func_state;
    i1 = LoadU32(ctx, i1 + 16u);
    l8 = i1;
    i2 = kDataFFFFFFFF;
    i1 = i1 == i2;
    if (i1) {goto block_5;}
    i1 = carry;
    i1 = LoadU32(ctx, i1 + 16u);
    level = i1;
    i2 = kDataFFFFFFFF;
    i1 = i1 == i2;
    if (i1) {goto block_6;}
    i1 = holder;
    i1 = LoadU32(ctx, i1);
    i1 = LoadU32(ctx, i1 + 12u);
    l10 = i1;
    loop_37: 
      i1 = level;
      l5 = i1;
      i1 = kDataFFFFFFFF;
      i2 = l5;
      i3 = l10;
      i4 = level;
      i5 = 2u;
      i4 <<= (i5 & 31);
      i3 += i4;
      l9 = i3;
      i3 = LoadU32(ctx, i3);
      l7 = i3;
      i4 = 14u;
      i3 >>= (i4 & 31);
      i4 = kData1FFFF;
      i3 -= i4;
      level = i3;
      i2 += i3;
      i3 = 1u;
      i2 += i3;
      i3 = level;
      i4 = kDataFFFFFFFF;
      i3 = i3 == i4;
      i1 = i3 ? i1 : i2;
      level = i1;
      i2 = kDataFFFFFFFF;
      i1 = i1 != i2;
      if (i1) {goto loop_37;}
    i1 = l9;
    l10 = i1;
    i1 = l8;
    i2 = l5;
    i3 = kDataFFFFFFFF;
    i2 ^= i3;
    i1 += i2;
    level = i1;
    i2 = 31u;
    i1 = (uint32_t)((int32_t)i1 >> (i2 & 31));
    l5 = i1;
    i2 = level;
    i3 = l5;
    i2 += i3;
    l8 = i2;
    i1 ^= i2;
    i2 = kData20000;
    i1 = (uint32_t)((int32_t)i1 >= (int32_t)i2);
    l5 = i1;
  }
  i1 = l5;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l12 = i0;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = holder;
      i0 = LoadU32(ctx, i0 + 12u);
      holder = i0;
    }
    i0 = l6;
    i1 = 3u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = holder;
      i1 = kControlStructureTooLongMessage;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 3u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l7;
    } else {
      i0 = l9;
      i0 = LoadU32(ctx, i0);
    }
    l7 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l10;
    i1 = l7;
    i2 = 16383u;
    i1 &= i2;
    i2 = level;
    i3 = 14u;
    i2 <<= (i3 & 31);
    i1 |= i2;
    i2 = kData7FFFC000;
    i1 += i2;
    holder = i1;
    StoreU32(ctx, i0, i1);
    goto block_5;
  }
  block_21:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = carry;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
    i1 = carry;
    i1 = LoadU32(ctx, i1 + 16u);
    i0 = i0 != i1;
    level = i0;
  }
  i0 = level;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l6;
    i1 = 4u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = holder;
      i1 = carry;
      i0 = EmitConditionJump(ctx, i0, i1);
      l4 = i0;
      i0 = 4u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      level = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_43;}
  }
  i0 = l6;
  i1 = 5u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = carry;
    DischargeExpression(ctx, i0, i1);
    i0 = 5u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_43:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = carry;
    i0 = LoadU32(ctx, i0);
    i1 = 11u;
    i0 = i0 != i1;
    level = i0;
    if (i0) {goto block_50;}
    i0 = holder;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    level = i0;
    i1 = carry;
    i1 = LoadU32(ctx, i1 + 8u);
    l5 = i1;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l7 = i0;
    i1 = 63u;
    i0 &= i1;
    i1 = 21u;
    i0 = i0 != i1;
    l9 = i0;
    if (i0) {goto block_50;}
    i0 = func_state;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    if (i0) {goto block_51;}
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 8u);
    l8 = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_51;}
    i0 = l8;
    i1 = holder;
    i1 = LoadU8(ctx, i1 + 50u);
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {goto block_51;}
    i0 = holder;
    i1 = holder;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 36u, i1);
    i0 = level;
    i1 = carry;
    i1 = LoadU32(ctx, i1 + 8u);
    l5 = i1;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l7 = i0;
    block_51:;
    i0 = level;
    i1 = l5;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    i1 = l7;
    i2 = kData7FFFFF;
    i1 &= i2;
    i2 = func_state;
    i2 = LoadU32(ctx, i2 + 8u);
    i3 = 23u;
    i2 <<= (i3 & 31);
    i1 |= i2;
    StoreU32(ctx, i0, i1);
    i0 = func_state;
    i1 = 11u;
    StoreU32(ctx, i0, i1);
    i0 = func_state;
    i1 = carry;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 8u, i1);
    goto func_exit;
    block_50:;
  }
  i0 = l6;
  i1 = 6u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = carry;
    DischargeExpression(ctx, i0, i1);
    i0 = 6u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = carry;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    if (i0) {goto block_54;}
    i0 = carry;
    i0 = LoadU32(ctx, i0 + 8u);
    level = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_54;}
    i0 = holder;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = level;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_54;}
    i0 = holder;
    i1 = holder;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 36u, i1);
    block_54:;
    i0 = holder;
    i0 = LoadU32(ctx, i0 + 36u);
    l8 = i0;
    i1 = 1u;
    i0 += i1;
    l5 = i0;
    i0 = l8;
    i1 = holder;
    i1 = LoadU32(ctx, i1);
    l7 = i1;
    i1 = LoadU8(ctx, i1 + 75u);
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    level = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = level;
    if (i0) {
      i0 = l5;
      level = i0;
      goto block_55;
    }
    i0 = l8;
    i1 = 249u;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    l9 = i0;
    i0 = l5;
    level = i0;
  }
  i0 = l9;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = holder;
      i0 = LoadU32(ctx, i0 + 12u);
      level = i0;
    }
    i0 = l6;
    i1 = 7u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = level;
      i1 = kFunctionOrExpressionTooComplexMessage;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 7u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = level;
    } else {
      i0 = holder;
      i0 = LoadU32(ctx, i0);
      l7 = i0;
      i0 = holder;
      i0 = LoadU32(ctx, i0 + 36u);
      i1 = 1u;
      i0 += i1;
    }
    level = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    i1 = l5;
    StoreU8(ctx, i0 + 75u, i1);
  }
  block_55:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = holder;
    i1 = level;
    StoreU32(ctx, i0 + 36u, i1);
    i0 = level;
    i1 = 1u;
    i0 -= i1;
    level = i0;
  }
  i0 = l6;
  i1 = 8u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = carry;
    i2 = level;
    CompileExpression(ctx, i0, i1, i2);
    i0 = 8u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l6;
  i1 = 9u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = 21u;
    i2 = func_state;
    i3 = carry;
    CompileNumericPower(ctx, i0, i1, i2, i3);
    i0 = 9u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    goto func_exit;
  }
  block_20:;
  i0 = l6;
  i1 = 10u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = 12u;
    i2 = func_state;
    i3 = carry;
    CompileNumericPower(ctx, i0, i1, i2, i3);
    i0 = 10u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    goto func_exit;
  }
  block_19:;
  i0 = l6;
  i1 = 11u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = 13u;
    i2 = func_state;
    i3 = carry;
    CompileNumericPower(ctx, i0, i1, i2, i3);
    i0 = 11u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    goto func_exit;
  }
  block_18:;
  i0 = l6;
  i1 = 12u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = 14u;
    i2 = func_state;
    i3 = carry;
    CompileNumericPower(ctx, i0, i1, i2, i3);
    i0 = 12u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    goto func_exit;
  }
  block_17:;
  i0 = l6;
  i1 = 13u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = 15u;
    i2 = func_state;
    i3 = carry;
    CompileNumericPower(ctx, i0, i1, i2, i3);
    i0 = 13u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    goto func_exit;
  }
  block_16:;
  i0 = l6;
  i1 = 14u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = 16u;
    i2 = func_state;
    i3 = carry;
    CompileNumericPower(ctx, i0, i1, i2, i3);
    i0 = 14u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    goto func_exit;
  }
  block_15:;
  i0 = l6;
  i1 = 15u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = 17u;
    i2 = func_state;
    i3 = carry;
    CompileNumericPower(ctx, i0, i1, i2, i3);
    i0 = 15u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    goto func_exit;
  }
  block_14:;
  i0 = l6;
  i1 = 16u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = func_state;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 16u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    level = i0;
  }
  i0 = l6;
  i1 = 17u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = carry;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 17u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = carry;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    if (i0) {goto block_82;}
    i0 = carry;
    i0 = LoadU32(ctx, i0 + 8u);
    carry = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_82;}
    i0 = holder;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = carry;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_82;}
    i0 = holder;
    i1 = holder;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 36u, i1);
    block_82:;
    i0 = func_state;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    carry = i0;
    if (i0) {goto block_83;}
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 8u);
    carry = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_83;}
    i0 = holder;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = carry;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    carry = i0;
    if (i0) {goto block_83;}
    i0 = holder;
    i1 = holder;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    carry = i1;
    StoreU32(ctx, i0 + 36u, i1);
    block_83:;
  }
  i0 = l6;
  i1 = 18u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = 23u;
    i2 = 1u;
    i3 = level;
    i4 = l5;
    i0 = ResolveBranchOffset(ctx, i0, i1, i2, i3, i4);
    l4 = i0;
    i0 = 18u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    holder = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_3;}
  block_13:;
  i0 = l6;
  i1 = 19u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = func_state;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 19u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    level = i0;
  }
  i0 = l6;
  i1 = 20u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = carry;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 20u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = carry;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    if (i0) {goto block_88;}
    i0 = carry;
    i0 = LoadU32(ctx, i0 + 8u);
    carry = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_88;}
    i0 = holder;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = carry;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_88;}
    i0 = holder;
    i1 = holder;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 36u, i1);
    block_88:;
    i0 = func_state;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    carry = i0;
    if (i0) {goto block_89;}
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 8u);
    carry = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_89;}
    i0 = holder;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = carry;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    carry = i0;
    if (i0) {goto block_89;}
    i0 = holder;
    i1 = holder;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    carry = i1;
    StoreU32(ctx, i0 + 36u, i1);
    block_89:;
  }
  i0 = l6;
  i1 = 21u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = 23u;
    i2 = 0u;
    i3 = level;
    i4 = l5;
    i0 = ResolveBranchOffset(ctx, i0, i1, i2, i3, i4);
    l4 = i0;
    i0 = 21u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    holder = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_3;}
  block_12:;
  i0 = l6;
  i1 = 22u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = func_state;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 22u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    level = i0;
  }
  i0 = l6;
  i1 = 23u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = carry;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 23u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = carry;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    if (i0) {goto block_94;}
    i0 = carry;
    i0 = LoadU32(ctx, i0 + 8u);
    carry = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_94;}
    i0 = holder;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = carry;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_94;}
    i0 = holder;
    i1 = holder;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 36u, i1);
    block_94:;
    i0 = func_state;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    carry = i0;
    if (i0) {goto block_95;}
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 8u);
    carry = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_95;}
    i0 = holder;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = carry;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    carry = i0;
    if (i0) {goto block_95;}
    i0 = holder;
    i1 = holder;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    carry = i1;
    StoreU32(ctx, i0 + 36u, i1);
    block_95:;
  }
  i0 = l6;
  i1 = 24u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = 24u;
    i2 = 1u;
    i3 = level;
    i4 = l5;
    i0 = ResolveBranchOffset(ctx, i0, i1, i2, i3, i4);
    l4 = i0;
    i0 = 24u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    holder = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_3;}
  block_11:;
  i0 = l6;
  i1 = 25u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = func_state;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 25u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    level = i0;
  }
  i0 = l6;
  i1 = 26u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = carry;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 26u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = carry;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    if (i0) {goto block_100;}
    i0 = carry;
    i0 = LoadU32(ctx, i0 + 8u);
    carry = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_100;}
    i0 = holder;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = carry;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_100;}
    i0 = holder;
    i1 = holder;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 36u, i1);
    block_100:;
    i0 = func_state;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    carry = i0;
    if (i0) {goto block_101;}
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 8u);
    carry = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_101;}
    i0 = holder;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = carry;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    carry = i0;
    if (i0) {goto block_101;}
    i0 = holder;
    i1 = holder;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    carry = i1;
    StoreU32(ctx, i0 + 36u, i1);
    block_101:;
  }
  i0 = l6;
  i1 = 27u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = 25u;
    i2 = 1u;
    i3 = level;
    i4 = l5;
    i0 = ResolveBranchOffset(ctx, i0, i1, i2, i3, i4);
    l4 = i0;
    i0 = 27u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    holder = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_3;}
  block_10:;
  i0 = l6;
  i1 = 28u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = func_state;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 28u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    level = i0;
  }
  i0 = l6;
  i1 = 29u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = carry;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 29u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = carry;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    if (i0) {goto block_106;}
    i0 = carry;
    i0 = LoadU32(ctx, i0 + 8u);
    carry = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_106;}
    i0 = holder;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = carry;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_106;}
    i0 = holder;
    i1 = holder;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 36u, i1);
    block_106:;
    i0 = func_state;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    carry = i0;
    if (i0) {goto block_107;}
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 8u);
    carry = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_107;}
    i0 = holder;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = carry;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    carry = i0;
    if (i0) {goto block_107;}
    i0 = holder;
    i1 = holder;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    carry = i1;
    StoreU32(ctx, i0 + 36u, i1);
    block_107:;
  }
  i0 = l6;
  i1 = 30u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = 24u;
    i2 = 1u;
    i3 = l5;
    i4 = level;
    i0 = ResolveBranchOffset(ctx, i0, i1, i2, i3, i4);
    l4 = i0;
    i0 = 30u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    holder = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_3;}
  block_9:;
  i0 = l6;
  i1 = 31u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = func_state;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 31u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    level = i0;
  }
  i0 = l6;
  i1 = 32u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = carry;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l4 = i0;
    i0 = 32u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = carry;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    if (i0) {goto block_112;}
    i0 = carry;
    i0 = LoadU32(ctx, i0 + 8u);
    carry = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_112;}
    i0 = holder;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = carry;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_112;}
    i0 = holder;
    i1 = holder;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 36u, i1);
    block_112:;
    i0 = func_state;
    i0 = LoadU32(ctx, i0);
    i1 = 12u;
    i0 = i0 != i1;
    carry = i0;
    if (i0) {goto block_113;}
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 8u);
    carry = i0;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto block_113;}
    i0 = holder;
    i0 = LoadU8(ctx, i0 + 50u);
    i1 = carry;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    carry = i0;
    if (i0) {goto block_113;}
    i0 = holder;
    i1 = holder;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = 1u;
    i1 -= i2;
    carry = i1;
    StoreU32(ctx, i0 + 36u, i1);
    block_113:;
  }
  i0 = l6;
  i1 = 33u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = holder;
    i1 = 25u;
    i2 = 1u;
    i3 = l5;
    i4 = level;
    i0 = ResolveBranchOffset(ctx, i0, i1, i2, i3, i4);
    l4 = i0;
    i0 = 33u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    holder = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = func_state;
    i1 = 10u;
    StoreU32(ctx, i0, i1);
    i0 = func_state;
    i1 = holder;
    StoreU32(ctx, i0 + 8u, i1);
  }
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    goto func_exit;
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = carry;
    i1 = l8;
    StoreU32(ctx, i0 + 20u, i1);
    goto running_exit;
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = carry;
    i1 = l8;
    StoreU32(ctx, i0 + 16u, i1);
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = func_state;
    i1 = carry;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0, j1);
    i0 = func_state;
    i1 = carry;
    j1 = LoadU64(ctx, i1 + 16u);
    StoreU64(ctx, i0 + 16u, j1);
    i0 = func_state;
    i1 = carry;
    j1 = LoadU64(ctx, i1 + 8u);
    StoreU64(ctx, i0 + 8u, j1);
    goto func_exit;
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = func_state;
    i1 = carry;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0, j1);
    i0 = func_state;
    i1 = carry;
    j1 = LoadU64(ctx, i1 + 16u);
    StoreU64(ctx, i0 + 16u, j1);
    i0 = func_state;
    i1 = carry;
    j1 = LoadU64(ctx, i1 + 8u);
    StoreU64(ctx, i0 + 8u, j1);
    goto func_exit;
  }
  block_3:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = func_state;
    i1 = 10u;
    StoreU32(ctx, i0, i1);
    i0 = func_state;
    i1 = holder;
    StoreU32(ctx, i0 + 8u, i1);
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
  i2 = kEmitExpressionResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  i1 = holder;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = level;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l4;
  i1 = func_state;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l4;
  i1 = carry;
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
  i0 = l4;
  i1 = l11;
  StoreU32(ctx, i0 + 36u, i1);
  i0 = l4;
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
