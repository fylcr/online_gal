// lua_tag_generic_del.cpp -- the delon* tag registry: matches the tag name against the delonautomodein ..
//
// TagGenericDelOn -- the delon* tag registry: matches the tag name against the delonautomodein .. delonwindowbutton handler table, erases the stored script block and releases the sound object slot it owned.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 1804237-1804237 (f3650); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void FindOrInsertSoundObjectSlot(EngineContext* ctx, uint32_t a, uint32_t b);
void Panic(EngineContext* ctx, uint32_t a0);
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void ReleaseNodeTree(EngineContext* ctx, uint32_t state, uint32_t node);
uint32_t SkipWhitespace(EngineContext* ctx, uint32_t text);
uint32_t CombineComparisonResults(EngineContext* ctx, uint32_t left, uint32_t right, uint32_t result, uint32_t box);
uint32_t EraseTaggedTreeEntry(EngineContext* ctx, uint32_t a, uint32_t b);

namespace {

constexpr uint32_t kF3650Const8C564 = 0x8C564u;  // 
constexpr uint32_t kF3650MsgDelonclick = 0xAF6A3u;  // delonclick
constexpr uint32_t kF3650MsgDelonrollover = 0xAF6BCu;  // delonrollover
constexpr uint32_t kF3650MsgDelonrollout = 0xAF6D7u;  // delonrollout
constexpr uint32_t kF3650MsgDelondragin = 0xAF6F0u;  // delondragin
constexpr uint32_t kF3650MsgDelondrag = 0xAF706u;  // delondrag
constexpr uint32_t kF3650MsgDelondragout = 0xAF71Du;  // delondragout
constexpr uint32_t kF3650MsgDelonpush = 0xAF734u;  // delonpush
constexpr uint32_t kF3650MsgDelonautomodein = 0xAF74Eu;  // delonautomodein
constexpr uint32_t kF3650MsgDelonautomodeout = 0xAF76Fu;  // delonautomodeout
constexpr uint32_t kF3650MsgDeloncommandskipin = 0xAF793u;  // deloncommandskipin
constexpr uint32_t kF3650MsgDeloncommandskipout = 0xAF7BAu;  // deloncommandskipout
constexpr uint32_t kF3650MsgDeloncontrolskipin = 0xAF7E1u;  // deloncontrolskipin
constexpr uint32_t kF3650MsgDeloncontrolskipout = 0xAF808u;  // deloncontrolskipout
constexpr uint32_t kF3650MsgDelonhidein = 0xAF828u;  // delonhidein
constexpr uint32_t kF3650MsgDelonhideout = 0xAF841u;  // delonhideout
constexpr uint32_t kF3650MsgDelonbacklogin = 0xAF85Du;  // delonbacklogin
constexpr uint32_t kF3650MsgDelonbacklogout = 0xAF87Cu;  // delonbacklogout
constexpr uint32_t kF3650MsgDelonwindowbutton = 0xAF89Eu;  // delonwindowbutton
constexpr uint32_t kF3650MsgDelondirchg = 0xAF8BCu;  // delondirchg
constexpr uint32_t kF3650MsgDelonvideofinish = 0xAF8D9u;  // delonvideofinish
constexpr uint32_t kF3650MsgDelonsoundfinish = 0xAF8FBu;  // delonsoundfinish
constexpr uint32_t kF3650ConstB53F7 = 0xB53F7u;  // 
constexpr uint32_t kF3650ConstB53F9 = 0xB53F9u;  // 
constexpr uint32_t kF3650MsgButton = 0xB53FBu;  // button
constexpr uint32_t kF3650ConstB53FF = 0xB53FFu;  // 
constexpr uint32_t kF3650ConstB5E30 = 0xB5E30u;  // 
constexpr uint32_t kF3650MsgPx = 0xB5E31u;  // px != 0
constexpr uint32_t kF3650MsgBoostBoostSmartPtr = 0xB5E39u;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
constexpr uint32_t kF3650MsgOperator = 0xB5E71u;  // operator->
constexpr uint32_t kF3650ConstB5F40 = 0xB5F40u;  // 
constexpr uint32_t kF3650ConstB8F8C = 0xB8F8Cu;  // 
constexpr uint32_t kF3650ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF3650ConstFFFFFFC0 = 0xFFFFFFC0u;  // 
constexpr uint32_t kF3650ConstFFFFFFFE = 0xFFFFFFFEu;  // 
constexpr uint32_t kF3650ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF3650SaveFrameBytes = 40u;
constexpr uint32_t kF3650ResumeWordBytes = 4u;

}  // namespace

namespace LuaEngine {
void TagGenericDelOn(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u; uint32_t i6 = 0u;
  uint64_t j1 = 0;
      l11 = 0, l12 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF3650SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l8 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 24u);
    l6 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 28u);
    l9 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 32u);
    l10 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 36u);
    l11 = i0;
    i0 = l8;
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
    i2 = kF3650ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l7 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    i1 = 192u;
    i0 -= i1;
    l3 = i0;
    GuestStackSp(ctx) = i0;
    i0 = b;
    i0 = LoadU8(ctx, i0 + 15u);
    l6 = i0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    l5 = i0;
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l10 = i0;
    i0 = b;
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
    i1 = l6;
    i2 = l10;
    i0 = i2 ? i0 : i1;
    i1 = 10u;
    i0 = i0 == i1;
    l10 = i0;
    i0 = b;
    i1 = kF3650ResumeWordBytes;
    i0 += i1;
    l9 = i0;
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
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = l9;
      i2 = 0u;
      i3 = kF3650ConstFFFFFFFF;
      i4 = kF3650MsgDelonclick;
      i5 = 10u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l8 = i0;
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l4;
    } else {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_5;}
      i0 = l4;
      i0 = !(i0);
      l5 = i0;
      if (i0) {goto block_18;}
      i0 = b;
      i0 = LoadU8(ctx, i0 + 15u);
      l6 = i0;
      l5 = i0;
      i0 = b;
      i0 = LoadU32(ctx, i0 + 8u);
    }
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l6;
    i2 = l5;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 13u;
    i0 = i0 == i1;
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
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = l9;
      i2 = 0u;
      i3 = kF3650ConstFFFFFFFF;
      i4 = kF3650MsgDelonrollover;
      i5 = 13u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l8 = i0;
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l4;
    } else {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_5;}
      i0 = l4;
      i0 = !(i0);
      l5 = i0;
      if (i0) {goto block_18;}
      i0 = b;
      i0 = LoadU8(ctx, i0 + 15u);
      l6 = i0;
      l5 = i0;
      i0 = b;
      i0 = LoadU32(ctx, i0 + 8u);
    }
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l6;
    i2 = l5;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 12u;
    i0 = i0 == i1;
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
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = l9;
      i2 = 0u;
      i3 = kF3650ConstFFFFFFFF;
      i4 = kF3650MsgDelonrollout;
      i5 = 12u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l8 = i0;
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l4;
    } else {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_5;}
      i0 = l4;
      i0 = !(i0);
      l5 = i0;
      if (i0) {goto block_18;}
      i0 = b;
      i0 = LoadU8(ctx, i0 + 15u);
      l6 = i0;
      l5 = i0;
      i0 = b;
      i0 = LoadU32(ctx, i0 + 8u);
    }
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l6;
    i2 = l5;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 11u;
    i0 = i0 == i1;
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
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 3u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = l9;
      i2 = 0u;
      i3 = kF3650ConstFFFFFFFF;
      i4 = kF3650MsgDelondragin;
      i5 = 11u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l8 = i0;
      i0 = 3u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l4;
    } else {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_5;}
      i0 = l4;
      i0 = !(i0);
      l5 = i0;
      if (i0) {goto block_18;}
      i0 = b;
      i0 = LoadU8(ctx, i0 + 15u);
      l6 = i0;
      l5 = i0;
      i0 = b;
      i0 = LoadU32(ctx, i0 + 8u);
    }
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l6;
    i2 = l5;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 9u;
    i0 = i0 == i1;
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
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 4u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = l9;
      i2 = 0u;
      i3 = kF3650ConstFFFFFFFF;
      i4 = kF3650MsgDelondrag;
      i5 = 9u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l8 = i0;
      i0 = 4u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l4;
    } else {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_5;}
      i0 = l4;
      i0 = !(i0);
      l5 = i0;
      if (i0) {goto block_18;}
      i0 = b;
      i0 = LoadU8(ctx, i0 + 15u);
      l6 = i0;
      l5 = i0;
      i0 = b;
      i0 = LoadU32(ctx, i0 + 8u);
    }
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l6;
    i2 = l5;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 12u;
    i0 = i0 == i1;
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
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 5u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = l9;
      i2 = 0u;
      i3 = kF3650ConstFFFFFFFF;
      i4 = kF3650MsgDelondragout;
      i5 = 12u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l8 = i0;
      i0 = 5u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l4;
    } else {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_16;}
      i0 = l4;
      i0 = !(i0);
      l5 = i0;
      if (i0) {goto block_18;}
      i0 = b;
      i0 = LoadU8(ctx, i0 + 15u);
      l6 = i0;
      l5 = i0;
      i0 = b;
      i0 = LoadU32(ctx, i0 + 8u);
    }
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l6;
    i2 = l5;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 9u;
    i0 = i0 == i1;
    c = i0;
    if (i0) {goto block_17;}
    goto block_11;
  }
  block_18:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = !(i0);
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
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 6u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3650MsgPx;
      i2 = kF3650MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3650MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 6u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_10;}
      goto block_5;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 24u);
    l4 = i0;
    i0 = !(i0);
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
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 7u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3650MsgPx;
      i2 = kF3650MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3650MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 7u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_10;}
      goto block_5;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 2u, i1);
    i0 = l3;
    i1 = 25705u;
    StoreU16(ctx, i0, i1);
    i0 = l3;
    i1 = 2u;
    StoreU8(ctx, i0 + 11u, i1);
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    StoreU32(ctx, i0 + 176u, i1);
    i0 = b;
    i1 = 16u;
    i0 += i1;
    l10 = i0;
    i0 = l3;
    i1 = 176u;
    i0 += i1;
    l6 = i0;
    i0 = l3;
    i1 = 168u;
    i0 += i1;
    l11 = i0;
    i0 = l3;
    i1 = 184u;
    i0 += i1;
    l5 = i0;
  }
  i0 = l7;
  i1 = 8u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = l5;
    i2 = l10;
    i3 = l3;
    i4 = kF3650ConstB5E30;
    i5 = l6;
    i6 = l11;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 8u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 == i1;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    if (i0) {goto block_70;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 184u);
    l5 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 304u);
    l6 = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 160u;
    i0 += i1;
    l11 = i0;
    i0 = l5;
    i1 = 28u;
    i0 += i1;
    l5 = i0;
  }
  i0 = l7;
  i1 = 9u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = l11;
    i2 = l4;
    i3 = l5;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 9u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    l5 = i0;
    if (i0) {goto block_70;}
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF3650ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 160u);
    i0 = !(i0);
    l5 = i0;
    if (i0) {goto block_12;}
    i0 = b;
    i0 = LoadU8(ctx, i0 + 15u);
    l6 = i0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    l5 = i0;
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l11 = i0;
    i0 = b;
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
    i1 = l6;
    i2 = l11;
    i0 = i2 ? i0 : i1;
    i1 = 10u;
    i0 = i0 == i1;
    l11 = i0;
    if (i0) {goto block_69;}
    goto block_15;
  }
  block_70:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l5 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF3650ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_8;}
    goto running_exit;
  }
  block_69:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 10u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l9;
    i2 = 0u;
    i3 = kF3650ConstFFFFFFFF;
    i4 = kF3650MsgDelonclick;
    i5 = 10u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l8 = i0;
    i0 = 10u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 == i1;
    l5 = i0;
    if (i0) {goto block_5;}
    i0 = l4;
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0 + 8u);
      l4 = i0;
      i0 = b;
      i0 = LoadU8(ctx, i0 + 15u);
      l6 = i0;
      l5 = i0;
      goto block_15;
    }
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 160u);
    l4 = i0;
    i0 = !(i0);
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
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 11u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3650MsgPx;
      i2 = kF3650MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3650MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 11u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_10;}
      goto block_5;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 0u;
    StoreU32(ctx, i0 + 144u, i1);
    i0 = l4;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 272u);
    l5 = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 144u;
    i0 += i1;
    l6 = i0;
  }
  i0 = l7;
  i1 = 12u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l5;
    i1 = l4;
    i2 = 0u;
    i3 = l6;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 12u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
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
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 144u);
      l4 = i0;
      i0 = !(i0);
      l5 = i0;
      if (i0) {goto block_14;}
      i0 = l4;
      i1 = 1u;
      i0 &= i1;
      l5 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      if (i0) {goto block_92;}
      i0 = l4;
      i1 = kF3650ConstFFFFFFFE;
      i0 &= i1;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = !(i0);
      l5 = i0;
      if (i0) {goto block_92;}
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      l5 = i0;
      i0 = l3;
      i1 = 144u;
      i0 += i1;
      l6 = i0;
      i1 = 4u;
      i0 |= i1;
      l4 = i0;
    }
    i0 = l7;
    i1 = 13u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l5;
      i1 = l4;
      i2 = l4;
      i3 = 2u;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 13u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_91;}
    }
    block_92:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 144u, i1);
      goto block_14;
    }
    block_91:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_5;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l5 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 144u);
    a = i0;
    i0 = !(i0);
    b = i0;
    if (i0) {goto block_13;}
    i0 = a;
    i1 = 1u;
    i0 &= i1;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    if (i0) {goto block_99;}
    i0 = a;
    i1 = kF3650ConstFFFFFFFE;
    i0 &= i1;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = !(i0);
    b = i0;
    if (i0) {goto block_99;}
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    b = i0;
    i0 = l3;
    i1 = 144u;
    i0 += i1;
    i1 = 4u;
    i0 |= i1;
    a = i0;
  }
  i0 = l7;
  i1 = 14u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = a;
    i2 = a;
    i3 = 2u;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 14u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_98;}
  }
  block_99:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 0u;
    StoreU32(ctx, i0 + 144u, i1);
    goto block_13;
  }
  block_98:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_5;}
  block_17:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 15u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l9;
    i2 = 0u;
    i3 = kF3650ConstFFFFFFFF;
    i4 = kF3650MsgDelonpush;
    i5 = 9u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l8 = i0;
    i0 = 15u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    l5 = i0;
    if (i0) {goto block_5;}
    i0 = c;
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0 + 8u);
      l4 = i0;
      i0 = b;
      i0 = LoadU8(ctx, i0 + 15u);
      l6 = i0;
      l5 = i0;
      goto block_11;
    }
    i0 = l3;
    i1 = 3u;
    StoreU8(ctx, i0 + 11u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 3u, i1);
    i0 = l3;
    i1 = kF3650ConstB53F7;
    i1 = LoadU16(ctx, i1);
    StoreU16(ctx, i0, i1);
    i0 = l3;
    i1 = kF3650ConstB53F9;
    i1 = LoadU8(ctx, i1);
    StoreU8(ctx, i0 + 2u, i1);
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    StoreU32(ctx, i0 + 160u, i1);
    i0 = l3;
    i1 = 184u;
    i0 += i1;
    c = i0;
    i0 = l3;
    i1 = 160u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = 176u;
    i0 += i1;
    l6 = i0;
    i0 = b;
    i1 = 16u;
    i0 += i1;
    b = i0;
  }
  i0 = l7;
  i1 = 16u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = c;
    i2 = b;
    i3 = l3;
    i4 = kF3650ConstB5E30;
    i5 = l5;
    i6 = l6;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 16u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 184u);
      c = i0;
      i1 = 28u;
      i0 += i1;
      b = i0;
      i0 = c;
      i0 = LoadS8(ctx, i0 + 39u);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
      } else {
        i0 = b;
      }
      i0 = SkipWhitespace(ctx, i0);
      c = i0;
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 11u);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = a;
      i1 = 13744u;
      i0 += i1;
      b = i0;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = !(i0);
      if (i0) {goto block_114;}
      i0 = l5;
      l4 = i0;
      loop_115: 
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 16u);
        l6 = i0;
        i1 = c;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = c;
          i1 = l6;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {goto block_116;}
          i0 = l4;
          i1 = 4u;
          i0 += i1;
        } else {
          i0 = l4;
        }
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        if (i0) {goto loop_115;}
        goto block_114;
        block_116:;
      i0 = b;
      l4 = i0;
      loop_118: 
        i0 = l4;
        i1 = l5;
        i2 = l5;
        i2 = LoadU32(ctx, i2 + 16u);
        i3 = c;
        i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
        l6 = i2;
        i0 = i2 ? i0 : i1;
        l4 = i0;
        i0 = l6;
        i1 = 2u;
        i0 <<= (i1 & 31);
        i1 = l5;
        i0 += i1;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        if (i0) {goto loop_118;}
      i0 = b;
      i1 = l4;
      i0 = i0 == i1;
      if (i0) {goto block_114;}
      i0 = c;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 16u);
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      if (i0) {goto block_114;}
      i0 = a;
      i1 = 13740u;
      i0 += i1;
      i1 = l4;
      i0 = EraseTaggedTreeEntry(ctx, i0, i1);
      block_114:;
      i0 = a;
      i1 = 13756u;
      i0 += i1;
      b = i0;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = !(i0);
      if (i0) {goto block_6;}
      i0 = l5;
      l4 = i0;
      loop_119: 
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 16u);
        l6 = i0;
        i1 = c;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = c;
          i1 = l6;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {goto block_110;}
          i0 = l4;
          i1 = 4u;
          i0 += i1;
        } else {
          i0 = l4;
        }
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        if (i0) {goto loop_119;}
      goto block_6;
    }
    i0 = HostRet0(ctx);
    l5 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF3650ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_8;}
    goto running_exit;
    block_110:;
    i0 = b;
    l4 = i0;
    loop_121: 
      i0 = l4;
      i1 = l5;
      i2 = l5;
      i2 = LoadU32(ctx, i2 + 16u);
      i3 = c;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      l6 = i2;
      i0 = i2 ? i0 : i1;
      l4 = i0;
      i0 = l6;
      i1 = 2u;
      i0 <<= (i1 & 31);
      i1 = l5;
      i0 += i1;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      if (i0) {goto loop_121;}
    i0 = b;
    i1 = l4;
    i0 = i0 == i1;
    if (i0) {goto block_6;}
    i0 = c;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 16u);
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {goto block_6;}
    i0 = a;
    i1 = 13752u;
    i0 += i1;
    i1 = l4;
    i0 = EraseTaggedTreeEntry(ctx, i0, i1);
    a = i0;
    goto block_6;
  }
  block_16:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    a = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
  }
  i0 = l7;
  i1 = 17u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 17u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_10;}
  block_15:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l6;
    i2 = l5;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 13u;
    i0 = i0 != i1;
    l11 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l11;
    if (i0) {goto block_125;}
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 18u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l9;
    i2 = 0u;
    i3 = kF3650ConstFFFFFFFF;
    i4 = kF3650MsgDelonrollover;
    i5 = 13u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l8 = i0;
    i0 = 18u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
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
      i0 = l4;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l4 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l6 = i0;
        l5 = i0;
        goto block_125;
      }
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 160u);
      l4 = i0;
      i0 = !(i0);
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
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 19u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3650MsgPx;
        i2 = kF3650MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3650MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 19u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3650ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_10;}
        goto block_5;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 128u, i1);
      i0 = l4;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 272u);
      l5 = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 128u;
      i0 += i1;
      l6 = i0;
    }
    i0 = l7;
    i1 = 20u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l5;
      i1 = l4;
      i2 = 1u;
      i3 = l6;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 20u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
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
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 128u);
        l4 = i0;
        i0 = !(i0);
        l5 = i0;
        if (i0) {goto block_14;}
        i0 = l4;
        i1 = 1u;
        i0 &= i1;
        l5 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        if (i0) {goto block_145;}
        i0 = l4;
        i1 = kF3650ConstFFFFFFFE;
        i0 &= i1;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i0 = !(i0);
        l5 = i0;
        if (i0) {goto block_145;}
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        l5 = i0;
        i0 = l3;
        i1 = 128u;
        i0 += i1;
        l6 = i0;
        i1 = 4u;
        i0 |= i1;
        l4 = i0;
      }
      i0 = l7;
      i1 = 21u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l5;
        i1 = l4;
        i2 = l4;
        i3 = 2u;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 21u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3650ConstD015C;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 1u;
        i0 = i0 == i1;
        l5 = i0;
        if (i0) {goto block_144;}
      }
      block_145:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i1 = 0u;
        StoreU32(ctx, i0 + 128u, i1);
        goto block_14;
      }
      block_144:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_5;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      l5 = i0;
      i0 = HostTempRet0(ctx);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 128u);
      a = i0;
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_13;}
      i0 = a;
      i1 = 1u;
      i0 &= i1;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      if (i0) {goto block_152;}
      i0 = a;
      i1 = kF3650ConstFFFFFFFE;
      i0 &= i1;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_152;}
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      b = i0;
      i0 = l3;
      i1 = 128u;
      i0 += i1;
      i1 = 4u;
      i0 |= i1;
      a = i0;
    }
    i0 = l7;
    i1 = 22u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = a;
      i2 = a;
      i3 = 2u;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 22u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_151;}
    }
    block_152:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 128u, i1);
      goto block_13;
    }
    block_151:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_5;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_5;}
  block_125:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l6;
    i2 = l5;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 12u;
    i0 = i0 != i1;
    l11 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l11;
    if (i0) {goto block_158;}
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 23u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l9;
    i2 = 0u;
    i3 = kF3650ConstFFFFFFFF;
    i4 = kF3650MsgDelonrollout;
    i5 = 12u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l8 = i0;
    i0 = 23u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
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
      i0 = l4;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l4 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l6 = i0;
        l5 = i0;
        goto block_158;
      }
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 160u);
      l4 = i0;
      i0 = !(i0);
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
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 24u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3650MsgPx;
        i2 = kF3650MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3650MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 24u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3650ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_10;}
        goto block_5;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 112u, i1);
      i0 = l4;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 272u);
      l5 = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 112u;
      i0 += i1;
      l6 = i0;
    }
    i0 = l7;
    i1 = 25u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l5;
      i1 = l4;
      i2 = 2u;
      i3 = l6;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 25u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
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
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 112u);
        l4 = i0;
        i0 = !(i0);
        l5 = i0;
        if (i0) {goto block_14;}
        i0 = l4;
        i1 = 1u;
        i0 &= i1;
        l5 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        if (i0) {goto block_178;}
        i0 = l4;
        i1 = kF3650ConstFFFFFFFE;
        i0 &= i1;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i0 = !(i0);
        l5 = i0;
        if (i0) {goto block_178;}
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        l5 = i0;
        i0 = l3;
        i1 = 112u;
        i0 += i1;
        l6 = i0;
        i1 = 4u;
        i0 |= i1;
        l4 = i0;
      }
      i0 = l7;
      i1 = 26u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l5;
        i1 = l4;
        i2 = l4;
        i3 = 2u;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 26u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3650ConstD015C;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 1u;
        i0 = i0 == i1;
        l5 = i0;
        if (i0) {goto block_177;}
      }
      block_178:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i1 = 0u;
        StoreU32(ctx, i0 + 112u, i1);
        goto block_14;
      }
      block_177:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_5;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      l5 = i0;
      i0 = HostTempRet0(ctx);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 112u);
      a = i0;
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_13;}
      i0 = a;
      i1 = 1u;
      i0 &= i1;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      if (i0) {goto block_185;}
      i0 = a;
      i1 = kF3650ConstFFFFFFFE;
      i0 &= i1;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_185;}
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      b = i0;
      i0 = l3;
      i1 = 112u;
      i0 += i1;
      i1 = 4u;
      i0 |= i1;
      a = i0;
    }
    i0 = l7;
    i1 = 27u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = a;
      i2 = a;
      i3 = 2u;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 27u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_184;}
    }
    block_185:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 112u, i1);
      goto block_13;
    }
    block_184:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_5;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_5;}
  block_158:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l6;
    i2 = l5;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 11u;
    i0 = i0 != i1;
    l11 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l11;
    if (i0) {goto block_191;}
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 28u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l9;
    i2 = 0u;
    i3 = kF3650ConstFFFFFFFF;
    i4 = kF3650MsgDelondragin;
    i5 = 11u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l8 = i0;
    i0 = 28u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
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
      i0 = l4;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l4 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l6 = i0;
        l5 = i0;
        goto block_191;
      }
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 160u);
      l4 = i0;
      i0 = !(i0);
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
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 29u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3650MsgPx;
        i2 = kF3650MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3650MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 29u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3650ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_10;}
        goto block_5;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 96u, i1);
      i0 = l4;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 272u);
      l5 = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 96u;
      i0 += i1;
      l6 = i0;
    }
    i0 = l7;
    i1 = 30u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l5;
      i1 = l4;
      i2 = 3u;
      i3 = l6;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 30u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
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
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 96u);
        l4 = i0;
        i0 = !(i0);
        l5 = i0;
        if (i0) {goto block_14;}
        i0 = l4;
        i1 = 1u;
        i0 &= i1;
        l5 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        if (i0) {goto block_211;}
        i0 = l4;
        i1 = kF3650ConstFFFFFFFE;
        i0 &= i1;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i0 = !(i0);
        l5 = i0;
        if (i0) {goto block_211;}
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        l5 = i0;
        i0 = l3;
        i1 = 96u;
        i0 += i1;
        l6 = i0;
        i1 = 4u;
        i0 |= i1;
        l4 = i0;
      }
      i0 = l7;
      i1 = 31u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l5;
        i1 = l4;
        i2 = l4;
        i3 = 2u;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 31u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3650ConstD015C;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 1u;
        i0 = i0 == i1;
        l5 = i0;
        if (i0) {goto block_210;}
      }
      block_211:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i1 = 0u;
        StoreU32(ctx, i0 + 96u, i1);
        goto block_14;
      }
      block_210:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_5;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      l5 = i0;
      i0 = HostTempRet0(ctx);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 96u);
      a = i0;
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_13;}
      i0 = a;
      i1 = 1u;
      i0 &= i1;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      if (i0) {goto block_218;}
      i0 = a;
      i1 = kF3650ConstFFFFFFFE;
      i0 &= i1;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_218;}
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      b = i0;
      i0 = l3;
      i1 = 96u;
      i0 += i1;
      i1 = 4u;
      i0 |= i1;
      a = i0;
    }
    i0 = l7;
    i1 = 32u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = a;
      i2 = a;
      i3 = 2u;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 32u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_217;}
    }
    block_218:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 96u, i1);
      goto block_13;
    }
    block_217:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_5;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_5;}
  block_191:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l6;
    i2 = l5;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 9u;
    i0 = i0 != i1;
    l11 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l11;
    if (i0) {goto block_224;}
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 33u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l9;
    i2 = 0u;
    i3 = kF3650ConstFFFFFFFF;
    i4 = kF3650MsgDelondrag;
    i5 = 9u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l8 = i0;
    i0 = 33u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
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
      i0 = l4;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l4 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l6 = i0;
        l5 = i0;
        goto block_224;
      }
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 160u);
      l4 = i0;
      i0 = !(i0);
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
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 34u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3650MsgPx;
        i2 = kF3650MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3650MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 34u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3650ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_10;}
        goto block_5;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 80u, i1);
      i0 = l4;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 272u);
      l5 = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 80u;
      i0 += i1;
      l6 = i0;
    }
    i0 = l7;
    i1 = 35u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l5;
      i1 = l4;
      i2 = 4u;
      i3 = l6;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 35u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
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
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 80u);
        l4 = i0;
        i0 = !(i0);
        l5 = i0;
        if (i0) {goto block_14;}
        i0 = l4;
        i1 = 1u;
        i0 &= i1;
        l5 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        if (i0) {goto block_244;}
        i0 = l4;
        i1 = kF3650ConstFFFFFFFE;
        i0 &= i1;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i0 = !(i0);
        l5 = i0;
        if (i0) {goto block_244;}
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        l5 = i0;
        i0 = l3;
        i1 = 80u;
        i0 += i1;
        l6 = i0;
        i1 = 4u;
        i0 |= i1;
        l4 = i0;
      }
      i0 = l7;
      i1 = 36u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l5;
        i1 = l4;
        i2 = l4;
        i3 = 2u;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 36u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3650ConstD015C;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 1u;
        i0 = i0 == i1;
        l5 = i0;
        if (i0) {goto block_243;}
      }
      block_244:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i1 = 0u;
        StoreU32(ctx, i0 + 80u, i1);
        goto block_14;
      }
      block_243:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_5;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      l5 = i0;
      i0 = HostTempRet0(ctx);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 80u);
      a = i0;
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_13;}
      i0 = a;
      i1 = 1u;
      i0 &= i1;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      if (i0) {goto block_251;}
      i0 = a;
      i1 = kF3650ConstFFFFFFFE;
      i0 &= i1;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_251;}
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      b = i0;
      i0 = l3;
      i1 = 80u;
      i0 += i1;
      i1 = 4u;
      i0 |= i1;
      a = i0;
    }
    i0 = l7;
    i1 = 37u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = a;
      i2 = a;
      i3 = 2u;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 37u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_250;}
    }
    block_251:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 80u, i1);
      goto block_13;
    }
    block_250:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_5;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_5;}
  block_224:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l6;
    i2 = l5;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 12u;
    i0 = i0 != i1;
    l5 = i0;
    if (i0) {goto block_14;}
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 38u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l9;
    i2 = 0u;
    i3 = kF3650ConstFFFFFFFF;
    i4 = kF3650MsgDelondragout;
    i5 = 12u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l8 = i0;
    i0 = 38u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
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
      i0 = l4;
      if (i0) {goto block_14;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 160u);
      l4 = i0;
      i0 = !(i0);
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
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 39u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3650MsgPx;
        i2 = kF3650MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3650MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 39u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3650ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_10;}
        goto block_5;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 64u, i1);
      i0 = l4;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 272u);
      l5 = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = kF3650ConstFFFFFFC0;
      i0 -= i1;
      l6 = i0;
    }
    i0 = l7;
    i1 = 40u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l5;
      i1 = l4;
      i2 = 5u;
      i3 = l6;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 40u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
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
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 64u);
        l4 = i0;
        i0 = !(i0);
        l5 = i0;
        if (i0) {goto block_14;}
        i0 = l4;
        i1 = 1u;
        i0 &= i1;
        l5 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        if (i0) {goto block_274;}
        i0 = l4;
        i1 = kF3650ConstFFFFFFFE;
        i0 &= i1;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i0 = !(i0);
        l5 = i0;
        if (i0) {goto block_274;}
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        l5 = i0;
        i0 = l3;
        i1 = kF3650ConstFFFFFFC0;
        i0 -= i1;
        l6 = i0;
        i1 = 4u;
        i0 |= i1;
        l4 = i0;
      }
      i0 = l7;
      i1 = 41u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l5;
        i1 = l4;
        i2 = l4;
        i3 = 2u;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 41u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3650ConstD015C;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 1u;
        i0 = i0 == i1;
        l5 = i0;
        if (i0) {goto block_273;}
      }
      block_274:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i1 = 0u;
        StoreU32(ctx, i0 + 64u, i1);
        goto block_14;
      }
      block_273:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_5;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      l5 = i0;
      i0 = HostTempRet0(ctx);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 64u);
      a = i0;
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_13;}
      i0 = a;
      i1 = 1u;
      i0 &= i1;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      if (i0) {goto block_281;}
      i0 = a;
      i1 = kF3650ConstFFFFFFFE;
      i0 &= i1;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_281;}
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      b = i0;
      i0 = l3;
      i1 = kF3650ConstFFFFFFC0;
      i0 -= i1;
      i1 = 4u;
      i0 |= i1;
      a = i0;
    }
    i0 = l7;
    i1 = 42u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = a;
      i2 = a;
      i3 = 2u;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 42u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_280;}
    }
    block_281:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 64u, i1);
      goto block_13;
    }
    block_280:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_5;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_5;}
  block_14:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    if (i0) {goto block_12;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 128u);
    a = i0;
    i0 = !(i0);
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
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 43u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3650MsgPx;
      i2 = kF3650MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3650MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 43u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_10;}
      goto block_5;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 2u, i1);
    i0 = l3;
    i1 = 25705u;
    StoreU16(ctx, i0, i1);
    i0 = l3;
    i1 = 2u;
    StoreU8(ctx, i0 + 11u, i1);
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    StoreU32(ctx, i0 + 176u, i1);
    i0 = l3;
    i1 = 176u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = 168u;
    i0 += i1;
    l6 = i0;
    i0 = l3;
    i1 = 184u;
    i0 += i1;
    c = i0;
  }
  i0 = l7;
  i1 = 44u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = c;
    i2 = l10;
    i3 = l3;
    i4 = kF3650ConstB5E30;
    i5 = l5;
    i6 = l6;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 44u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    c = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = 1u;
    i0 = i0 == i1;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    if (i0) {goto block_296;}
    i0 = l3;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 184u);
    i2 = 28u;
    i1 += i2;
    c = i1;
    StoreU32(ctx, i0 + 176u, i1);
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 184u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = 176u;
    i0 += i1;
    l6 = i0;
    i0 = l3;
    i1 = 168u;
    i0 += i1;
    l4 = i0;
    i0 = a;
    i1 = 76u;
    i0 += i1;
    a = i0;
  }
  i0 = l7;
  i1 = 45u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2613u;
    i1 = l5;
    i2 = a;
    i3 = c;
    i4 = kF3650ConstB5E30;
    i5 = l6;
    i6 = l4;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 45u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_296;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 184u);
    a = i0;
    i0 = LoadU32(ctx, i0 + 32u);
    c = i0;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 36u);
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
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 46u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2591u;
      i1 = c;
      i2 = b;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l8 = i0;
      i0 = 46u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {
        i0 = HostRet0(ctx);
        l5 = i0;
        i0 = HostTempRet0(ctx);
        b = i0;
        i0 = a;
        i1 = c;
        StoreU32(ctx, i0 + 32u, i1);
        goto block_295;
      }
      i0 = a;
      i1 = b;
      i2 = 44u;
      i1 += i2;
      b = i1;
      StoreU32(ctx, i0 + 32u, i1);
      goto block_299;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 28u;
    i0 += i1;
    a = i0;
  }
  i0 = l7;
  i1 = 47u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2614u;
    i1 = a;
    i2 = b;
    HostOsSchedule(ctx, i0, i1, i2);
    i0 = 47u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_296;}
  }
  block_299:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF3650ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_12;}
    i0 = l3;
    i0 = LoadU32(ctx, i0);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_12;
  }
  block_296:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l5 = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
  }
  block_295:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF3650ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_13;}
    i0 = l3;
    i0 = LoadU32(ctx, i0);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_13:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 164u);
    a = i0;
    i0 = !(i0);
    b = i0;
    if (i0) {goto block_8;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 4u);
    b = i1;
    i2 = 1u;
    i1 -= i2;
    c = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_8;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 48u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = a;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 48u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
  }
  i0 = b;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 8u);
      b = i1;
      i2 = 1u;
      i1 -= i2;
      c = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_8;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      b = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 49u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 49u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_8;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_5;}
  block_12:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 164u);
    a = i0;
    i0 = !(i0);
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    if (i0) {goto block_322;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 4u);
    b = i1;
    i2 = 1u;
    i1 -= i2;
    c = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_322;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 50u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = a;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 50u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
  }
  i0 = b;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 8u);
      b = i1;
      i2 = 1u;
      i1 -= i2;
      c = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_322;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      b = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 51u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 51u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_322;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_5;}
  block_322:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 192u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_11:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l6;
    i2 = l5;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 16u;
    i0 = i0 != i1;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    if (i0) {goto block_333;}
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 52u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l9;
    i2 = 0u;
    i3 = kF3650ConstFFFFFFFF;
    i4 = kF3650MsgDelonvideofinish;
    i5 = 16u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l8 = i0;
    i0 = 52u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
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
      i0 = c;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l4 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l6 = i0;
        l5 = i0;
        goto block_333;
      }
      i0 = a;
      i0 = LoadU32(ctx, i0 + 16u);
      a = i0;
      i0 = !(i0);
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
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 53u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3650MsgPx;
        i2 = kF3650MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3650MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 53u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3650ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_10;}
        goto block_5;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 24u);
      a = i0;
      i0 = !(i0);
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
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 54u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3650MsgPx;
        i2 = kF3650MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3650MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 54u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3650ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_10;}
        goto block_5;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 2u, i1);
      i0 = l3;
      i1 = 25705u;
      StoreU16(ctx, i0, i1);
      i0 = l3;
      i1 = 2u;
      StoreU8(ctx, i0 + 11u, i1);
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = l3;
      StoreU32(ctx, i0 + 176u, i1);
      i0 = l3;
      i1 = 184u;
      i0 += i1;
      c = i0;
      i0 = l3;
      i1 = 176u;
      i0 += i1;
      l5 = i0;
      i0 = l3;
      i1 = 168u;
      i0 += i1;
      l6 = i0;
      i0 = b;
      i1 = 16u;
      i0 += i1;
      b = i0;
    }
    i0 = l7;
    i1 = 55u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1959u;
      i1 = c;
      i2 = b;
      i3 = l3;
      i4 = kF3650ConstB5E30;
      i5 = l5;
      i6 = l6;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 55u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      if (i0) {goto block_359;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 184u);
      b = i0;
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 304u);
      c = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 160u;
      i0 += i1;
      l5 = i0;
      i0 = b;
      i1 = 28u;
      i0 += i1;
      b = i0;
    }
    i0 = l7;
    i1 = 56u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l5;
      i2 = a;
      i3 = b;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 56u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_359;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 160u);
      a = i0;
      i0 = !(i0);
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      if (i0) {
        i0 = 0u;
        l5 = i0;
        i0 = 0u;
        l6 = i0;
        goto block_363;
      }
      i0 = 0u;
      l6 = i0;
    }
    i0 = l7;
    i1 = 57u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = kF3650Const8C564;
      i2 = kF3650ConstB8F8C;
      i3 = 0u;
      i0 = CombineComparisonResults(ctx, i0, i1, i2, i3);
      l8 = i0;
      i0 = 57u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l5 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i0 = !(i0);
      a = i0;
      if (i0) {
        i0 = 0u;
        l5 = i0;
        goto block_363;
      }
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 164u);
      a = i0;
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_363;}
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 4u);
      i2 = 1u;
      i1 += i2;
      b = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = a;
      l6 = i0;
    }
    block_363:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 164u);
      a = i0;
      i0 = !(i0);
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      if (i0) {goto block_370;}
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 4u);
      b = i1;
      i2 = 1u;
      i1 -= i2;
      c = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_370;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      b = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 58u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 58u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_5;}
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 8u);
      b = i1;
      i2 = 1u;
      i1 -= i2;
      c = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_370;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      b = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 59u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 59u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_5;}
    }
    block_370:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 11u);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l5;
      i0 = !(i0);
      a = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      if (i0) {goto block_378;}
      i0 = l5;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 372u);
      a = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 60u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = l3;
      i2 = l5;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 60u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_357;}
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = !(i0);
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      if (i0) {goto block_382;}
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 48u, i1);
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 24u);
      b = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 48u;
      i0 += i1;
      c = i0;
    }
    i0 = l7;
    i1 = 61u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = a;
      i2 = c;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 61u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_358;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 48u);
      a = i0;
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_382;}
      i0 = a;
      i1 = 1u;
      i0 &= i1;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      if (i0) {goto block_386;}
      i0 = a;
      i1 = kF3650ConstFFFFFFFE;
      i0 &= i1;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_386;}
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      b = i0;
      i0 = l3;
      i1 = 48u;
      i0 += i1;
      i1 = 4u;
      i0 |= i1;
      a = i0;
    }
    i0 = l7;
    i1 = 62u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = a;
      i2 = a;
      i3 = 2u;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 62u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_5;}
    }
    block_386:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 48u, i1);
    }
    block_382:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 4u);
      a = i0;
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_378;}
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 4u);
      b = i1;
      i2 = 1u;
      i1 -= i2;
      c = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_378;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      b = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 63u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 63u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_5;}
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 8u);
      b = i1;
      i2 = 1u;
      i1 -= i2;
      c = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_378;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      b = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 64u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 64u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_5;}
    }
    block_378:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i0 = !(i0);
      a = i0;
      if (i0) {goto block_6;}
      i0 = l6;
      i1 = l6;
      i1 = LoadU32(ctx, i1 + 4u);
      a = i1;
      i2 = 1u;
      i1 -= i2;
      b = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_6;}
      i0 = l6;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      a = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 65u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = l6;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 65u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
    }
    i0 = a;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        i1 = l6;
        i1 = LoadU32(ctx, i1 + 8u);
        a = i1;
        i2 = 1u;
        i1 -= i2;
        b = i1;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_6;}
        i0 = l6;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        a = i0;
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 66u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l6;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 66u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3650ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_6;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_5;}
    block_359:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      l5 = i0;
      i0 = HostTempRet0(ctx);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 11u);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_8;}
      goto running_exit;
    }
    block_358:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      l5 = i0;
      i0 = HostTempRet0(ctx);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 48u);
      a = i0;
    }
    i0 = a;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = b;
      i1 = a;
      i2 = 1u;
      i1 &= i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      b = i0;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        if (i0) {goto block_407;}
        i0 = a;
        i1 = kF3650ConstFFFFFFFE;
        i0 &= i1;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = !(i0);
        b = i0;
        if (i0) {goto block_407;}
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        b = i0;
        i0 = l3;
        i1 = 48u;
        i0 += i1;
        i1 = 4u;
        i0 |= i1;
        a = i0;
      }
      i0 = l7;
      i1 = 67u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = b;
        i1 = a;
        i2 = a;
        i3 = 2u;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 67u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3650ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 == i1;
        a = i0;
        if (i0) {goto block_336;}
      }
      block_407:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i1 = 0u;
        StoreU32(ctx, i0 + 48u, i1);
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 4u);
      a = i0;
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_356;}
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 4u);
      b = i1;
      i2 = 1u;
      i1 -= i2;
      c = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_356;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      b = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 68u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 68u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
    }
    i0 = b;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 8u);
        b = i1;
        i2 = 1u;
        i1 -= i2;
        c = i1;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 != i1;
        b = i0;
        if (i0) {goto block_356;}
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        b = i0;
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 69u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = b;
        i1 = a;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 69u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3650ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_356;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_5;}
    block_357:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      l5 = i0;
      i0 = HostTempRet0(ctx);
      a = i0;
    }
    block_356:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i0 = !(i0);
      a = i0;
      if (i0) {goto block_8;}
      i0 = l6;
      i1 = l6;
      i1 = LoadU32(ctx, i1 + 4u);
      a = i1;
      i2 = 1u;
      i1 -= i2;
      b = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_8;}
      i0 = l6;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      a = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 70u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = l6;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 70u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
    }
    i0 = a;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        i1 = l6;
        i1 = LoadU32(ctx, i1 + 8u);
        a = i1;
        i2 = 1u;
        i1 -= i2;
        b = i1;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_8;}
        i0 = l6;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        a = i0;
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 71u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l6;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 71u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3650ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_8;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_5;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_5;}
  block_336:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_5;}
  block_333:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l6;
    i2 = l5;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 16u;
    i0 = i0 != i1;
    c = i0;
    if (i0) {goto block_9;}
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 72u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l9;
    i2 = 0u;
    i3 = kF3650ConstFFFFFFFF;
    i4 = kF3650MsgDelonsoundfinish;
    i5 = 16u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l8 = i0;
    i0 = 72u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    l5 = i0;
    if (i0) {goto block_5;}
    i0 = c;
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0 + 8u);
      l4 = i0;
      i0 = b;
      i0 = LoadU8(ctx, i0 + 15u);
      l6 = i0;
      l5 = i0;
      goto block_9;
    }
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 2u, i1);
    i0 = l3;
    i1 = 25705u;
    StoreU16(ctx, i0, i1);
    i0 = l3;
    i1 = 2u;
    StoreU8(ctx, i0 + 11u, i1);
    i0 = b;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
  }
  i0 = l6;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    loop_435: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        i0 = LoadU8(ctx, i0 + 27u);
        c = i0;
        i1 = 24u;
        i0 <<= (i1 & 31);
        i1 = 24u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l5 = i0;
        i0 = l6;
        i0 = LoadU32(ctx, i0 + 20u);
        i1 = c;
        i2 = l5;
        i0 = i2 ? i0 : i1;
        c = i0;
        i1 = 2u;
        i0 = i0 < i1;
        l9 = i0;
        i0 = c;
        i1 = 2u;
        i2 = l9;
        i0 = i2 ? i0 : i1;
        l4 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        if (i0) {
          i0 = l3;
          i1 = l6;
          i2 = 16u;
          i1 += i2;
          l10 = i1;
          i1 = LoadU32(ctx, i1);
          i2 = l10;
          i3 = l5;
          i1 = i3 ? i1 : i2;
          l5 = i1;
          i2 = l4;
          i0 = CompareBytes(ctx, i0, i1, i2);
          l10 = i0;
          i0 = !(i0);
          if (i0) {
            i0 = c;
            i1 = 2u;
            i0 = i0 <= i1;
            c = i0;
            if (i0) {goto block_443;}
            goto block_437;
          }
          i0 = l10;
          i1 = kF3650ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          c = i0;
          if (i0) {goto block_437;}
          block_443:;
          i0 = l5;
          i1 = l3;
          i2 = l4;
          i0 = CompareBytes(ctx, i0, i1, i2);
          c = i0;
          i0 = !(i0);
          l5 = i0;
          if (i0) {goto block_441;}
          i0 = c;
          i1 = kF3650ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          c = i0;
          if (i0) {goto block_440;}
          goto block_438;
        }
        i0 = c;
        i1 = 2u;
        i0 = i0 > i1;
        c = i0;
        if (i0) {goto block_437;}
        block_441:;
        i0 = l9;
        if (i0) {goto block_438;}
        block_440:;
        i0 = l3;
        i1 = 0u;
        StoreU8(ctx, i0 + 2u, i1);
        i0 = l3;
        i1 = 25705u;
        StoreU16(ctx, i0, i1);
        i0 = l3;
        i1 = 2u;
        StoreU8(ctx, i0 + 11u, i1);
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = l3;
        StoreU32(ctx, i0 + 160u, i1);
        i0 = l3;
        i1 = 184u;
        i0 += i1;
        c = i0;
        i0 = b;
        i1 = 16u;
        i0 += i1;
        l10 = i0;
        i0 = l3;
        i1 = 176u;
        i0 += i1;
        l5 = i0;
        i0 = l3;
        i1 = 160u;
        i0 += i1;
        b = i0;
      }
      i0 = l7;
      i1 = 73u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1959u;
        i1 = c;
        i2 = l10;
        i3 = l3;
        i4 = kF3650ConstB5E30;
        i5 = b;
        i6 = l5;
        HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
        i0 = 73u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3650ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3650ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = 1u;
        c = i0;
        i0 = b;
        i1 = 1u;
        i0 = i0 != i1;
        b = i0;
      }
      i0 = b;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = a;
          i1 = 14348u;
          i0 += i1;
          i0 = LoadU32(ctx, i0);
          l6 = i0;
          i0 = !(i0);
          if (i0) {goto block_450;}
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 184u);
          l4 = i0;
          i0 = LoadU32(ctx, i0 + 32u);
          l9 = i0;
          i0 = l4;
          i0 = LoadU8(ctx, i0 + 39u);
          b = i0;
          i1 = 24u;
          i0 <<= (i1 & 31);
          i1 = 24u;
          i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
          i1 = 0u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          l5 = i0;
          i0 = l9;
          i1 = b;
          i2 = l5;
          i0 = i2 ? i0 : i1;
          b = i0;
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 28u);
          i1 = l4;
          i2 = 28u;
          i1 += i2;
          i2 = l5;
          i0 = i2 ? i0 : i1;
          l5 = i0;
          loop_451: 
            i0 = l6;
            i0 = LoadU8(ctx, i0 + 27u);
            l4 = i0;
            i1 = 24u;
            i0 <<= (i1 & 31);
            i1 = 24u;
            i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
            i1 = 0u;
            i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
            l11 = i0;
            i0 = l6;
            i0 = LoadU32(ctx, i0 + 20u);
            i1 = l4;
            i2 = l11;
            i0 = i2 ? i0 : i1;
            l4 = i0;
            i1 = b;
            i0 = i0 < i1;
            l12 = i0;
            i0 = l4;
            i1 = b;
            i2 = l12;
            i0 = i2 ? i0 : i1;
            l9 = i0;
            if (i0) {
              i0 = l5;
              i1 = l6;
              i2 = 16u;
              i1 += i2;
              l8 = i1;
              i1 = LoadU32(ctx, i1);
              i2 = l8;
              i3 = l11;
              i1 = i3 ? i1 : i2;
              l11 = i1;
              i2 = l9;
              i0 = CompareBytes(ctx, i0, i1, i2);
              l8 = i0;
              i0 = !(i0);
              if (i0) {
                i0 = b;
                i1 = l4;
                i0 = i0 >= i1;
                l4 = i0;
                if (i0) {goto block_456;}
                goto block_452;
              }
              i0 = l8;
              i1 = kF3650ConstFFFFFFFF;
              i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
              l4 = i0;
              if (i0) {goto block_452;}
              block_456:;
              i0 = l11;
              i1 = l5;
              i2 = l9;
              i0 = CompareBytes(ctx, i0, i1, i2);
              l4 = i0;
              i0 = !(i0);
              l9 = i0;
              if (i0) {goto block_454;}
              i0 = l4;
              i1 = kF3650ConstFFFFFFFF;
              i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
              l4 = i0;
              if (i0) {goto block_453;}
              i0 = 0u;
              c = i0;
              goto block_450;
            }
            i0 = b;
            i1 = l4;
            i0 = i0 < i1;
            l4 = i0;
            if (i0) {goto block_452;}
            block_454:;
            i0 = l12;
            if (i0) {goto block_453;}
            i0 = 0u;
            c = i0;
            goto block_450;
            block_453:;
            i0 = l6;
            i1 = 4u;
            i0 += i1;
            l6 = i0;
            block_452:;
            i0 = l6;
            i0 = LoadU32(ctx, i0);
            l6 = i0;
            if (i0) {goto loop_451;}
          block_450:;
          i0 = l3;
          i0 = LoadS8(ctx, i0 + 11u);
          i1 = kF3650ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l3;
            i0 = LoadU32(ctx, i0);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = c;
          if (i0) {goto block_6;}
          i0 = a;
          i0 = LoadU32(ctx, i0 + 14336u);
          b = i0;
          i0 = !(i0);
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
            i0 = kF3650ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l7;
          i1 = 74u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kF3650MsgPx;
            i2 = kF3650MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF3650MsgOperator;
            HostFailAssertion(ctx, i0, i1, i2, i3, i4);
            i0 = 74u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3650ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3650ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_10;}
            goto block_5;
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i1 = 0u;
          StoreU8(ctx, i0 + 2u, i1);
          i0 = l3;
          i1 = 25705u;
          StoreU16(ctx, i0, i1);
          i0 = l3;
          i1 = 2u;
          StoreU8(ctx, i0 + 11u, i1);
          i0 = kF3650ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = l3;
          StoreU32(ctx, i0 + 160u, i1);
          i0 = l3;
          i1 = 160u;
          i0 += i1;
          l5 = i0;
          i0 = l3;
          i1 = 176u;
          i0 += i1;
          l6 = i0;
          i0 = l3;
          i1 = 184u;
          i0 += i1;
          c = i0;
        }
        i0 = l7;
        i1 = 75u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1959u;
          i1 = c;
          i2 = l10;
          i3 = l3;
          i4 = kF3650ConstB5E30;
          i5 = l5;
          i6 = l6;
          HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
          i0 = 75u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3650ConstD015C;
          i0 = LoadU32(ctx, i0);
          c = i0;
          i0 = kF3650ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = c;
          i1 = 1u;
          i0 = i0 == i1;
          c = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = c;
          if (i0) {goto block_466;}
          i0 = l3;
          i1 = l3;
          i1 = LoadU32(ctx, i1 + 184u);
          i2 = 28u;
          i1 += i2;
          c = i1;
          StoreU32(ctx, i0 + 160u, i1);
          i0 = kF3650ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = 184u;
          i0 += i1;
          l5 = i0;
          i0 = l3;
          i1 = 160u;
          i0 += i1;
          l6 = i0;
          i0 = l3;
          i1 = 176u;
          i0 += i1;
          l4 = i0;
          i0 = a;
          i1 = 14344u;
          i0 += i1;
          a = i0;
        }
        i0 = l7;
        i1 = 76u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2609u;
          i1 = l5;
          i2 = a;
          i3 = c;
          i4 = kF3650ConstB5E30;
          i5 = l6;
          i6 = l4;
          HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
          i0 = 76u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3650ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3650ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 == i1;
          a = i0;
          if (i0) {goto block_466;}
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 184u);
          i0 = LoadU32(ctx, i0 + 28u);
          a = i0;
          i0 = kF3650ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l7;
        i1 = 77u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2667u;
          i1 = b;
          i2 = a;
          HostOsSchedule(ctx, i0, i1, i2);
          i0 = 77u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3650ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3650ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 == i1;
          a = i0;
          if (i0) {goto block_466;}
          i0 = l3;
          i0 = LoadS8(ctx, i0 + 11u);
          i1 = kF3650ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_6;}
          i0 = l3;
          i0 = LoadU32(ctx, i0);
          a = i0;
          DeallocateRawBlock(ctx, i0);
          goto block_6;
        }
        block_466:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          l5 = i0;
          i0 = HostTempRet0(ctx);
          i0 = l3;
          i0 = LoadS8(ctx, i0 + 11u);
          i1 = kF3650ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_8;}
          goto running_exit;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        l5 = i0;
        i0 = HostTempRet0(ctx);
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 11u);
        i1 = kF3650ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        a = i0;
        if (i0) {goto block_8;}
        goto running_exit;
      }
      block_438:;
      i0 = l6;
      i1 = l6;
      i2 = 4u;
      i1 += i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      l6 = i0;
      block_437:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        i0 = LoadU32(ctx, i0);
        l6 = i0;
        if (i0) {goto loop_435;}
      }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 14320u);
    b = i0;
    i0 = !(i0);
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
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 78u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3650MsgPx;
      i2 = kF3650MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3650MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 78u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_10;}
      goto block_5;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 14328u);
    a = i0;
  }
  i0 = l7;
  i1 = 79u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = a;
    FindOrInsertSoundObjectSlot(ctx, i0, i1);
    i0 = 79u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_6;}
  block_10:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    TrapUnreachable();
  }
  block_9:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l6;
    i2 = l5;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 17u;
    i0 = i0 != i1;
    c = i0;
    if (i0) {goto block_7;}
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 80u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l9;
    i2 = 0u;
    i3 = kF3650ConstFFFFFFFF;
    i4 = kF3650MsgDelonwindowbutton;
    i5 = 17u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l8 = i0;
    i0 = 80u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
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
      i0 = c;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l4 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l6 = i0;
        l5 = i0;
        goto block_7;
      }
      i0 = l3;
      i1 = 6u;
      StoreU8(ctx, i0 + 11u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 6u, i1);
      i0 = l3;
      i1 = kF3650ConstB53FF;
      i1 = LoadU16(ctx, i1);
      StoreU16(ctx, i0 + 4u, i1);
      i0 = l3;
      i1 = kF3650MsgButton;
      i1 = LoadU32(ctx, i1);
      c = i1;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i0 = LoadU32(ctx, i0 + 20u);
      l6 = i0;
    }
    i0 = l6;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      loop_497: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l6;
          i0 = LoadU8(ctx, i0 + 27u);
          c = i0;
          i1 = 24u;
          i0 <<= (i1 & 31);
          i1 = 24u;
          i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
          i1 = 0u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          l5 = i0;
          i0 = l6;
          i0 = LoadU32(ctx, i0 + 20u);
          i1 = c;
          i2 = l5;
          i0 = i2 ? i0 : i1;
          c = i0;
          i1 = 6u;
          i0 = i0 < i1;
          l9 = i0;
          i0 = c;
          i1 = 6u;
          i2 = l9;
          i0 = i2 ? i0 : i1;
          l4 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          if (i0) {
            i0 = l3;
            i1 = l6;
            i2 = 16u;
            i1 += i2;
            l10 = i1;
            i1 = LoadU32(ctx, i1);
            i2 = l10;
            i3 = l5;
            i1 = i3 ? i1 : i2;
            l5 = i1;
            i2 = l4;
            i0 = CompareBytes(ctx, i0, i1, i2);
            l10 = i0;
            i0 = !(i0);
            if (i0) {
              i0 = c;
              i1 = 6u;
              i0 = i0 <= i1;
              c = i0;
              if (i0) {goto block_505;}
              goto block_499;
            }
            i0 = l10;
            i1 = kF3650ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            c = i0;
            if (i0) {goto block_499;}
            block_505:;
            i0 = l5;
            i1 = l3;
            i2 = l4;
            i0 = CompareBytes(ctx, i0, i1, i2);
            c = i0;
            i0 = !(i0);
            l5 = i0;
            if (i0) {goto block_503;}
            i0 = c;
            i1 = kF3650ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            c = i0;
            if (i0) {goto block_502;}
            goto block_500;
          }
          i0 = c;
          i1 = 6u;
          i0 = i0 > i1;
          c = i0;
          if (i0) {goto block_499;}
          block_503:;
          i0 = l9;
          if (i0) {goto block_500;}
          block_502:;
          i0 = l3;
          i1 = 6u;
          StoreU8(ctx, i0 + 11u, i1);
          i0 = l3;
          i1 = kF3650MsgButton;
          i1 = LoadU32(ctx, i1);
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = kF3650ConstB53FF;
          i1 = LoadU16(ctx, i1);
          StoreU16(ctx, i0 + 4u, i1);
          i0 = l3;
          i1 = 0u;
          StoreU8(ctx, i0 + 6u, i1);
          i0 = kF3650ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = l3;
          StoreU32(ctx, i0 + 160u, i1);
          i0 = l3;
          i1 = 184u;
          i0 += i1;
          c = i0;
          i0 = l3;
          i1 = 160u;
          i0 += i1;
          l5 = i0;
          i0 = l3;
          i1 = 176u;
          i0 += i1;
          l6 = i0;
          i0 = b;
          i1 = 16u;
          i0 += i1;
          b = i0;
        }
        i0 = l7;
        i1 = 81u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1959u;
          i1 = c;
          i2 = b;
          i3 = l3;
          i4 = kF3650ConstB5E30;
          i5 = l5;
          i6 = l6;
          HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
          i0 = 81u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3650ConstD015C;
          i0 = LoadU32(ctx, i0);
          b = i0;
          i0 = kF3650ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = b;
          i1 = 1u;
          i0 = i0 != i1;
          b = i0;
          if (i0) {goto block_492;}
          goto block_486;
        }
        block_500:;
        i0 = l6;
        i1 = l6;
        i2 = 4u;
        i1 += i2;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        l6 = i0;
        block_499:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l6;
          i0 = LoadU32(ctx, i0);
          l6 = i0;
          if (i0) {goto loop_497;}
        }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l3;
      i1 = 20u;
      i0 += i1;
      c = i0;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 36u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 4u, j1);
      i0 = l3;
      i1 = kF3650ConstB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 28u, j1);
      i0 = l3;
      i1 = c;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = a;
      i1 = 14220u;
      i0 += i1;
      b = i0;
      i0 = a;
      i1 = 14231u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = b;
      i1 = l3;
      i2 = 4u;
      i1 |= i2;
      l4 = i1;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = b;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 4u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 15u, i1);
      i0 = a;
      i1 = 14232u;
      i0 += i1;
      l4 = i0;
      i1 = a;
      i2 = 14236u;
      i1 += i2;
      b = i1;
      i1 = LoadU32(ctx, i1);
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 16u);
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 20u);
      l6 = i1;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 14240u;
      i0 += i1;
      l9 = i0;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 24u);
      l5 = i1;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      if (i0) {goto block_491;}
      i0 = l4;
      i1 = b;
      StoreU32(ctx, i0, i1);
      goto block_490;
    }
    block_492:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 184u);
      c = i0;
      i1 = 28u;
      i0 += i1;
      b = i0;
      i0 = c;
      i0 = LoadS8(ctx, i0 + 39u);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
      } else {
        i0 = b;
      }
      i0 = SkipWhitespace(ctx, i0);
      c = i0;
      i0 = a;
      i1 = 14208u;
      i0 += i1;
      b = i0;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = !(i0);
      l5 = i0;
      if (i0) {goto block_514;}
      i0 = b;
      l4 = i0;
      loop_515: 
        i0 = l4;
        i1 = l6;
        i2 = l6;
        i2 = LoadU32(ctx, i2 + 16u);
        i3 = c;
        i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
        l5 = i2;
        i0 = i2 ? i0 : i1;
        l4 = i0;
        i0 = l6;
        i1 = l5;
        i2 = 2u;
        i1 <<= (i2 & 31);
        i0 += i1;
        l5 = i0;
        i0 = LoadU32(ctx, i0);
        l6 = i0;
        if (i0) {goto loop_515;}
      i0 = b;
      i1 = l4;
      i0 = i0 == i1;
      if (i0) {goto block_514;}
      i0 = c;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 16u);
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      b = i0;
      if (i0) {goto block_514;}
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 14204u;
      i0 += i1;
      i1 = l4;
      i0 = EraseTaggedTreeEntry(ctx, i0, i1);
      i0 = kF3650ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3650ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_486;}
      block_514:;
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 11u);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_6;}
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_6;
    }
    block_491:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = b;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 20u, j1);
      i0 = l3;
      i1 = c;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = 0u;
      l6 = i0;
    }
    block_490:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = 14244u;
      i0 += i1;
      b = i0;
      i0 = a;
      i1 = 14255u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 20u);
        l6 = i0;
      }
      i0 = b;
      i1 = l3;
      j1 = LoadU64(ctx, i1 + 28u);
      StoreU64(ctx, i0, j1);
      i0 = b;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 36u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 28u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 39u, i1);
      i0 = a;
      i1 = 14256u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 40u);
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = kF3650ConstB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 16u;
      i0 += i1;
      i1 = l6;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 15u);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_6;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 4u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_6;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_5;}
  block_486:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l5 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF3650ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_8;}
    goto running_exit;
  }
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l6;
    i2 = l5;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 15u;
    i0 = i0 != i1;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    if (i0) {goto block_522;}
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 82u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l9;
    i2 = 0u;
    i3 = kF3650ConstFFFFFFFF;
    i4 = kF3650MsgDelonautomodein;
    i5 = 15u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l8 = i0;
    i0 = 82u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
    l5 = i0;
    if (i0) {
      i0 = c;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l4 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l6 = i0;
        l5 = i0;
        goto block_522;
      }
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l3;
      i1 = 20u;
      i0 += i1;
      c = i0;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 36u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 4u, j1);
      i0 = l3;
      i1 = kF3650ConstB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 28u, j1);
      i0 = l3;
      i1 = c;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = a;
      i1 = 13768u;
      i0 += i1;
      b = i0;
      i0 = a;
      i1 = 13779u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = b;
      i1 = l3;
      i2 = 4u;
      i1 |= i2;
      l4 = i1;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = b;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 4u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 15u, i1);
      i0 = a;
      i1 = 13780u;
      i0 += i1;
      l4 = i0;
      i1 = a;
      i2 = 13784u;
      i1 += i2;
      b = i1;
      i1 = LoadU32(ctx, i1);
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 16u);
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 20u);
      l6 = i1;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 13788u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 24u);
      l5 = i1;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i0 = !(i0);
      l5 = i0;
      if (i0) {
        i0 = l4;
        i1 = b;
        StoreU32(ctx, i0, i1);
        goto block_529;
      }
      i0 = l6;
      i1 = b;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 20u, j1);
      i0 = l3;
      i1 = c;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = 0u;
      l6 = i0;
      block_529:;
      i0 = a;
      i1 = 13792u;
      i0 += i1;
      b = i0;
      i0 = a;
      i1 = 13803u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 20u);
        l6 = i0;
      }
      i0 = b;
      i1 = l3;
      j1 = LoadU64(ctx, i1 + 28u);
      StoreU64(ctx, i0, j1);
      i0 = b;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 36u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 28u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 39u, i1);
      i0 = a;
      i1 = 13804u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 40u);
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = kF3650ConstB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 16u;
      i0 += i1;
      i1 = l6;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 15u);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_6;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 4u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_6;
    }
    goto block_5;
  }
  block_522:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l6;
    i2 = l5;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 16u;
    i0 = i0 != i1;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    if (i0) {goto block_533;}
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 83u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l9;
    i2 = 0u;
    i3 = kF3650ConstFFFFFFFF;
    i4 = kF3650MsgDelonautomodeout;
    i5 = 16u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l8 = i0;
    i0 = 83u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
    l5 = i0;
    if (i0) {
      i0 = c;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l4 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l6 = i0;
        l5 = i0;
        goto block_533;
      }
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l3;
      i1 = 20u;
      i0 += i1;
      c = i0;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 36u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 4u, j1);
      i0 = l3;
      i1 = kF3650ConstB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 28u, j1);
      i0 = l3;
      i1 = c;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = a;
      i1 = 13812u;
      i0 += i1;
      b = i0;
      i0 = a;
      i1 = 13823u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = b;
      i1 = l3;
      i2 = 4u;
      i1 |= i2;
      l4 = i1;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = b;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 4u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 15u, i1);
      i0 = a;
      i1 = 13824u;
      i0 += i1;
      l4 = i0;
      i1 = a;
      i2 = 13828u;
      i1 += i2;
      b = i1;
      i1 = LoadU32(ctx, i1);
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 16u);
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 20u);
      l6 = i1;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 13832u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 24u);
      l5 = i1;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i0 = !(i0);
      l5 = i0;
      if (i0) {
        i0 = l4;
        i1 = b;
        StoreU32(ctx, i0, i1);
        goto block_540;
      }
      i0 = l6;
      i1 = b;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 20u, j1);
      i0 = l3;
      i1 = c;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = 0u;
      l6 = i0;
      block_540:;
      i0 = a;
      i1 = 13836u;
      i0 += i1;
      b = i0;
      i0 = a;
      i1 = 13847u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 20u);
        l6 = i0;
      }
      i0 = b;
      i1 = l3;
      j1 = LoadU64(ctx, i1 + 28u);
      StoreU64(ctx, i0, j1);
      i0 = b;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 36u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 28u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 39u, i1);
      i0 = a;
      i1 = 13848u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 40u);
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = kF3650ConstB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 16u;
      i0 += i1;
      i1 = l6;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 15u);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_6;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 4u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_6;
    }
    goto block_5;
  }
  block_533:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l6;
    i2 = l5;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 18u;
    i0 = i0 != i1;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    if (i0) {goto block_544;}
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 84u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l9;
    i2 = 0u;
    i3 = kF3650ConstFFFFFFFF;
    i4 = kF3650MsgDeloncommandskipin;
    i5 = 18u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l8 = i0;
    i0 = 84u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
    l5 = i0;
    if (i0) {
      i0 = c;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l4 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l6 = i0;
        l5 = i0;
        goto block_544;
      }
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l3;
      i1 = 20u;
      i0 += i1;
      c = i0;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 36u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 4u, j1);
      i0 = l3;
      i1 = kF3650ConstB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 28u, j1);
      i0 = l3;
      i1 = c;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = a;
      i1 = 13856u;
      i0 += i1;
      b = i0;
      i0 = a;
      i1 = 13867u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = b;
      i1 = l3;
      i2 = 4u;
      i1 |= i2;
      l4 = i1;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = b;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 4u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 15u, i1);
      i0 = a;
      i1 = 13868u;
      i0 += i1;
      l4 = i0;
      i1 = a;
      i2 = 13872u;
      i1 += i2;
      b = i1;
      i1 = LoadU32(ctx, i1);
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 16u);
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 20u);
      l6 = i1;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 13876u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 24u);
      l5 = i1;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i0 = !(i0);
      l5 = i0;
      if (i0) {
        i0 = l4;
        i1 = b;
        StoreU32(ctx, i0, i1);
        goto block_551;
      }
      i0 = l6;
      i1 = b;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 20u, j1);
      i0 = l3;
      i1 = c;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = 0u;
      l6 = i0;
      block_551:;
      i0 = a;
      i1 = 13880u;
      i0 += i1;
      b = i0;
      i0 = a;
      i1 = 13891u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 20u);
        l6 = i0;
      }
      i0 = b;
      i1 = l3;
      j1 = LoadU64(ctx, i1 + 28u);
      StoreU64(ctx, i0, j1);
      i0 = b;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 36u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 28u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 39u, i1);
      i0 = a;
      i1 = 13892u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 40u);
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = kF3650ConstB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 16u;
      i0 += i1;
      i1 = l6;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 15u);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_6;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 4u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_6;
    }
    goto block_5;
  }
  block_544:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l6;
    i2 = l5;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 19u;
    i0 = i0 != i1;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    if (i0) {goto block_555;}
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 85u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l9;
    i2 = 0u;
    i3 = kF3650ConstFFFFFFFF;
    i4 = kF3650MsgDeloncommandskipout;
    i5 = 19u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l8 = i0;
    i0 = 85u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
    l5 = i0;
    if (i0) {
      i0 = c;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l4 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l6 = i0;
        l5 = i0;
        goto block_555;
      }
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l3;
      i1 = 20u;
      i0 += i1;
      c = i0;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 36u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 4u, j1);
      i0 = l3;
      i1 = kF3650ConstB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 28u, j1);
      i0 = l3;
      i1 = c;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = a;
      i1 = 13900u;
      i0 += i1;
      b = i0;
      i0 = a;
      i1 = 13911u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = b;
      i1 = l3;
      i2 = 4u;
      i1 |= i2;
      l4 = i1;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = b;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 4u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 15u, i1);
      i0 = a;
      i1 = 13912u;
      i0 += i1;
      l4 = i0;
      i1 = a;
      i2 = 13916u;
      i1 += i2;
      b = i1;
      i1 = LoadU32(ctx, i1);
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 16u);
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 20u);
      l6 = i1;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 13920u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 24u);
      l5 = i1;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i0 = !(i0);
      l5 = i0;
      if (i0) {
        i0 = l4;
        i1 = b;
        StoreU32(ctx, i0, i1);
        goto block_562;
      }
      i0 = l6;
      i1 = b;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 20u, j1);
      i0 = l3;
      i1 = c;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = 0u;
      l6 = i0;
      block_562:;
      i0 = a;
      i1 = 13924u;
      i0 += i1;
      b = i0;
      i0 = a;
      i1 = 13935u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 20u);
        l6 = i0;
      }
      i0 = b;
      i1 = l3;
      j1 = LoadU64(ctx, i1 + 28u);
      StoreU64(ctx, i0, j1);
      i0 = b;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 36u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 28u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 39u, i1);
      i0 = a;
      i1 = 13936u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 40u);
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = kF3650ConstB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 16u;
      i0 += i1;
      i1 = l6;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 15u);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_6;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 4u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_6;
    }
    goto block_5;
  }
  block_555:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l6;
    i2 = l5;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 18u;
    i0 = i0 != i1;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    if (i0) {goto block_566;}
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 86u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l9;
    i2 = 0u;
    i3 = kF3650ConstFFFFFFFF;
    i4 = kF3650MsgDeloncontrolskipin;
    i5 = 18u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l8 = i0;
    i0 = 86u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
    l5 = i0;
    if (i0) {
      i0 = c;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l4 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l6 = i0;
        l5 = i0;
        goto block_566;
      }
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l3;
      i1 = 20u;
      i0 += i1;
      c = i0;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 36u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 4u, j1);
      i0 = l3;
      i1 = kF3650ConstB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 28u, j1);
      i0 = l3;
      i1 = c;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = a;
      i1 = 13944u;
      i0 += i1;
      b = i0;
      i0 = a;
      i1 = 13955u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = b;
      i1 = l3;
      i2 = 4u;
      i1 |= i2;
      l4 = i1;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = b;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 4u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 15u, i1);
      i0 = a;
      i1 = 13956u;
      i0 += i1;
      l4 = i0;
      i1 = a;
      i2 = 13960u;
      i1 += i2;
      b = i1;
      i1 = LoadU32(ctx, i1);
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 16u);
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 20u);
      l6 = i1;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 13964u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 24u);
      l5 = i1;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i0 = !(i0);
      l5 = i0;
      if (i0) {
        i0 = l4;
        i1 = b;
        StoreU32(ctx, i0, i1);
        goto block_573;
      }
      i0 = l6;
      i1 = b;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 20u, j1);
      i0 = l3;
      i1 = c;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = 0u;
      l6 = i0;
      block_573:;
      i0 = a;
      i1 = 13968u;
      i0 += i1;
      b = i0;
      i0 = a;
      i1 = 13979u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 20u);
        l6 = i0;
      }
      i0 = b;
      i1 = l3;
      j1 = LoadU64(ctx, i1 + 28u);
      StoreU64(ctx, i0, j1);
      i0 = b;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 36u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 28u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 39u, i1);
      i0 = a;
      i1 = 13980u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 40u);
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = kF3650ConstB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 16u;
      i0 += i1;
      i1 = l6;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 15u);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_6;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 4u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_6;
    }
    goto block_5;
  }
  block_566:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l6;
    i2 = l5;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 19u;
    i0 = i0 != i1;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    if (i0) {goto block_577;}
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 87u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l9;
    i2 = 0u;
    i3 = kF3650ConstFFFFFFFF;
    i4 = kF3650MsgDeloncontrolskipout;
    i5 = 19u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l8 = i0;
    i0 = 87u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
    l5 = i0;
    if (i0) {
      i0 = c;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l4 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l6 = i0;
        l5 = i0;
        goto block_577;
      }
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l3;
      i1 = 20u;
      i0 += i1;
      c = i0;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 36u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 4u, j1);
      i0 = l3;
      i1 = kF3650ConstB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 28u, j1);
      i0 = l3;
      i1 = c;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = a;
      i1 = 13988u;
      i0 += i1;
      b = i0;
      i0 = a;
      i1 = 13999u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = b;
      i1 = l3;
      i2 = 4u;
      i1 |= i2;
      l4 = i1;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = b;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 4u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 15u, i1);
      i0 = a;
      i1 = 14000u;
      i0 += i1;
      l4 = i0;
      i1 = a;
      i2 = 14004u;
      i1 += i2;
      b = i1;
      i1 = LoadU32(ctx, i1);
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 16u);
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 20u);
      l6 = i1;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 14008u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 24u);
      l5 = i1;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i0 = !(i0);
      l5 = i0;
      if (i0) {
        i0 = l4;
        i1 = b;
        StoreU32(ctx, i0, i1);
        goto block_584;
      }
      i0 = l6;
      i1 = b;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 20u, j1);
      i0 = l3;
      i1 = c;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = 0u;
      l6 = i0;
      block_584:;
      i0 = a;
      i1 = 14012u;
      i0 += i1;
      b = i0;
      i0 = a;
      i1 = 14023u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 20u);
        l6 = i0;
      }
      i0 = b;
      i1 = l3;
      j1 = LoadU64(ctx, i1 + 28u);
      StoreU64(ctx, i0, j1);
      i0 = b;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 36u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 28u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 39u, i1);
      i0 = a;
      i1 = 14024u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 40u);
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = kF3650ConstB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 16u;
      i0 += i1;
      i1 = l6;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 15u);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_6;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 4u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_6;
    }
    goto block_5;
  }
  block_577:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l6;
    i2 = l5;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 11u;
    i0 = i0 != i1;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    if (i0) {goto block_588;}
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 88u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l9;
    i2 = 0u;
    i3 = kF3650ConstFFFFFFFF;
    i4 = kF3650MsgDelonhidein;
    i5 = 11u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l8 = i0;
    i0 = 88u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
    l5 = i0;
    if (i0) {
      i0 = c;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l4 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l6 = i0;
        l5 = i0;
        goto block_588;
      }
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l3;
      i1 = 20u;
      i0 += i1;
      c = i0;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 36u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 4u, j1);
      i0 = l3;
      i1 = kF3650ConstB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 28u, j1);
      i0 = l3;
      i1 = c;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = a;
      i1 = 14032u;
      i0 += i1;
      b = i0;
      i0 = a;
      i1 = 14043u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = b;
      i1 = l3;
      i2 = 4u;
      i1 |= i2;
      l4 = i1;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = b;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 4u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 15u, i1);
      i0 = a;
      i1 = 14044u;
      i0 += i1;
      l4 = i0;
      i1 = a;
      i2 = 14048u;
      i1 += i2;
      b = i1;
      i1 = LoadU32(ctx, i1);
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 16u);
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 20u);
      l6 = i1;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 14052u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 24u);
      l5 = i1;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i0 = !(i0);
      l5 = i0;
      if (i0) {
        i0 = l4;
        i1 = b;
        StoreU32(ctx, i0, i1);
        goto block_595;
      }
      i0 = l6;
      i1 = b;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 20u, j1);
      i0 = l3;
      i1 = c;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = 0u;
      l6 = i0;
      block_595:;
      i0 = a;
      i1 = 14056u;
      i0 += i1;
      b = i0;
      i0 = a;
      i1 = 14067u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 20u);
        l6 = i0;
      }
      i0 = b;
      i1 = l3;
      j1 = LoadU64(ctx, i1 + 28u);
      StoreU64(ctx, i0, j1);
      i0 = b;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 36u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 28u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 39u, i1);
      i0 = a;
      i1 = 14068u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 40u);
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = kF3650ConstB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 16u;
      i0 += i1;
      i1 = l6;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 15u);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_6;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 4u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_6;
    }
    goto block_5;
  }
  block_588:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l6;
    i2 = l5;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 12u;
    i0 = i0 != i1;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    if (i0) {goto block_599;}
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 89u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l9;
    i2 = 0u;
    i3 = kF3650ConstFFFFFFFF;
    i4 = kF3650MsgDelonhideout;
    i5 = 12u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l8 = i0;
    i0 = 89u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
    l5 = i0;
    if (i0) {
      i0 = c;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l4 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l6 = i0;
        l5 = i0;
        goto block_599;
      }
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l3;
      i1 = 20u;
      i0 += i1;
      c = i0;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 36u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 4u, j1);
      i0 = l3;
      i1 = kF3650ConstB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 28u, j1);
      i0 = l3;
      i1 = c;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = a;
      i1 = 14076u;
      i0 += i1;
      b = i0;
      i0 = a;
      i1 = 14087u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = b;
      i1 = l3;
      i2 = 4u;
      i1 |= i2;
      l4 = i1;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = b;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 4u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 15u, i1);
      i0 = a;
      i1 = 14088u;
      i0 += i1;
      l4 = i0;
      i1 = a;
      i2 = 14092u;
      i1 += i2;
      b = i1;
      i1 = LoadU32(ctx, i1);
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 16u);
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 20u);
      l6 = i1;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 14096u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 24u);
      l5 = i1;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i0 = !(i0);
      l5 = i0;
      if (i0) {
        i0 = l4;
        i1 = b;
        StoreU32(ctx, i0, i1);
        goto block_606;
      }
      i0 = l6;
      i1 = b;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 20u, j1);
      i0 = l3;
      i1 = c;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = 0u;
      l6 = i0;
      block_606:;
      i0 = a;
      i1 = 14100u;
      i0 += i1;
      b = i0;
      i0 = a;
      i1 = 14111u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 20u);
        l6 = i0;
      }
      i0 = b;
      i1 = l3;
      j1 = LoadU64(ctx, i1 + 28u);
      StoreU64(ctx, i0, j1);
      i0 = b;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 36u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 28u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 39u, i1);
      i0 = a;
      i1 = 14112u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 40u);
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = kF3650ConstB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 16u;
      i0 += i1;
      i1 = l6;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 15u);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_6;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 4u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_6;
    }
    goto block_5;
  }
  block_599:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l6;
    i2 = l5;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 14u;
    i0 = i0 != i1;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    if (i0) {goto block_610;}
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 90u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l9;
    i2 = 0u;
    i3 = kF3650ConstFFFFFFFF;
    i4 = kF3650MsgDelonbacklogin;
    i5 = 14u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l8 = i0;
    i0 = 90u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
    l5 = i0;
    if (i0) {
      i0 = c;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l4 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l6 = i0;
        l5 = i0;
        goto block_610;
      }
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l3;
      i1 = 20u;
      i0 += i1;
      c = i0;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 36u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 4u, j1);
      i0 = l3;
      i1 = kF3650ConstB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 28u, j1);
      i0 = l3;
      i1 = c;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = a;
      i1 = 14120u;
      i0 += i1;
      b = i0;
      i0 = a;
      i1 = 14131u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = b;
      i1 = l3;
      i2 = 4u;
      i1 |= i2;
      l4 = i1;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = b;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 4u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 15u, i1);
      i0 = a;
      i1 = 14132u;
      i0 += i1;
      l4 = i0;
      i1 = a;
      i2 = 14136u;
      i1 += i2;
      b = i1;
      i1 = LoadU32(ctx, i1);
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 16u);
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 20u);
      l6 = i1;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 14140u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 24u);
      l5 = i1;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i0 = !(i0);
      l5 = i0;
      if (i0) {
        i0 = l4;
        i1 = b;
        StoreU32(ctx, i0, i1);
        goto block_617;
      }
      i0 = l6;
      i1 = b;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 20u, j1);
      i0 = l3;
      i1 = c;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = 0u;
      l6 = i0;
      block_617:;
      i0 = a;
      i1 = 14144u;
      i0 += i1;
      b = i0;
      i0 = a;
      i1 = 14155u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 20u);
        l6 = i0;
      }
      i0 = b;
      i1 = l3;
      j1 = LoadU64(ctx, i1 + 28u);
      StoreU64(ctx, i0, j1);
      i0 = b;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 36u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 28u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 39u, i1);
      i0 = a;
      i1 = 14156u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 40u);
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = kF3650ConstB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 16u;
      i0 += i1;
      i1 = l6;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 15u);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_6;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 4u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_6;
    }
    goto block_5;
  }
  block_610:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l6;
    i2 = l5;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 15u;
    i0 = i0 != i1;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    if (i0) {goto block_621;}
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 91u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l9;
    i2 = 0u;
    i3 = kF3650ConstFFFFFFFF;
    i4 = kF3650MsgDelonbacklogout;
    i5 = 15u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l8 = i0;
    i0 = 91u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
    l5 = i0;
    if (i0) {
      i0 = c;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l4 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l6 = i0;
        l5 = i0;
        goto block_621;
      }
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l3;
      i1 = 20u;
      i0 += i1;
      c = i0;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 36u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 4u, j1);
      i0 = l3;
      i1 = kF3650ConstB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 28u, j1);
      i0 = l3;
      i1 = c;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = a;
      i1 = 14164u;
      i0 += i1;
      b = i0;
      i0 = a;
      i1 = 14175u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = b;
      i1 = l3;
      i2 = 4u;
      i1 |= i2;
      l4 = i1;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = b;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 4u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 15u, i1);
      i0 = a;
      i1 = 14176u;
      i0 += i1;
      l4 = i0;
      i1 = a;
      i2 = 14180u;
      i1 += i2;
      b = i1;
      i1 = LoadU32(ctx, i1);
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 16u);
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 20u);
      l6 = i1;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 14184u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 24u);
      l5 = i1;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i0 = !(i0);
      l5 = i0;
      if (i0) {
        i0 = l4;
        i1 = b;
        StoreU32(ctx, i0, i1);
        goto block_628;
      }
      i0 = l6;
      i1 = b;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 20u, j1);
      i0 = l3;
      i1 = c;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = 0u;
      l6 = i0;
      block_628:;
      i0 = a;
      i1 = 14188u;
      i0 += i1;
      b = i0;
      i0 = a;
      i1 = 14199u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 20u);
        l6 = i0;
      }
      i0 = b;
      i1 = l3;
      j1 = LoadU64(ctx, i1 + 28u);
      StoreU64(ctx, i0, j1);
      i0 = b;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 36u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 28u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 39u, i1);
      i0 = a;
      i1 = 14200u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 40u);
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = kF3650ConstB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 16u;
      i0 += i1;
      i1 = l6;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 15u);
      i1 = kF3650ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_6;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 4u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_6;
    }
    goto block_5;
  }
  block_621:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l6;
    i2 = l5;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 11u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_6;}
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 92u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l9;
    i2 = 0u;
    i3 = kF3650ConstFFFFFFFF;
    i4 = kF3650MsgDelondirchg;
    i5 = 11u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l8 = i0;
    i0 = 92u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3650ConstD015C;
    i0 = LoadU32(ctx, i0);
    c = i0;
    i0 = kF3650ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_5;}
    i0 = b;
    if (i0) {goto block_6;}
    i0 = l3;
    i1 = 0u;
    StoreU32(ctx, i0 + 12u, i1);
    i0 = l3;
    i1 = 20u;
    i0 += i1;
    c = i0;
    j1 = 0ull;
    StoreU64(ctx, i0, j1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 36u, j1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 4u, j1);
    i0 = l3;
    i1 = kF3650ConstB5F40;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 28u, j1);
    i0 = l3;
    i1 = c;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = a;
    i1 = 14264u;
    i0 += i1;
    b = i0;
    i0 = a;
    i1 = 14275u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF3650ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = b;
    i1 = l3;
    i2 = 4u;
    i1 |= i2;
    l4 = i1;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0, j1);
    i0 = b;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 4u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 15u, i1);
    i0 = a;
    i1 = 14276u;
    i0 += i1;
    l4 = i0;
    i1 = a;
    i2 = 14280u;
    i1 += i2;
    b = i1;
    i1 = LoadU32(ctx, i1);
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l4;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 16u);
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 20u);
    l6 = i1;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 14284u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 24u);
    l5 = i1;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i0 = !(i0);
    l5 = i0;
    if (i0) {
      i0 = l4;
      i1 = b;
      StoreU32(ctx, i0, i1);
      goto block_635;
    }
    i0 = l6;
    i1 = b;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 20u, j1);
    i0 = l3;
    i1 = c;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = 0u;
    l6 = i0;
    block_635:;
    i0 = a;
    i1 = 14288u;
    i0 += i1;
    b = i0;
    i0 = a;
    i1 = 14299u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF3650ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 20u);
      l6 = i0;
    }
    i0 = b;
    i1 = l3;
    j1 = LoadU64(ctx, i1 + 28u);
    StoreU64(ctx, i0, j1);
    i0 = b;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 36u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 28u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 39u, i1);
    i0 = a;
    i1 = 14300u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 40u);
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = kF3650ConstB5F40;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 16u;
    i0 += i1;
    i1 = l6;
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 15u);
    i1 = kF3650ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_6;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 192u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    a = i0;
    i0 = HostTempRet0(ctx);
  }
  i0 = l7;
  i1 = 93u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 93u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    TrapUnreachable();
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0);
    DeallocateRawBlock(ctx, i0);
    i0 = l5;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  goto func_exit;
  spill_frame:;
  l8 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l8;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF3650ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l8 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l8;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l8;
  i1 = c;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l8;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l8;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l8;
  i1 = l5;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l8;
  i1 = l6;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l8;
  i1 = l9;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l8;
  i1 = l10;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = l8;
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
}
