// format_number.cpp -- the number-to-string conversion, including the 14 significant digit default and the null fallback..
//
// FormatNumber -- the number-to-string conversion, including the 14 significant digit default and the null fallback.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 353732-353732 (f900); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void FormatNumberValue(EngineContext* ctx, uint32_t state, uint32_t value, uint32_t options);
uint32_t StringLength(EngineContext* ctx, uint32_t text);
uint32_t InternString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void GrowValueStack(EngineContext* ctx, uint32_t a, uint32_t b);
void StoreSuspendableEntry(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t FindPoolEntryByTagByte(EngineContext* ctx, uint32_t pool, uint32_t key);

namespace {

constexpr uint32_t kF900Const2B440 = 0x2B440u;  // 
constexpr uint32_t kF900MsgNull = 0x2B441u;  // (null)
constexpr uint32_t kF900Const2B448 = 0x2B448u;  // 
constexpr uint32_t kF900Const2B44B = 0x2B44Bu;  // 
constexpr uint32_t kF900ConstFFFFFFF8 = 0xFFFFFFF8u;  // 
constexpr uint32_t kF900SaveFrameBytes = 72u;
constexpr uint32_t kF900ResumeWordBytes = 4u;

}  // namespace

uint32_t FormatNumber(EngineContext* ctx, uint32_t state, uint32_t value, uint32_t options) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l13 = 0u; uint32_t l14 = 0u; uint32_t l15 = 0u; uint32_t l16 = 0u; uint32_t l17 = 0u; uint32_t l18 = 0u; uint32_t l19 = 0u; double l20 = 0; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  double d1 = 0; double d2 = 0;
      l11 = 0, l12 = 0, l13 = 0, l14 = 0, l15 = 0, l16 = 0, l17 = 0, l18 = 0, 
      l19 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF900SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    options = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l6 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 20u);
    l7 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 24u);
    l8 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 28u);
    l9 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 32u);
    l10 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 36u);
    l11 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 40u);
    l12 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 44u);
    l13 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 48u);
    l14 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 52u);
    l15 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 56u);
    l16 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 60u);
    l17 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 64u);
    l18 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 68u);
    l19 = i0;
    i0 = l4;
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
    i2 = kF900ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    i1 = 48u;
    i0 -= i1;
    l6 = i0;
    GuestStackSp(ctx) = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
  }
  i0 = l5;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = kF900Const2B440;
    i2 = 0u;
    i0 = InternString(ctx, i0, i1, i2);
    l4 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    l9 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = kF900ResumeWordBytes;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = l9;
    StoreU32(ctx, i0, i1);
    i0 = state;
    i0 = LoadU32(ctx, i0 + 28u);
    i1 = state;
    i1 = LoadU32(ctx, i1 + 8u);
    l3 = i1;
    i0 -= i1;
    i1 = 16u;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    l9 = i0;
    i0 = state;
    l7 = i0;
  }
  i0 = l11;
  i1 = l9;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l11 = i0;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l5;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = 1u;
      GrowValueStack(ctx, i0, i1);
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l3;
    } else {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 8u);
    }
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    i1 = l3;
    i2 = 16u;
    i1 += i2;
    l3 = i1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = value;
    i1 = 37u;
    i0 = FindPoolEntryByTagByte(ctx, i0, i1);
    l10 = i0;
    i0 = !(i0);
    l7 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    if (i0) {
      i0 = 1u;
      l9 = i0;
      goto block_10;
    }
    i0 = 1u;
    l9 = i0;
  }
  loop_13: 
    i0 = l7;
    i1 = l10;
    i2 = value;
    i1 -= i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l7 = i0;
    i0 = l5;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = value;
      i2 = l7;
      i0 = InternString(ctx, i0, i1, i2);
      l4 = i0;
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      value = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 4u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = value;
      StoreU32(ctx, i0, i1);
      i0 = state;
      i0 = LoadU32(ctx, i0 + 28u);
      i1 = state;
      i1 = LoadU32(ctx, i1 + 8u);
      l3 = i1;
      i0 -= i1;
      i1 = 16u;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      value = i0;
    }
    i0 = value;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l5;
      i1 = 3u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = 1u;
        GrowValueStack(ctx, i0, i1);
        i0 = 3u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l3;
      } else {
        i0 = state;
        i0 = LoadU32(ctx, i0 + 8u);
      }
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i1 = l3;
      i2 = 16u;
      i1 += i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l10;
      i0 = LoadS8(ctx, i0 + 1u);
      value = i0;
      i1 = 99u;
      i0 -= i1;
      l8 = i0;
      i0 = state;
      l7 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l8;
      switch (i0) {
        case 0: goto block_28;
        case 1: goto block_27;
        case 2: goto block_23;
        case 3: goto block_26;
        case 4: goto block_23;
        case 5: goto block_23;
        case 6: goto block_23;
        case 7: goto block_23;
        case 8: goto block_23;
        case 9: goto block_23;
        case 10: goto block_23;
        case 11: goto block_23;
        case 12: goto block_23;
        case 13: goto block_25;
        case 14: goto block_23;
        case 15: goto block_23;
        case 16: goto block_30;
        default: goto block_24;
      }
      block_30:;
      i0 = options;
      i0 = LoadU32(ctx, i0);
      value = i0;
      i1 = kF900MsgNull;
      i2 = value;
      i0 = i2 ? i0 : i1;
      value = i0;
      i0 = StringLength(ctx, i0);
      l8 = i0;
    }
    i0 = l5;
    i1 = 4u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = value;
      i2 = l8;
      i0 = InternString(ctx, i0, i1, i2);
      l4 = i0;
      i0 = 4u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      value = i0;
    }
    i0 = l12;
    i1 = ResumePhase(ctx);
    i1 = !(i1);
    if (i1) {
      i1 = l3;
      i2 = 4u;
      StoreU32(ctx, i1 + 24u, i2);
      i1 = l3;
      i2 = value;
      StoreU32(ctx, i1 + 16u, i2);
      i1 = options;
      i2 = 4u;
      i1 += i2;
      options = i1;
      i1 = state;
      i1 = LoadU32(ctx, i1 + 28u);
      i2 = state;
      i2 = LoadU32(ctx, i2 + 8u);
      l3 = i2;
      i1 -= i2;
      i2 = 16u;
      i1 = (uint32_t)((int32_t)i1 <= (int32_t)i2);
      value = i1;
    }
    i1 = value;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l12 = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l5;
      i1 = 5u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = 1u;
        GrowValueStack(ctx, i0, i1);
        i0 = 5u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l3;
      } else {
        i0 = state;
        i0 = LoadU32(ctx, i0 + 8u);
      }
      l3 = i0;
    }
    i0 = l3;
    i1 = 16u;
    i0 += i1;
    i1 = ResumePhase(ctx);
    i1 = !(i1);
    if (i1) {goto block_21;}
    block_28:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = options;
      i0 = LoadU32(ctx, i0);
      value = i0;
      i0 = l6;
      i1 = 0u;
      StoreU8(ctx, i0 + 17u, i1);
      i0 = l6;
      i1 = value;
      StoreU8(ctx, i0 + 16u, i1);
      i0 = l6;
      i1 = 16u;
      i0 += i1;
      i0 = StringLength(ctx, i0);
      l8 = i0;
      i0 = l6;
      i1 = 16u;
      i0 += i1;
      value = i0;
    }
    i0 = l5;
    i1 = 6u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = value;
      i2 = l8;
      i0 = InternString(ctx, i0, i1, i2);
      l4 = i0;
      i0 = 6u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      value = i0;
    }
    i0 = l13;
    i1 = ResumePhase(ctx);
    i1 = !(i1);
    if (i1) {
      i1 = l3;
      i2 = 4u;
      StoreU32(ctx, i1 + 24u, i2);
      i1 = l3;
      i2 = value;
      StoreU32(ctx, i1 + 16u, i2);
      i1 = options;
      i2 = 4u;
      i1 += i2;
      options = i1;
      i1 = state;
      i1 = LoadU32(ctx, i1 + 28u);
      i2 = state;
      i2 = LoadU32(ctx, i2 + 8u);
      l3 = i2;
      i1 -= i2;
      i2 = 16u;
      i1 = (uint32_t)((int32_t)i1 <= (int32_t)i2);
      value = i1;
    }
    i1 = value;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l13 = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l5;
      i1 = 7u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = 1u;
        GrowValueStack(ctx, i0, i1);
        i0 = 7u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l3;
      } else {
        i0 = state;
        i0 = LoadU32(ctx, i0 + 8u);
      }
      l3 = i0;
    }
    i0 = l3;
    i1 = 16u;
    i0 += i1;
    i1 = ResumePhase(ctx);
    i1 = !(i1);
    if (i1) {goto block_21;}
    block_27:;
    i0 = l14;
    i1 = ResumePhase(ctx);
    i1 = !(i1);
    if (i1) {
      i1 = options;
      i1 = LoadU32(ctx, i1);
      value = i1;
      i1 = l3;
      i2 = 3u;
      StoreU32(ctx, i1 + 24u, i2);
      i1 = l3;
      i2 = value;
      d2 = (double)(int32_t)(i2);
      StoreF64(ctx, i1 + 16u, d2);
      i1 = options;
      i2 = 4u;
      i1 += i2;
      options = i1;
      i1 = state;
      i1 = LoadU32(ctx, i1 + 28u);
      i2 = state;
      i2 = LoadU32(ctx, i2 + 8u);
      l3 = i2;
      i1 -= i2;
      i2 = 16u;
      i1 = (uint32_t)((int32_t)i1 <= (int32_t)i2);
      value = i1;
    }
    i1 = value;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l14 = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l5;
      i1 = 8u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = 1u;
        GrowValueStack(ctx, i0, i1);
        i0 = 8u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l3;
      } else {
        i0 = state;
        i0 = LoadU32(ctx, i0 + 8u);
      }
      l3 = i0;
    }
    i0 = l3;
    i1 = 16u;
    i0 += i1;
    i1 = ResumePhase(ctx);
    i1 = !(i1);
    if (i1) {goto block_21;}
    block_26:;
    i0 = l15;
    i1 = ResumePhase(ctx);
    i1 = !(i1);
    if (i1) {
      i1 = options;
      i2 = 7u;
      i1 += i2;
      i2 = kF900ConstFFFFFFF8;
      i1 &= i2;
      value = i1;
      d1 = LoadF64(ctx, i1);
      l20 = d1;
      i1 = l3;
      i2 = 3u;
      StoreU32(ctx, i1 + 24u, i2);
      i1 = l3;
      d2 = l20;
      StoreF64(ctx, i1 + 16u, d2);
      i1 = value;
      i2 = 8u;
      i1 += i2;
      options = i1;
      i1 = state;
      i1 = LoadU32(ctx, i1 + 28u);
      i2 = state;
      i2 = LoadU32(ctx, i2 + 8u);
      l3 = i2;
      i1 -= i2;
      i2 = 16u;
      i1 = (uint32_t)((int32_t)i1 <= (int32_t)i2);
      value = i1;
    }
    i1 = value;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l15 = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l5;
      i1 = 9u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = 1u;
        GrowValueStack(ctx, i0, i1);
        i0 = 9u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l3;
      } else {
        i0 = state;
        i0 = LoadU32(ctx, i0 + 8u);
      }
      l3 = i0;
    }
    i0 = l3;
    i1 = 16u;
    i0 += i1;
    i1 = ResumePhase(ctx);
    i1 = !(i1);
    if (i1) {goto block_21;}
    block_25:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = options;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 16u;
      i0 += i1;
      value = i0;
    }
    i0 = l5;
    i1 = 10u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = value;
      i1 = kF900Const2B448;
      i2 = l6;
      StoreSuspendableEntry(ctx, i0, i1, i2);
      i0 = 10u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 8u);
      value = i0;
      i0 = l6;
      i1 = 16u;
      i0 += i1;
      i0 = StringLength(ctx, i0);
      l8 = i0;
      i0 = l6;
      i1 = 16u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l5;
    i1 = 11u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l3;
      i2 = l8;
      i0 = InternString(ctx, i0, i1, i2);
      l4 = i0;
      i0 = 11u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l3 = i0;
    }
    i0 = l16;
    i1 = ResumePhase(ctx);
    i1 = !(i1);
    if (i1) {
      i1 = value;
      i2 = 4u;
      StoreU32(ctx, i1 + 8u, i2);
      i1 = value;
      i2 = l3;
      StoreU32(ctx, i1, i2);
      i1 = options;
      i2 = 4u;
      i1 += i2;
      options = i1;
      i1 = state;
      i1 = LoadU32(ctx, i1 + 28u);
      i2 = state;
      i2 = LoadU32(ctx, i2 + 8u);
      l3 = i2;
      i1 -= i2;
      i2 = 16u;
      i1 = (uint32_t)((int32_t)i1 <= (int32_t)i2);
      value = i1;
    }
    i1 = value;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l16 = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l5;
      i1 = 12u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = 1u;
        GrowValueStack(ctx, i0, i1);
        i0 = 12u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l3;
      } else {
        i0 = state;
        i0 = LoadU32(ctx, i0 + 8u);
      }
      l3 = i0;
    }
    i0 = l3;
    i1 = 16u;
    i0 += i1;
    i1 = ResumePhase(ctx);
    i1 = !(i1);
    if (i1) {goto block_21;}
    block_24:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = value;
      i1 = 37u;
      i0 = i0 == i1;
      l8 = i0;
      if (i0) {goto block_22;}
    }
    block_23:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = 0u;
      StoreU8(ctx, i0 + 18u, i1);
      i0 = l6;
      i1 = value;
      StoreU8(ctx, i0 + 17u, i1);
      i0 = l6;
      i1 = 37u;
      StoreU8(ctx, i0 + 16u, i1);
      i0 = l6;
      i1 = 16u;
      i0 += i1;
      i0 = StringLength(ctx, i0);
      l8 = i0;
      i0 = l6;
      i1 = 16u;
      i0 += i1;
      value = i0;
    }
    i0 = l5;
    i1 = 13u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = value;
      i2 = l8;
      i0 = InternString(ctx, i0, i1, i2);
      l4 = i0;
      i0 = 13u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      value = i0;
    }
    i0 = l17;
    i1 = ResumePhase(ctx);
    i1 = !(i1);
    if (i1) {
      i1 = l3;
      i2 = 4u;
      StoreU32(ctx, i1 + 24u, i2);
      i1 = l3;
      i2 = value;
      StoreU32(ctx, i1 + 16u, i2);
      i1 = state;
      i1 = LoadU32(ctx, i1 + 28u);
      i2 = state;
      i2 = LoadU32(ctx, i2 + 8u);
      l3 = i2;
      i1 -= i2;
      i2 = 16u;
      i1 = (uint32_t)((int32_t)i1 <= (int32_t)i2);
      value = i1;
    }
    i1 = value;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l17 = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l5;
      i1 = 14u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = 1u;
        GrowValueStack(ctx, i0, i1);
        i0 = 14u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l3;
      } else {
        i0 = state;
        i0 = LoadU32(ctx, i0 + 8u);
      }
      l3 = i0;
    }
    i0 = l3;
    i1 = 16u;
    i0 += i1;
    i1 = ResumePhase(ctx);
    i1 = !(i1);
    if (i1) {goto block_21;}
    block_22:;
    i0 = l5;
    i1 = 15u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = kF900Const2B44B;
      i2 = 1u;
      i0 = InternString(ctx, i0, i1, i2);
      l4 = i0;
      i0 = 15u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      value = i0;
    }
    i0 = l18;
    i1 = ResumePhase(ctx);
    i1 = !(i1);
    if (i1) {
      i1 = l3;
      i2 = 4u;
      StoreU32(ctx, i1 + 24u, i2);
      i1 = l3;
      i2 = value;
      StoreU32(ctx, i1 + 16u, i2);
      i1 = state;
      i1 = LoadU32(ctx, i1 + 28u);
      i2 = state;
      i2 = LoadU32(ctx, i2 + 8u);
      l3 = i2;
      i1 -= i2;
      i2 = 16u;
      i1 = (uint32_t)((int32_t)i1 <= (int32_t)i2);
      value = i1;
    }
    i1 = value;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l18 = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l5;
      i1 = 16u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = 1u;
        GrowValueStack(ctx, i0, i1);
        i0 = 16u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l3;
      } else {
        i0 = state;
        i0 = LoadU32(ctx, i0 + 8u);
      }
      l3 = i0;
    }
    i0 = l3;
    i1 = l3;
    i2 = 16u;
    i1 += i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    block_21:;
    l3 = i0;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l7;
      i1 = l3;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l9;
      i1 = 2u;
      i0 += i1;
      l9 = i0;
      i0 = l10;
      i1 = 2u;
      i0 += i1;
      value = i0;
      i1 = 37u;
      i0 = FindPoolEntryByTagByte(ctx, i0, i1);
      l10 = i0;
      if (i0) {goto loop_13;}
    }
  block_10:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = value;
    i0 = StringLength(ctx, i0);
    options = i0;
  }
  i0 = l5;
  i1 = 17u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = value;
    i2 = options;
    i0 = InternString(ctx, i0, i1, i2);
    l4 = i0;
    i0 = 17u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    value = i0;
  }
  i0 = l19;
  i1 = ResumePhase(ctx);
  i1 = !(i1);
  if (i1) {
    i1 = l3;
    i2 = 4u;
    StoreU32(ctx, i1 + 8u, i2);
    i1 = l3;
    i2 = value;
    StoreU32(ctx, i1, i2);
    i1 = state;
    options = i1;
    i1 = state;
    i1 = LoadU32(ctx, i1 + 28u);
    i2 = state;
    i2 = LoadU32(ctx, i2 + 8u);
    l3 = i2;
    i1 -= i2;
    i2 = 16u;
    i1 = (uint32_t)((int32_t)i1 <= (int32_t)i2);
    value = i1;
  }
  i1 = value;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l19 = i0;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l5;
    i1 = 18u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = 1u;
      GrowValueStack(ctx, i0, i1);
      i0 = 18u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l3;
    } else {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 8u);
    }
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = options;
    i1 = l3;
    i2 = 16u;
    i1 += i2;
    value = i1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    options = i0;
    i0 = value;
    i1 = state;
    i1 = LoadU32(ctx, i1 + 12u);
    i0 -= i1;
    i1 = 4u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    i1 = 1u;
    i0 -= i1;
    value = i0;
  }
  i0 = l5;
  i1 = 19u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = options;
    i2 = value;
    FormatNumberValue(ctx, i0, i1, i2);
    i0 = 19u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    value = i0;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = l9;
    i2 = 4u;
    i1 <<= (i2 & 31);
    i0 -= i1;
    state = i0;
    i0 = value;
    i1 = state;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = state;
    i1 = 16u;
    i0 -= i1;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l6;
    i1 = 48u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    i0 = state;
    i1 = 16u;
    i0 += i1;
    goto func_exit;
  }
  TrapUnreachable();
  spill_frame:;
  l4 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l4;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF900ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = value;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l4;
  i1 = options;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l4;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l4;
  i1 = l6;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l4;
  i1 = l7;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l4;
  i1 = l8;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l4;
  i1 = l9;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l4;
  i1 = l10;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = l4;
  i1 = l11;
  StoreU32(ctx, i0 + 36u, i1);
  i0 = l4;
  i1 = l12;
  StoreU32(ctx, i0 + 40u, i1);
  i0 = l4;
  i1 = l13;
  StoreU32(ctx, i0 + 44u, i1);
  i0 = l4;
  i1 = l14;
  StoreU32(ctx, i0 + 48u, i1);
  i0 = l4;
  i1 = l15;
  StoreU32(ctx, i0 + 52u, i1);
  i0 = l4;
  i1 = l16;
  StoreU32(ctx, i0 + 56u, i1);
  i0 = l4;
  i1 = l17;
  StoreU32(ctx, i0 + 60u, i1);
  i0 = l4;
  i1 = l18;
  StoreU32(ctx, i0 + 64u, i1);
  i0 = l4;
  i1 = l19;
  StoreU32(ctx, i0 + 68u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 72u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
