#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step calls.
uint32_t CompareBytes(EngineContext* ctx, uint32_t lhs, uint32_t rhs, uint32_t n);
uint32_t CopyBytes(EngineContext* ctx, uint32_t dst, uint32_t src, uint32_t n);
void StoreGlobalEntry(EngineContext* ctx, uint32_t globals, uint32_t index, uint32_t value);
void DeallocateRawBlock(EngineContext* ctx, uint32_t block);
void HostReleaseSlotPair(EngineContext* ctx, uint32_t slot, uint32_t count);
void HostSyncSlot(EngineContext* ctx, uint32_t slot, uint32_t a, uint32_t b, uint32_t c);
uint32_t HostFoldLayoutOffset(EngineContext* ctx, uint32_t slot, uint32_t count);
void HostInitResources(EngineContext* ctx, uint32_t slot, uint32_t a, uint32_t b, uint32_t c,
                       uint32_t d, uint32_t e);
void HostAcquireResource(EngineContext* ctx, uint32_t url, uint32_t buffer, uint32_t length,
                         uint32_t error);

// DispatchResourceStep -- the resource/cache step dispatcher: the saved resume
// word picks which pending resource action to run, each arm rebuilds one key
// into the step's scratch frame and drives its host slot, parking the guest
// while the action is performed.

uint32_t DispatchResourceStep(EngineContext* ctx, uint32_t p0, uint32_t p1) {
  EnterFrame();
  constexpr uint32_t kDataFFFFFFFF = 4294967295u;  // all-ones mask
  constexpr uint32_t kDataFFFFFFE0 = 4294967280u;  // 0xFFFFFFE0, the low-nibble mask
  constexpr uint32_t kData80000000 = 2147483648u;  // 0x80000000, the sign bit
  constexpr uint32_t kPendingThrowSlot = 852316u;  // the pending-throw slot the resume drain reads
  constexpr uint32_t kCacheSuffix = 716895u;  // the _cache key suffix
  constexpr uint32_t kSha1Suffix = 717011u;  // the .sha1 key suffix
  constexpr uint32_t kPool717017 = 717017u;  // data word beside the key suffixes
  constexpr uint32_t kPool717048 = 717048u;  // data word beside the key suffixes
  constexpr uint32_t kPool785488 = 785488u;  // data word holding the rebuilt resource handle
  constexpr uint32_t kHostSlot1286 = 1286u;  // the register-resource host slot
  constexpr uint32_t kHostSlot1032 = 1032u;  // the resolve-resource host slot
  constexpr uint32_t kTableSlot860 = 860u;  // the cache dispatch table slot
  constexpr uint32_t kQuerySlot870 = 870u;  // the os-query host slot
  constexpr uint32_t kSaveFrameBytes = 56u;  // the rewind frame this step saves
  constexpr uint32_t kResumeWordBytes = 4u;  // the resume word the dispatcher branches on
  constexpr uint32_t kScratchFrameBytes = 112u;  // the scratch frame the prologue allocates
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u; uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l13 = 0u; uint32_t l14 = 0u; uint32_t l15 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u;
  uint64_t j1 = 0;

  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, (uint64_t)(i1));
    i2 = kSaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, (uint64_t)(i0), i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, (uint64_t)(i0));
    l5 = i0;
    i0 = LoadU32(ctx, (uint64_t)(i0));
    p0 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, (uint64_t)(i0) + 8u);
    l2 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, (uint64_t)(i0) + 12u);
    l3 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, (uint64_t)(i0) + 16u);
    l4 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, (uint64_t)(i0) + 20u);
    l6 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, (uint64_t)(i0) + 24u);
    l7 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, (uint64_t)(i0) + 28u);
    l9 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, (uint64_t)(i0) + 32u);
    l10 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, (uint64_t)(i0) + 36u);
    l11 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, (uint64_t)(i0) + 40u);
    l12 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, (uint64_t)(i0) + 44u);
    l13 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, (uint64_t)(i0) + 48u);
    l14 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, (uint64_t)(i0) + 52u);
    l15 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, (uint64_t)(i0) + kResumeWordBytes);
    p1 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, (uint64_t)(i1));
    i2 = kResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, (uint64_t)(i0), i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, (uint64_t)(i0));
    i0 = LoadU32(ctx, (uint64_t)(i0));
    l8 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    i1 = kScratchFrameBytes;
    i0 -= i1;
    l2 = i0;
    GuestStackSp(ctx) = i0;
    i0 = l2;
    i1 = 0u;
    StoreU32(ctx, (uint64_t)(i0) + 92u, i1);
    i0 = l2;
    i1 = 0u;
    StoreU32(ctx, (uint64_t)(i0) + 88u, i1);
    i0 = l2;
    i1 = 0u;
    StoreU32(ctx, (uint64_t)(i0) + 84u, i1);
    i0 = l2;
    i1 = 88u;
    i0 += i1;
    l7 = i0;
    i0 = l2;
    i1 = 84u;
    i0 += i1;
    l12 = i0;
    i0 = p1;
    i0 = LoadU32(ctx, (uint64_t)(i0));
    i1 = p1;
    i2 = p1;
    i2 = LoadS8(ctx, (uint64_t)(i2) + 11u);
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    l4 = i0;
    i0 = l2;
    i1 = 92u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l8;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l4;
    i1 = l3;
    i2 = l7;
    i3 = l12;
    HostAcquireResource(ctx, i0, i1, i2, i3);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i0 = LoadU32(ctx, (uint64_t)(i0) + 84u);
    l13 = i0;
  }
  i0 = l13;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i1 = p1;
      i1 = LoadU32(ctx, (uint64_t)(i1));
      i2 = p1;
      i3 = p1;
      i3 = LoadS8(ctx, (uint64_t)(i3) + 11u);
      i4 = 0u;
      i3 = (uint32_t)((int32_t)i3 < (int32_t)i4);
      l4 = i3;
      i1 = i3 ? i1 : i2;
      StoreU32(ctx, (uint64_t)(i0) + 16u, i1);
      i0 = l2;
      i1 = 16u;
      i0 += i1;
      p1 = i0;
      i0 = kPool785488;
      i0 = LoadU32(ctx, (uint64_t)(i0));
      p0 = i0;
    }
    i0 = l8;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = p0;
      i1 = kPool717017;
      i2 = p1;
      StoreGlobalEntry(ctx, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_5;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i1 = 0u;
    StoreU32(ctx, (uint64_t)(i0) + 80u, i1);
    i0 = l2;
    j1 = 0;
    StoreU64(ctx, (uint64_t)(i0) + 72u, j1);
    i0 = l2;
    i0 = LoadU32(ctx, (uint64_t)(i0) + 88u);
    p1 = i0;
    i1 = kDataFFFFFFE0;
    i0 = i0 >= i1;
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
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = l2;
      i1 = kSaveFrameBytes;
      i0 += i1;
      p0 = i0;
    }
    i0 = l8;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = kHostSlot1286;
      i1 = p0;
      HostReleaseSlotPair(ctx, i0, i1);
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
      i0 = kPendingThrowSlot;
      i0 = LoadU32(ctx, (uint64_t)(i0));
      p0 = i0;
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = p0;
      i1 = 1u;
      i0 = i0 == i1;
      p0 = i0;
      if (i0) {goto block_16;}
      goto block_11;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i0 = LoadU32(ctx, (uint64_t)(i0) + 92u);
    l12 = i0;
    i0 = p1;
    i1 = 11u;
    i0 = i0 >= i1;
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
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = p1;
      i1 = 16u;
      i0 += i1;
      i1 = kDataFFFFFFE0;
      i0 &= i1;
      l4 = i0;
    }
    i0 = l8;
    i1 = 3u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = kHostSlot1032;
      i1 = l4;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l5 = i0;
      i0 = 3u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kPendingThrowSlot;
      i0 = LoadU32(ctx, (uint64_t)(i0));
      l7 = i0;
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = l7;
      i1 = 1u;
      i0 = i0 == i1;
      l7 = i0;
      if (i0) {goto block_16;}
      i0 = l2;
      i1 = l3;
      StoreU32(ctx, (uint64_t)(i0) + kSaveFrameBytes, i1);
      i0 = l2;
      i1 = p1;
      StoreU32(ctx, (uint64_t)(i0) + 60u, i1);
      i0 = l2;
      i1 = l4;
      i2 = kData80000000;
      i1 |= i2;
      StoreU32(ctx, (uint64_t)(i0) + 64u, i1);
      i0 = l2;
      i1 = kSaveFrameBytes;
      i0 += i1;
      l4 = i0;
      goto block_24;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i1 = p1;
    StoreU8(ctx, (uint64_t)(i0) + 67u, i1);
    i0 = l2;
    i1 = kSaveFrameBytes;
    i0 += i1;
    l4 = i0;
    l3 = i0;
    i0 = p1;
    i0 = !(i0);
    l7 = i0;
    if (i0) {goto block_23;}
  }
  block_24:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = l12;
    i2 = p1;
    i0 = CopyBytes(ctx, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    l7 = i0;
  }
  block_23:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = p1;
    i1 = l3;
    i0 += i1;
    i1 = 0u;
    StoreU8(ctx, (uint64_t)(i0), i1);
    i0 = l4;
    i0 = LoadS8(ctx, (uint64_t)(i0) + 11u);
    p1 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, (uint64_t)(i0) + kSaveFrameBytes);
    l3 = i0;
    i0 = l2;
    i1 = 10u;
    StoreU16(ctx, (uint64_t)(i0) + 40u, i1);
    i0 = l2;
    i1 = 1u;
    StoreU8(ctx, (uint64_t)(i0) + 51u, i1);
    i0 = p0;
    i0 = LoadU32(ctx, (uint64_t)(i0) + 16u);
    l12 = i0;
    i0 = kPendingThrowSlot;
    i1 = 0u;
    StoreU32(ctx, (uint64_t)(i0), i1);
    i0 = l2;
    i1 = 0u;
    StoreU8(ctx, (uint64_t)(i0) + 96u, i1);
    i0 = l2;
    i1 = 0u;
    StoreU8(ctx, (uint64_t)(i0) + 107u, i1);
    i0 = l3;
    i1 = l2;
    i2 = kSaveFrameBytes;
    i1 += i2;
    i2 = p1;
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    p1 = i0;
    i0 = l2;
    i1 = 96u;
    i0 += i1;
    l6 = i0;
    i0 = l2;
    i1 = 40u;
    i0 += i1;
    l7 = i0;
    i0 = l2;
    i1 = 72u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l8;
  i1 = kResumeWordBytes;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2015u;
    i1 = p1;
    i2 = l3;
    i3 = l7;
    i4 = l6;
    i5 = l12;
    HostInitResources(ctx, i0, i1, i2, i3, i4, i5);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    i0 = kResumeWordBytes;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kPendingThrowSlot;
    i0 = LoadU32(ctx, (uint64_t)(i0));
    p1 = i0;
    i0 = kPendingThrowSlot;
    i1 = 0u;
    StoreU32(ctx, (uint64_t)(i0), i1);
    i0 = p1;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {
      i0 = l2;
      i0 = LoadS8(ctx, (uint64_t)(i0) + 107u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, (uint64_t)(i0) + 96u);
        DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
      }
      i0 = l2;
      i0 = LoadS8(ctx, (uint64_t)(i0) + 51u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, (uint64_t)(i0) + 40u);
        DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
      }
      i0 = l4;
      i0 = LoadS8(ctx, (uint64_t)(i0) + 11u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, (uint64_t)(i0) + kSaveFrameBytes);
        DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
      }
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + 92u);
      DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + 72u);
      l7 = i0;
      i1 = l2;
      i1 = LoadU32(ctx, (uint64_t)(i1) + 76u);
      i0 = i0 != i1;
      p1 = i0;
      if (i0) {goto block_15;}
      i0 = l7;
      goto block_12;
    }
    i0 = HostRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    l7 = i0;
    i0 = HostTempRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    i0 = l2;
    i0 = LoadS8(ctx, (uint64_t)(i0) + 107u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + 96u);
      DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
    }
    i0 = l2;
    i0 = LoadS8(ctx, (uint64_t)(i0) + 51u);
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + 40u);
      DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
    }
    i0 = l4;
    i0 = LoadS8(ctx, (uint64_t)(i0) + 11u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    p0 = i0;
    if (i0) {goto block_14;}
    i0 = l2;
    i0 = LoadU32(ctx, (uint64_t)(i0) + kSaveFrameBytes);
    p0 = i0;
    DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    goto block_14;
  }
  block_16:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    l7 = i0;
    i0 = HostTempRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    p0 = i0;
    goto block_14;
  }
  block_15:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = p0;
    i1 = 96u;
    i0 += i1;
    l14 = i0;
    i0 = kPool785488;
    i0 = LoadU32(ctx, (uint64_t)(i0));
    l15 = i0;
    i0 = p0;
    i1 = kResumeWordBytes;
    i0 += i1;
    l12 = i0;
  }
  loop_50:; 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0) + 64u, i1);
      i0 = l2;
      j1 = 0;
      StoreU64(ctx, (uint64_t)(i0) + kSaveFrameBytes, j1);
      i0 = l7;
      i0 = LoadS8(ctx, (uint64_t)(i0) + 11u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l7;
        i0 = LoadU32(ctx, (uint64_t)(i0));
        goto block_52;
      }
      i0 = l7;
      block_52:;
      p1 = i0;
      i0 = l2;
      i1 = 44u;
      StoreU16(ctx, (uint64_t)(i0) + 40u, i1);
      i0 = l2;
      i1 = 1u;
      StoreU8(ctx, (uint64_t)(i0) + 51u, i1);
      i0 = p0;
      i0 = LoadU32(ctx, (uint64_t)(i0) + 16u);
      l4 = i0;
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = l2;
      i1 = 0u;
      StoreU8(ctx, (uint64_t)(i0) + 96u, i1);
      i0 = l2;
      i1 = 0u;
      StoreU8(ctx, (uint64_t)(i0) + 107u, i1);
      i0 = l2;
      i1 = 40u;
      i0 += i1;
      l6 = i0;
      i0 = l2;
      i1 = 96u;
      i0 += i1;
      l11 = i0;
      i0 = l2;
      i1 = kSaveFrameBytes;
      i0 += i1;
      l3 = i0;
    }
    i0 = l8;
    i1 = 5u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2015u;
      i1 = p1;
      i2 = l3;
      i3 = l6;
      i4 = l11;
      i5 = l4;
      HostInitResources(ctx, i0, i1, i2, i3, i4, i5);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = 5u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kPendingThrowSlot;
      i0 = LoadU32(ctx, (uint64_t)(i0));
      p1 = i0;
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = p1;
      i1 = 1u;
      i0 = i0 != i1;
      p1 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = p1;
      if (i0) {
        i0 = l2;
        i0 = LoadS8(ctx, (uint64_t)(i0) + 107u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l2;
          i0 = LoadU32(ctx, (uint64_t)(i0) + 96u);
          DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
          SyncMemoryBase(ctx);
#endif
        }
        i0 = l2;
        i0 = LoadS8(ctx, (uint64_t)(i0) + 51u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l2;
          i0 = LoadU32(ctx, (uint64_t)(i0) + 40u);
          DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
          SyncMemoryBase(ctx);
#endif
        }
        i0 = l2;
        i0 = LoadU32(ctx, (uint64_t)(i0) + 60u);
        i1 = l2;
        i1 = LoadU32(ctx, (uint64_t)(i1) + kSaveFrameBytes);
        p1 = i1;
        i0 -= i1;
        i1 = 24u;
        i0 = i0 != i1;
        if (i0) {goto block_60;}
        i0 = p1;
        i0 = LoadS8(ctx, (uint64_t)(i0) + 23u);
        l4 = i0;
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        l3 = i0;
        if (i0) {goto block_62;}
        i0 = p1;
        i0 = LoadU32(ctx, (uint64_t)(i0) + 16u);
        goto block_61;
      }
      i0 = HostRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l7 = i0;
      i0 = HostTempRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = l2;
      i0 = LoadS8(ctx, (uint64_t)(i0) + 107u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, (uint64_t)(i0) + 96u);
        DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
      }
      i0 = l2;
      i0 = LoadS8(ctx, (uint64_t)(i0) + 51u);
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
      p0 = i0;
      if (i0) {goto block_44;}
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + 40u);
      p0 = i0;
      DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      goto block_44;
      block_62:;
      i0 = l4;
      i1 = 255u;
      i0 &= i1;
      block_61:;
      i1 = 40u;
      i0 = i0 == i1;
      l4 = i0;
      if (i0) {goto block_58;}
      block_60:;
      i0 = l2;
      l4 = i0;
      i1 = l7;
      i1 = LoadS8(ctx, (uint64_t)(i1) + 11u);
      i2 = kDataFFFFFFFF;
      i1 = (uint32_t)((int32_t)i1 <= (int32_t)i2);
      if (i1) {
        i1 = l7;
        i1 = LoadU32(ctx, (uint64_t)(i1));
        goto block_67;
      }
      i1 = l7;
      block_67:;
      p1 = i1;
      StoreU32(ctx, (uint64_t)(i0), i1);
    }
    i0 = l8;
    i1 = 6u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l15;
      i1 = kPool717048;
      i2 = l2;
      StoreGlobalEntry(ctx, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = 6u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_57;}
    block_58:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = l2;
      i1 = 96u;
      i0 += i1;
      l4 = i0;
    }
    i0 = l8;
    i1 = 7u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2399u;
      i1 = l4;
      i2 = l14;
      i3 = p1;
      HostSyncSlot(ctx, i0, i1, i2, i3);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = 7u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kPendingThrowSlot;
      i0 = LoadU32(ctx, (uint64_t)(i0));
      l4 = i0;
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_45;}
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + 96u);
      i1 = 20u;
      i0 += i1;
      l4 = i0;
      i0 = p1;
      i1 = 12u;
      i0 += i1;
      p1 = i0;
    }
    i0 = l8;
    i1 = 8u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = kQuerySlot870;
      i1 = l4;
      i2 = p1;
      i0 = HostOsQuery(ctx, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l5 = i0;
      i0 = 8u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      p1 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kPendingThrowSlot;
      i0 = LoadU32(ctx, (uint64_t)(i0));
      p1 = i0;
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = p1;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_45;}
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0) + 104u, i1);
      i0 = l2;
      j1 = 0;
      StoreU64(ctx, (uint64_t)(i0) + 96u, j1);
      i0 = l12;
      i0 = LoadS8(ctx, (uint64_t)(i0) + 11u);
      p1 = i0;
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = p0;
        i0 = LoadU32(ctx, (uint64_t)(i0) + kResumeWordBytes);
        l4 = i0;
        i0 = p0;
        i0 = LoadU32(ctx, (uint64_t)(i0) + 8u);
        goto block_78;
      }
      i0 = l12;
      l4 = i0;
      i0 = p1;
      i1 = 255u;
      i0 &= i1;
      block_78:;
      p1 = i0;
      i1 = 6u;
      i0 += i1;
      l3 = i0;
      i1 = kDataFFFFFFE0;
      i0 = i0 >= i1;
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
        i0 = kPendingThrowSlot;
        i1 = 0u;
        StoreU32(ctx, (uint64_t)(i0), i1);
        i0 = l2;
        i1 = 96u;
        i0 += i1;
        p0 = i0;
      }
      i0 = l8;
      i1 = 9u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = kHostSlot1286;
        i1 = p0;
        HostReleaseSlotPair(ctx, i0, i1);
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
        i0 = kPendingThrowSlot;
        i0 = LoadU32(ctx, (uint64_t)(i0));
        p0 = i0;
        i0 = kPendingThrowSlot;
        i1 = 0u;
        StoreU32(ctx, (uint64_t)(i0), i1);
        i0 = p0;
        i1 = 1u;
        i0 = i0 != i1;
        p0 = i0;
        if (i0) {goto block_11;}
        goto block_75;
      }
    }
    i0 = l3;
    i1 = l3;
    i2 = 11u;
    i1 = i1 >= i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l3 = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kPendingThrowSlot;
        i1 = 0u;
        StoreU32(ctx, (uint64_t)(i0), i1);
        i0 = p1;
        i1 = 22u;
        i0 += i1;
        l3 = i0;
        i1 = kDataFFFFFFE0;
        i0 &= i1;
        l9 = i0;
      }
      i0 = l8;
      i1 = 10u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = kHostSlot1032;
        i1 = l9;
        i0 = HostFoldLayoutOffset(ctx, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        l5 = i0;
        i0 = 10u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l5;
        l3 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kPendingThrowSlot;
        i0 = LoadU32(ctx, (uint64_t)(i0));
        l10 = i0;
        i0 = kPendingThrowSlot;
        i1 = 0u;
        StoreU32(ctx, (uint64_t)(i0), i1);
        i0 = l10;
        i1 = 1u;
        i0 = i0 == i1;
        l6 = i0;
        if (i0) {goto block_84;}
        i0 = l2;
        i1 = l3;
        StoreU32(ctx, (uint64_t)(i0) + 96u, i1);
        i0 = l2;
        i1 = p1;
        StoreU32(ctx, (uint64_t)(i0) + 100u, i1);
        i0 = l2;
        i1 = l9;
        i2 = kData80000000;
        i1 |= i2;
        l6 = i1;
        StoreU32(ctx, (uint64_t)(i0) + 104u, i1);
        goto block_86;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i1 = p1;
      StoreU8(ctx, (uint64_t)(i0) + 107u, i1);
      i0 = l2;
      i1 = 96u;
      i0 += i1;
      l3 = i0;
      i0 = p1;
      i0 = !(i0);
      l6 = i0;
      if (i0) {goto block_85;}
    }
    block_86:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = l4;
      i2 = p1;
      i0 = CopyBytes(ctx, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l4 = i0;
    }
    block_85:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = p1;
      i1 = l3;
      i0 += i1;
      i1 = 0u;
      StoreU8(ctx, (uint64_t)(i0), i1);
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = l2;
      i1 = 96u;
      i0 += i1;
      p1 = i0;
    }
    i0 = l8;
    i1 = 11u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = kTableSlot860;
      i1 = p1;
      i2 = kCacheSuffix;
      i3 = 6u;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l5 = i0;
      i0 = 11u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      p1 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kPendingThrowSlot;
      i0 = LoadU32(ctx, (uint64_t)(i0));
      p1 = i0;
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = p1;
      i1 = 1u;
      i0 = i0 != i1;
      p1 = i0;
      if (i0) {goto block_74;}
    }
    block_84:;
    block_75:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l7 = i0;
      i0 = HostTempRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = l2;
      i0 = LoadS8(ctx, (uint64_t)(i0) + 107u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      p0 = i0;
      if (i0) {goto block_44;}
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + 96u);
      p0 = i0;
      DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      goto block_44;
    }
    block_74:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + 96u);
      l9 = i0;
      i0 = l2;
      i0 = LoadS8(ctx, (uint64_t)(i0) + 107u);
      l10 = i0;
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0) + 48u, i1);
      i0 = l2;
      j1 = 0;
      StoreU64(ctx, (uint64_t)(i0) + 40u, j1);
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + kSaveFrameBytes);
      l4 = i0;
      i0 = LoadU8(ctx, (uint64_t)(i0) + 11u);
      p1 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, (uint64_t)(i0) + kResumeWordBytes);
        p1 = i0;
        i0 = l4;
        i0 = LoadU32(ctx, (uint64_t)(i0));
        l4 = i0;
      }
      i0 = p1;
      i1 = 5u;
      i0 += i1;
      l3 = i0;
      i1 = kDataFFFFFFE0;
      i0 = i0 >= i1;
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
        i0 = kPendingThrowSlot;
        i1 = 0u;
        StoreU32(ctx, (uint64_t)(i0), i1);
        i0 = l2;
        i1 = 40u;
        i0 += i1;
        p0 = i0;
      }
      i0 = l8;
      i1 = 12u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = kHostSlot1286;
        i1 = p0;
        HostReleaseSlotPair(ctx, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 12u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kPendingThrowSlot;
        i0 = LoadU32(ctx, (uint64_t)(i0));
        p0 = i0;
        i0 = kPendingThrowSlot;
        i1 = 0u;
        StoreU32(ctx, (uint64_t)(i0), i1);
        i0 = p0;
        i1 = 1u;
        i0 = i0 != i1;
        p0 = i0;
        if (i0) {goto block_11;}
        goto block_100;
      }
    }
    i0 = l3;
    i1 = l3;
    i2 = 11u;
    i1 = i1 >= i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l3 = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kPendingThrowSlot;
        i1 = 0u;
        StoreU32(ctx, (uint64_t)(i0), i1);
        i0 = p1;
        i1 = 21u;
        i0 += i1;
        l3 = i0;
        i1 = kDataFFFFFFE0;
        i0 &= i1;
        l6 = i0;
      }
      i0 = l8;
      i1 = 13u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = kHostSlot1032;
        i1 = l6;
        i0 = HostFoldLayoutOffset(ctx, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        l5 = i0;
        i0 = 13u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l5;
        l3 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kPendingThrowSlot;
        i0 = LoadU32(ctx, (uint64_t)(i0));
        l11 = i0;
        i0 = kPendingThrowSlot;
        i1 = 0u;
        StoreU32(ctx, (uint64_t)(i0), i1);
        i0 = l11;
        i1 = 1u;
        i0 = i0 == i1;
        l11 = i0;
        if (i0) {goto block_108;}
        i0 = l2;
        i1 = l3;
        StoreU32(ctx, (uint64_t)(i0) + 40u, i1);
        i0 = l2;
        i1 = p1;
        StoreU32(ctx, (uint64_t)(i0) + 44u, i1);
        i0 = l2;
        i1 = l6;
        i2 = kData80000000;
        i1 |= i2;
        l6 = i1;
        StoreU32(ctx, (uint64_t)(i0) + 48u, i1);
        goto block_110;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i1 = p1;
      StoreU8(ctx, (uint64_t)(i0) + 51u, i1);
      i0 = l2;
      i1 = 40u;
      i0 += i1;
      l3 = i0;
      i0 = p1;
      i0 = !(i0);
      l6 = i0;
      if (i0) {goto block_109;}
    }
    block_110:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = l4;
      i2 = p1;
      i0 = CopyBytes(ctx, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l4 = i0;
    }
    block_109:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = p1;
      i1 = l3;
      i0 += i1;
      i1 = 0u;
      StoreU8(ctx, (uint64_t)(i0), i1);
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = l2;
      i1 = 40u;
      i0 += i1;
      p1 = i0;
    }
    i0 = l8;
    i1 = 14u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = kTableSlot860;
      i1 = p1;
      i2 = kSha1Suffix;
      i3 = 5u;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l5 = i0;
      i0 = 14u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      p1 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kPendingThrowSlot;
      i0 = LoadU32(ctx, (uint64_t)(i0));
      p1 = i0;
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = p1;
      i1 = 1u;
      i0 = i0 != i1;
      p1 = i0;
      if (i0) {goto block_99;}
    }
    block_108:;
    block_100:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l7 = i0;
      i0 = HostTempRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = l2;
      i0 = LoadS8(ctx, (uint64_t)(i0) + 51u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      p0 = i0;
      if (i0) {goto block_98;}
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + 40u);
      p0 = i0;
      DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      goto block_98;
    }
    block_99:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + 40u);
      i1 = l2;
      i2 = 40u;
      i1 += i2;
      i2 = l2;
      i2 = LoadS8(ctx, (uint64_t)(i2) + 51u);
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      i0 = i2 ? i0 : i1;
      l4 = i0;
      i0 = l2;
      i1 = 92u;
      i0 += i1;
      l3 = i0;
      i0 = l2;
      i1 = 88u;
      i0 += i1;
      l6 = i0;
      i0 = l2;
      i1 = 84u;
      i0 += i1;
      l11 = i0;
      i0 = l9;
      i1 = l2;
      i2 = 96u;
      i1 += i2;
      i2 = l10;
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      i0 = i2 ? i0 : i1;
      p1 = i0;
    }
    i0 = l8;
    i1 = 15u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2395u;
      i1 = p1;
      i2 = l4;
      i3 = l3;
      i4 = l6;
      i5 = l11;
      HostInitResources(ctx, i0, i1, i2, i3, i4, i5);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = 15u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kPendingThrowSlot;
      i0 = LoadU32(ctx, (uint64_t)(i0));
      p1 = i0;
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = p1;
      i1 = 1u;
      i0 = i0 != i1;
      p1 = i0;
    }
    i0 = p1;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = LoadS8(ctx, (uint64_t)(i0) + 51u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l2;
          i0 = LoadU32(ctx, (uint64_t)(i0) + 40u);
          DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
          SyncMemoryBase(ctx);
#endif
        }
        i0 = l2;
        i0 = LoadS8(ctx, (uint64_t)(i0) + 107u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l2;
          i0 = LoadU32(ctx, (uint64_t)(i0) + 96u);
          DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
          SyncMemoryBase(ctx);
#endif
        }
        i0 = l2;
        i0 = LoadU32(ctx, (uint64_t)(i0) + 84u);
        if (i0) {goto block_57;}
        i0 = l2;
        i0 = LoadU32(ctx, (uint64_t)(i0) + 88u);
        p1 = i0;
        i1 = kDataFFFFFFE0;
        i0 = i0 < i1;
        l4 = i0;
        if (i0) {goto block_97;}
        i0 = kPendingThrowSlot;
        i1 = 0u;
        StoreU32(ctx, (uint64_t)(i0), i1);
        i0 = l2;
        i1 = 96u;
        i0 += i1;
        p0 = i0;
      }
      i0 = l8;
      i1 = 16u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = kHostSlot1286;
        i1 = p0;
        HostReleaseSlotPair(ctx, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 16u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kPendingThrowSlot;
        i0 = LoadU32(ctx, (uint64_t)(i0));
        p0 = i0;
        i0 = kPendingThrowSlot;
        i1 = 0u;
        StoreU32(ctx, (uint64_t)(i0), i1);
        i0 = p0;
        i1 = 1u;
        i0 = i0 != i1;
        if (i0) {goto block_11;}
        i0 = HostRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        l7 = i0;
        i0 = HostTempRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        p0 = i0;
        goto block_44;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l7 = i0;
      i0 = HostTempRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = l2;
      i0 = LoadS8(ctx, (uint64_t)(i0) + 51u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      p0 = i0;
      if (i0) {goto block_98;}
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + 40u);
      p0 = i0;
      DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
    }
    block_98:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i0 = LoadS8(ctx, (uint64_t)(i0) + 107u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      p0 = i0;
      if (i0) {goto block_44;}
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + 96u);
      p0 = i0;
      DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      goto block_44;
    }
    block_97:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + 92u);
      l4 = i0;
      i0 = p1;
      i1 = 11u;
      i0 = i0 >= i1;
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
        i0 = kPendingThrowSlot;
        i1 = 0u;
        StoreU32(ctx, (uint64_t)(i0), i1);
        i0 = p1;
        i1 = 16u;
        i0 += i1;
        l3 = i0;
        i1 = kDataFFFFFFE0;
        i0 &= i1;
        l9 = i0;
      }
      i0 = l8;
      i1 = 17u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = kHostSlot1032;
        i1 = l9;
        i0 = HostFoldLayoutOffset(ctx, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        l5 = i0;
        i0 = 17u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l5;
        l3 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kPendingThrowSlot;
        i0 = LoadU32(ctx, (uint64_t)(i0));
        l10 = i0;
        i0 = kPendingThrowSlot;
        i1 = 0u;
        StoreU32(ctx, (uint64_t)(i0), i1);
        i0 = l10;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_46;}
        i0 = l2;
        i1 = l3;
        StoreU32(ctx, (uint64_t)(i0) + 96u, i1);
        i0 = l2;
        i1 = p1;
        StoreU32(ctx, (uint64_t)(i0) + 100u, i1);
        i0 = l2;
        i1 = l9;
        i2 = kData80000000;
        i1 |= i2;
        l6 = i1;
        StoreU32(ctx, (uint64_t)(i0) + 104u, i1);
        goto block_134;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i1 = p1;
      StoreU8(ctx, (uint64_t)(i0) + 107u, i1);
      i0 = l2;
      i1 = 96u;
      i0 += i1;
      l3 = i0;
      i0 = p1;
      i0 = !(i0);
      l6 = i0;
      if (i0) {goto block_133;}
    }
    block_134:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = l4;
      i2 = p1;
      i0 = CopyBytes(ctx, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l4 = i0;
    }
    block_133:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = p1;
      i1 = l3;
      i0 += i1;
      i1 = 0u;
      StoreU8(ctx, (uint64_t)(i0), i1);
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + 92u);
      DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = l2;
      i0 = LoadU8(ctx, (uint64_t)(i0) + 107u);
      l3 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l9 = i0;
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l10 = i0;
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + kSaveFrameBytes);
      p1 = i0;
      i0 = LoadU8(ctx, (uint64_t)(i0) + 23u);
      l6 = i0;
      l11 = i0;
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + 100u);
      i1 = l3;
      i2 = l10;
      i0 = i2 ? i0 : i1;
      l4 = i0;
      i1 = p1;
      i1 = LoadU32(ctx, (uint64_t)(i1) + 16u);
      i2 = l11;
      i3 = l6;
      i4 = 24u;
      i3 <<= (i4 & 31);
      i4 = 24u;
      i3 = (uint32_t)((int32_t)i3 >> (i4 & 31));
      i4 = 0u;
      i3 = (uint32_t)((int32_t)i3 < (int32_t)i4);
      l6 = i3;
      i1 = i3 ? i1 : i2;
      i0 = i0 != i1;
      l11 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l11;
      if (i0) {goto block_147;}
      i0 = p1;
      i1 = 12u;
      i0 += i1;
      p1 = i0;
      i0 = LoadU32(ctx, (uint64_t)(i0));
      i1 = p1;
      i2 = l6;
      i0 = i2 ? i0 : i1;
      p1 = i0;
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + 96u);
      l6 = i0;
      i0 = l10;
      i0 = !(i0);
      l11 = i0;
      if (i0) {
        i0 = l4;
        i0 = !(i0);
        if (i0) {goto block_57;}
        i0 = l2;
        i1 = 96u;
        i0 += i1;
        l4 = i0;
        i0 = p1;
        i0 = LoadU8(ctx, (uint64_t)(i0));
        l11 = i0;
        i1 = l6;
        i2 = 255u;
        i1 &= i2;
        i0 = i0 != i1;
        if (i0) {goto block_147;}
        loop_149:; 
          i0 = l3;
          i1 = 1u;
          i0 -= i1;
          l3 = i0;
          i0 = !(i0);
          l6 = i0;
          if (i0) {goto block_145;}
          i0 = p1;
          i0 = LoadU8(ctx, (uint64_t)(i0) + 1u);
          l10 = i0;
          i0 = p1;
          i1 = 1u;
          i0 += i1;
          p1 = i0;
          i0 = l10;
          i1 = l4;
          i2 = 1u;
          i1 += i2;
          l4 = i1;
          i1 = LoadU8(ctx, (uint64_t)(i1));
          i0 = i0 == i1;
          if (i0) {goto loop_149;}
        goto block_147;
      }
      i0 = l4;
      i0 = !(i0);
      l3 = i0;
      if (i0) {goto block_144;}
      i0 = l6;
      i1 = l2;
      i2 = 96u;
      i1 += i2;
      i2 = l10;
      i0 = i2 ? i0 : i1;
      l3 = i0;
      i1 = p1;
      i2 = l4;
      i0 = CompareBytes(ctx, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = !(i0);
      if (i0) {goto block_144;}
      block_147:;
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0) + 48u, i1);
      i0 = l2;
      j1 = 0;
      StoreU64(ctx, (uint64_t)(i0) + 40u, j1);
      i0 = l12;
      i0 = LoadS8(ctx, (uint64_t)(i0) + 11u);
      p1 = i0;
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = p0;
        i0 = LoadU32(ctx, (uint64_t)(i0) + kResumeWordBytes);
        l4 = i0;
        i0 = p0;
        i0 = LoadU32(ctx, (uint64_t)(i0) + 8u);
        goto block_150;
      }
      i0 = l12;
      l4 = i0;
      i0 = p1;
      i1 = 255u;
      i0 &= i1;
      block_150:;
      p1 = i0;
      i1 = 6u;
      i0 += i1;
      l3 = i0;
      i1 = kDataFFFFFFE0;
      i0 = i0 >= i1;
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
        i0 = kPendingThrowSlot;
        i1 = 0u;
        StoreU32(ctx, (uint64_t)(i0), i1);
        i0 = l2;
        i1 = 40u;
        i0 += i1;
        p0 = i0;
      }
      i0 = l8;
      i1 = 18u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = kHostSlot1286;
        i1 = p0;
        HostReleaseSlotPair(ctx, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 18u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kPendingThrowSlot;
        i0 = LoadU32(ctx, (uint64_t)(i0));
        p0 = i0;
        i0 = kPendingThrowSlot;
        i1 = 0u;
        StoreU32(ctx, (uint64_t)(i0), i1);
        i0 = p0;
        i1 = 1u;
        i0 = i0 != i1;
        p0 = i0;
        if (i0) {goto block_11;}
        goto block_153;
      }
    }
    i0 = l3;
    i1 = l3;
    i2 = 11u;
    i1 = i1 >= i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l3 = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kPendingThrowSlot;
        i1 = 0u;
        StoreU32(ctx, (uint64_t)(i0), i1);
        i0 = p1;
        i1 = 22u;
        i0 += i1;
        l3 = i0;
        i1 = kDataFFFFFFE0;
        i0 &= i1;
        l9 = i0;
      }
      i0 = l8;
      i1 = 19u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = kHostSlot1032;
        i1 = l9;
        i0 = HostFoldLayoutOffset(ctx, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        l5 = i0;
        i0 = 19u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l5;
        l3 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kPendingThrowSlot;
        i0 = LoadU32(ctx, (uint64_t)(i0));
        l10 = i0;
        i0 = kPendingThrowSlot;
        i1 = 0u;
        StoreU32(ctx, (uint64_t)(i0), i1);
        i0 = l10;
        i1 = 1u;
        i0 = i0 == i1;
        l6 = i0;
        if (i0) {goto block_158;}
        i0 = l2;
        i1 = l3;
        StoreU32(ctx, (uint64_t)(i0) + 40u, i1);
        i0 = l2;
        i1 = p1;
        StoreU32(ctx, (uint64_t)(i0) + 44u, i1);
        i0 = l2;
        i1 = l9;
        i2 = kData80000000;
        i1 |= i2;
        l6 = i1;
        StoreU32(ctx, (uint64_t)(i0) + 48u, i1);
        goto block_160;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i1 = p1;
      StoreU8(ctx, (uint64_t)(i0) + 51u, i1);
      i0 = l2;
      i1 = 40u;
      i0 += i1;
      l3 = i0;
      i0 = p1;
      i0 = !(i0);
      l6 = i0;
      if (i0) {goto block_159;}
    }
    block_160:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = l4;
      i2 = p1;
      i0 = CopyBytes(ctx, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l4 = i0;
    }
    block_159:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = p1;
      i1 = l3;
      i0 += i1;
      i1 = 0u;
      StoreU8(ctx, (uint64_t)(i0), i1);
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = l2;
      i1 = 40u;
      i0 += i1;
      p1 = i0;
    }
    i0 = l8;
    i1 = 20u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = kTableSlot860;
      i1 = p1;
      i2 = kCacheSuffix;
      i3 = 6u;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l5 = i0;
      i0 = 20u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      p1 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kPendingThrowSlot;
      i0 = LoadU32(ctx, (uint64_t)(i0));
      p1 = i0;
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = p1;
      i1 = 1u;
      i0 = i0 != i1;
      p1 = i0;
      if (i0) {goto block_152;}
    }
    block_158:;
    block_153:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l7 = i0;
      i0 = HostTempRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = l2;
      i0 = LoadS8(ctx, (uint64_t)(i0) + 51u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      p0 = i0;
      if (i0) {goto block_47;}
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + 40u);
      p0 = i0;
      DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      goto block_47;
    }
    block_152:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + 40u);
      i1 = l2;
      i2 = 40u;
      i1 += i2;
      i2 = l2;
      i2 = LoadS8(ctx, (uint64_t)(i2) + 51u);
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      i0 = i2 ? i0 : i1;
      l4 = i0;
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + kSaveFrameBytes);
      p1 = i0;
      i0 = LoadS8(ctx, (uint64_t)(i0) + 11u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = p1;
        i0 = LoadU32(ctx, (uint64_t)(i0));
        p1 = i0;
      }
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = l2;
      i1 = 84u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l8;
    i1 = 21u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2400u;
      i1 = l4;
      i2 = p1;
      i3 = l3;
      HostSyncSlot(ctx, i0, i1, i2, i3);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = 21u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kPendingThrowSlot;
      i0 = LoadU32(ctx, (uint64_t)(i0));
      p1 = i0;
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = p1;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_56;}
      i0 = l2;
      i0 = LoadS8(ctx, (uint64_t)(i0) + 51u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, (uint64_t)(i0) + 40u);
        DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0) + 48u, i1);
      i0 = l2;
      j1 = 0;
      StoreU64(ctx, (uint64_t)(i0) + 40u, j1);
      i0 = l12;
      i0 = LoadS8(ctx, (uint64_t)(i0) + 11u);
      p1 = i0;
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = p0;
        i0 = LoadU32(ctx, (uint64_t)(i0) + kResumeWordBytes);
        l4 = i0;
        i0 = p0;
        i0 = LoadU32(ctx, (uint64_t)(i0) + 8u);
        goto block_176;
      }
      i0 = l12;
      l4 = i0;
      i0 = p1;
      i1 = 255u;
      i0 &= i1;
      block_176:;
      p1 = i0;
      i1 = 6u;
      i0 += i1;
      l3 = i0;
      i1 = kDataFFFFFFE0;
      i0 = i0 >= i1;
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
        i0 = kPendingThrowSlot;
        i1 = 0u;
        StoreU32(ctx, (uint64_t)(i0), i1);
        i0 = l2;
        i1 = 40u;
        i0 += i1;
        p0 = i0;
      }
      i0 = l8;
      i1 = 22u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = kHostSlot1286;
        i1 = p0;
        HostReleaseSlotPair(ctx, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 22u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kPendingThrowSlot;
        i0 = LoadU32(ctx, (uint64_t)(i0));
        p0 = i0;
        i0 = kPendingThrowSlot;
        i1 = 0u;
        StoreU32(ctx, (uint64_t)(i0), i1);
        i0 = p0;
        i1 = 1u;
        i0 = i0 != i1;
        p0 = i0;
        if (i0) {goto block_11;}
        goto block_179;
      }
    }
    i0 = l3;
    i1 = l3;
    i2 = 11u;
    i1 = i1 >= i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l3 = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kPendingThrowSlot;
        i1 = 0u;
        StoreU32(ctx, (uint64_t)(i0), i1);
        i0 = p1;
        i1 = 22u;
        i0 += i1;
        l3 = i0;
        i1 = kDataFFFFFFE0;
        i0 &= i1;
        l9 = i0;
      }
      i0 = l8;
      i1 = 23u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = kHostSlot1032;
        i1 = l9;
        i0 = HostFoldLayoutOffset(ctx, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        l5 = i0;
        i0 = 23u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l5;
        l3 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kPendingThrowSlot;
        i0 = LoadU32(ctx, (uint64_t)(i0));
        l10 = i0;
        i0 = kPendingThrowSlot;
        i1 = 0u;
        StoreU32(ctx, (uint64_t)(i0), i1);
        i0 = l10;
        i1 = 1u;
        i0 = i0 == i1;
        l6 = i0;
        if (i0) {goto block_184;}
        i0 = l2;
        i1 = l3;
        StoreU32(ctx, (uint64_t)(i0) + 40u, i1);
        i0 = l2;
        i1 = p1;
        StoreU32(ctx, (uint64_t)(i0) + 44u, i1);
        i0 = l2;
        i1 = l9;
        i2 = kData80000000;
        i1 |= i2;
        l6 = i1;
        StoreU32(ctx, (uint64_t)(i0) + 48u, i1);
        goto block_186;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i1 = p1;
      StoreU8(ctx, (uint64_t)(i0) + 51u, i1);
      i0 = l2;
      i1 = 40u;
      i0 += i1;
      l3 = i0;
      i0 = p1;
      i0 = !(i0);
      l6 = i0;
      if (i0) {goto block_185;}
    }
    block_186:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = l4;
      i2 = p1;
      i0 = CopyBytes(ctx, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l4 = i0;
    }
    block_185:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = p1;
      i1 = l3;
      i0 += i1;
      i1 = 0u;
      StoreU8(ctx, (uint64_t)(i0), i1);
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = l2;
      i1 = 40u;
      i0 += i1;
      p1 = i0;
    }
    i0 = l8;
    i1 = 24u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = kTableSlot860;
      i1 = p1;
      i2 = kCacheSuffix;
      i3 = 6u;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l5 = i0;
      i0 = 24u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      p1 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kPendingThrowSlot;
      i0 = LoadU32(ctx, (uint64_t)(i0));
      p1 = i0;
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = p1;
      i1 = 1u;
      i0 = i0 != i1;
      p1 = i0;
      if (i0) {goto block_178;}
    }
    block_184:;
    block_179:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l7 = i0;
      i0 = HostTempRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = l2;
      i0 = LoadS8(ctx, (uint64_t)(i0) + 51u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      p0 = i0;
      if (i0) {goto block_47;}
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + 40u);
      p0 = i0;
      DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      goto block_47;
    }
    block_178:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + 40u);
      l9 = i0;
      i0 = l2;
      i0 = LoadS8(ctx, (uint64_t)(i0) + 51u);
      l10 = i0;
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0) + 32u, i1);
      i0 = l2;
      j1 = 0;
      StoreU64(ctx, (uint64_t)(i0) + 24u, j1);
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + kSaveFrameBytes);
      l4 = i0;
      i0 = LoadU8(ctx, (uint64_t)(i0) + 11u);
      p1 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, (uint64_t)(i0) + kResumeWordBytes);
        p1 = i0;
        i0 = l4;
        i0 = LoadU32(ctx, (uint64_t)(i0));
        l4 = i0;
      }
      i0 = p1;
      i1 = 5u;
      i0 += i1;
      l3 = i0;
      i1 = kDataFFFFFFE0;
      i0 = i0 >= i1;
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
        i0 = kPendingThrowSlot;
        i1 = 0u;
        StoreU32(ctx, (uint64_t)(i0), i1);
        i0 = l2;
        i1 = 24u;
        i0 += i1;
        p0 = i0;
      }
      i0 = l8;
      i1 = 25u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = kHostSlot1286;
        i1 = p0;
        HostReleaseSlotPair(ctx, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 25u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kPendingThrowSlot;
        i0 = LoadU32(ctx, (uint64_t)(i0));
        p0 = i0;
        i0 = kPendingThrowSlot;
        i1 = 0u;
        StoreU32(ctx, (uint64_t)(i0), i1);
        i0 = p0;
        i1 = 1u;
        i0 = i0 != i1;
        p0 = i0;
        if (i0) {goto block_11;}
        goto block_198;
      }
    }
    i0 = l3;
    i1 = l3;
    i2 = 11u;
    i1 = i1 >= i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l3 = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kPendingThrowSlot;
        i1 = 0u;
        StoreU32(ctx, (uint64_t)(i0), i1);
        i0 = p1;
        i1 = 21u;
        i0 += i1;
        l3 = i0;
        i1 = kDataFFFFFFE0;
        i0 &= i1;
        l6 = i0;
      }
      i0 = l8;
      i1 = 26u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = kHostSlot1032;
        i1 = l6;
        i0 = HostFoldLayoutOffset(ctx, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        l5 = i0;
        i0 = 26u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l5;
        l3 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kPendingThrowSlot;
        i0 = LoadU32(ctx, (uint64_t)(i0));
        l11 = i0;
        i0 = kPendingThrowSlot;
        i1 = 0u;
        StoreU32(ctx, (uint64_t)(i0), i1);
        i0 = l11;
        i1 = 1u;
        i0 = i0 == i1;
        l11 = i0;
        if (i0) {goto block_206;}
        i0 = l2;
        i1 = l3;
        StoreU32(ctx, (uint64_t)(i0) + 24u, i1);
        i0 = l2;
        i1 = p1;
        StoreU32(ctx, (uint64_t)(i0) + 28u, i1);
        i0 = l2;
        i1 = l6;
        i2 = kData80000000;
        i1 |= i2;
        l6 = i1;
        StoreU32(ctx, (uint64_t)(i0) + 32u, i1);
        goto block_208;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i1 = p1;
      StoreU8(ctx, (uint64_t)(i0) + 35u, i1);
      i0 = l2;
      i1 = 24u;
      i0 += i1;
      l3 = i0;
      i0 = p1;
      i0 = !(i0);
      l6 = i0;
      if (i0) {goto block_207;}
    }
    block_208:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = l4;
      i2 = p1;
      i0 = CopyBytes(ctx, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l4 = i0;
    }
    block_207:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = p1;
      i1 = l3;
      i0 += i1;
      i1 = 0u;
      StoreU8(ctx, (uint64_t)(i0), i1);
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = l2;
      i1 = 24u;
      i0 += i1;
      p1 = i0;
    }
    i0 = l8;
    i1 = 27u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = kTableSlot860;
      i1 = p1;
      i2 = kSha1Suffix;
      i3 = 5u;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l5 = i0;
      i0 = 27u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      p1 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kPendingThrowSlot;
      i0 = LoadU32(ctx, (uint64_t)(i0));
      p1 = i0;
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = p1;
      i1 = 1u;
      i0 = i0 != i1;
      p1 = i0;
      if (i0) {goto block_197;}
    }
    block_206:;
    block_198:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l7 = i0;
      i0 = HostTempRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = l2;
      i0 = LoadS8(ctx, (uint64_t)(i0) + 35u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      p0 = i0;
      if (i0) {goto block_48;}
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + 24u);
      p0 = i0;
      DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      goto block_48;
    }
    block_197:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + 24u);
      i1 = l2;
      i2 = 24u;
      i1 += i2;
      i2 = l2;
      i2 = LoadS8(ctx, (uint64_t)(i2) + 35u);
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      l6 = i2;
      i0 = i2 ? i0 : i1;
      l4 = i0;
      i0 = l2;
      i1 = 84u;
      i0 += i1;
      l3 = i0;
      i0 = l9;
      i1 = l2;
      i2 = 40u;
      i1 += i2;
      i2 = l10;
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      i0 = i2 ? i0 : i1;
      p1 = i0;
    }
    i0 = l8;
    i1 = 28u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2400u;
      i1 = p1;
      i2 = l4;
      i3 = l3;
      HostSyncSlot(ctx, i0, i1, i2, i3);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = 28u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kPendingThrowSlot;
      i0 = LoadU32(ctx, (uint64_t)(i0));
      p1 = i0;
      i0 = kPendingThrowSlot;
      i1 = 0u;
      StoreU32(ctx, (uint64_t)(i0), i1);
      i0 = p1;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_49;}
      i0 = l2;
      i0 = LoadS8(ctx, (uint64_t)(i0) + 35u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, (uint64_t)(i0) + 24u);
        DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
      }
      i0 = l2;
      i0 = LoadS8(ctx, (uint64_t)(i0) + 51u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, (uint64_t)(i0) + 40u);
        DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
      }
      i0 = l2;
      i0 = LoadS8(ctx, (uint64_t)(i0) + 107u);
      l9 = i0;
    }
    block_145:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l9;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      if (i0) {goto block_57;}
    }
    block_144:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + 96u);
      DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
    }
    block_57:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + kSaveFrameBytes);
      l4 = i0;
      if (i0) {
        i0 = l4;
        p1 = i0;
        i1 = l2;
        i1 = LoadU32(ctx, (uint64_t)(i1) + 60u);
        l3 = i1;
        i0 = i0 == i1;
        l6 = i0;
        i0 = !(i0);
        if (i0) {
          loop_229:; 
            i0 = l3;
            i1 = 12u;
            i0 -= i1;
            p1 = i0;
            i0 = l3;
            i1 = 1u;
            i0 -= i1;
            i0 = LoadS8(ctx, (uint64_t)(i0));
            i1 = kDataFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            if (i0) {
              i0 = p1;
              i0 = LoadU32(ctx, (uint64_t)(i0));
              DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
              SyncMemoryBase(ctx);
#endif
            }
            i0 = p1;
            l3 = i0;
            i0 = p1;
            i1 = l4;
            i0 = i0 != i1;
            if (i0) {goto loop_229;}
          i0 = l2;
          i0 = LoadU32(ctx, (uint64_t)(i0) + kSaveFrameBytes);
          p1 = i0;
        }
        i0 = l2;
        i1 = l4;
        StoreU32(ctx, (uint64_t)(i0) + 60u, i1);
        i0 = p1;
        DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
      }
      i0 = l7;
      i1 = 12u;
      i0 += i1;
      l7 = i0;
      i1 = l2;
      i1 = LoadU32(ctx, (uint64_t)(i1) + 76u);
      i0 = i0 == i1;
      p1 = i0;
      if (i0) {goto block_13;}
      goto loop_50;
    }
    block_56:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    l7 = i0;
    i0 = HostTempRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    i0 = l2;
    i0 = LoadS8(ctx, (uint64_t)(i0) + 51u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    p0 = i0;
    if (i0) {goto block_47;}
    i0 = l2;
    i0 = LoadU32(ctx, (uint64_t)(i0) + 40u);
    p0 = i0;
    DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    goto block_47;
  }
  block_49:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    l7 = i0;
    i0 = HostTempRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    i0 = l2;
    i0 = LoadS8(ctx, (uint64_t)(i0) + 35u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    p0 = i0;
    if (i0) {goto block_48;}
    i0 = l2;
    i0 = LoadU32(ctx, (uint64_t)(i0) + 24u);
    p0 = i0;
    DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
  }
  block_48:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i0 = LoadS8(ctx, (uint64_t)(i0) + 51u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    p0 = i0;
    if (i0) {goto block_47;}
    i0 = l2;
    i0 = LoadU32(ctx, (uint64_t)(i0) + 40u);
    p0 = i0;
    DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
  }
  block_47:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i0 = LoadS8(ctx, (uint64_t)(i0) + 107u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    p0 = i0;
    if (i0) {goto block_44;}
    i0 = l2;
    i0 = LoadU32(ctx, (uint64_t)(i0) + 96u);
    p0 = i0;
    DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    goto block_44;
  }
  block_46:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    l7 = i0;
    i0 = HostTempRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    p0 = i0;
    goto block_44;
  }
  block_45:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    l7 = i0;
    i0 = HostTempRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    p0 = i0;
  }
  block_44:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i0 = LoadU32(ctx, (uint64_t)(i0) + kSaveFrameBytes);
    p0 = i0;
    i0 = !(i0);
    if (i0) {goto block_14;}
    i0 = p0;
    l4 = i0;
    i1 = l2;
    i1 = LoadU32(ctx, (uint64_t)(i1) + 60u);
    l3 = i1;
    i0 = i0 != i1;
    if (i0) {
      loop_239:; 
        i0 = l3;
        i1 = 12u;
        i0 -= i1;
        p1 = i0;
        i0 = l3;
        i1 = 1u;
        i0 -= i1;
        i0 = LoadS8(ctx, (uint64_t)(i0));
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = p1;
          i0 = LoadU32(ctx, (uint64_t)(i0));
          DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
          SyncMemoryBase(ctx);
#endif
        }
        i0 = p1;
        l3 = i0;
        i0 = p0;
        i1 = p1;
        i0 = i0 != i1;
        if (i0) {goto loop_239;}
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + kSaveFrameBytes);
      l4 = i0;
    }
    i0 = l2;
    i1 = p0;
    StoreU32(ctx, (uint64_t)(i0) + 60u, i1);
    i0 = l4;
    DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
  }
  block_14:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i0 = LoadU32(ctx, (uint64_t)(i0) + 72u);
    p0 = i0;
    if (i0) {
      i0 = p0;
      l4 = i0;
      i1 = l2;
      i1 = LoadU32(ctx, (uint64_t)(i1) + 76u);
      l3 = i1;
      i0 = i0 != i1;
      if (i0) {
        loop_244:; 
          i0 = l3;
          i1 = 12u;
          i0 -= i1;
          p1 = i0;
          i0 = l3;
          i1 = 1u;
          i0 -= i1;
          i0 = LoadS8(ctx, (uint64_t)(i0));
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = p1;
            i0 = LoadU32(ctx, (uint64_t)(i0));
            DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
            SyncMemoryBase(ctx);
#endif
          }
          i0 = p1;
          l3 = i0;
          i0 = p0;
          i1 = p1;
          i0 = i0 != i1;
          if (i0) {goto loop_244;}
        i0 = l2;
        i0 = LoadU32(ctx, (uint64_t)(i0) + 72u);
        l4 = i0;
      }
      i0 = l2;
      i1 = p0;
      StoreU32(ctx, (uint64_t)(i0) + 76u, i1);
      i0 = l4;
      DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
    }
    i0 = l7;
    HostOsRelease(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    TrapUnreachable();
  }
  block_13:;
  i0 = ResumePhase(ctx);
  if (i0) {
    i0 = p0;
  } else {
    i0 = l2;
    i0 = LoadU32(ctx, (uint64_t)(i0) + 72u);
  }
  block_12:;
  p0 = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = p0;
    i0 = !(i0);
    if (i0) {goto block_5;}
    i0 = p0;
    i1 = l7;
    i0 = i0 == i1;
    if (i0) {
      i0 = p0;
    } else {
      loop_249:; 
        i0 = l7;
        i1 = 12u;
        i0 -= i1;
        p1 = i0;
        i0 = l7;
        i1 = 1u;
        i0 -= i1;
        i0 = LoadS8(ctx, (uint64_t)(i0));
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = p1;
          i0 = LoadU32(ctx, (uint64_t)(i0));
          DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
          SyncMemoryBase(ctx);
#endif
        }
        i0 = p1;
        l7 = i0;
        i0 = p0;
        i1 = p1;
        i0 = i0 != i1;
        if (i0) {goto loop_249;}
      i0 = l2;
      i0 = LoadU32(ctx, (uint64_t)(i0) + 72u);
    }
    p1 = i0;
    i0 = l2;
    i1 = p0;
    StoreU32(ctx, (uint64_t)(i0) + 76u, i1);
    i0 = p1;
    DeallocateRawBlock(ctx, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    goto block_5;
  }
  block_11:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    TrapUnreachable();
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i1 = kScratchFrameBytes;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    i0 = l13;
    i1 = 0u;
    i0 = i0 != i1;
    goto func_exit;
  }
  TrapUnreachable();
  spill_frame:;
  l5 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, (uint64_t)(i0));
  i1 = l5;
  StoreU32(ctx, (uint64_t)(i0), i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, (uint64_t)(i1));
  i2 = kResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, (uint64_t)(i0), i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, (uint64_t)(i0));
  l5 = i0;
  i1 = p0;
  StoreU32(ctx, (uint64_t)(i0), i1);
  i0 = l5;
  i1 = p1;
  StoreU32(ctx, (uint64_t)(i0) + kResumeWordBytes, i1);
  i0 = l5;
  i1 = l2;
  StoreU32(ctx, (uint64_t)(i0) + 8u, i1);
  i0 = l5;
  i1 = l3;
  StoreU32(ctx, (uint64_t)(i0) + 12u, i1);
  i0 = l5;
  i1 = l4;
  StoreU32(ctx, (uint64_t)(i0) + 16u, i1);
  i0 = l5;
  i1 = l6;
  StoreU32(ctx, (uint64_t)(i0) + 20u, i1);
  i0 = l5;
  i1 = l7;
  StoreU32(ctx, (uint64_t)(i0) + 24u, i1);
  i0 = l5;
  i1 = l9;
  StoreU32(ctx, (uint64_t)(i0) + 28u, i1);
  i0 = l5;
  i1 = l10;
  StoreU32(ctx, (uint64_t)(i0) + 32u, i1);
  i0 = l5;
  i1 = l11;
  StoreU32(ctx, (uint64_t)(i0) + 36u, i1);
  i0 = l5;
  i1 = l12;
  StoreU32(ctx, (uint64_t)(i0) + 40u, i1);
  i0 = l5;
  i1 = l13;
  StoreU32(ctx, (uint64_t)(i0) + 44u, i1);
  i0 = l5;
  i1 = l14;
  StoreU32(ctx, (uint64_t)(i0) + 48u, i1);
  i0 = l5;
  i1 = l15;
  StoreU32(ctx, (uint64_t)(i0) + 52u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, (uint64_t)(i1));
  i2 = kSaveFrameBytes;
  i1 += i2;
  StoreU32(ctx, (uint64_t)(i0), i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}  // namespace artc

}  // namespace artc
