#include "util/engine_context.h"

namespace artc {

// Forward declarations -- the sibling steps this body drives.
void ReleaseFontRecord(EngineContext* ctx, uint32_t a0);
void AccumulateGlyphBounds(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace GlyphAtlas {

// Font tag dispatch for the message layer: walks the queued tag records, matches them against
// the "print", "font", "/font", "fontinit", "annotation" and "on" tags, keeps the glyph atlas
// tables in step with the records, and releases the host slots the invocation still holds.
// It is one suspendable driver: a resume trampoline per call site, a single suspension exit at
// `suspend_point`, and a single return at `dispatch_exit`.
void FontDispatch(EngineContext* ctx, uint32_t p0, uint32_t p1, uint32_t p2, uint32_t p3,
                  uint32_t p4, uint32_t p5) {
  EnterFrame();
  constexpr uint32_t kSaveFrameBytes = 68u;  // saved state words below the resume code
  constexpr uint32_t kResumeWordBytes = 4u;  // resume code slot
  // Tag names this dispatch matches, as pointers into the module's data section.
  constexpr uint32_t kTagPrint = 0xB9B20u;            // "print"
  constexpr uint32_t kTagFont = 0xB9B26u;             // "font"
  constexpr uint32_t kTagFontEnd = 0xB9B2Bu;          // "/font"
  constexpr uint32_t kTagFontInit = 0xB9B31u;         // "fontinit"
  constexpr uint32_t kTagAnnotationText = 0xB9B3Au;   // "annotation", first eight bytes
  constexpr uint32_t kTagAnnotationTail = 0xB9B42u;   // "annotation", closing "on"
  constexpr uint32_t kEmptyName = 0xBA360u;           // ""
  constexpr uint32_t kData8A9A4 = 0x8A9A4u;
  constexpr uint32_t kDataBA2E4 = 0xBA2E4u;
  constexpr uint32_t kAssertExpression = 0xBA361u;    // px != 0
  constexpr uint32_t kAssertSource = 0xBA369u;        // boost shared_ptr header path
  constexpr uint32_t kAssertFunction = 0xBA3A1u;      // operator->
  uint32_t i0 = 0u, i1 = 0u, i2 = 0u, i3 = 0u, i4 = 0u, i5 = 0u, i6 = 0u;
  uint64_t j1 = 0u;
  float f0 = 0.0f, f1 = 0.0f, f2 = 0.0f, f3 = 0.0f;
  uint32_t l6 = 0u, l7 = 0u, l8 = 0u, l9 = 0u, l10 = 0u, l11 = 0u, l12 = 0u, l13 = 0u;
  uint32_t l14 = 0u, l15 = 0u, l16 = 0u, l17 = 0u, l18 = 0u;
  float l19 = 0.0f, l20 = 0.0f, l21 = 0.0f, l22 = 0.0f, l23 = 0.0f, l24 = 0.0f;
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
    l9 = i0;
    i0 = LoadU32(ctx, i0);
    p0 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 8u);
    p2 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 12u);
    p3 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 16u);
    p4 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 20u);
    p5 = i0;
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
    l12 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 44u);
    l13 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 48u);
    l14 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 52u);
    l15 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 56u);
    l16 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 60u);
    l17 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 64u);
    l18 = i0;
    i0 = l9;
    i0 = LoadU32(ctx, i0 + 4u);
    p1 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = 4u;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l11 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  i1 = 0u;
  i2 = ResumePhase(ctx);
  i2 = !(i2);
  if (i2) {
    i2 = GuestStackSp(ctx);
    i3 = 160u;
    i2 -= i3;
    l10 = i2;
    GuestStackSp(ctx) = i2;
    i2 = p3;
    i2 = LoadU32(ctx, i2);
    l7 = i2;
    i3 = p3;
    i3 = LoadU32(ctx, i3 + 4u);
    l8 = i3;
    i2 = i2 == i3;
    l6 = i2;
  }
  i2 = l6;
  i0 = i2 ? i0 : i1;
  if (i0) {goto restore_stack_pointer;}
  // ---- Walk the queued tag records and dispatch each one. ----
  scan_records:
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l7;
      i0 = LoadU8(ctx, i0 + 15u);
      l6 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (u32)((s32)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (u32)((s32)i0 < (s32)i1);
      l13 = i0;
      i0 = l7;
      i0 = LoadU32(ctx, i0 + 8u);
      l12 = i0;
      i1 = l6;
      i2 = l13;
      i0 = i2 ? i0 : i1;
      i1 = 5u;
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
        i0 = l7;
        i1 = 4u;
        i0 += i1;
        l6 = i0;
      }
      i0 = l11;
      i0 = !(i0);
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1354u;
        i1 = l6;
        i2 = 0u;
        i3 = kDataFFFFFFFF;
        i4 = kTagPrint;
        i5 = 5u;
        i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
        l9 = i0;
        i0 = 0u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto suspend_point;}
        i0 = l9;
        l6 = i0;
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l8;
      } else {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l8 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l8;
        i1 = 1u;
        i0 = i0 == i1;
        l8 = i0;
        if (i0) {goto resume_throw_probe;}
        i0 = l6;
        i0 = !(i0);
        l6 = i0;
        if (i0) {goto check_record_room;}
        i0 = p3;
        i0 = LoadU32(ctx, i0 + 4u);
      }
      l8 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l8;
      i1 = l7;
      i2 = 44u;
      i1 += i2;
      l7 = i1;
      i0 = i0 != i1;
      l6 = i0;
      if (i0) {goto scan_records;}
      goto restore_stack_pointer;
    }
  check_record_room:
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = p0;
    i0 = LoadU32(ctx, i0 + 508u);
    i1 = 101u;
    i0 = i0 < i1;
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    if (i0) {goto reset_font_metrics;}
    i0 = p0;
    i0 = LoadU32(ctx, i0 + 504u);
    l6 = i0;
    i1 = 68u;
    i0 = DIV_U(i0, i1);
    l8 = i0;
    i1 = 2u;
    i0 <<= (i1 & 31);
    l12 = i0;
    i1 = p0;
    i1 = LoadU32(ctx, i1 + 492u);
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l7 = i0;
    i0 = l7;
    i1 = l6;
    i2 = l8;
    i3 = 68u;
    i2 *= i3;
    i1 -= i2;
    i2 = 60u;
    i1 *= i2;
    i0 += i1;
    l6 = i0;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l8 = i0;
  }
  i0 = l11;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = l8;
    i0 = CallTableT2(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    l9 = i0;
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto suspend_point;}
    i0 = l9;
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = p0;
    i1 = p0;
    i1 = LoadU32(ctx, i1 + 508u);
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 508u, i1);
    i0 = p0;
    i1 = p0;
    i1 = LoadU32(ctx, i1 + 504u);
    i2 = 1u;
    i1 += i2;
    l6 = i1;
    StoreU32(ctx, i0 + 504u, i1);
    i0 = l6;
    i1 = 136u;
    i0 = i0 >= i1;
    if (i0) {
      i0 = p0;
      i0 = LoadU32(ctx, i0 + 492u);
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
      i0 = p0;
      i1 = p0;
      i1 = LoadU32(ctx, i1 + 492u);
      i2 = 4u;
      i1 += i2;
      StoreU32(ctx, i0 + 492u, i1);
      i0 = p0;
      i1 = p0;
      i1 = LoadU32(ctx, i1 + 504u);
      i2 = 68u;
      i1 -= i2;
      StoreU32(ctx, i0 + 504u, i1);
    }
    i0 = p0;
    i1 = 512u;
    i0 += i1;
    l6 = i0;
  }
  i0 = l11;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    ReleaseFontRecord(ctx, i0);
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto suspend_point;}
  }
  i0 = l16;
  i1 = ResumePhase(ctx);
  i1 = !(i1);
  if (i1) {
    i1 = p0;
    l13 = i1;
    i1 = p0;
    i1 = LoadU32(ctx, i1 + 552u);
    l7 = i1;
    i2 = 341u;
    i1 = DIV_U(i1, i2);
    l6 = i1;
    i2 = 2u;
    i1 <<= (i2 & 31);
    l12 = i1;
    i2 = p0;
    i2 = LoadU32(ctx, i2 + 540u);
    i1 += i2;
    i1 = LoadU32(ctx, i1);
    l8 = i1;
    i1 = l8;
    i2 = l7;
    i3 = l6;
    i4 = 341u;
    i3 *= i4;
    i2 -= i3;
    i3 = 12u;
    i2 *= i3;
    i1 += i2;
    l8 = i1;
    i1 = LoadU32(ctx, i1);
    l6 = i1;
  }
  i1 = l6;
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
      i0 = l8;
      i0 = LoadU32(ctx, i0 + 4u);
      l7 = i0;
      i1 = l6;
      i0 = i0 == i1;
      l14 = i0;
      i0 = l6;
      l12 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    i1 = 0u;
    i2 = l14;
    i0 = i2 ? i0 : i1;
    i0 = !(i0);
    if (i0) {
  release_records:
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l7;
          i1 = 44u;
          i0 -= i1;
          l7 = i0;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0);
          l12 = i0;
        }
        i0 = l11;
        i1 = 3u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l7;
          i1 = l12;
          i0 = CallTableT2(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
          SyncMemoryBase(ctx);
#endif
          l9 = i0;
          i0 = 3u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto suspend_point;}
          i0 = l9;
          l12 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l6;
          i1 = l7;
          i0 = i0 != i1;
          l12 = i0;
          if (i0) {goto release_records;}
        }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l12;
      } else {
        i0 = l8;
        i0 = LoadU32(ctx, i0);
      }
      l12 = i0;
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l7;
    } else {
      i0 = l8;
      i1 = l6;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l12;
      DeallocateRawBlock(ctx, i0);
      i0 = p0;
      i0 = LoadU32(ctx, i0 + 552u);
    }
    l7 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l13;
    i1 = l7;
    i2 = 1u;
    i1 += i2;
    l6 = i1;
    StoreU32(ctx, i0 + 552u, i1);
    i0 = p0;
    i1 = p0;
    i1 = LoadU32(ctx, i1 + 556u);
    i2 = 1u;
    i1 -= i2;
    l8 = i1;
    StoreU32(ctx, i0 + 556u, i1);
    i0 = l6;
    i1 = 682u;
    i0 = i0 < i1;
    l6 = i0;
    if (i0) {goto reset_font_metrics;}
    i0 = p0;
    i0 = LoadU32(ctx, i0 + 540u);
    i0 = LoadU32(ctx, i0);
    DeallocateRawBlock(ctx, i0);
    i0 = p0;
    i1 = p0;
    i1 = LoadU32(ctx, i1 + 540u);
    i2 = 4u;
    i1 += i2;
    StoreU32(ctx, i0 + 540u, i1);
    i0 = p0;
    i1 = p0;
    i1 = LoadU32(ctx, i1 + 552u);
    i2 = 341u;
    i1 -= i2;
    l6 = i1;
    StoreU32(ctx, i0 + 552u, i1);
  }
  // ---- Rebuild the atlas tables from scratch. ----
  reset_font_metrics:
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l10;
    j1 = 1065353216ull;
    StoreU64(ctx, i0 + 124u, j1);
    i0 = l10;
    j1 = 0ull;
    StoreU64(ctx, i0 + 116u, j1);
    i0 = l10;
    j1 = 1065353216ull;
    StoreU64(ctx, i0 + 108u, j1);
    i0 = l10;
    i1 = kData8A9A4;
    StoreU32(ctx, i0 + 104u, i1);
    i0 = l10;
    i1 = 0u;
    StoreU32(ctx, i0 + 96u, i1);
    i0 = l10;
    j1 = 0ull;
    StoreU64(ctx, i0 + 88u, j1);
    i0 = p1;
    i0 = LoadU32(ctx, i0);
    p1 = i0;
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
    i0 = l11;
    i1 = 4u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kAssertExpression;
      i2 = kAssertSource;
      i3 = 734u;
      i4 = kAssertFunction;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 4u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto suspend_point;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      p0 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = p0;
      i1 = 1u;
      i0 = i0 != i1;
      p0 = i0;
      if (i0) {goto unexpected_resume;}
      goto resume_catch_entry;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = p1;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 136u);
    l6 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l10;
    i1 = 104u;
    i0 += i1;
    l7 = i0;
    i0 = l10;
    i1 = 88u;
    i0 += i1;
    l8 = i0;
  }
  i0 = l11;
  i1 = 5u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = p1;
    i2 = l8;
    i3 = l7;
    i4 = 1u;
    HostFailAssertion(ctx, i0, i1, i2, i3, i4);
    i0 = 5u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto suspend_point;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    p1 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = p1;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto discard_pending_f;}
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l10;
    i1 = 88u;
    i0 += i1;
    i1 = l10;
    i2 = 72u;
    i1 += i2;
    l6 = i1;
    i2 = l10;
    i3 = 8u;
    i2 += i3;
    l8 = i2;
    AccumulateGlyphBounds(ctx, i0, i1, i2);
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    p1 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = p1;
    i1 = 1u;
    i0 = i0 != i1;
    p1 = i0;
  }
  i0 = p1;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l10;
      f0 = LoadF32(ctx, i0 + 72u);
      l20 = f0;
      f1 = 0.0f;
      f0 *= f1;
      i1 = l10;
      f1 = LoadF32(ctx, i1 + 76u);
      l19 = f1;
      f0 += f1;
      f1 = l19;
      f0 -= f1;
      l21 = f0;
      i1 = l10;
      f1 = LoadF32(ctx, i1 + 80u);
      l23 = f1;
      f2 = 0.0f;
      f1 *= f2;
      i2 = l10;
      f2 = LoadF32(ctx, i2 + 84u);
      l24 = f2;
      f1 += f2;
      f2 = l19;
      f1 -= f2;
      l22 = f1;
      i0 = f0 > f1;
      p1 = i0;
      i0 = l10;
      f1 = l21;
      f2 = l22;
      i3 = p1;
      f1 = i3 ? f1 : f2;
      StoreF32(ctx, i0 + 24u, f1);
      i0 = l10;
      f1 = l22;
      f2 = l21;
      i3 = p1;
      f1 = i3 ? f1 : f2;
      StoreF32(ctx, i0 + 16u, f1);
      f0 = l20;
      f1 = l19;
      f2 = 0.0f;
      f1 *= f2;
      f0 += f1;
      f1 = l20;
      f0 -= f1;
      l19 = f0;
      f0 = l19;
      f1 = l23;
      f2 = l24;
      f3 = 0.0f;
      f2 *= f3;
      f1 += f2;
      f2 = l20;
      f1 -= f2;
      l20 = f1;
      i0 = f0 > f1;
      p1 = i0;
      i0 = l10;
      f1 = l19;
      f2 = l20;
      i3 = p1;
      f1 = i3 ? f1 : f2;
      StoreF32(ctx, i0 + 20u, f1);
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l10;
      i1 = kDataBA2E4;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l10;
      f1 = l20;
      f2 = l19;
      i3 = p1;
      f1 = i3 ? f1 : f2;
      StoreF32(ctx, i0 + 12u, f1);
      i0 = l10;
      i1 = 44u;
      i0 += i1;
      p1 = i0;
    }
    i0 = l11;
    i1 = 6u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 873u;
      i1 = p1;
      i2 = p5;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l9 = i0;
      i0 = 6u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto suspend_point;}
      i0 = l9;
      l6 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      p1 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = p1;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto discard_pending_d;}
      i0 = l10;
      j1 = 0ull;
      StoreU64(ctx, i0 + 56u, j1);
      i0 = 0u;
      l7 = i0;
      i0 = l10;
      i1 = 0u;
      StoreU32(ctx, i0 + 64u, i1);
      i0 = p0;
      i0 = LoadU32(ctx, i0 + 508u);
      i1 = p0;
      i1 = LoadU32(ctx, i1 + 504u);
      i0 += i1;
      p1 = i0;
      i1 = p0;
      i1 = LoadU32(ctx, i1 + 496u);
      l13 = i1;
      i2 = p0;
      i2 = LoadU32(ctx, i2 + 492u);
      l8 = i2;
      i1 -= i2;
      l14 = i1;
      i2 = 2u;
      i1 = (u32)((s32)i1 >> (i2 & 31));
      i2 = 68u;
      i1 *= i2;
      i2 = 1u;
      i1 -= i2;
      i2 = 0u;
      i3 = l14;
      i1 = i3 ? i1 : i2;
      i0 = i0 == i1;
      l12 = i0;
    }
    i0 = l12;
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
        i0 = p0;
        i1 = 488u;
        i0 += i1;
        p1 = i0;
      }
      i0 = l11;
      i1 = 7u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 3221u;
        i1 = p1;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 7u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto suspend_point;}
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = p1;
      } else {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        p1 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = p1;
        i1 = 1u;
        i0 = i0 == i1;
        p1 = i0;
        if (i0) {goto release_pending_record;}
        i0 = p0;
        i0 = LoadU32(ctx, i0 + 496u);
        l13 = i0;
        i0 = p0;
        i0 = LoadU32(ctx, i0 + 492u);
        l8 = i0;
        i0 = p0;
        i0 = LoadU32(ctx, i0 + 508u);
        l12 = i0;
        i1 = p0;
        i1 = LoadU32(ctx, i1 + 504u);
        i0 += i1;
      }
      p1 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l8;
      i1 = l13;
      i0 = i0 != i1;
      l12 = i0;
      if (i0) {
        i0 = p1;
        i1 = 68u;
        i0 = DIV_U(i0, i1);
        l7 = i0;
        i1 = 2u;
        i0 <<= (i1 & 31);
        l12 = i0;
        i1 = l8;
        i0 += i1;
        i0 = LoadU32(ctx, i0);
        l8 = i0;
        i0 = l8;
        i1 = p1;
        i2 = l7;
        i3 = 68u;
        i2 *= i3;
        i1 -= i2;
        i2 = 60u;
        i1 *= i2;
        i0 += i1;
        l7 = i0;
      }
      i0 = l7;
      i1 = kDataBA2E4;
      StoreU32(ctx, i0, i1);
      i0 = l7;
      i1 = l10;
      j1 = LoadU64(ctx, i1 + 12u);
      StoreU64(ctx, i0 + 4u, j1);
      i0 = l7;
      i1 = l10;
      j1 = LoadU64(ctx, i1 + 20u);
      StoreU64(ctx, i0 + 12u, j1);
      i0 = l7;
      i1 = l10;
      j1 = LoadU64(ctx, i1 + 28u);
      StoreU64(ctx, i0 + 20u, j1);
      i0 = l7;
      i1 = l10;
      j1 = LoadU64(ctx, i1 + 36u);
      StoreU64(ctx, i0 + 28u, j1);
      i0 = l7;
      i1 = l6;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 44u, i1);
      i0 = l7;
      i1 = l6;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0 + 36u, j1);
      i0 = l6;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l6;
      i1 = 0u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l10;
      i0 = LoadU32(ctx, i0 + 56u);
      p1 = i0;
      i0 = l7;
      i1 = 0u;
      StoreU32(ctx, i0 + 52u, i1);
      i0 = l7;
      i1 = p1;
      StoreU32(ctx, i0 + 48u, i1);
      i0 = l10;
      i0 = LoadU32(ctx, i0 + 60u);
      p1 = i0;
      i0 = l10;
      i1 = 0u;
      StoreU32(ctx, i0 + 60u, i1);
      i0 = l7;
      i1 = p1;
      StoreU32(ctx, i0 + 52u, i1);
      i0 = l10;
      i1 = 0u;
      StoreU32(ctx, i0 + 56u, i1);
      i0 = l7;
      i1 = l10;
      f1 = LoadF32(ctx, i1 + 64u);
      StoreF32(ctx, i0 + 56u, f1);
      i0 = p0;
      i1 = p0;
      i1 = LoadU32(ctx, i1 + 508u);
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 508u, i1);
      i0 = l10;
      i1 = kDataBA2E4;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l10;
      i0 = LoadU32(ctx, i0 + 60u);
      p1 = i0;
      i0 = !(i0);
      l8 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l8;
      if (i0) {goto drop_record_payload;}
      i0 = p1;
      i1 = p1;
      i1 = LoadU32(ctx, i1 + 4u);
      l8 = i1;
      i2 = 1u;
      i1 -= i2;
      l7 = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l8;
      i1 = 1u;
      i0 = i0 != i1;
      l8 = i0;
      if (i0) {goto drop_record_payload;}
      i0 = p1;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      l8 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l11;
    i1 = 8u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l8;
      i1 = p1;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 8u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto suspend_point;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l8 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l8;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto resume_catch_entry;}
      i0 = p1;
      i1 = p1;
      i1 = LoadU32(ctx, i1 + 8u);
      l8 = i1;
      i2 = 1u;
      i1 -= i2;
      l7 = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l8;
      i1 = 1u;
      i0 = i0 != i1;
      l8 = i0;
      if (i0) {goto drop_record_payload;}
      i0 = p1;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      l8 = i0;
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
      i0 = l8;
      i1 = p1;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 9u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto suspend_point;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      p1 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = p1;
      i1 = 1u;
      i0 = i0 == i1;
      p1 = i0;
      if (i0) {goto resume_catch_entry;}
    }
  drop_record_payload:
    i0 = l17;
    i1 = ResumePhase(ctx);
    i1 = !(i1);
    if (i1) {
      i1 = l6;
      i1 = LoadS8(ctx, i1 + 11u);
      i2 = kDataFFFFFFFF;
      i1 = (u32)((s32)i1 <= (s32)i2);
      if (i1) {
        i1 = l6;
        i1 = LoadU32(ctx, i1);
        DeallocateRawBlock(ctx, i1);
      }
      i1 = 0u;
      l7 = i1;
      i1 = p0;
      i1 = LoadU32(ctx, i1 + 532u);
      i2 = p0;
      i2 = LoadU32(ctx, i2 + 528u);
      i1 += i2;
      l8 = i1;
      i2 = p0;
      i2 = LoadU32(ctx, i2 + 520u);
      p1 = i2;
      i3 = p0;
      i3 = LoadU32(ctx, i3 + 516u);
      l6 = i3;
      i2 -= i3;
      l13 = i2;
      i3 = 2u;
      i2 = (u32)((s32)i2 >> (i3 & 31));
      i3 = 170u;
      i2 *= i3;
      i3 = 1u;
      i2 -= i3;
      i3 = 0u;
      i4 = l13;
      i2 = i4 ? i2 : i3;
      i1 = i1 == i2;
      l12 = i1;
    }
    i1 = l12;
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = p0;
        i1 = 512u;
        i0 += i1;
        p1 = i0;
      }
      i0 = l11;
      i1 = 10u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 3222u;
        i1 = p1;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 10u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto suspend_point;}
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = p1;
      } else {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        p1 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = p1;
        i1 = 1u;
        i0 = i0 == i1;
        p1 = i0;
        if (i0) {goto discard_pending_c;}
        i0 = p0;
        i0 = LoadU32(ctx, i0 + 528u);
        i1 = p0;
        i1 = LoadU32(ctx, i1 + 532u);
        i0 += i1;
        l8 = i0;
        i0 = p0;
        i0 = LoadU32(ctx, i0 + 516u);
        l6 = i0;
        i0 = p0;
        i0 = LoadU32(ctx, i0 + 520u);
      }
      p1 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = p1;
      i1 = l6;
      i0 = i0 != i1;
      if (i0) {
        i0 = l8;
        i1 = 170u;
        i0 = DIV_U(i0, i1);
        p1 = i0;
        i1 = 2u;
        i0 <<= (i1 & 31);
        i1 = l6;
        i0 += i1;
        i0 = LoadU32(ctx, i0);
        l6 = i0;
        i0 = l6;
        i1 = l8;
        i2 = p1;
        i3 = 170u;
        i2 *= i3;
        i1 -= i2;
        i2 = 24u;
        i1 *= i2;
        i0 += i1;
        l7 = i0;
      }
      i0 = l7;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l7;
      j1 = 0ull;
      StoreU64(ctx, i0 + 16u, j1);
      i0 = l7;
      j1 = 0ull;
      StoreU64(ctx, i0 + 8u, j1);
      i0 = p0;
      i1 = p0;
      i1 = LoadU32(ctx, i1 + 532u);
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 532u, i1);
      i0 = p0;
      i0 = LoadU32(ctx, i0 + 556u);
      l13 = i0;
      i1 = p0;
      i1 = LoadU32(ctx, i1 + 552u);
      i0 += i1;
      l7 = i0;
      i0 = p0;
      i0 = LoadU32(ctx, i0 + 544u);
      l8 = i0;
      i1 = p0;
      i1 = LoadU32(ctx, i1 + 540u);
      l6 = i1;
      i0 -= i1;
      p1 = i0;
      i1 = 2u;
      i0 = (u32)((s32)i0 >> (i1 & 31));
      i1 = 341u;
      i0 *= i1;
      i1 = 1u;
      i0 -= i1;
      i1 = 0u;
      i2 = p1;
      i0 = i2 ? i0 : i1;
      p1 = i0;
    }
    i0 = p4;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = p1;
      i1 = p1;
      i2 = l7;
      i1 = i1 == i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      p1 = i0;
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
          i0 = p0;
          i1 = 536u;
          i0 += i1;
          p1 = i0;
        }
        i0 = l11;
        i1 = 11u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 3223u;
          i1 = p1;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 11u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto suspend_point;}
        }
        i0 = ResumePhase(ctx);
        if (i0) {
          i0 = l6;
        } else {
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          p1 = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = p1;
          i1 = 1u;
          i0 = i0 == i1;
          if (i0) {goto discard_pending_e;}
          i0 = p0;
          i0 = LoadU32(ctx, i0 + 544u);
          l8 = i0;
          i0 = p0;
          i0 = LoadU32(ctx, i0 + 552u);
          p1 = i0;
          i1 = p0;
          i1 = LoadU32(ctx, i1 + 556u);
          p4 = i1;
          i0 += i1;
          l7 = i0;
          i0 = p0;
          i0 = LoadU32(ctx, i0 + 540u);
        }
        l6 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = 0u;
        p1 = i0;
        i0 = l6;
        i1 = l8;
        i0 = i0 != i1;
        p4 = i0;
        if (i0) {
          i0 = l6;
          i1 = l7;
          i2 = 341u;
          i1 = DIV_U(i1, i2);
          p1 = i1;
          i2 = 2u;
          i1 <<= (i2 & 31);
          i0 += i1;
          i0 = LoadU32(ctx, i0);
          p4 = i0;
          i0 = p4;
          i1 = l7;
          i2 = p1;
          i3 = 341u;
          i2 *= i3;
          i1 -= i2;
          i2 = 12u;
          i1 *= i2;
          i0 += i1;
          p1 = i0;
        }
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
        i0 = 2632u;
        i1 = p1;
        i2 = p3;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l9 = i0;
        i0 = 12u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto suspend_point;}
        i0 = l9;
        p1 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        p1 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = p1;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto discard_pending_e;}
        i0 = p0;
        i1 = p0;
        i1 = LoadU32(ctx, i1 + 556u);
        i2 = 1u;
        i1 += i2;
        StoreU32(ctx, i0 + 556u, i1);
        i0 = p2;
        i0 = LoadU32(ctx, i0 + 20u);
        l6 = i0;
        i0 = !(i0);
        p1 = i0;
        if (i0) {goto select_released_entry;}
        i0 = 0u;
        l8 = i0;
        i0 = 0u;
        l14 = i0;
        i0 = p3;
        i0 = LoadU32(ctx, i0);
        l7 = i0;
        i1 = p3;
        i1 = LoadU32(ctx, i1 + 4u);
        i0 = i0 == i1;
        p1 = i0;
        if (i0) {goto grow_bucket_table;}
        i0 = 0u;
        l13 = i0;
      }
  // ---- Second pass: records tagged for annotation. ----
  scan_annotation_records:
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l7;
          i0 = LoadU8(ctx, i0 + 15u);
          l6 = i0;
          i1 = 24u;
          i0 <<= (i1 & 31);
          i1 = 24u;
          i0 = (u32)((s32)i0 >> (i1 & 31));
          p4 = i0;
          i1 = 0u;
          i0 = (u32)((s32)i0 < (s32)i1);
          l15 = i0;
          i0 = l7;
          i0 = LoadU32(ctx, i0 + 8u);
          l12 = i0;
          i1 = l6;
          i2 = l15;
          i0 = i2 ? i0 : i1;
          i1 = 4u;
          i0 = i0 == i1;
          l15 = i0;
          i0 = l7;
          i1 = 4u;
          i0 += i1;
          p1 = i0;
        }
        i0 = l18;
        i1 = l15;
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
            i0 = 1354u;
            i1 = p1;
            i2 = 0u;
            i3 = kDataFFFFFFFF;
            i4 = kTagFont;
            i5 = 4u;
            i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
            l9 = i0;
            i0 = 13u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto suspend_point;}
            i0 = l9;
            p4 = i0;
          }
          i0 = ResumePhase(ctx);
          if (i0) {
            i0 = p4;
          } else {
            i0 = kDataD015C;
            i0 = LoadU32(ctx, i0);
            l6 = i0;
            i0 = kDataD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l6;
            i1 = 1u;
            i0 = i0 == i1;
            if (i0) {goto resume_catch_entry;}
            i0 = l13;
            i1 = p4;
            i1 = !(i1);
            i0 += i1;
            l13 = i0;
            i0 = l7;
            i0 = LoadU32(ctx, i0 + 8u);
            l12 = i0;
            i0 = l7;
            i0 = LoadU8(ctx, i0 + 15u);
            l6 = i0;
          }
          p4 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l12;
          i1 = l6;
          i2 = p4;
          i3 = 24u;
          i2 <<= (i3 & 31);
          i3 = 24u;
          i2 = (u32)((s32)i2 >> (i3 & 31));
          i3 = 0u;
          i2 = (u32)((s32)i2 < (s32)i3);
          i0 = i2 ? i0 : i1;
          i1 = 5u;
          i0 = i0 != i1;
          p4 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = p4;
          if (i0) {goto advance_annotation_cursor;}
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l11;
        i1 = 14u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1354u;
          i1 = p1;
          i2 = 0u;
          i3 = kDataFFFFFFFF;
          i4 = kTagFontEnd;
          i5 = 5u;
          i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
          l9 = i0;
          i0 = 14u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto suspend_point;}
          i0 = l9;
          p1 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          p4 = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = p4;
          i1 = 1u;
          i0 = i0 == i1;
          p4 = i0;
          if (i0) {goto resume_catch_entry;}
          i0 = p1;
          if (i0) {goto advance_annotation_cursor;}
          i0 = l13;
          if (i0) {
            i0 = l13;
            i1 = 1u;
            i0 -= i1;
            l13 = i0;
            goto advance_annotation_cursor;
          }
          i0 = l14;
          i1 = 1u;
          i0 += i1;
          l14 = i0;
          i0 = 0u;
          l13 = i0;
        }
  advance_annotation_cursor:
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l7;
          i1 = 44u;
          i0 += i1;
          l7 = i0;
          i1 = p3;
          i1 = LoadU32(ctx, i1 + 4u);
          i0 = i0 != i1;
          p1 = i0;
          if (i0) {goto scan_annotation_records;}
        }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto read_pending_word;}
    }
    i0 = p1;
    i1 = p1;
    i2 = l7;
    i1 = i1 == i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    p1 = i0;
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
        i0 = p0;
        i1 = 536u;
        i0 += i1;
        p1 = i0;
      }
      i0 = l11;
      i1 = 15u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 3223u;
        i1 = p1;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 15u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto suspend_point;}
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l6;
      } else {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        p1 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = p1;
        i1 = 1u;
        i0 = i0 == i1;
        p1 = i0;
        if (i0) {goto discard_pending_a;}
        i0 = p0;
        i0 = LoadU32(ctx, i0 + 544u);
        l8 = i0;
        i0 = p0;
        i0 = LoadU32(ctx, i0 + 552u);
        p1 = i0;
        i1 = p0;
        i1 = LoadU32(ctx, i1 + 556u);
        l13 = i1;
        i0 += i1;
        l7 = i0;
        i0 = p0;
        i0 = LoadU32(ctx, i0 + 540u);
      }
      l6 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = 0u;
      p1 = i0;
      i0 = l6;
      i1 = l8;
      i0 = i0 != i1;
      p2 = i0;
      if (i0) {
        i0 = l6;
        i1 = l7;
        i2 = 341u;
        i1 = DIV_U(i1, i2);
        p1 = i1;
        i2 = 2u;
        i1 <<= (i2 & 31);
        i0 += i1;
        i0 = LoadU32(ctx, i0);
        p2 = i0;
        i0 = p2;
        i1 = l7;
        i2 = p1;
        i3 = 341u;
        i2 *= i3;
        i1 -= i2;
        i2 = 12u;
        i1 *= i2;
        i0 += i1;
        p1 = i0;
      }
      i0 = p1;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = p1;
      i1 = 0u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = p0;
      i1 = l13;
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 556u, i1);
      i0 = p3;
      i0 = LoadU32(ctx, i0);
      l7 = i0;
      i1 = p3;
      i1 = LoadU32(ctx, i1 + 4u);
      i0 = i0 == i1;
      p1 = i0;
      if (i0) {goto probe_entry_tag;}
    }
  // ---- Third pass: records tagged for font init. ----
  scan_fontinit_records:
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l7;
        i0 = LoadU8(ctx, i0 + 15u);
        l6 = i0;
        i1 = 24u;
        i0 <<= (i1 & 31);
        i1 = 24u;
        i0 = (u32)((s32)i0 >> (i1 & 31));
        p1 = i0;
        i1 = 0u;
        i0 = (u32)((s32)i0 < (s32)i1);
        p4 = i0;
        i0 = l7;
        i0 = LoadU32(ctx, i0 + 8u);
        l8 = i0;
        i1 = l6;
        i2 = p4;
        i0 = i2 ? i0 : i1;
        i1 = 8u;
        i0 = i0 == i1;
        p4 = i0;
        i0 = l7;
        i1 = 4u;
        i0 += i1;
        p2 = i0;
      }
      i0 = p4;
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
        i1 = 16u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1354u;
          i1 = p2;
          i2 = 0u;
          i3 = kDataFFFFFFFF;
          i4 = kTagFontInit;
          i5 = 8u;
          i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
          l9 = i0;
          i0 = 16u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto suspend_point;}
          i0 = l9;
          p1 = i0;
        }
        i0 = ResumePhase(ctx);
        if (i0) {
          i0 = p1;
        } else {
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          p4 = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = p4;
          i1 = 1u;
          i0 = i0 == i1;
          p4 = i0;
          if (i0) {goto resume_catch_entry;}
          i0 = p1;
          i0 = !(i0);
          p1 = i0;
          if (i0) {goto advance_fontinit_cursor;}
          i0 = l7;
          i0 = LoadU32(ctx, i0 + 8u);
          l8 = i0;
          i0 = l7;
          i0 = LoadU8(ctx, i0 + 15u);
          l6 = i0;
        }
        p1 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i1 = l6;
        i2 = p1;
        i3 = 24u;
        i2 <<= (i3 & 31);
        i3 = 24u;
        i2 = (u32)((s32)i2 >> (i3 & 31));
        i3 = 0u;
        i2 = (u32)((s32)i2 < (s32)i3);
        i0 = i2 ? i0 : i1;
        i1 = 4u;
        i0 = i0 == i1;
        p4 = i0;
      }
      i0 = p4;
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
        i1 = 17u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1354u;
          i1 = p2;
          i2 = 0u;
          i3 = kDataFFFFFFFF;
          i4 = kTagFont;
          i5 = 4u;
          i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
          l9 = i0;
          i0 = 17u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto suspend_point;}
          i0 = l9;
          p1 = i0;
        }
        i0 = ResumePhase(ctx);
        if (i0) {
          i0 = p1;
        } else {
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          p4 = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = p4;
          i1 = 1u;
          i0 = i0 == i1;
          p4 = i0;
          if (i0) {goto resume_catch_entry;}
          i0 = p1;
          i0 = !(i0);
          p1 = i0;
          if (i0) {goto advance_fontinit_cursor;}
          i0 = l7;
          i0 = LoadU32(ctx, i0 + 8u);
          l8 = i0;
          i0 = l7;
          i0 = LoadU8(ctx, i0 + 15u);
          l6 = i0;
        }
        p1 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i1 = l6;
        i2 = p1;
        i3 = 24u;
        i2 <<= (i3 & 31);
        i3 = 24u;
        i2 = (u32)((s32)i2 >> (i3 & 31));
        i3 = 0u;
        i2 = (u32)((s32)i2 < (s32)i3);
        i0 = i2 ? i0 : i1;
        i1 = 5u;
        i0 = i0 == i1;
        p1 = i0;
      }
      i0 = p1;
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
          i0 = 1354u;
          i1 = p2;
          i2 = 0u;
          i3 = kDataFFFFFFFF;
          i4 = kTagFontEnd;
          i5 = 5u;
          i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
          l9 = i0;
          i0 = 18u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto suspend_point;}
          i0 = l9;
          p1 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          p2 = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = p2;
          i1 = 1u;
          i0 = i0 == i1;
          p2 = i0;
          if (i0) {goto resume_catch_entry;}
          i0 = p1;
          i0 = !(i0);
          p1 = i0;
          if (i0) {goto advance_fontinit_cursor;}
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = p0;
        i0 = LoadU32(ctx, i0 + 556u);
        i1 = p0;
        i1 = LoadU32(ctx, i1 + 552u);
        i0 += i1;
        i1 = 1u;
        i0 -= i1;
        p1 = i0;
        i1 = 341u;
        i0 = DIV_U(i0, i1);
        p2 = i0;
        i1 = 2u;
        i0 <<= (i1 & 31);
        l6 = i0;
        i1 = p0;
        i1 = LoadU32(ctx, i1 + 540u);
        i0 += i1;
        i0 = LoadU32(ctx, i0);
        p4 = i0;
        i0 = p4;
        i1 = p1;
        i2 = p2;
        i3 = 341u;
        i2 *= i3;
        i1 -= i2;
        i2 = 12u;
        i1 *= i2;
        i0 += i1;
        p1 = i0;
        i0 = LoadU32(ctx, i0 + 4u);
        p2 = i0;
        i1 = p1;
        i1 = LoadU32(ctx, i1 + 8u);
        i0 = i0 != i1;
        p4 = i0;
      }
      i0 = p4;
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
        i1 = 19u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2591u;
          i1 = p2;
          i2 = l7;
          i0 = HostOsQuery(ctx, i0, i1, i2);
          l9 = i0;
          i0 = 19u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto suspend_point;}
          i0 = l9;
          p4 = i0;
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
          if (i0) {
            i0 = HostRet0(ctx);
            p0 = i0;
            i0 = HostTempRet0(ctx);
            i0 = p1;
            i1 = p2;
            StoreU32(ctx, i0 + 4u, i1);
            goto release_held_slots;
          }
          i0 = p1;
          i1 = p4;
          i2 = 44u;
          i1 += i2;
          p2 = i1;
          StoreU32(ctx, i0 + 4u, i1);
          goto advance_fontinit_cursor;
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
      i1 = 20u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 2614u;
        i1 = p1;
        i2 = l7;
        HostOsSchedule(ctx, i0, i1, i2);
        i0 = 20u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto suspend_point;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        p1 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = p1;
        i1 = 1u;
        i0 = i0 == i1;
        p1 = i0;
        if (i0) {goto discard_pending_b;}
      }
  advance_fontinit_cursor:
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l7;
        i1 = 44u;
        i0 += i1;
        l7 = i0;
        i1 = p3;
        i1 = LoadU32(ctx, i1 + 4u);
        i0 = i0 != i1;
        p1 = i0;
        if (i0) {goto scan_fontinit_records;}
      }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto probe_entry_tag;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    p0 = i0;
    i0 = HostTempRet0(ctx);
    goto release_held_slots;
  }
  // ---- Resume dispatch: one trampoline per suspendable call site. ----
  resume_throw_probe:
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    p0 = i0;
    i0 = HostTempRet0(ctx);
    p1 = i0;
  }
  i0 = l11;
  i1 = 21u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = p0;
    Panic(ctx, i0);
    i0 = 21u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto suspend_point;}
  }
  unexpected_resume:
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    TrapUnreachable();
  }
  discard_pending_a:
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    p0 = i0;
    i0 = HostTempRet0(ctx);
    goto release_held_slots;
  }
  discard_pending_b:
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    p0 = i0;
    i0 = HostTempRet0(ctx);
    goto release_held_slots;
  }
  read_pending_word:
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = p2;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
  }
  grow_bucket_table:
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = p2;
    i0 = LoadU32(ctx, i0 + 4u);
    p3 = i0;
    i1 = l6;
    i2 = p2;
    i2 = LoadU32(ctx, i2 + 16u);
    i1 += i2;
    p4 = i1;
    i2 = 24u;
    i1 = DIV_U(i1, i2);
    l6 = i1;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    p1 = i0;
    i0 = p2;
    i0 = LoadU32(ctx, i0 + 8u);
    l7 = i0;
    i1 = p3;
    i0 = i0 != i1;
    if (i0) {
      i0 = p4;
      i1 = l6;
      i2 = 24u;
      i1 *= i2;
      l6 = i1;
      i0 -= i1;
      i1 = 168u;
      i0 *= i1;
      p4 = i0;
      i1 = p1;
      i1 = LoadU32(ctx, i1);
      i0 += i1;
      l8 = i0;
    }
    i0 = l14;
    i1 = 0u;
    i0 = (u32)((s32)i0 < (s32)i1);
    p3 = i0;
    if (i0) {goto probe_entry_tag;}
    i0 = 0u;
    l7 = i0;
    i0 = p1;
    i0 = LoadU32(ctx, i0);
    l6 = i0;
  }
  // ---- Probe and grow the primary bucket table. ----
  scan_bucket_slots:
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = p0;
      i0 = LoadU32(ctx, i0 + 516u);
      i1 = p0;
      i1 = LoadU32(ctx, i1 + 532u);
      i2 = p0;
      i2 = LoadU32(ctx, i2 + 528u);
      i1 += i2;
      i2 = 1u;
      i1 -= i2;
      p3 = i1;
      i2 = 170u;
      i1 = DIV_U(i1, i2);
      p4 = i1;
      i2 = 2u;
      i1 <<= (i2 & 31);
      i0 += i1;
      i0 = LoadU32(ctx, i0);
      l12 = i0;
      i0 = l12;
      i1 = p3;
      i2 = p4;
      i3 = 170u;
      i2 *= i3;
      i1 -= i2;
      i2 = 24u;
      i1 *= i2;
      i0 += i1;
      p4 = i0;
      i0 = l6;
      i1 = l8;
      p3 = i1;
      i0 = i0 == i1;
      l6 = i0;
      if (i0) {
        i0 = p1;
        i1 = 4u;
        i0 -= i1;
        i0 = LoadU32(ctx, i0);
        i1 = 4032u;
        i0 += i1;
        p3 = i0;
      }
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = p3;
      i1 = 168u;
      i0 -= i1;
      p3 = i0;
    }
    i0 = l11;
    i1 = 22u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3224u;
      i1 = p4;
      i2 = p3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 22u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto suspend_point;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      p3 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = p3;
      i1 = 1u;
      i0 = i0 != i1;
      p3 = i0;
      if (i0) {
        i0 = l8;
        i1 = p1;
        i1 = LoadU32(ctx, i1);
        l6 = i1;
        i0 = i0 == i1;
        if (i0) {
          i0 = p1;
          i1 = 4u;
          i0 -= i1;
          p1 = i0;
          i0 = LoadU32(ctx, i0);
          l6 = i0;
          i1 = 4032u;
          i0 += i1;
        } else {
          i0 = l8;
        }
        i1 = 168u;
        i0 -= i1;
        l8 = i0;
        i0 = 0u;
        p4 = i0;
        i0 = p2;
        i0 = LoadU32(ctx, i0 + 4u);
        p3 = i0;
        i1 = p2;
        i1 = LoadU32(ctx, i1 + 8u);
        i0 = i0 != i1;
        if (i0) {
          i0 = p2;
          i0 = LoadU32(ctx, i0 + 16u);
          p4 = i0;
          i1 = 24u;
          i0 = DIV_U(i0, i1);
          l13 = i0;
          i1 = 2u;
          i0 <<= (i1 & 31);
          i1 = p3;
          i0 += i1;
          i0 = LoadU32(ctx, i0);
          i1 = p4;
          i2 = l13;
          i3 = 24u;
          i2 *= i3;
          i1 -= i2;
          i2 = 168u;
          i1 *= i2;
          i0 += i1;
          p4 = i0;
        }
        i0 = p4;
        i1 = l8;
        i0 = i0 == i1;
        p3 = i0;
        if (i0) {goto probe_entry_tag;}
        i0 = l7;
        i1 = l14;
        i0 = i0 == i1;
        p3 = i0;
        i0 = l7;
        i1 = 1u;
        i0 += i1;
        l7 = i0;
        i0 = p3;
        if (i0) {goto probe_entry_tag;}
        goto scan_bucket_slots;
      }
    }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    p0 = i0;
    i0 = HostTempRet0(ctx);
    goto release_held_slots;
  }
  probe_entry_tag:
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = p5;
    i0 = LoadU8(ctx, i0 + 11u);
    p1 = i0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (u32)((s32)i0 >> (i1 & 31));
    i1 = 0u;
    i0 = (u32)((s32)i0 < (s32)i1);
    p3 = i0;
    i0 = p5;
    i0 = LoadU32(ctx, i0 + 4u);
    p2 = i0;
    i1 = p1;
    i2 = p3;
    i0 = i2 ? i0 : i1;
    i0 = !(i0);
    p1 = i0;
    if (i0) {goto select_released_entry;}
    i0 = p0;
    i0 = LoadU32(ctx, i0 + 556u);
    i1 = p0;
    i1 = LoadU32(ctx, i1 + 552u);
    i0 += i1;
    i1 = 1u;
    i0 -= i1;
    p1 = i0;
    i1 = 341u;
    i0 = DIV_U(i0, i1);
    p2 = i0;
    i1 = 2u;
    i0 <<= (i1 & 31);
    p4 = i0;
    i1 = p0;
    i1 = LoadU32(ctx, i1 + 540u);
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    p3 = i0;
    i0 = p3;
    i1 = p1;
    i2 = p2;
    i3 = 341u;
    i2 *= i3;
    p2 = i2;
    i1 -= i2;
    i2 = 12u;
    i1 *= i2;
    i0 += i1;
    p1 = i0;
    i0 = LoadU32(ctx, i0);
    l7 = i0;
    i0 = l7;
    i1 = p1;
    i1 = LoadU32(ctx, i1 + 4u);
    i0 = i0 == i1;
    p1 = i0;
    if (i0) {goto select_released_entry;}
  }
  // ---- Probe and grow the secondary bucket table. ----
  scan_pending_slots:
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l7;
      i0 = LoadU8(ctx, i0 + 15u);
      p1 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (u32)((s32)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (u32)((s32)i0 < (s32)i1);
      p3 = i0;
      i0 = l7;
      i0 = LoadU32(ctx, i0 + 8u);
      p2 = i0;
      i1 = p1;
      i2 = p3;
      i0 = i2 ? i0 : i1;
      i1 = 5u;
      i0 = i0 != i1;
      p1 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = p1;
      if (i0) {goto grow_secondary_table;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l7;
      i1 = 4u;
      i0 += i1;
      p1 = i0;
    }
    i0 = l11;
    i1 = 23u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = p1;
      i2 = 0u;
      i3 = kDataFFFFFFFF;
      i4 = kTagPrint;
      i5 = 5u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l9 = i0;
      i0 = 23u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto suspend_point;}
      i0 = l9;
      p1 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      p2 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = p2;
      i1 = 1u;
      i0 = i0 == i1;
      p2 = i0;
      if (i0) {goto resume_catch_entry;}
      i0 = p1;
      if (i0) {goto grow_secondary_table;}
      i0 = l10;
      i1 = kTagAnnotationTail;
      i1 = LoadU16(ctx, i1);
      StoreU16(ctx, i0 + 16u, i1);
      i0 = l10;
      i1 = 2560u;
      StoreU16(ctx, i0 + 18u, i1);
      i0 = l10;
      i1 = kTagAnnotationText;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0 + 8u, j1);
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l10;
      i1 = l10;
      i2 = 8u;
      i1 += i2;
      StoreU32(ctx, i0 + 144u, i1);
      i0 = l7;
      i1 = 16u;
      i0 += i1;
      p2 = i0;
      i0 = l10;
      i1 = 8u;
      i0 += i1;
      p3 = i0;
      i0 = l10;
      i1 = 144u;
      i0 += i1;
      p4 = i0;
      i0 = l10;
      i1 = 136u;
      i0 += i1;
      l6 = i0;
      i0 = l10;
      i1 = 152u;
      i0 += i1;
      p1 = i0;
    }
    i0 = l11;
    i1 = 24u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1959u;
      i1 = p1;
      i2 = p2;
      i3 = p3;
      i4 = kEmptyName;
      i5 = p4;
      i6 = l6;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 24u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto suspend_point;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      p1 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = p1;
      i1 = 1u;
      i0 = i0 == i1;
      p1 = i0;
      if (i0) {goto release_small_payload;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l10;
      i0 = LoadU32(ctx, i0 + 152u);
      i1 = 28u;
      i0 += i1;
      p1 = i0;
    }
    i0 = l11;
    i1 = 25u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 870u;
      i1 = p1;
      i2 = p5;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l9 = i0;
      i0 = 25u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto suspend_point;}
      i0 = l9;
      p1 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      p1 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = p1;
      i1 = 1u;
      i0 = i0 == i1;
      p1 = i0;
      if (i0) {goto release_small_payload;}
      i0 = l10;
      i0 = LoadS8(ctx, i0 + 19u);
      i1 = kDataFFFFFFFF;
      i0 = (u32)((s32)i0 > (s32)i1);
      p1 = i0;
      if (i0) {goto grow_secondary_table;}
      i0 = l10;
      i0 = LoadU32(ctx, i0 + 8u);
      p1 = i0;
      DeallocateRawBlock(ctx, i0);
    }
  grow_secondary_table:
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = p0;
      i0 = LoadU32(ctx, i0 + 556u);
      i1 = p0;
      i1 = LoadU32(ctx, i1 + 552u);
      i0 += i1;
      i1 = 1u;
      i0 -= i1;
      p1 = i0;
      i1 = 341u;
      i0 = DIV_U(i0, i1);
      p2 = i0;
      i1 = 2u;
      i0 <<= (i1 & 31);
      p4 = i0;
      i1 = p0;
      i1 = LoadU32(ctx, i1 + 540u);
      i0 += i1;
      i0 = LoadU32(ctx, i0);
      p3 = i0;
      i0 = l7;
      i1 = 44u;
      i0 += i1;
      l7 = i0;
      i1 = p3;
      i2 = p1;
      i3 = p2;
      i4 = 341u;
      i3 *= i4;
      p2 = i3;
      i2 -= i3;
      i3 = 12u;
      i2 *= i3;
      i1 += i2;
      i1 = LoadU32(ctx, i1 + 4u);
      i0 = i0 != i1;
      p1 = i0;
      if (i0) {goto scan_pending_slots;}
    }
  select_released_entry:
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l10;
    i0 = LoadU32(ctx, i0 + 88u);
    p1 = i0;
    i0 = !(i0);
    p0 = i0;
    if (i0) {goto restore_stack_pointer;}
    i0 = l10;
    i0 = LoadU32(ctx, i0 + 92u);
    p3 = i0;
    i1 = p1;
    i0 = i0 != i1;
    p0 = i0;
    if (i0) {goto keep_active_entry;}
    i0 = p1;
    goto store_bucket_end;
  }
  release_small_payload:
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    p0 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l10;
    i0 = LoadS8(ctx, i0 + 19u);
    i1 = kDataFFFFFFFF;
    i0 = (u32)((s32)i0 > (s32)i1);
    if (i0) {goto release_held_slots;}
    i0 = l10;
    i0 = LoadU32(ctx, i0 + 8u);
    DeallocateRawBlock(ctx, i0);
    goto release_held_slots;
  }
  keep_active_entry:
  i0 = ResumePhase(ctx);
  if (i0) {
    i0 = p0;
  } else {
  // ---- Release the bucket chain hanging off the frame. ----
  release_bucket_range:
      i0 = p3;
      i1 = 12u;
      i0 -= i1;
      p5 = i0;
      i0 = LoadU32(ctx, i0);
      p2 = i0;
      if (i0) {
        i0 = p2;
        p0 = i0;
        i1 = p3;
        i2 = 8u;
        i1 -= i2;
        l8 = i1;
        i1 = LoadU32(ctx, i1);
        l6 = i1;
        i0 = i0 != i1;
        if (i0) {
  release_bucket_nodes:
            i0 = l6;
            i1 = 12u;
            i0 -= i1;
            p0 = i0;
            i0 = LoadU32(ctx, i0);
            p4 = i0;
            if (i0) {
              i0 = l6;
              i1 = 8u;
              i0 -= i1;
              i1 = p4;
              StoreU32(ctx, i0, i1);
              i0 = p4;
              DeallocateRawBlock(ctx, i0);
            }
            i0 = p0;
            l6 = i0;
            i1 = p2;
            i0 = i0 != i1;
            if (i0) {goto release_bucket_nodes;}
          i0 = p5;
          i0 = LoadU32(ctx, i0);
          p0 = i0;
        }
        i0 = l8;
        i1 = p2;
        StoreU32(ctx, i0, i1);
        i0 = p0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = p3;
      i1 = 24u;
      i0 -= i1;
      p0 = i0;
      i0 = LoadU32(ctx, i0);
      p2 = i0;
      if (i0) {
        i0 = p3;
        i1 = 20u;
        i0 -= i1;
        i1 = p2;
        StoreU32(ctx, i0, i1);
        i0 = p2;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = p0;
      p3 = i0;
      i0 = p0;
      i1 = p1;
      i0 = i0 != i1;
      if (i0) {goto release_bucket_range;}
    i0 = l10;
    i0 = LoadU32(ctx, i0 + 88u);
  }
  store_bucket_end:
  p0 = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l10;
    i1 = p1;
    StoreU32(ctx, i0 + 92u, i1);
    i0 = p0;
    DeallocateRawBlock(ctx, i0);
  }
  restore_stack_pointer:
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l10;
    i1 = 160u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto dispatch_exit;
  }
  discard_pending_c:
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    p0 = i0;
    i0 = HostTempRet0(ctx);
    goto release_held_slots;
  }
  release_pending_record:
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    p0 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l10;
    i1 = kDataBA2E4;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l10;
    i0 = LoadU32(ctx, i0 + 60u);
    p1 = i0;
    i0 = !(i0);
    p2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = p2;
    if (i0) {goto release_record_payload;}
    i0 = p1;
    i1 = p1;
    i1 = LoadU32(ctx, i1 + 4u);
    p2 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = p2;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto release_record_payload;}
    i0 = p1;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    p2 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l11;
  i1 = 26u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = p2;
    i1 = p1;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 26u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto suspend_point;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    p2 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = p2;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto resume_pending_throw;}
    i0 = p1;
    i1 = p1;
    i1 = LoadU32(ctx, i1 + 8u);
    p2 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = p2;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto release_record_payload;}
    i0 = p1;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    p2 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l11;
  i1 = 27u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = p2;
    i1 = p1;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 27u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto suspend_point;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    p1 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = p1;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto resume_pending_throw;}
  }
  release_record_payload:
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kDataFFFFFFFF;
    i0 = (u32)((s32)i0 > (s32)i1);
    if (i0) {goto release_held_slots;}
    i0 = l6;
    i0 = LoadU32(ctx, i0);
    DeallocateRawBlock(ctx, i0);
    goto release_held_slots;
  }
  resume_pending_throw:
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto resume_catch_entry;}
  discard_pending_d:
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    p0 = i0;
    i0 = HostTempRet0(ctx);
    goto release_held_slots;
  }
  discard_pending_e:
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    p0 = i0;
    i0 = HostTempRet0(ctx);
    goto release_held_slots;
  }
  discard_pending_f:
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    p0 = i0;
    i0 = HostTempRet0(ctx);
  }
  // ---- Release the host slots this invocation still holds. ----
  release_held_slots:
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l10;
    i0 = LoadU32(ctx, i0 + 88u);
    p3 = i0;
    if (i0) {
      i0 = p3;
      l8 = i0;
      i1 = l10;
      i1 = LoadU32(ctx, i1 + 92u);
      l6 = i1;
      i0 = i0 != i1;
      if (i0) {
  release_slot_range:
          i0 = l6;
          i1 = 12u;
          i0 -= i1;
          l8 = i0;
          i0 = LoadU32(ctx, i0);
          p2 = i0;
          if (i0) {
            i0 = p2;
            p4 = i0;
            i0 = p2;
            i1 = l6;
            i2 = 8u;
            i1 -= i2;
            l7 = i1;
            i1 = LoadU32(ctx, i1);
            p1 = i1;
            i0 = i0 != i1;
            if (i0) {
  release_slot_nodes:
                i0 = p1;
                i1 = 12u;
                i0 -= i1;
                p4 = i0;
                i0 = LoadU32(ctx, i0);
                p5 = i0;
                if (i0) {
                  i0 = p1;
                  i1 = 8u;
                  i0 -= i1;
                  i1 = p5;
                  StoreU32(ctx, i0, i1);
                  i0 = p5;
                  DeallocateRawBlock(ctx, i0);
                }
                i0 = p2;
                i1 = p4;
                p1 = i1;
                i0 = i0 != i1;
                if (i0) {goto release_slot_nodes;}
              i0 = l8;
              i0 = LoadU32(ctx, i0);
              p4 = i0;
            }
            i0 = l7;
            i1 = p2;
            StoreU32(ctx, i0, i1);
            i0 = p4;
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l6;
          i1 = 24u;
          i0 -= i1;
          p1 = i0;
          i0 = LoadU32(ctx, i0);
          p2 = i0;
          if (i0) {
            i0 = l6;
            i1 = 20u;
            i0 -= i1;
            i1 = p2;
            StoreU32(ctx, i0, i1);
            i0 = p2;
            DeallocateRawBlock(ctx, i0);
          }
          i0 = p1;
          l6 = i0;
          i0 = p1;
          i1 = p3;
          i0 = i0 != i1;
          if (i0) {goto release_slot_range;}
        i0 = l10;
        i0 = LoadU32(ctx, i0 + 88u);
        l8 = i0;
      }
      i0 = l10;
      i1 = p3;
      StoreU32(ctx, i0 + 92u, i1);
      i0 = l8;
      DeallocateRawBlock(ctx, i0);
    }
    i0 = p0;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  resume_catch_entry:
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    p0 = i0;
    i0 = HostTempRet0(ctx);
  }
  i0 = l11;
  i1 = 28u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = p0;
    Panic(ctx, i0);
    i0 = 28u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto suspend_point;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    TrapUnreachable();
  }
  goto dispatch_exit;
  // ---- Suspension exit: spill the resume code and rewind the frame. ----
  suspend_point:
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
  i1 = p0;
  StoreU32(ctx, i0, i1);
  i0 = l9;
  i1 = p1;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l9;
  i1 = p2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l9;
  i1 = p3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l9;
  i1 = p4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l9;
  i1 = p5;
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
  i1 = l12;
  StoreU32(ctx, i0 + 40u, i1);
  i0 = l9;
  i1 = l13;
  StoreU32(ctx, i0 + 44u, i1);
  i0 = l9;
  i1 = l14;
  StoreU32(ctx, i0 + 48u, i1);
  i0 = l9;
  i1 = l15;
  StoreU32(ctx, i0 + 52u, i1);
  i0 = l9;
  i1 = l16;
  StoreU32(ctx, i0 + 56u, i1);
  i0 = l9;
  i1 = l17;
  StoreU32(ctx, i0 + 60u, i1);
  i0 = l9;
  i1 = l18;
  StoreU32(ctx, i0 + 64u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kSaveFrameBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  dispatch_exit:
  LeaveFrame();
}
}  // namespace GlyphAtlas

}  // namespace artc
