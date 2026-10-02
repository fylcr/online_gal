// auto_read.cpp -- the per-frame auto-read tick: it runs the onEnterFrame pass, applies the auto-mode wait, click-skip and avoid-status rules and refreshes the wasm sync progress fields..
//
// AutoReadTimer::Dispatch -- the per-frame auto-read tick: it runs the onEnterFrame pass, applies the auto-mode wait, click-skip and avoid-status rules and refreshes the wasm sync progress fields.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 1620199-1620199 (f3551); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void ReleaseChainedResourceNodes(EngineContext* ctx, uint32_t a, uint32_t b);
void ReleaseChainedNodeBranches(EngineContext* ctx, uint32_t a, uint32_t b);
void ApplyControlSkipStatus(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t InvokeVirtualSlot(EngineContext* ctx, uint32_t a0);
void ReleaseLayerRenderData(EngineContext* ctx, uint32_t a);
void EmplaceHashedEntry(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d);
void UnorderedRebuildBucketChain(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d);
void ApplyAvoidStatus(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t RebucketUnorderedNodeArray(EngineContext* ctx, uint32_t a);
void AdvanceGainStatStage(EngineContext* ctx, uint32_t a0);
void UnorderedInsertCheckedBucketNode(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f);
void UnorderedReleaseBucketTable(EngineContext* ctx, uint32_t a);
void ReleaseNodeSlotArray(EngineContext* ctx, uint32_t a);
void Panic(EngineContext* ctx, uint32_t a0);
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
void ReleaseNodeTree(EngineContext* ctx, uint32_t state, uint32_t node);
uint32_t CombineComparisonResults(EngineContext* ctx, uint32_t left, uint32_t right, uint32_t result, uint32_t box);
uint32_t AppendStringText(EngineContext* ctx, uint32_t text, uint32_t source);
void UnorderedLinkBucketNode(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f);
uint32_t InitTaggedNodeDescriptor(EngineContext* ctx, uint32_t a0);
uint32_t CopyScriptBlock(EngineContext* ctx, uint32_t a, uint32_t b);
void ReleaseEngineFrameResources(EngineContext* ctx, uint32_t a);
uint32_t RaiseNodeInitError(EngineContext* ctx, uint32_t a, uint32_t b);
uint32_t AdvanceMessageEffectTimer(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t AdvanceHttpFetchState(EngineContext* ctx, uint32_t a0);
void PauseFullscreenVideo(EngineContext* ctx, uint32_t a0);
uint32_t RebuildUnorderedBucketArray(EngineContext* ctx, uint32_t a, uint32_t b);
uint32_t FormatLuaEventFilterLine(EngineContext* ctx, uint32_t a0, uint32_t a1);
void RebuildUnorderedIndexTable(EngineContext* ctx, uint32_t a0);
void UnorderedInsertBucketNode(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f);
void ResetEventFilterState(EngineContext* ctx, uint32_t a);
void PersistSessionState(EngineContext* ctx, uint32_t a, uint32_t b);
void ApplyEventRecordTable(EngineContext* ctx, uint32_t a);
void FormatLuaTagFilterLine(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3);
void ApplyCommandSkipStatus(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
void ApplyAutoModeStatus(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
void CopyTaggedTreeKeyToText(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f);
void ResetGlobalSlotTable(EngineContext* ctx, uint32_t a);
void ReleaseSharedObjectHolder(EngineContext* ctx, uint32_t a);
uint32_t FindUnorderedValueSlot(EngineContext* ctx, uint32_t a, uint32_t b);
// PROVISIONAL nested-namespace forwarders (the dispatch products land later).
namespace MessageLayer {
  void RpDispatch(EngineContext* ctx, uint32_t a);
}

namespace {

constexpr uint32_t kF3551Const8C450 = 0x8C450u;  // 
constexpr uint32_t kF3551Const8C564 = 0x8C564u;  // 
constexpr uint32_t kF3551ConstAF3ED = 0xAF3EDu;  // 
constexpr uint32_t kF3551MsgAlreadyread = 0xAF636u;  // alreadyread
constexpr uint32_t kF3551MsgSAutomodewait = 0xAF9EDu;  // s.automodewait
constexpr uint32_t kF3551ConstAF9F3 = 0xAF9F3u;  // 
constexpr uint32_t kF3551MsgLuaOnenterframeSS = 0xAFA11u;  // [Lua] onEnterFrame %s: %s
constexpr uint32_t kF3551MsgSStatusAlreadyread = 0xAFA2Bu;  // s.status.alreadyread
constexpr uint32_t kF3551ConstAFA33 = 0xAFA33u;  // 
constexpr uint32_t kF3551ConstAFA3B = 0xAFA3Bu;  // 
constexpr uint32_t kF3551MsgLuaLuaondebugskipoutSS = 0xAFA40u;  // [Lua] luaOnDebugSkipOut %s: %s
constexpr uint32_t kF3551MsgSStatusScreendirection = 0xAFA5Fu;  // s.status.screendirection
constexpr uint32_t kF3551ConstAFA67 = 0xAFA67u;  // 
constexpr uint32_t kF3551ConstAFA6F = 0xAFA6Fu;  // 
constexpr uint32_t kF3551MsgSStatusScreendirectionprevious = 0xAFA78u;  // s.status.screendirectionprevious
constexpr uint32_t kF3551ConstAFA80 = 0xAFA80u;  // 
constexpr uint32_t kF3551ConstAFA88 = 0xAFA88u;  // 
constexpr uint32_t kF3551ConstAFA90 = 0xAFA90u;  // 
constexpr uint32_t kF3551MsgLeave = 0xAFA99u;  // leave
constexpr uint32_t kF3551ConstAFA9D = 0xAFA9Du;  // 
constexpr uint32_t kF3551MsgSClickskip = 0xAFA9Fu;  // s.clickskip
constexpr uint32_t kF3551ConstAFAA6 = 0xAFAA6u;  // 
constexpr uint32_t kF3551MsgSHttpCancel = 0xAFAABu;  // s.http.cancel
constexpr uint32_t kF3551ConstAFAB0 = 0xAFAB0u;  // 
constexpr uint32_t kF3551MsgSWasmSyncStatus = 0xAFAB9u;  // s.wasm_sync_status
constexpr uint32_t kF3551ConstAFAC1 = 0xAFAC1u;  // 
constexpr uint32_t kF3551ConstAFAC9 = 0xAFAC9u;  // 
constexpr uint32_t kF3551MsgSWasmSyncTotal = 0xAFACCu;  // s.wasm_sync_total
constexpr uint32_t kF3551ConstAFAD4 = 0xAFAD4u;  // 
constexpr uint32_t kF3551ConstAFADC = 0xAFADCu;  // 
constexpr uint32_t kF3551MsgSWasmSyncCurrent = 0xAFADEu;  // s.wasm_sync_current
constexpr uint32_t kF3551ConstAFAE6 = 0xAFAE6u;  // 
constexpr uint32_t kF3551ConstAFAED = 0xAFAEDu;  // 
constexpr uint32_t kF3551MsgBuckets = 0xB5B6Cu;  // buckets_
constexpr uint32_t kF3551MsgBoostBoostUnorderedDetail = 0xB5B75u;  // ../../../../boost_1_66_0/boost/unordered/detail/implementation.hpp
constexpr uint32_t kF3551MsgGetBucket = 0xB5BB8u;  // get_bucket
constexpr uint32_t kF3551ConstB5E30 = 0xB5E30u;  // 
constexpr uint32_t kF3551MsgPx = 0xB5E31u;  // px != 0
constexpr uint32_t kF3551MsgBoostBoostSmartPtr = 0xB5E39u;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
constexpr uint32_t kF3551MsgOperator = 0xB5E71u;  // operator->
constexpr uint32_t kF3551ConstB5F40 = 0xB5F40u;  // 
constexpr uint32_t kF3551MsgCallToEmptyBoost = 0xB699Bu;  // call to empty boost::function
constexpr uint32_t kF3551ConstB8F8C = 0xB8F8Cu;  // 
constexpr uint32_t kF3551ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF3551Const7FFFFFFF = 0x7FFFFFFFu;  // 
constexpr uint32_t kF3551ConstFFFFFFC0 = 0xFFFFFFC0u;  // 
constexpr uint32_t kF3551ConstFFFFFFFE = 0xFFFFFFFEu;  // 
constexpr uint32_t kF3551ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF3551SaveFrameBytes = 140u;
constexpr uint32_t kF3551ResumeWordBytes = 4u;

}  // namespace

namespace AutoReadTimer {
void Dispatch(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l13 = 0u; uint32_t l14 = 0u; uint32_t l15 = 0u; uint32_t l16 = 0u; uint32_t l17 = 0u; uint32_t l18 = 0u; uint32_t l19 = 0u; uint32_t l2 = 0u; uint32_t l20 = 0u; uint32_t l21 = 0u; uint32_t l22 = 0u; uint32_t l23 = 0u; uint32_t l24 = 0u; uint32_t l25 = 0u; uint32_t l26 = 0u; uint32_t l27 = 0u; uint32_t l28 = 0u; uint32_t l29 = 0u; uint32_t l3 = 0u; uint32_t l30 = 0u; uint32_t l31 = 0u; uint32_t l32 = 0u; uint32_t l33 = 0u; uint32_t l34 = 0u; uint32_t l35 = 0u; uint32_t l36 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u;
  uint64_t j1 = 0;
      l10 = 0, l11 = 0, l12 = 0, l13 = 0, l14 = 0, l15 = 0, l16 = 0, l17 = 0, 
      l18 = 0, l19 = 0, l20 = 0, l21 = 0, l22 = 0, l23 = 0, l24 = 0, l25 = 0, 
      l26 = 0, l27 = 0, l28 = 0, l29 = 0, l30 = 0, l31 = 0, l32 = 0, l33 = 0, 
      l34 = 0, l35 = 0, l36 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF3551SaveFrameBytes;
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
    l4 = i0;
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
    l9 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 32u);
    l10 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 36u);
    l11 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 40u);
    l12 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 44u);
    l13 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 48u);
    l14 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 52u);
    l15 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 56u);
    l16 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 60u);
    l17 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 64u);
    l18 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 68u);
    l19 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 72u);
    l20 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 76u);
    l21 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 80u);
    l22 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 84u);
    l23 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 88u);
    l24 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 92u);
    l25 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 96u);
    l26 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 100u);
    l27 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 104u);
    l28 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 108u);
    l29 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 112u);
    l30 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 116u);
    l31 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 120u);
    l32 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 124u);
    l33 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 128u);
    l34 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 132u);
    l35 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 136u);
    l36 = i0;
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
    i2 = kF3551ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    l2 = i0;
    i1 = 224u;
    i0 -= i1;
    l4 = i0;
    GuestStackSp(ctx) = i0;
    i0 = a;
    i1 = b;
    StoreU32(ctx, i0 + 144u, i1);
  }
  i0 = l3;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    RebuildUnorderedIndexTable(ctx, i0);
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
    i1 = 2876u;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    i1 = 9u;
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
      i0 = LoadU32(ctx, i0 + 8u);
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
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3551MsgPx;
        i2 = kF3551MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3551MsgOperator;
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
        i0 = kF3551ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_10;}
        goto block_7;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      l6 = i0;
    }
    i0 = l3;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = l6;
      CallTableT1(ctx, i1, i0);
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
      i0 = a;
      i0 = LoadU32(ctx, i0);
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
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l3;
      i1 = 3u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3551MsgPx;
        i2 = kF3551MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3551MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 3u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3551ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_10;}
        goto block_7;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      l6 = i0;
    }
    i0 = l3;
    i1 = 4u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = b;
      i2 = l6;
      CallTableT0(ctx, i2, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = 4u;
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
      i0 = kF3551ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l3;
    i1 = 5u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3551MsgPx;
      i2 = kF3551MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3551MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 5u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3551ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3551ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_10;}
      goto block_7;
    }
  }
  i0 = l3;
  i1 = 6u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    ReleaseEngineFrameResources(ctx, i0);
    i0 = 6u;
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
    StoreU16(ctx, i0 + 32u, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 14320u);
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
      i0 = kF3551ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l3;
    i1 = 7u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3551MsgPx;
      i2 = kF3551MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3551MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 7u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3551ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3551ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_10;}
      goto block_7;
    }
  }
  i0 = l3;
  i1 = 8u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    UnorderedReleaseBucketTable(ctx, i0);
    i0 = 8u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 14336u);
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
      i0 = kF3551ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l3;
    i1 = 9u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3551MsgPx;
      i2 = kF3551MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3551MsgOperator;
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
      i0 = kF3551ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3551ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_10;}
      goto block_7;
    }
  }
  i0 = l3;
  i1 = 10u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    UnorderedReleaseBucketTable(ctx, i0);
    i0 = 10u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 148u);
    l5 = i0;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 152u);
    i0 = i0 != i1;
    l2 = i0;
  }
  i0 = l2;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    loop_55: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        i0 = LoadU32(ctx, i0);
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
          i1 = 4u;
          i0 += i1;
          l6 = i0;
          i0 = l2;
          i1 = kF3551ConstFFFFFFFE;
          i0 &= i1;
          i0 = LoadU32(ctx, i0 + 4u);
          l2 = i0;
        }
        i0 = l3;
        i1 = 11u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l6;
          i1 = l2;
          CallTableT1(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
          SyncMemoryBase(ctx);
#endif
          i0 = 11u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l5;
          i1 = 16u;
          i0 += i1;
          l5 = i0;
          i1 = a;
          i1 = LoadU32(ctx, i1 + 152u);
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {goto loop_55;}
          goto block_54;
        }
      }
    i0 = a;
    i1 = l4;
    i2 = 128u;
    i1 += i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    a = i0;
    i0 = l3;
    i1 = 12u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = kF3551MsgCallToEmptyBoost;
      i0 = RaiseNodeInitError(ctx, i0, i1);
      l7 = i0;
      i0 = 12u;
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
      i0 = kF3551ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = kF3551Const8C450;
      StoreU32(ctx, i0 + 128u, i1);
      i0 = l4;
      i1 = 128u;
      i0 += i1;
      a = i0;
    }
    i0 = l3;
    i1 = 13u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1423u;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 13u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3551ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3551ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_10;}
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = b;
      i0 = InitTaggedNodeDescriptor(ctx, i0);
      goto block_5;
    }
    block_54:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 148u);
      l2 = i1;
      i0 = i0 != i1;
      l6 = i0;
    }
    i0 = l6;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      loop_69: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l5;
          i1 = 16u;
          i0 -= i1;
          l6 = i0;
          i0 = LoadU32(ctx, i0);
          l8 = i0;
        }
        i0 = l8;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = l12;
          i1 = l8;
          i2 = 1u;
          i1 &= i2;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          l12 = i0;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l12;
            if (i0) {goto block_73;}
            i0 = l8;
            i1 = kF3551ConstFFFFFFFE;
            i0 &= i1;
            i0 = LoadU32(ctx, i0);
            l8 = i0;
            i0 = !(i0);
            l12 = i0;
            if (i0) {goto block_73;}
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l5;
            i1 = 12u;
            i0 -= i1;
            l5 = i0;
          }
          i0 = l3;
          i1 = 14u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l8;
            i1 = l5;
            i2 = l5;
            i3 = 2u;
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
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            l5 = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l5;
            i1 = 1u;
            i0 = i0 == i1;
            l5 = i0;
            if (i0) {goto block_7;}
          }
          block_73:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l6;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l6;
          l5 = i0;
          i1 = l2;
          i0 = i0 != i1;
          l6 = i0;
          if (i0) {goto loop_69;}
        }
    }
    i0 = l5;
    i1 = l2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = l5;
    StoreU32(ctx, i0 + 152u, i1);
    i0 = a;
    i1 = 14367u;
    i0 += i1;
    i0 = LoadU8(ctx, i0);
    l2 = i0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    i1 = kF3551ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    if (i0) {
      i0 = a;
      i1 = 14360u;
      i0 += i1;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      if (i0) {goto block_83;}
      goto block_80;
    }
    i0 = l2;
    i0 = !(i0);
    l2 = i0;
    if (i0) {goto block_80;}
    block_83:;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 136u);
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
      i0 = kF3551ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l3;
    i1 = 15u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3551MsgPx;
      i2 = kF3551MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3551MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 15u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3551ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3551ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_10;}
      goto block_7;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    j1 = 0ull;
    StoreU64(ctx, i0 + 132u, j1);
    i0 = kF3551ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = l4;
    i2 = 128u;
    i1 += i2;
    i2 = 4u;
    i1 |= i2;
    StoreU32(ctx, i0 + 128u, i1);
    i0 = l2;
    l6 = i0;
    i0 = l4;
    i1 = 128u;
    i0 += i1;
    l5 = i0;
    i0 = a;
    i1 = 14356u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l3;
  i1 = 16u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2595u;
    i1 = l6;
    i2 = l2;
    i3 = l5;
    i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
    l7 = i0;
    i0 = 16u;
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
    i0 = kF3551ConstD015C;
    i0 = LoadU32(ctx, i0);
    l6 = i0;
    i0 = kF3551ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l6;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_9;}
    i0 = l4;
    i1 = 128u;
    i0 += i1;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 132u);
    l8 = i1;
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l5;
    i0 = !(i0);
    l6 = i0;
    if (i0) {goto block_80;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 136u);
    l5 = i0;
    i0 = !(i0);
    l8 = i0;
    i0 = l2;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF3551ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      goto block_92;
    }
    i0 = l2;
    block_92:;
    l6 = i0;
  }
  i0 = l8;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3551ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l3;
    i1 = 17u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3551MsgPx;
      i2 = kF3551MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3551MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 17u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3551ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3551ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_10;}
      goto block_7;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = 4u;
    i0 += i1;
    l8 = i0;
    i0 = l5;
    i0 = LoadS8(ctx, i0 + 15u);
    i1 = kF3551ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    l9 = i0;
    if (i0) {
      i0 = l5;
      i0 = LoadU32(ctx, i0 + 4u);
      l8 = i0;
    }
    i0 = l4;
    l12 = i0;
    i1 = l8;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = l4;
    i1 = l6;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = l4;
    i1 = 16u;
    i0 += i1;
    l6 = i0;
  }
  i0 = l3;
  i1 = 18u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 1u;
    i2 = kF3551MsgLuaOnenterframeSS;
    i3 = l6;
    FormatLuaTagFilterLine(ctx, i0, i1, i2, i3);
    i0 = 18u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadS8(ctx, i0 + 14367u);
    i1 = kF3551ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    l6 = i0;
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 14356u);
      i1 = 0u;
      StoreU8(ctx, i0, i1);
      i0 = a;
      i1 = 14360u;
      i0 += i1;
      l2 = i0;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      goto block_80;
    }
    i0 = l2;
    i1 = 0u;
    StoreU8(ctx, i0 + 11u, i1);
    i0 = l2;
    i1 = 0u;
    StoreU8(ctx, i0, i1);
  }
  block_80:;
  i0 = l3;
  i1 = 19u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    AdvanceGainStatStage(ctx, i0);
    i0 = 19u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU8(ctx, i0 + 35u);
    i0 = !(i0);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {goto block_105;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 76u);
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
      i0 = kF3551ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l3;
    i1 = 20u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3551MsgPx;
      i2 = kF3551MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3551MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 20u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3551ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3551ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_10;}
      goto block_7;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 10u;
    i1 = 11u;
    i2 = a;
    i2 = LoadU8(ctx, i2 + 13272u);
    i0 = i2 ? i0 : i1;
    l6 = i0;
  }
  i0 = l3;
  i1 = 21u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = l6;
    i0 = RebuildUnorderedBucketArray(ctx, i0, i1);
    l7 = i0;
    i0 = 21u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i0 = !(i0);
    l2 = i0;
    if (i0) {goto block_105;}
  }
  i0 = l3;
  i1 = 22u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 0u;
    i2 = 0u;
    ApplyAutoModeStatus(ctx, i0, i1, i2);
    i0 = 22u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_8;}
  block_105:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU8(ctx, i0 + 49u);
    i0 = !(i0);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {goto block_117;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 76u);
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
      i0 = kF3551ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l3;
    i1 = 23u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3551MsgPx;
      i2 = kF3551MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3551MsgOperator;
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
      i0 = kF3551ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3551ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_10;}
      goto block_7;
    }
  }
  i0 = l3;
  i1 = 24u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = 13u;
    i0 = RebuildUnorderedBucketArray(ctx, i0, i1);
    l7 = i0;
    i0 = 24u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i0 = !(i0);
    l2 = i0;
    if (i0) {goto block_117;}
  }
  i0 = l3;
  i1 = 25u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 0u;
    i2 = 0u;
    ApplyCommandSkipStatus(ctx, i0, i1, i2);
    i0 = 25u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_8;}
  block_117:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU8(ctx, i0 + 50u);
    i0 = !(i0);
    l6 = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 76u);
    l2 = i0;
  }
  i0 = l6;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l6;
    i1 = l2;
    i1 = !(i1);
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l6 = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l3;
      i1 = 26u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3551MsgPx;
        i2 = kF3551MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3551MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 26u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3551ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_10;}
        goto block_7;
      }
    }
    i0 = l3;
    i1 = 27u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i0 = RebucketUnorderedNodeArray(ctx, i0);
      l7 = i0;
      i0 = 27u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i0 = !(i0);
      l2 = i0;
      if (i0) {goto block_128;}
    }
    i0 = l3;
    i1 = 28u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = 1u;
      i2 = 0u;
      ApplyControlSkipStatus(ctx, i0, i1, i2);
      i0 = 28u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_128;}
  }
  i0 = l6;
  i1 = l2;
  i1 = !(i1);
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l6 = i0;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3551ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l3;
    i1 = 29u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3551MsgPx;
      i2 = kF3551MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3551MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 29u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3551ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3551ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_10;}
      goto block_7;
    }
  }
  i0 = l3;
  i1 = 30u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i0 = RebucketUnorderedNodeArray(ctx, i0);
    l7 = i0;
    i0 = 30u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  i1 = 0u;
  i2 = l2;
  i0 = i2 ? i0 : i1;
  if (i0) {goto block_128;}
  i0 = l3;
  i1 = 31u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 0u;
    i2 = 0u;
    ApplyControlSkipStatus(ctx, i0, i1, i2);
    i0 = 31u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_128:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = 13303u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    l2 = i0;
    i1 = kF3551ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    l6 = i0;
    if (i0) {
      i0 = a;
      i1 = 13296u;
      i0 += i1;
      i0 = LoadU32(ctx, i0);
      goto block_144;
    }
    i0 = l2;
    i1 = 255u;
    i0 &= i1;
    block_144:;
    i0 = !(i0);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {goto block_146;}
    i0 = a;
    i0 = LoadU8(ctx, i0 + 51u);
    i0 = !(i0);
    l6 = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 76u);
    l2 = i0;
  }
  i0 = l6;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l6;
    i1 = l2;
    i1 = !(i1);
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l6 = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l3;
      i1 = 32u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3551MsgPx;
        i2 = kF3551MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3551MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 32u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3551ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_10;}
        goto block_7;
      }
    }
    i0 = l3;
    i1 = 33u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = 15u;
      i0 = RebuildUnorderedBucketArray(ctx, i0, i1);
      l7 = i0;
      i0 = 33u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i0 = !(i0);
      l2 = i0;
      if (i0) {goto block_146;}
    }
    i0 = l3;
    i1 = 34u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = 1u;
      ApplyAvoidStatus(ctx, i0, i1);
      i0 = 34u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_8;}
  }
  i0 = b;
  i1 = l2;
  i1 = !(i1);
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  b = i0;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3551ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l3;
    i1 = 35u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3551MsgPx;
      i2 = kF3551MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3551MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 35u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3551ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3551ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_10;}
      goto block_7;
    }
  }
  i0 = l3;
  i1 = 36u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = 16u;
    i0 = RebuildUnorderedBucketArray(ctx, i0, i1);
    l7 = i0;
    i0 = 36u;
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
    b = i0;
    if (i0) {goto block_8;}
  }
  i0 = l3;
  i1 = 37u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 0u;
    ApplyAvoidStatus(ctx, i0, i1);
    i0 = 37u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_8;}
  block_146:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    l8 = i0;
    i0 = a;
    i1 = 2796u;
    i0 += i1;
    l32 = i0;
    i0 = a;
    i1 = 14344u;
    i0 += i1;
    l33 = i0;
    i0 = a;
    i1 = 14116u;
    i0 += i1;
    l34 = i0;
    i0 = a;
    i1 = 13628u;
    i0 += i1;
    l21 = i0;
    i0 = a;
    i1 = 13572u;
    i0 += i1;
    l22 = i0;
    i0 = a;
    i1 = 14028u;
    i0 += i1;
    l35 = i0;
    i0 = a;
    i1 = 2936u;
    i0 += i1;
    l16 = i0;
    i0 = a;
    i1 = 14260u;
    i0 += i1;
    l36 = i0;
    i0 = a;
    i1 = 13756u;
    i0 += i1;
    l23 = i0;
    i0 = a;
    i1 = 13744u;
    i0 += i1;
    l24 = i0;
    i0 = a;
    i1 = 13620u;
    i0 += i1;
    l17 = i0;
    i0 = a;
    i1 = 13656u;
    i0 += i1;
    l14 = i0;
    i0 = a;
    i1 = 13560u;
    i0 += i1;
    l25 = i0;
    i0 = a;
    i1 = 13308u;
    i0 += i1;
    l11 = i0;
    i0 = a;
    i1 = 13648u;
    i0 += i1;
    l18 = i0;
    i0 = a;
    i1 = 2732u;
    i0 += i1;
    l26 = i0;
    i0 = a;
    i1 = 2764u;
    i0 += i1;
    l15 = i0;
    i0 = a;
    i1 = 2684u;
    i0 += i1;
    l20 = i0;
    i0 = a;
    i1 = 2752u;
    i0 += i1;
    l12 = i0;
    i0 = l4;
    i1 = 192u;
    i0 += i1;
    l27 = i0;
    i0 = l4;
    i1 = 144u;
    i0 += i1;
    l28 = i0;
    i0 = l4;
    i1 = 96u;
    i0 += i1;
    l29 = i0;
    i0 = 0u;
    a = i0;
  }
  loop_165: 
    i0 = l6;
    i1 = a;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l6 = i0;
    loop_169: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        i1 = 1u;
        i0 &= i1;
        a = i0;
        if (i0) {
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 13212u);
          i0 = !(i0);
          a = i0;
          if (i0) {goto block_8;}
        }
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 2876u);
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        if (i0) {
          i0 = l6;
          a = i0;
          i0 = l2;
          i1 = 1u;
          i0 -= i1;
          l5 = i0;
          switch (i0) {
            case 0: goto block_178;
            case 1: goto block_179;
            case 2: goto block_178;
            case 3: goto block_178;
            case 4: goto block_180;
            case 5: goto block_181;
            case 6: goto block_182;
            case 7: goto block_183;
            case 8: goto block_185;
            case 9: goto block_185;
            case 10: goto block_185;
            case 11: goto block_184;
            case 12: goto block_185;
            case 13: goto block_8;
            default: goto loop_165;
          }
        }
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 2828u);
        a = i0;
        if (i0) {goto block_173;}
        i0 = l12;
        i0 = LoadS8(ctx, i0 + 11u);
        a = i0;
        i1 = kF3551ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        l2 = i0;
        if (i0) {
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 2756u);
          goto block_190;
        }
        i0 = a;
        i1 = 255u;
        i0 &= i1;
        block_190:;
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
          i0 = l4;
          i1 = l12;
          StoreU32(ctx, i0 + 72u, i1);
          i0 = l4;
          i1 = 72u;
          i0 += i1;
          l2 = i0;
          i0 = l4;
          i1 = kF3551ConstFFFFFFC0;
          i0 -= i1;
          l5 = i0;
          i0 = l4;
          i1 = 128u;
          i0 += i1;
          a = i0;
        }
        i0 = l3;
        i1 = 38u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = a;
          i1 = l20;
          i2 = l12;
          i3 = kF3551ConstB5E30;
          i4 = l2;
          i5 = l5;
          UnorderedLinkBucketNode(ctx, i0, i1, i2, i3, i4, i5);
          i0 = 38u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l15;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i1 = l4;
          i1 = LoadU32(ctx, i1 + 128u);
          a = i1;
          i1 = LoadU32(ctx, i1 + 32u);
          l5 = i1;
          i2 = a;
          i2 = LoadU32(ctx, i2 + 28u);
          i1 -= i2;
          i2 = 2u;
          i1 = (uint32_t)((int32_t)i1 >> (i2 & 31));
          i0 = i0 < i1;
          a = i0;
          if (i0) {goto block_186;}
        }
      }
      i0 = l3;
      i1 = 39u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l8;
        i0 = InvokeVirtualSlot(ctx, i0);
        l7 = i0;
        i0 = 39u;
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
        i0 = a;
        i0 = !(i0);
        a = i0;
        if (i0) {goto loop_169;}
      }
      i0 = l3;
      i1 = 40u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l8;
        i1 = 14u;
        PersistSessionState(ctx, i0, i1);
        i0 = 40u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_8;}
      block_186:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU8(ctx, i0 + 2744u);
        i0 = !(i0);
        a = i0;
        if (i0) {goto block_168;}
        i0 = l4;
        i1 = l12;
        StoreU32(ctx, i0 + 72u, i1);
        i0 = l4;
        i1 = 72u;
        i0 += i1;
        l2 = i0;
        i0 = l4;
        i1 = kF3551ConstFFFFFFC0;
        i0 -= i1;
        l5 = i0;
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        a = i0;
      }
      i0 = l3;
      i1 = 41u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l20;
        i2 = l12;
        i3 = kF3551ConstB5E30;
        i4 = l2;
        i5 = l5;
        UnorderedLinkBucketNode(ctx, i0, i1, i2, i3, i4, i5);
        i0 = 41u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 128u);
        i0 = LoadU32(ctx, i0 + 28u);
        i1 = l15;
        i1 = LoadU32(ctx, i1);
        i2 = 2u;
        i1 <<= (i2 & 31);
        i0 += i1;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = LoadU32(ctx, i0 + 8u);
        l5 = i0;
        i0 = a;
        i0 = LoadU8(ctx, i0 + 15u);
        l2 = i0;
        i1 = 24u;
        i0 <<= (i1 & 31);
        i1 = 24u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l9 = i0;
        i0 = l5;
        i1 = l2;
        i2 = l9;
        i0 = i2 ? i0 : i1;
        i1 = 11u;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 4u;
          i0 += i1;
          a = i0;
        }
        i0 = l3;
        i1 = 42u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1354u;
          i1 = a;
          i2 = 0u;
          i3 = kF3551ConstFFFFFFFF;
          i4 = kF3551MsgAlreadyread;
          i5 = 11u;
          i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
          l7 = i0;
          i0 = 42u;
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
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 == i1;
          l2 = i0;
          if (i0) {goto block_201;}
          i0 = a;
          i0 = !(i0);
          a = i0;
          if (i0) {goto block_168;}
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i1 = l12;
        StoreU32(ctx, i0 + 72u, i1);
        i0 = l4;
        i1 = 72u;
        i0 += i1;
        l2 = i0;
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        a = i0;
      }
      i0 = l3;
      i1 = 43u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l26;
        i2 = l12;
        i3 = l2;
        UnorderedRebuildBucketChain(ctx, i0, i1, i2, i3);
        i0 = 43u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l15;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i1 = l4;
        i1 = LoadU32(ctx, i1 + 128u);
        l2 = i1;
        i1 = LoadU32(ctx, i1 + 32u);
        i0 = REM_U(i0, i1);
        l9 = i0;
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 36u);
        i0 = !(i0);
        l5 = i0;
        if (i0) {goto block_172;}
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 48u);
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
          i0 = l9;
          i1 = 2u;
          i0 <<= (i1 & 31);
          i1 = l2;
          i0 += i1;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = !(i0);
          l5 = i0;
          if (i0) {goto block_172;}
          i0 = l2;
          i0 = LoadU32(ctx, i0);
          l5 = i0;
          i0 = !(i0);
          l2 = i0;
          if (i0) {goto block_172;}
          i0 = l5;
          i0 = LoadU32(ctx, i0 + 8u);
          i1 = a;
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {
            i0 = l5;
            i0 = LoadU32(ctx, i0 + 4u);
            l2 = i0;
            loop_216: 
              i0 = l9;
              i1 = l2;
              i2 = kF3551Const7FFFFFFF;
              i1 &= i2;
              i0 = i0 != i1;
              l2 = i0;
              if (i0) {goto block_172;}
              loop_217: 
                i0 = l5;
                i0 = LoadU32(ctx, i0);
                l5 = i0;
                i0 = !(i0);
                l2 = i0;
                if (i0) {goto block_172;}
                i0 = l5;
                i0 = LoadU32(ctx, i0 + 4u);
                l2 = i0;
                i1 = 0u;
                i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
                l10 = i0;
                if (i0) {goto loop_217;}
              i0 = l5;
              i0 = LoadU32(ctx, i0 + 8u);
              i1 = a;
              i0 = i0 != i1;
              l10 = i0;
              if (i0) {goto loop_216;}
          }
          i0 = l8;
          i0 = LoadU8(ctx, i0 + 2745u);
          a = i0;
          if (i0) {goto block_168;}
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 112u);
          l2 = i0;
          i0 = !(i0);
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
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 44u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kF3551MsgPx;
            i2 = kF3551MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF3551MsgOperator;
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
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_10;}
            goto block_7;
          }
        }
        i0 = l3;
        i1 = 45u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 32u;
          i0 = AllocateStringBuffer(ctx, i0);
          l7 = i0;
          i0 = 45u;
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
          i0 = l4;
          i1 = a;
          StoreU32(ctx, i0 + 128u, i1);
          i0 = l4;
          j1 = 9223372174293729300ull;
          StoreU64(ctx, i0 + 132u, j1);
          i0 = a;
          i1 = 0u;
          StoreU8(ctx, i0 + 20u, i1);
          i0 = a;
          i1 = kF3551ConstAFA3B;
          i1 = LoadU32(ctx, i1);
          StoreU32(ctx, i0 + 16u, i1);
          i0 = a;
          i1 = kF3551ConstAFA33;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0 + 8u, j1);
          i0 = a;
          i1 = kF3551MsgSStatusAlreadyread;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0, j1);
          i0 = l2;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 20u);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = 128u;
          i0 += i1;
          l5 = i0;
        }
        i0 = l3;
        i1 = 46u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = a;
          i1 = l2;
          i2 = l5;
          i3 = 1u;
          i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
          l7 = i0;
          i0 = 46u;
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
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          if (i0) {
            i0 = l4;
            i0 = LoadS8(ctx, i0 + 139u);
            i1 = kF3551ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            a = i0;
            if (i0) {
              i0 = l4;
              i0 = LoadU32(ctx, i0 + 128u);
              a = i0;
              DeallocateRawBlock(ctx, i0);
            }
            i0 = l8;
            i1 = 1u;
            StoreU8(ctx, i0 + 2745u, i1);
            goto block_168;
          }
          i0 = HostRet0(ctx);
          a = i0;
          i0 = HostTempRet0(ctx);
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 139u);
          i1 = kF3551ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          if (i0) {goto block_164;}
          goto block_6;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3551MsgBuckets;
        i1 = kF3551MsgBoostBoostUnorderedDetail;
        i2 = 2956u;
        i3 = kF3551MsgGetBucket;
        HostAssertFail(ctx, i0, i1, i2, i3);
        TrapUnreachable();
      }
      block_201:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_7;}
      block_185:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = 1u;
        a = i0;
        goto loop_165;
      }
      block_184:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 47u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 47u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 8u);
        l2 = i0;
      }
      i0 = l3;
      i1 = 48u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l2;
        i0 = CallTableT2(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        l7 = i0;
        i0 = 48u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l7;
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 112u);
        l5 = i0;
        i0 = !(i0);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 49u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 49u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = l3;
      i1 = 50u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 32u;
        i0 = AllocateStringBuffer(ctx, i0);
        l7 = i0;
        i0 = 50u;
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
        i0 = l4;
        i1 = a;
        StoreU32(ctx, i0 + 128u, i1);
        i0 = l4;
        j1 = 9223372174293729298ull;
        StoreU64(ctx, i0 + 132u, j1);
        i0 = a;
        i1 = 0u;
        StoreU8(ctx, i0 + 18u, i1);
        i0 = a;
        i1 = kF3551ConstAFAC9;
        i1 = LoadU16(ctx, i1);
        StoreU16(ctx, i0 + 16u, i1);
        i0 = a;
        i1 = kF3551ConstAFAC1;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0 + 8u, j1);
        i0 = a;
        i1 = kF3551MsgSWasmSyncStatus;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0, j1);
        i0 = l5;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 20u);
        a = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        l6 = i0;
      }
      i0 = l3;
      i1 = 51u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l5;
        i2 = l6;
        i3 = l2;
        i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
        l7 = i0;
        i0 = 51u;
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
        i0 = kF3551ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
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
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 139u);
          i1 = kF3551ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 128u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = 1u;
          a = i0;
          i0 = l2;
          i1 = 6u;
          i0 = i0 > i1;
          l6 = i0;
          if (i0) {goto loop_165;}
          i0 = 1u;
          i1 = l2;
          i0 <<= (i1 & 31);
          l2 = i0;
          i1 = 81u;
          i0 &= i1;
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
            i0 = l2;
            i1 = 40u;
            i0 &= i1;
            i0 = !(i0);
            l2 = i0;
            if (i0) {goto loop_165;}
            i0 = l4;
            i1 = 0u;
            StoreU32(ctx, i0 + 72u, i1);
            i0 = l4;
            i1 = 0u;
            StoreU32(ctx, i0 + 64u, i1);
            i0 = l8;
            i0 = LoadU32(ctx, i0 + 24u);
            l2 = i0;
            if (i0) {goto block_247;}
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 52u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kF3551MsgPx;
            i2 = kF3551MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF3551MsgOperator;
            HostFailAssertion(ctx, i0, i1, i2, i3, i4);
            i0 = 52u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_177;}
            goto block_7;
          }
        }
        i0 = l3;
        i1 = 53u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l8;
          ResetEventFilterState(ctx, i0);
          i0 = 53u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto loop_165;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        a = i0;
        i0 = HostTempRet0(ctx);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 139u);
        i1 = kF3551ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {goto block_164;}
        goto block_6;
      }
      block_247:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i1 = kF3551ConstFFFFFFC0;
        i0 -= i1;
        l5 = i0;
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 24u);
        l9 = i0;
        i0 = l4;
        i1 = 72u;
        i0 += i1;
        l6 = i0;
      }
      i0 = l3;
      i1 = 54u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l6;
        i2 = l5;
        i3 = l9;
        CallTableT5(ctx, i3, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 54u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 112u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 55u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
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
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = l3;
      i1 = 56u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 32u;
        i0 = AllocateStringBuffer(ctx, i0);
        l7 = i0;
        i0 = 56u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l7;
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i1 = l2;
        StoreU32(ctx, i0 + 128u, i1);
        i0 = l4;
        j1 = 9223372174293729297ull;
        StoreU64(ctx, i0 + 132u, j1);
        i0 = l2;
        i1 = 0u;
        StoreU8(ctx, i0 + 17u, i1);
        i0 = l2;
        i1 = kF3551ConstAFADC;
        i1 = LoadU8(ctx, i1);
        StoreU8(ctx, i0 + 16u, i1);
        i0 = l2;
        i1 = kF3551ConstAFAD4;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0 + 8u, j1);
        i0 = l2;
        i1 = kF3551MsgSWasmSyncTotal;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0, j1);
        i0 = l5;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 20u);
        l2 = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 72u);
        l9 = i0;
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        l6 = i0;
      }
      i0 = l3;
      i1 = 57u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l5;
        i2 = l6;
        i3 = l9;
        i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
        l7 = i0;
        i0 = 57u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l7;
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3551ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3551ConstD015C;
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
          i0 = LoadS8(ctx, i0 + 139u);
          i1 = kF3551ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 128u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 112u);
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
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 58u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kF3551MsgPx;
            i2 = kF3551MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF3551MsgOperator;
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
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_177;}
            goto block_7;
          }
        }
        i0 = l3;
        i1 = 59u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 32u;
          i0 = AllocateStringBuffer(ctx, i0);
          l7 = i0;
          i0 = 59u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l7;
          l2 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i1 = l2;
          StoreU32(ctx, i0 + 128u, i1);
          i0 = l4;
          j1 = 9223372174293729299ull;
          StoreU64(ctx, i0 + 132u, j1);
          i0 = l2;
          i1 = 0u;
          StoreU8(ctx, i0 + 19u, i1);
          i0 = l2;
          i1 = kF3551ConstAFAED;
          i1 = LoadU32(ctx, i1);
          StoreU32(ctx, i0 + 15u, i1);
          i0 = l2;
          i1 = kF3551ConstAFAE6;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0 + 8u, j1);
          i0 = l2;
          i1 = kF3551MsgSWasmSyncCurrent;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0, j1);
          i0 = l5;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 20u);
          l2 = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 64u);
          l9 = i0;
          i0 = l4;
          i1 = 128u;
          i0 += i1;
          l6 = i0;
        }
        i0 = l3;
        i1 = 60u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l2;
          i1 = l5;
          i2 = l6;
          i3 = l9;
          i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
          l7 = i0;
          i0 = 60u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l7;
          l2 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 == i1;
          l2 = i0;
          if (i0) {goto block_270;}
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 139u);
          i1 = kF3551ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          l2 = i0;
          if (i0) {goto loop_165;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 128u);
          l2 = i0;
          DeallocateRawBlock(ctx, i0);
          goto loop_165;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        a = i0;
        i0 = HostTempRet0(ctx);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 139u);
        i1 = kF3551ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {goto block_164;}
        goto block_6;
      }
      block_270:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        a = i0;
        i0 = HostTempRet0(ctx);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 139u);
        i1 = kF3551ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {goto block_164;}
        goto block_6;
      }
      block_183:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 112u);
        l2 = i0;
        i0 = !(i0);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 61u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 61u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = l3;
      i1 = 62u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 16u;
        i0 = AllocateStringBuffer(ctx, i0);
        l7 = i0;
        i0 = 62u;
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
        i0 = l4;
        i1 = a;
        StoreU32(ctx, i0 + 128u, i1);
        i0 = l4;
        j1 = 9223372105574252557ull;
        StoreU64(ctx, i0 + 132u, j1);
        i0 = a;
        i1 = 0u;
        StoreU8(ctx, i0 + 13u, i1);
        i0 = a;
        i1 = kF3551ConstAFAB0;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0 + 5u, j1);
        i0 = a;
        i1 = kF3551MsgSHttpCancel;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0, j1);
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        a = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        l6 = i0;
      }
      i0 = l3;
      i1 = 63u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l2;
        i2 = l6;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l7 = i0;
        i0 = 63u;
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
        i0 = kF3551ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3551ConstD015C;
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
          i0 = LoadS8(ctx, i0 + 139u);
          i1 = kF3551ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 128u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 14440u);
          l2 = i0;
          i0 = a;
          if (i0) {goto block_295;}
          i0 = l2;
          i0 = !(i0);
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
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 64u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kF3551MsgPx;
            i2 = kF3551MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF3551MsgOperator;
            HostFailAssertion(ctx, i0, i1, i2, i3, i4);
            i0 = 64u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_177;}
            goto block_7;
          }
        }
        i0 = a;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        a = i0;
        i0 = l3;
        i1 = 65u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l2;
          i0 = AdvanceHttpFetchState(ctx, i0);
          l7 = i0;
          i0 = 65u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l7;
          l2 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          i0 = !(i0);
          l2 = i0;
          if (i0) {goto loop_165;}
        }
        i0 = l3;
        i1 = 66u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l8;
          ResetEventFilterState(ctx, i0);
          i0 = 66u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto loop_165;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        a = i0;
        i0 = HostTempRet0(ctx);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 139u);
        i1 = kF3551ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {goto block_164;}
        goto block_6;
      }
      block_295:;
      i0 = a;
      i1 = l2;
      i1 = !(i1);
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      a = i0;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 67u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
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
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i1 = 0u;
        StoreU32(ctx, i0 + 12u, i1);
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 16u);
        a = i0;
        i0 = l2;
        i1 = 0u;
        StoreU32(ctx, i0 + 16u, i1);
        i0 = a;
        i0 = !(i0);
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        if (i0) {goto block_315;}
        i0 = a;
        i1 = a;
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
        if (i0) {goto block_315;}
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 8u);
        l2 = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l3;
      i1 = 68u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = a;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 68u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3551ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        l2 = i0;
        if (i0) {goto block_314;}
        i0 = a;
        i1 = a;
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
        if (i0) {goto block_315;}
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        l2 = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l3;
      i1 = 69u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = a;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 69u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3551ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 == i1;
        a = i0;
        if (i0) {goto block_314;}
      }
      block_315:;
      i0 = l3;
      i1 = 70u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l8;
        ResetEventFilterState(ctx, i0);
        i0 = 70u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = 1u;
        a = i0;
        goto loop_165;
      }
      block_314:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = 0u;
        i0 = FindMatchingCatch(ctx, i0);
        a = i0;
        i0 = HostTempRet0(ctx);
        b = i0;
      }
      i0 = l3;
      i1 = 71u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        Panic(ctx, i0);
        i0 = 71u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_177;}
      block_182:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 72u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 72u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 132u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 73u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
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
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU8(ctx, i0 + 28u);
        i0 = !(i0);
        a = i0;
      }
      i0 = a;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = l3;
        i1 = 74u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l8;
          ResetEventFilterState(ctx, i0);
          i0 = 74u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 14448u);
        l5 = i0;
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
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 76u);
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
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 75u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kF3551MsgPx;
            i2 = kF3551MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF3551MsgOperator;
            HostFailAssertion(ctx, i0, i1, i2, i3, i4);
            i0 = 75u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_177;}
            goto block_7;
          }
        }
        i0 = l3;
        i1 = 76u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = a;
          i1 = 0u;
          i0 = RebuildUnorderedBucketArray(ctx, i0, i1);
          l7 = i0;
          i0 = 76u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l7;
          a = i0;
        }
        i0 = ResumePhase(ctx);
        if (i0) {
          i0 = l5;
        } else {
          i0 = a;
          if (i0) {goto block_341;}
          i0 = l8;
          i0 = LoadU8(ctx, i0 + 48u);
          if (i0) {
            i0 = l8;
            i0 = LoadU8(ctx, i0 + 49u);
            a = i0;
            if (i0) {goto block_341;}
            i0 = l8;
            i0 = LoadU8(ctx, i0 + 50u);
            a = i0;
            if (i0) {goto block_341;}
          }
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 13212u);
          a = i0;
          if (i0) {goto block_341;}
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 14448u);
        }
        l5 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = 1u;
        a = i0;
        i0 = l5;
        i1 = 2u;
        i0 = i0 != i1;
        l2 = i0;
        if (i0) {goto loop_165;}
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 76u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 77u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 77u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = l3;
      i1 = 78u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = 1u;
        i0 = RebuildUnorderedBucketArray(ctx, i0, i1);
        l7 = i0;
        i0 = 78u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l7;
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = !(i0);
        l2 = i0;
        if (i0) {goto loop_165;}
      }
      block_341:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 79u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 79u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 132u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 80u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 80u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = l3;
      i1 = 81u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        PauseFullscreenVideo(ctx, i0);
        i0 = 81u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = l3;
      i1 = 82u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l8;
        ResetEventFilterState(ctx, i0);
        i0 = 82u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 112u);
        l2 = i0;
        i0 = !(i0);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 83u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 83u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = l3;
      i1 = 84u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 16u;
        i0 = AllocateStringBuffer(ctx, i0);
        l7 = i0;
        i0 = 84u;
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
        i0 = l4;
        i1 = a;
        StoreU32(ctx, i0 + 128u, i1);
        i0 = l4;
        j1 = 9223372105574252555ull;
        StoreU64(ctx, i0 + 132u, j1);
        i0 = a;
        i1 = 0u;
        StoreU8(ctx, i0 + 11u, i1);
        i0 = a;
        i1 = kF3551ConstAFAA6;
        i1 = LoadU32(ctx, i1);
        StoreU32(ctx, i0 + 7u, i1);
        i0 = a;
        i1 = kF3551MsgSClickskip;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0, j1);
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 20u);
        a = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        l6 = i0;
      }
      i0 = l3;
      i1 = 85u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l2;
        i2 = l6;
        i3 = 1u;
        i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
        l7 = i0;
        i0 = 85u;
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
        i0 = kF3551ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        if (i0) {
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 139u);
          i1 = kF3551ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 128u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = 1u;
          a = i0;
          goto loop_165;
        }
        i0 = HostRet0(ctx);
        a = i0;
        i0 = HostTempRet0(ctx);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 139u);
        i1 = kF3551ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {goto block_164;}
        goto block_6;
      }
      block_181:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 76u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 86u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
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
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = l3;
      i1 = 87u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = 6u;
        i0 = RebuildUnorderedBucketArray(ctx, i0, i1);
        l7 = i0;
        i0 = 87u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l7;
        a = i0;
      }
      i0 = a;
      i1 = a;
      i1 = !(i1);
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      a = i0;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 120u);
          l2 = i0;
          i0 = !(i0);
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
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 88u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kF3551MsgPx;
            i2 = kF3551MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF3551MsgOperator;
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
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_177;}
            goto block_7;
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 76u);
          a = i0;
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
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 89u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kF3551MsgPx;
            i2 = kF3551MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF3551MsgOperator;
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
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_177;}
            goto block_7;
          }
        }
        i0 = l3;
        i1 = 90u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = a;
          i1 = 7u;
          i0 = RebuildUnorderedBucketArray(ctx, i0, i1);
          l7 = i0;
          i0 = 90u;
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
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 76u);
          l6 = i0;
          i0 = !(i0);
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
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 91u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kF3551MsgPx;
            i2 = kF3551MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF3551MsgOperator;
            HostFailAssertion(ctx, i0, i1, i2, i3, i4);
            i0 = 91u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_177;}
            goto block_7;
          }
        }
        i0 = a;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        a = i0;
        i0 = l3;
        i1 = 92u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l6;
          i1 = 8u;
          i0 = RebuildUnorderedBucketArray(ctx, i0, i1);
          l7 = i0;
          i0 = 92u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l7;
          l6 = i0;
        }
        i0 = l3;
        i1 = 93u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l2;
          i1 = l5;
          i2 = l6;
          i0 = AdvanceMessageEffectTimer(ctx, i0, i1, i2);
          l7 = i0;
          i0 = 93u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l7;
          l2 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          i0 = !(i0);
          l2 = i0;
          if (i0) {goto loop_165;}
        }
      }
      i0 = l3;
      i1 = 94u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l8;
        ResetEventFilterState(ctx, i0);
        i0 = 94u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 95u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 95u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 96u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 96u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 396u);
        l2 = i0;
      }
      i0 = l3;
      i1 = 97u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = 0u;
        i2 = l2;
        CallTableT0(ctx, i2, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 97u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l18;
        i1 = l8;
        i1 = LoadU32(ctx, i1 + 13644u);
        l2 = i1;
        i0 = i0 != i1;
        a = i0;
      }
      i0 = a;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        loop_437: 
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l2;
            a = i0;
            i1 = 16u;
            i0 += i1;
            l2 = i0;
          }
          i0 = l3;
          i1 = 98u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l8;
            i1 = l2;
            ReleaseChainedNodeBranches(ctx, i0, i1);
            i0 = 98u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = a;
            i0 = LoadU32(ctx, i0 + 4u);
            l5 = i0;
            i0 = !(i0);
            if (i0) {
              i0 = a;
              i0 = LoadU32(ctx, i0 + 8u);
              l2 = i0;
              i0 = LoadU32(ctx, i0);
              i1 = a;
              i0 = i0 == i1;
              l6 = i0;
              if (i0) {goto block_441;}
              i0 = a;
              i1 = 8u;
              i0 += i1;
              l5 = i0;
              loop_443: 
                i0 = l5;
                i0 = LoadU32(ctx, i0);
                a = i0;
                i1 = 8u;
                i0 += i1;
                l5 = i0;
                i0 = a;
                i0 = LoadU32(ctx, i0 + 8u);
                l2 = i0;
                i0 = LoadU32(ctx, i0);
                l6 = i0;
                i1 = a;
                i0 = i0 != i1;
                if (i0) {goto loop_443;}
              goto block_441;
            }
            loop_444: 
              i0 = l5;
              l2 = i0;
              i0 = LoadU32(ctx, i0);
              l5 = i0;
              if (i0) {goto loop_444;}
            block_441:;
            i0 = l2;
            i1 = l18;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto loop_437;}
          }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 99u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
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
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 100u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 100u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 100u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 436u);
        l2 = i0;
      }
      i0 = l3;
      i1 = 101u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l2;
        CallTableT1(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 101u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 102u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 102u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 100u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 103u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 103u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 388u);
        l2 = i0;
      }
      i0 = l3;
      i1 = 104u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l2;
        CallTableT1(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 104u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 105u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 105u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 100u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 106u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 106u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i1 = l11;
        StoreU32(ctx, i0 + 72u, i1);
        i0 = l4;
        i1 = 72u;
        i0 += i1;
        l6 = i0;
        i0 = l4;
        i1 = kF3551ConstFFFFFFC0;
        i0 -= i1;
        l5 = i0;
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        l2 = i0;
      }
      i0 = l3;
      i1 = 107u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l25;
        i2 = l11;
        i3 = kF3551ConstB5E30;
        i4 = l6;
        i5 = l5;
        UnorderedLinkBucketNode(ctx, i0, i1, i2, i3, i4, i5);
        i0 = 107u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 380u);
        l6 = i0;
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 128u);
        i1 = 28u;
        i0 += i1;
        l2 = i0;
      }
      i0 = l3;
      i1 = 108u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l2;
        i2 = 0u;
        i3 = l6;
        CallTableT5(ctx, i3, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 108u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i1 = l11;
        StoreU32(ctx, i0 + 72u, i1);
        i0 = l4;
        i1 = 72u;
        i0 += i1;
        l2 = i0;
        i0 = l4;
        i1 = kF3551ConstFFFFFFC0;
        i0 -= i1;
        l6 = i0;
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        a = i0;
      }
      i0 = l3;
      i1 = 109u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l14;
        i2 = l11;
        i3 = kF3551ConstB5E30;
        i4 = l2;
        i5 = l6;
        UnorderedInsertBucketNode(ctx, i0, i1, i2, i3, i4, i5);
        i0 = 109u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 128u);
        i0 = LoadU8(ctx, i0 + 32u);
        i0 = !(i0);
        a = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        if (i0) {goto block_493;}
        i0 = l4;
        i1 = l11;
        StoreU32(ctx, i0 + 72u, i1);
        i0 = l4;
        i1 = 72u;
        i0 += i1;
        l2 = i0;
        i0 = l4;
        i1 = kF3551ConstFFFFFFC0;
        i0 -= i1;
        l6 = i0;
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        a = i0;
      }
      i0 = l3;
      i1 = 110u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l14;
        i2 = l11;
        i3 = kF3551ConstB5E30;
        i4 = l2;
        i5 = l6;
        UnorderedInsertBucketNode(ctx, i0, i1, i2, i3, i4, i5);
        i0 = 110u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 128u);
        a = i0;
        i0 = LoadU32(ctx, i0 + 40u);
        l6 = i0;
        i0 = a;
        i0 = LoadU8(ctx, i0 + 47u);
        l2 = i0;
        i1 = 24u;
        i0 <<= (i1 & 31);
        i1 = 24u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l5 = i0;
        i0 = l6;
        i1 = l2;
        i2 = l5;
        i0 = i2 ? i0 : i1;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 36u;
          i0 += i1;
          a = i0;
        }
        i0 = l3;
        i1 = 111u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1354u;
          i1 = a;
          i2 = 0u;
          i3 = kF3551ConstFFFFFFFF;
          i4 = kF3551ConstAF3ED;
          i5 = 0u;
          i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
          l7 = i0;
          i0 = 111u;
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
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 == i1;
          l2 = i0;
          if (i0) {goto block_492;}
          i0 = a;
          i0 = !(i0);
          a = i0;
          if (i0) {goto block_493;}
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i1 = l11;
        StoreU32(ctx, i0 + 72u, i1);
        i0 = l4;
        i1 = 72u;
        i0 += i1;
        l2 = i0;
        i0 = l4;
        i1 = kF3551ConstFFFFFFC0;
        i0 -= i1;
        l6 = i0;
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        a = i0;
      }
      i0 = l3;
      i1 = 112u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l14;
        i2 = l11;
        i3 = kF3551ConstB5E30;
        i4 = l2;
        i5 = l6;
        UnorderedInsertBucketNode(ctx, i0, i1, i2, i3, i4, i5);
        i0 = 112u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 128u);
        i1 = 36u;
        i0 += i1;
        a = i0;
      }
      i0 = l3;
      i1 = 113u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l8;
        i1 = a;
        ReleaseChainedResourceNodes(ctx, i0, i1);
        i0 = 113u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      block_493:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 120u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 114u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 114u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = l3;
      i1 = 115u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        ReleaseLayerRenderData(ctx, i0);
        i0 = 115u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = 1u;
        a = i0;
        goto loop_165;
      }
      block_492:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_7;}
      block_180:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 76u);
        l2 = i0;
        i0 = !(i0);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 116u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 116u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = a;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      a = i0;
      i0 = l3;
      i1 = 117u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = 4u;
        i0 = RebuildUnorderedBucketArray(ctx, i0, i1);
        l7 = i0;
        i0 = 117u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l7;
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = !(i0);
        l2 = i0;
        if (i0) {goto loop_165;}
      }
      i0 = l3;
      i1 = 118u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l8;
        ResetEventFilterState(ctx, i0);
        i0 = 118u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 119u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 119u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 120u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 120u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 396u);
        l6 = i0;
      }
      i0 = l3;
      i1 = 121u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = 0u;
        i2 = l6;
        CallTableT0(ctx, i2, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 121u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l17;
        i1 = l8;
        i1 = LoadU32(ctx, i1 + 13616u);
        l2 = i1;
        i0 = i0 != i1;
        l6 = i0;
      }
      i0 = l6;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        loop_540: 
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l2;
            i1 = 16u;
            i0 += i1;
            l5 = i0;
            i0 = l2;
            l6 = i0;
          }
          i0 = l3;
          i1 = 122u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l8;
            i1 = l5;
            ReleaseChainedNodeBranches(ctx, i0, i1);
            i0 = 122u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l2;
            i0 = LoadU32(ctx, i0 + 4u);
            l5 = i0;
            i0 = !(i0);
            if (i0) {
              i0 = l6;
              i0 = LoadU32(ctx, i0 + 8u);
              l2 = i0;
              i0 = LoadU32(ctx, i0);
              i1 = l6;
              i0 = i0 == i1;
              l5 = i0;
              if (i0) {goto block_544;}
              i0 = l6;
              i1 = 8u;
              i0 += i1;
              l5 = i0;
              loop_546: 
                i0 = l5;
                i0 = LoadU32(ctx, i0);
                l6 = i0;
                i1 = 8u;
                i0 += i1;
                l5 = i0;
                i0 = l6;
                i0 = LoadU32(ctx, i0 + 8u);
                l2 = i0;
                i0 = LoadU32(ctx, i0);
                l9 = i0;
                i1 = l6;
                i0 = i0 != i1;
                if (i0) {goto loop_546;}
              goto block_544;
            }
            loop_547: 
              i0 = l5;
              l2 = i0;
              i0 = LoadU32(ctx, i0);
              l5 = i0;
              if (i0) {goto loop_547;}
            block_544:;
            i0 = l2;
            i1 = l17;
            i0 = i0 != i1;
            l6 = i0;
            if (i0) {goto loop_540;}
          }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 123u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 123u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 100u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 124u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 124u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 436u);
        l6 = i0;
      }
      i0 = l3;
      i1 = 125u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l6;
        CallTableT1(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 125u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 126u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 126u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 100u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 127u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 127u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 388u);
        l6 = i0;
      }
      i0 = l3;
      i1 = 128u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l6;
        CallTableT1(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 128u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 129u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 129u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 100u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 130u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 130u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i1 = l11;
        StoreU32(ctx, i0 + 72u, i1);
        i0 = l4;
        i1 = 72u;
        i0 += i1;
        l5 = i0;
        i0 = l4;
        i1 = kF3551ConstFFFFFFC0;
        i0 -= i1;
        l9 = i0;
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        l6 = i0;
      }
      i0 = l3;
      i1 = 131u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l6;
        i1 = l25;
        i2 = l11;
        i3 = kF3551ConstB5E30;
        i4 = l5;
        i5 = l9;
        UnorderedLinkBucketNode(ctx, i0, i1, i2, i3, i4, i5);
        i0 = 131u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 380u);
        l5 = i0;
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 128u);
        i1 = 28u;
        i0 += i1;
        l6 = i0;
      }
      i0 = l3;
      i1 = 132u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l6;
        i2 = 0u;
        i3 = l5;
        CallTableT5(ctx, i3, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 132u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto loop_165;}
      block_179:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 76u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 133u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 133u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = l3;
      i1 = 134u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = 0u;
        i0 = RebuildUnorderedBucketArray(ctx, i0, i1);
        l7 = i0;
        i0 = 134u;
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
        i0 = a;
        if (i0) {goto block_600;}
        i0 = l8;
        i0 = LoadU8(ctx, i0 + 48u);
        if (i0) {
          i0 = l8;
          i0 = LoadU8(ctx, i0 + 49u);
          if (i0) {goto block_600;}
          i0 = l8;
          i0 = LoadU8(ctx, i0 + 50u);
          if (i0) {goto block_600;}
        }
        i0 = 1u;
        a = i0;
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 13212u);
        i0 = !(i0);
        l2 = i0;
        if (i0) {goto loop_165;}
        block_600:;
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 13700u);
        a = i0;
      }
      i0 = l3;
      i1 = 135u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        ResetGlobalSlotTable(ctx, i0);
        i0 = 135u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 136u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 136u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = l3;
      i1 = 137u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        ReleaseSharedObjectHolder(ctx, i0);
        i0 = 137u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = l3;
      i1 = 138u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l8;
        ResetEventFilterState(ctx, i0);
        i0 = 138u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 112u);
        l2 = i0;
        i0 = !(i0);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 139u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 139u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = l3;
      i1 = 140u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 16u;
        i0 = AllocateStringBuffer(ctx, i0);
        l7 = i0;
        i0 = 140u;
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
        i0 = l4;
        i1 = a;
        StoreU32(ctx, i0 + 128u, i1);
        i0 = l4;
        j1 = 9223372105574252555ull;
        StoreU64(ctx, i0 + 132u, j1);
        i0 = a;
        i1 = 0u;
        StoreU8(ctx, i0 + 11u, i1);
        i0 = a;
        i1 = kF3551ConstAFAA6;
        i1 = LoadU32(ctx, i1);
        StoreU32(ctx, i0 + 7u, i1);
        i0 = a;
        i1 = kF3551MsgSClickskip;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0, j1);
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 20u);
        a = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        l6 = i0;
      }
      i0 = l3;
      i1 = 141u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l2;
        i2 = l6;
        i3 = 1u;
        i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
        l7 = i0;
        i0 = 141u;
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
        i0 = kF3551ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        if (i0) {
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 139u);
          i1 = kF3551ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 128u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = 1u;
          a = i0;
          goto loop_165;
        }
        i0 = HostRet0(ctx);
        a = i0;
        i0 = HostTempRet0(ctx);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 139u);
        i1 = kF3551ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {goto block_164;}
        goto block_6;
      }
      block_178:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 13212u);
        a = i0;
      }
      i0 = a;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = l5;
        i1 = l2;
        i2 = 3u;
        i1 = i1 != i2;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        l5 = i0;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l5;
          if (i0) {
            i0 = l15;
            i0 = LoadU32(ctx, i0);
            l5 = i0;
            i1 = a;
            i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
            a = i0;
            if (i0) {goto block_628;}
          }
          i0 = l8;
          i1 = 0u;
          StoreU32(ctx, i0 + 13212u, i1);
          i0 = l4;
          i1 = 72u;
          i0 += i1;
          a = i0;
        }
        i0 = l3;
        i1 = 142u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = a;
          ReleaseNodeSlotArray(ctx, i0);
          i0 = 142u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 72u);
          a = i0;
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
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 143u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kF3551MsgPx;
            i2 = kF3551MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF3551MsgOperator;
            HostFailAssertion(ctx, i0, i1, i2, i3, i4);
            i0 = 143u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_10;}
            goto block_7;
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = a;
          i1 = 1u;
          StoreU8(ctx, i0 + 12u, i1);
          i0 = l8;
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
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 144u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kF3551MsgPx;
            i2 = kF3551MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF3551MsgOperator;
            HostFailAssertion(ctx, i0, i1, i2, i3, i4);
            i0 = 144u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_10;}
            goto block_7;
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 145u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2743u;
          i1 = b;
          i2 = a;
          i3 = 1u;
          HostSyncSlot(ctx, i0, i1, i2, i3);
          i0 = 145u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
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
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 72u);
            a = i0;
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
              i0 = kF3551ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
            }
            i0 = l3;
            i1 = 146u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 857u;
              i1 = kF3551MsgPx;
              i2 = kF3551MsgBoostBoostSmartPtr;
              i3 = 734u;
              i4 = kF3551MsgOperator;
              HostFailAssertion(ctx, i0, i1, i2, i3, i4);
              i0 = 146u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3551ConstD015C;
              i0 = LoadU32(ctx, i0);
              a = i0;
              i0 = kF3551ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = a;
              i1 = 1u;
              i0 = i0 != i1;
              a = i0;
              if (i0) {goto block_10;}
              goto block_7;
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = kF3551ConstFFFFFFC0;
            i0 -= i1;
            b = i0;
          }
          i0 = l3;
          i1 = 147u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 2744u;
            i1 = b;
            i2 = a;
            HostOsSchedule(ctx, i0, i1, i2);
            i0 = 147u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 == i1;
            a = i0;
            if (i0) {goto block_647;}
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 72u);
            a = i0;
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
              i0 = kF3551ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
            }
            i0 = l3;
            i1 = 148u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 857u;
              i1 = kF3551MsgPx;
              i2 = kF3551MsgBoostBoostSmartPtr;
              i3 = 734u;
              i4 = kF3551MsgOperator;
              HostFailAssertion(ctx, i0, i1, i2, i3, i4);
              i0 = 148u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3551ConstD015C;
              i0 = LoadU32(ctx, i0);
              a = i0;
              i0 = kF3551ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = a;
              i1 = 1u;
              i0 = i0 != i1;
              a = i0;
              if (i0) {goto block_10;}
              goto block_7;
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 149u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 2745u;
            i1 = a;
            HostReleaseSlotPair(ctx, i0, i1);
            i0 = 149u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 == i1;
            a = i0;
            if (i0) {goto block_649;}
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 72u);
            b = i0;
            i0 = !(i0);
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
              i0 = kF3551ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
            }
            i0 = l3;
            i1 = 150u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 857u;
              i1 = kF3551MsgPx;
              i2 = kF3551MsgBoostBoostSmartPtr;
              i3 = 734u;
              i4 = kF3551MsgOperator;
              HostFailAssertion(ctx, i0, i1, i2, i3, i4);
              i0 = 150u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3551ConstD015C;
              i0 = LoadU32(ctx, i0);
              a = i0;
              i0 = kF3551ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = a;
              i1 = 1u;
              i0 = i0 != i1;
              a = i0;
              if (i0) {goto block_10;}
              goto block_7;
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l4;
            i1 = l4;
            i1 = LoadU32(ctx, i1 + 64u);
            StoreU32(ctx, i0 + 56u, i1);
            i0 = l4;
            i1 = l4;
            i1 = LoadU32(ctx, i1 + 68u);
            a = i1;
            StoreU32(ctx, i0 + 60u, i1);
            i0 = a;
            if (i0) {
              i0 = a;
              i1 = a;
              i1 = LoadU32(ctx, i1 + 4u);
              i2 = 1u;
              i1 += i2;
              l2 = i1;
              StoreU32(ctx, i0 + 4u, i1);
            }
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = 56u;
            i0 += i1;
            a = i0;
          }
          i0 = l3;
          i1 = 151u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 1289u;
            i1 = b;
            i2 = a;
            HostOsSchedule(ctx, i0, i1, i2);
            i0 = 151u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 == i1;
            a = i0;
            if (i0) {goto block_652;}
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 60u);
            a = i0;
            i0 = !(i0);
            b = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = b;
            if (i0) {goto block_682;}
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
            if (i0) {goto block_682;}
            i0 = a;
            i0 = LoadU32(ctx, i0);
            i0 = LoadU32(ctx, i0 + 8u);
            b = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 152u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = b;
            i1 = a;
            HostReleaseSlotPair(ctx, i0, i1);
            i0 = 152u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            b = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = b;
            i1 = 1u;
            i0 = i0 == i1;
            if (i0) {goto block_7;}
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
            if (i0) {goto block_682;}
            i0 = a;
            i0 = LoadU32(ctx, i0);
            i0 = LoadU32(ctx, i0 + 12u);
            b = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 153u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = b;
            i1 = a;
            HostReleaseSlotPair(ctx, i0, i1);
            i0 = 153u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 == i1;
            a = i0;
            if (i0) {goto block_7;}
          }
          block_682:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l8;
            i0 = LoadU32(ctx, i0 + 128u);
            a = i0;
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
              i0 = kF3551ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
            }
            i0 = l3;
            i1 = 154u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 857u;
              i1 = kF3551MsgPx;
              i2 = kF3551MsgBoostBoostSmartPtr;
              i3 = 734u;
              i4 = kF3551MsgOperator;
              HostFailAssertion(ctx, i0, i1, i2, i3, i4);
              i0 = 154u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3551ConstD015C;
              i0 = LoadU32(ctx, i0);
              a = i0;
              i0 = kF3551ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = a;
              i1 = 1u;
              i0 = i0 != i1;
              a = i0;
              if (i0) {goto block_10;}
              goto block_7;
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 72u);
            b = i0;
          }
          i0 = l3;
          i1 = 155u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 2746u;
            i1 = a;
            i2 = b;
            HostOsSchedule(ctx, i0, i1, i2);
            i0 = 155u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 == i1;
            a = i0;
            if (i0) {goto block_649;}
            i0 = l8;
            i0 = LoadU32(ctx, i0 + 136u);
            a = i0;
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
              i0 = kF3551ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
            }
            i0 = l3;
            i1 = 156u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 857u;
              i1 = kF3551MsgPx;
              i2 = kF3551MsgBoostBoostSmartPtr;
              i3 = 734u;
              i4 = kF3551MsgOperator;
              HostFailAssertion(ctx, i0, i1, i2, i3, i4);
              i0 = 156u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3551ConstD015C;
              i0 = LoadU32(ctx, i0);
              a = i0;
              i0 = kF3551ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = a;
              i1 = 1u;
              i0 = i0 != i1;
              a = i0;
              if (i0) {goto block_10;}
              goto block_7;
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l4;
            j1 = 0ull;
            StoreU64(ctx, i0 + 132u, j1);
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = l4;
            i2 = 128u;
            i1 += i2;
            i2 = 4u;
            i1 |= i2;
            StoreU32(ctx, i0 + 128u, i1);
            i0 = a;
            b = i0;
            i0 = l4;
            i1 = 128u;
            i0 += i1;
            l2 = i0;
            i0 = l8;
            i1 = 14428u;
            i0 += i1;
            a = i0;
          }
          i0 = l3;
          i1 = 157u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 2595u;
            i1 = b;
            i2 = a;
            i3 = l2;
            i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
            l7 = i0;
            i0 = 157u;
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
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 == i1;
            if (i0) {
              i0 = HostRet0(ctx);
              a = i0;
              i0 = HostTempRet0(ctx);
              i0 = l4;
              i1 = 128u;
              i0 += i1;
              b = i0;
              i1 = l4;
              i1 = LoadU32(ctx, i1 + 132u);
              l2 = i1;
              ReleaseNodeTree(ctx, i0, i1);
              goto block_648;
            }
            i0 = l4;
            i1 = 128u;
            i0 += i1;
            l2 = i0;
            i1 = l4;
            i1 = LoadU32(ctx, i1 + 132u);
            l6 = i1;
            ReleaseNodeTree(ctx, i0, i1);
            i0 = b;
            i0 = !(i0);
            b = i0;
            if (i0) {goto block_650;}
            i0 = a;
            i0 = LoadS8(ctx, i0 + 11u);
            i1 = kF3551ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            if (i0) {
              i0 = a;
              i0 = LoadU32(ctx, i0);
              goto block_706;
            }
            i0 = a;
            block_706:;
            l5 = i0;
            i0 = l8;
            i0 = LoadU32(ctx, i0 + 136u);
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
              i0 = kF3551ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
            }
            i0 = l3;
            i1 = 158u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 857u;
              i1 = kF3551MsgPx;
              i2 = kF3551MsgBoostBoostSmartPtr;
              i3 = 734u;
              i4 = kF3551MsgOperator;
              HostFailAssertion(ctx, i0, i1, i2, i3, i4);
              i0 = 158u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3551ConstD015C;
              i0 = LoadU32(ctx, i0);
              a = i0;
              i0 = kF3551ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = a;
              i1 = 1u;
              i0 = i0 != i1;
              a = i0;
              if (i0) {goto block_10;}
              goto block_7;
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = b;
            i1 = 4u;
            i0 += i1;
            l2 = i0;
            i0 = b;
            i0 = LoadS8(ctx, i0 + 15u);
            i1 = kF3551ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            l6 = i0;
            if (i0) {
              i0 = b;
              i0 = LoadU32(ctx, i0 + 4u);
              l2 = i0;
            }
            i0 = l4;
            i1 = l5;
            StoreU32(ctx, i0, i1);
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = l2;
            StoreU32(ctx, i0 + 4u, i1);
          }
          i0 = l3;
          i1 = 159u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 2597u;
            i1 = l8;
            i2 = 1u;
            i3 = kF3551MsgLuaLuaondebugskipoutSS;
            i4 = l4;
            HostFailAssertion(ctx, i0, i1, i2, i3, i4);
            i0 = 159u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            b = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = b;
            i1 = 1u;
            i0 = i0 == i1;
            b = i0;
            if (i0) {goto block_649;}
            i0 = a;
            i0 = LoadS8(ctx, i0 + 11u);
            i1 = kF3551ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            b = i0;
            if (i0) {goto block_651;}
            i0 = l8;
            i0 = LoadU32(ctx, i0 + 14428u);
            i1 = 0u;
            StoreU8(ctx, i0, i1);
            i0 = l8;
            i1 = 14432u;
            i0 += i1;
            a = i0;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            goto block_650;
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          a = i0;
          i0 = HostTempRet0(ctx);
          b = i0;
          goto block_646;
        }
        block_652:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          a = i0;
          i0 = HostTempRet0(ctx);
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 60u);
          b = i0;
          i0 = !(i0);
          l2 = i0;
          if (i0) {goto block_648;}
          i0 = b;
          i1 = b;
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
          if (i0) {goto block_648;}
          i0 = b;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 8u);
          l2 = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 160u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l2;
          i1 = b;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 160u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = kF3551ConstD015C;
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
            i0 = b;
            i1 = b;
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
            if (i0) {goto block_648;}
            i0 = b;
            i0 = LoadU32(ctx, i0);
            i0 = LoadU32(ctx, i0 + 12u);
            l2 = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 161u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l2;
            i1 = b;
            HostReleaseSlotPair(ctx, i0, i1);
            i0 = 161u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            b = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = b;
            i1 = 1u;
            i0 = i0 != i1;
            b = i0;
            if (i0) {goto block_648;}
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto block_7;}
        block_651:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = a;
          i1 = 0u;
          StoreU8(ctx, i0 + 11u, i1);
          i0 = a;
          i1 = 0u;
          StoreU8(ctx, i0, i1);
        }
        block_650:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 68u);
          a = i0;
          i0 = !(i0);
          b = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = b;
          if (i0) {goto block_727;}
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
          if (i0) {goto block_727;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 8u);
          b = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 162u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = b;
          i1 = a;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 162u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          b = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = b;
          i1 = 1u;
          i0 = i0 == i1;
          if (i0) {goto block_7;}
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
          if (i0) {goto block_727;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 12u);
          b = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 163u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = b;
          i1 = a;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 163u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 == i1;
          a = i0;
          if (i0) {goto block_7;}
        }
        block_727:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 76u);
          a = i0;
          i0 = !(i0);
          if (i0) {goto block_8;}
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
          if (i0) {goto block_8;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 8u);
          b = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 164u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = b;
          i1 = a;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 164u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          b = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = b;
          i1 = 1u;
          i0 = i0 != i1;
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
            if (i0) {goto block_8;}
            i0 = a;
            i0 = LoadU32(ctx, i0);
            i0 = LoadU32(ctx, i0 + 12u);
            b = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 165u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = b;
            i1 = a;
            HostReleaseSlotPair(ctx, i0, i1);
            i0 = 165u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_8;}
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto block_7;}
        block_649:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          a = i0;
          i0 = HostTempRet0(ctx);
          b = i0;
        }
        block_648:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 68u);
          b = i0;
          i0 = !(i0);
          l2 = i0;
          if (i0) {goto block_646;}
          i0 = b;
          i1 = b;
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
          if (i0) {goto block_646;}
          i0 = b;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 8u);
          l2 = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 166u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l2;
          i1 = b;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 166u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = kF3551ConstD015C;
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
            i0 = b;
            i1 = b;
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
            if (i0) {goto block_646;}
            i0 = b;
            i0 = LoadU32(ctx, i0);
            i0 = LoadU32(ctx, i0 + 12u);
            l2 = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 167u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l2;
            i1 = b;
            HostReleaseSlotPair(ctx, i0, i1);
            i0 = 167u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            b = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = b;
            i1 = 1u;
            i0 = i0 != i1;
            b = i0;
            if (i0) {goto block_646;}
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto block_7;}
        block_647:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          a = i0;
          i0 = HostTempRet0(ctx);
          b = i0;
        }
        block_646:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 76u);
          b = i0;
          i0 = !(i0);
          if (i0) {goto block_164;}
          i0 = b;
          i1 = b;
          i1 = LoadU32(ctx, i1 + 4u);
          l2 = i1;
          i2 = 1u;
          i1 -= i2;
          l6 = i1;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 != i1;
          if (i0) {goto block_164;}
          i0 = b;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 8u);
          l2 = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 168u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l2;
          i1 = b;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 168u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = kF3551ConstD015C;
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
            i0 = b;
            i1 = b;
            i1 = LoadU32(ctx, i1 + 8u);
            l2 = i1;
            i2 = 1u;
            i1 -= i2;
            l6 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 != i1;
            if (i0) {goto block_164;}
            i0 = b;
            i0 = LoadU32(ctx, i0);
            i0 = LoadU32(ctx, i0 + 12u);
            l2 = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 169u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l2;
            i1 = b;
            HostReleaseSlotPair(ctx, i0, i1);
            i0 = 169u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            b = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = b;
            i1 = 1u;
            i0 = i0 != i1;
            b = i0;
            if (i0) {goto block_164;}
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto block_7;}
        block_628:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          i1 = 4u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_624;}
          goto block_174;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i1 = 4u;
        i0 = i0 == i1;
        a = i0;
        if (i0) {goto block_623;}
      }
      block_624:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l24;
        i1 = l8;
        i1 = LoadU32(ctx, i1 + 13740u);
        l2 = i1;
        i0 = i0 != i1;
        a = i0;
      }
      i0 = a;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        loop_764: 
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l8;
            i0 = LoadU32(ctx, i0);
            l5 = i0;
            i0 = !(i0);
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
              i0 = kF3551ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
            }
            i0 = l3;
            i1 = 170u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 857u;
              i1 = kF3551MsgPx;
              i2 = kF3551MsgBoostBoostSmartPtr;
              i3 = 734u;
              i4 = kF3551MsgOperator;
              HostFailAssertion(ctx, i0, i1, i2, i3, i4);
              i0 = 170u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3551ConstD015C;
              i0 = LoadU32(ctx, i0);
              a = i0;
              i0 = kF3551ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = a;
              i1 = 1u;
              i0 = i0 != i1;
              a = i0;
              if (i0) {goto block_10;}
              goto block_7;
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l5;
            i0 = LoadU32(ctx, i0);
            i0 = LoadU32(ctx, i0 + 32u);
            l9 = i0;
            i0 = l2;
            a = i0;
            i0 = LoadU32(ctx, i0 + 16u);
            l2 = i0;
          }
          i0 = l3;
          i1 = 171u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l5;
            i1 = l2;
            i2 = l9;
            i0 = CallTableT3(ctx, i2, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
            SyncMemoryBase(ctx);
#endif
            l7 = i0;
            i0 = 171u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l7;
            l2 = i0;
          }
          i0 = l2;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = l2;
            i1 = a;
            i2 = 20u;
            i1 += i2;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            l2 = i0;
            i0 = l3;
            i1 = 172u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = l8;
              i1 = l2;
              i0 = FormatLuaEventFilterLine(ctx, i0, i1);
              l7 = i0;
              i0 = 172u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
              i0 = l7;
              l2 = i0;
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = l2;
              i0 = !(i0);
              l2 = i0;
              if (i0) {goto block_8;}
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = a;
            i0 = LoadU32(ctx, i0 + 4u);
            l5 = i0;
            i0 = !(i0);
            if (i0) {
              i0 = a;
              i0 = LoadU32(ctx, i0 + 8u);
              l2 = i0;
              i0 = LoadU32(ctx, i0);
              i1 = a;
              i0 = i0 == i1;
              l5 = i0;
              if (i0) {goto block_777;}
              i0 = a;
              i1 = 8u;
              i0 += i1;
              a = i0;
              loop_779: 
                i0 = a;
                i0 = LoadU32(ctx, i0);
                l5 = i0;
                i1 = 8u;
                i0 += i1;
                a = i0;
                i0 = l5;
                i0 = LoadU32(ctx, i0 + 8u);
                l2 = i0;
                i0 = LoadU32(ctx, i0);
                l9 = i0;
                i1 = l5;
                i0 = i0 != i1;
                l5 = i0;
                if (i0) {goto loop_779;}
              goto block_777;
            }
            loop_780: 
              i0 = l5;
              l2 = i0;
              i0 = LoadU32(ctx, i0);
              l5 = i0;
              if (i0) {goto loop_780;}
            block_777:;
            i0 = l2;
            i1 = l24;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto loop_764;}
          }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l23;
        i1 = l8;
        i1 = LoadU32(ctx, i1 + 13752u);
        l2 = i1;
        i0 = i0 != i1;
        a = i0;
      }
      i0 = a;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        loop_784: 
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l8;
            i0 = LoadU32(ctx, i0);
            l5 = i0;
            i0 = !(i0);
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
              i0 = kF3551ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
            }
            i0 = l3;
            i1 = 173u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 857u;
              i1 = kF3551MsgPx;
              i2 = kF3551MsgBoostBoostSmartPtr;
              i3 = 734u;
              i4 = kF3551MsgOperator;
              HostFailAssertion(ctx, i0, i1, i2, i3, i4);
              i0 = 173u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3551ConstD015C;
              i0 = LoadU32(ctx, i0);
              a = i0;
              i0 = kF3551ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = a;
              i1 = 1u;
              i0 = i0 != i1;
              a = i0;
              if (i0) {goto block_10;}
              goto block_7;
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l5;
            i0 = LoadU32(ctx, i0);
            i0 = LoadU32(ctx, i0 + 16u);
            l9 = i0;
            i0 = l2;
            a = i0;
            i0 = LoadU32(ctx, i0 + 16u);
            l2 = i0;
          }
          i0 = l3;
          i1 = 174u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l5;
            i1 = l2;
            i2 = l9;
            i0 = CallTableT3(ctx, i2, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
            SyncMemoryBase(ctx);
#endif
            l7 = i0;
            i0 = 174u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l7;
            l2 = i0;
          }
          i0 = l2;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = l2;
            i1 = a;
            i2 = 20u;
            i1 += i2;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            l2 = i0;
            i0 = l3;
            i1 = 175u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = l8;
              i1 = l2;
              i0 = FormatLuaEventFilterLine(ctx, i0, i1);
              l7 = i0;
              i0 = 175u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
              i0 = l7;
              l2 = i0;
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = l2;
              i0 = !(i0);
              l2 = i0;
              if (i0) {goto block_8;}
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = a;
            i0 = LoadU32(ctx, i0 + 4u);
            l5 = i0;
            i0 = !(i0);
            if (i0) {
              i0 = a;
              i0 = LoadU32(ctx, i0 + 8u);
              l2 = i0;
              i0 = LoadU32(ctx, i0);
              i1 = a;
              i0 = i0 == i1;
              l5 = i0;
              if (i0) {goto block_797;}
              i0 = a;
              i1 = 8u;
              i0 += i1;
              a = i0;
              loop_799: 
                i0 = a;
                i0 = LoadU32(ctx, i0);
                l5 = i0;
                i1 = 8u;
                i0 += i1;
                a = i0;
                i0 = l5;
                i0 = LoadU32(ctx, i0 + 8u);
                l2 = i0;
                i0 = LoadU32(ctx, i0);
                l9 = i0;
                i1 = l5;
                i0 = i0 != i1;
                l5 = i0;
                if (i0) {goto loop_799;}
              goto block_797;
            }
            loop_800: 
              i0 = l5;
              l2 = i0;
              i0 = LoadU32(ctx, i0);
              l5 = i0;
              if (i0) {goto loop_800;}
            block_797:;
            i0 = l2;
            i1 = l23;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto loop_784;}
          }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 13720u);
        l5 = i0;
        i1 = l8;
        i1 = LoadU32(ctx, i1 + 13724u);
        i0 = i0 != i1;
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
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 112u);
          l2 = i0;
          i0 = !(i0);
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
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 176u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kF3551MsgPx;
            i2 = kF3551MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF3551MsgOperator;
            HostFailAssertion(ctx, i0, i1, i2, i3, i4);
            i0 = 176u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_10;}
            goto block_7;
          }
        }
        i0 = l3;
        i1 = 177u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 32u;
          i0 = AllocateStringBuffer(ctx, i0);
          l7 = i0;
          i0 = 177u;
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
          i0 = l4;
          i1 = a;
          StoreU32(ctx, i0 + 128u, i1);
          i0 = l4;
          j1 = 9223372174293729304ull;
          StoreU64(ctx, i0 + 132u, j1);
          i0 = a;
          i1 = 0u;
          StoreU8(ctx, i0 + 24u, i1);
          i0 = a;
          i1 = kF3551ConstAFA6F;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0 + 16u, j1);
          i0 = a;
          i1 = kF3551ConstAFA67;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0 + 8u, j1);
          i0 = a;
          i1 = kF3551MsgSStatusScreendirection;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0, j1);
          i0 = l2;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 20u);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = 128u;
          i0 += i1;
          l9 = i0;
        }
        i0 = l3;
        i1 = 178u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = a;
          i1 = l2;
          i2 = l9;
          i3 = l5;
          i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
          l7 = i0;
          i0 = 178u;
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
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 == i1;
          a = i0;
          if (i0) {goto block_807;}
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 139u);
          i1 = kF3551ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 128u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 112u);
          l2 = i0;
          i0 = !(i0);
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
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 179u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kF3551MsgPx;
            i2 = kF3551MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF3551MsgOperator;
            HostFailAssertion(ctx, i0, i1, i2, i3, i4);
            i0 = 179u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_10;}
            goto block_7;
          }
        }
        i0 = l3;
        i1 = 180u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 48u;
          i0 = AllocateStringBuffer(ctx, i0);
          l7 = i0;
          i0 = 180u;
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
          i0 = l4;
          i1 = a;
          StoreU32(ctx, i0 + 128u, i1);
          i0 = l4;
          j1 = 9223372243013206048ull;
          StoreU64(ctx, i0 + 132u, j1);
          i0 = a;
          i1 = 0u;
          StoreU8(ctx, i0 + 32u, i1);
          i0 = a;
          i1 = kF3551ConstAFA90;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0 + 24u, j1);
          i0 = a;
          i1 = kF3551ConstAFA88;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0 + 16u, j1);
          i0 = a;
          i1 = kF3551ConstAFA80;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0 + 8u, j1);
          i0 = a;
          i1 = kF3551MsgSStatusScreendirectionprevious;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0, j1);
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 13724u);
          a = i0;
          i0 = l2;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 20u);
          l5 = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = 128u;
          i0 += i1;
          l9 = i0;
        }
        i0 = l3;
        i1 = 181u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l5;
          i1 = l2;
          i2 = l9;
          i3 = a;
          i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
          l7 = i0;
          i0 = 181u;
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
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 == i1;
          a = i0;
          if (i0) {goto block_806;}
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 139u);
          i1 = kF3551ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 128u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l8;
          i1 = l8;
          i1 = LoadU32(ctx, i1 + 13720u);
          StoreU32(ctx, i0 + 13724u, i1);
          i0 = l6;
          a = i0;
        }
        i0 = l3;
        i1 = 182u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l8;
          i1 = l36;
          i0 = FormatLuaEventFilterLine(ctx, i0, i1);
          l7 = i0;
          i0 = 182u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l7;
          l2 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          i0 = !(i0);
          l2 = i0;
          if (i0) {goto loop_165;}
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU8(ctx, i0 + 2948u);
        a = i0;
        if (i0) {goto block_802;}
        i0 = l8;
        i0 = LoadU8(ctx, i0 + 2935u);
        i0 = !(i0);
        a = i0;
        if (i0) {goto block_804;}
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 76u);
        a = i0;
        if (i0) {goto block_805;}
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l3;
      i1 = 183u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3551MsgPx;
        i2 = kF3551MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3551MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 183u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3551ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_10;}
        goto block_7;
      }
      block_807:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        a = i0;
        i0 = HostTempRet0(ctx);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 139u);
        i1 = kF3551ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {goto block_164;}
        goto block_6;
      }
      block_806:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        a = i0;
        i0 = HostTempRet0(ctx);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 139u);
        i1 = kF3551ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {goto block_164;}
        goto block_6;
      }
      block_805:;
      i0 = l3;
      i1 = 184u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = 1u;
        i0 = RebuildUnorderedBucketArray(ctx, i0, i1);
        l7 = i0;
        i0 = 184u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l7;
        a = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      i1 = 0u;
      i2 = a;
      i0 = i2 ? i0 : i1;
      if (i0) {goto block_803;}
      block_804:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU8(ctx, i0 + 2932u);
        i0 = !(i0);
        a = i0;
        if (i0) {goto block_801;}
      }
      block_803:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i1 = 0u;
        StoreU8(ctx, i0 + 2932u, i1);
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 2756u);
        i1 = l8;
        i1 = LoadS8(ctx, i1 + 2763u);
        a = i1;
        i2 = 255u;
        i1 &= i2;
        l2 = i1;
        i2 = a;
        i3 = 0u;
        i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
        l9 = i2;
        i0 = i2 ? i0 : i1;
        a = i0;
        i1 = l8;
        i1 = LoadU32(ctx, i1 + 2940u);
        i2 = l8;
        i2 = LoadS8(ctx, i2 + 2947u);
        l5 = i2;
        i3 = 255u;
        i2 &= i3;
        l13 = i2;
        i3 = l5;
        i4 = 0u;
        i3 = (uint32_t)((int32_t)i3 < (int32_t)i4);
        l5 = i3;
        i1 = i3 ? i1 : i2;
        i0 = i0 != i1;
        l10 = i0;
        if (i0) {goto block_841;}
        i0 = l16;
        i0 = LoadU32(ctx, i0);
        i1 = l16;
        i2 = l5;
        i0 = i2 ? i0 : i1;
        l5 = i0;
        i0 = l12;
        i0 = LoadU32(ctx, i0);
        l10 = i0;
        i0 = l9;
        i0 = !(i0);
        l13 = i0;
        if (i0) {
          i0 = a;
          i0 = !(i0);
          a = i0;
          if (i0) {goto block_801;}
          i0 = l12;
          a = i0;
          i0 = l10;
          i1 = 255u;
          i0 &= i1;
          l10 = i0;
          i1 = l5;
          i1 = LoadU8(ctx, i1);
          i0 = i0 != i1;
          l9 = i0;
          if (i0) {goto block_841;}
          loop_843: 
            i0 = l2;
            i1 = 1u;
            i0 -= i1;
            l2 = i0;
            i0 = !(i0);
            l9 = i0;
            if (i0) {goto block_801;}
            i0 = l5;
            i0 = LoadU8(ctx, i0 + 1u);
            l9 = i0;
            i0 = l5;
            i1 = 1u;
            i0 += i1;
            l5 = i0;
            i0 = a;
            i1 = 1u;
            i0 += i1;
            a = i0;
            i0 = LoadU8(ctx, i0);
            l10 = i0;
            i1 = l9;
            i0 = i0 == i1;
            l9 = i0;
            if (i0) {goto loop_843;}
          goto block_841;
        }
        i0 = a;
        i0 = !(i0);
        l2 = i0;
        if (i0) {goto block_801;}
        i0 = l10;
        i1 = l12;
        i2 = l9;
        i0 = i2 ? i0 : i1;
        l2 = i0;
        i1 = l5;
        i2 = a;
        i0 = CompareBytes(ctx, i0, i1, i2);
        i0 = !(i0);
        a = i0;
        if (i0) {goto block_801;}
        block_841:;
        i0 = l4;
        i1 = 0u;
        StoreU8(ctx, i0 + 128u, i1);
        i0 = l4;
        i1 = 0u;
        StoreU8(ctx, i0 + 139u, i1);
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        a = i0;
      }
      i0 = l3;
      i1 = 185u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 2594u;
        i1 = l8;
        i2 = l16;
        i3 = a;
        i4 = 0u;
        i0 = HostInvokeIiiii(ctx, i0, i1, i2, i3, i4);
        l7 = i0;
        i0 = 185u;
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
        i0 = kF3551ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        if (i0) {
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 139u);
          i1 = kF3551ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 128u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l8;
          i1 = 0u;
          StoreU32(ctx, i0 + 2952u, i1);
          i0 = l8;
          i1 = 1u;
          StoreU8(ctx, i0 + 2948u, i1);
          i0 = l6;
          a = i0;
          goto loop_165;
        }
        i0 = HostRet0(ctx);
        a = i0;
        i0 = HostTempRet0(ctx);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 139u);
        i1 = kF3551ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {goto block_164;}
        goto block_6;
      }
      block_802:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 76u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 186u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 186u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = l3;
      i1 = 187u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = 2u;
        i0 = RebuildUnorderedBucketArray(ctx, i0, i1);
        l7 = i0;
        i0 = 187u;
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
        i0 = a;
        i0 = !(i0);
        if (i0) {
          i0 = l8;
          i0 = LoadU8(ctx, i0 + 2932u);
          i0 = !(i0);
          a = i0;
          if (i0) {goto block_801;}
        }
        i0 = l8;
        i1 = 0u;
        StoreU8(ctx, i0 + 2932u, i1);
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 2952u);
        a = i0;
      }
      i0 = a;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        loop_859: 
          i0 = l3;
          i1 = 188u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l8;
            i0 = InvokeVirtualSlot(ctx, i0);
            l7 = i0;
            i0 = 188u;
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
            i0 = l8;
            i0 = LoadU32(ctx, i0 + 2952u);
            a = i0;
            if (i0) {goto loop_859;}
          }
      }
      i0 = a;
      i1 = l4;
      i2 = 40u;
      i1 += i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      a = i0;
      i0 = l3;
      i1 = 189u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l16;
        i0 = AppendStringText(ctx, i0, i1);
        l7 = i0;
        i0 = 189u;
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
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 5u;
        StoreU8(ctx, i0 + 139u, i1);
        i0 = l4;
        i1 = kF3551MsgLeave;
        i1 = LoadU32(ctx, i1);
        StoreU32(ctx, i0 + 128u, i1);
        i0 = l4;
        i1 = kF3551ConstAFA9D;
        i1 = LoadU8(ctx, i1);
        StoreU8(ctx, i0 + 132u, i1);
        i0 = l4;
        i1 = 0u;
        StoreU8(ctx, i0 + 133u, i1);
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        l2 = i0;
      }
      i0 = l3;
      i1 = 190u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 2593u;
        i1 = l8;
        i2 = a;
        i3 = l2;
        i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
        l7 = i0;
        i0 = 190u;
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
        i0 = kF3551ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        if (i0) {
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 139u);
          i1 = kF3551ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 128u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l6;
          a = i0;
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 51u);
          i1 = kF3551ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          l2 = i0;
          if (i0) {goto loop_165;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 40u);
          l2 = i0;
          DeallocateRawBlock(ctx, i0);
          goto loop_165;
        }
        i0 = HostRet0(ctx);
        a = i0;
        i0 = HostTempRet0(ctx);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 139u);
        i1 = kF3551ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 128u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 51u);
        i1 = kF3551ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {goto block_164;}
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 40u);
        DeallocateRawBlock(ctx, i0);
        goto block_164;
      }
      block_801:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU8(ctx, i0 + 2948u);
        a = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        if (i0) {goto block_870;}
        i0 = l8;
        i0 = LoadU8(ctx, i0 + 13612u);
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
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 76u);
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
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 191u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kF3551MsgPx;
            i2 = kF3551MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF3551MsgOperator;
            HostFailAssertion(ctx, i0, i1, i2, i3, i4);
            i0 = 191u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_10;}
            goto block_7;
          }
        }
        i0 = l3;
        i1 = 192u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = a;
          i1 = 3u;
          i0 = RebuildUnorderedBucketArray(ctx, i0, i1);
          l7 = i0;
          i0 = 192u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l7;
          a = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        i1 = 0u;
        i2 = a;
        i0 = i2 ? i0 : i1;
        if (i0) {goto block_871;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU8(ctx, i0 + 2933u);
        i0 = !(i0);
        a = i0;
        if (i0) {goto block_870;}
      }
      block_871:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i1 = 0u;
        StoreU8(ctx, i0 + 2933u, i1);
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 2876u);
        a = i0;
        i1 = 13u;
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
          i0 = l8;
          i1 = 5u;
          StoreU32(ctx, i0 + 2876u, i1);
          i0 = l8;
          i1 = a;
          StoreU32(ctx, i0 + 2880u, i1);
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 8u);
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
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 193u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kF3551MsgPx;
            i2 = kF3551MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF3551MsgOperator;
            HostFailAssertion(ctx, i0, i1, i2, i3, i4);
            i0 = 193u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_10;}
            goto block_7;
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = a;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 28u);
          l2 = i0;
        }
        i0 = l3;
        i1 = 194u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = a;
          i1 = 0u;
          i2 = l2;
          CallTableT0(ctx, i2, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
          SyncMemoryBase(ctx);
#endif
          i0 = 194u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = l3;
        i1 = 195u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l8;
          i1 = l35;
          i0 = FormatLuaEventFilterLine(ctx, i0, i1);
          l7 = i0;
          i0 = 195u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l7;
        } else {
          i0 = a;
        }
        a = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 196u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 196u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 197u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 197u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 396u);
        l2 = i0;
      }
      i0 = l3;
      i1 = 198u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = 1u;
        i2 = l2;
        CallTableT0(ctx, i2, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 198u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l17;
        i1 = l8;
        i1 = LoadU32(ctx, i1 + 13616u);
        l2 = i1;
        i0 = i0 == i1;
        a = i0;
        if (i0) {goto block_175;}
      }
      loop_910: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          a = i0;
          i1 = 16u;
          i0 += i1;
          l2 = i0;
        }
        i0 = l3;
        i1 = 199u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l8;
          i1 = l2;
          ReleaseChainedResourceNodes(ctx, i0, i1);
          i0 = 199u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = a;
          i0 = LoadU32(ctx, i0 + 4u);
          l5 = i0;
          i0 = !(i0);
          if (i0) {
            i0 = a;
            i0 = LoadU32(ctx, i0 + 8u);
            l2 = i0;
            i0 = LoadU32(ctx, i0);
            i1 = a;
            i0 = i0 == i1;
            l6 = i0;
            if (i0) {goto block_914;}
            i0 = a;
            i1 = 8u;
            i0 += i1;
            l5 = i0;
            loop_916: 
              i0 = l5;
              i0 = LoadU32(ctx, i0);
              a = i0;
              i1 = 8u;
              i0 += i1;
              l5 = i0;
              i0 = a;
              i0 = LoadU32(ctx, i0 + 8u);
              l2 = i0;
              i0 = LoadU32(ctx, i0);
              l6 = i0;
              i1 = a;
              i0 = i0 != i1;
              if (i0) {goto loop_916;}
            goto block_914;
          }
          loop_917: 
            i0 = l5;
            l2 = i0;
            i0 = LoadU32(ctx, i0);
            l5 = i0;
            if (i0) {goto loop_917;}
          block_914:;
          i0 = l2;
          i1 = l17;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto loop_910;}
        }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_175;}
      block_870:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 76u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 200u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 200u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = l3;
      i1 = 201u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = 5u;
        i0 = RebuildUnorderedBucketArray(ctx, i0, i1);
        l7 = i0;
        i0 = 201u;
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
        i0 = l8;
        i0 = LoadU8(ctx, i0 + 2948u);
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        if (i0) {goto block_926;}
        i0 = 0u;
        i1 = l8;
        i1 = LoadU8(ctx, i1 + 13642u);
        l2 = i1;
        i2 = a;
        i3 = 1u;
        i2 ^= i3;
        i0 = i2 ? i0 : i1;
        i0 = !(i0);
        if (i0) {
          i0 = l8;
          i0 = LoadU8(ctx, i0 + 2934u);
          i0 = !(i0);
          a = i0;
          if (i0) {goto block_926;}
        }
        i0 = l8;
        i1 = 0u;
        StoreU8(ctx, i0 + 2934u, i1);
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 120u);
        l2 = i0;
        i0 = !(i0);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 202u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 202u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 16u);
        a = i0;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 203u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 203u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i1 = l21;
        StoreU32(ctx, i0 + 72u, i1);
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        l6 = i0;
        i0 = l4;
        i1 = 72u;
        i0 += i1;
        l5 = i0;
        i0 = l4;
        i1 = kF3551ConstFFFFFFC0;
        i0 -= i1;
        l9 = i0;
        i0 = a;
        i1 = 76u;
        i0 += i1;
        a = i0;
      }
      i0 = l3;
      i1 = 204u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l6;
        i1 = a;
        i2 = l21;
        i3 = kF3551ConstB5E30;
        i4 = l5;
        i5 = l9;
        UnorderedInsertCheckedBucketNode(ctx, i0, i1, i2, i3, i4, i5);
        i0 = 204u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i1 = l4;
        i1 = LoadU32(ctx, i1 + 128u);
        a = i1;
        i1 = LoadU32(ctx, i1 + 28u);
        l6 = i1;
        StoreU32(ctx, i0 + 32u, i1);
        i0 = l4;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 32u);
        a = i1;
        StoreU32(ctx, i0 + 36u, i1);
        i0 = a;
        if (i0) {
          i0 = a;
          i1 = a;
          i1 = LoadU32(ctx, i1 + 4u);
          i2 = 1u;
          i1 += i2;
          l6 = i1;
          StoreU32(ctx, i0 + 4u, i1);
        }
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 32u;
        i0 += i1;
        a = i0;
      }
      i0 = l3;
      i1 = 205u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 2747u;
        i1 = l2;
        i2 = a;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l7 = i0;
        i0 = 205u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l7;
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3551ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
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
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 36u);
          a = i0;
          i0 = !(i0);
          l6 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l6;
          if (i0) {goto block_951;}
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
          l6 = i0;
          if (i0) {goto block_951;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          l6 = i0;
          i0 = LoadU32(ctx, i0 + 8u);
          l5 = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 206u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l5;
          i1 = a;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 206u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          l5 = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l5;
          i1 = 1u;
          i0 = i0 == i1;
          if (i0) {goto block_7;}
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
          l6 = i0;
          if (i0) {goto block_951;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          l6 = i0;
          i0 = LoadU32(ctx, i0 + 12u);
          l5 = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 207u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l5;
          i1 = a;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 207u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 == i1;
          a = i0;
          if (i0) {goto block_7;}
        }
        block_951:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = 1u;
          a = i0;
          i0 = l2;
          if (i0) {goto loop_165;}
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 2876u);
          l2 = i0;
          i1 = 13u;
          i0 = i0 == i1;
          l6 = i0;
          if (i0) {goto block_945;}
          i0 = l8;
          i1 = 6u;
          StoreU32(ctx, i0 + 2876u, i1);
          i0 = l8;
          i1 = l2;
          StoreU32(ctx, i0 + 2880u, i1);
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 8u);
          l2 = i0;
          if (i0) {goto block_946;}
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 208u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 208u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        a = i0;
        i0 = HostTempRet0(ctx);
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 36u);
        b = i0;
        i0 = !(i0);
        if (i0) {goto block_164;}
        i0 = b;
        i1 = b;
        i1 = LoadU32(ctx, i1 + 4u);
        l2 = i1;
        i2 = 1u;
        i1 -= i2;
        l6 = i1;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 != i1;
        if (i0) {goto block_164;}
        i0 = b;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 8u);
        l2 = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l3;
      i1 = 209u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = b;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 209u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3551ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3551ConstD015C;
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
          i0 = b;
          i1 = b;
          i1 = LoadU32(ctx, i1 + 8u);
          l2 = i1;
          i2 = 1u;
          i1 -= i2;
          l6 = i1;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 != i1;
          if (i0) {goto block_164;}
          i0 = b;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 12u);
          l2 = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 210u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l2;
          i1 = b;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 210u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          b = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = b;
          i1 = 1u;
          i0 = i0 != i1;
          b = i0;
          if (i0) {goto block_164;}
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_7;}
      block_946:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 28u);
        l6 = i0;
      }
      i0 = l3;
      i1 = 211u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = 0u;
        i2 = l6;
        CallTableT0(ctx, i2, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 211u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = l3;
      i1 = 212u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l8;
        i1 = l34;
        i0 = FormatLuaEventFilterLine(ctx, i0, i1);
        l7 = i0;
        i0 = 212u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l7;
        l2 = i0;
      }
      block_945:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 213u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 213u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 214u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 214u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 396u);
        l6 = i0;
      }
      i0 = l3;
      i1 = 215u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = 1u;
        i2 = l6;
        CallTableT0(ctx, i2, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 215u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l18;
        i1 = l8;
        i1 = LoadU32(ctx, i1 + 13644u);
        l2 = i1;
        i0 = i0 == i1;
        l6 = i0;
        if (i0) {goto block_176;}
      }
      loop_986: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          i1 = 16u;
          i0 += i1;
          l5 = i0;
          i0 = l2;
          l6 = i0;
        }
        i0 = l3;
        i1 = 216u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l8;
          i1 = l5;
          ReleaseChainedResourceNodes(ctx, i0, i1);
          i0 = 216u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          i0 = LoadU32(ctx, i0 + 4u);
          l5 = i0;
          i0 = !(i0);
          if (i0) {
            i0 = l6;
            i0 = LoadU32(ctx, i0 + 8u);
            l2 = i0;
            i0 = LoadU32(ctx, i0);
            i1 = l6;
            i0 = i0 == i1;
            l5 = i0;
            if (i0) {goto block_990;}
            i0 = l6;
            i1 = 8u;
            i0 += i1;
            l5 = i0;
            loop_992: 
              i0 = l5;
              i0 = LoadU32(ctx, i0);
              l6 = i0;
              i1 = 8u;
              i0 += i1;
              l5 = i0;
              i0 = l6;
              i0 = LoadU32(ctx, i0 + 8u);
              l2 = i0;
              i0 = LoadU32(ctx, i0);
              l9 = i0;
              i1 = l6;
              i0 = i0 != i1;
              if (i0) {goto loop_992;}
            goto block_990;
          }
          loop_993: 
            i0 = l5;
            l2 = i0;
            i0 = LoadU32(ctx, i0);
            l5 = i0;
            if (i0) {goto loop_993;}
          block_990:;
          i0 = l2;
          i1 = l18;
          i0 = i0 != i1;
          l6 = i0;
          if (i0) {goto loop_986;}
        }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_176;}
      block_926:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 76u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 217u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 217u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = l3;
      i1 = 218u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = 9u;
        i0 = RebuildUnorderedBucketArray(ctx, i0, i1);
        l7 = i0;
        i0 = 218u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l7;
        a = i0;
      }
      i0 = a;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = a;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        a = i0;
        i0 = l3;
        i1 = 219u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l8;
          i1 = 1u;
          i2 = 0u;
          ApplyAutoModeStatus(ctx, i0, i1, i2);
          i0 = 219u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto loop_165;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 76u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 220u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 220u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = l3;
      i1 = 221u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = 12u;
        i0 = RebuildUnorderedBucketArray(ctx, i0, i1);
        l7 = i0;
        i0 = 221u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l7;
        a = i0;
      }
      i0 = a;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = a;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        a = i0;
        i0 = l3;
        i1 = 222u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l8;
          i1 = 1u;
          i2 = 0u;
          ApplyCommandSkipStatus(ctx, i0, i1, i2);
          i0 = 222u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto loop_165;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 2876u);
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_623;}
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 76u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 223u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 223u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = l3;
      i1 = 224u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = 0u;
        i0 = RebuildUnorderedBucketArray(ctx, i0, i1);
        l7 = i0;
        i0 = 224u;
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
        i0 = a;
        if (i0) {goto block_1024;}
        i0 = l8;
        i0 = LoadU8(ctx, i0 + 48u);
        if (i0) {
          i0 = l8;
          i0 = LoadU8(ctx, i0 + 49u);
          if (i0) {goto block_1024;}
          i0 = l8;
          i0 = LoadU8(ctx, i0 + 50u);
          if (i0) {goto block_1024;}
        }
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 13212u);
        i0 = !(i0);
        a = i0;
        if (i0) {goto block_1021;}
        block_1024:;
        i0 = l8;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 225u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 225u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 100u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 226u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 226u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 392u);
        l2 = i0;
      }
      i0 = l3;
      i1 = 227u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l2;
        i0 = CallTableT2(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        l7 = i0;
        i0 = 227u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l7;
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
          i0 = l8;
          i0 = LoadU8(ctx, i0 + 2772u);
          if (i0) {goto block_1041;}
          i0 = l8;
          i0 = LoadU8(ctx, i0 + 2773u);
          if (i0) {goto block_1041;}
          i0 = l8;
          i0 = LoadU8(ctx, i0 + 2774u);
          if (i0) {goto block_1041;}
          i0 = l8;
          i0 = LoadU8(ctx, i0 + 2775u);
          if (i0) {goto block_1041;}
          i0 = l8;
          i0 = LoadU8(ctx, i0 + 2792u);
          i0 = !(i0);
          a = i0;
          if (i0) {goto block_174;}
          block_1041:;
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 112u);
          l2 = i0;
          i0 = !(i0);
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
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 228u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kF3551MsgPx;
            i2 = kF3551MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF3551MsgOperator;
            HostFailAssertion(ctx, i0, i1, i2, i3, i4);
            i0 = 228u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_10;}
            goto block_7;
          }
        }
        i0 = l3;
        i1 = 229u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 16u;
          i0 = AllocateStringBuffer(ctx, i0);
          l7 = i0;
          i0 = 229u;
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
          i0 = l4;
          i1 = a;
          StoreU32(ctx, i0 + 128u, i1);
          i0 = l4;
          j1 = 9223372105574252555ull;
          StoreU64(ctx, i0 + 132u, j1);
          i0 = a;
          i1 = 0u;
          StoreU8(ctx, i0 + 11u, i1);
          i0 = a;
          i1 = kF3551ConstAFAA6;
          i1 = LoadU32(ctx, i1);
          StoreU32(ctx, i0 + 7u, i1);
          i0 = a;
          i1 = kF3551MsgSClickskip;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0, j1);
          i0 = l2;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 20u);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = 128u;
          i0 += i1;
          l5 = i0;
        }
        i0 = l3;
        i1 = 230u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = a;
          i1 = l2;
          i2 = l5;
          i3 = 1u;
          i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
          l7 = i0;
          i0 = 230u;
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
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          if (i0) {
            i0 = l4;
            i0 = LoadS8(ctx, i0 + 139u);
            i1 = kF3551ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            a = i0;
            if (i0) {goto block_174;}
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 128u);
            a = i0;
            DeallocateRawBlock(ctx, i0);
            goto block_174;
          }
          i0 = HostRet0(ctx);
          a = i0;
          i0 = HostTempRet0(ctx);
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 139u);
          i1 = kF3551ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          if (i0) {goto block_164;}
          goto block_6;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 231u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 231u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 100u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 232u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 232u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 384u);
        l2 = i0;
      }
      i0 = l3;
      i1 = 233u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l2;
        CallTableT1(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 233u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 112u);
        l2 = i0;
        i0 = !(i0);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 234u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 234u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = l3;
      i1 = 235u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 16u;
        i0 = AllocateStringBuffer(ctx, i0);
        l7 = i0;
        i0 = 235u;
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
        i0 = l4;
        i1 = a;
        StoreU32(ctx, i0 + 128u, i1);
        i0 = l4;
        j1 = 9223372105574252555ull;
        StoreU64(ctx, i0 + 132u, j1);
        i0 = a;
        i1 = 0u;
        StoreU8(ctx, i0 + 11u, i1);
        i0 = a;
        i1 = kF3551ConstAFAA6;
        i1 = LoadU32(ctx, i1);
        StoreU32(ctx, i0 + 7u, i1);
        i0 = a;
        i1 = kF3551MsgSClickskip;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0, j1);
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 20u);
        a = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        l5 = i0;
      }
      i0 = l3;
      i1 = 236u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l2;
        i2 = l5;
        i3 = 1u;
        i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
        l7 = i0;
        i0 = 236u;
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
        i0 = kF3551ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 == i1;
        a = i0;
        if (i0) {goto block_1020;}
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 139u);
        i1 = kF3551ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        a = i0;
        if (i0) {goto block_1021;}
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 128u);
        a = i0;
        DeallocateRawBlock(ctx, i0);
      }
      block_1021:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU8(ctx, i0 + 2772u);
        a = i0;
        if (i0) {goto block_623;}
        i0 = l8;
        i0 = LoadU8(ctx, i0 + 2773u);
        a = i0;
        if (i0) {goto block_623;}
        i0 = l8;
        i0 = LoadU8(ctx, i0 + 2774u);
        a = i0;
        if (i0) {goto block_623;}
        i0 = l8;
        i0 = LoadU8(ctx, i0 + 2775u);
        a = i0;
        if (i0) {goto block_623;}
        i0 = l8;
        i0 = LoadU8(ctx, i0 + 2792u);
        i0 = !(i0);
        a = i0;
        if (i0) {goto block_1019;}
        goto block_623;
      }
      block_1020:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        a = i0;
        i0 = HostTempRet0(ctx);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 139u);
        i1 = kF3551ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {goto block_164;}
        goto block_6;
      }
      block_1019:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 237u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 237u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 100u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 238u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 238u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 392u);
        l2 = i0;
      }
      i0 = l3;
      i1 = 239u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l2;
        i0 = CallTableT2(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        l7 = i0;
        i0 = 239u;
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
        i0 = a;
        i0 = !(i0);
        a = i0;
        if (i0) {goto block_623;}
      }
      i0 = l3;
      i1 = 240u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l8;
        MessageLayer::RpDispatch(ctx, i0);
        i0 = 240u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 2768u);
        a = i0;
        if (i0) {goto block_623;}
        i0 = l8;
        i0 = LoadU8(ctx, i0 + 35u);
        i0 = !(i0);
        a = i0;
        if (i0) {goto block_623;}
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 112u);
        l2 = i0;
        i0 = !(i0);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 241u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 241u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = l3;
      i1 = 242u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 16u;
        i0 = AllocateStringBuffer(ctx, i0);
        l7 = i0;
        i0 = 242u;
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
        i0 = l4;
        i1 = a;
        StoreU32(ctx, i0 + 128u, i1);
        i0 = l4;
        j1 = 9223372105574252558ull;
        StoreU64(ctx, i0 + 132u, j1);
        i0 = a;
        i1 = 0u;
        StoreU8(ctx, i0 + 14u, i1);
        i0 = a;
        i1 = kF3551ConstAF9F3;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0 + 6u, j1);
        i0 = a;
        i1 = kF3551MsgSAutomodewait;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0, j1);
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        a = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        l5 = i0;
      }
      i0 = l3;
      i1 = 243u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l2;
        i2 = l5;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l7 = i0;
        i0 = 243u;
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
        i0 = kF3551ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 != i1;
        l2 = i0;
        if (i0) {
          i0 = l8;
          i1 = a;
          i2 = b;
          i1 += i2;
          StoreU32(ctx, i0 + 2768u, i1);
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 139u);
          i1 = kF3551ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_623;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 128u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
          goto block_623;
        }
        i0 = HostRet0(ctx);
        a = i0;
        i0 = HostTempRet0(ctx);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 139u);
        i1 = kF3551ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {goto block_164;}
        goto block_6;
      }
      block_623:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 2768u);
        a = i0;
        i0 = !(i0);
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        if (i0) {goto block_1105;}
        i0 = a;
        i1 = b;
        i0 = i0 >= i1;
        a = i0;
        if (i0) {goto block_1105;}
        i0 = l8;
        i0 = LoadU8(ctx, i0 + 35u);
        i0 = !(i0);
        a = i0;
        if (i0) {goto block_174;}
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 13276u);
        a = i0;
        i1 = l8;
        i1 = LoadU32(ctx, i1 + 13280u);
        l10 = i1;
        i0 = i0 == i1;
        l2 = i0;
        if (i0) {goto block_174;}
      }
      loop_1107: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 14348u);
          l5 = i0;
          i0 = !(i0);
          l2 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          if (i0) {goto block_1109;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          i1 = a;
          i2 = a;
          i2 = LoadU8(ctx, i2 + 11u);
          l2 = i2;
          i3 = 24u;
          i2 <<= (i3 & 31);
          i3 = 24u;
          i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
          i3 = 0u;
          i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
          l9 = i2;
          i0 = i2 ? i0 : i1;
          l30 = i0;
          i0 = a;
          i0 = LoadU32(ctx, i0 + 4u);
          l13 = i0;
          i1 = l2;
          i2 = l9;
          i0 = i2 ? i0 : i1;
          l2 = i0;
        }
        loop_1111: 
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l5;
            i0 = LoadU8(ctx, i0 + 27u);
            l9 = i0;
            i1 = 24u;
            i0 <<= (i1 & 31);
            i1 = 24u;
            i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
            i1 = 0u;
            i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
            l13 = i0;
            i0 = l5;
            i0 = LoadU32(ctx, i0 + 20u);
            i1 = l9;
            i2 = l13;
            i0 = i2 ? i0 : i1;
            l9 = i0;
            i1 = l2;
            i0 = i0 < i1;
            l31 = i0;
            i0 = l9;
            i1 = l2;
            i2 = l31;
            i0 = i2 ? i0 : i1;
            l19 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l19;
            if (i0) {
              i0 = l30;
              i1 = l5;
              i2 = 16u;
              i1 += i2;
              l7 = i1;
              i1 = LoadU32(ctx, i1);
              i2 = l7;
              i3 = l13;
              i1 = i3 ? i1 : i2;
              l13 = i1;
              i2 = l19;
              i0 = CompareBytes(ctx, i0, i1, i2);
              l7 = i0;
              i0 = !(i0);
              if (i0) {
                i0 = l2;
                i1 = l9;
                i0 = i0 >= i1;
                l9 = i0;
                if (i0) {goto block_1120;}
                goto block_1114;
              }
              i0 = l7;
              i1 = kF3551ConstFFFFFFFF;
              i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
              l9 = i0;
              if (i0) {goto block_1114;}
              block_1120:;
              i0 = l13;
              i1 = l30;
              i2 = l19;
              i0 = CompareBytes(ctx, i0, i1, i2);
              l9 = i0;
              i0 = !(i0);
              l13 = i0;
              if (i0) {goto block_1118;}
              i0 = l9;
              i1 = kF3551ConstFFFFFFFF;
              i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
              l9 = i0;
              if (i0) {goto block_1117;}
              goto block_1115;
            }
            i0 = l2;
            i1 = l9;
            i0 = i0 < i1;
            l9 = i0;
            if (i0) {goto block_1114;}
            block_1118:;
            i0 = l31;
            if (i0) {goto block_1115;}
            block_1117:;
            i0 = l8;
            i0 = LoadU32(ctx, i0 + 14336u);
            l2 = i0;
            if (i0) {goto block_1112;}
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 244u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kF3551MsgPx;
            i2 = kF3551MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF3551MsgOperator;
            HostFailAssertion(ctx, i0, i1, i2, i3, i4);
            i0 = 244u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_10;}
            goto block_7;
          }
          block_1115:;
          i0 = l5;
          i1 = l5;
          i2 = 4u;
          i1 += i2;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          l5 = i0;
          block_1114:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l5;
            i0 = LoadU32(ctx, i0);
            l5 = i0;
            if (i0) {goto loop_1111;}
            goto block_1109;
          }
          block_1112:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i1 = a;
          StoreU32(ctx, i0 + 72u, i1);
          i0 = l4;
          i1 = 72u;
          i0 += i1;
          l9 = i0;
          i0 = l4;
          i1 = kF3551ConstFFFFFFC0;
          i0 -= i1;
          l10 = i0;
          i0 = l4;
          i1 = 128u;
          i0 += i1;
          l5 = i0;
        }
        i0 = l3;
        i1 = 245u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l5;
          i1 = l33;
          i2 = a;
          i3 = kF3551ConstB5E30;
          i4 = l9;
          i5 = l10;
          CopyTaggedTreeKeyToText(ctx, i0, i1, i2, i3, i4, i5);
          i0 = 245u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 128u);
          i0 = LoadU32(ctx, i0 + 28u);
          l5 = i0;
        }
        i0 = l3;
        i1 = 246u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l2;
          i1 = l5;
          i0 = FindUnorderedValueSlot(ctx, i0, i1);
          l7 = i0;
          i0 = 246u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l7;
          l2 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          i1 = 1u;
          i0 = i0 == i1;
          l2 = i0;
          if (i0) {goto block_1105;}
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 13280u);
          l10 = i0;
        }
        block_1109:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l10;
          i1 = a;
          i2 = 12u;
          i1 += i2;
          a = i1;
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {goto loop_1107;}
        }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_174;}
      block_1105:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU8(ctx, i0 + 2773u);
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
          i0 = l8;
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
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 247u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kF3551MsgPx;
            i2 = kF3551MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF3551MsgOperator;
            HostFailAssertion(ctx, i0, i1, i2, i3, i4);
            i0 = 247u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_10;}
            goto block_7;
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = a;
          i0 = LoadU32(ctx, i0 + 100u);
          l2 = i0;
          i0 = !(i0);
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
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 248u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kF3551MsgPx;
            i2 = kF3551MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF3551MsgOperator;
            HostFailAssertion(ctx, i0, i1, i2, i3, i4);
            i0 = 248u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_10;}
            goto block_7;
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 392u);
          l5 = i0;
          i0 = 1u;
          a = i0;
        }
        i0 = l3;
        i1 = 249u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l2;
          i1 = l5;
          i0 = CallTableT2(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
          SyncMemoryBase(ctx);
#endif
          l7 = i0;
          i0 = 249u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l7;
          l2 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          i0 = !(i0);
          l2 = i0;
          if (i0) {goto loop_165;}
          goto block_174;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU8(ctx, i0 + 2774u);
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
          i0 = l8;
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
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 250u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kF3551MsgPx;
            i2 = kF3551MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF3551MsgOperator;
            HostFailAssertion(ctx, i0, i1, i2, i3, i4);
            i0 = 250u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_10;}
            goto block_7;
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = a;
          i0 = LoadU32(ctx, i0 + 100u);
          l2 = i0;
          i0 = !(i0);
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
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 251u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kF3551MsgPx;
            i2 = kF3551MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF3551MsgOperator;
            HostFailAssertion(ctx, i0, i1, i2, i3, i4);
            i0 = 251u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_10;}
            goto block_7;
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 396u);
          l5 = i0;
          i0 = 1u;
          a = i0;
        }
        i0 = l3;
        i1 = 252u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l2;
          i1 = l5;
          i0 = CallTableT2(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
          SyncMemoryBase(ctx);
#endif
          l7 = i0;
          i0 = 252u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l7;
          l2 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          i0 = !(i0);
          l2 = i0;
          if (i0) {goto loop_165;}
          goto block_174;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU8(ctx, i0 + 2775u);
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
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 14336u);
          l2 = i0;
          i0 = !(i0);
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
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
          }
          i0 = l3;
          i1 = 253u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 857u;
            i1 = kF3551MsgPx;
            i2 = kF3551MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF3551MsgOperator;
            HostFailAssertion(ctx, i0, i1, i2, i3, i4);
            i0 = 253u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3551ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3551ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_177;}
            goto block_7;
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 2788u);
          l5 = i0;
          i0 = 1u;
          a = i0;
        }
        i0 = l3;
        i1 = 254u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l2;
          i1 = l5;
          i0 = FindUnorderedValueSlot(ctx, i0, i1);
          l7 = i0;
          i0 = 254u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l7;
          l2 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          if (i0) {goto loop_165;}
          goto block_174;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = 1u;
        a = i0;
        i0 = l8;
        i0 = LoadU8(ctx, i0 + 2792u);
        i0 = !(i0);
        l2 = i0;
        if (i0) {goto loop_165;}
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 16u);
        l2 = i0;
        i0 = !(i0);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 255u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 255u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 24u);
        l2 = i0;
        i0 = !(i0);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 256u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 256u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_177;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 304u);
        l9 = i0;
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        l5 = i0;
      }
      i0 = l3;
      i1 = 257u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l5;
        i1 = l2;
        i2 = l32;
        i3 = l9;
        CallTableT5(ctx, i3, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 257u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 128u);
        l2 = i0;
        i0 = !(i0);
        l5 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        if (i0) {
          i0 = 0u;
          l5 = i0;
          i0 = 0u;
          l2 = i0;
          goto block_1196;
        }
        i0 = 0u;
        l5 = i0;
      }
      i0 = l3;
      i1 = 258u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = kF3551Const8C564;
        i2 = kF3551ConstB8F8C;
        i3 = 0u;
        i0 = CombineComparisonResults(ctx, i0, i1, i2, i3);
        l7 = i0;
        i0 = 258u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l7;
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = !(i0);
        l9 = i0;
        if (i0) {
          i0 = 0u;
          l2 = i0;
          goto block_1196;
        }
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 132u);
        l5 = i0;
        i0 = !(i0);
        l9 = i0;
        if (i0) {
          i0 = 0u;
          l5 = i0;
          goto block_1195;
        }
        i0 = l5;
        i1 = l5;
        i1 = LoadU32(ctx, i1 + 4u);
        i2 = 1u;
        i1 += i2;
        l9 = i1;
        StoreU32(ctx, i0 + 4u, i1);
      }
      block_1196:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 132u);
        l9 = i0;
        i0 = !(i0);
        l10 = i0;
        if (i0) {goto block_1195;}
        i0 = l9;
        i1 = l9;
        i1 = LoadU32(ctx, i1 + 4u);
        l10 = i1;
        i2 = 1u;
        i1 -= i2;
        l13 = i1;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = l10;
        i1 = 1u;
        i0 = i0 != i1;
        l10 = i0;
        if (i0) {goto block_1195;}
        i0 = l9;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 8u);
        l10 = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l3;
      i1 = 259u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l10;
        i1 = l9;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 259u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3551ConstD015C;
        i0 = LoadU32(ctx, i0);
        l10 = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l10;
        i1 = 1u;
        i0 = i0 == i1;
        l10 = i0;
        if (i0) {goto block_1194;}
        i0 = l9;
        i1 = l9;
        i1 = LoadU32(ctx, i1 + 8u);
        l10 = i1;
        i2 = 1u;
        i1 -= i2;
        l13 = i1;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l10;
        i1 = 1u;
        i0 = i0 != i1;
        l10 = i0;
        if (i0) {goto block_1195;}
        i0 = l9;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        l10 = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l3;
      i1 = 260u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l10;
        i1 = l9;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 260u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3551ConstD015C;
        i0 = LoadU32(ctx, i0);
        l9 = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l9;
        i1 = 1u;
        i0 = i0 == i1;
        l9 = i0;
        if (i0) {goto block_1194;}
      }
      block_1195:;
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
          i0 = LoadU32(ctx, i0 + 392u);
          l9 = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 261u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l9;
          i1 = l2;
          i0 = HostFoldLayoutOffset(ctx, i0, i1);
          l7 = i0;
          i0 = 261u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l7;
          l2 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          l9 = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l9;
          i1 = 1u;
          i0 = i0 == i1;
          l9 = i0;
          if (i0) {goto block_1209;}
          i0 = 0u;
          l9 = i0;
          i1 = l2;
          if (i1) {goto block_1210;}
        }
      }
      i0 = l9;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      block_1210:;
      l9 = i0;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l9;
        l2 = i0;
        i0 = l5;
        i0 = !(i0);
        l9 = i0;
        if (i0) {goto block_1208;}
        i0 = l5;
        i1 = l5;
        i1 = LoadU32(ctx, i1 + 4u);
        l9 = i1;
        i2 = 1u;
        i1 -= i2;
        l10 = i1;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = l9;
        i1 = 1u;
        i0 = i0 != i1;
        l9 = i0;
        if (i0) {goto block_1208;}
        i0 = l5;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 8u);
        l9 = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l3;
      i1 = 262u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l9;
        i1 = l5;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 262u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3551ConstD015C;
        i0 = LoadU32(ctx, i0);
        l9 = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l9;
        i1 = 1u;
        i0 = i0 != i1;
        l9 = i0;
      }
      i0 = l9;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l5;
          i1 = l5;
          i1 = LoadU32(ctx, i1 + 8u);
          l9 = i1;
          i2 = 1u;
          i1 -= i2;
          l10 = i1;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l9;
          i1 = 1u;
          i0 = i0 != i1;
          l9 = i0;
          if (i0) {goto block_1208;}
          i0 = l5;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 12u);
          l9 = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 263u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l9;
          i1 = l5;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 263u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          l5 = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l5;
          i1 = 1u;
          i0 = i0 != i1;
          l5 = i0;
          if (i0) {goto block_1208;}
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_7;}
      block_1209:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        a = i0;
        i0 = HostTempRet0(ctx);
        i0 = l5;
        i0 = !(i0);
        if (i0) {goto block_164;}
        i0 = l5;
        i1 = l5;
        i1 = LoadU32(ctx, i1 + 4u);
        b = i1;
        i2 = 1u;
        i1 -= i2;
        l2 = i1;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 != i1;
        if (i0) {goto block_164;}
        i0 = l5;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 8u);
        b = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l3;
      i1 = 264u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = b;
        i1 = l5;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 264u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3551ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 != i1;
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
          i0 = l5;
          i1 = l5;
          i1 = LoadU32(ctx, i1 + 8u);
          b = i1;
          i2 = 1u;
          i1 -= i2;
          l2 = i1;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = b;
          i1 = 1u;
          i0 = i0 != i1;
          if (i0) {goto block_164;}
          i0 = l5;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 12u);
          b = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 265u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = b;
          i1 = l5;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 265u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          b = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = b;
          i1 = 1u;
          i0 = i0 != i1;
          b = i0;
          if (i0) {goto block_164;}
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_7;}
      block_1208:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = !(i0);
        l2 = i0;
        if (i0) {goto loop_165;}
        goto block_174;
      }
      block_1194:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_7;}
      block_177:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        TrapUnreachable();
      }
      block_176:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 266u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 266u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 100u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 267u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 267u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 440u);
        l6 = i0;
      }
      i0 = l3;
      i1 = 268u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l6;
        CallTableT1(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 268u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 269u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 269u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 100u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 270u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 270u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 388u);
        l6 = i0;
      }
      i0 = l3;
      i1 = 271u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l6;
        CallTableT1(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 271u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 272u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 272u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 100u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 273u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 273u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i1 = l11;
        StoreU32(ctx, i0 + 72u, i1);
        i0 = l4;
        i1 = 72u;
        i0 += i1;
        l5 = i0;
        i0 = l4;
        i1 = kF3551ConstFFFFFFC0;
        i0 -= i1;
        l9 = i0;
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        l6 = i0;
      }
      i0 = l3;
      i1 = 274u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l6;
        i1 = l22;
        i2 = l11;
        i3 = kF3551ConstB5E30;
        i4 = l5;
        i5 = l9;
        UnorderedLinkBucketNode(ctx, i0, i1, i2, i3, i4, i5);
        i0 = 274u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 380u);
        l5 = i0;
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 128u);
        i1 = 28u;
        i0 += i1;
        l6 = i0;
      }
      i0 = l3;
      i1 = 275u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l6;
        i2 = 0u;
        i3 = l5;
        CallTableT5(ctx, i3, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 275u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = l3;
      i1 = 276u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l8;
        ApplyEventRecordTable(ctx, i0);
        i0 = 276u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i1 = l11;
        StoreU32(ctx, i0 + 72u, i1);
        i0 = l4;
        i1 = 72u;
        i0 += i1;
        l6 = i0;
        i0 = l4;
        i1 = kF3551ConstFFFFFFC0;
        i0 -= i1;
        l5 = i0;
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        l2 = i0;
      }
      i0 = l3;
      i1 = 277u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l14;
        i2 = l11;
        i3 = kF3551ConstB5E30;
        i4 = l6;
        i5 = l5;
        UnorderedInsertBucketNode(ctx, i0, i1, i2, i3, i4, i5);
        i0 = 277u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 128u);
        i0 = LoadU8(ctx, i0 + 32u);
        i0 = !(i0);
        l2 = i0;
        if (i0) {goto loop_165;}
        i0 = l4;
        i1 = l11;
        StoreU32(ctx, i0 + 72u, i1);
        i0 = l4;
        i1 = 72u;
        i0 += i1;
        l6 = i0;
        i0 = l4;
        i1 = kF3551ConstFFFFFFC0;
        i0 -= i1;
        l5 = i0;
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        l2 = i0;
      }
      i0 = l3;
      i1 = 278u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l14;
        i2 = l11;
        i3 = kF3551ConstB5E30;
        i4 = l6;
        i5 = l5;
        UnorderedInsertBucketNode(ctx, i0, i1, i2, i3, i4, i5);
        i0 = 278u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 128u);
        l2 = i0;
        i0 = LoadU32(ctx, i0 + 40u);
        l6 = i0;
        i0 = l2;
        i0 = LoadU8(ctx, i0 + 47u);
        l5 = i0;
        i1 = 24u;
        i0 <<= (i1 & 31);
        i1 = 24u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l9 = i0;
        i0 = l6;
        i1 = l5;
        i2 = l9;
        i0 = i2 ? i0 : i1;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l2;
          i1 = 36u;
          i0 += i1;
          l2 = i0;
        }
        i0 = l3;
        i1 = 279u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1354u;
          i1 = l2;
          i2 = 0u;
          i3 = kF3551ConstFFFFFFFF;
          i4 = kF3551ConstAF3ED;
          i5 = 0u;
          i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
          l7 = i0;
          i0 = 279u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l7;
          l2 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          l5 = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l5;
          i1 = 1u;
          i0 = i0 == i1;
          l6 = i0;
          if (i0) {goto block_1282;}
          i0 = l2;
          i0 = !(i0);
          l2 = i0;
          if (i0) {goto loop_165;}
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i1 = l11;
        StoreU32(ctx, i0 + 72u, i1);
        i0 = l4;
        i1 = 72u;
        i0 += i1;
        l6 = i0;
        i0 = l4;
        i1 = kF3551ConstFFFFFFC0;
        i0 -= i1;
        l5 = i0;
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        l2 = i0;
      }
      i0 = l3;
      i1 = 280u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l14;
        i2 = l11;
        i3 = kF3551ConstB5E30;
        i4 = l6;
        i5 = l5;
        UnorderedInsertBucketNode(ctx, i0, i1, i2, i3, i4, i5);
        i0 = 280u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 128u);
        i1 = 36u;
        i0 += i1;
        l2 = i0;
      }
      i0 = l3;
      i1 = 281u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l8;
        i1 = l2;
        ReleaseChainedNodeBranches(ctx, i0, i1);
        i0 = 281u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto loop_165;}
      block_1282:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_7;}
      block_175:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 282u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 282u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 100u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 283u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 283u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 440u);
        l2 = i0;
      }
      i0 = l3;
      i1 = 284u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l2;
        CallTableT1(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 284u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 285u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 285u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 100u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 286u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 286u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 388u);
        l2 = i0;
      }
      i0 = l3;
      i1 = 287u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l2;
        CallTableT1(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 287u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 288u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 288u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 100u);
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 289u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3551MsgPx;
          i2 = kF3551MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3551MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 289u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_10;}
          goto block_7;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i1 = l11;
        StoreU32(ctx, i0 + 72u, i1);
        i0 = l4;
        i1 = 72u;
        i0 += i1;
        l6 = i0;
        i0 = l4;
        i1 = kF3551ConstFFFFFFC0;
        i0 -= i1;
        l5 = i0;
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        l2 = i0;
      }
      i0 = l3;
      i1 = 290u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l22;
        i2 = l11;
        i3 = kF3551ConstB5E30;
        i4 = l6;
        i5 = l5;
        UnorderedLinkBucketNode(ctx, i0, i1, i2, i3, i4, i5);
        i0 = 290u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 380u);
        l6 = i0;
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 128u);
        i1 = 28u;
        i0 += i1;
        l2 = i0;
      }
      i0 = l3;
      i1 = 291u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l2;
        i2 = 0u;
        i3 = l6;
        CallTableT5(ctx, i3, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 291u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = l3;
      i1 = 292u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l8;
        ApplyEventRecordTable(ctx, i0);
        i0 = 292u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = 1u;
        a = i0;
        goto loop_165;
      }
      block_174:;
      i0 = l3;
      i1 = 293u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l8;
        i1 = 0u;
        PersistSessionState(ctx, i0, i1);
        i0 = 293u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        a = i0;
        goto loop_165;
      }
      block_173:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        l5 = i0;
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 2824u);
        a = i0;
        i1 = 93u;
        i0 = DIV_U(i0, i1);
        l2 = i0;
        i1 = 2u;
        i0 <<= (i1 & 31);
        l10 = i0;
        i1 = l8;
        i1 = LoadU32(ctx, i1 + 2812u);
        i0 += i1;
        i0 = LoadU32(ctx, i0);
        l9 = i0;
        i0 = l9;
        i1 = a;
        i2 = l2;
        i3 = 93u;
        i2 *= i3;
        l2 = i2;
        i1 -= i2;
        i2 = 44u;
        i1 *= i2;
        i0 += i1;
        a = i0;
      }
      i0 = l3;
      i1 = 294u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l5;
        i1 = a;
        i0 = CopyScriptBlock(ctx, i0, i1);
        l7 = i0;
        i0 = 294u;
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
        i0 = l8;
        i0 = LoadU32(ctx, i0 + 2824u);
        l2 = i0;
        i1 = 93u;
        i0 = DIV_U(i0, i1);
        l5 = i0;
        i1 = 2u;
        i0 <<= (i1 & 31);
        l10 = i0;
        i1 = l8;
        i1 = LoadU32(ctx, i1 + 2812u);
        i0 += i1;
        i0 = LoadU32(ctx, i0);
        l9 = i0;
        i0 = l9;
        i1 = l2;
        i2 = l5;
        i3 = 93u;
        i2 *= i3;
        i1 -= i2;
        i2 = 44u;
        i1 *= i2;
        i0 += i1;
        l2 = i0;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0);
        l5 = i0;
      }
      i0 = l3;
      i1 = 295u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l5;
        i0 = CallTableT2(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        l7 = i0;
        i0 = 295u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l7;
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i1 = l8;
        i1 = LoadU32(ctx, i1 + 2828u);
        i2 = 1u;
        i1 -= i2;
        StoreU32(ctx, i0 + 2828u, i1);
        i0 = l8;
        i1 = l8;
        i1 = LoadU32(ctx, i1 + 2824u);
        i2 = 1u;
        i1 += i2;
        l2 = i1;
        StoreU32(ctx, i0 + 2824u, i1);
        i0 = l2;
        i1 = 186u;
        i0 = i0 >= i1;
        if (i0) {
          i0 = l8;
          i0 = LoadU32(ctx, i0 + 2812u);
          i0 = LoadU32(ctx, i0);
          DeallocateRawBlock(ctx, i0);
          i0 = l8;
          i1 = l8;
          i1 = LoadU32(ctx, i1 + 2812u);
          i2 = 4u;
          i1 += i2;
          StoreU32(ctx, i0 + 2812u, i1);
          i0 = l8;
          i1 = l8;
          i1 = LoadU32(ctx, i1 + 2824u);
          i2 = 93u;
          i1 -= i2;
          StoreU32(ctx, i0 + 2824u, i1);
        }
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 80u;
        i0 += i1;
        l2 = i0;
      }
      i0 = l3;
      i1 = 296u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 2591u;
        i1 = l2;
        i2 = a;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l7 = i0;
        i0 = 296u;
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
        i0 = kF3551ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3551ConstD015C;
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
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 297u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2592u;
          i1 = l8;
          i2 = a;
          i3 = 0u;
          HostSyncSlot(ctx, i0, i1, i2, i3);
          i0 = 297u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3551ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3551ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 == i1;
          a = i0;
          if (i0) {goto block_1349;}
          i0 = l4;
          i1 = kF3551ConstB5F40;
          StoreU32(ctx, i0 + 80u, i1);
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 119u);
          i1 = kF3551ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 108u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l29;
          i1 = l4;
          i1 = LoadU32(ctx, i1 + 100u);
          ReleaseNodeTree(ctx, i0, i1);
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 95u);
          i1 = kF3551ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 84u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l4;
          i1 = kF3551ConstB5F40;
          StoreU32(ctx, i0 + 128u, i1);
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 167u);
          i1 = kF3551ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 156u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l28;
          i1 = l4;
          i1 = LoadU32(ctx, i1 + 148u);
          ReleaseNodeTree(ctx, i0, i1);
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 143u);
          i1 = kF3551ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_167;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 132u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
          goto block_167;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        a = i0;
        i0 = HostTempRet0(ctx);
        b = i0;
        goto block_1348;
      }
      block_1349:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        a = i0;
        i0 = HostTempRet0(ctx);
        i0 = l4;
        i1 = kF3551ConstB5F40;
        StoreU32(ctx, i0 + 80u, i1);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 119u);
        i1 = kF3551ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 108u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l29;
        i1 = l4;
        i1 = LoadU32(ctx, i1 + 100u);
        ReleaseNodeTree(ctx, i0, i1);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 95u);
        i1 = kF3551ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        b = i0;
        if (i0) {goto block_1348;}
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 84u);
        b = i0;
        DeallocateRawBlock(ctx, i0);
      }
      block_1348:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i1 = kF3551ConstB5F40;
        StoreU32(ctx, i0 + 128u, i1);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 167u);
        i1 = kF3551ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 156u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l28;
        i1 = l4;
        i1 = LoadU32(ctx, i1 + 148u);
        ReleaseNodeTree(ctx, i0, i1);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 143u);
        i1 = kF3551ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {goto block_164;}
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 132u);
        DeallocateRawBlock(ctx, i0);
        goto block_5;
      }
      block_172:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i0 = LoadU8(ctx, i0 + 13288u);
        i0 = !(i0);
        a = i0;
      }
      i0 = a;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = l3;
        i1 = 298u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l8;
          i1 = 0u;
          i2 = 0u;
          ApplyCommandSkipStatus(ctx, i0, i1, i2);
          i0 = 298u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l8;
          i0 = LoadS8(ctx, i0 + 2763u);
          a = i0;
          i1 = kF3551ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          l2 = i0;
          if (i0) {
            i0 = l8;
            i0 = LoadU32(ctx, i0 + 2756u);
            goto block_1369;
          }
          i0 = a;
          i1 = 255u;
          i0 &= i1;
          block_1369:;
          i0 = !(i0);
          a = i0;
          if (i0) {goto loop_169;}
        }
      }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = l12;
      StoreU32(ctx, i0 + 72u, i1);
      i0 = l4;
      i1 = 72u;
      i0 += i1;
      l2 = i0;
      i0 = l4;
      i1 = 128u;
      i0 += i1;
      a = i0;
    }
    i0 = l3;
    i1 = 299u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = l26;
      i2 = l12;
      i3 = l2;
      UnorderedRebuildBucketChain(ctx, i0, i1, i2, i3);
      i0 = 299u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 128u);
      i1 = 28u;
      i0 += i1;
      l2 = i0;
      i0 = l4;
      i1 = 128u;
      i0 += i1;
      a = i0;
    }
    i0 = l3;
    i1 = 300u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = l2;
      i2 = l15;
      i3 = l15;
      EmplaceHashedEntry(ctx, i0, i1, i2, i3);
      i0 = 300u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l8;
      i0 = LoadU8(ctx, i0 + 2745u);
      i0 = !(i0);
      a = i0;
      if (i0) {goto block_168;}
      i0 = l8;
      i0 = LoadU32(ctx, i0 + 112u);
      l2 = i0;
      i0 = !(i0);
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
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l3;
      i1 = 301u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3551MsgPx;
        i2 = kF3551MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3551MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 301u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3551ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3551ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_10;}
        goto block_7;
      }
    }
    i0 = l3;
    i1 = 302u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 32u;
      i0 = AllocateStringBuffer(ctx, i0);
      l7 = i0;
      i0 = 302u;
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
      i0 = l4;
      i1 = a;
      StoreU32(ctx, i0 + 128u, i1);
      i0 = l4;
      j1 = 9223372174293729300ull;
      StoreU64(ctx, i0 + 132u, j1);
      i0 = a;
      i1 = 0u;
      StoreU8(ctx, i0 + 20u, i1);
      i0 = a;
      i1 = kF3551ConstAFA3B;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0 + 16u, i1);
      i0 = a;
      i1 = kF3551ConstAFA33;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0 + 8u, j1);
      i0 = a;
      i1 = kF3551MsgSStatusAlreadyread;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 20u);
      a = i0;
      i0 = kF3551ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 128u;
      i0 += i1;
      l5 = i0;
    }
    i0 = l3;
    i1 = 303u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = l2;
      i2 = l5;
      i3 = 0u;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l7 = i0;
      i0 = 303u;
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
      i0 = kF3551ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3551ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      if (i0) {
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 139u);
        i1 = kF3551ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        a = i0;
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 128u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l8;
        i1 = 0u;
        StoreU8(ctx, i0 + 2745u, i1);
        goto block_168;
      }
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 139u);
      i1 = kF3551ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      if (i0) {goto block_164;}
      goto block_6;
    }
    block_168:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = l12;
      StoreU32(ctx, i0 + 72u, i1);
      i0 = l4;
      i1 = 72u;
      i0 += i1;
      l2 = i0;
      i0 = l4;
      i1 = kF3551ConstFFFFFFC0;
      i0 -= i1;
      l5 = i0;
      i0 = l4;
      i1 = 128u;
      i0 += i1;
      a = i0;
    }
    i0 = l3;
    i1 = 304u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = l20;
      i2 = l12;
      i3 = kF3551ConstB5E30;
      i4 = l2;
      i5 = l5;
      UnorderedLinkBucketNode(ctx, i0, i1, i2, i3, i4, i5);
      i0 = 304u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l8;
      i0 = LoadU32(ctx, i0 + 2764u);
      a = i0;
      i1 = 2u;
      i0 <<= (i1 & 31);
      l5 = i0;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 128u);
      i1 = LoadU32(ctx, i1 + 28u);
      i0 += i1;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = l8;
      i1 = a;
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 2764u, i1);
      i0 = l4;
      i1 = 176u;
      i0 += i1;
      a = i0;
    }
    i0 = l3;
    i1 = 305u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = l2;
      i0 = CopyScriptBlock(ctx, i0, i1);
      l7 = i0;
      i0 = 305u;
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
      i0 = kF3551ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l3;
    i1 = 306u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2592u;
      i1 = l8;
      i2 = a;
      i3 = 0u;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 306u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3551ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3551ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_166;}
      i0 = l4;
      i1 = kF3551ConstB5F40;
      StoreU32(ctx, i0 + 176u, i1);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 215u);
      i1 = kF3551ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 204u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l27;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 196u);
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 191u);
      i1 = kF3551ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_167;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 180u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
    }
    block_167:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l8;
      i0 = LoadU32(ctx, i0 + 2876u);
      a = i0;
      i1 = 14u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_8;}
      i0 = l6;
      i1 = a;
      i2 = 0u;
      i1 = i1 != i2;
      i0 |= i1;
      a = i0;
      goto loop_165;
    }
    block_166:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i1 = kF3551ConstB5F40;
    StoreU32(ctx, i0 + 176u, i1);
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 215u);
    i1 = kF3551ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 204u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l27;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 196u);
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 191u);
    i1 = kF3551ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_164;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 180u);
    DeallocateRawBlock(ctx, i0);
    goto block_5;
  }
  block_164:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_5;}
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
    i0 = l4;
    i1 = 128u;
    i0 += i1;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 132u);
    ReleaseNodeTree(ctx, i0, i1);
    goto block_5;
  }
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 224u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    a = i0;
    i0 = HostTempRet0(ctx);
  }
  i0 = l3;
  i1 = 307u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 307u;
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
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 128u);
    DeallocateRawBlock(ctx, i0);
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
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
  i2 = kF3551ResumeWordBytes;
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
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l7;
  i1 = l5;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l7;
  i1 = l6;
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
  i0 = l7;
  i1 = l11;
  StoreU32(ctx, i0 + 36u, i1);
  i0 = l7;
  i1 = l12;
  StoreU32(ctx, i0 + 40u, i1);
  i0 = l7;
  i1 = l13;
  StoreU32(ctx, i0 + 44u, i1);
  i0 = l7;
  i1 = l14;
  StoreU32(ctx, i0 + 48u, i1);
  i0 = l7;
  i1 = l15;
  StoreU32(ctx, i0 + 52u, i1);
  i0 = l7;
  i1 = l16;
  StoreU32(ctx, i0 + 56u, i1);
  i0 = l7;
  i1 = l17;
  StoreU32(ctx, i0 + 60u, i1);
  i0 = l7;
  i1 = l18;
  StoreU32(ctx, i0 + 64u, i1);
  i0 = l7;
  i1 = l19;
  StoreU32(ctx, i0 + 68u, i1);
  i0 = l7;
  i1 = l20;
  StoreU32(ctx, i0 + 72u, i1);
  i0 = l7;
  i1 = l21;
  StoreU32(ctx, i0 + 76u, i1);
  i0 = l7;
  i1 = l22;
  StoreU32(ctx, i0 + 80u, i1);
  i0 = l7;
  i1 = l23;
  StoreU32(ctx, i0 + 84u, i1);
  i0 = l7;
  i1 = l24;
  StoreU32(ctx, i0 + 88u, i1);
  i0 = l7;
  i1 = l25;
  StoreU32(ctx, i0 + 92u, i1);
  i0 = l7;
  i1 = l26;
  StoreU32(ctx, i0 + 96u, i1);
  i0 = l7;
  i1 = l27;
  StoreU32(ctx, i0 + 100u, i1);
  i0 = l7;
  i1 = l28;
  StoreU32(ctx, i0 + 104u, i1);
  i0 = l7;
  i1 = l29;
  StoreU32(ctx, i0 + 108u, i1);
  i0 = l7;
  i1 = l30;
  StoreU32(ctx, i0 + 112u, i1);
  i0 = l7;
  i1 = l31;
  StoreU32(ctx, i0 + 116u, i1);
  i0 = l7;
  i1 = l32;
  StoreU32(ctx, i0 + 120u, i1);
  i0 = l7;
  i1 = l33;
  StoreU32(ctx, i0 + 124u, i1);
  i0 = l7;
  i1 = l34;
  StoreU32(ctx, i0 + 128u, i1);
  i0 = l7;
  i1 = l35;
  StoreU32(ctx, i0 + 132u, i1);
  i0 = l7;
  i1 = l36;
  StoreU32(ctx, i0 + 136u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 140u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
}
