// assign_shared_owner.cpp -- installs a shared owner on a value, walking the tag field the owner must be recorded in..
//
// AssignSharedOwner -- installs a shared owner on a value, walking the tag field the owner must be recorded in.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 125260-125260 (f255); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void Panic(EngineContext* ctx, uint32_t a0);
uint32_t CopyBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void CreateSharedRecord(EngineContext* ctx, uint32_t state, uint32_t value);

namespace {

constexpr uint32_t kF255MsgPx = 0x91399u;  // px != 0
constexpr uint32_t kF255MsgBoostBoostSmartPtr = 0x913A1u;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
constexpr uint32_t kF255MsgOperator = 0x913D9u;  // operator->
constexpr uint32_t kF255ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF255SaveFrameBytes = 24u;
constexpr uint32_t kF255ResumeWordBytes = 4u;

}  // namespace

void AssignSharedOwner(EngineContext* ctx, uint32_t owner, uint32_t value, uint32_t shared) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF255SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    owner = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    shared = i0;
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
    i2 = kF255ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l7 = i0;
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
    i1 = shared;
    StoreU32(ctx, i0 + 12u, i1);
    i0 = owner;
    i0 = LoadU32(ctx, i0 + 36u);
    l4 = i0;
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
      i0 = owner;
      i1 = 36u;
      i0 += i1;
      l4 = i0;
      i0 = owner;
      i0 = LoadU32(ctx, i0 + 32u);
      l3 = i0;
      i0 = owner;
      i1 = 0u;
      StoreU32(ctx, i0 + 32u, i1);
      i0 = owner;
      i1 = l3;
      i2 = 1u;
      i1 <<= (i2 & 31);
      l3 = i1;
      StoreU32(ctx, i0 + 36u, i1);
      i0 = shared;
      i1 = l3;
      i0 = i0 > i1;
      if (i0) {
        i0 = l4;
        i1 = shared;
        StoreU32(ctx, i0, i1);
        i0 = shared;
        l3 = i0;
      }
      i0 = l3;
      i1 = owner;
      i1 = LoadU32(ctx, i1 + 24u);
      shared = i1;
      i0 = i0 < i1;
      l3 = i0;
      if (i0) {
        i0 = l4;
        i1 = shared;
        StoreU32(ctx, i0, i1);
      }
    }
    i0 = l7;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l6;
      i1 = l4;
      CreateSharedRecord(ctx, i0, i1);
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF255ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 16u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l5 = i0;
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      shared = i0;
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = shared;
    } else {
      i0 = kF255ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF255ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_9;}
      i0 = shared;
      i1 = l6;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 4u);
      l4 = i0;
      i0 = shared;
      i1 = owner;
      i2 = 4u;
      i1 += i2;
      l3 = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = shared;
      i1 = l4;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = shared;
      i1 = owner;
      i1 = LoadU32(ctx, i1 + 4u);
      l4 = i1;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = shared;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = owner;
      i1 = shared;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = owner;
      i1 = owner;
      i1 = LoadU32(ctx, i1 + 12u);
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = owner;
      i0 = LoadU32(ctx, i0 + 36u);
      l4 = i0;
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 12u);
    }
    shared = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = owner;
    i0 = LoadU32(ctx, i0 + 4u);
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i0 = shared;
    i1 = l4;
    i0 = i0 <= i1;
    if (i0) {goto block_7;}
    i0 = l3;
    if (i0) {goto block_8;}
    i0 = kF255ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 857u;
    i1 = kF255MsgPx;
    i2 = kF255MsgBoostBoostSmartPtr;
    i3 = 734u;
    i4 = kF255MsgOperator;
    HostFailAssertion(ctx, i0, i1, i2, i3, i4);
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF255ConstD015C;
    i0 = LoadU32(ctx, i0);
    owner = i0;
    i0 = kF255ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = owner;
    i1 = 1u;
    i0 = i0 != i1;
    owner = i0;
    if (i0) {goto block_5;}
    goto block_3;
  }
  block_9:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    value = i0;
    i0 = HostTempRet0(ctx);
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 4u);
    owner = i0;
    i0 = !(i0);
    if (i0) {goto running_exit;}
    i0 = owner;
    i1 = owner;
    i1 = LoadU32(ctx, i1 + 4u);
    shared = i1;
    i2 = 1u;
    i1 -= i2;
    l3 = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = shared;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto running_exit;}
    i0 = owner;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    shared = i0;
    i0 = kF255ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = shared;
    i1 = owner;
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
    i0 = kF255ConstD015C;
    i0 = LoadU32(ctx, i0);
    shared = i0;
    i0 = kF255ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = shared;
    i1 = 1u;
    i0 = i0 != i1;
    shared = i0;
  }
  i0 = shared;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = owner;
      i1 = owner;
      i1 = LoadU32(ctx, i1 + 8u);
      shared = i1;
      i2 = 1u;
      i1 -= i2;
      l3 = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = shared;
      i1 = 1u;
      i0 = i0 != i1;
      if (i0) {goto running_exit;}
      i0 = owner;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      shared = i0;
      i0 = kF255ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 4u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = shared;
      i1 = owner;
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
      i0 = kF255ConstD015C;
      i0 = LoadU32(ctx, i0);
      owner = i0;
      i0 = kF255ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = owner;
      i1 = 1u;
      i0 = i0 != i1;
      owner = i0;
      if (i0) {goto running_exit;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_3;}
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 4u);
    i0 += i1;
    l3 = i0;
    i1 = owner;
    i1 = LoadU32(ctx, i1 + 32u);
    i0 += i1;
    i1 = value;
    i2 = l4;
    i0 = CopyBytes(ctx, i0, i1, i2);
    i0 = owner;
    i1 = 0u;
    StoreU32(ctx, i0 + 36u, i1);
    i0 = owner;
    i1 = l4;
    i2 = owner;
    i2 = LoadU32(ctx, i2 + 28u);
    i1 += i2;
    StoreU32(ctx, i0 + 28u, i1);
    i0 = owner;
    i1 = l4;
    i2 = owner;
    i2 = LoadU32(ctx, i2 + 32u);
    i1 += i2;
    StoreU32(ctx, i0 + 32u, i1);
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 12u);
    i1 = l4;
    i0 -= i1;
    shared = i0;
    i0 = value;
    i1 = l4;
    i0 += i1;
    value = i0;
  }
  i0 = l7;
  i1 = 5u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = owner;
    i1 = value;
    i2 = shared;
    AssignSharedOwner(ctx, i0, i1, i2);
    i0 = 5u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_6;}
  block_7:;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  i1 = l4;
  i2 = l3;
  i2 = !(i2);
  i3 = ResumePhase(ctx);
  i1 = i3 ? i1 : i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF255ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 6u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF255MsgPx;
      i2 = kF255MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF255MsgOperator;
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
      i0 = kF255ConstD015C;
      i0 = LoadU32(ctx, i0);
      owner = i0;
      i0 = kF255ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = owner;
      i1 = 1u;
      i0 = i0 != i1;
      owner = i0;
      if (i0) {goto block_5;}
      goto block_3;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = owner;
    i0 = LoadU32(ctx, i0 + 32u);
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 12u);
    i2 = l3;
    i2 = LoadU32(ctx, i2 + 4u);
    i1 += i2;
    i0 += i1;
    i1 = value;
    i2 = shared;
    i0 = CopyBytes(ctx, i0, i1, i2);
    i0 = owner;
    i1 = l6;
    i1 = LoadU32(ctx, i1 + 12u);
    value = i1;
    i2 = owner;
    i2 = LoadU32(ctx, i2 + 28u);
    i1 += i2;
    StoreU32(ctx, i0 + 28u, i1);
    i0 = owner;
    i1 = owner;
    i1 = LoadU32(ctx, i1 + 32u);
    i2 = value;
    i1 += i2;
    StoreU32(ctx, i0 + 32u, i1);
    i0 = owner;
    i1 = owner;
    i1 = LoadU32(ctx, i1 + 36u);
    i2 = value;
    i1 -= i2;
    value = i1;
    StoreU32(ctx, i0 + 36u, i1);
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i1 = 16u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    TrapUnreachable();
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = value;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_3:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    owner = i0;
    i0 = HostTempRet0(ctx);
  }
  i0 = l7;
  i1 = 7u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = owner;
    Panic(ctx, i0);
    i0 = 7u;
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
  l5 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l5;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF255ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l5 = i0;
  i1 = owner;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = value;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l5;
  i1 = shared;
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
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 24u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
