#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step calls.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void Panic(EngineContext* ctx, uint32_t a0);
uint32_t CombineComparisonResults(EngineContext* ctx, uint32_t left, uint32_t right, uint32_t result, uint32_t box);

// ReleaseChainedResourceNodes -- frees a chain of resource nodes: the walk
// starts at the head, each node's payload is released, and the head slot is
// cleared last.

void ReleaseChainedResourceNodes(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  constexpr uint32_t kData8C564 = 0x8C564u;
  constexpr uint32_t kData8DDB4 = 0x8DDB4u;
  constexpr uint32_t kMsgPx = 0xB5E31u;  // px != 0
  constexpr uint32_t kMsgSmartPtrPath = 0xB5E39u;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
  constexpr uint32_t kMsgOperator = 0xB5E71u;  // operator->
  constexpr uint32_t kDataB8F8C = 0xB8F8Cu;
  constexpr uint32_t kDataC81C8 = 0xC81C8u;
  constexpr uint32_t kDataCF130 = 0xCF130u;
  constexpr uint32_t kDataCF148 = 0xCF148u;
  constexpr uint32_t kDataCF14C = 0xCF14Cu;
  constexpr uint32_t kDataFFFFFFC0 = 0xFFFFFFC0u;
  constexpr uint32_t kDataFFFFFFFE = 0xFFFFFFFEu;
  constexpr uint32_t kSaveFrameBytes = 40u;
  constexpr uint32_t kResumeWordBytes = 4u;
  uint32_t l10 = 0u; float l11 = 0; float l12 = 0; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  uint64_t j1 = 0;
  float f0 = 0; float f1 = 0;
  double d1 = 0;
      l10 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kSaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l7 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 24u);
    l8 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 28u);
    l9 = i0;
    i0 = l7;
    f0 = LoadF32(ctx, i0 + 32u);
    l11 = f0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 36u);
    l10 = i0;
    i0 = l7;
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
    i2 = kResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    i1 = kDataFFFFFFC0;
    i0 += i1;
    l5 = i0;
    GuestStackSp(ctx) = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
    a = i0;
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
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l6;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kMsgPx;
      i2 = kMsgSmartPtrPath;
      i3 = 734u;
      i4 = kMsgOperator;
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
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto exit_path;}
      goto block_3;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 24u);
    a = i0;
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
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l6;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kMsgPx;
      i2 = kMsgSmartPtrPath;
      i3 = 734u;
      i4 = kMsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto exit_path;}
      goto block_3;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 304u);
    l8 = i0;
    i0 = l5;
    i1 = 56u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l6;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = a;
    i2 = b;
    i3 = l8;
    CallTableT5(ctx, i3, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 56u);
    a = i0;
    i0 = !(i0);
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    if (i0) {
      i0 = 0u;
      b = i0;
      goto block_20;
    }
    i0 = 0u;
    b = i0;
  }
  i0 = l6;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kData8C564;
    i2 = kDataB8F8C;
    i3 = 0u;
    i0 = CombineComparisonResults(ctx, i0, i1, i2, i3);
    l7 = i0;
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = !(i0);
    a = i0;
    if (i0) {
      i0 = 0u;
      l4 = i0;
      goto block_20;
    }
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 60u);
    b = i0;
    i0 = !(i0);
    a = i0;
    if (i0) {
      i0 = 0u;
      b = i0;
      goto block_20;
    }
    i0 = b;
    i1 = b;
    i1 = LoadU32(ctx, i1 + 4u);
    i2 = 1u;
    i1 += i2;
    a = i1;
    StoreU32(ctx, i0 + 4u, i1);
  }
  block_20:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 60u);
    a = i0;
    i0 = !(i0);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {goto block_28;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 4u);
    l3 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 != i1;
    l2 = i0;
    if (i0) {goto block_28;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l6;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l3;
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
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kDataD015C;
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
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 != i1;
    l2 = i0;
    if (i0) {goto block_28;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l6;
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
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_3;}
  }
  block_28:;
  i0 = a;
  i1 = l4;
  i1 = !(i1);
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  a = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    if (i0) {goto block_39;}
    i0 = l4;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 388u);
    a = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l6;
  i1 = 6u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = l4;
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
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_38;}
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l6;
  i1 = 7u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1032u;
    i1 = 112u;
    i0 = HostFoldLayoutOffset(ctx, i0, i1);
    l7 = i0;
    i0 = 7u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    a = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_38;}
    i0 = l5;
    i1 = b;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = l5;
    i1 = l4;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = b;
    if (i0) {
      i0 = b;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 4u);
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 4u, i1);
    }
    i0 = l5;
    j1 = 4294967388ull;
    StoreU64(ctx, i0 + 8u, j1);
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    j1 = 4294967388ull;
    StoreU64(ctx, i0, j1);
    i0 = l5;
    i1 = 16u;
    i0 += i1;
    l8 = i0;
    i0 = l5;
    i1 = 24u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l6;
  i1 = 8u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2748u;
    i1 = l2;
    i2 = l5;
    i3 = l8;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 8u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = 1u;
    l9 = i0;
    i0 = l3;
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
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 24u;
      i0 += i1;
      l8 = i0;
      i0 = l5;
      i1 = 40u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l6;
    i1 = 9u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2749u;
      i1 = l2;
      i2 = l8;
      i3 = 0u;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l7 = i0;
      i0 = 9u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l9 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l9;
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
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = LoadU32(ctx, i0 + 44u);
        l9 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l6;
      i1 = 10u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l9;
        i1 = l4;
        f0 = HostInvokeFi(ctx, i0, i1);
        l12 = f0;
        i0 = 10u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        f0 = l12;
        l11 = f0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = 1u;
        l9 = i0;
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        l2 = i0;
        if (i0) {goto block_37;}
        i0 = a;
        j1 = 0ull;
        StoreU64(ctx, i0 + 4u, j1);
        i0 = a;
        i1 = kData8DDB4;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 0u;
        StoreU32(ctx, i0 + 72u, i1);
        i0 = a;
        i1 = 1000u;
        StoreU32(ctx, i0 + 40u, i1);
        i0 = a;
        j1 = 0ull;
        StoreU64(ctx, i0 + 32u, j1);
        i0 = a;
        j1 = 0ull;
        StoreU64(ctx, i0 + 44u, j1);
        i0 = a;
        j1 = 0ull;
        StoreU64(ctx, i0 + 12u, j1);
        i0 = a;
        i1 = 0u;
        StoreU32(ctx, i0 + 20u, i1);
        i0 = a;
        f1 = l11;
        d1 = (double)wasm_quietf(f1);
        StoreF64(ctx, i0 + 24u, d1);
        i0 = a;
        j1 = 0ull;
        StoreU64(ctx, i0 + 52u, j1);
        i0 = a;
        j1 = 0ull;
        StoreU64(ctx, i0 + 60u, j1);
        i0 = a;
        i1 = 0u;
        StoreU8(ctx, i0 + 68u, i1);
        i0 = l3;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = !(i0);
        l8 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        if (i0) {goto block_58;}
        i0 = a;
        i1 = l2;
        StoreU32(ctx, i0 + 72u, i1);
        i0 = l2;
        i1 = 1u;
        i0 &= i1;
        l8 = i0;
        if (i0) {
          i0 = a;
          i1 = l3;
          j1 = LoadU64(ctx, i1 + 4u);
          StoreU64(ctx, i0 + 76u, j1);
          i0 = a;
          i1 = l3;
          i1 = LoadU32(ctx, i1 + 12u);
          l2 = i1;
          StoreU32(ctx, i0 + 84u, i1);
          goto block_58;
        }
        i0 = l2;
        i1 = kDataFFFFFFFE;
        i0 &= i1;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 76u;
        i0 += i1;
        l10 = i0;
        i0 = l3;
        i1 = 4u;
        i0 += i1;
        l8 = i0;
      }
      i0 = l6;
      i1 = 11u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l8;
        i2 = l10;
        i3 = 0u;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 11u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        l2 = i0;
        if (i0) {goto block_37;}
      }
      block_58:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = 0u;
        l9 = i0;
        i0 = a;
        i1 = 0u;
        StoreU8(ctx, i0 + 104u, i1);
        i0 = a;
        i1 = 0u;
        StoreU32(ctx, i0 + 88u, i1);
        i0 = kDataCF148;
        i0 = LoadU8(ctx, i0);
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
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l5;
          i1 = 56u;
          i0 += i1;
          l2 = i0;
        }
        i0 = l6;
        i1 = 12u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1800u;
          i1 = l2;
          i2 = kDataCF130;
          i3 = kDataC81C8;
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
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {goto block_63;}
          goto block_37;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 56u;
        i0 += i1;
        l2 = i0;
      }
      i0 = l6;
      i1 = 13u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1800u;
        i1 = l2;
        i2 = kDataCF14C;
        i3 = kDataC81C8;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 13u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        l2 = i0;
        if (i0) {goto block_37;}
      }
      block_63:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        i0 = LoadU32(ctx, i0 + 56u);
        i1 = a;
        StoreU32(ctx, i0 + 12u, i1);
        i0 = kDataC81C8;
        i1 = kDataC81C8;
        i1 = LoadU32(ctx, i1);
        l2 = i1;
        i2 = 1u;
        i1 += i2;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 380u);
        l8 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l6;
      i1 = 14u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l8;
        i1 = l4;
        i2 = l2;
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
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 1u;
        i0 = i0 == i1;
        l2 = i0;
        if (i0) {goto block_37;}
        i0 = l3;
        i0 = LoadU32(ctx, i0);
        a = i0;
      }
      i0 = a;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = l2;
        i1 = a;
        i2 = 1u;
        i1 &= i2;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        l2 = i0;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          if (i0) {goto block_78;}
          i0 = a;
          i1 = kDataFFFFFFFE;
          i0 &= i1;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = !(i0);
          l2 = i0;
          if (i0) {goto block_78;}
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          l2 = i0;
          i0 = l3;
          i1 = 4u;
          i0 += i1;
          a = i0;
        }
        i0 = l6;
        i1 = 15u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l2;
          i1 = a;
          i2 = a;
          i3 = 2u;
          HostSyncSlot(ctx, i0, i1, i2, i3);
          i0 = 15u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 == i1;
          a = i0;
          if (i0) {goto block_3;}
        }
        block_78:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        i0 = LoadU32(ctx, i0 + 36u);
        a = i0;
        i0 = !(i0);
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        if (i0) {goto block_84;}
        i0 = a;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 4u);
        l4 = i1;
        i2 = 1u;
        i1 -= i2;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = l4;
        i1 = 1u;
        i0 = i0 != i1;
        l2 = i0;
        if (i0) {goto block_84;}
        i0 = a;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = LoadU32(ctx, i0 + 8u);
        l4 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l6;
      i1 = 16u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l4;
        i1 = a;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 16u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_3;}
        i0 = a;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 8u);
        l4 = i1;
        i2 = 1u;
        i1 -= i2;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l4;
        i1 = 1u;
        i0 = i0 != i1;
        l2 = i0;
        if (i0) {goto block_84;}
        i0 = a;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = LoadU32(ctx, i0 + 12u);
        l4 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l6;
      i1 = 17u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l4;
        i1 = a;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 17u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 == i1;
        a = i0;
        if (i0) {goto block_3;}
      }
      block_84:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        i0 = LoadU32(ctx, i0 + 20u);
        a = i0;
        i0 = !(i0);
        l2 = i0;
        if (i0) {goto block_39;}
        i0 = a;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 4u);
        l4 = i1;
        i2 = 1u;
        i1 -= i2;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = l4;
        i1 = 1u;
        i0 = i0 != i1;
        l2 = i0;
        if (i0) {goto block_39;}
        i0 = a;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = LoadU32(ctx, i0 + 8u);
        l4 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l6;
      i1 = 18u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l4;
        i1 = a;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 18u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
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
          i0 = a;
          i1 = a;
          i1 = LoadU32(ctx, i1 + 8u);
          l4 = i1;
          i2 = 1u;
          i1 -= i2;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l4;
          i1 = 1u;
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {goto block_39;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = LoadU32(ctx, i0 + 12u);
          l4 = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l6;
        i1 = 19u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l4;
          i1 = a;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 19u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_39;}
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_3;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      l4 = i0;
      i0 = 1u;
      l9 = i0;
      i0 = HostTempRet0(ctx);
      l2 = i0;
      goto block_36;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l4 = i0;
    i0 = HostTempRet0(ctx);
    l2 = i0;
    goto block_35;
  }
  block_39:;
  i0 = a;
  i1 = b;
  i1 = !(i1);
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  a = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    if (i0) {goto block_100;}
    i0 = b;
    i1 = b;
    i1 = LoadU32(ctx, i1 + 4u);
    a = i1;
    i2 = 1u;
    i1 -= i2;
    l2 = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 != i1;
    a = i0;
    if (i0) {goto block_100;}
    i0 = b;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    a = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l6;
  i1 = 20u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = b;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 20u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kDataD015C;
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
    l2 = i1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 != i1;
    a = i0;
    if (i0) {goto block_100;}
    i0 = b;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    a = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l6;
  i1 = 21u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = b;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 21u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_3;}
  }
  block_100:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = kDataFFFFFFC0;
    i0 -= i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_38:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l4 = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_34;
  }
  block_37:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l4 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = !(i0);
    if (i0) {goto block_36;}
    i0 = l2;
    i1 = 1u;
    i0 &= i1;
    l8 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l8;
    if (i0) {goto block_110;}
    i0 = l2;
    i1 = kDataFFFFFFFE;
    i0 &= i1;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = !(i0);
    if (i0) {goto block_110;}
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    l8 = i0;
    i0 = l3;
    i1 = 4u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l6;
  i1 = 22u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l8;
    i1 = l2;
    i2 = l2;
    i3 = 2u;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 22u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_109;}
  }
  block_110:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    goto block_36;
  }
  block_109:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_3;}
  block_36:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 36u);
    l3 = i0;
    i0 = !(i0);
    l2 = i0;
    if (i0) {goto block_35;}
    i0 = l3;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 4u);
    l2 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 != i1;
    l2 = i0;
    if (i0) {goto block_35;}
    i0 = l3;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l6;
  i1 = 23u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = l3;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 23u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kDataD015C;
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
      i0 = l3;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 8u);
      l2 = i1;
      i2 = 1u;
      i1 -= i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_35;}
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      l2 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l6;
    i1 = 24u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = l3;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 24u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_35;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_3;}
  block_35:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
    i0 = !(i0);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {goto block_124;}
    i0 = l5;
    i1 = l5;
    i1 = LoadU32(ctx, i1 + 4u);
    l3 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_124;}
    i0 = l5;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l6;
  i1 = 25u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l3;
    i1 = l5;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 25u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_3;}
    i0 = l5;
    i1 = l5;
    i1 = LoadU32(ctx, i1 + 8u);
    l3 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_124;}
    i0 = l5;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l6;
  i1 = 26u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l3;
    i1 = l5;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 26u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_3;}
  }
  block_124:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l9;
    i0 = !(i0);
    if (i0) {goto block_34;}
    i0 = a;
    DeallocateRawBlock(ctx, i0);
  }
  block_34:;
  i0 = a;
  i1 = b;
  i1 = !(i1);
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  a = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    if (i0) {goto block_131;}
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
    if (i0) {goto block_131;}
    i0 = b;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    a = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l6;
  i1 = 27u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = b;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 27u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kDataD015C;
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
    if (i0) {goto block_131;}
    i0 = b;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    a = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l6;
  i1 = 28u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = b;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 28u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_3;}
  }
  block_131:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    HostOsRelease(ctx, i0);
  }
  exit_path:;
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
  i0 = l6;
  i1 = 29u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 29u;
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
  l7 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l7;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l7 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l7;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l7;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l7;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l7;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l7;
  i1 = l5;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l7;
  i1 = l8;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l7;
  i1 = l9;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l7;
  f1 = l11;
  StoreF32(ctx, i0 + 32u, f1);
  i0 = l7;
  i1 = l10;
  StoreU32(ctx, i0 + 36u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 40u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}  // namespace artc
