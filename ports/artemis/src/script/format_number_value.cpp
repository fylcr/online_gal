// format_number_value.cpp -- formats a number for output, honouring the 14 significant digit default and the string length limit..
//
// FormatNumberValue -- formats a number for output, honouring the 14 significant digit default and the string length limit.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 478991-478991 (f1110); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t CopyBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t StringLength(EngineContext* ctx, uint32_t text);
uint32_t InternString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void GrowValueStack(EngineContext* ctx, uint32_t a0, uint32_t a1);
void RaiseScriptError(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t FetchObjectMetamethod(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void CallValue(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void FormatConcatenationError(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t MeasureFormattedNumber(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t ReallocateFormatBuffer(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

constexpr uint32_t kF1110MsgG = 0x2BED9u;  // %.14g
constexpr uint32_t kF1110MsgStringLengthOverflow = 0x2BF07u;  // string length overflow
constexpr uint32_t kF1110ConstFFFFFFFD = 0xFFFFFFFDu;  // 
constexpr uint32_t kF1110SaveFrameBytes = 44u;
constexpr uint32_t kF1110ResumeWordBytes = 4u;

}  // namespace

void FormatNumberValue(EngineContext* ctx, uint32_t state, uint32_t value, uint32_t options) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  uint64_t j1 = 0;
  double d1 = 0;
      l11 = 0, l12 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1110SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    options = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 24u);
    l7 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 28u);
    l8 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 32u);
    l9 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 36u);
    l11 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 40u);
    l12 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 4u);
    value = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1110ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l10 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    l3 = i0;
    i1 = 80u;
    i0 -= i1;
    l9 = i0;
    GuestStackSp(ctx) = i0;
  }
  loop_4: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 12u);
      i1 = options;
      i2 = 4u;
      i1 <<= (i2 & 31);
      i0 += i1;
      l3 = i0;
      i1 = 16u;
      i0 -= i1;
      l6 = i0;
      i0 = l3;
      i1 = 16u;
      i0 += i1;
      l11 = i0;
      i0 = l3;
      i1 = 8u;
      i0 -= i1;
      i0 = LoadU32(ctx, i0);
      i1 = 3u;
      i0 -= i1;
      i1 = 1u;
      i0 = i0 > i1;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      if (i0) {goto block_10;}
      i0 = l11;
      i1 = 16u;
      i0 -= i1;
      l7 = i0;
      i0 = l11;
      i1 = 8u;
      i0 -= i1;
      i0 = LoadU32(ctx, i0);
      i1 = 3u;
      i0 -= i1;
      l3 = i0;
      switch (i0) {
        case 0: goto block_8;
        case 1: goto block_7;
        default: goto block_10;
      }
      block_10:;
      i0 = state;
      i1 = l6;
      i2 = 15u;
      i0 = FetchObjectMetamethod(ctx, i0, i1, i2);
      l4 = i0;
      i0 = LoadU32(ctx, i0 + 8u);
      i0 = !(i0);
      l7 = i0;
      i0 = l11;
      i1 = 16u;
      i0 -= i1;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l7;
      if (i0) {
        i0 = state;
        i1 = l3;
        i2 = 15u;
        i0 = FetchObjectMetamethod(ctx, i0, i1, i2);
        l4 = i0;
        i0 = LoadU32(ctx, i0 + 8u);
        i0 = !(i0);
        l7 = i0;
        if (i0) {goto block_11;}
      }
      i0 = state;
      i0 = LoadU32(ctx, i0 + 32u);
      l11 = i0;
      i0 = state;
      i0 = LoadU32(ctx, i0 + 8u);
      l7 = i0;
      i1 = l4;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = l7;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = state;
      i0 = LoadU32(ctx, i0 + 8u);
      l4 = i0;
      i1 = l6;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0 + 16u, j1);
      i0 = l4;
      i1 = l6;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 24u, i1);
      i0 = state;
      i0 = LoadU32(ctx, i0 + 8u);
      l4 = i0;
      i1 = l3;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0 + 32u, j1);
      i0 = l4;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 40u, i1);
      i0 = state;
      i0 = LoadU32(ctx, i0 + 28u);
      i1 = state;
      i1 = LoadU32(ctx, i1 + 8u);
      l7 = i1;
      i0 -= i1;
      i1 = 48u;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l3 = i0;
    }
    i0 = l3;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l10;
      i0 = !(i0);
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = 3u;
        GrowValueStack(ctx, i0, i1);
        i0 = 0u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l7;
      } else {
        i0 = state;
        i0 = LoadU32(ctx, i0 + 8u);
      }
      l7 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i1 = l7;
      i2 = 48u;
      i1 += i2;
      l3 = i1;
      StoreU32(ctx, i0 + 8u, i1);
    }
    i0 = l10;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l7;
      i2 = 1u;
      CallValue(ctx, i0, i1, i2);
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i1 = state;
      i1 = LoadU32(ctx, i1 + 8u);
      i2 = 16u;
      i1 -= i2;
      l3 = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = state;
      i0 = LoadU32(ctx, i0 + 32u);
      l4 = i0;
      i1 = l6;
      i2 = l11;
      i1 -= i2;
      i0 += i1;
      l6 = i0;
      i1 = l3;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = l6;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = 2u;
      l3 = i0;
      goto block_6;
    }
    block_11:;
    i0 = l10;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l6;
      i2 = l3;
      FormatConcatenationError(ctx, i0, i1, i2);
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = 2u;
      l3 = i0;
      goto block_6;
    }
    block_8:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l9;
      i1 = l7;
      d1 = LoadF64(ctx, i1);
      StoreF64(ctx, i0 + 32u, d1);
      i0 = l9;
      i1 = 48u;
      i0 += i1;
      l3 = i0;
      i0 = l9;
      i1 = 32u;
      i0 += i1;
      l4 = i0;
    }
    i0 = l10;
    i1 = 3u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l3;
      i1 = kF1110MsgG;
      i2 = l4;
      i0 = MeasureFormattedNumber(ctx, i0, i1, i2);
      l5 = i0;
      i0 = 3u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l9;
      i1 = 48u;
      i0 += i1;
      l3 = i0;
      i0 = l9;
      i1 = 48u;
      i0 += i1;
      i0 = StringLength(ctx, i0);
      l4 = i0;
    }
    i0 = l10;
    i1 = 4u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l3;
      i2 = l4;
      i0 = InternString(ctx, i0, i1, i2);
      l5 = i0;
      i0 = 4u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l7;
      i1 = 4u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l7;
      i1 = l3;
      StoreU32(ctx, i0, i1);
    }
    block_7:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l7;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = LoadU32(ctx, i0 + 12u);
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
        i0 = value;
        i1 = 1u;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        l6 = i0;
        i0 = 1u;
        l3 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      i1 = 0u;
      i2 = l6;
      i0 = i2 ? i0 : i1;
      if (i0) {goto block_31;}
      loop_32: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l7;
          i1 = l3;
          i2 = 4u;
          i1 <<= (i2 & 31);
          i0 -= i1;
          l6 = i0;
          i0 = LoadU32(ctx, i0 + 8u);
          i1 = 3u;
          i0 -= i1;
          l8 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l8;
          switch (i0) {
            case 0: goto block_36;
            case 1: goto block_34;
            default: goto block_31;
          }
          block_36:;
          i0 = l9;
          i1 = l6;
          d1 = LoadF64(ctx, i1);
          StoreF64(ctx, i0 + 16u, d1);
          i0 = l9;
          i1 = 16u;
          i0 += i1;
          l12 = i0;
          i0 = l9;
          i1 = 48u;
          i0 += i1;
          l8 = i0;
        }
        i0 = l10;
        i1 = 5u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l8;
          i1 = kF1110MsgG;
          i2 = l12;
          i0 = MeasureFormattedNumber(ctx, i0, i1, i2);
          l5 = i0;
          i0 = 5u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l5;
          l8 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l9;
          i1 = 48u;
          i0 += i1;
          i0 = StringLength(ctx, i0);
          l12 = i0;
          i0 = l9;
          i1 = 48u;
          i0 += i1;
          l8 = i0;
        }
        i0 = l10;
        i1 = 6u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = state;
          i1 = l8;
          i2 = l12;
          i0 = InternString(ctx, i0, i1, i2);
          l5 = i0;
          i0 = 6u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l5;
          l8 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l6;
          i1 = 4u;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l6;
          i1 = l8;
          StoreU32(ctx, i0, i1);
        }
        block_34:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l6;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 12u);
          l6 = i0;
          i1 = kF1110ConstFFFFFFFD;
          i2 = l4;
          i1 -= i2;
          i0 = i0 >= i1;
          l8 = i0;
        }
        i0 = l8;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = l10;
          i1 = 7u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = kF1110MsgStringLengthOverflow;
            i2 = 0u;
            RaiseScriptError(ctx, i0, i1, i2);
            i0 = 7u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i1 = l6;
          i0 += i1;
          l4 = i0;
          i0 = l3;
          i1 = 1u;
          i0 += i1;
          l3 = i0;
          i1 = value;
          i0 = i0 != i1;
          l6 = i0;
          if (i0) {goto loop_32;}
        }
      i0 = l3;
      i1 = value;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      l3 = i0;
      block_31:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = 0u;
        l7 = i0;
        i0 = state;
        i0 = LoadU32(ctx, i0 + 16u);
        i1 = 52u;
        i0 += i1;
        l6 = i0;
      }
      i0 = l10;
      i1 = 8u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = l6;
        i2 = l4;
        i0 = ReallocateFormatBuffer(ctx, i0, i1, i2);
        l5 = i0;
        i0 = 8u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l5;
        l4 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        l6 = i0;
        loop_49: 
          i0 = l4;
          i1 = l7;
          i0 += i1;
          i1 = l11;
          i2 = l6;
          i3 = 4u;
          i2 <<= (i3 & 31);
          i1 -= i2;
          i1 = LoadU32(ctx, i1);
          l8 = i1;
          i2 = 16u;
          i1 += i2;
          i2 = l8;
          i2 = LoadU32(ctx, i2 + 12u);
          l8 = i2;
          i0 = CopyBytes(ctx, i0, i1, i2);
          l12 = i0;
          i0 = l7;
          i1 = l8;
          i0 += i1;
          l7 = i0;
          i0 = l6;
          i1 = 1u;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          l8 = i0;
          i0 = l6;
          i1 = 1u;
          i0 -= i1;
          l6 = i0;
          i0 = l8;
          if (i0) {goto loop_49;}
      }
      i0 = l10;
      i1 = 9u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = l4;
        i2 = l7;
        i0 = InternString(ctx, i0, i1, i2);
        l5 = i0;
        i0 = 9u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l5;
        l6 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l11;
        i1 = l3;
        i2 = 4u;
        i1 <<= (i2 & 31);
        i0 -= i1;
        l4 = i0;
        i1 = 4u;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l4;
        i1 = l6;
        StoreU32(ctx, i0, i1);
        goto block_6;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = 2u;
      l3 = i0;
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = 3u;
      i0 = i0 != i1;
      l4 = i0;
      if (i0) {goto block_6;}
      i0 = l9;
      i1 = l6;
      d1 = LoadF64(ctx, i1);
      StoreF64(ctx, i0, d1);
      i0 = l9;
      i1 = 48u;
      i0 += i1;
      l4 = i0;
    }
    i0 = l10;
    i1 = 10u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l4;
      i1 = kF1110MsgG;
      i2 = l9;
      i0 = MeasureFormattedNumber(ctx, i0, i1, i2);
      l5 = i0;
      i0 = 10u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l9;
      i1 = 48u;
      i0 += i1;
      i0 = StringLength(ctx, i0);
      l7 = i0;
      i0 = l9;
      i1 = 48u;
      i0 += i1;
      l4 = i0;
    }
    i0 = l10;
    i1 = 11u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l4;
      i2 = l7;
      i0 = InternString(ctx, i0, i1, i2);
      l5 = i0;
      i0 = 11u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = 4u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l6;
      i1 = l4;
      StoreU32(ctx, i0, i1);
    }
    block_6:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = options;
      i1 = l3;
      i2 = 1u;
      i1 -= i2;
      l3 = i1;
      i0 -= i1;
      options = i0;
      i0 = value;
      i1 = l3;
      i0 -= i1;
      value = i0;
      i1 = 1u;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      l3 = i0;
      if (i0) {goto loop_4;}
    }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l9;
    i1 = 80u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
  }
  goto func_exit;
  spill_frame:;
  l5 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l5;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1110ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l5 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = value;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l5;
  i1 = options;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l5;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l5;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l5;
  i1 = l6;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l5;
  i1 = l7;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l5;
  i1 = l8;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l5;
  i1 = l9;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = l5;
  i1 = l11;
  StoreU32(ctx, i0 + 36u, i1);
  i0 = l5;
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
