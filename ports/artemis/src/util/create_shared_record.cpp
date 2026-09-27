// create_shared_record.cpp -- builds a shared user record for a value and links it into the shared owner list..
//
// CreateSharedRecord -- builds a shared user record for a value and links it into the shared owner list.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 345454-345454 (f885); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void Panic(EngineContext* ctx, uint32_t a0);
uint32_t FillBytes(EngineContext* ctx, uint32_t dst, uint32_t value, uint32_t length);

namespace {

constexpr uint32_t kF885Const8AF98 = 0x8AF98u;  // 
constexpr uint32_t kF885Const8B050 = 0x8B050u;  // 
constexpr uint32_t kF885Const8B07C = 0x8B07Cu;  // 
constexpr uint32_t kF885ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF885ConstFFFFFFFC = 0xFFFFFFFCu;  // 
constexpr uint32_t kF885SaveFrameBytes = 20u;
constexpr uint32_t kF885ResumeWordBytes = 4u;

}  // namespace

void CreateSharedRecord(EngineContext* ctx, uint32_t state, uint32_t value) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF885SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 16u);
    l6 = i0;
    i0 = l3;
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
    i2 = kF885ResumeWordBytes;
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
    i0 = kF885ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l5;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1032u;
    i1 = 40u;
    i0 = HostFoldLayoutOffset(ctx, i0, i1);
    l3 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF885ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF885ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
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
      i0 = 0u;
      i0 = FindMatchingCatch(ctx, i0);
      state = i0;
      i0 = HostTempRet0(ctx);
      value = i0;
      i0 = state;
      i0 = HostRestorePendingThrow(ctx, i0);
      state = i0;
      i0 = kF885ConstD015C;
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
      i0 = kF885ConstD015C;
      i0 = LoadU32(ctx, i0);
      state = i0;
      i0 = kF885ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = state;
      i1 = 1u;
      i0 = i0 == i1;
      state = i0;
    }
    i0 = state;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        state = i0;
        i0 = HostTempRet0(ctx);
        value = i0;
        i0 = kF885ConstD015C;
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
        i0 = kF885ConstD015C;
        i0 = LoadU32(ctx, i0);
        value = i0;
        i0 = kF885ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = value;
        i1 = 1u;
        i0 = i0 != i1;
        if (i0) {goto block_6;}
        i0 = 0u;
        i0 = FindMatchingCatch(ctx, i0);
        state = i0;
        i0 = HostTempRet0(ctx);
        value = i0;
      }
      i0 = l5;
      i1 = 3u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
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
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    j1 = 4294967297ull;
    StoreU64(ctx, i0 + 4u, j1);
    i0 = l2;
    i1 = 0u;
    StoreU8(ctx, i0 + 16u, i1);
    i0 = l2;
    i1 = 0u;
    StoreU32(ctx, i0 + 12u, i1);
    i0 = l2;
    i1 = kF885Const8AF98;
    StoreU32(ctx, i0, i1);
    i0 = value;
    i0 = LoadU32(ctx, i0);
    value = i0;
    i0 = l2;
    i1 = kF885Const8B050;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = kF885ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = value;
    i1 = 23u;
    i0 += i1;
    i1 = kF885ConstFFFFFFFC;
    i0 &= i1;
    l4 = i0;
  }
  i0 = l5;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1032u;
    i1 = l4;
    i0 = HostFoldLayoutOffset(ctx, i0, i1);
    l3 = i0;
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  if (i0) {
    i0 = l6;
  } else {
    i0 = kF885ConstD015C;
    i0 = LoadU32(ctx, i0);
    l6 = i0;
    i0 = kF885ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l6;
    i1 = 1u;
    i0 = i0 != i1;
  }
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      j1 = 4294967297ull;
      StoreU64(ctx, i0 + 4u, j1);
      i0 = l4;
      i1 = value;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l4;
      i1 = kF885Const8B07C;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 20u;
      i0 += i1;
      l6 = i0;
      i0 = value;
      if (i0) {
        i0 = l6;
        i1 = 0u;
        i2 = value;
        i0 = FillBytes(ctx, i0, i1, i2);
      }
      i0 = l2;
      i1 = value;
      StoreU32(ctx, i0 + 36u, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 32u, i1);
      i0 = l2;
      i1 = l4;
      StoreU32(ctx, i0 + 28u, i1);
      i0 = l2;
      i1 = l6;
      StoreU32(ctx, i0 + 24u, i1);
      i0 = l2;
      i1 = 1u;
      StoreU8(ctx, i0 + 16u, i1);
      i0 = state;
      i1 = l2;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = state;
      i1 = l2;
      i2 = 20u;
      i1 += i2;
      value = i1;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 4u);
      state = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      if (i0) {goto block_26;}
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      state = i0;
      i0 = kF885ConstD015C;
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
      i0 = state;
      i1 = l2;
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
      i0 = kF885ConstD015C;
      i0 = LoadU32(ctx, i0);
      state = i0;
      i0 = kF885ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = state;
      i1 = 1u;
      i0 = i0 == i1;
      state = i0;
      if (i0) {goto block_5;}
      i0 = l2;
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 8u);
      state = i1;
      i2 = 1u;
      i1 -= i2;
      value = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = state;
      i1 = 1u;
      i0 = i0 != i1;
      state = i0;
      if (i0) {goto block_26;}
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      state = i0;
      i0 = kF885ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 6u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l2;
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
      i0 = kF885ConstD015C;
      i0 = LoadU32(ctx, i0);
      state = i0;
      i0 = kF885ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = state;
      i1 = 1u;
      i0 = i0 == i1;
      state = i0;
      if (i0) {goto block_5;}
    }
    block_26:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      goto func_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    state = i0;
    i0 = HostTempRet0(ctx);
    i0 = l2;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 4u);
    value = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = value;
    i1 = 1u;
    i0 = i0 != i1;
    value = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = value;
    if (i0) {goto block_34;}
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    value = i0;
    i0 = kF885ConstD015C;
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
    i0 = value;
    i1 = l2;
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
    i0 = kF885ConstD015C;
    i0 = LoadU32(ctx, i0);
    value = i0;
    i0 = kF885ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = value;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_5;}
    i0 = l2;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 8u);
    value = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = value;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_34;}
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    value = i0;
    i0 = kF885ConstD015C;
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
    i0 = value;
    i1 = l2;
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
    i0 = kF885ConstD015C;
    i0 = LoadU32(ctx, i0);
    value = i0;
    i0 = kF885ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = value;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_5;}
  }
  block_34:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    state = i0;
    i0 = HostTempRet0(ctx);
  }
  i0 = l5;
  i1 = 9u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    Panic(ctx, i0);
    i0 = 9u;
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
  l3 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l3;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF885ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = value;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l3;
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l3;
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
