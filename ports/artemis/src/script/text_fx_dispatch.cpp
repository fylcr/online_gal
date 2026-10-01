#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step calls.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
uint32_t PushObjectDequeElement(EngineContext* ctx, uint32_t a0, uint32_t a1);
void Panic(EngineContext* ctx, uint32_t a0);
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t CopyBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void ReleaseNodeTree(EngineContext* ctx, uint32_t state, uint32_t node);

namespace MessageLayer {
// MessageLayer::TextFxDispatch -- the text effect dispatch step: the pending
// text-effect slots are advanced one message layer step.

void TextFxDispatch(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e) {
  EnterFrame();
  constexpr uint32_t kDataB5EC4 = 0xB5EC4u;
  constexpr uint32_t kDataB5F40 = 0xB5F40u;
  constexpr uint32_t kMsgPrint = 0xB9B20u;  // print
  constexpr uint32_t kMsgFont = 0xB9B26u;  // font
  constexpr uint32_t kMsgFontX = 0xB9B2Bu;  // /font
  constexpr uint32_t kDataB9B45 = 0xB9B45u;
  constexpr uint32_t kMsgOmitblankline = 0xB9B48u;  // omitblankline
  constexpr uint32_t kDataB9B4D = 0xB9B4Du;
  constexpr uint32_t kDataB9B56 = 0xB9B56u;
  constexpr uint32_t kMsgExcludeBacklog = 0xB9B58u;  // exclude_backlog
  constexpr uint32_t kDataB9B5F = 0xB9B5Fu;
  constexpr uint32_t kMsgRuby = 0xB9B68u;  // ruby
  constexpr uint32_t kDataB9B6D = 0xB9B6Du;
  constexpr uint32_t kMsgRubyX = 0xB9B6Eu;  // /ruby
  constexpr uint32_t kMsgVoice = 0xB9B74u;  // voice
  constexpr uint32_t kMsgVoiceX = 0xB9B7Au;  // /voice
  constexpr uint32_t kDataBA360 = 0xBA360u;
  constexpr uint32_t kMsgPx = 0xBA361u;  // px != 0
  constexpr uint32_t kMsgSmartPtrPath = 0xBA369u;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
  constexpr uint32_t kMsgOperator = 0xBA3A1u;  // operator->
  constexpr uint32_t kData61746164 = 0x61746164u;
  constexpr uint32_t kData74786574 = 0x74786574u;
  constexpr uint32_t kDataFFFFFFFE = 0xFFFFFFFEu;
  constexpr uint32_t kDataFFFFFFFF = 0xFFFFFFFFu;
  constexpr uint32_t kSaveFrameBytes = 128u;
  constexpr uint32_t kResumeWordBytes = 4u;
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l13 = 0u; uint32_t l14 = 0u; uint32_t l15 = 0u; uint32_t l16 = 0u; uint32_t l17 = 0u; uint32_t l18 = 0u; uint32_t l19 = 0u; uint32_t l20 = 0u; uint32_t l21 = 0u; uint32_t l22 = 0u; uint32_t l23 = 0u; uint32_t l24 = 0u; uint32_t l25 = 0u; uint32_t l26 = 0u; uint32_t l27 = 0u; uint32_t l28 = 0u; uint32_t l29 = 0u; uint64_t l30 = 0; uint64_t l31 = 0; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u; uint32_t i6 = 0u;
  uint64_t j0 = 0; uint64_t j1 = 0;
      l13 = 0, l14 = 0, l15 = 0, l16 = 0, l17 = 0, l18 = 0, l19 = 0, l20 = 0, 
      l21 = 0, l22 = 0, l23 = 0, l24 = 0, l25 = 0, l26 = 0, l27 = 0, l28 = 0, 
      l29 = 0;
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
    l8 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 12u);
    d = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 16u);
    e = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 24u);
    l6 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 28u);
    l7 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 32u);
    l10 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 36u);
    l9 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 40u);
    l12 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 44u);
    l13 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 48u);
    l14 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 52u);
    l15 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 56u);
    l16 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 60u);
    l17 = i0;
    i0 = l8;
    j0 = LoadU64(ctx, i0 + 64u);
    l30 = j0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 72u);
    l18 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 76u);
    l19 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 80u);
    l20 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 84u);
    l21 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 88u);
    l22 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 92u);
    l23 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 96u);
    l24 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 100u);
    l25 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 104u);
    l26 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 108u);
    l27 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 112u);
    l28 = i0;
    i0 = l8;
    j0 = LoadU64(ctx, i0 + 116u);
    l31 = j0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 124u);
    l29 = i0;
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
    i2 = kResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l11 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    i1 = 480u;
    i0 -= i1;
    l6 = i0;
    GuestStackSp(ctx) = i0;
    i0 = b;
    i0 = LoadU32(ctx, i0);
    i0 = !(i0);
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    if (i0) {goto block_7;}
    i0 = l6;
    i1 = 432u;
    i0 += i1;
    l5 = i0;
  }
  i0 = l11;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l5;
    i1 = c;
    i0 = PushObjectDequeElement(ctx, i0, i1);
    l8 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    l12 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i1 = 0u;
    StoreU32(ctx, i0 + 424u, i1);
    i0 = l6;
    j1 = 0ull;
    StoreU64(ctx, i0 + 416u, j1);
    i0 = l6;
    i1 = 0u;
    StoreU32(ctx, i0 + 408u, i1);
    i0 = l6;
    j1 = 0ull;
    StoreU64(ctx, i0 + 400u, j1);
    i0 = e;
    i0 = !(i0);
    c = i0;
    if (i0) {
      i0 = l12;
      i0 = LoadU32(ctx, i0 + 20u);
      l5 = i0;
      goto block_11;
    }
    i0 = l12;
    i0 = LoadU32(ctx, i0 + 16u);
    l7 = i0;
    i1 = 24u;
    i0 = DIV_U(i0, i1);
    c = i0;
    i0 = l12;
    i0 = LoadU32(ctx, i0 + 4u);
    l15 = i0;
    i1 = l12;
    i1 = LoadU32(ctx, i1 + 8u);
    i0 = i0 != i1;
    if (i0) {
      i0 = l15;
      i1 = c;
      i2 = 2u;
      i1 <<= (i2 & 31);
      i0 += i1;
      l13 = i0;
      i0 = LoadU32(ctx, i0);
      i1 = l7;
      i2 = c;
      i3 = 24u;
      i2 *= i3;
      i1 -= i2;
      i2 = 168u;
      i1 *= i2;
      i0 += i1;
      c = i0;
      i0 = l15;
      i1 = l7;
      i2 = l12;
      i2 = LoadU32(ctx, i2 + 20u);
      l5 = i2;
      i1 += i2;
      l7 = i1;
      i2 = 24u;
      i1 = DIV_U(i1, i2);
      l10 = i1;
      i2 = 2u;
      i1 <<= (i2 & 31);
      l9 = i1;
      i0 += i1;
      i0 = LoadU32(ctx, i0);
      l15 = i0;
      i0 = l15;
      i1 = l7;
      i2 = l10;
      i3 = 24u;
      i2 *= i3;
      i1 -= i2;
      i2 = 168u;
      i1 *= i2;
      i0 += i1;
      l7 = i0;
      i1 = c;
      i0 = i0 == i1;
      l10 = i0;
      if (i0) {goto block_11;}
      loop_14: 
        i0 = c;
        i1 = 0u;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = c;
        i1 = 168u;
        i0 += i1;
        c = i0;
        i1 = l13;
        i1 = LoadU32(ctx, i1);
        i0 -= i1;
        i1 = 4032u;
        i0 = i0 == i1;
        if (i0) {
          i0 = l13;
          i0 = LoadU32(ctx, i0 + 4u);
          c = i0;
          i0 = l13;
          i1 = 4u;
          i0 += i1;
          l13 = i0;
        }
        i0 = c;
        i1 = l7;
        i0 = i0 != i1;
        l10 = i0;
        if (i0) {goto loop_14;}
      goto block_11;
    }
    i0 = l12;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
    block_11:;
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
      i0 = l5;
      i1 = l12;
      i1 = LoadU32(ctx, i1 + 16u);
      i0 += i1;
      i1 = 1u;
      i0 -= i1;
      l7 = i0;
      i1 = 24u;
      i0 = DIV_U(i0, i1);
      l13 = i0;
      i0 = b;
      i0 = LoadU32(ctx, i0);
      c = i0;
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l11;
      i1 = 1u;
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
        i0 = 1u;
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
        if (i0) {goto block_5;}
        goto exit_path;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = c;
      i1 = 488u;
      i0 += i1;
      i1 = l12;
      i1 = LoadU32(ctx, i1 + 4u);
      i2 = l13;
      i3 = 2u;
      i2 <<= (i3 & 31);
      i1 += i2;
      i1 = LoadU32(ctx, i1);
      l15 = i1;
      i2 = l7;
      i3 = l13;
      i4 = 24u;
      i3 *= i4;
      i2 -= i3;
      l7 = i2;
      i3 = 168u;
      i2 *= i3;
      i1 += i2;
      l13 = i1;
      i2 = 4u;
      i1 += i2;
      i2 = 68u;
      i0 = CopyBytes(ctx, i0, i1, i2);
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l13;
      i1 = 72u;
      i0 += i1;
      l10 = i0;
      i0 = c;
      i1 = 556u;
      i0 += i1;
      l5 = i0;
    }
    i0 = l11;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 870u;
      i1 = l5;
      i2 = l10;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l8 = i0;
      i0 = 2u;
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
      if (i0) {goto block_17;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l13;
      i1 = 84u;
      i0 += i1;
      l10 = i0;
      i0 = c;
      i1 = 568u;
      i0 += i1;
      l5 = i0;
    }
    i0 = l11;
    i1 = 3u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 870u;
      i1 = l5;
      i2 = l10;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l8 = i0;
      i0 = 3u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l5 = i0;
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = c;
    } else {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l13 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l13;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_17;}
      i0 = c;
      i1 = 580u;
      i0 += i1;
      i1 = l15;
      i2 = l7;
      i3 = 168u;
      i2 *= i3;
      i1 += i2;
      i2 = 96u;
      i1 += i2;
      l5 = i1;
      i2 = 66u;
      i0 = CopyBytes(ctx, i0, i1, i2);
    }
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = d;
    i0 = LoadU32(ctx, i0);
    l13 = i0;
    i1 = d;
    i1 = LoadU32(ctx, i1 + 4u);
    i0 = i0 == i1;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    if (i0) {goto block_31;}
    i0 = l6;
    i1 = 312u;
    i0 += i1;
    l23 = i0;
    i0 = l6;
    i1 = 300u;
    i0 += i1;
    l24 = i0;
    i0 = l6;
    i1 = 288u;
    i0 += i1;
    l25 = i0;
    i0 = l6;
    i1 = 216u;
    i0 += i1;
    i1 = 4u;
    i0 |= i1;
    l26 = i0;
    i0 = l6;
    i1 = 72u;
    i0 += i1;
    l27 = i0;
    i0 = l6;
    i1 = 132u;
    i0 += i1;
    l28 = i0;
    i0 = l6;
    i1 = 168u;
    i0 += i1;
    i1 = 4u;
    i0 |= i1;
    l18 = i0;
    i0 = l6;
    i1 = 32u;
    i0 += i1;
    i1 = 4u;
    i0 |= i1;
    l19 = i0;
    i0 = l6;
    i1 = 16u;
    i0 += i1;
    c = i0;
    i1 = 4u;
    i0 |= i1;
    l20 = i0;
    i0 = 0u;
    l15 = i0;
  }
  loop_34: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l13;
      i0 = LoadU8(ctx, i0 + 15u);
      c = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l5 = i0;
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l9 = i0;
      i0 = l13;
      i0 = LoadU32(ctx, i0 + 8u);
      l7 = i0;
      i1 = c;
      i2 = l9;
      i0 = i2 ? i0 : i1;
      i1 = 5u;
      i0 = i0 != i1;
      l9 = i0;
      i0 = l13;
      i1 = 4u;
      i0 += i1;
      l10 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l9;
      if (i0) {goto block_38;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l11;
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
      i4 = kMsgPrint;
      i5 = 5u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l8 = i0;
      i0 = 4u;
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
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l7 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l7;
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
          i0 = l13;
          i0 = LoadU32(ctx, i0 + 8u);
          l7 = i0;
          i0 = l13;
          i0 = LoadU8(ctx, i0 + 15u);
          c = i0;
          l5 = i0;
          goto block_38;
        }
      }
      i0 = l15;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l6;
          i1 = 0u;
          StoreU8(ctx, i0 + 220u, i1);
          i0 = l6;
          i1 = kData61746164;
          StoreU32(ctx, i0 + 216u, i1);
          i0 = l6;
          i1 = 4u;
          StoreU8(ctx, i0 + 227u, i1);
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l6;
          i1 = l6;
          i2 = 216u;
          i1 += i2;
          StoreU32(ctx, i0 + 184u, i1);
          i0 = l13;
          i1 = 16u;
          i0 += i1;
          l5 = i0;
          i0 = l6;
          i1 = 216u;
          i0 += i1;
          l7 = i0;
          i0 = l6;
          i1 = 184u;
          i0 += i1;
          l10 = i0;
          i0 = l6;
          i1 = 472u;
          i0 += i1;
          l15 = i0;
          i0 = l6;
          i1 = 200u;
          i0 += i1;
          c = i0;
        }
        i0 = l11;
        i1 = 5u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1959u;
          i1 = c;
          i2 = l5;
          i3 = l7;
          i4 = kDataBA360;
          i5 = l10;
          i6 = l15;
          HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
          i0 = 5u;
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
          if (i0) {goto block_50;}
          i0 = l6;
          i0 = LoadU32(ctx, i0 + 200u);
          c = i0;
          i1 = 28u;
          i0 += i1;
          l7 = i0;
          i0 = LoadU32(ctx, i0);
          l15 = i0;
          i0 = c;
          i0 = LoadU32(ctx, i0 + 32u);
          l5 = i0;
          i0 = c;
          i0 = LoadU8(ctx, i0 + 39u);
          c = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l6;
          i1 = 416u;
          i0 += i1;
          l9 = i0;
          i0 = l15;
          i1 = l7;
          i2 = c;
          i3 = 24u;
          i2 <<= (i3 & 31);
          i3 = 24u;
          i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
          i3 = 0u;
          i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
          l7 = i2;
          i0 = i2 ? i0 : i1;
          l10 = i0;
          i0 = l5;
          i1 = c;
          i2 = l7;
          i0 = i2 ? i0 : i1;
          c = i0;
        }
        i0 = l11;
        i1 = 6u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 860u;
          i1 = l9;
          i2 = l10;
          i3 = c;
          i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
          l8 = i0;
          i0 = 6u;
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
          if (i0) {goto block_50;}
          i0 = l6;
          i0 = LoadS8(ctx, i0 + 227u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          c = i0;
          if (i0) {
            i0 = l6;
            i0 = LoadU32(ctx, i0 + 216u);
            c = i0;
            DeallocateRawBlock(ctx, i0);
          }
          i0 = 1u;
          l15 = i0;
          goto block_37;
        }
        block_50:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          l5 = i0;
          i0 = HostTempRet0(ctx);
          i0 = l6;
          i0 = LoadS8(ctx, i0 + 227u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          a = i0;
          if (i0) {
            i0 = l6;
            i0 = LoadU32(ctx, i0 + 216u);
            a = i0;
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l6;
          i1 = 400u;
          i0 += i1;
          c = i0;
          goto block_6;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        c = i0;
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
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l11;
        i1 = 7u;
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
          i0 = 7u;
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
          if (i0) {goto block_5;}
          goto exit_path;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        i1 = 0u;
        StoreU8(ctx, i0 + 220u, i1);
        i0 = l6;
        i1 = kData61746164;
        StoreU32(ctx, i0 + 216u, i1);
        i0 = l6;
        i1 = 4u;
        StoreU8(ctx, i0 + 227u, i1);
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l6;
        i1 = l6;
        i2 = 216u;
        i1 += i2;
        StoreU32(ctx, i0 + 184u, i1);
        i0 = l6;
        i1 = 216u;
        i0 += i1;
        l10 = i0;
        i0 = l6;
        i1 = 184u;
        i0 += i1;
        l15 = i0;
        i0 = l6;
        i1 = 472u;
        i0 += i1;
        l9 = i0;
        i0 = l13;
        i1 = 16u;
        i0 += i1;
        l7 = i0;
        i0 = l6;
        i1 = 200u;
        i0 += i1;
        l5 = i0;
      }
      i0 = l11;
      i1 = 8u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1959u;
        i1 = l5;
        i2 = l7;
        i3 = l10;
        i4 = kDataBA360;
        i5 = l15;
        i6 = l9;
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
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l7 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l7;
        i1 = 1u;
        i0 = i0 == i1;
        l5 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        if (i0) {goto block_66;}
        i0 = l6;
        i0 = LoadU32(ctx, i0 + 200u);
        l15 = i0;
        i1 = 28u;
        i0 += i1;
        l7 = i0;
        i0 = l15;
        i0 = LoadS8(ctx, i0 + 39u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l7;
          i0 = LoadU32(ctx, i0);
          l7 = i0;
        }
        i0 = c;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i0 = LoadU32(ctx, i0 + 352u);
        l15 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l11;
      i1 = 9u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l15;
        i1 = c;
        i2 = l7;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l8 = i0;
        i0 = 9u;
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
        if (i0) {goto block_66;}
        i0 = l6;
        i0 = LoadS8(ctx, i0 + 227u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        c = i0;
        if (i0) {
          i0 = l6;
          i0 = LoadU32(ctx, i0 + 216u);
          c = i0;
          DeallocateRawBlock(ctx, i0);
        }
        i0 = 0u;
        l15 = i0;
        goto block_37;
      }
      block_66:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        l5 = i0;
        i0 = HostTempRet0(ctx);
        i0 = l6;
        i0 = LoadS8(ctx, i0 + 227u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        a = i0;
        if (i0) {
          i0 = l6;
          i0 = LoadU32(ctx, i0 + 216u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l6;
        i1 = 400u;
        i0 += i1;
        c = i0;
        goto block_6;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto exit_path;}
    block_38:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l7;
      i1 = c;
      i2 = l5;
      i3 = 24u;
      i2 <<= (i3 & 31);
      i3 = 24u;
      i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      i0 = i2 ? i0 : i1;
      i1 = 2u;
      i0 = i0 != i1;
      l9 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l9;
      if (i0) {goto block_75;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l11;
    i1 = 10u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = l10;
      i2 = 0u;
      i3 = kDataFFFFFFFF;
      i4 = kDataB9B45;
      i5 = 2u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l8 = i0;
      i0 = 10u;
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
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l7 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l7;
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
          i0 = l13;
          i0 = LoadU32(ctx, i0 + 8u);
          l7 = i0;
          i0 = l13;
          i0 = LoadU8(ctx, i0 + 15u);
          c = i0;
          l5 = i0;
          goto block_75;
        }
        i0 = b;
        i0 = LoadU32(ctx, i0);
        l21 = i0;
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
        i0 = l11;
        i1 = 11u;
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
          a = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_5;}
          goto exit_path;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l11;
      i1 = 12u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1032u;
        i1 = 16u;
        i0 = HostFoldLayoutOffset(ctx, i0, i1);
        l8 = i0;
        i0 = 12u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l8;
        l7 = i0;
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
        if (i0) {goto block_78;}
        i0 = l7;
        i1 = kDataB9B4D;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0 + 5u, j1);
        i0 = l7;
        i1 = kMsgOmitblankline;
        j1 = LoadU64(ctx, i1);
        l30 = j1;
        StoreU64(ctx, i0, j1);
        i0 = l7;
        i1 = 0u;
        StoreU8(ctx, i0 + 13u, i1);
        i0 = l13;
        i0 = LoadU32(ctx, i0 + 20u);
        c = i0;
        i0 = !(i0);
        l9 = i0;
        i0 = 1u;
        l10 = i0;
        i0 = 0u;
        l5 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      i1 = 0u;
      i2 = l9;
      i0 = i2 ? i0 : i1;
      if (i0) {goto block_97;}
      loop_98: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = c;
          i0 = LoadU8(ctx, i0 + 27u);
          l9 = i0;
          i1 = 24u;
          i0 <<= (i1 & 31);
          i1 = 24u;
          i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
          i1 = 0u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          l16 = i0;
          i0 = c;
          i0 = LoadU32(ctx, i0 + 20u);
          l14 = i0;
          i1 = l9;
          i2 = l16;
          i0 = i2 ? i0 : i1;
          l9 = i0;
          i1 = 13u;
          i0 = i0 < i1;
          l22 = i0;
          i0 = l9;
          i1 = 13u;
          i2 = l22;
          i0 = i2 ? i0 : i1;
          l17 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l17;
          if (i0) {
            i0 = l7;
            i1 = c;
            i2 = 16u;
            i1 += i2;
            l14 = i1;
            i1 = LoadU32(ctx, i1);
            i2 = l14;
            i3 = l16;
            i1 = i3 ? i1 : i2;
            l16 = i1;
            i2 = l17;
            i0 = CompareBytes(ctx, i0, i1, i2);
            l14 = i0;
            i0 = !(i0);
            if (i0) {
              i0 = l9;
              i1 = 13u;
              i0 = i0 <= i1;
              l9 = i0;
              if (i0) {goto block_107;}
              goto block_101;
            }
            i0 = l14;
            i1 = kDataFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            l9 = i0;
            if (i0) {goto block_101;}
            block_107:;
            i0 = l16;
            i1 = l7;
            i2 = l17;
            i0 = CompareBytes(ctx, i0, i1, i2);
            l9 = i0;
            i0 = !(i0);
            l14 = i0;
            if (i0) {goto block_105;}
            i0 = l9;
            i1 = kDataFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            l9 = i0;
            if (i0) {goto block_104;}
            goto block_102;
          }
          i0 = l9;
          i1 = 13u;
          i0 = i0 > i1;
          l9 = i0;
          if (i0) {goto block_101;}
          block_105:;
          i0 = l22;
          if (i0) {goto block_102;}
          block_104:;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l11;
        i1 = 13u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1032u;
          i1 = 16u;
          i0 = HostFoldLayoutOffset(ctx, i0, i1);
          l8 = i0;
          i0 = 13u;
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
          if (i0) {goto block_96;}
          i0 = l6;
          i1 = c;
          StoreU32(ctx, i0 + 384u, i1);
          i0 = l6;
          j1 = 9223372105574252557ull;
          StoreU64(ctx, i0 + 388u, j1);
          i0 = c;
          i1 = 0u;
          StoreU8(ctx, i0 + 13u, i1);
          i0 = c;
          i1 = kDataB9B4D;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0 + 5u, j1);
          i0 = c;
          i1 = kMsgOmitblankline;
          j1 = LoadU64(ctx, i1);
          l30 = j1;
          StoreU64(ctx, i0, j1);
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l6;
          i1 = l6;
          i2 = 384u;
          i1 += i2;
          StoreU32(ctx, i0 + 200u, i1);
          i0 = l13;
          i1 = 16u;
          i0 += i1;
          l5 = i0;
          i0 = l6;
          i1 = 384u;
          i0 += i1;
          l10 = i0;
          i0 = l6;
          i1 = 200u;
          i0 += i1;
          l9 = i0;
          i0 = l6;
          i1 = 184u;
          i0 += i1;
          l14 = i0;
          i0 = l6;
          i1 = 216u;
          i0 += i1;
          c = i0;
        }
        i0 = l11;
        i1 = 14u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1959u;
          i1 = c;
          i2 = l5;
          i3 = l10;
          i4 = kDataBA360;
          i5 = l9;
          i6 = l14;
          HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
          i0 = 14u;
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
          if (i0) {goto block_99;}
          i0 = HostRet0(ctx);
          l5 = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_94;
        }
        block_102:;
        i0 = c;
        i1 = c;
        i2 = 4u;
        i1 += i2;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        c = i0;
        block_101:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = c;
          i0 = LoadU32(ctx, i0);
          c = i0;
          if (i0) {goto loop_98;}
          goto block_97;
        }
        block_99:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = 0u;
        l10 = i0;
        i0 = 1u;
        l5 = i0;
        i0 = l6;
        i0 = LoadU32(ctx, i0 + 216u);
        c = i0;
        i0 = LoadU32(ctx, i0 + 32u);
        l14 = i0;
        i0 = c;
        i0 = LoadU8(ctx, i0 + 39u);
        l9 = i0;
        i1 = 24u;
        i0 <<= (i1 & 31);
        i1 = 24u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l16 = i0;
        i0 = l14;
        i1 = l9;
        i2 = l16;
        i0 = i2 ? i0 : i1;
        i1 = 1u;
        i0 = i0 != i1;
        l9 = i0;
        if (i0) {goto block_97;}
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = c;
        i1 = 28u;
        i0 += i1;
        c = i0;
      }
      i0 = l11;
      i1 = 15u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1354u;
        i1 = c;
        i2 = 0u;
        i3 = kDataFFFFFFFF;
        i4 = kDataB9B56;
        i5 = 1u;
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
        if (i0) {goto block_92;}
        i0 = c;
        i1 = 0u;
        i0 = i0 != i1;
        l5 = i0;
      }
      block_97:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l21;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 364u);
        c = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l11;
      i1 = 16u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = c;
        i1 = l21;
        i2 = l5;
        HostOsSchedule(ctx, i0, i1, i2);
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
        c = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = c;
        i1 = 1u;
        i0 = i0 == i1;
        c = i0;
        if (i0) {goto block_95;}
        i0 = l10;
        if (i0) {goto block_120;}
        i0 = l6;
        i0 = LoadS8(ctx, i0 + 395u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        c = i0;
        if (i0) {goto block_120;}
        i0 = l6;
        i0 = LoadU32(ctx, i0 + 384u);
        c = i0;
        DeallocateRawBlock(ctx, i0);
        block_120:;
        i0 = l7;
        DeallocateRawBlock(ctx, i0);
        goto block_37;
      }
      block_96:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        l5 = i0;
        i0 = HostTempRet0(ctx);
        a = i0;
        goto block_93;
      }
      block_95:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        l5 = i0;
        i0 = HostTempRet0(ctx);
        a = i0;
        i0 = l10;
        if (i0) {goto block_93;}
      }
      block_94:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        i0 = LoadS8(ctx, i0 + 395u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        a = i0;
        if (i0) {goto block_93;}
        i0 = l6;
        i0 = LoadU32(ctx, i0 + 384u);
        a = i0;
        DeallocateRawBlock(ctx, i0);
      }
      block_93:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l7;
        DeallocateRawBlock(ctx, i0);
        i0 = l6;
        i1 = 400u;
        i0 += i1;
        c = i0;
        goto block_6;
      }
      block_92:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto exit_path;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto exit_path;}
    block_78:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      l5 = i0;
      i0 = l6;
      i1 = 400u;
      i0 += i1;
      c = i0;
      i0 = HostTempRet0(ctx);
      a = i0;
      goto block_6;
    }
    block_75:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l7;
      i1 = c;
      i2 = l5;
      i3 = 24u;
      i2 <<= (i3 & 31);
      i3 = 24u;
      i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      i0 = i2 ? i0 : i1;
      i1 = kResumeWordBytes;
      i0 = i0 != i1;
      l9 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l9;
      if (i0) {goto block_127;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l11;
    i1 = 17u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = l10;
      i2 = 0u;
      i3 = kDataFFFFFFFF;
      i4 = kMsgFont;
      i5 = 4u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l8 = i0;
      i0 = 17u;
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
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l7 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l7;
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
          i0 = l13;
          i0 = LoadU32(ctx, i0 + 8u);
          l7 = i0;
          i0 = l13;
          i0 = LoadU8(ctx, i0 + 15u);
          c = i0;
          l5 = i0;
          goto block_127;
        }
        i0 = b;
        i0 = LoadU32(ctx, i0);
        c = i0;
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
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l11;
        i1 = 18u;
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
          i0 = 18u;
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
          if (i0) {goto block_5;}
          goto exit_path;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l26;
        i1 = c;
        i2 = 488u;
        i1 += i2;
        i2 = 68u;
        i0 = CopyBytes(ctx, i0, i1, i2);
        l21 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = c;
        i1 = 556u;
        i0 += i1;
        l5 = i0;
      }
      i0 = l11;
      i1 = 19u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 873u;
        i1 = l25;
        i2 = l5;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l8 = i0;
        i0 = 19u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l8;
        l17 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l7 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l7;
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
          i0 = c;
          i1 = 484u;
          i0 += i1;
          c = i0;
          i1 = 84u;
          i0 += i1;
          l5 = i0;
        }
        i0 = l11;
        i1 = 20u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 873u;
          i1 = l24;
          i2 = l5;
          i0 = HostOsQuery(ctx, i0, i1, i2);
          l8 = i0;
          i0 = 20u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l8;
          l16 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          l7 = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l7;
          i1 = 1u;
          i0 = i0 == i1;
          if (i0) {
            i0 = HostRet0(ctx);
            l5 = i0;
            i0 = HostTempRet0(ctx);
            i0 = l6;
            i0 = LoadS8(ctx, i0 + 299u);
            i1 = kDataFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            a = i0;
            if (i0) {goto block_130;}
            i0 = l6;
            i0 = LoadU32(ctx, i0 + 288u);
            a = i0;
            DeallocateRawBlock(ctx, i0);
            goto block_130;
          }
          i0 = l23;
          i1 = c;
          i2 = 96u;
          i1 += i2;
          l5 = i1;
          i2 = 66u;
          i0 = CopyBytes(ctx, i0, i1, i2);
          l22 = i0;
          i0 = l6;
          i1 = kDataB5EC4;
          StoreU32(ctx, i0 + 216u, i1);
          i0 = c;
          i0 = LoadU32(ctx, i0 + 164u);
          c = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l6;
          i1 = c;
          StoreU32(ctx, i0 + 380u, i1);
          i0 = l13;
          i1 = 16u;
          i0 += i1;
          l10 = i0;
          i0 = l6;
          i1 = 216u;
          i0 += i1;
          c = i0;
        }
        i0 = l11;
        i1 = 21u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2616u;
          i1 = c;
          i2 = l10;
          HostOsSchedule(ctx, i0, i1, i2);
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
          if (i0) {goto block_155;}
          i0 = e;
          if (i0) {
            i0 = l6;
            i1 = 0u;
            StoreU32(ctx, i0 + 224u, i1);
          }
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l11;
        i1 = 22u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1032u;
          i1 = 16u;
          i0 = HostFoldLayoutOffset(ctx, i0, i1);
          l8 = i0;
          i0 = 22u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l8;
          l7 = i0;
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
          if (i0) {goto block_132;}
          i0 = l7;
          i1 = kDataB9B5F;
          j1 = LoadU64(ctx, i1);
          l30 = j1;
          StoreU64(ctx, i0 + 7u, j1);
          i0 = l7;
          i1 = kMsgExcludeBacklog;
          j1 = LoadU64(ctx, i1);
          l31 = j1;
          StoreU64(ctx, i0, j1);
          i0 = l7;
          i1 = 0u;
          StoreU8(ctx, i0 + 15u, i1);
          i0 = l13;
          i0 = LoadU32(ctx, i0 + 20u);
          c = i0;
        }
        i0 = c;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          loop_171: 
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = c;
              i0 = LoadU8(ctx, i0 + 27u);
              l5 = i0;
              i1 = 24u;
              i0 <<= (i1 & 31);
              i1 = 24u;
              i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
              i1 = 0u;
              i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
              l14 = i0;
              i0 = c;
              i0 = LoadU32(ctx, i0 + 20u);
              i1 = l5;
              i2 = l14;
              i0 = i2 ? i0 : i1;
              l5 = i0;
              i1 = 15u;
              i0 = i0 < i1;
              l29 = i0;
              i0 = l5;
              i1 = 15u;
              i2 = l29;
              i0 = i2 ? i0 : i1;
              l9 = i0;
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = l9;
              if (i0) {
                i0 = l7;
                i1 = c;
                i2 = 16u;
                i1 += i2;
                l8 = i1;
                i1 = LoadU32(ctx, i1);
                i2 = l8;
                i3 = l14;
                i1 = i3 ? i1 : i2;
                l14 = i1;
                i2 = l9;
                i0 = CompareBytes(ctx, i0, i1, i2);
                l8 = i0;
                i0 = !(i0);
                if (i0) {
                  i0 = l5;
                  i1 = 15u;
                  i0 = i0 <= i1;
                  l5 = i0;
                  if (i0) {goto block_179;}
                  goto block_173;
                }
                i0 = l8;
                i1 = kDataFFFFFFFF;
                i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
                l5 = i0;
                if (i0) {goto block_173;}
                block_179:;
                i0 = l14;
                i1 = l7;
                i2 = l9;
                i0 = CompareBytes(ctx, i0, i1, i2);
                l5 = i0;
                i0 = !(i0);
                l9 = i0;
                if (i0) {goto block_177;}
                i0 = l5;
                i1 = kDataFFFFFFFF;
                i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
                l5 = i0;
                if (i0) {goto block_176;}
                goto block_174;
              }
              i0 = l5;
              i1 = 15u;
              i0 = i0 > i1;
              l5 = i0;
              if (i0) {goto block_173;}
              block_177:;
              i0 = l29;
              if (i0) {goto block_174;}
              block_176:;
              i0 = kDataD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
            }
            i0 = l11;
            i1 = 23u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 1032u;
              i1 = 16u;
              i0 = HostFoldLayoutOffset(ctx, i0, i1);
              l8 = i0;
              i0 = 23u;
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
              if (i0) {goto block_163;}
              i0 = l6;
              i1 = c;
              StoreU32(ctx, i0 + 200u, i1);
              i0 = l6;
              j1 = 9223372105574252559ull;
              StoreU64(ctx, i0 + 204u, j1);
              i0 = c;
              i1 = 0u;
              StoreU8(ctx, i0 + 15u, i1);
              i0 = c;
              j1 = l30;
              StoreU64(ctx, i0 + 7u, j1);
              i0 = c;
              j1 = l31;
              StoreU64(ctx, i0, j1);
              i0 = kDataD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = l6;
              i1 = l6;
              i2 = 200u;
              i1 += i2;
              StoreU32(ctx, i0 + 472u, i1);
              i0 = l6;
              i1 = 200u;
              i0 += i1;
              l5 = i0;
              i0 = l6;
              i1 = 472u;
              i0 += i1;
              l9 = i0;
              i0 = l6;
              i1 = 464u;
              i0 += i1;
              l14 = i0;
              i0 = l6;
              i1 = 184u;
              i0 += i1;
              c = i0;
            }
            i0 = l11;
            i1 = 24u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 1959u;
              i1 = c;
              i2 = l10;
              i3 = l5;
              i4 = kDataBA360;
              i5 = l9;
              i6 = l14;
              HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
              i0 = 24u;
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
              if (i0) {goto block_162;}
              i0 = l6;
              i0 = LoadU32(ctx, i0 + 184u);
              c = i0;
              i0 = LoadS8(ctx, i0 + 39u);
              l5 = i0;
              i1 = kDataFFFFFFFF;
              i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
              l9 = i0;
              if (i0) {goto block_167;}
              i0 = c;
              i0 = LoadU32(ctx, i0 + 32u);
              goto block_166;
            }
            block_174:;
            i0 = c;
            i1 = c;
            i2 = 4u;
            i1 += i2;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            c = i0;
            block_173:;
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = c;
              i0 = LoadU32(ctx, i0);
              c = i0;
              if (i0) {goto loop_171;}
            }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l7;
          DeallocateRawBlock(ctx, i0);
          goto block_165;
        }
        block_167:;
        i0 = c;
        i1 = l5;
        i2 = 255u;
        i1 &= i2;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        block_166:;
        c = i0;
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
            i0 = 1u;
            c = i0;
            goto block_187;
          }
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l11;
        i1 = 25u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1032u;
          i1 = 16u;
          i0 = HostFoldLayoutOffset(ctx, i0, i1);
          l8 = i0;
          i0 = 25u;
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
          if (i0) {goto block_161;}
          i0 = l6;
          i1 = c;
          StoreU32(ctx, i0 + 184u, i1);
          i0 = l6;
          j1 = 9223372105574252559ull;
          StoreU64(ctx, i0 + 188u, j1);
          i0 = c;
          i1 = 0u;
          StoreU8(ctx, i0 + 15u, i1);
          i0 = c;
          j1 = l30;
          StoreU64(ctx, i0 + 7u, j1);
          i0 = c;
          j1 = l31;
          StoreU64(ctx, i0, j1);
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l6;
          i1 = l6;
          i2 = 184u;
          i1 += i2;
          StoreU32(ctx, i0 + 464u, i1);
          i0 = l6;
          i1 = 184u;
          i0 += i1;
          l5 = i0;
          i0 = l6;
          i1 = 464u;
          i0 += i1;
          l9 = i0;
          i0 = l6;
          i1 = 456u;
          i0 += i1;
          l14 = i0;
          i0 = l6;
          i1 = 472u;
          i0 += i1;
          c = i0;
        }
        i0 = l11;
        i1 = 26u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1959u;
          i1 = c;
          i2 = l10;
          i3 = l5;
          i4 = kDataBA360;
          i5 = l9;
          i6 = l14;
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
          i0 = i0 == i1;
          c = i0;
          if (i0) {goto block_160;}
          i0 = l6;
          i0 = LoadU32(ctx, i0 + 472u);
          l5 = i0;
          i0 = LoadU32(ctx, i0 + 32u);
          l9 = i0;
          i0 = l5;
          i0 = LoadU8(ctx, i0 + 39u);
          l10 = i0;
          i1 = 24u;
          i0 <<= (i1 & 31);
          i1 = 24u;
          i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
          i1 = 0u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          l14 = i0;
          i0 = l9;
          i1 = l10;
          i2 = l14;
          i0 = i2 ? i0 : i1;
          i1 = 1u;
          i0 = i0 == i1;
          l10 = i0;
          i0 = 0u;
          c = i0;
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
            i0 = kDataD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l5;
            i1 = 28u;
            i0 += i1;
            c = i0;
          }
          i0 = l11;
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
            i4 = kDataB9B56;
            i5 = 1u;
            i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
            l8 = i0;
            i0 = 27u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            c = i0;
          }
          i0 = ResumePhase(ctx);
          if (i0) {
            i0 = c;
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
            l5 = i0;
            if (i0) {goto block_154;}
            i0 = c;
            i0 = !(i0);
          }
          c = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l6;
          i0 = LoadS8(ctx, i0 + 195u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          l5 = i0;
          if (i0) {goto block_187;}
          i0 = l6;
          i0 = LoadU32(ctx, i0 + 184u);
          l5 = i0;
          DeallocateRawBlock(ctx, i0);
        }
        block_187:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l6;
          i0 = LoadS8(ctx, i0 + 211u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          l5 = i0;
          if (i0) {
            i0 = l6;
            i0 = LoadU32(ctx, i0 + 200u);
            l5 = i0;
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l7;
          DeallocateRawBlock(ctx, i0);
          i0 = c;
          i0 = !(i0);
          c = i0;
          if (i0) {goto block_164;}
        }
        block_165:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = b;
          i0 = LoadU32(ctx, i0);
          c = i0;
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
            i0 = kDataD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l11;
          i1 = 28u;
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
            a = i0;
            i0 = kDataD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_5;}
            goto exit_path;
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = c;
          i1 = 488u;
          i0 += i1;
          i1 = l21;
          i2 = 68u;
          i0 = CopyBytes(ctx, i0, i1, i2);
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = c;
          i1 = 556u;
          i0 += i1;
          l5 = i0;
        }
        i0 = l11;
        i1 = 29u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 870u;
          i1 = l5;
          i2 = l17;
          i0 = HostOsQuery(ctx, i0, i1, i2);
          l8 = i0;
          i0 = 29u;
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
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          l7 = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l7;
          i1 = 1u;
          i0 = i0 == i1;
          l5 = i0;
          if (i0) {goto block_155;}
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = c;
          i1 = 568u;
          i0 += i1;
          l5 = i0;
        }
        i0 = l11;
        i1 = 30u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 870u;
          i1 = l5;
          i2 = l16;
          i0 = HostOsQuery(ctx, i0, i1, i2);
          l8 = i0;
          i0 = 30u;
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
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          l7 = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l7;
          i1 = 1u;
          i0 = i0 == i1;
          l5 = i0;
          if (i0) {goto block_155;}
          i0 = c;
          i1 = 580u;
          i0 += i1;
          i1 = l22;
          i2 = 66u;
          i0 = CopyBytes(ctx, i0, i1, i2);
          c = i0;
        }
        block_164:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l6;
          i1 = 216u;
          i0 += i1;
          c = i0;
        }
        i0 = l11;
        i1 = 31u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2617u;
          i1 = l12;
          i2 = c;
          HostOsSchedule(ctx, i0, i1, i2);
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
          c = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = c;
          i1 = 1u;
          i0 = i0 == i1;
          c = i0;
          if (i0) {goto block_155;}
          i0 = l6;
          i0 = LoadS8(ctx, i0 + 311u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l6;
            i0 = LoadU32(ctx, i0 + 300u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l6;
          i0 = LoadS8(ctx, i0 + 299u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          c = i0;
          if (i0) {goto block_37;}
          i0 = l6;
          i0 = LoadU32(ctx, i0 + 288u);
          c = i0;
          DeallocateRawBlock(ctx, i0);
          goto block_37;
        }
        block_163:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          l5 = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          i0 = l7;
          DeallocateRawBlock(ctx, i0);
          goto block_131;
        }
        block_162:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          l5 = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_159;
        }
        block_161:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          l5 = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_159;
        }
        block_160:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          l5 = i0;
          i0 = HostTempRet0(ctx);
          i0 = l6;
          i0 = LoadS8(ctx, i0 + 195u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_159;}
          i0 = l6;
          i0 = LoadU32(ctx, i0 + 184u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
        }
        block_159:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l6;
          i0 = LoadS8(ctx, i0 + 211u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          a = i0;
          if (i0) {
            i0 = l6;
            i0 = LoadU32(ctx, i0 + 200u);
            a = i0;
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l7;
          DeallocateRawBlock(ctx, i0);
          goto block_131;
        }
        block_155:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          l5 = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_131;
        }
        block_154:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto exit_path;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        l5 = i0;
        i0 = HostTempRet0(ctx);
        a = i0;
        goto block_130;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto exit_path;}
    block_132:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      l5 = i0;
      i0 = HostTempRet0(ctx);
      a = i0;
    }
    block_131:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i0 = LoadS8(ctx, i0 + 311u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l6;
        i0 = LoadU32(ctx, i0 + 300u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l6;
      i0 = LoadS8(ctx, i0 + 299u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_130;}
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 288u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
    }
    block_130:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = 400u;
      i0 += i1;
      c = i0;
      goto block_6;
    }
    block_127:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l7;
      i1 = c;
      i2 = l5;
      i3 = 24u;
      i2 <<= (i3 & 31);
      i3 = 24u;
      i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      i0 = i2 ? i0 : i1;
      i1 = 5u;
      i0 = i0 != i1;
      l9 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l9;
      if (i0) {goto block_229;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l11;
    i1 = 32u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = l10;
      i2 = 0u;
      i3 = kDataFFFFFFFF;
      i4 = kMsgFontX;
      i5 = 5u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l8 = i0;
      i0 = 32u;
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
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l7 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l7;
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
          i0 = l13;
          i0 = LoadU32(ctx, i0 + 8u);
          l7 = i0;
          i0 = l13;
          i0 = LoadU8(ctx, i0 + 15u);
          c = i0;
          l5 = i0;
          goto block_229;
        }
        i0 = l12;
        i0 = LoadU32(ctx, i0 + 20u);
        c = i0;
        i1 = 2u;
        i0 = i0 < i1;
        l5 = i0;
        if (i0) {goto block_37;}
        i0 = l12;
        i0 = LoadU32(ctx, i0 + 16u);
        i1 = c;
        i0 += i1;
        i1 = 1u;
        i0 -= i1;
        c = i0;
        i1 = 24u;
        i0 = DIV_U(i0, i1);
        l7 = i0;
        i1 = 2u;
        i0 <<= (i1 & 31);
        l10 = i0;
        i1 = l12;
        i1 = LoadU32(ctx, i1 + 4u);
        i0 += i1;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i0 = l5;
        i1 = c;
        i2 = l7;
        i3 = 24u;
        i2 *= i3;
        l7 = i2;
        i1 -= i2;
        i2 = 168u;
        i1 *= i2;
        i0 += i1;
        c = i0;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0);
        l5 = i0;
      }
      i0 = l11;
      i1 = 33u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = c;
        i1 = l5;
        i0 = CallTableT2(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        l8 = i0;
        i0 = 33u;
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
        i0 = l12;
        i1 = l12;
        i1 = LoadU32(ctx, i1 + 20u);
        c = i1;
        i2 = 1u;
        i1 -= i2;
        l10 = i1;
        StoreU32(ctx, i0 + 20u, i1);
        i0 = l12;
        i0 = LoadU32(ctx, i0 + 8u);
        l9 = i0;
        i1 = l12;
        i1 = LoadU32(ctx, i1 + 4u);
        l5 = i1;
        i0 -= i1;
        l7 = i0;
        i1 = 2u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        i1 = 24u;
        i0 *= i1;
        i1 = 1u;
        i0 -= i1;
        i1 = 0u;
        i2 = l7;
        i0 = i2 ? i0 : i1;
        l14 = i0;
        i1 = l12;
        i1 = LoadU32(ctx, i1 + 16u);
        l7 = i1;
        i2 = c;
        i1 += i2;
        i0 -= i1;
        i1 = 1u;
        i0 += i1;
        i1 = 48u;
        i0 = i0 >= i1;
        if (i0) {
          i0 = l9;
          i1 = 4u;
          i0 -= i1;
          i0 = LoadU32(ctx, i0);
          DeallocateRawBlock(ctx, i0);
          i0 = l12;
          i1 = l12;
          i1 = LoadU32(ctx, i1 + 8u);
          i2 = 4u;
          i1 -= i2;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l12;
          i0 = LoadU32(ctx, i0 + 4u);
          l5 = i0;
          i0 = l12;
          i0 = LoadU32(ctx, i0 + 20u);
          l10 = i0;
          i0 = l12;
          i0 = LoadU32(ctx, i0 + 16u);
        } else {
          i0 = l7;
        }
        i1 = l10;
        i0 += i1;
        i1 = 1u;
        i0 -= i1;
        l10 = i0;
        i1 = 24u;
        i0 = DIV_U(i0, i1);
        l7 = i0;
        i0 = b;
        i0 = LoadU32(ctx, i0);
        c = i0;
        i0 = !(i0);
        l9 = i0;
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
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l11;
        i1 = 34u;
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
          i0 = 34u;
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
          if (i0) {goto block_5;}
          goto exit_path;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = c;
        i1 = 488u;
        i0 += i1;
        l9 = i0;
        i1 = l7;
        i2 = 2u;
        i1 <<= (i2 & 31);
        l14 = i1;
        i2 = l5;
        i1 += i2;
        i1 = LoadU32(ctx, i1);
        i2 = l10;
        i3 = l7;
        i4 = 24u;
        i3 *= i4;
        i2 -= i3;
        i3 = 168u;
        i2 *= i3;
        i1 += i2;
        l7 = i1;
        i2 = 4u;
        i1 += i2;
        i2 = 68u;
        i0 = CopyBytes(ctx, i0, i1, i2);
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l7;
        i1 = 72u;
        i0 += i1;
        l10 = i0;
        i0 = c;
        i1 = 556u;
        i0 += i1;
        l5 = i0;
      }
      i0 = l11;
      i1 = 35u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 870u;
        i1 = l5;
        i2 = l10;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l8 = i0;
        i0 = 35u;
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
        if (i0) {goto block_33;}
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l7;
        i1 = 84u;
        i0 += i1;
        l10 = i0;
        i0 = c;
        i1 = 568u;
        i0 += i1;
        l5 = i0;
      }
      i0 = l11;
      i1 = 36u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 870u;
        i1 = l5;
        i2 = l10;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l8 = i0;
        i0 = 36u;
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
        if (i0) {goto block_33;}
        i0 = c;
        i1 = 580u;
        i0 += i1;
        i1 = l7;
        i2 = 96u;
        i1 += i2;
        l5 = i1;
        i2 = 66u;
        i0 = CopyBytes(ctx, i0, i1, i2);
        c = i0;
        goto block_37;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto exit_path;}
    block_229:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l7;
      i1 = c;
      i2 = l5;
      i3 = 24u;
      i2 <<= (i3 & 31);
      i3 = 24u;
      i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      i0 = i2 ? i0 : i1;
      i1 = kResumeWordBytes;
      i0 = i0 != i1;
      l9 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l9;
      if (i0) {goto block_250;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l11;
    i1 = 37u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = l10;
      i2 = 0u;
      i3 = kDataFFFFFFFF;
      i4 = kMsgRuby;
      i5 = 4u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l8 = i0;
      i0 = 37u;
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
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l7 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l7;
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
          i0 = l13;
          i0 = LoadU32(ctx, i0 + 8u);
          l7 = i0;
          i0 = l13;
          i0 = LoadU8(ctx, i0 + 15u);
          c = i0;
          l5 = i0;
          goto block_250;
        }
        i0 = l15;
        if (i0) {
          i0 = 1u;
          l15 = i0;
          goto block_37;
        }
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l6;
        i1 = 416u;
        i0 += i1;
        c = i0;
      }
      i0 = l11;
      i1 = 38u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 996u;
        i1 = c;
        i2 = kDataB9B6D;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l8 = i0;
        i0 = 38u;
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
        if (i0) {goto block_33;}
        i0 = l6;
        i1 = 0u;
        StoreU8(ctx, i0 + 220u, i1);
        i0 = l6;
        i1 = kData74786574;
        StoreU32(ctx, i0 + 216u, i1);
        i0 = l6;
        i1 = 4u;
        StoreU8(ctx, i0 + 227u, i1);
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l6;
        i1 = l6;
        i2 = 216u;
        i1 += i2;
        StoreU32(ctx, i0 + 184u, i1);
        i0 = l13;
        i1 = 16u;
        i0 += i1;
        l5 = i0;
        i0 = l6;
        i1 = 216u;
        i0 += i1;
        l7 = i0;
        i0 = l6;
        i1 = 184u;
        i0 += i1;
        l10 = i0;
        i0 = l6;
        i1 = 472u;
        i0 += i1;
        l15 = i0;
        i0 = l6;
        i1 = 200u;
        i0 += i1;
        c = i0;
      }
      i0 = l11;
      i1 = 39u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1959u;
        i1 = c;
        i2 = l5;
        i3 = l7;
        i4 = kDataBA360;
        i5 = l10;
        i6 = l15;
        HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
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
        if (i0) {goto block_263;}
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l6;
        i0 = LoadU32(ctx, i0 + 200u);
        i1 = 28u;
        i0 += i1;
        l5 = i0;
        i0 = l6;
        i1 = 400u;
        i0 += i1;
        c = i0;
      }
      i0 = l11;
      i1 = 40u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 870u;
        i1 = c;
        i2 = l5;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l8 = i0;
        i0 = 40u;
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
        if (i0) {goto block_263;}
        i0 = l6;
        i0 = LoadS8(ctx, i0 + 227u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        c = i0;
        if (i0) {
          i0 = l6;
          i0 = LoadU32(ctx, i0 + 216u);
          c = i0;
          DeallocateRawBlock(ctx, i0);
        }
        i0 = 1u;
        l15 = i0;
        goto block_37;
      }
      block_263:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        l5 = i0;
        i0 = HostTempRet0(ctx);
        i0 = l6;
        i0 = LoadS8(ctx, i0 + 227u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        a = i0;
        if (i0) {
          i0 = l6;
          i0 = LoadU32(ctx, i0 + 216u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l6;
        i1 = 400u;
        i0 += i1;
        c = i0;
        goto block_6;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto exit_path;}
    block_250:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l7;
      i1 = c;
      i2 = l5;
      i3 = 24u;
      i2 <<= (i3 & 31);
      i3 = 24u;
      i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      i0 = i2 ? i0 : i1;
      i1 = 5u;
      i0 = i0 != i1;
      l9 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l9;
      if (i0) {goto block_271;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l11;
    i1 = 41u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = l10;
      i2 = 0u;
      i3 = kDataFFFFFFFF;
      i4 = kMsgRubyX;
      i5 = 5u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l8 = i0;
      i0 = 41u;
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
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l7 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l7;
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
          i0 = l13;
          i0 = LoadU32(ctx, i0 + 8u);
          l7 = i0;
          i0 = l13;
          i0 = LoadU8(ctx, i0 + 15u);
          c = i0;
          l5 = i0;
          goto block_271;
        }
        i0 = l15;
        c = i0;
        i0 = 0u;
        l15 = i0;
        i0 = c;
        i0 = !(i0);
        c = i0;
        if (i0) {goto block_37;}
        i0 = b;
        i0 = LoadU32(ctx, i0);
        c = i0;
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
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l11;
        i1 = 42u;
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
          a = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_5;}
          goto exit_path;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = c;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 360u);
        l7 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l6;
        i0 = LoadU32(ctx, i0 + 400u);
        i1 = l6;
        i2 = 400u;
        i1 += i2;
        l9 = i1;
        i2 = l6;
        i2 = LoadS8(ctx, i2 + 411u);
        i3 = 0u;
        i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
        l14 = i2;
        i0 = i2 ? i0 : i1;
        l10 = i0;
        i0 = l6;
        i0 = LoadU32(ctx, i0 + 416u);
        i1 = l6;
        i2 = 416u;
        i1 += i2;
        i2 = l6;
        i2 = LoadS8(ctx, i2 + 427u);
        i3 = 0u;
        i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
        i0 = i2 ? i0 : i1;
        l5 = i0;
      }
      i0 = l11;
      i1 = 43u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l7;
        i1 = c;
        i2 = l5;
        i3 = l10;
        i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
        l8 = i0;
        i0 = 43u;
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
        if (i0) {goto block_37;}
        goto block_33;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto exit_path;}
    block_271:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l7;
      i1 = c;
      i2 = l5;
      i3 = 24u;
      i2 <<= (i3 & 31);
      i3 = 24u;
      i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      i0 = i2 ? i0 : i1;
      i1 = 5u;
      i0 = i0 != i1;
      l9 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l9;
      if (i0) {goto block_288;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l11;
    i1 = 44u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = l10;
      i2 = 0u;
      i3 = kDataFFFFFFFF;
      i4 = kMsgVoice;
      i5 = 5u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l8 = i0;
      i0 = 44u;
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
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l7 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l7;
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
          i0 = l13;
          i0 = LoadU32(ctx, i0 + 8u);
          l7 = i0;
          i0 = l13;
          i0 = LoadU8(ctx, i0 + 15u);
          c = i0;
          l5 = i0;
          goto block_288;
        }
        i0 = b;
        i0 = LoadU32(ctx, i0);
        c = i0;
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
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l11;
        i1 = 45u;
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
          a = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_5;}
          goto exit_path;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l26;
        i1 = c;
        i2 = 488u;
        i1 += i2;
        i2 = 68u;
        i0 = CopyBytes(ctx, i0, i1, i2);
        l7 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = c;
        i1 = 556u;
        i0 += i1;
        l5 = i0;
      }
      i0 = l11;
      i1 = 46u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 873u;
        i1 = l25;
        i2 = l5;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l8 = i0;
        i0 = 46u;
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
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l10 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l10;
        i1 = 1u;
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
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = c;
          i1 = 484u;
          i0 += i1;
          c = i0;
          i1 = 84u;
          i0 += i1;
          l10 = i0;
        }
        i0 = l11;
        i1 = 47u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 873u;
          i1 = l24;
          i2 = l10;
          i0 = HostOsQuery(ctx, i0, i1, i2);
          l8 = i0;
          i0 = 47u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l8;
          l10 = i0;
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
          if (i0) {
            i0 = HostRet0(ctx);
            l5 = i0;
            i0 = HostTempRet0(ctx);
            i0 = l6;
            i0 = LoadS8(ctx, i0 + 299u);
            i1 = kDataFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            a = i0;
            if (i0) {goto block_304;}
            i0 = l6;
            i0 = LoadU32(ctx, i0 + 288u);
            a = i0;
            DeallocateRawBlock(ctx, i0);
            goto block_304;
          }
          i0 = l23;
          i1 = c;
          i2 = 96u;
          i1 += i2;
          i2 = 66u;
          i0 = CopyBytes(ctx, i0, i1, i2);
          l9 = i0;
          i0 = l6;
          i1 = kDataB5EC4;
          StoreU32(ctx, i0 + 216u, i1);
          i0 = c;
          i0 = LoadU32(ctx, i0 + 164u);
          c = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l6;
          i1 = c;
          StoreU32(ctx, i0 + 380u, i1);
          i0 = l13;
          i1 = 16u;
          i0 += i1;
          l14 = i0;
          i0 = l6;
          i1 = 216u;
          i0 += i1;
          c = i0;
        }
        i0 = l11;
        i1 = 48u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2616u;
          i1 = c;
          i2 = l14;
          HostOsSchedule(ctx, i0, i1, i2);
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
          c = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = c;
          i1 = 1u;
          i0 = i0 == i1;
          c = i0;
          if (i0) {goto block_311;}
          i0 = b;
          i0 = LoadU32(ctx, i0);
          c = i0;
          i0 = !(i0);
          l14 = i0;
        }
        i0 = l14;
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
          i0 = l11;
          i1 = 49u;
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
            a = i0;
            i0 = kDataD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_5;}
            goto exit_path;
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = c;
          i1 = 488u;
          i0 += i1;
          l14 = i0;
          i1 = l7;
          i2 = 68u;
          i0 = CopyBytes(ctx, i0, i1, i2);
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = c;
          i1 = 556u;
          i0 += i1;
          l7 = i0;
        }
        i0 = l11;
        i1 = 50u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 870u;
          i1 = l7;
          i2 = l5;
          i0 = HostOsQuery(ctx, i0, i1, i2);
          l8 = i0;
          i0 = 50u;
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
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          l7 = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l7;
          i1 = 1u;
          i0 = i0 == i1;
          l5 = i0;
          if (i0) {goto block_311;}
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = c;
          i1 = 568u;
          i0 += i1;
          l5 = i0;
        }
        i0 = l11;
        i1 = 51u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 870u;
          i1 = l5;
          i2 = l10;
          i0 = HostOsQuery(ctx, i0, i1, i2);
          l8 = i0;
          i0 = 51u;
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
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          l7 = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l7;
          i1 = 1u;
          i0 = i0 == i1;
          l5 = i0;
          if (i0) {goto block_311;}
          i0 = c;
          i1 = 580u;
          i0 += i1;
          i1 = l9;
          i2 = 66u;
          i0 = CopyBytes(ctx, i0, i1, i2);
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l6;
          i1 = 216u;
          i0 += i1;
          c = i0;
        }
        i0 = l11;
        i1 = 52u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2617u;
          i1 = l12;
          i2 = c;
          HostOsSchedule(ctx, i0, i1, i2);
          i0 = 52u;
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
          if (i0) {goto block_311;}
          i0 = b;
          i0 = LoadU32(ctx, i0);
          c = i0;
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
            i0 = kDataD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l11;
          i1 = 53u;
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
            a = i0;
            i0 = kDataD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_5;}
            goto exit_path;
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l6;
          i1 = 56u;
          i0 += i1;
          l5 = i0;
        }
        i0 = l11;
        i1 = 54u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2591u;
          i1 = l5;
          i2 = l13;
          i0 = HostOsQuery(ctx, i0, i1, i2);
          l8 = i0;
          i0 = 54u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l8;
          l7 = i0;
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
          if (i0) {goto block_311;}
          i0 = b;
          i0 = LoadU32(ctx, i0);
          l5 = i0;
          i0 = l6;
          i1 = 0u;
          StoreU32(ctx, i0 + 52u, i1);
          i0 = l6;
          i1 = 3232u;
          StoreU32(ctx, i0 + 48u, i1);
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l6;
          i1 = l6;
          j1 = LoadU64(ctx, i1 + 48u);
          l30 = j1;
          StoreU64(ctx, i0 + 8u, j1);
          i0 = l6;
          i1 = 8u;
          i0 += i1;
          l9 = i0;
          i0 = l6;
          i1 = 104u;
          i0 += i1;
          l10 = i0;
        }
        i0 = l11;
        i1 = 55u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 3233u;
          i1 = l10;
          i2 = l9;
          i3 = a;
          i4 = l7;
          i5 = l5;
          HostInitResources(ctx, i0, i1, i2, i3, i4, i5);
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
          l7 = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l7;
          i1 = 1u;
          i0 = i0 == i1;
          l5 = i0;
          if (i0) {goto block_310;}
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l6;
          i1 = 104u;
          i0 += i1;
          l7 = i0;
          i0 = l6;
          i1 = 168u;
          i0 += i1;
          l5 = i0;
        }
        i0 = l11;
        i1 = 56u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 3234u;
          i1 = l5;
          i2 = l7;
          i3 = 0u;
          i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
          l8 = i0;
          i0 = 56u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l8;
          l7 = i0;
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
          i0 = l6;
          i1 = 0u;
          StoreU32(ctx, i0 + 16u, i1);
          i0 = l6;
          i1 = 0u;
          StoreU32(ctx, i0 + 32u, i1);
          i0 = c;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 424u);
          l5 = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l6;
          i1 = 16u;
          i0 += i1;
          l9 = i0;
          i0 = l6;
          i1 = 32u;
          i0 += i1;
          l10 = i0;
        }
        i0 = l11;
        i1 = 57u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l5;
          i1 = c;
          i2 = l7;
          i3 = l10;
          i4 = l9;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
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
          c = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = c;
          i1 = 1u;
          i0 = i0 == i1;
          c = i0;
          if (i0) {goto block_309;}
          i0 = l6;
          i0 = LoadU32(ctx, i0 + 16u);
          c = i0;
        }
        i0 = c;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = l5;
          i1 = c;
          i2 = 1u;
          i1 &= i2;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          l5 = i0;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l5;
            if (i0) {goto block_349;}
            i0 = c;
            i1 = kDataFFFFFFFE;
            i0 &= i1;
            i0 = LoadU32(ctx, i0);
            c = i0;
            i0 = !(i0);
            l5 = i0;
            if (i0) {goto block_349;}
            i0 = kDataD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l11;
          i1 = 58u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = c;
            i1 = l20;
            i2 = l20;
            i3 = 2u;
            HostSyncSlot(ctx, i0, i1, i2, i3);
            i0 = 58u;
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
          block_349:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l6;
            i1 = 0u;
            StoreU32(ctx, i0 + 16u, i1);
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l6;
          i0 = LoadU32(ctx, i0 + 32u);
          c = i0;
        }
        i0 = c;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = l5;
          i1 = c;
          i2 = 1u;
          i1 &= i2;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          l5 = i0;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l5;
            if (i0) {goto block_357;}
            i0 = c;
            i1 = kDataFFFFFFFE;
            i0 &= i1;
            i0 = LoadU32(ctx, i0);
            c = i0;
            i0 = !(i0);
            l5 = i0;
            if (i0) {goto block_357;}
            i0 = kDataD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l11;
          i1 = 59u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = c;
            i1 = l19;
            i2 = l19;
            i3 = 2u;
            HostSyncSlot(ctx, i0, i1, i2, i3);
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
          block_357:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l6;
            i1 = 0u;
            StoreU32(ctx, i0 + 32u, i1);
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l6;
          i0 = LoadU32(ctx, i0 + 168u);
          c = i0;
        }
        i0 = c;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = l5;
          i1 = c;
          i2 = 1u;
          i1 &= i2;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          l5 = i0;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l5;
            if (i0) {goto block_365;}
            i0 = c;
            i1 = kDataFFFFFFFE;
            i0 &= i1;
            i0 = LoadU32(ctx, i0);
            c = i0;
            i0 = !(i0);
            l5 = i0;
            if (i0) {goto block_365;}
            i0 = kDataD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l11;
          i1 = 60u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = c;
            i1 = l18;
            i2 = l18;
            i3 = 2u;
            HostSyncSlot(ctx, i0, i1, i2, i3);
            i0 = 60u;
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
          block_365:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l6;
            i1 = 0u;
            StoreU32(ctx, i0 + 168u, i1);
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l6;
          i1 = kDataB5F40;
          StoreU32(ctx, i0 + 116u, i1);
          i0 = l6;
          i0 = LoadS8(ctx, i0 + 155u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l6;
            i0 = LoadU32(ctx, i0 + 144u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l28;
          i1 = l6;
          i1 = LoadU32(ctx, i1 + 136u);
          ReleaseNodeTree(ctx, i0, i1);
          i0 = l6;
          i0 = LoadS8(ctx, i0 + 131u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l6;
            i0 = LoadU32(ctx, i0 + 120u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l6;
          i1 = kDataB5F40;
          StoreU32(ctx, i0 + 56u, i1);
          i0 = l6;
          i0 = LoadS8(ctx, i0 + 95u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l6;
            i0 = LoadU32(ctx, i0 + 84u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l27;
          i1 = l6;
          i1 = LoadU32(ctx, i1 + 76u);
          ReleaseNodeTree(ctx, i0, i1);
          i0 = l6;
          i0 = LoadS8(ctx, i0 + 71u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l6;
            i0 = LoadU32(ctx, i0 + 60u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l6;
          i0 = LoadS8(ctx, i0 + 311u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l6;
            i0 = LoadU32(ctx, i0 + 300u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l6;
          i0 = LoadS8(ctx, i0 + 299u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          c = i0;
          if (i0) {goto block_37;}
          i0 = l6;
          i0 = LoadU32(ctx, i0 + 288u);
          c = i0;
          DeallocateRawBlock(ctx, i0);
          goto block_37;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        l5 = i0;
        i0 = HostTempRet0(ctx);
        a = i0;
        goto block_304;
      }
      block_311:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        l5 = i0;
        i0 = HostTempRet0(ctx);
        a = i0;
        goto block_305;
      }
      block_310:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        l5 = i0;
        i0 = HostTempRet0(ctx);
        a = i0;
        goto block_306;
      }
      block_309:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        l5 = i0;
        i0 = HostTempRet0(ctx);
        i0 = l6;
        i0 = LoadU32(ctx, i0 + 16u);
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
          if (i0) {goto block_381;}
          i0 = a;
          i1 = kDataFFFFFFFE;
          i0 &= i1;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = !(i0);
          b = i0;
          if (i0) {goto block_381;}
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l11;
        i1 = 61u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = a;
          i1 = l20;
          i2 = l20;
          i3 = 2u;
          HostSyncSlot(ctx, i0, i1, i2, i3);
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
        block_381:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l6;
          i1 = 0u;
          StoreU32(ctx, i0 + 16u, i1);
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        i0 = LoadU32(ctx, i0 + 32u);
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
          if (i0) {goto block_389;}
          i0 = a;
          i1 = kDataFFFFFFFE;
          i0 &= i1;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = !(i0);
          b = i0;
          if (i0) {goto block_389;}
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l11;
        i1 = 62u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = a;
          i1 = l19;
          i2 = l19;
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
          if (i0) {goto block_291;}
        }
        block_389:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l6;
          i1 = 0u;
          StoreU32(ctx, i0 + 32u, i1);
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        i0 = LoadU32(ctx, i0 + 168u);
        a = i0;
        i0 = !(i0);
        b = i0;
        if (i0) {goto block_307;}
        i0 = a;
        i1 = 1u;
        i0 &= i1;
        b = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        if (i0) {goto block_396;}
        i0 = a;
        i1 = kDataFFFFFFFE;
        i0 &= i1;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = !(i0);
        b = i0;
        if (i0) {goto block_396;}
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l11;
      i1 = 63u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l18;
        i2 = l18;
        i3 = 2u;
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
        a = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 == i1;
        a = i0;
        if (i0) {goto block_395;}
      }
      block_396:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        i1 = 0u;
        StoreU32(ctx, i0 + 168u, i1);
        goto block_307;
      }
      block_395:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto exit_path;}
      block_308:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        l5 = i0;
        i0 = HostTempRet0(ctx);
        a = i0;
      }
      block_307:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        i1 = kDataB5F40;
        StoreU32(ctx, i0 + 116u, i1);
        i0 = l6;
        i0 = LoadS8(ctx, i0 + 155u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l6;
          i0 = LoadU32(ctx, i0 + 144u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l28;
        i1 = l6;
        i1 = LoadU32(ctx, i1 + 136u);
        ReleaseNodeTree(ctx, i0, i1);
        i0 = l6;
        i0 = LoadS8(ctx, i0 + 131u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        a = i0;
        if (i0) {goto block_306;}
        i0 = l6;
        i0 = LoadU32(ctx, i0 + 120u);
        a = i0;
        DeallocateRawBlock(ctx, i0);
      }
      block_306:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        i1 = kDataB5F40;
        StoreU32(ctx, i0 + 56u, i1);
        i0 = l6;
        i0 = LoadS8(ctx, i0 + 95u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l6;
          i0 = LoadU32(ctx, i0 + 84u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l27;
        i1 = l6;
        i1 = LoadU32(ctx, i1 + 76u);
        ReleaseNodeTree(ctx, i0, i1);
        i0 = l6;
        i0 = LoadS8(ctx, i0 + 71u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        a = i0;
        if (i0) {goto block_305;}
        i0 = l6;
        i0 = LoadU32(ctx, i0 + 60u);
        a = i0;
        DeallocateRawBlock(ctx, i0);
      }
      block_305:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        i0 = LoadS8(ctx, i0 + 311u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l6;
          i0 = LoadU32(ctx, i0 + 300u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l6;
        i0 = LoadS8(ctx, i0 + 299u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        a = i0;
        if (i0) {goto block_304;}
        i0 = l6;
        i0 = LoadU32(ctx, i0 + 288u);
        a = i0;
        DeallocateRawBlock(ctx, i0);
      }
      block_304:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        i1 = 400u;
        i0 += i1;
        c = i0;
        goto block_6;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto exit_path;}
    block_291:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto exit_path;}
    block_288:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l7;
      i1 = c;
      i2 = l5;
      i3 = 24u;
      i2 <<= (i3 & 31);
      i3 = 24u;
      i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      l5 = i2;
      i0 = i2 ? i0 : i1;
      i1 = 6u;
      i0 = i0 != i1;
      c = i0;
      if (i0) {goto block_37;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l11;
    i1 = 64u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = l10;
      i2 = 0u;
      i3 = kDataFFFFFFFF;
      i4 = kMsgVoiceX;
      i5 = 6u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l8 = i0;
      i0 = 64u;
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
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l7 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l7;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_35;}
      i0 = c;
      if (i0) {goto block_37;}
      i0 = b;
      i0 = LoadU32(ctx, i0);
      c = i0;
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l11;
      i1 = 65u;
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
        a = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_5;}
        goto exit_path;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = c;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = LoadU32(ctx, i0 + 432u);
      l7 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l11;
    i1 = 66u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l7;
      i1 = c;
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
      if (i0) {goto block_33;}
      i0 = l12;
      i0 = LoadU32(ctx, i0 + 20u);
      c = i0;
      i1 = 2u;
      i0 = i0 < i1;
      l5 = i0;
      if (i0) {goto block_37;}
      i0 = l12;
      i0 = LoadU32(ctx, i0 + 16u);
      i1 = c;
      i0 += i1;
      i1 = 1u;
      i0 -= i1;
      c = i0;
      i1 = 24u;
      i0 = DIV_U(i0, i1);
      l7 = i0;
      i1 = 2u;
      i0 <<= (i1 & 31);
      l10 = i0;
      i1 = l12;
      i1 = LoadU32(ctx, i1 + 4u);
      i0 += i1;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = l5;
      i1 = c;
      i2 = l7;
      i3 = 24u;
      i2 *= i3;
      l7 = i2;
      i1 -= i2;
      i2 = 168u;
      i1 *= i2;
      i0 += i1;
      c = i0;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0);
      l5 = i0;
    }
    i0 = l11;
    i1 = 67u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l5;
      i0 = CallTableT2(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l8 = i0;
      i0 = 67u;
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
      i0 = l12;
      i1 = l12;
      i1 = LoadU32(ctx, i1 + 20u);
      c = i1;
      i2 = 1u;
      i1 -= i2;
      l10 = i1;
      StoreU32(ctx, i0 + 20u, i1);
      i0 = l12;
      i0 = LoadU32(ctx, i0 + 8u);
      l9 = i0;
      i1 = l12;
      i1 = LoadU32(ctx, i1 + 4u);
      l5 = i1;
      i0 -= i1;
      l7 = i0;
      i1 = 2u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 24u;
      i0 *= i1;
      i1 = 1u;
      i0 -= i1;
      i1 = 0u;
      i2 = l7;
      i0 = i2 ? i0 : i1;
      l14 = i0;
      i1 = l12;
      i1 = LoadU32(ctx, i1 + 16u);
      l7 = i1;
      i2 = c;
      i1 += i2;
      i0 -= i1;
      i1 = 1u;
      i0 += i1;
      i1 = 48u;
      i0 = i0 >= i1;
      if (i0) {
        i0 = l9;
        i1 = 4u;
        i0 -= i1;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
        i0 = l12;
        i1 = l12;
        i1 = LoadU32(ctx, i1 + 8u);
        i2 = 4u;
        i1 -= i2;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l12;
        i0 = LoadU32(ctx, i0 + 4u);
        l5 = i0;
        i0 = l12;
        i0 = LoadU32(ctx, i0 + 20u);
        l10 = i0;
        i0 = l12;
        i0 = LoadU32(ctx, i0 + 16u);
      } else {
        i0 = l7;
      }
      i1 = l10;
      i0 += i1;
      i1 = 1u;
      i0 -= i1;
      l10 = i0;
      i1 = 24u;
      i0 = DIV_U(i0, i1);
      l7 = i0;
      i0 = b;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = !(i0);
      l9 = i0;
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l11;
      i1 = 68u;
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
        i0 = 68u;
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
        if (i0) {goto block_5;}
        goto exit_path;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = c;
      i1 = 488u;
      i0 += i1;
      l9 = i0;
      i1 = l7;
      i2 = 2u;
      i1 <<= (i2 & 31);
      l14 = i1;
      i2 = l5;
      i1 += i2;
      i1 = LoadU32(ctx, i1);
      i2 = l10;
      i3 = l7;
      i4 = 24u;
      i3 *= i4;
      i2 -= i3;
      i3 = 168u;
      i2 *= i3;
      i1 += i2;
      l7 = i1;
      i2 = 4u;
      i1 += i2;
      i2 = 68u;
      i0 = CopyBytes(ctx, i0, i1, i2);
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l7;
      i1 = 72u;
      i0 += i1;
      l10 = i0;
      i0 = c;
      i1 = 556u;
      i0 += i1;
      l5 = i0;
    }
    i0 = l11;
    i1 = 69u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 870u;
      i1 = l5;
      i2 = l10;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l8 = i0;
      i0 = 69u;
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
      if (i0) {goto block_33;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l7;
      i1 = 84u;
      i0 += i1;
      l10 = i0;
      i0 = c;
      i1 = 568u;
      i0 += i1;
      l5 = i0;
    }
    i0 = l11;
    i1 = 70u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 870u;
      i1 = l5;
      i2 = l10;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l8 = i0;
      i0 = 70u;
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
      if (i0) {goto block_33;}
      i0 = c;
      i1 = 580u;
      i0 += i1;
      i1 = l7;
      i2 = 96u;
      i1 += i2;
      l5 = i1;
      i2 = 66u;
      i0 = CopyBytes(ctx, i0, i1, i2);
      c = i0;
    }
    block_37:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l13;
      i1 = 44u;
      i0 += i1;
      l13 = i0;
      i1 = d;
      i1 = LoadU32(ctx, i1 + 4u);
      i0 = i0 != i1;
      c = i0;
      if (i0) {goto loop_34;}
      goto block_31;
    }
    block_35:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto exit_path;}
  block_33:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l5 = i0;
    i0 = l6;
    i1 = 400u;
    i0 += i1;
    c = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_6;
  }
  block_31:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = !(i0);
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
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l11;
    i1 = 71u;
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
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_5;}
      goto exit_path;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 388u);
    b = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l11;
  i1 = 72u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = a;
    HostReleaseSlotPair(ctx, i0, i1);
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
    a = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 != i1;
    a = i0;
    if (i0) {goto block_16;}
  }
  block_17:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l5 = i0;
    i0 = l6;
    i1 = 400u;
    i0 += i1;
    c = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_6;
  }
  block_16:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i0 = LoadS8(ctx, i0 + 411u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 400u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l6;
    i0 = LoadS8(ctx, i0 + 427u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 416u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l12;
    i0 = LoadU32(ctx, i0 + 4u);
    c = i0;
    i1 = l12;
    i1 = LoadU32(ctx, i1 + 8u);
    l7 = i1;
    i0 = i0 == i1;
    d = i0;
    i0 = l12;
    i0 = LoadU32(ctx, i0 + 16u);
    a = i0;
    i1 = 24u;
    i0 = DIV_U(i0, i1);
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = d;
    if (i0) {
      i0 = c;
      l7 = i0;
      goto block_447;
    }
    i0 = b;
    i1 = 2u;
    i0 <<= (i1 & 31);
    i1 = c;
    i0 += i1;
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    i1 = a;
    i2 = b;
    i3 = 24u;
    i2 *= i3;
    i1 -= i2;
    i2 = 168u;
    i1 *= i2;
    i0 += i1;
    l13 = i0;
    i0 = l12;
    i0 = LoadU32(ctx, i0 + 20u);
    i1 = a;
    i0 += i1;
    a = i0;
    i1 = 24u;
    i0 = DIV_U(i0, i1);
    b = i0;
    i1 = 2u;
    i0 <<= (i1 & 31);
    i1 = c;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    d = i0;
    i0 = l13;
    i1 = d;
    i2 = a;
    i3 = b;
    i4 = 24u;
    i3 *= i4;
    i2 -= i3;
    i3 = 168u;
    i2 *= i3;
    i1 += i2;
    a = i1;
    i0 = i0 == i1;
    b = i0;
    if (i0) {goto block_447;}
  }
  loop_450: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l13;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0);
      b = i0;
    }
    i0 = l11;
    i1 = 73u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l13;
      i1 = b;
      i0 = CallTableT2(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l8 = i0;
      i0 = 73u;
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
      i0 = l13;
      i1 = 168u;
      i0 += i1;
      l13 = i0;
      i1 = l5;
      i1 = LoadU32(ctx, i1);
      i0 -= i1;
      i1 = 4032u;
      i0 = i0 == i1;
      if (i0) {
        i0 = l5;
        i0 = LoadU32(ctx, i0 + 4u);
        l13 = i0;
        i0 = l5;
        i1 = 4u;
        i0 += i1;
        l5 = i0;
      }
      i0 = a;
      i1 = l13;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto loop_450;}
    }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l12;
    i0 = LoadU32(ctx, i0 + 8u);
    l7 = i0;
    i0 = l12;
    i0 = LoadU32(ctx, i0 + 4u);
    c = i0;
  }
  block_447:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l12;
    i1 = 0u;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = l7;
    i1 = c;
    i0 -= i1;
    i1 = 2u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    l13 = i0;
    i1 = 2u;
    i0 = i0 > i1;
    if (i0) {
      loop_458: 
        i0 = c;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
        i0 = l12;
        i1 = l12;
        i1 = LoadU32(ctx, i1 + 4u);
        i2 = 4u;
        i1 += i2;
        c = i1;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = l12;
        i0 = LoadU32(ctx, i0 + 8u);
        l7 = i0;
        i1 = c;
        i0 -= i1;
        i1 = 2u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        l13 = i0;
        i1 = 2u;
        i0 = i0 > i1;
        if (i0) {goto loop_458;}
    }
    i0 = 12u;
    l5 = i0;
    i0 = l13;
    i1 = 1u;
    i0 -= i1;
    switch (i0) {
      case 0: goto block_460;
      case 1: goto block_461;
      default: goto block_459;
    }
    block_461:;
    i0 = 24u;
    l5 = i0;
    block_460:;
    i0 = l12;
    i1 = l5;
    StoreU32(ctx, i0 + 16u, i1);
    block_459:;
    i0 = c;
    i1 = l7;
    i0 = i0 == i1;
    if (i0) {goto block_462;}
    loop_463: 
      i0 = c;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
      i0 = l7;
      i1 = c;
      i2 = 4u;
      i1 += i2;
      c = i1;
      i0 = i0 != i1;
      if (i0) {goto loop_463;}
    i0 = l12;
    i0 = LoadU32(ctx, i0 + 8u);
    a = i0;
    i1 = l12;
    i1 = LoadU32(ctx, i1 + 4u);
    b = i1;
    i0 = i0 == i1;
    c = i0;
    if (i0) {goto block_462;}
    i0 = l12;
    i1 = a;
    i2 = b;
    i1 -= i2;
    i2 = 4u;
    i1 -= i2;
    i2 = 2u;
    i1 >>= (i2 & 31);
    i2 = kDataFFFFFFFF;
    i1 ^= i2;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i2 = a;
    i1 += i2;
    StoreU32(ctx, i0 + 8u, i1);
    block_462:;
    i0 = l12;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = !(i0);
    b = i0;
    if (i0) {goto block_7;}
    i0 = a;
    DeallocateRawBlock(ctx, i0);
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i1 = 480u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 400u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l6;
    i0 = LoadS8(ctx, i0 + 427u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 416u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l12;
    i0 = LoadU32(ctx, i0 + 4u);
    c = i0;
    i1 = l12;
    i1 = LoadU32(ctx, i1 + 8u);
    l13 = i1;
    i0 = i0 == i1;
    b = i0;
    i0 = l12;
    i0 = LoadU32(ctx, i0 + 16u);
    d = i0;
    i1 = 24u;
    i0 = DIV_U(i0, i1);
    a = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    if (i0) {
      i0 = c;
      l13 = i0;
      goto block_468;
    }
    i0 = c;
    i1 = a;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    b = i0;
    i0 = LoadU32(ctx, i0);
    i1 = d;
    i2 = a;
    i3 = 24u;
    i2 *= i3;
    i1 -= i2;
    i2 = 168u;
    i1 *= i2;
    i0 += i1;
    a = i0;
    i0 = l12;
    i0 = LoadU32(ctx, i0 + 20u);
    i1 = d;
    i0 += i1;
    d = i0;
    i1 = 24u;
    i0 = DIV_U(i0, i1);
    e = i0;
    i1 = 2u;
    i0 <<= (i1 & 31);
    i1 = c;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l7 = i0;
    i0 = l7;
    i1 = d;
    i2 = e;
    i3 = 24u;
    i2 *= i3;
    i1 -= i2;
    i2 = 168u;
    i1 *= i2;
    i0 += i1;
    d = i0;
    i1 = a;
    i0 = i0 == i1;
    if (i0) {goto block_468;}
  }
  loop_471: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0);
      c = i0;
    }
    i0 = l11;
    i1 = 74u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = c;
      i0 = CallTableT2(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l8 = i0;
      i0 = 74u;
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
      i0 = a;
      i1 = 168u;
      i0 += i1;
      a = i0;
      i1 = b;
      i1 = LoadU32(ctx, i1);
      i0 -= i1;
      i1 = 4032u;
      i0 = i0 == i1;
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 4u);
        a = i0;
        i0 = b;
        i1 = 4u;
        i0 += i1;
        b = i0;
      }
      i0 = a;
      i1 = d;
      i0 = i0 != i1;
      c = i0;
      if (i0) {goto loop_471;}
    }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l12;
    i0 = LoadU32(ctx, i0 + 8u);
    l13 = i0;
    i0 = l12;
    i0 = LoadU32(ctx, i0 + 4u);
    c = i0;
  }
  block_468:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l12;
    i1 = 0u;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = l13;
    i1 = c;
    i0 -= i1;
    i1 = 2u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    a = i0;
    i1 = 2u;
    i0 = i0 > i1;
    if (i0) {
      loop_479: 
        i0 = c;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
        i0 = l12;
        i1 = l12;
        i1 = LoadU32(ctx, i1 + 4u);
        i2 = 4u;
        i1 += i2;
        c = i1;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = l12;
        i0 = LoadU32(ctx, i0 + 8u);
        l13 = i0;
        i1 = c;
        i0 -= i1;
        i1 = 2u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        a = i0;
        i1 = 2u;
        i0 = i0 > i1;
        if (i0) {goto loop_479;}
    }
    i0 = 12u;
    l7 = i0;
    i0 = a;
    i1 = 1u;
    i0 -= i1;
    switch (i0) {
      case 0: goto block_481;
      case 1: goto block_482;
      default: goto block_480;
    }
    block_482:;
    i0 = 24u;
    l7 = i0;
    block_481:;
    i0 = l12;
    i1 = l7;
    StoreU32(ctx, i0 + 16u, i1);
    block_480:;
    i0 = c;
    i1 = l13;
    i0 = i0 == i1;
    if (i0) {goto block_483;}
    loop_484: 
      i0 = c;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
      i0 = l13;
      i1 = c;
      i2 = 4u;
      i1 += i2;
      c = i1;
      i0 = i0 != i1;
      if (i0) {goto loop_484;}
    i0 = l12;
    i0 = LoadU32(ctx, i0 + 8u);
    a = i0;
    i1 = l12;
    i1 = LoadU32(ctx, i1 + 4u);
    b = i1;
    i0 = i0 == i1;
    if (i0) {goto block_483;}
    i0 = l12;
    i1 = a;
    i2 = b;
    i1 -= i2;
    i2 = 4u;
    i1 -= i2;
    i2 = 2u;
    i1 >>= (i2 & 31);
    i2 = kDataFFFFFFFF;
    i1 ^= i2;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i2 = a;
    i1 += i2;
    StoreU32(ctx, i0 + 8u, i1);
    block_483:;
    i0 = l12;
    i0 = LoadU32(ctx, i0);
    a = i0;
    if (i0) {
      i0 = a;
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l5;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
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
  i0 = l11;
  i1 = 75u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 75u;
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
  l8 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l8;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kResumeWordBytes;
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
  i1 = d;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l8;
  i1 = e;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l8;
  i1 = l5;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l8;
  i1 = l6;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l8;
  i1 = l7;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l8;
  i1 = l10;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = l8;
  i1 = l9;
  StoreU32(ctx, i0 + 36u, i1);
  i0 = l8;
  i1 = l12;
  StoreU32(ctx, i0 + 40u, i1);
  i0 = l8;
  i1 = l13;
  StoreU32(ctx, i0 + 44u, i1);
  i0 = l8;
  i1 = l14;
  StoreU32(ctx, i0 + 48u, i1);
  i0 = l8;
  i1 = l15;
  StoreU32(ctx, i0 + 52u, i1);
  i0 = l8;
  i1 = l16;
  StoreU32(ctx, i0 + 56u, i1);
  i0 = l8;
  i1 = l17;
  StoreU32(ctx, i0 + 60u, i1);
  i0 = l8;
  j1 = l30;
  StoreU64(ctx, i0 + 64u, j1);
  i0 = l8;
  i1 = l18;
  StoreU32(ctx, i0 + 72u, i1);
  i0 = l8;
  i1 = l19;
  StoreU32(ctx, i0 + 76u, i1);
  i0 = l8;
  i1 = l20;
  StoreU32(ctx, i0 + 80u, i1);
  i0 = l8;
  i1 = l21;
  StoreU32(ctx, i0 + 84u, i1);
  i0 = l8;
  i1 = l22;
  StoreU32(ctx, i0 + 88u, i1);
  i0 = l8;
  i1 = l23;
  StoreU32(ctx, i0 + 92u, i1);
  i0 = l8;
  i1 = l24;
  StoreU32(ctx, i0 + 96u, i1);
  i0 = l8;
  i1 = l25;
  StoreU32(ctx, i0 + 100u, i1);
  i0 = l8;
  i1 = l26;
  StoreU32(ctx, i0 + 104u, i1);
  i0 = l8;
  i1 = l27;
  StoreU32(ctx, i0 + 108u, i1);
  i0 = l8;
  i1 = l28;
  StoreU32(ctx, i0 + 112u, i1);
  i0 = l8;
  j1 = l31;
  StoreU64(ctx, i0 + 116u, j1);
  i0 = l8;
  i1 = l29;
  StoreU32(ctx, i0 + 124u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 128u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
}  // namespace artc
