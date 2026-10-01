#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step calls.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t MemChr(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

namespace Babel {
// Babel::ScoreEncoding -- the encode scorer: the byte buffer is walked against
// every known encoding mode (EUC / Shift-JIS / Shift_JIS / UTF-8) and each
// candidate is scored so the caller can pick the best decode.

void ScoreEncoding(EngineContext* ctx, uint32_t data, uint32_t length, uint32_t out) {
  EnterFrame();
  constexpr uint32_t kMsgContribsBabelBabelCpp = 0x2F8DCu;  // ../../../Contribs/babel/babel.cpp
  constexpr uint32_t kData2F910 = 0x2F910u;
  constexpr uint32_t kMsgUtf = 0x2FB8Bu;  // UTF-8
  constexpr uint32_t kMsgShiftJis = 0x2FB91u;  // Shift_JIS
  constexpr uint32_t kMsgShiftJisX = 0x2FB9Bu;  // shift_jis
  constexpr uint32_t kMsgShiftJisXX = 0x2FBA5u;  // Shift-JIS
  constexpr uint32_t kMsgShiftJisXXX = 0x2FBAFu;  // shift-jis
  constexpr uint32_t kData2FBB9 = 0x2FBB9u;
  constexpr uint32_t kData2FBBD = 0x2FBBDu;
  constexpr uint32_t kData303A0 = 0x303A0u;
  constexpr uint32_t kMsgIndex = 0x304A0u;  // 0 <= index
  constexpr uint32_t kMsgOperator = 0x304ABu;  // operator[]
  constexpr uint32_t kData304C0 = 0x304C0u;
  constexpr uint32_t kData305C0 = 0x305C0u;
  constexpr uint32_t kMsgEncodeScorerFirstByte = 0x309E0u;  // encode_scorer::first_byte == mode
  constexpr uint32_t kMsgScanChar = 0x30A02u;  // scan_char
  constexpr uint32_t kData30A10 = 0x30A10u;
  constexpr uint32_t kMsgMode = 0x30B10u;  // mode < 0
  constexpr uint32_t kDataC87BC = 0xC87BCu;
  constexpr uint32_t kDataC87C0 = 0xC87C0u;
  constexpr uint32_t kDataC87C4 = 0xC87C4u;
  constexpr uint32_t kDataC87E0 = 0xC87E0u;
  constexpr uint32_t kDataC87E4 = 0xC87E4u;
  constexpr uint32_t kDataC87EB = 0xC87EBu;
  constexpr uint32_t kDataC87EC = 0xC87ECu;
  constexpr uint32_t kDataC87F0 = 0xC87F0u;
  constexpr uint32_t kDataC87F7 = 0xC87F7u;
  constexpr uint32_t kDataC87F8 = 0xC87F8u;
  constexpr uint32_t kDataC87FC = 0xC87FCu;
  constexpr uint32_t kDataC8803 = 0xC8803u;
  constexpr uint32_t kDataC8810 = 0xC8810u;
  constexpr uint32_t kDataC8814 = 0xC8814u;
  constexpr uint32_t kDataC881B = 0xC881Bu;
  constexpr uint32_t kDataC881C = 0xC881Cu;
  constexpr uint32_t kDataC8820 = 0xC8820u;
  constexpr uint32_t kDataC8827 = 0xC8827u;
  constexpr uint32_t kDataC8828 = 0xC8828u;
  constexpr uint32_t kDataC882C = 0xC882Cu;
  constexpr uint32_t kDataC8833 = 0xC8833u;
  constexpr uint32_t kDataC8834 = 0xC8834u;
  constexpr uint32_t kDataC8838 = 0xC8838u;
  constexpr uint32_t kDataC883F = 0xC883Fu;
  constexpr uint32_t kDataC8840 = 0xC8840u;
  constexpr uint32_t kDataC8844 = 0xC8844u;
  constexpr uint32_t kDataC884B = 0xC884Bu;
  constexpr uint32_t kDataC884C = 0xC884Cu;
  constexpr uint32_t kDataC8850 = 0xC8850u;
  constexpr uint32_t kDataC8857 = 0xC8857u;
  constexpr uint32_t kDataC8858 = 0xC8858u;
  constexpr uint32_t kDataC885C = 0xC885Cu;
  constexpr uint32_t kDataC8863 = 0xC8863u;
  constexpr uint32_t kDataFFFFFF = 0xFFFFFFu;
  constexpr uint32_t kData3FFFFFC = 0x3FFFFFCu;
  constexpr uint32_t kData53494A53 = 0x53494A53u;
  constexpr uint32_t kData73696A73 = 0x73696A73u;
  constexpr uint32_t kDataFFF9F6C6 = 0xFFF9F6C6u;
  constexpr uint32_t kDataFFFFFF8F = 0xFFFFFF8Fu;
  constexpr uint32_t kDataFFFFFFFC = 0xFFFFFFFCu;
  constexpr uint32_t kDataFFFFFFFE = 0xFFFFFFFEu;
  constexpr uint32_t kDataFFFFFFFF = 0xFFFFFFFFu;
  constexpr uint32_t kResumeWordBytes = 4u;
  constexpr uint32_t kResumeStepDeallocateRawBlock0 = 0u;
  constexpr uint32_t kResumeStepDeallocateRawBlock1 = 1u;
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l13 = 0u; uint32_t l14 = 0u; uint32_t l15 = 0u; uint32_t l16 = 0u; uint32_t l17 = 0u; uint32_t l18 = 0u; uint32_t l19 = 0u; uint32_t l20 = 0u; uint32_t l21 = 0u; uint32_t l22 = 0u; uint32_t l23 = 0u; uint32_t l24 = 0u; uint32_t l25 = 0u; uint32_t l26 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u;
  uint64_t j1 = 0;
      l11 = 0, l12 = 0, l13 = 0, l14 = 0, l15 = 0, l16 = 0, l17 = 0, l18 = 0, 
      l19 = 0, l20 = 0, l21 = 0, l22 = 0, l23 = 0, l24 = 0, l25 = 0, l26 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = 100u;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l9 = i0;
    i0 = LoadU32(ctx, i0);
    data = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 8u);
    out = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 24u);
    l6 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 28u);
    l7 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 32u);
    l8 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 36u);
    l10 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 40u);
    l11 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 44u);
    l12 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 48u);
    l13 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 52u);
    l14 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 56u);
    l15 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 60u);
    l17 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 64u);
    l18 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 68u);
    l19 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 72u);
    l20 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 76u);
    l21 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 80u);
    l22 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 84u);
    l23 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 88u);
    l24 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 92u);
    l25 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 96u);
    l26 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 4u);
    length = i0;
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
    l16 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    i1 = 32u;
    i0 -= i1;
    l15 = i0;
    GuestStackSp(ctx) = i0;
    i0 = length;
    i0 = LoadU8(ctx, i0 + 11u);
    l3 = i0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l7 = i0;
    i0 = length;
    i0 = LoadU32(ctx, i0 + 4u);
    i1 = l3;
    i2 = l7;
    i0 = i2 ? i0 : i1;
    l11 = i0;
    i1 = out;
    i0 = i0 < i1;
    l3 = i0;
    i0 = l11;
    i1 = out;
    i2 = l3;
    i0 = i2 ? i0 : i1;
    l19 = i0;
    i0 = !(i0);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    if (i0) {
      i0 = data;
      j1 = 4294967296ull;
      StoreU64(ctx, i0, j1);
      goto exit_path;
    }
    i0 = l15;
    i1 = 0u;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l15;
    j1 = 0ull;
    StoreU64(ctx, i0 + 16u, j1);
    i0 = l3;
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
    i0 = l16;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 854u;
      i1 = l15;
      i2 = length;
      i3 = 0u;
      i4 = out;
      i5 = length;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l9 = i0;
      i0 = kResumeStepDeallocateRawBlock0;
      i1 = ResumePhase(ctx);
      i2 = kResumeStepDeallocateRawBlock1;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l9;
      length = i0;
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = length;
    } else {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      length = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = length;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_8;}
      i0 = l15;
      i0 = LoadS8(ctx, i0 + 27u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l15;
        i0 = LoadU32(ctx, i0 + 16u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l15;
      i1 = l15;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 24u, i1);
      i0 = l15;
      i1 = l15;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0 + 16u, j1);
      i0 = l15;
      i1 = 16u;
      i0 += i1;
    }
    length = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l11;
    i1 = 4u;
    i2 = l11;
    i3 = 4u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    out = i0;
  }
  i0 = l16;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 854u;
    i1 = l15;
    i2 = length;
    i3 = 0u;
    i4 = out;
    i5 = length;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l9 = i0;
    i0 = kResumeStepDeallocateRawBlock1;
    i1 = ResumePhase(ctx);
    i2 = kResumeStepDeallocateRawBlock1;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l9;
    out = i0;
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
    if (i0) {
      i0 = out;
      i0 = LoadU8(ctx, i0 + 11u);
      l3 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l7 = i0;
      i0 = out;
      i0 = LoadU32(ctx, i0);
      l8 = i0;
      i0 = kDataC8827;
      i0 = LoadU8(ctx, i0);
      l4 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l6 = i0;
      i0 = kDataC8820;
      i0 = LoadU32(ctx, i0);
      i1 = l4;
      i2 = l6;
      i0 = i2 ? i0 : i1;
      l10 = i0;
      if (i0) {
        i0 = out;
        i0 = LoadU32(ctx, i0 + 4u);
        i1 = l3;
        i2 = l7;
        i3 = 0u;
        i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
        i0 = i2 ? i0 : i1;
        out = i0;
        i1 = l8;
        i2 = l15;
        i3 = l7;
        i4 = 0u;
        i3 = (uint32_t)((int32_t)i3 < (int32_t)i4);
        i1 = i3 ? i1 : i2;
        l4 = i1;
        i0 += i1;
        l5 = i0;
        i0 = out;
        i1 = l10;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        if (i0) {goto block_20;}
        i0 = kDataC881C;
        i0 = LoadU32(ctx, i0);
        i1 = kDataC881C;
        i2 = l6;
        i0 = i2 ? i0 : i1;
        l13 = i0;
        i0 = LoadU8(ctx, i0);
        l12 = i0;
        i0 = out;
        l3 = i0;
        i0 = l4;
        l6 = i0;
        loop_23: 
          i0 = l3;
          i1 = l10;
          i0 -= i1;
          l3 = i0;
          i1 = 1u;
          i0 += i1;
          l14 = i0;
          i0 = l3;
          i1 = l14;
          i0 = i0 > i1;
          if (i0) {goto block_20;}
          i0 = l6;
          i1 = l12;
          i2 = l14;
          i0 = MemChr(ctx, i0, i1, i2);
          l3 = i0;
          i0 = !(i0);
          if (i0) {goto block_20;}
          i0 = l3;
          i1 = l13;
          i2 = l10;
          i0 = CompareBytes(ctx, i0, i1, i2);
          if (i0) {
            i0 = l10;
            i1 = l5;
            i2 = l3;
            i3 = 1u;
            i2 += i3;
            l6 = i2;
            i1 -= i2;
            l3 = i1;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            l14 = i0;
            if (i0) {goto loop_23;}
            goto block_20;
          }
        i0 = l3;
        i1 = l5;
        i0 = i0 == i1;
        if (i0) {goto block_20;}
        i0 = l3;
        i1 = l4;
        i0 = i0 != i1;
        if (i0) {goto block_20;}
      }
      i0 = data;
      j1 = 42949672970ull;
      StoreU64(ctx, i0, j1);
      i0 = l7;
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      if (i0) {goto block_5;}
      i0 = l8;
      DeallocateRawBlock(ctx, i0);
      goto block_5;
    }
    goto block_8;
    block_20:;
    i0 = kDataC882C;
    i0 = LoadU32(ctx, i0);
    i1 = kDataC8833;
    i1 = LoadU8(ctx, i1);
    l3 = i1;
    i2 = l3;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    l3 = i2;
    i0 = i2 ? i0 : i1;
    l10 = i0;
    if (i0) {
      i0 = out;
      i1 = l10;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      if (i0) {goto block_25;}
      i0 = kDataC8828;
      i0 = LoadU32(ctx, i0);
      i1 = kDataC8828;
      i2 = l3;
      i0 = i2 ? i0 : i1;
      l13 = i0;
      i0 = LoadU8(ctx, i0);
      l12 = i0;
      i0 = out;
      l3 = i0;
      i0 = l4;
      l6 = i0;
      loop_27: 
        i0 = l3;
        i1 = l10;
        i0 -= i1;
        l3 = i0;
        i1 = 1u;
        i0 += i1;
        l14 = i0;
        i0 = l3;
        i1 = l14;
        i0 = i0 > i1;
        if (i0) {goto block_25;}
        i0 = l6;
        i1 = l12;
        i2 = l14;
        i0 = MemChr(ctx, i0, i1, i2);
        l3 = i0;
        i0 = !(i0);
        if (i0) {goto block_25;}
        i0 = l3;
        i1 = l13;
        i2 = l10;
        i0 = CompareBytes(ctx, i0, i1, i2);
        if (i0) {
          i0 = l10;
          i1 = l5;
          i2 = l3;
          i3 = 1u;
          i2 += i3;
          l6 = i2;
          i1 -= i2;
          l3 = i1;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          l14 = i0;
          if (i0) {goto loop_27;}
          goto block_25;
        }
      i0 = l3;
      i1 = l5;
      i0 = i0 == i1;
      if (i0) {goto block_25;}
      i0 = l3;
      i1 = l4;
      i0 = i0 != i1;
      if (i0) {goto block_25;}
    }
    i0 = data;
    j1 = 47244640267ull;
    StoreU64(ctx, i0, j1);
    i0 = l7;
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_5;}
    i0 = l8;
    DeallocateRawBlock(ctx, i0);
    goto block_5;
    block_25:;
    i0 = kDataC8838;
    i0 = LoadU32(ctx, i0);
    i1 = kDataC883F;
    i1 = LoadU8(ctx, i1);
    l3 = i1;
    i2 = l3;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    l3 = i2;
    i0 = i2 ? i0 : i1;
    l10 = i0;
    if (i0) {
      i0 = out;
      i1 = l10;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      if (i0) {goto block_29;}
      i0 = kDataC8834;
      i0 = LoadU32(ctx, i0);
      i1 = kDataC8834;
      i2 = l3;
      i0 = i2 ? i0 : i1;
      l13 = i0;
      i0 = LoadU8(ctx, i0);
      l12 = i0;
      i0 = out;
      l3 = i0;
      i0 = l4;
      l6 = i0;
      loop_31: 
        i0 = l3;
        i1 = l10;
        i0 -= i1;
        l3 = i0;
        i1 = 1u;
        i0 += i1;
        l14 = i0;
        i0 = l3;
        i1 = l14;
        i0 = i0 > i1;
        if (i0) {goto block_29;}
        i0 = l6;
        i1 = l12;
        i2 = l14;
        i0 = MemChr(ctx, i0, i1, i2);
        l3 = i0;
        i0 = !(i0);
        if (i0) {goto block_29;}
        i0 = l3;
        i1 = l13;
        i2 = l10;
        i0 = CompareBytes(ctx, i0, i1, i2);
        if (i0) {
          i0 = l10;
          i1 = l5;
          i2 = l3;
          i3 = 1u;
          i2 += i3;
          l6 = i2;
          i1 -= i2;
          l3 = i1;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          l14 = i0;
          if (i0) {goto loop_31;}
          goto block_29;
        }
      i0 = l3;
      i1 = l5;
      i0 = i0 == i1;
      if (i0) {goto block_29;}
      i0 = l3;
      i1 = l4;
      i0 = i0 != i1;
      if (i0) {goto block_29;}
    }
    i0 = data;
    j1 = 30064771079ull;
    StoreU64(ctx, i0, j1);
    i0 = l7;
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_5;}
    i0 = l8;
    DeallocateRawBlock(ctx, i0);
    goto block_5;
    block_29:;
    i0 = kDataC8844;
    i0 = LoadU32(ctx, i0);
    i1 = kDataC884B;
    i1 = LoadU8(ctx, i1);
    l3 = i1;
    i2 = l3;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    l3 = i2;
    i0 = i2 ? i0 : i1;
    l10 = i0;
    if (i0) {
      i0 = out;
      i1 = l10;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      if (i0) {goto block_33;}
      i0 = kDataC8840;
      i0 = LoadU32(ctx, i0);
      i1 = kDataC8840;
      i2 = l3;
      i0 = i2 ? i0 : i1;
      l13 = i0;
      i0 = LoadU8(ctx, i0);
      l12 = i0;
      i0 = out;
      l3 = i0;
      i0 = l4;
      l6 = i0;
      loop_35: 
        i0 = l3;
        i1 = l10;
        i0 -= i1;
        l3 = i0;
        i1 = 1u;
        i0 += i1;
        l14 = i0;
        i0 = l3;
        i1 = l14;
        i0 = i0 > i1;
        if (i0) {goto block_33;}
        i0 = l6;
        i1 = l12;
        i2 = l14;
        i0 = MemChr(ctx, i0, i1, i2);
        l3 = i0;
        i0 = !(i0);
        if (i0) {goto block_33;}
        i0 = l3;
        i1 = l13;
        i2 = l10;
        i0 = CompareBytes(ctx, i0, i1, i2);
        if (i0) {
          i0 = l10;
          i1 = l5;
          i2 = l3;
          i3 = 1u;
          i2 += i3;
          l6 = i2;
          i1 -= i2;
          l3 = i1;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          l14 = i0;
          if (i0) {goto loop_35;}
          goto block_33;
        }
      i0 = l3;
      i1 = l5;
      i0 = i0 == i1;
      if (i0) {goto block_33;}
      i0 = l3;
      i1 = l4;
      i0 = i0 != i1;
      if (i0) {goto block_33;}
    }
    i0 = data;
    j1 = 34359738376ull;
    StoreU64(ctx, i0, j1);
    i0 = l7;
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_5;}
    i0 = l8;
    DeallocateRawBlock(ctx, i0);
    goto block_5;
    block_33:;
    i0 = kDataC8857;
    i0 = LoadU8(ctx, i0);
    l3 = i0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l6 = i0;
    i0 = kDataC8850;
    i0 = LoadU32(ctx, i0);
    i1 = l3;
    i2 = l6;
    i0 = i2 ? i0 : i1;
    l3 = i0;
    if (i0) {
      i0 = out;
      i1 = l3;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l10 = i0;
      if (i0) {goto block_37;}
      i0 = kDataC884C;
      i0 = LoadU32(ctx, i0);
      i1 = kDataC884C;
      i2 = l6;
      i0 = i2 ? i0 : i1;
      l10 = i0;
      i0 = LoadU8(ctx, i0);
      l13 = i0;
      i0 = l4;
      l6 = i0;
      loop_39: 
        i0 = out;
        i1 = l3;
        i0 -= i1;
        out = i0;
        i1 = 1u;
        i0 += i1;
        l12 = i0;
        i0 = out;
        i1 = l12;
        i0 = i0 > i1;
        if (i0) {goto block_37;}
        i0 = l6;
        i1 = l13;
        i2 = l12;
        i0 = MemChr(ctx, i0, i1, i2);
        out = i0;
        i0 = !(i0);
        if (i0) {goto block_37;}
        i0 = out;
        i1 = l10;
        i2 = l3;
        i0 = CompareBytes(ctx, i0, i1, i2);
        if (i0) {
          i0 = l3;
          i1 = l5;
          i2 = out;
          i3 = 1u;
          i2 += i3;
          l6 = i2;
          i1 -= i2;
          out = i1;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          l12 = i0;
          if (i0) {goto loop_39;}
          goto block_37;
        }
      i0 = out;
      i1 = l5;
      i0 = i0 == i1;
      if (i0) {goto block_37;}
      i0 = out;
      i1 = l4;
      i0 = i0 != i1;
      if (i0) {goto block_37;}
    }
    i0 = data;
    j1 = 38654705673ull;
    StoreU64(ctx, i0, j1);
    i0 = l7;
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_5;}
    i0 = l8;
    DeallocateRawBlock(ctx, i0);
    goto block_5;
    block_37:;
    i0 = l7;
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l8;
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l15;
    i1 = 0u;
    StoreU16(ctx, i0, i1);
    i0 = l15;
    i1 = 2u;
    StoreU8(ctx, i0 + 11u, i1);
    i0 = length;
    i0 = LoadU8(ctx, i0 + 11u);
    l4 = i0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    l5 = i0;
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = length;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = length;
      i0 = LoadU32(ctx, i0 + 4u);
      goto block_43;
    }
    i0 = length;
    l6 = i0;
    i0 = l4;
    block_43:;
    out = i0;
    i1 = 2u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {goto block_42;}
    i0 = out;
    i1 = l6;
    i0 += i1;
    l7 = i0;
    i0 = l6;
    l3 = i0;
    loop_45: 
      i0 = l3;
      i1 = 0u;
      i2 = out;
      i3 = 1u;
      i2 -= i3;
      i0 = MemChr(ctx, i0, i1, i2);
      out = i0;
      i0 = !(i0);
      if (i0) {goto block_42;}
      i0 = out;
      i0 = LoadU16(ctx, i0);
      i1 = l15;
      i1 = LoadU16(ctx, i1);
      i0 = i0 != i1;
      if (i0) {
        i0 = l7;
        i1 = out;
        i2 = 1u;
        i1 += i2;
        l3 = i1;
        i0 -= i1;
        out = i0;
        i1 = 1u;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {goto loop_45;}
        goto block_42;
      }
    i0 = out;
    i1 = l7;
    i0 = i0 == i1;
    if (i0) {goto block_42;}
    i0 = out;
    i1 = l6;
    i0 -= i1;
    out = i0;
    i1 = kDataFFFFFFFF;
    i0 = i0 == i1;
    if (i0) {goto block_42;}
    i0 = out;
    i1 = 3u;
    i0 &= i1;
    switch (i0) {
      case 0: goto block_48;
      case 1: goto block_42;
      case 2: goto block_47;
      default: goto block_42;
    }
    block_48:;
    i0 = data;
    j1 = 4294967306ull;
    StoreU64(ctx, i0, j1);
    goto block_5;
    block_47:;
    i0 = data;
    j1 = 4294967307ull;
    StoreU64(ctx, i0, j1);
    goto block_5;
    block_42:;
    i0 = l5;
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = length;
      i0 = LoadU32(ctx, i0 + 4u);
      l3 = i0;
      i0 = length;
      i0 = LoadU32(ctx, i0);
      goto block_49;
    }
    i0 = l4;
    l3 = i0;
    i0 = length;
    block_49:;
    out = i0;
    i0 = kDataC87EB;
    i0 = LoadU8(ctx, i0);
    l6 = i0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l7 = i0;
    i0 = kDataC87E4;
    i0 = LoadU32(ctx, i0);
    i1 = l6;
    i2 = l7;
    i0 = i2 ? i0 : i1;
    l6 = i0;
    i0 = !(i0);
    if (i0) {goto block_52;}
    i0 = l3;
    i1 = l6;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {goto block_53;}
    i0 = out;
    i1 = l3;
    i0 += i1;
    l8 = i0;
    i0 = kDataC87E0;
    i0 = LoadU32(ctx, i0);
    i1 = kDataC87E0;
    i2 = l7;
    i0 = i2 ? i0 : i1;
    l10 = i0;
    i0 = LoadU8(ctx, i0);
    l13 = i0;
    i0 = out;
    l7 = i0;
    loop_54: 
      i0 = l3;
      i1 = l6;
      i0 -= i1;
      l3 = i0;
      i1 = 1u;
      i0 += i1;
      l12 = i0;
      i0 = l3;
      i1 = l12;
      i0 = i0 > i1;
      if (i0) {goto block_53;}
      i0 = l7;
      i1 = l13;
      i2 = l12;
      i0 = MemChr(ctx, i0, i1, i2);
      l3 = i0;
      i0 = !(i0);
      if (i0) {goto block_53;}
      i0 = l3;
      i1 = l10;
      i2 = l6;
      i0 = CompareBytes(ctx, i0, i1, i2);
      if (i0) {
        i0 = l6;
        i1 = l8;
        i2 = l3;
        i3 = 1u;
        i2 += i3;
        l7 = i2;
        i1 -= i2;
        l3 = i1;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        l12 = i0;
        if (i0) {goto loop_54;}
        goto block_53;
      }
    i0 = l3;
    i1 = l8;
    i0 = i0 == i1;
    if (i0) {goto block_53;}
    i0 = l3;
    i1 = out;
    i0 -= i1;
    i1 = kDataFFFFFFFF;
    i0 = i0 != i1;
    if (i0) {goto block_52;}
    block_53:;
    i0 = l5;
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = length;
      i0 = LoadU32(ctx, i0 + 4u);
      l3 = i0;
      i0 = length;
      i0 = LoadU32(ctx, i0);
      goto block_56;
    }
    i0 = l4;
    l3 = i0;
    i0 = length;
    block_56:;
    out = i0;
    i0 = kDataC87F7;
    i0 = LoadU8(ctx, i0);
    l6 = i0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l7 = i0;
    i0 = kDataC87F0;
    i0 = LoadU32(ctx, i0);
    i1 = l6;
    i2 = l7;
    i0 = i2 ? i0 : i1;
    l6 = i0;
    i0 = !(i0);
    if (i0) {goto block_52;}
    i0 = l3;
    i1 = l6;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {goto block_58;}
    i0 = out;
    i1 = l3;
    i0 += i1;
    l8 = i0;
    i0 = kDataC87EC;
    i0 = LoadU32(ctx, i0);
    i1 = kDataC87EC;
    i2 = l7;
    i0 = i2 ? i0 : i1;
    l10 = i0;
    i0 = LoadU8(ctx, i0);
    l13 = i0;
    i0 = out;
    l7 = i0;
    loop_59: 
      i0 = l3;
      i1 = l6;
      i0 -= i1;
      l3 = i0;
      i1 = 1u;
      i0 += i1;
      l12 = i0;
      i0 = l3;
      i1 = l12;
      i0 = i0 > i1;
      if (i0) {goto block_58;}
      i0 = l7;
      i1 = l13;
      i2 = l12;
      i0 = MemChr(ctx, i0, i1, i2);
      l3 = i0;
      i0 = !(i0);
      if (i0) {goto block_58;}
      i0 = l3;
      i1 = l10;
      i2 = l6;
      i0 = CompareBytes(ctx, i0, i1, i2);
      if (i0) {
        i0 = l6;
        i1 = l8;
        i2 = l3;
        i3 = 1u;
        i2 += i3;
        l7 = i2;
        i1 -= i2;
        l3 = i1;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        l12 = i0;
        if (i0) {goto loop_59;}
        goto block_58;
      }
    i0 = l3;
    i1 = l8;
    i0 = i0 == i1;
    if (i0) {goto block_58;}
    i0 = l3;
    i1 = out;
    i0 -= i1;
    i1 = kDataFFFFFFFF;
    i0 = i0 != i1;
    if (i0) {goto block_52;}
    block_58:;
    i0 = l5;
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = length;
      i0 = LoadU32(ctx, i0 + 4u);
      l3 = i0;
      i0 = length;
      i0 = LoadU32(ctx, i0);
      goto block_61;
    }
    i0 = l4;
    l3 = i0;
    i0 = length;
    block_61:;
    out = i0;
    i0 = kDataC8803;
    i0 = LoadU8(ctx, i0);
    l6 = i0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l7 = i0;
    i0 = kDataC87FC;
    i0 = LoadU32(ctx, i0);
    i1 = l6;
    i2 = l7;
    i0 = i2 ? i0 : i1;
    l6 = i0;
    i0 = !(i0);
    if (i0) {goto block_52;}
    i0 = l3;
    i1 = l6;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l8 = i0;
    if (i0) {goto block_51;}
    i0 = out;
    i1 = l3;
    i0 += i1;
    l8 = i0;
    i0 = kDataC87F8;
    i0 = LoadU32(ctx, i0);
    i1 = kDataC87F8;
    i2 = l7;
    i0 = i2 ? i0 : i1;
    l10 = i0;
    i0 = LoadU8(ctx, i0);
    l13 = i0;
    i0 = out;
    l7 = i0;
    loop_63: 
      i0 = l3;
      i1 = l6;
      i0 -= i1;
      l3 = i0;
      i1 = 1u;
      i0 += i1;
      l12 = i0;
      i0 = l3;
      i1 = l12;
      i0 = i0 > i1;
      if (i0) {goto block_51;}
      i0 = l7;
      i1 = l13;
      i2 = l12;
      i0 = MemChr(ctx, i0, i1, i2);
      l3 = i0;
      i0 = !(i0);
      if (i0) {goto block_51;}
      i0 = l3;
      i1 = l10;
      i2 = l6;
      i0 = CompareBytes(ctx, i0, i1, i2);
      if (i0) {
        i0 = l6;
        i1 = l8;
        i2 = l3;
        i3 = 1u;
        i2 += i3;
        l7 = i2;
        i1 -= i2;
        l3 = i1;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        l12 = i0;
        if (i0) {goto loop_63;}
        goto block_51;
      }
    i0 = l3;
    i1 = l8;
    i0 = i0 == i1;
    if (i0) {goto block_51;}
    i0 = l3;
    i1 = out;
    i0 -= i1;
    i1 = kDataFFFFFFFF;
    i0 = i0 == i1;
    if (i0) {goto block_51;}
    block_52:;
    i0 = data;
    j1 = 17179869188ull;
    StoreU64(ctx, i0, j1);
    goto block_5;
    block_51:;
    i0 = length;
    i0 = LoadU32(ctx, i0 + 4u);
    i1 = l4;
    i2 = l5;
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    out = i2;
    i0 = i2 ? i0 : i1;
    l5 = i0;
    i0 = !(i0);
    if (i0) {goto block_6;}
    i0 = kDataC885C;
    i0 = LoadU32(ctx, i0);
    i1 = kDataC8863;
    i1 = LoadU8(ctx, i1);
    l3 = i1;
    i2 = l3;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    l3 = i2;
    i0 = i2 ? i0 : i1;
    l4 = i0;
    i0 = !(i0);
    if (i0) {goto block_6;}
    i0 = l4;
    l7 = i0;
    i0 = l7;
    i1 = kDataC8858;
    i1 = LoadU32(ctx, i1);
    i2 = kDataC8858;
    i3 = l3;
    i1 = i3 ? i1 : i2;
    l4 = i1;
    i0 += i1;
    l7 = i0;
    i0 = l5;
    i1 = length;
    i1 = LoadU32(ctx, i1);
    i2 = length;
    i3 = out;
    i1 = i3 ? i1 : i2;
    l3 = i1;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    l6 = i0;
  }
  loop_68: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i0 = LoadU8(ctx, i0);
      l8 = i0;
      i0 = l4;
      out = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      loop_75: 
        i0 = l8;
        i1 = out;
        i1 = LoadU8(ctx, i1);
        i0 = i0 != i1;
        l10 = i0;
        if (i0) {
          i0 = l7;
          i1 = out;
          i2 = 1u;
          i1 += i2;
          out = i1;
          i0 = i0 != i1;
          l10 = i0;
          if (i0) {goto loop_75;}
          goto block_71;
        }
      i0 = l5;
      i1 = l6;
      i0 = i0 == i1;
      if (i0) {goto block_6;}
      i0 = l6;
      i1 = l3;
      i0 -= i1;
      out = i0;
      i1 = kDataFFFFFFFF;
      i0 = i0 == i1;
      if (i0) {goto block_6;}
      i0 = l11;
      i1 = out;
      i2 = kDataFFFFFFFC;
      i1 &= i2;
      l3 = i1;
      i2 = 256u;
      i1 += i2;
      out = i1;
      i0 = i0 < i1;
      l4 = i0;
      i0 = l11;
      i1 = out;
      i2 = l4;
      i0 = i2 ? i0 : i1;
      l26 = i0;
      i1 = l3;
      i0 = i0 > i1;
      out = i0;
    }
    i0 = out;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = 0u;
        l6 = i0;
        i0 = 0u;
        l7 = i0;
        i0 = 0u;
        l12 = i0;
        i0 = 0u;
        l5 = i0;
        i0 = 0u;
        l4 = i0;
        i0 = 0u;
        l14 = i0;
        i0 = 0u;
        out = i0;
      }
      loop_79: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          l10 = i0;
          i0 = l6;
          l13 = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = out;
          l8 = i0;
        }
        i0 = l16;
        i1 = 2u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 876u;
          i1 = length;
          i2 = l3;
          i0 = HostOsQuery(ctx, i0, i1, i2);
          l9 = i0;
          i0 = 2u;
          i1 = ResumePhase(ctx);
          i2 = kResumeStepDeallocateRawBlock1;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l9;
          out = i0;
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
            i0 = out;
            i0 = LoadU8(ctx, i0);
            l11 = i0;
            i1 = 24u;
            i0 <<= (i1 & 31);
            i1 = 24u;
            i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
            l6 = i0;
            i0 = l11;
            i1 = 1u;
            i0 -= i1;
            i1 = 251u;
            i0 = i0 <= i1;
            out = i0;
          }
          i0 = out;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = l14;
              switch (i0) {
                case 0: goto block_95;
                case 1: goto block_92;
                default: goto block_94;
              }
              block_95:;
              i0 = 1u;
              l14 = i0;
              i0 = l11;
              i1 = kData303A0;
              i0 += i1;
              i0 = LoadU8(ctx, i0);
              out = i0;
              if (i0) {
                i0 = l11;
                l4 = i0;
                goto block_87;
              }
              i0 = 0u;
              l14 = i0;
              i0 = 0u;
              i1 = kDataC87BC;
              i1 = LoadU32(ctx, i1);
              i1 = LoadU32(ctx, i1);
              out = i1;
              i1 = !(i1);
              l10 = i1;
              if (i1) {goto block_97;}
              i0 = 0u;
              i1 = l11;
              i2 = 3u;
              i1 >>= (i2 & 31);
              i2 = 28u;
              i1 &= i2;
              i2 = out;
              i1 += i2;
              i1 = LoadU32(ctx, i1);
              out = i1;
              i1 = !(i1);
              l10 = i1;
              if (i1) {goto block_97;}
              i0 = l11;
              i1 = 31u;
              i0 &= i1;
              i1 = 2u;
              i0 <<= (i1 & 31);
              i1 = out;
              i0 += i1;
              out = i0;
              i0 = LoadU32(ctx, i0);
              block_97:;
              i1 = l20;
              i0 += i1;
              l20 = i0;
              i0 = l11;
              l4 = i0;
              goto block_87;
              block_94:;
              i0 = kDataD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
            }
            i0 = l16;
            i1 = 3u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 857u;
              i1 = kMsgEncodeScorerFirstByte;
              i2 = kMsgContribsBabelBabelCpp;
              i3 = 3976u;
              i4 = kMsgScanChar;
              HostFailAssertion(ctx, i0, i1, i2, i3, i4);
              i0 = 3u;
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
              data = i0;
              i0 = kDataD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = data;
              i1 = 1u;
              i0 = i0 == i1;
              data = i0;
              if (i0) {goto block_65;}
              goto block_7;
            }
            block_92:;
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = l11;
              i1 = kData30A10;
              i0 += i1;
              i0 = LoadU8(ctx, i0);
              out = i0;
            }
            i0 = out;
            i1 = ResumePhase(ctx);
            i2 = 2u;
            i1 = i1 == i2;
            i0 |= i1;
            if (i0) {
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = l11;
                i1 = l10;
                i2 = 8u;
                i1 <<= (i2 & 31);
                i0 |= i1;
                l4 = i0;
                i1 = kDataFFFFFFFF;
                i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
                out = i0;
              }
              i0 = out;
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
                i0 = l16;
                i1 = 4u;
                i0 = i0 == i1;
                i1 = 1u;
                i2 = ResumePhase(ctx);
                i0 = i2 ? i0 : i1;
                if (i0) {
                  i0 = 857u;
                  i1 = kMsgIndex;
                  i2 = kMsgContribsBabelBabelCpp;
                  i3 = 157u;
                  i4 = kMsgOperator;
                  HostFailAssertion(ctx, i0, i1, i2, i3, i4);
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
                  data = i0;
                  i0 = kDataD015C;
                  i1 = 0u;
                  StoreU32(ctx, i0, i1);
                  i0 = data;
                  i1 = 1u;
                  i0 = i0 != i1;
                  data = i0;
                  if (i0) {goto block_7;}
                  goto block_65;
                }
              }
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = 0u;
                l14 = i0;
                i0 = 0u;
                out = i0;
                i0 = l4;
                i1 = 65535u;
                i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
                if (i0) {goto block_110;}
                i0 = kDataC87BC;
                i0 = LoadU32(ctx, i0);
                i1 = l10;
                i2 = kDataFFFFFF;
                i1 &= i2;
                i2 = 2u;
                i1 <<= (i2 & 31);
                i0 += i1;
                i0 = LoadU32(ctx, i0);
                l10 = i0;
                i0 = !(i0);
                if (i0) {goto block_110;}
                i0 = l11;
                i1 = 3u;
                i0 >>= (i1 & 31);
                i1 = 28u;
                i0 &= i1;
                i1 = l10;
                i0 += i1;
                i0 = LoadU32(ctx, i0);
                l10 = i0;
                i0 = !(i0);
                if (i0) {goto block_110;}
                i0 = l10;
                i1 = l11;
                i2 = 31u;
                i1 &= i2;
                i2 = 2u;
                i1 <<= (i2 & 31);
                i0 += i1;
                i0 = LoadU32(ctx, i0);
                out = i0;
                block_110:;
                i0 = out;
                i1 = l20;
                i0 += i1;
                l20 = i0;
                goto block_87;
              }
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = l17;
              i1 = 1u;
              i0 += i1;
              l17 = i0;
              i0 = 0u;
              l14 = i0;
              goto block_88;
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l17;
            i1 = 1u;
            i0 += i1;
            l17 = i0;
            i0 = 0u;
            l14 = i0;
            i0 = l6;
            i0 = !(i0);
            out = i0;
            if (i0) {goto block_83;}
          }
          block_88:;
          i0 = l4;
          i1 = l10;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          l4 = i0;
          block_87:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l5;
            switch (i0) {
              case 0: goto block_120;
              case 1: goto block_119;
              default: goto block_116;
            }
            block_120:;
            i0 = 1u;
            l5 = i0;
            i0 = l11;
            i1 = kData304C0;
            i0 += i1;
            i0 = LoadU8(ctx, i0);
            out = i0;
            if (i0) {goto block_114;}
            i0 = 0u;
            l5 = i0;
            i0 = l6;
            i1 = kDataFFFFFF8F;
            i0 = i0 != i1;
            out = i0;
            if (i0) {goto block_114;}
            i0 = kDataFFFFFFFE;
            l5 = i0;
            i0 = 143u;
            out = i0;
            goto block_113;
            block_119:;
            i0 = l6;
            i1 = kDataFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            out = i0;
            if (i0) {goto block_115;}
            i0 = l11;
            i1 = l8;
            i2 = 8u;
            i1 <<= (i2 & 31);
            i0 |= i1;
            out = i0;
            i1 = kDataFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
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
            i0 = l16;
            i1 = 5u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 857u;
              i1 = kMsgIndex;
              i2 = kMsgContribsBabelBabelCpp;
              i3 = 157u;
              i4 = kMsgOperator;
              HostFailAssertion(ctx, i0, i1, i2, i3, i4);
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
              data = i0;
              i0 = kDataD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = data;
              i1 = 1u;
              i0 = i0 != i1;
              data = i0;
              if (i0) {goto block_7;}
              goto block_65;
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = 0u;
            l5 = i0;
            i0 = 0u;
            l6 = i0;
            i0 = out;
            i1 = 65535u;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            l10 = i0;
            if (i0) {goto block_126;}
            i0 = kDataC87C0;
            i0 = LoadU32(ctx, i0);
            i1 = l8;
            i2 = kDataFFFFFF;
            i1 &= i2;
            i2 = 2u;
            i1 <<= (i2 & 31);
            i0 += i1;
            i0 = LoadU32(ctx, i0);
            l8 = i0;
            i0 = !(i0);
            l10 = i0;
            if (i0) {goto block_126;}
            i0 = l8;
            i1 = l11;
            i2 = 3u;
            i1 >>= (i2 & 31);
            i2 = 28u;
            i1 &= i2;
            i0 += i1;
            i0 = LoadU32(ctx, i0);
            l8 = i0;
            i0 = !(i0);
            l10 = i0;
            if (i0) {goto block_126;}
            i0 = l8;
            i1 = l11;
            i2 = 31u;
            i1 &= i2;
            i2 = 2u;
            i1 <<= (i2 & 31);
            i0 += i1;
            i0 = LoadU32(ctx, i0);
            l6 = i0;
            block_126:;
            i0 = l6;
            i1 = l23;
            i0 += i1;
            l23 = i0;
            goto block_113;
          }
          block_116:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l5;
            i1 = kDataFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            out = i0;
            if (i0) {goto block_115;}
            i0 = l5;
            i1 = 1u;
            i0 += i1;
            l5 = i0;
            i0 = l8;
            out = i0;
            goto block_113;
          }
          block_115:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l18;
            i1 = 1u;
            i0 += i1;
            l18 = i0;
            i0 = 0u;
            l5 = i0;
            i0 = l8;
            out = i0;
            goto block_113;
          }
          block_114:;
          i0 = out;
          i1 = l11;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          out = i0;
          block_113:;
          i0 = l6;
          i1 = l7;
          i1 = !(i1);
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          l6 = i0;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l6;
            if (i0) {
              i0 = l11;
              i1 = 2u;
              i0 <<= (i1 & 31);
              i1 = kData305C0;
              i0 += i1;
              i0 = LoadU32(ctx, i0);
              l6 = i0;
              i0 = !(i0);
              l7 = i0;
              if (i0) {goto block_130;}
              i0 = 1u;
              i1 = l6;
              i0 -= i1;
              l7 = i0;
              i0 = l11;
              i1 = l6;
              i2 = 2u;
              i1 <<= (i2 & 31);
              i2 = kData2F910;
              i1 += i2;
              i1 = LoadU32(ctx, i1);
              i0 &= i1;
              l6 = i0;
              goto block_129;
            }
            i0 = l7;
            i1 = 0u;
            i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
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
            i0 = l16;
            i1 = 6u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 857u;
              i1 = kMsgMode;
              i2 = kMsgContribsBabelBabelCpp;
              i3 = 4064u;
              i4 = kMsgScanChar;
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
              i0 = kDataD015C;
              i0 = LoadU32(ctx, i0);
              data = i0;
              i0 = kDataD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = data;
              i1 = 1u;
              i0 = i0 != i1;
              data = i0;
              if (i0) {goto block_7;}
              goto block_65;
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l11;
            i1 = 192u;
            i0 &= i1;
            i1 = 128u;
            i0 = i0 != i1;
            l6 = i0;
            if (i0) {goto block_130;}
            i0 = l11;
            i1 = 63u;
            i0 &= i1;
            i1 = l13;
            i2 = 6u;
            i1 <<= (i2 & 31);
            i0 |= i1;
            l6 = i0;
            i0 = l7;
            i1 = 1u;
            i0 += i1;
            l8 = i0;
            i1 = l7;
            i0 = i0 >= i1;
            if (i0) {
              i0 = l8;
              l7 = i0;
              goto block_129;
            }
            i0 = l6;
            i1 = kDataFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            l7 = i0;
          }
          i0 = l7;
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
            i0 = l16;
            i1 = 7u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 857u;
              i1 = kMsgIndex;
              i2 = kMsgContribsBabelBabelCpp;
              i3 = 157u;
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
              data = i0;
              i0 = kDataD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = data;
              i1 = 1u;
              i0 = i0 != i1;
              data = i0;
              if (i0) {goto block_7;}
              goto block_65;
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = 0u;
            l7 = i0;
            i0 = 0u;
            l8 = i0;
            i0 = l6;
            i1 = 65535u;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            l10 = i0;
            if (i0) {goto block_146;}
            i0 = kDataC87C4;
            i0 = LoadU32(ctx, i0);
            i1 = l13;
            i2 = kData3FFFFFC;
            i1 &= i2;
            i0 += i1;
            i0 = LoadU32(ctx, i0);
            l10 = i0;
            i0 = !(i0);
            l13 = i0;
            if (i0) {goto block_146;}
            i0 = l6;
            i1 = 3u;
            i0 >>= (i1 & 31);
            i1 = 28u;
            i0 &= i1;
            i1 = l10;
            i0 += i1;
            i0 = LoadU32(ctx, i0);
            l10 = i0;
            i0 = !(i0);
            l13 = i0;
            if (i0) {goto block_146;}
            i0 = l11;
            i1 = 31u;
            i0 &= i1;
            i1 = 2u;
            i0 <<= (i1 & 31);
            i1 = l10;
            i0 += i1;
            i0 = LoadU32(ctx, i0);
            l8 = i0;
            block_146:;
            i0 = l8;
            i1 = l24;
            i0 += i1;
            l24 = i0;
            goto block_129;
          }
          block_130:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l12;
            i1 = 1u;
            i0 += i1;
            l12 = i0;
            i0 = 0u;
            l7 = i0;
            i0 = l13;
            l6 = i0;
          }
          block_129:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l25;
            i1 = l11;
            i2 = 7u;
            i1 >>= (i2 & 31);
            l8 = i1;
            i0 += i1;
            l25 = i0;
            goto block_82;
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto block_8;}
        block_83:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l12;
          i1 = 1u;
          i0 += i1;
          l12 = i0;
          i0 = l18;
          i1 = 1u;
          i0 += i1;
          l18 = i0;
          i0 = l3;
          i1 = 1u;
          i0 &= i1;
          i0 = !(i0);
          if (i0) {
            i0 = l21;
            i1 = 1u;
            i0 += i1;
            l21 = i0;
            goto block_150;
          }
          i0 = l22;
          i1 = 1u;
          i0 += i1;
          l22 = i0;
          block_150:;
          i0 = 0u;
          l7 = i0;
          i0 = l13;
          l6 = i0;
          i0 = l10;
          l4 = i0;
          i0 = 0u;
          l5 = i0;
          i0 = l8;
          out = i0;
        }
        block_82:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l26;
          i1 = l3;
          i2 = 1u;
          i1 += i2;
          l3 = i1;
          i0 = i0 != i1;
          l8 = i0;
          if (i0) {goto loop_79;}
        }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l21;
        i1 = 0u;
        i2 = l22;
        i1 -= i2;
        i0 = i0 != i1;
        if (i0) {
          i0 = l21;
          i1 = l22;
          i0 = i0 < i1;
          if (i0) {
            i0 = data;
            j1 = 4294967305ull;
            StoreU64(ctx, i0, j1);
            goto block_5;
          }
          i0 = data;
          j1 = 4294967304ull;
          StoreU64(ctx, i0, j1);
          goto block_5;
        }
        i0 = l25;
        if (i0) {goto block_72;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = length;
      i0 = LoadS8(ctx, i0 + 11u);
      out = i0;
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = length;
        i0 = LoadU32(ctx, i0 + 4u);
        l3 = i0;
        i0 = length;
        i0 = LoadU32(ctx, i0);
        length = i0;
        goto block_157;
      }
      i0 = out;
      i1 = 255u;
      i0 &= i1;
      l3 = i0;
      block_157:;
      i0 = kDataC881B;
      i0 = LoadU8(ctx, i0);
      out = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l5 = i0;
      i0 = kDataC8814;
      i0 = LoadU32(ctx, i0);
      i1 = out;
      i2 = l5;
      i0 = i2 ? i0 : i1;
      out = i0;
      if (i0) {
        i0 = out;
        i1 = l3;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {goto block_66;}
        i0 = length;
        i1 = l3;
        i0 += i1;
        l4 = i0;
        i0 = kDataC8810;
        i0 = LoadU32(ctx, i0);
        i1 = kDataC8810;
        i2 = l5;
        i0 = i2 ? i0 : i1;
        l5 = i0;
        i0 = LoadU8(ctx, i0);
        l6 = i0;
        i0 = length;
        l7 = i0;
        loop_160: 
          i0 = l3;
          i1 = out;
          i0 -= i1;
          l3 = i0;
          i1 = 1u;
          i0 += i1;
          l8 = i0;
          i0 = l3;
          i1 = l8;
          i0 = i0 > i1;
          if (i0) {goto block_66;}
          i0 = l7;
          i1 = l6;
          i2 = l8;
          i0 = MemChr(ctx, i0, i1, i2);
          l3 = i0;
          i0 = !(i0);
          if (i0) {goto block_66;}
          i0 = l3;
          i1 = l5;
          i2 = out;
          i0 = CompareBytes(ctx, i0, i1, i2);
          if (i0) {
            i0 = l4;
            i1 = l3;
            i2 = 1u;
            i1 += i2;
            l7 = i1;
            i0 -= i1;
            l3 = i0;
            i1 = out;
            i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
            if (i0) {goto loop_160;}
            goto block_66;
          }
        i0 = l3;
        i1 = l4;
        i0 = i0 == i1;
        if (i0) {goto block_66;}
        i0 = l3;
        i1 = length;
        i0 -= i1;
        i1 = kDataFFFFFFFF;
        i0 = i0 == i1;
        if (i0) {goto block_66;}
      }
      i0 = data;
      j1 = 17179869188ull;
      StoreU64(ctx, i0, j1);
      goto block_5;
    }
    block_72:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l24;
      i1 = l12;
      i2 = kDataFFF9F6C6;
      i1 *= i2;
      i0 += i1;
      out = i0;
      i0 = l20;
      i1 = l17;
      i2 = kDataFFF9F6C6;
      i1 *= i2;
      i0 += i1;
      l3 = i0;
      i1 = l23;
      i2 = l18;
      i3 = kDataFFF9F6C6;
      i2 *= i3;
      i1 += i2;
      l4 = i1;
      i0 = i0 != i1;
      if (i0) {goto block_163;}
      i0 = out;
      i1 = l3;
      i0 = i0 != i1;
      if (i0) {goto block_163;}
      i0 = length;
      i0 = LoadS8(ctx, i0 + 11u);
      l8 = i0;
      i1 = 255u;
      i0 &= i1;
      l7 = i0;
      i0 = l8;
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = length;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i0 = length;
        i0 = LoadU32(ctx, i0 + 4u);
        goto block_164;
      }
      i0 = length;
      l4 = i0;
      i0 = l7;
      block_164:;
      out = i0;
      i1 = 1u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      if (i0) {goto block_67;}
      i0 = out;
      i1 = l4;
      i0 += i1;
      l5 = i0;
      i0 = l4;
      l3 = i0;
      loop_166: 
        i0 = l3;
        i1 = 10u;
        i2 = out;
        i0 = MemChr(ctx, i0, i1, i2);
        out = i0;
        i0 = !(i0);
        if (i0) {goto block_67;}
        i0 = out;
        i0 = LoadU8(ctx, i0);
        i1 = 10u;
        i0 = i0 != i1;
        if (i0) {
          i0 = l5;
          i1 = out;
          i2 = 1u;
          i1 += i2;
          l3 = i1;
          i0 -= i1;
          out = i0;
          i1 = 1u;
          i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
          if (i0) {goto loop_166;}
          goto block_67;
        }
      i0 = out;
      i1 = l5;
      i0 = i0 == i1;
      if (i0) {goto block_67;}
      i0 = out;
      i1 = l4;
      i0 -= i1;
      i1 = kDataFFFFFFFF;
      i0 = i0 == i1;
      if (i0) {goto block_67;}
      i0 = l8;
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = length;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i0 = length;
        i0 = LoadU32(ctx, i0 + 4u);
        goto block_170;
      }
      i0 = length;
      l4 = i0;
      i0 = l7;
      block_170:;
      out = i0;
      i1 = 2u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      if (i0) {goto block_169;}
      i0 = out;
      i1 = l4;
      i0 += i1;
      l5 = i0;
      i0 = l4;
      l3 = i0;
      loop_172: 
        i0 = l3;
        i1 = 13u;
        i2 = out;
        i3 = 1u;
        i2 -= i3;
        i0 = MemChr(ctx, i0, i1, i2);
        out = i0;
        i0 = !(i0);
        if (i0) {goto block_169;}
        i0 = out;
        i0 = LoadU16(ctx, i0);
        i1 = 2573u;
        i0 = i0 != i1;
        if (i0) {
          i0 = l5;
          i1 = out;
          i2 = 1u;
          i1 += i2;
          l3 = i1;
          i0 -= i1;
          out = i0;
          i1 = 2u;
          i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
          if (i0) {goto loop_172;}
          goto block_169;
        }
      i0 = out;
      i1 = l5;
      i0 = i0 == i1;
      if (i0) {goto block_169;}
      i0 = out;
      i1 = l4;
      i0 -= i1;
      i1 = kDataFFFFFFFF;
      i0 = i0 != i1;
      if (i0) {goto block_168;}
      block_169:;
      i0 = l8;
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = length;
        i0 = LoadU32(ctx, i0 + 4u);
        l7 = i0;
        i0 = length;
        i0 = LoadU32(ctx, i0);
        length = i0;
      }
      i0 = l7;
      i1 = 2u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      if (i0) {goto block_70;}
      i0 = length;
      i1 = l7;
      i0 += i1;
      l3 = i0;
      i0 = length;
      out = i0;
      loop_175: 
        i0 = out;
        i1 = 10u;
        i2 = l7;
        i3 = 1u;
        i2 -= i3;
        i0 = MemChr(ctx, i0, i1, i2);
        out = i0;
        i0 = !(i0);
        if (i0) {goto block_70;}
        i0 = out;
        i0 = LoadU16(ctx, i0);
        i1 = 3338u;
        i0 = i0 != i1;
        if (i0) {
          i0 = l3;
          i1 = out;
          i2 = 1u;
          i1 += i2;
          out = i1;
          i0 -= i1;
          l7 = i0;
          i1 = 2u;
          i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
          if (i0) {goto loop_175;}
          goto block_70;
        }
      i0 = out;
      i1 = l3;
      i0 = i0 == i1;
      if (i0) {goto block_70;}
      i0 = out;
      i1 = length;
      i0 -= i1;
      i1 = kDataFFFFFFFF;
      i0 = i0 == i1;
      length = i0;
      if (i0) {goto block_70;}
      block_168:;
      i0 = data;
      j1 = 12884901891ull;
      StoreU64(ctx, i0, j1);
      goto block_5;
      block_163:;
      i0 = l3;
      i1 = l4;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      if (i0) {
        i0 = out;
        i1 = l3;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = data;
          i1 = 3u;
          StoreU32(ctx, i0, i1);
          i0 = data;
          i1 = 3u;
          i2 = 1u;
          i3 = l19;
          i4 = l17;
          i5 = 100u;
          i4 *= i5;
          i3 = i3 > i4;
          i1 = i3 ? i1 : i2;
          StoreU32(ctx, i0 + 4u, i1);
          goto block_5;
        }
        i0 = data;
        i1 = 7u;
        StoreU32(ctx, i0, i1);
        i0 = data;
        i1 = 7u;
        i2 = 1u;
        i3 = l19;
        i4 = l12;
        i5 = 100u;
        i4 *= i5;
        i3 = i3 > i4;
        i1 = i3 ? i1 : i2;
        StoreU32(ctx, i0 + 4u, i1);
        goto block_5;
      }
      i0 = out;
      i1 = l4;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = data;
        i1 = 5u;
        StoreU32(ctx, i0, i1);
        i0 = data;
        i1 = 5u;
        i2 = 1u;
        i3 = l19;
        i4 = l18;
        i5 = 100u;
        i4 *= i5;
        i3 = i3 > i4;
        i1 = i3 ? i1 : i2;
        StoreU32(ctx, i0 + 4u, i1);
        goto block_5;
      }
      i0 = data;
      i1 = 7u;
      StoreU32(ctx, i0, i1);
      i0 = data;
      i1 = 7u;
      i2 = 1u;
      i3 = l19;
      i4 = l12;
      i5 = 100u;
      i4 *= i5;
      i3 = i3 > i4;
      i1 = i3 ? i1 : i2;
      StoreU32(ctx, i0 + 4u, i1);
      goto block_5;
    }
    block_71:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i1 = l6;
      i2 = 1u;
      i1 += i2;
      l6 = i1;
      i0 = i0 != i1;
      out = i0;
      if (i0) {goto loop_68;}
      goto block_6;
    }
    block_70:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = data;
    j1 = 21474836485ull;
    StoreU64(ctx, i0, j1);
    goto block_5;
  }
  block_67:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l8;
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = length;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = length;
      i0 = LoadU32(ctx, i0 + 4u);
      goto block_184;
    }
    i0 = length;
    l4 = i0;
    i0 = l7;
    block_184:;
    out = i0;
    i1 = 1u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {goto block_183;}
    i0 = out;
    i1 = l4;
    i0 += i1;
    l5 = i0;
    i0 = l4;
    l3 = i0;
    loop_186: 
      i0 = l3;
      i1 = 13u;
      i2 = out;
      i0 = MemChr(ctx, i0, i1, i2);
      out = i0;
      i0 = !(i0);
      if (i0) {goto block_183;}
      i0 = out;
      i0 = LoadU8(ctx, i0);
      i1 = 13u;
      i0 = i0 != i1;
      if (i0) {
        i0 = l5;
        i1 = out;
        i2 = 1u;
        i1 += i2;
        l3 = i1;
        i0 -= i1;
        out = i0;
        i1 = 1u;
        i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
        if (i0) {goto loop_186;}
        goto block_183;
      }
    i0 = out;
    i1 = l5;
    i0 = i0 == i1;
    if (i0) {goto block_183;}
    i0 = out;
    i1 = l4;
    i0 -= i1;
    i1 = kDataFFFFFFFF;
    i0 = i0 == i1;
    if (i0) {goto block_183;}
    i0 = data;
    j1 = 12884901891ull;
    StoreU64(ctx, i0, j1);
    goto block_5;
    block_183:;
    i0 = l8;
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = length;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = length;
      i0 = LoadU32(ctx, i0 + 4u);
      goto block_188;
    }
    i0 = length;
    l4 = i0;
    i0 = l7;
    block_188:;
    out = i0;
    i1 = l4;
    i0 += i1;
    l5 = i0;
    i0 = out;
    i1 = 5u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {goto block_191;}
    i0 = l4;
    l3 = i0;
    loop_192: 
      i0 = l3;
      i1 = 85u;
      i2 = out;
      i3 = 4u;
      i2 -= i3;
      i0 = MemChr(ctx, i0, i1, i2);
      out = i0;
      i0 = !(i0);
      if (i0) {goto block_191;}
      i0 = out;
      i1 = kMsgUtf;
      i2 = 5u;
      i0 = CompareBytes(ctx, i0, i1, i2);
      i0 = !(i0);
      if (i0) {goto block_190;}
      i0 = l5;
      i1 = out;
      i2 = 1u;
      i1 += i2;
      l3 = i1;
      i0 -= i1;
      out = i0;
      i1 = 4u;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      if (i0) {goto loop_192;}
    block_191:;
    i0 = l5;
    out = i0;
    block_190:;
    i0 = out;
    i1 = l5;
    i0 = i0 == i1;
    l10 = i0;
    i0 = out;
    i1 = l4;
    i0 -= i1;
    l13 = i0;
    i0 = l8;
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = length;
      i0 = LoadU32(ctx, i0 + 4u);
      l3 = i0;
      i1 = length;
      i1 = LoadU32(ctx, i1);
      l6 = i1;
      i0 += i1;
      l11 = i0;
      i0 = l3;
      i1 = 9u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      if (i0) {goto block_200;}
      i0 = l3;
      l5 = i0;
      i0 = l6;
      l4 = i0;
      loop_201: 
        i0 = l4;
        i1 = 83u;
        i2 = l5;
        i3 = 8u;
        i2 -= i3;
        i0 = MemChr(ctx, i0, i1, i2);
        out = i0;
        i0 = !(i0);
        if (i0) {goto block_200;}
        i0 = out;
        i1 = kMsgShiftJis;
        i2 = 9u;
        i0 = CompareBytes(ctx, i0, i1, i2);
        if (i0) {
          i0 = l11;
          i1 = out;
          i2 = 1u;
          i1 += i2;
          l4 = i1;
          i0 -= i1;
          l5 = i0;
          i1 = 9u;
          i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
          if (i0) {goto loop_201;}
          goto block_200;
        }
      i0 = out;
      i1 = l11;
      i0 = i0 == i1;
      if (i0) {goto block_200;}
      i0 = out;
      i1 = l6;
      i0 -= i1;
      l4 = i0;
      i1 = kDataFFFFFFFF;
      i0 = i0 != i1;
      if (i0) {goto block_199;}
      block_200:;
      i0 = kDataFFFFFFFF;
      l4 = i0;
      block_199:;
      i0 = l3;
      i1 = 9u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      if (i0) {goto block_196;}
      i0 = l3;
      l5 = i0;
      i0 = l6;
      out = i0;
      loop_203: 
        i0 = out;
        i1 = 115u;
        i2 = l5;
        i3 = 8u;
        i2 -= i3;
        i0 = MemChr(ctx, i0, i1, i2);
        out = i0;
        i0 = !(i0);
        if (i0) {goto block_196;}
        i0 = out;
        i1 = kMsgShiftJisX;
        i2 = 9u;
        i0 = CompareBytes(ctx, i0, i1, i2);
        i0 = !(i0);
        if (i0) {goto block_197;}
        i0 = l11;
        i1 = out;
        i2 = 1u;
        i1 += i2;
        out = i1;
        i0 -= i1;
        l5 = i0;
        i1 = 9u;
        i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
        if (i0) {goto loop_203;}
      goto block_196;
    }
    i0 = length;
    i1 = l7;
    i0 += i1;
    out = i0;
    i0 = l8;
    i1 = 255u;
    i0 &= i1;
    i1 = 9u;
    i0 = i0 < i1;
    if (i0) {goto block_205;}
    i0 = l7;
    l3 = i0;
    i0 = length;
    l5 = i0;
    loop_206: 
      i0 = l3;
      i1 = 8u;
      i0 -= i1;
      l3 = i0;
      i0 = !(i0);
      if (i0) {goto block_205;}
      i0 = l5;
      i1 = 83u;
      i2 = l3;
      i0 = MemChr(ctx, i0, i1, i2);
      l3 = i0;
      i0 = !(i0);
      if (i0) {goto block_205;}
      i0 = l3;
      i1 = kMsgShiftJis;
      i2 = 9u;
      i0 = CompareBytes(ctx, i0, i1, i2);
      if (i0) {
        i0 = out;
        i1 = l3;
        i2 = 1u;
        i1 += i2;
        l5 = i1;
        i0 -= i1;
        l3 = i0;
        i1 = 9u;
        i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
        if (i0) {goto loop_206;}
        goto block_205;
      }
    i0 = out;
    i1 = l3;
    i0 = i0 == i1;
    if (i0) {goto block_205;}
    i0 = l3;
    i1 = length;
    i0 -= i1;
    l4 = i0;
    i1 = kDataFFFFFFFF;
    i0 = i0 != i1;
    if (i0) {goto block_204;}
    block_205:;
    i0 = kDataFFFFFFFF;
    l4 = i0;
    block_204:;
    i0 = l8;
    i1 = 255u;
    i0 &= i1;
    i1 = 9u;
    i0 = i0 < i1;
    if (i0) {goto block_194;}
    i0 = l7;
    l3 = i0;
    i0 = length;
    l5 = i0;
    loop_208: 
      i0 = l3;
      i1 = 8u;
      i0 -= i1;
      l3 = i0;
      i0 = !(i0);
      if (i0) {goto block_194;}
      i0 = l5;
      i1 = 115u;
      i2 = l3;
      i0 = MemChr(ctx, i0, i1, i2);
      l3 = i0;
      i0 = !(i0);
      if (i0) {goto block_194;}
      i0 = l3;
      i1 = kMsgShiftJisX;
      i2 = 9u;
      i0 = CompareBytes(ctx, i0, i1, i2);
      i0 = !(i0);
      if (i0) {goto block_195;}
      i0 = out;
      i1 = l3;
      i2 = 1u;
      i1 += i2;
      l5 = i1;
      i0 -= i1;
      l3 = i0;
      i1 = 9u;
      i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
      if (i0) {goto loop_208;}
    goto block_194;
    block_197:;
    i0 = out;
    i1 = l11;
    i0 = i0 == i1;
    if (i0) {goto block_196;}
    i0 = out;
    i1 = l6;
    i0 -= i1;
    out = i0;
    i1 = kDataFFFFFFFF;
    i0 = i0 == i1;
    if (i0) {goto block_196;}
    i0 = out;
    i1 = out;
    i2 = l4;
    i3 = out;
    i4 = l4;
    i3 = i3 < i4;
    i1 = i3 ? i1 : i2;
    i2 = l4;
    i3 = kDataFFFFFFFF;
    i2 = i2 == i3;
    i0 = i2 ? i0 : i1;
    l4 = i0;
    block_196:;
    i0 = l3;
    i1 = 4u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {goto block_209;}
    i0 = l3;
    l5 = i0;
    i0 = l6;
    out = i0;
    loop_210: 
      i0 = out;
      i1 = 115u;
      i2 = l5;
      i3 = 3u;
      i2 -= i3;
      i0 = MemChr(ctx, i0, i1, i2);
      out = i0;
      i0 = !(i0);
      if (i0) {goto block_209;}
      i0 = out;
      i0 = LoadU32(ctx, i0);
      i1 = kData73696A73;
      i0 = i0 != i1;
      if (i0) {
        i0 = l11;
        i1 = out;
        i2 = 1u;
        i1 += i2;
        out = i1;
        i0 -= i1;
        l5 = i0;
        i1 = 4u;
        i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
        if (i0) {goto loop_210;}
        goto block_209;
      }
    i0 = out;
    i1 = l11;
    i0 = i0 == i1;
    if (i0) {goto block_209;}
    i0 = out;
    i1 = l6;
    i0 -= i1;
    out = i0;
    i1 = kDataFFFFFFFF;
    i0 = i0 == i1;
    if (i0) {goto block_209;}
    i0 = out;
    i1 = out;
    i2 = l4;
    i3 = out;
    i4 = l4;
    i3 = i3 < i4;
    i1 = i3 ? i1 : i2;
    i2 = l4;
    i3 = kDataFFFFFFFF;
    i2 = i2 == i3;
    i0 = i2 ? i0 : i1;
    l4 = i0;
    block_209:;
    i0 = l3;
    i1 = 9u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {goto block_212;}
    i0 = l3;
    l5 = i0;
    i0 = l6;
    out = i0;
    loop_213: 
      i0 = out;
      i1 = 83u;
      i2 = l5;
      i3 = 8u;
      i2 -= i3;
      i0 = MemChr(ctx, i0, i1, i2);
      out = i0;
      i0 = !(i0);
      if (i0) {goto block_212;}
      i0 = out;
      i1 = kMsgShiftJisXX;
      i2 = 9u;
      i0 = CompareBytes(ctx, i0, i1, i2);
      if (i0) {
        i0 = l11;
        i1 = out;
        i2 = 1u;
        i1 += i2;
        out = i1;
        i0 -= i1;
        l5 = i0;
        i1 = 9u;
        i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
        if (i0) {goto loop_213;}
        goto block_212;
      }
    i0 = out;
    i1 = l11;
    i0 = i0 == i1;
    if (i0) {goto block_212;}
    i0 = out;
    i1 = l6;
    i0 -= i1;
    out = i0;
    i1 = kDataFFFFFFFF;
    i0 = i0 == i1;
    if (i0) {goto block_212;}
    i0 = out;
    i1 = out;
    i2 = l4;
    i3 = out;
    i4 = l4;
    i3 = i3 < i4;
    i1 = i3 ? i1 : i2;
    i2 = l4;
    i3 = kDataFFFFFFFF;
    i2 = i2 == i3;
    i0 = i2 ? i0 : i1;
    l4 = i0;
    block_212:;
    i0 = l3;
    i1 = 9u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {goto block_215;}
    i0 = l3;
    l5 = i0;
    i0 = l6;
    out = i0;
    loop_216: 
      i0 = out;
      i1 = 115u;
      i2 = l5;
      i3 = 8u;
      i2 -= i3;
      i0 = MemChr(ctx, i0, i1, i2);
      out = i0;
      i0 = !(i0);
      if (i0) {goto block_215;}
      i0 = out;
      i1 = kMsgShiftJisXXX;
      i2 = 9u;
      i0 = CompareBytes(ctx, i0, i1, i2);
      if (i0) {
        i0 = l11;
        i1 = out;
        i2 = 1u;
        i1 += i2;
        out = i1;
        i0 -= i1;
        l5 = i0;
        i1 = 9u;
        i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
        if (i0) {goto loop_216;}
        goto block_215;
      }
    i0 = out;
    i1 = l11;
    i0 = i0 == i1;
    if (i0) {goto block_215;}
    i0 = out;
    i1 = l6;
    i0 -= i1;
    out = i0;
    i1 = kDataFFFFFFFF;
    i0 = i0 == i1;
    if (i0) {goto block_215;}
    i0 = out;
    i1 = out;
    i2 = l4;
    i3 = out;
    i4 = l4;
    i3 = i3 < i4;
    i1 = i3 ? i1 : i2;
    i2 = l4;
    i3 = kDataFFFFFFFF;
    i2 = i2 == i3;
    i0 = i2 ? i0 : i1;
    l4 = i0;
    block_215:;
    i0 = l3;
    i1 = 4u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {goto block_193;}
    i0 = l6;
    l5 = i0;
    loop_218: 
      i0 = l5;
      i1 = 83u;
      i2 = l3;
      i3 = 3u;
      i2 -= i3;
      i0 = MemChr(ctx, i0, i1, i2);
      out = i0;
      i0 = !(i0);
      if (i0) {goto block_193;}
      i0 = out;
      i0 = LoadU32(ctx, i0);
      i1 = kData53494A53;
      i0 = i0 != i1;
      if (i0) {
        i0 = l11;
        i1 = out;
        i2 = 1u;
        i1 += i2;
        l5 = i1;
        i0 -= i1;
        l3 = i0;
        i1 = 4u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        if (i0) {goto block_193;}
        goto loop_218;
      }
    i0 = out;
    i1 = l11;
    i0 = i0 == i1;
    if (i0) {goto block_193;}
    i0 = out;
    i1 = l6;
    i0 -= i1;
    out = i0;
    i1 = kDataFFFFFFFF;
    i0 = i0 == i1;
    if (i0) {goto block_193;}
    i0 = out;
    i1 = out;
    i2 = l4;
    i3 = out;
    i4 = l4;
    i3 = i3 < i4;
    i1 = i3 ? i1 : i2;
    i2 = l4;
    i3 = kDataFFFFFFFF;
    i2 = i2 == i3;
    i0 = i2 ? i0 : i1;
    l4 = i0;
    goto block_193;
    block_195:;
    i0 = out;
    i1 = l3;
    i0 = i0 == i1;
    if (i0) {goto block_194;}
    i0 = l3;
    i1 = length;
    i0 -= i1;
    l3 = i0;
    i1 = kDataFFFFFFFF;
    i0 = i0 == i1;
    if (i0) {goto block_194;}
    i0 = l3;
    i1 = l3;
    i2 = l4;
    i3 = l3;
    i4 = l4;
    i3 = i3 < i4;
    i1 = i3 ? i1 : i2;
    i2 = l4;
    i3 = kDataFFFFFFFF;
    i2 = i2 == i3;
    i0 = i2 ? i0 : i1;
    l4 = i0;
    block_194:;
    i0 = l8;
    i1 = 255u;
    i0 &= i1;
    i1 = 4u;
    i0 = i0 < i1;
    if (i0) {goto block_220;}
    i0 = l7;
    l3 = i0;
    i0 = length;
    l5 = i0;
    loop_221: 
      i0 = l3;
      i1 = 3u;
      i0 -= i1;
      l3 = i0;
      i0 = !(i0);
      if (i0) {goto block_220;}
      i0 = l5;
      i1 = 115u;
      i2 = l3;
      i0 = MemChr(ctx, i0, i1, i2);
      l3 = i0;
      i0 = !(i0);
      if (i0) {goto block_220;}
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      i1 = kData73696A73;
      i0 = i0 != i1;
      if (i0) {
        i0 = out;
        i1 = l3;
        i2 = 1u;
        i1 += i2;
        l5 = i1;
        i0 -= i1;
        l3 = i0;
        i1 = 4u;
        i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
        if (i0) {goto loop_221;}
        goto block_220;
      }
    i0 = out;
    i1 = l3;
    i0 = i0 == i1;
    if (i0) {goto block_220;}
    i0 = l3;
    i1 = length;
    i0 -= i1;
    l3 = i0;
    i1 = kDataFFFFFFFF;
    i0 = i0 == i1;
    if (i0) {goto block_220;}
    i0 = l3;
    i1 = l3;
    i2 = l4;
    i3 = l3;
    i4 = l4;
    i3 = i3 < i4;
    i1 = i3 ? i1 : i2;
    i2 = l4;
    i3 = kDataFFFFFFFF;
    i2 = i2 == i3;
    i0 = i2 ? i0 : i1;
    l4 = i0;
    block_220:;
    i0 = l8;
    i1 = 255u;
    i0 &= i1;
    i1 = 9u;
    i0 = i0 < i1;
    if (i0) {goto block_223;}
    i0 = l7;
    l3 = i0;
    i0 = length;
    l5 = i0;
    loop_224: 
      i0 = l3;
      i1 = 8u;
      i0 -= i1;
      l3 = i0;
      i0 = !(i0);
      if (i0) {goto block_223;}
      i0 = l5;
      i1 = 83u;
      i2 = l3;
      i0 = MemChr(ctx, i0, i1, i2);
      l3 = i0;
      i0 = !(i0);
      if (i0) {goto block_223;}
      i0 = l3;
      i1 = kMsgShiftJisXX;
      i2 = 9u;
      i0 = CompareBytes(ctx, i0, i1, i2);
      if (i0) {
        i0 = out;
        i1 = l3;
        i2 = 1u;
        i1 += i2;
        l5 = i1;
        i0 -= i1;
        l3 = i0;
        i1 = 9u;
        i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
        if (i0) {goto loop_224;}
        goto block_223;
      }
    i0 = out;
    i1 = l3;
    i0 = i0 == i1;
    if (i0) {goto block_223;}
    i0 = l3;
    i1 = length;
    i0 -= i1;
    l3 = i0;
    i1 = kDataFFFFFFFF;
    i0 = i0 == i1;
    if (i0) {goto block_223;}
    i0 = l3;
    i1 = l3;
    i2 = l4;
    i3 = l3;
    i4 = l4;
    i3 = i3 < i4;
    i1 = i3 ? i1 : i2;
    i2 = l4;
    i3 = kDataFFFFFFFF;
    i2 = i2 == i3;
    i0 = i2 ? i0 : i1;
    l4 = i0;
    block_223:;
    i0 = l8;
    i1 = 255u;
    i0 &= i1;
    i1 = 9u;
    i0 = i0 < i1;
    if (i0) {goto block_226;}
    i0 = l7;
    l3 = i0;
    i0 = length;
    l5 = i0;
    loop_227: 
      i0 = l3;
      i1 = 8u;
      i0 -= i1;
      l3 = i0;
      i0 = !(i0);
      if (i0) {goto block_226;}
      i0 = l5;
      i1 = 115u;
      i2 = l3;
      i0 = MemChr(ctx, i0, i1, i2);
      l3 = i0;
      i0 = !(i0);
      if (i0) {goto block_226;}
      i0 = l3;
      i1 = kMsgShiftJisXXX;
      i2 = 9u;
      i0 = CompareBytes(ctx, i0, i1, i2);
      if (i0) {
        i0 = out;
        i1 = l3;
        i2 = 1u;
        i1 += i2;
        l5 = i1;
        i0 -= i1;
        l3 = i0;
        i1 = 9u;
        i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
        if (i0) {goto loop_227;}
        goto block_226;
      }
    i0 = out;
    i1 = l3;
    i0 = i0 == i1;
    if (i0) {goto block_226;}
    i0 = l3;
    i1 = length;
    i0 -= i1;
    l3 = i0;
    i1 = kDataFFFFFFFF;
    i0 = i0 == i1;
    if (i0) {goto block_226;}
    i0 = l3;
    i1 = l3;
    i2 = l4;
    i3 = l3;
    i4 = l4;
    i3 = i3 < i4;
    i1 = i3 ? i1 : i2;
    i2 = l4;
    i3 = kDataFFFFFFFF;
    i2 = i2 == i3;
    i0 = i2 ? i0 : i1;
    l4 = i0;
    block_226:;
    i0 = l8;
    i1 = 255u;
    i0 &= i1;
    i1 = 4u;
    i0 = i0 < i1;
    if (i0) {goto block_193;}
    i0 = l7;
    l3 = i0;
    i0 = length;
    l5 = i0;
    loop_229: 
      i0 = l3;
      i1 = 3u;
      i0 -= i1;
      l3 = i0;
      i0 = !(i0);
      if (i0) {goto block_193;}
      i0 = l5;
      i1 = 83u;
      i2 = l3;
      i0 = MemChr(ctx, i0, i1, i2);
      l3 = i0;
      i0 = !(i0);
      if (i0) {goto block_193;}
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      i1 = kData53494A53;
      i0 = i0 != i1;
      if (i0) {
        i0 = out;
        i1 = l3;
        i2 = 1u;
        i1 += i2;
        l5 = i1;
        i0 -= i1;
        l3 = i0;
        i1 = 4u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        if (i0) {goto block_193;}
        goto loop_229;
      }
    i0 = out;
    i1 = l3;
    i0 = i0 == i1;
    if (i0) {goto block_193;}
    i0 = l3;
    i1 = length;
    i0 -= i1;
    out = i0;
    i1 = kDataFFFFFFFF;
    i0 = i0 == i1;
    if (i0) {goto block_193;}
    i0 = out;
    i1 = out;
    i2 = l4;
    i3 = out;
    i4 = l4;
    i3 = i3 < i4;
    i1 = i3 ? i1 : i2;
    i2 = l4;
    i3 = kDataFFFFFFFF;
    i2 = i2 == i3;
    i0 = i2 ? i0 : i1;
    l4 = i0;
    block_193:;
    i0 = kDataFFFFFFFF;
    i1 = l13;
    i2 = l10;
    i0 = i2 ? i0 : i1;
    l11 = i0;
    i0 = l8;
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = length;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = l3;
      i1 = length;
      i1 = LoadU32(ctx, i1 + 4u);
      length = i1;
      i0 += i1;
      l6 = i0;
      i0 = length;
      i1 = 3u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      if (i0) {goto block_236;}
      i0 = length;
      out = i0;
      i0 = l3;
      l7 = i0;
      loop_237: 
        i0 = l7;
        i1 = 69u;
        i2 = out;
        i3 = 2u;
        i2 -= i3;
        i0 = MemChr(ctx, i0, i1, i2);
        out = i0;
        i0 = !(i0);
        if (i0) {goto block_236;}
        i0 = out;
        i1 = kData2FBB9;
        i2 = 3u;
        i0 = CompareBytes(ctx, i0, i1, i2);
        if (i0) {
          i0 = l6;
          i1 = out;
          i2 = 1u;
          i1 += i2;
          l7 = i1;
          i0 -= i1;
          out = i0;
          i1 = 3u;
          i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
          if (i0) {goto loop_237;}
          goto block_236;
        }
      i0 = out;
      i1 = l6;
      i0 = i0 == i1;
      if (i0) {goto block_236;}
      i0 = out;
      i1 = l3;
      i0 -= i1;
      l5 = i0;
      i1 = kDataFFFFFFFF;
      i0 = i0 != i1;
      if (i0) {goto block_235;}
      block_236:;
      i0 = kDataFFFFFFFF;
      l5 = i0;
      block_235:;
      i0 = length;
      i1 = 3u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      if (i0) {goto block_231;}
      i0 = l3;
      out = i0;
      loop_239: 
        i0 = out;
        i1 = 101u;
        i2 = length;
        i3 = 2u;
        i2 -= i3;
        i0 = MemChr(ctx, i0, i1, i2);
        length = i0;
        i0 = !(i0);
        if (i0) {goto block_231;}
        i0 = length;
        i1 = kData2FBBD;
        i2 = 3u;
        i0 = CompareBytes(ctx, i0, i1, i2);
        i0 = !(i0);
        if (i0) {goto block_233;}
        i0 = l6;
        i1 = length;
        i2 = 1u;
        i1 += i2;
        out = i1;
        i0 -= i1;
        length = i0;
        i1 = 3u;
        i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
        if (i0) {goto loop_239;}
      goto block_231;
    }
    i0 = length;
    i1 = l7;
    i0 += i1;
    l3 = i0;
    i0 = l8;
    i1 = 255u;
    i0 &= i1;
    i1 = 3u;
    i0 = i0 < i1;
    if (i0) {goto block_241;}
    i0 = l7;
    out = i0;
    i0 = length;
    l5 = i0;
    loop_242: 
      i0 = out;
      i1 = 2u;
      i0 -= i1;
      out = i0;
      i0 = !(i0);
      if (i0) {goto block_241;}
      i0 = l5;
      i1 = 69u;
      i2 = out;
      i0 = MemChr(ctx, i0, i1, i2);
      out = i0;
      i0 = !(i0);
      if (i0) {goto block_241;}
      i0 = out;
      i1 = kData2FBB9;
      i2 = 3u;
      i0 = CompareBytes(ctx, i0, i1, i2);
      if (i0) {
        i0 = l3;
        i1 = out;
        i2 = 1u;
        i1 += i2;
        l5 = i1;
        i0 -= i1;
        out = i0;
        i1 = 3u;
        i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
        if (i0) {goto loop_242;}
        goto block_241;
      }
    i0 = out;
    i1 = l3;
    i0 = i0 == i1;
    if (i0) {goto block_241;}
    i0 = out;
    i1 = length;
    i0 -= i1;
    l5 = i0;
    i1 = kDataFFFFFFFF;
    i0 = i0 != i1;
    if (i0) {goto block_240;}
    block_241:;
    i0 = kDataFFFFFFFF;
    l5 = i0;
    block_240:;
    i0 = l8;
    i1 = 255u;
    i0 &= i1;
    i1 = 3u;
    i0 = i0 >= i1;
    if (i0) {goto block_232;}
    goto block_231;
    block_233:;
    i0 = length;
    i1 = l6;
    i0 = i0 == i1;
    if (i0) {goto block_231;}
    i0 = length;
    i1 = l3;
    i0 -= i1;
    length = i0;
    i1 = kDataFFFFFFFF;
    i0 = i0 == i1;
    if (i0) {goto block_231;}
    i0 = length;
    i1 = length;
    i2 = l5;
    i3 = length;
    i4 = l5;
    i3 = i3 < i4;
    i1 = i3 ? i1 : i2;
    i2 = l5;
    i3 = kDataFFFFFFFF;
    i2 = i2 == i3;
    i0 = i2 ? i0 : i1;
    l5 = i0;
    goto block_231;
    block_232:;
    i0 = length;
    out = i0;
    loop_244: 
      i0 = l7;
      i1 = 2u;
      i0 -= i1;
      l7 = i0;
      i0 = !(i0);
      if (i0) {goto block_231;}
      i0 = out;
      i1 = 101u;
      i2 = l7;
      i0 = MemChr(ctx, i0, i1, i2);
      out = i0;
      i0 = !(i0);
      if (i0) {goto block_231;}
      i0 = out;
      i1 = kData2FBBD;
      i2 = 3u;
      i0 = CompareBytes(ctx, i0, i1, i2);
      if (i0) {
        i0 = l3;
        i1 = out;
        i2 = 1u;
        i1 += i2;
        out = i1;
        i0 -= i1;
        l7 = i0;
        i1 = 3u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        if (i0) {goto block_231;}
        goto loop_244;
      }
    i0 = out;
    i1 = l3;
    i0 = i0 == i1;
    if (i0) {goto block_231;}
    i0 = out;
    i1 = length;
    i0 -= i1;
    length = i0;
    i1 = kDataFFFFFFFF;
    i0 = i0 == i1;
    if (i0) {goto block_231;}
    i0 = length;
    i1 = length;
    i2 = l5;
    i3 = length;
    i4 = l5;
    i3 = i3 < i4;
    i1 = i3 ? i1 : i2;
    i2 = l5;
    i3 = kDataFFFFFFFF;
    i2 = i2 == i3;
    i0 = i2 ? i0 : i1;
    l5 = i0;
    block_231:;
    i0 = l11;
    i1 = kDataFFFFFFFF;
    i0 = i0 == i1;
    if (i0) {goto block_250;}
    i0 = l4;
    i1 = kDataFFFFFFFF;
    i0 = i0 != i1;
    i1 = 0u;
    i2 = l4;
    i3 = l11;
    i2 = i2 <= i3;
    i0 = i2 ? i0 : i1;
    if (i0) {goto block_249;}
    i0 = l5;
    i1 = kDataFFFFFFFF;
    i0 = i0 != i1;
    i1 = 0u;
    i2 = l5;
    i3 = l11;
    i2 = i2 <= i3;
    i0 = i2 ? i0 : i1;
    if (i0) {goto block_250;}
    i0 = data;
    j1 = 30064771079ull;
    StoreU64(ctx, i0, j1);
    goto block_5;
    block_250:;
    i0 = l4;
    i1 = kDataFFFFFFFF;
    i0 = i0 == i1;
    if (i0) {goto block_248;}
    block_249:;
    i0 = l5;
    i1 = kDataFFFFFFFF;
    i0 = i0 != i1;
    i1 = 0u;
    i2 = l4;
    i3 = l5;
    i2 = i2 >= i3;
    i0 = i2 ? i0 : i1;
    if (i0) {goto block_247;}
    i0 = data;
    j1 = 12884901891ull;
    StoreU64(ctx, i0, j1);
    goto block_5;
    block_248:;
    i0 = l5;
    i1 = kDataFFFFFFFF;
    i0 = i0 == i1;
    if (i0) {goto block_246;}
    block_247:;
    i0 = data;
    j1 = 21474836485ull;
    StoreU64(ctx, i0, j1);
    goto block_5;
    block_246:;
    i0 = data;
    j1 = 12884901891ull;
    StoreU64(ctx, i0, j1);
    goto block_5;
  }
  block_66:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = data;
    j1 = 8589934594ull;
    StoreU64(ctx, i0, j1);
    goto block_5;
  }
  block_65:;
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    data = i0;
    i0 = HostTempRet0(ctx);
    i0 = l15;
    i0 = LoadS8(ctx, i0 + 27u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l15;
      i0 = LoadU32(ctx, i0 + 16u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = data;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    TrapUnreachable();
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = data;
    j1 = 8589934594ull;
    StoreU64(ctx, i0, j1);
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l15;
    i0 = LoadS8(ctx, i0 + 27u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto exit_path;}
    i0 = l15;
    i0 = LoadU32(ctx, i0 + 16u);
    DeallocateRawBlock(ctx, i0);
  }
  exit_path:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l15;
    i1 = 32u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
  }
  goto func_exit;
  spill_frame:;
  l9 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l9;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l9 = i0;
  i1 = data;
  StoreU32(ctx, i0, i1);
  i0 = l9;
  i1 = length;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l9;
  i1 = out;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l9;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l9;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l9;
  i1 = l5;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l9;
  i1 = l6;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l9;
  i1 = l7;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l9;
  i1 = l8;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = l9;
  i1 = l10;
  StoreU32(ctx, i0 + 36u, i1);
  i0 = l9;
  i1 = l11;
  StoreU32(ctx, i0 + 40u, i1);
  i0 = l9;
  i1 = l12;
  StoreU32(ctx, i0 + 44u, i1);
  i0 = l9;
  i1 = l13;
  StoreU32(ctx, i0 + 48u, i1);
  i0 = l9;
  i1 = l14;
  StoreU32(ctx, i0 + 52u, i1);
  i0 = l9;
  i1 = l15;
  StoreU32(ctx, i0 + 56u, i1);
  i0 = l9;
  i1 = l17;
  StoreU32(ctx, i0 + 60u, i1);
  i0 = l9;
  i1 = l18;
  StoreU32(ctx, i0 + 64u, i1);
  i0 = l9;
  i1 = l19;
  StoreU32(ctx, i0 + 68u, i1);
  i0 = l9;
  i1 = l20;
  StoreU32(ctx, i0 + 72u, i1);
  i0 = l9;
  i1 = l21;
  StoreU32(ctx, i0 + 76u, i1);
  i0 = l9;
  i1 = l22;
  StoreU32(ctx, i0 + 80u, i1);
  i0 = l9;
  i1 = l23;
  StoreU32(ctx, i0 + 84u, i1);
  i0 = l9;
  i1 = l24;
  StoreU32(ctx, i0 + 88u, i1);
  i0 = l9;
  i1 = l25;
  StoreU32(ctx, i0 + 92u, i1);
  i0 = l9;
  i1 = l26;
  StoreU32(ctx, i0 + 96u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 100u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
}  // namespace artc
