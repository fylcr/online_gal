#include "util/engine_context.h"

namespace artc {

// Callees whose products land in other files of the same translation unit.
void Panic(EngineContext* ctx, uint32_t a0);
void IntegerToString(EngineContext* ctx, uint32_t a0, uint32_t a1);
void RaiseStringLengthError(EngineContext* ctx, uint32_t a0);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t CopyBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
// The per-item action this dispatch hands each queued entry to.
void IetItemAction(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace IetRunner {

// Walks the queued flow items. One pass reads the current item's kind from the
// container, hands the item to the item action, then builds and schedules the
// two temporary strings the host callback consumes, growing both guest arrays
// when they are full. Every temporary string is released on every exit path.
void FlowDispatch(EngineContext* ctx, uint32_t p0, uint32_t p1, uint32_t p2,
                  uint32_t p3, uint32_t p4) {
  EnterFrame();
  // The host leaves its pending-throw flag here.
  constexpr uint32_t kPendingThrowSlot = 852316u;
  // Host callback ids: the item callback and the two scheduling callbacks.
  constexpr uint32_t kItemCallbackId = 1354u;
  constexpr uint32_t kScheduleCallbackId = 3193u;
  constexpr uint32_t kQueueCallbackId = 1655u;
  uint32_t l5 = 0u;
  uint32_t l6 = 0u;
  uint32_t l7 = 0u;
  uint32_t l8 = 0u;
  uint32_t l10 = 0u;
  uint32_t l11 = 0u;
  uint32_t l12 = 0u;
  uint32_t l13 = 0u;
  uint32_t l14 = 0u;
  uint32_t l15 = 0u;
  uint32_t i0 = 0u;
  uint32_t i1 = 0u;
  uint32_t i2 = 0u;
  uint32_t i3 = 0u;
  uint32_t i4 = 0u;
  uint32_t i5 = 0u;
  uint32_t i6 = 0u;
  uint32_t i7 = 0u;
  uint64_t j1 = 0ull;
  uint32_t resume_point = 0u;
  // Rewinding: pop the saved locals frame and read the locals back.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 56u);
    l7 = LoadU32(ctx, ReturnFrameSp(ctx));
    p0 = LoadU32(ctx, l7);
    p2 = LoadU32(ctx, l7 + 8u);
    p3 = LoadU32(ctx, l7 + 12u);
    p4 = LoadU32(ctx, l7 + 16u);
    l5 = LoadU32(ctx, l7 + 20u);
    l6 = LoadU32(ctx, l7 + 24u);
    l8 = LoadU32(ctx, l7 + 28u);
    l10 = LoadU32(ctx, l7 + 32u);
    l11 = LoadU32(ctx, l7 + 36u);
    l12 = LoadU32(ctx, l7 + 40u);
    l13 = LoadU32(ctx, l7 + 44u);
    l14 = LoadU32(ctx, l7 + 48u);
    l15 = LoadU32(ctx, l7 + 52u);
    p1 = LoadU32(ctx, l7 + 4u);
  }
  // Rewinding: pop the resume point off the frame.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  // The dispatch keeps an 80-byte scratch frame at the guest stack top.
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = GuestStackSp(ctx) - 80u;
    GuestStackSp(ctx) = p1;
  }
  // Read the item's leading byte: the container pointer is taken from the item
  // word when the item's sign byte is negative.
  if (ResumePhase(ctx) == kResumeRunning) {
    i0 = LoadU32(ctx, p3);
    i1 = p3;
    l8 = LoadU8(ctx, p3 + 11u);
    l5 = static_cast<uint32_t>(static_cast<int32_t>(l8 << 24u) >> 24);
    l6 = static_cast<uint32_t>(static_cast<int32_t>(l5) < 0);
    i0 = l6 ? i0 : i1;
    p2 = static_cast<uint32_t>(LoadU8(ctx, i0) == 38u);
  }
  // Item is a container reference (or the pass is rewinding): run the item.
  i0 = p2;
  i0 |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (i0) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      IetItemAction(ctx, p0, p3, p4);
      SyncMemoryBase(ctx);
      resume_point = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto restore_sp_and_return; }
  }
  // The item word is the value; keep the kind in l6, and re-read it once the
  // host callback below has run.
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = LoadU32(ctx, p3 + 4u);
    i0 = l6 ? p2 : l8;
    l6 = static_cast<uint32_t>(i0 == 2u);
  }
  i0 = l6;
  i0 |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (i0) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kPendingThrowSlot, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
      l7 = HostInvokeIiiiii(ctx, kItemCallbackId, p3, 0u, 4294967295u, 758722u, 2u);
      SyncMemoryBase(ctx);
      resume_point = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = l7;
    }
    if (ResumePhase(ctx) != kResumeRunning) {
      i0 = p2;
    } else {
      l6 = LoadU32(ctx, kPendingThrowSlot);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      l5 = static_cast<uint32_t>(l6 == 1u);
      if (l5) { goto find_catch_and_panic; }
      p2 = static_cast<uint32_t>(!p2);
      if (p2) { goto load_item_from_container; }
      l8 = LoadU8(ctx, p3 + 11u);
      l5 = l8;
      i0 = LoadU32(ctx, p3 + 4u);
    }
    p2 = i0;
  }
  // Kind 6 is the raise path; everything else falls through to the kind check.
  if (ResumePhase(ctx) == kResumeRunning) {
    i2 = static_cast<uint32_t>(static_cast<int32_t>(l5 << 24u) >> 24);
    i2 = static_cast<uint32_t>(static_cast<int32_t>(i2) < 0);
    i0 = i2 ? p2 : l8;
    l6 = static_cast<uint32_t>(i0 == 6u);
    if (l6) { goto raise_exception_for_item; }
    goto dispatch_kind_check;
  }
  // Merge point load_item_from_container.
  load_item_from_container:;
  if (ResumePhase(ctx) == kResumeRunning) {
    i0 = LoadU32(ctx, p0 + 96u);
    i0 -= LoadU32(ctx, p0 + 92u);
    l8 = static_cast<uint32_t>(static_cast<int32_t>(i0) >> 2);
    p2 = LoadU32(ctx, p0 + 204u);
    l5 = LoadU32(ctx, p0 + 208u);
    l6 = static_cast<uint32_t>(p2 < l5);
  }
  // Room for the item's kind at the tail of the first array, or grow it.
  if (ResumePhase(ctx) == kResumeRunning) {
    if (l6) {
      StoreU32(ctx, p2, l8);
      p2 += 4u;
      StoreU32(ctx, p0 + 204u, p2);
      goto clear_result_value;
    }
    i0 = p2;
    l12 = p0 + 200u;
    p2 = LoadU32(ctx, l12);
    l11 = static_cast<uint32_t>(i0 - p2);
    l13 = static_cast<uint32_t>(static_cast<int32_t>(l11) >> 2);
    l6 = l13 + 1u;
    l10 = static_cast<uint32_t>(l6 >= 1073741824u);
    if (l10) { goto allocator_assert_fail_first; }
    l5 = static_cast<uint32_t>(l5 - p2);
    l12 = static_cast<uint32_t>(static_cast<int32_t>(l5) >> 1);
    l10 = static_cast<uint32_t>(l12 < l6);
    l6 = l10 ? l6 : l12;
    i2 = static_cast<uint32_t>(static_cast<int32_t>(l5) >> 2);
    l6 = static_cast<uint32_t>(i2 < 536870911u) ? l6 : 1073741823u;
    l10 = static_cast<uint32_t>(!l6);
    l5 = 0u;
  }
  // Grow the first array, or jump straight to the store when a size was found.
  if (ResumePhase(ctx) == kResumeRunning) {
    if (l10) { goto store_new_element; }
    if (l6 >= 1073741824u) { goto alloc_limit_error; }
    l5 = l6 << 2u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
    l7 = AllocateStringBuffer(ctx, l5);
    SyncMemoryBase(ctx);
    resume_point = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    l5 = l7;
  }
  // Merge point store_new_element.
  store_new_element:;
  if (ResumePhase(ctx) == kResumeRunning) {
    l10 = l13 << 2u;
    l12 = l10 + l5;
    StoreU32(ctx, l12, l8);
    if (static_cast<int32_t>(l11) >= 1) {
      CopyBytes(ctx, l5, p2, l11);
      SyncMemoryBase(ctx);
    }
    i1 = l6 << 2u;
    StoreU32(ctx, p0 + 208u, i1 + l5);
    l8 = l12 + 4u;
    StoreU32(ctx, p0 + 204u, l8);
    StoreU32(ctx, p0 + 200u, l5);
    l5 = static_cast<uint32_t>(!p2);
    if (l5) { goto clear_result_value; }
    DeallocateRawBlock(ctx, p2);
    SyncMemoryBase(ctx);
  }
  // Clear the item result record, then queue the entry into the second array.
  // Merge point clear_result_value.
  clear_result_value:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, p1 + 24u, 0u);
    StoreU64(ctx, p1 + 16u, 0ull);
    p2 = LoadU32(ctx, p0 + 216u);
    if (p2 < LoadU32(ctx, p0 + 220u)) {
      StoreU32(ctx, p2 + 8u, 0u);
      StoreU64(ctx, p2, 0ull);
      StoreU32(ctx, p2, LoadU32(ctx, p1 + 16u));
      StoreU32(ctx, p2 + 4u, LoadU32(ctx, p1 + 20u));
      l5 = LoadU32(ctx, p1 + 24u);
      StoreU32(ctx, p2 + 8u, l5);
      p2 += 12u;
      StoreU32(ctx, p0 + 216u, p2);
      goto dispatch_tail;
    }
    StoreU32(ctx, kPendingThrowSlot, 0u);
    l5 = p1 + 16u;
    p2 = p0 + 212u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
    HostOsSchedule(ctx, kQueueCallbackId, p2, l5);
    SyncMemoryBase(ctx);
    resume_point = 3u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // The queue callback reports back through the pending-throw slot.
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = LoadU32(ctx, kPendingThrowSlot);
    StoreU32(ctx, kPendingThrowSlot, 0u);
    if (p2 != 1u) {
      p2 = LoadU32(ctx, p1 + 16u);
      l5 = static_cast<uint32_t>(!p2);
      if (l5) { goto dispatch_tail; }
      StoreU32(ctx, p1 + 20u, p2);
      DeallocateRawBlock(ctx, p2);
      SyncMemoryBase(ctx);
      goto dispatch_tail;
    }
    p4 = HostRet0(ctx);
    SyncMemoryBase(ctx);
    HostTempRet0(ctx);
    SyncMemoryBase(ctx);
    p0 = LoadU32(ctx, p1 + 16u);
    if (!p0) { goto release_slot_then_trap; }
    StoreU32(ctx, p1 + 20u, p0);
    DeallocateRawBlock(ctx, p0);
    SyncMemoryBase(ctx);
    goto release_slot_then_trap;
  }
  // Merge point raise_exception_for_item.
  raise_exception_for_item:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, kPendingThrowSlot, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
    l7 = HostInvokeIiiiii(ctx, kItemCallbackId, p3, 0u, 4294967295u, 758725u, 6u);
    SyncMemoryBase(ctx);
    resume_point = 4u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    p2 = l7;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    l6 = LoadU32(ctx, kPendingThrowSlot);
    StoreU32(ctx, kPendingThrowSlot, 0u);
    if (l6 == 1u) { goto find_catch_and_panic; }
    if (p2) {
      p2 = LoadU32(ctx, p3 + 4u);
      l8 = LoadU8(ctx, p3 + 11u);
      l5 = l8;
      goto dispatch_kind_check;
    }
    p2 = LoadU32(ctx, p0 + 204u);
    l5 = static_cast<uint32_t>(p2 == LoadU32(ctx, p0 + 200u));
  }
  i0 = l5;
  i0 |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (i0) {
    // The first temporary string: 48 bytes holding a fixed literal.
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
      l7 = AllocateStringBuffer(ctx, 48u);
      SyncMemoryBase(ctx);
      resume_point = 5u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = l7;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, p1 + 16u, p2);
      j1 = 9223372243013206055ull;
      StoreU64(ctx, p1 + 20u, j1);
      StoreU8(ctx, p2 + 39u, 0u);
      j1 = LoadU64(ctx, 758763u);
      StoreU64(ctx, p2 + 31u, j1);
      j1 = LoadU64(ctx, 758756u);
      StoreU64(ctx, p2 + 24u, j1);
      j1 = LoadU64(ctx, 758748u);
      StoreU64(ctx, p2 + 16u, j1);
      j1 = LoadU64(ctx, 758740u);
      StoreU64(ctx, p2 + 8u, j1);
      j1 = LoadU64(ctx, 758732u);
      StoreU64(ctx, p2, j1);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      p2 = p1 + 16u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
      HostOsSchedule(ctx, kScheduleCallbackId, p0, p2);
      SyncMemoryBase(ctx);
      resume_point = 6u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kPendingThrowSlot);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      if (p0 != 1u) {
        if (static_cast<int32_t>(LoadS8(ctx, p1 + 27u)) > -1) {
          goto restore_sp_and_return;
        }
        p0 = LoadU32(ctx, p1 + 16u);
        DeallocateRawBlock(ctx, p0);
        SyncMemoryBase(ctx);
        goto restore_sp_and_return;
      }
      p4 = HostRet0(ctx);
      SyncMemoryBase(ctx);
      HostTempRet0(ctx);
      SyncMemoryBase(ctx);
      goto release_string_then_unwind;
    }
  }
  // The scheduled string was consumed; check the item word before rebuilding.
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = static_cast<uint32_t>(static_cast<int32_t>(LoadU32(ctx, p2 - 4u)) <= -1);
  }
  i0 = p2;
  i0 |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (i0) {
    // The second temporary string: 48 bytes holding a fixed literal.
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 7u) {
      l7 = AllocateStringBuffer(ctx, 48u);
      SyncMemoryBase(ctx);
      resume_point = 7u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = l7;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, p1 + 16u, p2);
      j1 = 9223372243013206049ull;
      StoreU64(ctx, p1 + 20u, j1);
      StoreU8(ctx, p2 + 33u, 0u);
      p3 = LoadU8(ctx, 758804u);
      StoreU8(ctx, p2 + 32u, p3);
      j1 = LoadU64(ctx, 758796u);
      StoreU64(ctx, p2 + 24u, j1);
      j1 = LoadU64(ctx, 758788u);
      StoreU64(ctx, p2 + 16u, j1);
      j1 = LoadU64(ctx, 758780u);
      StoreU64(ctx, p2 + 8u, j1);
      j1 = LoadU64(ctx, 758772u);
      StoreU64(ctx, p2, j1);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      p2 = p1 + 16u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 8u) {
      HostOsSchedule(ctx, kScheduleCallbackId, p0, p2);
      SyncMemoryBase(ctx);
      resume_point = 8u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kPendingThrowSlot);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      if (p0 != 1u) {
        if (static_cast<int32_t>(LoadS8(ctx, p1 + 27u)) > -1) {
          goto restore_sp_and_return;
        }
        p0 = LoadU32(ctx, p1 + 16u);
        DeallocateRawBlock(ctx, p0);
        SyncMemoryBase(ctx);
        goto restore_sp_and_return;
      }
      p4 = HostRet0(ctx);
      SyncMemoryBase(ctx);
      HostTempRet0(ctx);
      SyncMemoryBase(ctx);
      goto release_string_then_unwind;
    }
  }
  // Room for the item's kind at the tail of the second array, or grow it.
  if (ResumePhase(ctx) == kResumeRunning) {
    l6 = LoadU32(ctx, p0 + 216u);
    l11 = l6 - 8u;
    p2 = LoadU32(ctx, l11);
    l14 = l6 - 4u;
    l5 = LoadU32(ctx, l14);
    l10 = static_cast<uint32_t>(p2 < l5);
    i0 = LoadU32(ctx, p0 + 96u);
    i0 -= LoadU32(ctx, p0 + 92u);
    l8 = static_cast<uint32_t>(static_cast<int32_t>(i0) >> 2);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (l10) {
      StoreU32(ctx, p2, l8);
      p2 += 4u;
      StoreU32(ctx, l11, p2);
      goto resume_call_10;
    }
    i0 = p2;
    l12 = l6 - 12u;
    p2 = LoadU32(ctx, l12);
    l13 = static_cast<uint32_t>(i0 - p2);
    l10 = static_cast<uint32_t>(static_cast<int32_t>(l13) >> 2);
    l6 = l10 + 1u;
    if (l6 >= 1073741824u) { goto allocator_assert_fail_second; }
    l5 = static_cast<uint32_t>(l5 - p2);
    l15 = static_cast<uint32_t>(static_cast<int32_t>(l5) >> 1);
    l6 = static_cast<uint32_t>(l6 > l15) ? l6 : l15;
    i2 = static_cast<uint32_t>(static_cast<int32_t>(l5) >> 2);
    l6 = static_cast<uint32_t>(i2 < 536870911u) ? l6 : 1073741823u;
    l15 = static_cast<uint32_t>(!l6);
    l5 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (l15) { goto resume_call_9; }
    if (l6 >= 1073741824u) { goto alloc_limit_error; }
    l5 = l6 << 2u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 9u) {
    l7 = AllocateStringBuffer(ctx, l5);
    SyncMemoryBase(ctx);
    resume_point = 9u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    l5 = l7;
  }
  // Merge point resume_call_9.
  resume_call_9:;
  // ---- second array growth: publish the new block and the element ----
  if (ResumePhase(ctx) == kResumeRunning) {
    l10 = (l10 << 2u) + l5;
    StoreU32(ctx, l10, l8);
    if (static_cast<int32_t>(l13) >= 1) {
      CopyBytes(ctx, l5, p2, l13);
      SyncMemoryBase(ctx);
    }
    StoreU32(ctx, l12, l5);
    StoreU32(ctx, l11, l10 + 4u);
    l8 = l6 << 2u;
    StoreU32(ctx, l14, l8 + l5);
    l5 = static_cast<uint32_t>(!p2);
    if (l5) { goto resume_call_10; }
    DeallocateRawBlock(ctx, p2);
    SyncMemoryBase(ctx);
  }
  // ---- the 44-byte flow string for this entry ----
  // Merge point resume_call_10.
  resume_call_10:;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 10u) {
    l7 = AllocateStringBuffer(ctx, 44u);
    SyncMemoryBase(ctx);
    resume_point = 10u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    p2 = l7;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, p1 + 27u, 5u);
    StoreU32(ctx, p1 + 16u, LoadU32(ctx, 758806u));
    StoreU8(ctx, p1 + 20u, LoadU8(ctx, 758810u));
    StoreU8(ctx, p1 + 21u, 0u);
    StoreU32(ctx, p2, 745280u);
    StoreU32(ctx, kPendingThrowSlot, 0u);
    l8 = p1 + 16u;
    l5 = p2 + 4u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 11u) {
    l7 = HostOsQuery(ctx, 873u, l5, l8);
    SyncMemoryBase(ctx);
    resume_point = 11u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    l5 = l7;
  }
  // ---- push the flow string into the first array ----
  if (ResumePhase(ctx) == kResumeRunning) {
    l6 = LoadU32(ctx, kPendingThrowSlot);
    StoreU32(ctx, kPendingThrowSlot, 0u);
    l11 = 1u;
    l5 = static_cast<uint32_t>(l6 == 1u);
    if (l5) { goto release_pair_then_unwind; }
    StoreU64(ctx, p2 + 20u, 0ull);
    StoreU64(ctx, p2 + 28u, 0ull);
    StoreU32(ctx, p2 + 16u, p2 + 20u);
    StoreU64(ctx, p2 + 36u, 0ull);
    l6 = LoadU32(ctx, p0 + 96u);
    l8 = LoadU32(ctx, p0 + 100u);
    l5 = static_cast<uint32_t>(l6 < l8);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (l5) {
      StoreU32(ctx, l6, p2);
      p2 = l6 + 4u;
      StoreU32(ctx, p0 + 96u, p2);
      goto second_entry_stored;
    }
    i0 = l6;
    l13 = p0 + 92u;
    l6 = LoadU32(ctx, l13);
    l12 = static_cast<uint32_t>(i0 - l6);
    l14 = static_cast<uint32_t>(static_cast<int32_t>(l12) >> 2);
    l5 = l14 + 1u;
    l10 = static_cast<uint32_t>(l5 >= 1073741824u);
  }
  i0 = l10;
  i0 |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (i0) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kPendingThrowSlot, 0u);
      l11 = 0u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 12u) {
      HostReleaseSlotPair(ctx, 1053u, l13);
      SyncMemoryBase(ctx);
      resume_point = 12u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kPendingThrowSlot);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      p0 = static_cast<uint32_t>(p0 == 1u);
      if (p0) { goto release_pair_then_unwind; }
      goto trap_landing;
    }
  }
  // ---- grow the first array when the push above found no room ----
  if (ResumePhase(ctx) == kResumeRunning) {
    l8 = static_cast<uint32_t>(l8 - l6);
    l11 = static_cast<uint32_t>(static_cast<int32_t>(l8) >> 1);
    l10 = static_cast<uint32_t>(l11 < l5);
    l8 = l10 ? l5 : l11;
    i2 = static_cast<uint32_t>(static_cast<int32_t>(l8) >> 2);
    l8 = static_cast<uint32_t>(i2 < 536870911u) ? l8 : 1073741823u;
    l5 = static_cast<uint32_t>(!l8);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (l5) {
      l5 = 0u;
      goto publish_second_entry;
    }
    l5 = static_cast<uint32_t>(l8 >= 1073741824u);
  }
  i0 = l5;
  i0 |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (i0) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kPendingThrowSlot, 0u);
      l11 = 0u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 13u) {
      HostReleaseSlotPair(ctx, 1033u, 759343u);
      SyncMemoryBase(ctx);
      resume_point = 13u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kPendingThrowSlot);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0) { goto trap_landing; }
      goto release_pair_then_unwind;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    l11 = 0u;
    StoreU32(ctx, kPendingThrowSlot, 0u);
    l5 = l8 << 2u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 14u) {
    l7 = HostFoldLayoutOffset(ctx, 1032u, l5);
    SyncMemoryBase(ctx);
    resume_point = 14u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    l5 = l7;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    l13 = LoadU32(ctx, kPendingThrowSlot);
    StoreU32(ctx, kPendingThrowSlot, 0u);
    l10 = static_cast<uint32_t>(l13 == 1u);
    if (l10) { goto release_pair_then_unwind; }
  }
  // ---- publish the grown first array ----
  // Merge point publish_second_entry.
  publish_second_entry:;
  if (ResumePhase(ctx) == kResumeRunning) {
    l10 = l14 << 2u;
    l11 = l10 + l5;
    StoreU32(ctx, l11, p2);
    if (static_cast<int32_t>(l12) >= 1) {
      CopyBytes(ctx, l5, l6, l12);
      SyncMemoryBase(ctx);
    }
    l8 = l6 << 2u;
    StoreU32(ctx, p0 + 100u, l8 + l5);
    StoreU32(ctx, p0 + 96u, l11 + 4u);
    StoreU32(ctx, p0 + 92u, l5);
    p2 = static_cast<uint32_t>(!l6);
    if (p2) { goto second_entry_stored; }
    DeallocateRawBlock(ctx, l6);
    SyncMemoryBase(ctx);
  }
  // ---- second temporary string: the formatted flow text ----
  // Merge point second_entry_stored.
  second_entry_stored:;
  if (ResumePhase(ctx) == kResumeRunning) {
    if (static_cast<int32_t>(LoadS8(ctx, p1 + 27u)) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, p1 + 16u));
      SyncMemoryBase(ctx);
    }
    l6 = LoadU32(ctx, p0 + 92u);
    l5 = LoadU32(ctx, p0 + 96u);
    StoreU32(ctx, p1 + 8u, 0u);
    StoreU64(ctx, p1, 0ull);
    p2 = p1 + 38u;
    StoreU32(ctx, p1 + 44u, p2);
    l8 = (p1 + 16u) | 2u;
    StoreU32(ctx, p1 + 40u, l8);
    StoreU32(ctx, kPendingThrowSlot, 0u);
    StoreU32(ctx, p1 + 60u, 48u);
    StoreU8(ctx, p1 + 56u, 48u);
    StoreU32(ctx, p1 + 52u, p2);
    StoreU32(ctx, p1 + 48u, static_cast<uint32_t>(static_cast<int32_t>(l5 - l6) >> 2));
    l5 = p1 + 48u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 15u) {
    l7 = HostFoldLayoutOffset(ctx, 3194u, l5);
    SyncMemoryBase(ctx);
    resume_point = 15u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    l8 = l7;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    l6 = LoadU32(ctx, kPendingThrowSlot);
    StoreU32(ctx, kPendingThrowSlot, 0u);
    if (l6 == 1u) { goto release_buffer_slot_trap; }
    StoreU32(ctx, p1 + 40u, l8);
    StoreU32(ctx, p1 + 44u, p2);
    l6 = static_cast<uint32_t>(p2 - l8);
    l10 = static_cast<uint32_t>(l6 < 11u);
    l5 = p1;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (l10) { goto copy_flow_text; }
    StoreU32(ctx, kPendingThrowSlot, 0u);
    l10 = l6 - 10u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 16u) {
    HostInvokeViiiiiii(ctx, 1276u, p1, 10u, l10, 0u, 0u, 0u, 0u);
    SyncMemoryBase(ctx);
    resume_point = 16u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    l11 = LoadU32(ctx, kPendingThrowSlot);
    StoreU32(ctx, kPendingThrowSlot, 0u);
    if (l11 == 1u) { goto release_buffer_slot_trap; }
    l10 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, p1 + 11u)) > -1);
    if (l10) { goto copy_flow_text; }
    l5 = LoadU32(ctx, p1);
  }
  // ---- copy the formatted text into the string, then store its kind ----
  // Merge point copy_flow_text.
  copy_flow_text:;
  if (ResumePhase(ctx) == kResumeRunning) {
    if (p2 != l8) {
      do {
        StoreU8(ctx, l5, LoadU8(ctx, l8));
        l5 += 1u;
        l8 += 1u;
      } while (l8 != p2);
    }
    StoreU8(ctx, l5, 0u);
    if (static_cast<int32_t>(LoadS8(ctx, p1 + 11u)) <= -1) {
      StoreU32(ctx, p1 + 4u, l6);
      goto set_flow_kind;
    }
    StoreU8(ctx, p1 + 11u, l6);
    // Merge point set_flow_kind.
    set_flow_kind:;
    i0 = LoadU32(ctx, p0 + 92u);
    i1 = LoadU32(ctx, p0 + 204u) - 4u;
    i1 = LoadU32(ctx, i1);
    i1 <<= 2u;
    i0 += i1;
    p2 = LoadU32(ctx, i0);
    StoreU8(ctx, p1 + 27u, 6u);
    StoreU32(ctx, p1 + 16u, LoadU32(ctx, 758812u));
    StoreU16(ctx, p1 + 20u, LoadU16(ctx, 758816u));
    StoreU8(ctx, p1 + 22u, 0u);
    StoreU32(ctx, kPendingThrowSlot, 0u);
    StoreU32(ctx, p1 + 72u, p1 + 16u);
    l5 = p1 + 48u;
    l8 = p1 + 16u;
    l6 = p1 + 72u;
    l10 = p1 + 64u;
    p2 += 16u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 17u) {
    HostInvokeViiiiii(ctx, 1959u, l5, p2, l8, 759411u, l6, l10);
    SyncMemoryBase(ctx);
    resume_point = 17u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // ---- store the flow result, releasing the temporaries ----
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = LoadU32(ctx, kPendingThrowSlot);
    StoreU32(ctx, kPendingThrowSlot, 0u);
    if (p2 != 1u) {
      l6 = LoadU32(ctx, p1 + 48u);
      p2 = l6;
      if (static_cast<int32_t>(LoadS8(ctx, p2 + 39u)) <= -1) {
        DeallocateRawBlock(ctx, LoadU32(ctx, p2 + 28u));
        SyncMemoryBase(ctx);
      }
      StoreU64(ctx, p2 + 28u, LoadU64(ctx, p1));
      StoreU32(ctx, p2 + 36u, LoadU32(ctx, p1 + 8u));
      StoreU8(ctx, p1 + 11u, 0u);
      StoreU8(ctx, p1, 0u);
      if (static_cast<int32_t>(LoadS8(ctx, p1 + 27u)) >= 0) { goto release_temp_strings; }
      DeallocateRawBlock(ctx, LoadU32(ctx, p1 + 16u));
      SyncMemoryBase(ctx);
      if (static_cast<int32_t>(LoadS8(ctx, p1 + 11u)) > -1) { goto release_temp_strings; }
      DeallocateRawBlock(ctx, LoadU32(ctx, p1));
      SyncMemoryBase(ctx);
      // Merge point release_temp_strings.
      release_temp_strings:;
      p2 = LoadU32(ctx, p0 + 204u) - 4u;
      l8 = LoadU32(ctx, p0 + 92u);
      l5 = static_cast<uint32_t>(static_cast<int32_t>(LoadU32(ctx, p0 + 96u) - l8) >> 2);
      StoreU32(ctx, p2, l5);
      goto dispatch_tail;
    }
    goto release_all_then_unwind;
  }
  // Merge point allocator_assert_fail_first.
  allocator_assert_fail_first:;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 18u) {
    RaiseAllocatorAssert(ctx, l12);
    SyncMemoryBase(ctx);
    resume_point = 18u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  // Merge point allocator_assert_fail_second.
  allocator_assert_fail_second:;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 19u) {
    RaiseAllocatorAssert(ctx, l12);
    SyncMemoryBase(ctx);
    resume_point = 19u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  // ---- the item is not text: release the buffers taken so far ----
  // Merge point release_pair_then_unwind.
  release_pair_then_unwind:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p4 = HostRet0(ctx);
    SyncMemoryBase(ctx);
    HostTempRet0(ctx);
    SyncMemoryBase(ctx);
    if (static_cast<int32_t>(LoadS8(ctx, p1 + 27u)) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, p1 + 16u));
      SyncMemoryBase(ctx);
    }
    p0 = static_cast<uint32_t>(!l11);
    if (p0) { goto release_slot_then_trap; }
    DeallocateRawBlock(ctx, p2);
    SyncMemoryBase(ctx);
    goto release_slot_then_trap;
  }
  // ---- kind check: a text item (kind 4) takes the string path ----
  // Merge point dispatch_kind_check.
  dispatch_kind_check:;
  if (ResumePhase(ctx) == kResumeRunning) {
    i2 = static_cast<uint32_t>(static_cast<int32_t>(l5 << 24u) >> 24);
    i2 = static_cast<uint32_t>(static_cast<int32_t>(i2) < 0);
    i0 = i2 ? p2 : l8;
    l6 = static_cast<uint32_t>(i0 != 4u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (l6) { goto check_kind_three; }
    StoreU32(ctx, kPendingThrowSlot, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 20u) {
    l7 = HostInvokeIiiiii(ctx, kItemCallbackId, p3, 0u, 4294967295u, 758819u, 4u);
    SyncMemoryBase(ctx);
    resume_point = 20u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    p2 = l7;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    l6 = LoadU32(ctx, kPendingThrowSlot);
    StoreU32(ctx, kPendingThrowSlot, 0u);
    l5 = static_cast<uint32_t>(l6 != 1u);
  }
  i0 = l5;
  i0 |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (i0) {
    if (ResumePhase(ctx) == kResumeRunning) {
      if (p2) {
        p2 = LoadU32(ctx, p3 + 4u);
        l8 = LoadU8(ctx, p3 + 11u);
        l5 = l8;
        goto check_kind_three;
      }
      p2 = LoadU32(ctx, p0 + 204u);
      p3 = static_cast<uint32_t>(p2 == LoadU32(ctx, p0 + 200u));
    }
    i0 = p3;
    i0 |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
    if (i0) {
      // The 48-byte string holding the fixed literal for this pass.
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 21u) {
        l7 = AllocateStringBuffer(ctx, 48u);
        SyncMemoryBase(ctx);
        resume_point = 21u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p2 = l7;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, p1 + 16u, p2);
        StoreU64(ctx, p1 + 20u, 9223372243013206053ull);
        StoreU8(ctx, p2 + 37u, 0u);
        j1 = LoadU64(ctx, 758853u);
        StoreU64(ctx, p2 + 29u, j1);
        j1 = LoadU64(ctx, 758848u);
        StoreU64(ctx, p2 + 24u, j1);
        j1 = LoadU64(ctx, 758840u);
        StoreU64(ctx, p2 + 16u, j1);
        j1 = LoadU64(ctx, 758832u);
        StoreU64(ctx, p2 + 8u, j1);
        j1 = LoadU64(ctx, 758824u);
        StoreU64(ctx, p2, j1);
        StoreU32(ctx, kPendingThrowSlot, 0u);
        p2 = p1 + 16u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 22u) {
        HostOsSchedule(ctx, kScheduleCallbackId, p0, p2);
        SyncMemoryBase(ctx);
        resume_point = 22u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kPendingThrowSlot);
        StoreU32(ctx, kPendingThrowSlot, 0u);
        if (p0 != 1u) {
          p0 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, p1 + 27u)) > -1);
          if (p0) { goto restore_sp_and_return; }
          p0 = LoadU32(ctx, p1 + 16u);
          DeallocateRawBlock(ctx, p0);
          SyncMemoryBase(ctx);
          goto restore_sp_and_return;
        }
        p4 = HostRet0(ctx);
        SyncMemoryBase(ctx);
        p0 = HostTempRet0(ctx);
        SyncMemoryBase(ctx);
        goto release_string_then_unwind;
      }
    }
    // Check the item word left behind before building the second string.
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = static_cast<uint32_t>(static_cast<int32_t>(LoadU32(ctx, p2 - 4u)) <= -1);
    }
    i0 = p2;
    i0 |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
    if (i0) {
      // The 32-byte string holding the fixed literal for this pass.
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 23u) {
        l7 = AllocateStringBuffer(ctx, 32u);
        SyncMemoryBase(ctx);
        resume_point = 23u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p2 = l7;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, p1 + 16u, p2);
        StoreU64(ctx, p1 + 20u, 9223372174293729311ull);
        StoreU8(ctx, p2 + 31u, 0u);
        j1 = LoadU64(ctx, 758885u);
        StoreU64(ctx, p2 + 23u, j1);
        j1 = LoadU64(ctx, 758878u);
        StoreU64(ctx, p2 + 16u, j1);
        j1 = LoadU64(ctx, 758870u);
        StoreU64(ctx, p2 + 8u, j1);
        j1 = LoadU64(ctx, 758862u);
        StoreU64(ctx, p2, j1);
        StoreU32(ctx, kPendingThrowSlot, 0u);
        p2 = p1 + 16u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 24u) {
        HostOsSchedule(ctx, kScheduleCallbackId, p0, p2);
        SyncMemoryBase(ctx);
        resume_point = 24u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kPendingThrowSlot);
        StoreU32(ctx, kPendingThrowSlot, 0u);
        if (p0 != 1u) {
          p0 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, p1 + 27u)) > -1);
          if (p0) { goto restore_sp_and_return; }
          p0 = LoadU32(ctx, p1 + 16u);
          DeallocateRawBlock(ctx, p0);
          SyncMemoryBase(ctx);
          goto restore_sp_and_return;
        }
        p4 = HostRet0(ctx);
        SyncMemoryBase(ctx);
        p0 = HostTempRet0(ctx);
        SyncMemoryBase(ctx);
        goto release_string_then_unwind;
      }
    }
    // ---- push the flow string into the first array ----
    if (ResumePhase(ctx) == kResumeRunning) {
      p3 = LoadU32(ctx, p0 + 216u);
      l5 = p3 - 8u;
      p2 = LoadU32(ctx, l5);
      l12 = p3 - 4u;
      p4 = LoadU32(ctx, l12);
      l8 = static_cast<uint32_t>(p2 < p4);
      i0 = LoadU32(ctx, p0 + 96u);
      i0 -= LoadU32(ctx, p0 + 92u);
      l6 = static_cast<uint32_t>(static_cast<int32_t>(i0) >> 2);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (l8) {
        StoreU32(ctx, p2, l6);
        p2 += 4u;
        StoreU32(ctx, l5, p2);
        goto second_array_ready;
      }
      i0 = p2;
      l8 = p3 - 12u;
      p2 = LoadU32(ctx, l8);
      l11 = static_cast<uint32_t>(i0 - p2);
      l13 = static_cast<uint32_t>(static_cast<int32_t>(l11) >> 2);
      p3 = l13 + 1u;
      l10 = static_cast<uint32_t>(p3 >= 1073741824u);
      if (l10) { goto allocator_assert_fail_third; }
      p4 = static_cast<uint32_t>(p4 - p2);
      l14 = static_cast<uint32_t>(static_cast<int32_t>(p4) >> 1);
      l10 = static_cast<uint32_t>(l14 < p3);
      p3 = l10 ? p3 : l14;
      l10 = static_cast<uint32_t>(static_cast<int32_t>(p4) >> 2) < 536870911u;
      p3 = l10 ? p3 : 1073741823u;
      l10 = static_cast<uint32_t>(!p3);
      p4 = 0u;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (l10) { goto second_payload_ready; }
      if (p3 >= 1073741824u) { goto alloc_limit_error; }
      p4 = p3 << 2u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 25u) {
      l7 = AllocateStringBuffer(ctx, p4);
      SyncMemoryBase(ctx);
      resume_point = 25u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p4 = l7;
    }
    // Merge point second_payload_ready.
    second_payload_ready:;
    if (ResumePhase(ctx) == kResumeRunning) {
      l10 = l13 << 2u;
      l13 = l10 + p4;
      StoreU32(ctx, l13, l6);
      l6 = static_cast<uint32_t>(static_cast<int32_t>(l11) >= 1);
      if (l6) {
        l6 = CopyBytes(ctx, p4, p2, l11);
        SyncMemoryBase(ctx);
      }
      StoreU32(ctx, l8, p4);
      l8 = l13 + 4u;
      StoreU32(ctx, l5, l8);
      StoreU32(ctx, l12, p4 + (p3 << 2u));
      p3 = static_cast<uint32_t>(!p2);
      if (p3) { goto second_array_ready; }
      DeallocateRawBlock(ctx, p2);
      SyncMemoryBase(ctx);
    }
    // Merge point second_array_ready.
    second_array_ready:;
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 26u) {
      l7 = AllocateStringBuffer(ctx, 44u);
      SyncMemoryBase(ctx);
      resume_point = 26u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p3 = l7;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU8(ctx, p1 + 27u, 5u);
      StoreU32(ctx, p1 + 16u, LoadU32(ctx, 758806u));
      StoreU8(ctx, p1 + 20u, LoadU8(ctx, 758810u));
      StoreU8(ctx, p1 + 21u, 0u);
      StoreU32(ctx, p3, 745280u);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      p4 = p1 + 16u;
      p2 = p3 + 4u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 27u) {
      l7 = HostOsQuery(ctx, 873u, p2, p4);
      SyncMemoryBase(ctx);
      resume_point = 27u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = l7;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p4 = LoadU32(ctx, kPendingThrowSlot);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      p2 = 1u;
      p4 = static_cast<uint32_t>(p4 == 1u);
      if (p4) { goto release_buffers_then_trap; }
      StoreU64(ctx, p3 + 20u, 0ull);
      StoreU64(ctx, p3 + 28u, 0ull);
      StoreU32(ctx, p3 + 16u, p3 + 20u);
      StoreU64(ctx, p3 + 36u, 0ull);
      p2 = LoadU32(ctx, p0 + 96u);
      p4 = LoadU32(ctx, p0 + 100u);
      l5 = static_cast<uint32_t>(p2 < p4);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (l5) {
        StoreU32(ctx, p2, p3);
        p2 += 4u;
        StoreU32(ctx, p0 + 96u, p2);
        goto first_array_ready;
      }
      l5 = p0 + 92u;
      l6 = LoadU32(ctx, l5);
      l8 = static_cast<uint32_t>(p2 - l6);
      l11 = static_cast<uint32_t>(static_cast<int32_t>(l8) >> 2);
      p2 = l11 + 1u;
      l10 = static_cast<uint32_t>(p2 >= 1073741824u);
    }
    i0 = l10;
    i0 |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
    if (i0) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kPendingThrowSlot, 0u);
        p2 = 0u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 28u) {
        HostReleaseSlotPair(ctx, 1053u, l5);
        SyncMemoryBase(ctx);
        resume_point = 28u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kPendingThrowSlot);
        StoreU32(ctx, kPendingThrowSlot, 0u);
        p0 = static_cast<uint32_t>(p0 != 1u);
        if (p0) { goto trap_landing; }
        goto release_buffers_then_trap;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p4 = static_cast<uint32_t>(p4 - l6);
      l5 = static_cast<uint32_t>(static_cast<int32_t>(p4) >> 1);
      l10 = static_cast<uint32_t>(l5 < p2);
      l5 = l10 ? p2 : l5;
      p4 = static_cast<uint32_t>(static_cast<int32_t>(p4) >> 2) < 536870911u;
      l5 = p4 ? l5 : 1073741823u;
      p2 = static_cast<uint32_t>(!l5);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (p2) {
        p4 = 0u;
        goto first_array_payload;
      }
      p2 = static_cast<uint32_t>(l5 >= 1073741824u);
    }
    i0 = p2;
    i0 |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
    if (i0) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kPendingThrowSlot, 0u);
        p2 = 0u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 29u) {
        HostReleaseSlotPair(ctx, 1033u, 759343u);
        SyncMemoryBase(ctx);
        resume_point = 29u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kPendingThrowSlot);
        StoreU32(ctx, kPendingThrowSlot, 0u);
        p0 = static_cast<uint32_t>(p0 != 1u);
        if (p0) { goto trap_landing; }
        goto release_buffers_then_trap;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kPendingThrowSlot, 0u);
      p4 = l5 << 2u;
      p2 = 0u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 30u) {
      l7 = HostFoldLayoutOffset(ctx, 1032u, p4);
      SyncMemoryBase(ctx);
      resume_point = 30u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p4 = l7;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      l12 = LoadU32(ctx, kPendingThrowSlot);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      l10 = static_cast<uint32_t>(l12 == 1u);
      if (l10) { goto release_buffers_then_trap; }
    }
    // Merge point first_array_payload.
    first_array_payload:;
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = p4 + (l11 << 2u);
      StoreU32(ctx, p2, p3);
      if (static_cast<int32_t>(l8) >= 1) {
        CopyBytes(ctx, p4, l6, l8);
        SyncMemoryBase(ctx);
      }
      p3 = p4 + (l5 << 2u);
      StoreU32(ctx, p0 + 100u, p3);
      StoreU32(ctx, p0 + 96u, p2 + 4u);
      StoreU32(ctx, p0 + 92u, p4);
      p2 = static_cast<uint32_t>(!l6);
      if (p2) { goto first_array_ready; }
      DeallocateRawBlock(ctx, l6);
      SyncMemoryBase(ctx);
    }
    // ---- rebuild the second string from the queue and store the result ----
    // Merge point first_array_ready.
    first_array_ready:;
    if (ResumePhase(ctx) == kResumeRunning) {
      if (static_cast<int32_t>(LoadS8(ctx, p1 + 27u)) <= -1) {
        DeallocateRawBlock(ctx, LoadU32(ctx, p1 + 16u));
        SyncMemoryBase(ctx);
      }
      p3 = LoadU32(ctx, p0 + 92u);
      p4 = LoadU32(ctx, p0 + 96u);
      StoreU32(ctx, p1 + 8u, 0u);
      StoreU64(ctx, p1, 0ull);
      p2 = p1 + 38u;
      StoreU32(ctx, p1 + 44u, p2);
      l5 = (p1 + 16u) | 2u;
      StoreU32(ctx, p1 + 40u, l5);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      StoreU32(ctx, p1 + 60u, 48u);
      StoreU8(ctx, p1 + 56u, 48u);
      StoreU32(ctx, p1 + 52u, p2);
      StoreU32(ctx, p1 + 48u, static_cast<uint32_t>(static_cast<int32_t>(p4 - p3) >> 2));
      p3 = p1 + 48u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 31u) {
      l7 = HostFoldLayoutOffset(ctx, 3194u, p3);
      SyncMemoryBase(ctx);
      resume_point = 31u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p3 = l7;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p4 = LoadU32(ctx, kPendingThrowSlot);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      if (p4 == 1u) { goto release_buffer_slot_trap; }
      StoreU32(ctx, p1 + 40u, p3);
      StoreU32(ctx, p1 + 44u, p2);
      l6 = static_cast<uint32_t>(p2 - p3);
      l5 = static_cast<uint32_t>(l6 < 11u);
      p4 = p1;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (l5) { goto text_ready; }
      StoreU32(ctx, kPendingThrowSlot, 0u);
      l5 = l6 - 10u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 32u) {
      HostInvokeViiiiiii(ctx, 1276u, p1, 10u, l5, 0u, 0u, 0u, 0u);
      SyncMemoryBase(ctx);
      resume_point = 32u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      l5 = LoadU32(ctx, kPendingThrowSlot);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      if (l5 == 1u) { goto release_buffer_slot_trap; }
      l5 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, p1 + 11u)) > -1);
      if (l5) { goto text_ready; }
      p4 = LoadU32(ctx, p1);
    }
    // Merge point text_ready.
    text_ready:;
    if (ResumePhase(ctx) == kResumeRunning) {
      if (p2 != p3) {
        do {
          StoreU8(ctx, p4, LoadU8(ctx, p3));
          p4 += 1u;
          p3 += 1u;
        } while (p3 != p2);
      }
      StoreU8(ctx, p4, 0u);
      if (static_cast<int32_t>(LoadS8(ctx, p1 + 11u)) <= -1) {
        StoreU32(ctx, p1 + 4u, l6);
        goto text_stored;
      }
      StoreU8(ctx, p1 + 11u, l6);
    }
    // Merge point text_stored.
    text_stored:;
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, LoadU32(ctx, p0 + 92u) + (LoadU32(ctx, p0 + 204u) - 4u) * 4u);
      StoreU8(ctx, p1 + 27u, 6u);
      StoreU32(ctx, p1 + 16u, LoadU32(ctx, 758812u));
      StoreU16(ctx, p1 + 20u, LoadU16(ctx, 758816u));
      StoreU8(ctx, p1 + 22u, 0u);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      StoreU32(ctx, p1 + 72u, p1 + 16u);
      p3 = p1 + 48u;
      p4 = p1 + 16u;
      l5 = p1 + 72u;
      l8 = p1 + 64u;
      p2 += 16u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 33u) {
      HostInvokeViiiiii(ctx, 1959u, p3, p2, p4, 759411u, l5, l8);
      SyncMemoryBase(ctx);
      resume_point = 33u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    // ---- store the flow result, releasing the temporaries ----
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kPendingThrowSlot);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      if (p2 != 1u) {
        p2 = LoadU32(ctx, p1 + 48u);
        if (static_cast<int32_t>(LoadS8(ctx, p2 + 39u)) <= -1) {
          DeallocateRawBlock(ctx, LoadU32(ctx, p2 + 28u));
          SyncMemoryBase(ctx);
        }
        StoreU64(ctx, p2 + 28u, LoadU64(ctx, p1));
        StoreU32(ctx, p2 + 36u, LoadU32(ctx, p1 + 8u));
        StoreU8(ctx, p1 + 11u, 0u);
        StoreU8(ctx, p1, 0u);
        p2 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, p1 + 27u)) >= 0);
        if (p2) { goto store_flow_end_marker; }
        DeallocateRawBlock(ctx, LoadU32(ctx, p1 + 16u));
        SyncMemoryBase(ctx);
        p2 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, p1 + 11u)) > -1);
        if (p2) { goto store_flow_end_marker; }
        p2 = LoadU32(ctx, p1);
        DeallocateRawBlock(ctx, p2);
        SyncMemoryBase(ctx);
        // Merge point store_flow_end_marker.
        store_flow_end_marker:;
        p0 = LoadU32(ctx, p0 + 204u) - 4u;
        StoreU32(ctx, p0, 4294967295u);
        goto restore_sp_and_return;
      }
      goto release_all_then_unwind;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) { goto find_catch_and_panic; }
  // Merge point allocator_assert_fail_third.
  allocator_assert_fail_third:;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 34u) {
    RaiseAllocatorAssert(ctx, l8);
    SyncMemoryBase(ctx);
    resume_point = 34u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  // Merge point release_buffers_then_trap.
  release_buffers_then_trap:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p4 = HostRet0(ctx);
    SyncMemoryBase(ctx);
    HostTempRet0(ctx);
    SyncMemoryBase(ctx);
    if (static_cast<int32_t>(LoadS8(ctx, p1 + 27u)) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, p1 + 16u));
      SyncMemoryBase(ctx);
    }
    p0 = static_cast<uint32_t>(!p2);
    if (p0) { goto release_slot_then_trap; }
    DeallocateRawBlock(ctx, p3);
    SyncMemoryBase(ctx);
    goto release_slot_then_trap;
  }
  // ---- kind check: a stored-data item (kind 3) builds the second string ----
  // Merge point check_kind_three.
  check_kind_three:;
  if (ResumePhase(ctx) == kResumeRunning) {
    i2 = static_cast<uint32_t>(static_cast<int32_t>(l5 << 24u) >> 24);
    i2 = static_cast<uint32_t>(static_cast<int32_t>(i2) < 0);
    i0 = i2 ? p2 : l8;
    l6 = static_cast<uint32_t>(i0 != 3u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (l6) { goto check_kind_four; }
    StoreU32(ctx, kPendingThrowSlot, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 35u) {
    l7 = HostInvokeIiiiii(ctx, kItemCallbackId, p3, 0u, 4294967295u, 758894u, 3u);
    SyncMemoryBase(ctx);
    resume_point = 35u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    p2 = l7;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    l6 = LoadU32(ctx, kPendingThrowSlot);
    StoreU32(ctx, kPendingThrowSlot, 0u);
    l5 = static_cast<uint32_t>(l6 != 1u);
  }
  i0 = l5;
  i0 |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (i0) {
    if (ResumePhase(ctx) == kResumeRunning) {
      if (p2) {
        p2 = LoadU32(ctx, p3 + 4u);
        l8 = LoadU8(ctx, p3 + 11u);
        l5 = l8;
        goto check_kind_four;
      }
      p2 = LoadU32(ctx, p0 + 204u);
      p3 = static_cast<uint32_t>(p2 == LoadU32(ctx, p0 + 200u));
    }
    i0 = p3;
    i0 |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
    if (i0) {
      // The 48-byte string holding the stored-data literal.
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 36u) {
        l7 = AllocateStringBuffer(ctx, 48u);
        SyncMemoryBase(ctx);
        resume_point = 36u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p2 = l7;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, p1 + 16u, p2);
        StoreU64(ctx, p1 + 20u, 9223372243013206052ull);
        StoreU8(ctx, p2 + 36u, 0u);
        p3 = LoadU32(ctx, 758930u);
        StoreU32(ctx, p2 + 32u, p3);
        j1 = LoadU64(ctx, 758922u);
        StoreU64(ctx, p2 + 24u, j1);
        j1 = LoadU64(ctx, 758914u);
        StoreU64(ctx, p2 + 16u, j1);
        j1 = LoadU64(ctx, 758906u);
        StoreU64(ctx, p2 + 8u, j1);
        j1 = LoadU64(ctx, 758898u);
        StoreU64(ctx, p2, j1);
        StoreU32(ctx, kPendingThrowSlot, 0u);
        p2 = p1 + 16u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 37u) {
        HostOsSchedule(ctx, kScheduleCallbackId, p0, p2);
        SyncMemoryBase(ctx);
        resume_point = 37u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kPendingThrowSlot);
        StoreU32(ctx, kPendingThrowSlot, 0u);
        if (p0 != 1u) {
          p0 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, p1 + 27u)) > -1);
          if (p0) { goto restore_sp_and_return; }
          p0 = LoadU32(ctx, p1 + 16u);
          DeallocateRawBlock(ctx, p0);
          SyncMemoryBase(ctx);
          goto restore_sp_and_return;
        }
        p4 = HostRet0(ctx);
        SyncMemoryBase(ctx);
        p0 = HostTempRet0(ctx);
        SyncMemoryBase(ctx);
        goto release_string_then_unwind;
      }
    }
    // Rebuild the second string only when the queue entry is still held.
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = static_cast<uint32_t>(static_cast<int32_t>(LoadU32(ctx, p2 - 4u)) < 0);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (p2) { goto second_string_done; }
      p3 = LoadU32(ctx, p0 + 96u);
      p4 = LoadU32(ctx, p0 + 92u);
      StoreU32(ctx, p1 + 8u, 0u);
      StoreU64(ctx, p1, 0ull);
      p2 = p1 + 38u;
      StoreU32(ctx, p1 + 44u, p2);
      l5 = (p1 + 16u) | 2u;
      StoreU32(ctx, p1 + 40u, l5);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      StoreU32(ctx, p1 + 60u, 48u);
      StoreU8(ctx, p1 + 56u, 48u);
      StoreU32(ctx, p1 + 52u, p2);
      StoreU32(ctx, p1 + 48u, static_cast<uint32_t>(static_cast<int32_t>(p3 - p4) >> 2));
      p3 = p1 + 48u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 38u) {
      l7 = HostFoldLayoutOffset(ctx, 3194u, p3);
      SyncMemoryBase(ctx);
      resume_point = 38u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p3 = l7;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p4 = LoadU32(ctx, kPendingThrowSlot);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      if (p4 == 1u) { goto release_buffer_slot_trap; }
      StoreU32(ctx, p1 + 40u, p3);
      StoreU32(ctx, p1 + 44u, p2);
      l6 = static_cast<uint32_t>(p2 - p3);
      l5 = static_cast<uint32_t>(l6 < 11u);
      p4 = p1;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (l5) { goto second_text_ready; }
      StoreU32(ctx, kPendingThrowSlot, 0u);
      l5 = l6 - 10u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 39u) {
      HostInvokeViiiiiii(ctx, 1276u, p1, 10u, l5, 0u, 0u, 0u, 0u);
      SyncMemoryBase(ctx);
      resume_point = 39u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      l5 = LoadU32(ctx, kPendingThrowSlot);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      if (l5 == 1u) { goto release_buffer_slot_trap; }
      l5 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, p1 + 11u)) > -1);
      if (l5) { goto second_text_ready; }
      p4 = LoadU32(ctx, p1);
    }
    // Merge point second_text_ready.
    second_text_ready:;
    if (ResumePhase(ctx) == kResumeRunning) {
      if (p2 != p3) {
        do {
          StoreU8(ctx, p4, LoadU8(ctx, p3));
          p4 += 1u;
          p3 += 1u;
        } while (p3 != p2);
      }
      StoreU8(ctx, p4, 0u);
      if (static_cast<int32_t>(LoadS8(ctx, p1 + 11u)) <= -1) {
        StoreU32(ctx, p1 + 4u, l6);
        goto second_text_stored;
      }
      StoreU8(ctx, p1 + 11u, l6);
    }
    // Merge point second_text_stored.
    second_text_stored:;
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, LoadU32(ctx, p0 + 92u) + (LoadU32(ctx, p0 + 204u) - 4u) * 4u);
      StoreU8(ctx, p1 + 27u, 6u);
      StoreU32(ctx, p1 + 16u, LoadU32(ctx, 758812u));
      StoreU16(ctx, p1 + 20u, LoadU16(ctx, 758816u));
      StoreU8(ctx, p1 + 22u, 0u);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      StoreU32(ctx, p1 + 72u, p1 + 16u);
      p3 = p1 + 48u;
      p4 = p1 + 16u;
      l5 = p1 + 72u;
      l8 = p1 + 64u;
      p2 += 16u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 40u) {
      HostInvokeViiiiii(ctx, 1959u, p3, p2, p4, 759411u, l5, l8);
      SyncMemoryBase(ctx);
      resume_point = 40u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kPendingThrowSlot);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      p2 = static_cast<uint32_t>(p2 == 1u);
      if (p2) { goto abort_flow_unwind; }
      p2 = LoadU32(ctx, p1 + 48u);
      if (static_cast<int32_t>(LoadS8(ctx, p2 + 39u)) <= -1) {
        DeallocateRawBlock(ctx, LoadU32(ctx, p2 + 28u));
        SyncMemoryBase(ctx);
      }
      StoreU64(ctx, p2 + 28u, LoadU64(ctx, p1));
      p3 = LoadU32(ctx, p1 + 8u);
      StoreU32(ctx, p2 + 36u, p3);
      StoreU8(ctx, p1 + 11u, 0u);
      StoreU8(ctx, p1, 0u);
      p2 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, p1 + 27u)) >= 0);
      if (p2) { goto second_string_done; }
      DeallocateRawBlock(ctx, LoadU32(ctx, p1 + 16u));
      SyncMemoryBase(ctx);
      p2 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, p1 + 11u)) > -1);
      if (p2) { goto second_string_done; }
      p2 = LoadU32(ctx, p1);
      DeallocateRawBlock(ctx, p2);
      SyncMemoryBase(ctx);
    }
    // Merge point second_string_done.
    second_string_done:;
    if (ResumePhase(ctx) == kResumeRunning) {
      p3 = LoadU32(ctx, p0 + 216u);
      p2 = p3 - 12u;
      l5 = LoadU32(ctx, p2);
      p2 = static_cast<uint32_t>(l5 == LoadU32(ctx, p2 + 4u));
      if (p2) { goto release_entry_arrays_then_return; }
      p3 = p1 + 16u;
      l8 = p3 | 2u;
      p2 = p1 + 38u;
    }
    // ---- walk the remaining flow entries until the arrays run out ----
    // Merge point next_entry_loop.
    next_entry_loop:;
    if (ResumePhase(ctx) == kResumeRunning) {
      p3 = LoadU32(ctx, p0 + 92u);
      p4 = LoadU32(ctx, p0 + 96u);
      StoreU32(ctx, p1 + 8u, 0u);
      StoreU64(ctx, p1, 0ull);
      StoreU32(ctx, p1 + 44u, p2);
      StoreU32(ctx, p1 + 40u, l8);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      StoreU32(ctx, p1 + 60u, 48u);
      StoreU8(ctx, p1 + 56u, 48u);
      StoreU32(ctx, p1 + 52u, p2);
      StoreU32(ctx, p1 + 48u, static_cast<uint32_t>(static_cast<int32_t>(p4 - p3) >> 2));
      p3 = p1 + 48u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 41u) {
      l7 = HostFoldLayoutOffset(ctx, 3194u, p3);
      SyncMemoryBase(ctx);
      resume_point = 41u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p3 = l7;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p4 = LoadU32(ctx, kPendingThrowSlot);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      if (p4 == 1u) { goto release_buffer_slot_trap; }
      StoreU32(ctx, p1 + 40u, p3);
      StoreU32(ctx, p1 + 44u, p2);
      l6 = static_cast<uint32_t>(p2 - p3);
      l10 = static_cast<uint32_t>(l6 >= 11u);
      p4 = p1;
    }
    i0 = l10;
    i0 |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
    if (i0) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kPendingThrowSlot, 0u);
        p4 = l6 - 10u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 42u) {
        HostInvokeViiiiiii(ctx, 1276u, p1, 10u, p4, 0u, 0u, 0u, 0u);
        SyncMemoryBase(ctx);
        resume_point = 42u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p4 = LoadU32(ctx, kPendingThrowSlot);
        StoreU32(ctx, kPendingThrowSlot, 0u);
        if (p4 == 1u) { goto release_buffer_slot_trap; }
        l10 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, p1 + 11u)) < 0);
        p4 = l10 ? LoadU32(ctx, p1) : p1;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (p2 != p3) {
        do {
          StoreU8(ctx, p4, LoadU8(ctx, p3));
          p4 += 1u;
          p3 += 1u;
        } while (p3 != p2);
      }
      StoreU8(ctx, p4, 0u);
      if (static_cast<int32_t>(LoadS8(ctx, p1 + 11u)) <= -1) {
        StoreU32(ctx, p1 + 4u, l6);
        goto second_entry_text_stored;
      }
      StoreU8(ctx, p1 + 11u, l6);
      // Merge point second_entry_text_stored.
      second_entry_text_stored:;
      p3 = LoadU32(ctx, LoadU32(ctx, p0 + 92u) + (LoadU32(ctx, l5) << 2u));
      StoreU8(ctx, p1 + 27u, 6u);
      StoreU32(ctx, p1 + 16u, LoadU32(ctx, 758812u));
      StoreU16(ctx, p1 + 20u, LoadU16(ctx, 758816u));
      StoreU8(ctx, p1 + 22u, 0u);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      StoreU32(ctx, p1 + 72u, p1 + 16u);
      p4 = p1 + 48u;
      l6 = p1 + 16u;
      l10 = p1 + 72u;
      l11 = p1 + 64u;
      p3 += 16u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 43u) {
      HostInvokeViiiiii(ctx, 1959u, p4, p3, l6, 759411u, l10, l11);
      SyncMemoryBase(ctx);
      resume_point = 43u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p3 = LoadU32(ctx, kPendingThrowSlot);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      p3 = static_cast<uint32_t>(p3 != 1u);
      if (p3) {
        p3 = LoadU32(ctx, p1 + 48u);
        if (static_cast<int32_t>(LoadS8(ctx, p3 + 39u)) <= -1) {
          DeallocateRawBlock(ctx, LoadU32(ctx, p3 + 28u));
          SyncMemoryBase(ctx);
        }
        StoreU64(ctx, p3 + 28u, LoadU64(ctx, p1));
        StoreU32(ctx, p3 + 36u, LoadU32(ctx, p1 + 8u));
        StoreU8(ctx, p1 + 11u, 0u);
        StoreU8(ctx, p1, 0u);
        if (static_cast<int32_t>(LoadS8(ctx, p1 + 27u)) >= 0) {
          goto second_entry_result_stored;
        }
        DeallocateRawBlock(ctx, LoadU32(ctx, p1 + 16u));
        SyncMemoryBase(ctx);
        if (static_cast<int32_t>(LoadS8(ctx, p1 + 11u)) > -1) {
          goto second_entry_result_stored;
        }
        DeallocateRawBlock(ctx, LoadU32(ctx, p1));
        SyncMemoryBase(ctx);
        // Merge point second_entry_result_stored.
        second_entry_result_stored:;
        l5 += 4u;
        p3 = LoadU32(ctx, p0 + 216u);
        p4 = static_cast<uint32_t>(l5 != LoadU32(ctx, p3 - 8u));
        if (p4) { goto next_entry_loop; }
        goto release_entry_arrays_then_return;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto release_all_then_unwind; }
  }
  if (ResumePhase(ctx) == kResumeRunning) { goto find_catch_and_panic; }
  // ---- drop the current entry and release its arrays ----
  // Merge point release_entry_arrays_then_return.
  release_entry_arrays_then_return:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, p0 + 204u, LoadU32(ctx, p0 + 204u) - 4u);
    p4 = p3 - 12u;
    p2 = LoadU32(ctx, p4);
    if (p2) {
      StoreU32(ctx, p3 - 8u, p2);
      DeallocateRawBlock(ctx, p2);
      SyncMemoryBase(ctx);
    }
    StoreU32(ctx, p0 + 216u, p4);
    goto restore_sp_and_return;
  }
  // Merge point abort_flow_unwind.
  abort_flow_unwind:;
  if (ResumePhase(ctx) == kResumeRunning) { goto release_all_then_unwind; }
  // ---- kind check: kind 4 keeps the value in the first array ----
  // Merge point check_kind_four.
  check_kind_four:;
  if (ResumePhase(ctx) == kResumeRunning) {
    i2 = static_cast<uint32_t>(static_cast<int32_t>(l5 << 24u) >> 24);
    i2 = static_cast<uint32_t>(static_cast<int32_t>(i2) < 0);
    i0 = i2 ? p2 : l8;
    l6 = static_cast<uint32_t>(i0 != 4u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (l6) { goto check_kind_five; }
    StoreU32(ctx, kPendingThrowSlot, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 44u) {
    l7 = HostInvokeIiiiii(ctx, kItemCallbackId, p3, 0u, 4294967295u, 758935u, 4u);
    SyncMemoryBase(ctx);
    resume_point = 44u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    p2 = l7;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    l6 = LoadU32(ctx, kPendingThrowSlot);
    StoreU32(ctx, kPendingThrowSlot, 0u);
    l5 = static_cast<uint32_t>(l6 != 1u);
  }
  i0 = l5;
  i0 |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (i0) {
    if (ResumePhase(ctx) == kResumeRunning) {
      if (p2) {
        p2 = LoadU32(ctx, p3 + 4u);
        l8 = LoadU8(ctx, p3 + 11u);
        l5 = l8;
        goto check_kind_five;
      }
      l8 = static_cast<uint32_t>(static_cast<int32_t>(LoadU32(ctx, p0 + 96u) -
                                                      LoadU32(ctx, p0 + 92u)) >> 2);
      p2 = LoadU32(ctx, p0 + 228u);
      l5 = LoadU32(ctx, p0 + 232u);
      l6 = static_cast<uint32_t>(p2 < l5);
      if (l6) {
        StoreU32(ctx, p2, l8);
        p2 += 4u;
        StoreU32(ctx, p0 + 228u, p2);
        goto dispatch_tail;
      }
      i0 = p2;
      l12 = p0 + 224u;
      p2 = LoadU32(ctx, l12);
      l11 = static_cast<uint32_t>(i0 - p2);
      l13 = static_cast<uint32_t>(static_cast<int32_t>(l11) >> 2);
      l6 = static_cast<uint32_t>(l13 + 1u);
      l10 = static_cast<uint32_t>(l6 >= 1073741824u);
      if (l10) { goto allocator_assert_fail_fourth; }
      l5 = static_cast<uint32_t>(l5 - p2);
      l12 = static_cast<uint32_t>(static_cast<int32_t>(l5) >> 1);
      l10 = static_cast<uint32_t>(l12 < l6);
      l6 = static_cast<uint32_t>(l10 ? l6 : l12);
      i1 = 1073741823u;
      i2 = static_cast<uint32_t>(static_cast<int32_t>(l5) >> 2);
      i2 = static_cast<uint32_t>(i2 < 536870911u);
      l6 = static_cast<uint32_t>(i2 ? l6 : i1);
      l10 = static_cast<uint32_t>(l6 == 0u);
      l5 = 0u;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (l10) { goto array_c_slot_ready; }
      l5 = static_cast<uint32_t>(l6 >= 1073741824u);
      if (l5) { goto alloc_limit_error; }
      l5 = l6 << 2u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 45u) {
      l7 = AllocateStringBuffer(ctx, l5);
      SyncMemoryBase(ctx);
      resume_point = 45u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      l5 = l7;
    }
    // Merge point array_c_slot_ready.
    array_c_slot_ready:;
    if (ResumePhase(ctx) == kResumeRunning) {
      l10 = l13 << 2u;
      l12 = l10 + l5;
      StoreU32(ctx, l12, l8);
      if (static_cast<int32_t>(l11) >= 1) {
        CopyBytes(ctx, l5, p2, l11);
        SyncMemoryBase(ctx);
      }
      StoreU32(ctx, p0 + 232u, l5 + (l6 << 2u));
      l8 = l12 + 4u;
      StoreU32(ctx, p0 + 228u, l8);
      StoreU32(ctx, p0 + 224u, l5);
      l5 = static_cast<uint32_t>(!p2);
      if (l5) { goto dispatch_tail; }
      DeallocateRawBlock(ctx, p2);
      SyncMemoryBase(ctx);
      goto dispatch_tail;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) { goto find_catch_and_panic; }
  // Merge point allocator_assert_fail_fourth.
  allocator_assert_fail_fourth:;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 46u) {
    RaiseAllocatorAssert(ctx, l12);
    SyncMemoryBase(ctx);
    resume_point = 46u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  if (ResumePhase(ctx) == kResumeRunning) { goto alloc_limit_error; }
  // ---- kind check: kind 5 stores a queued request object ----
  // Merge point check_kind_five.
  check_kind_five:;
  if (ResumePhase(ctx) == kResumeRunning) {
    i2 = static_cast<uint32_t>(static_cast<int32_t>(l5 << 24u) >> 24);
    i2 = static_cast<uint32_t>(static_cast<int32_t>(i2) < 0);
    i0 = i2 ? p2 : l8;
    l6 = static_cast<uint32_t>(i0 != 5u);
    if (l6) { goto final_kind_check; }
    StoreU32(ctx, kPendingThrowSlot, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 47u) {
    l7 = HostInvokeIiiiii(ctx, kItemCallbackId, p3, 0u, 4294967295u, 758940u, 5u);
    SyncMemoryBase(ctx);
    resume_point = 47u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    p2 = l7;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    l6 = LoadU32(ctx, kPendingThrowSlot);
    StoreU32(ctx, kPendingThrowSlot, 0u);
    l5 = static_cast<uint32_t>(l6 != 1u);
  }
  i0 = l5;
  i0 |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (i0) {
    if (ResumePhase(ctx) == kResumeRunning) {
      if (p2) {
        p2 = LoadU32(ctx, p3 + 4u);
        l8 = LoadU8(ctx, p3 + 11u);
        l5 = l8;
        goto final_kind_check;
      }
      p3 = LoadU32(ctx, p0 + 228u);
      p2 = static_cast<uint32_t>(p3 == LoadU32(ctx, p0 + 224u));
    }
    i0 = p2;
    i0 |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
    if (i0) {
      // The 48-byte string holding the request literal.
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 48u) {
        l7 = AllocateStringBuffer(ctx, 48u);
        SyncMemoryBase(ctx);
        resume_point = 48u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p2 = l7;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, p1 + 16u, p2);
        StoreU64(ctx, p1 + 20u, 9223372243013206056ull);
        StoreU8(ctx, p2 + 40u, 0u);
        j1 = LoadU64(ctx, 758978u);
        StoreU64(ctx, p2 + 32u, j1);
        j1 = LoadU64(ctx, 758970u);
        StoreU64(ctx, p2 + 24u, j1);
        j1 = LoadU64(ctx, 758962u);
        StoreU64(ctx, p2 + 16u, j1);
        j1 = LoadU64(ctx, 758954u);
        StoreU64(ctx, p2 + 8u, j1);
        j1 = LoadU64(ctx, 758946u);
        StoreU64(ctx, p2, j1);
        StoreU32(ctx, kPendingThrowSlot, 0u);
        p2 = p1 + 16u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 49u) {
        HostOsSchedule(ctx, kScheduleCallbackId, p0, p2);
        SyncMemoryBase(ctx);
        resume_point = 49u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kPendingThrowSlot);
        StoreU32(ctx, kPendingThrowSlot, 0u);
        if (p0 != 1u) {
          p0 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, p1 + 27u)) > -1);
          if (p0) { goto restore_sp_and_return; }
          p0 = LoadU32(ctx, p1 + 16u);
          DeallocateRawBlock(ctx, p0);
          SyncMemoryBase(ctx);
          goto restore_sp_and_return;
        }
        p4 = HostRet0(ctx);
        SyncMemoryBase(ctx);
        p0 = HostTempRet0(ctx);
        SyncMemoryBase(ctx);
        goto release_string_then_unwind;
      }
    }
    // Build the queued request object and hand it to the host.
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 50u) {
      l7 = AllocateStringBuffer(ctx, 44u);
      SyncMemoryBase(ctx);
      resume_point = 50u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p3 = l7;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU8(ctx, p1 + 27u, 5u);
      StoreU32(ctx, p1 + 16u, LoadU32(ctx, 758806u));
      StoreU8(ctx, p1 + 20u, LoadU8(ctx, 758810u));
      StoreU8(ctx, p1 + 21u, 0u);
      StoreU32(ctx, p3, 745280u);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      p4 = p1 + 16u;
      p2 = p3 + 4u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 51u) {
      l7 = HostOsQuery(ctx, 873u, p2, p4);
      SyncMemoryBase(ctx);
      resume_point = 51u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = l7;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p4 = LoadU32(ctx, kPendingThrowSlot);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      p2 = 1u;
      p4 = static_cast<uint32_t>(p4 == 1u);
      if (p4) { goto release_item_buffer_then_trap; }
      StoreU64(ctx, p3 + 20u, 0ull);
      StoreU64(ctx, p3 + 28u, 0ull);
      StoreU32(ctx, p3 + 16u, p3 + 20u);
      StoreU64(ctx, p3 + 36u, 0ull);
      p2 = LoadU32(ctx, p0 + 96u);
      p4 = LoadU32(ctx, p0 + 100u);
      l5 = static_cast<uint32_t>(p2 < p4);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (l5) {
        StoreU32(ctx, p2, p3);
        p2 += 4u;
        StoreU32(ctx, p0 + 96u, p2);
        goto array_a_slot_pushed;
      }
      l5 = p0 + 92u;
      l6 = LoadU32(ctx, l5);
      l8 = static_cast<uint32_t>(p2 - l6);
      l11 = static_cast<uint32_t>(static_cast<int32_t>(l8) >> 2);
      p2 = l11 + 1u;
      l10 = static_cast<uint32_t>(p2 >= 1073741824u);
    }
    i0 = l10;
    i0 |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
    if (i0) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kPendingThrowSlot, 0u);
        p2 = 0u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 52u) {
        HostReleaseSlotPair(ctx, 1053u, l5);
        SyncMemoryBase(ctx);
        resume_point = 52u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kPendingThrowSlot);
        StoreU32(ctx, kPendingThrowSlot, 0u);
        p0 = static_cast<uint32_t>(p0 != 1u);
        if (p0) { goto trap_landing; }
        goto release_item_buffer_then_trap;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p4 = static_cast<uint32_t>(p4 - l6);
      l5 = static_cast<uint32_t>(static_cast<int32_t>(p4) >> 1);
      l10 = static_cast<uint32_t>(l5 < p2);
      i0 = static_cast<uint32_t>(l10 ? p2 : l5);
      i2 = static_cast<uint32_t>(static_cast<int32_t>(p4) >> 2);
      p4 = static_cast<uint32_t>(i2 < 536870911u);
      l5 = static_cast<uint32_t>(p4 ? i0 : 1073741823u);
      p2 = static_cast<uint32_t>(l5 == 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (p2) {
        p4 = 0u;
        goto array_a_slot_ready;
      }
      p2 = static_cast<uint32_t>(l5 >= 1073741824u);
    }
    i0 = p2;
    i0 |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
    if (i0) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kPendingThrowSlot, 0u);
        p2 = 0u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 53u) {
        HostReleaseSlotPair(ctx, 1033u, 759343u);
        SyncMemoryBase(ctx);
        resume_point = 53u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kPendingThrowSlot);
        StoreU32(ctx, kPendingThrowSlot, 0u);
        p0 = static_cast<uint32_t>(p0 != 1u);
        if (p0) { goto trap_landing; }
        goto release_item_buffer_then_trap;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kPendingThrowSlot, 0u);
      p4 = l5 << 2u;
      p2 = 0u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 54u) {
      l7 = HostFoldLayoutOffset(ctx, 1032u, p4);
      SyncMemoryBase(ctx);
      resume_point = 54u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p4 = l7;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      l12 = LoadU32(ctx, kPendingThrowSlot);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      l10 = static_cast<uint32_t>(l12 == 1u);
      if (l10) { goto release_item_buffer_then_trap; }
    }
    // Merge point array_a_slot_ready.
    array_a_slot_ready:;
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = p4 + (l11 << 2u);
      StoreU32(ctx, p2, p3);
      if (static_cast<int32_t>(l8) >= 1) {
        CopyBytes(ctx, p4, l6, l8);
        SyncMemoryBase(ctx);
      }
      StoreU32(ctx, p0 + 100u, p4 + (l5 << 2u));
      StoreU32(ctx, p0 + 96u, p2 + 4u);
      StoreU32(ctx, p0 + 92u, p4);
      p2 = static_cast<uint32_t>(!l6);
      if (p2) { goto array_a_slot_pushed; }
      DeallocateRawBlock(ctx, l6);
      SyncMemoryBase(ctx);
    }
    // Merge point array_a_slot_pushed.
    array_a_slot_pushed:;
    if (ResumePhase(ctx) == kResumeRunning) {
      if (static_cast<int32_t>(LoadS8(ctx, p1 + 27u)) <= -1) {
        DeallocateRawBlock(ctx, LoadU32(ctx, p1 + 16u));
        SyncMemoryBase(ctx);
      }
      p3 = LoadU32(ctx, p0 + 228u) - 4u;
      p2 = p1 + 16u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 55u) {
      IntegerToString(ctx, p2, p3);
      SyncMemoryBase(ctx);
      resume_point = 55u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, LoadU32(ctx, p0 + 96u) - 4u);
      StoreU8(ctx, p1 + 59u, 6u);
      StoreU32(ctx, p1 + 48u, LoadU32(ctx, 758812u));
      StoreU16(ctx, p1 + 52u, LoadU16(ctx, 758816u));
      StoreU8(ctx, p1 + 54u, 0u);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      StoreU32(ctx, p1 + 72u, p1 + 48u);
      p3 = p1 + 48u;
      p4 = p1 + 72u;
      l5 = p1 + 64u;
      p2 += 16u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 56u) {
      HostInvokeViiiiii(ctx, 1959u, p1, p2, p3, 759411u, p4, l5);
      SyncMemoryBase(ctx);
      resume_point = 56u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kPendingThrowSlot);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      p2 = static_cast<uint32_t>(p2 != 1u);
    }
    i0 = p2;
    i0 |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
    if (i0) {
      if (ResumePhase(ctx) == kResumeRunning) {
        p2 = LoadU32(ctx, p1);
        if (static_cast<int32_t>(LoadS8(ctx, p2 + 39u)) <= -1) {
          DeallocateRawBlock(ctx, LoadU32(ctx, p2 + 28u));
          SyncMemoryBase(ctx);
        }
        StoreU64(ctx, p2 + 28u, LoadU64(ctx, p1 + 16u));
        StoreU32(ctx, p2 + 36u, LoadU32(ctx, p1 + 24u));
        StoreU8(ctx, p1 + 27u, 0u);
        StoreU8(ctx, p1 + 16u, 0u);
        if (static_cast<int32_t>(LoadS8(ctx, p1 + 59u)) >= 0) { goto item_value_ready; }
        DeallocateRawBlock(ctx, LoadU32(ctx, p1 + 48u));
        SyncMemoryBase(ctx);
        if (static_cast<int32_t>(LoadS8(ctx, p1 + 27u)) > -1) { goto item_value_ready; }
        DeallocateRawBlock(ctx, LoadU32(ctx, p1 + 16u));
        SyncMemoryBase(ctx);
        // Merge point item_value_ready.
        item_value_ready:;
        p3 = LoadU32(ctx, p0 + 92u);
        p4 = LoadU32(ctx, p0 + 96u);
        StoreU32(ctx, p1 + 8u, 0u);
        StoreU64(ctx, p1, 0ull);
        p2 = p1 + 38u;
        StoreU32(ctx, p1 + 44u, p2);
        l5 = (p1 + 16u) | 2u;
        StoreU32(ctx, p1 + 40u, l5);
        StoreU32(ctx, kPendingThrowSlot, 0u);
        StoreU32(ctx, p1 + 60u, 48u);
        StoreU8(ctx, p1 + 56u, 48u);
        StoreU32(ctx, p1 + 52u, p2);
        StoreU32(ctx, p1 + 48u, static_cast<uint32_t>(static_cast<int32_t>(p4 - p3) >> 2));
        p3 = p1 + 48u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 57u) {
        l7 = HostFoldLayoutOffset(ctx, 3194u, p3);
        SyncMemoryBase(ctx);
        resume_point = 57u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p3 = l7;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p4 = LoadU32(ctx, kPendingThrowSlot);
        StoreU32(ctx, kPendingThrowSlot, 0u);
        if (p4 == 1u) { goto release_buffer_slot_trap; }
        StoreU32(ctx, p1 + 40u, p3);
        StoreU32(ctx, p1 + 44u, p2);
        l6 = static_cast<uint32_t>(p2 - p3);
        l5 = static_cast<uint32_t>(l6 < 11u);
        p4 = p1;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        if (l5) { goto item_text_ready; }
        StoreU32(ctx, kPendingThrowSlot, 0u);
        l5 = l6 - 10u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 58u) {
        HostInvokeViiiiiii(ctx, 1276u, p1, 10u, l5, 0u, 0u, 0u, 0u);
        SyncMemoryBase(ctx);
        resume_point = 58u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        l5 = LoadU32(ctx, kPendingThrowSlot);
        StoreU32(ctx, kPendingThrowSlot, 0u);
        if (l5 == 1u) { goto release_buffer_slot_trap; }
        l5 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, p1 + 11u)) > -1);
        if (l5) { goto item_text_ready; }
        p4 = LoadU32(ctx, p1);
      }
      // Merge point item_text_ready.
      item_text_ready:;
      if (ResumePhase(ctx) == kResumeRunning) {
        if (p2 != p3) {
          do {
            StoreU8(ctx, p4, LoadU8(ctx, p3));
            p4 += 1u;
            p3 += 1u;
          } while (p3 != p2);
        }
        StoreU8(ctx, p4, 0u);
        if (static_cast<int32_t>(LoadS8(ctx, p1 + 11u)) <= -1) {
          StoreU32(ctx, p1 + 4u, l6);
          goto item_text_stored;
        }
        StoreU8(ctx, p1 + 11u, l6);
        // Merge point item_text_stored.
        item_text_stored:;
        p2 = LoadU32(ctx, LoadU32(ctx, p0 + 92u) + (LoadU32(ctx, p0 + 228u) - 4u) * 4u);
        StoreU8(ctx, p1 + 27u, 6u);
        StoreU32(ctx, p1 + 16u, LoadU32(ctx, 758812u));
        StoreU16(ctx, p1 + 20u, LoadU16(ctx, 758816u));
        StoreU8(ctx, p1 + 22u, 0u);
        StoreU32(ctx, kPendingThrowSlot, 0u);
        StoreU32(ctx, p1 + 72u, p1 + 16u);
        p3 = p1 + 48u;
        p4 = p1 + 16u;
        l5 = p1 + 72u;
        l8 = p1 + 64u;
        p2 += 16u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 59u) {
        HostInvokeViiiiii(ctx, 1959u, p3, p2, p4, 759411u, l5, l8);
        SyncMemoryBase(ctx);
        resume_point = 59u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p2 = LoadU32(ctx, kPendingThrowSlot);
        StoreU32(ctx, kPendingThrowSlot, 0u);
        p2 = static_cast<uint32_t>(p2 == 1u);
        if (p2) { goto host_failure_unwind; }
        p2 = LoadU32(ctx, p1 + 48u);
        if (static_cast<int32_t>(LoadS8(ctx, p2 + 39u)) <= -1) {
          DeallocateRawBlock(ctx, LoadU32(ctx, p2 + 28u));
          SyncMemoryBase(ctx);
        }
        StoreU64(ctx, p2 + 28u, LoadU64(ctx, p1));
        StoreU32(ctx, p2 + 36u, LoadU32(ctx, p1 + 8u));
        StoreU8(ctx, p1 + 11u, 0u);
        StoreU8(ctx, p1, 0u);
        if (static_cast<int32_t>(LoadS8(ctx, p1 + 27u)) >= 0) { goto item_published; }
        DeallocateRawBlock(ctx, LoadU32(ctx, p1 + 16u));
        SyncMemoryBase(ctx);
        if (static_cast<int32_t>(LoadS8(ctx, p1 + 11u)) > -1) { goto item_published; }
        DeallocateRawBlock(ctx, LoadU32(ctx, p1));
        SyncMemoryBase(ctx);
        // Merge point item_published.
        item_published:;
        StoreU32(ctx, p0 + 228u, LoadU32(ctx, p0 + 228u) - 4u);
        goto restore_sp_and_return;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p4 = HostRet0(ctx);
      SyncMemoryBase(ctx);
      HostTempRet0(ctx);
      SyncMemoryBase(ctx);
      p0 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, p1 + 59u)) <= -1);
      if (p0) {
        p0 = LoadU32(ctx, p1 + 48u);
        DeallocateRawBlock(ctx, p0);
        SyncMemoryBase(ctx);
      }
      goto release_string_then_unwind;
    }
    // Merge point host_failure_unwind.
    host_failure_unwind:;
    if (ResumePhase(ctx) == kResumeRunning) { goto release_all_then_unwind; }
  }
  if (ResumePhase(ctx) == kResumeRunning) { goto find_catch_and_panic; }
  // ---- release the request buffer and trap ----
  // Merge point release_item_buffer_then_trap.
  release_item_buffer_then_trap:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p4 = HostRet0(ctx);
    SyncMemoryBase(ctx);
    HostTempRet0(ctx);
    SyncMemoryBase(ctx);
    if (static_cast<int32_t>(LoadS8(ctx, p1 + 27u)) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, p1 + 16u));
      SyncMemoryBase(ctx);
    }
    p0 = static_cast<uint32_t>(!p2);
    if (p0) { goto release_slot_then_trap; }
    DeallocateRawBlock(ctx, p3);
    SyncMemoryBase(ctx);
    goto release_slot_then_trap;
  }
  // Merge point trap_landing.
  trap_landing:;
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  // ---- kind check: kind 3 continues into the entry dispatch ----
  // Merge point final_kind_check.
  final_kind_check:;
  if (ResumePhase(ctx) == kResumeRunning) {
    l5 = static_cast<uint32_t>(static_cast<int32_t>(l5 << 24u) >> 24 < 0);
    p2 = static_cast<uint32_t>((l5 ? p2 : l8) != 3u);
    if (p2) { goto dispatch_tail; }
    StoreU32(ctx, kPendingThrowSlot, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 60u) {
    l7 = HostInvokeIiiiii(ctx, kItemCallbackId, p3, 0u, 4294967295u, 758987u, 3u);
    SyncMemoryBase(ctx);
    resume_point = 60u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    p2 = l7;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    l6 = LoadU32(ctx, kPendingThrowSlot);
    StoreU32(ctx, kPendingThrowSlot, 0u);
    l5 = static_cast<uint32_t>(l6 != 1u);
    if (l5) {
      if (p2) { goto dispatch_tail; }
      l6 = LoadU32(ctx, p0 + 8u);
      l11 = LoadU8(ctx, l6);
      p2 = static_cast<uint32_t>(l11 == 0u);
      if (p2) { goto dispatch_tail; }
      p2 = LoadU32(ctx, p0 + 24u);
      if (p2 == 0u) {
        p2 = l6;
        // Scan the pending text word by word until the terminator.
        do {
          if ((static_cast<uint32_t>(l11 ^ 32u) - 161u) <= 59u) {
            i1 = (l11 == 10u) ? (LoadU32(ctx, p0 + 16u) + 1u) : 0u;
            StoreU32(ctx, p0 + 20u, i1);
            l8 = p2 + 1u;
            StoreU32(ctx, p0 + 8u, l8);
            goto scan_step_done;
          }
          if (CompareBytes(ctx, p2, 758991u, 6u) == 0u) { goto scan_nonprintable_tail; }
          i1 = (l11 == 10u) ? (LoadU32(ctx, p0 + 16u) + 1u) : 0u;
          StoreU32(ctx, p0 + 20u, i1);
          l8 = p2;
          // Merge point scan_step_done.
          scan_step_done:;
          p2 = l8 + 1u;
          StoreU32(ctx, p0 + 8u, p2);
          l11 = LoadU8(ctx, l8 + 1u);
        } while (l11 != 0u);
        goto dispatch_tail;
      }
      l12 = p2 - 1u;
      p2 = l6;
      // Walk one token of the pending text, counting lines and columns.
      do {
        switch (l12) {
          case 0u: goto scan_zero_case;
          case 1u: goto scan_one_case;
          default: goto scan_nonword;
        }
        // Merge point scan_zero_case.
        scan_zero_case:;
        l8 = 1u;
        if (l11 == 142u) { goto scan_class_lookup; }
        if ((static_cast<uint32_t>(l11 + 95u) & 255u) < 84u) { goto scan_class_lookup; }
        goto scan_nonword;
        // Merge point scan_one_case.
        scan_one_case:;
        l5 = 128u;
        l8 = 4294967295u;
        l13 = static_cast<uint32_t>(static_cast<int32_t>(l11 << 24u) >> 24);
        if ((l13 & 128u) == 0u) { goto scan_nonword; }
        do {
          l8 += 1u;
          l5 = static_cast<uint32_t>(static_cast<int32_t>(l5) >> 1);
          i1 = l13 & l5;
        } while (i1 != 0u);
        // Merge point scan_class_lookup.
        scan_class_lookup:;
        l5 = 0u;
        switch (l11) {
          case 0u: goto scan_advance;
          case 10u: goto scan_newline_counter;
          default: goto scan_counters_done;
        }
        // Merge point scan_newline_counter.
        scan_newline_counter:;
        StoreU32(ctx, p0 + 16u, LoadU32(ctx, p0 + 16u) + 1u);
        l5 = LoadU32(ctx, p0 + 20u) + 1u;
        // Merge point scan_counters_done.
        scan_counters_done:;
        StoreU32(ctx, p0 + 20u, l5);
        p2 += 1u;
        StoreU32(ctx, p0 + 8u, p2);
        // Merge point scan_advance.
        scan_advance:;
        i1 = p2 + l8;
        goto scan_step_advance;
        // Merge point scan_nonword.
        scan_nonword:;
        if (CompareBytes(ctx, p2, 758991u, 6u) == 0u) { goto scan_nonprintable_tail; }
        l8 = 0u;
        switch (l11) {
          case 0u: goto scan_token_ready;
          case 10u: goto scan_newline_second;
          default: goto scan_counter_second;
        }
        // Merge point scan_newline_second.
        scan_newline_second:;
        StoreU32(ctx, p0 + 16u, LoadU32(ctx, p0 + 16u) + 1u);
        l5 = LoadU32(ctx, p0 + 20u);
        l8 = l5 + 1u;
        // Merge point scan_counter_second.
        scan_counter_second:;
        StoreU32(ctx, p0 + 20u, l8);
        i1 = p2 + 1u;
        // Merge point scan_step_advance.
        scan_step_advance:;
        p2 = i1;
        StoreU32(ctx, p0 + 8u, i1);
        // Merge point scan_token_ready.
        scan_token_ready:;
        l11 = LoadU8(ctx, p2);
      } while (l11 != 0u);
      goto dispatch_tail;
    }
    goto find_catch_and_panic;
    // Merge point scan_nonprintable_tail.
    scan_nonprintable_tail:;
    p2 = static_cast<uint32_t>(p2 - l6);
    l5 = static_cast<uint32_t>(p2 < 4294967280u);
  }
  i0 = l5;
  i0 |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (i0) {
    l5 = (ResumePhase(ctx) != kResumeRunning) ? l5 : static_cast<uint32_t>(p2 >= 11u);
    i0 = l5;
    i0 |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
    if (i0) {
      if (ResumePhase(ctx) == kResumeRunning) {
        l5 = p2 + 16u;
        l8 = l5 & 4294967280u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 61u) {
        l5 = AllocateStringBuffer(ctx, l8);
        SyncMemoryBase(ctx);
        resume_point = 61u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        l8 |= 2147483648u;
        StoreU32(ctx, p1 + 24u, l8);
        StoreU32(ctx, p1 + 16u, l5);
        StoreU32(ctx, p1 + 20u, p2);
        goto text_allocated;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU8(ctx, p1 + 27u, p2);
      l5 = p1 + 16u;
      l8 = static_cast<uint32_t>(p2 == 0u);
      if (l8) { goto text_length_ready; }
    }
    // Merge point text_allocated.
    text_allocated:;
    if (ResumePhase(ctx) == kResumeRunning) {
      l8 = CopyBytes(ctx, l5, l6, p2);
      SyncMemoryBase(ctx);
    }
    // Merge point text_length_ready.
    text_length_ready:;
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU8(ctx, p2 + l5, 0u);
      StoreU8(ctx, p1 + 59u, 6u);
      StoreU32(ctx, p1 + 48u, LoadU32(ctx, 758998u));
      StoreU16(ctx, p1 + 52u, LoadU16(ctx, 759002u));
      StoreU8(ctx, p1 + 54u, 0u);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      StoreU32(ctx, p1 + 72u, p1 + 48u);
      l5 = p1 + 72u;
      l8 = p1 + 64u;
      p2 = p1 + 48u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 62u) {
      HostInvokeViiiiii(ctx, 1959u, p1, p4, p2, 759411u, l5, l8);
      SyncMemoryBase(ctx);
      resume_point = 62u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kPendingThrowSlot);
      StoreU32(ctx, kPendingThrowSlot, 0u);
      if (p2 != 1u) {
        l6 = LoadU32(ctx, p1);
        p2 = l6;
        if (static_cast<int32_t>(LoadS8(ctx, p2 + 39u)) <= -1) {
          DeallocateRawBlock(ctx, LoadU32(ctx, p2 + 28u));
          SyncMemoryBase(ctx);
        }
        StoreU64(ctx, p2 + 28u, LoadU64(ctx, p1 + 16u));
        l5 = LoadU32(ctx, p1 + 24u);
        StoreU32(ctx, p2 + 36u, l5);
        StoreU8(ctx, p1 + 27u, 0u);
        StoreU8(ctx, p1 + 16u, 0u);
        if (static_cast<int32_t>(LoadS8(ctx, p1 + 59u)) >= 0) { goto text_result_ready; }
        DeallocateRawBlock(ctx, LoadU32(ctx, p1 + 48u));
        SyncMemoryBase(ctx);
        if (static_cast<int32_t>(LoadS8(ctx, p1 + 27u)) > -1) { goto text_result_ready; }
        DeallocateRawBlock(ctx, LoadU32(ctx, p1 + 16u));
        SyncMemoryBase(ctx);
        // Merge point text_result_ready.
        text_result_ready:;
        p2 = LoadU32(ctx, p0 + 8u) + 6u;
        StoreU32(ctx, p0 + 8u, p2);
        goto dispatch_tail;
      }
      p4 = HostRet0(ctx);
      SyncMemoryBase(ctx);
      HostTempRet0(ctx);
      SyncMemoryBase(ctx);
      p0 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, p1 + 59u)) <= -1);
      if (p0) {
        p0 = LoadU32(ctx, p1 + 48u);
        DeallocateRawBlock(ctx, p0);
        SyncMemoryBase(ctx);
      }
      goto release_string_then_unwind;
    }
  }
  p0 = (ResumePhase(ctx) != kResumeRunning) ? p0 : (p1 + 16u);
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 63u) {
    RaiseStringLengthError(ctx, p0);
    SyncMemoryBase(ctx);
    resume_point = 63u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) { TrapUnreachable(); }
  // Merge point dispatch_tail.
  dispatch_tail:;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 64u) {
    l7 = AllocateStringBuffer(ctx, 44u);
    SyncMemoryBase(ctx);
    resume_point = 64u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    p2 = l7;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    l6 = LoadU32(ctx, p0 + 16u);
    StoreU32(ctx, p2, 745280u);
    StoreU32(ctx, kPendingThrowSlot, 0u);
    l5 = p2 + 4u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 65u) {
    l7 = HostOsQuery(ctx, 873u, l5, p3);
    SyncMemoryBase(ctx);
    resume_point = 65u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    p3 = l7;
  }
  // Publish the next flow entry and append it to the entry array.
  if (ResumePhase(ctx) == kResumeRunning) {
    p3 = LoadU32(ctx, kPendingThrowSlot);
    StoreU32(ctx, kPendingThrowSlot, 0u);
    if (p3 == 1u) { goto release_pending_then_unwind; }
    StoreU64(ctx, p2 + 28u, 0ull);
    StoreU32(ctx, p2 + 40u, l6);
    StoreU32(ctx, p2 + 36u, 0u);
    StoreU32(ctx, p2 + 16u, LoadU32(ctx, p4));
    p3 = p2 + 20u;
    StoreU32(ctx, p4, p3);
    l6 = LoadU32(ctx, p4 + 4u);
    StoreU32(ctx, p2 + 20u, l6);
    StoreU32(ctx, p4 + 4u, 0u);
    l5 = LoadU32(ctx, p4 + 8u);
    StoreU32(ctx, p2 + 24u, l5);
    StoreU32(ctx, p4 + 8u, 0u);
    i0 = l6 + 8u;
    i1 = p2 + 16u;
    i0 = (l5 != 0u) ? i0 : i1;
    StoreU32(ctx, i0, p3);
    StoreU32(ctx, p4, p4 + 4u);
    p3 = LoadU32(ctx, p0 + 96u);
    p4 = LoadU32(ctx, p0 + 100u);
    l5 = static_cast<uint32_t>(p3 != p4);
    if (l5) {
      StoreU32(ctx, p3, p2);
      p2 = p3 + 4u;
      StoreU32(ctx, p0 + 96u, p2);
      goto restore_sp_and_return;
    }
    l5 = p0 + 92u;
    p3 = LoadU32(ctx, l5);
    p4 = static_cast<uint32_t>(p4 - p3);
    l8 = static_cast<uint32_t>(static_cast<int32_t>(p4) >> 2);
    l6 = l8 + 1u;
    if (l6 >= 1073741824u) { goto allocator_assert_fail; }
    l5 = static_cast<uint32_t>(static_cast<int32_t>(p4) >> 1);
    l10 = static_cast<uint32_t>(l6 > l5);
    i0 = static_cast<uint32_t>(l10 ? l6 : l5);
    i1 = 1073741823u;
    i2 = static_cast<uint32_t>(l8 < 536870911u);
    l6 = static_cast<uint32_t>(i2 ? i0 : i1);
    l10 = static_cast<uint32_t>(l6 == 0u);
    l5 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (l10) { goto text_slot_ready; }
    if (l6 >= 1073741824u) { goto alloc_limit_error; }
    l5 = l6 << 2u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 66u) {
    l7 = AllocateStringBuffer(ctx, l5);
    SyncMemoryBase(ctx);
    resume_point = 66u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    l5 = l7;
  }
  // Merge point text_slot_ready.
  text_slot_ready:;
  if (ResumePhase(ctx) == kResumeRunning) {
    l8 = (l8 << 2u) + l5;
    StoreU32(ctx, l8, p2);
    if (static_cast<int32_t>(p4) >= 1) {
      l5 = CopyBytes(ctx, l5, p3, p4);
      SyncMemoryBase(ctx);
    }
    StoreU32(ctx, p0 + 100u, l5 + (l6 << 2u));
    p2 = l8 + 4u;
    StoreU32(ctx, p0 + 96u, p2);
    StoreU32(ctx, p0 + 92u, l5);
    p0 = static_cast<uint32_t>(p3 == 0u);
    if (p0) { goto restore_sp_and_return; }
    DeallocateRawBlock(ctx, p3);
    SyncMemoryBase(ctx);
  }
  // Merge point restore_sp_and_return.
  restore_sp_and_return:;
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = p1 + 80u;
    goto return_from_flow;
  }
  // Merge point release_pending_then_unwind.
  release_pending_then_unwind:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p4 = HostRet0(ctx);
    SyncMemoryBase(ctx);
    p0 = HostTempRet0(ctx);
    SyncMemoryBase(ctx);
    DeallocateRawBlock(ctx, p2);
    SyncMemoryBase(ctx);
    goto release_slot_then_trap;
  }
  // Merge point allocator_assert_fail.
  allocator_assert_fail:;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 67u) {
    RaiseAllocatorAssert(ctx, l5);
    SyncMemoryBase(ctx);
    resume_point = 67u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) { TrapUnreachable(); }
  // Merge point release_string_then_unwind.
  release_string_then_unwind:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, p1 + 27u)) > -1);
    if (p0) { goto release_slot_then_trap; }
    p0 = LoadU32(ctx, p1 + 16u);
    DeallocateRawBlock(ctx, p0);
    SyncMemoryBase(ctx);
    goto release_slot_then_trap;
  }
  // Merge point release_all_then_unwind.
  release_all_then_unwind:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p4 = HostRet0(ctx);
    SyncMemoryBase(ctx);
    p0 = HostTempRet0(ctx);
    SyncMemoryBase(ctx);
    if (static_cast<int32_t>(LoadS8(ctx, p1 + 27u)) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, p1 + 16u));
      SyncMemoryBase(ctx);
    }
    p0 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, p1 + 11u)) > -1);
    if (p0) { goto release_slot_then_trap; }
    p0 = LoadU32(ctx, p1);
    DeallocateRawBlock(ctx, p0);
    SyncMemoryBase(ctx);
  }
  // Merge point release_slot_then_trap.
  release_slot_then_trap:;
  if (ResumePhase(ctx) == kResumeRunning) {
    HostOsRelease(ctx, p4);
    SyncMemoryBase(ctx);
    TrapUnreachable();
  }
  // Merge point release_buffer_slot_trap.
  release_buffer_slot_trap:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = HostRet0(ctx);
    SyncMemoryBase(ctx);
    HostTempRet0(ctx);
    SyncMemoryBase(ctx);
    if (static_cast<int32_t>(LoadS8(ctx, p1 + 11u)) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, p1));
      SyncMemoryBase(ctx);
    }
    HostOsRelease(ctx, p0);
    SyncMemoryBase(ctx);
    TrapUnreachable();
  }
  // Merge point find_catch_and_panic.
  find_catch_and_panic:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = FindMatchingCatch(ctx, 0u);
    SyncMemoryBase(ctx);
    HostTempRet0(ctx);
    SyncMemoryBase(ctx);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 68u) {
    Panic(ctx, p0);
    SyncMemoryBase(ctx);
    resume_point = 68u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) { TrapUnreachable(); }
  // Merge point alloc_limit_error.
  alloc_limit_error:;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 69u) {
    RaiseAllocLimitError(ctx, 759343u);
    SyncMemoryBase(ctx);
    resume_point = 69u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) { TrapUnreachable(); }
  goto return_from_flow;
  // Merge point suspend_point.
  suspend_point:;
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  l7 = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, l7, p0);
  StoreU32(ctx, l7 + 4u, p1);
  StoreU32(ctx, l7 + 8u, p2);
  StoreU32(ctx, l7 + 12u, p3);
  StoreU32(ctx, l7 + 16u, p4);
  StoreU32(ctx, l7 + 20u, l5);
  StoreU32(ctx, l7 + 24u, l6);
  StoreU32(ctx, l7 + 28u, l8);
  StoreU32(ctx, l7 + 32u, l10);
  StoreU32(ctx, l7 + 36u, l11);
  StoreU32(ctx, l7 + 40u, l12);
  StoreU32(ctx, l7 + 44u, l13);
  StoreU32(ctx, l7 + 48u, l14);
  StoreU32(ctx, l7 + 52u, l15);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 56u);
  // Merge point return_from_flow.
  return_from_flow:;
  LeaveFrame();
}
}  // namespace IetRunner
}  // namespace artc
