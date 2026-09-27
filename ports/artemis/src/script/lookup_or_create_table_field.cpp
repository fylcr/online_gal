// lookup_or_create_table_field.cpp -- looks a field up, creating the slot on a miss and running __index and __newindex as the table demands..
//
// LookupOrCreateTableField -- looks a field up, creating the slot on a miss and running __index and __newindex as the table demands.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 480085-480085 (f1112); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void InitializeTableArray(EngineContext* ctx, uint32_t func_state, uint32_t table, uint32_t size);
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size);
uint32_t RaiseBlockTooBig(EngineContext* ctx, uint32_t a0);
void RaiseScriptError(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t FinishTableIndex(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t LookupTableField(EngineContext* ctx, uint32_t a0, uint32_t a1);

namespace {

constexpr uint32_t kF1112Const2B328 = 0x2B328u;  // 
constexpr uint32_t kF1112MsgTableIndexIsNil = 0x2BB93u;  // table index is nil
constexpr uint32_t kF1112MsgTableIndexIsNan = 0x2BBA6u;  // table index is NaN
constexpr uint32_t kF1112Const2BBD0 = 0x2BBD0u;  // 
constexpr uint32_t kF1112ConstFFFFFFF = 0xFFFFFFFu;  // 
constexpr uint32_t kF1112ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF1112SaveFrameBytes = 48u;
constexpr uint32_t kF1112ResumeWordBytes = 4u;

}  // namespace

void LookupOrCreateTableField(EngineContext* ctx, uint32_t state, uint32_t table, uint32_t key, uint32_t value) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l13 = 0u; uint64_t l14 = 0; double l15 = 0; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  uint64_t j1 = 0; uint64_t j2 = 0; uint64_t j3 = 0;
  double d0 = 0; double d1 = 0;
      l12 = 0, l13 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1112SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    key = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    value = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 24u);
    l7 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 28u);
    l8 = i0;
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
    i0 = LoadU32(ctx, i0 + 4u);
    table = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1112ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l9 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    i1 = 16u;
    i0 -= i1;
    l11 = i0;
    GuestStackSp(ctx) = i0;
    i0 = table;
    i0 = LoadU32(ctx, i0 + 16u);
    l12 = i0;
    i0 = table;
    i0 = LoadU8(ctx, i0 + 7u);
    l10 = i0;
    i0 = table;
    i0 = LoadU32(ctx, i0 + 28u);
    l8 = i0;
    i1 = key;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
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
      i0 = key;
      i1 = 1u;
      i0 += i1;
      i1 = kF1112ConstFFFFFFF;
      i0 = i0 <= i1;
      l6 = i0;
      i0 = table;
      l5 = i0;
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
        i0 = l8;
        i1 = 4u;
        i0 <<= (i1 & 31);
        l7 = i0;
        i0 = key;
        i1 = 4u;
        i0 <<= (i1 & 31);
        l13 = i0;
        i0 = table;
        i0 = LoadU32(ctx, i0 + 12u);
        l6 = i0;
      }
      i0 = l9;
      i0 = !(i0);
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = l6;
        i2 = l7;
        i3 = l13;
        i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
        l4 = i0;
        i0 = 0u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l4;
        l6 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_7;}
    }
    i0 = l9;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i0 = RaiseBlockTooBig(ctx, i0);
      l4 = i0;
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l6 = i0;
    }
    block_7:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i1 = l6;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = table;
      i0 = LoadU32(ctx, i0 + 28u);
      l5 = i0;
      i1 = key;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l7 = i0;
      if (i0) {
        loop_14: 
          i0 = l6;
          i1 = l5;
          i2 = 4u;
          i1 <<= (i2 & 31);
          i0 += i1;
          i1 = 0u;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l5;
          i1 = 1u;
          i0 += i1;
          l5 = i0;
          i1 = key;
          i0 = i0 != i1;
          l7 = i0;
          if (i0) {goto loop_14;}
      }
      i0 = table;
      i1 = key;
      StoreU32(ctx, i0 + 28u, i1);
    }
  }
  i0 = l9;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = table;
    i2 = value;
    InitializeTableArray(ctx, i0, i1, i2);
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = value;
  i1 = key;
  i2 = l8;
  i1 = (uint32_t)((int32_t)i1 < (int32_t)i2);
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  value = i0;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = table;
      i1 = key;
      StoreU32(ctx, i0 + 28u, i1);
      i0 = key;
      l5 = i0;
    }
    loop_18: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = table;
        i0 = LoadU32(ctx, i0 + 12u);
        i1 = l5;
        i2 = 4u;
        i1 <<= (i2 & 31);
        i0 += i1;
        l6 = i0;
        i0 = LoadU32(ctx, i0 + 8u);
        l7 = i0;
        i0 = l5;
        i1 = 1u;
        i0 += i1;
        value = i0;
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
          i0 = table;
          i0 = LoadU32(ctx, i0 + 28u);
          l7 = i0;
          i1 = l5;
          i0 = i0 > i1;
          l5 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l5;
          if (i0) {
            i0 = l6;
            l5 = i0;
            goto block_25;
          }
          i0 = value;
          d0 = (double)(int32_t)(i0);
          l15 = d0;
          i0 = value;
          i0 = !(i0);
          if (i0) {
            i0 = table;
            i0 = LoadU32(ctx, i0 + 16u);
            goto block_27;
          }
          i0 = table;
          i0 = LoadU32(ctx, i0 + 16u);
          d1 = l15;
          j1 = i64_reinterpret_f64(d1);
          l14 = j1;
          j2 = l14;
          j3 = 32ull;
          j2 >>= (j3 & 63);
          j1 += j2;
          i1 = (uint32_t)(j1);
          i2 = kF1112ConstFFFFFFFF;
          i3 = table;
          i3 = LoadU8(ctx, i3 + 7u);
          i2 <<= (i3 & 31);
          i3 = kF1112ConstFFFFFFFF;
          i2 ^= i3;
          i3 = 1u;
          i2 |= i3;
          i1 = REM_U(i1, i2);
          i2 = 5u;
          i1 <<= (i2 & 31);
          i0 += i1;
          block_27:;
          l5 = i0;
          loop_29: 
            i0 = l5;
            i0 = LoadU32(ctx, i0 + 24u);
            i1 = 3u;
            i0 = i0 == i1;
            l7 = i0;
            if (i0) {
              i0 = l5;
              d0 = LoadF64(ctx, i0 + 16u);
              d1 = l15;
              i0 = d0 == d1;
              l7 = i0;
              if (i0) {goto block_25;}
            }
            i0 = l5;
            i0 = LoadU32(ctx, i0 + 28u);
            l5 = i0;
            if (i0) {goto loop_29;}
          goto block_24;
          block_25:;
          i0 = l5;
          i1 = kF1112Const2B328;
          i0 = i0 != i1;
          l7 = i0;
          if (i0) {goto block_22;}
          i0 = value;
          d0 = (double)(int32_t)(i0);
          l15 = d0;
          block_24:;
          i0 = l11;
          i1 = 3u;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l11;
          d1 = l15;
          StoreF64(ctx, i0, d1);
        }
        i0 = l9;
        i1 = 3u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = state;
          i1 = table;
          i2 = l11;
          i0 = FinishTableIndex(ctx, i0, i1, i2);
          l4 = i0;
          i0 = 3u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l4;
          l5 = i0;
        }
        block_22:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l5;
          i1 = l6;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0, j1);
          i0 = l5;
          i1 = l6;
          i1 = LoadU32(ctx, i1 + 8u);
          l6 = i1;
          StoreU32(ctx, i0 + 8u, i1);
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i1 = value;
        l5 = i1;
        i0 = i0 != i1;
        value = i0;
        if (i0) {goto loop_18;}
      }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = key;
      i1 = 1u;
      i0 += i1;
      i1 = kF1112ConstFFFFFFF;
      i0 = i0 <= i1;
      l5 = i0;
      i0 = table;
      value = i0;
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
        i0 = table;
        i0 = LoadU32(ctx, i0 + 12u);
        l5 = i0;
        i0 = l8;
        i1 = 4u;
        i0 <<= (i1 & 31);
        l6 = i0;
        i0 = key;
        i1 = kF1112ResumeWordBytes;
        i0 <<= (i1 & 31);
        key = i0;
      }
      i0 = l9;
      i1 = 4u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = l5;
        i2 = l6;
        i3 = key;
        i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
        l4 = i0;
        i0 = 4u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l4;
        key = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_35;}
    }
    i0 = l9;
    i1 = 5u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i0 = RaiseBlockTooBig(ctx, i0);
      l4 = i0;
      i0 = 5u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      key = i0;
    }
    block_35:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = value;
      i1 = key;
      StoreU32(ctx, i0 + 12u, i1);
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l10;
    i1 = 31u;
    i0 = i0 != i1;
    value = i0;
    i0 = 1u;
    i1 = l10;
    i0 <<= (i1 & 31);
    key = i0;
  }
  i0 = value;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l6;
    i1 = key;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l6 = i0;
    loop_43: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l12;
        i1 = l6;
        i2 = 1u;
        i1 -= i2;
        l5 = i1;
        i2 = 5u;
        i1 <<= (i2 & 31);
        i0 += i1;
        l8 = i0;
        i0 = LoadU32(ctx, i0 + 8u);
        value = i0;
      }
      i0 = value;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = table;
          i1 = l8;
          i2 = 16u;
          i1 += i2;
          l10 = i1;
          i0 = LookupTableField(ctx, i0, i1);
          value = i0;
          i0 = table;
          i1 = 0u;
          StoreU8(ctx, i0 + 6u, i1);
          i0 = value;
          i1 = kF1112Const2B328;
          i0 = i0 == i1;
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
            i0 = l8;
            i0 = LoadU32(ctx, i0 + 24u);
            value = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = value;
            switch (i0) {
              case 0: goto block_53;
              case 1: goto block_50;
              case 2: goto block_50;
              case 3: goto block_51;
              default: goto block_50;
            }
            block_53:;
          }
          i0 = l9;
          i1 = 6u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = kF1112MsgTableIndexIsNil;
            i2 = 0u;
            RaiseScriptError(ctx, i0, i1, i2);
            i0 = 6u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {goto block_50;}
          block_51:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l10;
            d0 = LoadF64(ctx, i0);
            l15 = d0;
            d1 = l15;
            i0 = d0 == d1;
            value = i0;
            if (i0) {goto block_50;}
          }
          i0 = l9;
          i1 = 7u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = kF1112MsgTableIndexIsNan;
            i2 = 0u;
            RaiseScriptError(ctx, i0, i1, i2);
            i0 = 7u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          block_50:;
          i0 = l9;
          i1 = 8u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = table;
            i2 = l10;
            i0 = FinishTableIndex(ctx, i0, i1, i2);
            l4 = i0;
            i0 = 8u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l4;
          } else {
            i0 = value;
          }
          value = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = value;
          i1 = l8;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0, j1);
          i0 = value;
          i1 = l8;
          i1 = LoadU32(ctx, i1 + 8u);
          l8 = i1;
          StoreU32(ctx, i0 + 8u, i1);
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        i1 = 1u;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        value = i0;
        i0 = l5;
        l6 = i0;
        i0 = value;
        if (i0) {goto loop_43;}
      }
  }
  i0 = table;
  i1 = l12;
  i2 = kF1112Const2BBD0;
  i1 = i1 != i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  table = i0;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = table;
    i1 = key;
    i2 = 5u;
    i1 <<= (i2 & 31);
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    table = i0;
    i0 = l9;
    i1 = 9u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l12;
      i2 = table;
      i3 = 0u;
      i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
      i0 = 9u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l11;
    i1 = 16u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
  }
  goto func_exit;
  spill_frame:;
  l4 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l4;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1112ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = table;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l4;
  i1 = key;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l4;
  i1 = value;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l4;
  i1 = l5;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l4;
  i1 = l6;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l4;
  i1 = l7;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l4;
  i1 = l8;
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
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 48u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
