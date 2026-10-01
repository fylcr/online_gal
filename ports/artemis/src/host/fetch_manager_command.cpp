#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step calls.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void ClearUnorderedBuckets(EngineContext* ctx, uint32_t a);
void Panic(EngineContext* ctx, uint32_t a0);
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
uint32_t CopyBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t StringLength(EngineContext* ctx, uint32_t text);
void RethrowPendingException(EngineContext* ctx, uint32_t a);
uint32_t DestroyFileObject(EngineContext* ctx, uint32_t a);
uint32_t ReleaseBufferedChainNode(EngineContext* ctx, uint32_t a);
uint32_t Fwrite(EngineContext* ctx, uint32_t ptr, uint32_t size, uint32_t count, uint32_t stream);

namespace FetchManager {
// FetchManager::CommandDispatch -- the async fetch command dispatcher: it
// matches the command word and serves the clear_cache, baseurl and wait
// commands, parking the guest until the host reply arrives.

void CommandDispatch(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  constexpr uint32_t kData8BA0C = 0x8BA0Cu;
  constexpr uint32_t kData90610 = 0x90610u;
  constexpr uint32_t kData90680 = 0x90680u;
  constexpr uint32_t kDataAE298 = 0xAE298u;
  constexpr uint32_t kMsgSWasmSyncStatus = 0xAFAB9u;  // s.wasm_sync_status
  constexpr uint32_t kDataAFAC1 = 0xAFAC1u;
  constexpr uint32_t kDataAFAC9 = 0xAFAC9u;
  constexpr uint32_t kMsgSWasmSyncTotal = 0xAFACCu;  // s.wasm_sync_total
  constexpr uint32_t kDataAFAD4 = 0xAFAD4u;
  constexpr uint32_t kDataAFADC = 0xAFADCu;
  constexpr uint32_t kMsgSWasmSyncCurrent = 0xAFADEu;  // s.wasm_sync_current
  constexpr uint32_t kDataAFAE6 = 0xAFAE6u;
  constexpr uint32_t kDataAFAED = 0xAFAEDu;
  constexpr uint32_t kDataB4BE6 = 0xB4BE6u;
  constexpr uint32_t kDataB4BE8 = 0xB4BE8u;
  constexpr uint32_t kMsgCommand = 0xB5200u;  // command
  constexpr uint32_t kDataB5203 = 0xB5203u;
  constexpr uint32_t kMsgDelete = 0xB538Bu;  // delete
  constexpr uint32_t kMsgTarget = 0xB5589u;  // target
  constexpr uint32_t kDataB558D = 0xB558Du;
  constexpr uint32_t kMsgCopy = 0xB59A5u;  // copy
  constexpr uint32_t kDataB59AA = 0xB59AAu;
  constexpr uint32_t kDataB59AC = 0xB59ACu;
  constexpr uint32_t kDataB59AE = 0xB59AEu;
  constexpr uint32_t kDataB59C9 = 0xB59C9u;
  constexpr uint32_t kDataB59CB = 0xB59CBu;
  constexpr uint32_t kDataB59CD = 0xB59CDu;
  constexpr uint32_t kMsgClearCache = 0xB59F2u;  // clear_cache
  constexpr uint32_t kMsgWasmSync = 0xB59FEu;  // wasm_sync
  constexpr uint32_t kMsgBaseurl = 0xB5A08u;  // baseurl
  constexpr uint32_t kDataB5A0B = 0xB5A0Bu;
  constexpr uint32_t kMsgWasmSyncAddPersistent = 0xB5A10u;  // wasm_sync_add_persistent
  constexpr uint32_t kMsgWasmSyncDeletePersistent = 0xB5A29u;  // wasm_sync_delete_persistent
  constexpr uint32_t kDataB5E30 = 0xB5E30u;
  constexpr uint32_t kMsgPx = 0xB5E31u;  // px != 0
  constexpr uint32_t kMsgSmartPtrPath = 0xB5E39u;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
  constexpr uint32_t kMsgOperator = 0xB5E71u;  // operator->
  constexpr uint32_t kDataB90E8 = 0xB90E8u;
  constexpr uint32_t kDataCF0B4 = 0xCF0B4u;
  constexpr uint32_t kDataCF0CC = 0xCF0CCu;
  constexpr uint32_t kDataCF0E4 = 0xCF0E4u;
  constexpr uint32_t kDataCF12C = 0xCF12Cu;
  constexpr uint32_t kDataFFFFF = 0xFFFFFu;
  constexpr uint32_t kData100000 = 0x100000u;
  constexpr uint32_t kData7473696C = 0x7473696Cu;
  constexpr uint32_t kData80000000 = 0x80000000u;
  constexpr uint32_t kDataFFF00000 = 0xFFF00000u;
  constexpr uint32_t kDataFFFFFFC0 = 0xFFFFFFC0u;
  constexpr uint32_t kDataFFFFFFF0 = 0xFFFFFFF0u;
  constexpr uint32_t kDataFFFFFFFF = 0xFFFFFFFFu;
  constexpr uint32_t kSaveFrameBytes = 40u;
  constexpr uint32_t kResumeWordBytes = 4u;
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u; uint32_t i6 = 0u;
  uint64_t j1 = 0; uint64_t j3 = 0;
      l11 = 0;
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
    l6 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 24u);
    l8 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 28u);
    l9 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 32u);
    l10 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 36u);
    l11 = i0;
    i0 = l6;
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
    l7 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    i1 = 144u;
    i0 -= i1;
    l3 = i0;
    GuestStackSp(ctx) = i0;
    i0 = l3;
    i1 = 7u;
    StoreU8(ctx, i0 + 75u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 71u, i1);
    i0 = l3;
    i1 = kMsgCommand;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 64u, i1);
    i0 = l3;
    i1 = kDataB5203;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 67u, i1);
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    i2 = kDataFFFFFFC0;
    i1 -= i2;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l3;
    i1 = 40u;
    i0 += i1;
    l4 = i0;
    i0 = b;
    i1 = 16u;
    i0 += i1;
    l10 = i0;
    i0 = l3;
    i1 = kDataFFFFFFC0;
    i0 -= i1;
    l5 = i0;
    i0 = l3;
    i1 = 24u;
    i0 += i1;
    l8 = i0;
    i0 = l3;
    i1 = 8u;
    i0 += i1;
    l9 = i0;
    i0 = 0u;
    c = i0;
  }
  i0 = l7;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = l4;
    i2 = l10;
    i3 = l5;
    i4 = kDataB5E30;
    i5 = l8;
    i6 = l9;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
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
    l4 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
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
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 40u);
      l4 = i0;
      i0 = LoadU32(ctx, i0 + 32u);
      l8 = i0;
      i0 = l4;
      i0 = LoadU8(ctx, i0 + 39u);
      l5 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l9 = i0;
      i0 = l8;
      i1 = l5;
      i2 = l9;
      i0 = i2 ? i0 : i1;
      i1 = 4u;
      i0 = i0 == i1;
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 28u;
        i0 += i1;
        c = i0;
      }
      i0 = l7;
      i1 = 1u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1354u;
        i1 = c;
        i2 = 0u;
        i3 = kDataFFFFFFFF;
        i4 = kMsgCopy;
        i5 = 4u;
        i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
        l6 = i0;
        i0 = 1u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l6;
        c = i0;
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = c;
      } else {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 1u;
        i0 = i0 == i1;
        l4 = i0;
        if (i0) {goto block_5;}
        i0 = c;
        i0 = !(i0);
      }
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 75u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l4 = i0;
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 64u);
        l4 = i0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = c;
      i0 = !(i0);
      c = i0;
      if (i0) {goto block_11;}
      i0 = l3;
      i1 = kDataFFFFFFC0;
      i0 -= i1;
      l5 = i0;
      i1 = kDataAE298;
      StoreU32(ctx, i0, i1);
      i0 = kDataCF12C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = l5;
      j1 = 0ull;
      StoreU64(ctx, i0 + 8u, j1);
      i0 = l5;
      i1 = c;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l5;
      j1 = 0ull;
      StoreU64(ctx, i0 + 16u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 92u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 100u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 108u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 113u, j1);
      i0 = l3;
      i1 = kData90680;
      StoreU32(ctx, i0 + 88u, i1);
      i0 = l3;
      i1 = kDataB90E8;
      StoreU32(ctx, i0 + 64u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 56u, i1);
      i0 = l3;
      i1 = 3u;
      StoreU8(ctx, i0 + 51u, i1);
      i0 = l3;
      i1 = kDataB59AA;
      i1 = LoadU16(ctx, i1);
      StoreU16(ctx, i0 + 40u, i1);
      i0 = l3;
      i1 = kDataB59AC;
      i1 = LoadU8(ctx, i1);
      StoreU8(ctx, i0 + 42u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 43u, i1);
      i0 = l3;
      i1 = l3;
      i2 = 40u;
      i1 += i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 40u;
      i0 += i1;
      l4 = i0;
      i0 = l3;
      i1 = 8u;
      i0 += i1;
      l8 = i0;
      i0 = l3;
      i1 = 136u;
      i0 += i1;
      l9 = i0;
      i0 = l3;
      i1 = 24u;
      i0 += i1;
      c = i0;
    }
    i0 = l7;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1959u;
      i1 = c;
      i2 = l10;
      i3 = l4;
      i4 = kDataB5E30;
      i5 = l8;
      i6 = l9;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
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
      c = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 == i1;
      c = i0;
      if (i0) {goto block_20;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 24u);
      l4 = i0;
      i1 = 28u;
      i0 += i1;
      c = i0;
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 39u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = c;
        i0 = LoadU32(ctx, i0);
        c = i0;
      }
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = kDataFFFFFFC0;
      i0 -= i1;
      l4 = i0;
    }
    i0 = l7;
    i1 = 3u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2647u;
      i1 = l4;
      i2 = c;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l6 = i0;
      i0 = 3u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l6;
      c = i0;
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
      l4 = i0;
      if (i0) {goto block_20;}
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 51u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l4 = i0;
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 40u);
        l4 = i0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = c;
      i0 = !(i0);
      c = i0;
      if (i0) {goto block_19;}
      i0 = kDataD015C;
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
      i0 = 2590u;
      i1 = a;
      i2 = 1u;
      i3 = b;
      i4 = kDataB59AE;
      i5 = 0u;
      HostInitResources(ctx, i0, i1, i2, i3, i4, i5);
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
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_18;}
      goto block_13;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 75u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_8;}
    goto block_6;
  }
  block_20:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 51u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_9;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 40u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_9;
  }
  block_19:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 3u;
    StoreU8(ctx, i0 + 35u, i1);
    i0 = l3;
    i1 = kDataB59C9;
    i1 = LoadU16(ctx, i1);
    StoreU16(ctx, i0 + 24u, i1);
    i0 = l3;
    i1 = kDataB59CB;
    i1 = LoadU8(ctx, i1);
    StoreU8(ctx, i0 + 26u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 27u, i1);
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    i2 = 24u;
    i1 += i2;
    StoreU32(ctx, i0 + 136u, i1);
    i0 = l3;
    i1 = 24u;
    i0 += i1;
    l4 = i0;
    i0 = l3;
    i1 = 136u;
    i0 += i1;
    l8 = i0;
    i0 = l3;
    i1 = 128u;
    i0 += i1;
    l9 = i0;
    i0 = l3;
    i1 = 8u;
    i0 += i1;
    c = i0;
  }
  i0 = l7;
  i1 = 5u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = c;
    i2 = l10;
    i3 = l4;
    i4 = kDataB5E30;
    i5 = l8;
    i6 = l9;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
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
    c = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = 1u;
    i0 = i0 == i1;
    c = i0;
    if (i0) {goto block_17;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
    i1 = 28u;
    i0 += i1;
    c = i0;
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 39u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = c;
      i0 = LoadU32(ctx, i0);
      c = i0;
    }
    i0 = c;
    i0 = StringLength(ctx, i0);
    l10 = i0;
    i1 = kDataFFFFFFF0;
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
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 40u;
      i0 += i1;
      a = i0;
    }
    i0 = l7;
    i1 = 6u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1286u;
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
      if (i0) {goto block_17;}
      goto block_7;
    }
  }
  i0 = l4;
  i1 = l10;
  i2 = 11u;
  i1 = i1 >= i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l4 = i0;
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
      i0 = l10;
      i1 = 16u;
      i0 += i1;
      l4 = i0;
      i1 = kDataFFFFFFF0;
      i0 &= i1;
      l9 = i0;
    }
    i0 = l7;
    i1 = 7u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = l9;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l6 = i0;
      i0 = 7u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l6;
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      l8 = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l8;
      i1 = 1u;
      i0 = i0 == i1;
      l8 = i0;
      if (i0) {goto block_17;}
      i0 = l3;
      i1 = l4;
      StoreU32(ctx, i0 + 40u, i1);
      i0 = l3;
      i1 = l10;
      StoreU32(ctx, i0 + 44u, i1);
      i0 = l3;
      i1 = l9;
      i2 = kData80000000;
      i1 |= i2;
      l8 = i1;
      StoreU32(ctx, i0 + 48u, i1);
      goto block_52;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = l10;
    StoreU8(ctx, i0 + 51u, i1);
    i0 = l3;
    i1 = 40u;
    i0 += i1;
    l4 = i0;
    i0 = l10;
    i0 = !(i0);
    l8 = i0;
    if (i0) {goto block_51;}
  }
  block_52:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = c;
    i2 = l10;
    i0 = CopyBytes(ctx, i0, i1, i2);
    c = i0;
  }
  block_51:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l10;
    i0 += i1;
    i1 = 0u;
    StoreU8(ctx, i0, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 2680u);
    c = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 56u;
    i0 += i1;
    l8 = i0;
    i0 = l3;
    i1 = 40u;
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
    i0 = 2398u;
    i1 = l4;
    i2 = l8;
    i3 = c;
    i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
    l6 = i0;
    i0 = 8u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l6;
    c = i0;
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
    l4 = i0;
    if (i0) {goto block_16;}
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 51u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 40u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 35u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    l4 = i0;
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 24u);
      l4 = i0;
      DeallocateRawBlock(ctx, i0);
    }
    i0 = c;
    i0 = !(i0);
    c = i0;
    if (i0) {goto block_14;}
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 9u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2590u;
    i1 = a;
    i2 = 1u;
    i3 = b;
    i4 = kDataB59CD;
    i5 = 0u;
    HostInitResources(ctx, i0, i1, i2, i3, i4, i5);
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
    if (i0) {goto block_13;}
  }
  block_18:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_9;
  }
  block_17:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_15;
  }
  block_16:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 51u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_15;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 40u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_15:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 35u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_9;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 24u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_9;
  }
  block_14:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 10u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1407u;
    i1 = kData100000;
    i0 = HostFoldLayoutOffset(ctx, i0, i1);
    l6 = i0;
    i0 = 10u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l6;
    b = i0;
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
    if (i0) {goto block_12;}
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 11u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1032u;
    i1 = 20u;
    i0 = HostFoldLayoutOffset(ctx, i0, i1);
    l6 = i0;
    i0 = 11u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l6;
    a = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    c = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = 1u;
    i0 = i0 == i1;
    c = i0;
  }
  i0 = c;
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
      a = i0;
      i0 = HostTempRet0(ctx);
      c = i0;
      i0 = a;
      i0 = HostRestorePendingThrow(ctx, i0);
      a = i0;
      i0 = b;
      DeallocateRawBlock(ctx, i0);
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 12u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1019u;
      HostMarkRegionSeen(ctx, i0);
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
      if (i0) {goto block_7;}
      i0 = HostRet0(ctx);
      c = i0;
      i0 = HostTempRet0(ctx);
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 13u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1021u;
      HostMarkRegionSeen(ctx, i0);
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
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_9;}
      goto block_5;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  i1 = 0u;
  i2 = ResumePhase(ctx);
  i2 = !(i2);
  if (i2) {
    i2 = a;
    j3 = 4294967297ull;
    StoreU64(ctx, i2 + 4u, j3);
    i2 = a;
    i3 = b;
    StoreU32(ctx, i2 + 12u, i3);
    i2 = a;
    i3 = kData8BA0C;
    StoreU32(ctx, i2, i3);
    i2 = l3;
    i2 = LoadU32(ctx, i2 + 108u);
    c = i2;
    i2 = !(i2);
    l4 = i2;
  }
  i2 = l4;
  i0 = i2 ? i0 : i1;
  if (i0) {goto block_82;}
  loop_85: 
    i0 = l4;
    i1 = c;
    i2 = kDataFFFFF;
    i1 = i1 <= i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l4 = i0;
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
        i0 = l3;
        i1 = kDataFFFFFFC0;
        i0 -= i1;
        l4 = i0;
      }
      i0 = l7;
      i1 = 14u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 2052u;
        i1 = l4;
        i2 = b;
        i3 = c;
        i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
        l6 = i0;
        i0 = 14u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l6;
        l4 = i0;
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
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 56u);
          l4 = i0;
        }
        i0 = l7;
        i1 = 15u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = b;
          i1 = 1u;
          i2 = c;
          i3 = l4;
          i0 = Fwrite(ctx, i0, i1, i2, i3);
          l6 = i0;
          i0 = 15u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l6;
          b = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto block_82;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_10;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = kDataFFFFFFC0;
      i0 -= i1;
      l4 = i0;
    }
    i0 = l7;
    i1 = 16u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2052u;
      i1 = l4;
      i2 = b;
      i3 = kData100000;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l6 = i0;
      i0 = 16u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l6;
      l4 = i0;
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
      if (i0) {goto block_10;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 56u);
      l4 = i0;
    }
    i0 = l7;
    i1 = 17u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = 1u;
      i2 = kData100000;
      i3 = l4;
      i0 = Fwrite(ctx, i0, i1, i2, i3);
      l6 = i0;
      i0 = 17u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l6;
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = c;
      i1 = kDataFFF00000;
      i0 += i1;
      c = i0;
      if (i0) {goto loop_85;}
    }
  block_82:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 56u);
    b = i0;
  }
  i0 = l7;
  i1 = 18u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i0 = ReleaseBufferedChainNode(ctx, i0);
    l6 = i0;
    i0 = 18u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l6;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 4u);
    b = i1;
    i2 = 1u;
    i1 -= i2;
    c = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_13;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 19u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
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
    b = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_5;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 8u);
    b = i1;
    i2 = 1u;
    i1 -= i2;
    c = i1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_13;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    b = i0;
    i0 = kDataD015C;
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
    i0 = b;
    i1 = a;
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
    if (i0) {goto block_5;}
  }
  block_13:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = kData90610;
    StoreU32(ctx, i0 + 64u, i1);
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 116u);
    a = i0;
    i0 = !(i0);
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    if (i0) {goto block_107;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 4u);
    b = i1;
    i2 = 1u;
    i1 -= i2;
    c = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_107;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
    i0 = kDataD015C;
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
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_5;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 8u);
    b = i1;
    i2 = 1u;
    i1 -= i2;
    c = i1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_107;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    b = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
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
    if (i0) {goto block_5;}
  }
  block_107:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = kData90680;
    StoreU32(ctx, i0 + 88u, i1);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 103u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    a = i0;
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 92u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
    }
  }
  i0 = l7;
  i1 = 23u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l5;
    i0 = DestroyFileObject(ctx, i0);
    l6 = i0;
    i0 = 23u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l6;
    a = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 144u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_12:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_9;
  }
  block_11:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 7u;
    StoreU8(ctx, i0 + 75u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 71u, i1);
    i0 = l3;
    i1 = kMsgCommand;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 64u, i1);
    i0 = l3;
    i1 = kDataB5203;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 67u, i1);
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    i2 = kDataFFFFFFC0;
    i1 -= i2;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l3;
    i1 = 40u;
    i0 += i1;
    l4 = i0;
    i0 = l3;
    i1 = kDataFFFFFFC0;
    i0 -= i1;
    l5 = i0;
    i0 = l3;
    i1 = 24u;
    i0 += i1;
    l8 = i0;
    i0 = l3;
    i1 = 8u;
    i0 += i1;
    l9 = i0;
    i0 = 0u;
    c = i0;
  }
  i0 = l7;
  i1 = 24u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = l4;
    i2 = l10;
    i3 = l5;
    i4 = kDataB5E30;
    i5 = l8;
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
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
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
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 40u);
      l4 = i0;
      i0 = LoadU32(ctx, i0 + 32u);
      l8 = i0;
      i0 = l4;
      i0 = LoadU8(ctx, i0 + 39u);
      l5 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l9 = i0;
      i0 = l8;
      i1 = l5;
      i2 = l9;
      i0 = i2 ? i0 : i1;
      i1 = 6u;
      i0 = i0 == i1;
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 28u;
        i0 += i1;
        c = i0;
      }
      i0 = l7;
      i1 = 25u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1354u;
        i1 = c;
        i2 = 0u;
        i3 = kDataFFFFFFFF;
        i4 = kMsgDelete;
        i5 = 6u;
        i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
        l6 = i0;
        i0 = 25u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l6;
        c = i0;
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = c;
      } else {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 1u;
        i0 = i0 == i1;
        l4 = i0;
        if (i0) {goto block_5;}
        i0 = c;
        i0 = !(i0);
      }
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 75u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l4 = i0;
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 64u);
        l4 = i0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = c;
      i0 = !(i0);
      c = i0;
      if (i0) {goto block_121;}
      i0 = l3;
      i1 = 6u;
      StoreU8(ctx, i0 + 75u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 70u, i1);
      i0 = l3;
      i1 = kMsgTarget;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0 + 64u, i1);
      i0 = l3;
      i1 = kDataB558D;
      i1 = LoadU16(ctx, i1);
      StoreU16(ctx, i0 + 68u, i1);
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = l3;
      i2 = kDataFFFFFFC0;
      i1 -= i2;
      StoreU32(ctx, i0 + 24u, i1);
      i0 = l3;
      i1 = kDataFFFFFFC0;
      i0 -= i1;
      c = i0;
      i0 = l3;
      i1 = 24u;
      i0 += i1;
      l4 = i0;
      i0 = l3;
      i1 = 8u;
      i0 += i1;
      l5 = i0;
      i0 = l3;
      i1 = 40u;
      i0 += i1;
      b = i0;
    }
    i0 = l7;
    i1 = 26u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1959u;
      i1 = b;
      i2 = l10;
      i3 = c;
      i4 = kDataB5E30;
      i5 = l4;
      i6 = l5;
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
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
      if (i0) {goto block_122;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 40u);
      b = i0;
      i0 = a;
      i0 = LoadU32(ctx, i0 + 2680u);
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 28u;
      i0 += i1;
      b = i0;
    }
    i0 = l7;
    i1 = 27u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2697u;
      i1 = b;
      i2 = a;
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
      if (i0) {goto block_122;}
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 75u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_120;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 64u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_120;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 75u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_8;}
    goto block_6;
  }
  block_122:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 75u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_8;}
    goto block_6;
  }
  block_121:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 7u;
    StoreU8(ctx, i0 + 75u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 71u, i1);
    i0 = l3;
    i1 = kMsgCommand;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 64u, i1);
    i0 = l3;
    i1 = kDataB5203;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 67u, i1);
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    i2 = kDataFFFFFFC0;
    i1 -= i2;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l3;
    i1 = 40u;
    i0 += i1;
    l4 = i0;
    i0 = l3;
    i1 = kDataFFFFFFC0;
    i0 -= i1;
    l5 = i0;
    i0 = l3;
    i1 = 24u;
    i0 += i1;
    l8 = i0;
    i0 = l3;
    i1 = 8u;
    i0 += i1;
    l9 = i0;
    i0 = 0u;
    c = i0;
  }
  i0 = l7;
  i1 = 28u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = l4;
    i2 = l10;
    i3 = l5;
    i4 = kDataB5E30;
    i5 = l8;
    i6 = l9;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
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
    l4 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
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
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 40u);
      l4 = i0;
      i0 = LoadU32(ctx, i0 + 32u);
      l8 = i0;
      i0 = l4;
      i0 = LoadU8(ctx, i0 + 39u);
      l5 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l9 = i0;
      i0 = l8;
      i1 = l5;
      i2 = l9;
      i0 = i2 ? i0 : i1;
      i1 = 11u;
      i0 = i0 == i1;
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 28u;
        i0 += i1;
        c = i0;
      }
      i0 = l7;
      i1 = 29u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1354u;
        i1 = c;
        i2 = 0u;
        i3 = kDataFFFFFFFF;
        i4 = kMsgClearCache;
        i5 = 11u;
        i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
        l6 = i0;
        i0 = 29u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l6;
        c = i0;
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = c;
      } else {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 1u;
        i0 = i0 == i1;
        l4 = i0;
        if (i0) {goto block_5;}
        i0 = c;
        i0 = !(i0);
      }
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 75u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l4 = i0;
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 64u);
        l4 = i0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = c;
      i0 = !(i0);
      c = i0;
      if (i0) {goto block_142;}
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 30u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 2698u;
      i1 = kDataCF0B4;
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
      a = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_5;}
    }
    i0 = l7;
    i1 = 31u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = kDataCF0CC;
      ClearUnorderedBuckets(ctx, i0);
      i0 = 31u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = l7;
    i1 = 32u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = kDataCF0E4;
      RethrowPendingException(ctx, i0);
      i0 = 32u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_120;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 75u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_8;}
    goto block_6;
  }
  block_142:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 7u;
    StoreU8(ctx, i0 + 75u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 71u, i1);
    i0 = l3;
    i1 = kMsgCommand;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 64u, i1);
    i0 = l3;
    i1 = kDataB5203;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 67u, i1);
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    i2 = kDataFFFFFFC0;
    i1 -= i2;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l3;
    i1 = 40u;
    i0 += i1;
    l4 = i0;
    i0 = l3;
    i1 = kDataFFFFFFC0;
    i0 -= i1;
    l5 = i0;
    i0 = l3;
    i1 = 24u;
    i0 += i1;
    l8 = i0;
    i0 = l3;
    i1 = 8u;
    i0 += i1;
    l9 = i0;
    i0 = 0u;
    c = i0;
  }
  i0 = l7;
  i1 = 33u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = l4;
    i2 = l10;
    i3 = l5;
    i4 = kDataB5E30;
    i5 = l8;
    i6 = l9;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
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
    l4 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
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
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 40u);
      l4 = i0;
      i0 = LoadU32(ctx, i0 + 32u);
      l8 = i0;
      i0 = l4;
      i0 = LoadU8(ctx, i0 + 39u);
      l5 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l9 = i0;
      i0 = l8;
      i1 = l5;
      i2 = l9;
      i0 = i2 ? i0 : i1;
      i1 = 9u;
      i0 = i0 == i1;
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
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 28u;
        i0 += i1;
        c = i0;
      }
      i0 = l7;
      i1 = 34u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1354u;
        i1 = c;
        i2 = 0u;
        i3 = kDataFFFFFFFF;
        i4 = kMsgWasmSync;
        i5 = 9u;
        i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
        l6 = i0;
        i0 = 34u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l6;
        c = i0;
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = c;
      } else {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 1u;
        i0 = i0 == i1;
        l4 = i0;
        if (i0) {goto block_5;}
        i0 = c;
        i0 = !(i0);
      }
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 75u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l4 = i0;
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 64u);
        l4 = i0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = c;
      i0 = !(i0);
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = c;
      if (i0) {goto block_179;}
      i0 = a;
      i0 = LoadU32(ctx, i0 + 112u);
      l4 = i0;
      i0 = !(i0);
      c = i0;
    }
    i0 = c;
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
      i0 = l7;
      i1 = 35u;
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
        a = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_7;}
        goto block_5;
      }
    }
    i0 = l7;
    i1 = 36u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 32u;
      i0 = AllocateStringBuffer(ctx, i0);
      l6 = i0;
      i0 = 36u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l6;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = c;
      StoreU32(ctx, i0 + 64u, i1);
      i0 = l3;
      j1 = 9223372174293729297ull;
      StoreU64(ctx, i0 + 68u, j1);
      i0 = c;
      i1 = 0u;
      StoreU8(ctx, i0 + 17u, i1);
      i0 = c;
      i1 = kDataAFADC;
      i1 = LoadU8(ctx, i1);
      StoreU8(ctx, i0 + 16u, i1);
      i0 = c;
      i1 = kDataAFAD4;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0 + 8u, j1);
      i0 = c;
      i1 = kMsgSWasmSyncTotal;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = l4;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 20u);
      c = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = kDataFFFFFFC0;
      i0 -= i1;
      l5 = i0;
    }
    i0 = l7;
    i1 = 37u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l4;
      i2 = l5;
      i3 = 0u;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l6 = i0;
      i0 = 37u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l6;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 == i1;
      c = i0;
      if (i0) {goto block_177;}
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 75u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 64u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = a;
      i0 = LoadU32(ctx, i0 + 112u);
      l4 = i0;
      i0 = !(i0);
      c = i0;
    }
    i0 = c;
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
      i0 = l7;
      i1 = 38u;
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
        a = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_7;}
        goto block_5;
      }
    }
    i0 = l7;
    i1 = 39u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 32u;
      i0 = AllocateStringBuffer(ctx, i0);
      l6 = i0;
      i0 = 39u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l6;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = c;
      StoreU32(ctx, i0 + 64u, i1);
      i0 = l3;
      j1 = 9223372174293729299ull;
      StoreU64(ctx, i0 + 68u, j1);
      i0 = c;
      i1 = 0u;
      StoreU8(ctx, i0 + 19u, i1);
      i0 = c;
      i1 = kDataAFAED;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0 + 15u, i1);
      i0 = c;
      i1 = kDataAFAE6;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0 + 8u, j1);
      i0 = c;
      i1 = kMsgSWasmSyncCurrent;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = l4;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 20u);
      c = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = kDataFFFFFFC0;
      i0 -= i1;
      l5 = i0;
    }
    i0 = l7;
    i1 = 40u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = l4;
      i2 = l5;
      i3 = 0u;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l6 = i0;
      i0 = 40u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l6;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kDataD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 == i1;
      c = i0;
      if (i0) {goto block_178;}
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 75u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 64u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l3;
      i1 = 3u;
      StoreU8(ctx, i0 + 75u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 67u, i1);
      i0 = l3;
      i1 = kDataB4BE8;
      i1 = LoadU8(ctx, i1);
      StoreU8(ctx, i0 + 66u, i1);
      i0 = l3;
      i1 = kDataB4BE6;
      i1 = LoadU16(ctx, i1);
      c = i1;
      StoreU16(ctx, i0 + 64u, i1);
      i0 = b;
      i0 = LoadU32(ctx, i0 + 20u);
      b = i0;
    }
    i0 = b;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = c;
      i1 = b;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      c = i0;
      loop_201: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = c;
          i0 = LoadU8(ctx, i0 + 27u);
          l4 = i0;
          i1 = 24u;
          i0 <<= (i1 & 31);
          i1 = 24u;
          i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
          i1 = 0u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          l9 = i0;
          i0 = c;
          i0 = LoadU32(ctx, i0 + 20u);
          i1 = l4;
          i2 = l9;
          i0 = i2 ? i0 : i1;
          l4 = i0;
          i1 = 3u;
          i0 = i0 < i1;
          l8 = i0;
          i0 = l4;
          i1 = 3u;
          i2 = l8;
          i0 = i2 ? i0 : i1;
          l5 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l5;
          if (i0) {
            i0 = l3;
            i1 = kDataFFFFFFC0;
            i0 -= i1;
            i1 = c;
            i2 = 16u;
            i1 += i2;
            l11 = i1;
            i1 = LoadU32(ctx, i1);
            i2 = l11;
            i3 = l9;
            i1 = i3 ? i1 : i2;
            l9 = i1;
            i2 = l5;
            i0 = CompareBytes(ctx, i0, i1, i2);
            l11 = i0;
            i0 = !(i0);
            if (i0) {
              i0 = l4;
              i1 = 3u;
              i0 = i0 <= i1;
              l4 = i0;
              if (i0) {goto block_209;}
              goto block_203;
            }
            i0 = l11;
            i1 = kDataFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            l4 = i0;
            if (i0) {goto block_203;}
            block_209:;
            i0 = l9;
            i1 = l3;
            i2 = kDataFFFFFFC0;
            i1 -= i2;
            i2 = l5;
            i0 = CompareBytes(ctx, i0, i1, i2);
            l4 = i0;
            i0 = !(i0);
            l5 = i0;
            if (i0) {goto block_207;}
            i0 = l4;
            i1 = kDataFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            l4 = i0;
            if (i0) {goto block_206;}
            goto block_204;
          }
          i0 = l4;
          i1 = 3u;
          i0 = i0 > i1;
          l4 = i0;
          if (i0) {goto block_203;}
          block_207:;
          i0 = l8;
          if (i0) {goto block_204;}
          block_206:;
          i0 = a;
          i0 = LoadU32(ctx, i0 + 112u);
          c = i0;
          if (i0) {goto block_165;}
          i0 = kDataD015C;
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
          i0 = 857u;
          i1 = kMsgPx;
          i2 = kMsgSmartPtrPath;
          i3 = 734u;
          i4 = kMsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 41u;
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
          if (i0) {goto block_7;}
          goto block_5;
        }
        block_204:;
        i0 = c;
        i1 = c;
        i2 = 4u;
        i1 += i2;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        c = i0;
        block_203:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = c;
          i0 = LoadU32(ctx, i0);
          c = i0;
          if (i0) {goto loop_201;}
        }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 7u;
      StoreU8(ctx, i0 + 75u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 71u, i1);
      i0 = l3;
      i1 = kMsgBaseurl;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0 + 64u, i1);
      i0 = l3;
      i1 = kDataB5A0B;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0 + 67u, i1);
      i0 = b;
      i0 = !(i0);
      c = i0;
      if (i0) {goto block_164;}
      i0 = b;
      c = i0;
    }
    loop_215: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = c;
        i0 = LoadU8(ctx, i0 + 27u);
        l4 = i0;
        i1 = 24u;
        i0 <<= (i1 & 31);
        i1 = 24u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l9 = i0;
        i0 = c;
        i0 = LoadU32(ctx, i0 + 20u);
        i1 = l4;
        i2 = l9;
        i0 = i2 ? i0 : i1;
        l4 = i0;
        i1 = 7u;
        i0 = i0 < i1;
        l8 = i0;
        i0 = l4;
        i1 = 7u;
        i2 = l8;
        i0 = i2 ? i0 : i1;
        l5 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        if (i0) {
          i0 = l3;
          i1 = kDataFFFFFFC0;
          i0 -= i1;
          i1 = c;
          i2 = 16u;
          i1 += i2;
          l11 = i1;
          i1 = LoadU32(ctx, i1);
          i2 = l11;
          i3 = l9;
          i1 = i3 ? i1 : i2;
          l9 = i1;
          i2 = l5;
          i0 = CompareBytes(ctx, i0, i1, i2);
          l11 = i0;
          i0 = !(i0);
          if (i0) {
            i0 = l4;
            i1 = 7u;
            i0 = i0 <= i1;
            l4 = i0;
            if (i0) {goto block_223;}
            goto block_217;
          }
          i0 = l11;
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          l4 = i0;
          if (i0) {goto block_217;}
          block_223:;
          i0 = l9;
          i1 = l3;
          i2 = kDataFFFFFFC0;
          i1 -= i2;
          i2 = l5;
          i0 = CompareBytes(ctx, i0, i1, i2);
          l4 = i0;
          i0 = !(i0);
          l5 = i0;
          if (i0) {goto block_221;}
          i0 = l4;
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          l4 = i0;
          if (i0) {goto block_220;}
          goto block_218;
        }
        i0 = l4;
        i1 = 7u;
        i0 = i0 > i1;
        l4 = i0;
        if (i0) {goto block_217;}
        block_221:;
        i0 = l8;
        if (i0) {goto block_218;}
        block_220:;
        i0 = l3;
        i1 = 0u;
        StoreU8(ctx, i0 + 44u, i1);
        i0 = l3;
        i1 = kData7473696C;
        StoreU32(ctx, i0 + 40u, i1);
        i0 = l3;
        i1 = 4u;
        StoreU8(ctx, i0 + 51u, i1);
      }
      loop_225: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = b;
          i0 = LoadU8(ctx, i0 + 27u);
          c = i0;
          i1 = 24u;
          i0 <<= (i1 & 31);
          i1 = 24u;
          i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
          i1 = 0u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          l5 = i0;
          i0 = b;
          i0 = LoadU32(ctx, i0 + 20u);
          i1 = c;
          i2 = l5;
          i0 = i2 ? i0 : i1;
          c = i0;
          i1 = kResumeWordBytes;
          i0 = i0 < i1;
          l9 = i0;
          i0 = c;
          i1 = kResumeWordBytes;
          i2 = l9;
          i0 = i2 ? i0 : i1;
          l4 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          if (i0) {
            i0 = l3;
            i1 = 40u;
            i0 += i1;
            i1 = b;
            i2 = 16u;
            i1 += i2;
            l8 = i1;
            i1 = LoadU32(ctx, i1);
            i2 = l8;
            i3 = l5;
            i1 = i3 ? i1 : i2;
            l5 = i1;
            i2 = l4;
            i0 = CompareBytes(ctx, i0, i1, i2);
            l8 = i0;
            i0 = !(i0);
            l11 = i0;
            if (i0) {
              i0 = c;
              i1 = 4u;
              i0 = i0 <= i1;
              c = i0;
              if (i0) {goto block_234;}
              goto block_228;
            }
            i0 = l8;
            i1 = kDataFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            c = i0;
            if (i0) {goto block_228;}
            block_234:;
            i0 = l5;
            i1 = l3;
            i2 = 40u;
            i1 += i2;
            i2 = l4;
            i0 = CompareBytes(ctx, i0, i1, i2);
            c = i0;
            i0 = !(i0);
            l4 = i0;
            if (i0) {goto block_232;}
            i0 = c;
            i1 = kDataFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            c = i0;
            if (i0) {goto block_231;}
            goto block_229;
          }
          i0 = c;
          i1 = 4u;
          i0 = i0 > i1;
          c = i0;
          if (i0) {goto block_228;}
          block_232:;
          i0 = l9;
          if (i0) {goto block_229;}
          block_231:;
          i0 = a;
          i0 = LoadU32(ctx, i0 + 112u);
          c = i0;
          if (i0) {goto block_226;}
          i0 = kDataD015C;
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
          i1 = kMsgPx;
          i2 = kMsgSmartPtrPath;
          i3 = 734u;
          i4 = kMsgOperator;
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
          if (i0) {goto block_7;}
          goto block_5;
        }
        block_229:;
        i0 = b;
        i1 = b;
        i2 = 4u;
        i1 += i2;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        b = i0;
        block_228:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = b;
          i0 = LoadU32(ctx, i0);
          b = i0;
          if (i0) {goto loop_225;}
          goto block_164;
        }
        block_226:;
      i0 = l7;
      i1 = 43u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 32u;
        i0 = AllocateStringBuffer(ctx, i0);
        l6 = i0;
        i0 = 43u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l6;
        b = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i1 = b;
        StoreU32(ctx, i0 + 64u, i1);
        i0 = l3;
        j1 = 9223372174293729298ull;
        StoreU64(ctx, i0 + 68u, j1);
        i0 = b;
        i1 = 0u;
        StoreU8(ctx, i0 + 18u, i1);
        i0 = b;
        i1 = kDataAFAC9;
        i1 = LoadU16(ctx, i1);
        StoreU16(ctx, i0 + 16u, i1);
        i0 = b;
        i1 = kDataAFAC1;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0 + 8u, j1);
        i0 = b;
        i1 = kMsgSWasmSyncStatus;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0, j1);
        i0 = c;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 20u);
        b = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = kDataFFFFFFC0;
        i0 -= i1;
        l4 = i0;
      }
      i0 = l7;
      i1 = 44u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = b;
        i1 = c;
        i2 = l4;
        i3 = 5u;
        i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
        l6 = i0;
        i0 = 44u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l6;
        b = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kDataD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kDataD015C;
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
          i0 = l3;
          i0 = LoadS8(ctx, i0 + 75u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 64u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l3;
          i1 = 0u;
          StoreU32(ctx, i0 + 72u, i1);
          i0 = l3;
          j1 = 0ull;
          StoreU64(ctx, i0 + 64u, j1);
          i0 = l3;
          i1 = 0u;
          StoreU8(ctx, i0 + 44u, i1);
          i0 = l3;
          i1 = kData7473696C;
          StoreU32(ctx, i0 + 40u, i1);
          i0 = l3;
          i1 = 4u;
          StoreU8(ctx, i0 + 51u, i1);
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = l3;
          i2 = 40u;
          i1 += i2;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l3;
          i1 = 40u;
          i0 += i1;
          c = i0;
          i0 = l3;
          i1 = 8u;
          i0 += i1;
          l4 = i0;
          i0 = l3;
          i1 = 136u;
          i0 += i1;
          l5 = i0;
          i0 = l3;
          i1 = 24u;
          i0 += i1;
          b = i0;
        }
        i0 = l7;
        i1 = 45u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1959u;
          i1 = b;
          i2 = l10;
          i3 = c;
          i4 = kDataB5E30;
          i5 = l4;
          i6 = l5;
          HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
          i0 = 45u;
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
          b = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = b;
          i1 = 1u;
          i0 = i0 == i1;
          b = i0;
          if (i0) {goto block_248;}
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 24u);
          b = i0;
          i1 = 28u;
          i0 += i1;
          c = i0;
          i0 = b;
          i0 = LoadS8(ctx, i0 + 39u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = c;
            i0 = LoadU32(ctx, i0);
            c = i0;
          }
          i0 = l3;
          i1 = 44u;
          StoreU16(ctx, i0 + 24u, i1);
          i0 = l3;
          i1 = 1u;
          StoreU8(ctx, i0 + 35u, i1);
          i0 = l3;
          i1 = 92u;
          StoreU16(ctx, i0 + 8u, i1);
          i0 = l3;
          i1 = 1u;
          StoreU8(ctx, i0 + 19u, i1);
          i0 = a;
          i0 = LoadU32(ctx, i0 + 2680u);
          b = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = 24u;
          i0 += i1;
          l5 = i0;
          i0 = l3;
          i1 = 8u;
          i0 += i1;
          l8 = i0;
          i0 = l3;
          i1 = kDataFFFFFFC0;
          i0 -= i1;
          l4 = i0;
        }
        i0 = l7;
        i1 = 46u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2015u;
          i1 = c;
          i2 = l4;
          i3 = l5;
          i4 = l8;
          i5 = b;
          HostInitResources(ctx, i0, i1, i2, i3, i4, i5);
          i0 = 46u;
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
          b = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = b;
          i1 = 1u;
          i0 = i0 == i1;
          b = i0;
          if (i0) {goto block_247;}
          i0 = l3;
          i0 = LoadS8(ctx, i0 + 19u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 8u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l3;
          i0 = LoadS8(ctx, i0 + 35u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 24u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l3;
          i0 = LoadS8(ctx, i0 + 51u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 40u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l3;
          i1 = 0u;
          StoreU32(ctx, i0 + 48u, i1);
          i0 = l3;
          j1 = 0ull;
          StoreU64(ctx, i0 + 40u, j1);
          i0 = l3;
          i1 = 10u;
          StoreU16(ctx, i0 + 24u, i1);
          i0 = l3;
          i1 = 1u;
          StoreU8(ctx, i0 + 35u, i1);
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 64u);
          c = i0;
          i1 = l3;
          i1 = LoadU32(ctx, i1 + 68u);
          i0 = i0 == i1;
          b = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = b;
          if (i0) {goto block_262;}
          i0 = 0u;
          l4 = i0;
        }
        loop_264: 
          i0 = l4;
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
              i0 = l3;
              i1 = 40u;
              i0 += i1;
              l8 = i0;
              i0 = l3;
              i0 = LoadU32(ctx, i0 + 24u);
              i1 = l3;
              i2 = 24u;
              i1 += i2;
              i2 = l3;
              i2 = LoadU8(ctx, i2 + 35u);
              b = i2;
              i3 = 24u;
              i2 <<= (i3 & 31);
              i3 = 24u;
              i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
              i3 = 0u;
              i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
              l5 = i2;
              i0 = i2 ? i0 : i1;
              l9 = i0;
              i0 = l3;
              i0 = LoadU32(ctx, i0 + 28u);
              l11 = i0;
              i1 = b;
              i2 = l5;
              i0 = i2 ? i0 : i1;
              b = i0;
            }
            i0 = l7;
            i1 = 47u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 860u;
              i1 = l8;
              i2 = l9;
              i3 = b;
              i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
              l6 = i0;
              i0 = 47u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
              i0 = l6;
              b = i0;
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kDataD015C;
              i0 = LoadU32(ctx, i0);
              b = i0;
              i0 = kDataD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = b;
              i1 = 1u;
              i0 = i0 == i1;
              b = i0;
              if (i0) {goto block_245;}
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = c;
            i0 = LoadU32(ctx, i0);
            l5 = i0;
            i0 = c;
            i0 = LoadU32(ctx, i0 + 4u);
            l9 = i0;
            i0 = c;
            i0 = LoadU8(ctx, i0 + 11u);
            b = i0;
            i0 = kDataD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 40u;
            i0 += i1;
            l11 = i0;
            i0 = l5;
            i1 = c;
            i2 = b;
            i3 = 24u;
            i2 <<= (i3 & 31);
            i3 = 24u;
            i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
            i3 = 0u;
            i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
            l5 = i2;
            i0 = i2 ? i0 : i1;
            l8 = i0;
            i0 = l9;
            i1 = b;
            i2 = l5;
            i0 = i2 ? i0 : i1;
            b = i0;
          }
          i0 = l7;
          i1 = 48u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 860u;
            i1 = l11;
            i2 = l8;
            i3 = b;
            i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
            l6 = i0;
            i0 = 48u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l6;
            b = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kDataD015C;
            i0 = LoadU32(ctx, i0);
            b = i0;
            i0 = kDataD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = b;
            i1 = 1u;
            i0 = i0 == i1;
            b = i0;
            if (i0) {goto block_245;}
            i0 = l4;
            i1 = 1u;
            i0 += i1;
            l4 = i0;
            i0 = c;
            i1 = 12u;
            i0 += i1;
            c = i0;
            i1 = l3;
            i1 = LoadU32(ctx, i1 + 68u);
            i0 = i0 != i1;
            b = i0;
            if (i0) {goto loop_264;}
          }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i0 = LoadS8(ctx, i0 + 35u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          b = i0;
          if (i0) {goto block_262;}
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 24u);
          b = i0;
          DeallocateRawBlock(ctx, i0);
        }
        block_262:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = a;
          i0 = LoadU32(ctx, i0 + 24u);
          b = i0;
          i0 = !(i0);
          c = i0;
        }
        i0 = c;
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
          i0 = l7;
          i1 = 49u;
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
            i0 = 49u;
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
            if (i0) {goto block_7;}
            goto block_5;
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i1 = 7u;
          StoreU8(ctx, i0 + 35u, i1);
          i0 = l3;
          i1 = 0u;
          StoreU8(ctx, i0 + 31u, i1);
          i0 = l3;
          i1 = kMsgBaseurl;
          i1 = LoadU32(ctx, i1);
          StoreU32(ctx, i0 + 24u, i1);
          i0 = l3;
          i1 = kDataB5A0B;
          i1 = LoadU32(ctx, i1);
          StoreU32(ctx, i0 + 27u, i1);
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = l3;
          i2 = 24u;
          i1 += i2;
          StoreU32(ctx, i0 + 136u, i1);
          i0 = l3;
          i1 = 24u;
          i0 += i1;
          l4 = i0;
          i0 = l3;
          i1 = 136u;
          i0 += i1;
          l5 = i0;
          i0 = l3;
          i1 = 56u;
          i0 += i1;
          l8 = i0;
          i0 = l3;
          i1 = 8u;
          i0 += i1;
          c = i0;
        }
        i0 = l7;
        i1 = 50u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1959u;
          i1 = c;
          i2 = l10;
          i3 = l4;
          i4 = kDataB5E30;
          i5 = l5;
          i6 = l8;
          HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
          i0 = 50u;
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
          c = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = c;
          i1 = 1u;
          i0 = i0 == i1;
          c = i0;
          if (i0) {goto block_244;}
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 8u);
          c = i0;
          i0 = b;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 20u);
          l4 = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = 40u;
          i0 += i1;
          l5 = i0;
          i0 = c;
          i1 = 28u;
          i0 += i1;
          c = i0;
        }
        i0 = l7;
        i1 = 51u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l4;
          i1 = b;
          i2 = c;
          i3 = l5;
          i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
          l6 = i0;
          i0 = 51u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l6;
          b = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          b = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = b;
          i1 = 1u;
          i0 = i0 == i1;
          b = i0;
          if (i0) {goto block_244;}
          i0 = l3;
          i0 = LoadS8(ctx, i0 + 35u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 24u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l3;
          i0 = LoadS8(ctx, i0 + 51u);
          i1 = kDataFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 40u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 64u);
          l4 = i0;
          i0 = !(i0);
          b = i0;
          if (i0) {goto block_164;}
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 68u);
          c = i0;
          i1 = l4;
          b = i1;
          i0 = i0 == i1;
          l5 = i0;
          i0 = !(i0);
          if (i0) {
            loop_287: 
              i0 = c;
              i1 = 12u;
              i0 -= i1;
              b = i0;
              i0 = c;
              i1 = 1u;
              i0 -= i1;
              i0 = LoadS8(ctx, i0);
              i1 = kDataFFFFFFFF;
              i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
              if (i0) {
                i0 = b;
                i0 = LoadU32(ctx, i0);
                DeallocateRawBlock(ctx, i0);
              }
              i0 = b;
              c = i0;
              i0 = b;
              i1 = l4;
              i0 = i0 != i1;
              if (i0) {goto loop_287;}
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 64u);
            b = i0;
          }
          i0 = l3;
          i1 = l4;
          StoreU32(ctx, i0 + 68u, i1);
          i0 = b;
          DeallocateRawBlock(ctx, i0);
          goto block_164;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        c = i0;
        i0 = HostTempRet0(ctx);
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 75u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        a = i0;
        if (i0) {goto block_8;}
        goto block_6;
      }
      block_248:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        c = i0;
        i0 = HostTempRet0(ctx);
        a = i0;
        goto block_246;
      }
      block_247:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        c = i0;
        i0 = HostTempRet0(ctx);
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 19u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 8u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 35u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        a = i0;
        if (i0) {goto block_246;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 24u);
        a = i0;
        DeallocateRawBlock(ctx, i0);
      }
      block_246:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 51u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        a = i0;
        if (i0) {goto block_242;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 40u);
        a = i0;
        DeallocateRawBlock(ctx, i0);
        goto block_242;
      }
      block_245:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        c = i0;
        i0 = HostTempRet0(ctx);
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 35u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        a = i0;
        if (i0) {goto block_243;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 24u);
        a = i0;
        DeallocateRawBlock(ctx, i0);
        goto block_243;
      }
      block_244:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        c = i0;
        i0 = HostTempRet0(ctx);
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 35u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        a = i0;
        if (i0) {goto block_243;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 24u);
        a = i0;
        DeallocateRawBlock(ctx, i0);
      }
      block_243:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 51u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        a = i0;
        if (i0) {goto block_242;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 40u);
        a = i0;
        DeallocateRawBlock(ctx, i0);
      }
      block_242:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 64u);
        a = i0;
        i0 = !(i0);
        if (i0) {goto block_8;}
        i0 = a;
        b = i0;
        i1 = l3;
        i1 = LoadU32(ctx, i1 + 68u);
        l4 = i1;
        i0 = i0 != i1;
        if (i0) {
          loop_299: 
            i0 = l4;
            i1 = 12u;
            i0 -= i1;
            b = i0;
            i0 = l4;
            i1 = 1u;
            i0 -= i1;
            i0 = LoadS8(ctx, i0);
            i1 = kDataFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            if (i0) {
              i0 = b;
              i0 = LoadU32(ctx, i0);
              DeallocateRawBlock(ctx, i0);
            }
            i0 = b;
            l4 = i0;
            i0 = a;
            i1 = b;
            i0 = i0 != i1;
            if (i0) {goto loop_299;}
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 64u);
          b = i0;
        }
        i0 = l3;
        i1 = a;
        StoreU32(ctx, i0 + 68u, i1);
        i0 = b;
        DeallocateRawBlock(ctx, i0);
        i0 = c;
        HostOsRelease(ctx, i0);
        TrapUnreachable();
      }
      block_218:;
      i0 = c;
      i1 = c;
      i2 = 4u;
      i1 += i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      c = i0;
      block_217:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = c;
        i0 = LoadU32(ctx, i0);
        c = i0;
        if (i0) {goto loop_215;}
      }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_164;}
    block_179:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 7u;
      StoreU8(ctx, i0 + 75u, i1);
      i0 = 0u;
      c = i0;
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 71u, i1);
      i0 = l3;
      i1 = kMsgCommand;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0 + 64u, i1);
      i0 = l3;
      i1 = kDataB5203;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0 + 67u, i1);
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = l3;
      i2 = kDataFFFFFFC0;
      i1 -= i2;
      StoreU32(ctx, i0 + 24u, i1);
      i0 = l3;
      i1 = kDataFFFFFFC0;
      i0 -= i1;
      l4 = i0;
      i0 = l3;
      i1 = 24u;
      i0 += i1;
      l5 = i0;
      i0 = l3;
      i1 = 8u;
      i0 += i1;
      l8 = i0;
      i0 = l3;
      i1 = 40u;
      i0 += i1;
      b = i0;
    }
    i0 = l7;
    i1 = 52u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1959u;
      i1 = b;
      i2 = l10;
      i3 = l4;
      i4 = kDataB5E30;
      i5 = l5;
      i6 = l8;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 52u;
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
      b = i0;
      i0 = kDataD015C;
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
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 40u);
        b = i0;
        i0 = LoadU32(ctx, i0 + 32u);
        l5 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 39u);
        l4 = i0;
        i1 = 24u;
        i0 <<= (i1 & 31);
        i1 = 24u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l8 = i0;
        i0 = l5;
        i1 = l4;
        i2 = l8;
        i0 = i2 ? i0 : i1;
        i1 = 24u;
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
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = b;
          i1 = 28u;
          i0 += i1;
          b = i0;
        }
        i0 = l7;
        i1 = 53u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1354u;
          i1 = b;
          i2 = 0u;
          i3 = kDataFFFFFFFF;
          i4 = kMsgWasmSyncAddPersistent;
          i5 = 24u;
          i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
          l6 = i0;
          i0 = 53u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l6;
          b = i0;
        }
        i0 = ResumePhase(ctx);
        if (i0) {
          i0 = c;
        } else {
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          c = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = c;
          i1 = 1u;
          i0 = i0 == i1;
          if (i0) {goto block_5;}
          i0 = b;
          i0 = !(i0);
        }
        c = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 75u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 64u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = c;
        i0 = !(i0);
        b = i0;
        if (i0) {goto block_304;}
        i0 = a;
        i0 = LoadU32(ctx, i0 + 24u);
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
          i0 = kDataD015C;
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
          i1 = kMsgPx;
          i2 = kMsgSmartPtrPath;
          i3 = 734u;
          i4 = kMsgOperator;
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
          if (i0) {goto block_7;}
          goto block_5;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i1 = 6u;
        StoreU8(ctx, i0 + 75u, i1);
        i0 = l3;
        i1 = 0u;
        StoreU8(ctx, i0 + 70u, i1);
        i0 = l3;
        i1 = kMsgTarget;
        i1 = LoadU32(ctx, i1);
        StoreU32(ctx, i0 + 64u, i1);
        i0 = l3;
        i1 = kDataB558D;
        i1 = LoadU16(ctx, i1);
        StoreU16(ctx, i0 + 68u, i1);
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = l3;
        i2 = kDataFFFFFFC0;
        i1 -= i2;
        StoreU32(ctx, i0 + 24u, i1);
        i0 = l3;
        i1 = kDataFFFFFFC0;
        i0 -= i1;
        c = i0;
        i0 = l3;
        i1 = 24u;
        i0 += i1;
        l4 = i0;
        i0 = l3;
        i1 = 8u;
        i0 += i1;
        l5 = i0;
        i0 = l3;
        i1 = 40u;
        i0 += i1;
        b = i0;
      }
      i0 = l7;
      i1 = 55u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1959u;
        i1 = b;
        i2 = l10;
        i3 = c;
        i4 = kDataB5E30;
        i5 = l4;
        i6 = l5;
        HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
        i0 = 55u;
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
        b = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto block_305;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 40u);
        b = i0;
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 28u);
        c = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 28u;
        i0 += i1;
        b = i0;
      }
      i0 = l7;
      i1 = 56u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = c;
        i1 = a;
        i2 = b;
        HostOsSchedule(ctx, i0, i1, i2);
        i0 = 56u;
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
        if (i0) {goto block_305;}
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 75u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        a = i0;
        if (i0) {goto block_120;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 64u);
        a = i0;
        DeallocateRawBlock(ctx, i0);
        goto block_120;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      c = i0;
      i0 = HostTempRet0(ctx);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 75u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_8;}
      goto block_6;
    }
    block_305:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      c = i0;
      i0 = HostTempRet0(ctx);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 75u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_8;}
      goto block_6;
    }
    block_304:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 7u;
      StoreU8(ctx, i0 + 75u, i1);
      i0 = 0u;
      c = i0;
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 71u, i1);
      i0 = l3;
      i1 = kMsgCommand;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0 + 64u, i1);
      i0 = l3;
      i1 = kDataB5203;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0 + 67u, i1);
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = l3;
      i2 = kDataFFFFFFC0;
      i1 -= i2;
      StoreU32(ctx, i0 + 24u, i1);
      i0 = l3;
      i1 = kDataFFFFFFC0;
      i0 -= i1;
      l4 = i0;
      i0 = l3;
      i1 = 24u;
      i0 += i1;
      l5 = i0;
      i0 = l3;
      i1 = 8u;
      i0 += i1;
      l8 = i0;
      i0 = l3;
      i1 = 40u;
      i0 += i1;
      b = i0;
    }
    i0 = l7;
    i1 = 57u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1959u;
      i1 = b;
      i2 = l10;
      i3 = l4;
      i4 = kDataB5E30;
      i5 = l5;
      i6 = l8;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 57u;
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
      b = i0;
      i0 = kDataD015C;
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
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 40u);
        b = i0;
        i0 = LoadU32(ctx, i0 + 32u);
        l5 = i0;
        i0 = b;
        i0 = LoadU8(ctx, i0 + 39u);
        l4 = i0;
        i1 = 24u;
        i0 <<= (i1 & 31);
        i1 = 24u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l8 = i0;
        i0 = l5;
        i1 = l4;
        i2 = l8;
        i0 = i2 ? i0 : i1;
        i1 = 27u;
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
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = b;
          i1 = 28u;
          i0 += i1;
          b = i0;
        }
        i0 = l7;
        i1 = 58u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1354u;
          i1 = b;
          i2 = 0u;
          i3 = kDataFFFFFFFF;
          i4 = kMsgWasmSyncDeletePersistent;
          i5 = 27u;
          i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
          l6 = i0;
          i0 = 58u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l6;
          b = i0;
        }
        i0 = ResumePhase(ctx);
        if (i0) {
          i0 = c;
        } else {
          i0 = kDataD015C;
          i0 = LoadU32(ctx, i0);
          c = i0;
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = c;
          i1 = 1u;
          i0 = i0 == i1;
          c = i0;
          if (i0) {goto block_330;}
          i0 = b;
          i0 = !(i0);
        }
        c = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 75u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 64u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = c;
        i0 = !(i0);
        b = i0;
        if (i0) {goto block_120;}
        i0 = a;
        i0 = LoadU32(ctx, i0 + 24u);
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
          i0 = kDataD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l7;
        i1 = 59u;
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
          i0 = 59u;
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
          if (i0) {goto block_7;}
          goto block_5;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i1 = 6u;
        StoreU8(ctx, i0 + 75u, i1);
        i0 = l3;
        i1 = 0u;
        StoreU8(ctx, i0 + 70u, i1);
        i0 = l3;
        i1 = kMsgTarget;
        i1 = LoadU32(ctx, i1);
        StoreU32(ctx, i0 + 64u, i1);
        i0 = l3;
        i1 = kDataB558D;
        i1 = LoadU16(ctx, i1);
        StoreU16(ctx, i0 + 68u, i1);
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = l3;
        i2 = kDataFFFFFFC0;
        i1 -= i2;
        StoreU32(ctx, i0 + 24u, i1);
        i0 = l3;
        i1 = kDataFFFFFFC0;
        i0 -= i1;
        c = i0;
        i0 = l3;
        i1 = 24u;
        i0 += i1;
        l4 = i0;
        i0 = l3;
        i1 = 8u;
        i0 += i1;
        l5 = i0;
        i0 = l3;
        i1 = 40u;
        i0 += i1;
        b = i0;
      }
      i0 = l7;
      i1 = 60u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1959u;
        i1 = b;
        i2 = l10;
        i3 = c;
        i4 = kDataB5E30;
        i5 = l4;
        i6 = l5;
        HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
        i0 = 60u;
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
        b = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto block_331;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 40u);
        b = i0;
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 32u);
        c = i0;
        i0 = kDataD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 28u;
        i0 += i1;
        b = i0;
      }
      i0 = l7;
      i1 = 61u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = c;
        i1 = a;
        i2 = b;
        HostOsSchedule(ctx, i0, i1, i2);
        i0 = 61u;
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
        if (i0) {goto block_331;}
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 75u);
        i1 = kDataFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        a = i0;
        if (i0) {goto block_120;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 64u);
        a = i0;
        DeallocateRawBlock(ctx, i0);
        goto block_120;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      c = i0;
      i0 = HostTempRet0(ctx);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 75u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_8;}
      goto block_6;
    }
    block_331:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      c = i0;
      i0 = HostTempRet0(ctx);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 75u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_8;}
      goto block_6;
    }
    block_330:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_5;}
    block_178:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      c = i0;
      i0 = HostTempRet0(ctx);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 75u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_8;}
      goto block_6;
    }
    block_177:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      c = i0;
      i0 = HostTempRet0(ctx);
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 75u);
      i1 = kDataFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_8;}
      goto block_6;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 75u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_8;}
    goto block_6;
  }
  block_165:;
  i0 = l7;
  i1 = 62u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 32u;
    i0 = AllocateStringBuffer(ctx, i0);
    l6 = i0;
    i0 = 62u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l6;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = b;
    StoreU32(ctx, i0 + 64u, i1);
    i0 = l3;
    j1 = 9223372174293729298ull;
    StoreU64(ctx, i0 + 68u, j1);
    i0 = b;
    i1 = 0u;
    StoreU8(ctx, i0 + 18u, i1);
    i0 = b;
    i1 = kDataAFAC9;
    i1 = LoadU16(ctx, i1);
    StoreU16(ctx, i0 + 16u, i1);
    i0 = b;
    i1 = kDataAFAC1;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0 + 8u, j1);
    i0 = b;
    i1 = kMsgSWasmSyncStatus;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0, j1);
    i0 = c;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 20u);
    b = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = kDataFFFFFFC0;
    i0 -= i1;
    l4 = i0;
  }
  i0 = l7;
  i1 = 63u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = c;
    i2 = l4;
    i3 = 3u;
    i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
    l6 = i0;
    i0 = 63u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l6;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    b = i0;
    if (i0) {goto block_163;}
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 75u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 64u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = a;
    i0 = LoadU32(ctx, i0 + 24u);
    b = i0;
    i0 = !(i0);
    c = i0;
  }
  i0 = c;
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
    i0 = l7;
    i1 = 64u;
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
      i0 = 64u;
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
      if (i0) {goto block_7;}
      goto block_5;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 3u;
    StoreU8(ctx, i0 + 75u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 67u, i1);
    i0 = l3;
    i1 = kDataB4BE6;
    i1 = LoadU16(ctx, i1);
    StoreU16(ctx, i0 + 64u, i1);
    i0 = l3;
    i1 = kDataB4BE8;
    i1 = LoadU8(ctx, i1);
    StoreU8(ctx, i0 + 66u, i1);
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    i2 = kDataFFFFFFC0;
    i1 -= i2;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l3;
    i1 = kDataFFFFFFC0;
    i0 -= i1;
    l4 = i0;
    i0 = l3;
    i1 = 24u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = 8u;
    i0 += i1;
    l8 = i0;
    i0 = l3;
    i1 = 40u;
    i0 += i1;
    c = i0;
  }
  i0 = l7;
  i1 = 65u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = c;
    i2 = l10;
    i3 = l4;
    i4 = kDataB5E30;
    i5 = l5;
    i6 = l8;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 65u;
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
    c = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = 1u;
    i0 = i0 == i1;
    c = i0;
    if (i0) {goto block_162;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 40u);
    c = i0;
    i0 = b;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = 28u;
    i0 += i1;
    c = i0;
  }
  i0 = l7;
  i1 = 66u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l4;
    i1 = b;
    i2 = c;
    i0 = HostOsQuery(ctx, i0, i1, i2);
    l6 = i0;
    i0 = 66u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l6;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    b = i0;
    if (i0) {goto block_162;}
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 75u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    b = i0;
    if (i0) {goto block_164;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 64u);
    b = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_164:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = 2876u;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i1 = 13u;
    i0 = i0 == i1;
    c = i0;
    if (i0) {goto block_120;}
    i0 = a;
    i1 = 12u;
    StoreU32(ctx, i0 + 2876u, i1);
    i0 = a;
    i1 = 2880u;
    i0 += i1;
    c = i0;
    i1 = b;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 8u);
    a = i0;
    if (i0) {goto block_161;}
    i0 = kDataD015C;
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
    i1 = kMsgPx;
    i2 = kMsgSmartPtrPath;
    i3 = 734u;
    i4 = kMsgOperator;
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
    if (i0) {goto block_7;}
    goto block_5;
  }
  block_163:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 75u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_8;}
    goto block_6;
  }
  block_162:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 75u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_8;}
    goto block_6;
  }
  block_161:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 28u);
    b = i0;
  }
  i0 = l7;
  i1 = 68u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = 0u;
    i2 = b;
    CallTableT0(ctx, i2, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    i0 = 68u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_120:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 144u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_10:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 4u);
    b = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_9;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
    i0 = kDataD015C;
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
    i0 = b;
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
    i0 = kDataD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kDataD015C;
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
      StoreU32(ctx, i0 + 8u, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_9;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      b = i0;
      i0 = kDataD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 70u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 70u;
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
      if (i0) {goto block_9;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_5;}
  block_9:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = kData90610;
    StoreU32(ctx, i0 + 64u, i1);
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 116u);
    a = i0;
    i0 = !(i0);
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    if (i0) {goto block_388;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 4u);
    b = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_388;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
    i0 = kDataD015C;
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
    i0 = b;
    i1 = a;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 71u;
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
    b = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_5;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 8u);
    b = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_388;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    b = i0;
    i0 = kDataD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 72u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = a;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 72u;
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
    if (i0) {goto block_5;}
  }
  block_388:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = kData90680;
    StoreU32(ctx, i0 + 88u, i1);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 103u);
    i1 = kDataFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    a = i0;
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 92u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
    }
  }
  i0 = l7;
  i1 = 73u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l5;
    i0 = DestroyFileObject(ctx, i0);
    l6 = i0;
    i0 = 73u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l6;
    a = i0;
  }
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    TrapUnreachable();
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 64u);
    DeallocateRawBlock(ctx, i0);
    i0 = c;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    a = i0;
    i0 = HostTempRet0(ctx);
  }
  i0 = l7;
  i1 = 74u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 74u;
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
  l6 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l6;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l6 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l6;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l6;
  i1 = c;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l6;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l6;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l6;
  i1 = l5;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l6;
  i1 = l8;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l6;
  i1 = l9;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l6;
  i1 = l10;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = l6;
  i1 = l11;
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
}
}  // namespace artc
