#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step calls.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void Panic(EngineContext* ctx, uint32_t a0);
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void ReleaseNodeTree(EngineContext* ctx, uint32_t state, uint32_t node);
void UnlinkListNode(EngineContext* ctx, uint32_t node, uint32_t victim);
uint32_t SkipWhitespace(EngineContext* ctx, uint32_t text);
uint32_t SetStringFromSource(EngineContext* ctx, uint32_t text, uint32_t source);
uint32_t CombineComparisonResults(EngineContext* ctx, uint32_t left, uint32_t right, uint32_t result, uint32_t box);
uint32_t CopyScriptBlock(EngineContext* ctx, uint32_t a, uint32_t b);
void AssignTaggedTreeRange(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

namespace LuaEngine {
// TagGenericSetOn -- the seton* tag registry: it matches the tag name against
// the setonautomodein .. setonwindowbutton handler table and deep-copies the
// handler description into the registry entry.

void TagGenericSetOn(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  constexpr uint32_t kData8C564 = 0x8C564u;
  constexpr uint32_t kMsgSetonclick = 0xAF698u;  // setonclick
  constexpr uint32_t kMsgSetonrollover = 0xAF6AEu;  // setonrollover
  constexpr uint32_t kMsgSetonrollout = 0xAF6CAu;  // setonrollout
  constexpr uint32_t kMsgSetondragin = 0xAF6E4u;  // setondragin
  constexpr uint32_t kMsgSetondrag = 0xAF6FCu;  // setondrag
  constexpr uint32_t kMsgSetondragout = 0xAF710u;  // setondragout
  constexpr uint32_t kMsgSetonpush = 0xAF72Au;  // setonpush
  constexpr uint32_t kMsgSetonautomodein = 0xAF73Eu;  // setonautomodein
  constexpr uint32_t kMsgSetonautomodeout = 0xAF75Eu;  // setonautomodeout
  constexpr uint32_t kMsgSetoncommandskipin = 0xAF780u;  // setoncommandskipin
  constexpr uint32_t kMsgSetoncommandskipout = 0xAF7A6u;  // setoncommandskipout
  constexpr uint32_t kMsgSetoncontrolskipin = 0xAF7CEu;  // setoncontrolskipin
  constexpr uint32_t kMsgSetoncontrolskipout = 0xAF7F4u;  // setoncontrolskipout
  constexpr uint32_t kMsgSetonhidein = 0xAF81Cu;  // setonhidein
  constexpr uint32_t kMsgSetonhideout = 0xAF834u;  // setonhideout
  constexpr uint32_t kMsgSetonbacklogin = 0xAF84Eu;  // setonbacklogin
  constexpr uint32_t kMsgSetonbacklogout = 0xAF86Cu;  // setonbacklogout
  constexpr uint32_t kMsgSetonwindowbutton = 0xAF88Cu;  // setonwindowbutton
  constexpr uint32_t kMsgSetondirchg = 0xAF8B0u;  // setondirchg
  constexpr uint32_t kMsgSetonvideofinish = 0xAF8C8u;  // setonvideofinish
  constexpr uint32_t kMsgSetonsoundfinish = 0xAF8EAu;  // setonsoundfinish
  constexpr uint32_t kMsgLabel = 0xB4B1Bu;  // label
  constexpr uint32_t kDataB4B1F = 0xB4B1Fu;
  constexpr uint32_t kDataB4B29 = 0xB4B29u;
  constexpr uint32_t kMsgPenetration = 0xB5339u;  // penetration
  constexpr uint32_t kDataB5340 = 0xB5340u;
  constexpr uint32_t kMsgKeyrepeat = 0xB53EDu;  // keyrepeat
  constexpr uint32_t kDataB53F5 = 0xB53F5u;
  constexpr uint32_t kDataB53F7 = 0xB53F7u;
  constexpr uint32_t kDataB53F9 = 0xB53F9u;
  constexpr uint32_t kMsgButton = 0xB53FBu;  // button
  constexpr uint32_t kDataB53FF = 0xB53FFu;
  constexpr uint32_t kDataB5E30 = 0xB5E30u;
  constexpr uint32_t kMsgPx = 0xB5E31u;  // px != 0
  constexpr uint32_t kMsgSmartPtrPath = 0xB5E39u;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
  constexpr uint32_t kMsgOperator = 0xB5E71u;  // operator->
  constexpr uint32_t kDataB5F40 = 0xB5F40u;
  constexpr uint32_t kDataB8F8C = 0xB8F8Cu;
  constexpr uint32_t kData656C6966 = 0x656C6966u;
  constexpr uint32_t kDataFFFFFFC0 = 0xFFFFFFC0u;
  constexpr uint32_t kDataFFFFFFFE = 0xFFFFFFFEu;
  constexpr uint32_t kDataFFFFFFFF = 0xFFFFFFFFu;
  constexpr uint32_t kSaveFrameBytes = 52u;
  constexpr uint32_t kResumeWordBytes = 4u;
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l13 = 0u; uint32_t l14 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u; uint32_t i6 = 0u;
  uint64_t j1 = 0; uint64_t j3 = 0;
      l11 = 0, l12 = 0, l13 = 0, l14 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kSaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l7 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
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
    l9 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 32u);
    l10 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 36u);
    l11 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 40u);
    l12 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 44u);
    l13 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 48u);
    l14 = i0;
    i0 = l7;
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
    i2 = kResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l8 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    l3 = i0;
    i1 = 1360u;
    i0 -= i1;
    l4 = i0;
    GuestStackSp(ctx) = i0;
    i0 = l4;
    i1 = 0u;
    StoreU8(ctx, i0 + 1324u, i1);
    i0 = l4;
    i1 = kData656C6966;
    StoreU32(ctx, i0 + 1320u, i1);
    i0 = l4;
    i1 = 4u;
    StoreU8(ctx, i0 + 1331u, i1);
    i0 = b;
    i1 = 16u;
    i0 += i1;
    l14 = i0;
    i0 = b;
    i1 = 20u;
    i0 += i1;
    l9 = i0;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    if (i0) {
      i0 = l5;
      l3 = i0;
      loop_12: 
        i0 = l3;
        i0 = LoadU8(ctx, i0 + 27u);
        l6 = i0;
        i1 = 24u;
        i0 <<= (i1 & 31);
        i1 = 24u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l12 = i0;
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 20u);
        i1 = l6;
        i2 = l12;
        i0 = i2 ? i0 : i1;
        l6 = i0;
        i1 = 4u;
        i0 = i0 < i1;
        l11 = i0;
        i0 = l6;
        i1 = 4u;
        i2 = l11;
        i0 = i2 ? i0 : i1;
        l10 = i0;
        if (i0) {
          i0 = l4;
          i1 = 1320u;
          i0 += i1;
          i1 = l3;
          i2 = 16u;
          i1 += i2;
          l13 = i1;
          i1 = LoadU32(ctx, i1);
          i2 = l13;
          i3 = l12;
          i1 = i3 ? i1 : i2;
          l12 = i1;
          i2 = l10;
          i0 = CompareBytes(ctx, i0, i1, i2);
          l13 = i0;
          i0 = !(i0);
          if (i0) {
            i0 = l6;
            i1 = 4u;
            i0 = i0 <= i1;
            l6 = i0;
            if (i0) {goto block_17;}
            goto block_13;
          }
          i0 = l13;
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          l6 = i0;
          if (i0) {goto block_13;}
          block_17:;
          i0 = l12;
          i1 = l4;
          i2 = 1320u;
          i1 += i2;
          i2 = l10;
          i0 = CompareBytes(ctx, i0, i1, i2);
          l6 = i0;
          i0 = !(i0);
          l10 = i0;
          if (i0) {goto block_15;}
          i0 = l6;
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          l6 = i0;
          if (i0) {goto block_9;}
          goto block_14;
        }
        i0 = l6;
        i1 = 4u;
        i0 = i0 > i1;
        l6 = i0;
        if (i0) {goto block_13;}
        block_15:;
        i0 = l11;
        i0 = !(i0);
        l6 = i0;
        if (i0) {goto block_9;}
        block_14:;
        i0 = l3;
        i1 = 4u;
        i0 += i1;
        l3 = i0;
        block_13:;
        i0 = l3;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        if (i0) {goto loop_12;}
    }
    i0 = l4;
    i1 = 5u;
    StoreU8(ctx, i0 + 1315u, i1);
    i0 = l4;
    i1 = 0u;
    StoreU8(ctx, i0 + 1309u, i1);
    i0 = l4;
    i1 = kMsgLabel;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 1304u, i1);
    i0 = l4;
    i1 = kDataB4B1F;
    i1 = LoadU8(ctx, i1);
    StoreU8(ctx, i0 + 1308u, i1);
    i0 = l5;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_9;}
  }
  loop_19: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i0 = LoadU8(ctx, i0 + 27u);
      l3 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l11 = i0;
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l10 = i0;
      i0 = l5;
      i0 = LoadU32(ctx, i0 + 20u);
      i1 = l3;
      i2 = l10;
      i0 = i2 ? i0 : i1;
      l3 = i0;
      i1 = 5u;
      i0 = i0 < i1;
      l12 = i0;
      i0 = l3;
      i1 = 5u;
      i2 = l12;
      i0 = i2 ? i0 : i1;
      l6 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      if (i0) {
        i0 = l4;
        i1 = 1304u;
        i0 += i1;
        i1 = l5;
        i2 = 16u;
        i1 += i2;
        l11 = i1;
        i1 = LoadU32(ctx, i1);
        i2 = l11;
        i3 = l10;
        i1 = i3 ? i1 : i2;
        l10 = i1;
        i2 = l6;
        i0 = CompareBytes(ctx, i0, i1, i2);
        l11 = i0;
        i0 = !(i0);
        l13 = i0;
        if (i0) {
          i0 = l3;
          i1 = 5u;
          i0 = i0 <= i1;
          l3 = i0;
          if (i0) {goto block_28;}
          goto block_22;
        }
        i0 = l11;
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        l3 = i0;
        if (i0) {goto block_22;}
        block_28:;
        i0 = l10;
        i1 = l4;
        i2 = 1304u;
        i1 += i2;
        i2 = l6;
        i0 = CompareBytes(ctx, i0, i1, i2);
        l3 = i0;
        i0 = !(i0);
        l6 = i0;
        if (i0) {goto block_26;}
        i0 = l3;
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        l3 = i0;
        if (i0) {goto block_25;}
        goto block_23;
      }
      i0 = l3;
      i1 = 5u;
      i0 = i0 > i1;
      l3 = i0;
      if (i0) {goto block_22;}
      block_26:;
      i0 = l12;
      if (i0) {goto block_23;}
      block_25:;
      i0 = l4;
      i1 = 0u;
      StoreU8(ctx, i0 + 1324u, i1);
      i0 = l4;
      i1 = kData656C6966;
      StoreU32(ctx, i0 + 1320u, i1);
      i0 = l4;
      i1 = 4u;
      StoreU8(ctx, i0 + 1331u, i1);
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l4;
      i2 = 1320u;
      i1 += i2;
      StoreU32(ctx, i0 + 1352u, i1);
      i0 = l4;
      i1 = 1320u;
      i0 += i1;
      l6 = i0;
      i0 = l4;
      i1 = 1352u;
      i0 += i1;
      l5 = i0;
      i0 = l4;
      i1 = 1296u;
      i0 += i1;
      l11 = i0;
      i0 = l4;
      i1 = 1304u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l8;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1959u;
      i1 = l3;
      i2 = l14;
      i3 = l6;
      i4 = kDataB5E30;
      i5 = l5;
      i6 = l11;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_20;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 2752u;
      i0 += i1;
      l6 = i0;
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1304u);
      i1 = 28u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l8;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 870u;
      i1 = l3;
      i2 = l6;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l7 = i0;
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_20;}
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 1331u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      l3 = i0;
      if (i0) {goto block_9;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1320u);
      l3 = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_9;
    }
    block_23:;
    i0 = l5;
    i1 = l5;
    i2 = 4u;
    i1 += i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l5 = i0;
    block_22:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      if (i0) {goto loop_19;}
      goto block_9;
    }
    block_20:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 1331u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    b = i0;
    if (i0) {goto block_8;}
    goto block_5;
  }
  block_9:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i1 = 4u;
    i0 += i1;
    l10 = i0;
    i0 = b;
    i0 = LoadU8(ctx, i0 + 15u);
    l3 = i0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    l6 = i0;
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l11 = i0;
    i0 = b;
    i0 = LoadU32(ctx, i0 + 8u);
    l5 = i0;
    i1 = l3;
    i2 = l11;
    i0 = i2 ? i0 : i1;
    i1 = 10u;
    i0 = i0 == i1;
    l11 = i0;
  }
  i0 = l11;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = l10;
      i2 = 0u;
      i3 = kDataFFFFFFFF;
      i4 = kMsgSetonclick;
      i5 = 10u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l7 = i0;
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l5;
    } else {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l6 = i0;
      if (i0) {goto exit_path;}
      i0 = l3;
      i0 = !(i0);
      l3 = i0;
      if (i0) {goto block_46;}
      i0 = b;
      i0 = LoadU8(ctx, i0 + 15u);
      l3 = i0;
      l6 = i0;
      i0 = b;
      i0 = LoadU32(ctx, i0 + 8u);
    }
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    i2 = l6;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 13u;
    i0 = i0 == i1;
    l11 = i0;
  }
  i0 = l11;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 3u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = l10;
      i2 = 0u;
      i3 = kDataFFFFFFFF;
      i4 = kMsgSetonrollover;
      i5 = 13u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l7 = i0;
      i0 = 3u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l5;
    } else {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l6 = i0;
      if (i0) {goto exit_path;}
      i0 = l3;
      i0 = !(i0);
      l3 = i0;
      if (i0) {goto block_46;}
      i0 = b;
      i0 = LoadU8(ctx, i0 + 15u);
      l3 = i0;
      l6 = i0;
      i0 = b;
      i0 = LoadU32(ctx, i0 + 8u);
    }
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    i2 = l6;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 12u;
    i0 = i0 == i1;
    l11 = i0;
  }
  i0 = l11;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 4u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = l10;
      i2 = 0u;
      i3 = kDataFFFFFFFF;
      i4 = kMsgSetonrollout;
      i5 = 12u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l7 = i0;
      i0 = 4u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l5;
    } else {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l6 = i0;
      if (i0) {goto exit_path;}
      i0 = l3;
      i0 = !(i0);
      l3 = i0;
      if (i0) {goto block_46;}
      i0 = b;
      i0 = LoadU8(ctx, i0 + 15u);
      l3 = i0;
      l6 = i0;
      i0 = b;
      i0 = LoadU32(ctx, i0 + 8u);
    }
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    i2 = l6;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 11u;
    i0 = i0 == i1;
    l11 = i0;
  }
  i0 = l11;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 5u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = l10;
      i2 = 0u;
      i3 = kDataFFFFFFFF;
      i4 = kMsgSetondragin;
      i5 = 11u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l7 = i0;
      i0 = 5u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l5;
    } else {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l6 = i0;
      if (i0) {goto exit_path;}
      i0 = l3;
      i0 = !(i0);
      l3 = i0;
      if (i0) {goto block_46;}
      i0 = b;
      i0 = LoadU8(ctx, i0 + 15u);
      l3 = i0;
      l6 = i0;
      i0 = b;
      i0 = LoadU32(ctx, i0 + 8u);
    }
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    i2 = l6;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 9u;
    i0 = i0 == i1;
    l11 = i0;
  }
  i0 = l11;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 6u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = l10;
      i2 = 0u;
      i3 = kDataFFFFFFFF;
      i4 = kMsgSetondrag;
      i5 = 9u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l7 = i0;
      i0 = 6u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l5;
    } else {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l6 = i0;
      if (i0) {goto exit_path;}
      i0 = l3;
      i0 = !(i0);
      l3 = i0;
      if (i0) {goto block_46;}
      i0 = b;
      i0 = LoadU8(ctx, i0 + 15u);
      l3 = i0;
      l6 = i0;
      i0 = b;
      i0 = LoadU32(ctx, i0 + 8u);
    }
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    i2 = l6;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 12u;
    i0 = i0 == i1;
    l11 = i0;
  }
  i0 = l11;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 7u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = l10;
      i2 = 0u;
      i3 = kDataFFFFFFFF;
      i4 = kMsgSetondragout;
      i5 = 12u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l7 = i0;
      i0 = 7u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l5;
    } else {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l6 = i0;
      if (i0) {goto block_45;}
      i0 = l3;
      i0 = !(i0);
      l3 = i0;
      if (i0) {goto block_46;}
      i0 = b;
      i0 = LoadU8(ctx, i0 + 15u);
      l3 = i0;
      l6 = i0;
      i0 = b;
      i0 = LoadU32(ctx, i0 + 8u);
    }
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    i2 = l6;
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
    if (i0) {goto block_44;}
    goto block_38;
  }
  block_46:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
    l3 = i0;
    i0 = !(i0);
    l6 = i0;
  }
  i0 = l6;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 8u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kMsgPx;
      i2 = kMsgSmartPtrPath;
      i3 = 734u;
      i4 = kMsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 8u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_37;}
      goto exit_path;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 24u);
    l3 = i0;
    i0 = !(i0);
    l6 = i0;
  }
  i0 = l6;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 9u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kMsgPx;
      i2 = kMsgSmartPtrPath;
      i3 = 734u;
      i4 = kMsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 9u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_37;}
      goto exit_path;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 0u;
    StoreU8(ctx, i0 + 1322u, i1);
    i0 = l4;
    i1 = 25705u;
    StoreU16(ctx, i0 + 1320u, i1);
    i0 = l4;
    i1 = 2u;
    StoreU8(ctx, i0 + 1331u, i1);
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = l4;
    i2 = 1320u;
    i1 += i2;
    StoreU32(ctx, i0 + 1352u, i1);
    i0 = l4;
    i1 = 1304u;
    i0 += i1;
    l6 = i0;
    i0 = l4;
    i1 = 1352u;
    i0 += i1;
    l11 = i0;
    i0 = l4;
    i1 = 1344u;
    i0 += i1;
    l12 = i0;
    i0 = l4;
    i1 = 1320u;
    i0 += i1;
    l5 = i0;
  }
  i0 = l8;
  i1 = 10u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = l6;
    i2 = l14;
    i3 = l5;
    i4 = kDataB5E30;
    i5 = l11;
    i6 = l12;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 10u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 == i1;
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    if (i0) {goto block_100;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 1304u);
    l5 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 304u);
    l6 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1296u;
    i0 += i1;
    l11 = i0;
    i0 = l5;
    i1 = 28u;
    i0 += i1;
    l5 = i0;
  }
  i0 = l8;
  i1 = 11u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = l11;
    i2 = l3;
    i3 = l5;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 11u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    l3 = i0;
    if (i0) {goto block_100;}
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 1331u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1320u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 1296u);
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_39;}
    i0 = b;
    i0 = LoadU8(ctx, i0 + 15u);
    l3 = i0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    l6 = i0;
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l11 = i0;
    i0 = b;
    i0 = LoadU32(ctx, i0 + 8u);
    l5 = i0;
    i1 = l3;
    i2 = l11;
    i0 = i2 ? i0 : i1;
    i1 = 10u;
    i0 = i0 == i1;
    l11 = i0;
    if (i0) {goto block_99;}
    goto block_42;
  }
  block_100:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 1331u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    b = i0;
    if (i0) {goto block_8;}
    goto block_5;
  }
  block_99:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 12u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l10;
    i2 = 0u;
    i3 = kDataFFFFFFFF;
    i4 = kMsgSetonclick;
    i5 = 10u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l7 = i0;
    i0 = 12u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 != i1;
    l6 = i0;
  }
  i0 = l6;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l5 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l3 = i0;
        l6 = i0;
        goto block_42;
      }
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1296u);
      l12 = i0;
      i0 = !(i0);
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l8;
      i1 = 13u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kMsgPx;
        i2 = kMsgSmartPtrPath;
        i3 = 734u;
        i4 = kMsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 13u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_37;}
        goto exit_path;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = 0u;
      l5 = i0;
    }
    i0 = l8;
    i1 = 14u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 16u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l7 = i0;
      i0 = 14u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l10 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    i1 = 0u;
    i2 = ResumePhase(ctx);
    i2 = !(i2);
    if (i2) {
      i2 = kDataD015C;
      i2 = LoadU32(ctx, i2);
      l3 = i2;
      i2 = kDataD015C;
      i3 = 0u;
      StoreU32(ctx, i2, i3);
      i2 = 1u;
      l6 = i2;
      i2 = l3;
      i3 = 1u;
      i2 = i2 == i3;
      l3 = i2;
      if (i2) {goto block_43;}
      i2 = l10;
      i3 = kDataB5340;
      i3 = LoadU32(ctx, i3);
      StoreU32(ctx, i2 + 7u, i3);
      i2 = l10;
      i3 = kMsgPenetration;
      j3 = LoadU64(ctx, i3);
      StoreU64(ctx, i2, j3);
      i2 = l10;
      i3 = 0u;
      StoreU8(ctx, i2 + 11u, i3);
      i2 = l9;
      i2 = LoadU32(ctx, i2);
      l3 = i2;
      i2 = !(i2);
      l9 = i2;
    }
    i2 = l9;
    i0 = i2 ? i0 : i1;
    if (i0) {goto block_130;}
    loop_134: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadU8(ctx, i0 + 27u);
        l5 = i0;
        i1 = 24u;
        i0 <<= (i1 & 31);
        i1 = 24u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l11 = i0;
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 20u);
        i1 = l5;
        i2 = l11;
        i0 = i2 ? i0 : i1;
        l5 = i0;
        i1 = 11u;
        i0 = i0 < i1;
        l13 = i0;
        i0 = l5;
        i1 = 11u;
        i2 = l13;
        i0 = i2 ? i0 : i1;
        l9 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l9;
        if (i0) {
          i0 = l10;
          i1 = l3;
          i2 = 16u;
          i1 += i2;
          l7 = i1;
          i1 = LoadU32(ctx, i1);
          i2 = l7;
          i3 = l11;
          i1 = i3 ? i1 : i2;
          l11 = i1;
          i2 = l9;
          i0 = CompareBytes(ctx, i0, i1, i2);
          l7 = i0;
          i0 = !(i0);
          if (i0) {
            i0 = l5;
            i1 = 11u;
            i0 = i0 <= i1;
            l5 = i0;
            if (i0) {goto block_142;}
            goto block_136;
          }
          i0 = l7;
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          l5 = i0;
          if (i0) {goto block_136;}
          block_142:;
          i0 = l11;
          i1 = l10;
          i2 = l9;
          i0 = CompareBytes(ctx, i0, i1, i2);
          l5 = i0;
          i0 = !(i0);
          l9 = i0;
          if (i0) {goto block_140;}
          i0 = l5;
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          l5 = i0;
          if (i0) {goto block_139;}
          goto block_137;
        }
        i0 = l5;
        i1 = 11u;
        i0 = i0 > i1;
        l5 = i0;
        if (i0) {goto block_136;}
        block_140:;
        i0 = l13;
        if (i0) {goto block_137;}
        block_139:;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l8;
      i1 = 15u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1032u;
        i1 = 16u;
        i0 = HostFoldLayoutOffset(ctx, i0, i1);
        l7 = i0;
        i0 = 15u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l7;
        l3 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 1u;
        i0 = i0 == i1;
        l6 = i0;
        if (i0) {goto block_129;}
        i0 = l4;
        i1 = l3;
        StoreU32(ctx, i0 + 1320u, i1);
        i0 = l4;
        j1 = 9223372105574252555ull;
        StoreU64(ctx, i0 + 1324u, j1);
        i0 = l3;
        i1 = 0u;
        StoreU8(ctx, i0 + 11u, i1);
        i0 = l3;
        i1 = kDataB5340;
        i1 = LoadU32(ctx, i1);
        StoreU32(ctx, i0 + 7u, i1);
        i0 = l3;
        i1 = kMsgPenetration;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0, j1);
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = l4;
        i2 = 1320u;
        i1 += i2;
        StoreU32(ctx, i0 + 1352u, i1);
        i0 = l4;
        i1 = 1320u;
        i0 += i1;
        l6 = i0;
        i0 = l4;
        i1 = 1352u;
        i0 += i1;
        l5 = i0;
        i0 = l4;
        i1 = 1344u;
        i0 += i1;
        l9 = i0;
        i0 = l4;
        i1 = 1304u;
        i0 += i1;
        l3 = i0;
      }
      i0 = l8;
      i1 = 16u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1959u;
        i1 = l3;
        i2 = l14;
        i3 = l6;
        i4 = kDataB5E30;
        i5 = l5;
        i6 = l9;
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
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 != i1;
        l3 = i0;
        if (i0) {goto block_133;}
        i0 = HostRet0(ctx);
        a = i0;
        i0 = HostTempRet0(ctx);
        b = i0;
        goto block_122;
      }
      block_137:;
      i0 = l3;
      i1 = l3;
      i2 = 4u;
      i1 += i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      l3 = i0;
      block_136:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        if (i0) {goto loop_134;}
      }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = 0u;
      l5 = i0;
      goto block_130;
    }
    block_133:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = 0u;
      l6 = i0;
      i0 = 1u;
      l5 = i0;
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1304u);
      l3 = i0;
      i0 = LoadU32(ctx, i0 + 32u);
      l11 = i0;
      i0 = l3;
      i0 = LoadU8(ctx, i0 + 39u);
      l9 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l13 = i0;
      i0 = l11;
      i1 = l9;
      i2 = l13;
      i0 = i2 ? i0 : i1;
      i1 = 1u;
      i0 = i0 != i1;
      l9 = i0;
      if (i0) {goto block_130;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 28u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l8;
    i1 = 17u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = l3;
      i2 = 0u;
      i3 = kDataFFFFFFFF;
      i4 = kDataB4B29;
      i5 = 1u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l7 = i0;
      i0 = 17u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto exit_path;}
      i0 = l3;
      i1 = 0u;
      i0 = i0 != i1;
      l5 = i0;
    }
    block_130:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1168u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l8;
    i1 = 18u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2591u;
      i1 = l3;
      i2 = b;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l7 = i0;
      i0 = 18u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l9 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l9;
      i1 = 1u;
      i0 = i0 == i1;
      l9 = i0;
      if (i0) {goto block_128;}
      i0 = l4;
      i1 = 0u;
      StoreU32(ctx, i0 + 1164u, i1);
      i0 = l4;
      i1 = 2648u;
      StoreU32(ctx, i0 + 1160u, i1);
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l4;
      j1 = LoadU64(ctx, i1 + 1160u);
      StoreU64(ctx, i0, j1);
      i0 = l4;
      i1 = 1216u;
      i0 += i1;
      l9 = i0;
    }
    i0 = l8;
    i1 = 19u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2649u;
      i1 = l9;
      i2 = l4;
      i3 = a;
      i4 = 0u;
      i5 = l5;
      i6 = l3;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 19u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_127;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1216u;
      i0 += i1;
      l9 = i0;
      i0 = l4;
      i1 = 1280u;
      i0 += i1;
      l5 = i0;
    }
    i0 = l8;
    i1 = 20u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2650u;
      i1 = l5;
      i2 = l9;
      i3 = 0u;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l7 = i0;
      i0 = 20u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l5 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l9 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l9;
      i1 = 1u;
      i0 = i0 == i1;
      l9 = i0;
      if (i0) {goto block_125;}
      i0 = l12;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 272u);
      l9 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 21u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l9;
      i1 = l12;
      i2 = 0u;
      i3 = l5;
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
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l9 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l9;
      i1 = 1u;
      i0 = i0 == i1;
      l9 = i0;
      if (i0) {goto block_126;}
      i0 = l5;
      i0 = LoadU32(ctx, i0);
      l9 = i0;
    }
    i0 = l9;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l11;
      i1 = l9;
      i2 = 1u;
      i1 &= i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      l11 = i0;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l11;
        if (i0) {goto block_164;}
        i0 = l9;
        i1 = kDataFFFFFFFE;
        i0 &= i1;
        i0 = LoadU32(ctx, i0);
        l9 = i0;
        i0 = !(i0);
        l11 = i0;
        if (i0) {goto block_164;}
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l9;
        l11 = i0;
        i0 = l5;
        i1 = kResumeWordBytes;
        i0 += i1;
        l9 = i0;
      }
      i0 = l8;
      i1 = 22u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l11;
        i1 = l9;
        i2 = l9;
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
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l9 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l9;
        i1 = 1u;
        i0 = i0 == i1;
        l9 = i0;
        if (i0) {goto block_120;}
      }
      block_164:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = 1236u;
      i0 += i1;
      i1 = kDataB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1275u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 1264u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l4;
      i1 = 1252u;
      i0 += i1;
      i1 = l4;
      i2 = 1256u;
      i1 += i2;
      i1 = LoadU32(ctx, i1);
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l4;
      i1 = 1251u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 1240u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l3;
      i1 = kDataB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 39u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 28u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l3;
      i1 = 16u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 20u);
      l9 = i1;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 15u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l5 = i0;
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 4u);
        l3 = i0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l6;
      if (i0) {goto block_174;}
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 1331u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      l3 = i0;
      if (i0) {goto block_174;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1320u);
      l3 = i0;
      DeallocateRawBlock(ctx, i0);
      block_174:;
      i0 = l10;
      DeallocateRawBlock(ctx, i0);
      goto block_41;
    }
    block_129:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
      i0 = l10;
      DeallocateRawBlock(ctx, i0);
      goto block_40;
    }
    block_128:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = l6;
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_122;}
      goto block_121;
    }
    block_127:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
      goto block_123;
    }
    block_126:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = l5;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = !(i0);
      c = i0;
      if (i0) {goto block_124;}
      i0 = b;
      i1 = 1u;
      i0 &= i1;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = c;
      if (i0) {goto block_180;}
      i0 = b;
      i1 = kDataFFFFFFFE;
      i0 &= i1;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = !(i0);
      c = i0;
      if (i0) {goto block_180;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      c = i0;
      i0 = l5;
      i1 = 4u;
      i0 += i1;
      b = i0;
    }
    i0 = l8;
    i1 = 23u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = b;
      i2 = b;
      i3 = 2u;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 23u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
      if (i0) {goto block_179;}
    }
    block_180:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      goto block_124;
    }
    block_179:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto exit_path;}
    block_125:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
    }
    block_124:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = 1236u;
      i0 += i1;
      i1 = kDataB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1275u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 1264u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l4;
      i1 = 1252u;
      i0 += i1;
      i1 = l4;
      i2 = 1256u;
      i1 += i2;
      i1 = LoadU32(ctx, i1);
      c = i1;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l4;
      i1 = 1251u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      b = i0;
      if (i0) {goto block_123;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1240u);
      b = i0;
      DeallocateRawBlock(ctx, i0);
    }
    block_123:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = kDataB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 39u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 28u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l3;
      i1 = 16u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 20u);
      c = i1;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 15u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      b = i0;
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 4u);
        b = i0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l6;
      if (i0) {goto block_121;}
    }
    block_122:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 1331u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      b = i0;
      if (i0) {goto block_121;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1320u);
      b = i0;
      DeallocateRawBlock(ctx, i0);
    }
    block_121:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l10;
      DeallocateRawBlock(ctx, i0);
      goto block_40;
    }
    block_120:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto exit_path;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_45:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    a = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
  }
  i0 = l8;
  i1 = 24u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 24u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_37;}
  block_44:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 25u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l10;
    i2 = 0u;
    i3 = kDataFFFFFFFF;
    i4 = kMsgSetonpush;
    i5 = 9u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l7 = i0;
    i0 = 25u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
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
    i1 = 0u;
    i2 = ResumePhase(ctx);
    i2 = !(i2);
    if (i2) {
      i2 = c;
      if (i2) {
        i2 = b;
        i2 = LoadU32(ctx, i2 + 8u);
        l5 = i2;
        i2 = b;
        i2 = LoadU8(ctx, i2 + 15u);
        l3 = i2;
        l6 = i2;
        goto block_38;
      }
      i2 = l4;
      i3 = 0u;
      StoreU8(ctx, i2 + 1329u, i3);
      i2 = l4;
      i3 = 1328u;
      i2 += i3;
      i3 = kDataB53F5;
      i3 = LoadU8(ctx, i3);
      StoreU8(ctx, i2, i3);
      i2 = l4;
      i3 = 9u;
      StoreU8(ctx, i2 + 1331u, i3);
      i2 = l4;
      i3 = kMsgKeyrepeat;
      j3 = LoadU64(ctx, i3);
      StoreU64(ctx, i2 + 1320u, j3);
      i2 = l9;
      i2 = LoadU32(ctx, i2);
      l3 = i2;
      i2 = !(i2);
      c = i2;
    }
    i2 = c;
    i0 = i2 ? i0 : i1;
    if (i0) {goto block_200;}
    loop_204: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadU8(ctx, i0 + 27u);
        c = i0;
        i1 = 24u;
        i0 <<= (i1 & 31);
        i1 = 24u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l6 = i0;
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 20u);
        i1 = c;
        i2 = l6;
        i0 = i2 ? i0 : i1;
        c = i0;
        i1 = 9u;
        i0 = i0 < i1;
        l12 = i0;
        i0 = c;
        i1 = 9u;
        i2 = l12;
        i0 = i2 ? i0 : i1;
        l5 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        if (i0) {
          i0 = l4;
          i1 = 1320u;
          i0 += i1;
          i1 = l3;
          i2 = 16u;
          i1 += i2;
          l11 = i1;
          i1 = LoadU32(ctx, i1);
          i2 = l11;
          i3 = l6;
          i1 = i3 ? i1 : i2;
          l6 = i1;
          i2 = l5;
          i0 = CompareBytes(ctx, i0, i1, i2);
          l11 = i0;
          i0 = !(i0);
          l13 = i0;
          if (i0) {
            i0 = c;
            i1 = 9u;
            i0 = i0 <= i1;
            c = i0;
            if (i0) {goto block_212;}
            goto block_206;
          }
          i0 = l11;
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          c = i0;
          if (i0) {goto block_206;}
          block_212:;
          i0 = l6;
          i1 = l4;
          i2 = 1320u;
          i1 += i2;
          i2 = l5;
          i0 = CompareBytes(ctx, i0, i1, i2);
          c = i0;
          i0 = !(i0);
          l6 = i0;
          if (i0) {goto block_210;}
          i0 = c;
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          c = i0;
          if (i0) {goto block_209;}
          goto block_207;
        }
        i0 = c;
        i1 = 9u;
        i0 = i0 > i1;
        c = i0;
        if (i0) {goto block_206;}
        block_210:;
        i0 = l12;
        if (i0) {goto block_207;}
        block_209:;
        i0 = 0u;
        l3 = i0;
        i0 = l4;
        i1 = 1312u;
        i0 += i1;
        i1 = kDataB53F5;
        i1 = LoadU8(ctx, i1);
        StoreU8(ctx, i0, i1);
        i0 = l4;
        i1 = 9u;
        StoreU8(ctx, i0 + 1315u, i1);
        i0 = l4;
        i1 = 0u;
        StoreU8(ctx, i0 + 1313u, i1);
        i0 = l4;
        i1 = kMsgKeyrepeat;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0 + 1304u, j1);
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = l4;
        i2 = 1304u;
        i1 += i2;
        StoreU32(ctx, i0 + 1296u, i1);
        i0 = l4;
        i1 = 1304u;
        i0 += i1;
        l6 = i0;
        i0 = l4;
        i1 = 1296u;
        i0 += i1;
        l5 = i0;
        i0 = l4;
        i1 = 1344u;
        i0 += i1;
        l11 = i0;
        i0 = l4;
        i1 = 1352u;
        i0 += i1;
        c = i0;
      }
      i0 = l8;
      i1 = 26u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1959u;
        i1 = c;
        i2 = l14;
        i3 = l6;
        i4 = kDataB5E30;
        i5 = l5;
        i6 = l11;
        HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
        i0 = 26u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        c = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = c;
        i1 = 1u;
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
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 1352u);
          c = i0;
          i0 = LoadU32(ctx, i0 + 32u);
          l6 = i0;
          i0 = c;
          i0 = LoadU8(ctx, i0 + 39u);
          l5 = i0;
          i1 = 24u;
          i0 <<= (i1 & 31);
          i1 = 24u;
          i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
          i1 = 0u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          l11 = i0;
          i0 = l6;
          i1 = l5;
          i2 = l11;
          i0 = i2 ? i0 : i1;
          i1 = 1u;
          i0 = i0 == i1;
          l6 = i0;
        }
        i0 = l6;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kDataD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = c;
            i1 = 28u;
            i0 += i1;
            c = i0;
          }
          i0 = l8;
          i1 = 27u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 1354u;
            i1 = c;
            i2 = 0u;
            i3 = kDataFFFFFFFF;
            i4 = kDataB4B29;
            i5 = 1u;
            i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
            l7 = i0;
            i0 = 27u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l7;
            c = i0;
          }
          i0 = ResumePhase(ctx);
          if (i0) {
            i0 = l3;
          } else {
            i0 = kDataD015C;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            i0 = kDataD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 1u;
            i0 = i0 == i1;
            if (i0) {goto exit_path;}
            i0 = c;
            i0 = !(i0);
          }
          l3 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 1315u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          c = i0;
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 1304u);
            c = i0;
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l3;
          if (i0) {goto block_200;}
          i0 = l4;
          i1 = 3u;
          StoreU8(ctx, i0 + 1331u, i1);
          i0 = l4;
          i1 = 0u;
          StoreU8(ctx, i0 + 1323u, i1);
          i0 = l4;
          i1 = kDataB53F7;
          i1 = LoadU16(ctx, i1);
          StoreU16(ctx, i0 + 1320u, i1);
          i0 = l4;
          i1 = kDataB53F9;
          i1 = LoadU8(ctx, i1);
          StoreU8(ctx, i0 + 1322u, i1);
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = l4;
          i2 = 1320u;
          i1 += i2;
          StoreU32(ctx, i0 + 1352u, i1);
          i0 = l4;
          i1 = 1320u;
          i0 += i1;
          l3 = i0;
          i0 = l4;
          i1 = 1352u;
          i0 += i1;
          l6 = i0;
          i0 = l4;
          i1 = 1296u;
          i0 += i1;
          l5 = i0;
          i0 = l4;
          i1 = 1304u;
          i0 += i1;
          c = i0;
        }
        i0 = l8;
        i1 = 28u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1959u;
          i1 = c;
          i2 = l14;
          i3 = l3;
          i4 = kDataB5E30;
          i5 = l6;
          i6 = l5;
          HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
          i0 = 28u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          c = i0;
          i0 = kDataD015C;
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
          if (i0) {goto block_228;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 1304u);
          l3 = i0;
          i1 = 28u;
          i0 += i1;
          c = i0;
          i0 = l3;
          i0 = LoadS8(ctx, i0 + 39u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = c;
            i0 = LoadU32(ctx, i0);
          } else {
            i0 = c;
          }
          i0 = SkipWhitespace(ctx, i0);
          c = i0;
          i0 = a;
          i1 = 13756u;
          i0 += i1;
          l5 = i0;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          if (i0) {
            i0 = a;
            i1 = 13756u;
            i0 += i1;
            l5 = i0;
            loop_233: 
              i0 = l3;
              i0 = LoadU32(ctx, i0 + 16u);
              l6 = i0;
              i1 = c;
              i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
              if (i0) {
                i0 = l3;
                i0 = LoadU32(ctx, i0);
                l6 = i0;
                if (i0) {goto block_234;}
                i0 = l3;
                l5 = i0;
                goto block_231;
              }
              i0 = c;
              i1 = l6;
              i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
              if (i0) {goto block_231;}
              i0 = l3;
              i1 = 4u;
              i0 += i1;
              l5 = i0;
              i0 = l3;
              i0 = LoadU32(ctx, i0 + 4u);
              l6 = i0;
              i0 = !(i0);
              if (i0) {goto block_231;}
              i0 = l5;
              l3 = i0;
              block_234:;
              i0 = l3;
              l5 = i0;
              i0 = l6;
              l3 = i0;
              goto loop_233;
            TrapUnreachable();
          }
          i0 = l5;
          l3 = i0;
          block_231:;
          i0 = l5;
          i0 = LoadU32(ctx, i0);
          l6 = i0;
          i0 = !(i0);
          l11 = i0;
        }
        i0 = l11;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kDataD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l8;
          i1 = 29u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 1032u;
            i1 = 64u;
            i0 = HostFoldLayoutOffset(ctx, i0, i1);
            l7 = i0;
            i0 = 29u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l7;
            l6 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kDataD015C;
            i0 = LoadU32(ctx, i0);
            l14 = i0;
            i0 = kDataD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l14;
            i1 = 1u;
            i0 = i0 == i1;
            l11 = i0;
            if (i0) {goto block_228;}
            i0 = l6;
            j1 = 0ull;
            StoreU64(ctx, i0 + 24u, j1);
            i0 = l6;
            i1 = kDataB5F40;
            StoreU32(ctx, i0 + 20u, i1);
            i0 = l6;
            i1 = c;
            StoreU32(ctx, i0 + 16u, i1);
            i0 = l6;
            j1 = 0ull;
            StoreU64(ctx, i0 + 40u, j1);
            i0 = l6;
            j1 = 0ull;
            StoreU64(ctx, i0 + 48u, j1);
            i0 = l6;
            i1 = l3;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = l6;
            j1 = 0ull;
            StoreU64(ctx, i0, j1);
            i0 = l6;
            i1 = 0u;
            StoreU32(ctx, i0 + 32u, i1);
            i0 = l6;
            i1 = l6;
            i2 = 40u;
            i1 += i2;
            StoreU32(ctx, i0 + 36u, i1);
            i0 = l6;
            j1 = 0ull;
            StoreU64(ctx, i0 + 56u, j1);
            i0 = l5;
            i1 = l6;
            StoreU32(ctx, i0, i1);
            i0 = l6;
            l3 = i0;
            i0 = a;
            i0 = LoadU32(ctx, i0 + 13752u);
            i0 = LoadU32(ctx, i0);
            c = i0;
            i0 = !(i0);
            l11 = i0;
            if (i0) {
              i0 = l3;
            } else {
              i0 = a;
              i1 = c;
              StoreU32(ctx, i0 + 13752u, i1);
              i0 = l5;
              i0 = LoadU32(ctx, i0);
            }
            c = i0;
            i0 = a;
            i1 = 13756u;
            i0 += i1;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            i1 = c;
            UnlinkListNode(ctx, i0, i1);
            i0 = a;
            i1 = 13760u;
            i0 += i1;
            a = i0;
            i0 = LoadU32(ctx, i0);
            i1 = 1u;
            i0 += i1;
            c = i0;
            i0 = a;
            i1 = c;
            StoreU32(ctx, i0, i1);
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l6;
          i1 = 24u;
          i0 += i1;
          a = i0;
        }
        i0 = l8;
        i1 = 30u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 870u;
          i1 = a;
          i2 = l10;
          i0 = HostOsQuery(ctx, i0, i1, i2);
          l7 = i0;
          i0 = 30u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l7;
          a = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 == i1;
          a = i0;
          if (i0) {goto block_228;}
          i0 = b;
          i1 = l6;
          i2 = 20u;
          i1 += i2;
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
            i0 = b;
            i0 = LoadU32(ctx, i0 + 16u);
            a = i0;
            i0 = kDataD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l6;
            i1 = 36u;
            i0 += i1;
            c = i0;
          }
          i0 = l8;
          i1 = 31u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 2619u;
            i1 = c;
            i2 = a;
            i3 = l9;
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
            i0 = kDataD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kDataD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 == i1;
            a = i0;
            if (i0) {goto block_228;}
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = b;
          i1 = 28u;
          i0 += i1;
          c = i0;
          i0 = l6;
          i1 = 48u;
          i0 += i1;
          a = i0;
        }
        i0 = l8;
        i1 = 32u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 870u;
          i1 = a;
          i2 = c;
          i0 = HostOsQuery(ctx, i0, i1, i2);
          l7 = i0;
          i0 = 32u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l7;
          a = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 == i1;
          a = i0;
          if (i0) {goto block_228;}
          i0 = l6;
          i1 = b;
          i1 = LoadU32(ctx, i1 + 40u);
          StoreU32(ctx, i0 + 60u, i1);
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 1331u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_6;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 1320u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
          goto block_6;
        }
        block_228:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          a = i0;
          i0 = HostTempRet0(ctx);
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 1331u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          b = i0;
          if (i0) {goto block_8;}
          goto block_5;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        a = i0;
        i0 = HostTempRet0(ctx);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 1315u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        b = i0;
        if (i0) {goto block_8;}
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 1304u);
        DeallocateRawBlock(ctx, i0);
        i0 = a;
        HostOsRelease(ctx, i0);
        TrapUnreachable();
      }
      block_207:;
      i0 = l3;
      i1 = l3;
      i2 = 4u;
      i1 += i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      l3 = i0;
      block_206:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        if (i0) {goto loop_204;}
      }
    block_200:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = 3u;
      StoreU8(ctx, i0 + 1331u, i1);
      i0 = l4;
      i1 = 0u;
      StoreU8(ctx, i0 + 1323u, i1);
      i0 = l4;
      i1 = kDataB53F7;
      i1 = LoadU16(ctx, i1);
      StoreU16(ctx, i0 + 1320u, i1);
      i0 = l4;
      i1 = kDataB53F9;
      i1 = LoadU8(ctx, i1);
      StoreU8(ctx, i0 + 1322u, i1);
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l4;
      i2 = 1320u;
      i1 += i2;
      StoreU32(ctx, i0 + 1352u, i1);
      i0 = l4;
      i1 = 1320u;
      i0 += i1;
      l3 = i0;
      i0 = l4;
      i1 = 1352u;
      i0 += i1;
      l6 = i0;
      i0 = l4;
      i1 = 1296u;
      i0 += i1;
      l5 = i0;
      i0 = l4;
      i1 = 1304u;
      i0 += i1;
      c = i0;
    }
    i0 = l8;
    i1 = 33u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1959u;
      i1 = c;
      i2 = l14;
      i3 = l3;
      i4 = kDataB5E30;
      i5 = l6;
      i6 = l5;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 33u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kDataD015C;
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
      if (i0) {goto block_258;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1304u);
      l3 = i0;
      i1 = 28u;
      i0 += i1;
      c = i0;
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 39u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = c;
        i0 = LoadU32(ctx, i0);
      } else {
        i0 = c;
      }
      i0 = SkipWhitespace(ctx, i0);
      c = i0;
      i0 = a;
      i1 = 13744u;
      i0 += i1;
      l5 = i0;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      if (i0) {
        i0 = a;
        i1 = 13744u;
        i0 += i1;
        l5 = i0;
        loop_263: 
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 16u);
          l6 = i0;
          i1 = c;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          if (i0) {
            i0 = l3;
            i0 = LoadU32(ctx, i0);
            l6 = i0;
            if (i0) {goto block_264;}
            i0 = l3;
            l5 = i0;
            goto block_261;
          }
          i0 = c;
          i1 = l6;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {goto block_261;}
          i0 = l3;
          i1 = 4u;
          i0 += i1;
          l5 = i0;
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 4u);
          l6 = i0;
          i0 = !(i0);
          if (i0) {goto block_261;}
          i0 = l5;
          l3 = i0;
          block_264:;
          i0 = l3;
          l5 = i0;
          i0 = l6;
          l3 = i0;
          goto loop_263;
        TrapUnreachable();
      }
      i0 = l5;
      l3 = i0;
      block_261:;
      i0 = l5;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = !(i0);
      l11 = i0;
    }
    i0 = l11;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l8;
      i1 = 34u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1032u;
        i1 = 64u;
        i0 = HostFoldLayoutOffset(ctx, i0, i1);
        l7 = i0;
        i0 = 34u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l7;
        l6 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l14 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l14;
        i1 = 1u;
        i0 = i0 == i1;
        l11 = i0;
        if (i0) {goto block_258;}
        i0 = l6;
        j1 = 0ull;
        StoreU64(ctx, i0 + 24u, j1);
        i0 = l6;
        i1 = kDataB5F40;
        StoreU32(ctx, i0 + 20u, i1);
        i0 = l6;
        i1 = c;
        StoreU32(ctx, i0 + 16u, i1);
        i0 = l6;
        j1 = 0ull;
        StoreU64(ctx, i0 + 40u, j1);
        i0 = l6;
        j1 = 0ull;
        StoreU64(ctx, i0 + 48u, j1);
        i0 = l6;
        i1 = l3;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l6;
        j1 = 0ull;
        StoreU64(ctx, i0, j1);
        i0 = l6;
        i1 = 0u;
        StoreU32(ctx, i0 + 32u, i1);
        i0 = l6;
        i1 = l6;
        i2 = 40u;
        i1 += i2;
        StoreU32(ctx, i0 + 36u, i1);
        i0 = l6;
        j1 = 0ull;
        StoreU64(ctx, i0 + 56u, j1);
        i0 = l5;
        i1 = l6;
        StoreU32(ctx, i0, i1);
        i0 = l6;
        l3 = i0;
        i0 = a;
        i0 = LoadU32(ctx, i0 + 13740u);
        i0 = LoadU32(ctx, i0);
        c = i0;
        i0 = !(i0);
        l11 = i0;
        if (i0) {
          i0 = l3;
        } else {
          i0 = a;
          i1 = c;
          StoreU32(ctx, i0 + 13740u, i1);
          i0 = l5;
          i0 = LoadU32(ctx, i0);
        }
        c = i0;
        i0 = a;
        i1 = 13744u;
        i0 += i1;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        i1 = c;
        UnlinkListNode(ctx, i0, i1);
        i0 = a;
        i1 = 13748u;
        i0 += i1;
        a = i0;
        i0 = LoadU32(ctx, i0);
        i1 = 1u;
        i0 += i1;
        c = i0;
        i0 = a;
        i1 = c;
        StoreU32(ctx, i0, i1);
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 24u;
      i0 += i1;
      a = i0;
    }
    i0 = l8;
    i1 = 35u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 870u;
      i1 = a;
      i2 = l10;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l7 = i0;
      i0 = 35u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      a = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_258;}
      i0 = b;
      i1 = l6;
      i2 = 20u;
      i1 += i2;
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
        i0 = b;
        i0 = LoadU32(ctx, i0 + 16u);
        a = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l6;
        i1 = 36u;
        i0 += i1;
        c = i0;
      }
      i0 = l8;
      i1 = 36u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 2619u;
        i1 = c;
        i2 = a;
        i3 = l9;
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
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 == i1;
        a = i0;
        if (i0) {goto block_258;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 28u;
      i0 += i1;
      c = i0;
      i0 = l6;
      i1 = 48u;
      i0 += i1;
      a = i0;
    }
    i0 = l8;
    i1 = 37u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 870u;
      i1 = a;
      i2 = c;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l7 = i0;
      i0 = 37u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      a = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_258;}
      i0 = l6;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 40u);
      StoreU32(ctx, i0 + 60u, i1);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 1331u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_6;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1320u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_6;
    }
    block_258:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 1331u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      b = i0;
      if (i0) {goto block_8;}
      goto block_5;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_43:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_40;
  }
  block_42:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    i2 = l6;
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
    if (i0) {goto block_285;}
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 38u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l10;
    i2 = 0u;
    i3 = kDataFFFFFFFF;
    i4 = kMsgSetonrollover;
    i5 = 13u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l7 = i0;
    i0 = 38u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 != i1;
    l6 = i0;
  }
  i0 = l6;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l5 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l3 = i0;
        l6 = i0;
        goto block_285;
      }
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1296u);
      l12 = i0;
      i0 = !(i0);
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l8;
      i1 = 39u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kMsgPx;
        i2 = kMsgSmartPtrPath;
        i3 = 734u;
        i4 = kMsgOperator;
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
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_37;}
        goto exit_path;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = 0u;
      l5 = i0;
    }
    i0 = l8;
    i1 = 40u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 16u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l7 = i0;
      i0 = 40u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l10 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    i1 = 0u;
    i2 = ResumePhase(ctx);
    i2 = !(i2);
    if (i2) {
      i2 = kDataD015C;
      i2 = LoadU32(ctx, i2);
      l3 = i2;
      i2 = kDataD015C;
      i3 = 0u;
      StoreU32(ctx, i2, i3);
      i2 = 1u;
      l6 = i2;
      i2 = l3;
      i3 = 1u;
      i2 = i2 == i3;
      l3 = i2;
      if (i2) {goto block_288;}
      i2 = l10;
      i3 = kDataB5340;
      i3 = LoadU32(ctx, i3);
      StoreU32(ctx, i2 + 7u, i3);
      i2 = l10;
      i3 = kMsgPenetration;
      j3 = LoadU64(ctx, i3);
      StoreU64(ctx, i2, j3);
      i2 = l10;
      i3 = 0u;
      StoreU8(ctx, i2 + 11u, i3);
      i2 = l9;
      i2 = LoadU32(ctx, i2);
      l3 = i2;
      i2 = !(i2);
      l9 = i2;
    }
    i2 = l9;
    i0 = i2 ? i0 : i1;
    if (i0) {goto block_311;}
    loop_315: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadU8(ctx, i0 + 27u);
        l5 = i0;
        i1 = 24u;
        i0 <<= (i1 & 31);
        i1 = 24u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l11 = i0;
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 20u);
        i1 = l5;
        i2 = l11;
        i0 = i2 ? i0 : i1;
        l5 = i0;
        i1 = 11u;
        i0 = i0 < i1;
        l13 = i0;
        i0 = l5;
        i1 = 11u;
        i2 = l13;
        i0 = i2 ? i0 : i1;
        l9 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l9;
        if (i0) {
          i0 = l10;
          i1 = l3;
          i2 = 16u;
          i1 += i2;
          l7 = i1;
          i1 = LoadU32(ctx, i1);
          i2 = l7;
          i3 = l11;
          i1 = i3 ? i1 : i2;
          l11 = i1;
          i2 = l9;
          i0 = CompareBytes(ctx, i0, i1, i2);
          l7 = i0;
          i0 = !(i0);
          if (i0) {
            i0 = l5;
            i1 = 11u;
            i0 = i0 <= i1;
            l5 = i0;
            if (i0) {goto block_323;}
            goto block_317;
          }
          i0 = l7;
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          l5 = i0;
          if (i0) {goto block_317;}
          block_323:;
          i0 = l11;
          i1 = l10;
          i2 = l9;
          i0 = CompareBytes(ctx, i0, i1, i2);
          l5 = i0;
          i0 = !(i0);
          l9 = i0;
          if (i0) {goto block_321;}
          i0 = l5;
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          l5 = i0;
          if (i0) {goto block_320;}
          goto block_318;
        }
        i0 = l5;
        i1 = 11u;
        i0 = i0 > i1;
        l5 = i0;
        if (i0) {goto block_317;}
        block_321:;
        i0 = l13;
        if (i0) {goto block_318;}
        block_320:;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l8;
      i1 = 41u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1032u;
        i1 = 16u;
        i0 = HostFoldLayoutOffset(ctx, i0, i1);
        l7 = i0;
        i0 = 41u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l7;
        l3 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 1u;
        i0 = i0 == i1;
        l6 = i0;
        if (i0) {goto block_310;}
        i0 = l4;
        i1 = l3;
        StoreU32(ctx, i0 + 1320u, i1);
        i0 = l4;
        j1 = 9223372105574252555ull;
        StoreU64(ctx, i0 + 1324u, j1);
        i0 = l3;
        i1 = 0u;
        StoreU8(ctx, i0 + 11u, i1);
        i0 = l3;
        i1 = kDataB5340;
        i1 = LoadU32(ctx, i1);
        StoreU32(ctx, i0 + 7u, i1);
        i0 = l3;
        i1 = kMsgPenetration;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0, j1);
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = l4;
        i2 = 1320u;
        i1 += i2;
        StoreU32(ctx, i0 + 1352u, i1);
        i0 = l4;
        i1 = 1320u;
        i0 += i1;
        l6 = i0;
        i0 = l4;
        i1 = 1352u;
        i0 += i1;
        l5 = i0;
        i0 = l4;
        i1 = 1344u;
        i0 += i1;
        l9 = i0;
        i0 = l4;
        i1 = 1304u;
        i0 += i1;
        l3 = i0;
      }
      i0 = l8;
      i1 = 42u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1959u;
        i1 = l3;
        i2 = l14;
        i3 = l6;
        i4 = kDataB5E30;
        i5 = l5;
        i6 = l9;
        HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
        i0 = 42u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 != i1;
        l3 = i0;
        if (i0) {goto block_314;}
        i0 = HostRet0(ctx);
        a = i0;
        i0 = HostTempRet0(ctx);
        b = i0;
        goto block_303;
      }
      block_318:;
      i0 = l3;
      i1 = l3;
      i2 = 4u;
      i1 += i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      l3 = i0;
      block_317:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        if (i0) {goto loop_315;}
      }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = 0u;
      l5 = i0;
      goto block_311;
    }
    block_314:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = 0u;
      l6 = i0;
      i0 = 1u;
      l5 = i0;
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1304u);
      l3 = i0;
      i0 = LoadU32(ctx, i0 + 32u);
      l11 = i0;
      i0 = l3;
      i0 = LoadU8(ctx, i0 + 39u);
      l9 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l13 = i0;
      i0 = l11;
      i1 = l9;
      i2 = l13;
      i0 = i2 ? i0 : i1;
      i1 = 1u;
      i0 = i0 != i1;
      l9 = i0;
      if (i0) {goto block_311;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 28u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l8;
    i1 = 43u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = l3;
      i2 = 0u;
      i3 = kDataFFFFFFFF;
      i4 = kDataB4B29;
      i5 = 1u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l7 = i0;
      i0 = 43u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto exit_path;}
      i0 = l3;
      i1 = 0u;
      i0 = i0 != i1;
      l5 = i0;
    }
    block_311:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1032u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l8;
    i1 = 44u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2591u;
      i1 = l3;
      i2 = b;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l7 = i0;
      i0 = 44u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l9 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l9;
      i1 = 1u;
      i0 = i0 == i1;
      l9 = i0;
      if (i0) {goto block_309;}
      i0 = l4;
      i1 = 0u;
      StoreU32(ctx, i0 + 1028u, i1);
      i0 = l4;
      i1 = 2648u;
      StoreU32(ctx, i0 + 1024u, i1);
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l4;
      j1 = LoadU64(ctx, i1 + 1024u);
      StoreU64(ctx, i0 + 8u, j1);
      i0 = l4;
      i1 = 8u;
      i0 += i1;
      l11 = i0;
      i0 = l4;
      i1 = 1080u;
      i0 += i1;
      l9 = i0;
    }
    i0 = l8;
    i1 = 45u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2649u;
      i1 = l9;
      i2 = l11;
      i3 = a;
      i4 = 1u;
      i5 = l5;
      i6 = l3;
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
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_308;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1080u;
      i0 += i1;
      l9 = i0;
      i0 = l4;
      i1 = 1144u;
      i0 += i1;
      l5 = i0;
    }
    i0 = l8;
    i1 = 46u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2650u;
      i1 = l5;
      i2 = l9;
      i3 = 0u;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l7 = i0;
      i0 = 46u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l5 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l9 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l9;
      i1 = 1u;
      i0 = i0 == i1;
      l9 = i0;
      if (i0) {goto block_306;}
      i0 = l12;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 272u);
      l9 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 47u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l9;
      i1 = l12;
      i2 = 1u;
      i3 = l5;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 47u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l9 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l9;
      i1 = 1u;
      i0 = i0 == i1;
      l9 = i0;
      if (i0) {goto block_307;}
      i0 = l5;
      i0 = LoadU32(ctx, i0);
      l9 = i0;
    }
    i0 = l9;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l11;
      i1 = l9;
      i2 = 1u;
      i1 &= i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      l11 = i0;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l11;
        if (i0) {goto block_345;}
        i0 = l9;
        i1 = kDataFFFFFFFE;
        i0 &= i1;
        i0 = LoadU32(ctx, i0);
        l9 = i0;
        i0 = !(i0);
        l11 = i0;
        if (i0) {goto block_345;}
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l9;
        l11 = i0;
        i0 = l5;
        i1 = kResumeWordBytes;
        i0 += i1;
        l9 = i0;
      }
      i0 = l8;
      i1 = 48u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l11;
        i1 = l9;
        i2 = l9;
        i3 = 2u;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 48u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l9 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l9;
        i1 = 1u;
        i0 = i0 == i1;
        l9 = i0;
        if (i0) {goto block_301;}
      }
      block_345:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = 1100u;
      i0 += i1;
      i1 = kDataB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1139u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 1128u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l4;
      i1 = 1116u;
      i0 += i1;
      i1 = l4;
      i2 = 1120u;
      i1 += i2;
      i1 = LoadU32(ctx, i1);
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l4;
      i1 = 1115u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 1104u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l3;
      i1 = kDataB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 39u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 28u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l3;
      i1 = 16u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 20u);
      l9 = i1;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 15u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l5 = i0;
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 4u);
        l3 = i0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l6;
      if (i0) {goto block_355;}
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 1331u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      l3 = i0;
      if (i0) {goto block_355;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1320u);
      l3 = i0;
      DeallocateRawBlock(ctx, i0);
      block_355:;
      i0 = l10;
      DeallocateRawBlock(ctx, i0);
      goto block_41;
    }
    block_310:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
      i0 = l10;
      DeallocateRawBlock(ctx, i0);
      goto block_40;
    }
    block_309:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = l6;
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_303;}
      goto block_302;
    }
    block_308:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
      goto block_304;
    }
    block_307:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = l5;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = !(i0);
      c = i0;
      if (i0) {goto block_305;}
      i0 = b;
      i1 = 1u;
      i0 &= i1;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = c;
      if (i0) {goto block_361;}
      i0 = b;
      i1 = kDataFFFFFFFE;
      i0 &= i1;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = !(i0);
      c = i0;
      if (i0) {goto block_361;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      c = i0;
      i0 = l5;
      i1 = 4u;
      i0 += i1;
      b = i0;
    }
    i0 = l8;
    i1 = 49u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = b;
      i2 = b;
      i3 = 2u;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 49u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
      if (i0) {goto block_360;}
    }
    block_361:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      goto block_305;
    }
    block_360:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto exit_path;}
    block_306:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
    }
    block_305:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = 1100u;
      i0 += i1;
      i1 = kDataB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1139u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 1128u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l4;
      i1 = 1116u;
      i0 += i1;
      i1 = l4;
      i2 = 1120u;
      i1 += i2;
      i1 = LoadU32(ctx, i1);
      c = i1;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l4;
      i1 = 1115u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      b = i0;
      if (i0) {goto block_304;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1104u);
      b = i0;
      DeallocateRawBlock(ctx, i0);
    }
    block_304:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = kDataB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 39u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 28u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l3;
      i1 = 16u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 20u);
      c = i1;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 15u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      b = i0;
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 4u);
        b = i0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l6;
      if (i0) {goto block_302;}
    }
    block_303:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 1331u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      b = i0;
      if (i0) {goto block_302;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1320u);
      b = i0;
      DeallocateRawBlock(ctx, i0);
    }
    block_302:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l10;
      DeallocateRawBlock(ctx, i0);
      goto block_40;
    }
    block_301:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto exit_path;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_288:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_40;
  }
  block_285:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    i2 = l6;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 12u;
    i0 = i0 != i1;
    l9 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l9;
    if (i0) {goto block_377;}
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 50u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l10;
    i2 = 0u;
    i3 = kDataFFFFFFFF;
    i4 = kMsgSetonrollout;
    i5 = 12u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l7 = i0;
    i0 = 50u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 != i1;
    l6 = i0;
  }
  i0 = l6;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l5 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l3 = i0;
        l6 = i0;
        goto block_377;
      }
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1296u);
      l6 = i0;
      i0 = !(i0);
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l8;
      i1 = 51u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kMsgPx;
        i2 = kMsgSmartPtrPath;
        i3 = 734u;
        i4 = kMsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 51u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_37;}
        goto exit_path;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 896u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l8;
    i1 = 52u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2591u;
      i1 = l3;
      i2 = b;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l7 = i0;
      i0 = 52u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_376;}
      i0 = l4;
      i1 = 0u;
      StoreU32(ctx, i0 + 892u, i1);
      i0 = l4;
      i1 = 2648u;
      StoreU32(ctx, i0 + 888u, i1);
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l4;
      j1 = LoadU64(ctx, i1 + 888u);
      StoreU64(ctx, i0 + 16u, j1);
      i0 = l4;
      i1 = 16u;
      i0 += i1;
      l9 = i0;
      i0 = l4;
      i1 = 944u;
      i0 += i1;
      l5 = i0;
    }
    i0 = l8;
    i1 = 53u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2649u;
      i1 = l5;
      i2 = l9;
      i3 = a;
      i4 = 2u;
      i5 = 1u;
      i6 = l3;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 53u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kDataD015C;
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 944u;
        i0 += i1;
        l9 = i0;
        i0 = l4;
        i1 = 1008u;
        i0 += i1;
        l5 = i0;
      }
      i0 = l8;
      i1 = 54u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 2650u;
        i1 = l5;
        i2 = l9;
        i3 = 0u;
        i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
        l7 = i0;
        i0 = 54u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l7;
        l5 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l10 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l10;
        i1 = 1u;
        i0 = i0 == i1;
        l9 = i0;
        if (i0) {goto block_397;}
        i0 = l6;
        i0 = LoadU32(ctx, i0);
        l9 = i0;
        i0 = LoadU32(ctx, i0 + 272u);
        l10 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l8;
      i1 = 55u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l10;
        i1 = l6;
        i2 = 2u;
        i3 = l5;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 55u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l6 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l6;
        i1 = 1u;
        i0 = i0 == i1;
        l6 = i0;
        if (i0) {goto block_398;}
        i0 = l5;
        i0 = LoadU32(ctx, i0);
        l6 = i0;
      }
      i0 = l6;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = l9;
        i1 = l6;
        i2 = 1u;
        i1 &= i2;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        l9 = i0;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l9;
          if (i0) {goto block_409;}
          i0 = l6;
          i1 = kDataFFFFFFFE;
          i0 &= i1;
          i0 = LoadU32(ctx, i0);
          l6 = i0;
          i0 = !(i0);
          l9 = i0;
          if (i0) {goto block_409;}
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l6;
          l9 = i0;
          i0 = l5;
          i1 = 4u;
          i0 += i1;
          l6 = i0;
        }
        i0 = l8;
        i1 = 56u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l9;
          i1 = l6;
          i2 = l6;
          i3 = 2u;
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
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          l6 = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l6;
          i1 = 1u;
          i0 = i0 == i1;
          l6 = i0;
          if (i0) {goto block_380;}
        }
        block_409:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l5;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i1 = kDataB5F40;
        StoreU32(ctx, i0 + 964u, i1);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 1003u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 992u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l4;
        i1 = 980u;
        i0 += i1;
        i1 = l4;
        i1 = LoadU32(ctx, i1 + 984u);
        ReleaseNodeTree(ctx, i0, i1);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 979u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 968u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l3;
        i1 = kDataB5F40;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 39u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 28u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l3;
        i1 = 16u;
        i0 += i1;
        i1 = l3;
        i1 = LoadU32(ctx, i1 + 20u);
        l5 = i1;
        ReleaseNodeTree(ctx, i0, i1);
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 15u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        l6 = i0;
        if (i0) {goto block_41;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 4u);
        l3 = i0;
        DeallocateRawBlock(ctx, i0);
        goto block_41;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
      goto block_395;
    }
    block_398:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = l5;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = !(i0);
      c = i0;
      if (i0) {goto block_396;}
      i0 = b;
      i1 = 1u;
      i0 &= i1;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = c;
      if (i0) {goto block_421;}
      i0 = b;
      i1 = kDataFFFFFFFE;
      i0 &= i1;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = !(i0);
      c = i0;
      if (i0) {goto block_421;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      c = i0;
      i0 = l5;
      i1 = 4u;
      i0 += i1;
      b = i0;
    }
    i0 = l8;
    i1 = 57u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = b;
      i2 = b;
      i3 = 2u;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 57u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
      if (i0) {goto block_420;}
    }
    block_421:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      goto block_396;
    }
    block_420:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto exit_path;}
    block_397:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
    }
    block_396:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = kDataB5F40;
      StoreU32(ctx, i0 + 964u, i1);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 1003u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 992u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l4;
      i1 = 980u;
      i0 += i1;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 984u);
      c = i1;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 979u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      b = i0;
      if (i0) {goto block_395;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 968u);
      b = i0;
      DeallocateRawBlock(ctx, i0);
    }
    block_395:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = kDataB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 39u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 28u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l3;
      i1 = 16u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 20u);
      c = i1;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 15u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      b = i0;
      if (i0) {goto block_40;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 4u);
      b = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_40;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_380:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_377:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    i2 = l6;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 11u;
    i0 = i0 != i1;
    l9 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l9;
    if (i0) {goto block_432;}
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 58u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l10;
    i2 = 0u;
    i3 = kDataFFFFFFFF;
    i4 = kMsgSetondragin;
    i5 = 11u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l7 = i0;
    i0 = 58u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 != i1;
    l6 = i0;
  }
  i0 = l6;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l5 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l3 = i0;
        l6 = i0;
        goto block_432;
      }
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1296u);
      l6 = i0;
      i0 = !(i0);
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l8;
      i1 = 59u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kMsgPx;
        i2 = kMsgSmartPtrPath;
        i3 = 734u;
        i4 = kMsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 59u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_37;}
        goto exit_path;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 760u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l8;
    i1 = 60u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2591u;
      i1 = l3;
      i2 = b;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l7 = i0;
      i0 = 60u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_376;}
      i0 = l4;
      i1 = 0u;
      StoreU32(ctx, i0 + 756u, i1);
      i0 = l4;
      i1 = 2648u;
      StoreU32(ctx, i0 + 752u, i1);
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l4;
      j1 = LoadU64(ctx, i1 + 752u);
      StoreU64(ctx, i0 + 24u, j1);
      i0 = l4;
      i1 = 24u;
      i0 += i1;
      l9 = i0;
      i0 = l4;
      i1 = 808u;
      i0 += i1;
      l5 = i0;
    }
    i0 = l8;
    i1 = 61u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2649u;
      i1 = l5;
      i2 = l9;
      i3 = a;
      i4 = 3u;
      i5 = 1u;
      i6 = l3;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 61u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kDataD015C;
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 808u;
        i0 += i1;
        l9 = i0;
        i0 = l4;
        i1 = 872u;
        i0 += i1;
        l5 = i0;
      }
      i0 = l8;
      i1 = 62u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 2650u;
        i1 = l5;
        i2 = l9;
        i3 = 0u;
        i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
        l7 = i0;
        i0 = 62u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l7;
        l5 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l10 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l10;
        i1 = 1u;
        i0 = i0 == i1;
        l9 = i0;
        if (i0) {goto block_452;}
        i0 = l6;
        i0 = LoadU32(ctx, i0);
        l9 = i0;
        i0 = LoadU32(ctx, i0 + 272u);
        l10 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l8;
      i1 = 63u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l10;
        i1 = l6;
        i2 = 3u;
        i3 = l5;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 63u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l6 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l6;
        i1 = 1u;
        i0 = i0 == i1;
        l6 = i0;
        if (i0) {goto block_453;}
        i0 = l5;
        i0 = LoadU32(ctx, i0);
        l6 = i0;
      }
      i0 = l6;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = l9;
        i1 = l6;
        i2 = 1u;
        i1 &= i2;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        l9 = i0;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l9;
          if (i0) {goto block_464;}
          i0 = l6;
          i1 = kDataFFFFFFFE;
          i0 &= i1;
          i0 = LoadU32(ctx, i0);
          l6 = i0;
          i0 = !(i0);
          l9 = i0;
          if (i0) {goto block_464;}
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l6;
          l9 = i0;
          i0 = l5;
          i1 = 4u;
          i0 += i1;
          l6 = i0;
        }
        i0 = l8;
        i1 = 64u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l9;
          i1 = l6;
          i2 = l6;
          i3 = 2u;
          HostSyncSlot(ctx, i0, i1, i2, i3);
          i0 = 64u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          l6 = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l6;
          i1 = 1u;
          i0 = i0 == i1;
          l6 = i0;
          if (i0) {goto block_435;}
        }
        block_464:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l5;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i1 = kDataB5F40;
        StoreU32(ctx, i0 + 828u, i1);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 867u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 856u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l4;
        i1 = 844u;
        i0 += i1;
        i1 = l4;
        i1 = LoadU32(ctx, i1 + 848u);
        ReleaseNodeTree(ctx, i0, i1);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 843u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 832u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l3;
        i1 = kDataB5F40;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 39u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 28u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l3;
        i1 = 16u;
        i0 += i1;
        i1 = l3;
        i1 = LoadU32(ctx, i1 + 20u);
        l5 = i1;
        ReleaseNodeTree(ctx, i0, i1);
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 15u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        l6 = i0;
        if (i0) {goto block_41;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 4u);
        l3 = i0;
        DeallocateRawBlock(ctx, i0);
        goto block_41;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
      goto block_450;
    }
    block_453:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = l5;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = !(i0);
      c = i0;
      if (i0) {goto block_451;}
      i0 = b;
      i1 = 1u;
      i0 &= i1;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = c;
      if (i0) {goto block_476;}
      i0 = b;
      i1 = kDataFFFFFFFE;
      i0 &= i1;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = !(i0);
      c = i0;
      if (i0) {goto block_476;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      c = i0;
      i0 = l5;
      i1 = 4u;
      i0 += i1;
      b = i0;
    }
    i0 = l8;
    i1 = 65u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = b;
      i2 = b;
      i3 = 2u;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 65u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
      if (i0) {goto block_475;}
    }
    block_476:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      goto block_451;
    }
    block_475:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto exit_path;}
    block_452:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
    }
    block_451:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = kDataB5F40;
      StoreU32(ctx, i0 + 828u, i1);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 867u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 856u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l4;
      i1 = 844u;
      i0 += i1;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 848u);
      c = i1;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 843u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      b = i0;
      if (i0) {goto block_450;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 832u);
      b = i0;
      DeallocateRawBlock(ctx, i0);
    }
    block_450:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = kDataB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 39u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 28u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l3;
      i1 = 16u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 20u);
      c = i1;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 15u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      b = i0;
      if (i0) {goto block_40;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 4u);
      b = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_40;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_435:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_432:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    i2 = l6;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = 9u;
    i0 = i0 != i1;
    l9 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l9;
    if (i0) {goto block_487;}
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 66u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l10;
    i2 = 0u;
    i3 = kDataFFFFFFFF;
    i4 = kMsgSetondrag;
    i5 = 9u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l7 = i0;
    i0 = 66u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 != i1;
    l6 = i0;
  }
  i0 = l6;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l5 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l3 = i0;
        l6 = i0;
        goto block_487;
      }
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1296u);
      l6 = i0;
      i0 = !(i0);
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l8;
      i1 = 67u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kMsgPx;
        i2 = kMsgSmartPtrPath;
        i3 = 734u;
        i4 = kMsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 67u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_37;}
        goto exit_path;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 624u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l8;
    i1 = 68u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2591u;
      i1 = l3;
      i2 = b;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l7 = i0;
      i0 = 68u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_376;}
      i0 = l4;
      i1 = 0u;
      StoreU32(ctx, i0 + 620u, i1);
      i0 = l4;
      i1 = 2648u;
      StoreU32(ctx, i0 + 616u, i1);
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l4;
      j1 = LoadU64(ctx, i1 + 616u);
      StoreU64(ctx, i0 + 32u, j1);
      i0 = l4;
      i1 = 32u;
      i0 += i1;
      l9 = i0;
      i0 = l4;
      i1 = 672u;
      i0 += i1;
      l5 = i0;
    }
    i0 = l8;
    i1 = 69u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2649u;
      i1 = l5;
      i2 = l9;
      i3 = a;
      i4 = 4u;
      i5 = 1u;
      i6 = l3;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 69u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kDataD015C;
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 672u;
        i0 += i1;
        l9 = i0;
        i0 = l4;
        i1 = 736u;
        i0 += i1;
        l5 = i0;
      }
      i0 = l8;
      i1 = 70u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 2650u;
        i1 = l5;
        i2 = l9;
        i3 = 0u;
        i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
        l7 = i0;
        i0 = 70u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l7;
        l5 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l10 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l10;
        i1 = 1u;
        i0 = i0 == i1;
        l9 = i0;
        if (i0) {goto block_507;}
        i0 = l6;
        i0 = LoadU32(ctx, i0);
        l9 = i0;
        i0 = LoadU32(ctx, i0 + 272u);
        l10 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l8;
      i1 = 71u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l10;
        i1 = l6;
        i2 = 4u;
        i3 = l5;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 71u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l6 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l6;
        i1 = 1u;
        i0 = i0 == i1;
        l6 = i0;
        if (i0) {goto block_508;}
        i0 = l5;
        i0 = LoadU32(ctx, i0);
        l6 = i0;
      }
      i0 = l6;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = l9;
        i1 = l6;
        i2 = 1u;
        i1 &= i2;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        l9 = i0;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l9;
          if (i0) {goto block_519;}
          i0 = l6;
          i1 = kDataFFFFFFFE;
          i0 &= i1;
          i0 = LoadU32(ctx, i0);
          l6 = i0;
          i0 = !(i0);
          l9 = i0;
          if (i0) {goto block_519;}
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l6;
          l9 = i0;
          i0 = l5;
          i1 = 4u;
          i0 += i1;
          l6 = i0;
        }
        i0 = l8;
        i1 = 72u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l9;
          i1 = l6;
          i2 = l6;
          i3 = 2u;
          HostSyncSlot(ctx, i0, i1, i2, i3);
          i0 = 72u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          l6 = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l6;
          i1 = 1u;
          i0 = i0 == i1;
          l6 = i0;
          if (i0) {goto block_490;}
        }
        block_519:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l5;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i1 = kDataB5F40;
        StoreU32(ctx, i0 + 692u, i1);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 731u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 720u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l4;
        i1 = 708u;
        i0 += i1;
        i1 = l4;
        i1 = LoadU32(ctx, i1 + 712u);
        ReleaseNodeTree(ctx, i0, i1);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 707u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 696u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l3;
        i1 = kDataB5F40;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 39u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 28u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l3;
        i1 = 16u;
        i0 += i1;
        i1 = l3;
        i1 = LoadU32(ctx, i1 + 20u);
        l5 = i1;
        ReleaseNodeTree(ctx, i0, i1);
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 15u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        l6 = i0;
        if (i0) {goto block_41;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 4u);
        l3 = i0;
        DeallocateRawBlock(ctx, i0);
        goto block_41;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
      goto block_505;
    }
    block_508:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = l5;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = !(i0);
      c = i0;
      if (i0) {goto block_506;}
      i0 = b;
      i1 = 1u;
      i0 &= i1;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = c;
      if (i0) {goto block_531;}
      i0 = b;
      i1 = kDataFFFFFFFE;
      i0 &= i1;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = !(i0);
      c = i0;
      if (i0) {goto block_531;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      c = i0;
      i0 = l5;
      i1 = 4u;
      i0 += i1;
      b = i0;
    }
    i0 = l8;
    i1 = 73u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = b;
      i2 = b;
      i3 = 2u;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 73u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
      if (i0) {goto block_530;}
    }
    block_531:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      goto block_506;
    }
    block_530:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto exit_path;}
    block_507:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
    }
    block_506:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = kDataB5F40;
      StoreU32(ctx, i0 + 692u, i1);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 731u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 720u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l4;
      i1 = 708u;
      i0 += i1;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 712u);
      c = i1;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 707u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      b = i0;
      if (i0) {goto block_505;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 696u);
      b = i0;
      DeallocateRawBlock(ctx, i0);
    }
    block_505:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = kDataB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 39u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 28u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l3;
      i1 = 16u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 20u);
      c = i1;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 15u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      b = i0;
      if (i0) {goto block_40;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 4u);
      b = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_40;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_490:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_487:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    i2 = l6;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    l6 = i2;
    i0 = i2 ? i0 : i1;
    i1 = 12u;
    i0 = i0 != i1;
    l3 = i0;
    if (i0) {goto block_41;}
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 74u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l10;
    i2 = 0u;
    i3 = kDataFFFFFFFF;
    i4 = kMsgSetondragout;
    i5 = 12u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l7 = i0;
    i0 = 74u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 != i1;
    l6 = i0;
  }
  i0 = l6;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      if (i0) {goto block_41;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1296u);
      l6 = i0;
      i0 = !(i0);
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l8;
      i1 = 75u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kMsgPx;
        i2 = kMsgSmartPtrPath;
        i3 = 734u;
        i4 = kMsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 75u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_37;}
        goto exit_path;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 488u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l8;
    i1 = 76u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2591u;
      i1 = l3;
      i2 = b;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l7 = i0;
      i0 = 76u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_376;}
      i0 = l4;
      i1 = 0u;
      StoreU32(ctx, i0 + 484u, i1);
      i0 = l4;
      i1 = 2648u;
      StoreU32(ctx, i0 + 480u, i1);
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l4;
      j1 = LoadU64(ctx, i1 + 480u);
      StoreU64(ctx, i0 + 40u, j1);
      i0 = l4;
      i1 = 40u;
      i0 += i1;
      l9 = i0;
      i0 = l4;
      i1 = 536u;
      i0 += i1;
      l5 = i0;
    }
    i0 = l8;
    i1 = 77u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2649u;
      i1 = l5;
      i2 = l9;
      i3 = a;
      i4 = 5u;
      i5 = 1u;
      i6 = l3;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 77u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kDataD015C;
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 536u;
        i0 += i1;
        l9 = i0;
        i0 = l4;
        i1 = 600u;
        i0 += i1;
        l5 = i0;
      }
      i0 = l8;
      i1 = 78u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 2650u;
        i1 = l5;
        i2 = l9;
        i3 = 0u;
        i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
        l7 = i0;
        i0 = 78u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l7;
        l5 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l10 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l10;
        i1 = 1u;
        i0 = i0 == i1;
        l9 = i0;
        if (i0) {goto block_559;}
        i0 = l6;
        i0 = LoadU32(ctx, i0);
        l9 = i0;
        i0 = LoadU32(ctx, i0 + 272u);
        l10 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l8;
      i1 = 79u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l10;
        i1 = l6;
        i2 = 5u;
        i3 = l5;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 79u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l6 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l6;
        i1 = 1u;
        i0 = i0 == i1;
        l6 = i0;
        if (i0) {goto block_560;}
        i0 = l5;
        i0 = LoadU32(ctx, i0);
        l6 = i0;
      }
      i0 = l6;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = l9;
        i1 = l6;
        i2 = 1u;
        i1 &= i2;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        l9 = i0;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l9;
          if (i0) {goto block_571;}
          i0 = l6;
          i1 = kDataFFFFFFFE;
          i0 &= i1;
          i0 = LoadU32(ctx, i0);
          l6 = i0;
          i0 = !(i0);
          l9 = i0;
          if (i0) {goto block_571;}
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l6;
          l9 = i0;
          i0 = l5;
          i1 = 4u;
          i0 += i1;
          l6 = i0;
        }
        i0 = l8;
        i1 = 80u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l9;
          i1 = l6;
          i2 = l6;
          i3 = 2u;
          HostSyncSlot(ctx, i0, i1, i2, i3);
          i0 = 80u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          l6 = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l6;
          i1 = 1u;
          i0 = i0 == i1;
          l6 = i0;
          if (i0) {goto block_543;}
        }
        block_571:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l5;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i1 = kDataB5F40;
        StoreU32(ctx, i0 + 556u, i1);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 595u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 584u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l4;
        i1 = 572u;
        i0 += i1;
        i1 = l4;
        i1 = LoadU32(ctx, i1 + 576u);
        ReleaseNodeTree(ctx, i0, i1);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 571u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 560u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l3;
        i1 = kDataB5F40;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 39u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 28u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l3;
        i1 = 16u;
        i0 += i1;
        i1 = l3;
        i1 = LoadU32(ctx, i1 + 20u);
        l5 = i1;
        ReleaseNodeTree(ctx, i0, i1);
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 15u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        l6 = i0;
        if (i0) {goto block_41;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 4u);
        l3 = i0;
        DeallocateRawBlock(ctx, i0);
        goto block_41;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
      goto block_557;
    }
    block_560:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = l5;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = !(i0);
      c = i0;
      if (i0) {goto block_558;}
      i0 = b;
      i1 = 1u;
      i0 &= i1;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = c;
      if (i0) {goto block_583;}
      i0 = b;
      i1 = kDataFFFFFFFE;
      i0 &= i1;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = !(i0);
      c = i0;
      if (i0) {goto block_583;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      c = i0;
      i0 = l5;
      i1 = 4u;
      i0 += i1;
      b = i0;
    }
    i0 = l8;
    i1 = 81u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = b;
      i2 = b;
      i3 = 2u;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 81u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
      if (i0) {goto block_582;}
    }
    block_583:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      goto block_558;
    }
    block_582:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto exit_path;}
    block_559:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
    }
    block_558:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = kDataB5F40;
      StoreU32(ctx, i0 + 556u, i1);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 595u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 584u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l4;
      i1 = 572u;
      i0 += i1;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 576u);
      c = i1;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 571u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      b = i0;
      if (i0) {goto block_557;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 560u);
      b = i0;
      DeallocateRawBlock(ctx, i0);
    }
    block_557:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = kDataB5F40;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 39u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 28u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l3;
      i1 = 16u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 20u);
      c = i1;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 15u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      b = i0;
      if (i0) {goto block_40;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 4u);
      b = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_40;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_543:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_376:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_40;
  }
  block_41:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    if (i0) {goto block_39;}
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
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 82u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kMsgPx;
      i2 = kMsgSmartPtrPath;
      i3 = 734u;
      i4 = kMsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 82u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_37;}
      goto exit_path;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 0u;
    StoreU8(ctx, i0 + 1306u, i1);
    i0 = l4;
    i1 = 25705u;
    StoreU16(ctx, i0 + 1304u, i1);
    i0 = l4;
    i1 = 2u;
    StoreU8(ctx, i0 + 1315u, i1);
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = l4;
    i2 = 1304u;
    i1 += i2;
    StoreU32(ctx, i0 + 1344u, i1);
    i0 = l4;
    i1 = 1304u;
    i0 += i1;
    l3 = i0;
    i0 = l4;
    i1 = 1344u;
    i0 += i1;
    l6 = i0;
    i0 = l4;
    i1 = 1336u;
    i0 += i1;
    l5 = i0;
    i0 = l4;
    i1 = 1352u;
    i0 += i1;
    c = i0;
  }
  i0 = l8;
  i1 = 83u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = c;
    i2 = l14;
    i3 = l3;
    i4 = kDataB5E30;
    i5 = l6;
    i6 = l5;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 83u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    c = i0;
    i0 = kDataD015C;
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
    if (i0) {goto block_604;}
    i0 = l4;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 1352u);
    i2 = 28u;
    i1 += i2;
    c = i1;
    StoreU32(ctx, i0 + 1344u, i1);
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1352u;
    i0 += i1;
    l3 = i0;
    i0 = l4;
    i1 = 1344u;
    i0 += i1;
    l6 = i0;
    i0 = l4;
    i1 = 1336u;
    i0 += i1;
    l5 = i0;
    i0 = a;
    i1 = 76u;
    i0 += i1;
    a = i0;
  }
  i0 = l8;
  i1 = 84u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2613u;
    i1 = l3;
    i2 = a;
    i3 = c;
    i4 = kDataB5E30;
    i5 = l6;
    i6 = l5;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 84u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_604;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 1352u);
    c = i0;
    i0 = LoadU32(ctx, i0 + 32u);
    l3 = i0;
    i0 = l3;
    i1 = c;
    i1 = LoadU32(ctx, i1 + 36u);
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
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 85u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2591u;
      i1 = l3;
      i2 = b;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l7 = i0;
      i0 = 85u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      a = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
      if (i0) {
        i0 = HostRet0(ctx);
        a = i0;
        i0 = HostTempRet0(ctx);
        b = i0;
        i0 = c;
        i1 = l3;
        StoreU32(ctx, i0 + 32u, i1);
        goto block_603;
      }
      i0 = c;
      i1 = a;
      i2 = 44u;
      i1 += i2;
      a = i1;
      StoreU32(ctx, i0 + 32u, i1);
      goto block_607;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = 28u;
    i0 += i1;
    a = i0;
  }
  i0 = l8;
  i1 = 86u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2614u;
    i1 = a;
    i2 = b;
    HostOsSchedule(ctx, i0, i1, i2);
    i0 = 86u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_604;}
  }
  block_607:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 1315u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_39;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 1304u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_39;
  }
  block_604:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
  }
  block_603:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 1315u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    b = i0;
    if (i0) {goto block_40;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 1304u);
    b = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_40:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 1300u);
    b = i0;
    i0 = !(i0);
    c = i0;
    if (i0) {goto block_8;}
    i0 = b;
    i1 = b;
    i1 = LoadU32(ctx, i1 + 4u);
    c = i1;
    i2 = 1u;
    i1 -= i2;
    l3 = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = c;
    i1 = 1u;
    i0 = i0 != i1;
    c = i0;
    if (i0) {goto block_8;}
    i0 = b;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 87u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = c;
    i1 = b;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 87u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    c = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = 1u;
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
      i0 = b;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 8u);
      c = i1;
      i2 = 1u;
      i1 -= i2;
      l3 = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 != i1;
      c = i0;
      if (i0) {goto block_8;}
      i0 = b;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      c = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 88u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = b;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 88u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_8;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_39:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 1300u);
    a = i0;
    i0 = !(i0);
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    if (i0) {goto block_630;}
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
    if (i0) {goto block_630;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 89u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = a;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 89u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kDataD015C;
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
      if (i0) {goto block_630;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      b = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 90u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 90u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_630;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_630:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 1360u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_38:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    i2 = l6;
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
    if (i0) {goto block_641;}
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 91u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l10;
    i2 = 0u;
    i3 = kDataFFFFFFFF;
    i4 = kMsgSetonvideofinish;
    i5 = 16u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l7 = i0;
    i0 = 91u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
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
      i0 = c;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l5 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l3 = i0;
        l6 = i0;
        goto block_641;
      }
      i0 = a;
      i0 = LoadU32(ctx, i0 + 16u);
      c = i0;
      i0 = !(i0);
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l8;
      i1 = 92u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kMsgPx;
        i2 = kMsgSmartPtrPath;
        i3 = 734u;
        i4 = kMsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 92u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_37;}
        goto exit_path;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = c;
      i0 = LoadU32(ctx, i0 + 24u);
      c = i0;
      i0 = !(i0);
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l8;
      i1 = 93u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kMsgPx;
        i2 = kMsgSmartPtrPath;
        i3 = 734u;
        i4 = kMsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 93u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_37;}
        goto exit_path;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = 0u;
      StoreU8(ctx, i0 + 1322u, i1);
      i0 = l4;
      i1 = 25705u;
      StoreU16(ctx, i0 + 1320u, i1);
      i0 = l4;
      i1 = 2u;
      StoreU8(ctx, i0 + 1331u, i1);
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l4;
      i2 = 1320u;
      i1 += i2;
      StoreU32(ctx, i0 + 1296u, i1);
      i0 = l4;
      i1 = 1320u;
      i0 += i1;
      l6 = i0;
      i0 = l4;
      i1 = 1296u;
      i0 += i1;
      l5 = i0;
      i0 = l4;
      i1 = 1344u;
      i0 += i1;
      l9 = i0;
      i0 = l4;
      i1 = 1304u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l8;
    i1 = 94u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1959u;
      i1 = l3;
      i2 = l14;
      i3 = l6;
      i4 = kDataB5E30;
      i5 = l5;
      i6 = l9;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 94u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      if (i0) {goto block_673;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1304u);
      l3 = i0;
      i0 = c;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 304u);
      l5 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1352u;
      i0 += i1;
      l6 = i0;
      i0 = l3;
      i1 = 28u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l8;
    i1 = 95u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l5;
      i1 = l6;
      i2 = c;
      i3 = l3;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 95u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 == i1;
      c = i0;
      if (i0) {goto block_673;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1352u);
      c = i0;
      i0 = !(i0);
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      if (i0) {
        i0 = 0u;
        l6 = i0;
        i0 = 0u;
        l3 = i0;
        goto block_677;
      }
      i0 = 0u;
      l3 = i0;
    }
    i0 = l8;
    i1 = 96u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = kData8C564;
      i2 = kDataB8F8C;
      i3 = 0u;
      i0 = CombineComparisonResults(ctx, i0, i1, i2, i3);
      l7 = i0;
      i0 = 96u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l6 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i0 = !(i0);
      c = i0;
      if (i0) {
        i0 = 0u;
        l6 = i0;
        goto block_677;
      }
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1356u);
      c = i0;
      i0 = !(i0);
      l5 = i0;
      if (i0) {goto block_677;}
      i0 = c;
      i1 = c;
      i1 = LoadU32(ctx, i1 + 4u);
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = c;
      l3 = i0;
    }
    block_677:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1356u);
      c = i0;
      i0 = !(i0);
      l5 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      if (i0) {goto block_684;}
      i0 = c;
      i1 = c;
      i1 = LoadU32(ctx, i1 + 4u);
      l5 = i1;
      i2 = 1u;
      i1 -= i2;
      l9 = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 != i1;
      l5 = i0;
      if (i0) {goto block_684;}
      i0 = c;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      l5 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 97u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l5;
      i1 = c;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 97u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto exit_path;}
      i0 = c;
      i1 = c;
      i1 = LoadU32(ctx, i1 + 8u);
      l5 = i1;
      i2 = 1u;
      i1 -= i2;
      l9 = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 != i1;
      l5 = i0;
      if (i0) {goto block_684;}
      i0 = c;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      l5 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 98u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l5;
      i1 = c;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 98u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 == i1;
      c = i0;
      if (i0) {goto exit_path;}
    }
    block_684:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 1331u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 1320u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l6;
      i0 = !(i0);
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = c;
      if (i0) {goto block_692;}
      i0 = l6;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 372u);
      c = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1320u;
      i0 += i1;
      l5 = i0;
    }
    i0 = l8;
    i1 = 99u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l5;
      i2 = l6;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 99u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 == i1;
      c = i0;
      if (i0) {goto block_665;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1320u);
      l5 = i0;
      i0 = !(i0);
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = c;
      if (i0) {goto block_696;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 352u;
      i0 += i1;
      c = i0;
    }
    i0 = l8;
    i1 = 100u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2591u;
      i1 = c;
      i2 = b;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l7 = i0;
      i0 = 100u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 == i1;
      c = i0;
      if (i0) {goto block_672;}
      i0 = l4;
      i1 = 0u;
      StoreU32(ctx, i0 + 348u, i1);
      i0 = l4;
      i1 = 2648u;
      StoreU32(ctx, i0 + 344u, i1);
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l4;
      j1 = LoadU64(ctx, i1 + 344u);
      StoreU64(ctx, i0 + 48u, j1);
      i0 = l4;
      i1 = 48u;
      i0 += i1;
      l6 = i0;
      i0 = l4;
      i1 = 400u;
      i0 += i1;
      c = i0;
    }
    i0 = l8;
    i1 = 101u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2649u;
      i1 = c;
      i2 = l6;
      i3 = a;
      i4 = 0u;
      i5 = 1u;
      i6 = b;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 101u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_671;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 400u;
      i0 += i1;
      c = i0;
      i0 = l4;
      i1 = 464u;
      i0 += i1;
      a = i0;
    }
    i0 = l8;
    i1 = 102u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2650u;
      i1 = a;
      i2 = c;
      i3 = 0u;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l7 = i0;
      i0 = 102u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_669;}
      i0 = l5;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 24u);
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 103u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = l5;
      i2 = c;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 103u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_670;}
      i0 = c;
      i0 = LoadU32(ctx, i0);
      a = i0;
    }
    i0 = a;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l6;
      i1 = a;
      i2 = 1u;
      i1 &= i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      l6 = i0;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        if (i0) {goto block_708;}
        i0 = a;
        i1 = kDataFFFFFFFE;
        i0 &= i1;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = !(i0);
        l6 = i0;
        if (i0) {goto block_708;}
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        l6 = i0;
        i0 = c;
        i1 = 4u;
        i0 += i1;
        a = i0;
      }
      i0 = l8;
      i1 = 104u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l6;
        i1 = a;
        i2 = a;
        i3 = 2u;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 104u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 == i1;
        a = i0;
        if (i0) {goto exit_path;}
      }
      block_708:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = c;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = kDataB5F40;
      StoreU32(ctx, i0 + 420u, i1);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 459u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 448u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l4;
      i1 = 436u;
      i0 += i1;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 440u);
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 435u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 424u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = b;
      i1 = kDataB5F40;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i0 = LoadS8(ctx, i0 + 39u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 28u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = b;
      i1 = 16u;
      i0 += i1;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 20u);
      c = i1;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = b;
      i0 = LoadS8(ctx, i0 + 15u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_696;}
      i0 = b;
      i0 = LoadU32(ctx, i0 + 4u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
    }
    block_696:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1324u);
      a = i0;
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_692;}
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
      if (i0) {goto block_692;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      b = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 105u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 105u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
      if (i0) {goto block_644;}
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
      if (i0) {goto block_692;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      b = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 106u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 106u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_644;}
    }
    block_692:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = !(i0);
      a = i0;
      if (i0) {goto block_6;}
      i0 = l3;
      i1 = l3;
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
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 107u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = l3;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 107u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kDataD015C;
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
        i0 = l3;
        i1 = l3;
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
        i0 = l3;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        a = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l8;
      i1 = 108u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l3;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 108u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kDataD015C;
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
    if (i0) {goto exit_path;}
    block_673:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 1331u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      b = i0;
      if (i0) {goto block_8;}
      goto block_5;
    }
    block_672:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
      goto block_666;
    }
    block_671:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      c = i0;
      goto block_667;
    }
    block_670:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = c;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = !(i0);
      l6 = i0;
      if (i0) {goto block_668;}
      i0 = l5;
      i1 = 1u;
      i0 &= i1;
      l6 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      if (i0) {goto block_735;}
      i0 = l5;
      i1 = kDataFFFFFFFE;
      i0 &= i1;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = !(i0);
      l6 = i0;
      if (i0) {goto block_735;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      l6 = i0;
      i0 = c;
      i1 = 4u;
      i0 += i1;
      l5 = i0;
    }
    i0 = l8;
    i1 = 109u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l6;
      i1 = l5;
      i2 = l5;
      i3 = 2u;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 109u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l6 = i0;
      if (i0) {goto block_734;}
    }
    block_735:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = c;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      goto block_668;
    }
    block_734:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto exit_path;}
    block_669:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      c = i0;
    }
    block_668:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = kDataB5F40;
      StoreU32(ctx, i0 + 420u, i1);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 459u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 448u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l4;
      i1 = 436u;
      i0 += i1;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 440u);
      l6 = i1;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 435u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      c = i0;
      if (i0) {goto block_667;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 424u);
      c = i0;
      DeallocateRawBlock(ctx, i0);
    }
    block_667:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i1 = kDataB5F40;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i0 = LoadS8(ctx, i0 + 39u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 28u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = b;
      i1 = 16u;
      i0 += i1;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 20u);
      l6 = i1;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = b;
      i0 = LoadS8(ctx, i0 + 15u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      c = i0;
      if (i0) {goto block_666;}
      i0 = b;
      i0 = LoadU32(ctx, i0 + 4u);
      b = i0;
      DeallocateRawBlock(ctx, i0);
    }
    block_666:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1324u);
      b = i0;
      i0 = !(i0);
      c = i0;
      if (i0) {goto block_664;}
      i0 = b;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 4u);
      c = i1;
      i2 = 1u;
      i1 -= i2;
      l6 = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 != i1;
      c = i0;
      if (i0) {goto block_664;}
      i0 = b;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      c = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 110u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = b;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 110u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
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
        i0 = b;
        i1 = b;
        i1 = LoadU32(ctx, i1 + 8u);
        c = i1;
        i2 = 1u;
        i1 -= i2;
        l6 = i1;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = c;
        i1 = 1u;
        i0 = i0 != i1;
        c = i0;
        if (i0) {goto block_664;}
        i0 = b;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        c = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l8;
      i1 = 111u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = c;
        i1 = b;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 111u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 != i1;
        b = i0;
        if (i0) {goto block_664;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto exit_path;}
    block_665:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
    }
    block_664:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_8;}
      i0 = l3;
      i1 = l3;
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
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      b = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 112u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = l3;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 112u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kDataD015C;
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
        i0 = l3;
        i1 = l3;
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
        i0 = l3;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        b = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l8;
      i1 = 113u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = b;
        i1 = l3;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 113u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 != i1;
        b = i0;
        if (i0) {goto block_8;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto exit_path;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_644:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_641:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    i2 = l6;
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
    if (i0) {goto block_36;}
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 114u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l10;
    i2 = 0u;
    i3 = kDataFFFFFFFF;
    i4 = kMsgSetonsoundfinish;
    i5 = 16u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l7 = i0;
    i0 = 114u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto exit_path;}
    i0 = c;
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0 + 8u);
      l5 = i0;
      i0 = b;
      i0 = LoadU8(ctx, i0 + 15u);
      l3 = i0;
      l6 = i0;
      goto block_36;
    }
    i0 = l4;
    i1 = 0u;
    StoreU8(ctx, i0 + 1322u, i1);
    i0 = l4;
    i1 = 25705u;
    StoreU16(ctx, i0 + 1320u, i1);
    i0 = l4;
    i1 = 2u;
    StoreU8(ctx, i0 + 1331u, i1);
    i0 = l9;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
  }
  i0 = l3;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    loop_768: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadU8(ctx, i0 + 27u);
        c = i0;
        i1 = 24u;
        i0 <<= (i1 & 31);
        i1 = 24u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l6 = i0;
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 20u);
        i1 = c;
        i2 = l6;
        i0 = i2 ? i0 : i1;
        c = i0;
        i1 = 2u;
        i0 = i0 < i1;
        l10 = i0;
        i0 = c;
        i1 = 2u;
        i2 = l10;
        i0 = i2 ? i0 : i1;
        l5 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        if (i0) {
          i0 = l4;
          i1 = 1320u;
          i0 += i1;
          i1 = l3;
          i2 = 16u;
          i1 += i2;
          l9 = i1;
          i1 = LoadU32(ctx, i1);
          l12 = i1;
          i2 = l9;
          i3 = l6;
          i1 = i3 ? i1 : i2;
          l6 = i1;
          i2 = l5;
          i0 = CompareBytes(ctx, i0, i1, i2);
          l9 = i0;
          i0 = !(i0);
          l11 = i0;
          if (i0) {
            i0 = c;
            i1 = 2u;
            i0 = i0 <= i1;
            c = i0;
            if (i0) {goto block_776;}
            goto block_770;
          }
          i0 = l9;
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          c = i0;
          if (i0) {goto block_770;}
          block_776:;
          i0 = l6;
          i1 = l4;
          i2 = 1320u;
          i1 += i2;
          i2 = l5;
          i0 = CompareBytes(ctx, i0, i1, i2);
          c = i0;
          i0 = !(i0);
          l6 = i0;
          if (i0) {goto block_774;}
          i0 = c;
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          c = i0;
          if (i0) {goto block_773;}
          goto block_771;
        }
        i0 = c;
        i1 = 2u;
        i0 = i0 > i1;
        c = i0;
        if (i0) {goto block_770;}
        block_774:;
        i0 = l10;
        if (i0) {goto block_771;}
        block_773:;
        i0 = l4;
        i1 = 0u;
        StoreU8(ctx, i0 + 1322u, i1);
        i0 = l4;
        i1 = 25705u;
        StoreU16(ctx, i0 + 1320u, i1);
        i0 = l4;
        i1 = 2u;
        StoreU8(ctx, i0 + 1331u, i1);
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = l4;
        i2 = 1320u;
        i1 += i2;
        StoreU32(ctx, i0 + 1352u, i1);
        i0 = l4;
        i1 = 1320u;
        i0 += i1;
        l3 = i0;
        i0 = l4;
        i1 = 1352u;
        i0 += i1;
        l6 = i0;
        i0 = l4;
        i1 = 1296u;
        i0 += i1;
        l5 = i0;
        i0 = l4;
        i1 = 1304u;
        i0 += i1;
        c = i0;
      }
      i0 = l8;
      i1 = 115u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1959u;
        i1 = c;
        i2 = l14;
        i3 = l3;
        i4 = kDataB5E30;
        i5 = l6;
        i6 = l5;
        HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
        i0 = 115u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        c = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = 1u;
        l5 = i0;
        i0 = c;
        i1 = 1u;
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
          i1 = 14348u;
          i0 += i1;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          i0 = !(i0);
          if (i0) {goto block_783;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 1304u);
          l6 = i0;
          i0 = LoadU32(ctx, i0 + 32u);
          l9 = i0;
          i0 = l6;
          i0 = LoadU8(ctx, i0 + 39u);
          c = i0;
          i1 = 24u;
          i0 <<= (i1 & 31);
          i1 = 24u;
          i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
          i1 = 0u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          l10 = i0;
          i0 = l9;
          i1 = c;
          i2 = l10;
          i0 = i2 ? i0 : i1;
          c = i0;
          i0 = l6;
          i0 = LoadU32(ctx, i0 + 28u);
          i1 = l6;
          i2 = 28u;
          i1 += i2;
          i2 = l10;
          i0 = i2 ? i0 : i1;
          l10 = i0;
          loop_784: 
            i0 = l3;
            i0 = LoadU8(ctx, i0 + 27u);
            l6 = i0;
            i1 = 24u;
            i0 <<= (i1 & 31);
            i1 = 24u;
            i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
            i1 = 0u;
            i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
            l12 = i0;
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 20u);
            i1 = l6;
            i2 = l12;
            i0 = i2 ? i0 : i1;
            l6 = i0;
            i1 = c;
            i0 = i0 < i1;
            l11 = i0;
            i0 = l6;
            i1 = c;
            i2 = l11;
            i0 = i2 ? i0 : i1;
            l9 = i0;
            if (i0) {
              i0 = l10;
              i1 = l3;
              i2 = 16u;
              i1 += i2;
              l13 = i1;
              i1 = LoadU32(ctx, i1);
              i2 = l13;
              i3 = l12;
              i1 = i3 ? i1 : i2;
              l12 = i1;
              i2 = l9;
              i0 = CompareBytes(ctx, i0, i1, i2);
              l13 = i0;
              i0 = !(i0);
              if (i0) {
                i0 = c;
                i1 = l6;
                i0 = i0 >= i1;
                l6 = i0;
                if (i0) {goto block_789;}
                goto block_785;
              }
              i0 = l13;
              i1 = kDataFFFFFFFF;
              i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
              l6 = i0;
              if (i0) {goto block_785;}
              block_789:;
              i0 = l12;
              i1 = l10;
              i2 = l9;
              i0 = CompareBytes(ctx, i0, i1, i2);
              l6 = i0;
              i0 = !(i0);
              l9 = i0;
              if (i0) {goto block_787;}
              i0 = l6;
              i1 = kDataFFFFFFFF;
              i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
              l6 = i0;
              if (i0) {goto block_786;}
              i0 = 0u;
              l5 = i0;
              goto block_783;
            }
            i0 = c;
            i1 = l6;
            i0 = i0 < i1;
            l6 = i0;
            if (i0) {goto block_785;}
            block_787:;
            i0 = l11;
            if (i0) {goto block_786;}
            i0 = 0u;
            l5 = i0;
            goto block_783;
            block_786:;
            i0 = l3;
            i1 = 4u;
            i0 += i1;
            l3 = i0;
            block_785:;
            i0 = l3;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            if (i0) {goto loop_784;}
          block_783:;
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 1331u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 1320u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l5;
          if (i0) {goto block_6;}
          i0 = a;
          i0 = LoadU32(ctx, i0 + 14336u);
          l3 = i0;
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
            i0 = kDataD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l8;
          i1 = 116u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kMsgPx;
            i2 = kMsgSmartPtrPath;
            i3 = 734u;
            i4 = kMsgOperator;
            HostFailAssertion(ctx, i0, i1, i2, i3, i4);
            i0 = 116u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kDataD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kDataD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_37;}
            goto exit_path;
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i1 = 0u;
          StoreU8(ctx, i0 + 1322u, i1);
          i0 = l4;
          i1 = 25705u;
          StoreU16(ctx, i0 + 1320u, i1);
          i0 = l4;
          i1 = 2u;
          StoreU8(ctx, i0 + 1331u, i1);
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = l4;
          i2 = 1320u;
          i1 += i2;
          StoreU32(ctx, i0 + 1352u, i1);
          i0 = l4;
          i1 = 1320u;
          i0 += i1;
          l6 = i0;
          i0 = l4;
          i1 = 1352u;
          i0 += i1;
          l5 = i0;
          i0 = l4;
          i1 = 1296u;
          i0 += i1;
          l9 = i0;
          i0 = l4;
          i1 = 1304u;
          i0 += i1;
          c = i0;
        }
        i0 = l8;
        i1 = 117u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1959u;
          i1 = c;
          i2 = l14;
          i3 = l6;
          i4 = kDataB5E30;
          i5 = l5;
          i6 = l9;
          HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
          i0 = 117u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          c = i0;
          i0 = kDataD015C;
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
          if (i0) {goto block_806;}
          i0 = l4;
          i1 = l4;
          i1 = LoadU32(ctx, i1 + 1304u);
          i2 = 28u;
          i1 += i2;
          c = i1;
          StoreU32(ctx, i0 + 1352u, i1);
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = 1304u;
          i0 += i1;
          l6 = i0;
          i0 = l4;
          i1 = 1352u;
          i0 += i1;
          l9 = i0;
          i0 = l4;
          i1 = 1296u;
          i0 += i1;
          l11 = i0;
          i0 = a;
          i1 = 14344u;
          i0 += i1;
          l5 = i0;
        }
        i0 = l8;
        i1 = 118u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2609u;
          i1 = l6;
          i2 = l5;
          i3 = c;
          i4 = kDataB5E30;
          i5 = l9;
          i6 = l11;
          HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
          i0 = 118u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          c = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = c;
          i1 = 1u;
          i0 = i0 == i1;
          c = i0;
          if (i0) {goto block_806;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 1304u);
          i0 = LoadU32(ctx, i0 + 28u);
          l5 = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = 80u;
          i0 += i1;
          c = i0;
        }
        i0 = l8;
        i1 = 119u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2591u;
          i1 = c;
          i2 = b;
          i0 = HostOsQuery(ctx, i0, i1, i2);
          l7 = i0;
          i0 = 119u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l7;
          b = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          c = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = c;
          i1 = 1u;
          i0 = i0 == i1;
          c = i0;
          if (i0) {goto block_806;}
          i0 = l4;
          i1 = 0u;
          StoreU32(ctx, i0 + 76u, i1);
          i0 = l4;
          i1 = 2648u;
          StoreU32(ctx, i0 + 72u, i1);
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = l4;
          j1 = LoadU64(ctx, i1 + 72u);
          StoreU64(ctx, i0 + 64u, j1);
          i0 = l4;
          i1 = kDataFFFFFFC0;
          i0 -= i1;
          l6 = i0;
          i0 = l4;
          i1 = 128u;
          i0 += i1;
          c = i0;
        }
        i0 = l8;
        i1 = 120u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2649u;
          i1 = c;
          i2 = l6;
          i3 = a;
          i4 = 0u;
          i5 = 1u;
          i6 = b;
          HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
          i0 = 120u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 == i1;
          a = i0;
          if (i0) {goto block_805;}
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = 128u;
          i0 += i1;
          c = i0;
          i0 = l4;
          i1 = 192u;
          i0 += i1;
          a = i0;
        }
        i0 = l8;
        i1 = 121u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2650u;
          i1 = a;
          i2 = c;
          i3 = 0u;
          i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
          l7 = i0;
          i0 = 121u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l7;
          c = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 == i1;
          a = i0;
          if (i0) {goto block_803;}
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l8;
        i1 = 122u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2666u;
          i1 = l3;
          i2 = l5;
          i3 = c;
          HostSyncSlot(ctx, i0, i1, i2, i3);
          i0 = 122u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 == i1;
          a = i0;
          if (i0) {goto block_804;}
          i0 = c;
          i0 = LoadU32(ctx, i0);
          a = i0;
        }
        i0 = a;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = l3;
          i1 = a;
          i2 = 1u;
          i1 &= i2;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          l3 = i0;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            if (i0) {goto block_820;}
            i0 = a;
            i1 = kDataFFFFFFFE;
            i0 &= i1;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = !(i0);
            l3 = i0;
            if (i0) {goto block_820;}
            i0 = kDataD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            l3 = i0;
            i0 = c;
            i1 = 4u;
            i0 += i1;
            a = i0;
          }
          i0 = l8;
          i1 = 123u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l3;
            i1 = a;
            i2 = a;
            i3 = 2u;
            HostSyncSlot(ctx, i0, i1, i2, i3);
            i0 = 123u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kDataD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kDataD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 == i1;
            a = i0;
            if (i0) {goto block_799;}
          }
          block_820:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = c;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i1 = kDataB5F40;
          StoreU32(ctx, i0 + 148u, i1);
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 187u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 176u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l4;
          i1 = 164u;
          i0 += i1;
          i1 = l4;
          i1 = LoadU32(ctx, i1 + 168u);
          ReleaseNodeTree(ctx, i0, i1);
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 163u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 152u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = b;
          i1 = kDataB5F40;
          StoreU32(ctx, i0, i1);
          i0 = b;
          i0 = LoadS8(ctx, i0 + 39u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = b;
            i0 = LoadU32(ctx, i0 + 28u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = b;
          i1 = 16u;
          i0 += i1;
          i1 = b;
          i1 = LoadU32(ctx, i1 + 20u);
          ReleaseNodeTree(ctx, i0, i1);
          i0 = b;
          i0 = LoadS8(ctx, i0 + 15u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = b;
            i0 = LoadU32(ctx, i0 + 4u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 1331u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_6;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 1320u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
          goto block_6;
        }
        block_806:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          a = i0;
          i0 = HostTempRet0(ctx);
          b = i0;
          goto block_800;
        }
        block_805:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          a = i0;
          i0 = HostTempRet0(ctx);
          c = i0;
          goto block_801;
        }
        block_804:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          a = i0;
          i0 = HostTempRet0(ctx);
          i0 = c;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          i0 = !(i0);
          l6 = i0;
          if (i0) {goto block_802;}
          i0 = l3;
          i1 = 1u;
          i0 &= i1;
          l6 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l6;
          if (i0) {goto block_834;}
          i0 = l3;
          i1 = kDataFFFFFFFE;
          i0 &= i1;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          i0 = !(i0);
          l6 = i0;
          if (i0) {goto block_834;}
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          l6 = i0;
          i0 = c;
          i1 = 4u;
          i0 += i1;
          l3 = i0;
        }
        i0 = l8;
        i1 = 124u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l6;
          i1 = l3;
          i2 = l3;
          i3 = 2u;
          HostSyncSlot(ctx, i0, i1, i2, i3);
          i0 = 124u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 == i1;
          l3 = i0;
          if (i0) {goto block_833;}
        }
        block_834:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = c;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          goto block_802;
        }
        block_833:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto exit_path;}
        block_803:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          a = i0;
          i0 = HostTempRet0(ctx);
          c = i0;
        }
        block_802:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i1 = kDataB5F40;
          StoreU32(ctx, i0 + 148u, i1);
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 187u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 176u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l4;
          i1 = 164u;
          i0 += i1;
          i1 = l4;
          i1 = LoadU32(ctx, i1 + 168u);
          l3 = i1;
          ReleaseNodeTree(ctx, i0, i1);
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 163u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          c = i0;
          if (i0) {goto block_801;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 152u);
          c = i0;
          DeallocateRawBlock(ctx, i0);
        }
        block_801:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = b;
          i1 = kDataB5F40;
          StoreU32(ctx, i0, i1);
          i0 = b;
          i0 = LoadS8(ctx, i0 + 39u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = b;
            i0 = LoadU32(ctx, i0 + 28u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = b;
          i1 = 16u;
          i0 += i1;
          i1 = b;
          i1 = LoadU32(ctx, i1 + 20u);
          l3 = i1;
          ReleaseNodeTree(ctx, i0, i1);
          i0 = b;
          i0 = LoadS8(ctx, i0 + 15u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          c = i0;
          if (i0) {goto block_800;}
          i0 = b;
          i0 = LoadU32(ctx, i0 + 4u);
          b = i0;
          DeallocateRawBlock(ctx, i0);
        }
        block_800:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 1331u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          b = i0;
          if (i0) {goto block_8;}
          goto block_5;
        }
        block_799:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto exit_path;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        a = i0;
        i0 = HostTempRet0(ctx);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 1331u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        b = i0;
        if (i0) {goto block_8;}
        goto block_5;
      }
      block_771:;
      i0 = l3;
      i1 = l3;
      i2 = 4u;
      i1 += i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      l3 = i0;
      block_770:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        if (i0) {goto loop_768;}
      }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 14320u);
    l3 = i0;
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
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 125u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kMsgPx;
      i2 = kMsgSmartPtrPath;
      i3 = 734u;
      i4 = kMsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 125u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_37;}
      goto exit_path;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 14328u);
    l5 = i0;
    i0 = l4;
    i1 = 216u;
    i0 += i1;
    c = i0;
  }
  i0 = l8;
  i1 = 126u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = c;
    i1 = b;
    i0 = CopyScriptBlock(ctx, i0, i1);
    l7 = i0;
    i0 = 126u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 0u;
    StoreU32(ctx, i0 + 212u, i1);
    i0 = l4;
    i1 = 2648u;
    StoreU32(ctx, i0 + 208u, i1);
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = l4;
    j1 = LoadU64(ctx, i1 + 208u);
    StoreU64(ctx, i0 + 56u, j1);
    i0 = l4;
    i1 = 56u;
    i0 += i1;
    l6 = i0;
    i0 = l4;
    i1 = 264u;
    i0 += i1;
    c = i0;
  }
  i0 = l8;
  i1 = 127u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2649u;
    i1 = c;
    i2 = l6;
    i3 = a;
    i4 = 0u;
    i5 = 1u;
    i6 = b;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 127u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kDataD015C;
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
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 264u;
      i0 += i1;
      c = i0;
      i0 = l4;
      i1 = 328u;
      i0 += i1;
      a = i0;
    }
    i0 = l8;
    i1 = 128u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2650u;
      i1 = a;
      i2 = c;
      i3 = 0u;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l7 = i0;
      i0 = 128u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_859;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 129u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2666u;
      i1 = l3;
      i2 = l5;
      i3 = c;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 129u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_860;}
      i0 = c;
      i0 = LoadU32(ctx, i0);
      a = i0;
    }
    i0 = a;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l3;
      i1 = a;
      i2 = 1u;
      i1 &= i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      l3 = i0;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        if (i0) {goto block_871;}
        i0 = a;
        i1 = kDataFFFFFFFE;
        i0 &= i1;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = !(i0);
        l3 = i0;
        if (i0) {goto block_871;}
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        l3 = i0;
        i0 = c;
        i1 = 4u;
        i0 += i1;
        a = i0;
      }
      i0 = l8;
      i1 = 130u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l3;
        i1 = a;
        i2 = a;
        i3 = 2u;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 130u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 == i1;
        a = i0;
        if (i0) {goto exit_path;}
      }
      block_871:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = c;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = kDataB5F40;
      StoreU32(ctx, i0 + 284u, i1);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 323u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 312u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l4;
      i1 = 300u;
      i0 += i1;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 304u);
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 299u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 288u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = b;
      i1 = kDataB5F40;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i0 = LoadS8(ctx, i0 + 39u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 28u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = b;
      i1 = 16u;
      i0 += i1;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 20u);
      ReleaseNodeTree(ctx, i0, i1);
      i0 = b;
      i0 = LoadS8(ctx, i0 + 15u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_6;}
      i0 = b;
      i0 = LoadU32(ctx, i0 + 4u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_6;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    c = i0;
    goto block_857;
  }
  block_860:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = c;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = !(i0);
    l6 = i0;
    if (i0) {goto block_858;}
    i0 = l3;
    i1 = 1u;
    i0 &= i1;
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    if (i0) {goto block_883;}
    i0 = l3;
    i1 = kDataFFFFFFFE;
    i0 &= i1;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = !(i0);
    l6 = i0;
    if (i0) {goto block_883;}
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    l6 = i0;
    i0 = c;
    i1 = 4u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l8;
  i1 = 131u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = l3;
    i2 = l3;
    i3 = 2u;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 131u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    l3 = i0;
    if (i0) {goto block_882;}
  }
  block_883:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    goto block_858;
  }
  block_882:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_859:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    c = i0;
  }
  block_858:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = kDataB5F40;
    StoreU32(ctx, i0 + 284u, i1);
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 323u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 312u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l4;
    i1 = 300u;
    i0 += i1;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 304u);
    l3 = i1;
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 299u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    c = i0;
    if (i0) {goto block_857;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 288u);
    c = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_857:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i1 = kDataB5F40;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i0 = LoadS8(ctx, i0 + 39u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0 + 28u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = b;
    i1 = 16u;
    i0 += i1;
    i1 = b;
    i1 = LoadU32(ctx, i1 + 20u);
    l3 = i1;
    ReleaseNodeTree(ctx, i0, i1);
    i0 = b;
    i0 = LoadS8(ctx, i0 + 15u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    c = i0;
    if (i0) {goto block_8;}
    i0 = b;
    i0 = LoadU32(ctx, i0 + 4u);
    b = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_8;
  }
  block_37:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    TrapUnreachable();
  }
  block_36:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    i2 = l6;
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
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 132u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l10;
    i2 = 0u;
    i3 = kDataFFFFFFFF;
    i4 = kMsgSetonwindowbutton;
    i5 = 17u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l7 = i0;
    i0 = 132u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
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
      i0 = c;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l5 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l3 = i0;
        l6 = i0;
        goto block_7;
      }
      i0 = l4;
      i1 = 6u;
      StoreU8(ctx, i0 + 1331u, i1);
      i0 = l4;
      i1 = 0u;
      StoreU8(ctx, i0 + 1326u, i1);
      i0 = l4;
      i1 = kDataB53FF;
      i1 = LoadU16(ctx, i1);
      StoreU16(ctx, i0 + 1324u, i1);
      i0 = l4;
      i1 = kMsgButton;
      i1 = LoadU32(ctx, i1);
      c = i1;
      StoreU32(ctx, i0 + 1320u, i1);
      i0 = l9;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
    }
    i0 = l3;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      loop_905: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i0 = LoadU8(ctx, i0 + 27u);
          c = i0;
          i1 = 24u;
          i0 <<= (i1 & 31);
          i1 = 24u;
          i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
          i1 = 0u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          l6 = i0;
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 20u);
          i1 = c;
          i2 = l6;
          i0 = i2 ? i0 : i1;
          c = i0;
          i1 = 6u;
          i0 = i0 < i1;
          l12 = i0;
          i0 = c;
          i1 = 6u;
          i2 = l12;
          i0 = i2 ? i0 : i1;
          l5 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l5;
          if (i0) {
            i0 = l4;
            i1 = 1320u;
            i0 += i1;
            i1 = l3;
            i2 = 16u;
            i1 += i2;
            l11 = i1;
            i1 = LoadU32(ctx, i1);
            i2 = l11;
            i3 = l6;
            i1 = i3 ? i1 : i2;
            l6 = i1;
            i2 = l5;
            i0 = CompareBytes(ctx, i0, i1, i2);
            l11 = i0;
            i0 = !(i0);
            if (i0) {
              i0 = c;
              i1 = 6u;
              i0 = i0 <= i1;
              c = i0;
              if (i0) {goto block_913;}
              goto block_907;
            }
            i0 = l11;
            i1 = kDataFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            c = i0;
            if (i0) {goto block_907;}
            block_913:;
            i0 = l6;
            i1 = l4;
            i2 = 1320u;
            i1 += i2;
            i2 = l5;
            i0 = CompareBytes(ctx, i0, i1, i2);
            c = i0;
            i0 = !(i0);
            l6 = i0;
            if (i0) {goto block_911;}
            i0 = c;
            i1 = kDataFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            c = i0;
            if (i0) {goto block_910;}
            goto block_908;
          }
          i0 = c;
          i1 = 6u;
          i0 = i0 > i1;
          c = i0;
          if (i0) {goto block_907;}
          block_911:;
          i0 = l12;
          if (i0) {goto block_908;}
          block_910:;
          i0 = l4;
          i1 = 6u;
          StoreU8(ctx, i0 + 1331u, i1);
          i0 = l4;
          i1 = kMsgButton;
          i1 = LoadU32(ctx, i1);
          StoreU32(ctx, i0 + 1320u, i1);
          i0 = l4;
          i1 = kDataB53FF;
          i1 = LoadU16(ctx, i1);
          StoreU16(ctx, i0 + 1324u, i1);
          i0 = l4;
          i1 = 0u;
          StoreU8(ctx, i0 + 1326u, i1);
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = l4;
          i2 = 1320u;
          i1 += i2;
          StoreU32(ctx, i0 + 1352u, i1);
          i0 = l4;
          i1 = 1320u;
          i0 += i1;
          l3 = i0;
          i0 = l4;
          i1 = 1352u;
          i0 += i1;
          l6 = i0;
          i0 = l4;
          i1 = 1296u;
          i0 += i1;
          l5 = i0;
          i0 = l4;
          i1 = 1304u;
          i0 += i1;
          c = i0;
        }
        i0 = l8;
        i1 = 133u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1959u;
          i1 = c;
          i2 = l14;
          i3 = l3;
          i4 = kDataB5E30;
          i5 = l6;
          i6 = l5;
          HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
          i0 = 133u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          c = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = c;
          i1 = 1u;
          i0 = i0 != i1;
          c = i0;
          if (i0) {goto block_900;}
          goto block_896;
        }
        block_908:;
        i0 = l3;
        i1 = l3;
        i2 = 4u;
        i1 += i2;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        l3 = i0;
        block_907:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          if (i0) {goto loop_905;}
        }
    }
    i0 = c;
    i1 = a;
    i2 = 14220u;
    i1 += i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    c = i0;
    i0 = l8;
    i1 = 134u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l10;
      i0 = SetStringFromSource(ctx, i0, i1);
      l7 = i0;
      i0 = 134u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
    } else {
      i0 = c;
    }
    i1 = a;
    i2 = 14216u;
    i1 += i2;
    i2 = b;
    i1 = i1 != i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    c = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 16u);
        l3 = i0;
        i0 = a;
        i1 = 14232u;
        i0 += i1;
        c = i0;
      }
      i0 = l8;
      i1 = 135u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = c;
        i1 = l3;
        i2 = l9;
        AssignTaggedTreeRange(ctx, i0, i1, i2);
        i0 = 135u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i1 = 28u;
      i0 += i1;
      l3 = i0;
      i0 = a;
      i1 = 14244u;
      i0 += i1;
      c = i0;
    }
    i0 = l8;
    i1 = 136u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l3;
      i0 = SetStringFromSource(ctx, i0, i1);
      l7 = i0;
      i0 = 136u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = 14256u;
      i0 += i1;
      a = i0;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 40u);
      StoreU32(ctx, i0, i1);
      goto block_6;
    }
    block_900:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1304u);
      l3 = i0;
      i1 = 28u;
      i0 += i1;
      c = i0;
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 39u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = c;
        i0 = LoadU32(ctx, i0);
      } else {
        i0 = c;
      }
      i0 = SkipWhitespace(ctx, i0);
      c = i0;
      i0 = a;
      i1 = 14208u;
      i0 += i1;
      l5 = i0;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      if (i0) {
        i0 = a;
        i1 = 14208u;
        i0 += i1;
        l5 = i0;
        loop_929: 
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 16u);
          l6 = i0;
          i1 = c;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          if (i0) {
            i0 = l3;
            i0 = LoadU32(ctx, i0);
            l6 = i0;
            if (i0) {goto block_930;}
            i0 = l3;
            l5 = i0;
            goto block_927;
          }
          i0 = c;
          i1 = l6;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {goto block_927;}
          i0 = l3;
          i1 = 4u;
          i0 += i1;
          l5 = i0;
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 4u);
          l6 = i0;
          i0 = !(i0);
          if (i0) {goto block_927;}
          i0 = l5;
          l3 = i0;
          block_930:;
          i0 = l3;
          l5 = i0;
          i0 = l6;
          l3 = i0;
          goto loop_929;
        TrapUnreachable();
      }
      i0 = l5;
      l3 = i0;
      block_927:;
      i0 = l5;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = !(i0);
      l11 = i0;
    }
    i0 = l11;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l8;
      i1 = 137u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1032u;
        i1 = 64u;
        i0 = HostFoldLayoutOffset(ctx, i0, i1);
        l7 = i0;
        i0 = 137u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l7;
        l6 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l14 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l14;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_896;}
        i0 = l6;
        j1 = 0ull;
        StoreU64(ctx, i0 + 24u, j1);
        i0 = l6;
        i1 = kDataB5F40;
        StoreU32(ctx, i0 + 20u, i1);
        i0 = l6;
        i1 = c;
        StoreU32(ctx, i0 + 16u, i1);
        i0 = l6;
        j1 = 0ull;
        StoreU64(ctx, i0 + 40u, j1);
        i0 = l6;
        j1 = 0ull;
        StoreU64(ctx, i0 + 48u, j1);
        i0 = l6;
        i1 = l3;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l6;
        j1 = 0ull;
        StoreU64(ctx, i0, j1);
        i0 = l6;
        i1 = 0u;
        StoreU32(ctx, i0 + 32u, i1);
        i0 = l6;
        i1 = l6;
        i2 = 40u;
        i1 += i2;
        StoreU32(ctx, i0 + 36u, i1);
        i0 = l6;
        j1 = 0ull;
        StoreU64(ctx, i0 + 56u, j1);
        i0 = l5;
        i1 = l6;
        StoreU32(ctx, i0, i1);
        i0 = l6;
        l3 = i0;
        i0 = a;
        i0 = LoadU32(ctx, i0 + 14204u);
        i0 = LoadU32(ctx, i0);
        c = i0;
        if (i0) {
          i0 = a;
          i1 = c;
          StoreU32(ctx, i0 + 14204u, i1);
          i0 = l5;
          i0 = LoadU32(ctx, i0);
        } else {
          i0 = l3;
        }
        c = i0;
        i0 = a;
        i1 = 14208u;
        i0 += i1;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        i1 = c;
        UnlinkListNode(ctx, i0, i1);
        i0 = a;
        i1 = 14212u;
        i0 += i1;
        a = i0;
        i0 = LoadU32(ctx, i0);
        i1 = 1u;
        i0 += i1;
        c = i0;
        i0 = a;
        i1 = c;
        StoreU32(ctx, i0, i1);
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 24u;
      i0 += i1;
      a = i0;
    }
    i0 = l8;
    i1 = 138u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 870u;
      i1 = a;
      i2 = l10;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l7 = i0;
      i0 = 138u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      a = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_896;}
      i0 = b;
      i1 = l6;
      i2 = 20u;
      i1 += i2;
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
        i0 = b;
        i0 = LoadU32(ctx, i0 + 16u);
        a = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l6;
        i1 = 36u;
        i0 += i1;
        c = i0;
      }
      i0 = l8;
      i1 = 139u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 2619u;
        i1 = c;
        i2 = a;
        i3 = l9;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 139u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 == i1;
        a = i0;
        if (i0) {goto block_896;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 28u;
      i0 += i1;
      c = i0;
      i0 = l6;
      i1 = 48u;
      i0 += i1;
      a = i0;
    }
    i0 = l8;
    i1 = 140u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 870u;
      i1 = a;
      i2 = c;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l7 = i0;
      i0 = 140u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      a = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_896;}
      i0 = l6;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 40u);
      StoreU32(ctx, i0 + 60u, i1);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 1331u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_6;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 1320u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_6;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_896:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 1331u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    b = i0;
    if (i0) {goto block_8;}
    goto block_5;
  }
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    i2 = l6;
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
    if (i0) {goto block_951;}
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 141u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l10;
    i2 = 0u;
    i3 = kDataFFFFFFFF;
    i4 = kMsgSetonautomodein;
    i5 = 15u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l7 = i0;
    i0 = 141u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
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
      i0 = c;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l5 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l3 = i0;
        l6 = i0;
        goto block_951;
      }
      i0 = a;
      i1 = 13768u;
      i0 += i1;
      c = i0;
    }
    i0 = l8;
    i1 = 142u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l10;
      i0 = SetStringFromSource(ctx, i0, i1);
      l7 = i0;
      i0 = 142u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
    } else {
      i0 = c;
    }
    i1 = a;
    i2 = 13764u;
    i1 += i2;
    i2 = b;
    i1 = i1 != i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    c = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 16u);
        l3 = i0;
        i0 = a;
        i1 = 13780u;
        i0 += i1;
        c = i0;
      }
      i0 = l8;
      i1 = 143u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = c;
        i1 = l3;
        i2 = l9;
        AssignTaggedTreeRange(ctx, i0, i1, i2);
        i0 = 143u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i1 = 28u;
      i0 += i1;
      l3 = i0;
      i0 = a;
      i1 = 13792u;
      i0 += i1;
      c = i0;
    }
    i0 = l8;
    i1 = 144u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l3;
      i0 = SetStringFromSource(ctx, i0, i1);
      l7 = i0;
      i0 = 144u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = 13804u;
      i0 += i1;
      a = i0;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 40u);
      StoreU32(ctx, i0, i1);
      goto block_6;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_951:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    i2 = l6;
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
    if (i0) {goto block_967;}
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 145u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l10;
    i2 = 0u;
    i3 = kDataFFFFFFFF;
    i4 = kMsgSetonautomodeout;
    i5 = 16u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l7 = i0;
    i0 = 145u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
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
      i0 = c;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l5 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l3 = i0;
        l6 = i0;
        goto block_967;
      }
      i0 = a;
      i1 = 13812u;
      i0 += i1;
      c = i0;
    }
    i0 = l8;
    i1 = 146u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l10;
      i0 = SetStringFromSource(ctx, i0, i1);
      l7 = i0;
      i0 = 146u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
    } else {
      i0 = c;
    }
    i1 = a;
    i2 = 13808u;
    i1 += i2;
    i2 = b;
    i1 = i1 != i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    c = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 16u);
        l3 = i0;
        i0 = a;
        i1 = 13824u;
        i0 += i1;
        c = i0;
      }
      i0 = l8;
      i1 = 147u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = c;
        i1 = l3;
        i2 = l9;
        AssignTaggedTreeRange(ctx, i0, i1, i2);
        i0 = 147u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i1 = 28u;
      i0 += i1;
      l3 = i0;
      i0 = a;
      i1 = 13836u;
      i0 += i1;
      c = i0;
    }
    i0 = l8;
    i1 = 148u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l3;
      i0 = SetStringFromSource(ctx, i0, i1);
      l7 = i0;
      i0 = 148u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = 13848u;
      i0 += i1;
      a = i0;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 40u);
      StoreU32(ctx, i0, i1);
      goto block_6;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_967:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    i2 = l6;
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
    if (i0) {goto block_983;}
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 149u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l10;
    i2 = 0u;
    i3 = kDataFFFFFFFF;
    i4 = kMsgSetoncommandskipin;
    i5 = 18u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l7 = i0;
    i0 = 149u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
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
      i0 = c;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l5 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l3 = i0;
        l6 = i0;
        goto block_983;
      }
      i0 = a;
      i1 = 13856u;
      i0 += i1;
      c = i0;
    }
    i0 = l8;
    i1 = 150u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l10;
      i0 = SetStringFromSource(ctx, i0, i1);
      l7 = i0;
      i0 = 150u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
    } else {
      i0 = c;
    }
    i1 = a;
    i2 = 13852u;
    i1 += i2;
    i2 = b;
    i1 = i1 != i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    c = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 16u);
        l3 = i0;
        i0 = a;
        i1 = 13868u;
        i0 += i1;
        c = i0;
      }
      i0 = l8;
      i1 = 151u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = c;
        i1 = l3;
        i2 = l9;
        AssignTaggedTreeRange(ctx, i0, i1, i2);
        i0 = 151u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i1 = 28u;
      i0 += i1;
      l3 = i0;
      i0 = a;
      i1 = 13880u;
      i0 += i1;
      c = i0;
    }
    i0 = l8;
    i1 = 152u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l3;
      i0 = SetStringFromSource(ctx, i0, i1);
      l7 = i0;
      i0 = 152u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = 13892u;
      i0 += i1;
      a = i0;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 40u);
      StoreU32(ctx, i0, i1);
      goto block_6;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_983:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    i2 = l6;
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
    if (i0) {goto block_999;}
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 153u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l10;
    i2 = 0u;
    i3 = kDataFFFFFFFF;
    i4 = kMsgSetoncommandskipout;
    i5 = 19u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l7 = i0;
    i0 = 153u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
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
      i0 = c;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l5 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l3 = i0;
        l6 = i0;
        goto block_999;
      }
      i0 = a;
      i1 = 13900u;
      i0 += i1;
      c = i0;
    }
    i0 = l8;
    i1 = 154u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l10;
      i0 = SetStringFromSource(ctx, i0, i1);
      l7 = i0;
      i0 = 154u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
    } else {
      i0 = c;
    }
    i1 = a;
    i2 = 13896u;
    i1 += i2;
    i2 = b;
    i1 = i1 != i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    c = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 16u);
        l3 = i0;
        i0 = a;
        i1 = 13912u;
        i0 += i1;
        c = i0;
      }
      i0 = l8;
      i1 = 155u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = c;
        i1 = l3;
        i2 = l9;
        AssignTaggedTreeRange(ctx, i0, i1, i2);
        i0 = 155u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i1 = 28u;
      i0 += i1;
      l3 = i0;
      i0 = a;
      i1 = 13924u;
      i0 += i1;
      c = i0;
    }
    i0 = l8;
    i1 = 156u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l3;
      i0 = SetStringFromSource(ctx, i0, i1);
      l7 = i0;
      i0 = 156u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = 13936u;
      i0 += i1;
      a = i0;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 40u);
      StoreU32(ctx, i0, i1);
      goto block_6;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_999:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    i2 = l6;
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
    if (i0) {goto block_1015;}
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 157u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l10;
    i2 = 0u;
    i3 = kDataFFFFFFFF;
    i4 = kMsgSetoncontrolskipin;
    i5 = 18u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l7 = i0;
    i0 = 157u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
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
      i0 = c;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l5 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l3 = i0;
        l6 = i0;
        goto block_1015;
      }
      i0 = a;
      i1 = 13944u;
      i0 += i1;
      c = i0;
    }
    i0 = l8;
    i1 = 158u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l10;
      i0 = SetStringFromSource(ctx, i0, i1);
      l7 = i0;
      i0 = 158u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
    } else {
      i0 = c;
    }
    i1 = a;
    i2 = 13940u;
    i1 += i2;
    i2 = b;
    i1 = i1 != i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    c = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 16u);
        l3 = i0;
        i0 = a;
        i1 = 13956u;
        i0 += i1;
        c = i0;
      }
      i0 = l8;
      i1 = 159u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = c;
        i1 = l3;
        i2 = l9;
        AssignTaggedTreeRange(ctx, i0, i1, i2);
        i0 = 159u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i1 = 28u;
      i0 += i1;
      l3 = i0;
      i0 = a;
      i1 = 13968u;
      i0 += i1;
      c = i0;
    }
    i0 = l8;
    i1 = 160u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l3;
      i0 = SetStringFromSource(ctx, i0, i1);
      l7 = i0;
      i0 = 160u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = 13980u;
      i0 += i1;
      a = i0;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 40u);
      StoreU32(ctx, i0, i1);
      goto block_6;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_1015:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    i2 = l6;
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
    if (i0) {goto block_1031;}
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 161u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l10;
    i2 = 0u;
    i3 = kDataFFFFFFFF;
    i4 = kMsgSetoncontrolskipout;
    i5 = 19u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l7 = i0;
    i0 = 161u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
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
      i0 = c;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l5 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l3 = i0;
        l6 = i0;
        goto block_1031;
      }
      i0 = a;
      i1 = 13988u;
      i0 += i1;
      c = i0;
    }
    i0 = l8;
    i1 = 162u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l10;
      i0 = SetStringFromSource(ctx, i0, i1);
      l7 = i0;
      i0 = 162u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
    } else {
      i0 = c;
    }
    i1 = a;
    i2 = 13984u;
    i1 += i2;
    i2 = b;
    i1 = i1 != i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    c = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 16u);
        l3 = i0;
        i0 = a;
        i1 = 14000u;
        i0 += i1;
        c = i0;
      }
      i0 = l8;
      i1 = 163u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = c;
        i1 = l3;
        i2 = l9;
        AssignTaggedTreeRange(ctx, i0, i1, i2);
        i0 = 163u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i1 = 28u;
      i0 += i1;
      l3 = i0;
      i0 = a;
      i1 = 14012u;
      i0 += i1;
      c = i0;
    }
    i0 = l8;
    i1 = 164u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l3;
      i0 = SetStringFromSource(ctx, i0, i1);
      l7 = i0;
      i0 = 164u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = 14024u;
      i0 += i1;
      a = i0;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 40u);
      StoreU32(ctx, i0, i1);
      goto block_6;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_1031:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    i2 = l6;
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
    if (i0) {goto block_1047;}
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 165u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l10;
    i2 = 0u;
    i3 = kDataFFFFFFFF;
    i4 = kMsgSetonhidein;
    i5 = 11u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l7 = i0;
    i0 = 165u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
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
      i0 = c;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l5 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l3 = i0;
        l6 = i0;
        goto block_1047;
      }
      i0 = a;
      i1 = 14032u;
      i0 += i1;
      c = i0;
    }
    i0 = l8;
    i1 = 166u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l10;
      i0 = SetStringFromSource(ctx, i0, i1);
      l7 = i0;
      i0 = 166u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
    } else {
      i0 = c;
    }
    i1 = a;
    i2 = 14028u;
    i1 += i2;
    i2 = b;
    i1 = i1 != i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    c = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 16u);
        l3 = i0;
        i0 = a;
        i1 = 14044u;
        i0 += i1;
        c = i0;
      }
      i0 = l8;
      i1 = 167u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = c;
        i1 = l3;
        i2 = l9;
        AssignTaggedTreeRange(ctx, i0, i1, i2);
        i0 = 167u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i1 = 28u;
      i0 += i1;
      l3 = i0;
      i0 = a;
      i1 = 14056u;
      i0 += i1;
      c = i0;
    }
    i0 = l8;
    i1 = 168u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l3;
      i0 = SetStringFromSource(ctx, i0, i1);
      l7 = i0;
      i0 = 168u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = 14068u;
      i0 += i1;
      a = i0;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 40u);
      StoreU32(ctx, i0, i1);
      goto block_6;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_1047:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    i2 = l6;
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
    if (i0) {goto block_1063;}
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 169u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l10;
    i2 = 0u;
    i3 = kDataFFFFFFFF;
    i4 = kMsgSetonhideout;
    i5 = 12u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l7 = i0;
    i0 = 169u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
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
      i0 = c;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l5 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l3 = i0;
        l6 = i0;
        goto block_1063;
      }
      i0 = a;
      i1 = 14076u;
      i0 += i1;
      c = i0;
    }
    i0 = l8;
    i1 = 170u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l10;
      i0 = SetStringFromSource(ctx, i0, i1);
      l7 = i0;
      i0 = 170u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
    } else {
      i0 = c;
    }
    i1 = a;
    i2 = 14072u;
    i1 += i2;
    i2 = b;
    i1 = i1 != i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    c = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 16u);
        l3 = i0;
        i0 = a;
        i1 = 14088u;
        i0 += i1;
        c = i0;
      }
      i0 = l8;
      i1 = 171u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = c;
        i1 = l3;
        i2 = l9;
        AssignTaggedTreeRange(ctx, i0, i1, i2);
        i0 = 171u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i1 = 28u;
      i0 += i1;
      l3 = i0;
      i0 = a;
      i1 = 14100u;
      i0 += i1;
      c = i0;
    }
    i0 = l8;
    i1 = 172u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l3;
      i0 = SetStringFromSource(ctx, i0, i1);
      l7 = i0;
      i0 = 172u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = 14112u;
      i0 += i1;
      a = i0;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 40u);
      StoreU32(ctx, i0, i1);
      goto block_6;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_1063:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    i2 = l6;
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
    if (i0) {goto block_1079;}
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 173u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l10;
    i2 = 0u;
    i3 = kDataFFFFFFFF;
    i4 = kMsgSetonbacklogin;
    i5 = 14u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l7 = i0;
    i0 = 173u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
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
      i0 = c;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l5 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l3 = i0;
        l6 = i0;
        goto block_1079;
      }
      i0 = a;
      i1 = 14120u;
      i0 += i1;
      c = i0;
    }
    i0 = l8;
    i1 = 174u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l10;
      i0 = SetStringFromSource(ctx, i0, i1);
      l7 = i0;
      i0 = 174u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
    } else {
      i0 = c;
    }
    i1 = a;
    i2 = 14116u;
    i1 += i2;
    i2 = b;
    i1 = i1 != i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    c = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 16u);
        l3 = i0;
        i0 = a;
        i1 = 14132u;
        i0 += i1;
        c = i0;
      }
      i0 = l8;
      i1 = 175u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = c;
        i1 = l3;
        i2 = l9;
        AssignTaggedTreeRange(ctx, i0, i1, i2);
        i0 = 175u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i1 = 28u;
      i0 += i1;
      l3 = i0;
      i0 = a;
      i1 = 14144u;
      i0 += i1;
      c = i0;
    }
    i0 = l8;
    i1 = 176u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l3;
      i0 = SetStringFromSource(ctx, i0, i1);
      l7 = i0;
      i0 = 176u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = 14156u;
      i0 += i1;
      a = i0;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 40u);
      StoreU32(ctx, i0, i1);
      goto block_6;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_1079:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    i2 = l6;
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
    if (i0) {goto block_1095;}
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 177u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l10;
    i2 = 0u;
    i3 = kDataFFFFFFFF;
    i4 = kMsgSetonbacklogout;
    i5 = 15u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l7 = i0;
    i0 = 177u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
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
      i0 = c;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        l5 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 15u);
        l3 = i0;
        l6 = i0;
        goto block_1095;
      }
      i0 = a;
      i1 = 14164u;
      i0 += i1;
      c = i0;
    }
    i0 = l8;
    i1 = 178u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l10;
      i0 = SetStringFromSource(ctx, i0, i1);
      l7 = i0;
      i0 = 178u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
    } else {
      i0 = c;
    }
    i1 = a;
    i2 = 14160u;
    i1 += i2;
    i2 = b;
    i1 = i1 != i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    c = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 16u);
        l3 = i0;
        i0 = a;
        i1 = 14176u;
        i0 += i1;
        c = i0;
      }
      i0 = l8;
      i1 = 179u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = c;
        i1 = l3;
        i2 = l9;
        AssignTaggedTreeRange(ctx, i0, i1, i2);
        i0 = 179u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i1 = 28u;
      i0 += i1;
      l3 = i0;
      i0 = a;
      i1 = 14188u;
      i0 += i1;
      c = i0;
    }
    i0 = l8;
    i1 = 180u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l3;
      i0 = SetStringFromSource(ctx, i0, i1);
      l7 = i0;
      i0 = 180u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = 14200u;
      i0 += i1;
      a = i0;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 40u);
      StoreU32(ctx, i0, i1);
      goto block_6;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_1095:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l3;
    i2 = l6;
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
    if (i0) {goto block_6;}
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 181u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l10;
    i2 = 0u;
    i3 = kDataFFFFFFFF;
    i4 = kMsgSetondirchg;
    i5 = 11u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l7 = i0;
    i0 = 181u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    l3 = i0;
    if (i0) {goto exit_path;}
    i0 = c;
    if (i0) {goto block_6;}
    i0 = a;
    i1 = 14264u;
    i0 += i1;
    c = i0;
  }
  i0 = l8;
  i1 = 182u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = c;
    i1 = l10;
    i0 = SetStringFromSource(ctx, i0, i1);
    l7 = i0;
    i0 = 182u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
  } else {
    i0 = c;
  }
  i1 = a;
  i2 = 14260u;
  i1 += i2;
  i2 = b;
  i1 = i1 != i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  c = i0;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0 + 16u);
      l3 = i0;
      i0 = a;
      i1 = 14276u;
      i0 += i1;
      c = i0;
    }
    i0 = l8;
    i1 = 183u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l3;
      i2 = l9;
      AssignTaggedTreeRange(ctx, i0, i1, i2);
      i0 = 183u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i1 = 28u;
    i0 += i1;
    l3 = i0;
    i0 = a;
    i1 = 14288u;
    i0 += i1;
    c = i0;
  }
  i0 = l8;
  i1 = 184u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = c;
    i1 = l3;
    i0 = SetStringFromSource(ctx, i0, i1);
    i0 = 184u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = 14300u;
    i0 += i1;
    a = i0;
    i1 = b;
    i1 = LoadU32(ctx, i1 + 40u);
    StoreU32(ctx, i0, i1);
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 1360u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 1320u);
    DeallocateRawBlock(ctx, i0);
    i0 = a;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  exit_path:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    a = i0;
    i0 = HostTempRet0(ctx);
  }
  i0 = l8;
  i1 = 185u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 185u;
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
  i2 = kResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l7 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l7;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l7;
  i1 = c;
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
  i1 = l9;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l7;
  i1 = l10;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = l7;
  i1 = l11;
  StoreU32(ctx, i0 + 36u, i1);
  i0 = l7;
  i1 = l12;
  StoreU32(ctx, i0 + 40u, i1);
  i0 = l7;
  i1 = l13;
  StoreU32(ctx, i0 + 44u, i1);
  i0 = l7;
  i1 = l14;
  StoreU32(ctx, i0 + 48u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 52u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
}  // namespace artc
