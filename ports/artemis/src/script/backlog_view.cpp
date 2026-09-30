// backlog_view.cpp -- the second dispatch unit of the backlog view: one suspendable step of the backlog tag family, transcribed call for call..
//
// BacklogView::DispatchAux -- the second dispatch unit of the backlog view: one suspendable step of the backlog tag family, transcribed call for call.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 422500-422500 (f1042); R1 transcribe-keep.
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
uint32_t CopyBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void ReleaseNodeTree(EngineContext* ctx, uint32_t state, uint32_t node);
uint32_t FindOrderedChainEntry(EngineContext* ctx, uint32_t a0, uint32_t a1);
void ReleaseLayerNodeTree(EngineContext* ctx, uint32_t a0, uint32_t a1);
void ReleaseNoSaveNodeTree(EngineContext* ctx, uint32_t a0, uint32_t a1);
void ReleaseNestedNodePair(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t DestroyNodeObject(EngineContext* ctx, uint32_t a0);
void ReleaseNodeListTail(EngineContext* ctx, uint32_t a0, uint32_t a1);
void UnlinkOrderedChainNode(EngineContext* ctx, uint32_t a0);
void ReleaseShallowNodePair(EngineContext* ctx, uint32_t a0, uint32_t a1);
void ClearLinkedNodeFields(EngineContext* ctx, uint32_t a0, uint32_t a1);
void ReleaseTaggedNodeTriple(EngineContext* ctx, uint32_t a0, uint32_t a1);
void InitNodeStorage(EngineContext* ctx, uint32_t a0);
void ReleaseTaggedChildNodes(EngineContext* ctx, uint32_t a0, uint32_t a1);
void ReleaseAssertedNodeTree(EngineContext* ctx, uint32_t a0, uint32_t a1);
void ReleaseSubtreeTaggedNodes(EngineContext* ctx, uint32_t a0, uint32_t a1);
void ReleasePlainNodeTree(EngineContext* ctx, uint32_t a0, uint32_t a1);
void ReleaseNestedTaggedNodes(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t ClearStructFields(EngineContext* ctx, uint32_t a0);
void InitLayerStorage(EngineContext* ctx, uint32_t a0);

namespace {

constexpr uint32_t kF1042Const8ADC0 = 0x8ADC0u;  // 
constexpr uint32_t kF1042Const8AE54 = 0x8AE54u;  // 
constexpr uint32_t kF1042MsgBuckets = 0x8AE58u;  // buckets_
constexpr uint32_t kF1042MsgRclickIet = 0xAF4DBu;  // rclick.iet
constexpr uint32_t kF1042MsgBacklog = 0xAF4E6u;  // backlog
constexpr uint32_t kF1042ConstAF4E9 = 0xAF4E9u;  // 
constexpr uint32_t kF1042MsgMacroIet = 0xAF99Bu;  // macro.iet
constexpr uint32_t kF1042ConstAF9A3 = 0xAF9A3u;  // 
constexpr uint32_t kF1042MsgIniDebugmode = 0xAF9A5u;  // ini.DebugMode
constexpr uint32_t kF1042ConstAF9AA = 0xAF9AAu;  // 
constexpr uint32_t kF1042MsgIniDebuglevel = 0xAF9B3u;  // ini.DebugLevel
constexpr uint32_t kF1042ConstAF9B9 = 0xAF9B9u;  // 
constexpr uint32_t kF1042MsgIniAsyncifyhashlisturl = 0xAF9C2u;  // ini.AsyncifyHashListURL
constexpr uint32_t kF1042ConstAF9CA = 0xAF9CAu;  // 
constexpr uint32_t kF1042ConstAF9D1 = 0xAF9D1u;  // 
constexpr uint32_t kF1042MsgSSevol = 0xAF9DAu;  // s.sevol
constexpr uint32_t kF1042ConstAF9DD = 0xAF9DDu;  // 
constexpr uint32_t kF1042MsgSVideovol = 0xAF9E2u;  // s.videovol
constexpr uint32_t kF1042ConstAF9EA = 0xAF9EAu;  // 
constexpr uint32_t kF1042MsgSAutomodewait = 0xAF9EDu;  // s.automodewait
constexpr uint32_t kF1042ConstAF9F3 = 0xAF9F3u;  // 
constexpr uint32_t kF1042MsgSavegDat = 0xAF9FCu;  // saveg.dat
constexpr uint32_t kF1042ConstAFA04 = 0xAFA04u;  // 
constexpr uint32_t kF1042MsgSystemDat = 0xAFA06u;  // system.dat
constexpr uint32_t kF1042ConstAFA0E = 0xAFA0Eu;  // 
constexpr uint32_t kF1042ConstB5C94 = 0xB5C94u;  // 
constexpr uint32_t kF1042ConstB5E30 = 0xB5E30u;  // 
constexpr uint32_t kF1042MsgPx = 0xB5E31u;  // px != 0
constexpr uint32_t kF1042MsgBoostBoostSmartPtr = 0xB5E39u;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
constexpr uint32_t kF1042MsgOperator = 0xB5E71u;  // operator->
constexpr uint32_t kF1042ConstB5E84 = 0xB5E84u;  // 
constexpr uint32_t kF1042ConstB5EC4 = 0xB5EC4u;  // 
constexpr uint32_t kF1042ConstB5F40 = 0xB5F40u;  // 
constexpr uint32_t kF1042ConstCF11C = 0xCF11Cu;  // 
constexpr uint32_t kF1042ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF1042ConstFFFFFFC0 = 0xFFFFFFC0u;  // 
constexpr uint32_t kF1042ConstFFFFFFFE = 0xFFFFFFFEu;  // 
constexpr uint32_t kF1042ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF1042SaveFrameBytes = 36u;
constexpr uint32_t kF1042ResumeWordBytes = 4u;

}  // namespace

namespace BacklogView {
void DispatchAux(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u; uint32_t i6 = 0u;
  uint64_t j1 = 0;
      l10 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1042SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l8 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 24u);
    l6 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 28u);
    l9 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 32u);
    l10 = i0;
    i0 = l8;
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
    i2 = kF1042ResumeWordBytes;
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
    i1 = 272u;
    i0 -= i1;
    l3 = i0;
    GuestStackSp(ctx) = i0;
    i0 = l3;
    i1 = 0u;
    StoreU32(ctx, i0 + 248u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 240u, j1);
    i0 = kF1042ConstCF11C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
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
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 28u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l8 = i0;
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l5 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_12;}
      i0 = l5;
      i1 = 0u;
      StoreU8(ctx, i0 + 4u, i1);
      i0 = l5;
      i1 = kF1042ConstB5C94;
      StoreU32(ctx, i0, i1);
      i0 = 38u;
      l2 = i0;
      i0 = kF1042Const8ADC0;
      l4 = i0;
      loop_19: 
        i0 = l2;
        i1 = 1u;
        i0 >>= (i1 & 31);
        l6 = i0;
        i1 = 2u;
        i0 <<= (i1 & 31);
        i1 = l4;
        i0 += i1;
        l9 = i0;
        i1 = kF1042ResumeWordBytes;
        i0 += i1;
        i1 = l4;
        i2 = l9;
        i2 = LoadU32(ctx, i2);
        i3 = 11u;
        i2 = i2 < i3;
        l9 = i2;
        i0 = i2 ? i0 : i1;
        l4 = i0;
        i0 = l6;
        i1 = kF1042ConstFFFFFFFF;
        i0 ^= i1;
        l10 = i0;
        i1 = l2;
        i0 += i1;
        i1 = l6;
        i2 = l9;
        i0 = i2 ? i0 : i1;
        l2 = i0;
        if (i0) {goto loop_19;}
      i0 = kF1042Const8AE54;
      i1 = l4;
      i2 = l4;
      i3 = kF1042MsgBuckets;
      i2 = i2 == i3;
      i0 = i2 ? i0 : i1;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = l5;
      j1 = 0ull;
      StoreU64(ctx, i0 + 20u, j1);
      i0 = l5;
      j1 = 4575657221408423936ull;
      StoreU64(ctx, i0 + 12u, j1);
      i0 = l5;
      i1 = l2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = kF1042ConstCF11C;
      i1 = l5;
      StoreU32(ctx, i0, i1);
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 232u, i1);
    i0 = l3;
    j1 = 8390046960851709545ull;
    StoreU64(ctx, i0 + 224u, j1);
    i0 = l3;
    i1 = 8u;
    StoreU8(ctx, i0 + 235u, i1);
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 224u;
    i0 += i1;
    l4 = i0;
    i0 = l3;
    i1 = 56u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l7;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2454u;
    i1 = l2;
    i2 = l5;
    i3 = l4;
    i4 = 0u;
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
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF1042ConstD015C;
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
      i0 = LoadS8(ctx, i0 + 251u);
      i1 = kF1042ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 240u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l3;
      i1 = l3;
      i2 = kF1042ConstFFFFFFC0;
      i1 -= i2;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0 + 248u, i1);
      i0 = l3;
      i1 = l3;
      j1 = LoadU64(ctx, i1 + 56u);
      StoreU64(ctx, i0 + 240u, j1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 56u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 67u, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 235u);
      i1 = kF1042ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l2 = i0;
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 224u);
        l2 = i0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = kF1042ConstD015C;
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
      i0 = 2712u;
      i1 = a;
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
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_11;}
      i0 = kF1042ConstD015C;
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
      i0 = 2626u;
      i1 = a;
      i2 = 0u;
      i3 = 0u;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 3u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_11;}
      i0 = a;
      i0 = LoadU32(ctx, i0 + 8u);
      l2 = i0;
      i0 = !(i0);
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
        i0 = kF1042ConstD015C;
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
        i0 = 857u;
        i1 = kF1042MsgPx;
        i2 = kF1042MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF1042MsgOperator;
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
        i0 = kF1042ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF1042ConstD015C;
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
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 28u);
      l4 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 5u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l4;
      i1 = l2;
      i2 = 0u;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 5u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_11;}
      i0 = a;
      i1 = 1u;
      StoreU8(ctx, i0 + 48u, i1);
      i0 = a;
      i1 = 0u;
      StoreU8(ctx, i0 + 34u, i1);
      i0 = a;
      i1 = 36u;
      i0 += i1;
      l4 = i0;
      i1 = a;
      i2 = 40u;
      i1 += i2;
      l2 = i1;
      i1 = LoadU32(ctx, i1);
      l5 = i1;
      ReleaseTaggedNodeTriple(ctx, i0, i1);
      i0 = a;
      i1 = l2;
      StoreU32(ctx, i0 + 36u, i1);
      i0 = a;
      j1 = 0ull;
      StoreU64(ctx, i0 + 40u, j1);
      i0 = kF1042ConstD015C;
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
      i0 = 1032u;
      i1 = 48u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l8 = i0;
      i0 = 6u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_11;}
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 7u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2457u;
      i1 = l4;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l8 = i0;
      i0 = 7u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_35;}
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 76u;
      i0 += i1;
      l4 = i0;
    }
    i0 = l7;
    i1 = 8u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2713u;
      i1 = l4;
      i2 = l2;
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
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_11;}
      i0 = a;
      i0 = LoadU32(ctx, i0 + 152u);
      l2 = i0;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 148u);
      l5 = i1;
      i0 = i0 != i1;
      l4 = i0;
    }
    i0 = l4;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      loop_62: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          i1 = 16u;
          i0 -= i1;
          l4 = i0;
          i0 = LoadU32(ctx, i0);
          l6 = i0;
        }
        i0 = l6;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = l9;
          i1 = l6;
          i2 = 1u;
          i1 &= i2;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          l9 = i0;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l9;
            if (i0) {goto block_66;}
            i0 = l6;
            i1 = kF1042ConstFFFFFFFE;
            i0 &= i1;
            i0 = LoadU32(ctx, i0);
            l6 = i0;
            i0 = !(i0);
            l9 = i0;
            if (i0) {goto block_66;}
            i0 = kF1042ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = 12u;
            i0 -= i1;
            l2 = i0;
          }
          i0 = l7;
          i1 = 9u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l6;
            i1 = l2;
            i2 = l2;
            i3 = 2u;
            HostSyncSlot(ctx, i0, i1, i2, i3);
            i0 = 9u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF1042ConstD015C;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = kF1042ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 == i1;
            l2 = i0;
            if (i0) {goto block_3;}
          }
          block_66:;
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
          i1 = l4;
          l2 = i1;
          i0 = i0 != i1;
          l4 = i0;
          if (i0) {goto loop_62;}
        }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = l5;
      StoreU32(ctx, i0 + 152u, i1);
      i0 = a;
      i1 = 2724u;
      i0 += i1;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 2720u);
      l5 = i1;
      i0 = i0 != i1;
      if (i0) {
        loop_74: 
          i0 = l2;
          i1 = 12u;
          i0 -= i1;
          l4 = i0;
          i0 = l2;
          i1 = 1u;
          i0 -= i1;
          i0 = LoadS8(ctx, i0);
          i1 = kF1042ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l5;
          i1 = l4;
          l2 = i1;
          i0 = i0 != i1;
          if (i0) {goto loop_74;}
      }
      i0 = a;
      i1 = l5;
      StoreU32(ctx, i0 + 2724u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 65u, i1);
      i0 = l3;
      i1 = kF1042ConstFFFFFFC0;
      i0 -= i1;
      l2 = i0;
      i1 = kF1042ConstAF9A3;
      i1 = LoadU8(ctx, i1);
      StoreU8(ctx, i0, i1);
      i0 = l3;
      i1 = 9u;
      StoreU8(ctx, i0 + 67u, i1);
      i0 = l3;
      i1 = kF1042MsgMacroIet;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0 + 56u, j1);
      i0 = l5;
      i1 = a;
      i2 = 2728u;
      i1 += i2;
      i1 = LoadU32(ctx, i1);
      i0 = i0 < i1;
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      if (i0) {
        i0 = l5;
        i1 = l3;
        j1 = LoadU64(ctx, i1 + 56u);
        StoreU64(ctx, i0, j1);
        i0 = l5;
        i1 = l2;
        i1 = LoadU32(ctx, i1);
        StoreU32(ctx, i0 + 8u, i1);
        i0 = a;
        i1 = l5;
        i2 = 12u;
        i1 += i2;
        l2 = i1;
        StoreU32(ctx, i0 + 2724u, i1);
        goto block_76;
      }
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 56u;
      i0 += i1;
      l4 = i0;
      i0 = a;
      i1 = 2720u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l7;
    i1 = 10u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1412u;
      i1 = l2;
      i2 = l4;
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
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_34;}
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 67u);
      i1 = kF1042ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      l2 = i0;
      if (i0) {goto block_76;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 56u);
      l2 = i0;
      DeallocateRawBlock(ctx, i0);
    }
    block_76:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = 1u;
      StoreU16(ctx, i0 + 2744u, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 68u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 76u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 84u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 92u, j1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 100u, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 112u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 120u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 128u, j1);
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 136u, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 144u, j1);
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 152u, i1);
      i0 = l3;
      i1 = 160u;
      i0 += i1;
      l2 = i0;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l3;
      i1 = l2;
      StoreU32(ctx, i0 + 156u, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 184u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 176u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 168u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 60u, j1);
      i0 = l3;
      i1 = kF1042ConstB5E84;
      StoreU32(ctx, i0 + 56u, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 104u, j1);
      i0 = l3;
      i1 = kF1042ConstB5F40;
      StoreU32(ctx, i0 + 140u, i1);
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 56u;
      i0 += i1;
      l4 = i0;
      i0 = a;
      i1 = 2748u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l7;
    i1 = 11u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2704u;
      i1 = l2;
      i2 = l4;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l8 = i0;
      i0 = 11u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_33;}
      i0 = l3;
      i1 = 56u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l7;
    i1 = 12u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i0 = DestroyNodeObject(ctx, i0);
      l8 = i0;
      i0 = 12u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 240u;
      i0 += i1;
      l4 = i0;
      i0 = a;
      i1 = 2752u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l7;
    i1 = 13u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 870u;
      i1 = l2;
      i2 = l4;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l8 = i0;
      i0 = 13u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_11;}
      i0 = a;
      i1 = 2884u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l7;
    i1 = 14u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      InitLayerStorage(ctx, i0);
      i0 = 14u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = l2;
    i1 = a;
    i2 = 2908u;
    i1 += i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l2 = i0;
    i0 = l7;
    i1 = 15u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      InitNodeStorage(ctx, i0);
      i0 = 15u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = 0u;
      StoreU32(ctx, i0 + 2932u, i1);
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 2936u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l7;
    i1 = 16u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 996u;
      i1 = l2;
      i2 = kF1042MsgRclickIet;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l8 = i0;
      i0 = 16u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_11;}
      i0 = a;
      i1 = 0u;
      StoreU32(ctx, i0 + 13212u, i1);
      i0 = a;
      j1 = 0ull;
      StoreU64(ctx, i0 + 2952u, j1);
      i0 = a;
      i1 = 0u;
      StoreU8(ctx, i0 + 2948u, i1);
      i0 = kF1042ConstCF11C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
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
        i0 = kF1042ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 17u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1032u;
        i1 = 28u;
        i0 = HostFoldLayoutOffset(ctx, i0, i1);
        l8 = i0;
        i0 = 17u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l8;
        l5 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF1042ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF1042ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        l2 = i0;
        if (i0) {goto block_11;}
        i0 = l5;
        i1 = 0u;
        StoreU8(ctx, i0 + 4u, i1);
        i0 = l5;
        i1 = kF1042ConstB5C94;
        StoreU32(ctx, i0, i1);
        i0 = 38u;
        l2 = i0;
        i0 = kF1042Const8ADC0;
        l4 = i0;
        loop_98: 
          i0 = l2;
          i1 = 1u;
          i0 >>= (i1 & 31);
          l6 = i0;
          i1 = 2u;
          i0 <<= (i1 & 31);
          i1 = l4;
          i0 += i1;
          l9 = i0;
          i1 = kF1042ResumeWordBytes;
          i0 += i1;
          i1 = l4;
          i2 = l9;
          i2 = LoadU32(ctx, i2);
          i3 = 11u;
          i2 = i2 < i3;
          l9 = i2;
          i0 = i2 ? i0 : i1;
          l4 = i0;
          i0 = l6;
          i1 = kF1042ConstFFFFFFFF;
          i0 ^= i1;
          l10 = i0;
          i1 = l2;
          i0 += i1;
          i1 = l6;
          i2 = l9;
          i0 = i2 ? i0 : i1;
          l2 = i0;
          if (i0) {goto loop_98;}
        i0 = kF1042Const8AE54;
        i1 = l4;
        i2 = l4;
        i3 = kF1042MsgBuckets;
        i2 = i2 == i3;
        i0 = i2 ? i0 : i1;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = l5;
        j1 = 0ull;
        StoreU64(ctx, i0 + 20u, j1);
        i0 = l5;
        j1 = 4575657221408423936ull;
        StoreU64(ctx, i0 + 12u, j1);
        i0 = l5;
        i1 = l2;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = kF1042ConstCF11C;
        i1 = l5;
        StoreU32(ctx, i0, i1);
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 18u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 16u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l8 = i0;
      i0 = 18u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      l4 = i0;
      if (i0) {goto block_32;}
      i0 = l3;
      i1 = l2;
      StoreU32(ctx, i0 + 56u, i1);
      i0 = l3;
      j1 = 9223372105574252557ull;
      StoreU64(ctx, i0 + 60u, j1);
      i0 = l2;
      i1 = 0u;
      StoreU8(ctx, i0 + 13u, i1);
      i0 = l2;
      i1 = kF1042ConstAF9AA;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0 + 5u, j1);
      i0 = l2;
      i1 = kF1042MsgIniDebugmode;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 56u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l7;
    i1 = 19u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2714u;
      i1 = l5;
      i2 = l2;
      i3 = 0u;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l8 = i0;
      i0 = 19u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      l4 = i0;
      if (i0) {goto block_31;}
      i0 = a;
      i1 = l2;
      StoreU32(ctx, i0 + 13216u, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 67u);
      i1 = kF1042ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 56u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = kF1042ConstCF11C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
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
        i0 = kF1042ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 20u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1032u;
        i1 = 28u;
        i0 = HostFoldLayoutOffset(ctx, i0, i1);
        l8 = i0;
        i0 = 20u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l8;
        l5 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF1042ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF1042ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        l2 = i0;
        if (i0) {goto block_11;}
        i0 = l5;
        i1 = 0u;
        StoreU8(ctx, i0 + 4u, i1);
        i0 = l5;
        i1 = kF1042ConstB5C94;
        StoreU32(ctx, i0, i1);
        i0 = 38u;
        l2 = i0;
        i0 = kF1042Const8ADC0;
        l4 = i0;
        loop_109: 
          i0 = l2;
          i1 = 1u;
          i0 >>= (i1 & 31);
          l6 = i0;
          i1 = 2u;
          i0 <<= (i1 & 31);
          i1 = l4;
          i0 += i1;
          l9 = i0;
          i1 = kF1042ResumeWordBytes;
          i0 += i1;
          i1 = l4;
          i2 = l9;
          i2 = LoadU32(ctx, i2);
          i3 = 11u;
          i2 = i2 < i3;
          l9 = i2;
          i0 = i2 ? i0 : i1;
          l4 = i0;
          i0 = l6;
          i1 = kF1042ConstFFFFFFFF;
          i0 ^= i1;
          l10 = i0;
          i1 = l2;
          i0 += i1;
          i1 = l6;
          i2 = l9;
          i0 = i2 ? i0 : i1;
          l2 = i0;
          if (i0) {goto loop_109;}
        i0 = kF1042Const8AE54;
        i1 = l4;
        i2 = l4;
        i3 = kF1042MsgBuckets;
        i2 = i2 == i3;
        i0 = i2 ? i0 : i1;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = l5;
        j1 = 0ull;
        StoreU64(ctx, i0 + 20u, j1);
        i0 = l5;
        j1 = 4575657221408423936ull;
        StoreU64(ctx, i0 + 12u, j1);
        i0 = l5;
        i1 = l2;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = kF1042ConstCF11C;
        i1 = l5;
        StoreU32(ctx, i0, i1);
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 21u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 16u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l8 = i0;
      i0 = 21u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      l4 = i0;
      if (i0) {goto block_29;}
      i0 = l3;
      i1 = l2;
      StoreU32(ctx, i0 + 56u, i1);
      i0 = l3;
      j1 = 9223372105574252558ull;
      StoreU64(ctx, i0 + 60u, j1);
      i0 = l2;
      i1 = 0u;
      StoreU8(ctx, i0 + 14u, i1);
      i0 = l2;
      i1 = kF1042ConstAF9B9;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0 + 6u, j1);
      i0 = l2;
      i1 = kF1042MsgIniDebuglevel;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 56u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l7;
    i1 = 22u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2714u;
      i1 = l5;
      i2 = l2;
      i3 = 0u;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l8 = i0;
      i0 = 22u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      l4 = i0;
      if (i0) {goto block_28;}
      i0 = a;
      i1 = l2;
      StoreU32(ctx, i0 + 13220u, i1);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 67u);
      i1 = kF1042ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 56u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = a;
      i1 = 257u;
      StoreU16(ctx, i0 + 13272u, i1);
      i0 = a;
      i1 = 13280u;
      i0 += i1;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 13276u);
      l5 = i1;
      i0 = i0 != i1;
      if (i0) {
        loop_117: 
          i0 = l2;
          i1 = 12u;
          i0 -= i1;
          l4 = i0;
          i0 = l2;
          i1 = 1u;
          i0 -= i1;
          i0 = LoadS8(ctx, i0);
          i1 = kF1042ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l5;
          i1 = l4;
          l2 = i1;
          i0 = i0 != i1;
          if (i0) {goto loop_117;}
      }
      i0 = a;
      i1 = l5;
      StoreU32(ctx, i0 + 13280u, i1);
      i0 = a;
      i1 = 13303u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF1042ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 13292u);
        i1 = 0u;
        StoreU8(ctx, i0, i1);
        i0 = a;
        i1 = 13296u;
        i0 += i1;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        goto block_119;
      }
      i0 = a;
      i1 = 0u;
      StoreU8(ctx, i0 + 13303u, i1);
      i0 = a;
      i1 = 0u;
      StoreU8(ctx, i0 + 13292u, i1);
      block_119:;
      i0 = a;
      i1 = 13319u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF1042ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 13308u);
        i1 = 0u;
        StoreU8(ctx, i0, i1);
        i0 = a;
        i1 = 13312u;
        i0 += i1;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        goto block_121;
      }
      i0 = a;
      i1 = 0u;
      StoreU8(ctx, i0 + 13319u, i1);
      i0 = a;
      i1 = 0u;
      StoreU8(ctx, i0 + 13308u, i1);
      block_121:;
      i0 = a;
      i1 = 13320u;
      i0 += i1;
      UnlinkOrderedChainNode(ctx, i0);
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 56u;
      i0 += i1;
      i1 = 4u;
      i0 |= i1;
      l2 = i0;
      i0 = ClearStructFields(ctx, i0);
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      l4 = i0;
      if (i0) {goto block_26;}
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 220u, i1);
      i0 = l3;
      i1 = kF1042ConstB5EC4;
      StoreU32(ctx, i0 + 56u, i1);
      i0 = a;
      i1 = 13348u;
      i0 += i1;
      i1 = l2;
      i2 = 68u;
      i0 = CopyBytes(ctx, i0, i1, i2);
      i0 = a;
      i1 = 13416u;
      i0 += i1;
      l2 = i0;
      i0 = a;
      i1 = 13427u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF1042ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l2;
      i1 = l3;
      j1 = LoadU64(ctx, i1 + 128u);
      StoreU64(ctx, i0, j1);
      i0 = l2;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 136u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 128u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 139u, i1);
      i0 = a;
      i1 = 13428u;
      i0 += i1;
      l2 = i0;
      i0 = 0u;
      l4 = i0;
      i0 = a;
      i1 = 13439u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF1042ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
        i0 = l3;
        i0 = LoadU8(ctx, i0 + 139u);
        l4 = i0;
      }
      i0 = l2;
      i1 = l3;
      j1 = LoadU64(ctx, i1 + 140u);
      StoreU64(ctx, i0, j1);
      i0 = l2;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 148u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 140u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 151u, i1);
      i0 = a;
      i1 = 13440u;
      i0 += i1;
      i1 = l3;
      i2 = 152u;
      i1 += i2;
      i2 = 66u;
      i0 = CopyBytes(ctx, i0, i1, i2);
      i0 = a;
      i1 = 13508u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 220u);
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = kF1042ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 128u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = a;
      i1 = 13516u;
      i0 += i1;
      l4 = i0;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = a;
      i1 = 13512u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l7;
    i1 = 23u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = l5;
      ReleaseNodeListTail(ctx, i0, i1);
      i0 = 23u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = l4;
      StoreU32(ctx, i0 + 13512u, i1);
      i0 = l4;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 56u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 67u, i1);
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = l3;
      i2 = 56u;
      i1 += i2;
      StoreU32(ctx, i0 + 264u, i1);
      i0 = l3;
      i1 = 56u;
      i0 += i1;
      l5 = i0;
      i0 = l3;
      i1 = 264u;
      i0 += i1;
      l6 = i0;
      i0 = l3;
      i1 = 256u;
      i0 += i1;
      l9 = i0;
      i0 = l3;
      i1 = 224u;
      i0 += i1;
      l4 = i0;
    }
    i0 = l7;
    i1 = 24u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2715u;
      i1 = l4;
      i2 = l2;
      i3 = l5;
      i4 = kF1042ConstB5E30;
      i5 = l6;
      i6 = l9;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 24u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      l4 = i0;
      if (i0) {goto block_25;}
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 224u);
      i1 = 28u;
      i0 += i1;
      l5 = i0;
      i0 = a;
      i1 = 13344u;
      i0 += i1;
      l4 = i0;
    }
    i0 = l7;
    i1 = 25u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2617u;
      i1 = l5;
      i2 = l4;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 25u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_25;}
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 67u);
      i1 = kF1042ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 56u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l3;
      i1 = 7u;
      StoreU8(ctx, i0 + 67u, i1);
      i0 = l3;
      i1 = kF1042MsgBacklog;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0 + 56u, i1);
      i0 = l3;
      i1 = kF1042ConstAF4E9;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0 + 59u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 63u, i1);
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = l3;
      i2 = 56u;
      i1 += i2;
      StoreU32(ctx, i0 + 264u, i1);
      i0 = l3;
      i1 = 56u;
      i0 += i1;
      l6 = i0;
      i0 = l3;
      i1 = 264u;
      i0 += i1;
      l9 = i0;
      i0 = l3;
      i1 = 256u;
      i0 += i1;
      l10 = i0;
      i0 = l3;
      i1 = 224u;
      i0 += i1;
      l5 = i0;
    }
    i0 = l7;
    i1 = 26u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2715u;
      i1 = l5;
      i2 = l2;
      i3 = l6;
      i4 = kF1042ConstB5E30;
      i5 = l9;
      i6 = l10;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 26u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_24;}
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 224u);
      i1 = 28u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l7;
    i1 = 27u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2617u;
      i1 = l2;
      i2 = l4;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 27u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_24;}
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 67u);
      i1 = kF1042ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 56u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = a;
      i1 = 13536u;
      i0 += i1;
      i1 = a;
      i2 = 13540u;
      i1 += i2;
      l2 = i1;
      i1 = LoadU32(ctx, i1);
      ReleaseNestedNodePair(ctx, i0, i1);
      i0 = a;
      i1 = l2;
      StoreU32(ctx, i0 + 13536u, i1);
      i0 = l2;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = a;
      i1 = 13548u;
      i0 += i1;
      i1 = a;
      i2 = 13552u;
      i1 += i2;
      l2 = i1;
      i1 = LoadU32(ctx, i1);
      ReleaseNestedNodePair(ctx, i0, i1);
      i0 = a;
      i1 = l2;
      StoreU32(ctx, i0 + 13548u, i1);
      i0 = l2;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = a;
      i1 = 13560u;
      i0 += i1;
      i1 = a;
      i2 = 13564u;
      i1 += i2;
      l2 = i1;
      i1 = LoadU32(ctx, i1);
      ReleaseNestedNodePair(ctx, i0, i1);
      i0 = a;
      i1 = l2;
      StoreU32(ctx, i0 + 13560u, i1);
      i0 = l2;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = a;
      i1 = 13572u;
      i0 += i1;
      i1 = a;
      i2 = 13576u;
      i1 += i2;
      l2 = i1;
      i1 = LoadU32(ctx, i1);
      ReleaseNestedNodePair(ctx, i0, i1);
      i0 = a;
      i1 = 0u;
      StoreU8(ctx, i0 + 13584u, i1);
      i0 = a;
      i1 = l2;
      StoreU32(ctx, i0 + 13572u, i1);
      i0 = l2;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = a;
      i1 = 13599u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF1042ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 13588u);
        i1 = 0u;
        StoreU8(ctx, i0, i1);
        i0 = a;
        i1 = 13592u;
        i0 += i1;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        goto block_138;
      }
      i0 = a;
      i1 = 0u;
      StoreU8(ctx, i0 + 13599u, i1);
      i0 = a;
      i1 = 0u;
      StoreU8(ctx, i0 + 13588u, i1);
      block_138:;
      i0 = a;
      i1 = 13611u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF1042ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 13600u);
        i1 = 0u;
        StoreU8(ctx, i0, i1);
        i0 = a;
        i1 = 13604u;
        i0 += i1;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        goto block_140;
      }
      i0 = a;
      i1 = 0u;
      StoreU8(ctx, i0 + 13611u, i1);
      i0 = a;
      i1 = 0u;
      StoreU8(ctx, i0 + 13600u, i1);
      block_140:;
      i0 = a;
      i1 = 0u;
      StoreU8(ctx, i0 + 13612u, i1);
      i0 = a;
      i1 = 13616u;
      i0 += i1;
      l4 = i0;
      i1 = a;
      i2 = 13620u;
      i1 += i2;
      l2 = i1;
      i1 = LoadU32(ctx, i1);
      l5 = i1;
      ReleaseLayerNodeTree(ctx, i0, i1);
      i0 = a;
      i1 = l2;
      StoreU32(ctx, i0 + 13616u, i1);
      i0 = l2;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 13628u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l7;
    i1 = 28u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 996u;
      i1 = l2;
      i2 = kF1042MsgBacklog;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l8 = i0;
      i0 = 28u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_11;}
      i0 = a;
      i1 = 0u;
      StoreU8(ctx, i0 + 13642u, i1);
      i0 = a;
      i1 = 257u;
      StoreU16(ctx, i0 + 13640u, i1);
      i0 = a;
      i1 = 13644u;
      i0 += i1;
      i1 = a;
      i2 = 13648u;
      i1 += i2;
      l2 = i1;
      i1 = LoadU32(ctx, i1);
      ReleaseLayerNodeTree(ctx, i0, i1);
      i0 = a;
      i1 = l2;
      StoreU32(ctx, i0 + 13644u, i1);
      i0 = l2;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = a;
      i1 = 13656u;
      i0 += i1;
      i1 = a;
      i2 = 13660u;
      i1 += i2;
      l2 = i1;
      i1 = LoadU32(ctx, i1);
      ReleaseTaggedNodeTriple(ctx, i0, i1);
      i0 = a;
      i1 = l2;
      StoreU32(ctx, i0 + 13656u, i1);
      i0 = l2;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = a;
      i1 = 13668u;
      i0 += i1;
      l4 = i0;
      i1 = a;
      i2 = 13672u;
      i1 += i2;
      l2 = i1;
      i1 = LoadU32(ctx, i1);
      l5 = i1;
      ReleaseNestedTaggedNodes(ctx, i0, i1);
      i0 = a;
      i1 = l2;
      StoreU32(ctx, i0 + 13668u, i1);
      i0 = l2;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = a;
      i0 = LoadU32(ctx, i0 + 13700u);
      l2 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 29u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1776u;
      i1 = l2;
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
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_11;}
      i0 = a;
      i1 = 0u;
      StoreU32(ctx, i0 + 13700u, i1);
      i0 = a;
      i0 = LoadU32(ctx, i0 + 13704u);
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
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 4u);
        l4 = i0;
      }
      i0 = l7;
      i1 = 30u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l4;
        CallTableT1(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 30u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i1 = 0u;
        StoreU32(ctx, i0 + 13704u, i1);
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = 13708u;
      i0 += i1;
      i1 = a;
      i2 = 13712u;
      i1 += i2;
      l2 = i1;
      i1 = LoadU32(ctx, i1);
      ReleaseLayerNodeTree(ctx, i0, i1);
      i0 = a;
      j1 = 0ull;
      StoreU64(ctx, i0 + 13728u, j1);
      i0 = a;
      i1 = l2;
      StoreU32(ctx, i0 + 13708u, i1);
      i0 = l2;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = a;
      i1 = 13740u;
      i0 += i1;
      i1 = a;
      i2 = 13744u;
      i1 += i2;
      l2 = i1;
      i1 = LoadU32(ctx, i1);
      ReleaseShallowNodePair(ctx, i0, i1);
      i0 = a;
      i1 = l2;
      StoreU32(ctx, i0 + 13740u, i1);
      i0 = l2;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = a;
      i1 = 13752u;
      i0 += i1;
      i1 = a;
      i2 = 13756u;
      i1 += i2;
      l2 = i1;
      i1 = LoadU32(ctx, i1);
      ReleaseShallowNodePair(ctx, i0, i1);
      i0 = a;
      i1 = l2;
      StoreU32(ctx, i0 + 13752u, i1);
      i0 = l2;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 68u, i1);
      i0 = l3;
      i1 = 76u;
      i0 += i1;
      l5 = i0;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 92u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 60u, j1);
      i0 = l3;
      i1 = kF1042ConstB5F40;
      StoreU32(ctx, i0 + 56u, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 84u, j1);
      i0 = l3;
      i1 = l5;
      StoreU32(ctx, i0 + 72u, i1);
      i0 = a;
      i1 = 13768u;
      i0 += i1;
      l2 = i0;
      i0 = a;
      i1 = 13779u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF1042ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l2;
      i1 = l3;
      i2 = 56u;
      i1 += i2;
      i2 = 4u;
      i1 |= i2;
      l4 = i1;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = l2;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 60u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 71u, i1);
      i0 = a;
      i1 = 13780u;
      i0 += i1;
      l6 = i0;
      i1 = a;
      i2 = 13784u;
      i1 += i2;
      l2 = i1;
      i1 = LoadU32(ctx, i1);
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l6;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 72u);
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 76u);
      l4 = i1;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 13788u;
      i0 += i1;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 80u);
      l9 = i1;
      StoreU32(ctx, i0, i1);
      i0 = l9;
      if (i0) {goto block_23;}
      i0 = l6;
      i1 = l2;
      StoreU32(ctx, i0, i1);
      goto block_22;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 235u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    a = i0;
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 224u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
    goto block_6;
  }
  block_35:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    i0 = l4;
    DeallocateRawBlock(ctx, i0);
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
    goto block_6;
  }
  block_34:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 67u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    a = i0;
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 56u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
    goto block_6;
  }
  block_33:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i1 = 56u;
    i0 += i1;
    a = i0;
  }
  i0 = l7;
  i1 = 31u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i0 = DestroyNodeObject(ctx, i0);
    l8 = i0;
    i0 = 31u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    a = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
    goto block_6;
  }
  block_32:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_30;
  }
  block_31:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 67u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_30;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 56u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_30:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
    goto block_6;
  }
  block_29:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_27;
  }
  block_28:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 67u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_27;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 56u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_27:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
    goto block_6;
  }
  block_26:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
    goto block_6;
  }
  block_25:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 67u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    a = i0;
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 56u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
    goto block_6;
  }
  block_24:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 67u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    a = i0;
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 56u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
    goto block_6;
  }
  block_23:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 76u, j1);
    i0 = l3;
    i1 = l5;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = 0u;
    l4 = i0;
  }
  block_22:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = 13792u;
    i0 += i1;
    l2 = i0;
    i0 = a;
    i1 = 13803u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 76u);
      l4 = i0;
    }
    i0 = l2;
    i1 = l3;
    j1 = LoadU64(ctx, i1 + 84u);
    StoreU64(ctx, i0, j1);
    i0 = l2;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 92u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 84u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 95u, i1);
    i0 = a;
    i1 = 13804u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 96u);
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = kF1042ConstB5F40;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    i1 = 72u;
    i0 += i1;
    i1 = l4;
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 71u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 60u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i1 = 0u;
    StoreU32(ctx, i0 + 68u, i1);
    i0 = l3;
    i1 = 76u;
    i0 += i1;
    l5 = i0;
    j1 = 0ull;
    StoreU64(ctx, i0, j1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 92u, j1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 60u, j1);
    i0 = l3;
    i1 = kF1042ConstB5F40;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 84u, j1);
    i0 = l3;
    i1 = l5;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = a;
    i1 = 13812u;
    i0 += i1;
    l2 = i0;
    i0 = a;
    i1 = 13823u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l2;
    i1 = l3;
    i2 = 56u;
    i1 += i2;
    i2 = 4u;
    i1 |= i2;
    l4 = i1;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0, j1);
    i0 = l2;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 60u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 71u, i1);
    i0 = a;
    i1 = 13824u;
    i0 += i1;
    l6 = i0;
    i1 = a;
    i2 = 13828u;
    i1 += i2;
    l2 = i1;
    i1 = LoadU32(ctx, i1);
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l6;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 72u);
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 76u);
    l4 = i1;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 13832u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 80u);
    l9 = i1;
    StoreU32(ctx, i0, i1);
    i0 = l9;
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = l2;
      StoreU32(ctx, i0, i1);
      goto block_177;
    }
    i0 = l4;
    i1 = l2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 76u, j1);
    i0 = l3;
    i1 = l5;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = 0u;
    l4 = i0;
    block_177:;
    i0 = a;
    i1 = 13836u;
    i0 += i1;
    l2 = i0;
    i0 = a;
    i1 = 13847u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 76u);
      l4 = i0;
    }
    i0 = l2;
    i1 = l3;
    j1 = LoadU64(ctx, i1 + 84u);
    StoreU64(ctx, i0, j1);
    i0 = l2;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 92u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 84u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 95u, i1);
    i0 = a;
    i1 = 13848u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 96u);
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = kF1042ConstB5F40;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    i1 = 72u;
    i0 += i1;
    i1 = l4;
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 71u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 60u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i1 = 0u;
    StoreU32(ctx, i0 + 68u, i1);
    i0 = l3;
    i1 = 76u;
    i0 += i1;
    l5 = i0;
    j1 = 0ull;
    StoreU64(ctx, i0, j1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 92u, j1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 60u, j1);
    i0 = l3;
    i1 = kF1042ConstB5F40;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 84u, j1);
    i0 = l3;
    i1 = l5;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = a;
    i1 = 13856u;
    i0 += i1;
    l2 = i0;
    i0 = a;
    i1 = 13867u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l2;
    i1 = l3;
    i2 = 56u;
    i1 += i2;
    i2 = 4u;
    i1 |= i2;
    l4 = i1;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0, j1);
    i0 = l2;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 60u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 71u, i1);
    i0 = a;
    i1 = 13868u;
    i0 += i1;
    l6 = i0;
    i1 = a;
    i2 = 13872u;
    i1 += i2;
    l2 = i1;
    i1 = LoadU32(ctx, i1);
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l6;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 72u);
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 76u);
    l4 = i1;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 13876u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 80u);
    l9 = i1;
    StoreU32(ctx, i0, i1);
    i0 = l9;
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = l2;
      StoreU32(ctx, i0, i1);
      goto block_182;
    }
    i0 = l4;
    i1 = l2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 76u, j1);
    i0 = l3;
    i1 = l5;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = 0u;
    l4 = i0;
    block_182:;
    i0 = a;
    i1 = 13880u;
    i0 += i1;
    l2 = i0;
    i0 = a;
    i1 = 13891u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 76u);
      l4 = i0;
    }
    i0 = l2;
    i1 = l3;
    j1 = LoadU64(ctx, i1 + 84u);
    StoreU64(ctx, i0, j1);
    i0 = l2;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 92u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 84u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 95u, i1);
    i0 = a;
    i1 = 13892u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 96u);
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = kF1042ConstB5F40;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    i1 = 72u;
    i0 += i1;
    i1 = l4;
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 71u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 60u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i1 = 0u;
    StoreU32(ctx, i0 + 68u, i1);
    i0 = l3;
    i1 = 76u;
    i0 += i1;
    l5 = i0;
    j1 = 0ull;
    StoreU64(ctx, i0, j1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 92u, j1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 60u, j1);
    i0 = l3;
    i1 = kF1042ConstB5F40;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 84u, j1);
    i0 = l3;
    i1 = l5;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = a;
    i1 = 13900u;
    i0 += i1;
    l2 = i0;
    i0 = a;
    i1 = 13911u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l2;
    i1 = l3;
    i2 = 56u;
    i1 += i2;
    i2 = 4u;
    i1 |= i2;
    l4 = i1;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0, j1);
    i0 = l2;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 60u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 71u, i1);
    i0 = a;
    i1 = 13912u;
    i0 += i1;
    l6 = i0;
    i1 = a;
    i2 = 13916u;
    i1 += i2;
    l2 = i1;
    i1 = LoadU32(ctx, i1);
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l6;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 72u);
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 76u);
    l4 = i1;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 13920u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 80u);
    l9 = i1;
    StoreU32(ctx, i0, i1);
    i0 = l9;
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = l2;
      StoreU32(ctx, i0, i1);
      goto block_187;
    }
    i0 = l4;
    i1 = l2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 76u, j1);
    i0 = l3;
    i1 = l5;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = 0u;
    l4 = i0;
    block_187:;
    i0 = a;
    i1 = 13924u;
    i0 += i1;
    l2 = i0;
    i0 = a;
    i1 = 13935u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 76u);
      l4 = i0;
    }
    i0 = l2;
    i1 = l3;
    j1 = LoadU64(ctx, i1 + 84u);
    StoreU64(ctx, i0, j1);
    i0 = l2;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 92u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 84u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 95u, i1);
    i0 = a;
    i1 = 13936u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 96u);
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = kF1042ConstB5F40;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    i1 = 72u;
    i0 += i1;
    i1 = l4;
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 71u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 60u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i1 = 0u;
    StoreU32(ctx, i0 + 68u, i1);
    i0 = l3;
    i1 = 76u;
    i0 += i1;
    l5 = i0;
    j1 = 0ull;
    StoreU64(ctx, i0, j1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 92u, j1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 60u, j1);
    i0 = l3;
    i1 = kF1042ConstB5F40;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 84u, j1);
    i0 = l3;
    i1 = l5;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = a;
    i1 = 13944u;
    i0 += i1;
    l2 = i0;
    i0 = a;
    i1 = 13955u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l2;
    i1 = l3;
    i2 = 56u;
    i1 += i2;
    i2 = 4u;
    i1 |= i2;
    l4 = i1;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0, j1);
    i0 = l2;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 60u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 71u, i1);
    i0 = a;
    i1 = 13956u;
    i0 += i1;
    l6 = i0;
    i1 = a;
    i2 = 13960u;
    i1 += i2;
    l2 = i1;
    i1 = LoadU32(ctx, i1);
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l6;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 72u);
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 76u);
    l4 = i1;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 13964u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 80u);
    l9 = i1;
    StoreU32(ctx, i0, i1);
    i0 = l9;
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = l2;
      StoreU32(ctx, i0, i1);
      goto block_192;
    }
    i0 = l4;
    i1 = l2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 76u, j1);
    i0 = l3;
    i1 = l5;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = 0u;
    l4 = i0;
    block_192:;
    i0 = a;
    i1 = 13968u;
    i0 += i1;
    l2 = i0;
    i0 = a;
    i1 = 13979u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 76u);
      l4 = i0;
    }
    i0 = l2;
    i1 = l3;
    j1 = LoadU64(ctx, i1 + 84u);
    StoreU64(ctx, i0, j1);
    i0 = l2;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 92u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 84u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 95u, i1);
    i0 = a;
    i1 = 13980u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 96u);
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = kF1042ConstB5F40;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    i1 = 72u;
    i0 += i1;
    i1 = l4;
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 71u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 60u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i1 = 0u;
    StoreU32(ctx, i0 + 68u, i1);
    i0 = l3;
    i1 = 76u;
    i0 += i1;
    l5 = i0;
    j1 = 0ull;
    StoreU64(ctx, i0, j1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 92u, j1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 60u, j1);
    i0 = l3;
    i1 = kF1042ConstB5F40;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 84u, j1);
    i0 = l3;
    i1 = l5;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = a;
    i1 = 13988u;
    i0 += i1;
    l2 = i0;
    i0 = a;
    i1 = 13999u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l2;
    i1 = l3;
    i2 = 56u;
    i1 += i2;
    i2 = 4u;
    i1 |= i2;
    l4 = i1;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0, j1);
    i0 = l2;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 60u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 71u, i1);
    i0 = a;
    i1 = 14000u;
    i0 += i1;
    l6 = i0;
    i1 = a;
    i2 = 14004u;
    i1 += i2;
    l2 = i1;
    i1 = LoadU32(ctx, i1);
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l6;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 72u);
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 76u);
    l4 = i1;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 14008u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 80u);
    l9 = i1;
    StoreU32(ctx, i0, i1);
    i0 = l9;
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = l2;
      StoreU32(ctx, i0, i1);
      goto block_197;
    }
    i0 = l4;
    i1 = l2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 76u, j1);
    i0 = l3;
    i1 = l5;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = 0u;
    l4 = i0;
    block_197:;
    i0 = a;
    i1 = 14012u;
    i0 += i1;
    l2 = i0;
    i0 = a;
    i1 = 14023u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 76u);
      l4 = i0;
    }
    i0 = l2;
    i1 = l3;
    j1 = LoadU64(ctx, i1 + 84u);
    StoreU64(ctx, i0, j1);
    i0 = l2;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 92u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 84u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 95u, i1);
    i0 = a;
    i1 = 14024u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 96u);
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = kF1042ConstB5F40;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    i1 = 72u;
    i0 += i1;
    i1 = l4;
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 71u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 60u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i1 = 0u;
    StoreU32(ctx, i0 + 68u, i1);
    i0 = l3;
    i1 = 76u;
    i0 += i1;
    l5 = i0;
    j1 = 0ull;
    StoreU64(ctx, i0, j1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 92u, j1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 60u, j1);
    i0 = l3;
    i1 = kF1042ConstB5F40;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 84u, j1);
    i0 = l3;
    i1 = l5;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = a;
    i1 = 14032u;
    i0 += i1;
    l2 = i0;
    i0 = a;
    i1 = 14043u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l2;
    i1 = l3;
    i2 = 56u;
    i1 += i2;
    i2 = 4u;
    i1 |= i2;
    l4 = i1;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0, j1);
    i0 = l2;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 60u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 71u, i1);
    i0 = a;
    i1 = 14044u;
    i0 += i1;
    l6 = i0;
    i1 = a;
    i2 = 14048u;
    i1 += i2;
    l2 = i1;
    i1 = LoadU32(ctx, i1);
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l6;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 72u);
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 76u);
    l4 = i1;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 14052u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 80u);
    l9 = i1;
    StoreU32(ctx, i0, i1);
    i0 = l9;
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = l2;
      StoreU32(ctx, i0, i1);
      goto block_202;
    }
    i0 = l4;
    i1 = l2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 76u, j1);
    i0 = l3;
    i1 = l5;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = 0u;
    l4 = i0;
    block_202:;
    i0 = a;
    i1 = 14056u;
    i0 += i1;
    l2 = i0;
    i0 = a;
    i1 = 14067u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 76u);
      l4 = i0;
    }
    i0 = l2;
    i1 = l3;
    j1 = LoadU64(ctx, i1 + 84u);
    StoreU64(ctx, i0, j1);
    i0 = l2;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 92u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 84u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 95u, i1);
    i0 = a;
    i1 = 14068u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 96u);
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = kF1042ConstB5F40;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    i1 = 72u;
    i0 += i1;
    i1 = l4;
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 71u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 60u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i1 = 0u;
    StoreU32(ctx, i0 + 68u, i1);
    i0 = l3;
    i1 = 76u;
    i0 += i1;
    l5 = i0;
    j1 = 0ull;
    StoreU64(ctx, i0, j1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 92u, j1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 60u, j1);
    i0 = l3;
    i1 = kF1042ConstB5F40;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 84u, j1);
    i0 = l3;
    i1 = l5;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = a;
    i1 = 14076u;
    i0 += i1;
    l2 = i0;
    i0 = a;
    i1 = 14087u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l2;
    i1 = l3;
    i2 = 56u;
    i1 += i2;
    i2 = 4u;
    i1 |= i2;
    l4 = i1;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0, j1);
    i0 = l2;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 60u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 71u, i1);
    i0 = a;
    i1 = 14088u;
    i0 += i1;
    l6 = i0;
    i1 = a;
    i2 = 14092u;
    i1 += i2;
    l2 = i1;
    i1 = LoadU32(ctx, i1);
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l6;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 72u);
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 76u);
    l4 = i1;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 14096u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 80u);
    l9 = i1;
    StoreU32(ctx, i0, i1);
    i0 = l9;
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = l2;
      StoreU32(ctx, i0, i1);
      goto block_207;
    }
    i0 = l4;
    i1 = l2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 76u, j1);
    i0 = l3;
    i1 = l5;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = 0u;
    l4 = i0;
    block_207:;
    i0 = a;
    i1 = 14100u;
    i0 += i1;
    l2 = i0;
    i0 = a;
    i1 = 14111u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 76u);
      l4 = i0;
    }
    i0 = l2;
    i1 = l3;
    j1 = LoadU64(ctx, i1 + 84u);
    StoreU64(ctx, i0, j1);
    i0 = l2;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 92u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 84u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 95u, i1);
    i0 = a;
    i1 = 14112u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 96u);
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = kF1042ConstB5F40;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    i1 = 72u;
    i0 += i1;
    i1 = l4;
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 71u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 60u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i1 = 0u;
    StoreU32(ctx, i0 + 68u, i1);
    i0 = l3;
    i1 = 76u;
    i0 += i1;
    l5 = i0;
    j1 = 0ull;
    StoreU64(ctx, i0, j1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 92u, j1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 60u, j1);
    i0 = l3;
    i1 = kF1042ConstB5F40;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 84u, j1);
    i0 = l3;
    i1 = l5;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = a;
    i1 = 14120u;
    i0 += i1;
    l2 = i0;
    i0 = a;
    i1 = 14131u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l2;
    i1 = l3;
    i2 = 56u;
    i1 += i2;
    i2 = 4u;
    i1 |= i2;
    l4 = i1;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0, j1);
    i0 = l2;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 60u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 71u, i1);
    i0 = a;
    i1 = 14132u;
    i0 += i1;
    l6 = i0;
    i1 = a;
    i2 = 14136u;
    i1 += i2;
    l2 = i1;
    i1 = LoadU32(ctx, i1);
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l6;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 72u);
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 76u);
    l4 = i1;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 14140u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 80u);
    l9 = i1;
    StoreU32(ctx, i0, i1);
    i0 = l9;
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = l2;
      StoreU32(ctx, i0, i1);
      goto block_212;
    }
    i0 = l4;
    i1 = l2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 76u, j1);
    i0 = l3;
    i1 = l5;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = 0u;
    l4 = i0;
    block_212:;
    i0 = a;
    i1 = 14144u;
    i0 += i1;
    l2 = i0;
    i0 = a;
    i1 = 14155u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 76u);
      l4 = i0;
    }
    i0 = l2;
    i1 = l3;
    j1 = LoadU64(ctx, i1 + 84u);
    StoreU64(ctx, i0, j1);
    i0 = l2;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 92u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 84u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 95u, i1);
    i0 = a;
    i1 = 14156u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 96u);
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = kF1042ConstB5F40;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    i1 = 72u;
    i0 += i1;
    i1 = l4;
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 71u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 60u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i1 = 0u;
    StoreU32(ctx, i0 + 68u, i1);
    i0 = l3;
    i1 = 76u;
    i0 += i1;
    l5 = i0;
    j1 = 0ull;
    StoreU64(ctx, i0, j1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 92u, j1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 60u, j1);
    i0 = l3;
    i1 = kF1042ConstB5F40;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 84u, j1);
    i0 = l3;
    i1 = l5;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = a;
    i1 = 14164u;
    i0 += i1;
    l2 = i0;
    i0 = a;
    i1 = 14175u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l2;
    i1 = l3;
    i2 = 56u;
    i1 += i2;
    i2 = 4u;
    i1 |= i2;
    l4 = i1;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0, j1);
    i0 = l2;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 60u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 71u, i1);
    i0 = a;
    i1 = 14176u;
    i0 += i1;
    l6 = i0;
    i1 = a;
    i2 = 14180u;
    i1 += i2;
    l2 = i1;
    i1 = LoadU32(ctx, i1);
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l6;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 72u);
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 76u);
    l4 = i1;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 14184u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 80u);
    l9 = i1;
    StoreU32(ctx, i0, i1);
    i0 = l9;
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = l2;
      StoreU32(ctx, i0, i1);
      goto block_217;
    }
    i0 = l4;
    i1 = l2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 76u, j1);
    i0 = l3;
    i1 = l5;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = 0u;
    l4 = i0;
    block_217:;
    i0 = a;
    i1 = 14188u;
    i0 += i1;
    l2 = i0;
    i0 = a;
    i1 = 14199u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 76u);
      l4 = i0;
    }
    i0 = l2;
    i1 = l3;
    j1 = LoadU64(ctx, i1 + 84u);
    StoreU64(ctx, i0, j1);
    i0 = l2;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 92u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 84u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 95u, i1);
    i0 = a;
    i1 = 14200u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 96u);
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = kF1042ConstB5F40;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    i1 = 72u;
    i0 += i1;
    i1 = l4;
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 71u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 60u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i1 = 0u;
    StoreU32(ctx, i0 + 68u, i1);
    i0 = l3;
    i1 = 76u;
    i0 += i1;
    l5 = i0;
    j1 = 0ull;
    StoreU64(ctx, i0, j1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 92u, j1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 60u, j1);
    i0 = l3;
    i1 = kF1042ConstB5F40;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 84u, j1);
    i0 = l3;
    i1 = l5;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = a;
    i1 = 14264u;
    i0 += i1;
    l2 = i0;
    i0 = a;
    i1 = 14275u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l2;
    i1 = l3;
    i2 = 56u;
    i1 += i2;
    i2 = 4u;
    i1 |= i2;
    l4 = i1;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0, j1);
    i0 = l2;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 60u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 71u, i1);
    i0 = a;
    i1 = 14276u;
    i0 += i1;
    l6 = i0;
    i1 = a;
    i2 = 14280u;
    i1 += i2;
    l2 = i1;
    i1 = LoadU32(ctx, i1);
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l6;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 72u);
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 76u);
    l4 = i1;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 14284u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 80u);
    l9 = i1;
    StoreU32(ctx, i0, i1);
    i0 = l9;
    i0 = !(i0);
    l9 = i0;
    if (i0) {
      i0 = l6;
      i1 = l2;
      StoreU32(ctx, i0, i1);
      goto block_222;
    }
    i0 = l4;
    i1 = l2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 76u, j1);
    i0 = l3;
    i1 = l5;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = 0u;
    l4 = i0;
    block_222:;
    i0 = a;
    i1 = 14288u;
    i0 += i1;
    l2 = i0;
    i0 = a;
    i1 = 14299u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 76u);
      l4 = i0;
    }
    i0 = l2;
    i1 = l3;
    j1 = LoadU64(ctx, i1 + 84u);
    StoreU64(ctx, i0, j1);
    i0 = l2;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 92u);
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 84u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 95u, i1);
    i0 = a;
    i1 = 14300u;
    i0 += i1;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 96u);
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = kF1042ConstB5F40;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    i1 = 72u;
    i0 += i1;
    i1 = l4;
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 71u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 60u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = a;
    j1 = 0ull;
    StoreU64(ctx, i0 + 14328u, j1);
    i0 = a;
    i1 = 14344u;
    i0 += i1;
    i1 = a;
    i2 = 14348u;
    i1 += i2;
    l2 = i1;
    i1 = LoadU32(ctx, i1);
    ReleaseTaggedChildNodes(ctx, i0, i1);
    i0 = a;
    i1 = l2;
    StoreU32(ctx, i0 + 14344u, i1);
    i0 = l2;
    j1 = 0ull;
    StoreU64(ctx, i0, j1);
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 14312u;
    i0 += i1;
    l4 = i0;
    i0 = a;
    i1 = 14304u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = 56u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l7;
  i1 = 32u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2582u;
    i1 = l2;
    i2 = l4;
    i3 = l5;
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
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_7;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 60u);
    l6 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 56u);
    l2 = i0;
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 56u, j1);
    i0 = a;
    i1 = l2;
    StoreU32(ctx, i0 + 14320u, i1);
    i0 = a;
    i1 = 14324u;
    i0 += i1;
    l9 = i0;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = l9;
    i1 = l6;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i0 = !(i0);
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    if (i0) {goto block_228;}
    i0 = l2;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 4u);
    l6 = i1;
    i2 = 1u;
    i1 -= i2;
    l9 = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l6;
    i1 = 1u;
    i0 = i0 != i1;
    l6 = i0;
    if (i0) {goto block_228;}
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    l6 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 33u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = l2;
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
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    l6 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l6;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_3;}
    i0 = l2;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 8u);
    l6 = i1;
    i2 = 1u;
    i1 -= i2;
    l9 = i1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l6;
    i1 = 1u;
    i0 = i0 != i1;
    l6 = i0;
    if (i0) {goto block_228;}
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    l6 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 34u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = l2;
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
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_3;}
  }
  block_228:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 60u);
    l2 = i0;
    i0 = !(i0);
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    if (i0) {goto block_235;}
    i0 = l2;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 4u);
    l6 = i1;
    i2 = 1u;
    i1 -= i2;
    l9 = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l6;
    i1 = 1u;
    i0 = i0 != i1;
    l6 = i0;
    if (i0) {goto block_235;}
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    l6 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 35u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = l2;
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
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    l6 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l6;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_3;}
    i0 = l2;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 8u);
    l6 = i1;
    i2 = 1u;
    i1 -= i2;
    l9 = i1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l6;
    i1 = 1u;
    i0 = i0 != i1;
    l6 = i0;
    if (i0) {goto block_235;}
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    l6 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 36u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = l2;
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
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_3;}
  }
  block_235:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 56u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l7;
  i1 = 37u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2582u;
    i1 = l2;
    i2 = l4;
    i3 = l5;
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
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_8;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 60u);
    l4 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 56u);
    l2 = i0;
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 56u, j1);
    i0 = a;
    i1 = l2;
    StoreU32(ctx, i0 + 14336u, i1);
    i0 = a;
    i1 = 14340u;
    i0 += i1;
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = l5;
    i1 = l4;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i0 = !(i0);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    if (i0) {goto block_244;}
    i0 = l2;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 4u);
    l4 = i1;
    i2 = 1u;
    i1 -= i2;
    l5 = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
    l4 = i0;
    if (i0) {goto block_244;}
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 38u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l4;
    i1 = l2;
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
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_3;}
    i0 = l2;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 8u);
    l4 = i1;
    i2 = 1u;
    i1 -= i2;
    l5 = i1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
    l4 = i0;
    if (i0) {goto block_244;}
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 39u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l4;
    i1 = l2;
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
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_3;}
  }
  block_244:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 60u);
    l2 = i0;
    i0 = !(i0);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    if (i0) {goto block_251;}
    i0 = l2;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 4u);
    l4 = i1;
    i2 = 1u;
    i1 -= i2;
    l5 = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
    l4 = i0;
    if (i0) {goto block_251;}
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 40u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l4;
    i1 = l2;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 40u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_3;}
    i0 = l2;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 8u);
    l4 = i1;
    i2 = 1u;
    i1 -= i2;
    l5 = i1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
    l4 = i0;
    if (i0) {goto block_251;}
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 41u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l4;
    i1 = l2;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 41u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_3;}
  }
  block_251:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = 14367u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 14356u);
      i1 = 0u;
      StoreU8(ctx, i0, i1);
      i0 = a;
      i1 = 14360u;
      i0 += i1;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      goto block_259;
    }
    i0 = a;
    i1 = 0u;
    StoreU8(ctx, i0 + 14367u, i1);
    i0 = a;
    i1 = 0u;
    StoreU8(ctx, i0 + 14356u, i1);
    block_259:;
    i0 = a;
    i1 = 14379u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 14368u);
      i1 = 0u;
      StoreU8(ctx, i0, i1);
      i0 = a;
      i1 = 14372u;
      i0 += i1;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      goto block_261;
    }
    i0 = a;
    i1 = 0u;
    StoreU8(ctx, i0 + 14379u, i1);
    i0 = a;
    i1 = 0u;
    StoreU8(ctx, i0 + 14368u, i1);
    block_261:;
    i0 = a;
    i1 = 14391u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 14380u);
      i1 = 0u;
      StoreU8(ctx, i0, i1);
      i0 = a;
      i1 = 14384u;
      i0 += i1;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      goto block_263;
    }
    i0 = a;
    i1 = 0u;
    StoreU8(ctx, i0 + 14391u, i1);
    i0 = a;
    i1 = 0u;
    StoreU8(ctx, i0 + 14380u, i1);
    block_263:;
    i0 = a;
    i1 = 14403u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 14392u);
      i1 = 0u;
      StoreU8(ctx, i0, i1);
      i0 = a;
      i1 = 14396u;
      i0 += i1;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      goto block_265;
    }
    i0 = a;
    i1 = 0u;
    StoreU8(ctx, i0 + 14403u, i1);
    i0 = a;
    i1 = 0u;
    StoreU8(ctx, i0 + 14392u, i1);
    block_265:;
    i0 = a;
    i1 = 14415u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 14404u);
      i1 = 0u;
      StoreU8(ctx, i0, i1);
      i0 = a;
      i1 = 14408u;
      i0 += i1;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      goto block_267;
    }
    i0 = a;
    i1 = 0u;
    StoreU8(ctx, i0 + 14415u, i1);
    i0 = a;
    i1 = 0u;
    StoreU8(ctx, i0 + 14404u, i1);
    block_267:;
    i0 = a;
    i1 = 14427u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 14416u);
      i1 = 0u;
      StoreU8(ctx, i0, i1);
      i0 = a;
      i1 = 14420u;
      i0 += i1;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      goto block_269;
    }
    i0 = a;
    i1 = 0u;
    StoreU8(ctx, i0 + 14427u, i1);
    i0 = a;
    i1 = 0u;
    StoreU8(ctx, i0 + 14416u, i1);
    block_269:;
    i0 = a;
    i1 = 14439u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 14428u);
      i1 = 0u;
      StoreU8(ctx, i0, i1);
      i0 = a;
      i1 = 14432u;
      i0 += i1;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      goto block_271;
    }
    i0 = a;
    i1 = 0u;
    StoreU8(ctx, i0 + 14439u, i1);
    i0 = a;
    i1 = 0u;
    StoreU8(ctx, i0 + 14428u, i1);
    block_271:;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
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
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 42u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF1042MsgPx;
      i2 = kF1042MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF1042MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 42u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF1042ConstD015C;
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
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 43u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1032u;
    i1 = 152u;
    i0 = HostFoldLayoutOffset(ctx, i0, i1);
    l8 = i0;
    i0 = 43u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_11;}
    i0 = l3;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 140u);
    StoreU32(ctx, i0 + 48u, i1);
    i0 = l3;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 144u);
    l5 = i1;
    StoreU32(ctx, i0 + 52u, i1);
    i0 = l5;
    i0 = !(i0);
    l9 = i0;
    i0 = l4;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l9;
    if (i0) {goto block_280;}
    i0 = l5;
    i1 = l5;
    i1 = LoadU32(ctx, i1 + 4u);
    i2 = 1u;
    i1 += i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
    l2 = i0;
    if (i0) {goto block_280;}
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 44u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 857u;
    i1 = kF1042MsgPx;
    i2 = kF1042MsgBoostBoostSmartPtr;
    i3 = 734u;
    i4 = kF1042MsgOperator;
    HostFailAssertion(ctx, i0, i1, i2, i3, i4);
    i0 = 44u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 != i1;
    a = i0;
    if (i0) {goto running_exit;}
    goto block_3;
  }
  block_280:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 16u);
    StoreU32(ctx, i0 + 40u, i1);
    i0 = l3;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 20u);
    l2 = i1;
    StoreU32(ctx, i0 + 44u, i1);
    i0 = l2;
    if (i0) {
      i0 = l2;
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 4u);
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 4u, i1);
    }
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 40u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = 48u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l7;
  i1 = 45u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2469u;
    i1 = l6;
    i2 = l2;
    i3 = l5;
    i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
    l8 = i0;
    i0 = 45u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    l9 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = 1u;
    l5 = i0;
    i0 = l9;
    i1 = 1u;
    i0 = i0 == i1;
    l9 = i0;
    if (i0) {goto block_9;}
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 24u;
    i0 += i1;
    l4 = i0;
  }
  i0 = l7;
  i1 = 46u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2716u;
    i1 = l4;
    i2 = l2;
    HostOsSchedule(ctx, i0, i1, i2);
    i0 = 46u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = 0u;
    l5 = i0;
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_9;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 44u);
    l2 = i0;
    i0 = !(i0);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    if (i0) {goto block_290;}
    i0 = l2;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 4u);
    l4 = i1;
    i2 = 1u;
    i1 -= i2;
    l5 = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
    l4 = i0;
    if (i0) {goto block_290;}
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 47u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l4;
    i1 = l2;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 47u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_3;}
    i0 = l2;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 8u);
    l4 = i1;
    i2 = 1u;
    i1 -= i2;
    l5 = i1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
    l4 = i0;
    if (i0) {goto block_290;}
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 48u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l4;
    i1 = l2;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 48u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_3;}
  }
  block_290:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 52u);
    l2 = i0;
    i0 = !(i0);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    if (i0) {goto block_297;}
    i0 = l2;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 4u);
    l4 = i1;
    i2 = 1u;
    i1 -= i2;
    l5 = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
    l4 = i0;
    if (i0) {goto block_297;}
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 49u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l4;
    i1 = l2;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 49u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_3;}
    i0 = l2;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 8u);
    l4 = i1;
    i2 = 1u;
    i1 -= i2;
    l5 = i1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
    l4 = i0;
    if (i0) {goto block_297;}
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 50u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l4;
    i1 = l2;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 50u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_3;}
  }
  block_297:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
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
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 51u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF1042MsgPx;
      i2 = kF1042MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF1042MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 51u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF1042ConstD015C;
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
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 76u);
    l2 = i0;
  }
  loop_310: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      l5 = i0;
      i0 = l4;
      i1 = 80u;
      i0 += i1;
      l6 = i0;
      i1 = l2;
      i0 = i0 == i1;
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
        i0 = l5;
        i0 = LoadU32(ctx, i0);
        l6 = i0;
        i0 = l4;
        i1 = 76u;
        i0 += i1;
        l2 = i0;
      }
      i0 = l7;
      i1 = 52u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l6;
        ReleasePlainNodeTree(ctx, i0, i1);
        i0 = 52u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i1 = l5;
        StoreU32(ctx, i0 + 76u, i1);
        i0 = l4;
        i1 = 0u;
        StoreU32(ctx, i0 + 84u, i1);
        i0 = l5;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i0 = LoadU32(ctx, i0 + 16u);
        l2 = i0;
        i0 = !(i0);
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
          i0 = kF1042ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l7;
        i1 = 53u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF1042MsgPx;
          i2 = kF1042MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF1042MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 53u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF1042ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF1042ConstD015C;
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
        i0 = l2;
        i1 = 88u;
        i0 += i1;
        l5 = i0;
        i1 = l2;
        i2 = 92u;
        i1 += i2;
        l4 = i1;
        i1 = LoadU32(ctx, i1);
        l6 = i1;
        ReleaseSubtreeTaggedNodes(ctx, i0, i1);
        i0 = l2;
        i1 = l4;
        StoreU32(ctx, i0 + 88u, i1);
        i0 = l2;
        j1 = 0ull;
        StoreU64(ctx, i0 + 92u, j1);
        i0 = a;
        i0 = LoadU32(ctx, i0 + 16u);
        l2 = i0;
        if (i0) {goto block_311;}
        i0 = kF1042ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 54u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF1042MsgPx;
        i2 = kF1042MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF1042MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 54u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF1042ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF1042ConstD015C;
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
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 88u;
      i0 += i1;
      i1 = l5;
      i2 = 16u;
      i1 += i2;
      i0 = FindOrderedChainEntry(ctx, i0, i1);
      l4 = i0;
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_10;}
      i0 = a;
      i0 = LoadU32(ctx, i0 + 16u);
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
        i0 = kF1042ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 55u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF1042MsgPx;
        i2 = kF1042MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF1042MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 55u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF1042ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF1042ConstD015C;
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
    i0 = l2;
    i1 = l4;
    i2 = l2;
    i3 = 92u;
    i2 += i3;
    i1 = i1 == i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l2 = i0;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      if (i0) {goto block_331;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 28u);
      l2 = i0;
      i1 = l4;
      i2 = 32u;
      i1 += i2;
      l9 = i1;
      i0 = i0 == i1;
      l4 = i0;
      if (i0) {goto block_331;}
    }
    loop_333: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        l6 = i0;
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 16u);
        l2 = i0;
        i0 = kF1042ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 56u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1592u;
        i1 = l2;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 56u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF1042ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF1042ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 != i1;
        if (i0) {
          i0 = l6;
          i0 = LoadU32(ctx, i0 + 4u);
          l4 = i0;
          if (i0) {goto block_338;}
          i0 = l6;
          i1 = l6;
          i1 = LoadU32(ctx, i1 + 8u);
          l2 = i1;
          i1 = LoadU32(ctx, i1);
          i0 = i0 == i1;
          if (i0) {goto block_337;}
          i0 = l6;
          i1 = 8u;
          i0 += i1;
          l4 = i0;
          loop_340: 
            i0 = l4;
            i0 = LoadU32(ctx, i0);
            l6 = i0;
            i1 = 8u;
            i0 += i1;
            l4 = i0;
            i0 = l6;
            i0 = LoadU32(ctx, i0 + 8u);
            l2 = i0;
            i0 = LoadU32(ctx, i0);
            i1 = l6;
            i0 = i0 != i1;
            l6 = i0;
            if (i0) {goto loop_340;}
          goto block_337;
        }
        i0 = HostRet0(ctx);
        l2 = i0;
        i0 = l3;
        i1 = 240u;
        i0 += i1;
        l4 = i0;
        i0 = HostTempRet0(ctx);
        a = i0;
        goto block_6;
        block_338:;
        loop_341: 
          i0 = l4;
          l2 = i0;
          i0 = LoadU32(ctx, i0);
          l4 = i0;
          if (i0) {goto loop_341;}
        block_337:;
        i0 = l2;
        i1 = l9;
        i0 = i0 != i1;
        l4 = i0;
        if (i0) {goto loop_333;}
      }
    block_331:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i0 = LoadU32(ctx, i0 + 4u);
      l4 = i0;
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        i1 = l5;
        i1 = LoadU32(ctx, i1 + 8u);
        l2 = i1;
        i1 = LoadU32(ctx, i1);
        i0 = i0 == i1;
        if (i0) {goto block_343;}
        i0 = l5;
        i1 = 8u;
        i0 += i1;
        l4 = i0;
        loop_345: 
          i0 = l4;
          i0 = LoadU32(ctx, i0);
          l5 = i0;
          i1 = 8u;
          i0 += i1;
          l4 = i0;
          i0 = l5;
          i0 = LoadU32(ctx, i0 + 8u);
          l2 = i0;
          i0 = LoadU32(ctx, i0);
          l6 = i0;
          i1 = l5;
          i0 = i0 != i1;
          l5 = i0;
          if (i0) {goto loop_345;}
        goto block_343;
      }
      loop_346: 
        i0 = l4;
        l2 = i0;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        if (i0) {goto loop_346;}
      block_343:;
      i0 = a;
      i0 = LoadU32(ctx, i0 + 16u);
      l4 = i0;
      if (i0) {goto loop_310;}
      goto block_5;
    }
    block_311:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 7u;
    StoreU8(ctx, i0 + 67u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 63u, i1);
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = kF1042MsgBacklog;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    i1 = kF1042ConstAF4E9;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 59u, i1);
    i0 = l3;
    i1 = 56u;
    i0 += i1;
    l4 = i0;
  }
  i0 = l7;
  i1 = 57u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2717u;
    i1 = l2;
    i2 = l4;
    i3 = 0u;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 57u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF1042ConstD015C;
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
      i0 = LoadS8(ctx, i0 + 67u);
      i1 = kF1042ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 56u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = a;
      i0 = LoadU32(ctx, i0 + 16u);
      l2 = i0;
      i0 = !(i0);
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
        i0 = kF1042ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 58u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF1042MsgPx;
        i2 = kF1042MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF1042MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 58u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF1042ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF1042ConstD015C;
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
      i1 = 0u;
      StoreU8(ctx, i0 + 56u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 67u, i1);
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 56u;
      i0 += i1;
      l4 = i0;
    }
    i0 = l7;
    i1 = 59u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2717u;
      i1 = l2;
      i2 = l4;
      i3 = 0u;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 59u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_364;}
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 67u);
      i1 = kF1042ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 56u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = a;
      i0 = LoadU32(ctx, i0 + 16u);
      l2 = i0;
      i0 = !(i0);
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
        i0 = kF1042ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 60u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF1042MsgPx;
        i2 = kF1042MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF1042MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 60u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF1042ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF1042ConstD015C;
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
      i0 = l2;
      i1 = 112u;
      i0 += i1;
      l4 = i0;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = l2;
      i1 = 108u;
      i0 += i1;
      l5 = i0;
    }
    i0 = l7;
    i1 = 61u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l5;
      i1 = l6;
      ReleaseAssertedNodeTree(ctx, i0, i1);
      i0 = 61u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i1 = l4;
      StoreU32(ctx, i0 + 108u, i1);
      i0 = l2;
      j1 = 0ull;
      StoreU64(ctx, i0 + 112u, j1);
      i0 = a;
      i0 = LoadU32(ctx, i0 + 24u);
      l6 = i0;
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
        i0 = kF1042ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 62u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF1042MsgPx;
        i2 = kF1042MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF1042MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 62u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF1042ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF1042ConstD015C;
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
      i0 = kF1042ConstCF11C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
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
        i0 = kF1042ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 63u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1032u;
        i1 = 28u;
        i0 = HostFoldLayoutOffset(ctx, i0, i1);
        l8 = i0;
        i0 = 63u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l8;
        l5 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF1042ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF1042ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        l2 = i0;
        if (i0) {goto block_363;}
        i0 = l5;
        i1 = 0u;
        StoreU8(ctx, i0 + 4u, i1);
        i0 = l5;
        i1 = kF1042ConstB5C94;
        StoreU32(ctx, i0, i1);
        i0 = 38u;
        l2 = i0;
        i0 = kF1042Const8ADC0;
        l4 = i0;
        loop_396: 
          i0 = l2;
          i1 = 1u;
          i0 >>= (i1 & 31);
          l9 = i0;
          i1 = 2u;
          i0 <<= (i1 & 31);
          i1 = l4;
          i0 += i1;
          l10 = i0;
          i1 = 4u;
          i0 += i1;
          i1 = l4;
          i2 = l10;
          i2 = LoadU32(ctx, i2);
          i3 = 11u;
          i2 = i2 < i3;
          l10 = i2;
          i0 = i2 ? i0 : i1;
          l4 = i0;
          i0 = l9;
          i1 = kF1042ConstFFFFFFFF;
          i0 ^= i1;
          i1 = l2;
          i0 += i1;
          i1 = l9;
          i2 = l10;
          i0 = i2 ? i0 : i1;
          l2 = i0;
          if (i0) {goto loop_396;}
        i0 = kF1042Const8AE54;
        i1 = l4;
        i2 = l4;
        i3 = kF1042MsgBuckets;
        i2 = i2 == i3;
        i0 = i2 ? i0 : i1;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = l5;
        j1 = 0ull;
        StoreU64(ctx, i0 + 20u, j1);
        i0 = l5;
        j1 = 4575657221408423936ull;
        StoreU64(ctx, i0 + 12u, j1);
        i0 = l5;
        i1 = l2;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = kF1042ConstCF11C;
        i1 = l5;
        StoreU32(ctx, i0, i1);
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 64u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 32u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l8 = i0;
      i0 = 64u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      l4 = i0;
      if (i0) {goto block_362;}
      i0 = l3;
      i1 = l2;
      StoreU32(ctx, i0 + 224u, i1);
      i0 = l3;
      j1 = 9223372174293729303ull;
      StoreU64(ctx, i0 + 228u, j1);
      i0 = l2;
      i1 = 0u;
      StoreU8(ctx, i0 + 23u, i1);
      i0 = l2;
      i1 = kF1042ConstAF9D1;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0 + 15u, j1);
      i0 = l2;
      i1 = kF1042ConstAF9CA;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0 + 8u, j1);
      i0 = l2;
      i1 = kF1042MsgIniAsyncifyhashlisturl;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 224u;
      i0 += i1;
      l4 = i0;
      i0 = l3;
      i1 = 56u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l7;
    i1 = 65u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2454u;
      i1 = l2;
      i2 = l5;
      i3 = l4;
      i4 = 0u;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 65u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_361;}
      i0 = l6;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 40u);
      l2 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 56u;
      i0 += i1;
      l4 = i0;
    }
    i0 = l7;
    i1 = 66u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = l6;
      i2 = l4;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l8 = i0;
      i0 = 66u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_360;}
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 67u);
      i1 = kF1042ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 56u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 235u);
      i1 = kF1042ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l2 = i0;
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 224u);
        l2 = i0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = b;
      if (i0) {goto block_349;}
      i0 = a;
      i0 = LoadU32(ctx, i0 + 112u);
      b = i0;
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
        i0 = kF1042ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 67u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF1042MsgPx;
        i2 = kF1042MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF1042MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 67u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF1042ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF1042ConstD015C;
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
      i1 = 0u;
      StoreU8(ctx, i0 + 64u, i1);
      i0 = l3;
      j1 = 7813594090769100403ull;
      StoreU64(ctx, i0 + 56u, j1);
      i0 = l3;
      i1 = 8u;
      StoreU8(ctx, i0 + 67u, i1);
      i0 = b;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 20u);
      l2 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 56u;
      i0 += i1;
      l4 = i0;
    }
    i0 = l7;
    i1 = 68u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = b;
      i2 = l4;
      i3 = 1000u;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l8 = i0;
      i0 = 68u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
      if (i0) {goto block_357;}
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 67u);
      i1 = kF1042ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 56u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = a;
      i0 = LoadU32(ctx, i0 + 112u);
      b = i0;
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
        i0 = kF1042ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 69u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF1042MsgPx;
        i2 = kF1042MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF1042MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 69u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF1042ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF1042ConstD015C;
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
      i1 = 7u;
      StoreU8(ctx, i0 + 67u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 63u, i1);
      i0 = l3;
      i1 = kF1042MsgSSevol;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0 + 56u, i1);
      i0 = l3;
      i1 = kF1042ConstAF9DD;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0 + 59u, i1);
      i0 = b;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 20u);
      l2 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 56u;
      i0 += i1;
      l4 = i0;
    }
    i0 = l7;
    i1 = 70u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = b;
      i2 = l4;
      i3 = 1000u;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l8 = i0;
      i0 = 70u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
      if (i0) {goto block_356;}
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 67u);
      i1 = kF1042ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 56u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = a;
      i0 = LoadU32(ctx, i0 + 112u);
      b = i0;
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
        i0 = kF1042ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 71u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF1042MsgPx;
        i2 = kF1042MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF1042MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 71u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF1042ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF1042ConstD015C;
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
      i1 = kF1042ConstFFFFFFC0;
      i0 -= i1;
      i1 = kF1042ConstAF9EA;
      i1 = LoadU16(ctx, i1);
      StoreU16(ctx, i0, i1);
      i0 = l3;
      i1 = 2560u;
      StoreU16(ctx, i0 + 66u, i1);
      i0 = l3;
      i1 = kF1042MsgSVideovol;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0 + 56u, j1);
      i0 = b;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 20u);
      l2 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 56u;
      i0 += i1;
      l4 = i0;
    }
    i0 = l7;
    i1 = 72u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = b;
      i2 = l4;
      i3 = 1000u;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l8 = i0;
      i0 = 72u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
      if (i0) {goto block_355;}
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 67u);
      i1 = kF1042ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 56u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = a;
      i0 = LoadU32(ctx, i0 + 112u);
      l2 = i0;
      i0 = !(i0);
      b = i0;
    }
    i0 = b;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF1042ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 73u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF1042MsgPx;
        i2 = kF1042MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF1042MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 73u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF1042ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF1042ConstD015C;
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
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 74u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 16u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l8 = i0;
      i0 = 74u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      l4 = i0;
      if (i0) {goto block_354;}
      i0 = l3;
      i1 = b;
      StoreU32(ctx, i0 + 56u, i1);
      i0 = l3;
      j1 = 9223372105574252558ull;
      StoreU64(ctx, i0 + 60u, j1);
      i0 = b;
      i1 = 0u;
      StoreU8(ctx, i0 + 14u, i1);
      i0 = b;
      i1 = kF1042ConstAF9F3;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0 + 6u, j1);
      i0 = b;
      i1 = kF1042MsgSAutomodewait;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 20u);
      b = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 56u;
      i0 += i1;
      l4 = i0;
    }
    i0 = l7;
    i1 = 75u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = l2;
      i2 = l4;
      i3 = 1000u;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l8 = i0;
      i0 = 75u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
      if (i0) {goto block_353;}
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 67u);
      i1 = kF1042ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 56u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = a;
      i0 = LoadU32(ctx, i0 + 128u);
      b = i0;
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
        i0 = kF1042ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 76u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF1042MsgPx;
        i2 = kF1042MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF1042MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 76u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF1042ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF1042ConstD015C;
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
      i1 = kF1042ConstAFA04;
      i1 = LoadU8(ctx, i1);
      StoreU8(ctx, i0 + 32u, i1);
      i0 = l3;
      i1 = 9u;
      StoreU8(ctx, i0 + 35u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 33u, i1);
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = kF1042MsgSavegDat;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0 + 24u, j1);
      i0 = l3;
      i1 = 24u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l7;
    i1 = 77u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2601u;
      i1 = b;
      i2 = l2;
      i3 = 0u;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l8 = i0;
      i0 = 77u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
      if (i0) {goto block_351;}
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 35u);
      i1 = kF1042ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 24u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = a;
      i0 = LoadU32(ctx, i0 + 128u);
      b = i0;
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
        i0 = kF1042ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 78u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF1042MsgPx;
        i2 = kF1042MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF1042MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 78u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF1042ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF1042ConstD015C;
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
      i1 = kF1042ConstAFA0E;
      i1 = LoadU16(ctx, i1);
      StoreU16(ctx, i0 + 16u, i1);
      i0 = l3;
      i1 = 2560u;
      StoreU16(ctx, i0 + 18u, i1);
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = kF1042MsgSystemDat;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0 + 8u, j1);
      i0 = l3;
      i1 = 8u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l7;
    i1 = 79u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2601u;
      i1 = b;
      i2 = l2;
      i3 = 0u;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l8 = i0;
      i0 = 79u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l8;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
      if (i0) {goto block_350;}
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 19u);
      i1 = kF1042ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      b = i0;
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 8u);
        b = i0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 80u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2718u;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 80u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_349;}
      goto block_11;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 67u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    a = i0;
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 56u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
    goto block_6;
  }
  block_364:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 67u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    a = i0;
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 56u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
    goto block_6;
  }
  block_363:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
    goto block_6;
  }
  block_362:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_358;
  }
  block_361:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_359;
  }
  block_360:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 67u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_359;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 56u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_359:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 235u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_358;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 224u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_358:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
    goto block_6;
  }
  block_357:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 67u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    a = i0;
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 56u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
    goto block_6;
  }
  block_356:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 67u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    a = i0;
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 56u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
    goto block_6;
  }
  block_355:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 67u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    a = i0;
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 56u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
    goto block_6;
  }
  block_354:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_352;
  }
  block_353:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 67u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_352;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 56u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_352:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
    goto block_6;
  }
  block_351:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 35u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_6;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 24u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_6;
  }
  block_350:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 19u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_6;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_6;
  }
  block_349:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 81u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2719u;
    i1 = a;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 81u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    b = i0;
    if (i0) {goto block_11;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 128u);
    b = i0;
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
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 82u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF1042MsgPx;
      i2 = kF1042MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF1042MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 82u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF1042ConstD015C;
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
    i0 = b;
    i1 = 8u;
    i0 += i1;
    l2 = i0;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = b;
    i1 = 4u;
    i0 += i1;
    l4 = i0;
  }
  i0 = l7;
  i1 = 83u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l4;
    i1 = l5;
    ClearLinkedNodeFields(ctx, i0, i1);
    i0 = 83u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i1 = l2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = b;
    j1 = 0ull;
    StoreU64(ctx, i0 + 8u, j1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 128u);
    b = i0;
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
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 84u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF1042MsgPx;
      i2 = kF1042MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF1042MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 84u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF1042ConstD015C;
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
    i0 = b;
    i1 = 20u;
    i0 += i1;
    l2 = i0;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = b;
    i1 = 16u;
    i0 += i1;
    l4 = i0;
  }
  i0 = l7;
  i1 = 85u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l4;
    i1 = l5;
    ReleaseNodeListTail(ctx, i0, i1);
    i0 = 85u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i1 = l2;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = b;
    j1 = 0ull;
    StoreU64(ctx, i0 + 20u, j1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 128u);
    b = i0;
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
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 86u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF1042MsgPx;
      i2 = kF1042MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF1042MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 86u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF1042ConstD015C;
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
    i0 = b;
    i1 = 28u;
    i0 += i1;
    l4 = i0;
    i1 = b;
    i2 = 32u;
    i1 += i2;
    l2 = i1;
    i1 = LoadU32(ctx, i1);
    l5 = i1;
    ReleaseNoSaveNodeTree(ctx, i0, i1);
    i0 = b;
    i1 = l2;
    StoreU32(ctx, i0 + 28u, i1);
    i0 = b;
    j1 = 0ull;
    StoreU64(ctx, i0 + 32u, j1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 128u);
    b = i0;
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
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 87u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF1042MsgPx;
      i2 = kF1042MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF1042MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 87u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF1042ConstD015C;
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
    i0 = b;
    i1 = 40u;
    i0 += i1;
    l4 = i0;
    i1 = b;
    i2 = 44u;
    i1 += i2;
    l2 = i1;
    i1 = LoadU32(ctx, i1);
    l5 = i1;
    ReleaseNoSaveNodeTree(ctx, i0, i1);
    i0 = b;
    i1 = l2;
    StoreU32(ctx, i0 + 40u, i1);
    i0 = b;
    j1 = 0ull;
    StoreU64(ctx, i0 + 44u, j1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 128u);
    b = i0;
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
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 88u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF1042MsgPx;
      i2 = kF1042MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF1042MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 88u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF1042ConstD015C;
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
    i0 = b;
    i1 = 52u;
    i0 += i1;
    l4 = i0;
    i1 = b;
    i2 = 56u;
    i1 += i2;
    l2 = i1;
    i1 = LoadU32(ctx, i1);
    l5 = i1;
    ReleaseNoSaveNodeTree(ctx, i0, i1);
    i0 = b;
    i1 = l2;
    StoreU32(ctx, i0 + 52u, i1);
    i0 = b;
    j1 = 0ull;
    StoreU64(ctx, i0 + 56u, j1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 128u);
    b = i0;
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
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 89u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF1042MsgPx;
      i2 = kF1042MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF1042MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 89u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF1042ConstD015C;
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
    i0 = b;
    i1 = kF1042ConstFFFFFFC0;
    i0 -= i1;
    l4 = i0;
    i1 = b;
    i2 = 68u;
    i1 += i2;
    l2 = i1;
    i1 = LoadU32(ctx, i1);
    l5 = i1;
    ReleaseNoSaveNodeTree(ctx, i0, i1);
    i0 = b;
    i1 = l2;
    StoreU32(ctx, i0 + 64u, i1);
    i0 = b;
    j1 = 0ull;
    StoreU64(ctx, i0 + 68u, j1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 128u);
    b = i0;
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
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 90u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF1042MsgPx;
      i2 = kF1042MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF1042MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 90u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF1042ConstD015C;
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
    i0 = b;
    i1 = 80u;
    i0 += i1;
    l2 = i0;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = b;
    i1 = 76u;
    i0 += i1;
    l4 = i0;
  }
  i0 = l7;
  i1 = 91u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l4;
    i1 = l5;
    ClearLinkedNodeFields(ctx, i0, i1);
    i0 = 91u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i1 = l2;
    StoreU32(ctx, i0 + 76u, i1);
    i0 = b;
    j1 = 0ull;
    StoreU64(ctx, i0 + 80u, j1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 128u);
    b = i0;
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
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 92u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF1042MsgPx;
      i2 = kF1042MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF1042MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 92u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF1042ConstD015C;
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
    i0 = b;
    i1 = 88u;
    i0 += i1;
    l4 = i0;
    i1 = b;
    i2 = 92u;
    i1 += i2;
    l2 = i1;
    i1 = LoadU32(ctx, i1);
    l5 = i1;
    ReleaseNoSaveNodeTree(ctx, i0, i1);
    i0 = b;
    i1 = l2;
    StoreU32(ctx, i0 + 88u, i1);
    i0 = b;
    j1 = 0ull;
    StoreU64(ctx, i0 + 92u, j1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 128u);
    b = i0;
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
      i0 = kF1042ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 93u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF1042MsgPx;
      i2 = kF1042MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF1042MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 93u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF1042ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF1042ConstD015C;
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
    i0 = b;
    i1 = 0u;
    StoreU8(ctx, i0 + 100u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU32(ctx, i0 + 68u, i1);
    i0 = l3;
    i1 = 76u;
    i0 += i1;
    l4 = i0;
    j1 = 0ull;
    StoreU64(ctx, i0, j1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 92u, j1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 60u, j1);
    i0 = l3;
    i1 = kF1042ConstB5F40;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 84u, j1);
    i0 = l3;
    i1 = l4;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = b;
    i0 = LoadS8(ctx, i0 + 119u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0 + 108u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = b;
    i1 = l3;
    i2 = 56u;
    i1 += i2;
    i2 = 4u;
    i1 |= i2;
    l2 = i1;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0 + 108u, j1);
    i0 = b;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 116u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 60u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 71u, i1);
    i0 = b;
    i1 = 120u;
    i0 += i1;
    i1 = b;
    i2 = 124u;
    i1 += i2;
    l5 = i1;
    i1 = LoadU32(ctx, i1);
    ReleaseNodeTree(ctx, i0, i1);
    i0 = b;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 72u);
    StoreU32(ctx, i0 + 120u, i1);
    i0 = b;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 76u);
    l2 = i1;
    StoreU32(ctx, i0 + 124u, i1);
    i0 = b;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 80u);
    l6 = i1;
    StoreU32(ctx, i0 + 128u, i1);
    i0 = l6;
    i0 = !(i0);
    l6 = i0;
    if (i0) {
      i0 = b;
      i1 = l5;
      StoreU32(ctx, i0 + 120u, i1);
      goto block_538;
    }
    i0 = l2;
    i1 = l5;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    j1 = 0ull;
    StoreU64(ctx, i0 + 76u, j1);
    i0 = l3;
    i1 = l4;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = 0u;
    l2 = i0;
    block_538:;
    i0 = b;
    i0 = LoadS8(ctx, i0 + 143u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0 + 132u);
      DeallocateRawBlock(ctx, i0);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 76u);
      l2 = i0;
    }
    i0 = b;
    i1 = l3;
    j1 = LoadU64(ctx, i1 + 84u);
    StoreU64(ctx, i0 + 132u, j1);
    i0 = b;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 92u);
    StoreU32(ctx, i0 + 140u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 84u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 95u, i1);
    i0 = b;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 96u);
    l4 = i1;
    StoreU32(ctx, i0 + 144u, i1);
    i0 = l3;
    i1 = kF1042ConstB5F40;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    i1 = 72u;
    i0 += i1;
    i1 = l2;
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 71u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 60u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    b = i0;
  }
  i0 = l7;
  i1 = 94u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2720u;
    i1 = a;
    i2 = b;
    i0 = HostOsQuery(ctx, i0, i1, i2);
    l8 = i0;
    i0 = 94u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    a = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_11;}
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 251u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 240u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i1 = 272u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_12:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
    goto block_6;
  }
  block_11:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_6;
  }
  block_10:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_6;
  }
  block_9:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 44u);
    a = i0;
    i0 = !(i0);
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    if (i0) {goto block_549;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 4u);
    b = i1;
    i2 = 1u;
    i1 -= i2;
    l4 = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_549;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 95u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = a;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 95u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF1042ConstD015C;
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
    l4 = i1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_549;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    b = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 96u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = a;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 96u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_3;}
  }
  block_549:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 52u);
    a = i0;
    i0 = !(i0);
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    if (i0) {goto block_557;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 4u);
    b = i1;
    i2 = 1u;
    i1 -= i2;
    l4 = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_557;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 97u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = a;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 97u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_556;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 8u);
    b = i1;
    i2 = 1u;
    i1 -= i2;
    l4 = i1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_557;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    b = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 98u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = a;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 98u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_556;}
  }
  block_557:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
    i0 = l5;
    i0 = !(i0);
    a = i0;
    if (i0) {goto block_6;}
    i0 = l6;
    DeallocateRawBlock(ctx, i0);
    goto block_6;
  }
  block_556:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_3;}
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
    goto block_6;
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = l3;
    i1 = 240u;
    i0 += i1;
    l4 = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF1042ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    a = i0;
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 240u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l2;
    HostOsRelease(ctx, i0);
    goto running_exit;
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 99u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 857u;
    i1 = kF1042MsgPx;
    i2 = kF1042MsgBoostBoostSmartPtr;
    i3 = 734u;
    i4 = kF1042MsgOperator;
    HostFailAssertion(ctx, i0, i1, i2, i3, i4);
    i0 = 99u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF1042ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF1042ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 != i1;
    a = i0;
    if (i0) {goto running_exit;}
    goto block_3;
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
  i0 = l7;
  i1 = 100u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 100u;
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
  l8 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l8;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1042ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l8 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l8;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l8;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l8;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l8;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l8;
  i1 = l5;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l8;
  i1 = l6;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l8;
  i1 = l9;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l8;
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
}
