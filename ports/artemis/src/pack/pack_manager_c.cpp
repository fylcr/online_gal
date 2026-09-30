// pack_manager_c.cpp -- the third file and index tag body: the reservedCommands side of the file API..
//
// PackManager::FileDispatchC -- the third file and index tag body: the reservedCommands side of the file API.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 1374213-1374213 (f3087); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void Panic(EngineContext* ctx, uint32_t a0);
void ResizeValueStack(EngineContext* ctx, uint32_t a0, uint32_t a1);
void SetStackTop(EngineContext* ctx, uint32_t stack, uint32_t index);
void AppendTableSlot(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

namespace {

constexpr uint32_t kF3087MsgFile = 0xBAE4Cu;  // file
constexpr uint32_t kF3087MsgIndex = 0xBB108u;  // index
constexpr uint32_t kF3087MsgReservedcommands = 0xBB32Eu;  // reservedCommands
constexpr uint32_t kF3087ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF3087ConstFFFFD8F0 = 0xFFFFD8F0u;  // 
constexpr uint32_t kF3087ConstFFFFFFC0 = 0xFFFFFFC0u;  // 
constexpr uint32_t kF3087ConstFFFFFFFE = 0xFFFFFFFEu;  // 
constexpr uint32_t kF3087ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF3087SaveFrameBytes = 48u;
constexpr uint32_t kF3087ResumeWordBytes = 4u;

}  // namespace

namespace PackManager {
void FileDispatchC(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l13 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  double d1 = 0;
      l10 = 0, l11 = 0, l12 = 0, l13 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF3087SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l6 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
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
    l5 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 24u);
    l7 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 28u);
    l8 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 32u);
    l9 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 36u);
    l11 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 40u);
    l12 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 44u);
    l13 = i0;
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
    i2 = kF3087ResumeWordBytes;
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
    i1 = kF3087ConstFFFFFFC0;
    i0 += i1;
    l4 = i0;
    GuestStackSp(ctx) = i0;
    i0 = b;
    i0 = LoadU32(ctx, i0 + 16u);
    l2 = i0;
  }
  i0 = l10;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = 0u;
    i2 = 0u;
    AppendTableSlot(ctx, i0, i1, i2);
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = kF3087ConstFFFFFFFE;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = a;
    i1 = l2;
    StoreU32(ctx, i0, i1);
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = kF3087ConstFFFFFFFF;
    SetStackTop(ctx, i0, i1);
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF3087ConstD015C;
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
    if (i0) {goto block_8;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l10;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1001u;
    i1 = l2;
    i2 = kF3087ConstFFFFD8F0;
    i0 = HostOsQuery(ctx, i0, i1, i2);
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
  i0 = !(i0);
  if (i0) {
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    l8 = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l8;
    i1 = 1u;
    i0 = i0 == i1;
    l5 = i0;
    if (i0) {goto block_8;}
    i0 = a;
    i1 = l3;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = 0u;
    l8 = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = kF3087ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 != i1;
    l3 = i0;
    if (i0) {goto block_7;}
    goto block_6;
  }
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = kF3087ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_6;}
    i0 = a;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i0 = LoadU32(ctx, i0 + 20u);
    l3 = i0;
    i1 = 2888u;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i1 = l3;
    i2 = 2900u;
    i1 += i2;
    i1 = LoadU32(ctx, i1);
    l7 = i1;
    i2 = 30u;
    i1 = DIV_U(i1, i2);
    l12 = i1;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    l13 = i0;
    i0 = l3;
    i1 = 2892u;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l9 = i0;
    i1 = l2;
    i0 = i0 != i1;
    l5 = i0;
    if (i0) {
      i0 = l13;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i1 = l7;
      i2 = l12;
      i3 = 30u;
      i2 *= i3;
      i1 -= i2;
      i2 = 136u;
      i1 *= i2;
      i0 += i1;
      l8 = i0;
    }
    i0 = 1u;
    l12 = i0;
  }
  loop_46: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i1 = l9;
      i0 = i0 != i1;
      l5 = i0;
      if (i0) {
        i0 = l7;
        i1 = l3;
        i2 = 2904u;
        i1 += i2;
        i1 = LoadU32(ctx, i1);
        i0 += i1;
        l3 = i0;
        i1 = 30u;
        i0 = DIV_U(i0, i1);
        l9 = i0;
        i1 = 2u;
        i0 <<= (i1 & 31);
        i1 = l2;
        i0 += i1;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i0 = l5;
        i1 = l3;
        i2 = l9;
        i3 = 30u;
        i2 *= i3;
        l2 = i2;
        i1 -= i2;
        i2 = 136u;
        i1 *= i2;
        i0 += i1;
      } else {
        i0 = 0u;
      }
      i1 = l8;
      i0 = i0 == i1;
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
        i0 = b;
        i0 = LoadU32(ctx, i0 + 16u);
        l3 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l10;
      i1 = 2u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1007u;
        i1 = l3;
        i2 = 0u;
        i3 = 0u;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 2u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_37;}
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = l3;
        StoreU32(ctx, i0 + 32u, i1);
        i0 = l3;
        i1 = kF3087ConstFFFFFFFF;
        SetStackTop(ctx, i0, i1);
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        l5 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        if (i0) {goto block_54;}
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l10;
      i1 = 3u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1001u;
        i1 = l3;
        i2 = kF3087ConstFFFFD8F0;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l6 = i0;
        i0 = 3u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l6;
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l8 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l8;
        i1 = 1u;
        i0 = i0 == i1;
        l5 = i0;
        if (i0) {goto block_54;}
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = l2;
        StoreU32(ctx, i0 + 36u, i1);
        i0 = l3;
        i1 = kF3087ConstFFFFFFFE;
        ResizeValueStack(ctx, i0, i1);
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 != i1;
        l3 = i0;
        if (i0) {goto block_38;}
        goto block_6;
      }
      block_54:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        l2 = i0;
        i0 = HostTempRet0(ctx);
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = kF3087ConstFFFFFFFE;
        ResizeValueStack(ctx, i0, i1);
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 != i1;
        b = i0;
        if (i0) {goto block_15;}
        goto block_6;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0 + 16u);
      l3 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l10;
    i1 = 4u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1007u;
      i1 = l3;
      i2 = 0u;
      i3 = 0u;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 4u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_19;}
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      StoreU32(ctx, i0 + 32u, i1);
      i0 = l3;
      i1 = kF3087ConstFFFFFFFF;
      SetStackTop(ctx, i0, i1);
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      if (i0) {goto block_63;}
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l10;
    i1 = 5u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1001u;
      i1 = l3;
      i2 = kF3087ConstFFFFD8F0;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l6 = i0;
      i0 = 5u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l6;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l9 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l9;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_63;}
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l2;
      StoreU32(ctx, i0 + 36u, i1);
      i0 = l3;
      i1 = kF3087ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l3 = i0;
      if (i0) {goto block_62;}
      goto block_6;
    }
    block_63:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      l2 = i0;
      i0 = HostTempRet0(ctx);
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = kF3087ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_15;}
      goto block_6;
    }
    block_62:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l2;
      StoreU32(ctx, i0 + 48u, i1);
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 12u);
      i0 -= i1;
      i1 = 4u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l3 = i0;
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l9 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l9;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_18;}
      i0 = l4;
      i1 = a;
      StoreU32(ctx, i0 + 56u, i1);
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 52u, i1);
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 8u);
      l3 = i0;
      i1 = 3u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = l12;
      d1 = (double)(int32_t)(i1);
      StoreF64(ctx, i0, d1);
      i0 = l2;
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 8u);
      i2 = 16u;
      i1 += i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_18;}
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 32u;
      i0 += i1;
      l5 = i0;
      i0 = l4;
      i1 = 48u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l10;
    i1 = 6u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1025u;
      i1 = l3;
      i2 = l5;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l6 = i0;
      i0 = 6u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l6;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 48u);
      l2 = i0;
      if (i0) {
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = kF3087ConstFFFFFFFE;
        ResizeValueStack(ctx, i0, i1);
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_6;}
      }
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 32u);
      l2 = i0;
      i0 = !(i0);
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      if (i0) {goto block_72;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 36u);
      l3 = i0;
      i1 = kF3087ConstFFFFFFFE;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_72;}
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l10;
    i1 = 7u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1002u;
      i1 = l2;
      i2 = kF3087ConstFFFFD8F0;
      i3 = l3;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 7u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_6;}
    }
    block_72:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l2;
      StoreU32(ctx, i0 + 32u, i1);
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 12u);
      i0 -= i1;
      i1 = 4u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l3 = i0;
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l9 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l9;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_23;}
      i0 = l4;
      i1 = a;
      StoreU32(ctx, i0 + 40u, i1);
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 36u, i1);
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 8u);
      l3 = i0;
      i1 = 3u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = l12;
      d1 = (double)(int32_t)(i1);
      StoreF64(ctx, i0, d1);
      i0 = l2;
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 8u);
      i2 = 16u;
      i1 += i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_23;}
      i0 = l4;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 32u);
      l2 = i1;
      StoreU32(ctx, i0 + 48u, i1);
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 12u);
      i0 -= i1;
      i1 = 4u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l3 = i0;
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l9 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l9;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_22;}
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 52u, i1);
      i0 = l4;
      i1 = l4;
      i2 = 32u;
      i1 += i2;
      l3 = i1;
      StoreU32(ctx, i0 + 56u, i1);
    }
    i0 = l10;
    i1 = 8u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 969u;
      i1 = l2;
      i2 = kF3087MsgFile;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 8u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_22;}
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l8;
      i1 = 4u;
      i0 += i1;
      l5 = i0;
      i0 = l4;
      i1 = 48u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l10;
    i1 = 9u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3393u;
      i1 = l3;
      i2 = l5;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l6 = i0;
      i0 = 9u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l6;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_21;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 48u);
      l2 = i0;
      if (i0) {
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = kF3087ConstFFFFFFFE;
        ResizeValueStack(ctx, i0, i1);
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_6;}
      }
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 32u);
      l2 = i0;
      if (i0) {
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = kF3087ConstFFFFFFFE;
        ResizeValueStack(ctx, i0, i1);
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_6;}
      }
      i0 = a;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l2;
      StoreU32(ctx, i0 + 32u, i1);
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 12u);
      i0 -= i1;
      i1 = 4u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l3 = i0;
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l9 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l9;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_27;}
      i0 = l4;
      i1 = a;
      StoreU32(ctx, i0 + 40u, i1);
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 36u, i1);
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 8u);
      l3 = i0;
      i1 = 3u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = l12;
      d1 = (double)(int32_t)(i1);
      StoreF64(ctx, i0, d1);
      i0 = l2;
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 8u);
      i2 = 16u;
      i1 += i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_27;}
      i0 = l4;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 32u);
      l2 = i1;
      StoreU32(ctx, i0 + 48u, i1);
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 12u);
      i0 -= i1;
      i1 = 4u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l3 = i0;
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l9 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l9;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_26;}
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 52u, i1);
      i0 = l4;
      i1 = l4;
      i2 = 32u;
      i1 += i2;
      l3 = i1;
      StoreU32(ctx, i0 + 56u, i1);
    }
    i0 = l10;
    i1 = 10u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 969u;
      i1 = l2;
      i2 = kF3087MsgIndex;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 10u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_26;}
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l8;
      i1 = 16u;
      i0 += i1;
      l5 = i0;
      i0 = l4;
      i1 = 48u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l10;
    i1 = 11u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3394u;
      i1 = l3;
      i2 = l5;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l6 = i0;
      i0 = 11u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l6;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_25;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 48u);
      l2 = i0;
      if (i0) {
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = kF3087ConstFFFFFFFE;
        ResizeValueStack(ctx, i0, i1);
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_6;}
      }
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 32u);
      l2 = i0;
      if (i0) {
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = kF3087ConstFFFFFFFE;
        ResizeValueStack(ctx, i0, i1);
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_6;}
      }
      i0 = b;
      i0 = LoadU32(ctx, i0 + 16u);
      l3 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l10;
    i1 = 12u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1007u;
      i1 = l3;
      i2 = 0u;
      i3 = 0u;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 12u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_33;}
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = kF3087ConstFFFFFFFF;
      SetStackTop(ctx, i0, i1);
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      if (i0) {goto block_92;}
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l10;
    i1 = 13u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1001u;
      i1 = l3;
      i2 = kF3087ConstFFFFD8F0;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l6 = i0;
      i0 = 13u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l6;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l9 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l9;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_92;}
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l2;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l3;
      i1 = kF3087ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l3 = i0;
      if (i0) {goto block_91;}
      goto block_6;
    }
    block_92:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      l2 = i0;
      i0 = HostTempRet0(ctx);
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = kF3087ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_15;}
      goto block_6;
    }
    block_91:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l2;
      StoreU32(ctx, i0 + 32u, i1);
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 12u);
      i0 -= i1;
      i1 = 4u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l3 = i0;
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l9 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l9;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_32;}
      i0 = l4;
      i1 = a;
      StoreU32(ctx, i0 + 40u, i1);
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 36u, i1);
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 8u);
      l3 = i0;
      i1 = 3u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = l12;
      d1 = (double)(int32_t)(i1);
      StoreF64(ctx, i0, d1);
      i0 = l2;
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 8u);
      i2 = 16u;
      i1 += i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_32;}
      i0 = l4;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 32u);
      l2 = i1;
      StoreU32(ctx, i0 + 48u, i1);
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 12u);
      i0 -= i1;
      i1 = 4u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l3 = i0;
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l9 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l9;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_31;}
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 52u, i1);
      i0 = l4;
      i1 = l4;
      i2 = 32u;
      i1 += i2;
      l3 = i1;
      StoreU32(ctx, i0 + 56u, i1);
    }
    i0 = l10;
    i1 = 14u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 969u;
      i1 = l2;
      i2 = kF3087MsgReservedcommands;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 14u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_31;}
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 8u;
      i0 += i1;
      l5 = i0;
      i0 = l4;
      i1 = 48u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l10;
    i1 = 15u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3395u;
      i1 = l3;
      i2 = l5;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l6 = i0;
      i0 = 15u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l6;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_30;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 48u);
      l2 = i0;
      if (i0) {
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = kF3087ConstFFFFFFFE;
        ResizeValueStack(ctx, i0, i1);
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_6;}
      }
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 32u);
      l2 = i0;
      if (i0) {
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = kF3087ConstFFFFFFFE;
        ResizeValueStack(ctx, i0, i1);
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_6;}
      }
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 8u);
      l2 = i0;
      i0 = !(i0);
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      if (i0) {goto block_104;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 12u);
      l3 = i0;
      i1 = kF3087ConstFFFFFFFE;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_104;}
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l10;
    i1 = 16u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1002u;
      i1 = l2;
      i2 = kF3087ConstFFFFD8F0;
      i3 = l3;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 16u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_6;}
    }
    block_104:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l8;
      i1 = kF3087ConstFFFFFFC0;
      i0 -= i1;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i1 = l8;
      i1 = LoadU32(ctx, i1 + 76u);
      l11 = i1;
      i2 = 93u;
      i1 = DIV_U(i1, i2);
      l3 = i1;
      i2 = 2u;
      i1 <<= (i2 & 31);
      i0 += i1;
      l9 = i0;
      i0 = 0u;
      l5 = i0;
      i0 = l2;
      i1 = l8;
      i1 = LoadU32(ctx, i1 + 68u);
      l7 = i1;
      i0 = i0 != i1;
      if (i0) {
        i0 = l9;
        i0 = LoadU32(ctx, i0);
        i1 = l11;
        i2 = l3;
        i3 = 93u;
        i2 *= i3;
        i1 -= i2;
        i2 = 44u;
        i1 *= i2;
        i0 += i1;
      } else {
        i0 = l5;
      }
      l3 = i0;
      i0 = 1u;
      l5 = i0;
    }
    loop_110: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i1 = l7;
        i0 = i0 == i1;
        l7 = i0;
        if (i0) {
          i0 = 0u;
        } else {
          i0 = l11;
          i1 = l8;
          i1 = LoadU32(ctx, i1 + 80u);
          i0 += i1;
          l7 = i0;
          i1 = 93u;
          i0 = DIV_U(i0, i1);
          l11 = i0;
          i1 = 2u;
          i0 <<= (i1 & 31);
          i1 = l2;
          i0 += i1;
          i0 = LoadU32(ctx, i0);
          i1 = l7;
          i2 = l11;
          i3 = 93u;
          i2 *= i3;
          l11 = i2;
          i1 -= i2;
          i2 = 44u;
          i1 *= i2;
          l7 = i1;
          i0 += i1;
        }
        i1 = l3;
        i0 = i0 == i1;
        if (i0) {
          i0 = l8;
          i1 = 136u;
          i0 += i1;
          l8 = i0;
          i1 = l13;
          i1 = LoadU32(ctx, i1);
          i0 -= i1;
          i1 = 4080u;
          i0 = i0 == i1;
          if (i0) {
            i0 = l13;
            i0 = LoadU32(ctx, i0 + 4u);
            l8 = i0;
            i0 = l13;
            i1 = 4u;
            i0 += i1;
            l13 = i0;
          }
          i0 = l12;
          i1 = 1u;
          i0 += i1;
          l12 = i0;
          i0 = b;
          i0 = LoadU32(ctx, i0 + 20u);
          l3 = i0;
          i1 = 2892u;
          i0 += i1;
          i0 = LoadU32(ctx, i0);
          l9 = i0;
          i0 = l3;
          i1 = 2888u;
          i0 += i1;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = l3;
          i1 = 2900u;
          i0 += i1;
          l5 = i0;
          i0 = LoadU32(ctx, i0);
          l7 = i0;
          goto loop_46;
        }
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 24u;
        i0 += i1;
        l2 = i0;
      }
      i0 = l10;
      i1 = 17u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 3396u;
        i1 = l2;
        i2 = b;
        i3 = l3;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 17u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        l2 = i0;
        if (i0) {goto block_39;}
        i0 = a;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = l2;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 8u);
        i1 = l2;
        i1 = LoadU32(ctx, i1 + 12u);
        i0 -= i1;
        i1 = 4u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        l7 = i0;
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l11 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l11;
        i1 = 1u;
        i0 = i0 == i1;
        l11 = i0;
        if (i0) {goto block_41;}
        i0 = l4;
        i1 = a;
        StoreU32(ctx, i0 + 16u, i1);
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = l7;
        i2 = 1u;
        i1 += i2;
        StoreU32(ctx, i0 + 12u, i1);
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 8u);
        l7 = i0;
        i1 = 3u;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l7;
        i1 = l12;
        d1 = (double)(int32_t)(i1);
        StoreF64(ctx, i0, d1);
        i0 = l2;
        i1 = l2;
        i1 = LoadU32(ctx, i1 + 8u);
        i2 = 16u;
        i1 += i2;
        l7 = i1;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        l2 = i0;
        if (i0) {goto block_41;}
        i0 = l4;
        i1 = l4;
        i1 = LoadU32(ctx, i1 + 8u);
        l2 = i1;
        StoreU32(ctx, i0 + 32u, i1);
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 8u);
        i1 = l2;
        i1 = LoadU32(ctx, i1 + 12u);
        i0 -= i1;
        i1 = 4u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        l7 = i0;
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l11 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l11;
        i1 = 1u;
        i0 = i0 == i1;
        l11 = i0;
        if (i0) {goto block_43;}
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = l7;
        i2 = 1u;
        i1 += i2;
        StoreU32(ctx, i0 + 36u, i1);
        i0 = l4;
        i1 = l4;
        i2 = 8u;
        i1 += i2;
        l7 = i1;
        StoreU32(ctx, i0 + 40u, i1);
      }
      i0 = l10;
      i1 = 18u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 969u;
        i1 = l2;
        i2 = kF3087MsgReservedcommands;
        HostOsSchedule(ctx, i0, i1, i2);
        i0 = 18u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        l2 = i0;
        if (i0) {goto block_43;}
        i0 = l4;
        i1 = l4;
        i1 = LoadU32(ctx, i1 + 32u);
        l2 = i1;
        StoreU32(ctx, i0 + 48u, i1);
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 8u);
        i1 = l2;
        i1 = LoadU32(ctx, i1 + 12u);
        i0 -= i1;
        i1 = 4u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        l7 = i0;
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l11 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l11;
        i1 = 1u;
        i0 = i0 == i1;
        l11 = i0;
        if (i0) {goto block_45;}
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = l7;
        i2 = 1u;
        i1 += i2;
        StoreU32(ctx, i0 + 52u, i1);
        i0 = l4;
        i1 = l4;
        i2 = 32u;
        i1 += i2;
        StoreU32(ctx, i0 + 56u, i1);
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 8u);
        l7 = i0;
        i1 = 3u;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l7;
        i1 = l5;
        d1 = (double)(int32_t)(i1);
        StoreF64(ctx, i0, d1);
        i0 = l2;
        i1 = l2;
        i1 = LoadU32(ctx, i1 + 8u);
        i2 = 16u;
        i1 += i2;
        l7 = i1;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        l2 = i0;
        if (i0) {goto block_45;}
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 24u;
        i0 += i1;
        l7 = i0;
        i0 = l4;
        i1 = 48u;
        i0 += i1;
        l2 = i0;
      }
      i0 = l10;
      i1 = 19u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 3397u;
        i1 = l2;
        i2 = l7;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l6 = i0;
        i0 = 19u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l6;
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 != i1;
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
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 48u);
          l2 = i0;
          if (i0) {
            i0 = kF3087ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = kF3087ConstFFFFFFFE;
            ResizeValueStack(ctx, i0, i1);
            i0 = kF3087ConstD015C;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = kF3087ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 == i1;
            if (i0) {goto block_6;}
          }
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 32u);
          l2 = i0;
          if (i0) {
            i0 = kF3087ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = kF3087ConstFFFFFFFE;
            ResizeValueStack(ctx, i0, i1);
            i0 = kF3087ConstD015C;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = kF3087ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 == i1;
            if (i0) {goto block_6;}
          }
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 8u);
          l2 = i0;
          if (i0) {
            i0 = kF3087ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = kF3087ConstFFFFFFFE;
            ResizeValueStack(ctx, i0, i1);
            i0 = kF3087ConstD015C;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = kF3087ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 == i1;
            if (i0) {goto block_6;}
          }
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 24u);
          l2 = i0;
          i0 = !(i0);
          l7 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l7;
          if (i0) {goto block_127;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 28u);
          l7 = i0;
          i1 = kF3087ConstFFFFFFFE;
          i0 = i0 == i1;
          l11 = i0;
          if (i0) {goto block_127;}
          i0 = kF3087ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l10;
        i1 = 20u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1002u;
          i1 = l2;
          i2 = kF3087ConstFFFFD8F0;
          i3 = l7;
          HostSyncSlot(ctx, i0, i1, i2, i3);
          i0 = 20u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3087ConstD015C;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = kF3087ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 == i1;
          l2 = i0;
          if (i0) {goto block_6;}
        }
        block_127:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i1 = 44u;
          i0 += i1;
          l3 = i0;
          i1 = l9;
          i1 = LoadU32(ctx, i1);
          i0 -= i1;
          i1 = 4092u;
          i0 = i0 == i1;
          if (i0) {
            i0 = l9;
            i0 = LoadU32(ctx, i0 + 4u);
            l3 = i0;
            i0 = l9;
            i1 = kF3087ResumeWordBytes;
            i0 += i1;
            l9 = i0;
          }
          i0 = l5;
          i1 = 1u;
          i0 += i1;
          l5 = i0;
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 68u);
          l7 = i0;
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 64u);
          l2 = i0;
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 76u);
          l11 = i0;
          goto loop_110;
        }
      }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 48u);
    b = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_44;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF3087ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_44;}
    goto block_6;
  }
  block_45:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
  }
  block_44:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 32u);
    b = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_42;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF3087ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_42;}
    goto block_6;
  }
  block_43:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
  }
  block_42:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_40;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF3087ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_40;}
    goto block_6;
  }
  block_41:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
  }
  block_40:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 24u);
    b = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_15;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 28u);
    l4 = i0;
    i1 = kF3087ConstFFFFFFFE;
    i0 = i0 == i1;
    l3 = i0;
    if (i0) {goto block_15;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l10;
  i1 = 21u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1002u;
    i1 = b;
    i2 = kF3087ConstFFFFD8F0;
    i3 = l4;
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
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_15;}
    goto block_6;
  }
  block_39:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_15;
  }
  block_38:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = l2;
    StoreU32(ctx, i0 + 48u, i1);
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 12u);
    i0 -= i1;
    i1 = 4u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    l3 = i0;
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    l8 = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l8;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_36;}
    i0 = l4;
    i1 = a;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = l3;
    i2 = 1u;
    i1 += i2;
    StoreU32(ctx, i0 + 52u, i1);
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i1 = 3u;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = l12;
    d1 = (double)(int32_t)(i1);
    StoreF64(ctx, i0, d1);
    i0 = l2;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 8u);
    i2 = 16u;
    i1 += i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l3 = i0;
    if (i0) {goto block_36;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 32u;
    i0 += i1;
    l5 = i0;
    i0 = l4;
    i1 = 48u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l10;
  i1 = 22u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1025u;
    i1 = l3;
    i2 = l5;
    i0 = HostOsQuery(ctx, i0, i1, i2);
    l6 = i0;
    i0 = 22u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l6;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l3 = i0;
    if (i0) {goto block_35;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 48u);
    l2 = i0;
    if (i0) {
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3087ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_6;}
    }
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 32u);
    l2 = i0;
    i0 = !(i0);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    if (i0) {goto block_147;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 36u);
    l3 = i0;
    i1 = kF3087ConstFFFFFFFE;
    i0 = i0 == i1;
    l5 = i0;
    if (i0) {goto block_147;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l10;
  i1 = 23u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1002u;
    i1 = l2;
    i2 = kF3087ConstFFFFD8F0;
    i3 = l3;
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
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l3 = i0;
    if (i0) {goto block_6;}
  }
  block_147:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i0 = LoadU32(ctx, i0 + 20u);
    l3 = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = l2;
    StoreU32(ctx, i0 + 32u, i1);
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 12u);
    i0 -= i1;
    i1 = 4u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    l8 = i0;
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    l9 = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l9;
    i1 = 1u;
    i0 = i0 == i1;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    if (i0) {goto block_155;}
    i0 = l4;
    i1 = a;
    StoreU32(ctx, i0 + 40u, i1);
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = l8;
    i2 = 1u;
    i1 += i2;
    StoreU32(ctx, i0 + 36u, i1);
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 8u);
    l8 = i0;
    i1 = 3u;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l8;
    i1 = l12;
    d1 = (double)(int32_t)(i1);
    StoreF64(ctx, i0, d1);
    i0 = l2;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 8u);
    i2 = 16u;
    i1 += i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_155;}
    i0 = l4;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 32u);
    l2 = i1;
    StoreU32(ctx, i0 + 48u, i1);
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 12u);
    i0 -= i1;
    i1 = 4u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    l8 = i0;
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    l9 = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l9;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_154;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = l8;
    i2 = 1u;
    i1 += i2;
    StoreU32(ctx, i0 + 52u, i1);
    i0 = l4;
    i1 = l4;
    i2 = 32u;
    i1 += i2;
    l5 = i1;
    StoreU32(ctx, i0 + 56u, i1);
  }
  i0 = l10;
  i1 = 24u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 969u;
    i1 = l2;
    i2 = kF3087MsgFile;
    HostOsSchedule(ctx, i0, i1, i2);
    i0 = 24u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_154;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 48u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = 2752u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l10;
  i1 = 25u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 3393u;
    i1 = l5;
    i2 = l3;
    i0 = HostOsQuery(ctx, i0, i1, i2);
    l6 = i0;
    i0 = 25u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l6;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l3 = i0;
    if (i0) {goto block_153;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 48u);
    l2 = i0;
    if (i0) {
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3087ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_6;}
    }
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 32u);
    l2 = i0;
    if (i0) {
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3087ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_6;}
    }
    i0 = b;
    i0 = LoadU32(ctx, i0 + 20u);
    l3 = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = l2;
    StoreU32(ctx, i0 + 32u, i1);
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 12u);
    i0 -= i1;
    i1 = 4u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    l8 = i0;
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    l9 = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l9;
    i1 = 1u;
    i0 = i0 == i1;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    if (i0) {goto block_166;}
    i0 = l4;
    i1 = a;
    StoreU32(ctx, i0 + 40u, i1);
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = l8;
    i2 = 1u;
    i1 += i2;
    StoreU32(ctx, i0 + 36u, i1);
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 8u);
    l8 = i0;
    i1 = 3u;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l8;
    i1 = l12;
    d1 = (double)(int32_t)(i1);
    StoreF64(ctx, i0, d1);
    i0 = l2;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 8u);
    i2 = 16u;
    i1 += i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_166;}
    i0 = l4;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 32u);
    l2 = i1;
    StoreU32(ctx, i0 + 48u, i1);
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 12u);
    i0 -= i1;
    i1 = 4u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    l8 = i0;
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    l9 = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l9;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_165;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = l8;
    i2 = 1u;
    i1 += i2;
    StoreU32(ctx, i0 + 52u, i1);
    i0 = l4;
    i1 = l4;
    i2 = 32u;
    i1 += i2;
    l5 = i1;
    StoreU32(ctx, i0 + 56u, i1);
  }
  i0 = l10;
  i1 = 26u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 969u;
    i1 = l2;
    i2 = kF3087MsgIndex;
    HostOsSchedule(ctx, i0, i1, i2);
    i0 = 26u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_165;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 48u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = 2764u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l10;
  i1 = 27u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 3394u;
    i1 = l5;
    i2 = l3;
    i0 = HostOsQuery(ctx, i0, i1, i2);
    l6 = i0;
    i0 = 27u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l6;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l3 = i0;
    if (i0) {goto block_164;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 48u);
    l2 = i0;
    if (i0) {
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3087ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_6;}
    }
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 32u);
    l2 = i0;
    if (i0) {
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3087ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_6;}
    }
    i0 = b;
    i0 = LoadU32(ctx, i0 + 16u);
    l3 = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l10;
  i1 = 28u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1007u;
    i1 = l3;
    i2 = 0u;
    i3 = 0u;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 28u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
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
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = kF3087ConstFFFFFFFF;
      SetStackTop(ctx, i0, i1);
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      if (i0) {goto block_185;}
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l10;
    i1 = 29u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1001u;
      i1 = l3;
      i2 = kF3087ConstFFFFD8F0;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l6 = i0;
      i0 = 29u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l6;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l8 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l8;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_185;}
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l2;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l3;
      i1 = kF3087ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l3 = i0;
      if (i0) {goto block_184;}
      goto block_6;
    }
    block_185:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      l2 = i0;
      i0 = HostTempRet0(ctx);
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = kF3087ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_15;}
      goto block_6;
    }
    block_184:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l2;
      StoreU32(ctx, i0 + 32u, i1);
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 12u);
      i0 -= i1;
      i1 = 4u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l3 = i0;
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l8 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l8;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_179;}
      i0 = l4;
      i1 = a;
      StoreU32(ctx, i0 + 40u, i1);
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 36u, i1);
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 8u);
      l3 = i0;
      i1 = 3u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = l12;
      d1 = (double)(int32_t)(i1);
      StoreF64(ctx, i0, d1);
      i0 = l2;
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 8u);
      i2 = 16u;
      i1 += i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_179;}
      i0 = l4;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 32u);
      l2 = i1;
      StoreU32(ctx, i0 + 48u, i1);
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 12u);
      i0 -= i1;
      i1 = 4u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l3 = i0;
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l8 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l8;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_178;}
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 52u, i1);
      i0 = l4;
      i1 = l4;
      i2 = 32u;
      i1 += i2;
      l3 = i1;
      StoreU32(ctx, i0 + 56u, i1);
    }
    i0 = l10;
    i1 = 30u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 969u;
      i1 = l2;
      i2 = kF3087MsgReservedcommands;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 30u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_178;}
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 8u;
      i0 += i1;
      l5 = i0;
      i0 = l4;
      i1 = 48u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l10;
    i1 = 31u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3395u;
      i1 = l3;
      i2 = l5;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l6 = i0;
      i0 = 31u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l6;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_177;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 48u);
      l2 = i0;
      if (i0) {
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = kF3087ConstFFFFFFFE;
        ResizeValueStack(ctx, i0, i1);
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_6;}
      }
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 32u);
      l2 = i0;
      if (i0) {
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = kF3087ConstFFFFFFFE;
        ResizeValueStack(ctx, i0, i1);
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_6;}
      }
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 8u);
      l2 = i0;
      i0 = !(i0);
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      if (i0) {goto block_197;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 12u);
      l3 = i0;
      i1 = kF3087ConstFFFFFFFE;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_197;}
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l10;
    i1 = 32u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1002u;
      i1 = l2;
      i2 = kF3087ConstFFFFD8F0;
      i3 = l3;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 32u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_6;}
    }
    block_197:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0 + 20u);
      l2 = i0;
      i1 = 2812u;
      i0 += i1;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = l5;
      i1 = l2;
      i2 = 2824u;
      i1 += i2;
      i1 = LoadU32(ctx, i1);
      l11 = i1;
      i2 = 93u;
      i1 = DIV_U(i1, i2);
      l8 = i1;
      i2 = 2u;
      i1 <<= (i2 & 31);
      i0 += i1;
      l9 = i0;
      i0 = 0u;
      l3 = i0;
      i0 = l2;
      i1 = 2816u;
      i0 += i1;
      i0 = LoadU32(ctx, i0);
      l7 = i0;
      i1 = l5;
      i0 = i0 != i1;
      if (i0) {
        i0 = l9;
        i0 = LoadU32(ctx, i0);
        i1 = l11;
        i2 = l8;
        i3 = 93u;
        i2 *= i3;
        i1 -= i2;
        i2 = 44u;
        i1 *= i2;
        i0 += i1;
        l3 = i0;
      }
      i0 = 0u;
      l8 = i0;
    }
    loop_210: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        i1 = l7;
        i0 = i0 == i1;
        l7 = i0;
        if (i0) {
          i0 = 0u;
        } else {
          i0 = l11;
          i1 = l2;
          i2 = 2828u;
          i1 += i2;
          i1 = LoadU32(ctx, i1);
          i0 += i1;
          l2 = i0;
          i1 = 93u;
          i0 = DIV_U(i0, i1);
          l7 = i0;
          i1 = 2u;
          i0 <<= (i1 & 31);
          i1 = l5;
          i0 += i1;
          i0 = LoadU32(ctx, i0);
          l5 = i0;
          i0 = l5;
          i1 = l2;
          i2 = l7;
          i3 = 93u;
          i2 *= i3;
          l7 = i2;
          i1 -= i2;
          i2 = 44u;
          i1 *= i2;
          l2 = i1;
          i0 += i1;
        }
        i1 = l3;
        i0 = i0 == i1;
        if (i0) {
          i0 = l4;
          i1 = kF3087ConstFFFFFFC0;
          i0 -= i1;
          GuestStackSp(ctx) = i0;
          goto func_exit;
        }
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 24u;
        i0 += i1;
        l5 = i0;
      }
      i0 = l10;
      i1 = 33u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 3396u;
        i1 = l5;
        i2 = b;
        i3 = l3;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 33u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_203;}
        i0 = a;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = l2;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 8u);
        i1 = l2;
        i1 = LoadU32(ctx, i1 + 12u);
        i0 -= i1;
        i1 = 4u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        l7 = i0;
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_205;}
        i0 = l4;
        i1 = a;
        StoreU32(ctx, i0 + 16u, i1);
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = l7;
        i2 = 1u;
        i1 += i2;
        StoreU32(ctx, i0 + 12u, i1);
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 8u);
        l7 = i0;
        i1 = 3u;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l7;
        i1 = l12;
        d1 = (double)(int32_t)(i1);
        StoreF64(ctx, i0, d1);
        i0 = l2;
        i1 = l2;
        i1 = LoadU32(ctx, i1 + 8u);
        i2 = 16u;
        i1 += i2;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_205;}
        i0 = l4;
        i1 = l4;
        i1 = LoadU32(ctx, i1 + 8u);
        l2 = i1;
        StoreU32(ctx, i0 + 32u, i1);
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 8u);
        i1 = l2;
        i1 = LoadU32(ctx, i1 + 12u);
        i0 -= i1;
        i1 = 4u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        l7 = i0;
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_207;}
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = l7;
        i2 = 1u;
        i1 += i2;
        StoreU32(ctx, i0 + 36u, i1);
        i0 = l4;
        i1 = l4;
        i2 = 8u;
        i1 += i2;
        l5 = i1;
        StoreU32(ctx, i0 + 40u, i1);
      }
      i0 = l10;
      i1 = 34u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 969u;
        i1 = l2;
        i2 = kF3087MsgReservedcommands;
        HostOsSchedule(ctx, i0, i1, i2);
        i0 = 34u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_207;}
        i0 = l4;
        i1 = l4;
        i1 = LoadU32(ctx, i1 + 32u);
        l2 = i1;
        StoreU32(ctx, i0 + 48u, i1);
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 8u);
        i1 = l2;
        i1 = LoadU32(ctx, i1 + 12u);
        i0 -= i1;
        i1 = 4u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        l7 = i0;
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_209;}
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = l7;
        i2 = 1u;
        i1 += i2;
        StoreU32(ctx, i0 + 52u, i1);
        i0 = l4;
        i1 = l4;
        i2 = 32u;
        i1 += i2;
        StoreU32(ctx, i0 + 56u, i1);
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 8u);
        l7 = i0;
        i1 = 3u;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l7;
        i1 = l8;
        d1 = (double)(int32_t)(i1);
        StoreF64(ctx, i0, d1);
        i0 = l2;
        i1 = l2;
        i1 = LoadU32(ctx, i1 + 8u);
        i2 = 16u;
        i1 += i2;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_209;}
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 48u;
        i0 += i1;
        l5 = i0;
        i0 = l4;
        i1 = 24u;
        i0 += i1;
        l2 = i0;
      }
      i0 = l10;
      i1 = 35u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 3397u;
        i1 = l5;
        i2 = l2;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l6 = i0;
        i0 = 35u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l6;
        l5 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3087ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3087ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
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
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 48u);
          l2 = i0;
          if (i0) {
            i0 = kF3087ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = kF3087ConstFFFFFFFE;
            ResizeValueStack(ctx, i0, i1);
            i0 = kF3087ConstD015C;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = kF3087ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 == i1;
            if (i0) {goto block_6;}
          }
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 32u);
          l2 = i0;
          if (i0) {
            i0 = kF3087ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = kF3087ConstFFFFFFFE;
            ResizeValueStack(ctx, i0, i1);
            i0 = kF3087ConstD015C;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = kF3087ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 == i1;
            if (i0) {goto block_6;}
          }
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 8u);
          l2 = i0;
          if (i0) {
            i0 = kF3087ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = kF3087ConstFFFFFFFE;
            ResizeValueStack(ctx, i0, i1);
            i0 = kF3087ConstD015C;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = kF3087ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 == i1;
            if (i0) {goto block_6;}
          }
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 24u);
          l2 = i0;
          i0 = !(i0);
          l5 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l5;
          if (i0) {goto block_226;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 28u);
          l7 = i0;
          i1 = kF3087ConstFFFFFFFE;
          i0 = i0 == i1;
          if (i0) {goto block_226;}
          i0 = kF3087ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l10;
        i1 = 36u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1002u;
          i1 = l2;
          i2 = kF3087ConstFFFFD8F0;
          i3 = l7;
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
          i0 = kF3087ConstD015C;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = kF3087ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 == i1;
          if (i0) {goto block_6;}
        }
        block_226:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i1 = 44u;
          i0 += i1;
          l3 = i0;
          i1 = l9;
          i1 = LoadU32(ctx, i1);
          i0 -= i1;
          i1 = 4092u;
          i0 = i0 == i1;
          if (i0) {
            i0 = l9;
            i0 = LoadU32(ctx, i0 + 4u);
            l3 = i0;
            i0 = l9;
            i1 = kF3087ResumeWordBytes;
            i0 += i1;
            l9 = i0;
          }
          i0 = l8;
          i1 = 1u;
          i0 += i1;
          l8 = i0;
          i0 = b;
          i0 = LoadU32(ctx, i0 + 20u);
          l2 = i0;
          i1 = 2816u;
          i0 += i1;
          i0 = LoadU32(ctx, i0);
          l7 = i0;
          i0 = l2;
          i1 = 2812u;
          i0 += i1;
          i0 = LoadU32(ctx, i0);
          l5 = i0;
          i0 = l2;
          i1 = 2824u;
          i0 += i1;
          i0 = LoadU32(ctx, i0);
          l11 = i0;
          goto loop_210;
        }
      }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      l2 = i0;
      i0 = HostTempRet0(ctx);
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 48u);
      b = i0;
      i0 = !(i0);
      l3 = i0;
      if (i0) {goto block_208;}
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = kF3087ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_208;}
      goto block_6;
    }
    block_209:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      l2 = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
    }
    block_208:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 32u);
      b = i0;
      i0 = !(i0);
      l3 = i0;
      if (i0) {goto block_206;}
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = kF3087ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_206;}
      goto block_6;
    }
    block_207:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      l2 = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
    }
    block_206:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 8u);
      b = i0;
      i0 = !(i0);
      l3 = i0;
      if (i0) {goto block_204;}
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = kF3087ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_204;}
      goto block_6;
    }
    block_205:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      l2 = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
    }
    block_204:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 24u);
      b = i0;
      i0 = !(i0);
      l3 = i0;
      if (i0) {goto block_15;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 28u);
      l4 = i0;
      i1 = kF3087ConstFFFFFFFE;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_15;}
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l10;
    i1 = 37u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1002u;
      i1 = b;
      i2 = kF3087ConstFFFFD8F0;
      i3 = l4;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 37u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3087ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3087ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_15;}
      goto block_6;
    }
    block_203:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      l2 = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
      goto block_15;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_15;
  }
  block_179:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_175;
  }
  block_178:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_176;
  }
  block_177:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 48u);
    b = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_176;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF3087ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_176;}
    goto block_6;
  }
  block_176:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 32u);
    b = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_175;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF3087ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_175;}
    goto block_6;
  }
  block_175:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_15;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i1 = kF3087ConstFFFFFFFE;
    i0 = i0 == i1;
    l3 = i0;
    if (i0) {goto block_15;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l10;
  i1 = 38u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1002u;
    i1 = b;
    i2 = kF3087ConstFFFFD8F0;
    i3 = l4;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 38u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_15;}
    goto block_6;
  }
  block_166:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_15;
  }
  block_165:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_163;
  }
  block_164:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 48u);
    b = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_163;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF3087ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_163;}
    goto block_6;
  }
  block_163:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 32u);
    b = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_15;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF3087ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_15;}
    goto block_6;
  }
  block_155:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_15;
  }
  block_154:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_152;
  }
  block_153:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 48u);
    b = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_152;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF3087ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_152;}
    goto block_6;
  }
  block_152:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 32u);
    b = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_15;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF3087ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_15;}
    goto block_6;
  }
  block_37:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_15;
  }
  block_36:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_34;
  }
  block_35:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 48u);
    b = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_34;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF3087ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_34;}
    goto block_6;
  }
  block_34:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 32u);
    b = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_15;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 36u);
    l4 = i0;
    i1 = kF3087ConstFFFFFFFE;
    i0 = i0 == i1;
    l3 = i0;
    if (i0) {goto block_15;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l10;
  i1 = 39u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1002u;
    i1 = b;
    i2 = kF3087ConstFFFFD8F0;
    i3 = l4;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 39u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_15;}
    goto block_6;
  }
  block_33:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_15;
  }
  block_32:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_28;
  }
  block_31:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_29;
  }
  block_30:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 48u);
    b = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_29;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF3087ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_29;}
    goto block_6;
  }
  block_29:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 32u);
    b = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_28;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF3087ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_28;}
    goto block_6;
  }
  block_28:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_15;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i1 = kF3087ConstFFFFFFFE;
    i0 = i0 == i1;
    l3 = i0;
    if (i0) {goto block_15;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l10;
  i1 = 40u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1002u;
    i1 = b;
    i2 = kF3087ConstFFFFD8F0;
    i3 = l4;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 40u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_15;}
    goto block_6;
  }
  block_27:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_15;
  }
  block_26:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_24;
  }
  block_25:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 48u);
    b = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_24;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF3087ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_24;}
    goto block_6;
  }
  block_24:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 32u);
    b = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_15;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF3087ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_15;}
    goto block_6;
  }
  block_23:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_15;
  }
  block_22:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_20;
  }
  block_21:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 48u);
    b = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_20;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF3087ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_20;}
    goto block_6;
  }
  block_20:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 32u);
    b = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_15;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF3087ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_15;}
    goto block_6;
  }
  block_19:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_15;
  }
  block_18:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_16;
  }
  block_17:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 48u);
    b = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_16;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF3087ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_16;}
    goto block_6;
  }
  block_16:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 32u);
    b = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_15;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 36u);
    l4 = i0;
    i1 = kF3087ConstFFFFFFFE;
    i0 = i0 == i1;
    l3 = i0;
    if (i0) {goto block_15;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l10;
  i1 = 41u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1002u;
    i1 = b;
    i2 = kF3087ConstFFFFD8F0;
    i3 = l4;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 41u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_15;}
    goto block_6;
  }
  block_15:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = !(i0);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    if (i0) {goto block_288;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 4u);
    a = i0;
    i1 = kF3087ConstFFFFFFFE;
    i0 = i0 == i1;
    if (i0) {goto block_288;}
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l10;
  i1 = 42u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1002u;
    i1 = b;
    i2 = kF3087ConstFFFFD8F0;
    i3 = a;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 42u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3087ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF3087ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_287;}
  }
  block_288:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_287:;
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    a = i0;
    i0 = HostTempRet0(ctx);
  }
  i0 = l10;
  i1 = 43u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 43u;
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
  l6 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l6;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF3087ResumeWordBytes;
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
  i1 = l2;
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
  i1 = l8;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l6;
  i1 = l9;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = l6;
  i1 = l11;
  StoreU32(ctx, i0 + 36u, i1);
  i0 = l6;
  i1 = l12;
  StoreU32(ctx, i0 + 40u, i1);
  i0 = l6;
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
}
