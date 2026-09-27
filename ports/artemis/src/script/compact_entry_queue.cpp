// compact_entry_queue.cpp -- slides the live entries of a queue together and resets the tail past the holes..
//
// CompactEntryQueue -- slides the live entries of a queue together and resets the tail past the holes.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 489298-489298 (f1128); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void RelocateElementArray(EngineContext* ctx, uint32_t state, uint32_t table);

namespace {

constexpr uint32_t kF1128ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF1128SaveFrameBytes = 40u;
constexpr uint32_t kF1128ResumeWordBytes = 4u;

}  // namespace

uint32_t CompactEntryQueue(EngineContext* ctx, uint32_t state, uint32_t queue) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  uint64_t j1 = 0;
      l10 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1128SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l6 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 4u);
    queue = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 20u);
    l7 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 24u);
    l8 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 28u);
    l9 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 32u);
    l10 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 36u);
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1128ResumeWordBytes;
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
    i1 = 112u;
    i0 -= i1;
    l9 = i0;
    GuestStackSp(ctx) = i0;
    i0 = state;
    i0 = LoadU8(ctx, i0 + 56u);
    i1 = 2u;
    i0 &= i1;
    i0 = !(i0);
    l7 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 20u);
      l4 = i0;
      goto running_exit;
    }
    i0 = state;
    i0 = LoadU32(ctx, i0 + 68u);
    l8 = i0;
    i0 = !(i0);
    l3 = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 32u);
    l7 = i0;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    if (i0) {goto block_7;}
    i0 = l7;
    l2 = i0;
    i0 = state;
    i0 = LoadU8(ctx, i0 + 57u);
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_7;}
    i0 = state;
    i0 = LoadU32(ctx, i0 + 20u);
    l2 = i0;
    i0 = LoadU32(ctx, i0 + 8u);
    l10 = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
    i0 = l9;
    i1 = kF1128ConstFFFFFFFF;
    StoreU32(ctx, i0 + 28u, i1);
    i0 = l9;
    i1 = 1u;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l9;
    i1 = l2;
    i2 = state;
    i2 = LoadU32(ctx, i2 + 40u);
    l3 = i2;
    i1 -= i2;
    i2 = 24u;
    i1 = I32_DIV_S(i1, i2);
    StoreU32(ctx, i0 + 104u, i1);
    i0 = state;
    i0 = LoadU32(ctx, i0 + 28u);
    i1 = l4;
    i0 -= i1;
    i1 = 320u;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    l2 = i0;
  }
  i0 = l2;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 44u);
      l2 = i0;
      i1 = 20u;
      i0 += i1;
      i1 = l2;
      i2 = 1u;
      i1 <<= (i2 & 31);
      l3 = i1;
      i2 = l2;
      i3 = 20u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      i0 = i2 ? i0 : i1;
      l2 = i0;
    }
    i0 = l5;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l2;
      RelocateElementArray(ctx, i0, i1);
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 20u);
    l3 = i0;
    i1 = state;
    i1 = LoadU32(ctx, i1 + 8u);
    i2 = 320u;
    i1 += i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = state;
    i1 = 0u;
    StoreU8(ctx, i0 + 57u, i1);
    i0 = l9;
    i1 = 8u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l5;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l2;
    i2 = l8;
    CallTableT0(ctx, i2, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
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
    i1 = 1u;
    StoreU8(ctx, i0 + 57u, i1);
    i0 = state;
    i0 = LoadU32(ctx, i0 + 20u);
    i1 = state;
    i1 = LoadU32(ctx, i1 + 32u);
    l2 = i1;
    i2 = l10;
    i3 = l7;
    i2 -= i3;
    i1 += i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = state;
    i1 = l2;
    i2 = l4;
    i3 = l7;
    i2 -= i3;
    i1 += i2;
    l3 = i1;
    StoreU32(ctx, i0 + 8u, i1);
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 20u);
    l4 = i0;
    i0 = LoadU32(ctx, i0 + 4u);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU8(ctx, i0 + 6u);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    if (i0) {goto block_17;}
    i0 = state;
    i0 = LoadU8(ctx, i0 + 56u);
    l8 = i0;
    i1 = 2u;
    i0 &= i1;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_17;}
  }
  loop_19: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 20u);
      l10 = i1;
      i2 = 1u;
      i1 -= i2;
      StoreU32(ctx, i0 + 20u, i1);
      i0 = l10;
      i0 = !(i0);
      if (i0) {goto block_17;}
      i0 = state;
      i0 = LoadU32(ctx, i0 + 68u);
      l10 = i0;
      i0 = !(i0);
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      if (i0) {goto block_21;}
      i0 = state;
      i0 = LoadU8(ctx, i0 + 57u);
      i0 = !(i0);
      if (i0) {goto block_21;}
      i0 = state;
      i0 = LoadU32(ctx, i0 + 8u);
      l8 = i0;
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 8u);
      l6 = i0;
      i0 = l9;
      i1 = 0u;
      StoreU32(ctx, i0 + 104u, i1);
      i0 = l9;
      i1 = kF1128ConstFFFFFFFF;
      StoreU32(ctx, i0 + 28u, i1);
      i0 = l9;
      i1 = 4u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = state;
      i0 = LoadU32(ctx, i0 + 28u);
      i1 = l8;
      i0 -= i1;
      i1 = 320u;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
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
        i0 = state;
        i0 = LoadU32(ctx, i0 + 44u);
        l4 = i0;
        i1 = 1u;
        i0 <<= (i1 & 31);
        l3 = i0;
        i0 = l4;
        i1 = 20u;
        i0 += i1;
        i1 = l3;
        i2 = l4;
        i3 = 20u;
        i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
        i0 = i2 ? i0 : i1;
        l3 = i0;
      }
      i0 = l5;
      i1 = 2u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = l3;
        RelocateElementArray(ctx, i0, i1);
        i0 = 2u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 20u);
      i1 = state;
      i1 = LoadU32(ctx, i1 + 8u);
      i2 = 320u;
      i1 += i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = state;
      i1 = 0u;
      StoreU8(ctx, i0 + 57u, i1);
      i0 = l9;
      i1 = 8u;
      i0 += i1;
      l3 = i0;
      i0 = l8;
      i1 = l2;
      i0 -= i1;
      l8 = i0;
    }
    i0 = l5;
    i1 = 3u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l3;
      i2 = l10;
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
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i1 = 1u;
      StoreU8(ctx, i0 + 57u, i1);
      i0 = state;
      i0 = LoadU32(ctx, i0 + 20u);
      l4 = i0;
      i1 = l6;
      i2 = l2;
      i1 -= i2;
      i2 = state;
      i2 = LoadU32(ctx, i2 + 32u);
      l2 = i2;
      i1 += i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = state;
      i1 = l2;
      i2 = l8;
      i1 += i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = state;
      i0 = LoadU8(ctx, i0 + 56u);
      l8 = i0;
    }
    block_21:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l8;
      i1 = 2u;
      i0 &= i1;
      l3 = i0;
      if (i0) {goto loop_19;}
    }
  block_17:;
  i0 = queue;
  i1 = l2;
  i2 = queue;
  i3 = l7;
  i2 -= i3;
  i1 += i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  queue = i0;
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i1 = l4;
    i2 = 24u;
    i1 -= i2;
    l2 = i1;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l7 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 4u);
    l4 = i0;
    i0 = state;
    i1 = l2;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 12u, i1);
    i0 = state;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 12u);
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l7;
    i0 = !(i0);
    if (i0) {goto block_32;}
    i0 = l7;
    l2 = i0;
    loop_33: 
      i0 = state;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = queue;
      i0 = i0 > i1;
      if (i0) {
        i0 = l4;
        i1 = queue;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0, j1);
        i0 = l4;
        i1 = queue;
        i1 = LoadU32(ctx, i1 + 8u);
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l4;
        i1 = 16u;
        i0 += i1;
        l4 = i0;
        i0 = queue;
        i1 = 16u;
        i0 += i1;
        queue = i0;
        i0 = l2;
        i1 = 1u;
        i0 -= i1;
        l2 = i0;
        if (i0) {goto loop_33;}
        goto block_32;
      }
    i0 = l2;
    i1 = 1u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {goto block_32;}
    loop_35: 
      i0 = l4;
      i1 = 0u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l4;
      i1 = 16u;
      i0 += i1;
      l4 = i0;
      i0 = l2;
      i1 = 1u;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      queue = i0;
      i0 = l2;
      i1 = 1u;
      i0 -= i1;
      l2 = i0;
      i0 = queue;
      if (i0) {goto loop_35;}
    block_32:;
    i0 = state;
    i1 = l4;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l9;
    i1 = 112u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    i0 = l7;
    i1 = 1u;
    i0 += i1;
    goto func_exit;
  }
  TrapUnreachable();
  spill_frame:;
  l5 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l5;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1128ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l5 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = queue;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l5;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l5;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l5;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l5;
  i1 = l7;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l5;
  i1 = l8;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l5;
  i1 = l9;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l5;
  i1 = l10;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = l5;
  i1 = l6;
  StoreU32(ctx, i0 + 36u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 40u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
