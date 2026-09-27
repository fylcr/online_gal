// compile_expression.cpp -- the code generator's expression-compilation entry: reads the expression opcode and every shape it recognises discharges into the register and operand emitters it drives.
//
// CompileExpression -- the code generator's expression-compilation entry: reads the expression opcode and every shape it recognises discharges into the register and operand emitters it drives. Self-recursive on nested expressions.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 270114-271841 (f707); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void RaiseSyntaxError(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t EmitInstruction(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void ExpressionToOperand(EngineContext* ctx, uint32_t func_state, uint32_t expr, uint32_t operand);

namespace {

constexpr uint32_t kF707Const1FFFF = 0x1FFFFu;  // 
constexpr uint32_t kF707Const20000 = 0x20000u;  // 
constexpr uint32_t kF707MsgControlStructureTooLong = 0x2A394u;  // control structure too long
constexpr uint32_t kF707Const2B460 = 0x2B460u;  // 
constexpr uint32_t kF707Const7FC000 = 0x7FC000u;  // 
constexpr uint32_t kF707Const800002 = 0x800002u;  // 
constexpr uint32_t kF707Const7FFF8016 = 0x7FFF8016u;  // 
constexpr uint32_t kF707Const7FFFC000 = 0x7FFFC000u;  // 
constexpr uint32_t kF707ConstFFFFC03F = 0xFFFFC03Fu;  // 
constexpr uint32_t kF707ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF707SaveFrameBytes = 68u;
constexpr uint32_t kF707ResumeWordBytes = 4u;

}  // namespace

void CompileExpression(EngineContext* ctx, uint32_t func_state, uint32_t code, uint32_t line) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l13 = 0u; uint32_t l14 = 0u; uint32_t l15 = 0u; uint32_t l16 = 0u; uint32_t l17 = 0u; uint32_t l18 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u;
  uint64_t j1 = 0;
      l11 = 0, l12 = 0, l13 = 0, l14 = 0, l15 = 0, l16 = 0, l17 = 0, l18 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF707SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l7 = i0;
    i0 = LoadU32(ctx, i0);
    func_state = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 8u);
    line = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 24u);
    l6 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 28u);
    l8 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 32u);
    l9 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 36u);
    l10 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 40u);
    l11 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 44u);
    l13 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 48u);
    l14 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 52u);
    l15 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 56u);
    l16 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 60u);
    l17 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 64u);
    l18 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 4u);
    code = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF707ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l12 = i0;
  }
  i0 = l12;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = func_state;
    i1 = code;
    i2 = line;
    ExpressionToOperand(ctx, i0, i1, i2);
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = code;
    i0 = LoadU32(ctx, i0);
    i1 = 10u;
    i0 = i0 != i1;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    if (i0) {goto block_6;}
    i0 = code;
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
    i1 = kF707ConstFFFFFFFF;
    i0 = i0 == i1;
    l3 = i0;
    if (i0) {goto block_6;}
    i0 = code;
    i0 = LoadU32(ctx, i0 + 16u);
    l3 = i0;
    i1 = kF707ConstFFFFFFFF;
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
      i0 = func_state;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      l9 = i0;
      loop_11: 
        i0 = l3;
        l5 = i0;
        i0 = kF707ConstFFFFFFFF;
        i1 = l5;
        i2 = l9;
        i3 = l3;
        i4 = 2u;
        i3 <<= (i4 & 31);
        i2 += i3;
        l8 = i2;
        i2 = LoadU32(ctx, i2);
        l6 = i2;
        i3 = 14u;
        i2 >>= (i3 & 31);
        i3 = kF707Const1FFFF;
        i2 -= i3;
        l3 = i2;
        i1 += i2;
        i2 = 1u;
        i1 += i2;
        i2 = l3;
        i3 = kF707ConstFFFFFFFF;
        i2 = i2 == i3;
        i0 = i2 ? i0 : i1;
        l3 = i0;
        i1 = kF707ConstFFFFFFFF;
        i0 = i0 != i1;
        l10 = i0;
        if (i0) {goto loop_11;}
      i0 = l4;
      i1 = l5;
      i2 = kF707ConstFFFFFFFF;
      i1 ^= i2;
      i0 += i1;
      l3 = i0;
      i1 = 31u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l5 = i0;
      i0 = l5;
      i1 = l3;
      i2 = l5;
      i1 += i2;
      i0 ^= i1;
      i1 = kF707Const20000;
      i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
      l4 = i0;
      i0 = l8;
      l9 = i0;
    }
    i0 = l16;
    i1 = l4;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l16 = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = func_state;
        i0 = LoadU32(ctx, i0 + 12u);
        l4 = i0;
      }
      i0 = l12;
      i1 = 1u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l4;
        i1 = kF707MsgControlStructureTooLong;
        RaiseSyntaxError(ctx, i0, i1);
        i0 = 1u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l6;
      } else {
        i0 = l8;
        i0 = LoadU32(ctx, i0);
      }
      l6 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l9;
      i1 = l6;
      i2 = 16383u;
      i1 &= i2;
      l4 = i1;
      i2 = l3;
      i3 = 14u;
      i2 <<= (i3 & 31);
      i1 |= i2;
      i2 = kF707Const7FFFC000;
      i1 += i2;
      l3 = i1;
      StoreU32(ctx, i0, i1);
      goto block_6;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = code;
    i1 = l4;
    StoreU32(ctx, i0 + 16u, i1);
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = code;
    i0 = LoadU32(ctx, i0 + 16u);
    l3 = i0;
    i1 = code;
    i1 = LoadU32(ctx, i1 + 20u);
    l5 = i1;
    i0 = i0 == i1;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    if (i0) {goto block_19;}
    i0 = kF707ConstFFFFFFFF;
    l13 = i0;
    i0 = l3;
    i1 = kF707ConstFFFFFFFF;
    i0 = i0 != i1;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    if (i0) {
      i0 = func_state;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      l8 = i0;
      loop_25: 
        i0 = l8;
        i1 = l3;
        i2 = 2u;
        i1 <<= (i2 & 31);
        i0 += i1;
        l4 = i0;
        i0 = l3;
        i1 = 1u;
        i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
        if (i0) {
          i0 = l4;
          i1 = 4u;
          i0 -= i1;
          i0 = LoadU32(ctx, i0);
          l6 = i0;
          i1 = 63u;
          i0 &= i1;
          i1 = kF707Const2B460;
          i0 += i1;
          i0 = LoadS8(ctx, i0);
          i1 = 0u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          l9 = i0;
          if (i0) {goto block_26;}
        }
        i0 = l4;
        i0 = LoadU32(ctx, i0);
        l6 = i0;
        block_26:;
        i0 = l6;
        i1 = 63u;
        i0 &= i1;
        i1 = 27u;
        i0 = i0 != i1;
        l6 = i0;
        if (i0) {goto block_23;}
        i0 = kF707ConstFFFFFFFF;
        i1 = l4;
        i1 = LoadU32(ctx, i1);
        i2 = 14u;
        i1 >>= (i2 & 31);
        i2 = kF707Const1FFFF;
        i1 -= i2;
        l6 = i1;
        i2 = l3;
        i1 += i2;
        i2 = 1u;
        i1 += i2;
        i2 = l6;
        i3 = kF707ConstFFFFFFFF;
        i2 = i2 == i3;
        i0 = i2 ? i0 : i1;
        l3 = i0;
        i1 = kF707ConstFFFFFFFF;
        i0 = i0 != i1;
        l4 = i0;
        if (i0) {goto loop_25;}
    }
    i0 = kF707ConstFFFFFFFF;
    l10 = i0;
    i0 = l5;
    i1 = kF707ConstFFFFFFFF;
    i0 = i0 == i1;
    l3 = i0;
    if (i0) {goto block_21;}
    i0 = func_state;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    loop_28: 
      i0 = l4;
      i1 = l5;
      i2 = 2u;
      i1 <<= (i2 & 31);
      i0 += i1;
      l3 = i0;
      i0 = l5;
      i1 = 1u;
      i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i1 = 4u;
        i0 -= i1;
        i0 = LoadU32(ctx, i0);
        l6 = i0;
        i1 = 63u;
        i0 &= i1;
        i1 = kF707Const2B460;
        i0 += i1;
        i0 = LoadS8(ctx, i0);
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l8 = i0;
        if (i0) {goto block_29;}
      }
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      block_29:;
      i0 = l6;
      i1 = 63u;
      i0 &= i1;
      i1 = 27u;
      i0 = i0 != i1;
      l6 = i0;
      if (i0) {goto block_23;}
      i0 = kF707ConstFFFFFFFF;
      i1 = l5;
      i2 = l3;
      i2 = LoadU32(ctx, i2);
      i3 = 14u;
      i2 >>= (i3 & 31);
      i3 = kF707Const1FFFF;
      i2 -= i3;
      l3 = i2;
      i1 += i2;
      i2 = 1u;
      i1 += i2;
      i2 = l3;
      i3 = kF707ConstFFFFFFFF;
      i2 = i2 == i3;
      i0 = i2 ? i0 : i1;
      l5 = i0;
      i1 = kF707ConstFFFFFFFF;
      i0 = i0 != i1;
      l3 = i0;
      if (i0) {goto loop_28;}
    goto block_21;
    block_23:;
    i0 = code;
    i0 = LoadU32(ctx, i0);
    i1 = 10u;
    i0 = i0 == i1;
    l3 = i0;
    i0 = kF707ConstFFFFFFFF;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    if (i0) {goto block_31;}
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 32u);
    l4 = i0;
    i0 = func_state;
    i1 = kF707ConstFFFFFFFF;
    StoreU32(ctx, i0 + 32u, i1);
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 12u);
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
  }
  i0 = l12;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = func_state;
    i1 = kF707Const7FFF8016;
    i2 = l3;
    i0 = EmitInstruction(ctx, i0, i1, i2);
    l7 = i0;
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    l5 = i0;
  }
  i0 = l17;
  i1 = ResumePhase(ctx);
  i1 = !(i1);
  if (i1) {
    i1 = l4;
    i2 = kF707ConstFFFFFFFF;
    i1 = i1 == i2;
    l3 = i1;
    if (i1) {goto block_31;}
    i1 = l5;
    i2 = kF707ConstFFFFFFFF;
    i1 = i1 == i2;
    l3 = i1;
    if (i1) {
      i1 = l4;
      l5 = i1;
      goto block_31;
    }
    i1 = func_state;
    i1 = LoadU32(ctx, i1);
    i1 = LoadU32(ctx, i1 + 12u);
    l13 = i1;
    i1 = l5;
    l3 = i1;
    loop_37: 
      i1 = l3;
      l8 = i1;
      i1 = kF707ConstFFFFFFFF;
      i2 = l8;
      i3 = l13;
      i4 = l3;
      i5 = 2u;
      i4 <<= (i5 & 31);
      i3 += i4;
      l9 = i3;
      i3 = LoadU32(ctx, i3);
      l6 = i3;
      i4 = 14u;
      i3 >>= (i4 & 31);
      i4 = kF707Const1FFFF;
      i3 -= i4;
      l3 = i3;
      i2 += i3;
      i3 = 1u;
      i2 += i3;
      i3 = l3;
      i4 = kF707ConstFFFFFFFF;
      i3 = i3 == i4;
      i1 = i3 ? i1 : i2;
      l3 = i1;
      i2 = kF707ConstFFFFFFFF;
      i1 = i1 != i2;
      if (i1) {goto loop_37;}
    i1 = l9;
    l10 = i1;
    i1 = l4;
    i2 = l8;
    i3 = kF707ConstFFFFFFFF;
    i2 ^= i3;
    i1 += i2;
    l3 = i1;
    i2 = 31u;
    i1 = (uint32_t)((int32_t)i1 >> (i2 & 31));
    l4 = i1;
    i2 = l3;
    i3 = l4;
    i2 += i3;
    l8 = i2;
    i1 ^= i2;
    i2 = kF707Const20000;
    i1 = (uint32_t)((int32_t)i1 >= (int32_t)i2);
    l4 = i1;
  }
  i1 = l4;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l17 = i0;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = func_state;
      i0 = LoadU32(ctx, i0 + 12u);
      l4 = i0;
    }
    i0 = l12;
    i1 = 3u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l4;
      i1 = kF707MsgControlStructureTooLong;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 3u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l6;
    } else {
      i0 = l9;
      i0 = LoadU32(ctx, i0);
    }
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l10;
    i1 = l6;
    i2 = 16383u;
    i1 &= i2;
    l4 = i1;
    i2 = l3;
    i3 = 14u;
    i2 <<= (i3 & 31);
    i1 |= i2;
    i2 = kF707Const7FFFC000;
    i1 += i2;
    l3 = i1;
    StoreU32(ctx, i0, i1);
  }
  block_31:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = func_state;
    i1 = func_state;
    i1 = LoadU32(ctx, i1 + 24u);
    StoreU32(ctx, i0 + 28u, i1);
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 12u);
    i0 = LoadU32(ctx, i0 + 8u);
    l6 = i0;
    i0 = line;
    i1 = 6u;
    i0 <<= (i1 & 31);
    l3 = i0;
    i1 = 16386u;
    i0 |= i1;
    l4 = i0;
  }
  i0 = l12;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = func_state;
    i1 = l4;
    i2 = l6;
    i0 = EmitInstruction(ctx, i0, i1, i2);
    l7 = i0;
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    l10 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = func_state;
    i1 = func_state;
    i1 = LoadU32(ctx, i1 + 24u);
    StoreU32(ctx, i0 + 28u, i1);
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 12u);
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
    i0 = l3;
    i1 = kF707Const800002;
    i0 |= i1;
    l3 = i0;
  }
  i0 = l12;
  i1 = 5u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = func_state;
    i1 = l3;
    i2 = l4;
    i0 = EmitInstruction(ctx, i0, i1, i2);
    l7 = i0;
    i0 = 5u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    l13 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = func_state;
    i1 = func_state;
    i1 = LoadU32(ctx, i1 + 24u);
    StoreU32(ctx, i0 + 28u, i1);
    i0 = l5;
    i1 = kF707ConstFFFFFFFF;
    i0 = i0 == i1;
    l3 = i0;
    if (i0) {goto block_21;}
    i0 = func_state;
    i0 = LoadU32(ctx, i0 + 32u);
    l3 = i0;
    i1 = kF707ConstFFFFFFFF;
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
      i0 = func_state;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      l9 = i0;
      loop_51: 
        i0 = l3;
        l4 = i0;
        i0 = kF707ConstFFFFFFFF;
        i1 = l4;
        i2 = l9;
        i3 = l3;
        i4 = 2u;
        i3 <<= (i4 & 31);
        i2 += i3;
        l8 = i2;
        i2 = LoadU32(ctx, i2);
        l6 = i2;
        i3 = 14u;
        i2 >>= (i3 & 31);
        i3 = kF707Const1FFFF;
        i2 -= i3;
        l3 = i2;
        i1 += i2;
        i2 = 1u;
        i1 += i2;
        i2 = l3;
        i3 = kF707ConstFFFFFFFF;
        i2 = i2 == i3;
        i0 = i2 ? i0 : i1;
        l3 = i0;
        i1 = kF707ConstFFFFFFFF;
        i0 = i0 != i1;
        l14 = i0;
        if (i0) {goto loop_51;}
      i0 = l5;
      i1 = l4;
      i2 = kF707ConstFFFFFFFF;
      i1 ^= i2;
      i0 += i1;
      l3 = i0;
      i1 = 31u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l5 = i0;
      i0 = l5;
      i1 = l3;
      i2 = l5;
      i1 += i2;
      i0 ^= i1;
      i1 = kF707Const20000;
      i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
      l4 = i0;
      i0 = l8;
      l9 = i0;
    }
    i0 = l18;
    i1 = l4;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l18 = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = func_state;
        i0 = LoadU32(ctx, i0 + 12u);
        l4 = i0;
      }
      i0 = l12;
      i1 = 6u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l4;
        i1 = kF707MsgControlStructureTooLong;
        RaiseSyntaxError(ctx, i0, i1);
        i0 = 6u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l6;
      } else {
        i0 = l8;
        i0 = LoadU32(ctx, i0);
      }
      l6 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l9;
      i1 = l6;
      i2 = 16383u;
      i1 &= i2;
      l4 = i1;
      i2 = l3;
      i3 = 14u;
      i2 <<= (i3 & 31);
      i1 |= i2;
      i2 = kF707Const7FFFC000;
      i1 += i2;
      l3 = i1;
      StoreU32(ctx, i0, i1);
      goto block_21;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = func_state;
    i1 = l5;
    StoreU32(ctx, i0 + 32u, i1);
  }
  block_21:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = func_state;
    i1 = func_state;
    i1 = LoadU32(ctx, i1 + 24u);
    l14 = i1;
    StoreU32(ctx, i0 + 28u, i1);
    i0 = code;
    i0 = LoadU32(ctx, i0 + 20u);
    l3 = i0;
    i1 = kF707ConstFFFFFFFF;
    i0 = i0 == i1;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    if (i0) {goto block_59;}
    i0 = line;
    i1 = 255u;
    i0 = i0 == i1;
    l4 = i0;
  }
  i0 = l4;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    loop_63: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = func_state;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        i1 = l3;
        i2 = 2u;
        i1 <<= (i2 & 31);
        i0 += i1;
        l8 = i0;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i1 = 14u;
        i0 >>= (i1 & 31);
        i1 = kF707Const1FFFF;
        i0 -= i1;
        l6 = i0;
        i1 = kF707ConstFFFFFFFF;
        i0 = i0 == i1;
        l9 = i0;
        i0 = l3;
        i1 = l6;
        i0 += i1;
        i1 = 1u;
        i0 += i1;
        l11 = i0;
        i0 = l3;
        i1 = 1u;
        i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
        if (i0) {
          i0 = l8;
          i1 = 4u;
          i0 -= i1;
          l4 = i0;
          i0 = LoadU32(ctx, i0);
          l6 = i0;
          i1 = 63u;
          i0 &= i1;
          i1 = kF707Const2B460;
          i0 += i1;
          i0 = LoadS8(ctx, i0);
          i1 = 0u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          l15 = i0;
          if (i0) {goto block_65;}
        }
        i0 = l5;
        l6 = i0;
        i0 = l8;
        l4 = i0;
        block_65:;
        i0 = kF707ConstFFFFFFFF;
        i1 = l11;
        i2 = l9;
        i0 = i2 ? i0 : i1;
        l9 = i0;
        i0 = l6;
        i1 = 63u;
        i0 &= i1;
        i1 = 27u;
        i0 = i0 != i1;
        l11 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l11;
        if (i0) {
          i0 = l10;
          i1 = l3;
          i2 = kF707ConstFFFFFFFF;
          i1 ^= i2;
          i0 += i1;
          l3 = i0;
          i1 = 31u;
          i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
          l6 = i0;
          i0 = l6;
          i1 = l3;
          i2 = l6;
          i1 += i2;
          i0 ^= i1;
          i1 = kF707Const1FFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          l4 = i0;
          if (i0) {goto block_70;}
          goto block_67;
        }
        i0 = l4;
        i1 = l6;
        i2 = kF707Const7FC000;
        i1 &= i2;
        i2 = l6;
        i3 = 17u;
        i2 >>= (i3 & 31);
        i3 = 32704u;
        i2 &= i3;
        l6 = i2;
        i1 |= i2;
        i2 = 26u;
        i1 |= i2;
        StoreU32(ctx, i0, i1);
        i0 = l14;
        i1 = l3;
        i2 = kF707ConstFFFFFFFF;
        i1 ^= i2;
        i0 += i1;
        l3 = i0;
        i1 = 31u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        l5 = i0;
        i0 = l5;
        i1 = l3;
        i2 = l5;
        i1 += i2;
        i0 ^= i1;
        i1 = kF707Const20000;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l4 = i0;
        if (i0) {goto block_68;}
        block_70:;
        i0 = func_state;
        i0 = LoadU32(ctx, i0 + 12u);
        l4 = i0;
      }
      i0 = l12;
      i1 = 7u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l4;
        i1 = kF707MsgControlStructureTooLong;
        RaiseSyntaxError(ctx, i0, i1);
        i0 = 7u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      block_68:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
      }
      block_67:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i1 = l5;
        i2 = 16383u;
        i1 &= i2;
        i2 = l3;
        i3 = 14u;
        i2 <<= (i3 & 31);
        i1 |= i2;
        i2 = kF707Const7FFFC000;
        i1 += i2;
        StoreU32(ctx, i0, i1);
        i0 = l9;
        l3 = i0;
        i1 = kF707ConstFFFFFFFF;
        i0 = i0 != i1;
        l4 = i0;
        if (i0) {goto loop_63;}
      }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_59;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = line;
    i1 = 6u;
    i0 <<= (i1 & 31);
    l4 = i0;
    i1 = 16320u;
    i0 &= i1;
    l11 = i0;
  }
  loop_76: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = func_state;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      i1 = l3;
      i2 = 2u;
      i1 <<= (i2 & 31);
      i0 += i1;
      l8 = i0;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i1 = 14u;
      i0 >>= (i1 & 31);
      i1 = kF707Const1FFFF;
      i0 -= i1;
      l5 = i0;
      i1 = kF707ConstFFFFFFFF;
      i0 = i0 == i1;
      l9 = i0;
      i0 = l3;
      i1 = l5;
      i0 += i1;
      i1 = 1u;
      i0 += i1;
      l15 = i0;
      i0 = l3;
      i1 = 1u;
      i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
      if (i0) {
        i0 = l8;
        i1 = 4u;
        i0 -= i1;
        l4 = i0;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i1 = 63u;
        i0 &= i1;
        i1 = kF707Const2B460;
        i0 += i1;
        i0 = LoadS8(ctx, i0);
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        if (i0) {goto block_78;}
      }
      i0 = l6;
      l5 = i0;
      i0 = l8;
      l4 = i0;
      block_78:;
      i0 = kF707ConstFFFFFFFF;
      i1 = l15;
      i2 = l9;
      i0 = i2 ? i0 : i1;
      l9 = i0;
      i0 = l5;
      i1 = 63u;
      i0 &= i1;
      i1 = 27u;
      i0 = i0 == i1;
      l15 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l15;
      if (i0) {
        i0 = l4;
        i1 = l5;
        i2 = 23u;
        i1 >>= (i2 & 31);
        i2 = line;
        i1 = i1 != i2;
        l6 = i1;
        if (i1) {
          i1 = l11;
          i2 = l5;
          i3 = kF707ConstFFFFC03F;
          i2 &= i3;
          i1 |= i2;
        } else {
          i1 = l5;
          i2 = kF707Const7FC000;
          i1 &= i2;
          l6 = i1;
          i2 = l5;
          i3 = 17u;
          i2 >>= (i3 & 31);
          i3 = 32704u;
          i2 &= i3;
          i1 |= i2;
          i2 = 26u;
          i1 |= i2;
        }
        StoreU32(ctx, i0, i1);
        i0 = l14;
        i1 = l3;
        i2 = kF707ConstFFFFFFFF;
        i1 ^= i2;
        i0 += i1;
        l3 = i0;
        i1 = 31u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        l5 = i0;
        i0 = l5;
        i1 = l3;
        i2 = l5;
        i1 += i2;
        i0 ^= i1;
        i1 = kF707Const1FFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        l4 = i0;
        if (i0) {goto block_83;}
        goto block_81;
      }
      i0 = l10;
      i1 = l3;
      i2 = kF707ConstFFFFFFFF;
      i1 ^= i2;
      i0 += i1;
      l3 = i0;
      i1 = 31u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l5 = i0;
      i0 = l5;
      i1 = l3;
      i2 = l5;
      i1 += i2;
      i0 ^= i1;
      i1 = kF707Const20000;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l4 = i0;
      if (i0) {goto block_80;}
      block_83:;
      i0 = func_state;
      i0 = LoadU32(ctx, i0 + 12u);
      l4 = i0;
    }
    i0 = l12;
    i1 = 8u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l4;
      i1 = kF707MsgControlStructureTooLong;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 8u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    block_81:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l8;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
    }
    block_80:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l8;
      i1 = l6;
      i2 = 16383u;
      i1 &= i2;
      i2 = l3;
      i3 = 14u;
      i2 <<= (i3 & 31);
      i1 |= i2;
      i2 = kF707Const7FFFC000;
      i1 += i2;
      StoreU32(ctx, i0, i1);
      i0 = l9;
      l3 = i0;
      i1 = kF707ConstFFFFFFFF;
      i0 = i0 != i1;
      l4 = i0;
      if (i0) {goto loop_76;}
    }
  block_59:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = code;
    i0 = LoadU32(ctx, i0 + 16u);
    l3 = i0;
    i1 = kF707ConstFFFFFFFF;
    i0 = i0 == i1;
    if (i0) {goto block_19;}
    i0 = line;
    i1 = 255u;
    i0 = i0 == i1;
    l4 = i0;
  }
  i0 = l4;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    loop_92: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = func_state;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        i1 = l3;
        i2 = 2u;
        i1 <<= (i2 & 31);
        i0 += i1;
        l8 = i0;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i1 = 14u;
        i0 >>= (i1 & 31);
        i1 = kF707Const1FFFF;
        i0 -= i1;
        l6 = i0;
        i1 = kF707ConstFFFFFFFF;
        i0 = i0 == i1;
        l9 = i0;
        i0 = l3;
        i1 = l6;
        i0 += i1;
        i1 = 1u;
        i0 += i1;
        l10 = i0;
        i0 = l3;
        i1 = 1u;
        i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
        if (i0) {
          i0 = l8;
          i1 = 4u;
          i0 -= i1;
          l4 = i0;
          i0 = LoadU32(ctx, i0);
          l6 = i0;
          i1 = 63u;
          i0 &= i1;
          i1 = kF707Const2B460;
          i0 += i1;
          i0 = LoadS8(ctx, i0);
          i1 = 0u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          l11 = i0;
          if (i0) {goto block_94;}
        }
        i0 = l5;
        l6 = i0;
        i0 = l8;
        l4 = i0;
        block_94:;
        i0 = kF707ConstFFFFFFFF;
        i1 = l10;
        i2 = l9;
        i0 = i2 ? i0 : i1;
        l9 = i0;
        i0 = l6;
        i1 = 63u;
        i0 &= i1;
        i1 = 27u;
        i0 = i0 != i1;
        l10 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l10;
        if (i0) {
          i0 = l13;
          i1 = l3;
          i2 = kF707ConstFFFFFFFF;
          i1 ^= i2;
          i0 += i1;
          l3 = i0;
          i1 = 31u;
          i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
          l6 = i0;
          i0 = l6;
          i1 = l3;
          i2 = l6;
          i1 += i2;
          i0 ^= i1;
          i1 = kF707Const1FFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          l4 = i0;
          if (i0) {goto block_99;}
          goto block_96;
        }
        i0 = l4;
        i1 = l6;
        i2 = kF707Const7FC000;
        i1 &= i2;
        i2 = l6;
        i3 = 17u;
        i2 >>= (i3 & 31);
        i3 = 32704u;
        i2 &= i3;
        l6 = i2;
        i1 |= i2;
        i2 = 26u;
        i1 |= i2;
        StoreU32(ctx, i0, i1);
        i0 = l14;
        i1 = l3;
        i2 = kF707ConstFFFFFFFF;
        i1 ^= i2;
        i0 += i1;
        l3 = i0;
        i1 = 31u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        l5 = i0;
        i0 = l5;
        i1 = l3;
        i2 = l5;
        i1 += i2;
        i0 ^= i1;
        i1 = kF707Const20000;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l4 = i0;
        if (i0) {goto block_97;}
        block_99:;
        i0 = func_state;
        i0 = LoadU32(ctx, i0 + 12u);
        l4 = i0;
      }
      i0 = l12;
      i1 = 9u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l4;
        i1 = kF707MsgControlStructureTooLong;
        RaiseSyntaxError(ctx, i0, i1);
        i0 = 9u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      block_97:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
      }
      block_96:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i1 = l5;
        i2 = 16383u;
        i1 &= i2;
        i2 = l3;
        i3 = 14u;
        i2 <<= (i3 & 31);
        i1 |= i2;
        i2 = kF707Const7FFFC000;
        i1 += i2;
        StoreU32(ctx, i0, i1);
        i0 = l9;
        l3 = i0;
        i1 = kF707ConstFFFFFFFF;
        i0 = i0 != i1;
        l4 = i0;
        if (i0) {goto loop_92;}
      }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_19;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = line;
    i1 = 6u;
    i0 <<= (i1 & 31);
    l4 = i0;
    i1 = 16320u;
    i0 &= i1;
    l10 = i0;
  }
  loop_105: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = func_state;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      i1 = l3;
      i2 = 2u;
      i1 <<= (i2 & 31);
      i0 += i1;
      l8 = i0;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i1 = 14u;
      i0 >>= (i1 & 31);
      i1 = kF707Const1FFFF;
      i0 -= i1;
      l5 = i0;
      i1 = kF707ConstFFFFFFFF;
      i0 = i0 == i1;
      l9 = i0;
      i0 = l3;
      i1 = l5;
      i0 += i1;
      i1 = 1u;
      i0 += i1;
      l11 = i0;
      i0 = l3;
      i1 = 1u;
      i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
      if (i0) {
        i0 = l8;
        i1 = 4u;
        i0 -= i1;
        l4 = i0;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i1 = 63u;
        i0 &= i1;
        i1 = kF707Const2B460;
        i0 += i1;
        i0 = LoadS8(ctx, i0);
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        if (i0) {goto block_107;}
      }
      i0 = l6;
      l5 = i0;
      i0 = l8;
      l4 = i0;
      block_107:;
      i0 = kF707ConstFFFFFFFF;
      i1 = l11;
      i2 = l9;
      i0 = i2 ? i0 : i1;
      l9 = i0;
      i0 = l5;
      i1 = 63u;
      i0 &= i1;
      i1 = 27u;
      i0 = i0 == i1;
      l11 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l11;
      if (i0) {
        i0 = l4;
        i1 = l5;
        i2 = 23u;
        i1 >>= (i2 & 31);
        i2 = line;
        i1 = i1 != i2;
        l6 = i1;
        if (i1) {
          i1 = l10;
          i2 = l5;
          i3 = kF707ConstFFFFC03F;
          i2 &= i3;
          i1 |= i2;
        } else {
          i1 = l5;
          i2 = kF707Const7FC000;
          i1 &= i2;
          l6 = i1;
          i2 = l5;
          i3 = 17u;
          i2 >>= (i3 & 31);
          i3 = 32704u;
          i2 &= i3;
          i1 |= i2;
          i2 = 26u;
          i1 |= i2;
        }
        StoreU32(ctx, i0, i1);
        i0 = l14;
        i1 = l3;
        i2 = kF707ConstFFFFFFFF;
        i1 ^= i2;
        i0 += i1;
        l3 = i0;
        i1 = 31u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        l5 = i0;
        i0 = l5;
        i1 = l3;
        i2 = l5;
        i1 += i2;
        i0 ^= i1;
        i1 = kF707Const1FFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {goto block_112;}
        goto block_110;
      }
      i0 = l13;
      i1 = l3;
      i2 = kF707ConstFFFFFFFF;
      i1 ^= i2;
      i0 += i1;
      l3 = i0;
      i1 = 31u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l5 = i0;
      i0 = l5;
      i1 = l3;
      i2 = l5;
      i1 += i2;
      i0 ^= i1;
      i1 = kF707Const20000;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      if (i0) {goto block_109;}
      block_112:;
      i0 = func_state;
      i0 = LoadU32(ctx, i0 + 12u);
      l4 = i0;
    }
    i0 = l12;
    i1 = 10u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l4;
      i1 = kF707MsgControlStructureTooLong;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 10u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    block_110:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l8;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
    }
    block_109:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l8;
      i1 = l6;
      i2 = 16383u;
      i1 &= i2;
      i2 = l3;
      i3 = 14u;
      i2 <<= (i3 & 31);
      i1 |= i2;
      i2 = kF707Const7FFFC000;
      i1 += i2;
      StoreU32(ctx, i0, i1);
      i0 = l9;
      l3 = i0;
      i1 = kF707ConstFFFFFFFF;
      i0 = i0 != i1;
      l4 = i0;
      if (i0) {goto loop_105;}
    }
  block_19:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = code;
    j1 = 18446744073709551615ull;
    StoreU64(ctx, i0 + 16u, j1);
    i0 = code;
    i1 = line;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = code;
    i1 = 12u;
    StoreU32(ctx, i0, i1);
  }
  goto func_exit;
  spill_frame:;
  l7 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l7;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF707ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l7 = i0;
  i1 = func_state;
  StoreU32(ctx, i0, i1);
  i0 = l7;
  i1 = code;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l7;
  i1 = line;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l7;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l7;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l7;
  i1 = l5;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l7;
  i1 = l6;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l7;
  i1 = l8;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l7;
  i1 = l9;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = l7;
  i1 = l10;
  StoreU32(ctx, i0 + 36u, i1);
  i0 = l7;
  i1 = l11;
  StoreU32(ctx, i0 + 40u, i1);
  i0 = l7;
  i1 = l13;
  StoreU32(ctx, i0 + 44u, i1);
  i0 = l7;
  i1 = l14;
  StoreU32(ctx, i0 + 48u, i1);
  i0 = l7;
  i1 = l15;
  StoreU32(ctx, i0 + 52u, i1);
  i0 = l7;
  i1 = l16;
  StoreU32(ctx, i0 + 56u, i1);
  i0 = l7;
  i1 = l17;
  StoreU32(ctx, i0 + 60u, i1);
  i0 = l7;
  i1 = l18;
  StoreU32(ctx, i0 + 64u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 68u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
