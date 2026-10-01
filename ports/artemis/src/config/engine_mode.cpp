#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step calls.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void Panic(EngineContext* ctx, uint32_t a0);
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t PollEventKind(EngineContext* ctx);
void ApplyCommandSkipStatus(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
void ApplyAutoModeStatus(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

namespace EngineMode {
// EngineMode::Dispatch -- the engine mode dispatch: one suspendable step that
// carries a mode transition through the mode handler table.

void Dispatch(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  constexpr uint32_t kMsgBacklog = 0xAF4E6u;  // backlog
  constexpr uint32_t kMsgRclick = 0xAF568u;  // rclick
  constexpr uint32_t kMsgAutomode = 0xAF628u;  // automode
  constexpr uint32_t kMsgSkip = 0xAF631u;  // skip
  constexpr uint32_t kMsgHide = 0xAF642u;  // hide
  constexpr uint32_t kDataB4B29 = 0xB4B29u;
  constexpr uint32_t kMsgCommand = 0xB5200u;  // command
  constexpr uint32_t kDataB5203 = 0xB5203u;
  constexpr uint32_t kDataB5E30 = 0xB5E30u;
  constexpr uint32_t kMsgPx = 0xB5E31u;  // px != 0
  constexpr uint32_t kMsgSmartPtrPath = 0xB5E39u;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
  constexpr uint32_t kMsgOperator = 0xB5E71u;  // operator->
  constexpr uint32_t kData65646F6D = 0x65646F6Du;
  constexpr uint32_t kDataFFFFFFFF = 0xFFFFFFFFu;
  constexpr uint32_t kSaveFrameBytes = 36u;
  constexpr uint32_t kResumeWordBytes = 4u;
  uint32_t l10 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u; uint32_t i6 = 0u;
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
    l6 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 24u);
    l7 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 28u);
    l9 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 32u);
    l10 = i0;
    i0 = l6;
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
    i1 = 48u;
    i0 -= i1;
    c = i0;
    GuestStackSp(ctx) = i0;
    i0 = c;
    i1 = 7u;
    StoreU8(ctx, i0 + 19u, i1);
    i0 = c;
    i1 = 0u;
    StoreU8(ctx, i0 + 15u, i1);
    i0 = c;
    i1 = kMsgCommand;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = c;
    i1 = kDataB5203;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 11u, i1);
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = c;
    i2 = 8u;
    i1 += i2;
    StoreU32(ctx, i0 + 32u, i1);
    i0 = b;
    i1 = 16u;
    i0 += i1;
    l10 = i0;
    i0 = c;
    i1 = 8u;
    i0 += i1;
    l5 = i0;
    i0 = c;
    i1 = 32u;
    i0 += i1;
    l7 = i0;
    i0 = c;
    i1 = 24u;
    i0 += i1;
    l9 = i0;
    i0 = c;
    i1 = 40u;
    i0 += i1;
    l4 = i0;
  }
  i0 = l8;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = l4;
    i2 = l10;
    i3 = l5;
    i4 = kDataB5E30;
    i5 = l7;
    i6 = l9;
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
    l4 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
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
      i0 = c;
      i0 = LoadU32(ctx, i0 + 40u);
      l4 = i0;
      i0 = LoadU32(ctx, i0 + 32u);
      l7 = i0;
      i0 = l4;
      i0 = LoadU8(ctx, i0 + 39u);
      l5 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l9 = i0;
      i0 = l7;
      i1 = l5;
      i2 = l9;
      i0 = i2 ? i0 : i1;
      i1 = 6u;
      i0 = i0 == i1;
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
        i0 = 1354u;
        i1 = l3;
        i2 = 0u;
        i3 = kDataFFFFFFFF;
        i4 = kMsgRclick;
        i5 = 6u;
        i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
        l6 = i0;
        i0 = 1u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l6;
        l3 = i0;
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l3;
      } else {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 1u;
        i0 = i0 == i1;
        l4 = i0;
        if (i0) {goto block_14;}
        i0 = l3;
        i0 = !(i0);
      }
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = c;
      i0 = LoadS8(ctx, i0 + 19u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l4 = i0;
      if (i0) {
        i0 = c;
        i0 = LoadU32(ctx, i0 + 8u);
        l4 = i0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l3;
      i0 = !(i0);
      l3 = i0;
      if (i0) {goto block_15;}
      i0 = a;
      i1 = 1u;
      StoreU8(ctx, i0 + 2932u, i1);
      i0 = a;
      i1 = 2876u;
      i0 += i1;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i1 = 13u;
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
        i0 = a;
        i1 = 4u;
        StoreU32(ctx, i0 + 2876u, i1);
        i0 = a;
        i1 = 2880u;
        i0 += i1;
        i1 = b;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i0 = LoadU32(ctx, i0 + 8u);
        b = i0;
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
        i1 = 2u;
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
          i0 = 2u;
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
          if (i0) {goto block_10;}
          goto block_6;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 28u);
        l3 = i0;
      }
      i0 = l8;
      i1 = 3u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = b;
        i1 = 0u;
        i2 = l3;
        CallTableT0(ctx, i2, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 3u;
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
      i1 = 2768u;
      i0 += i1;
      a = i0;
      i1 = PollEventKind(ctx);
      StoreU32(ctx, i0, i1);
      goto block_12;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l3 = i0;
    i0 = HostTempRet0(ctx);
    i0 = c;
    i0 = LoadS8(ctx, i0 + 19u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_7;}
    goto block_5;
  }
  block_15:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i1 = 7u;
    StoreU8(ctx, i0 + 19u, i1);
    i0 = c;
    i1 = 0u;
    StoreU8(ctx, i0 + 15u, i1);
    i0 = c;
    i1 = kMsgCommand;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = c;
    i1 = kDataB5203;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 11u, i1);
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = c;
    i2 = 8u;
    i1 += i2;
    StoreU32(ctx, i0 + 32u, i1);
    i0 = c;
    i1 = 40u;
    i0 += i1;
    l4 = i0;
    i0 = c;
    i1 = 8u;
    i0 += i1;
    l5 = i0;
    i0 = c;
    i1 = 32u;
    i0 += i1;
    l7 = i0;
    i0 = c;
    i1 = 24u;
    i0 += i1;
    l9 = i0;
    i0 = 0u;
    l3 = i0;
  }
  i0 = l8;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = l4;
    i2 = l10;
    i3 = l5;
    i4 = kDataB5E30;
    i5 = l7;
    i6 = l9;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 4u;
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
    l4 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_8;}
    i0 = c;
    i0 = LoadU32(ctx, i0 + 40u);
    l4 = i0;
    i0 = LoadU32(ctx, i0 + 32u);
    l7 = i0;
    i0 = l4;
    i0 = LoadU8(ctx, i0 + 39u);
    l5 = i0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l9 = i0;
    i0 = l7;
    i1 = l5;
    i2 = l9;
    i0 = i2 ? i0 : i1;
    i1 = 8u;
    i0 = i0 == i1;
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
      i1 = 28u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l8;
    i1 = 5u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = l3;
      i2 = 0u;
      i3 = kDataFFFFFFFF;
      i4 = kMsgAutomode;
      i5 = 8u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l6 = i0;
      i0 = 5u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l6;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l3;
    } else {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      l4 = i0;
      if (i0) {goto block_40;}
      i0 = l3;
      i0 = !(i0);
    }
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i0 = LoadS8(ctx, i0 + 19u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    l4 = i0;
    if (i0) {
      i0 = c;
      i0 = LoadU32(ctx, i0 + 8u);
      l4 = i0;
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_13;}
    i0 = c;
    i1 = 0u;
    StoreU8(ctx, i0 + 12u, i1);
    i0 = c;
    i1 = kData65646F6D;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = c;
    i1 = 4u;
    StoreU8(ctx, i0 + 19u, i1);
    i0 = b;
    i0 = LoadU32(ctx, i0 + 20u);
    l3 = i0;
  }
  i0 = l3;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    loop_52: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadU8(ctx, i0 + 27u);
        b = i0;
        i1 = 24u;
        i0 <<= (i1 & 31);
        i1 = 24u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l5 = i0;
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 20u);
        i1 = b;
        i2 = l5;
        i0 = i2 ? i0 : i1;
        b = i0;
        i1 = kResumeWordBytes;
        i0 = i0 < i1;
        l9 = i0;
        i0 = b;
        i1 = kResumeWordBytes;
        i2 = l9;
        i0 = i2 ? i0 : i1;
        l4 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        if (i0) {
          i0 = c;
          i1 = 8u;
          i0 += i1;
          i1 = l3;
          i2 = 16u;
          i1 += i2;
          l7 = i1;
          i1 = LoadU32(ctx, i1);
          i2 = l7;
          i3 = l5;
          i1 = i3 ? i1 : i2;
          l5 = i1;
          i2 = l4;
          i0 = CompareBytes(ctx, i0, i1, i2);
          l7 = i0;
          i0 = !(i0);
          if (i0) {
            i0 = b;
            i1 = 4u;
            i0 = i0 <= i1;
            b = i0;
            if (i0) {goto block_60;}
            goto block_54;
          }
          i0 = l7;
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          b = i0;
          if (i0) {goto block_54;}
          block_60:;
          i0 = l5;
          i1 = c;
          i2 = 8u;
          i1 += i2;
          i2 = l4;
          i0 = CompareBytes(ctx, i0, i1, i2);
          b = i0;
          i0 = !(i0);
          l4 = i0;
          if (i0) {goto block_58;}
          i0 = b;
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          b = i0;
          if (i0) {goto block_57;}
          goto block_55;
        }
        i0 = b;
        i1 = 4u;
        i0 = i0 > i1;
        b = i0;
        if (i0) {goto block_54;}
        block_58:;
        i0 = l9;
        if (i0) {goto block_55;}
        block_57:;
        i0 = c;
        i1 = 0u;
        StoreU8(ctx, i0 + 12u, i1);
        i0 = c;
        i1 = kData65646F6D;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = c;
        i1 = 4u;
        StoreU8(ctx, i0 + 19u, i1);
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = c;
        i1 = c;
        i2 = 8u;
        i1 += i2;
        StoreU32(ctx, i0 + 32u, i1);
        i0 = c;
        i1 = 8u;
        i0 += i1;
        l3 = i0;
        i0 = c;
        i1 = 32u;
        i0 += i1;
        l4 = i0;
        i0 = c;
        i1 = 24u;
        i0 += i1;
        l5 = i0;
        i0 = c;
        i1 = 40u;
        i0 += i1;
        b = i0;
      }
      i0 = l8;
      i1 = 6u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1959u;
        i1 = b;
        i2 = l10;
        i3 = l3;
        i4 = kDataB5E30;
        i5 = l4;
        i6 = l5;
        HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
        i0 = 6u;
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
        i0 = 1u;
        l3 = i0;
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto block_50;}
        i0 = c;
        i0 = LoadU32(ctx, i0 + 40u);
        b = i0;
        i0 = LoadU32(ctx, i0 + 32u);
        l4 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 39u);
        l10 = i0;
        i1 = 24u;
        i0 <<= (i1 & 31);
        i1 = 24u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l5 = i0;
        i0 = l4;
        i1 = l10;
        i2 = l5;
        i0 = i2 ? i0 : i1;
        i1 = 1u;
        i0 = i0 == i1;
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
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = b;
          i1 = 28u;
          i0 += i1;
          b = i0;
        }
        i0 = l8;
        i1 = 7u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1354u;
          i1 = b;
          i2 = 0u;
          i3 = kDataFFFFFFFF;
          i4 = kDataB4B29;
          i5 = 1u;
          i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
          l6 = i0;
          i0 = 7u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l6;
          b = i0;
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
          l3 = i0;
          if (i0) {goto block_49;}
          i0 = b;
          i1 = 0u;
          i0 = i0 != i1;
        }
        l3 = i0;
      }
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
        i0 = 2626u;
        i1 = a;
        i2 = l3;
        i3 = 1u;
        HostSyncSlot(ctx, i0, i1, i2, i3);
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
        i0 = i0 == i1;
        a = i0;
        if (i0) {goto block_50;}
        i0 = c;
        i0 = LoadS8(ctx, i0 + 19u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        a = i0;
        if (i0) {goto block_12;}
        i0 = c;
        i0 = LoadU32(ctx, i0 + 8u);
        a = i0;
        DeallocateRawBlock(ctx, i0);
        goto block_12;
      }
      block_55:;
      i0 = l3;
      i1 = l3;
      i2 = 4u;
      i1 += i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      l3 = i0;
      block_54:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        if (i0) {goto loop_52;}
      }
  }
  i0 = l8;
  i1 = 9u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 1u;
    i2 = 1u;
    ApplyAutoModeStatus(ctx, i0, i1, i2);
    i0 = 9u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_12;}
  block_50:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l3 = i0;
    i0 = HostTempRet0(ctx);
    i0 = c;
    i0 = LoadS8(ctx, i0 + 19u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_7;}
    goto block_5;
  }
  block_49:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_6;}
  block_40:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_6;}
  block_14:;
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
  i1 = 10u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 10u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_10;}
  block_13:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i1 = 7u;
    StoreU8(ctx, i0 + 19u, i1);
    i0 = c;
    i1 = 0u;
    StoreU8(ctx, i0 + 15u, i1);
    i0 = c;
    i1 = kMsgCommand;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = c;
    i1 = kDataB5203;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 11u, i1);
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = c;
    i2 = 8u;
    i1 += i2;
    StoreU32(ctx, i0 + 32u, i1);
    i0 = c;
    i1 = 40u;
    i0 += i1;
    l4 = i0;
    i0 = c;
    i1 = 8u;
    i0 += i1;
    l5 = i0;
    i0 = c;
    i1 = 32u;
    i0 += i1;
    l7 = i0;
    i0 = c;
    i1 = 24u;
    i0 += i1;
    l9 = i0;
    i0 = 0u;
    l3 = i0;
  }
  i0 = l8;
  i1 = 11u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = l4;
    i2 = l10;
    i3 = l5;
    i4 = kDataB5E30;
    i5 = l7;
    i6 = l9;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
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
    l4 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_9;}
    i0 = c;
    i0 = LoadU32(ctx, i0 + 40u);
    l4 = i0;
    i0 = LoadU32(ctx, i0 + 32u);
    l7 = i0;
    i0 = l4;
    i0 = LoadU8(ctx, i0 + 39u);
    l5 = i0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l9 = i0;
    i0 = l7;
    i1 = l5;
    i2 = l9;
    i0 = i2 ? i0 : i1;
    i1 = 4u;
    i0 = i0 == i1;
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
      i1 = 28u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l8;
    i1 = 12u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = l3;
      i2 = 0u;
      i3 = kDataFFFFFFFF;
      i4 = kMsgSkip;
      i5 = 4u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l6 = i0;
      i0 = 12u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l6;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l3;
    } else {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      l4 = i0;
      if (i0) {goto block_6;}
      i0 = l3;
      i0 = !(i0);
    }
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i0 = LoadS8(ctx, i0 + 19u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    l4 = i0;
    if (i0) {
      i0 = c;
      i0 = LoadU32(ctx, i0 + 8u);
      l4 = i0;
      DeallocateRawBlock(ctx, i0);
    }
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
      i1 = 0u;
      StoreU8(ctx, i0 + 12u, i1);
      i0 = c;
      i1 = kData65646F6D;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = c;
      i1 = 4u;
      StoreU8(ctx, i0 + 19u, i1);
      i0 = b;
      i0 = LoadU32(ctx, i0 + 20u);
      l3 = i0;
    }
    i0 = l3;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      loop_93: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i0 = LoadU8(ctx, i0 + 27u);
          b = i0;
          i1 = 24u;
          i0 <<= (i1 & 31);
          i1 = 24u;
          i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
          i1 = 0u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          l5 = i0;
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 20u);
          i1 = b;
          i2 = l5;
          i0 = i2 ? i0 : i1;
          b = i0;
          i1 = kResumeWordBytes;
          i0 = i0 < i1;
          l9 = i0;
          i0 = b;
          i1 = kResumeWordBytes;
          i2 = l9;
          i0 = i2 ? i0 : i1;
          l4 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          if (i0) {
            i0 = c;
            i1 = 8u;
            i0 += i1;
            i1 = l3;
            i2 = 16u;
            i1 += i2;
            l7 = i1;
            i1 = LoadU32(ctx, i1);
            i2 = l7;
            i3 = l5;
            i1 = i3 ? i1 : i2;
            l5 = i1;
            i2 = l4;
            i0 = CompareBytes(ctx, i0, i1, i2);
            l7 = i0;
            i0 = !(i0);
            if (i0) {
              i0 = b;
              i1 = 4u;
              i0 = i0 <= i1;
              b = i0;
              if (i0) {goto block_101;}
              goto block_95;
            }
            i0 = l7;
            i1 = kDataFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            b = i0;
            if (i0) {goto block_95;}
            block_101:;
            i0 = l5;
            i1 = c;
            i2 = 8u;
            i1 += i2;
            i2 = l4;
            i0 = CompareBytes(ctx, i0, i1, i2);
            b = i0;
            i0 = !(i0);
            l4 = i0;
            if (i0) {goto block_99;}
            i0 = b;
            i1 = kDataFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            b = i0;
            if (i0) {goto block_98;}
            goto block_96;
          }
          i0 = b;
          i1 = 4u;
          i0 = i0 > i1;
          b = i0;
          if (i0) {goto block_95;}
          block_99:;
          i0 = l9;
          if (i0) {goto block_96;}
          block_98:;
          i0 = c;
          i1 = 0u;
          StoreU8(ctx, i0 + 12u, i1);
          i0 = c;
          i1 = kData65646F6D;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = c;
          i1 = 4u;
          StoreU8(ctx, i0 + 19u, i1);
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = c;
          i1 = c;
          i2 = 8u;
          i1 += i2;
          StoreU32(ctx, i0 + 32u, i1);
          i0 = c;
          i1 = 8u;
          i0 += i1;
          l3 = i0;
          i0 = c;
          i1 = 32u;
          i0 += i1;
          l4 = i0;
          i0 = c;
          i1 = 24u;
          i0 += i1;
          l5 = i0;
          i0 = c;
          i1 = 40u;
          i0 += i1;
          b = i0;
        }
        i0 = l8;
        i1 = 13u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1959u;
          i1 = b;
          i2 = l10;
          i3 = l3;
          i4 = kDataB5E30;
          i5 = l4;
          i6 = l5;
          HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
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
          b = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = 1u;
          l3 = i0;
          i0 = b;
          i1 = 1u;
          i0 = i0 == i1;
          b = i0;
          if (i0) {goto block_89;}
          i0 = c;
          i0 = LoadU32(ctx, i0 + 40u);
          b = i0;
          i0 = LoadU32(ctx, i0 + 32u);
          l4 = i0;
          i0 = b;
          i0 = LoadU8(ctx, i0 + 39u);
          l10 = i0;
          i1 = 24u;
          i0 <<= (i1 & 31);
          i1 = 24u;
          i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
          i1 = 0u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          l5 = i0;
          i0 = l4;
          i1 = l10;
          i2 = l5;
          i0 = i2 ? i0 : i1;
          i1 = 1u;
          i0 = i0 == i1;
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
            i0 = kDataD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = b;
            i1 = 28u;
            i0 += i1;
            b = i0;
          }
          i0 = l8;
          i1 = 14u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 1354u;
            i1 = b;
            i2 = 0u;
            i3 = kDataFFFFFFFF;
            i4 = kDataB4B29;
            i5 = 1u;
            i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
            l6 = i0;
            i0 = 14u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l6;
            b = i0;
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
            l3 = i0;
            if (i0) {goto block_88;}
            i0 = b;
            i1 = 0u;
            i0 = i0 != i1;
          }
          l3 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
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
          i0 = 2627u;
          i1 = a;
          i2 = l3;
          i3 = 1u;
          HostSyncSlot(ctx, i0, i1, i2, i3);
          i0 = 15u;
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
          if (i0) {goto block_89;}
          i0 = c;
          i0 = LoadS8(ctx, i0 + 19u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_12;}
          i0 = c;
          i0 = LoadU32(ctx, i0 + 8u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
          goto block_12;
        }
        block_96:;
        i0 = l3;
        i1 = l3;
        i2 = 4u;
        i1 += i2;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        l3 = i0;
        block_95:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          if (i0) {goto loop_93;}
        }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU8(ctx, i0 + 49u);
      i0 = !(i0);
      b = i0;
    }
    i0 = l8;
    i1 = 16u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = b;
      i2 = 1u;
      ApplyCommandSkipStatus(ctx, i0, i1, i2);
      i0 = 16u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_12;}
    block_89:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      l3 = i0;
      i0 = HostTempRet0(ctx);
      i0 = c;
      i0 = LoadS8(ctx, i0 + 19u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_7;}
      goto block_5;
    }
    block_88:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_6;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i1 = 7u;
    StoreU8(ctx, i0 + 19u, i1);
    i0 = 0u;
    l3 = i0;
    i0 = c;
    i1 = 0u;
    StoreU8(ctx, i0 + 15u, i1);
    i0 = c;
    i1 = kMsgCommand;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = c;
    i1 = kDataB5203;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 11u, i1);
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = c;
    i2 = 8u;
    i1 += i2;
    StoreU32(ctx, i0 + 32u, i1);
    i0 = c;
    i1 = 8u;
    i0 += i1;
    l4 = i0;
    i0 = c;
    i1 = 32u;
    i0 += i1;
    l5 = i0;
    i0 = c;
    i1 = 24u;
    i0 += i1;
    l7 = i0;
    i0 = c;
    i1 = 40u;
    i0 += i1;
    b = i0;
  }
  i0 = l8;
  i1 = 17u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = b;
    i2 = l10;
    i3 = l4;
    i4 = kDataB5E30;
    i5 = l5;
    i6 = l7;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 17u;
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
      i0 = c;
      i0 = LoadU32(ctx, i0 + 40u);
      b = i0;
      i0 = LoadU32(ctx, i0 + 32u);
      l5 = i0;
      i0 = b;
      i0 = LoadU8(ctx, i0 + 39u);
      l4 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l7 = i0;
      i0 = l5;
      i1 = l4;
      i2 = l7;
      i0 = i2 ? i0 : i1;
      i1 = 4u;
      i0 = i0 == i1;
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 28u;
        i0 += i1;
        b = i0;
      }
      i0 = l8;
      i1 = 18u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1354u;
        i1 = b;
        i2 = 0u;
        i3 = kDataFFFFFFFF;
        i4 = kMsgHide;
        i5 = 4u;
        i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
        l6 = i0;
        i0 = 18u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l6;
        b = i0;
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
        l3 = i0;
        if (i0) {goto block_6;}
        i0 = b;
        i0 = !(i0);
      }
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = c;
      i0 = LoadS8(ctx, i0 + 19u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = c;
        i0 = LoadU32(ctx, i0 + 8u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l3;
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_119;}
      i0 = a;
      i1 = 1u;
      StoreU8(ctx, i0 + 2933u, i1);
      i0 = a;
      i1 = 2876u;
      i0 += i1;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i1 = 13u;
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
        i0 = a;
        i1 = 4u;
        StoreU32(ctx, i0 + 2876u, i1);
        i0 = a;
        i1 = 2880u;
        i0 += i1;
        i1 = b;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i0 = LoadU32(ctx, i0 + 8u);
        b = i0;
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
        i1 = 19u;
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
          a = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_6;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 28u);
        l3 = i0;
      }
      i0 = l8;
      i1 = 20u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = b;
        i1 = 0u;
        i2 = l3;
        CallTableT0(ctx, i2, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 20u;
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
      i1 = 2768u;
      i0 += i1;
      a = i0;
      i1 = PollEventKind(ctx);
      StoreU32(ctx, i0, i1);
      goto block_12;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l3 = i0;
    i0 = HostTempRet0(ctx);
    i0 = c;
    i0 = LoadS8(ctx, i0 + 19u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_7;}
    goto block_5;
  }
  block_119:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i1 = 7u;
    StoreU8(ctx, i0 + 19u, i1);
    i0 = 0u;
    l3 = i0;
    i0 = c;
    i1 = 0u;
    StoreU8(ctx, i0 + 15u, i1);
    i0 = c;
    i1 = kMsgCommand;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = c;
    i1 = kDataB5203;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 11u, i1);
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = c;
    i2 = 8u;
    i1 += i2;
    StoreU32(ctx, i0 + 32u, i1);
    i0 = c;
    i1 = 8u;
    i0 += i1;
    l4 = i0;
    i0 = c;
    i1 = 32u;
    i0 += i1;
    l5 = i0;
    i0 = c;
    i1 = 24u;
    i0 += i1;
    l7 = i0;
    i0 = c;
    i1 = 40u;
    i0 += i1;
    b = i0;
  }
  i0 = l8;
  i1 = 21u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = b;
    i2 = l10;
    i3 = l4;
    i4 = kDataB5E30;
    i5 = l5;
    i6 = l7;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 21u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  if (i0) {
    i0 = l4;
  } else {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_11;}
    i0 = c;
    i0 = LoadU32(ctx, i0 + 40u);
    b = i0;
    i0 = LoadU32(ctx, i0 + 32u);
    l4 = i0;
    i0 = b;
    i0 = LoadU8(ctx, i0 + 39u);
    l10 = i0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l5 = i0;
    i0 = l4;
    i1 = l10;
    i2 = l5;
    i0 = i2 ? i0 : i1;
    i1 = 7u;
    i0 = i0 == i1;
  }
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
      i0 = b;
      i1 = 28u;
      i0 += i1;
      b = i0;
    }
    i0 = l8;
    i1 = 22u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = b;
      i2 = 0u;
      i3 = kDataFFFFFFFF;
      i4 = kMsgBacklog;
      i5 = 7u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l6 = i0;
      i0 = 22u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l6;
      b = i0;
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
      l3 = i0;
      if (i0) {goto block_6;}
      i0 = b;
      i0 = !(i0);
    }
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i0 = LoadS8(ctx, i0 + 19u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = c;
      i0 = LoadU32(ctx, i0 + 8u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i0 = !(i0);
    if (i0) {goto block_12;}
    i0 = a;
    i1 = 1u;
    StoreU8(ctx, i0 + 2934u, i1);
    i0 = a;
    i1 = 2876u;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i1 = 13u;
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
      i0 = a;
      i1 = 4u;
      StoreU32(ctx, i0 + 2876u, i1);
      i0 = a;
      i1 = 2880u;
      i0 += i1;
      i1 = b;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i0 = LoadU32(ctx, i0 + 8u);
      b = i0;
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
      i1 = 23u;
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
        a = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_10;}
        goto block_6;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 28u);
      l3 = i0;
    }
    i0 = l8;
    i1 = 24u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = 0u;
      i2 = l3;
      CallTableT0(ctx, i2, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = 24u;
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
    i1 = 2768u;
    i0 += i1;
    a = i0;
    i1 = PollEventKind(ctx);
    StoreU32(ctx, i0, i1);
  }
  block_12:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i1 = 48u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_11:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l3 = i0;
    i0 = HostTempRet0(ctx);
    i0 = c;
    i0 = LoadS8(ctx, i0 + 19u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_7;}
    i0 = c;
    i0 = LoadU32(ctx, i0 + 8u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_7;
  }
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
    i0 = HostRet0(ctx);
    l3 = i0;
    i0 = HostTempRet0(ctx);
    i0 = c;
    i0 = LoadS8(ctx, i0 + 19u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_7;}
    goto block_5;
  }
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l3 = i0;
    i0 = HostTempRet0(ctx);
    i0 = c;
    i0 = LoadS8(ctx, i0 + 19u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_7;}
    goto block_5;
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    a = i0;
    i0 = HostTempRet0(ctx);
  }
  i0 = l8;
  i1 = 25u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 25u;
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
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i0 = LoadU32(ctx, i0 + 8u);
    DeallocateRawBlock(ctx, i0);
    i0 = l3;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  goto func_exit;
  spill_frame:;
  l6 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l6;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l6 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l6;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l6;
  i1 = c;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l6;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l6;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l6;
  i1 = l5;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l6;
  i1 = l7;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l6;
  i1 = l9;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l6;
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
}  // namespace artc
