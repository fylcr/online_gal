// lua_keywords.cpp -- the keyword table initialiser: one suspendable step that builds the table the lexer matches keywords against..
//
// LuaKeywords::Init -- the keyword table initialiser: one suspendable step that builds the table the lexer matches keywords against.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 2722264-2722264 (f5596); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t InternString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

constexpr uint32_t kF5596Const2AB8B = 0x2AB8Bu;  // 
constexpr uint32_t kF5596MsgBreak = 0x2AB8Fu;  // break
constexpr uint32_t kF5596Const2AB95 = 0x2AB95u;  // 
constexpr uint32_t kF5596MsgElse = 0x2AB98u;  // else
constexpr uint32_t kF5596MsgElseif = 0x2AB9Du;  // elseif
constexpr uint32_t kF5596Const2ABA4 = 0x2ABA4u;  // 
constexpr uint32_t kF5596MsgFalse = 0x2ABA8u;  // false
constexpr uint32_t kF5596Const2ABAE = 0x2ABAEu;  // 
constexpr uint32_t kF5596MsgFunction = 0x2ABB2u;  // function
constexpr uint32_t kF5596Const2ABBB = 0x2ABBBu;  // 
constexpr uint32_t kF5596Const2ABBE = 0x2ABBEu;  // 
constexpr uint32_t kF5596MsgLocal = 0x2ABC1u;  // local
constexpr uint32_t kF5596Const2ABC7 = 0x2ABC7u;  // 
constexpr uint32_t kF5596Const2ABCB = 0x2ABCBu;  // 
constexpr uint32_t kF5596Const2ABCF = 0x2ABCFu;  // 
constexpr uint32_t kF5596MsgRepeat = 0x2ABD2u;  // repeat
constexpr uint32_t kF5596MsgReturn = 0x2ABD9u;  // return
constexpr uint32_t kF5596MsgThen = 0x2ABE0u;  // then
constexpr uint32_t kF5596MsgTrue = 0x2ABE5u;  // true
constexpr uint32_t kF5596MsgUntil = 0x2ABEAu;  // until
constexpr uint32_t kF5596MsgWhile = 0x2ABF0u;  // while
constexpr uint32_t kF5596SaveFrameBytes = 8u;
constexpr uint32_t kF5596ResumeWordBytes = 4u;

}  // namespace

namespace LuaKeywords {
void Init(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF5596SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l2;
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
    i2 = kF5596ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
  }
  i0 = l3;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF5596Const2AB8B;
    i2 = 3u;
    i0 = InternString(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i1 = 1u;
    StoreU8(ctx, i0 + 6u, i1);
    i0 = l1;
    i1 = l1;
    i1 = LoadU8(ctx, i1 + 5u);
    i2 = 32u;
    i1 |= i2;
    StoreU8(ctx, i0 + 5u, i1);
  }
  i0 = l3;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF5596MsgBreak;
    i2 = 5u;
    i0 = InternString(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i1 = 2u;
    StoreU8(ctx, i0 + 6u, i1);
    i0 = l1;
    i1 = l1;
    i1 = LoadU8(ctx, i1 + 5u);
    i2 = 32u;
    i1 |= i2;
    StoreU8(ctx, i0 + 5u, i1);
  }
  i0 = l3;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF5596Const2AB95;
    i2 = 2u;
    i0 = InternString(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i1 = 3u;
    StoreU8(ctx, i0 + 6u, i1);
    i0 = l1;
    i1 = l1;
    i1 = LoadU8(ctx, i1 + 5u);
    i2 = 32u;
    i1 |= i2;
    StoreU8(ctx, i0 + 5u, i1);
  }
  i0 = l3;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF5596MsgElse;
    i2 = 4u;
    i0 = InternString(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i1 = 4u;
    StoreU8(ctx, i0 + 6u, i1);
    i0 = l1;
    i1 = l1;
    i1 = LoadU8(ctx, i1 + 5u);
    i2 = 32u;
    i1 |= i2;
    StoreU8(ctx, i0 + 5u, i1);
  }
  i0 = l3;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF5596MsgElseif;
    i2 = 6u;
    i0 = InternString(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i1 = 5u;
    StoreU8(ctx, i0 + 6u, i1);
    i0 = l1;
    i1 = l1;
    i1 = LoadU8(ctx, i1 + 5u);
    i2 = 32u;
    i1 |= i2;
    StoreU8(ctx, i0 + 5u, i1);
  }
  i0 = l3;
  i1 = 5u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF5596Const2ABA4;
    i2 = 3u;
    i0 = InternString(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 5u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i1 = 6u;
    StoreU8(ctx, i0 + 6u, i1);
    i0 = l1;
    i1 = l1;
    i1 = LoadU8(ctx, i1 + 5u);
    i2 = 32u;
    i1 |= i2;
    StoreU8(ctx, i0 + 5u, i1);
  }
  i0 = l3;
  i1 = 6u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF5596MsgFalse;
    i2 = 5u;
    i0 = InternString(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 6u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i1 = 7u;
    StoreU8(ctx, i0 + 6u, i1);
    i0 = l1;
    i1 = l1;
    i1 = LoadU8(ctx, i1 + 5u);
    i2 = 32u;
    i1 |= i2;
    StoreU8(ctx, i0 + 5u, i1);
  }
  i0 = l3;
  i1 = 7u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF5596Const2ABAE;
    i2 = 3u;
    i0 = InternString(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 7u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i1 = 8u;
    StoreU8(ctx, i0 + 6u, i1);
    i0 = l1;
    i1 = l1;
    i1 = LoadU8(ctx, i1 + 5u);
    i2 = 32u;
    i1 |= i2;
    StoreU8(ctx, i0 + 5u, i1);
  }
  i0 = l3;
  i1 = 8u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF5596MsgFunction;
    i2 = 8u;
    i0 = InternString(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 8u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i1 = 9u;
    StoreU8(ctx, i0 + 6u, i1);
    i0 = l1;
    i1 = l1;
    i1 = LoadU8(ctx, i1 + 5u);
    i2 = 32u;
    i1 |= i2;
    StoreU8(ctx, i0 + 5u, i1);
  }
  i0 = l3;
  i1 = 9u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF5596Const2ABBB;
    i2 = 2u;
    i0 = InternString(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 9u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i1 = 10u;
    StoreU8(ctx, i0 + 6u, i1);
    i0 = l1;
    i1 = l1;
    i1 = LoadU8(ctx, i1 + 5u);
    i2 = 32u;
    i1 |= i2;
    StoreU8(ctx, i0 + 5u, i1);
  }
  i0 = l3;
  i1 = 10u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF5596Const2ABBE;
    i2 = 2u;
    i0 = InternString(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 10u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i1 = 11u;
    StoreU8(ctx, i0 + 6u, i1);
    i0 = l1;
    i1 = l1;
    i1 = LoadU8(ctx, i1 + 5u);
    i2 = 32u;
    i1 |= i2;
    StoreU8(ctx, i0 + 5u, i1);
  }
  i0 = l3;
  i1 = 11u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF5596MsgLocal;
    i2 = 5u;
    i0 = InternString(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 11u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i1 = 12u;
    StoreU8(ctx, i0 + 6u, i1);
    i0 = l1;
    i1 = l1;
    i1 = LoadU8(ctx, i1 + 5u);
    i2 = 32u;
    i1 |= i2;
    StoreU8(ctx, i0 + 5u, i1);
  }
  i0 = l3;
  i1 = 12u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF5596Const2ABC7;
    i2 = 3u;
    i0 = InternString(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 12u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i1 = 13u;
    StoreU8(ctx, i0 + 6u, i1);
    i0 = l1;
    i1 = l1;
    i1 = LoadU8(ctx, i1 + 5u);
    i2 = 32u;
    i1 |= i2;
    StoreU8(ctx, i0 + 5u, i1);
  }
  i0 = l3;
  i1 = 13u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF5596Const2ABCB;
    i2 = 3u;
    i0 = InternString(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 13u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i1 = 14u;
    StoreU8(ctx, i0 + 6u, i1);
    i0 = l1;
    i1 = l1;
    i1 = LoadU8(ctx, i1 + 5u);
    i2 = 32u;
    i1 |= i2;
    StoreU8(ctx, i0 + 5u, i1);
  }
  i0 = l3;
  i1 = 14u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF5596Const2ABCF;
    i2 = 2u;
    i0 = InternString(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 14u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i1 = 15u;
    StoreU8(ctx, i0 + 6u, i1);
    i0 = l1;
    i1 = l1;
    i1 = LoadU8(ctx, i1 + 5u);
    i2 = 32u;
    i1 |= i2;
    StoreU8(ctx, i0 + 5u, i1);
  }
  i0 = l3;
  i1 = 15u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF5596MsgRepeat;
    i2 = 6u;
    i0 = InternString(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 15u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i1 = 16u;
    StoreU8(ctx, i0 + 6u, i1);
    i0 = l1;
    i1 = l1;
    i1 = LoadU8(ctx, i1 + 5u);
    i2 = 32u;
    i1 |= i2;
    StoreU8(ctx, i0 + 5u, i1);
  }
  i0 = l3;
  i1 = 16u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF5596MsgReturn;
    i2 = 6u;
    i0 = InternString(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 16u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i1 = 17u;
    StoreU8(ctx, i0 + 6u, i1);
    i0 = l1;
    i1 = l1;
    i1 = LoadU8(ctx, i1 + 5u);
    i2 = 32u;
    i1 |= i2;
    StoreU8(ctx, i0 + 5u, i1);
  }
  i0 = l3;
  i1 = 17u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF5596MsgThen;
    i2 = 4u;
    i0 = InternString(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 17u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i1 = 18u;
    StoreU8(ctx, i0 + 6u, i1);
    i0 = l1;
    i1 = l1;
    i1 = LoadU8(ctx, i1 + 5u);
    i2 = 32u;
    i1 |= i2;
    StoreU8(ctx, i0 + 5u, i1);
  }
  i0 = l3;
  i1 = 18u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF5596MsgTrue;
    i2 = 4u;
    i0 = InternString(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 18u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i1 = 19u;
    StoreU8(ctx, i0 + 6u, i1);
    i0 = l1;
    i1 = l1;
    i1 = LoadU8(ctx, i1 + 5u);
    i2 = 32u;
    i1 |= i2;
    StoreU8(ctx, i0 + 5u, i1);
  }
  i0 = l3;
  i1 = 19u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF5596MsgUntil;
    i2 = 5u;
    i0 = InternString(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 19u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i1 = 20u;
    StoreU8(ctx, i0 + 6u, i1);
    i0 = l1;
    i1 = l1;
    i1 = LoadU8(ctx, i1 + 5u);
    i2 = 32u;
    i1 |= i2;
    StoreU8(ctx, i0 + 5u, i1);
  }
  i0 = l3;
  i1 = 20u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF5596MsgWhile;
    i2 = 5u;
    i0 = InternString(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 20u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    a = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = 21u;
    StoreU8(ctx, i0 + 6u, i1);
    i0 = a;
    i1 = a;
    i1 = LoadU8(ctx, i1 + 5u);
    i2 = 32u;
    i1 |= i2;
    StoreU8(ctx, i0 + 5u, i1);
  }
  goto func_exit;
  spill_frame:;
  l2 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF5596ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l2 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l2;
  i1 = l1;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 8u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
}
