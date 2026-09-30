// release_shared_object_holder.cpp -- releases a shared object holder: the holder's reference is dropped and the object is freed when it was the last owner..
//
// ReleaseSharedObjectHolder -- releases a shared object holder: the holder's reference is dropped and the object is freed when it was the last owner.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 318349-318349 (f839); R1 transcribe-keep.
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

constexpr uint32_t kF839MsgPx = 0xBCA76u;  // px != 0
constexpr uint32_t kF839MsgBoostBoostSmartPtr = 0xBCA7Eu;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
constexpr uint32_t kF839MsgOperator = 0xBCAB6u;  // operator->
constexpr uint32_t kF839ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF839ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF839SaveFrameBytes = 20u;
constexpr uint32_t kF839ResumeWordBytes = 4u;

}  // namespace

void ReleaseSharedObjectHolder(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF839SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 4u);
    l1 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF839ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    i1 = 16u;
    i0 -= i1;
    l3 = i0;
    GuestStackSp(ctx) = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 48u);
    i1 = 2u;
    i0 = i0 == i1;
    l1 = i0;
  }
  i0 = l1;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 32u);
      l1 = i0;
      i0 = !(i0);
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
        i0 = kF839ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i0 = !(i0);
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF839MsgPx;
        i2 = kF839MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF839MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 0u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF839ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF839ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto running_exit;}
        goto block_3;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 8u, j1);
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 216u);
      l2 = i0;
      i0 = kF839ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 8u;
      i0 += i1;
      l5 = i0;
    }
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = l1;
      i2 = l5;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF839ConstD015C;
      i0 = LoadU32(ctx, i0);
      l1 = i0;
      i0 = kF839ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l1;
      i1 = 1u;
      i0 = i0 != i1;
      l1 = i0;
    }
    i0 = l1;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 12u);
        l1 = i0;
        i0 = !(i0);
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        if (i0) {goto block_21;}
        i0 = l1;
        i1 = l1;
        i1 = LoadU32(ctx, i1 + 4u);
        l2 = i1;
        i2 = 1u;
        i1 -= i2;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 != i1;
        l2 = i0;
        if (i0) {goto block_21;}
        i0 = l1;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 8u);
        l2 = i0;
        i0 = kF839ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 2u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l1;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 2u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF839ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF839ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_3;}
        i0 = l1;
        i1 = l1;
        i1 = LoadU32(ctx, i1 + 8u);
        l2 = i1;
        i2 = 1u;
        i1 -= i2;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 != i1;
        l2 = i0;
        if (i0) {goto block_21;}
        i0 = l1;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        l2 = i0;
        i0 = kF839ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 3u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l1;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 3u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF839ConstD015C;
        i0 = LoadU32(ctx, i0);
        l1 = i0;
        i0 = kF839ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l1;
        i1 = 1u;
        i0 = i0 == i1;
        l1 = i0;
        if (i0) {goto block_3;}
      }
      block_21:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 32u);
        l1 = i0;
        if (i0) {goto block_16;}
        i0 = kF839ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 4u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF839MsgPx;
        i2 = kF839MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF839MsgOperator;
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
        i0 = kF839ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF839ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto running_exit;}
        goto block_3;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      l1 = i0;
      i0 = HostTempRet0(ctx);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 12u);
      a = i0;
      i0 = !(i0);
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      if (i0) {goto block_31;}
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 4u);
      l3 = i1;
      i2 = 1u;
      i1 -= i2;
      l2 = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 != i1;
      l3 = i0;
      if (i0) {goto block_31;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      l3 = i0;
      i0 = kF839ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 5u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l3;
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
      i0 = kF839ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF839ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_3;}
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 8u);
      l3 = i1;
      i2 = 1u;
      i1 -= i2;
      l2 = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 != i1;
      l3 = i0;
      if (i0) {goto block_31;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      l3 = i0;
      i0 = kF839ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 6u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l3;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 6u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF839ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF839ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_3;}
    }
    block_31:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l1;
      HostOsRelease(ctx, i0);
      TrapUnreachable();
    }
    block_16:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 256u);
      l2 = i0;
    }
    i0 = l4;
    i1 = 7u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l1;
      i1 = kF839ConstFFFFFFFF;
      i2 = 0u;
      i3 = l2;
      CallTableT5(ctx, i3, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
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
    i0 = a;
    i1 = 1u;
    StoreU8(ctx, i0 + 60u, i1);
    i0 = a;
    i1 = 0u;
    StoreU32(ctx, i0 + 48u, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 24u);
    l1 = i0;
    i0 = !(i0);
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
      i0 = kF839ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 8u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF839MsgPx;
      i2 = kF839MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF839MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 8u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF839ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF839ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto running_exit;}
      goto block_3;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 348u);
    l2 = i0;
  }
  i0 = l4;
  i1 = 9u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l1;
    i1 = l2;
    CallTableT1(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    i0 = 9u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 128u);
    if (i0) {
      i0 = a;
      i1 = 2u;
      StoreU32(ctx, i0 + 128u, i1);
    }
    i0 = l3;
    i1 = 16u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    TrapUnreachable();
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
  i0 = l4;
  i1 = 10u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 10u;
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
  i2 = kF839ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = l1;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l4;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l4;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l4;
  i1 = l5;
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
