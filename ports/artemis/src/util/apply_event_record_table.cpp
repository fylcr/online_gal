#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step calls.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void Panic(EngineContext* ctx, uint32_t a0);
void FindTaggedTreeEntry(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);

// ApplyEventRecordTable -- walks the queued twenty byte event records of the
// engine object, resolves each one through the tagged tree and applies it.

void ApplyEventRecordTable(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  constexpr uint32_t kDataB5E30 = 0xB5E30u;
  constexpr uint32_t kMsgPx = 0xB5E31u;  // px != 0
  constexpr uint32_t kMsgSmartPtrPath = 0xB5E39u;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
  constexpr uint32_t kMsgOperator = 0xB5E71u;  // operator->
  constexpr uint32_t kDataFFFFFFC0 = 0xFFFFFFC0u;
  constexpr uint32_t kDataFFFFFFFF = 0xFFFFFFFFu;
  constexpr uint32_t kSaveFrameBytes = 32u;
  constexpr uint32_t kResumeWordBytes = 4u;
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u;
  uint64_t j1 = 0;
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
    i0 = LoadU32(ctx, i0 + 4u);
    l1 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 24u);
    l8 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 28u);
    l7 = i0;
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
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    i1 = kDataFFFFFFC0;
    i0 += i1;
    l3 = i0;
    GuestStackSp(ctx) = i0;
    i0 = a;
    i1 = 13699u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    l1 = i0;
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = a;
      i1 = 13692u;
      i0 += i1;
      i0 = LoadU32(ctx, i0);
      goto exit_path;
    }
    i0 = l1;
    i1 = 255u;
    i0 &= i1;
    exit_path:;
    i0 = !(i0);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {goto block_11;}
    i0 = a;
    i1 = 13688u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = a;
    i2 = 13308u;
    i1 += i2;
    l1 = i1;
    StoreU32(ctx, i0 + 48u, i1);
    i0 = l3;
    i1 = 48u;
    i0 += i1;
    l8 = i0;
    i0 = l3;
    i1 = 56u;
    i0 += i1;
    l7 = i0;
    i0 = a;
    i1 = 13668u;
    i0 += i1;
    l6 = i0;
    i0 = l3;
    i1 = 32u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l4;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = l6;
    i2 = l1;
    i3 = kDataB5E30;
    i4 = l8;
    i5 = l7;
    FindTaggedTreeEntry(ctx, i0, i1, i2, i3, i4, i5);
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 32u);
    i0 = LoadU8(ctx, i0 + 32u);
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
      i0 = a;
      i0 = LoadU32(ctx, i0 + 16u);
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
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
        if (i0) {goto block_10;}
        goto block_6;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l1;
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
        i0 = kDataD015C;
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
        i0 = 857u;
        i1 = kMsgPx;
        i2 = kMsgSmartPtrPath;
        i3 = 734u;
        i4 = kMsgOperator;
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
        if (i0) {goto block_10;}
        goto block_6;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 304u);
      l6 = i0;
      i0 = l3;
      i1 = 32u;
      i0 += i1;
      l2 = i0;
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
      i2 = l5;
      i3 = l6;
      CallTableT5(ctx, i3, i0, i1, i2);
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
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 36u);
      l1 = i0;
      i0 = 0u;
      l6 = i0;
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 32u);
      l2 = i0;
      i0 = !(i0);
      l8 = i0;
      if (i0) {
        i0 = l6;
      } else {
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        i1 = 152u;
        i0 -= i1;
        i0 = LoadU32(ctx, i0);
        i1 = l2;
        i0 += i1;
      }
      l2 = i0;
      i0 = !(i0);
      l6 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      if (i0) {goto block_35;}
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 124u);
      l6 = i0;
      i0 = kDataD015C;
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
      i0 = l6;
      i1 = l2;
      i2 = 0u;
      HostOsSchedule(ctx, i0, i1, i2);
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
      l2 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_35;}
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = l1;
      i0 = !(i0);
      if (i0) {goto block_7;}
      i0 = l1;
      i1 = l1;
      i1 = LoadU32(ctx, i1 + 4u);
      l3 = i1;
      i2 = 1u;
      i1 -= i2;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 != i1;
      if (i0) {goto block_7;}
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = LoadU32(ctx, i0 + 8u);
      l3 = i0;
      i0 = kDataD015C;
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
      i1 = l1;
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
      l3 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
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
        i0 = l1;
        i1 = l1;
        i1 = LoadU32(ctx, i1 + 8u);
        l3 = i1;
        i2 = 1u;
        i1 -= i2;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 != i1;
        if (i0) {goto block_7;}
        i0 = l1;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = LoadU32(ctx, i0 + 12u);
        l3 = i0;
        i0 = kDataD015C;
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
        i1 = l1;
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
        l3 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 != i1;
        l2 = i0;
        if (i0) {goto block_7;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_6;}
    block_35:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l1;
      i0 = !(i0);
      if (i0) {goto block_15;}
      i0 = l1;
      i1 = l1;
      i1 = LoadU32(ctx, i1 + 4u);
      l2 = i1;
      i2 = 1u;
      i1 -= i2;
      l6 = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      if (i0) {goto block_15;}
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      l2 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 7u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = l1;
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
        i0 = l1;
        i1 = l1;
        i1 = LoadU32(ctx, i1 + 8u);
        l2 = i1;
        i2 = 1u;
        i1 -= i2;
        l6 = i1;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 != i1;
        if (i0) {goto block_15;}
        i0 = l1;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        l2 = i0;
        i0 = kDataD015C;
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
        i0 = l2;
        i1 = l1;
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
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l1 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l1;
        i1 = 1u;
        i0 = i0 != i1;
        l2 = i0;
        if (i0) {goto block_15;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_6;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 13680u);
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
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 9u;
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
      i0 = 9u;
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
      if (i0) {goto block_10;}
      goto block_6;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 32u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 43u, i1);
    i0 = l1;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 304u);
    l2 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 32u;
    i0 += i1;
    l8 = i0;
    i0 = l3;
    i1 = 48u;
    i0 += i1;
    l6 = i0;
  }
  i0 = l4;
  i1 = 10u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = l6;
    i2 = l1;
    i3 = l8;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 10u;
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
    l1 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l1;
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
      i0 = LoadS8(ctx, i0 + 43u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 32u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 48u);
      l1 = i0;
      i0 = !(i0);
      if (i0) {goto block_14;}
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 124u);
      l2 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 11u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = l1;
      i2 = 0u;
      HostOsSchedule(ctx, i0, i1, i2);
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
      l1 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l1;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_62;}
      goto block_9;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 43u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_7;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 32u);
    DeallocateRawBlock(ctx, i0);
    i0 = a;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_62:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 13680u);
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
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 12u;
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
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_10;}
      goto block_6;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 32u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 43u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 24u, j1);
    i0 = l1;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 324u);
    l2 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 24u;
    i0 += i1;
    l8 = i0;
    i0 = l3;
    i1 = 32u;
    i0 += i1;
    l6 = i0;
  }
  i0 = l4;
  i1 = 13u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = l1;
    i2 = l6;
    i3 = l8;
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
    l1 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l1;
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
      i0 = LoadU32(ctx, i0 + 28u);
      l1 = i0;
      i0 = !(i0);
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      if (i0) {goto block_85;}
      i0 = l1;
      i1 = l1;
      i1 = LoadU32(ctx, i1 + 4u);
      l2 = i1;
      i2 = 1u;
      i1 -= i2;
      l6 = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_85;}
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      l2 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 14u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = l1;
      HostReleaseSlotPair(ctx, i0, i1);
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
      l2 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_6;}
      i0 = l1;
      i1 = l1;
      i1 = LoadU32(ctx, i1 + 8u);
      l2 = i1;
      i2 = 1u;
      i1 -= i2;
      l6 = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_85;}
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      l2 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 15u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = l1;
      HostReleaseSlotPair(ctx, i0, i1);
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
      l1 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l1;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_6;}
    }
    block_85:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 43u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 32u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = a;
      i0 = LoadU32(ctx, i0 + 13680u);
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 16u;
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
        a = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_10;}
        goto block_6;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 348u);
      l2 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 17u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = l1;
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
      l1 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l1;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_9;}
      i0 = a;
      i0 = LoadU32(ctx, i0 + 16u);
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 18u;
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
        a = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_10;}
        goto block_6;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l1;
      i0 = LoadU32(ctx, i0 + 100u);
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 19u;
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
        if (i0) {goto block_10;}
        goto block_6;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 16u, j1);
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 368u);
      l2 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 16u;
      i0 += i1;
      l6 = i0;
    }
    i0 = l4;
    i1 = 20u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = l1;
      i2 = l6;
      HostOsSchedule(ctx, i0, i1, i2);
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
      l1 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l1;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_80;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 20u);
      l1 = i0;
      i0 = !(i0);
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      if (i0) {goto block_114;}
      i0 = l1;
      i1 = l1;
      i1 = LoadU32(ctx, i1 + 4u);
      l2 = i1;
      i2 = 1u;
      i1 -= i2;
      l6 = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_114;}
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      l2 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 21u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = l1;
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
      l2 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_6;}
      i0 = l1;
      i1 = l1;
      i1 = LoadU32(ctx, i1 + 8u);
      l2 = i1;
      i2 = 1u;
      i1 -= i2;
      l6 = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_114;}
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      l2 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 22u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = l1;
      HostReleaseSlotPair(ctx, i0, i1);
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
      l1 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l1;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_6;}
    }
    block_114:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 16u);
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 23u;
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
        a = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_10;}
        goto block_6;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l1;
      i0 = LoadU32(ctx, i0 + 24u);
      l2 = i0;
      i0 = !(i0);
      l6 = i0;
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 24u;
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
        a = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_10;}
        goto block_6;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 48u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 52u);
      l1 = i1;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l1;
      if (i0) {
        i0 = l1;
        i1 = l1;
        i1 = LoadU32(ctx, i1 + 4u);
        i2 = 1u;
        i1 += i2;
        StoreU32(ctx, i0 + 4u, i1);
      }
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 324u);
      l1 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 8u;
      i0 += i1;
      l6 = i0;
    }
    i0 = l4;
    i1 = 25u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l1;
      i1 = l2;
      i2 = l5;
      i3 = l6;
      HostSyncSlot(ctx, i0, i1, i2, i3);
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
      l1 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l1;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_79;}
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
      if (i0) {goto block_136;}
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
      if (i0) {goto block_136;}
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      l2 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 26u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = l1;
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
      l2 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_6;}
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
      if (i0) {goto block_136;}
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      l2 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 27u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = l1;
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
      l1 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l1;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_6;}
    }
    block_136:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 52u);
      l1 = i0;
      i0 = !(i0);
      if (i0) {goto block_15;}
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
      if (i0) {goto block_15;}
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      l2 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 28u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = l1;
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
        if (i0) {goto block_15;}
        i0 = l1;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        l2 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 29u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l1;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 29u;
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
        l1 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l1;
        i1 = 1u;
        i0 = i0 != i1;
        l2 = i0;
        if (i0) {goto block_15;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_6;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 28u);
    l1 = i0;
    i0 = !(i0);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {goto block_151;}
    i0 = l1;
    i1 = l1;
    i1 = LoadU32(ctx, i1 + 4u);
    l5 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 != i1;
    l2 = i0;
    if (i0) {goto block_151;}
    i0 = l1;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0 + 8u);
    l5 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l4;
  i1 = 30u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l5;
    i1 = l1;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 30u;
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
    if (i0) {goto block_6;}
    i0 = l1;
    i1 = l1;
    i1 = LoadU32(ctx, i1 + 8u);
    l5 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 != i1;
    l2 = i0;
    if (i0) {goto block_151;}
    i0 = l1;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0 + 12u);
    l5 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l4;
  i1 = 31u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l5;
    i1 = l1;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 31u;
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
    l1 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l1;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_6;}
  }
  block_151:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 43u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_8;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 32u);
    DeallocateRawBlock(ctx, i0);
    goto block_8;
  }
  block_80:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 20u);
    l1 = i0;
    i0 = !(i0);
    if (i0) {goto block_8;}
    i0 = l1;
    i1 = l1;
    i1 = LoadU32(ctx, i1 + 4u);
    l5 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_8;}
    i0 = l1;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0 + 8u);
    l5 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l4;
  i1 = 32u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l5;
    i1 = l1;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 32u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  if (i0) {
    i0 = l2;
  } else {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
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
      i0 = l1;
      i1 = l1;
      i1 = LoadU32(ctx, i1 + 8u);
      l5 = i1;
      i2 = 1u;
      i1 -= i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 != i1;
      if (i0) {goto block_8;}
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      l5 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 33u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l5;
      i1 = l1;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 33u;
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
      l1 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l1;
      i1 = 1u;
      i0 = i0 != i1;
      if (i0) {goto block_8;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_6;}
  block_79:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l1 = i0;
    i0 = !(i0);
    if (i0) {goto block_8;}
    i0 = l1;
    i1 = l1;
    i1 = LoadU32(ctx, i1 + 4u);
    l5 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_8;}
    i0 = l1;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    l5 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l4;
  i1 = 34u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l5;
    i1 = l1;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 34u;
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
    if (i0) {goto block_6;}
    i0 = l1;
    i1 = l1;
    i1 = LoadU32(ctx, i1 + 8u);
    l5 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_8;}
    i0 = l1;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    l5 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l4;
  i1 = 35u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l5;
    i1 = l1;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 35u;
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
    l1 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l1;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_6;}
    goto block_8;
  }
  block_15:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 13688u);
      i1 = 0u;
      StoreU8(ctx, i0, i1);
      i0 = a;
      i1 = 13692u;
      i0 += i1;
      a = i0;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      goto block_11;
    }
    i0 = l5;
    i1 = 0u;
    StoreU8(ctx, i0 + 11u, i1);
    i0 = l5;
    i1 = 0u;
    StoreU8(ctx, i0, i1);
    goto block_11;
  }
  block_14:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 52u);
    a = i0;
    i0 = !(i0);
    if (i0) {goto block_11;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 4u);
    l1 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l1;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_11;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    l1 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l4;
  i1 = 36u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l1;
    i1 = a;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 36u;
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
    l1 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l1;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_6;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 8u);
    l1 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l1;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_11;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    l1 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l4;
  i1 = 37u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l1;
    i1 = a;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 37u;
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
    if (i0) {goto block_6;}
  }
  block_11:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = kDataFFFFFFC0;
    i0 -= i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_10:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    TrapUnreachable();
  }
  block_9:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
  }
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 52u);
    l3 = i0;
    i0 = !(i0);
    if (i0) {goto block_7;}
    i0 = l3;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 4u);
    l1 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l1;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_7;}
    i0 = l3;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    l1 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l4;
  i1 = 38u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l1;
    i1 = l3;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 38u;
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
    l1 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l1;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_6;}
    i0 = l3;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 8u);
    l1 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l1;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_7;}
    i0 = l3;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    l1 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l4;
  i1 = 39u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l1;
    i1 = l3;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 39u;
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
    if (i0) {goto block_6;}
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    a = i0;
    i0 = HostTempRet0(ctx);
  }
  i0 = l4;
  i1 = 40u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 40u;
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
  i2 = kResumeWordBytes;
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
  i0 = l4;
  i1 = l6;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l4;
  i1 = l8;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l4;
  i1 = l7;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 32u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}  // namespace artc
