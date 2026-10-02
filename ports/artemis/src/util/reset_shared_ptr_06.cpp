// reset_shared_ptr_06.cpp -- one instantiation of boost::shared_ptr<T>::reset: the data segment carries the shared_ptr.hpp assert trio (__FILE__, 'p == 0 || p != px', __func__ 'reset') and the body is that reset -- the owned pointer goes through the swap/release the assert guards; f199 Panic is the only callee..
//
// ResetSharedPtr06 -- one instantiation of boost::shared_ptr<T>::reset: the data segment carries the shared_ptr.hpp assert trio (__FILE__, 'p == 0 || p != px', __func__ 'reset') and the body is that reset -- the owned pointer goes through the swap/release the assert guards; f199 Panic is the only callee.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 1568372-1568372 (f3395); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void Panic(EngineContext* ctx, uint32_t a0);

namespace {

constexpr uint32_t kF3395MsgReset = 0xAF57Cu;  // reset
constexpr uint32_t kF3395MsgBoostBoostSmartPtr = 0xB5E39u;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
constexpr uint32_t kF3395MsgPPPx = 0xB71B4u;  // p == 0 || p != px
constexpr uint32_t kF3395ConstB7354 = 0xB7354u;  // 
constexpr uint32_t kF3395ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF3395SaveFrameBytes = 20u;
constexpr uint32_t kF3395ResumeWordBytes = 4u;

}  // namespace

void ResetSharedPtr06(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF3395SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l6 = i0;
    i0 = l4;
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
    i2 = kF3395ResumeWordBytes;
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
    i1 = 16u;
    i0 -= i1;
    l2 = i0;
    GuestStackSp(ctx) = i0;
    i0 = b;
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
      i0 = l2;
      j1 = 0ull;
      StoreU64(ctx, i0 + 8u, j1);
      i0 = kF3395ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = 0u;
      b = i0;
    }
    i0 = l5;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 16u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3395ConstD015C;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = kF3395ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 != i1;
      l6 = i0;
      if (i0) {goto block_5;}
      i0 = 0u;
      i0 = FindMatchingCatch(ctx, i0);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
      i0 = a;
      i0 = HostRestorePendingThrow(ctx, i0);
      a = i0;
      i0 = kF3395ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1019u;
      HostMarkRegionSeen(ctx, i0);
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3395ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3395ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
    }
    i0 = a;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        a = i0;
        i0 = kF3395ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l5;
      i1 = 2u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1021u;
        HostMarkRegionSeen(ctx, i0);
        i0 = 2u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3395ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3395ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_13;}
        i0 = 0u;
        i0 = FindMatchingCatch(ctx, i0);
        a = i0;
        i0 = HostTempRet0(ctx);
        b = i0;
      }
      i0 = l5;
      i1 = 3u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        Panic(ctx, i0);
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
      TrapUnreachable();
    }
    block_13:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 12u);
      a = i0;
      i0 = !(i0);
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      if (i0) {goto block_23;}
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 4u);
      l2 = i1;
      i2 = 1u;
      i1 -= i2;
      l3 = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_23;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      l2 = i0;
      i0 = kF3395ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 4u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 4u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3395ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3395ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_3;}
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 8u);
      l2 = i1;
      i2 = 1u;
      i1 -= i2;
      l3 = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_23;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      l2 = i0;
      i0 = kF3395ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 5u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 5u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3395ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3395ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_3;}
    }
    block_23:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      HostOsRelease(ctx, i0);
      TrapUnreachable();
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i1 = b;
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
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = b;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = kF3395ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 8u;
      i0 += i1;
      i1 = 4u;
      i0 |= i1;
      l6 = i0;
      i0 = l2;
      i1 = 8u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l5;
    i1 = 6u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2791u;
      i1 = l3;
      i2 = b;
      i3 = l6;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 6u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3395ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3395ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 12u);
        l3 = i0;
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 8u);
        b = i0;
        goto running_exit;
      }
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 12u);
      a = i0;
      i0 = !(i0);
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      if (i0) {goto block_37;}
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 4u);
      l2 = i1;
      i2 = 1u;
      i1 -= i2;
      l3 = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_37;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      l2 = i0;
      i0 = kF3395ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 7u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 7u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3395ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3395ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_3;}
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 8u);
      l2 = i1;
      i2 = 1u;
      i1 -= i2;
      l3 = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_37;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      l2 = i0;
      i0 = kF3395ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 8u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 8u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3395ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3395ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_3;}
    }
    block_37:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      HostOsRelease(ctx, i0);
      TrapUnreachable();
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3395MsgPPPx;
    i1 = kF3395MsgBoostBoostSmartPtr;
    i2 = 698u;
    i3 = kF3395MsgReset;
    HostAssertFail(ctx, i0, i1, i2, i3);
    TrapUnreachable();
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    j1 = 4294967297ull;
    StoreU64(ctx, i0 + 4u, j1);
    i0 = l3;
    i1 = 0u;
    StoreU32(ctx, i0 + 12u, i1);
    i0 = l3;
    i1 = kF3395ConstB7354;
    StoreU32(ctx, i0, i1);
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i1 = a;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = a;
    i1 = b;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 4u);
    b = i0;
    i0 = a;
    i1 = l3;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l2;
    i1 = b;
    StoreU32(ctx, i0 + 12u, i1);
    i0 = b;
    i0 = !(i0);
    a = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    if (i0) {goto block_47;}
    i0 = b;
    i1 = b;
    i1 = LoadU32(ctx, i1 + 4u);
    a = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 != i1;
    a = i0;
    if (i0) {goto block_47;}
    i0 = b;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    a = i0;
    i0 = kF3395ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l5;
  i1 = 9u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = b;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 9u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3395ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF3395ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_3;}
    i0 = b;
    i1 = b;
    i1 = LoadU32(ctx, i1 + 8u);
    a = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 != i1;
    a = i0;
    if (i0) {goto block_47;}
    i0 = b;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    a = i0;
    i0 = kF3395ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l5;
  i1 = 10u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = b;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 10u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3395ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF3395ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_3;}
  }
  block_47:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i1 = 16u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_3:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    a = i0;
    i0 = HostTempRet0(ctx);
  }
  i0 = l5;
  i1 = 11u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 11u;
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
  l4 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l4;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF3395ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l4;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l4;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l4;
  i1 = l6;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 20u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
