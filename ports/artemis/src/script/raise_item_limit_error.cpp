// raise_item_limit_error.cpp -- formats the "too many ..." item limit error at the current source position..
//
// RaiseItemLimitError -- formats the "too many ..." item limit error at the current source position.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 798460-798460 (f1715); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void MatchKeyword(EngineContext* ctx, uint32_t a, uint32_t b);
uint32_t FormatPushString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
void StepRecordResume(EngineContext* ctx, uint32_t a0);
uint32_t CheckStatementLimits(EngineContext* ctx, uint32_t state, uint32_t func_state, uint32_t message);
uint32_t FormatCharValue(EngineContext* ctx, uint32_t a, uint32_t b);
uint32_t ExpressionToRegister(EngineContext* ctx, uint32_t func_state, uint32_t expr);
void LexError(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t EmitOpInstruction(EngineContext* ctx, uint32_t func_state, uint32_t op, uint32_t arg_a, uint32_t arg_b, uint32_t line);
uint32_t EncodeConstant(EngineContext* ctx, uint32_t func_state, uint32_t value);

namespace {

constexpr uint32_t kF1715MsgSExpected = 0x2B625u;  // '%s' expected
constexpr uint32_t kF1715MsgItemsInAConstructor = 0x2B6A9u;  // items in a constructor
constexpr uint32_t kF1715MsgMainFunctionHasMore = 0x2B6C0u;  // main function has more than %d %s
constexpr uint32_t kF1715MsgFunctionAtLineD = 0x2B6E2u;  // function at line %d has more than %d %s
constexpr uint32_t kF1715Const7FFFFFFD = 0x7FFFFFFDu;  // 
constexpr uint32_t kF1715Const7FFFFFFE = 0x7FFFFFFEu;  // 
constexpr uint32_t kF1715ConstFFFFFFC0 = 0xFFFFFFC0u;  // 
constexpr uint32_t kF1715SaveFrameBytes = 28u;
constexpr uint32_t kF1715ResumeWordBytes = 4u;

}  // namespace

void RaiseItemLimitError(EngineContext* ctx, uint32_t state, uint32_t limit) {
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
    i2 = kF1715SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l7 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 4u);
    limit = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 20u);
    l8 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 24u);
    l7 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1715ResumeWordBytes;
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
    i1 = 112u;
    i0 -= i1;
    l5 = i0;
    GuestStackSp(ctx) = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    l8 = i0;
    i0 = LoadU32(ctx, i0 + 36u);
    l7 = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = 285u;
    i0 = i0 == i1;
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
      i0 = limit;
      i0 = LoadU32(ctx, i0 + 28u);
      i1 = kF1715Const7FFFFFFE;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l3;
    } else {
      i0 = l2;
      if (i0) {goto block_8;}
      i0 = l8;
      i0 = LoadU32(ctx, i0 + 16u);
      l2 = i0;
      i0 = l8;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 60u);
      l4 = i0;
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
        i1 = kF1715MsgItemsInAConstructor;
        StoreU32(ctx, i0 + 36u, i1);
        i0 = l5;
        i1 = kF1715Const7FFFFFFD;
        StoreU32(ctx, i0 + 32u, i1);
        i0 = l5;
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
        i0 = l2;
        i1 = kF1715MsgMainFunctionHasMore;
        i2 = l4;
        i0 = FormatPushString(ctx, i0, i1, i2);
        l3 = i0;
        i0 = 0u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l3;
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_9;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i1 = kF1715MsgItemsInAConstructor;
      StoreU32(ctx, i0 + 56u, i1);
      i0 = l5;
      i1 = kF1715Const7FFFFFFD;
      StoreU32(ctx, i0 + 52u, i1);
      i0 = l5;
      i1 = l4;
      StoreU32(ctx, i0 + 48u, i1);
      i0 = l5;
      i1 = 48u;
      i0 += i1;
      l4 = i0;
    }
    i0 = l6;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = kF1715MsgFunctionAtLineD;
      i2 = l4;
      i0 = FormatPushString(ctx, i0, i1, i2);
      l3 = i0;
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l3;
      l2 = i0;
    }
    block_9:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l8;
      i0 = LoadU32(ctx, i0 + 12u);
      l4 = i0;
    }
    i0 = l6;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l4;
      i1 = l2;
      i2 = 0u;
      LexError(ctx, i0, i1, i2);
      i0 = 2u;
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
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_8;}
      i0 = state;
      i0 = LoadU32(ctx, i0 + 52u);
      l2 = i0;
    }
    i0 = l6;
    i1 = 3u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = 285u;
      i0 = FormatCharValue(ctx, i0, i1);
      l3 = i0;
      i0 = 3u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l3;
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i1 = l4;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l5;
      i1 = 16u;
      i0 += i1;
      l4 = i0;
    }
    i0 = l6;
    i1 = 4u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = kF1715MsgSExpected;
      i2 = l4;
      i0 = FormatPushString(ctx, i0, i1, i2);
      l3 = i0;
      i0 = 4u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l3;
      l2 = i0;
    }
    i0 = l6;
    i1 = 5u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l2;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 5u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    block_8:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 24u);
      l2 = i0;
    }
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
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 48u);
      l4 = i0;
    }
    i0 = l6;
    i1 = 7u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l4;
      i1 = l2;
      i0 = EncodeConstant(ctx, i0, i1);
      l3 = i0;
      i0 = 7u;
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
      StoreU64(ctx, i0 + 104u, j1);
      i0 = l5;
      i1 = l2;
      StoreU32(ctx, i0 + 96u, i1);
      i0 = l5;
      i1 = 4u;
      StoreU32(ctx, i0 + 88u, i1);
      goto block_3;
    }
  }
  i0 = l2;
  i1 = l5;
  i2 = 88u;
  i1 += i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l2 = i0;
  i0 = l6;
  i1 = 8u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l2;
    MatchKeyword(ctx, i0, i1);
    i0 = 8u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_3:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = limit;
    i1 = limit;
    i1 = LoadU32(ctx, i1 + 28u);
    i2 = 1u;
    i1 += i2;
    StoreU32(ctx, i0 + 28u, i1);
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = 61u;
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
    i0 = l6;
    i1 = 9u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = 61u;
      i0 = FormatCharValue(ctx, i0, i1);
      l3 = i0;
      i0 = 9u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l3;
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i1 = l4;
      StoreU32(ctx, i0, i1);
    }
    i0 = l6;
    i1 = 10u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = kF1715MsgSExpected;
      i2 = l5;
      i0 = FormatPushString(ctx, i0, i1, i2);
      l3 = i0;
      i0 = 10u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l3;
      l2 = i0;
    }
    i0 = l6;
    i1 = 11u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l2;
      RaiseSyntaxError(ctx, i0, i1);
      i0 = 11u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
  }
  i0 = l6;
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
  i0 = l2;
  i1 = l5;
  i2 = 88u;
  i1 += i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l2 = i0;
  i0 = l6;
  i1 = 13u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l8;
    i1 = l2;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l3 = i0;
    i0 = 13u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    l2 = i0;
  }
  i0 = l4;
  i1 = l5;
  i2 = kF1715ConstFFFFFFC0;
  i1 -= i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l4 = i0;
  i0 = l6;
  i1 = 14u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l4;
    i2 = 0u;
    i0 = CheckStatementLimits(ctx, i0, i1, i2);
    l3 = i0;
    i0 = 14u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    state = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = limit;
    i0 = LoadU32(ctx, i0 + 24u);
    i0 = LoadU32(ctx, i0 + 8u);
    state = i0;
    i0 = l5;
    i1 = kF1715ConstFFFFFFC0;
    i0 -= i1;
    limit = i0;
  }
  i0 = l6;
  i1 = 15u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l8;
    i1 = limit;
    i0 = ExpressionToRegister(ctx, i0, i1);
    l3 = i0;
    i0 = 15u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    limit = i0;
  }
  i0 = l6;
  i1 = 16u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l8;
    i1 = 9u;
    i2 = state;
    i3 = l2;
    i4 = limit;
    i0 = EmitOpInstruction(ctx, i0, i1, i2, i3, i4);
    i0 = 16u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l8;
    i1 = l7;
    StoreU32(ctx, i0 + 36u, i1);
    i0 = l5;
    i1 = 112u;
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
  i2 = kF1715ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = limit;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l3;
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l3;
  i1 = l5;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l3;
  i1 = l8;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l3;
  i1 = l7;
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
