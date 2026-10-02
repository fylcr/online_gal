#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this dispatcher drives that have no product file yet.
// Product helpers that live outside this file and are not declared by the shared header.
void Panic(EngineContext* ctx, uint32_t code);
uint32_t CompareBytes(EngineContext* ctx, uint32_t lhs, uint32_t rhs, uint32_t n);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t bytes);
uint32_t CopyBytes(EngineContext* ctx, uint32_t dst, uint32_t src, uint32_t n);
void ReleaseNodeTree(EngineContext* ctx, uint32_t state, uint32_t node);
void ResetSharedPtr03(EngineContext* ctx, uint32_t target, uint32_t value);

namespace BacklogView {
void DispatchAux(EngineContext* ctx, uint32_t target, uint32_t mode);
}  // namespace BacklogView

namespace Compositor {

// Compositor::EffectDispatch -- apply one scene tag record to the scenario.
// The operands are the scenario core object and the tag record being dispatched.
void EffectDispatch(EngineContext* ctx, uint32_t p0, uint32_t p1) {
  EnterFrame();
  constexpr uint32_t kSaveFrameBytes = 80u;
  constexpr uint32_t kResumeWordBytes = 4u;
  constexpr uint32_t kGuestFrameBytes = 496u;
  // Operands of the shared_ptr null-dereference assertion every member read below carries.
  constexpr uint32_t kAssertWord = 857u;
  constexpr uint32_t kAssertLine = 734u;
  constexpr uint32_t kMsgPx = 0xBC7ADu;
  constexpr uint32_t kMsgSharedPtr = 0xBC7B5u;
  constexpr uint32_t kMsgArrow = 0xBC7EDu;
  // The operator* variant of the same assertion, and the Lua-side report word.
  constexpr uint32_t kAssertLineStar = 728u;
  constexpr uint32_t kMsgOperatorStar = 0xBCAE3u;
  constexpr uint32_t kAssertWordLua = 2597u;
  constexpr uint32_t kMsgPxRef = 0xBC7ACu;
  // Reports the self-modifying data segment raises when it rewrites a load record.
  constexpr uint32_t kMsgSelfModifyFail = 771585u;
  constexpr uint32_t kMsgSelfModified = 771639u;
  // Property names the six claimed tags write onto the scenario; the empty and colour ones too.
  constexpr uint32_t kMsgEmpty = 771564u;
  constexpr uint32_t kNameSceIn = 771692u;
  constexpr uint32_t kNameSceOut = 771698u;
  constexpr uint32_t kNameLyProp = 771729u;
  constexpr uint32_t kNameAnime = 771736u;
  constexpr uint32_t kNameColorMultiply = 771742u;
  constexpr uint32_t kNameMultiply = 771747u;
  constexpr uint32_t kColorWhite = 771756u;
  constexpr uint32_t kNameLyc = 771763u;
  constexpr uint32_t kNameVideo = 771767u;
  // "s.current_message_layer" plus its two 8-byte tails, copied out as one literal.
  constexpr uint32_t kNameMessageLayer = 771705u;
  constexpr uint32_t kNameMessageLayerTail = 771713u;
  constexpr uint32_t kNameLayerTail = 771720u;
  // Selectors of the host hooks this dispatcher drives.
  constexpr uint32_t kHookTagArgs = 2709u;
  constexpr uint32_t kSelectorReadTagArg = 2720u;
  constexpr uint32_t kSelectorNotify = 3540u;
  constexpr uint32_t kSelectorNamedProp = 1354u;
  constexpr uint32_t kSelectorDispatch = 1290u;
  constexpr uint32_t kSlotPairBase = 2032u;
  // Two bytes of read-only data used as the assertion message of the teardown paths.
  constexpr uint32_t kAssertMessageData = 745280u;
  // Guard word the tag-record assertion raises, and the selector that reads the fold.
  constexpr uint32_t kAssertWordTagGuard = 3539u;
  constexpr uint32_t kSelectorScenarioQuery = 2460u;
  // Selectors of the host hooks the node, slot and list scans drive.
  constexpr uint32_t kSelectorScheduleTail = 3542u;
  constexpr uint32_t kSelectorSyncTail = 3543u;
  constexpr uint32_t kSelectorSyncNode = 2717u;
  constexpr uint32_t kSelectorQueryNode = 2591u;
  constexpr uint32_t kSelectorSyncField = 2592u;
  constexpr uint32_t kSelectorQuerySeq = 870u;
  constexpr uint32_t kSelectorQuerySlot = 873u;
  // Generated register slots under short names; `save` is the per-call scratch.
  uint32_t i0 = 0, i1 = 0, i2 = 0, i3 = 0, i4 = 0, i5 = 0, i6 = 0;
  uint64_t j0 = 0, j1 = 0;
  uint32_t tmp = 0, ret = 0, exc = 0, save = 0, frame = 0, flag = 0, idx = 0;
  uint32_t ptr = 0, ptr2 = 0, aux = 0, aux2 = 0, aux3 = 0, str15 = 0, cmp16 = 0, cmp17 = 0;
  uint64_t str64a = 0, str64b = 0;
  uint32_t resume_point = 0;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 80-byte frame the suspend path pushed, then rebuild every slot.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    uint32_t const base = LoadU32(ctx, ReturnFrameSp(ctx));
    p0 = LoadU32(ctx, base);
    p1 = LoadU32(ctx, base + 4u);
    tmp = LoadU32(ctx, base + 8u);
    ret = LoadU32(ctx, base + 12u);
    exc = LoadU32(ctx, base + 16u);
    frame = LoadU32(ctx, base + 20u);
    flag = LoadU32(ctx, base + 24u);
    idx = LoadU32(ctx, base + 28u);
    ptr = LoadU32(ctx, base + 32u);
    ptr2 = LoadU32(ctx, base + 36u);
    aux = LoadU32(ctx, base + 40u);
    aux2 = LoadU32(ctx, base + 44u);
    aux3 = LoadU32(ctx, base + 48u);
    str15 = LoadU32(ctx, base + 52u);
    str64a = LoadU64(ctx, base + 56u);
    cmp16 = LoadU32(ctx, base + 64u);
    str64b = LoadU64(ctx, base + 68u);
    cmp17 = LoadU32(ctx, base + 76u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word naming the call to replay.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    frame = GuestStackSp(ctx) - kGuestFrameBytes;
    GuestStackSp(ctx) = frame;
    tmp = LoadU32(ctx, p0 + 148u);
  }
  // Boot: let the scenario core finish its backlog pass before the tag is read.
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    BacklogView::DispatchAux(ctx, tmp, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Kind 1: the tag's one-byte member, copied in unless the key is already present.
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, p0 + 148u);
    tmp = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
      tmp = LuaTagReadElement(ctx, p1, 1u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto done_kind1; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p1 + 4u);
    flag = static_cast<uint32_t>(tmp == 0u);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 2u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      uint32_t const pending = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (pending != 1u) { goto unreachable_path; }
      goto pending_throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret += 34u;
    LuaTagAppendBytes(ctx, tmp, ret, 1u);
  }
  // Merge point done_kind1.
  done_kind1:;
  // Kind 72: another one-byte member.
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, p0 + 148u);
    tmp = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
      tmp = LuaTagReadElement(ctx, p1, 72u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 3u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto done_kind72; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p1 + 4u);
    flag = static_cast<uint32_t>(tmp == 0u);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 4u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      uint32_t const pending = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (pending != 1u) { goto unreachable_path; }
      goto pending_throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret += 35u;
    LuaTagAppendBytes(ctx, tmp, ret, 1u);
  }
  // Merge point done_kind72.
  done_kind72:;
  // Numeric slot 19 of the scenario sub-table at +36.
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 36u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
    w2c_artemis_f1366(ctx, p1, tmp, 19u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 5u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Kind 2: one more one-byte member.
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, p0 + 148u);
    tmp = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
      tmp = LuaTagReadElement(ctx, p1, 2u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 6u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto done_kind2; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p1 + 4u);
    flag = static_cast<uint32_t>(tmp == 0u);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 7u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 7u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      uint32_t const pending = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (pending != 1u) { goto unreachable_path; }
      goto pending_throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret += 48u;
    LuaTagAppendBytes(ctx, tmp, ret, 1u);
  }
  // Merge point done_kind2.
  done_kind2:;
  // Kind 73: another one-byte member.
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, p0 + 148u);
    tmp = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 8u) {
      tmp = LuaTagReadElement(ctx, p1, 73u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 8u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto done_kind73; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p1 + 4u);
    flag = static_cast<uint32_t>(tmp == 0u);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 9u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 9u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      uint32_t const pending = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (pending != 1u) { goto unreachable_path; }
      goto pending_throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret += 49u;
    LuaTagAppendBytes(ctx, tmp, ret, 1u);
  }
  // Merge point done_kind73.
  done_kind73:;
  // Kind 74: another one-byte member.
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, p0 + 148u);
    tmp = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 10u) {
      tmp = LuaTagReadElement(ctx, p1, 74u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 10u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto done_kind74; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p1 + 4u);
    flag = static_cast<uint32_t>(tmp == 0u);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 11u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 11u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      uint32_t const pending = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (pending != 1u) { goto unreachable_path; }
      goto pending_throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret += 50u;
    LuaTagAppendBytes(ctx, tmp, ret, 1u);
  }
  // Merge point done_kind74.
  done_kind74:;
  // The scenario sub-table at +2720 hands the tag a 36-entry list slot.
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 2720u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 12u) {
    w2c_artemis_f1025(ctx, p1, tmp, 36u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 12u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Kind 71: the wide one-byte member at +2744 of the scenario sub-table.
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, p0 + 148u);
    tmp = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 13u) {
      tmp = LuaTagReadElement(ctx, p1, 71u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 13u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto done_kind71; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p1 + 4u);
    flag = static_cast<uint32_t>(tmp == 0u);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 14u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 14u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      uint32_t const pending = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (pending != 1u) { goto unreachable_path; }
      goto pending_throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret += 2744u;
    LuaTagAppendBytes(ctx, tmp, ret, 1u);
  }
  // Merge point done_kind71.
  done_kind71:;
  // Kind 3 at index 1: the record names a slot whose own handler runs over the tag.
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
    tmp = LoadU32(ctx, p0 + 148u);
  }
  if (ret != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 15u) {
      ret = LuaTagReadElement(ctx, p1, 3u, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 15u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ret != 0u && ResumePhase(ctx) == kResumeRunning) { goto done_kind3; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp += 2748u;
    tmp = LoadU32(ctx, tmp);
    ret = LoadU32(ctx, tmp + 12u);
  }
  // Dispatch the slot's handler through the table; it receives (slot object, tag record).
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 16u) {
    CallTableT0(ctx, ret, tmp, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 16u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU8(ctx, p1 + 12u);
    if (tmp != 0u) { goto done_kind3; }
  }
  // The record carries no live flag, so the temporaries it built are released here.
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 17u) {
    w2c_artemis_f354(ctx, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 17u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Merge point done_kind3.
  done_kind3:;
  // The scenario submodule at +2884 is initialised before the next member.
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 2884u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 18u) {
    w2c_artemis_f2941(ctx, p1, tmp);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 18u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Kind 6: one-byte member at +2935 of the scenario sub-table.
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, p0 + 148u);
    tmp = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 19u) {
      tmp = LuaTagReadElement(ctx, p1, 6u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 19u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto done_kind6; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p1 + 4u);
    flag = static_cast<uint32_t>(tmp == 0u);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 20u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 20u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      uint32_t const pending = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (pending != 1u) { goto unreachable_path; }
      goto pending_throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret += 2935u;
    LuaTagAppendBytes(ctx, tmp, ret, 1u);
  }
  // Merge point done_kind6.
  done_kind6:;
  // The scenario sub-table at +2936 takes the tag's value under selector 51.
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 2936u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 21u) {
    LuaTagAppendValue(ctx, p1, tmp, 51u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 21u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Kind 7: the four-byte member at +2948 of the scenario sub-table.
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, p0 + 148u);
    tmp = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 22u) {
      tmp = LuaTagReadElement(ctx, p1, 7u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 22u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto done_kind7; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p1 + 4u);
    flag = static_cast<uint32_t>(tmp == 0u);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 23u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 23u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      uint32_t const pending = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (pending != 1u) { goto unreachable_path; }
      goto pending_throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret += 2948u;
    LuaTagAppendBytes(ctx, tmp, ret, 4u);
  }
  // Merge point done_kind7.
  done_kind7:;
  // Kind 8: the four-byte member at +2952 of the scenario sub-table.
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, p0 + 148u);
    tmp = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 24u) {
      tmp = LuaTagReadElement(ctx, p1, 8u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 24u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto done_kind8; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p1 + 4u);
    flag = static_cast<uint32_t>(tmp == 0u);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 25u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 25u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      uint32_t const pending = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (pending != 1u) { goto unreachable_path; }
      goto pending_throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret += 2952u;
    LuaTagAppendBytes(ctx, tmp, ret, 4u);
  }
  // Merge point done_kind8.
  done_kind8:;
  // Kind 5: the four-byte member at +2956 of the scenario sub-table.
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, p0 + 148u);
    tmp = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 26u) {
      tmp = LuaTagReadElement(ctx, p1, 5u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 26u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto done_kind5; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p1 + 4u);
    flag = static_cast<uint32_t>(tmp == 0u);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 27u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 27u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      uint32_t const pending = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (pending != 1u) { goto unreachable_path; }
      goto pending_throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret += 2956u;
    LuaTagAppendBytes(ctx, tmp, ret, 4u);
  }
  // Merge point done_kind5.
  done_kind5:;
  // Kind 9: the four-byte member at +13216 of the scenario sub-table.
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, p0 + 148u);
    tmp = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 28u) {
      tmp = LuaTagReadElement(ctx, p1, 9u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 28u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto done_kind9; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p1 + 4u);
    flag = static_cast<uint32_t>(tmp == 0u);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 29u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 29u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      uint32_t const pending = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (pending != 1u) { goto unreachable_path; }
      goto pending_throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret += 13216u;
    LuaTagAppendBytes(ctx, tmp, ret, 4u);
  }
  // Merge point done_kind9.
  done_kind9:;
  // Kind 39: four-byte member of the field at +13220.
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, p0 + 148u);
    tmp = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 30u) {
      tmp = LuaTagReadElement(ctx, p1, 39u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 30u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto done_kind39; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p1 + 4u);
    flag = static_cast<uint32_t>(tmp == 0u);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 31u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 31u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      uint32_t const pending = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (pending != 1u) { goto unreachable_path; }
      goto pending_throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret += 13220u;
    LuaTagAppendBytes(ctx, tmp, ret, 4u);
  }
  // Merge point done_kind39.
  done_kind39:;
  // Kind 57: one-byte member of the field at +13272.
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, p0 + 148u);
    tmp = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 32u) {
      tmp = LuaTagReadElement(ctx, p1, 57u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 32u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto done_kind57; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p1 + 4u);
    flag = static_cast<uint32_t>(tmp == 0u);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 33u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 33u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      uint32_t const pending = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (pending != 1u) { goto unreachable_path; }
      goto pending_throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret += 13272u;
    LuaTagAppendBytes(ctx, tmp, ret, 1u);
  }
  // Merge point done_kind57.
  done_kind57:;
  // Kind 58: one-byte member of the field at +13273.
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, p0 + 148u);
    tmp = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 34u) {
      tmp = LuaTagReadElement(ctx, p1, 58u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 34u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto done_kind58; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p1 + 4u);
    flag = static_cast<uint32_t>(tmp == 0u);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 35u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 35u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      uint32_t const pending = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (pending != 1u) { goto unreachable_path; }
      goto pending_throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret += 13273u;
    LuaTagAppendBytes(ctx, tmp, ret, 1u);
  }
  // Merge point done_kind58.
  done_kind58:;
  // List field at +13276: write index 35.
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 13276u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 36u) {
    w2c_artemis_f1025(ctx, p1, tmp, 35u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 36u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Kind 68: one-byte member of the field at +13288.
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, p0 + 148u);
    tmp = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 37u) {
      tmp = LuaTagReadElement(ctx, p1, 68u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 37u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto done_kind68; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p1 + 4u);
    flag = static_cast<uint32_t>(tmp == 0u);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 38u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 38u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      uint32_t const pending = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (pending != 1u) { goto unreachable_path; }
      goto pending_throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret += 13288u;
    LuaTagAppendBytes(ctx, tmp, ret, 1u);
  }
  // Merge point done_kind68.
  done_kind68:;
  // Property at +13292 takes value 81.
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 13292u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 39u) {
    LuaTagAppendValue(ctx, p1, tmp, 81u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 39u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Kind 82: four-byte member of the field at +13304.
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, p0 + 148u);
    tmp = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 40u) {
      tmp = LuaTagReadElement(ctx, p1, 82u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 40u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto done_kind82; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p1 + 4u);
    flag = static_cast<uint32_t>(tmp == 0u);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 41u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 41u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      uint32_t const pending = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (pending != 1u) { goto unreachable_path; }
      goto pending_throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret += 13304u;
    LuaTagAppendBytes(ctx, tmp, ret, 4u);
  }
  // Merge point done_kind82.
  done_kind82:;
  // Property at +13308 takes value 11.
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 13308u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 42u) {
    LuaTagAppendValue(ctx, p1, tmp, 11u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 42u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Scene submodule at +13320.
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 13320u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 43u) {
    w2c_artemis_f2940(ctx, p1, tmp);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 43u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Chain slot at +13344: follow the tag's link list and dispatch one entry.
  if (ResumePhase(ctx) == kResumeRunning) {
    flag = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
    tmp = LoadU32(ctx, p0 + 148u);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 44u) {
      flag = LuaTagReadElement(ctx, p1, 63u, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 44u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (flag != 0u && ResumePhase(ctx) == kResumeRunning) { goto chain_done63; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp += 13344u;
    ret = LoadU32(ctx, LoadU32(ctx, tmp) + 12u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 45u) {
    CallTableT0(ctx, ret, tmp, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 45u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU8(ctx, p1 + 12u);
    if (tmp != 0u) { goto chain_done63; }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 46u) {
    w2c_artemis_f354(ctx, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 46u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Merge point chain_done63.
  chain_done63:;
  // Slot 13 of the batch at +13536.
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 13536u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 47u) {
    w2c_artemis_f840(ctx, p1, tmp, 13u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 47u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Slot 14 of the batch at +13548.
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 13548u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 48u) {
    w2c_artemis_f840(ctx, p1, tmp, 14u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 48u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Slot 59 of the batch at +13560.
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 13560u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 49u) {
    w2c_artemis_f840(ctx, p1, tmp, 59u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 49u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Slot 60 of the batch at +13572.
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 13572u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 50u) {
    w2c_artemis_f840(ctx, p1, tmp, 60u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 50u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Kind 15: one-byte member of the field at +13612.
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, p0 + 148u);
    tmp = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 51u) {
      tmp = LuaTagReadElement(ctx, p1, 15u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 51u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto done_kind15; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p1 + 4u);
    flag = static_cast<uint32_t>(tmp == 0u);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 52u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 52u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      uint32_t const pending = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (pending != 1u) { goto unreachable_path; }
      goto pending_throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret += 13612u;
    LuaTagAppendBytes(ctx, tmp, ret, 1u);
  }
  // Merge point done_kind15.
  done_kind15:;
  // Numeric slot 16 of the field at +13616.
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 13616u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 53u) {
    w2c_artemis_f1365(ctx, p1, tmp, 16u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 53u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Property at +13628 takes value 62.
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 13628u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 54u) {
    LuaTagAppendValue(ctx, p1, tmp, 62u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 54u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Kind 52: one-byte member of the field at +13640.
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, p0 + 148u);
    tmp = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 55u) {
      tmp = LuaTagReadElement(ctx, p1, 52u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 55u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto done_kind52; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p1 + 4u);
    flag = static_cast<uint32_t>(tmp == 0u);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 56u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 56u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      uint32_t const pending = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (pending != 1u) { goto unreachable_path; }
      goto pending_throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret += 13640u;
    LuaTagAppendBytes(ctx, tmp, ret, 1u);
  }
  // Merge point done_kind52.
  done_kind52:;
  // Kind 61: one-byte member of the field at +13641.
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, p0 + 148u);
    tmp = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 57u) {
      tmp = LuaTagReadElement(ctx, p1, 61u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 57u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto done_kind61; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p1 + 4u);
    flag = static_cast<uint32_t>(tmp == 0u);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 58u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 58u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      uint32_t const pending = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (pending != 1u) { goto unreachable_path; }
      goto pending_throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret += 13641u;
    LuaTagAppendBytes(ctx, tmp, ret, 1u);
  }
  // Merge point done_kind61.
  done_kind61:;
  // Kind 17: one-byte member of the field at +13642.
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, p0 + 148u);
    tmp = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 59u) {
      tmp = LuaTagReadElement(ctx, p1, 17u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 59u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto done_kind17; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p1 + 4u);
    flag = static_cast<uint32_t>(tmp == 0u);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 60u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 60u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      uint32_t const pending = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (pending != 1u) { goto unreachable_path; }
      goto pending_throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret += 13642u;
    LuaTagAppendBytes(ctx, tmp, ret, 1u);
  }
  // Merge point done_kind17.
  done_kind17:;
  // Numeric slot 37 of the field at +13644.
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 13644u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 61u) {
    w2c_artemis_f1365(ctx, p1, tmp, 37u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 61u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Scalar slot 20 of the field at +13656.
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 13656u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 62u) {
    w2c_artemis_f1366(ctx, p1, tmp, 20u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 62u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Scene submodule at +13668.
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 13668u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 63u) {
    w2c_artemis_f2939(ctx, p1, tmp);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 63u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Kind 79: four-byte member of the field at +13728.
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, p0 + 148u);
    tmp = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 64u) {
      tmp = LuaTagReadElement(ctx, p1, 79u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 64u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto done_kind79; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p1 + 4u);
    flag = static_cast<uint32_t>(tmp == 0u);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 65u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 65u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      uint32_t const pending = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (pending != 1u) { goto unreachable_path; }
      goto pending_throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret += 13728u;
    LuaTagAppendBytes(ctx, tmp, ret, 4u);
  }
  // Merge point done_kind79.
  done_kind79:;
  // Text slot 56 of the field at +13740.
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 13740u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 66u) {
    w2c_artemis_f1024(ctx, p1, tmp, 56u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 66u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Text slot 78 of the field at +13752.
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 13752u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 67u) {
    w2c_artemis_f1024(ctx, p1, tmp, 78u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 67u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Chain slot at +13764: follow the tag's link list and dispatch one entry.
  if (ResumePhase(ctx) == kResumeRunning) {
    flag = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
    tmp = LoadU32(ctx, p0 + 148u);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 68u) {
      flag = LuaTagReadElement(ctx, p1, 40u, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 68u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (flag != 0u && ResumePhase(ctx) == kResumeRunning) { goto chain_done40; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp += 13764u;
    ret = LoadU32(ctx, LoadU32(ctx, tmp) + 12u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 69u) {
    CallTableT0(ctx, ret, tmp, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 69u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU8(ctx, p1 + 12u);
    if (tmp != 0u) {
      exc = LoadU32(ctx, p0 + 148u) + 13808u;
      goto dispatch_13808;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 70u) {
    w2c_artemis_f354(ctx, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 70u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Merge point chain_done40.
  chain_done40:;
  // Entry slot at +13808.
  if (ResumePhase(ctx) == kResumeRunning) {
    exc = LoadU32(ctx, p0 + 148u) + 13808u;
    tmp = LoadU8(ctx, p1 + 12u);
    if (tmp != 0u) { goto dispatch_13808; }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 71u) {
    tmp = LuaTagReadElement(ctx, p1, 41u, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 71u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto skip_dispatch41; }
  // Merge point dispatch_13808.
  dispatch_13808:;
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, LoadU32(ctx, exc) + 12u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 72u) {
    CallTableT0(ctx, tmp, exc, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 72u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU8(ctx, p1 + 12u);
    if (tmp != 0u) {
      exc = LoadU32(ctx, p0 + 148u) + 13852u;
      goto dispatch_13852;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 73u) {
    w2c_artemis_f354(ctx, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 73u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Merge point skip_dispatch41.
  skip_dispatch41:;
  // Entry slot at +13852.
  if (ResumePhase(ctx) == kResumeRunning) {
    exc = LoadU32(ctx, p0 + 148u) + 13852u;
    tmp = LoadU8(ctx, p1 + 12u);
    if (tmp != 0u) { goto dispatch_13852; }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 74u) {
    tmp = LuaTagReadElement(ctx, p1, 42u, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 74u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto skip_dispatch42; }
  // Merge point dispatch_13852.
  dispatch_13852:;
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, LoadU32(ctx, exc) + 12u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 75u) {
    CallTableT0(ctx, tmp, exc, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 75u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU8(ctx, p1 + 12u);
    if (tmp != 0u) {
      exc = LoadU32(ctx, p0 + 148u) + 13896u;
      goto dispatch_13896;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 76u) {
    w2c_artemis_f354(ctx, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 76u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Merge point skip_dispatch42.
  skip_dispatch42:;
  // Entry slot at +13896.
  if (ResumePhase(ctx) == kResumeRunning) {
    exc = LoadU32(ctx, p0 + 148u) + 13896u;
    tmp = LoadU8(ctx, p1 + 12u);
    if (tmp != 0u) { goto dispatch_13896; }
  }
  // Scenario key slots +13896 through +14204: each slot dispatches its virtual
  // entry when the tag does not already carry the matching kind, then chains on.
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 77u) {
    tmp = LuaTagReadElement(ctx, p1, 43u, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 77u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto setup_13940; }
dispatch_13896:;
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, LoadU32(ctx, exc) + 12u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 78u) {
    CallTableT0(ctx, tmp, exc, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 78u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU8(ctx, p1 + 12u);
    if (tmp != 0u) {
      exc = LoadU32(ctx, p0 + 148u) + 13940u;
      goto dispatch_13940;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 79u) {
    w2c_artemis_f354(ctx, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 79u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
setup_13940:;
  if (ResumePhase(ctx) == kResumeRunning) {
    exc = LoadU32(ctx, p0 + 148u) + 13940u;
    tmp = LoadU8(ctx, p1 + 12u);
    if (tmp != 0u) { goto dispatch_13940; }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 80u) {
    tmp = LuaTagReadElement(ctx, p1, 44u, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 80u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto setup_13984; }
dispatch_13940:;
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, LoadU32(ctx, exc) + 12u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 81u) {
    CallTableT0(ctx, tmp, exc, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 81u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU8(ctx, p1 + 12u);
    if (tmp != 0u) {
      exc = LoadU32(ctx, p0 + 148u) + 13984u;
      goto dispatch_13984;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 82u) {
    w2c_artemis_f354(ctx, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 82u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
setup_13984:;
  if (ResumePhase(ctx) == kResumeRunning) {
    exc = LoadU32(ctx, p0 + 148u) + 13984u;
    tmp = LoadU8(ctx, p1 + 12u);
    if (tmp != 0u) { goto dispatch_13984; }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 83u) {
    tmp = LuaTagReadElement(ctx, p1, 45u, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 83u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto setup_14028; }
dispatch_13984:;
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, LoadU32(ctx, exc) + 12u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 84u) {
    CallTableT0(ctx, tmp, exc, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 84u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU8(ctx, p1 + 12u);
    if (tmp != 0u) {
      exc = LoadU32(ctx, p0 + 148u) + 14028u;
      goto dispatch_14028;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 85u) {
    w2c_artemis_f354(ctx, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 85u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
setup_14028:;
  if (ResumePhase(ctx) == kResumeRunning) {
    exc = LoadU32(ctx, p0 + 148u) + 14028u;
    tmp = LoadU8(ctx, p1 + 12u);
    if (tmp != 0u) { goto dispatch_14028; }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 86u) {
    tmp = LuaTagReadElement(ctx, p1, 46u, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 86u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto setup_14072; }
dispatch_14028:;
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, LoadU32(ctx, exc) + 12u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 87u) {
    CallTableT0(ctx, tmp, exc, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 87u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU8(ctx, p1 + 12u);
    if (tmp != 0u) {
      exc = LoadU32(ctx, p0 + 148u) + 14072u;
      goto dispatch_14072;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 88u) {
    w2c_artemis_f354(ctx, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 88u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
setup_14072:;
  if (ResumePhase(ctx) == kResumeRunning) {
    exc = LoadU32(ctx, p0 + 148u) + 14072u;
    tmp = LoadU8(ctx, p1 + 12u);
    if (tmp != 0u) { goto dispatch_14072; }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 89u) {
    tmp = LuaTagReadElement(ctx, p1, 47u, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 89u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto setup_14116; }
dispatch_14072:;
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, LoadU32(ctx, exc) + 12u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 90u) {
    CallTableT0(ctx, tmp, exc, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 90u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU8(ctx, p1 + 12u);
    if (tmp != 0u) {
      exc = LoadU32(ctx, p0 + 148u) + 14116u;
      goto dispatch_14116;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 91u) {
    w2c_artemis_f354(ctx, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 91u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
setup_14116:;
  if (ResumePhase(ctx) == kResumeRunning) {
    exc = LoadU32(ctx, p0 + 148u) + 14116u;
    tmp = LoadU8(ctx, p1 + 12u);
    if (tmp != 0u) { goto dispatch_14116; }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 92u) {
    tmp = LuaTagReadElement(ctx, p1, 48u, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 92u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto setup_14160; }
dispatch_14116:;
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, LoadU32(ctx, exc) + 12u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 93u) {
    CallTableT0(ctx, tmp, exc, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 93u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU8(ctx, p1 + 12u);
    if (tmp != 0u) {
      exc = LoadU32(ctx, p0 + 148u) + 14160u;
      goto dispatch_14160;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 94u) {
    w2c_artemis_f354(ctx, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 94u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
setup_14160:;
  if (ResumePhase(ctx) == kResumeRunning) {
    exc = LoadU32(ctx, p0 + 148u) + 14160u;
    tmp = LoadU8(ctx, p1 + 12u);
    if (tmp != 0u) { goto dispatch_14160; }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 95u) {
    tmp = LuaTagReadElement(ctx, p1, 49u, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 95u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto setup_14204; }
dispatch_14160:;
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, LoadU32(ctx, exc) + 12u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 96u) {
    CallTableT0(ctx, tmp, exc, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 96u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU8(ctx, p1 + 12u);
    if (tmp != 0u) {
      tmp = LoadU32(ctx, p0 + 148u) + 14204u;
      goto setup_14204;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 97u) {
    w2c_artemis_f354(ctx, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 97u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
setup_14204:;
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 14204u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 98u) {
    w2c_artemis_f1024(ctx, p1, tmp, 83u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 98u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Slot +14204 keeps its own virtual entry one word further along.
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
    tmp = LoadU32(ctx, p0 + 148u);
  }
  if (ret != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 99u) {
      ret = LuaTagReadElement(ctx, p1, 77u, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 99u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ret != 0u && ResumePhase(ctx) == kResumeRunning) { goto setup_14260; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp += 14216u;
    ret = LoadU32(ctx, LoadU32(ctx, tmp) + 12u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 100u) {
    CallTableT0(ctx, ret, tmp, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 100u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU8(ctx, p1 + 12u);
    if (tmp != 0u) {
      exc = LoadU32(ctx, p0 + 148u) + 14260u;
      goto dispatch_14260;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 101u) {
    w2c_artemis_f354(ctx, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 101u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
setup_14260:;
  if (ResumePhase(ctx) == kResumeRunning) {
    exc = LoadU32(ctx, p0 + 148u) + 14260u;
    tmp = LoadU8(ctx, p1 + 12u);
    if (tmp != 0u) { goto dispatch_14260; }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 102u) {
    tmp = LuaTagReadElement(ctx, p1, 33u, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 102u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto setup_14356; }
dispatch_14260:;
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, LoadU32(ctx, exc) + 12u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 103u) {
    CallTableT0(ctx, tmp, exc, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 103u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU8(ctx, p1 + 12u);
    if (tmp != 0u) { goto setup_14356; }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 104u) {
    w2c_artemis_f354(ctx, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 104u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
setup_14356:;
  // Fixed members of the scenario key subobject written by kind code.
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 14356u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 105u) {
    LuaTagAppendValue(ctx, p1, tmp, 34u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 105u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 14368u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 106u) {
    LuaTagAppendValue(ctx, p1, tmp, 38u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 106u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 14380u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 107u) {
    LuaTagAppendValue(ctx, p1, tmp, 50u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 107u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 14392u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 108u) {
    LuaTagAppendValue(ctx, p1, tmp, 69u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 108u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 14404u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 109u) {
    LuaTagAppendValue(ctx, p1, tmp, 70u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 109u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 14416u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 110u) {
    LuaTagAppendValue(ctx, p1, tmp, 84u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 110u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 14428u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 111u) {
    LuaTagAppendValue(ctx, p1, tmp, 80u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 111u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Members taken straight from the scenario core object; the tag walks them.
  tmp = (ResumePhase(ctx) != kResumeRunning) ? tmp : p0 + 4u;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 112u) {
    w2c_artemis_f1364(ctx, p1, tmp, 24u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 112u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  idx = (ResumePhase(ctx) != kResumeRunning) ? idx : p0 + 16u;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 113u) {
    w2c_artemis_f2938(ctx, p1, idx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 113u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  tmp = (ResumePhase(ctx) != kResumeRunning) ? tmp : p0 + 28u;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 114u) {
    w2c_artemis_f736(ctx, p1, tmp, 66u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 114u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  tmp = (ResumePhase(ctx) != kResumeRunning) ? tmp : p0 + 40u;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 115u) {
    w2c_artemis_f736(ctx, p1, tmp, 67u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 115u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  tmp = (ResumePhase(ctx) != kResumeRunning) ? tmp : p0 + 52u;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 116u) {
    w2c_artemis_f736(ctx, p1, tmp, 75u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 116u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  tmp = (ResumePhase(ctx) != kResumeRunning) ? tmp : p0 + 64u;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 117u) {
    w2c_artemis_f736(ctx, p1, tmp, 76u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 117u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  tmp = (ResumePhase(ctx) != kResumeRunning) ? tmp : p0 + 76u;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 118u) {
    w2c_artemis_f1364(ctx, p1, tmp, 26u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 118u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  tmp = (ResumePhase(ctx) != kResumeRunning) ? tmp : p0 + 88u;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 119u) {
    w2c_artemis_f736(ctx, p1, tmp, 53u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 119u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Kind 28 copies one byte out of the scenario key string into the tag.
  if (ResumePhase(ctx) == kResumeRunning) {
    str15 = p0 + 100u;
    tmp = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 120u) {
      tmp = LuaTagReadElement(ctx, p1, 28u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 120u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto copy_done; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p1 + 4u);
    ret = static_cast<uint32_t>(tmp == 0u);
  }
  if (ret != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 121u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 121u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (tmp != 1u) { goto unreachable_path; }
      goto pending_throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    LuaTagAppendBytes(ctx, tmp, str15, 1u);
  }
copy_done:;
  // Kind 29 dispatches the slot held at scenario core +104.
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = static_cast<uint32_t>(LoadU8(ctx, str15) == 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (tmp != 0u) { goto done_kind29; }
    tmp = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 122u) {
      tmp = LuaTagReadElement(ctx, p1, 29u, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 122u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto done_kind29; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, LoadU32(ctx, p0 + 104u) + 12u);
    tmp = p0 + 104u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 123u) {
    CallTableT0(ctx, ret, tmp, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 123u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU8(ctx, p1 + 12u);
    if (tmp != 0u) { goto done_kind29; }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 124u) {
    w2c_artemis_f354(ctx, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 124u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
done_kind29:;
  // A scratch string buffer is allocated for the layout fold that follows.
  if (ResumePhase(ctx) == kResumeRunning) {
    flag = LoadU32(ctx, p0 + 148u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 125u) {
    ptr2 = AllocateStringBuffer(ctx, 48u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 125u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 126u) {
    ret = HostFoldLayoutOffset(ctx, 2457u, ptr2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 126u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    tmp = static_cast<uint32_t>(tmp != 1u);
  }
  // The +104 slot: release the shared pointer, dispatch the key 65 and key 30 virtual
  // entries, register the layout fold, then walk the scenario's property table.
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    tmp = (ResumePhase(ctx) != kResumeRunning) ? tmp : flag + 76u;
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 127u) {
      ResetSharedPtr03(ctx, tmp, ret);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 127u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, LoadU32(ctx, p0 + 148u) + 76u);
      ret = static_cast<uint32_t>(tmp == 0u);
    }
    if (ret != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 128u) {
        HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLineStar,
                          kMsgOperatorStar);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_point = 128u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        tmp = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (tmp != 1u) { goto unreachable_path; }
        goto pending_throw_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      ret = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
    }
    if (ret != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 129u) {
        ret = LuaTagReadElement(ctx, p1, 65u, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_point = 129u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ret != 0u && ResumePhase(ctx) == kResumeRunning) { goto done_kind65; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      ret = LoadU32(ctx, LoadU32(ctx, tmp) + 12u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 130u) {
      CallTableT0(ctx, ret, tmp, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 130u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU8(ctx, p1 + 12u);
      if (tmp != 0u) { goto done_kind65; }
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 131u) {
      w2c_artemis_f354(ctx, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 131u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
  done_kind65:;
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, LoadU32(ctx, p0 + 148u) + 112u);
      ret = static_cast<uint32_t>(tmp == 0u);
    }
    if (ret != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 132u) {
        HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLineStar,
                          kMsgOperatorStar);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_point = 132u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        tmp = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (tmp != 1u) { goto unreachable_path; }
        goto pending_throw_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      ret = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
    }
    if (ret != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 133u) {
        ret = LuaTagReadElement(ctx, p1, 30u, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_point = 133u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ret != 0u && ResumePhase(ctx) == kResumeRunning) { goto done_kind30; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      ret = LoadU32(ctx, LoadU32(ctx, tmp) + 36u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 134u) {
      CallTableT0(ctx, ret, tmp, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 134u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU8(ctx, p1 + 12u);
      if (tmp != 0u) { goto done_kind30; }
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 135u) {
      w2c_artemis_f354(ctx, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 135u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
  done_kind30:;
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, frame + 464u, 0u);
      j1 = 0ull;
      StoreU64(ctx, frame + 456u, j1);
      StoreU32(ctx, kDataD015C, 0u);
      tmp = frame + 456u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 136u) {
      HostFailAssertion(ctx, kAssertWordTagGuard, p1, tmp, 64u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 136u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (tmp == 1u) { goto exception_cleanup_a; }
      exc = LoadU32(ctx, p0 + 148u);
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 137u) {
      flag = HostFoldLayoutOffset(ctx, 1032u, 640u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 137u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (tmp == 1u) { goto exception_cleanup_a; }
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 138u) {
      ret = HostOsQuery(ctx, kSelectorScenarioQuery, flag, exc);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 138u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (tmp == 1u) { goto exception_cleanup_b; }
      StoreU32(ctx, kDataD015C, 0u);
      tmp = exc + 120u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 139u) {
      HostOsSchedule(ctx, kSelectorNotify, tmp, ret);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 139u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (tmp == 1u) { goto exception_cleanup_a; }
      flag = LoadU32(ctx, LoadU32(ctx, p0 + 148u) + 120u);
      tmp = static_cast<uint32_t>(flag == 0u);
    }
    if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 140u) {
        HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLineStar,
                          kMsgOperatorStar);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_point = 140u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        tmp = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (tmp != 1u) { goto unreachable_path; }
        goto pending_throw_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      ret = static_cast<uint32_t>(LoadU8(ctx, p1 + 12u) == 0u);
    }
    if (ret != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 141u) {
        ret = HostDispatchInvoke(ctx, kSelectorDispatch, p1, 31u, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_point = 141u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        tmp = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (tmp == 1u) { goto exception_cleanup_a; }
        if (ret != 0u) { goto dispatch_exit; }
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, LoadU32(ctx, flag) + 320u);
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 142u) {
      HostOsSchedule(ctx, tmp, flag, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 142u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (tmp == 1u) { goto exception_cleanup_a; }
      tmp = LoadU8(ctx, p1 + 12u);
      if (tmp != 0u) { goto dispatch_exit; }
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 143u) {
      HostReleaseSlotPair(ctx, kSlotPairBase, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 143u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p1 = static_cast<uint32_t>(p1 == 1u);
      if (p1 != 0u) { goto exception_cleanup_a; }
    }
  dispatch_exit:;
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, p0 + 148u);
      exc = LoadU32(ctx, tmp + 2888u);
      aux2 = LoadU32(ctx, tmp + 2900u);
      flag = aux2 / 30u;
      ret = exc + (flag << 2);
      p1 = 0u;
      ptr = LoadU32(ctx, tmp + 2892u);
      aux = static_cast<uint32_t>(ptr != exc);
      if (aux != 0u) {
        flag = (aux2 - flag * 30u) * 136u;
        p1 = flag + LoadU32(ctx, ret);
      } else {
        p1 = 0u;
      }
    }
  // Merge point outer_loop.
  outer_loop:;
    if (ResumePhase(ctx) == kResumeRunning) {
      if (exc == ptr) {
        flag = static_cast<uint32_t>(0u == p1);
      } else {
        ptr2 = aux2 + LoadU32(ctx, tmp + 2904u);
        flag = ptr2 / 30u;
        aux = flag << 2;
        exc = LoadU32(ctx, aux + exc);
        flag = static_cast<uint32_t>((exc + (ptr2 - flag * 30u) * 136u) == p1);
      }
    }
    if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        ret = tmp + 2752u;
        p1 = 0u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 144u) {
        tmp = HostOsQuery(ctx, kSelectorReadTagArg, tmp, ret);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_point = 144u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        tmp = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (tmp != 1u) { goto inner_loop_entry; }
        goto exception_cleanup_a;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      flag = p1 + 4u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 145u) {
      tmp = HostOsQuery(ctx, kSelectorReadTagArg, tmp, flag);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 145u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (tmp != 1u) {
        p1 = p1 + 136u - LoadU32(ctx, ret);
        if (p1 == 4080u) {
          p1 = LoadU32(ctx, ret + 4u);
          ret = ret + 4u;
        }
        tmp = LoadU32(ctx, p0 + 148u);
        ptr = LoadU32(ctx, tmp + 2892u);
        exc = LoadU32(ctx, tmp + 2888u);
        flag = tmp + 2900u;
        aux2 = LoadU32(ctx, flag);
        goto outer_loop;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      exc = HostRet0(ctx);
      p0 = HostTempRet0(ctx);
      goto exception_reload;
    }
  // Merge point inner_loop_entry.
  inner_loop_entry:;
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, p0 + 148u);
      exc = LoadU32(ctx, tmp + 2888u);
      flag = LoadU32(ctx, tmp + 2900u);
      ret = flag / 30u;
      aux2 = exc + (ret << 2);
      ptr = LoadU32(ctx, tmp + 2892u);
      aux = static_cast<uint32_t>(ptr != exc);
      if (aux != 0u) {
        ret = (flag - ret * 30u) * 136u;
        p1 = ret + LoadU32(ctx, aux2);
      }
    }
  // Merge point inner_loop.
  inner_loop:;
    if (ResumePhase(ctx) == kResumeRunning) {
      if (exc != ptr) {
        flag = flag + LoadU32(ctx, tmp + 2904u);
        ret = flag / 30u;
        aux = ret << 2;
        exc = LoadU32(ctx, aux + exc);
        flag = static_cast<uint32_t>((exc + (flag - ret * 30u) * 136u) == p1);
      } else {
        flag = static_cast<uint32_t>(0u == p1);
      }
    }
    if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        p1 = static_cast<uint32_t>(static_cast<int32_t>(LoadU32(ctx, tmp + 2872u)) <= 0);
        if (p1 != 0u) { goto skip_kind_dec; }
        ret = LoadU32(ctx, p0 + 148u);
        p1 = tmp + 2752u;
        StoreU32(ctx, frame + 280u, p1);
        StoreU32(ctx, kDataD015C, 0u);
        ret = frame + 344u;
        flag = frame + 280u;
        exc = frame + 488u;
        tmp = tmp + 2684u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 146u) {
        HostInvokeViiiiii(ctx, kHookTagArgs, ret, tmp, p1, kMsgPxRef, flag, exc);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_point = 146u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p1 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (p1 == 1u) { goto exception_cleanup_c; }
        ret = LoadU32(ctx, p0 + 148u);
        tmp = LoadU32(ctx, ret + 2764u);
        ptr2 = LoadU32(ctx, frame + 344u);
        p1 = LoadU32(ctx, ptr2 + 28u);
        flag = static_cast<uint32_t>(static_cast<int32_t>(tmp) <
            static_cast<int32_t>(static_cast<uint32_t>(static_cast<int32_t>(
                LoadU32(ctx, ptr2 + 32u) - p1) >> 2)));
        if (flag != 0u) { goto skip_kind_pair; }
        p1 = ret + 2752u;
        flag = frame;
        exc = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, ret + 2763u)) <= -1);
        StoreU32(ctx, frame + 32u, exc != 0u ? LoadU32(ctx, p1) : p1);
        StoreU32(ctx, kDataD015C, 0u);
        StoreU32(ctx, frame + 36u, tmp);
        p1 = frame + 32u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 147u) {
        HostFailAssertion(ctx, kAssertWordLua, ret, 1u, kMsgSelfModifyFail, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_point = 147u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p1 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p1 = static_cast<uint32_t>(p1 != 1u);
        if (p1 != 0u) { goto skip_kind_dec; }
        goto exception_cleanup_c;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      ret = static_cast<uint32_t>(static_cast<int32_t>(LoadU32(ctx, p1 + 124u)) < 1);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (ret != 0u) { goto skip_kind_anim; }
      ptr2 = p1 + 4u;
      StoreU32(ctx, frame + 280u, ptr2);
      StoreU32(ctx, kDataD015C, 0u);
      ret = frame + 344u;
      flag = frame + 280u;
      exc = frame + 488u;
      tmp = tmp + 2684u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 148u) {
      HostInvokeViiiiii(ctx, kHookTagArgs, ret, tmp, ptr2, kMsgPxRef, flag, exc);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 148u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (tmp == 1u) { goto exception_cleanup_d; }
      ret = LoadU32(ctx, p1 + 16u);
      aux3 = LoadU32(ctx, frame + 344u);
      tmp = LoadU32(ctx, aux3 + 28u);
      flag = static_cast<uint32_t>(static_cast<int32_t>(ret) >=
          static_cast<int32_t>(static_cast<uint32_t>(static_cast<int32_t>(
              LoadU32(ctx, aux3 + 32u) - tmp) >> 2)));
    }
    if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        tmp = LoadU32(ctx, p0 + 148u);
        exc = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, ptr2 + 11u)) <= -1);
        if (exc != 0u) { ptr2 = LoadU32(ctx, ptr2); }
        flag = frame;
        StoreU32(ctx, frame + 80u, ptr2);
        StoreU32(ctx, kDataD015C, 0u);
        StoreU32(ctx, frame + 84u, ret);
        ret = frame + 80u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 149u) {
        HostFailAssertion(ctx, kAssertWordLua, tmp, 1u, kMsgSelfModifyFail, ret);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_point = 149u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        tmp = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        tmp = static_cast<uint32_t>(tmp != 1u);
        if (tmp != 0u) { goto skip_kind_anim; }
        goto exception_cleanup_d;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, (ret << 2) + tmp);
      StoreU32(ctx, kDataD015C, 0u);
      ptr2 = p1 + 84u;
      ret = w2c_artemis_f735(ctx, ptr2, tmp);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      tmp = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (tmp == 1u) { goto exception_cleanup_d; }
      if (ret != 0u) { goto skip_kind_anim; }
      ptr = LoadU32(ctx, aux3 + 32u);
      ret = ptr;
      exc = LoadU32(ctx, aux3 + 28u);
      flag = static_cast<uint32_t>(ptr == exc);
      tmp = 0u;
    }
    if (flag != 0u && ResumePhase(ctx) == kResumeRunning) { goto inner_done; }
  // Merge point inner_scan.
  inner_scan:;
    if (ResumePhase(ctx) == kResumeRunning) {
      ret = LoadU32(ctx, exc + (tmp << 2));
      StoreU32(ctx, kDataD015C, 0u);
      flag = w2c_artemis_f735(ctx, ret, ptr2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      ret = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      ret = static_cast<uint32_t>(ret != 1u);
    }
    if (ret != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        ret = static_cast<uint32_t>(flag == 0u);
        if (ret != 0u) { goto inner_skip; }
        flag = LoadU32(ctx, p0 + 148u);
        ret = LoadU32(ctx, p1 + 16u);
        ptr = frame;
        if (static_cast<int32_t>(LoadS8(ctx, ptr2 + 11u)) <= -1) {
          exc = LoadU32(ctx, ptr2);
          goto byte_seq_merge;
        }
        exc = ptr2;
      // Merge point byte_seq_merge.
      byte_seq_merge:;
        StoreU32(ctx, ptr + 64u, exc);
        StoreU32(ctx, kDataD015C, 0u);
        StoreU32(ctx, frame + 68u, ret);
        StoreU32(ctx, frame + 72u, tmp);
        ret = frame + 64u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 150u) {
        HostFailAssertion(ctx, kAssertWordLua, flag, 1u, kMsgSelfModified, ret);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_point = 150u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        ret = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (ret == 1u) { goto exception_cleanup_e; }
        StoreU32(ctx, p1 + 16u, tmp);
        ptr = LoadU32(ctx, aux3 + 28u);
        ret = LoadU32(ctx, aux3 + 32u);
        goto inner_done;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      exc = HostRet0(ctx);
      p0 = HostTempRet0(ctx);
      goto exception_reload;
    }
  // Merge point inner_skip.
  inner_skip:;
    if (ResumePhase(ctx) == kResumeRunning) {
      exc = LoadU32(ctx, aux3 + 28u);
      ptr = exc;
      tmp = tmp + 1u;
      ret = LoadU32(ctx, aux3 + 32u);
      flag = static_cast<uint32_t>(tmp < static_cast<uint32_t>(
          static_cast<int32_t>(ret - exc) >> 2));
      if (flag != 0u) { goto inner_scan; }
    }
  // Merge point inner_done.
  inner_done:;
    if (ResumePhase(ctx) == kResumeRunning) {
      ret = static_cast<uint32_t>(static_cast<int32_t>(ret - ptr) >> 2);
      flag = static_cast<uint32_t>(ret > tmp);
      if (flag != 0u) { goto skip_kind_anim; }
      ret = LoadU32(ctx, p0 + 148u);
      tmp = LoadU32(ctx, p1 + 16u);
      exc = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, aux + 11u)) <= -1);
      if (exc != 0u) { aux = LoadU32(ctx, aux); }
      StoreU32(ctx, frame + 48u, aux);
      StoreU32(ctx, kDataD015C, 0u);
      StoreU32(ctx, frame + 52u, tmp);
      tmp = frame + 48u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 151u) {
      HostFailAssertion(ctx, kAssertWordLua, ret, 1u, kMsgSelfModifyFail, tmp);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 151u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (tmp == 1u) { goto exception_cleanup_e; }
    }
  // Merge point skip_kind_anim.
  skip_kind_anim:;
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = p1 + 136u;
      p1 = p1 - LoadU32(ctx, aux2);
      if (p1 == 4080u) {
        p1 = LoadU32(ctx, aux2 + 4u);
        aux2 = aux2 + 4u;
      }
      tmp = LoadU32(ctx, p0 + 148u);
      ptr = LoadU32(ctx, tmp + 2892u);
      exc = LoadU32(ctx, tmp + 2888u);
      ret = tmp + 2900u;
      flag = LoadU32(ctx, ret);
      goto inner_loop;
    }
  // Merge point skip_kind_pair.
  skip_kind_pair:;
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, (tmp << 2) + p1);
      StoreU32(ctx, kDataD015C, 0u);
      tmp = w2c_artemis_f735(ctx, ret + 2832u, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p1 = static_cast<uint32_t>(p1 == 1u);
      if (p1 != 0u) { goto exception_cleanup_c; }
      if (tmp != 0u) { goto skip_kind_dec; }
      ret = LoadU32(ctx, ptr2 + 32u);
      exc = LoadU32(ctx, ptr2 + 28u);
      tmp = static_cast<uint32_t>(ret == exc);
      p1 = 0u;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (tmp != 0u) {
        exc = ret;
        goto pair_done;
      }
    }
  // Merge point pair_scan.
  pair_scan:;
    if (ResumePhase(ctx) == kResumeRunning) {
      ret = LoadU32(ctx, exc + (p1 << 2));
      tmp = LoadU32(ctx, p0 + 148u);
      StoreU32(ctx, kDataD015C, 0u);
      ret = w2c_artemis_f735(ctx, ret, tmp + 2832u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      tmp = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      tmp = static_cast<uint32_t>(tmp != 1u);
    }
    if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        tmp = static_cast<uint32_t>(ret == 0u);
        if (tmp != 0u) { goto pair_next; }
        flag = LoadU32(ctx, p0 + 148u);
        ret = flag + 2752u;
        tmp = LoadU32(ctx, flag + 2764u);
        aux = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, flag + 2763u)) <= -1);
        if (aux != 0u) { ret = LoadU32(ctx, ret); }
        exc = frame;
        StoreU32(ctx, frame + 16u, ret);
        StoreU32(ctx, kDataD015C, 0u);
        StoreU32(ctx, frame + 20u, tmp);
        StoreU32(ctx, frame + 24u, p1);
        tmp = frame + 16u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 152u) {
        HostFailAssertion(ctx, kAssertWordLua, flag, 1u, kMsgSelfModified, tmp);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_point = 152u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        tmp = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (tmp == 1u) { goto exception_cleanup_f; }
        StoreU32(ctx, LoadU32(ctx, p0 + 148u) + 2764u, p1);
        exc = LoadU32(ctx, ptr2 + 28u);
        ret = LoadU32(ctx, ptr2 + 32u);
        goto pair_done;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      exc = HostRet0(ctx);
      p0 = HostTempRet0(ctx);
      goto exception_reload;
    }
  // Merge point pair_next.
  pair_next:;
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = p1 + 1u;
      ret = LoadU32(ctx, ptr2 + 32u);
      exc = LoadU32(ctx, ptr2 + 28u);
      tmp = static_cast<uint32_t>(p1 < static_cast<uint32_t>(
          static_cast<int32_t>(ret - exc) >> 2));
      if (tmp != 0u) { goto pair_scan; }
    }
  // Merge point pair_done.
  pair_done:;
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = static_cast<uint32_t>(static_cast<int32_t>(ret - exc) >> 2);
      p1 = static_cast<uint32_t>(tmp > p1);
      if (p1 != 0u) { goto skip_kind_dec; }
      ret = LoadU32(ctx, p0 + 148u);
      tmp = ret + 2752u;
      p1 = LoadU32(ctx, ret + 2764u);
      exc = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, ret + 2763u)) <= -1);
      if (exc != 0u) { tmp = LoadU32(ctx, tmp); }
      flag = frame;
      StoreU32(ctx, frame, tmp);
      StoreU32(ctx, kDataD015C, 0u);
      StoreU32(ctx, frame + 4u, p1);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 153u) {
      HostFailAssertion(ctx, kAssertWordLua, ret, 1u, kMsgSelfModifyFail, frame);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 153u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p1 == 1u) { goto exception_cleanup_f; }
    }
  // Merge point skip_kind_dec.
  skip_kind_dec:;
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, p0 + 148u);
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 154u) {
      HostOsSchedule(ctx, kSelectorScheduleTail, p1, 13u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 154u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p1 == 1u) { goto exception_cleanup_a; }
      p1 = LoadU32(ctx, frame + 456u);
      tmp = static_cast<uint32_t>(p1 == LoadU32(ctx, frame + 460u));
      if (tmp != 0u) { goto exception_cleanup_inner; }
    }
  // Merge point props_scan.
  props_scan:;
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, LoadU32(ctx, p0 + 148u) + 16u);
      ret = static_cast<uint32_t>(tmp == 0u);
    }
    if (ret != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 155u) {
        HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_point = 155u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p0 = static_cast<uint32_t>(p0 != 1u);
        if (p0 != 0u) { goto unreachable_path; }
        goto pending_throw_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 156u) {
      HostSyncSlot(ctx, kSelectorSyncNode, tmp, p1, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 156u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      tmp = static_cast<uint32_t>(tmp != 1u);
      if (tmp != 0u) {
        p1 = p1 + 12u;
        tmp = static_cast<uint32_t>(p1 != LoadU32(ctx, frame + 460u));
        if (tmp != 0u) { goto props_scan; }
        goto exception_cleanup_inner;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      exc = HostRet0(ctx);
      p0 = HostTempRet0(ctx);
      goto exception_reload;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    exc = HostRet0(ctx);
    p0 = HostTempRet0(ctx);
    DeallocateRawBlock(ctx, ptr2);
    goto exception_release;
  }
// Merge point exception_cleanup_inner.
exception_cleanup_inner:;
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 148u) + 13512u;
    p1 = static_cast<uint32_t>(idx != tmp);
  }
  if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, p0 + 16u);
      StoreU32(ctx, kDataD015C, 0u);
      ret = p0 + 20u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 157u) {
      HostSyncSlot(ctx, kSelectorSyncTail, tmp, p1, ret);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 157u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p1 = static_cast<uint32_t>(p1 == 1u);
      if (p1 != 0u) { goto exception_cleanup_a; }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 16u);
    aux3 = p0 + 20u;
    p1 = static_cast<uint32_t>(tmp == aux3);
  }
  if (p1 != 0u && ResumePhase(ctx) == kResumeRunning) { goto node_scan_entry; }
// Merge point chain_scan.
chain_scan:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = tmp;
    p1 = LoadU32(ctx, tmp + 48u);
  }
  if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, LoadU32(ctx, p0 + 148u) + 16u);
      tmp = static_cast<uint32_t>(p1 == 0u);
    }
    if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 158u) {
        HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_point = 158u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p0 = static_cast<uint32_t>(p0 != 1u);
        if (p0 != 0u) { goto unreachable_path; }
        goto pending_throw_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      tmp = ret + 16u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 159u) {
      HostSyncSlot(ctx, kSelectorSyncNode, p1, tmp, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 159u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p1 = static_cast<uint32_t>(p1 == 1u);
      if (p1 != 0u) { goto exception_bridge; }
      p1 = LoadU32(ctx, LoadU32(ctx, p0 + 148u) + 16u);
      aux = static_cast<uint32_t>(p1 == 0u);
      tmp = LoadU32(ctx, ret + 48u) + LoadU32(ctx, ret + 44u) - 1u;
      exc = tmp / 24u;
    }
    if (aux != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 160u) {
        HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_point = 160u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p0 = static_cast<uint32_t>(p0 != 1u);
        if (p0 != 0u) { goto unreachable_path; }
        goto pending_throw_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      ptr2 = LoadU32(ctx, p1 + 100u);
      p1 = static_cast<uint32_t>(ptr2 == 0u);
    }
    if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 161u) {
        HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_point = 161u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p0 = static_cast<uint32_t>(p0 != 1u);
        if (p0 != 0u) { goto unreachable_path; }
        goto pending_throw_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      ptr2 = ptr2 + 488u;
      ptr = exc << 2;
      aux = LoadU32(ctx, ptr + LoadU32(ctx, ret + 32u));
      tmp = aux + ((tmp - exc * 24u) * 168u);
      p1 = CopyBytes(ctx, ptr2, tmp + 4u, 68u);
      StoreU32(ctx, kDataD015C, 0u);
      exc = tmp + 72u;
      p1 = ptr2 + 556u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 162u) {
      save = HostOsQuery(ctx, kSelectorQuerySeq, p1, exc);
      p1 = save;
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 162u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p1 = static_cast<uint32_t>(p1 == 1u);
      if (p1 != 0u) { goto exception_bridge; }
      StoreU32(ctx, kDataD015C, 0u);
      exc = tmp + 84u;
      p1 = ptr2 + 568u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 163u) {
      save = HostOsQuery(ctx, kSelectorQuerySeq, p1, exc);
      p1 = save;
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 163u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p1 = static_cast<uint32_t>(p1 == 1u);
      if (p1 != 0u) { goto exception_bridge; }
      p1 = CopyBytes(ctx, ptr2 + 580u, tmp + 96u, 66u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, ret + 4u);
    if (p1 == 0u) {
      if (ret == LoadU32(ctx, ret + 8u)) { goto chain_loaded; }
      exc = ret + 8u;
    // Merge point chain_pair_loop.
    chain_pair_loop:;
      p1 = LoadU32(ctx, exc);
      exc = p1 + 8u;
      tmp = LoadU32(ctx, p1 + 8u);
      ret = LoadU32(ctx, tmp);
      if (ret != p1) { goto chain_pair_loop; }
      goto chain_loaded;
    }
  // Merge point chain_tail.
  chain_tail:;
    tmp = p1;
    p1 = LoadU32(ctx, tmp);
    if (p1 != 0u) { goto chain_tail; }
  // Merge point chain_loaded.
  chain_loaded:;
    p1 = static_cast<uint32_t>(tmp != aux);
    if (p1 != 0u) { goto chain_scan; }
    goto node_scan_entry;
  }
// Merge point exception_bridge.
exception_bridge:;
  if (ResumePhase(ctx) == kResumeRunning) {
    exc = HostRet0(ctx);
    p0 = HostTempRet0(ctx);
    goto exception_reload;
  }
// Merge point node_scan_entry.
node_scan_entry:;
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 28u);
    ptr2 = p0 + 32u;
    p1 = static_cast<uint32_t>(tmp == ptr2);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (p1 != 0u) { goto slot_scan_entry; }
    idx = frame + 424u;
  }
// Merge point node_scan.
node_scan:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, LoadU32(ctx, p0 + 148u) + 16u);
    ret = static_cast<uint32_t>(p1 == 0u);
  }
  if (ret != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 164u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 164u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0 != 0u) { goto unreachable_path; }
      goto pending_throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, kDataD015C, 0u);
    ret = tmp;
    tmp = tmp + 16u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 165u) {
    HostSyncSlot(ctx, kSelectorSyncNode, p1, tmp, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 165u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (p1 != 0u) { goto exception_bridge_b; }
    flag = LoadU32(ctx, p0 + 148u);
    StoreU32(ctx, kDataD015C, 0u);
    tmp = ret + 28u;
    p1 = frame + 408u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 166u) {
    save = HostOsQuery(ctx, kSelectorQueryNode, p1, tmp);
    tmp = save;
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 166u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p1 = static_cast<uint32_t>(p1 == 1u);
    if (p1 != 0u) { goto exception_bridge_b; }
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 167u) {
    HostSyncSlot(ctx, kSelectorSyncField, flag, tmp, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 167u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p1 = static_cast<uint32_t>(p1 == 1u);
    if (p1 != 0u) { goto exception_cleanup_g; }
    StoreU32(ctx, frame + 408u, kAssertMessageData);
    if (static_cast<int32_t>(LoadS8(ctx, frame + 447u)) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 436u));
    }
    ReleaseNodeTree(ctx, idx, LoadU32(ctx, frame + 428u));
    if (static_cast<int32_t>(LoadS8(ctx, frame + 423u)) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 412u));
    }
    p1 = LoadU32(ctx, ret + 4u);
    if (p1 == 0u) {
      if (ret == LoadU32(ctx, ret + 8u)) { goto node_done; }
      exc = ret + 8u;
    // Merge point node_pair_loop.
    node_pair_loop:;
      p1 = LoadU32(ctx, exc);
      exc = p1 + 8u;
      tmp = LoadU32(ctx, p1 + 8u);
      ret = LoadU32(ctx, tmp);
      if (ret != p1) { goto node_pair_loop; }
      goto node_done;
    }
  // Merge point node_loop.
  node_loop:;
    tmp = p1;
    p1 = LoadU32(ctx, tmp);
    if (p1 != 0u) { goto node_loop; }
  // Merge point node_done.
  node_done:;
    p1 = static_cast<uint32_t>(tmp != ptr2);
    if (p1 != 0u) { goto node_scan; }
    goto slot_scan_entry;
  }
// Merge point exception_bridge_b.
exception_bridge_b:;
  if (ResumePhase(ctx) == kResumeRunning) {
    exc = HostRet0(ctx);
    p0 = HostTempRet0(ctx);
    goto exception_reload;
  }
// Merge point exception_cleanup_g.
exception_cleanup_g:;
  if (ResumePhase(ctx) == kResumeRunning) {
    exc = HostRet0(ctx);
    p0 = HostTempRet0(ctx);
    StoreU32(ctx, frame + 408u, kAssertMessageData);
    if (static_cast<int32_t>(LoadS8(ctx, frame + 447u)) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 436u));
    }
    ReleaseNodeTree(ctx, idx, LoadU32(ctx, frame + 428u));
    p0 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 423u)) > -1);
    if (p0 != 0u) { goto exception_reload; }
    p0 = LoadU32(ctx, frame + 412u);
    DeallocateRawBlock(ctx, p0);
    goto exception_reload;
  }
// Merge point slot_scan_entry.
slot_scan_entry:;
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, p0 + 52u);
    ptr2 = p0 + 56u;
    p1 = static_cast<uint32_t>(tmp != ptr2);
  }
  if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    idx = (ResumePhase(ctx) != kResumeRunning) ? idx : frame + 376u;
  // Merge point slot_scan.
  slot_scan:;
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, LoadU32(ctx, p0 + 148u) + 16u);
      ret = static_cast<uint32_t>(p1 == 0u);
    }
    if (ret != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 168u) {
        HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_point = 168u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p0 = static_cast<uint32_t>(p0 != 1u);
        if (p0 != 0u) { goto unreachable_path; }
        goto pending_throw_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      ret = tmp;
      tmp = tmp + 16u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 169u) {
      HostSyncSlot(ctx, kSelectorSyncNode, p1, tmp, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 169u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p1 = static_cast<uint32_t>(p1 == 1u);
      if (p1 != 0u) { goto exception_bridge_c; }
      flag = LoadU32(ctx, p0 + 148u);
      StoreU32(ctx, kDataD015C, 0u);
      tmp = ret + 28u;
      p1 = frame + 360u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 170u) {
      save = HostOsQuery(ctx, kSelectorQueryNode, p1, tmp);
      tmp = save;
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 170u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p1 = static_cast<uint32_t>(p1 == 1u);
      if (p1 != 0u) { goto exception_bridge_c; }
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 171u) {
      HostSyncSlot(ctx, kSelectorSyncField, flag, tmp, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 171u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p1 = static_cast<uint32_t>(p1 == 1u);
      if (p1 != 0u) { goto exception_bridge_d; }
      StoreU32(ctx, frame + 360u, kAssertMessageData);
      if (static_cast<int32_t>(LoadS8(ctx, frame + 399u)) <= -1) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 388u));
      }
      ReleaseNodeTree(ctx, idx, LoadU32(ctx, frame + 380u));
      if (static_cast<int32_t>(LoadS8(ctx, frame + 375u)) <= -1) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 364u));
      }
      p1 = LoadU32(ctx, ret + 4u);
      if (p1 == 0u) {
        if (ret == LoadU32(ctx, ret + 8u)) { goto slot_chain_done; }
        exc = ret + 8u;
      // Merge point slot_chain_loop.
      slot_chain_loop:;
        p1 = LoadU32(ctx, exc);
        exc = p1 + 8u;
        tmp = LoadU32(ctx, p1 + 8u);
        ret = LoadU32(ctx, tmp);
        if (ret != p1) { goto slot_chain_loop; }
        goto slot_chain_done;
      }
    // Merge point slot_chain_tail.
    slot_chain_tail:;
      tmp = p1;
      p1 = LoadU32(ctx, tmp);
      if (p1 != 0u) { goto slot_chain_tail; }
    // Merge point slot_chain_done.
    slot_chain_done:;
      p1 = static_cast<uint32_t>(tmp != ptr2);
      if (p1 != 0u) { goto slot_scan; }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, p0 + 148u);
    StoreU32(ctx, kDataD015C, 0u);
    tmp = frame + 344u;
    p1 = p1 + 13308u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 172u) {
    save = HostOsQuery(ctx, kSelectorQuerySlot, tmp, p1);
    aux = save;
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 172u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p1 = static_cast<uint32_t>(p1 != 1u);
  }
  if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, p0 + 4u);
      ptr2 = p0 + 8u;
      p1 = static_cast<uint32_t>(tmp == ptr2);
      if (p1 != 0u) { goto list_scan_done; }
      aux3 = frame + 312u;
    }
  // Merge point list_scan.
  list_scan:;
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, LoadU32(ctx, p0 + 148u) + 16u);
      ret = static_cast<uint32_t>(p1 == 0u);
    }
    if (ret != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 173u) {
        HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_point = 173u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p0 = static_cast<uint32_t>(p0 != 1u);
        if (p0 != 0u) { goto unreachable_path; }
        goto pending_throw_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      flag = tmp;
      tmp = tmp + 16u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 174u) {
      HostSyncSlot(ctx, kSelectorSyncNode, p1, tmp, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 174u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p1 = static_cast<uint32_t>(p1 == 1u);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (p1 != 0u) { goto exception_list_abort; }
      p1 = LoadU32(ctx, p0 + 148u);
      StoreU32(ctx, kDataD015C, 0u);
      p1 = p1 + 13308u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 175u) {
      save = HostOsQuery(ctx, kSelectorQuerySeq, p1, tmp);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
  #endif
      resume_point = 175u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p1 = save;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p1 == 1u) { goto exception_list_abort; }
      aux2 = 0u;
      p1 = LoadU32(ctx, flag + 28u);
      tmp = static_cast<uint32_t>(p1 == LoadU32(ctx, flag + 32u));
    }
    if (tmp != 0u && ResumePhase(ctx) == kResumeRunning) { goto list_walk_end; }
    // Merge point list_walk.
    list_walk:;
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU8(ctx, p1 + 15u);
      ret = static_cast<uint32_t>(static_cast<int32_t>(tmp << 24) >> 24);
      ptr = static_cast<uint32_t>(static_cast<int32_t>(ret) < 0);
      idx = p1 + 4u;
      exc = LoadU32(ctx, p1 + 8u);
      ptr = static_cast<uint32_t>((ptr != 0u ? exc : tmp) == 5u);
    }
    if (ptr != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 176u) {
        save = HostInvokeIiiiii(ctx, kSelectorNamedProp, idx, 0u, kDataFFFFFFFF, kNameSceIn, 5u);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
  #endif
        resume_point = 176u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        ret = save;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        if (LoadU32(ctx, kDataD015C) == 1u) { goto pending_throw_path; }
        tmp = static_cast<uint32_t>(ret == 0u);
        if (tmp != 0u) { goto list_step_end; }
        exc = LoadU32(ctx, p1 + 8u);
        tmp = LoadU8(ctx, p1 + 15u);
      }
      ret = (ResumePhase(ctx) != kResumeRunning) ? ret : tmp;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      ret = static_cast<uint32_t>(static_cast<int32_t>(ret << 24) >> 24);
      ret = static_cast<uint32_t>(static_cast<int32_t>(ret) < 0);
      tmp = static_cast<uint32_t>((ret != 0u ? exc : tmp) != 6u);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (tmp != 0u) { goto list_retry; }
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 177u) {
      save = HostInvokeIiiiii(ctx, kSelectorNamedProp, idx, 0u, kDataFFFFFFFF, kNameSceOut, 6u);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
  #endif
      resume_point = 177u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      ret = save;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      tmp = static_cast<uint32_t>(tmp == 1u);
      if (tmp != 0u) { goto pending_throw_path; }
      if (ret != 0u) { goto list_retry; }
      aux2 = 1u;
      goto list_advance;
    }
    // Merge point list_retry.
    list_retry:;
    if (ResumePhase(ctx) == kResumeRunning) {
      exc = LoadU32(ctx, p0 + 148u);
      StoreU32(ctx, kDataD015C, 0u);
      tmp = frame + 296u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 178u) {
      save = HostOsQuery(ctx, kSelectorQueryNode, tmp, p1);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
  #endif
      resume_point = 178u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      ret = save;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      tmp = static_cast<uint32_t>(tmp == 1u);
      if (tmp != 0u) { goto exception_list_abort_two; }
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 179u) {
      HostSyncSlot(ctx, kSelectorSyncField, exc, ret, 1u);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
  #endif
      resume_point = 179u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      tmp = static_cast<uint32_t>(tmp == 1u);
      if (tmp != 0u) { goto exception_list_cleanup; }
      StoreU32(ctx, frame + 296u, kAssertMessageData);
      if (static_cast<int32_t>(LoadS8(ctx, frame + 335u)) <= -1) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 324u));
      }
      ReleaseNodeTree(ctx, aux3, LoadU32(ctx, frame + 316u));
      tmp = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 311u)) > -1);
      if (tmp != 0u) { goto list_advance; }
      tmp = LoadU32(ctx, frame + 300u);
      DeallocateRawBlock(ctx, tmp);
    }
    // Merge point list_advance.
    list_advance:;
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = p1 + 44u;
      tmp = static_cast<uint32_t>(p1 != LoadU32(ctx, flag + 32u));
      if (tmp != 0u) { goto list_walk; }
      p1 = static_cast<uint32_t>(aux2 == 0u);
      if (p1 != 0u) { goto list_walk_end; }
      tmp = LoadU32(ctx, p0 + 148u);
      p1 = LoadU32(ctx, tmp + 16u);
      if (p1 != 0u) { goto list_next; }
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 180u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
  #endif
      resume_point = 180u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0 != 0u) { goto unreachable_path; }
      goto pending_throw_path;
    }
    // Merge point exception_list_cleanup.
    exception_list_cleanup:;
    if (ResumePhase(ctx) == kResumeRunning) {
      // The staged tag list is dropped: read back the pending throw, then release
      // its two sub-blocks and the nested node tree it owns.
      exc = HostRet0(ctx);
      HostTempRet0(ctx);
      StoreU32(ctx, frame + 296u, kAssertMessageData);
      if (static_cast<int32_t>(LoadS8(ctx, frame + 335u)) <= -1) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 324u));
      }
      ReleaseNodeTree(ctx, aux3, LoadU32(ctx, frame + 316u));
      tmp = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 311u)) > -1);
      if (tmp != 0u) { goto release_call_result; }
      p0 = LoadU32(ctx, frame + 300u);
      DeallocateRawBlock(ctx, p0);
      goto release_call_result;
    }
    // Unwinding always resumes at the step after the one that failed.
    if (ResumePhase(ctx) == kResumeRunning) {
      aux2 = 0u;
      p1 = p1 + 44u;
      tmp = static_cast<uint32_t>(p1 != LoadU32(ctx, flag + 32u));
      if (tmp != 0u) { goto list_walk; }
    }
    // Merge point list_walk_end.
    list_walk_end:;
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, p0 + 148u);
      p1 = LoadU32(ctx, tmp + 16u);
      ret = static_cast<uint32_t>(p1 == 0u);
    }
    if (ret != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 181u) {
        HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_point = 181u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p0 = static_cast<uint32_t>(p0 != 1u);
        if (p0 != 0u) { goto unreachable_path; }
        goto pending_throw_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      ret = LoadU32(ctx, p1 + 100u);
      p1 = static_cast<uint32_t>(ret == 0u);
    }
    if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 182u) {
        HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_point = 182u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p0 = static_cast<uint32_t>(p0 != 1u);
        if (p0 != 0u) { goto unreachable_path; }
        goto pending_throw_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The scenario core keeps the tag list of this step at +13308; hand the tail
      // of it, plus the two staging slots, to the host as a four-part record.
      p1 = tmp + 13308u;
      StoreU32(ctx, frame + 488u, p1);
      StoreU32(ctx, kDataD015C, 0u);
      exc = frame + 280u;
      ptr = frame + 488u;
      idx = frame + 480u;
      tmp = tmp + 13572u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 183u) {
      HostInvokeViiiiii(ctx, 3544u, exc, tmp, p1, kMsgPxRef, ptr, idx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 183u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p1 = static_cast<uint32_t>(p1 == 1u);
      if (p1 != 0u) { goto exception_list_abort_two; }
      tmp = LoadU32(ctx, frame + 280u);
      p1 = LoadU32(ctx, LoadU32(ctx, ret) + 380u);
      StoreU32(ctx, kDataD015C, 0u);
      tmp = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 184u) {
      HostSyncSlot(ctx, p1, ret, tmp, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 184u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p1 = static_cast<uint32_t>(p1 != 1u);
      if (p1 != 0u) { goto list_scan_step; }
      goto list_step_end;
    }
    // Merge point list_next.
    list_next:;
    if (ResumePhase(ctx) == kResumeRunning) {
      ret = LoadU32(ctx, p1 + 100u);
      p1 = static_cast<uint32_t>(ret == 0u);
    }
    if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 185u) {
        HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_point = 185u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p0 = static_cast<uint32_t>(p0 != 1u);
        if (p0 != 0u) { goto unreachable_path; }
        goto pending_throw_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = tmp + 13308u;
      StoreU32(ctx, frame + 488u, p1);
      StoreU32(ctx, kDataD015C, 0u);
      exc = frame + 280u;
      ptr = frame + 488u;
      idx = frame + 480u;
      tmp = tmp + 13572u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 186u) {
      HostInvokeViiiiii(ctx, 3544u, exc, tmp, p1, kMsgPxRef, ptr, idx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 186u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p1 = static_cast<uint32_t>(p1 == 1u);
      if (p1 != 0u) { goto exception_list_abort_two; }
      tmp = LoadU32(ctx, frame + 280u);
      p1 = LoadU32(ctx, LoadU32(ctx, ret) + 380u);
      StoreU32(ctx, kDataD015C, 0u);
      tmp = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 187u) {
      HostSyncSlot(ctx, p1, ret, tmp, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 187u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p1 = static_cast<uint32_t>(p1 == 1u);
      if (p1 != 0u) { goto exception_list_abort_two; }
      p1 = LoadU32(ctx, LoadU32(ctx, p0 + 148u) + 16u);
      tmp = static_cast<uint32_t>(p1 == 0u);
    }
    if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 188u) {
        HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_point = 188u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p0 = static_cast<uint32_t>(p0 != 1u);
        if (p0 != 0u) { goto unreachable_path; }
        goto pending_throw_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      ret = LoadU32(ctx, p1 + 100u);
      p1 = static_cast<uint32_t>(ret == 0u);
    }
    if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 189u) {
        HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_point = 189u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p0 = static_cast<uint32_t>(p0 != 1u);
        if (p0 != 0u) { goto unreachable_path; }
        goto pending_throw_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, LoadU32(ctx, ret) + 384u);
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 190u) {
      HostReleaseSlotPair(ctx, p1, ret);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 190u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p1 = static_cast<uint32_t>(p1 == 1u);
      if (p1 != 0u) { goto exception_list_abort_two; }
    }
    // Merge point list_scan_step.
    list_scan_step:;
    if (ResumePhase(ctx) == kResumeRunning) {
      // Walk the tag list of this step pair to its last node, then compare it with
      // the node the scenario core is currently holding.
      p1 = LoadU32(ctx, flag + 4u);
      if (p1 == 0u) {
        tmp = LoadU32(ctx, flag + 8u);
        if (flag == LoadU32(ctx, tmp)) { goto list_head_merge; }
        exc = flag + 8u;
        do {
          p1 = LoadU32(ctx, exc);
          exc = p1 + 8u;
          tmp = LoadU32(ctx, p1 + 8u);
          ret = LoadU32(ctx, tmp);
        } while (ret != p1);
        goto list_head_merge;
      }
      do {
        tmp = p1;
        p1 = LoadU32(ctx, p1);
      } while (p1 != 0u);
      // Merge point list_head_merge.
      list_head_merge:;
      p1 = static_cast<uint32_t>(tmp != ptr2);
      if (p1 != 0u) { goto list_scan; }
      goto list_scan_done;
    }
    // Merge point exception_list_abort.
    exception_list_abort:;
    if (ResumePhase(ctx) == kResumeRunning) {
      exc = HostRet0(ctx);
      p0 = HostTempRet0(ctx);
      goto release_call_result;
    }
    // Merge point exception_list_abort_two.
    exception_list_abort_two:;
    if (ResumePhase(ctx) == kResumeRunning) {
      exc = HostRet0(ctx);
      p0 = HostTempRet0(ctx);
      goto release_call_result;
    }
    // Merge point list_step_end.
    list_step_end:;
    if (ResumePhase(ctx) == kResumeRunning) {
      exc = HostRet0(ctx);
      p0 = HostTempRet0(ctx);
      goto release_call_result;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // No tag was handed in: capture the caller's exception state and unwind.
    exc = HostRet0(ctx);
    p0 = HostTempRet0(ctx);
    goto exception_reload;
  }
  // Merge point list_scan_done.
  list_scan_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, LoadU32(ctx, p0 + 148u) + 16u);
    tmp = static_cast<uint32_t>(p1 == 0u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 191u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 191u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0 != 0u) { goto unreachable_path; }
      goto pending_throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 192u) {
    HostSyncSlot(ctx, kSelectorSyncNode, p1, aux, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 192u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p1 = static_cast<uint32_t>(p1 == 1u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (p1 != 0u) { goto list_walk_abort; }
    p1 = LoadU32(ctx, p0 + 148u);
    StoreU32(ctx, kDataD015C, 0u);
    p1 = p1 + 13308u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 193u) {
    save = HostOsQuery(ctx, kSelectorQuerySeq, p1, aux);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 193u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    p1 = save;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p1 = static_cast<uint32_t>(p1 == 1u);
    if (p1 != 0u) { goto list_walk_abort; }
    tmp = LoadU32(ctx, LoadU32(ctx, p0 + 148u) + 112u);
    p1 = static_cast<uint32_t>(tmp == 0u);
  }
  if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 194u) {
      HostFailAssertion(ctx, kAssertWord, kMsgPx, kMsgSharedPtr, kAssertLine, kMsgArrow);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 194u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0 != 0u) { goto unreachable_path; }
      goto pending_throw_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 195u) {
    // Lay out a 32-byte record for the layer name the scenario core will store.
    save = HostFoldLayoutOffset(ctx, 1032u, 32u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 195u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    ret = save;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p1 = static_cast<uint32_t>(p1 != 1u);
  }
  if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
        if (ResumePhase(ctx) == kResumeRunning) {
          StoreU32(ctx, frame + 280u, ret);
          StoreU64(ctx, frame + 284u, 9223372174293729303ull);
          StoreU8(ctx, ret + 23u, 0u);
          StoreU64(ctx, ret + 15u, LoadU64(ctx, 771720u));
          StoreU64(ctx, ret + 8u, LoadU64(ctx, 771713u));
          str64a = LoadU64(ctx, 771705u);
          StoreU64(ctx, ret, str64a);
          p1 = LoadU32(ctx, LoadU32(ctx, tmp) + 24u);
          StoreU32(ctx, kDataD015C, 0u);
          ret = frame + 280u;
        }
        if (ResumePhase(ctx) == kResumeRunning || resume_point == 196u) {
          save = HostDispatchInvoke(ctx, p1, tmp, ret, aux);
          resume_point = 196u;
          if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
          p1 = save;
        }
        if (ResumePhase(ctx) == kResumeRunning) {
          p1 = LoadU32(ctx, kDataD015C);
          StoreU32(ctx, kDataD015C, 0u);
          p1 = static_cast<uint32_t>(p1 != 1u);
        }
        if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
          if (ResumePhase(ctx) == kResumeRunning) {
            if (static_cast<int32_t>(LoadS8(ctx, frame + 291u)) <= -1) {
              DeallocateRawBlock(ctx, LoadU32(ctx, frame + 280u));
            }
            p1 = static_cast<int32_t>(LoadU32(ctx, p0 + 168u)) > static_cast<int32_t>(1002u);
          }
          if (ResumePhase(ctx) == kResumeRunning) {
            if (p1 != 0u) { goto entry_walk_skip; }
            tmp = LoadU32(ctx, p0 + 76u);
            ptr2 = p0 + 80u;
            p1 = static_cast<uint32_t>(tmp == ptr2);
            if (p1 != 0u) { goto entry_walk_skip; }
          }
          // Merge point entry_loop: restart the entry walk for this dispatching layer.
          entry_loop:;
          if (ResumePhase(ctx) == kResumeRunning) {
            ret = tmp;
            ptr = LoadU32(ctx, tmp + 28u);
            p1 = static_cast<uint32_t>(ptr == LoadU32(ctx, tmp + 32u));
            if (p1 != 0u) { goto entry_loop_end; }
          }
          // Merge point entry_scan: inspect one 44-byte entry.
          entry_scan:;
          if (ResumePhase(ctx) == kResumeRunning) {
            p1 = LoadU8(ctx, ptr + 15u);
            exc = (static_cast<int32_t>((p1 << 24u)) >> 24u);
            idx = static_cast<int32_t>(exc) < 0;
            tmp = LoadU32(ctx, ptr + 8u);
            idx = static_cast<uint32_t>(((idx != 0u) ? tmp : p1) == 6u);
            flag = ptr + 4u;
          }
          cmp16 = static_cast<uint32_t>((ResumePhase(ctx) == kResumeRunning) ? idx : cmp16);
          if (cmp16 != 0u || ResumePhase(ctx) == kResumeRewinding) {
            if (ResumePhase(ctx) == kResumeRunning) {
              StoreU32(ctx, kDataD015C, 0u);
            }
            if (ResumePhase(ctx) == kResumeRunning || resume_point == 197u) {
              save = HostInvokeIiiiii(ctx, 1354u, flag, 0u, 0xFFFFFFFFu, 771729u, 6u);
              resume_point = 197u;
              if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
              tmp = save;
            }
            if (ResumePhase(ctx) == kResumeRunning) {
              p1 = LoadU32(ctx, kDataD015C);
              StoreU32(ctx, kDataD015C, 0u);
              if ((p1 == 1u)) { goto pending_throw_path; }
              p1 = static_cast<uint32_t>(tmp == 0u);
              if (p1 != 0u) { goto entry_skip_hook; }
              p1 = LoadU8(ctx, ptr + 15u);
              exc = p1;
            tmp = LoadU32(ctx, ptr + 8u);
          }
          }
          if (ResumePhase(ctx) == kResumeRunning) {
            exc = static_cast<int32_t>(static_cast<int8_t>(exc)) < 0;
            p1 = static_cast<uint32_t>(((exc != 0u) ? tmp : p1) != 5u);
            // No usable name or value in this entry: step to the next one.
            if (p1 != 0u) { goto entry_advance; }
            StoreU32(ctx, kDataD015C, 0u);
          }
          if (ResumePhase(ctx) == kResumeRunning || resume_point == 198u) {
            save = HostInvokeIiiiii(ctx, 1354u, flag, 0u, 0xFFFFFFFFu, 771736u, 5u);
            resume_point = 198u;
            if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
            tmp = save;
          }
          if (ResumePhase(ctx) == kResumeRunning) {
            p1 = LoadU32(ctx, kDataD015C);
            StoreU32(ctx, kDataD015C, 0u);
            p1 = static_cast<uint32_t>(p1 == 1u);
            if (p1 != 0u) { goto pending_throw_path; }
            if (tmp != 0u) { goto entry_advance; }
          }
          // Merge point entry_skip_hook: this entry has nothing to hook.
          entry_skip_hook:;
          if (ResumePhase(ctx) == kResumeRunning) {
            StoreU32(ctx, kDataD015C, 0u);
          }
          if (ResumePhase(ctx) == kResumeRunning || resume_point == 199u) {
            save = HostFoldLayoutOffset(ctx, 1032u, 16u);
            resume_point = 199u;
            if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
            aux2 = save;
          }
          if (ResumePhase(ctx) == kResumeRunning) {
            p1 = LoadU32(ctx, kDataD015C);
            StoreU32(ctx, kDataD015C, 0u);
            p1 = static_cast<uint32_t>(p1 == 1u);
            if (p1 != 0u) { goto throw_release; }
            str64a = LoadU64(ctx, 771747u);
            StoreU64(ctx, aux2 + 5u, str64a);
            str64b = LoadU64(ctx, 771742u);
            StoreU64(ctx, aux2, str64b);
            StoreU8(ctx, aux2 + 13u, 0u);
            p1 = LoadU32(ctx, ptr + 20u);
          }
          if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
          // Walk the layer name chain for the entry that owns the pending name.
            name_chain_scan:;
            if (ResumePhase(ctx) == kResumeRunning) {
              tmp = LoadU8(ctx, p1 + 27u);
              flag = static_cast<int32_t>(static_cast<int8_t>(tmp)) < 0;
              aux3 = static_cast<uint32_t>((flag != 0u) ? LoadU32(ctx, p1 + 20u) : tmp);
              exc = static_cast<uint32_t>(aux3 < 13u);
              idx = static_cast<uint32_t>((exc != 0u) ? aux3 : 13u);
            }
            if (ResumePhase(ctx) == kResumeRunning) {
              if (idx != 0u) {
                tmp = p1 + 16u;
                flag = static_cast<uint32_t>((flag != 0u) ? LoadU32(ctx, tmp) : tmp);
                save = CompareBytes(ctx, aux2, flag, idx);
                tmp = save;
                if ((tmp == 0u)) {
                  tmp = static_cast<uint32_t>(aux3 <= 13u);
                  if (tmp != 0u) { goto name_scan_second; }
                  goto name_link_next;
                }
                tmp = static_cast<int32_t>(tmp) <= -1;
                if (tmp != 0u) { goto name_link_next; }
          // Merge point name_scan_second: compare again with the operands swapped.
                name_scan_second:;
                save = CompareBytes(ctx, flag, aux2, idx);
                tmp = save;
                flag = static_cast<uint32_t>(tmp == 0u);
                if (flag != 0u) { goto name_scan_check; }
                tmp = static_cast<int32_t>(tmp) > -1;
                if (tmp != 0u) { goto name_scan_done; }
                goto name_link_advance;
              }
              tmp = static_cast<uint32_t>(aux3 > 13u);
              if (tmp != 0u) { goto name_link_next; }
          // Merge point name_scan_check.
              name_scan_check:;
              if (exc != 0u) { goto name_link_advance; }
          // Merge point name_scan_done: the name resolved.
              name_scan_done:;
              StoreU32(ctx, kDataD015C, 0u);
            }
            if (ResumePhase(ctx) == kResumeRunning || resume_point == 200u) {
              save = HostFoldLayoutOffset(ctx, 1032u, 16u);
              resume_point = 200u;
              if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
              tmp = save;
            }
            if (ResumePhase(ctx) == kResumeRunning) {
              p1 = LoadU32(ctx, kDataD015C);
              StoreU32(ctx, kDataD015C, 0u);
              p1 = static_cast<uint32_t>(p1 == 1u);
              if (p1 != 0u) { goto throw_free_layout; }
              StoreU32(ctx, frame + 280u, tmp);
              StoreU64(ctx, frame + 284u, 9223372105574252557ull);
              StoreU8(ctx, tmp + 13u, 0u);
              StoreU64(ctx, tmp + 5u, str64a);
              StoreU64(ctx, tmp, str64b);
              StoreU32(ctx, kDataD015C, 0u);
              StoreU32(ctx, frame + 480u, (frame + 280u));
              tmp = ptr + 16u;
              flag = frame + 280u;
              exc = frame + 480u;
              idx = frame + 472u;
              p1 = frame + 488u;
            }
            if (ResumePhase(ctx) == kResumeRunning || resume_point == 201u) {
              HostInvokeViiiiii(ctx, 1959u, p1, tmp, flag, 772012u, exc, idx);
              #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
              SyncMemoryBase(ctx);
              #endif
              resume_point = 201u;
              if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
            }
            if (ResumePhase(ctx) == kResumeRunning) {
              p1 = LoadU32(ctx, kDataD015C);
              StoreU32(ctx, kDataD015C, 0u);
              p1 = static_cast<uint32_t>(p1 == 1u);
              if (p1 != 0u) { goto throw_free_result_layout; }
              p1 = LoadU32(ctx, frame + 488u);
              flag = p1 + 28u;
              // The name text sits inline in the node, or behind a pointer.
              flag = (static_cast<int32_t>(LoadS8(ctx, p1 + 39u)) <= -1)
                  ? LoadU32(ctx, flag) : flag;
            }
            if (ResumePhase(ctx) == kResumeRunning || resume_point == 202u) {
              save = ReadPackedI64Value(ctx, flag, 0u, 16u);
              resume_point = 202u;
              if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
              p1 = save;
            }
            if (ResumePhase(ctx) == kResumeRunning) {
              flag = static_cast<int32_t>(LoadS8(ctx, frame + 291u)) <= -1;
              if (flag != 0u) {
                flag = LoadU32(ctx, frame + 280u);
                DeallocateRawBlock(ctx, flag);
              }
              DeallocateRawBlock(ctx, aux2);
              if (p1 != 0u) { goto entry_advance; }
              StoreU32(ctx, kDataD015C, 0u);
            }
            if (ResumePhase(ctx) == kResumeRunning || resume_point == 203u) {
              save = HostFoldLayoutOffset(ctx, 1032u, 16u);
              resume_point = 203u;
              if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
              flag = save;
            }
            if (ResumePhase(ctx) == kResumeRunning) {
              p1 = LoadU32(ctx, kDataD015C);
              StoreU32(ctx, kDataD015C, 0u);
              p1 = static_cast<uint32_t>(p1 == 1u);
              if (p1 != 0u) { goto throw_release; }
              StoreU32(ctx, frame + 280u, flag);
              StoreU64(ctx, frame + 284u, 9223372105574252557ull);
              StoreU8(ctx, flag + 13u, 0u);
              StoreU64(ctx, flag + 5u, str64a);
              StoreU64(ctx, flag, str64b);
              StoreU32(ctx, kDataD015C, 0u);
              StoreU32(ctx, frame + 480u, (frame + 280u));
              flag = frame + 280u;
              exc = frame + 480u;
              idx = frame + 472u;
              p1 = frame + 488u;
            }
            if (ResumePhase(ctx) == kResumeRunning || resume_point == 204u) {
              HostInvokeViiiiii(ctx, 1959u, p1, tmp, flag, 772012u, exc, idx);
              #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
              SyncMemoryBase(ctx);
              #endif
              resume_point = 204u;
              if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
            }
            if (ResumePhase(ctx) == kResumeRunning) {
              p1 = LoadU32(ctx, kDataD015C);
              StoreU32(ctx, kDataD015C, 0u);
              p1 = static_cast<uint32_t>(p1 == 1u);
              if (p1 != 0u) { goto throw_free_result; }
              StoreU32(ctx, kDataD015C, 0u);
              p1 = (LoadU32(ctx, frame + 488u) + 28u);
            }
            if (ResumePhase(ctx) == kResumeRunning || resume_point == 205u) {
              save = HostOsQuery(ctx, 996u, p1, 771756u);
              resume_point = 205u;
              if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
              p1 = save;
            }
            if (ResumePhase(ctx) == kResumeRunning) {
              p1 = LoadU32(ctx, kDataD015C);
              StoreU32(ctx, kDataD015C, 0u);
              p1 = static_cast<uint32_t>(p1 == 1u);
              if (p1 != 0u) { goto throw_free_result; }
              p1 = static_cast<int32_t>(LoadS8(ctx, frame + 291u)) > -1;
              if (p1 != 0u) { goto entry_advance; }
              p1 = LoadU32(ctx, frame + 280u);
              DeallocateRawBlock(ctx, p1);
              goto entry_advance;
            }
            name_link_advance:;
            p1 = (ResumePhase(ctx) == kResumeRunning) ? p1 + 4u : p1;
            name_link_next:;
            if (ResumePhase(ctx) == kResumeRunning) {
              p1 = LoadU32(ctx, p1);
              if (p1 != 0u) { goto name_chain_scan; }
            }
          }
          if (ResumePhase(ctx) == kResumeRunning) {
            DeallocateRawBlock(ctx, aux2);
          }
          entry_advance:;
          if (ResumePhase(ctx) == kResumeRunning) {
            ptr = ptr + 44u;
            p1 = static_cast<uint32_t>(ptr != LoadU32(ctx, ret + 32u));
            if (p1 != 0u) { goto entry_scan; }
            goto entry_loop_end;
          }
          throw_free_layout:;
          if (ResumePhase(ctx) == kResumeRunning) {
            save = HostRet0(ctx);
            exc = save;
            save = HostTempRet0(ctx);
            p0 = save;
            DeallocateRawBlock(ctx, aux2);
            goto release_call_result;
          }
          throw_free_result_layout:;
          if (ResumePhase(ctx) == kResumeRunning) {
            save = HostRet0(ctx);
            exc = save;
            save = HostTempRet0(ctx);
            p0 = static_cast<int32_t>(LoadS8(ctx, frame + 291u)) <= -1;
            if (p0 != 0u) {
              p0 = LoadU32(ctx, frame + 280u);
              DeallocateRawBlock(ctx, p0);
            }
            DeallocateRawBlock(ctx, aux2);
            goto release_call_result;
          }
throw_release:;
          if (ResumePhase(ctx) == kResumeRunning) {
            save = HostRet0(ctx);
            exc = save;
            save = HostTempRet0(ctx);
            p0 = save;
            goto release_call_result;
          }
          throw_free_result:;
          if (ResumePhase(ctx) == kResumeRunning) {
            save = HostRet0(ctx);
            exc = save;
            save = HostTempRet0(ctx);
            p0 = static_cast<int32_t>(LoadS8(ctx, frame + 291u)) > -1;
            if (p0 != 0u) { goto release_call_result; }
            p0 = LoadU32(ctx, frame + 280u);
            DeallocateRawBlock(ctx, p0);
            goto release_call_result;
          }
          entry_loop_end:;
          if (ResumePhase(ctx) == kResumeRunning) {
            p1 = LoadU32(ctx, ret + 4u);
            if (p1 == 0u) {
              tmp = LoadU32(ctx, ret + 8u);
              if (ret == LoadU32(ctx, tmp)) { goto chain_follow_done; }
              exc = ret + 8u;
              chain_follow:;
              p1 = LoadU32(ctx, exc);
              exc = p1 + 8u;
              tmp = LoadU32(ctx, p1 + 8u);
              ret = LoadU32(ctx, tmp);
              if (ret != p1) { goto chain_follow; }
              goto chain_follow_done;
            }
          // Merge point chain_release: release the linked entry chain.
            chain_release:;
            tmp = p1;
            p1 = LoadU32(ctx, tmp);
            if (p1 != 0u) { goto chain_release; }
          // Merge point chain_follow_done: the layer entry list is a single self-linked node.
            chain_follow_done:;
            p1 = static_cast<uint32_t>(tmp != ptr2);
            if (p1 != 0u) { goto entry_loop; }
          }
          entry_walk_skip:;
          if (ResumePhase(ctx) == kResumeRunning) {
            tmp = LoadU32(ctx, p0 + 76u);
            aux3 = p0 + 80u;
            p1 = static_cast<uint32_t>(tmp == aux3);
          }
          if (ResumePhase(ctx) == kResumeRunning) {
            if (p1 != 0u) { goto hook_lyc_skip_all; }
            ptr2 = frame + 248u;
          }
          // Merge point hook_lyc_loop: walk the collected clip entries.
          hook_lyc_loop:;
          if (ResumePhase(ctx) == kResumeRunning) {
            flag = tmp;
            p1 = LoadU32(ctx, tmp + 28u);
            tmp = static_cast<uint32_t>(p1 == LoadU32(ctx, tmp + 32u));
            if (tmp != 0u) { goto hook_lyc_end; }
          }
          // Merge point hook_lyc_scan: hook one clip entry by name.
          hook_lyc_scan:;
          if (ResumePhase(ctx) == kResumeRunning) {
            tmp = LoadU8(ctx, p1 + 15u);
            ret = static_cast<int32_t>(static_cast<int8_t>(tmp));
            ptr = static_cast<int32_t>(ret) < 0;
            idx = p1 + 4u;
            exc = LoadU32(ctx, p1 + 8u);
            ptr = static_cast<uint32_t>((ptr != 0u ? exc : tmp) == 3u);
          }
          if (ptr != 0u || ResumePhase(ctx) == kResumeRewinding) {
            if (ResumePhase(ctx) == kResumeRunning) {
              StoreU32(ctx, kDataD015C, 0u);
            }
            if (ResumePhase(ctx) == kResumeRunning || resume_point == 206u) {
              save = HostInvokeIiiiii(ctx, kSelectorNamedProp, idx, 0u, 0xFFFFFFFFu, kNameLyc, 3u);
              resume_point = 206u;
              if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
              ret = save;
            }
            if (ResumePhase(ctx) == kResumeRunning) {
              tmp = LoadU32(ctx, kDataD015C);
              StoreU32(ctx, kDataD015C, 0u);
              if (tmp == 1u) { goto pending_throw_path; }
              tmp = static_cast<uint32_t>(ret == 0u);
              if (tmp != 0u) { goto hook_lyc_skip; }
              exc = LoadU32(ctx, p1 + 8u);
              tmp = LoadU8(ctx, p1 + 15u);
            }
            ret = tmp;
          }
          if (ResumePhase(ctx) == kResumeRunning) {
            ptr = static_cast<uint32_t>(
              (static_cast<int32_t>(static_cast<int8_t>(ret)) < 0 ? exc : tmp) == 5u);
          }
          if (ptr != 0u || ResumePhase(ctx) == kResumeRewinding) {
            if (ResumePhase(ctx) == kResumeRunning) {
              StoreU32(ctx, kDataD015C, 0u);
            }
            if (ResumePhase(ctx) == kResumeRunning || resume_point == 207u) {
              save = HostInvokeIiiiii(ctx, kSelectorNamedProp, idx, 0u, 0xFFFFFFFFu,
                kNameAnime, 5u);
              resume_point = 207u;
              if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
              ret = save;
            }
            if (ResumePhase(ctx) == kResumeRunning) {
              tmp = LoadU32(ctx, kDataD015C);
              StoreU32(ctx, kDataD015C, 0u);
              if (tmp == 1u) { goto pending_throw_path; }
              tmp = static_cast<uint32_t>(ret == 0u);
              if (tmp != 0u) { goto hook_lyc_skip; }
              exc = LoadU32(ctx, p1 + 8u);
              tmp = LoadU8(ctx, p1 + 15u);
            }
            ret = tmp;
          }
          if (ResumePhase(ctx) == kResumeRunning) {
            ret = static_cast<int32_t>(static_cast<int8_t>(ret)) < 0;
            tmp = static_cast<uint32_t>((ret != 0u ? exc : tmp) != 5u);
            if (tmp != 0u) { goto hook_lyc_advance; }
            StoreU32(ctx, kDataD015C, 0u);
          }
          if (ResumePhase(ctx) == kResumeRunning || resume_point == 208u) {
            save = HostInvokeIiiiii(ctx, kSelectorNamedProp, idx, 0u, 0xFFFFFFFFu, kNameVideo, 5u);
            resume_point = 208u;
            if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
            ret = save;
          }
          if (ResumePhase(ctx) == kResumeRunning) {
            tmp = LoadU32(ctx, kDataD015C);
            StoreU32(ctx, kDataD015C, 0u);
            tmp = static_cast<uint32_t>(tmp == 1u);
            if (tmp != 0u) { goto pending_throw_path; }
            if (ret != 0u) { goto hook_lyc_advance; }
          }
          // Merge point hook_lyc_skip: this entry has no hook to run.
          hook_lyc_skip:;
          if (ResumePhase(ctx) == kResumeRunning) {
            exc = LoadU32(ctx, p0 + 148u);
            StoreU32(ctx, kDataD015C, 0u);
            tmp = frame + 232u;
          }
          if (ResumePhase(ctx) == kResumeRunning || resume_point == 209u) {
            save = HostOsQuery(ctx, kSelectorQueryNode, tmp, p1);
            resume_point = 209u;
            if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
            ret = save;
          }
          if (ResumePhase(ctx) == kResumeRunning) {
            tmp = LoadU32(ctx, kDataD015C);
            StoreU32(ctx, kDataD015C, 0u);
            tmp = static_cast<uint32_t>(tmp == 1u);
            if (tmp != 0u) { goto throw_release; }
            StoreU32(ctx, kDataD015C, 0u);
          }
          if (ResumePhase(ctx) == kResumeRunning || resume_point == 210u) {
            HostSyncSlot(ctx, kSelectorSyncField, exc, ret, 1u);
            #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
            SyncMemoryBase(ctx);
            #endif
            resume_point = 210u;
            if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
          }
          if (ResumePhase(ctx) == kResumeRunning) {
            tmp = LoadU32(ctx, kDataD015C);
            StoreU32(ctx, kDataD015C, 0u);
            tmp = static_cast<uint32_t>(tmp == 1u);
            if (tmp != 0u) { goto throw_free_hook_lyc; }
            StoreU32(ctx, frame + 232u, kAssertMessageData);
            if (static_cast<int32_t>(LoadS8(ctx, frame + 271u)) <= -1) {
              DeallocateRawBlock(ctx, LoadU32(ctx, frame + 260u));
            }
            ReleaseNodeTree(ctx, ptr2, LoadU32(ctx, frame + 252u));
            tmp = static_cast<int32_t>(LoadS8(ctx, frame + 247u)) > -1;
            if (tmp != 0u) { goto hook_lyc_advance; }
            tmp = LoadU32(ctx, frame + 236u);
            DeallocateRawBlock(ctx, tmp);
          }
          // Merge point hook_lyc_advance: step to the next clip entry.
          hook_lyc_advance:;
          if (ResumePhase(ctx) == kResumeRunning) {
            p1 = p1 + 44u;
            tmp = static_cast<uint32_t>(p1 != LoadU32(ctx, flag + 32u));
            if (tmp != 0u) { goto hook_lyc_scan; }
            goto hook_lyc_end;
          }
          // Merge point throw_free_hook_lyc: free the clip entry buffers first.
          throw_free_hook_lyc:;
          if (ResumePhase(ctx) == kResumeRunning) {
            save = HostRet0(ctx);
            exc = save;
            save = HostTempRet0(ctx);
            StoreU32(ctx, frame + 232u, kAssertMessageData);
            if (static_cast<int32_t>(LoadS8(ctx, frame + 271u)) <= -1) {
              DeallocateRawBlock(ctx, LoadU32(ctx, frame + 260u));
            }
            ReleaseNodeTree(ctx, ptr2, LoadU32(ctx, frame + 252u));
            p0 = static_cast<int32_t>(LoadS8(ctx, frame + 247u)) > -1;
            if (p0 != 0u) { goto release_call_result; }
            p0 = LoadU32(ctx, frame + 236u);
            DeallocateRawBlock(ctx, p0);
            goto release_call_result;
          }
          // Merge point hook_lyc_end: the clip list is exhausted.
          hook_lyc_end:;
          if (ResumePhase(ctx) == kResumeRunning) {
            p1 = LoadU32(ctx, flag + 4u);
            if (p1 == 0u) {
              tmp = LoadU32(ctx, flag + 8u);
              if (flag == LoadU32(ctx, tmp)) { goto chain_follow_done_two; }
              exc = flag + 8u;
              chain_follow_two:;
              p1 = LoadU32(ctx, exc);
              exc = p1 + 8u;
              tmp = LoadU32(ctx, p1 + 8u);
              ret = LoadU32(ctx, tmp);
              if (ret != p1) { goto chain_follow_two; }
              goto chain_follow_done_two;
            }
          // Merge point chain_release_two: release the second linked entry chain.
            chain_release_two:;
            tmp = p1;
            p1 = LoadU32(ctx, tmp);
            if (p1 != 0u) { goto chain_release_two; }
          // Merge point chain_follow_done_two.
            chain_follow_done_two:;
            p1 = static_cast<uint32_t>(tmp != aux3);
            if (p1 != 0u) { goto hook_lyc_loop; }
          }
          if (ResumePhase(ctx) == kResumeRunning) {
            tmp = LoadU32(ctx, p0 + 76u);
            p1 = static_cast<uint32_t>(aux3 == tmp);
            if (p1 != 0u) { goto hook_lyc_skip_all; }
            ptr2 = frame + 200u;
          }
          // Merge point hook_video_loop: walk the collected video entries.
          hook_video_loop:;
          if (ResumePhase(ctx) == kResumeRunning) {
            flag = tmp;
            p1 = LoadU32(ctx, tmp + 28u);
            tmp = static_cast<uint32_t>(p1 == LoadU32(ctx, tmp + 32u));
            if (tmp != 0u) { goto hook_video_end; }
          }
          // Merge point hook_video_scan: hook one video entry by name.
          hook_video_scan:;
          if (ResumePhase(ctx) == kResumeRunning) {
            tmp = LoadU8(ctx, p1 + 15u);
            ret = static_cast<int32_t>(static_cast<int8_t>(tmp));
            ptr = static_cast<int32_t>(ret) < 0;
            idx = p1 + 4u;
            exc = LoadU32(ctx, p1 + 8u);
            ptr = static_cast<uint32_t>((ptr != 0u ? exc : tmp) == 3u);
          }
          if (ptr != 0u || ResumePhase(ctx) == kResumeRewinding) {
            if (ResumePhase(ctx) == kResumeRunning) {
              StoreU32(ctx, kDataD015C, 0u);
            }
            if (ResumePhase(ctx) == kResumeRunning || resume_point == 211u) {
              save = HostInvokeIiiiii(ctx, kSelectorNamedProp, idx, 0u, 0xFFFFFFFFu, kNameLyc, 3u);
              resume_point = 211u;
              if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
              ret = save;
            }
            if (ResumePhase(ctx) == kResumeRunning) {
              tmp = LoadU32(ctx, kDataD015C);
              StoreU32(ctx, kDataD015C, 0u);
              if (tmp == 1u) { goto pending_throw_path; }
              tmp = static_cast<uint32_t>(ret == 0u);
              if (tmp != 0u) { goto hook_video_advance; }
              exc = LoadU32(ctx, p1 + 8u);
              tmp = LoadU8(ctx, p1 + 15u);
            }
            ret = tmp;
          }
          if (ResumePhase(ctx) == kResumeRunning) {
            ptr = static_cast<uint32_t>(
              (static_cast<int32_t>(static_cast<int8_t>(ret)) < 0 ? exc : tmp) == 5u);
          }
          if (ptr != 0u || ResumePhase(ctx) == kResumeRewinding) {
            if (ResumePhase(ctx) == kResumeRunning) {
              StoreU32(ctx, kDataD015C, 0u);
            }
            if (ResumePhase(ctx) == kResumeRunning || resume_point == 212u) {
              save = HostInvokeIiiiii(ctx, kSelectorNamedProp, idx, 0u, 0xFFFFFFFFu,
                kNameAnime, 5u);
              resume_point = 212u;
              if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
              ret = save;
            }
            if (ResumePhase(ctx) == kResumeRunning) {
              tmp = LoadU32(ctx, kDataD015C);
              StoreU32(ctx, kDataD015C, 0u);
              if (tmp == 1u) { goto pending_throw_path; }
              tmp = static_cast<uint32_t>(ret == 0u);
              if (tmp != 0u) { goto hook_video_advance; }
              exc = LoadU32(ctx, p1 + 8u);
              tmp = LoadU8(ctx, p1 + 15u);
            }
            ret = tmp;
          }
          if (ResumePhase(ctx) == kResumeRunning) {
            ret = static_cast<int32_t>(static_cast<int8_t>(ret)) < 0;
            tmp = static_cast<uint32_t>((ret != 0u ? exc : tmp) == 5u);
          }
          if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
            if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
              if (ResumePhase(ctx) == kResumeRunning) {
                StoreU32(ctx, kDataD015C, 0u);
              }
              if (ResumePhase(ctx) == kResumeRunning || resume_point == 213u) {
                save = HostInvokeIiiiii(ctx, kSelectorNamedProp, idx, 0u, 0xFFFFFFFFu,
                  kNameVideo, 5u);
                resume_point = 213u;
                if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
                ret = save;
              }
              if (ResumePhase(ctx) == kResumeRunning) {
                tmp = LoadU32(ctx, kDataD015C);
                StoreU32(ctx, kDataD015C, 0u);
                if (tmp == 1u) { goto pending_throw_path; }
                tmp = static_cast<uint32_t>(ret == 0u);
                if (tmp != 0u) { goto hook_video_advance; }
              }
            }
            if (ResumePhase(ctx) == kResumeRunning) {
              exc = LoadU32(ctx, p0 + 148u);
              StoreU32(ctx, kDataD015C, 0u);
              tmp = frame + 184u;
            }
            if (ResumePhase(ctx) == kResumeRunning || resume_point == 214u) {
              save = HostOsQuery(ctx, kSelectorQueryNode, tmp, p1);
              resume_point = 214u;
              if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
              ret = save;
            }
            if (ResumePhase(ctx) == kResumeRunning) {
              tmp = LoadU32(ctx, kDataD015C);
              StoreU32(ctx, kDataD015C, 0u);
              tmp = static_cast<uint32_t>(tmp == 1u);
              if (tmp != 0u) { goto throw_release; }
              StoreU32(ctx, kDataD015C, 0u);
            }
            if (ResumePhase(ctx) == kResumeRunning || resume_point == 215u) {
              HostSyncSlot(ctx, kSelectorSyncField, exc, ret, 1u);
              #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
              SyncMemoryBase(ctx);
              #endif
              resume_point = 215u;
              if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
            }
            if (ResumePhase(ctx) == kResumeRunning) {
              tmp = LoadU32(ctx, kDataD015C);
              StoreU32(ctx, kDataD015C, 0u);
              tmp = static_cast<uint32_t>(tmp == 1u);
              if (tmp != 0u) { goto throw_free_video_result; }
              StoreU32(ctx, frame + 184u, kAssertMessageData);
              if (static_cast<int32_t>(LoadS8(ctx, frame + 223u)) <= -1) {
                DeallocateRawBlock(ctx, LoadU32(ctx, frame + 212u));
              }
              ReleaseNodeTree(ctx, ptr2, LoadU32(ctx, frame + 204u));
              tmp = static_cast<int32_t>(LoadS8(ctx, frame + 199u)) > -1;
              if (tmp != 0u) { goto hook_video_advance; }
              tmp = LoadU32(ctx, frame + 188u);
              DeallocateRawBlock(ctx, tmp);
            }
            // Merge point hook_video_advance: step to the next video entry.
            hook_video_advance:;
            if (ResumePhase(ctx) == kResumeRunning) {
              p1 = (p1 + 44u);
              tmp = static_cast<uint32_t>(p1 != LoadU32(ctx, flag + 32u));
              if (tmp != 0u) { goto hook_video_scan; }
              goto hook_video_end;
            }
            // Merge point throw_free_video_result: unwind the video entry buffers.
            throw_free_video_result:;
            if (ResumePhase(ctx) == kResumeRunning) {
              save = HostRet0(ctx);
              exc = save;
              save = HostTempRet0(ctx);
              StoreU32(ctx, frame + 184u, kAssertMessageData);
              if (static_cast<int32_t>(LoadS8(ctx, frame + 223u)) <= -1) {
                DeallocateRawBlock(ctx, LoadU32(ctx, frame + 212u));
              }
              ReleaseNodeTree(ctx, ptr2, LoadU32(ctx, frame + 204u));
              p0 = static_cast<int32_t>(LoadS8(ctx, frame + 199u)) > -1;
              if (p0 != 0u) { goto release_call_result; }
              p0 = LoadU32(ctx, frame + 188u);
              DeallocateRawBlock(ctx, p0);
              goto release_call_result;
            }
            // Merge point hook_video_end: the video entry list is exhausted.
            hook_video_end:;
            if (ResumePhase(ctx) == kResumeRunning) {
              p1 = LoadU32(ctx, flag + 4u);
              if (p1 == 0u) {
                tmp = LoadU32(ctx, flag + 8u);
                if (flag == LoadU32(ctx, tmp)) { goto video_list_done; }
                exc = (flag + 8u);
              // Merge point video_chain_walk: follow the shared chain links.
                video_chain_walk:;
                p1 = LoadU32(ctx, exc);
                exc = (p1 + 8u);
                tmp = LoadU32(ctx, p1 + 8u);
                ret = LoadU32(ctx, tmp);
                if (ret != p1) { goto video_chain_walk; }
                goto video_list_done;
              }
              // Merge point video_list_walk: walk to the tail of the entry list.
              video_list_walk:;
              tmp = p1;
              p1 = LoadU32(ctx, tmp);
              if (p1 != 0u) { goto video_list_walk; }
              // Merge point video_list_done: compare the tail with the list head.
              video_list_done:;
              p1 = static_cast<uint32_t>(tmp != aux3);
              if (p1 != 0u) { goto hook_video_loop; }
            }
            hook_lyc_skip_all:;
            if (ResumePhase(ctx) == kResumeRunning) {
              p1 = LoadU32(ctx, p0 + 148u);
              tmp = LoadU32(ctx, p1 + 13668u);
              ret = static_cast<uint32_t>(tmp != (p1 + 13672u));
            }
            if (ret != 0u || ResumePhase(ctx) == kResumeRewinding) {
            // Merge point lyc_entry_loop: walk the layer clip entries.
              lyc_entry_loop:;
              if (ResumePhase(ctx) == kResumeRunning) {
                ret = tmp;
                flag = (tmp + 36u);
                exc = LoadU32(ctx, tmp + 40u);
                tmp = LoadU8(ctx, tmp + 47u);
                ptr = static_cast<int32_t>(static_cast<int8_t>(tmp)) < 0;
                tmp = static_cast<uint32_t>(ptr != 0u ? exc : tmp);
              }
              cmp17 = (ResumePhase(ctx) == kResumeRunning) ? cmp17 : tmp;
              if ((cmp17 == 0u) || ResumePhase(ctx) == kResumeRewinding) {
                if (ResumePhase(ctx) == kResumeRunning) {
                  StoreU32(ctx, kDataD015C, 0u);
                }
                if (ResumePhase(ctx) == kResumeRunning || resume_point == 216u) {
                  save = HostInvokeIiiiii(ctx, kSelectorNamedProp, flag, 0u, 0xFFFFFFFFu,
                    kMsgEmpty, 0u);
                  resume_point = 216u;
                  if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
                  tmp = save;
                }
                if (ResumePhase(ctx) == kResumeRunning) {
                  p1 = LoadU32(ctx, kDataD015C);
                  StoreU32(ctx, kDataD015C, 0u);
                  if (p1 == 1u) { goto pending_throw_path; }
                  p1 = static_cast<uint32_t>(tmp == 0u);
                  if (p1 != 0u) { goto slot_release_done; }
                  p1 = LoadU32(ctx, p0 + 148u);
                }
              }
              if (ResumePhase(ctx) == kResumeRunning) {
                p1 = LoadU32(ctx, p1 + 16u);
                tmp = static_cast<uint32_t>(p1 == 0u);
              }
              if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
                if (ResumePhase(ctx) == kResumeRunning) {
                  StoreU32(ctx, kDataD015C, 0u);
                }
                if (ResumePhase(ctx) == kResumeRunning || resume_point == 217u) {
                  HostFailAssertion(ctx, 857u, 772013u, 772021u, 734u, 772077u);
                  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
                  SyncMemoryBase(ctx);
                  #endif
                  resume_point = 217u;
                  if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
                }
                if (ResumePhase(ctx) == kResumeRunning) {
                  p0 = LoadU32(ctx, kDataD015C);
                  StoreU32(ctx, kDataD015C, 0u);
                  p0 = static_cast<uint32_t>(p0 != 1u);
                  if (p0 != 0u) { goto unreachable_path; }
                  goto pending_throw_path;
                }
              }
              if (ResumePhase(ctx) == kResumeRunning) {
                tmp = LoadU32(ctx, p1 + 24u);
                p1 = static_cast<uint32_t>(tmp == 0u);
              }
              if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
                if (ResumePhase(ctx) == kResumeRunning) {
                  StoreU32(ctx, kDataD015C, 0u);
                }
                if (ResumePhase(ctx) == kResumeRunning || resume_point == 218u) {
                  HostFailAssertion(ctx, 857u, 772013u, 772021u, 734u, 772077u);
                  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
                  SyncMemoryBase(ctx);
                  #endif
                  resume_point = 218u;
                  if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
                }
                if (ResumePhase(ctx) == kResumeRunning) {
                  p0 = LoadU32(ctx, kDataD015C);
                  StoreU32(ctx, kDataD015C, 0u);
                  p0 = static_cast<uint32_t>(p0 != 1u);
                  if (p0 != 0u) { goto unreachable_path; }
                  goto pending_throw_path;
                }
              }
              if (ResumePhase(ctx) == kResumeRunning) {
                p1 = LoadU32(ctx, LoadU32(ctx, tmp) + 304u);
                StoreU32(ctx, kDataD015C, 0u);
                exc = frame + 280u;
              }
              if (ResumePhase(ctx) == kResumeRunning || resume_point == 219u) {
                HostSyncSlot(ctx, p1, exc, tmp, flag);
                #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
                SyncMemoryBase(ctx);
                #endif
                resume_point = 219u;
                if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
              }
              if (ResumePhase(ctx) == kResumeRunning) {
                p1 = LoadU32(ctx, kDataD015C);
                StoreU32(ctx, kDataD015C, 0u);
                p1 = static_cast<uint32_t>(p1 != 1u);
              }
              if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
                if (ResumePhase(ctx) == kResumeRunning) {
                  tmp = LoadU32(ctx, frame + 280u);
                  if (tmp == 0u) {
                    p1 = LoadU32(ctx, frame + 284u);
                    goto slot_pair_guard;
                  }
                  p1 = LoadU32(ctx, frame + 284u);
                  flag = LoadU32(ctx, LoadU32(ctx, tmp) - 152u) + tmp;
                  tmp = LoadU32(ctx, LoadU32(ctx, flag) + 124u);
                  StoreU32(ctx, kDataD015C, 0u);
                }
                if (ResumePhase(ctx) == kResumeRunning || resume_point == 220u) {
                  HostOsSchedule(ctx, tmp, flag, 0u);
                  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
                  SyncMemoryBase(ctx);
                  #endif
                  resume_point = 220u;
                  if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
                }
                if (ResumePhase(ctx) == kResumeRunning) {
                  tmp = LoadU32(ctx, kDataD015C);
                  StoreU32(ctx, kDataD015C, 0u);
                  tmp = static_cast<uint32_t>(tmp != 1u);
                  if (tmp != 0u) { goto slot_pair_guard; }
                  save = HostRet0(ctx);
                  exc = save;
                  save = HostTempRet0(ctx);
                  p0 = static_cast<uint32_t>(p1 == 0u);
                  if (p0 != 0u) { goto release_call_result; }
                  p0 = LoadU32(ctx, p1 + 4u);
                  tmp = p0 - 1u;
                  StoreU32(ctx, p1 + 4u, tmp);
                  p0 = static_cast<uint32_t>(p0 != 1u);
                  if (p0 != 0u) { goto release_call_result; }
                  p0 = LoadU32(ctx, LoadU32(ctx, p1) + 8u);
                  StoreU32(ctx, kDataD015C, 0u);
                }
                if (ResumePhase(ctx) == kResumeRunning || resume_point == 221u) {
                  HostReleaseSlotPair(ctx, p0, p1);
                  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
                  SyncMemoryBase(ctx);
                  #endif
                  resume_point = 221u;
                  if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
                }
                if (ResumePhase(ctx) == kResumeRunning) {
                  p0 = LoadU32(ctx, kDataD015C);
                  StoreU32(ctx, kDataD015C, 0u);
                  p0 = static_cast<uint32_t>(p0 != 1u);
                }
                if (p0 != 0u || ResumePhase(ctx) == kResumeRewinding) {
                  if (ResumePhase(ctx) == kResumeRunning) {
                    p0 = LoadU32(ctx, p1 + 8u);
                    tmp = p0 - 1u;
                    StoreU32(ctx, p1 + 8u, tmp);
                    p0 = static_cast<uint32_t>(p0 != 1u);
                    if (p0 != 0u) { goto release_call_result; }
                    p0 = LoadU32(ctx, LoadU32(ctx, p1) + 12u);
                    StoreU32(ctx, kDataD015C, 0u);
                  }
                  if (ResumePhase(ctx) == kResumeRunning || resume_point == 222u) {
                    HostReleaseSlotPair(ctx, p0, p1);
                    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
                    SyncMemoryBase(ctx);
                    #endif
                    resume_point = 222u;
                    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
                  }
                  if (ResumePhase(ctx) == kResumeRunning) {
                    p0 = LoadU32(ctx, kDataD015C);
                    StoreU32(ctx, kDataD015C, 0u);
                    p0 = static_cast<uint32_t>(p0 != 1u);
                    if (p0 != 0u) { goto release_call_result; }
                  }
                }
                if (ResumePhase(ctx) == kResumeRunning) { goto pending_throw_path; }
              }
              if (ResumePhase(ctx) == kResumeRunning) {
                save = HostRet0(ctx);
                exc = save;
                save = HostTempRet0(ctx);
                p0 = save;
                goto release_call_result;
              }
                // Merge point slot_pair_guard: the schedule call did not suspend.
              slot_pair_guard:;
              if (ResumePhase(ctx) == kResumeRunning) {
                tmp = static_cast<uint32_t>(p1 == 0u);
                if (tmp != 0u) { goto slot_release_done; }
                tmp = LoadU32(ctx, p1 + 4u);
                flag = tmp - 1u;
                StoreU32(ctx, p1 + 4u, flag);
                tmp = static_cast<uint32_t>(tmp != 1u);
                if (tmp != 0u) { goto slot_release_done; }
                tmp = LoadU32(ctx, LoadU32(ctx, p1) + 8u);
                StoreU32(ctx, kDataD015C, 0u);
              }
              if (ResumePhase(ctx) == kResumeRunning || resume_point == 223u) {
                HostReleaseSlotPair(ctx, tmp, p1);
                #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
                SyncMemoryBase(ctx);
                #endif
                resume_point = 223u;
                if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
              }
              if (ResumePhase(ctx) == kResumeRunning) {
                tmp = LoadU32(ctx, kDataD015C);
                StoreU32(ctx, kDataD015C, 0u);
                tmp = static_cast<uint32_t>(tmp != 1u);
              }
              if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
                if (ResumePhase(ctx) == kResumeRunning) {
                  tmp = LoadU32(ctx, p1 + 8u);
                  flag = tmp - 1u;
                  StoreU32(ctx, p1 + 8u, flag);
                  tmp = static_cast<uint32_t>(tmp != 1u);
                  if (tmp != 0u) { goto slot_release_done; }
                  tmp = LoadU32(ctx, LoadU32(ctx, p1) + 12u);
                  StoreU32(ctx, kDataD015C, 0u);
                }
                if (ResumePhase(ctx) == kResumeRunning || resume_point == 224u) {
                  HostReleaseSlotPair(ctx, tmp, p1);
                  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
                  SyncMemoryBase(ctx);
                  #endif
                  resume_point = 224u;
                  if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
                }
                if (ResumePhase(ctx) == kResumeRunning) {
                  p1 = LoadU32(ctx, kDataD015C);
                  StoreU32(ctx, kDataD015C, 0u);
                  p1 = static_cast<uint32_t>(p1 != 1u);
                  if (p1 != 0u) { goto slot_release_done; }
                }
              }
              if (ResumePhase(ctx) == kResumeRunning) { goto pending_throw_path; }
              // Merge point slot_release_done: the slot pair needs no release.
              slot_release_done:;
              if (ResumePhase(ctx) == kResumeRunning) {
                flag = ret + 48u;
                p1 = LoadU8(ctx, ret + 59u);
                exc = static_cast<int32_t>(static_cast<int8_t>(p1)) < 0;
                tmp = LoadU32(ctx, ret + 52u);
                p1 = static_cast<uint32_t>((exc != 0u ? tmp : p1) == 0u);
              }
              if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
                if (ResumePhase(ctx) == kResumeRunning) {
                  StoreU32(ctx, kDataD015C, 0u);
                }
                if (ResumePhase(ctx) == kResumeRunning || resume_point == 225u) {
                  save = HostInvokeIiiiii(ctx, kSelectorNamedProp, flag, 0u, 0xFFFFFFFFu,
                kMsgEmpty, 0u);
                  resume_point = 225u;
                  if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
                  tmp = save;
                }
                if (ResumePhase(ctx) == kResumeRunning) {
                  p1 = LoadU32(ctx, kDataD015C);
                  StoreU32(ctx, kDataD015C, 0u);
                  if (p1 == 1u) { goto pending_throw_path; }
                  p1 = static_cast<uint32_t>(tmp == 0u);
                  if (p1 != 0u) { goto lyc_entry_advance; }
                }
              }
              if (ResumePhase(ctx) == kResumeRunning) {
                p1 = LoadU32(ctx, LoadU32(ctx, p0 + 148u) + 16u);
                tmp = static_cast<uint32_t>(p1 == 0u);
              }
              if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
                if (ResumePhase(ctx) == kResumeRunning) {
                  StoreU32(ctx, kDataD015C, 0u);
                }
                if (ResumePhase(ctx) == kResumeRunning || resume_point == 226u) {
                  HostFailAssertion(ctx, 857u, 772013u, 772021u, 734u, 772077u);
                  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
                  SyncMemoryBase(ctx);
                  #endif
                  resume_point = 226u;
                  if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
                }
                if (ResumePhase(ctx) == kResumeRunning) {
                  p0 = LoadU32(ctx, kDataD015C);
                  StoreU32(ctx, kDataD015C, 0u);
                  p0 = static_cast<uint32_t>(p0 != 1u);
                  if (p0 != 0u) { goto unreachable_path; }
                  goto pending_throw_path;
                }
              }
              if (ResumePhase(ctx) == kResumeRunning) {
                tmp = LoadU32(ctx, p1 + 24u);
                p1 = static_cast<uint32_t>(tmp == 0u);
              }
              if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
                if (ResumePhase(ctx) == kResumeRunning) {
                  StoreU32(ctx, kDataD015C, 0u);
                }
                if (ResumePhase(ctx) == kResumeRunning || resume_point == 227u) {
                  HostFailAssertion(ctx, 857u, 772013u, 772021u, 734u, 772077u);
                  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
                  SyncMemoryBase(ctx);
                  #endif
                  resume_point = 227u;
                  if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
                }
                if (ResumePhase(ctx) == kResumeRunning) {
                  p0 = LoadU32(ctx, kDataD015C);
                  StoreU32(ctx, kDataD015C, 0u);
                  p0 = static_cast<uint32_t>(p0 != 1u);
                  if (p0 != 0u) { goto unreachable_path; }
                  goto pending_throw_path;
                }
              }
              if (ResumePhase(ctx) == kResumeRunning) {
                p1 = LoadU32(ctx, LoadU32(ctx, tmp) + 304u);
                StoreU32(ctx, kDataD015C, 0u);
                exc = frame + 280u;
              }
              if (ResumePhase(ctx) == kResumeRunning || resume_point == 228u) {
                HostSyncSlot(ctx, p1, exc, tmp, flag);
                #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
                SyncMemoryBase(ctx);
                #endif
                resume_point = 228u;
                if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
              }
              if (ResumePhase(ctx) == kResumeRunning) {
                p1 = LoadU32(ctx, kDataD015C);
                StoreU32(ctx, kDataD015C, 0u);
                p1 = static_cast<uint32_t>(p1 != 1u);
              }
              if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
                if (ResumePhase(ctx) == kResumeRunning) {
                  tmp = LoadU32(ctx, frame + 280u);
                  if (tmp == 0u) {
                    p1 = LoadU32(ctx, frame + 284u);
                    goto slot_release_guard;
                  }
                  p1 = LoadU32(ctx, frame + 284u);
                  flag = LoadU32(ctx, LoadU32(ctx, tmp) - 152u) + tmp;
                  tmp = LoadU32(ctx, LoadU32(ctx, flag) + 124u);
                  StoreU32(ctx, kDataD015C, 0u);
                }
                if (ResumePhase(ctx) == kResumeRunning || resume_point == 229u) {
                  HostOsSchedule(ctx, tmp, flag, 0u);
                  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
                  SyncMemoryBase(ctx);
                  #endif
                  resume_point = 229u;
                  if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
                }
                if (ResumePhase(ctx) == kResumeRunning) {
                  tmp = LoadU32(ctx, kDataD015C);
                  StoreU32(ctx, kDataD015C, 0u);
                  tmp = static_cast<uint32_t>(tmp != 1u);
                  if (tmp != 0u) { goto slot_release_guard; }
                  save = HostRet0(ctx);
                  exc = save;
                  save = HostTempRet0(ctx);
                  p0 = static_cast<uint32_t>(p1 == 0u);
                  if (p0 != 0u) { goto release_call_result; }
                  p0 = LoadU32(ctx, p1 + 4u);
                  tmp = p0 - 1u;
                  StoreU32(ctx, p1 + 4u, tmp);
                  p0 = static_cast<uint32_t>(p0 != 1u);
                  if (p0 != 0u) { goto release_call_result; }
                  p0 = LoadU32(ctx, LoadU32(ctx, p1) + 8u);
                  StoreU32(ctx, kDataD015C, 0u);
                }
                if (ResumePhase(ctx) == kResumeRunning || resume_point == 230u) {
                  HostReleaseSlotPair(ctx, p0, p1);
                  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
                  SyncMemoryBase(ctx);
                  #endif
                  resume_point = 230u;
                  if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
                }
                if (ResumePhase(ctx) == kResumeRunning) {
                  p0 = LoadU32(ctx, kDataD015C);
                  StoreU32(ctx, kDataD015C, 0u);
                  p0 = static_cast<uint32_t>(p0 != 1u);
                }
                if (p0 != 0u || ResumePhase(ctx) == kResumeRewinding) {
                  if (ResumePhase(ctx) == kResumeRunning) {
                    p0 = LoadU32(ctx, p1 + 8u);
                    tmp = p0 - 1u;
                    StoreU32(ctx, p1 + 8u, tmp);
                    p0 = static_cast<uint32_t>(p0 != 1u);
                    if (p0 != 0u) { goto release_call_result; }
                    p0 = LoadU32(ctx, LoadU32(ctx, p1) + 12u);
                    StoreU32(ctx, kDataD015C, 0u);
                  }
                  if (ResumePhase(ctx) == kResumeRunning || resume_point == 231u) {
                    HostReleaseSlotPair(ctx, p0, p1);
                    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
                    SyncMemoryBase(ctx);
                    #endif
                    resume_point = 231u;
                    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
                  }
                  if (ResumePhase(ctx) == kResumeRunning) {
                    p0 = LoadU32(ctx, kDataD015C);
                    StoreU32(ctx, kDataD015C, 0u);
                    p0 = static_cast<uint32_t>(p0 != 1u);
                    if (p0 != 0u) { goto release_call_result; }
                  }
                }
                if (ResumePhase(ctx) == kResumeRunning) { goto pending_throw_path; }
              }
              if (ResumePhase(ctx) == kResumeRunning) {
                save = HostRet0(ctx);
                exc = save;
                save = HostTempRet0(ctx);
                p0 = save;
                goto release_call_result;
              }
                // Merge point slot_release_guard: the schedule call did not suspend.
              slot_release_guard:;
              if (ResumePhase(ctx) == kResumeRunning) {
                tmp = static_cast<uint32_t>(p1 == 0u);
                if (tmp != 0u) { goto lyc_entry_advance; }
                tmp = LoadU32(ctx, p1 + 4u);
                flag = tmp - 1u;
                StoreU32(ctx, p1 + 4u, flag);
                tmp = static_cast<uint32_t>(tmp != 1u);
                if (tmp != 0u) { goto lyc_entry_advance; }
                tmp = LoadU32(ctx, LoadU32(ctx, p1) + 8u);
                StoreU32(ctx, kDataD015C, 0u);
              }
              if (ResumePhase(ctx) == kResumeRunning || resume_point == 232u) {
                HostReleaseSlotPair(ctx, tmp, p1);
                #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
                SyncMemoryBase(ctx);
                #endif
                resume_point = 232u;
                if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
              }
              if (ResumePhase(ctx) == kResumeRunning) {
                tmp = LoadU32(ctx, kDataD015C);
                StoreU32(ctx, kDataD015C, 0u);
                tmp = static_cast<uint32_t>(tmp != 1u);
              }
              if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
                if (ResumePhase(ctx) == kResumeRunning) {
                  tmp = LoadU32(ctx, p1 + 8u);
                  flag = tmp - 1u;
                  StoreU32(ctx, p1 + 8u, flag);
                  tmp = static_cast<uint32_t>(tmp != 1u);
                  if (tmp != 0u) { goto lyc_entry_advance; }
                  tmp = LoadU32(ctx, LoadU32(ctx, p1) + 12u);
                  StoreU32(ctx, kDataD015C, 0u);
                }
                if (ResumePhase(ctx) == kResumeRunning || resume_point == 233u) {
                  HostReleaseSlotPair(ctx, tmp, p1);
                  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
                  SyncMemoryBase(ctx);
                  #endif
                  resume_point = 233u;
                  if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
                }
                if (ResumePhase(ctx) == kResumeRunning) {
                  p1 = LoadU32(ctx, kDataD015C);
                  StoreU32(ctx, kDataD015C, 0u);
                  p1 = static_cast<uint32_t>(p1 != 1u);
                  if (p1 != 0u) { goto lyc_entry_advance; }
                }
              }
              if (ResumePhase(ctx) == kResumeRunning) { goto pending_throw_path; }
              // Merge point lyc_entry_advance: advance to the next clip entry.
              lyc_entry_advance:;
              if (ResumePhase(ctx) == kResumeRunning) {
                p1 = LoadU32(ctx, ret + 4u);
                if (p1 == 0u) {
                  tmp = LoadU32(ctx, ret + 8u);
                  if (ret == LoadU32(ctx, tmp)) { goto lyc_list_done; }
                  exc = ret + 8u;
                // Merge point lyc_chain_walk: follow the shared chain links.
                  lyc_chain_walk:;
                  p1 = LoadU32(ctx, exc);
                  exc = p1 + 8u;
                  tmp = LoadU32(ctx, p1 + 8u);
                  if (LoadU32(ctx, tmp) != p1) { goto lyc_chain_walk; }
                  goto lyc_list_done;
                }
                // Merge point lyc_list_walk: walk to the tail of the entry list.
                lyc_list_walk:;
                tmp = p1;
                p1 = LoadU32(ctx, tmp);
                if (p1 != 0u) { goto lyc_list_walk; }
                // Merge point lyc_list_done: the entry list is fully released.
                lyc_list_done:;
                p1 = LoadU32(ctx, p0 + 148u);
                ret = static_cast<uint32_t>((p1 + 13672u) != tmp);
                if (ret != 0u) { goto lyc_entry_loop; }
              }
            }
            if (ResumePhase(ctx) == kResumeRunning) {
              tmp = static_cast<uint32_t>(LoadU8(ctx, str15) == 0u);
            }
            if (ResumePhase(ctx) == kResumeRunning) {
              if (tmp != 0u) { goto lyc_done; }
              StoreU32(ctx, kDataD015C, 0u);
              ret = p0 + 104u;
              tmp = frame + 136u;
            }
            if (ResumePhase(ctx) == kResumeRunning || resume_point == 234u) {
              save = HostOsQuery(ctx, kSelectorQueryNode, tmp, ret);
              resume_point = 234u;
              if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
              ret = save;
            }
            if (ResumePhase(ctx) == kResumeRunning) {
              tmp = LoadU32(ctx, kDataD015C);
              StoreU32(ctx, kDataD015C, 0u);
              tmp = static_cast<uint32_t>(tmp == 1u);
              if (tmp != 0u) { goto list_walk_abort; }
              StoreU32(ctx, kDataD015C, 0u);
            }
            if (ResumePhase(ctx) == kResumeRunning || resume_point == 235u) {
              HostSyncSlot(ctx, kSelectorSyncField, p1, ret, 1u);
              #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
              SyncMemoryBase(ctx);
              #endif
              resume_point = 235u;
              if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
            }
            if (ResumePhase(ctx) == kResumeRunning) {
              p1 = LoadU32(ctx, kDataD015C);
              StoreU32(ctx, kDataD015C, 0u);
              if (p1 == 1u) { goto throw_release_root; }
              StoreU32(ctx, ret, kAssertMessageData);
              if (static_cast<int32_t>(LoadS8(ctx, ret + 39u)) <= -1) {
                DeallocateRawBlock(ctx, LoadU32(ctx, ret + 28u));
              }
              tmp = LoadU32(ctx, ret + 20u);
              ReleaseNodeTree(ctx, (ret + 16u), tmp);
              p1 = static_cast<int32_t>(LoadS8(ctx, ret + 15u)) > -1;
              if (p1 != 0u) { goto lyc_done; }
              p1 = LoadU32(ctx, ret + 4u);
              DeallocateRawBlock(ctx, p1);
            }
            lyc_done:;
            if (ResumePhase(ctx) == kResumeRunning) {
              tmp = LoadU32(ctx, p0 + 88u);
              ptr2 = p0 + 92u;
              p1 = static_cast<uint32_t>(tmp != ptr2);
            }
            if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
              idx = static_cast<uint32_t>(ResumePhase(ctx) != 0u ? idx : (frame + 104u));
              lyc_chain_retry:;
              if (ResumePhase(ctx) == kResumeRunning) {
                if (i0 != 0u) {
                  flag = LoadU32(ctx, p0 + 148u);
                  StoreU32(ctx, kDataD015C, 0u);
                  ret = tmp;
                  tmp = (tmp + 28u);
                  p1 = (frame + 88u);
                }
                if (ResumePhase(ctx) == kResumeRunning || resume_point == 236u) {
                  save = HostOsQuery(ctx, kSelectorQueryNode, p1, tmp);
                  resume_point = 236u;
                  if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
                  tmp = save;
                }
                if (ResumePhase(ctx) == kResumeRunning) {
                  p1 = LoadU32(ctx, kDataD015C);
                  StoreU32(ctx, kDataD015C, 0u);
                  p1 = static_cast<uint32_t>(p1 == 1u);
                  if (p1 != 0u) { goto throw_release; }
                  StoreU32(ctx, kDataD015C, 0u);
                }
                if (ResumePhase(ctx) == kResumeRunning || resume_point == 237u) {
                  HostSyncSlot(ctx, kSelectorSyncField, flag, tmp, 1u);
                  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
                  SyncMemoryBase(ctx);
                  #endif
                  resume_point = 237u;
                  if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
                }
                if (ResumePhase(ctx) == kResumeRunning) {
                  p1 = LoadU32(ctx, kDataD015C);
                  StoreU32(ctx, kDataD015C, 0u);
                  p1 = static_cast<uint32_t>(p1 == 1u);
                  if (p1 != 0u) { goto throw_free_hook_assert; }
                  StoreU32(ctx, frame + 88u, kAssertMessageData);
                  if (static_cast<int32_t>(LoadS8(ctx, frame + 127u)) <= -1) {
                    DeallocateRawBlock(ctx, LoadU32(ctx, frame + 116u));
                  }
                  ReleaseNodeTree(ctx, idx, LoadU32(ctx, frame + 108u));
                  if (static_cast<int32_t>(LoadS8(ctx, frame + 103u)) <= -1) {
                    DeallocateRawBlock(ctx, LoadU32(ctx, frame + 92u));
                  }
                  p1 = LoadU32(ctx, ret + 4u);
                  if (p1 == 0u) {
                    tmp = LoadU32(ctx, ret + 8u);
                    if (ret == LoadU32(ctx, tmp)) { goto name_lookup_done; }
                    exc = (ret + 8u);
                    hash_link_step:;
                    p1 = LoadU32(ctx, exc);
                    exc = (p1 + 8u);
                    tmp = LoadU32(ctx, p1 + 8u);
                    ret = LoadU32(ctx, tmp);
                    if (ret != p1) { goto hash_link_step; }
                    goto name_lookup_done;
                  }
                  chain_tail_walk:;
                  tmp = p1;
                  p1 = LoadU32(ctx, tmp);
                  if (p1 != 0u) { goto chain_tail_walk; }
                  name_lookup_done:;
                  p1 = static_cast<uint32_t>(tmp != ptr2);
                  if (p1 != 0u) { goto lyc_chain_retry; }
                }
              }
              if (ResumePhase(ctx) == kResumeRunning) {
                p1 = LoadU32(ctx, p0 + 148u);
                StoreU32(ctx, kDataD015C, 0u);
              }
              if (ResumePhase(ctx) == kResumeRunning || resume_point == 238u) {
                HostReleaseSlotPair(ctx, 3545u, p1);
                #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
                SyncMemoryBase(ctx);
                #endif
                resume_point = 238u;
                if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
              }
              if (ResumePhase(ctx) == kResumeRunning) {
                p1 = LoadU32(ctx, kDataD015C);
                StoreU32(ctx, kDataD015C, 0u);
                if (p1 == 1u) { goto list_walk_abort; }
                p1 = LoadU32(ctx, p0 + 148u);
                StoreU32(ctx, kDataD015C, 0u);
              }
              if (ResumePhase(ctx) == kResumeRunning || resume_point == 239u) {
                HostSyncSlot(ctx, 2626u, p1, 0u, 0u);
                #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
                SyncMemoryBase(ctx);
                #endif
                resume_point = 239u;
                if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
              }
              if (ResumePhase(ctx) == kResumeRunning) {
                p1 = LoadU32(ctx, kDataD015C);
                StoreU32(ctx, kDataD015C, 0u);
                if (p1 == 1u) { goto list_walk_abort; }
                p1 = LoadU32(ctx, p0 + 148u);
                StoreU32(ctx, kDataD015C, 0u);
              }
              if (ResumePhase(ctx) == kResumeRunning || resume_point == 240u) {
                HostSyncSlot(ctx, 2627u, p1, 0u, 0u);
                #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
                SyncMemoryBase(ctx);
                #endif
                resume_point = 240u;
                if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
              }
              if (ResumePhase(ctx) == kResumeRunning) {
                p1 = LoadU32(ctx, kDataD015C);
                StoreU32(ctx, kDataD015C, 0u);
                p1 = static_cast<uint32_t>(p1 == 1u);
                if (p1 != 0u) { goto list_walk_abort; }
                p0 = LoadU32(ctx, p0 + 148u);
                StoreU32(ctx, kDataD015C, 0u);
              }
              if (ResumePhase(ctx) == kResumeRunning || resume_point == 241u) {
                HostSyncSlot(ctx, 2628u, p0, 0u, 0u);
                #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
                SyncMemoryBase(ctx);
                #endif
                resume_point = 241u;
                if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
              }
              if (ResumePhase(ctx) == kResumeRunning) {
                p0 = LoadU32(ctx, kDataD015C);
                StoreU32(ctx, kDataD015C, 0u);
                p0 = static_cast<uint32_t>(p0 == 1u);
                if (p0 != 0u) { goto list_walk_abort; }
                if (static_cast<int32_t>(LoadS8(ctx, aux + 11u)) <= -1) {
                  DeallocateRawBlock(ctx, LoadU32(ctx, aux));
                }
                p1 = LoadU32(ctx, frame + 456u);
                p0 = static_cast<uint32_t>(p1 == 0u);
                if (p0 != 0u) { goto restore_stack_exit; }
                tmp = LoadU32(ctx, frame + 460u);
                p0 = static_cast<uint32_t>(tmp != p1);
                if (p0 != 0u) { goto free_result_chain; }
                goto free_result_buffer;
              }
              throw_free_hook_assert:;
              if (ResumePhase(ctx) == kResumeRunning) {
                save = HostRet0(ctx);
                exc = save;
                save = HostTempRet0(ctx);
                StoreU32(ctx, frame + 88u, kAssertMessageData);
                if (static_cast<int32_t>(LoadS8(ctx, frame + 127u)) <= -1) {
                  DeallocateRawBlock(ctx, LoadU32(ctx, frame + 116u));
                }
                ReleaseNodeTree(ctx, idx, LoadU32(ctx, frame + 108u));
                p0 = static_cast<int32_t>(LoadS8(ctx, frame + 103u)) > -1;
                if (p0 != 0u) { goto release_call_result; }
                p0 = LoadU32(ctx, frame + 92u);
                DeallocateRawBlock(ctx, p0);
                goto release_call_result;
              }
              free_result_chain:;
              if (ResumePhase(ctx) == kResumeRunning) {
                chain_free_step:;
                p0 = (tmp - 12u);
                if (static_cast<int32_t>(LoadS8(ctx, (tmp - 1u))) <= -1) {
                  DeallocateRawBlock(ctx, LoadU32(ctx, p0));
                }
                tmp = p0;
                if (tmp != p1) { goto chain_free_step; }
              }
              free_result_buffer:;
              p0 = i0;
              if (ResumePhase(ctx) == kResumeRunning) {
                StoreU32(ctx, frame + 460u, p1);
                DeallocateRawBlock(ctx, p0);
              }
              restore_stack_exit:;
              if (ResumePhase(ctx) == kResumeRunning) {
                GuestStackSp(ctx) = frame + kGuestFrameBytes;
                goto done;
              }
              throw_release_root:;
              if (ResumePhase(ctx) == kResumeRunning) {
                save = HostRet0(ctx);
                exc = save;
                save = HostTempRet0(ctx);
                StoreU32(ctx, ret, kAssertMessageData);
                if (static_cast<int32_t>(LoadS8(ctx, ret + 39u)) <= -1) {
                  DeallocateRawBlock(ctx, LoadU32(ctx, ret + 28u));
                }
                ReleaseNodeTree(ctx, (ret + 16u), LoadU32(ctx, ret + 20u));
                p0 = static_cast<int32_t>(LoadS8(ctx, ret + 15u)) > -1;
                if (p0 != 0u) { goto release_call_result; }
                p0 = LoadU32(ctx, ret + 4u);
                DeallocateRawBlock(ctx, p0);
                goto release_call_result;
              }
            }
            if (ResumePhase(ctx) == kResumeRunning) {
              save = HostRet0(ctx);
              exc = save;
              save = HostTempRet0(ctx);
              p0 = static_cast<int32_t>(LoadS8(ctx, frame + 291u)) > -1;
              if (p0 != 0u) { goto release_call_result; }
              p0 = LoadU32(ctx, frame + 280u);
              DeallocateRawBlock(ctx, p0);
              goto release_call_result;
            }
          }
          if (ResumePhase(ctx) == kResumeRunning) {
            save = HostRet0(ctx);
            exc = save;
            save = HostTempRet0(ctx);
            p0 = save;
            goto release_call_result;
          }
          list_walk_abort:;
          if (ResumePhase(ctx) == kResumeRunning) {
            save = HostRet0(ctx);
            exc = save;
            save = HostTempRet0(ctx);
            p0 = save;
          }
          release_call_result:;
          if (ResumePhase(ctx) == kResumeRunning) {
            p0 = static_cast<int32_t>(LoadS8(ctx, aux + 11u)) > -1;
            if (p0 != 0u) { goto exception_reload; }
            p0 = LoadU32(ctx, aux);
            DeallocateRawBlock(ctx, p0);
            goto exception_reload;
          }
          exception_bridge_d:;
          if (ResumePhase(ctx) == kResumeRunning) {
            save = HostRet0(ctx);
            exc = save;
            save = HostTempRet0(ctx);
            StoreU32(ctx, frame + 360u, kAssertMessageData);
            if (static_cast<int32_t>(LoadS8(ctx, frame + 399u)) <= -1) {
              DeallocateRawBlock(ctx, LoadU32(ctx, frame + 388u));
            }
            ReleaseNodeTree(ctx, idx, LoadU32(ctx, frame + 380u));
            p0 = static_cast<int32_t>(LoadS8(ctx, frame + 375u)) > -1;
            if (p0 != 0u) { goto exception_reload; }
            p0 = LoadU32(ctx, frame + 364u);
            DeallocateRawBlock(ctx, p0);
            goto exception_reload;
          }
          exception_bridge_c:;
          if (ResumePhase(ctx) == kResumeRunning) {
            save = HostRet0(ctx);
            exc = save;
            save = HostTempRet0(ctx);
            p0 = save;
            goto exception_reload;
          }
          exception_cleanup_f:;
          if (ResumePhase(ctx) == kResumeRunning) {
            save = HostRet0(ctx);
            exc = save;
            save = HostTempRet0(ctx);
            p0 = save;
            goto exception_reload;
          }
          exception_cleanup_e:;
          if (ResumePhase(ctx) == kResumeRunning) {
            save = HostRet0(ctx);
            exc = save;
            save = HostTempRet0(ctx);
            p0 = save;
            goto exception_reload;
          }
          exception_cleanup_d:;
          if (ResumePhase(ctx) == kResumeRunning) {
            save = HostRet0(ctx);
            exc = save;
            save = HostTempRet0(ctx);
            p0 = save;
            goto exception_reload;
          }
          exception_cleanup_c:;
          if (ResumePhase(ctx) == kResumeRunning) {
            save = HostRet0(ctx);
            exc = save;
            save = HostTempRet0(ctx);
            p0 = save;
            goto exception_reload;
          }
          exception_cleanup_b:;
          if (ResumePhase(ctx) == kResumeRunning) {
            save = HostRet0(ctx);
            exc = save;
            save = HostTempRet0(ctx);
            p0 = save;
            DeallocateRawBlock(ctx, flag);
            goto exception_reload;
          }
          exception_cleanup_a:;
          if (ResumePhase(ctx) == kResumeRunning) {
            save = HostRet0(ctx);
            exc = save;
            save = HostTempRet0(ctx);
            p0 = save;
          }
          exception_reload:;
          if (ResumePhase(ctx) == kResumeRunning) {
            p1 = LoadU32(ctx, frame + 456u);
            p0 = static_cast<uint32_t>(p1 == 0u);
            if (p0 != 0u) { goto exception_release; }
            ret = p1;
            tmp = LoadU32(ctx, frame + 460u);
            p0 = static_cast<uint32_t>(tmp != p1);
            if (p0 != 0u) {
              catch_chain_walk:;
              p0 = (tmp - 12u);
              if (static_cast<int32_t>(LoadS8(ctx, (tmp - 1u))) <= -1) {
                DeallocateRawBlock(ctx, LoadU32(ctx, p0));
              }
              tmp = p0;
              p0 = static_cast<uint32_t>(tmp != p1);
              if (p0 != 0u) { goto catch_chain_walk; }
              ret = LoadU32(ctx, frame + 456u);
            }
            StoreU32(ctx, frame + 460u, p1);
            DeallocateRawBlock(ctx, ret);
          }
          // Merge point exception_release.
          exception_release:;
          if (ResumePhase(ctx) == kResumeRunning) {
            HostOsRelease(ctx, exc);
          }
          // Merge point unreachable_path.
          unreachable_path:;
          if (ResumePhase(ctx) == kResumeRunning) {
            TrapUnreachable();
          }
          // Merge point pending_throw_path.
          pending_throw_path:;
          if (ResumePhase(ctx) == kResumeRunning) {
            p0 = FindMatchingCatch(ctx, 0u);
            save = HostTempRet0(ctx);
          }
          if (ResumePhase(ctx) == kResumeRunning || resume_point == 242u) {
            Panic(ctx, p0);
            #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
            SyncMemoryBase(ctx);
            #endif
            resume_point = 242u;
            if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
          }
          if (ResumePhase(ctx) == kResumeRunning) {
            TrapUnreachable();
            goto done;
          }
          // ---- Suspension exit: spill the resume code and rewind the frame. ----
          suspend_point:;
          StoreU32(ctx, ReturnFrameSp(ctx), resume_point);
          StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
          save = LoadU32(ctx, ReturnFrameSp(ctx));
          StoreU32(ctx, save, p0);
          StoreU32(ctx, save + 4u, p1);
          StoreU32(ctx, save + 8u, tmp);
          StoreU32(ctx, save + 12u, ret);
          StoreU32(ctx, save + 16u, exc);
          StoreU32(ctx, save + 20u, frame);
          StoreU32(ctx, save + 24u, flag);
          StoreU32(ctx, save + 28u, idx);
          StoreU32(ctx, save + 32u, ptr);
          StoreU32(ctx, save + 36u, ptr2);
          StoreU32(ctx, save + 40u, aux);
          StoreU32(ctx, save + 44u, aux2);
          StoreU32(ctx, save + 48u, aux3);
          StoreU32(ctx, save + 52u, str15);
          StoreU64(ctx, save + 56u, str64a);
          StoreU32(ctx, save + 64u, cmp16);
          StoreU64(ctx, save + 68u, str64b);
          StoreU32(ctx, save + 76u, cmp17);
          StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
          done:;
          LeaveFrame();
        }
      }
}
}  // namespace Compositor
}  // namespace artc
