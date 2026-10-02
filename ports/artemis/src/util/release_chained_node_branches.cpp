// release_chained_node_branches.cpp -- frees the branch links of a chained node: every branch on the chain is released through the node release path and the chain head is cleared..
//
// ReleaseChainedNodeBranches -- frees the branch links of a chained node: every branch on the chain is released through the node release path and the chain head is cleared.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 412239-412239 (f1033); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void Panic(EngineContext* ctx, uint32_t a0);
uint32_t CombineComparisonResults(EngineContext* ctx, uint32_t left, uint32_t right, uint32_t result, uint32_t box);

namespace {

constexpr uint32_t kF1033Const8C564 = 0x8C564u;  // 
constexpr uint32_t kF1033Const8DDB4 = 0x8DDB4u;  // 
constexpr uint32_t kF1033MsgPx = 0xB5E31u;  // px != 0
constexpr uint32_t kF1033MsgBoostBoostSmartPtr = 0xB5E39u;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
constexpr uint32_t kF1033MsgOperator = 0xB5E71u;  // operator->
constexpr uint32_t kF1033ConstB8F8C = 0xB8F8Cu;  // 
constexpr uint32_t kF1033ConstC81C8 = 0xC81C8u;  // 
constexpr uint32_t kF1033ConstCF130 = 0xCF130u;  // 
constexpr uint32_t kF1033ConstCF148 = 0xCF148u;  // 
constexpr uint32_t kF1033ConstCF14C = 0xCF14Cu;  // 
constexpr uint32_t kF1033ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF1033ConstFFFFFFC0 = 0xFFFFFFC0u;  // 
constexpr uint32_t kF1033ConstFFFFFFFE = 0xFFFFFFFEu;  // 
constexpr uint32_t kF1033SaveFrameBytes = 36u;
constexpr uint32_t kF1033ResumeWordBytes = 4u;

}  // namespace

void ReleaseChainedNodeBranches(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l10 = 0u; float l11 = 0; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  uint64_t j1 = 0;
  float f0 = 0; float f1 = 0; float f2 = 0;
  double d1 = 0;
      l10 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1033SaveFrameBytes;
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
    i0 = LoadU32(ctx, i0 + 32u);
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
    i2 = kF1033ResumeWordBytes;
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
    i1 = kF1033ConstFFFFFFC0;
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
      i0 = kF1033ConstD015C;
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
      i1 = kF1033MsgPx;
      i2 = kF1033MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF1033MsgOperator;
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
      i0 = kF1033ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF1033ConstD015C;
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
      i0 = kF1033ConstD015C;
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
      i1 = kF1033MsgPx;
      i2 = kF1033MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF1033MsgOperator;
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
      i0 = kF1033ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF1033ConstD015C;
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
    b = i0;
    i0 = !(i0);
    a = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    if (i0) {
      i0 = 0u;
      a = i0;
      i0 = 0u;
      b = i0;
      goto block_20;
    }
    i0 = 0u;
    a = i0;
  }
  i0 = l6;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = kF1033Const8C564;
    i2 = kF1033ConstB8F8C;
    i3 = 0u;
    i0 = CombineComparisonResults(ctx, i0, i1, i2, i3);
    l7 = i0;
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i0 = !(i0);
    l2 = i0;
    if (i0) {
      i0 = 0u;
      b = i0;
      goto block_20;
    }
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 60u);
    a = i0;
    i0 = !(i0);
    l2 = i0;
    if (i0) {
      i0 = 0u;
      a = i0;
      goto block_20;
    }
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 4u);
    i2 = 1u;
    i1 += i2;
    l2 = i1;
    StoreU32(ctx, i0 + 4u, i1);
  }
  block_20:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 60u);
    l3 = i0;
    i0 = !(i0);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {goto block_28;}
    i0 = l3;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 4u);
    l4 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
    l2 = i0;
    if (i0) {goto block_28;}
    i0 = l3;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
    i0 = kF1033ConstD015C;
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
    i0 = l4;
    i1 = l3;
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
    i0 = kF1033ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF1033ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_3;}
    i0 = l3;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 8u);
    l4 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
    l2 = i0;
    if (i0) {goto block_28;}
    i0 = l3;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = kF1033ConstD015C;
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
    i0 = l4;
    i1 = l3;
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
    i0 = kF1033ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF1033ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_3;}
  }
  block_28:;
  i0 = l2;
  i1 = b;
  i1 = !(i1);
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l2 = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {goto block_39;}
    i0 = b;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0 + 388u);
    l3 = i0;
    i0 = kF1033ConstD015C;
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
    i0 = l3;
    i1 = b;
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
    i0 = kF1033ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF1033ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_38;}
    i0 = b;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0 + 92u);
    l3 = i0;
    i0 = kF1033ConstD015C;
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
    i0 = l3;
    i1 = b;
    f2 = 0.0f;
    HostInvokeVif(ctx, i0, i1, f2);
    i0 = 7u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1033ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF1033ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_38;}
    i0 = b;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0 + 124u);
    l3 = i0;
    i0 = kF1033ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l6;
  i1 = 8u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l3;
    i1 = b;
    i2 = 1u;
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
    i0 = kF1033ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF1033ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_38;}
    i0 = kF1033ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l6;
  i1 = 9u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1032u;
    i1 = 112u;
    i0 = HostFoldLayoutOffset(ctx, i0, i1);
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
    i0 = kF1033ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF1033ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_38;}
    i0 = l5;
    i1 = a;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = l5;
    i1 = b;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = a;
    if (i0) {
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 4u);
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 4u, i1);
    }
    i0 = l5;
    j1 = 4294967388ull;
    StoreU64(ctx, i0 + 8u, j1);
    i0 = kF1033ConstD015C;
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
  i1 = 10u;
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
    i0 = 10u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1033ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF1033ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = 1u;
    l9 = i0;
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
      i0 = kF1033ConstD015C;
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
    i1 = 11u;
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
      i0 = 11u;
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
      i0 = kF1033ConstD015C;
      i0 = LoadU32(ctx, i0);
      l9 = i0;
      i0 = kF1033ConstD015C;
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
        i0 = b;
        f0 = LoadF32(ctx, i0 + 112u);
        l11 = f0;
        i0 = l3;
        j1 = 0ull;
        StoreU64(ctx, i0 + 4u, j1);
        i0 = l3;
        i1 = kF1033Const8DDB4;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        j1 = 0ull;
        StoreU64(ctx, i0 + 12u, j1);
        i0 = l3;
        j1 = 0ull;
        StoreU64(ctx, i0 + 20u, j1);
        i0 = l3;
        i1 = 0u;
        StoreU32(ctx, i0 + 28u, i1);
        i0 = l3;
        i1 = 0u;
        StoreU32(ctx, i0 + 72u, i1);
        i0 = l3;
        i1 = 1000u;
        StoreU32(ctx, i0 + 40u, i1);
        i0 = l3;
        j1 = 0ull;
        StoreU64(ctx, i0 + 44u, j1);
        i0 = l3;
        j1 = 0ull;
        StoreU64(ctx, i0 + 52u, j1);
        i0 = l3;
        j1 = 0ull;
        StoreU64(ctx, i0 + 60u, j1);
        i0 = l3;
        i1 = 0u;
        StoreU8(ctx, i0 + 68u, i1);
        i0 = l3;
        f1 = l11;
        d1 = (double)wasm_quietf(f1);
        StoreF64(ctx, i0 + 32u, d1);
        i0 = l4;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = !(i0);
        l8 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        if (i0) {goto block_60;}
        i0 = l3;
        i1 = l2;
        StoreU32(ctx, i0 + 72u, i1);
        i0 = 1u;
        l9 = i0;
        i0 = l2;
        i1 = 1u;
        i0 &= i1;
        l8 = i0;
        if (i0) {
          i0 = l3;
          i1 = l4;
          j1 = LoadU64(ctx, i1 + 4u);
          StoreU64(ctx, i0 + 76u, j1);
          i0 = l3;
          i1 = l4;
          i1 = LoadU32(ctx, i1 + 12u);
          l2 = i1;
          StoreU32(ctx, i0 + 84u, i1);
          goto block_60;
        }
        i0 = l2;
        i1 = kF1033ConstFFFFFFFE;
        i0 &= i1;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF1033ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 76u;
        i0 += i1;
        l10 = i0;
        i0 = l4;
        i1 = 4u;
        i0 += i1;
        l8 = i0;
      }
      i0 = l6;
      i1 = 12u;
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
        i0 = 12u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF1033ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF1033ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        l2 = i0;
        if (i0) {goto block_37;}
      }
      block_60:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i1 = 0u;
        StoreU8(ctx, i0 + 104u, i1);
        i0 = l3;
        i1 = 0u;
        StoreU32(ctx, i0 + 88u, i1);
        i0 = kF1033ConstCF148;
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
          i0 = 0u;
          l9 = i0;
          i0 = kF1033ConstD015C;
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
          i2 = kF1033ConstCF130;
          i3 = kF1033ConstC81C8;
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
          i0 = kF1033ConstD015C;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = kF1033ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {goto block_65;}
          goto block_37;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = 0u;
        l9 = i0;
        i0 = kF1033ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 56u;
        i0 += i1;
        l2 = i0;
      }
      i0 = l6;
      i1 = 14u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1800u;
        i1 = l2;
        i2 = kF1033ConstCF14C;
        i3 = kF1033ConstC81C8;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 14u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF1033ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF1033ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        l2 = i0;
        if (i0) {goto block_37;}
      }
      block_65:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        i0 = LoadU32(ctx, i0 + 56u);
        i1 = l3;
        StoreU32(ctx, i0 + 12u, i1);
        i0 = kF1033ConstC81C8;
        i1 = kF1033ConstC81C8;
        i1 = LoadU32(ctx, i1);
        l2 = i1;
        i2 = 1u;
        i1 += i2;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 380u);
        l8 = i0;
        i0 = kF1033ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l6;
      i1 = 15u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l8;
        i1 = b;
        i2 = l2;
        HostOsSchedule(ctx, i0, i1, i2);
        i0 = 15u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF1033ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF1033ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto block_37;}
        i0 = l4;
        i0 = LoadU32(ctx, i0);
        b = i0;
      }
      i0 = b;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = l2;
        i1 = b;
        i2 = 1u;
        i1 &= i2;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        l2 = i0;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          if (i0) {goto block_80;}
          i0 = b;
          i1 = kF1033ConstFFFFFFFE;
          i0 &= i1;
          i0 = LoadU32(ctx, i0);
          b = i0;
          i0 = !(i0);
          l2 = i0;
          if (i0) {goto block_80;}
          i0 = kF1033ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = b;
          l2 = i0;
          i0 = l4;
          i1 = 4u;
          i0 += i1;
          b = i0;
        }
        i0 = l6;
        i1 = 16u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l2;
          i1 = b;
          i2 = b;
          i3 = 2u;
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
          i0 = kF1033ConstD015C;
          i0 = LoadU32(ctx, i0);
          b = i0;
          i0 = kF1033ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = b;
          i1 = 1u;
          i0 = i0 == i1;
          b = i0;
          if (i0) {goto block_3;}
        }
        block_80:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        i0 = LoadU32(ctx, i0 + 36u);
        b = i0;
        i0 = !(i0);
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        if (i0) {goto block_86;}
        i0 = b;
        i1 = b;
        i1 = LoadU32(ctx, i1 + 4u);
        l3 = i1;
        i2 = 1u;
        i1 -= i2;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 != i1;
        l2 = i0;
        if (i0) {goto block_86;}
        i0 = b;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = LoadU32(ctx, i0 + 8u);
        l3 = i0;
        i0 = kF1033ConstD015C;
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
        i0 = l3;
        i1 = b;
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
        i0 = kF1033ConstD015C;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        i0 = kF1033ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_3;}
        i0 = b;
        i1 = b;
        i1 = LoadU32(ctx, i1 + 8u);
        l3 = i1;
        i2 = 1u;
        i1 -= i2;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 != i1;
        l2 = i0;
        if (i0) {goto block_86;}
        i0 = b;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = LoadU32(ctx, i0 + 12u);
        l3 = i0;
        i0 = kF1033ConstD015C;
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
        i0 = l3;
        i1 = b;
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
        i0 = kF1033ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF1033ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto block_3;}
      }
      block_86:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        i0 = LoadU32(ctx, i0 + 20u);
        b = i0;
        i0 = !(i0);
        l2 = i0;
        if (i0) {goto block_39;}
        i0 = b;
        i1 = b;
        i1 = LoadU32(ctx, i1 + 4u);
        l3 = i1;
        i2 = 1u;
        i1 -= i2;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 != i1;
        l2 = i0;
        if (i0) {goto block_39;}
        i0 = b;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = LoadU32(ctx, i0 + 8u);
        l3 = i0;
        i0 = kF1033ConstD015C;
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
        i0 = l3;
        i1 = b;
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
        i0 = kF1033ConstD015C;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        i0 = kF1033ConstD015C;
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
          i0 = b;
          i1 = b;
          i1 = LoadU32(ctx, i1 + 8u);
          l3 = i1;
          i2 = 1u;
          i1 -= i2;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {goto block_39;}
          i0 = b;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = LoadU32(ctx, i0 + 12u);
          l3 = i0;
          i0 = kF1033ConstD015C;
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
          i0 = l3;
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
          i0 = kF1033ConstD015C;
          i0 = LoadU32(ctx, i0);
          b = i0;
          i0 = kF1033ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = b;
          i1 = 1u;
          i0 = i0 != i1;
          b = i0;
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
      b = i0;
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
    b = i0;
    i0 = HostTempRet0(ctx);
    l2 = i0;
    goto block_35;
  }
  block_39:;
  i0 = b;
  i1 = a;
  i1 = !(i1);
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  b = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    if (i0) {goto block_102;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 4u);
    b = i1;
    i2 = 1u;
    i1 -= i2;
    l2 = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_102;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
    i0 = kF1033ConstD015C;
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
    i0 = b;
    i1 = a;
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
    i0 = kF1033ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF1033ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_3;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 8u);
    b = i1;
    i2 = 1u;
    i1 -= i2;
    l2 = i1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_102;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    b = i0;
    i0 = kF1033ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l6;
  i1 = 22u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = a;
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
    i0 = kF1033ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF1033ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_3;}
  }
  block_102:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = kF1033ConstFFFFFFC0;
    i0 -= i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_38:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    l2 = i0;
    goto block_34;
  }
  block_37:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
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
    if (i0) {goto block_112;}
    i0 = l2;
    i1 = kF1033ConstFFFFFFFE;
    i0 &= i1;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = !(i0);
    if (i0) {goto block_112;}
    i0 = kF1033ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    l8 = i0;
    i0 = l4;
    i1 = 4u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l6;
  i1 = 23u;
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
    i0 = 23u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1033ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF1033ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_111;}
  }
  block_112:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    goto block_36;
  }
  block_111:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_3;}
  block_36:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 36u);
    l4 = i0;
    i0 = !(i0);
    l2 = i0;
    if (i0) {goto block_35;}
    i0 = l4;
    i1 = l4;
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
    i0 = l4;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = kF1033ConstD015C;
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
    i1 = l4;
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
    i0 = kF1033ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF1033ConstD015C;
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
      i1 = l4;
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
      i0 = l4;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      l2 = i0;
      i0 = kF1033ConstD015C;
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
      i0 = l2;
      i1 = l4;
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
      i0 = kF1033ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF1033ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
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
    if (i0) {goto block_126;}
    i0 = l5;
    i1 = l5;
    i1 = LoadU32(ctx, i1 + 4u);
    l4 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
    l2 = i0;
    if (i0) {goto block_126;}
    i0 = l5;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
    i0 = kF1033ConstD015C;
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
    i0 = l4;
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
    i0 = kF1033ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF1033ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_3;}
    i0 = l5;
    i1 = l5;
    i1 = LoadU32(ctx, i1 + 8u);
    l4 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
    l2 = i0;
    if (i0) {goto block_126;}
    i0 = l5;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = kF1033ConstD015C;
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
    i0 = l4;
    i1 = l5;
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
    i0 = kF1033ConstD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF1033ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_3;}
  }
  block_126:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l9;
    i0 = !(i0);
    l2 = i0;
    if (i0) {goto block_34;}
    i0 = l3;
    DeallocateRawBlock(ctx, i0);
  }
  block_34:;
  i0 = l2;
  i1 = a;
  i1 = !(i1);
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l2 = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {goto block_133;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 4u);
    l5 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_133;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    l5 = i0;
    i0 = kF1033ConstD015C;
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
    i0 = l5;
    i1 = a;
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
    i0 = kF1033ConstD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF1033ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_3;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 8u);
    l5 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_133;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    l5 = i0;
    i0 = kF1033ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l6;
  i1 = 29u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l5;
    i1 = a;
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
    i0 = kF1033ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF1033ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_3;}
  }
  block_133:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    HostOsRelease(ctx, i0);
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
  i0 = l6;
  i1 = 30u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 30u;
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
  i2 = kF1033ResumeWordBytes;
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
  i1 = l10;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 36u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
