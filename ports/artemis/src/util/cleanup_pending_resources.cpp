// cleanup_pending_resources.cpp -- the C++ unwind and guard cleanup: drives each pending destructor through the try/resume handoff on the asyncify throw slot, frees what the guards own, and releases the collectors the frame held..
//
// CleanupPendingResources -- the C++ unwind and guard cleanup: drives each pending destructor through the try/resume handoff on the asyncify throw slot, frees what the guards own, and releases the collectors the frame held.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 2251235-2252539 (f4082); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
uint32_t CopyBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t StringLength(EngineContext* ctx, uint32_t text);
void RaiseStringLengthError(EngineContext* ctx, uint32_t a0);

namespace {

constexpr uint32_t kF4082ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF4082Const80000000 = 0x80000000u;  // 
constexpr uint32_t kF4082ConstFFFFFFF0 = 0xFFFFFFF0u;  // 
constexpr uint32_t kF4082ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF4082SaveFrameBytes = 48u;
constexpr uint32_t kF4082ResumeWordBytes = 4u;

}  // namespace

void CleanupPendingResources(EngineContext* ctx, uint32_t p0, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l13 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  uint64_t j1 = 0;
      l13 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF4082SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l7 = i0;
    i0 = LoadU32(ctx, i0);
    p0 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 8u);
    p2 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 12u);
    p3 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 16u);
    p4 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 24u);
    l5 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 28u);
    l8 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 32u);
    l9 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 36u);
    l10 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 40u);
    l11 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 44u);
    l13 = i0;
    i0 = l7;
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
    i2 = kF4082ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l12 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    i1 = 16u;
    i0 -= i1;
    l6 = i0;
    GuestStackSp(ctx) = i0;
    i0 = l6;
    i1 = 0u;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l6;
    j1 = 0ull;
    StoreU64(ctx, i0, j1);
    i0 = p1;
    i0 = LoadU32(ctx, i0 + 4u);
    l5 = i0;
    i1 = p1;
    i1 = LoadU32(ctx, i1 + 8u);
    i0 = i0 < i1;
    l8 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l8;
    if (i0) {
      i0 = l5;
      i1 = l6;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = l5;
      i1 = l6;
      i1 = LoadU32(ctx, i1 + 8u);
      l8 = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = p1;
      i1 = l5;
      i2 = 12u;
      i1 += i2;
      l5 = i1;
      StoreU32(ctx, i0 + 4u, i1);
      goto block_8;
    }
    i0 = kF4082ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l12;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1412u;
    i1 = p1;
    i2 = l6;
    HostOsSchedule(ctx, i0, i1, i2);
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF4082ConstD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF4082ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_7;}
    i0 = l6;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF4082ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    l5 = i0;
    if (i0) {goto block_8;}
    i0 = l6;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  i1 = 0u;
  i2 = ResumePhase(ctx);
  i2 = !(i2);
  if (i2) {
    i2 = p0;
    i2 = LoadU8(ctx, i2);
    l8 = i2;
    i2 = !(i2);
    l5 = i2;
  }
  i2 = l5;
  i0 = i2 ? i0 : i1;
  if (i0) {goto block_14;}
  loop_17: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = p0;
      l9 = i0;
      i0 = p3;
      i0 = LoadS8(ctx, i0 + 11u);
      p0 = i0;
      i1 = kF4082ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      l5 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      if (i0) {
        i0 = p0;
        i1 = 255u;
        i0 &= i1;
        l10 = i0;
        i0 = 0u;
        p0 = i0;
        loop_24: 
          i0 = p0;
          i1 = l10;
          i0 = i0 == i1;
          l5 = i0;
          if (i0) {goto block_22;}
          i0 = p0;
          i1 = p3;
          i0 += i1;
          l5 = i0;
          i0 = p0;
          i1 = 1u;
          i0 += i1;
          p0 = i0;
          i0 = l5;
          i0 = LoadU8(ctx, i0);
          i1 = l8;
          i2 = 255u;
          i1 &= i2;
          i0 = i0 != i1;
          l5 = i0;
          if (i0) {goto loop_24;}
        goto block_20;
      }
      i0 = p3;
      i0 = LoadU32(ctx, i0);
      l7 = i0;
      i0 = p3;
      i0 = LoadU32(ctx, i0 + 4u);
      l10 = i0;
      i0 = 0u;
      p0 = i0;
      loop_25: 
        i0 = p0;
        i1 = l10;
        i0 = i0 == i1;
        l5 = i0;
        if (i0) {goto block_22;}
        i0 = p0;
        i1 = l7;
        i0 += i1;
        l5 = i0;
        i0 = p0;
        i1 = 1u;
        i0 += i1;
        p0 = i0;
        i0 = l8;
        i1 = 255u;
        i0 &= i1;
        l13 = i0;
        i1 = l5;
        i1 = LoadU8(ctx, i1);
        i0 = i0 != i1;
        l5 = i0;
        if (i0) {goto loop_25;}
      goto block_20;
      block_22:;
      i0 = l9;
      l10 = i0;
      i0 = l8;
      i1 = 255u;
      i0 &= i1;
      l13 = i0;
      i0 = !(i0);
      p0 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = p0;
      if (i0) {goto block_28;}
      i0 = p2;
      i0 = LoadU32(ctx, i0);
      i1 = p2;
      i2 = p2;
      i2 = LoadS8(ctx, i2 + 11u);
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      i0 = i2 ? i0 : i1;
      p0 = i0;
      i0 = l8;
      l7 = i0;
      i0 = l9;
      l5 = i0;
      loop_29: 
        i0 = p0;
        i0 = LoadU8(ctx, i0);
        l10 = i0;
        i0 = !(i0);
        if (i0) {
          i0 = l5;
          l10 = i0;
          goto block_28;
        }
        i0 = l7;
        i1 = 255u;
        i0 &= i1;
        i1 = l10;
        i0 = i0 != i1;
        l10 = i0;
        if (i0) {goto block_26;}
        i0 = p0;
        i1 = 1u;
        i0 += i1;
        p0 = i0;
        i0 = l5;
        i0 = LoadU8(ctx, i0 + 1u);
        l7 = i0;
        i0 = l5;
        i1 = 1u;
        i0 += i1;
        l10 = i0;
        l5 = i0;
        i0 = l7;
        if (i0) {goto loop_29;}
      block_28:;
      i0 = l11;
      i0 = !(i0);
      p0 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = p0;
      if (i0) {goto block_35;}
      i0 = l9;
      i1 = l11;
      i0 -= i1;
      l8 = i0;
      i1 = kF4082ConstFFFFFFF0;
      i0 = i0 >= i1;
      p0 = i0;
      if (i0) {goto block_5;}
      i0 = l8;
      i1 = 11u;
      i0 = i0 >= i1;
      p0 = i0;
    }
    i0 = p0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = p0;
      i1 = l8;
      i2 = 16u;
      i1 += i2;
      i2 = kF4082ConstFFFFFFF0;
      i1 &= i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      p0 = i0;
      i0 = l12;
      i1 = 1u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = p0;
        i0 = AllocateStringBuffer(ctx, i0);
        l7 = i0;
        i0 = 1u;
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
        i0 = l6;
        i1 = p0;
        i2 = kF4082Const80000000;
        i1 |= i2;
        p0 = i1;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l6;
        i1 = l5;
        StoreU32(ctx, i0, i1);
        i0 = l6;
        i1 = l8;
        StoreU32(ctx, i0 + 4u, i1);
        goto block_37;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = l8;
      StoreU8(ctx, i0 + 11u, i1);
      i0 = l6;
      l5 = i0;
      i0 = l8;
      i0 = !(i0);
      p0 = i0;
      if (i0) {goto block_36;}
    }
    block_37:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i1 = l11;
      i2 = l8;
      i0 = CopyBytes(ctx, i0, i1, i2);
      p0 = i0;
    }
    block_36:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i1 = l8;
      i0 += i1;
      i1 = 0u;
      StoreU8(ctx, i0, i1);
      i0 = p1;
      i0 = LoadU32(ctx, i0 + 4u);
      p0 = i0;
      i0 = kF4082ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = p0;
      i1 = 12u;
      i0 -= i1;
      l8 = i0;
      i0 = l6;
      i0 = LoadU32(ctx, i0);
      i1 = l6;
      i2 = l6;
      i2 = LoadU8(ctx, i2 + 11u);
      l5 = i2;
      i3 = 24u;
      i2 <<= (i3 & 31);
      i3 = 24u;
      i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      p0 = i2;
      i0 = i2 ? i0 : i1;
      l9 = i0;
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 4u);
      i1 = l5;
      i2 = p0;
      i0 = i2 ? i0 : i1;
      p0 = i0;
    }
    i0 = l12;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 860u;
      i1 = l8;
      i2 = l9;
      i3 = p0;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l7 = i0;
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      p0 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF4082ConstD015C;
      i0 = LoadU32(ctx, i0);
      p0 = i0;
      i0 = kF4082ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = p0;
      i1 = 1u;
      i0 = i0 == i1;
      p0 = i0;
      if (i0) {goto block_34;}
      i0 = l6;
      i0 = LoadS8(ctx, i0 + 11u);
      i1 = kF4082ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      p0 = i0;
      if (i0) {goto block_35;}
      i0 = l6;
      i0 = LoadU32(ctx, i0);
      p0 = i0;
      DeallocateRawBlock(ctx, i0);
    }
    block_35:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = 0u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l6;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = p1;
      i0 = LoadU32(ctx, i0 + 4u);
      p0 = i0;
      i1 = p1;
      i1 = LoadU32(ctx, i1 + 8u);
      i0 = i0 >= i1;
      l5 = i0;
      if (i0) {goto block_33;}
      i0 = p0;
      i1 = l6;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = p0;
      i1 = l6;
      i1 = LoadU32(ctx, i1 + 8u);
      l5 = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = p1;
      i1 = p0;
      i2 = 12u;
      i1 += i2;
      p0 = i1;
      StoreU32(ctx, i0 + 4u, i1);
      goto block_32;
    }
    block_34:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      p0 = i0;
      i0 = HostTempRet0(ctx);
      i0 = l6;
      i0 = LoadS8(ctx, i0 + 11u);
      i1 = kF4082ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      if (i0) {goto block_6;}
      goto running_exit;
    }
    block_33:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF4082ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l12;
    i1 = 3u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1412u;
      i1 = p1;
      i2 = l6;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 3u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF4082ConstD015C;
      i0 = LoadU32(ctx, i0);
      p0 = i0;
      i0 = kF4082ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = p0;
      i1 = 1u;
      i0 = i0 == i1;
      p0 = i0;
      if (i0) {goto block_31;}
      i0 = l6;
      i0 = LoadS8(ctx, i0 + 11u);
      i1 = kF4082ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      p0 = i0;
      if (i0) {goto block_32;}
      i0 = l6;
      i0 = LoadU32(ctx, i0);
      p0 = i0;
      DeallocateRawBlock(ctx, i0);
    }
    block_32:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l10;
      i1 = 1u;
      i0 -= i1;
      l9 = i0;
      i0 = 0u;
      l11 = i0;
      goto block_19;
    }
    block_31:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      p0 = i0;
      i0 = HostTempRet0(ctx);
      i0 = l6;
      i0 = LoadS8(ctx, i0 + 11u);
      i1 = kF4082ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      if (i0) {goto block_6;}
      goto running_exit;
    }
    block_26:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l11;
      i1 = l9;
      i2 = l11;
      i0 = i2 ? i0 : i1;
      l11 = i0;
      i0 = p4;
      switch (i0) {
        case 0: goto block_59;
        case 1: goto block_58;
        case 2: goto block_57;
        default: goto block_19;
      }
      block_59:;
      i0 = 1u;
      p0 = i0;
      i0 = l8;
      i1 = 32u;
      i0 ^= i1;
      i1 = 255u;
      i0 &= i1;
      i1 = 161u;
      i0 -= i1;
      i1 = 60u;
      i0 = i0 < i1;
      l5 = i0;
      if (i0) {goto block_56;}
      goto block_19;
      block_58:;
      i0 = 1u;
      p0 = i0;
      i0 = l13;
      i1 = 142u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_56;}
      i0 = l8;
      i1 = 95u;
      i0 += i1;
      i1 = 255u;
      i0 &= i1;
      i1 = 84u;
      i0 = i0 < i1;
      l5 = i0;
      if (i0) {goto block_56;}
      goto block_19;
      block_57:;
      i0 = 128u;
      l5 = i0;
      i0 = kF4082ConstFFFFFFFF;
      p0 = i0;
      i0 = l8;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l8 = i0;
      i1 = 128u;
      i0 &= i1;
      i0 = !(i0);
      l10 = i0;
      if (i0) {goto block_19;}
      loop_60: 
        i0 = p0;
        i1 = 1u;
        i0 += i1;
        p0 = i0;
        i0 = l8;
        i1 = l5;
        i2 = 1u;
        i1 = (uint32_t)((int32_t)i1 >> (i2 & 31));
        l5 = i1;
        i0 &= i1;
        l10 = i0;
        if (i0) {goto loop_60;}
      block_56:;
      i0 = p0;
      i1 = l9;
      i0 += i1;
      l9 = i0;
      goto block_19;
    }
    block_20:;
    i0 = p0;
    i1 = l11;
    i1 = !(i1);
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    p0 = i0;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = p0;
      if (i0) {goto block_62;}
      i0 = l9;
      i1 = l11;
      i0 -= i1;
      l8 = i0;
      i1 = kF4082ConstFFFFFFF0;
      i0 = i0 >= i1;
      p0 = i0;
      if (i0) {goto block_5;}
      i0 = l8;
      i1 = 11u;
      i0 = i0 >= i1;
      p0 = i0;
    }
    i0 = p0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = p0;
      i1 = l8;
      i2 = 16u;
      i1 += i2;
      i2 = kF4082ConstFFFFFFF0;
      i1 &= i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      p0 = i0;
      i0 = l12;
      i1 = 4u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = p0;
        i0 = AllocateStringBuffer(ctx, i0);
        l7 = i0;
        i0 = 4u;
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
        i0 = l6;
        i1 = p0;
        i2 = kF4082Const80000000;
        i1 |= i2;
        p0 = i1;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l6;
        i1 = l5;
        StoreU32(ctx, i0, i1);
        i0 = l6;
        i1 = l8;
        StoreU32(ctx, i0 + 4u, i1);
        goto block_64;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = l8;
      StoreU8(ctx, i0 + 11u, i1);
      i0 = l6;
      l5 = i0;
      i0 = l8;
      i0 = !(i0);
      p0 = i0;
      if (i0) {goto block_63;}
    }
    block_64:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i1 = l11;
      i2 = l8;
      i0 = CopyBytes(ctx, i0, i1, i2);
      p0 = i0;
    }
    block_63:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i1 = l8;
      i0 += i1;
      i1 = 0u;
      StoreU8(ctx, i0, i1);
      i0 = p1;
      i0 = LoadU32(ctx, i0 + 4u);
      p0 = i0;
      i0 = kF4082ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = p0;
      i1 = 12u;
      i0 -= i1;
      l8 = i0;
      i0 = l6;
      i0 = LoadU32(ctx, i0);
      i1 = l6;
      i2 = l6;
      i2 = LoadU8(ctx, i2 + 11u);
      l5 = i2;
      i3 = 24u;
      i2 <<= (i3 & 31);
      i3 = 24u;
      i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      p0 = i2;
      i0 = i2 ? i0 : i1;
      l10 = i0;
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 4u);
      i1 = l5;
      i2 = p0;
      i0 = i2 ? i0 : i1;
      p0 = i0;
    }
    i0 = l12;
    i1 = 5u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 860u;
      i1 = l8;
      i2 = l10;
      i3 = p0;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l7 = i0;
      i0 = 5u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      p0 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF4082ConstD015C;
      i0 = LoadU32(ctx, i0);
      p0 = i0;
      i0 = kF4082ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = p0;
      i1 = 1u;
      i0 = i0 == i1;
      p0 = i0;
      if (i0) {goto block_61;}
      i0 = l6;
      i0 = LoadS8(ctx, i0 + 11u);
      i1 = kF4082ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      p0 = i0;
      if (i0) {goto block_62;}
      i0 = l6;
      i0 = LoadU32(ctx, i0);
      p0 = i0;
      DeallocateRawBlock(ctx, i0);
    }
    block_62:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l9;
      i1 = l9;
      i2 = 1u;
      i1 += i2;
      l5 = i1;
      i2 = l9;
      i2 = LoadU8(ctx, i2 + 1u);
      i3 = 10u;
      i2 = i2 == i3;
      p0 = i2;
      i0 = i2 ? i0 : i1;
      l9 = i0;
      i0 = 0u;
      i1 = l5;
      i2 = p0;
      i0 = i2 ? i0 : i1;
      l11 = i0;
      goto block_19;
    }
    block_61:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      p0 = i0;
      i0 = HostTempRet0(ctx);
      i0 = l6;
      i0 = LoadS8(ctx, i0 + 11u);
      i1 = kF4082ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      if (i0) {goto block_6;}
      goto running_exit;
    }
    block_19:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l9;
      i1 = 1u;
      i0 += i1;
      p0 = i0;
      i0 = l9;
      i0 = LoadU8(ctx, i0 + 1u);
      l8 = i0;
      if (i0) {goto loop_17;}
    }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l11;
    i0 = !(i0);
    p0 = i0;
    if (i0) {goto block_14;}
    i0 = l11;
    i0 = StringLength(ctx, i0);
    p2 = i0;
    i1 = kF4082ConstFFFFFFF0;
    i0 = i0 >= i1;
    p0 = i0;
    if (i0) {goto block_5;}
    i0 = p2;
    i1 = 11u;
    i0 = i0 >= i1;
    p0 = i0;
  }
  i0 = p0;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = p0;
    i1 = p2;
    i2 = 16u;
    i1 += i2;
    i2 = kF4082ConstFFFFFFF0;
    i1 &= i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    p0 = i0;
    i0 = l12;
    i1 = 6u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = p0;
      i0 = AllocateStringBuffer(ctx, i0);
      l7 = i0;
      i0 = 6u;
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
      i0 = l6;
      i1 = p0;
      i2 = kF4082Const80000000;
      i1 |= i2;
      p0 = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l6;
      i1 = l5;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = p2;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l6;
      l8 = i0;
      goto block_80;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i1 = p2;
    StoreU8(ctx, i0 + 11u, i1);
    i0 = l6;
    l8 = i0;
    i0 = l6;
    l5 = i0;
    i0 = p2;
    i0 = !(i0);
    p0 = i0;
    if (i0) {goto block_79;}
  }
  block_80:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = l11;
    i2 = p2;
    i0 = CopyBytes(ctx, i0, i1, i2);
    p0 = i0;
  }
  block_79:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = p2;
    i1 = l5;
    i0 += i1;
    i1 = 0u;
    StoreU8(ctx, i0, i1);
    i0 = p1;
    i0 = LoadU32(ctx, i0 + 4u);
    p0 = i0;
    i0 = l8;
    i0 = LoadU8(ctx, i0 + 11u);
    p1 = i0;
    i0 = kF4082ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = p0;
    i1 = 12u;
    i0 -= i1;
    p2 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0);
    i1 = l6;
    i2 = p1;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    p0 = i2;
    i0 = i2 ? i0 : i1;
    p3 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 4u);
    i1 = p1;
    i2 = p0;
    i0 = i2 ? i0 : i1;
    p0 = i0;
  }
  i0 = l12;
  i1 = 7u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 860u;
    i1 = p2;
    i2 = p3;
    i3 = p0;
    i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
    l7 = i0;
    i0 = 7u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    p0 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF4082ConstD015C;
    i0 = LoadU32(ctx, i0);
    p0 = i0;
    i0 = kF4082ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = p0;
    i1 = 1u;
    i0 = i0 == i1;
    p0 = i0;
    if (i0) {goto block_13;}
    i0 = l8;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF4082ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    p0 = i0;
    if (i0) {goto block_14;}
    i0 = l6;
    i0 = LoadU32(ctx, i0);
    p0 = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_14:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i1 = 16u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_13:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    p0 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l8;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF4082ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_6;}
    i0 = l6;
    i0 = LoadU32(ctx, i0);
    DeallocateRawBlock(ctx, i0);
    goto block_6;
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    p0 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l6;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF4082ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_6;}
    goto running_exit;
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = p0;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_5:;
  i0 = l12;
  i1 = 8u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    RaiseStringLengthError(ctx, i0);
    i0 = 8u;
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
    i0 = l6;
    i0 = LoadU32(ctx, i0);
    DeallocateRawBlock(ctx, i0);
    i0 = p0;
    HostOsRelease(ctx, i0);
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
  i2 = kF4082ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l7 = i0;
  i1 = p0;
  StoreU32(ctx, i0, i1);
  i0 = l7;
  i1 = p1;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l7;
  i1 = p2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l7;
  i1 = p3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l7;
  i1 = p4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l7;
  i1 = l6;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l7;
  i1 = l5;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l7;
  i1 = l8;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l7;
  i1 = l9;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = l7;
  i1 = l10;
  StoreU32(ctx, i0 + 36u, i1);
  i0 = l7;
  i1 = l11;
  StoreU32(ctx, i0 + 40u, i1);
  i0 = l7;
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
