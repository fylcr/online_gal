#include "util/engine_context.h"

namespace artc {

// Forward declarations -- the registered callees this step drives.
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void Panic(EngineContext* ctx, uint32_t a0);
uint32_t SetStringFromSource(EngineContext* ctx, uint32_t a0, uint32_t a1);
void AssignTaggedTreeRange(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void UnorderedLinkBucketNode(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2,
                             uint32_t a3, uint32_t a4, uint32_t a5);
uint32_t FindOrderedChainEntry(EngineContext* ctx, uint32_t a0, uint32_t a1);
// No product file yet -- the bridge jumpers are added next to these.
void LookupTagTableEntry(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3,
                         uint32_t a4, uint32_t a5);
void LookupTagTableEntryByKey(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2,
                              uint32_t a3, uint32_t a4, uint32_t a5);
void FillTagKeyRange(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3);
uint32_t UnlinkChainEntry(EngineContext* ctx, uint32_t a0, uint32_t a1);

namespace BacklogView {

// BacklogView::DispatchTag -- walks the backlog's tag chain, matches each
// node's key against the pending tag and runs the callbacks the match selects.
void DispatchTag(EngineContext* ctx, uint32_t p0, uint32_t p1, uint32_t p2) {
  EnterFrame();
  uint32_t l3 = 0u, l4 = 0u, l5 = 0u, l6 = 0u, l8 = 0u, l9 = 0u;
  uint32_t l10 = 0u, l11 = 0u, l12 = 0u, l13 = 0u;
  uint32_t resume_point = 0u;
  uint32_t i0 = 0u, i1 = 0u, i2 = 0u, i3 = 0u, i4 = 0u, i5 = 0u, i6 = 0u;

  // Message ids, the shared annotation table and the frame sizes this step uses.
  constexpr uint32_t kBacklogName = 718054u;              // "backlog"
  constexpr uint32_t kAnnotationKeys = 740656u;           // "s.backlog.annotation"
  constexpr uint32_t kMsgEmpty = 745008u;
  constexpr uint32_t kMsgAssertCondition = 745009u;       // px != 0
  constexpr uint32_t kMsgAssertFile = 745017u;            // boost/smart_ptr/shared_ptr.hpp
  constexpr uint32_t kMsgAssertName = 745073u;            // operator->
  constexpr uint32_t kMsgSlotPayload = 740137u;
  constexpr uint32_t kAssertWord = 857u;
  constexpr uint32_t kAssertLine = 734u;
  constexpr uint32_t kSaveFrameBytes = 48u;
  constexpr uint32_t kResumeWordBytes = 4u;
  constexpr uint32_t kStackBytes = 80u;
  constexpr uint32_t kScratchBase = 0xFFFFFFC0u;
  constexpr uint32_t kAllOnes = 0xFFFFFFFFu;
  constexpr int32_t kMinusOne = -1;

  // Rewind -- pop the saved slots, then the resume word this pass stored.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    l8 = LoadU32(ctx, ReturnFrameSp(ctx));
    p0 = LoadU32(ctx, l8);
    p2 = LoadU32(ctx, l8 + 8u);
    l3 = LoadU32(ctx, l8 + 12u);
    l4 = LoadU32(ctx, l8 + 16u);
    l5 = LoadU32(ctx, l8 + 20u);
    l6 = LoadU32(ctx, l8 + 24u);
    l9 = LoadU32(ctx, l8 + 28u);
    l10 = LoadU32(ctx, l8 + 32u);
    l11 = LoadU32(ctx, l8 + 36u);
    l12 = LoadU32(ctx, l8 + 40u);
    l13 = LoadU32(ctx, l8 + 44u);
    p1 = LoadU32(ctx, l8 + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    l4 = GuestStackSp(ctx) - kStackBytes;
    GuestStackSp(ctx) = l4;
  }
  // Build the pending key: the 12-byte inline "backlog" record, or the key
  // object the caller already handed over.
  if (ResumePhase(ctx) == kResumeRunning) {
    if (p2 != 0u) { goto entry_ready; }
    l11 = LoadU8(ctx, p0 + 13640u);
    StoreU8(ctx, l4 + 59u, 7u);
    StoreU8(ctx, l4 + 55u, 0u);
    StoreU32(ctx, l4 + 51u, LoadU32(ctx, kBacklogName + 3u));
    StoreU32(ctx, l4 + 48u, LoadU32(ctx, kBacklogName));
    p2 = LoadU32(ctx, p1 + 20u);
  }
  if (p2 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    // Merge point chain_loop.  Each link holds one candidate tag node.
    chain_loop:;
    if (ResumePhase(ctx) == kResumeRunning) {
      l6 = LoadU8(ctx, p2 + 27u);
      l6 = static_cast<uint32_t>(static_cast<int32_t>(l6 << 24u) >> 24);
      l12 = static_cast<uint32_t>(static_cast<int32_t>(l6) < 0);
      l3 = LoadU32(ctx, p2 + 20u);
      l6 = l12 ? l3 : l6;
      l10 = static_cast<uint32_t>(l6 < 7u);
      l5 = l10 ? l6 : 7u;
    }
    // Compare the node key against the pending key, then against the copy the
    // node keeps inline; a short inline key needs the first compare only.
    if (ResumePhase(ctx) == kResumeRunning) {
      if (l5 != 0u) {
        i0 = l4 + 48u;
        i1 = p2 + 16u;
        l9 = i1;
        i1 = LoadU32(ctx, i1);
        l13 = i1;
        i2 = l9;
        i3 = l12;
        i1 = i3 ? i1 : i2;
        l12 = i1;
        i2 = l5;
        l9 = CompareBytes(ctx, i0, i1, i2);
        if (l9 == 0u) {
          l3 = static_cast<uint32_t>(l6 <= 7u);
          if (l3) { goto key_reverse_merge; }
          goto chain_next;
        }
        l3 = static_cast<uint32_t>(static_cast<int32_t>(l9) <= kMinusOne);
        if (l3) { goto chain_next; }
        // Merge point key_reverse_merge.
        key_reverse_merge:;
        i0 = l12;
        i1 = l4 + 48u;
        i2 = l5;
        l6 = CompareBytes(ctx, i0, i1, i2);
        l3 = static_cast<uint32_t>(l6 == 0u);
        if (l3) { goto key_short_merge; }
        l3 = static_cast<uint32_t>(static_cast<int32_t>(l6) > kMinusOne);
        if (l3) { goto key_inline_reset; }
        goto chain_step;
      }
      l3 = static_cast<uint32_t>(l6 > 7u);
      if (l3) { goto chain_next; }
      // Merge point key_short_merge.
      key_short_merge:;
      if (l10) { goto chain_step; }
      // Merge point key_inline_reset.
      key_inline_reset:;
      StoreU8(ctx, l4 + 59u, 7u);
      StoreU32(ctx, l4 + 48u, LoadU32(ctx, kBacklogName));
      StoreU32(ctx, l4 + 51u, LoadU32(ctx, kBacklogName + 3u));
      StoreU8(ctx, l4 + 55u, 0u);
      StoreU32(ctx, kDataD015C, 0u);
      StoreU32(ctx, l4 + 72u, l4 + 48u);
      p2 = l4 + 24u;
      l3 = l4 + 48u;
      l6 = l4 + 72u;
      l5 = l4 - kScratchBase;
      p1 = p1 + 16u;
    }
    // Drive this node's tag callback.
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      HostInvokeViiiiii(ctx, 1959u, p2, p1, l3, kMsgEmpty, l6, l5);
      i0 = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p2 = 1u;
      p1 = static_cast<uint32_t>(p1 == 1u);
      if (p1) { goto catch_lookup; }
      p1 = LoadU32(ctx, l4 + 24u);
      l3 = LoadU32(ctx, p1 + 32u);
      l6 = LoadU8(ctx, p1 + 39u);
      l6 = static_cast<uint32_t>(static_cast<int32_t>(l6 << 24u) >> 24);
      l5 = static_cast<uint32_t>(static_cast<int32_t>(l6) < 0);
      l3 = static_cast<uint32_t>((l5 ? l3 : l6) == 1u);
    }
    // The callback raised a pending throw: unwind this node before re-raising.
    if (l3 != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p1 = p1 + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
        l8 = HostInvokeIiiiii(ctx, 1354u, p1, 0u, kAllOnes, kMsgSlotPayload, 1u);
        i0 = 1u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p1 = l8;
      }
      if (ResumePhase(ctx) != kResumeRunning) {
        i0 = p2;
      } else {
        p2 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p2 = static_cast<uint32_t>(p2 == 1u);
        if (p2) { goto catch_panic; }
        i0 = static_cast<uint32_t>(p1 != 0u);
      }
      p2 = i0;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (LoadS8(ctx, l4 + 59u) <= kMinusOne) {
        DeallocateRawBlock(ctx, LoadU32(ctx, l4 + 48u));
      }
      p1 = static_cast<uint32_t>(p2 == 0u);
      if (p1) { goto node_finish; }
      goto dispatch_entry;
    }
    // Merge point chain_step.
    chain_step:;
    p2 = ResumePhase(ctx) != kResumeRunning ? p2 : p2 + 4u;
    // Merge point chain_next.
    chain_next:;
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, p2);
      if (p2 != 0u) { goto chain_loop; }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (l11) { goto dispatch_entry; }
    goto node_finish;
  }
  // Merge point catch_lookup.  A failed lookup reports its own return value.
  catch_lookup:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = static_cast<uint32_t>(LoadS8(ctx, l4 + 59u) > kMinusOne);
    if (p0) { goto release_holder; }
    p0 = LoadU32(ctx, l4 + 48u);
    DeallocateRawBlock(ctx, p0);
    goto release_holder;
  }
  // Merge point catch_panic.  A raised panic unwinds through the catch chain.
  catch_panic:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = FindMatchingCatch(ctx, 0u);
    p1 = HostTempRet0(ctx);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
    Panic(ctx, p0);
    i0 = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) { goto trap_path; }
  // Merge point dispatch_entry.  The tag callback table hangs off the owner.
  dispatch_entry:;
  if (ResumePhase(ctx) == kResumeRunning) {
    l6 = LoadU32(ctx, p0 + 120u);
    p1 = static_cast<uint32_t>(l6 == 0u);
  }
  // The tag callback pointer is null or an exception is propagating: report it.
  if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
      HostFailAssertion(ctx, kAssertWord, kMsgAssertCondition, kMsgAssertFile, kAssertLine,
                        kMsgAssertName);
      i0 = 3u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0 != 0u) { goto trap_path; }
      goto throw_path;
    }
  }
  // Same guard for the annotation table the view holds at offset 16.
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, p0 + 16u);
    p2 = static_cast<uint32_t>(p1 == 0u);
  }
  if (p2 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
      HostFailAssertion(ctx, kAssertWord, kMsgAssertCondition, kMsgAssertFile, kAssertLine,
                        kMsgAssertName);
      i0 = 4u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0 != 0u) { goto trap_path; }
      goto throw_path;
    }
  }
  // Copy the tag node's own key holder into the scratch frame and take the
  // pending callback pair out of the view.
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, l4 + 40u, LoadU32(ctx, p1 + 100u));
    p1 = LoadU32(ctx, p1 + 104u);
    StoreU32(ctx, l4 + 44u, p1);
    if (p1 != 0u) {
      StoreU32(ctx, p1 + 4u, LoadU32(ctx, p1 + 4u) + 1u);
    }
    p2 = LoadU32(ctx, p0 + 128u);
    p1 = static_cast<uint32_t>(p2 == 0u);
  }
  if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
      HostFailAssertion(ctx, kAssertWord, kMsgAssertCondition, kMsgAssertFile, kAssertLine,
                        kMsgAssertName);
      i0 = 5u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0 != 0u) { goto trap_path; }
      goto throw_path;
    }
  }
  // Hand the callback the view's own key at offset 13308.
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = p0 + 13308u;
    StoreU32(ctx, l4 + 24u, p1);
    StoreU32(ctx, kDataD015C, 0u);
    l3 = l4 + 48u;
    l5 = l4 + 24u;
    l9 = l4 + 72u;
    p2 = p2 + 16u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
    HostInvokeViiiiii(ctx, 2611u, l3, p2, p1, kMsgEmpty, l5, l9);
    i0 = 6u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (p2 == 1u) { goto slot_unwind_6; }
    p2 = LoadU32(ctx, p0 + 128u);
    l3 = static_cast<uint32_t>(p2 == 0u);
  }
  if (l3 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 7u) {
      HostFailAssertion(ctx, kAssertWord, kMsgAssertCondition, kMsgAssertFile, kAssertLine,
                        kMsgAssertName);
      i0 = 7u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0 != 0u) { goto trap_path; }
      goto throw_path;
    }
  }
  // Hand the callback the key the node keeps alone.
  if (ResumePhase(ctx) == kResumeRunning) {
    l5 = LoadU32(ctx, l4 + 48u);
    StoreU32(ctx, l4 + 24u, p1);
    StoreU32(ctx, kDataD015C, 0u);
    l3 = l4 + 48u;
    l9 = l4 + 24u;
    l11 = l4 + 72u;
    p2 = p2 + 4u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 8u) {
    HostInvokeViiiiii(ctx, 2613u, l3, p2, p1, kMsgEmpty, l9, l11);
    i0 = 8u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (p1 == 1u) { goto slot_unwind_6; }
    p2 = LoadU32(ctx, p0 + 112u);
    p1 = static_cast<uint32_t>(p2 == 0u);
  }
  if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 9u) {
      HostFailAssertion(ctx, kAssertWord, kMsgAssertCondition, kMsgAssertFile, kAssertLine,
                        kMsgAssertName);
      i0 = 9u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0 != 0u) { goto trap_path; }
      goto throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    l11 = LoadU32(ctx, l4 + 48u);
    l12 = LoadU8(ctx, p0 + 13641u);
    StoreU32(ctx, kDataD015C, 0u);
  }
  // Build the 32-byte tag descriptor the view stores for this dispatch.
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 10u) {
    l8 = HostFoldLayoutOffset(ctx, 1032u, 32u);
    i0 = 10u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    p1 = l8;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    l10 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    l3 = static_cast<uint32_t>(l10 == 1u);
    if (l3 != 0u) { goto slot_unwind_5; }
    StoreU32(ctx, l4 + 24u, p1);
    StoreU64(ctx, l4 + 28u, 0x80000000FFED1C14ull);
    StoreU8(ctx, p1 + 20u, 0u);
    StoreU32(ctx, p1 + 16u, LoadU32(ctx, kAnnotationKeys + 16u));
    StoreU64(ctx, p1 + 8u, LoadU64(ctx, kAnnotationKeys + 8u));
    StoreU64(ctx, p1, LoadU64(ctx, kAnnotationKeys));
    p1 = LoadU32(ctx, LoadU32(ctx, p2) + 16u);
    StoreU32(ctx, kDataD015C, 0u);
    l9 = l4 + 24u;
    l3 = l4 + 48u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 11u) {
    HostSyncSlot(ctx, p1, l3, p2, l9);
    i0 = 11u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (p1 == 1u) { goto slot_unwind_4; }
    StoreU32(ctx, kDataD015C, 0u);
    p2 = l5 + 28u;
    l3 = l11 + 28u;
    l5 = static_cast<uint32_t>(l12 != 0u);
    l9 = l4 + 48u;
    p1 = l4 + 40u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 12u) {
    HostInvokeViiiiii(ctx, 2615u, l6, p1, p2, l3, l5, l9);
    i0 = 12u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Release the scratch key holders before the dispatch result is stored.
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (p1 == 1u) { goto slot_unwind_3; }
    if (LoadS8(ctx, l4 + 59u) <= kMinusOne) {
      DeallocateRawBlock(ctx, LoadU32(ctx, l4 + 48u));
    }
    if (LoadS8(ctx, l4 + 35u) <= kMinusOne) {
      DeallocateRawBlock(ctx, LoadU32(ctx, l4 + 24u));
    }
    p1 = LoadU32(ctx, l4 + 44u);
    if (p1 == 0u) { goto node_finish; }
    p2 = LoadU32(ctx, p1 + 4u);
    l3 = p2 - 1u;
    StoreU32(ctx, p1 + 4u, l3);
    if (p2 != 1u) { goto node_finish; }
    p2 = LoadU32(ctx, LoadU32(ctx, p1) + 8u);
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 13u) {
    HostReleaseSlotPair(ctx, p2, p1);
    i0 = 13u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Second holder: the callback pair copied into the scratch frame at offset 44.
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p2 = static_cast<uint32_t>(p2 != 1u);
  }
  if (p2 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, p1 + 8u);
      l3 = p2 - 1u;
      StoreU32(ctx, p1 + 8u, l3);
      if (p2 != 1u) { goto node_finish; }
      p2 = LoadU32(ctx, LoadU32(ctx, p1) + 12u);
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 14u) {
      HostReleaseSlotPair(ctx, p2, p1);
      i0 = 14u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p1 = static_cast<uint32_t>(p1 != 1u);
      if (p1 != 0u) { goto node_finish; }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) { goto throw_path; }
  // Merge point slot_unwind_6.  A throwing callback unwinds every holder here.
  slot_unwind_6:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = HostRet0(ctx);
    p0 = HostTempRet0(ctx);
    goto slot_unwind_1;
  }
  // Merge point slot_unwind_5.
  slot_unwind_5:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = HostRet0(ctx);
    p0 = HostTempRet0(ctx);
    goto slot_unwind_1;
  }
  // Merge point slot_unwind_4.
  slot_unwind_4:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = HostRet0(ctx);
    p0 = HostTempRet0(ctx);
    goto slot_unwind_2;
  }
  // Merge point slot_unwind_3.  Drop the scratch copy of the node key.
  slot_unwind_3:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = HostRet0(ctx);
    p0 = HostTempRet0(ctx);
    p0 = static_cast<uint32_t>(LoadS8(ctx, l4 + 59u) > kMinusOne);
    if (p0 != 0u) { goto slot_unwind_2; }
    p0 = LoadU32(ctx, l4 + 48u);
    DeallocateRawBlock(ctx, p0);
  }
  // Merge point slot_unwind_2.  Drop the inline key the view rebuilt.
  slot_unwind_2:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = static_cast<uint32_t>(LoadS8(ctx, l4 + 35u) > kMinusOne);
    if (p0 != 0u) { goto slot_unwind_1; }
    p0 = LoadU32(ctx, l4 + 24u);
    DeallocateRawBlock(ctx, p0);
  }
  // Merge point slot_unwind_1.  Release the callback pair copied into the frame.
  slot_unwind_1:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = LoadU32(ctx, l4 + 44u);
    p2 = static_cast<uint32_t>(p0 == 0u);
    if (p2 != 0u) { goto release_holder; }
    p2 = LoadU32(ctx, p0 + 4u);
    l4 = p2 - 1u;
    StoreU32(ctx, p0 + 4u, l4);
    p2 = static_cast<uint32_t>(p2 != 1u);
    if (p2 != 0u) { goto release_holder; }
    p2 = LoadU32(ctx, LoadU32(ctx, p0) + 8u);
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 15u) {
    HostReleaseSlotPair(ctx, p2, p0);
    i0 = 15u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // The view's own holder counts down next.
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p2 = static_cast<uint32_t>(p2 != 1u);
  }
  if (p2 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, p0 + 8u);
      l4 = p2 - 1u;
      StoreU32(ctx, p0 + 8u, l4);
      p2 = static_cast<uint32_t>(p2 != 1u);
      if (p2 != 0u) { goto release_holder; }
      p2 = LoadU32(ctx, LoadU32(ctx, p0) + 12u);
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 16u) {
      HostReleaseSlotPair(ctx, p2, p0);
      i0 = 16u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0 != 0u) { goto release_holder; }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) { goto throw_path; }
  // Merge point release_holder.  The guest slot pair is dropped for good here.
  release_holder:;
  if (ResumePhase(ctx) == kResumeRunning) {
    HostOsRelease(ctx, p1);
    TrapUnreachable();
  }
  // Merge point node_finish.  No callback claimed the key: store the dispatch.
  node_finish:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = LoadU32(ctx, p0 + 128u);
    p1 = static_cast<uint32_t>(p2 == 0u);
  }
  if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 17u) {
      HostFailAssertion(ctx, kAssertWord, kMsgAssertCondition, kMsgAssertFile, kAssertLine,
                        kMsgAssertName);
      i0 = 17u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0 != 0u) { goto trap_path; }
      goto throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = p0 + 13308u;
    StoreU32(ctx, l4 + 24u, p1);
    l3 = l4 + 48u;
    l6 = l4 + 24u;
    l5 = l4 + 72u;
    p2 = p2 + 4u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 18u) {
    UnorderedLinkBucketNode(ctx, l3, p2, p1, kMsgEmpty, l6, l5);
    i0 = 18u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Walk the bucket chain hanging off the stored entry and drop each node.
  if (ResumePhase(ctx) == kResumeRunning) {
    l6 = LoadU32(ctx, l4 + 48u);
    p2 = LoadU32(ctx, l6 + 32u);
    l5 = LoadU32(ctx, l6 + 28u);
    l3 = static_cast<uint32_t>(p2 != l5);
  }
  if (l3 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    // Merge point prop_walk.
    prop_walk:;
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = p2 - 44u;
      l3 = LoadU32(ctx, LoadU32(ctx, p2));
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 19u) {
      l8 = CallTableT2(ctx, l3, p2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = 19u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      l3 = l8;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      l3 = static_cast<uint32_t>(p2 != l5);
      if (l3 != 0u) { goto prop_walk; }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, l6 + 32u, l5);
    StoreU32(ctx, l4 + 24u, p1);
    l3 = p0 + 13512u;
    l6 = l4 + 24u;
    l5 = l4 + 72u;
    p2 = l4 + 48u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 20u) {
    LookupTagTableEntry(ctx, p2, l3, p1, kMsgEmpty, l6, l5);
    i0 = 20u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    l6 = LoadU32(ctx, p0 + 128u);
    p2 = static_cast<uint32_t>(l6 == 0u);
  }
  if (p2 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 21u) {
      HostFailAssertion(ctx, kAssertWord, kMsgAssertCondition, kMsgAssertFile, kAssertLine,
                        kMsgAssertName);
      i0 = 21u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0 != 0u) { goto trap_path; }
      goto throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = LoadU32(ctx, l4 + 48u);
    StoreU32(ctx, l4 + 24u, p1);
    l6 = l6 + 16u;
    l5 = l4 + 24u;
    l9 = l4 + 72u;
    l3 = l4 + 48u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 22u) {
    LookupTagTableEntry(ctx, l3, l6, p1, kMsgEmpty, l5, l9);
    i0 = 22u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    l12 = LoadU32(ctx, l4 + 48u);
    l3 = static_cast<uint32_t>(l12 != p2);
  }
  // Rehash the bucket the entry was linked into when its key moved.
  if (l3 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      l6 = LoadU32(ctx, p2 + 32u);
      l5 = LoadU32(ctx, p2 + 44u);
      l10 = l5 / 24u;
      l11 = l6 + (l10 << 2u);
      l9 = static_cast<uint32_t>(LoadU32(ctx, p2 + 36u) == l6);
      if (l9 != 0u) {
        l10 = 0u;
      } else {
        l13 = LoadU32(ctx, l11);
        l10 = l13 + (l5 - l10 * 24u) * 168u;
      }
      StoreU32(ctx, l4 + 52u, l10);
      StoreU32(ctx, l4 + 48u, l11);
      l5 = l6 + LoadU32(ctx, p2 + 48u);
      l11 = l5 / 24u;
      p2 = l6 + (l11 << 2u);
      if (l9 != 0u) {
        StoreU32(ctx, l4 + 28u, 0u);
      } else {
        l9 = l11 * 24u;
        l5 = (l5 - l9) * 168u;
        StoreU32(ctx, l4 + 28u, l5 + LoadU32(ctx, p2));
      }
      StoreU32(ctx, l4 + 24u, p2);
      StoreU64(ctx, l4 + 16u, LoadU64(ctx, l4 + 48u));
      StoreU64(ctx, l4 + 8u, LoadU64(ctx, l4 + 24u));
      l3 = l4 + 16u;
      l6 = l4 + 8u;
      p2 = l12 + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 23u) {
      FillTagKeyRange(ctx, p2, l3, l6, 0u);
      i0 = 23u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    l11 = LoadU32(ctx, p0 + 128u);
    p2 = static_cast<uint32_t>(l11 == 0u);
  }
  if (p2 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 24u) {
      HostFailAssertion(ctx, kAssertWord, kMsgAssertCondition, kMsgAssertFile, kAssertLine,
                        kMsgAssertName);
      i0 = 24u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0 != 0u) { goto trap_path; }
      goto throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = LoadU32(ctx, l11 + 44u);
    l3 = static_cast<uint32_t>(p2 == 0u);
  }
  // An empty bucket in the freshly linked entry: nothing left to walk.
  if (ResumePhase(ctx) == kResumeRunning) {
    if (l3 != 0u) { goto field_entry; }
    l3 = LoadU32(ctx, p0 + 13308u);
    l6 = LoadU8(ctx, p0 + 13319u);
    l5 = static_cast<uint32_t>(static_cast<int32_t>(l6 << 24u) >> 24 < 0);
    l12 = (l5 != 0u) ? l3 : p1;
    l6 = (l5 != 0u) ? LoadU32(ctx, p0 + 13312u) : l6;
  }
  // Merge point prop_inner_loop.  Compare the pending key against each node.
  prop_inner_loop:;
  if (ResumePhase(ctx) == kResumeRunning) {
    l5 = LoadU8(ctx, p2 + 27u);
    l9 = static_cast<uint32_t>(static_cast<int32_t>(l5 << 24u) >> 24 < 0);
    l3 = LoadU32(ctx, p2 + 20u);
    l5 = (l9 != 0u) ? l3 : l5;
    l13 = static_cast<uint32_t>(l6 > l5);
    l10 = (l13 != 0u) ? l5 : l6;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (l10 != 0u) {
      l3 = p2 + 16u;
      l9 = (l9 != 0u) ? LoadU32(ctx, l3) : l3;
      l3 = CompareBytes(ctx, l12, l9, l10);
      if (l3 == 0u) {
        l3 = static_cast<uint32_t>(l5 <= l6);
        if (l3 != 0u) { goto prop_reverse_merge; }
        goto prop_next;
      }
      l3 = static_cast<uint32_t>(static_cast<int32_t>(l3) <= kMinusOne);
      if (l3 != 0u) { goto prop_next; }
      // Merge point prop_reverse_merge.
      prop_reverse_merge:;
      l5 = CompareBytes(ctx, l9, l12, l10);
      l3 = static_cast<uint32_t>(l5 == 0u);
      if (l3 != 0u) { goto prop_short_merge; }
      l3 = static_cast<uint32_t>(static_cast<int32_t>(l5) > kMinusOne);
      if (l3 != 0u) { goto prop_inline_reset; }
      goto prop_step;
    }
    l3 = static_cast<uint32_t>(l5 > l6);
    if (l3 != 0u) { goto prop_next; }
    // Merge point prop_short_merge.
    prop_short_merge:;
    if (l13 != 0u) { goto prop_step; }
    // Merge point prop_inline_reset.
    prop_inline_reset:;
    StoreU32(ctx, l4 + 24u, p1);
    l3 = l11 + 40u;
    l6 = l4 + 24u;
    l5 = l4 + 72u;
    p2 = l4 + 48u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 25u) {
    LookupTagTableEntryByKey(ctx, p2, l3, p1, kMsgEmpty, l6, l5);
    i0 = 25u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    l6 = LoadU32(ctx, p0 + 128u);
    if (l6 != 0u) { goto entry_key_write; }
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 26u) {
    HostFailAssertion(ctx, kAssertWord, kMsgAssertCondition, kMsgAssertFile, kAssertLine,
                      kMsgAssertName);
    i0 = 26u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p0 = static_cast<uint32_t>(p0 != 1u);
    if (p0 != 0u) { goto trap_path; }
    goto throw_path;
  }
  // Merge point prop_step.
  prop_step:;
  p2 = ResumePhase(ctx) != kResumeRunning ? p2 : p2 + 4u;
  // Merge point prop_next.
  prop_next:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = LoadU32(ctx, p2);
    if (p2 != 0u) { goto prop_inner_loop; }
    goto field_entry;
  }
  // Merge point entry_key_write.  Store the entry's own copy of the key.
  entry_key_write:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = LoadU32(ctx, l4 + 48u);
    StoreU32(ctx, l4 + 24u, p1);
    l6 = l6 + 28u;
    l5 = l4 + 24u;
    l9 = l4 + 72u;
    l3 = l4 + 48u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 27u) {
    LookupTagTableEntryByKey(ctx, l3, l6, p1, kMsgEmpty, l5, l9);
    i0 = 27u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    l5 = p2 + 32u;
    l6 = LoadU32(ctx, l4 + 48u);
    l3 = l6 + 32u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 28u) {
    l8 = SetStringFromSource(ctx, l3, l5);
    i0 = 28u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    l3 = l8;
  }
  l3 = (ResumePhase(ctx) == kResumeRunning) ? static_cast<uint32_t>(p2 != l6) : l3;
  // The entry moved into another bucket: fix up the range it was tagged with.
  if (l3 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      l5 = LoadU32(ctx, p2 + 44u);
      l9 = p2 + 48u;
      l3 = l6 + 44u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 29u) {
      AssignTaggedTreeRange(ctx, l3, l5, l9);
      i0 = 29u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    l5 = p2 + 56u;
    l3 = l6 + 56u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 30u) {
    l8 = SetStringFromSource(ctx, l3, l5);
    i0 = 30u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    l3 = l8;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, l6 + 68u, LoadU32(ctx, p2 + 68u));
    p2 = LoadU32(ctx, p0 + 128u);
    l3 = static_cast<uint32_t>(p2 == 0u);
  }
  if (l3 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 31u) {
      HostFailAssertion(ctx, kAssertWord, kMsgAssertCondition, kMsgAssertFile, kAssertLine,
                        kMsgAssertName);
      i0 = 31u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0 != 0u) { goto trap_path; }
      goto throw_path;
    }
  }
  // Unlink the entry from the ordered chain when it was not found there.
  if (ResumePhase(ctx) == kResumeRunning) {
    l6 = p2 + 40u;
    l5 = FindOrderedChainEntry(ctx, l6, p1);
    p2 = static_cast<uint32_t>(l5 == p2 + 44u);
    if (p2 != 0u) { goto field_entry; }
    p2 = UnlinkChainEntry(ctx, l6, l5);
  }
  // Merge point field_entry.  Look the key up in the second table.
  field_entry:;
  if (ResumePhase(ctx) == kResumeRunning) {
    l11 = LoadU32(ctx, p0 + 128u);
    p2 = static_cast<uint32_t>(l11 == 0u);
  }
  if (p2 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 32u) {
      HostFailAssertion(ctx, kAssertWord, kMsgAssertCondition, kMsgAssertFile, kAssertLine,
                        kMsgAssertName);
      i0 = 32u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0 != 0u) { goto trap_path; }
      goto throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = LoadU32(ctx, l11 + 68u);
    l3 = static_cast<uint32_t>(p2 == 0u);
    if (l3 != 0u) { goto entry_ready; }
    l3 = LoadU32(ctx, p0 + 13308u);
    l6 = LoadU8(ctx, p0 + 13319u);
    l5 = static_cast<uint32_t>(static_cast<int32_t>(l6 << 24u) >> 24 < 0);
    l12 = (l5 != 0u) ? l3 : p1;
    l6 = (l5 != 0u) ? LoadU32(ctx, p0 + 13312u) : l6;
  }
  // Merge point field_walk.  Compare the pending key against each node.
  field_walk:;
  if (ResumePhase(ctx) == kResumeRunning) {
    l5 = LoadU8(ctx, p2 + 27u);
    l9 = static_cast<uint32_t>(static_cast<int32_t>(l5 << 24u) >> 24 < 0);
    l3 = LoadU32(ctx, p2 + 20u);
    l5 = (l9 != 0u) ? l3 : l5;
    l13 = static_cast<uint32_t>(l6 > l5);
    l10 = (l13 != 0u) ? l5 : l6;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (l10 != 0u) {
      l3 = p2 + 16u;
      l9 = (l9 != 0u) ? LoadU32(ctx, l3) : l3;
      l3 = CompareBytes(ctx, l12, l9, l10);
      if (l3 == 0u) {
        l3 = static_cast<uint32_t>(l5 <= l6);
        if (l3 != 0u) { goto field_reverse_merge; }
        goto field_next;
      }
      l3 = static_cast<uint32_t>(static_cast<int32_t>(l3) <= kMinusOne);
      if (l3 != 0u) { goto field_next; }
      // Merge point field_reverse_merge.
      field_reverse_merge:;
      l5 = CompareBytes(ctx, l9, l12, l10);
      l3 = static_cast<uint32_t>(l5 == 0u);
      if (l3 != 0u) { goto field_short_merge; }
      l3 = static_cast<uint32_t>(static_cast<int32_t>(l5) > kMinusOne);
      if (l3 != 0u) { goto field_inline_reset; }
      goto field_step;
    }
    l3 = static_cast<uint32_t>(l5 > l6);
    if (l3 != 0u) { goto field_next; }
    // Merge point field_short_merge.
    field_short_merge:;
    if (l13 != 0u) { goto field_step; }
    // Merge point field_inline_reset.
    field_inline_reset:;
    StoreU32(ctx, l4 + 24u, p1);
    l3 = l11 - kScratchBase;
    l6 = l4 + 24u;
    l5 = l4 + 72u;
    p2 = l4 + 48u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 33u) {
    LookupTagTableEntryByKey(ctx, p2, l3, p1, kMsgEmpty, l6, l5);
    i0 = 33u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    l6 = LoadU32(ctx, p0 + 128u);
    if (l6 != 0u) { goto field_cmp_reverse; }
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 34u) {
    HostFailAssertion(ctx, kAssertWord, kMsgAssertCondition, kMsgAssertFile, kAssertLine,
                      kMsgAssertName);
    i0 = 34u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p0 = static_cast<uint32_t>(p0 != 1u);
    if (p0 != 0u) { goto trap_path; }
    goto throw_path;
  }
  // Merge point field_step.
  field_step:;
  p2 = ResumePhase(ctx) != kResumeRunning ? p2 : p2 + 4u;
  // Merge point field_next.
  field_next:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = LoadU32(ctx, p2);
    if (p2 != 0u) { goto field_walk; }
    goto entry_ready;
  }
  // Merge point field_cmp_reverse.  Write the key into the entry found above.
  field_cmp_reverse:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = LoadU32(ctx, l4 + 48u);
    StoreU32(ctx, l4 + 24u, p1);
    l6 = l6 + 52u;
    l5 = l4 + 24u;
    l9 = l4 + 72u;
    l3 = l4 + 48u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 35u) {
    LookupTagTableEntryByKey(ctx, l3, l6, p1, kMsgEmpty, l5, l9);
    i0 = 35u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    l5 = p2 + 32u;
    l6 = LoadU32(ctx, l4 + 48u);
    l3 = l6 + 32u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 36u) {
    l8 = SetStringFromSource(ctx, l3, l5);
    i0 = 36u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    l3 = l8;
  }
  l3 = (ResumePhase(ctx) == kResumeRunning) ? static_cast<uint32_t>(p2 != l6) : l3;
  // The entry moved into another bucket: fix up the range it was tagged with.
  if (l3 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      l5 = LoadU32(ctx, p2 + 44u);
      l9 = p2 + 48u;
      l3 = l6 + 44u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 37u) {
      AssignTaggedTreeRange(ctx, l3, l5, l9);
      i0 = 37u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    l5 = p2 + 56u;
    l3 = l6 + 56u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 38u) {
    l8 = SetStringFromSource(ctx, l3, l5);
    i0 = 38u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    l3 = l8;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, l6 + 68u, LoadU32(ctx, p2 + 68u));
    p2 = LoadU32(ctx, p0 + 128u);
    l3 = static_cast<uint32_t>(p2 == 0u);
  }
  if (l3 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 39u) {
      HostFailAssertion(ctx, kAssertWord, kMsgAssertCondition, kMsgAssertFile, kAssertLine,
                        kMsgAssertName);
      i0 = 39u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0 != 0u) { goto trap_path; }
      goto throw_path;
    }
  }
  // Unlink the entry from the ordered chain when it was not found there.
  if (ResumePhase(ctx) == kResumeRunning) {
    l6 = p2 - kScratchBase;
    p1 = FindOrderedChainEntry(ctx, l6, p1);
    p2 = static_cast<uint32_t>(p1 == p2 + 68u);
    if (p2 != 0u) { goto entry_ready; }
    p1 = UnlinkChainEntry(ctx, l6, p1);
  }
  // Merge point entry_ready.  Walk the pending slot caches left to right.
  entry_ready:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU8(ctx, p0 + 48u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (p1 != 0u) {
      p1 = LoadU8(ctx, p0 + 49u);
      if (p1 != 0u) { goto slot_retry_entry; }
      p1 = LoadU8(ctx, p0 + 50u);
      if (p1 != 0u) { goto slot_retry_entry; }
    }
    p1 = LoadU32(ctx, p0 + 13212u);
    if (p1 != 0u) { goto slot_retry_entry; }
    p1 = LoadU32(ctx, p0 + 16u);
    p2 = static_cast<uint32_t>(p1 == 0u);
  }
  if (p2 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 40u) {
      HostFailAssertion(ctx, kAssertWord, kMsgAssertCondition, kMsgAssertFile, kAssertLine,
                        kMsgAssertName);
      i0 = 40u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0 != 0u) { goto trap_path; }
      goto throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, p1 + 100u);
    p2 = static_cast<uint32_t>(p1 == 0u);
  }
  if (p2 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 41u) {
      HostFailAssertion(ctx, kAssertWord, kMsgAssertCondition, kMsgAssertFile, kAssertLine,
                        kMsgAssertName);
      i0 = 41u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0 != 0u) { goto trap_path; }
      goto throw_path;
    }
  }
  // Tag the range the pending key covers, then re-enter the annotation table.
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = p0 + 13308u;
    StoreU32(ctx, l4 + 24u, p2);
    l3 = l4 + 48u;
    l6 = l4 + 24u;
    l5 = l4 + 72u;
    p0 = p0 + 13548u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 42u) {
    UnorderedLinkBucketNode(ctx, l3, p0, p2, kMsgEmpty, l6, l5);
    i0 = 42u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = LoadU32(ctx, LoadU32(ctx, p1) + 376u);
    p0 = LoadU32(ctx, l4 + 48u) + 28u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 43u) {
    CallTableT0(ctx, p2, p1, p0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    i0 = 43u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto unwind_exit;
  }
  // Merge point slot_retry_entry.  Fall through the remaining slot caches.
  slot_retry_entry:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = LoadU32(ctx, p0 + 16u);
    p1 = static_cast<uint32_t>(p0 == 0u);
  }
  if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 44u) {
      HostFailAssertion(ctx, kAssertWord, kMsgAssertCondition, kMsgAssertFile, kAssertLine,
                        kMsgAssertName);
      i0 = 44u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0 != 0u) { goto trap_path; }
      goto throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = LoadU32(ctx, p0 + 100u);
    p1 = static_cast<uint32_t>(p0 == 0u);
  }
  if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 45u) {
      HostFailAssertion(ctx, kAssertWord, kMsgAssertCondition, kMsgAssertFile, kAssertLine,
                        kMsgAssertName);
      i0 = 45u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0 != 0u) { goto trap_path; }
      goto throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, LoadU32(ctx, p0) + 372u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 46u) {
    CallTableFunction(ctx, p1, p0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    i0 = 46u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Merge point unwind_exit.  Hand the scratch frame back and leave.
  unwind_exit:;
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = l4 + kStackBytes;
    goto exit_frame;
  }
  // Merge point trap_path.
  trap_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  // Merge point throw_path.  Hand the pending exception to the host.
  throw_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = FindMatchingCatch(ctx, 0u);
    HostTempRet0(ctx);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 47u) {
    Panic(ctx, p0);
    i0 = 47u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  goto exit_frame;
  // Suspend here: record the resume point, then spill every live local.
  suspend_point:;
  l8 = i0;
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), l8);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
  l8 = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, l8, p0);
  StoreU32(ctx, l8 + 4u, p1);
  StoreU32(ctx, l8 + 8u, p2);
  StoreU32(ctx, l8 + 12u, l3);
  StoreU32(ctx, l8 + 16u, l4);
  StoreU32(ctx, l8 + 20u, l5);
  StoreU32(ctx, l8 + 24u, l6);
  StoreU32(ctx, l8 + 28u, l9);
  StoreU32(ctx, l8 + 32u, l10);
  StoreU32(ctx, l8 + 36u, l11);
  StoreU32(ctx, l8 + 40u, l12);
  StoreU32(ctx, l8 + 44u, l13);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  exit_frame:;
  LeaveFrame();
}

}  // namespace BacklogView
}  // namespace artc
