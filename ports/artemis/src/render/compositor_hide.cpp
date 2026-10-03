#include "util/engine_context.h"

namespace artc {

// Products this dispatcher drives, under their rewrite-names.tsv targets.
void Panic(EngineContext* ctx, uint32_t code);
uint32_t CompareBytes(EngineContext* ctx, uint32_t lhs, uint32_t rhs, uint32_t n);
uint32_t CopyBytes(EngineContext* ctx, uint32_t dst, uint32_t src, uint32_t n);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t bytes);
uint32_t SkipWhitespace(EngineContext* ctx, uint32_t text);
void UnorderedLinkBucketNode(EngineContext* ctx, uint32_t p0, uint32_t p1, uint32_t p2, uint32_t p3,
                             uint32_t p4, uint32_t p5);
double UnorderedLookupNumber(EngineContext* ctx, uint32_t node);

namespace Compositor {

// Compositor::HideDispatch -- art_hide_dispatch(p0, p1, p2), the screen-effect dispatcher that
// hides a layer.  It prepares the scenario's hide record, asks the host hook which entries the
// scenario wants hidden, and walks the resulting table, re-running the hook for every entry whose
// kind matches.  Failures travel through the engine's exception scopes ("mode" on entry, "type"
// around every hook call) and are reported to the host by the tail ladder of this function.
void HideDispatch(EngineContext* ctx, uint32_t p0, uint32_t p1, uint32_t p2) {
  EnterFrame();

  // Sizes of the two stack frames this function pushes, and the scope tags it writes into them.
  constexpr uint32_t kSaveFrameBytes = 68u;
  constexpr uint32_t kResumeWordBytes = 4u;
  constexpr uint32_t kFrameBytes = 96u;
  constexpr uint32_t kScopeTagMade = 1701080941u;  // the four bytes "mode": the entry scope
  constexpr uint32_t kScopeTagTote = 1701869940u;  // the four bytes "type": the per-call scope
  constexpr uint32_t kScopeTagEase = 1702060389u;  // the four bytes "ease": the walk scope
  constexpr uint32_t kHookSelect = 1959u;          // selector of the "which entries" hook
  constexpr uint32_t kHookDescribe = 1354u;        // selector of the "describe entry" hook
  constexpr uint32_t kDescriptor = 745008u;
  constexpr uint32_t kAnyIndex = 4294967295u;
  constexpr uint32_t kScopeTagDiff = 1717987684u;  // the four bytes "diff": the episode scope
  constexpr uint32_t kUnexpectedTag = 740257u;     // tag word of the "unexpected object" scope
  constexpr uint32_t kUnexpectedWait = 740261u;    // its one-byte wait flag, the very next byte

  // Register slots.  The first two lines are the ones a suspend spills; the rest is scratch.
  uint32_t frame = 0, tmp = 0, slot = 0, block = 0, result = 0, aux = 0;
  uint32_t self = 0, tag = 0, count = 0, span = 0, flag = 0, word = 0, resume_point = 0;
  uint32_t i0 = 0, i1 = 0, i2 = 0, i3 = 0, i4 = 0, i5 = 0, i6 = 0;
  uint32_t ptr = 0, ptr2 = 0, ret = 0, exc = 0, save = 0, idx = 0, bit = 0;
  float num = 0.0f, f0 = 0.0f, f1 = 0.0f;
  double dbl = 0.0, dbl2 = 0.0, d0 = 0.0, d1 = 0.0;
  uint64_t j1 = 0;

  // Rewind: pop the spill frame and restore the locals the suspended call had written out.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    uint32_t const base = LoadU32(ctx, ReturnFrameSp(ctx));
    p0 = LoadU32(ctx, base);
    p2 = LoadU32(ctx, base + 8u);
    frame = LoadU32(ctx, base + 12u);
    tmp = LoadU32(ctx, base + 16u);
    slot = LoadU32(ctx, base + 20u);
    block = LoadU32(ctx, base + 24u);
    aux = LoadU32(ctx, base + 28u);
    self = LoadU32(ctx, base + 32u);
    tag = LoadU32(ctx, base + 36u);
    count = LoadU32(ctx, base + 40u);
    span = LoadU32(ctx, base + 44u);
    flag = LoadU32(ctx, base + 48u);
    num = LoadF32(ctx, base + 52u);
    word = LoadU32(ctx, base + 56u);
    dbl = LoadF64(ctx, base + 60u);
    p1 = LoadU32(ctx, base + 4u);
  }
  // Rewind: pop the resume index, which selects the call the walk has to re-enter.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }

  // Running entry: claim the 96 bytes of guest stack this dispatch keeps its scope slots in.
  if (ResumePhase(ctx) == kResumeRunning) {
    frame = GuestStackSp(ctx) - kFrameBytes;
    GuestStackSp(ctx) = frame;
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagMade);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    self = p1 + 16u;
    tag = frame + 56u;
    slot = frame + 16u;
    p2 = 0u;
  }

  // Ask the host which entries this scenario wants hidden; the answer arrives in the scope object.
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, tag, kDescriptor, slot, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    tmp = static_cast<uint32_t>(tmp != 1u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The caught object stores its kind either inline or behind the field at +32.
      tmp = LoadU32(ctx, frame + 32u);
      slot = LoadU32(ctx, tmp + 32u);
      tag = LoadU8(ctx, tmp + 39u);
      aux = static_cast<uint32_t>(static_cast<int32_t>(tag << 24) >> 24 < 0);
      tag = (aux != 0u) ? slot : tag;
      tag = static_cast<uint32_t>(tag == 4u);
    }
    if (tag != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p2 = tmp + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
        result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 740718u, 4u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 1u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p2 = result;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        tmp = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (tmp == 1u) { goto pending_throw_path; }
        p2 = static_cast<uint32_t>(p2 == 0u);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (LoadS8(ctx, frame + 67u) <= -1) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
      }
      p2 = static_cast<uint32_t>(p2 == 0u);
      if (p2 != 0u) { goto p02_branch_entry; }
      p2 = 0u;
      StoreU8(ctx, frame + 60u, 0u);
      StoreU32(ctx, frame + 56u, kScopeTagTote);
      StoreU8(ctx, frame + 67u, 4u);
      StoreU32(ctx, kDataD015C, 0u);
      StoreU32(ctx, frame + 16u, frame + 56u);
      tmp = frame + 56u;
      tag = frame + 16u;
      p1 = frame + 32u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
      HostInvokeViiiiii(ctx, kHookSelect, p1, self, tmp, kDescriptor, tag, frame);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 2u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p1 == 1u) { goto unreachable_path; }
      p1 = LoadU32(ctx, frame + 32u);
      tag = LoadU32(ctx, p1 + 32u);
      tmp = LoadU8(ctx, p1 + 39u);
      slot = static_cast<uint32_t>(static_cast<int32_t>(tmp << 24) >> 24 < 0);
      tmp = (slot != 0u) ? tag : tmp;
      tmp = static_cast<uint32_t>(tmp == 2u);
    }
    if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p1 = p1 + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
        result = HostInvokeIiiiii(ctx, kHookDescribe, p1, 0u, kAnyIndex, 740723u, 2u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 3u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p1 = result;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p2 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (p2 == 1u) { goto pending_throw_path; }
        p2 = static_cast<uint32_t>(p1 == 0u);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (LoadS8(ctx, frame + 67u) <= -1) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
      }
      p1 = static_cast<uint32_t>(p2 == 0u);
      if (p1 != 0u) { goto retry_hook_5; }
      p1 = p0 + 13308u;
      StoreU32(ctx, frame + 32u, p1);
      p2 = frame + 56u;
      tmp = frame + 32u;
      tag = frame + 16u;
      p0 = p0 + 13536u;
    }
    // Emplace the described entry into the hide table.
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
      UnorderedLinkBucketNode(ctx, p2, p0, p1, kDescriptor, tmp, tag);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 4u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, frame + 56u);
      p1 = LoadU32(ctx, p0 + 28u);
      StoreU32(ctx, p0 + 32u, p1);
      goto done;
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto unreachable_path; }
  }

  // Merge point retry_hook_5.
retry_hook_5:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = 0u;
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagTote);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 56u;
    tag = frame + 16u;
    p1 = frame + 32u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
    HostInvokeViiiiii(ctx, kHookSelect, p1, self, tmp, kDescriptor, tag, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 5u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p1 = static_cast<uint32_t>(p1 != 1u);
  }
  if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, frame + 32u);
      tag = LoadU32(ctx, p1 + 32u);
      tmp = LoadU8(ctx, p1 + 39u);
      slot = static_cast<uint32_t>(static_cast<int32_t>(tmp << 24) >> 24 < 0);
      tmp = (slot != 0u) ? tag : tmp;
      tmp = static_cast<uint32_t>(tmp == 3u);
    }
    if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p1 = p1 + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
        result = HostInvokeIiiiii(ctx, kHookDescribe, p1, 0u, kAnyIndex, 740726u, 3u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 6u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p1 = result;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p2 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (p2 == 1u) { goto pending_throw_path; }
        p2 = static_cast<uint32_t>(p1 == 0u);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (LoadS8(ctx, frame + 67u) <= -1) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
      }
      p1 = static_cast<uint32_t>(p2 == 0u);
      if (p1 != 0u) { goto retry_hook_8; }
      p1 = p0 + 13308u;
      StoreU32(ctx, frame + 32u, p1);
      p2 = frame + 56u;
      tmp = frame + 32u;
      tag = frame + 16u;
      p0 = p0 + 13548u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 7u) {
      UnorderedLinkBucketNode(ctx, p2, p0, p1, kDescriptor, tmp, tag);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 7u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, frame + 56u);
      p1 = LoadU32(ctx, p0 + 28u);
      StoreU32(ctx, p0 + 32u, p1);
      goto done;
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto unreachable_path; }
  }

  // Merge point retry_hook_8.
retry_hook_8:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = 0u;
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagTote);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 56u;
    tag = frame + 16u;
    p1 = frame + 32u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 8u) {
    HostInvokeViiiiii(ctx, kHookSelect, p1, self, tmp, kDescriptor, tag, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 8u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p1 = static_cast<uint32_t>(p1 != 1u);
  }
  if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, frame + 32u);
      tag = LoadU32(ctx, p1 + 32u);
      tmp = LoadU8(ctx, p1 + 39u);
      slot = static_cast<uint32_t>(static_cast<int32_t>(tmp << 24) >> 24 < 0);
      tmp = (slot != 0u) ? tag : tmp;
      tmp = static_cast<uint32_t>(tmp == 4u);
    }
    if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p1 = p1 + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 9u) {
        result = HostInvokeIiiiii(ctx, kHookDescribe, p1, 0u, kAnyIndex, 740730u, 4u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 9u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p1 = result;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p2 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (p2 == 1u) { goto pending_throw_path; }
        p2 = static_cast<uint32_t>(p1 == 0u);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (LoadS8(ctx, frame + 67u) <= -1) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
      }
      p1 = static_cast<uint32_t>(p2 == 0u);
      if (p1 != 0u) { goto retry_hook_11; }
      p1 = p0 + 13308u;
      StoreU32(ctx, frame + 32u, p1);
      p2 = frame + 56u;
      tmp = frame + 32u;
      tag = frame + 16u;
      p0 = p0 + 13560u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 10u) {
      UnorderedLinkBucketNode(ctx, p2, p0, p1, kDescriptor, tmp, tag);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 10u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, frame + 56u);
      p1 = LoadU32(ctx, p0 + 28u);
      StoreU32(ctx, p0 + 32u, p1);
      goto done;
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto unreachable_path; }
  }

  // Merge point retry_hook_11.
retry_hook_11:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = 0u;
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagTote);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 56u;
    tag = frame + 16u;
    p1 = frame + 32u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 11u) {
    HostInvokeViiiiii(ctx, kHookSelect, p1, self, tmp, kDescriptor, tag, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 11u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p1 = static_cast<uint32_t>(p1 != 1u);
  }
  if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, frame + 32u);
      tag = LoadU32(ctx, p1 + 32u);
      tmp = LoadU8(ctx, p1 + 39u);
      slot = static_cast<uint32_t>(static_cast<int32_t>(tmp << 24) >> 24 < 0);
      tmp = (slot != 0u) ? tag : tmp;
      tmp = static_cast<uint32_t>(tmp == 4u);
    }
    if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p1 = p1 + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 12u) {
        result = HostInvokeIiiiii(ctx, kHookDescribe, p1, 0u, kAnyIndex, 718402u, 4u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 12u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p1 = result;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p2 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (p2 == 1u) { goto pending_throw_path; }
        p2 = static_cast<uint32_t>(p1 == 0u);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (LoadS8(ctx, frame + 67u) <= -1) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
      }
      p1 = static_cast<uint32_t>(p2 == 0u);
      if (p1 != 0u) { goto retry_hook_14; }
      p1 = p0 + 13308u;
      StoreU32(ctx, frame + 32u, p1);
      p2 = frame + 56u;
      tmp = frame + 32u;
      tag = frame + 16u;
      p0 = p0 + 13572u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 13u) {
      UnorderedLinkBucketNode(ctx, p2, p0, p1, kDescriptor, tmp, tag);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 13u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, frame + 56u);
      p1 = LoadU32(ctx, p0 + 28u);
      StoreU32(ctx, p0 + 32u, p1);
      goto done;
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto unreachable_path; }
  }

  // Merge point retry_hook_14.
retry_hook_14:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = 0u;
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagTote);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 56u;
    tag = frame + 16u;
    p1 = frame + 32u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 14u) {
    HostInvokeViiiiii(ctx, kHookSelect, p1, self, tmp, kDescriptor, tag, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 14u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p1 = static_cast<uint32_t>(p1 != 1u);
  }
  if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, frame + 32u);
      tag = LoadU32(ctx, p1 + 32u);
      tmp = LoadU8(ctx, p1 + 39u);
      slot = static_cast<uint32_t>(static_cast<int32_t>(tmp << 24) >> 24 < 0);
      tmp = (slot != 0u) ? tag : tmp;
      tmp = static_cast<uint32_t>(tmp == 15u);
    }
    if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p1 = p1 + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 15u) {
        result = HostInvokeIiiiii(ctx, kHookDescribe, p1, 0u, kAnyIndex, 740735u, 15u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 15u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p1 = result;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p2 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (p2 == 1u) { goto pending_throw_path; }
        p2 = static_cast<uint32_t>(p1 == 0u);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (LoadS8(ctx, frame + 67u) <= -1) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
      }
      p1 = static_cast<uint32_t>(p2 == 0u);
      if (p1 != 0u) { goto retry_hook_17; }
      p0 = LoadU32(ctx, p0 + 120u);
      p1 = static_cast<uint32_t>(p0 == 0u);
    }
    if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 16u) {
        HostFailAssertion(ctx, 857u, 745009u, 745017u, 734u, 745073u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 16u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p0 = static_cast<uint32_t>(p0 != 1u);
        if (p0 != 0u) { goto trap_path; }
        goto pending_throw_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, p0 + 596u, LoadU32(ctx, p0 + 592u));
      goto done;
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto unreachable_path; }
  }

  // Merge point retry_hook_17.
retry_hook_17:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = 0u;
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagTote);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 56u;
    tag = frame + 16u;
    p1 = frame + 32u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 17u) {
    HostInvokeViiiiii(ctx, kHookSelect, p1, self, tmp, kDescriptor, tag, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 17u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p1 = static_cast<uint32_t>(p1 != 1u);
  }
  if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, frame + 32u);
      tag = LoadU32(ctx, p1 + 32u);
      tmp = LoadU8(ctx, p1 + 39u);
      slot = static_cast<uint32_t>(static_cast<int32_t>(tmp << 24) >> 24 < 0);
      tmp = (slot != 0u) ? tag : tmp;
      tmp = static_cast<uint32_t>(tmp == 16u);
    }
    if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p1 = p1 + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 18u) {
        result = HostInvokeIiiiii(ctx, kHookDescribe, p1, 0u, kAnyIndex, 740751u, 16u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 18u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p1 = result;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p2 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (p2 == 1u) { goto pending_throw_path; }
        p2 = static_cast<uint32_t>(p1 == 0u);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (LoadS8(ctx, frame + 67u) <= -1) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
      }
      p1 = static_cast<uint32_t>(p2 == 0u);
      if (p1 != 0u) { goto retry_hook_20; }
      p0 = LoadU32(ctx, p0 + 120u);
      p1 = static_cast<uint32_t>(p0 == 0u);
    }
    if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 19u) {
        HostFailAssertion(ctx, 857u, 745009u, 745017u, 734u, 745073u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 19u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p0 = static_cast<uint32_t>(p0 != 1u);
        if (p0 != 0u) { goto trap_path; }
        goto pending_throw_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, p0 + 608u, LoadU32(ctx, p0 + 604u));
      goto done;
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto unreachable_path; }
  }

  // Merge point retry_hook_20.
retry_hook_20:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = 0u;
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagTote);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 56u;
    tag = frame + 16u;
    p1 = frame + 32u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 20u) {
    HostInvokeViiiiii(ctx, kHookSelect, p1, self, tmp, kDescriptor, tag, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 20u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p1 = static_cast<uint32_t>(p1 != 1u);
  }
  if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, frame + 32u);
      tag = LoadU32(ctx, p1 + 32u);
      tmp = LoadU8(ctx, p1 + 39u);
      slot = static_cast<uint32_t>(static_cast<int32_t>(tmp << 24) >> 24 < 0);
      tmp = (slot != 0u) ? tag : tmp;
      tmp = static_cast<uint32_t>(tmp == 13u);
    }
    if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p1 = p1 + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 21u) {
        result = HostInvokeIiiiii(ctx, kHookDescribe, p1, 0u, kAnyIndex, 740768u, 13u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 21u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p1 = result;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p2 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (p2 == 1u) { goto pending_throw_path; }
        p2 = static_cast<uint32_t>(p1 == 0u);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (LoadS8(ctx, frame + 67u) <= -1) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
      }
      p1 = static_cast<uint32_t>(p2 == 0u);
      if (p1 != 0u) { goto retry_hook_23; }
      p0 = LoadU32(ctx, p0 + 120u);
      p1 = static_cast<uint32_t>(p0 == 0u);
    }
    if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 22u) {
        HostFailAssertion(ctx, 857u, 745009u, 745017u, 734u, 745073u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 22u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p0 = static_cast<uint32_t>(p0 != 1u);
        if (p0 != 0u) { goto trap_path; }
        goto pending_throw_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, p0 + 620u, LoadU32(ctx, p0 + 616u));
      goto done;
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto unreachable_path; }
  }

  // Merge point retry_hook_23.
retry_hook_23:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagTote);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 56u;
    tag = frame + 16u;
    p1 = frame + 32u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 23u) {
    HostInvokeViiiiii(ctx, kHookSelect, p1, self, tmp, kDescriptor, tag, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 23u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p1 = static_cast<uint32_t>(p1 != 1u);
  }
  if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, frame + 32u);
      tag = LoadU32(ctx, p1 + 32u);
      tmp = LoadU8(ctx, p1 + 39u);
      slot = static_cast<uint32_t>(static_cast<int32_t>(tmp << 24) >> 24 < 0);
      tmp = (slot != 0u) ? tag : tmp;
      tmp = static_cast<uint32_t>(tmp == 14u);
    }
    if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p1 = p1 + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 24u) {
        result = HostInvokeIiiiii(ctx, kHookDescribe, p1, 0u, kAnyIndex, 740782u, 14u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 24u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p1 = result;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p2 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (p2 == 1u) { goto pending_throw_path; }
        p2 = static_cast<uint32_t>(p1 == 0u);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (LoadS8(ctx, frame + 67u) <= -1) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
      }
      p2 = static_cast<uint32_t>(p2 == 0u);
      if (p2 != 0u) { goto done; }
      p0 = LoadU32(ctx, p0 + 120u);
      p1 = static_cast<uint32_t>(p0 == 0u);
    }
    if (p1 != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) { StoreU32(ctx, kDataD015C, 0u); }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 25u) {
        HostFailAssertion(ctx, 857u, 745009u, 745017u, 734u, 745073u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 25u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p0 = static_cast<uint32_t>(p0 != 1u);
        if (p0 != 0u) { goto trap_path; }
        goto pending_throw_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, p0 + 632u, LoadU32(ctx, p0 + 628u));
      goto done;
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto unreachable_path; }
  }

  // Merge point p02_branch_entry.  The entries the hook reported have all been consumed, so the
  // outer "Made" scope is opened again to describe the object the caller asked about.
p02_branch_entry:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagMade);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    tag = frame + 56u;
    slot = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 26u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, tag, kDescriptor, slot, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 26u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    tmp = static_cast<uint32_t>(tmp != 1u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, frame + 32u);
      slot = LoadU32(ctx, tmp + 32u);
      tag = LoadU8(ctx, tmp + 39u);
      aux = static_cast<uint32_t>(static_cast<int32_t>(tag << 24) >> 24 < 0);
      tag = (aux != 0u) ? slot : tag;
      tag = static_cast<uint32_t>(tag == 3u);
    }
    if (tag != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p2 = tmp + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 27u) {
        result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 740797u, 3u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 27u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p2 = result;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p2 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (p2 == 1u) { goto pending_throw_path; }
        p2 = static_cast<uint32_t>(p2 == 0u);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (LoadS8(ctx, frame + 67u) <= -1) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
      }
      p2 = static_cast<uint32_t>(p2 == 0u);
      if (p2 != 0u) { goto done; }
      // Nothing was described: arm the catch-all scope, whose tag word is read out of the data
      // segment rather than written inline.
      StoreU8(ctx, frame + 67u, 5u);
      StoreU32(ctx, frame + 56u, LoadU32(ctx, kUnexpectedTag));
      StoreU8(ctx, frame + 60u, LoadU8(ctx, kUnexpectedWait));
      StoreU8(ctx, frame + 61u, 0u);
      StoreU32(ctx, kDataD015C, 0u);
      StoreU32(ctx, frame + 16u, frame + 56u);
      tmp = frame + 32u;
      tag = frame + 56u;
      slot = frame + 16u;
      p2 = 0u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 28u) {
      HostInvokeViiiiii(ctx, kHookSelect, tmp, self, tag, kDescriptor, slot, frame);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 28u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      tmp = static_cast<uint32_t>(tmp == 1u);
      if (tmp != 0u) { goto unreachable_path; }
      tmp = LoadU32(ctx, frame + 32u);
      slot = LoadU32(ctx, tmp + 32u);
      tag = LoadU8(ctx, tmp + 39u);
      aux = static_cast<uint32_t>(static_cast<int32_t>(tag << 24) >> 24 < 0);
      tag = (aux != 0u) ? slot : tag;
      tag = static_cast<uint32_t>(tag == 4u);
    }
    if (tag != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p2 = tmp + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 29u) {
        result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 740801u, 4u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 29u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p2 = result;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p2 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (p2 == 1u) { goto pending_throw_path; }
        p2 = static_cast<uint32_t>(p2 == 0u);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (LoadS8(ctx, frame + 67u) <= -1) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
      }
      if (p2 != 0u) {
        i0 = 0u;
        goto p03_scope_open;
      }
      StoreU8(ctx, frame + 67u, 5u);
      StoreU8(ctx, frame + 61u, 0u);
      StoreU32(ctx, frame + 56u, LoadU32(ctx, kUnexpectedTag));
      StoreU8(ctx, frame + 60u, LoadU8(ctx, kUnexpectedWait));
      StoreU32(ctx, kDataD015C, 0u);
      StoreU32(ctx, frame + 16u, frame + 56u);
      tmp = frame + 32u;
      tag = frame + 56u;
      slot = frame + 16u;
      p2 = 0u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 30u) {
      HostInvokeViiiiii(ctx, kHookSelect, tmp, self, tag, kDescriptor, slot, frame);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 30u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      tmp = static_cast<uint32_t>(tmp == 1u);
      if (tmp != 0u) { goto unreachable_path; }
      tmp = LoadU32(ctx, frame + 32u);
      slot = LoadU32(ctx, tmp + 32u);
      tag = LoadU8(ctx, tmp + 39u);
      aux = static_cast<uint32_t>(static_cast<int32_t>(tag << 24) >> 24 < 0);
      tag = (aux != 0u) ? slot : tag;
      tag = static_cast<uint32_t>(tag == 3u);
    }
    if (tag != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p2 = tmp + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 31u) {
        result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 740806u, 3u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 31u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p2 = result;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p2 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (p2 == 1u) { goto pending_throw_path; }
        p2 = static_cast<uint32_t>(p2 == 0u);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (LoadS8(ctx, frame + 67u) <= -1) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
      }
      p2 = static_cast<uint32_t>(p2 == 0u);
      if (p2 != 0u) { goto catch_all_scope; }
      i0 = 1u;
      goto p03_scope_open;
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto unreachable_path; }
  }

  // Merge point catch_all_scope.  Every branch that reported a catchable failure lands here with
  // the catch-all scope armed, so the next hook call reports the failure as an exception.
catch_all_scope:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 67u, 5u);
    StoreU8(ctx, frame + 61u, 0u);
    StoreU32(ctx, frame + 56u, LoadU32(ctx, kUnexpectedTag));
    StoreU8(ctx, frame + 60u, LoadU8(ctx, kUnexpectedWait));
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    tag = frame + 56u;
    slot = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 32u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, tag, kDescriptor, slot, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 32u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    tmp = static_cast<uint32_t>(tmp != 1u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, frame + 32u);
      slot = LoadU32(ctx, tmp + 32u);
      tag = LoadU8(ctx, tmp + 39u);
      aux = static_cast<uint32_t>(static_cast<int32_t>(tag << 24) >> 24 < 0);
      tag = (aux != 0u) ? slot : tag;
      tag = static_cast<uint32_t>(tag == 5u);
    }
    if (tag != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p2 = tmp + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 33u) {
        result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 740810u, 5u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 33u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p2 = result;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p2 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (p2 == 1u) { goto pending_throw_path; }
        p2 = static_cast<uint32_t>(p2 == 0u);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (LoadS8(ctx, frame + 67u) <= -1) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
      }
      if (p2 != 0u) {
        i0 = 2u;
        goto p03_scope_open;
      }
      StoreU8(ctx, frame + 67u, 5u);
      StoreU8(ctx, frame + 61u, 0u);
      StoreU32(ctx, frame + 56u, LoadU32(ctx, kUnexpectedTag));
      StoreU8(ctx, frame + 60u, LoadU8(ctx, kUnexpectedWait));
      StoreU32(ctx, kDataD015C, 0u);
      StoreU32(ctx, frame + 16u, frame + 56u);
      tmp = frame + 32u;
      tag = frame + 56u;
      slot = frame + 16u;
      p2 = 0u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 34u) {
      HostInvokeViiiiii(ctx, kHookSelect, tmp, self, tag, kDescriptor, slot, frame);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 34u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      tmp = static_cast<uint32_t>(tmp == 1u);
      if (tmp != 0u) { goto unreachable_path; }
      tmp = LoadU32(ctx, frame + 32u);
      slot = LoadU32(ctx, tmp + 32u);
      tag = LoadU8(ctx, tmp + 39u);
      aux = static_cast<uint32_t>(static_cast<int32_t>(tag << 24) >> 24 < 0);
      tag = (aux != 0u) ? slot : tag;
      tag = static_cast<uint32_t>(tag == 6u);
    }
    if (tag != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p2 = tmp + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 35u) {
        result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 740816u, 6u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 35u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p2 = result;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p2 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (p2 == 1u) { goto pending_throw_path; }
        p2 = static_cast<uint32_t>(p2 == 0u);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (LoadS8(ctx, frame + 67u) <= -1) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
      }
      if (p2 != 0u) {
        i0 = 3u;
        goto p03_scope_open;
      }
      StoreU8(ctx, frame + 67u, 5u);
      StoreU8(ctx, frame + 61u, 0u);
      StoreU32(ctx, frame + 56u, LoadU32(ctx, kUnexpectedTag));
      StoreU8(ctx, frame + 60u, LoadU8(ctx, kUnexpectedWait));
      StoreU32(ctx, kDataD015C, 0u);
      StoreU32(ctx, frame + 16u, frame + 56u);
      tmp = frame + 32u;
      tag = frame + 56u;
      slot = frame + 16u;
      p2 = 0u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 36u) {
      HostInvokeViiiiii(ctx, kHookSelect, tmp, self, tag, kDescriptor, slot, frame);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 36u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      tmp = static_cast<uint32_t>(tmp == 1u);
      if (tmp != 0u) { goto unreachable_path; }
      tmp = LoadU32(ctx, frame + 32u);
      slot = LoadU32(ctx, tmp + 32u);
      tag = LoadU8(ctx, tmp + 39u);
      aux = static_cast<uint32_t>(static_cast<int32_t>(tag << 24) >> 24 < 0);
      tag = (aux != 0u) ? slot : tag;
      tag = static_cast<uint32_t>(tag == 6u);
    }
    if (tag != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p2 = tmp + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 37u) {
        result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 740823u, 6u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 37u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p2 = result;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p2 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (p2 == 1u) { goto pending_throw_path; }
        p2 = static_cast<uint32_t>(p2 == 0u);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (LoadS8(ctx, frame + 67u) <= -1) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
      }
      if (p2 != 0u) {
        i0 = 4u;
        goto p03_scope_open;
      }
      StoreU8(ctx, frame + 67u, 5u);
      StoreU8(ctx, frame + 61u, 0u);
      StoreU32(ctx, frame + 56u, LoadU32(ctx, kUnexpectedTag));
      StoreU8(ctx, frame + 60u, LoadU8(ctx, kUnexpectedWait));
      StoreU32(ctx, kDataD015C, 0u);
      StoreU32(ctx, frame + 16u, frame + 56u);
      tmp = frame + 32u;
      tag = frame + 56u;
      slot = frame + 16u;
      p2 = 0u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 38u) {
      HostInvokeViiiiii(ctx, kHookSelect, tmp, self, tag, kDescriptor, slot, frame);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 38u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      tmp = static_cast<uint32_t>(tmp == 1u);
      if (tmp != 0u) { goto unreachable_path; }
      tmp = LoadU32(ctx, frame + 32u);
      slot = LoadU32(ctx, tmp + 32u);
      tag = LoadU8(ctx, tmp + 39u);
      aux = static_cast<uint32_t>(static_cast<int32_t>(tag << 24) >> 24 < 0);
      tag = (aux != 0u) ? slot : tag;
      tag = static_cast<uint32_t>(tag == 6u);
    }
    if (tag != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p2 = tmp + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 39u) {
        result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 740830u, 6u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 39u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p2 = result;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p2 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (p2 == 1u) { goto pending_throw_path; }
        p2 = static_cast<uint32_t>(p2 == 0u);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (LoadS8(ctx, frame + 67u) <= -1) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
      }
      p2 = static_cast<uint32_t>(p2 == 0u);
      if (p2 != 0u) { goto p03_branch_entry; }
      i0 = 5u;
      goto p03_scope_open;
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto unreachable_path; }
  }
  // Merge point p03_branch_entry.  Nothing was described for the object, so the entry hook runs
  // once more under the catch-all scope.
p03_branch_entry:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 67u, 5u);
    StoreU8(ctx, frame + 61u, 0u);
    StoreU32(ctx, frame + 56u, LoadU32(ctx, kUnexpectedTag));
    StoreU8(ctx, frame + 60u, LoadU8(ctx, kUnexpectedWait));
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    tag = frame + 56u;
    slot = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 40u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, tag, kDescriptor, slot, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 40u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    tmp = static_cast<uint32_t>(tmp != 1u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, frame + 32u);
      tag = LoadU32(ctx, tmp + 32u);
      flag = LoadU8(ctx, tmp + 39u);
      slot = static_cast<uint32_t>(static_cast<int32_t>(flag << 24) >> 24 < 0);
      tag = (slot != 0u) ? tag : flag;
      tag = static_cast<uint32_t>(tag == 10u);
    }
    if (tag != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p2 = tmp + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 41u) {
        result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 740837u, 10u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 41u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p2 = result;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p2 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (p2 == 1u) { goto pending_throw_path; }
        p2 = static_cast<uint32_t>(p2 == 0u);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (LoadS8(ctx, frame + 67u) <= -1) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
      }
      p2 = static_cast<uint32_t>(p2 == 0u);
      if (p2 != 0u) { goto retry_hook_42; }
      flag = 1u;
      i0 = 0u;
      goto p03_scope_open;
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto unreachable_path; }
  }

  // Merge point retry_hook_42.
retry_hook_42:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 67u, 5u);
    StoreU8(ctx, frame + 61u, 0u);
    StoreU32(ctx, frame + 56u, LoadU32(ctx, kUnexpectedTag));
    StoreU8(ctx, frame + 60u, LoadU8(ctx, kUnexpectedWait));
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    tag = frame + 56u;
    slot = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 42u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, tag, kDescriptor, slot, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 42u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    tag = LoadU32(ctx, tmp + 32u);
    flag = LoadU8(ctx, tmp + 39u);
    slot = static_cast<uint32_t>(static_cast<int32_t>(flag << 24) >> 24 < 0);
    tag = (slot != 0u) ? tag : flag;
    tag = static_cast<uint32_t>(tag == 9u);
  }
  if (tag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 43u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 740848u, 9u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 43u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    flag = 1u;
    i0 = 1u;
    if (p2 != 0u) { goto p03_scope_open; }
    StoreU8(ctx, frame + 67u, 5u);
    StoreU8(ctx, frame + 61u, 0u);
    StoreU32(ctx, frame + 56u, LoadU32(ctx, kUnexpectedTag));
    StoreU8(ctx, frame + 60u, LoadU8(ctx, kUnexpectedWait));
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    tag = frame + 56u;
    slot = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 44u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, tag, kDescriptor, slot, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 44u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    tag = LoadU8(ctx, tmp + 39u);
    aux = static_cast<uint32_t>(static_cast<int32_t>(tag << 24) >> 24 < 0);
    tag = (aux != 0u) ? slot : tag;
    tag = static_cast<uint32_t>(tag == 11u);
  }
  if (tag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 45u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 740858u, 11u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 45u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    i0 = 2u;
    if (p2 != 0u) { goto p03_scope_open; }
    StoreU8(ctx, frame + 67u, 5u);
    StoreU8(ctx, frame + 61u, 0u);
    StoreU32(ctx, frame + 56u, LoadU32(ctx, kUnexpectedTag));
    StoreU8(ctx, frame + 60u, LoadU8(ctx, kUnexpectedWait));
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    tag = frame + 56u;
    slot = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 46u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, tag, kDescriptor, slot, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 46u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    tag = LoadU8(ctx, tmp + 39u);
    aux = static_cast<uint32_t>(static_cast<int32_t>(tag << 24) >> 24 < 0);
    tag = (aux != 0u) ? slot : tag;
    tag = static_cast<uint32_t>(tag == 12u);
  }
  if (tag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 47u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 740870u, 12u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 47u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    i0 = 3u;
    if (p2 != 0u) { goto p03_scope_open; }
    StoreU8(ctx, frame + 67u, 5u);
    StoreU8(ctx, frame + 61u, 0u);
    StoreU32(ctx, frame + 56u, LoadU32(ctx, kUnexpectedTag));
    StoreU8(ctx, frame + 60u, LoadU8(ctx, kUnexpectedWait));
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    tag = frame + 56u;
    slot = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 48u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, tag, kDescriptor, slot, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 48u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    tag = LoadU8(ctx, tmp + 39u);
    aux = static_cast<uint32_t>(static_cast<int32_t>(tag << 24) >> 24 < 0);
    tag = (aux != 0u) ? slot : tag;
    tag = static_cast<uint32_t>(tag == 12u);
  }
  if (tag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 49u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 740883u, 12u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 49u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    i0 = 4u;
    if (p2 != 0u) { goto p03_scope_open; }
    // The last of the catch-all attempts arms the scope with the argument roles rotated.
    StoreU8(ctx, frame + 67u, 5u);
    StoreU8(ctx, frame + 61u, 0u);
    StoreU32(ctx, frame + 56u, LoadU32(ctx, kUnexpectedTag));
    StoreU8(ctx, frame + 60u, LoadU8(ctx, kUnexpectedWait));
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 56u;
    tag = frame + 16u;
    p2 = frame + 32u;
    flag = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 50u) {
    HostInvokeViiiiii(ctx, kHookSelect, p2, self, tmp, kDescriptor, tag, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 50u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (p2 == 1u) { goto unreachable_path; }
    p2 = LoadU32(ctx, frame + 32u);
    tag = LoadU32(ctx, p2 + 32u);
    tmp = LoadU8(ctx, p2 + 39u);
    slot = static_cast<uint32_t>(static_cast<int32_t>(tmp << 24) >> 24 < 0);
    tmp = (slot != 0u) ? tag : tmp;
    tmp = static_cast<uint32_t>(tmp == 12u);
  }
  if (tmp != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = p2 + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 51u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 740896u, 12u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 51u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    flag = (ResumePhase(ctx) == kResumeRunning) ? static_cast<uint32_t>(p2 == 0u) : flag;
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
  }
  i0 = (ResumePhase(ctx) == kResumeRunning) ? ((flag != 0u) ? 5u : 6u) : tag;

  // Merge point p03_scope_open.  The carried value picks which stage of the walk runs next under
  // the "ease" scope.
p03_scope_open:;
  tag = i0;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 56u;
    slot = frame + 16u;
    p2 = frame + 32u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 52u) {
    HostInvokeViiiiii(ctx, kHookSelect, p2, self, tmp, kDescriptor, slot, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 52u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (p2 == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 4u);
    p2 = 0u;
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 53u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 740909u, 4u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 53u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 54u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 54u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 11u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 55u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 740914u, 11u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 55u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 1u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 56u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 56u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 12u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 57u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 740926u, 12u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 57u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 2u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 58u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 58u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 14u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 59u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 740939u, 14u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 59u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 3u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 60u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 60u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 12u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 61u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 740954u, 12u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 61u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  // The entry was described, so it leaves the walk; the scope is closed before the next one opens.
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 4u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 62u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 62u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 13u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 63u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 740967u, 13u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 63u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 5u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 64u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 64u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 15u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 65u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 740981u, 15u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 65u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 6u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 66u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 66u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 12u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 67u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 740997u, 12u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 67u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 7u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 68u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 68u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 13u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 69u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 741010u, 13u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 69u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 8u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 70u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 70u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 15u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 71u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 741024u, 15u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 71u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 9u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 72u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 72u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 12u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 73u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 741040u, 12u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 73u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 10u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 74u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 74u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 13u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 75u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 741053u, 13u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 75u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 11u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 76u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 76u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 15u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 77u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 741067u, 15u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 77u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 12u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 78u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 78u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 11u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 79u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 741083u, 11u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 79u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 13u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 80u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 80u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 12u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 81u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 741095u, 12u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 81u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 14u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 82u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 82u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 14u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 83u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 741108u, 14u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 83u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  // The walk is still going: close the scope, pick the next dispatch key, and open a fresh one.
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 15u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 84u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 84u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 11u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 85u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 741123u, 11u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 85u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 16u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 86u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 86u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 12u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 87u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 741135u, 12u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 87u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 17u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 88u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 88u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 14u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 89u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 741148u, 14u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 89u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 18u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 90u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 90u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 11u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 91u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 741163u, 11u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 91u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 19u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 92u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 92u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 12u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 93u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 741175u, 12u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 93u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 20u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 94u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 94u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 14u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 95u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 741188u, 14u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 95u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 21u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 96u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 96u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 11u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 97u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 741203u, 11u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 97u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 22u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 98u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 98u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 12u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 99u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 741215u, 12u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 99u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 23u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 100u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 100u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 14u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 101u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 741228u, 14u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 101u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 24u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 102u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 102u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 14u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 103u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 741243u, 14u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 103u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 25u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 104u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 104u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 15u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 105u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 741258u, 15u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 105u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 26u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 106u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 106u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 17u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 107u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 741274u, 17u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 107u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 27u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 108u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 108u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 13u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 109u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 741292u, 13u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 109u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 28u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 110u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 110u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 14u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 111u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 741306u, 14u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 111u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = 29u;
    if (p2 != 0u) { goto p06_branch_entry; }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagEase);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 32u;
    slot = frame + 56u;
    aux = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 112u) {
    HostInvokeViiiiii(ctx, kHookSelect, tmp, self, slot, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 112u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (tmp == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    slot = LoadU32(ctx, tmp + 32u);
    aux = LoadU8(ctx, tmp + 39u);
    block = static_cast<uint32_t>(static_cast<int32_t>(aux << 24) >> 24 < 0);
    slot = (block != 0u) ? slot : aux;
    slot = static_cast<uint32_t>(slot == 16u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = tmp + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 113u) {
      result = HostInvokeIiiiii(ctx, kHookDescribe, p2, 0u, kAnyIndex, 741321u, 16u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 113u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto pending_throw_path; }
      p2 = static_cast<uint32_t>(p2 == 0u);
    }
  }
  // The walk is over: the two "closing" keys hand control to the episode dispatch at the merge
  // point part 06 continues from.
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    tmp = (p2 != 0u) ? 30u : 31u;
  }
  // Merge point p06_branch_entry.
p06_branch_entry:;
  // The episode scope the hook reports into: tagged "diff", and it accepts no waiter.
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = tmp;
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, kScopeTagDiff);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 56u;
    aux = frame + 16u;
    p2 = frame + 32u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 114u) {
    HostInvokeViiiiii(ctx, kHookSelect, p2, self, tmp, kDescriptor, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 114u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (p2 == 1u) { goto unreachable_path; }
    // Unwrap the value in field 28: a boxed value sits behind a pointer, a plain one inline.
    tmp = LoadU32(ctx, frame + 32u);
    p2 = tmp + 28u;
    tmp = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, tmp + 39u)) <= -1);
    p2 = (tmp != 0u) ? LoadU32(ctx, p2) : p2;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 115u) {
    dbl = UnorderedLookupNumber(ctx, p2);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 115u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    // The number the hook looked up is the effect strength: 0..255 scaled down to 0..1.
    num = static_cast<float>(dbl);
    p2 = static_cast<uint32_t>(tag != 2u);
    if (p2 != 0u) {
      // Tags other than 2 keep hundredths; only 3 and 4 are rescaled.
      if (tag - 3u <= 1u) {
        num = num / 100.0f;
      }
      goto p06_scan_entry;
    }
    num = num / 255.0f;
  }
  // The scan loop below is reached the same way on a fresh run and after a rewind.
  goto p06_scan_entry;
  // Merge point p06_scan_entry.
p06_scan_entry:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 67u, 5u);
    StoreU8(ctx, frame + 61u, 0u);
    StoreU32(ctx, frame + 56u, LoadU32(ctx, 741338u));
    StoreU8(ctx, frame + 60u, LoadU8(ctx, 741342u));
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 56u;
    aux = frame + 16u;
    p2 = frame + 32u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 116u) {
    HostInvokeViiiiii(ctx, 1959u, p2, self, tmp, 745008u, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 116u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (p2 == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    p2 = tmp + 28u;
    tmp = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, tmp + 39u)) <= -1);
    aux = SkipWhitespace(ctx, tmp ? LoadU32(ctx, p2) : p2);
    if (static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 67u)) <= -1)) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, 1701669236u);
    StoreU8(ctx, frame + 67u, 4u);
    tmp = LoadU32(ctx, p1 + 20u);
    if (tmp != 0u) {
      p2 = tmp;
  // Merge point tag_walk_loop.
tag_walk_loop:;
        aux = LoadU8(ctx, p2 + 27u);
        span = static_cast<uint32_t>((aux & 0x80u) != 0u);
        aux = span ? LoadU32(ctx, p2 + 20u) : aux;
        count = aux < 4u;
        block = count ? aux : 4u;
        if (block != 0u) {
          result = p2 + 16u;
          span = span ? LoadU32(ctx, result) : result;
          result = CompareBytes(ctx, frame + 56u, span, block);
          if (!(result)) {
            aux = aux <= 4u;
            if (aux != 0u) { goto tag_walk_step; }
            goto tag_walk_next;
          }
          aux = static_cast<uint32_t>(static_cast<int32_t>(result) <= -1);
          if (aux != 0u) { goto tag_walk_next; }
  // Merge point tag_walk_step.
tag_walk_step:;
          aux = CompareBytes(ctx, span, frame + 56u, block);
          block = !(aux);
          if (block != 0u) { goto tag_walk_found; }
          aux = static_cast<uint32_t>(static_cast<int32_t>(aux) > -1);
          if (aux != 0u) { goto diff_scope_entry; }
          goto tag_walk_skip;
        }
        aux = aux > 4u;
        if (aux != 0u) { goto tag_walk_next; }
  // Merge point tag_walk_found.
tag_walk_found:;
        aux = !(count);
        if (aux != 0u) { goto diff_scope_entry; }
  // Merge point tag_walk_skip.
tag_walk_skip:;
        p2 = p2 + 4u;
  // Merge point tag_walk_next.
tag_walk_next:;
        p2 = LoadU32(ctx, p2);
        if (p2 != 0u) { goto tag_walk_loop; }
    }
    StoreU8(ctx, frame + 36u, 0u);
    StoreU32(ctx, frame + 32u, 1667593069u);
    StoreU8(ctx, frame + 43u, 4u);
    p2 = !(tmp);
    if (p2 != 0u) { goto diff_scope_entry; }
  }
  // Merge point word_walk_loop.
word_walk_loop:;
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU8(ctx, tmp + 27u);
      block = static_cast<uint32_t>((p2 & 0x80u) != 0u);
      p2 = block ? LoadU32(ctx, tmp + 20u) : p2;
      span = p2 < 4u;
      aux = span ? p2 : 4u;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (aux != 0u) {
        count = tmp + 16u;
        block = block ? LoadU32(ctx, count) : count;
        count = CompareBytes(ctx, frame + 32u, block, aux);
        if (!(count)) {
          if (p2 <= 4u) { goto word_walk_step; }
          goto word_walk_done;
        }
        p2 = static_cast<uint32_t>(static_cast<int32_t>(count) <= -1);
        if (p2 != 0u) { goto word_walk_done; }
  // Merge point word_walk_step.
word_walk_step:;
        p2 = CompareBytes(ctx, block, frame + 32u, aux);
        aux = !(p2);
        if (aux != 0u) { goto word_walk_found; }
        p2 = static_cast<uint32_t>(static_cast<int32_t>(p2) > -1);
        if (p2 != 0u) { goto word_walk_rescan; }
        goto word_walk_next;
      }
      if (p2 > 4u) { goto word_walk_done; }
  // Merge point word_walk_found.
word_walk_found:;
      if (span != 0u) { goto word_walk_next; }
  // Merge point word_walk_rescan.
word_walk_rescan:;
      StoreU8(ctx, frame + 60u, 0u);
      StoreU32(ctx, frame + 56u, 1667593069u);
      StoreU8(ctx, frame + 67u, 4u);
      StoreU32(ctx, kDataD015C, 0u);
      StoreU32(ctx, frame + 16u, frame + 56u);
      tmp = frame + 56u;
      aux = frame + 16u;
      p2 = frame + 32u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 117u) {
      HostInvokeViiiiii(ctx, 1959u, p2, self, tmp, 745008u, aux, frame);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 117u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto unreachable_path; }
      tmp = LoadU32(ctx, frame + 32u);
      p2 = tmp + 28u;
      tmp = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, tmp + 39u)) <= -1);
      aux = SkipWhitespace(ctx, tmp ? LoadU32(ctx, p2) : p2);
      p2 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 67u)) > -1);
      if (p2 != 0u) { goto diff_lookup; }
      p2 = LoadU32(ctx, frame + 56u);
      DeallocateRawBlock(ctx, p2);
      goto diff_lookup;
    }
  // Merge point word_walk_next.
word_walk_next:;
    tmp = ResumePhase(ctx) ? tmp : (tmp + 4u);
  // Merge point word_walk_done.
word_walk_done:;
    if (ResumePhase(ctx) == kResumeRunning) {
      tmp = LoadU32(ctx, tmp);
      if (tmp != 0u) { goto word_walk_loop; }
      goto diff_scope_entry;
    }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 118u) {
    Panic(ctx, p0);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 118u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (!(ResumePhase(ctx))) { goto trap_path; }
  // Merge point diff_scope_entry.
diff_scope_entry:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, 1701669236u);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    tmp = frame + 56u;
    aux = frame + 16u;
    p2 = frame + 32u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 119u) {
    HostInvokeViiiiii(ctx, 1959u, p2, self, tmp, 745008u, aux, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 119u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (p2 == 1u) { goto unreachable_path; }
    tmp = LoadU32(ctx, frame + 32u);
    p2 = tmp + 28u;
    tmp = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, tmp + 39u)) <= -1);
    aux = SkipWhitespace(ctx, tmp ? LoadU32(ctx, p2) : p2);
    p2 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 67u)) > -1);
    if (p2 != 0u) { goto diff_lookup; }
    p2 = LoadU32(ctx, frame + 56u);
    DeallocateRawBlock(ctx, p2);
  }
  // Merge point diff_lookup.
diff_lookup:;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 120u) {
    result = AllocateStringBuffer(ctx, 16u);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 120u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    block = result;
  }
  if (!(ResumePhase(ctx))) {
    StoreU32(ctx, block + 7u, LoadU32(ctx, 741351u));
    StoreU64(ctx, block, LoadU64(ctx, 741344u));
    StoreU8(ctx, block + 11u, 0u);
    tmp = 0u;
    p2 = LoadU32(ctx, p1 + 20u);
    p1 = !(p2);
  }
  if (p1 ? !(ResumePhase(ctx)) : 0u) { goto key_in_branch; }
  // Merge point key_walk_loop.
key_walk_loop:;
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU8(ctx, p2 + 27u);
      span = static_cast<uint32_t>((p1 & 0x80u) != 0u);
      p1 = span ? LoadU32(ctx, p2 + 20u) : p1;
      count = p1 < 11u;
      tmp = count ? p1 : 11u;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (tmp != 0u) {
        result = p2 + 16u;
        span = span ? LoadU32(ctx, result) : result;
        result = CompareBytes(ctx, block, span, tmp);
        if (!(result)) {
          p1 = p1 <= 11u;
          if (p1 != 0u) { goto key_walk_step; }
          goto key_walk_rewind;
        }
        p1 = static_cast<uint32_t>(static_cast<int32_t>(result) <= -1);
        if (p1 != 0u) { goto key_walk_rewind; }
  // Merge point key_walk_step.
key_walk_step:;
        p1 = CompareBytes(ctx, span, block, tmp);
        tmp = !(p1);
        if (tmp != 0u) { goto key_walk_found; }
        p1 = static_cast<uint32_t>(static_cast<int32_t>(p1) > -1);
        if (p1 != 0u) { goto key_walk_compare; }
        goto key_walk_next;
      }
      p1 = p1 > 11u;
      if (p1 != 0u) { goto key_walk_rewind; }
  // Merge point key_walk_found.
key_walk_found:;
      if (count != 0u) { goto key_walk_next; }
  // Merge point key_walk_compare.
key_walk_compare:;
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 121u) {
      result = HostFoldLayoutOffset(ctx, 1032u, 16u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 121u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p1 = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p2 == 1u) { goto unreachable_path; }
      StoreU32(ctx, frame + 56u, p1);
      StoreU64(ctx, frame + 60u, 9223372105574252555ull);
      StoreU8(ctx, p1 + 11u, 0u);
      StoreU32(ctx, p1 + 7u, LoadU32(ctx, 741351u));
      StoreU64(ctx, p1, LoadU64(ctx, 741344u));
      StoreU32(ctx, kDataD015C, 0u);
      StoreU32(ctx, frame + 16u, frame + 56u);
      p2 = frame + 56u;
      tmp = frame + 16u;
      p1 = frame + 32u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 122u) {
      HostInvokeViiiiii(ctx, 1959u, p1, self, p2, 745008u, tmp, frame);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 122u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p1 = p1 == 1u;
      if (p1 != 0u) { goto unreachable_path; }
      tmp = 1u;
      p1 = LoadU32(ctx, frame + 32u);
      count = LoadU32(ctx, p1 + 32u);
      p2 = LoadU8(ctx, p1 + 39u);
      span = static_cast<uint32_t>((p2 & 0x80u) != 0u);
      p2 = span ? count : p2 == 1u;
    }
    if (p2 || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p1 = p1 + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 123u) {
        result = HostInvokeIiiiii(ctx, 1354u, p1, 0u, 4294967295u, 740137u, 1u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 123u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p1 = result;
      }
      if (ResumePhase(ctx)) {
      } else {
        p2 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (p2 == 1u) { goto pending_throw_path; }
      }
      tmp = p1 != 0u;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 67u)) > -1);
      if (p1 != 0u) { goto key_in_branch; }
      p1 = LoadU32(ctx, frame + 56u);
      DeallocateRawBlock(ctx, p1);
      goto key_in_branch;
    }
  // Merge point key_walk_next.
key_walk_next:;
    p2 = ResumePhase(ctx) ? p2 : (p2 + 4u);
  // Merge point key_walk_rewind.
key_walk_rewind:;
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, p2);
      if (p2 != 0u) { goto key_walk_loop; }
    }
  tmp = ResumePhase(ctx) ? tmp : 0u;
  // Merge point key_in_branch.
key_in_branch:;
  if (ResumePhase(ctx) == kResumeRunning) {
    DeallocateRawBlock(ctx, block);
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, 1701869940u);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    p2 = frame + 56u;
    block = frame + 16u;
    p1 = frame + 32u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 124u) {
    HostInvokeViiiiii(ctx, 1959u, p1, self, p2, 745008u, block, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 124u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p1 = p1 == 1u;
    if (p1 != 0u) { goto unreachable_path; }
    p1 = LoadU32(ctx, frame + 32u);
    block = LoadU32(ctx, p1 + 32u);
    p2 = LoadU8(ctx, p1 + 39u);
    count = static_cast<uint32_t>((p2 & 0x80u) != 0u);
    p2 = count ? block : p2 != 2u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (p2 != 0u) { goto key_show_branch; }
    StoreU32(ctx, kDataD015C, 0u);
    p1 = p1 + 28u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 125u) {
    result = HostInvokeIiiiii(ctx, 1354u, p1, 0u, 4294967295u, 740723u, 2u);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 125u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    p1 = result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (p2 == 1u) { goto pending_throw_path; }
    if (p1 != 0u) { goto key_show_branch; }
    p1 = 0u;
    goto key_in_second_branch;
  }
  // Merge point key_show_branch.
key_show_branch:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 36u, 0u);
    StoreU32(ctx, frame + 32u, 1701869940u);
    StoreU8(ctx, frame + 43u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame, frame + 32u);
    p2 = frame + 32u;
    block = frame + 88u;
    p1 = frame + 16u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 126u) {
    HostInvokeViiiiii(ctx, 1959u, p1, self, p2, 745008u, frame, block);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 126u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p1 = p1 == 1u;
    if (p1 != 0u) { goto unreachable_path; }
    p1 = LoadU32(ctx, frame + 16u);
    block = LoadU32(ctx, p1 + 32u);
    p2 = LoadU8(ctx, p1 + 39u);
    count = static_cast<uint32_t>((p2 & 0x80u) != 0u);
    p2 = count ? block : p2 != 4u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (p2 != 0u) { goto key_backlog_down_branch; }
    StoreU32(ctx, kDataD015C, 0u);
    p1 = p1 + 28u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 127u) {
    result = HostInvokeIiiiii(ctx, 1354u, p1, 0u, 4294967295u, 740730u, 4u);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 127u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    p1 = result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (p2 == 1u) { goto pending_throw_path; }
    if (p1 != 0u) { goto key_backlog_down_branch; }
    p1 = 0u;
    goto backlog_down_next;
  }
  // Merge point key_backlog_down_branch.
key_backlog_down_branch:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 20u, 0u);
    StoreU32(ctx, frame + 16u, 1701869940u);
    StoreU8(ctx, frame + 27u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 88u, frame + 16u);
    p2 = frame + 88u;
    block = frame + 80u;
    p1 = frame + 16u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 128u) {
    HostInvokeViiiiii(ctx, 1959u, frame, self, p1, 745008u, p2, block);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 128u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p1 = p1 == 1u;
    if (p1 != 0u) { goto unreachable_path; }
    p1 = LoadU32(ctx, frame);
    block = LoadU32(ctx, p1 + 32u);
    p2 = LoadU8(ctx, p1 + 39u);
    count = static_cast<uint32_t>((p2 & 0x80u) != 0u);
    p2 = count ? block : p2 != 15u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (p2 != 0u) { goto key_backlog_up_branch; }
    StoreU32(ctx, kDataD015C, 0u);
    p1 = p1 + 28u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 129u) {
    result = HostInvokeIiiiii(ctx, 1354u, p1, 0u, 4294967295u, 740735u, 15u);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 129u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    p1 = result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (p2 == 1u) { goto pending_throw_path; }
    if (p1 != 0u) { goto key_backlog_up_branch; }
    goto backlog_up_next;
  }
  // Merge point key_backlog_up_branch.
key_backlog_up_branch:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 4u, 0u);
    StoreU32(ctx, frame, 1701869940u);
    StoreU8(ctx, frame + 11u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 80u, frame);
    p2 = frame + 80u;
    block = frame + 72u;
    p1 = frame + 88u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 130u) {
    HostInvokeViiiiii(ctx, 1959u, p1, self, frame, 745008u, p2, block);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 130u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p1 = p1 == 1u;
    if (p1 != 0u) { goto release_mode_scope; }
    p1 = LoadU32(ctx, frame + 88u);
    count = LoadU32(ctx, p1 + 32u);
    block = LoadU8(ctx, p1 + 39u);
    span = static_cast<uint32_t>((block & 0x80u) != 0u);
    block = span ? count : block == 13u;
    p2 = 0u;
  }
  if (block || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p1 = p1 + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 131u) {
      result = HostInvokeIiiiii(ctx, 1354u, p1, 0u, 4294967295u, 740768u, 13u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 131u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p1 = result;
    }
    if (ResumePhase(ctx)) {
    } else {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if ((p2 == 1u)) { goto pending_throw_path; }
    }
    p2 = !(p1);
  }
  if (ResumePhase(ctx)) {
  } else {
    if (static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 11u)) <= -1)) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame));
    }
  }
  // Merge point backlog_up_next.
backlog_up_next:;
  p1 = p2 ^ 1u;
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 27u)) > -1);
    if (p2 != 0u) { goto backlog_down_next; }
    p2 = LoadU32(ctx, frame + 16u);
    DeallocateRawBlock(ctx, p2);
  }
  // Merge point backlog_down_next.
backlog_down_next:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 43u)) > -1);
    if (p2 != 0u) { goto key_in_second_branch; }
    p2 = LoadU32(ctx, frame + 32u);
    DeallocateRawBlock(ctx, p2);
  }
  // Merge point key_in_second_branch.
key_in_second_branch:;
  if (ResumePhase(ctx) == kResumeRunning) {
    if (static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 67u)) <= -1)) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, 1701869940u);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    block = frame + 32u;
    count = frame + 56u;
    span = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 132u) {
    HostInvokeViiiiii(ctx, 1959u, block, self, count, 745008u, span, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 132u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    block = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    block = block == 1u;
    if (block != 0u) { goto unreachable_path; }
    block = LoadU32(ctx, frame + 32u);
    count = LoadU32(ctx, block + 32u);
    span = LoadU8(ctx, block + 39u);
    result = static_cast<uint32_t>((span & 0x80u) != 0u);
    count = result ? count : span == 2u;
  }
  if (count || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p2 = block + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 133u) {
      result = HostInvokeIiiiii(ctx, 1354u, p2, 0u, 4294967295u, 740723u, 2u);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 133u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p2 = result;
    }
    if (ResumePhase(ctx)) {
    } else {
      block = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      block = block == 1u;
      if (block != 0u) { goto pending_throw_path; }
    }
    p2 = !(p2);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    block = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 67u)) <= -1);
    if (block != 0u) {
      block = LoadU32(ctx, frame + 56u);
      DeallocateRawBlock(ctx, block);
    }
    p2 = !(p2);
    if (p2 != 0u) { goto key_in_third_branch; }
    p2 = p0 + 13308u;
    StoreU32(ctx, frame + 32u, p2);
    self = frame + 56u;
    block = frame + 32u;
    count = frame + 16u;
    p0 = p0 + 13536u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 134u) {
    UnorderedLinkBucketNode(ctx, self, p0, p2, 745008u, block, count);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 134u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    self = LoadU32(ctx, frame + 56u);
    p0 = LoadU32(ctx, self + 32u);
    if (p0 != LoadU32(ctx, self + 36u)) {
      StoreU8(ctx, p0 + 4u, flag);
      StoreU32(ctx, p0, tag);
      StoreU16(ctx, p0 + 5u, LoadU16(ctx, frame + 53u));
      StoreU8(ctx, p0 + 7u, LoadU8(ctx, frame + 55u));
      StoreU8(ctx, p0 + 24u, tmp);
      StoreU32(ctx, p0 + 20u, word);
      StoreU32(ctx, p0 + 16u, aux);
      StoreF32(ctx, p0 + 12u, num);
      StoreU32(ctx, p0 + 8u, slot);
      StoreU16(ctx, p0 + 25u, LoadU16(ctx, frame + 50u));
      p2 = LoadU8(ctx, frame + 52u);
      StoreU8(ctx, p0 + 27u, p2);
      StoreU8(ctx, p0 + 32u, p1);
      StoreU32(ctx, p0 + 28u, 0u);
      StoreU16(ctx, p0 + 33u, LoadU16(ctx, frame + 47u));
      StoreU8(ctx, p0 + 35u, LoadU8(ctx, frame + 49u));
      p0 = p0 + 36u;
      StoreU32(ctx, self + 32u, p0);
      goto done;
    }
    p0 = self + 28u;
    block = LoadU32(ctx, p0);
    span = p0 - block;
    p2 = static_cast<uint32_t>(static_cast<int32_t>(span) / static_cast<int32_t>(36u));
    count = p2 + 1u;
    if ((count >= 119304648u)) { goto alloc_assert_path; }
    result = p2 << 1;
    p0 = result < count;
    count = (p2 < 59652323u) ? p0 ? count : result : 119304647u;
    p0 = 0u;
  }
  if (count || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      if ((count >= 119304648u)) { goto alloc_limit_path; }
      p0 = count * 36u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 135u) {
      result = AllocateStringBuffer(ctx, p0);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 135u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      p0 = result;
    } else {
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = (p2 * 36u) + p0;
    StoreU8(ctx, p2 + 4u, flag);
    StoreU32(ctx, p2, tag);
    StoreU16(ctx, p2 + 5u, LoadU16(ctx, frame + 53u));
    StoreU8(ctx, p2 + 7u, LoadU8(ctx, frame + 55u));
    StoreU8(ctx, p2 + 24u, tmp);
    StoreU32(ctx, p2 + 20u, word);
    StoreU32(ctx, p2 + 16u, aux);
    StoreF32(ctx, p2 + 12u, num);
    StoreU32(ctx, p2 + 8u, slot);
    StoreU16(ctx, p2 + 25u, LoadU16(ctx, frame + 50u));
    StoreU8(ctx, p2 + 27u, LoadU8(ctx, frame + 52u));
    StoreU8(ctx, p2 + 32u, p1);
    StoreU32(ctx, p2 + 28u, 0u);
    StoreU16(ctx, p2 + 33u, LoadU16(ctx, frame + 47u));
    StoreU8(ctx, p2 + 35u, LoadU8(ctx, frame + 49u));
    p1 = p2 + (static_cast<uint32_t>(static_cast<int32_t>(span) / -36) * 36u);
    if (static_cast<uint32_t>(static_cast<int32_t>(span) >= 1u)) {
      CopyBytes(ctx, p1, block, span);
    }
    StoreU32(ctx, self + 36u, ((count * 36u) + p0));
    StoreU32(ctx, self + 32u, (p2 + 36u));
    StoreU32(ctx, self + 28u, p1);
    p0 = !(block);
    if (p0 != 0u) { goto done; }
    DeallocateRawBlock(ctx, block);
    goto done;
  }
  // Merge point release_mode_scope.
release_mode_scope:;
  if (ResumePhase(ctx) == kResumeRunning) {
    HostRet0(ctx);
    p0 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 11u)) > -1);
    if (p0 != 0u) { goto release_tote_scope; }
    p0 = LoadU32(ctx, frame);
    DeallocateRawBlock(ctx, p0);
  }
  // Merge point release_tote_scope.
release_tote_scope:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 27u)) > -1);
    if (p0 != 0u) { goto release_ease_scope; }
    p0 = LoadU32(ctx, frame + 16u);
    DeallocateRawBlock(ctx, p0);
  }
  // Merge point release_ease_scope.
release_ease_scope:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 43u)) > -1);
    if (p0 != 0u) { goto unreachable_path; }
    p0 = LoadU32(ctx, frame + 32u);
    DeallocateRawBlock(ctx, p0);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto unreachable_path;
  }
  // Merge point key_in_third_branch.
key_in_third_branch:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, 1701869940u);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    block = frame + 32u;
    count = frame + 56u;
    span = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 136u) {
    HostInvokeViiiiii(ctx, 1959u, block, self, count, 745008u, span, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 136u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    block = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    block = block != 1u;
  }
  if (block || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      block = LoadU32(ctx, frame + 32u);
      count = LoadU32(ctx, block + 32u);
      span = LoadU8(ctx, block + 39u);
      result = static_cast<uint32_t>((span & 0x80u) != 0u);
      count = result ? count : span == 3u;
    }
    if (count || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p2 = block + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 137u) {
        result = HostInvokeIiiiii(ctx, 1354u, p2, 0u, 4294967295u, 740726u, 3u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 137u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p2 = result;
      }
      if (ResumePhase(ctx)) {
      } else {
        block = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        block = block == 1u;
        if (block != 0u) { goto pending_throw_path; }
      }
      p2 = !(p2);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      block = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 67u)) <= -1);
      if (block != 0u) {
        block = LoadU32(ctx, frame + 56u);
        DeallocateRawBlock(ctx, block);
      }
      p2 = !(p2);
      if (p2 != 0u) { goto key_show_setter; }
      p2 = p0 + 13308u;
      StoreU32(ctx, frame + 32u, p2);
      self = frame + 56u;
      block = frame + 32u;
      count = frame + 16u;
      p0 = p0 + 13548u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 138u) {
      UnorderedLinkBucketNode(ctx, self, p0, p2, 745008u, block, count);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 138u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      self = LoadU32(ctx, frame + 56u);
      p0 = LoadU32(ctx, self + 32u);
      if (p0 != LoadU32(ctx, self + 36u)) {
        StoreU8(ctx, p0 + 4u, flag);
        StoreU32(ctx, p0, tag);
        StoreU16(ctx, p0 + 5u, LoadU16(ctx, frame + 53u));
        StoreU8(ctx, p0 + 7u, LoadU8(ctx, frame + 55u));
        StoreU8(ctx, p0 + 24u, tmp);
        StoreU32(ctx, p0 + 20u, word);
        StoreU32(ctx, p0 + 16u, aux);
        StoreF32(ctx, p0 + 12u, num);
        StoreU32(ctx, p0 + 8u, slot);
        StoreU16(ctx, p0 + 25u, LoadU16(ctx, frame + 50u));
        p2 = LoadU8(ctx, frame + 52u);
        StoreU8(ctx, p0 + 27u, p2);
        StoreU8(ctx, p0 + 32u, p1);
        StoreU32(ctx, p0 + 28u, 0u);
        StoreU16(ctx, p0 + 33u, LoadU16(ctx, frame + 47u));
        StoreU8(ctx, p0 + 35u, LoadU8(ctx, frame + 49u));
        p0 = p0 + 36u;
        StoreU32(ctx, self + 32u, p0);
        goto done;
      }
      p0 = self + 28u;
      block = LoadU32(ctx, p0);
      span = p0 - block;
      p2 = static_cast<uint32_t>(static_cast<int32_t>(span) / static_cast<int32_t>(36u));
      count = p2 + 1u;
      if ((count >= 119304648u)) { goto alloc_assert_path; }
      result = p2 << 1;
      p0 = result < count;
      count = (p2 < 59652323u) ? p0 ? count : result : 119304647u;
      p0 = 0u;
    }
    if (count || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        if ((count >= 119304648u)) { goto alloc_limit_path; }
        p0 = count * 36u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 139u) {
        result = AllocateStringBuffer(ctx, p0);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 139u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p0 = result;
      } else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = (p2 * 36u) + p0;
      StoreU8(ctx, p2 + 4u, flag);
      StoreU32(ctx, p2, tag);
      StoreU16(ctx, p2 + 5u, LoadU16(ctx, frame + 53u));
      StoreU8(ctx, p2 + 7u, LoadU8(ctx, frame + 55u));
      StoreU8(ctx, p2 + 24u, tmp);
      StoreU32(ctx, p2 + 20u, word);
      StoreU32(ctx, p2 + 16u, aux);
      StoreF32(ctx, p2 + 12u, num);
      StoreU32(ctx, p2 + 8u, slot);
      StoreU16(ctx, p2 + 25u, LoadU16(ctx, frame + 50u));
      StoreU8(ctx, p2 + 27u, LoadU8(ctx, frame + 52u));
      StoreU8(ctx, p2 + 32u, p1);
      StoreU32(ctx, p2 + 28u, 0u);
      StoreU16(ctx, p2 + 33u, LoadU16(ctx, frame + 47u));
      StoreU8(ctx, p2 + 35u, LoadU8(ctx, frame + 49u));
      p1 = p2 + (static_cast<uint32_t>(static_cast<int32_t>(span) / -36) * 36u);
      if (static_cast<uint32_t>(static_cast<int32_t>(span) >= 1u)) {
        CopyBytes(ctx, p1, block, span);
      }
      StoreU32(ctx, self + 36u, ((count * 36u) + p0));
      StoreU32(ctx, self + 32u, (p2 + 36u));
      StoreU32(ctx, self + 28u, p1);
      p0 = !(block);
      if (p0 != 0u) { goto done; }
      DeallocateRawBlock(ctx, block);
      goto done;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    HostRet0(ctx);
    p0 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 67u)) > -1);
    if (p0 != 0u) { goto unreachable_path; }
    goto unreachable_path;
  }
  // Merge point key_show_setter.
key_show_setter:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, 1701869940u);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    block = frame + 32u;
    count = frame + 56u;
    span = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 140u) {
    HostInvokeViiiiii(ctx, 1959u, block, self, count, 745008u, span, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 140u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    block = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    block = block != 1u;
  }
  if (block || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      block = LoadU32(ctx, frame + 32u);
      count = LoadU32(ctx, block + 32u);
      span = LoadU8(ctx, block + 39u);
      result = static_cast<uint32_t>((span & 0x80u) != 0u);
      count = result ? count : span == 4u;
    }
    if (count || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p2 = block + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 141u) {
        result = HostInvokeIiiiii(ctx, 1354u, p2, 0u, 4294967295u, 740730u, 4u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 141u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p2 = result;
      }
      if (ResumePhase(ctx)) {
      } else {
        block = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        block = block == 1u;
        if (block != 0u) { goto pending_throw_path; }
      }
      p2 = !(p2);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      block = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 67u)) <= -1);
      if (block != 0u) {
        block = LoadU32(ctx, frame + 56u);
        DeallocateRawBlock(ctx, block);
      }
      p2 = !(p2);
      if (p2 != 0u) { goto key_len4_setter; }
      p2 = p0 + 13308u;
      StoreU32(ctx, frame + 32u, p2);
      self = frame + 56u;
      block = frame + 32u;
      count = frame + 16u;
      p0 = p0 + 13560u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 142u) {
      UnorderedLinkBucketNode(ctx, self, p0, p2, 745008u, block, count);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 142u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      self = LoadU32(ctx, frame + 56u);
      p0 = LoadU32(ctx, self + 32u);
      if (p0 != LoadU32(ctx, self + 36u)) {
        StoreU8(ctx, p0 + 4u, flag);
        StoreU32(ctx, p0, tag);
        StoreU16(ctx, p0 + 5u, LoadU16(ctx, frame + 53u));
        StoreU8(ctx, p0 + 7u, LoadU8(ctx, frame + 55u));
        StoreU8(ctx, p0 + 24u, tmp);
        StoreU32(ctx, p0 + 20u, word);
        StoreU32(ctx, p0 + 16u, aux);
        StoreF32(ctx, p0 + 12u, num);
        StoreU32(ctx, p0 + 8u, slot);
        StoreU16(ctx, p0 + 25u, LoadU16(ctx, frame + 50u));
        p2 = LoadU8(ctx, frame + 52u);
        StoreU8(ctx, p0 + 27u, p2);
        StoreU8(ctx, p0 + 32u, p1);
        StoreU32(ctx, p0 + 28u, 0u);
        StoreU16(ctx, p0 + 33u, LoadU16(ctx, frame + 47u));
        StoreU8(ctx, p0 + 35u, LoadU8(ctx, frame + 49u));
        p0 = p0 + 36u;
        StoreU32(ctx, self + 32u, p0);
        goto done;
      }
      p0 = self + 28u;
      block = LoadU32(ctx, p0);
      span = p0 - block;
      p2 = static_cast<uint32_t>(static_cast<int32_t>(span) / static_cast<int32_t>(36u));
      count = p2 + 1u;
      if ((count >= 119304648u)) { goto alloc_assert_path; }
      result = p2 << 1;
      p0 = result < count;
      count = (p2 < 59652323u) ? p0 ? count : result : 119304647u;
      p0 = 0u;
    }
    if (count || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        if ((count >= 119304648u)) { goto alloc_limit_path; }
        p0 = count * 36u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 143u) {
        result = AllocateStringBuffer(ctx, p0);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 143u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p0 = result;
      } else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = (p2 * 36u) + p0;
      StoreU8(ctx, p2 + 4u, flag);
      StoreU32(ctx, p2, tag);
      StoreU16(ctx, p2 + 5u, LoadU16(ctx, frame + 53u));
      StoreU8(ctx, p2 + 7u, LoadU8(ctx, frame + 55u));
      StoreU8(ctx, p2 + 24u, tmp);
      StoreU32(ctx, p2 + 20u, word);
      StoreU32(ctx, p2 + 16u, aux);
      StoreF32(ctx, p2 + 12u, num);
      StoreU32(ctx, p2 + 8u, slot);
      StoreU16(ctx, p2 + 25u, LoadU16(ctx, frame + 50u));
      StoreU8(ctx, p2 + 27u, LoadU8(ctx, frame + 52u));
      StoreU8(ctx, p2 + 32u, p1);
      StoreU32(ctx, p2 + 28u, 0u);
      StoreU16(ctx, p2 + 33u, LoadU16(ctx, frame + 47u));
      StoreU8(ctx, p2 + 35u, LoadU8(ctx, frame + 49u));
      p1 = p2 + (static_cast<uint32_t>(static_cast<int32_t>(span) / -36) * 36u);
      if (static_cast<uint32_t>(static_cast<int32_t>(span) >= 1u)) {
        CopyBytes(ctx, p1, block, span);
      }
      StoreU32(ctx, self + 36u, ((count * 36u) + p0));
      StoreU32(ctx, self + 32u, (p2 + 36u));
      StoreU32(ctx, self + 28u, p1);
      p0 = !(block);
      if (p0 != 0u) { goto done; }
      DeallocateRawBlock(ctx, block);
      goto done;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    HostRet0(ctx);
    p0 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 67u)) > -1);
    if (p0 != 0u) { goto unreachable_path; }
    goto unreachable_path;
  }
  // Merge point key_len4_setter.
key_len4_setter:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, 1701869940u);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    block = frame + 32u;
    count = frame + 56u;
    span = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 144u) {
    HostInvokeViiiiii(ctx, 1959u, block, self, count, 745008u, span, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 144u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    block = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    block = block != 1u;
  }
  if (block || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      block = LoadU32(ctx, frame + 32u);
      count = LoadU32(ctx, block + 32u);
      span = LoadU8(ctx, block + 39u);
      result = static_cast<uint32_t>((span & 0x80u) != 0u);
      count = result ? count : span == 4u;
    }
    if (count || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p2 = block + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 145u) {
        result = HostInvokeIiiiii(ctx, 1354u, p2, 0u, 4294967295u, 718402u, 4u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 145u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p2 = result;
      }
      if (ResumePhase(ctx)) {
      } else {
        block = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        block = block == 1u;
        if (block != 0u) { goto pending_throw_path; }
      }
      p2 = !(p2);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      block = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 67u)) <= -1);
      if (block != 0u) {
        block = LoadU32(ctx, frame + 56u);
        DeallocateRawBlock(ctx, block);
      }
      p2 = !(p2);
      if (p2 != 0u) { goto key_backlog_down_setter; }
      p2 = p0 + 13308u;
      StoreU32(ctx, frame + 32u, p2);
      self = frame + 56u;
      block = frame + 32u;
      count = frame + 16u;
      p0 = p0 + 13572u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 146u) {
      UnorderedLinkBucketNode(ctx, self, p0, p2, 745008u, block, count);
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
    #endif
      resume_point = 146u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      self = LoadU32(ctx, frame + 56u);
      p0 = LoadU32(ctx, self + 32u);
      if (p0 != LoadU32(ctx, self + 36u)) {
        StoreU8(ctx, p0 + 4u, flag);
        StoreU32(ctx, p0, tag);
        StoreU16(ctx, p0 + 5u, LoadU16(ctx, frame + 53u));
        StoreU8(ctx, p0 + 7u, LoadU8(ctx, frame + 55u));
        StoreU8(ctx, p0 + 24u, tmp);
        StoreU32(ctx, p0 + 20u, word);
        StoreU32(ctx, p0 + 16u, aux);
        StoreF32(ctx, p0 + 12u, num);
        StoreU32(ctx, p0 + 8u, slot);
        StoreU16(ctx, p0 + 25u, LoadU16(ctx, frame + 50u));
        p2 = LoadU8(ctx, frame + 52u);
        StoreU8(ctx, p0 + 27u, p2);
        StoreU8(ctx, p0 + 32u, p1);
        StoreU32(ctx, p0 + 28u, 0u);
        StoreU16(ctx, p0 + 33u, LoadU16(ctx, frame + 47u));
        StoreU8(ctx, p0 + 35u, LoadU8(ctx, frame + 49u));
        p0 = p0 + 36u;
        StoreU32(ctx, self + 32u, p0);
        goto done;
      }
      p0 = self + 28u;
      block = LoadU32(ctx, p0);
      span = p0 - block;
      p2 = static_cast<uint32_t>(static_cast<int32_t>(span) / static_cast<int32_t>(36u));
      count = p2 + 1u;
      if ((count >= 119304648u)) { goto alloc_assert_path; }
      result = p2 << 1;
      p0 = result < count;
      count = (p2 < 59652323u) ? p0 ? count : result : 119304647u;
      p0 = 0u;
    }
    if (count || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        if ((count >= 119304648u)) { goto alloc_limit_path; }
        p0 = count * 36u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 147u) {
        result = AllocateStringBuffer(ctx, p0);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 147u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p0 = result;
      } else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = (p2 * 36u) + p0;
      StoreU8(ctx, p2 + 4u, flag);
      StoreU32(ctx, p2, tag);
      StoreU16(ctx, p2 + 5u, LoadU16(ctx, frame + 53u));
      StoreU8(ctx, p2 + 7u, LoadU8(ctx, frame + 55u));
      StoreU8(ctx, p2 + 24u, tmp);
      StoreU32(ctx, p2 + 20u, word);
      StoreU32(ctx, p2 + 16u, aux);
      StoreF32(ctx, p2 + 12u, num);
      StoreU32(ctx, p2 + 8u, slot);
      StoreU16(ctx, p2 + 25u, LoadU16(ctx, frame + 50u));
      StoreU8(ctx, p2 + 27u, LoadU8(ctx, frame + 52u));
      StoreU8(ctx, p2 + 32u, p1);
      StoreU32(ctx, p2 + 28u, 0u);
      StoreU16(ctx, p2 + 33u, LoadU16(ctx, frame + 47u));
      StoreU8(ctx, p2 + 35u, LoadU8(ctx, frame + 49u));
      p1 = p2 + (static_cast<uint32_t>(static_cast<int32_t>(span) / -36) * 36u);
      if (static_cast<uint32_t>(static_cast<int32_t>(span) >= 1u)) {
        CopyBytes(ctx, p1, block, span);
      }
      StoreU32(ctx, self + 36u, ((count * 36u) + p0));
      StoreU32(ctx, self + 32u, (p2 + 36u));
      StoreU32(ctx, self + 28u, p1);
      p0 = !(block);
      if (p0 != 0u) { goto done; }
      DeallocateRawBlock(ctx, block);
      goto done;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    HostRet0(ctx);
    p0 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 67u)) > -1);
    if (p0 != 0u) { goto unreachable_path; }
    goto unreachable_path;
  }
  // Merge point key_backlog_down_setter.
key_backlog_down_setter:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, 1701869940u);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    block = frame + 32u;
    count = frame + 56u;
    span = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 148u) {
    HostInvokeViiiiii(ctx, 1959u, block, self, count, 745008u, span, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 148u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    block = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    block = block != 1u;
  }
  if (block || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      block = LoadU32(ctx, frame + 32u);
      count = LoadU32(ctx, block + 32u);
      span = LoadU8(ctx, block + 39u);
      result = static_cast<uint32_t>((span & 0x80u) != 0u);
      count = result ? count : span == 15u;
    }
    if (count || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p2 = block + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 149u) {
        result = HostInvokeIiiiii(ctx, 1354u, p2, 0u, 4294967295u, 740735u, 15u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 149u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p2 = result;
      }
      if (ResumePhase(ctx)) {
      } else {
        block = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        block = block == 1u;
        if (block != 0u) { goto pending_throw_path; }
      }
      p2 = !(p2);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      block = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 67u)) <= -1);
      if (block != 0u) {
        block = LoadU32(ctx, frame + 56u);
        DeallocateRawBlock(ctx, block);
      }
      p2 = !(p2);
      if (p2 != 0u) { goto key_len16_setter; }
      self = LoadU32(ctx, p0 + 120u);
      p0 = !(self);
    }
    if (p0 || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 150u) {
        HostFailAssertion(ctx, 857u, 745009u, 745017u, 734u, 745073u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 150u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p0 = p0 != 1u;
        if (p0 != 0u) { goto trap_path; }
        goto pending_throw_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, self + 596u);
      if (p0 != LoadU32(ctx, self + 600u)) {
        StoreU8(ctx, p0 + 4u, flag);
        StoreU32(ctx, p0, tag);
        StoreU16(ctx, p0 + 5u, LoadU16(ctx, frame + 53u));
        StoreU8(ctx, p0 + 7u, LoadU8(ctx, frame + 55u));
        StoreU8(ctx, p0 + 24u, tmp);
        StoreU32(ctx, p0 + 20u, word);
        StoreU32(ctx, p0 + 16u, aux);
        StoreF32(ctx, p0 + 12u, num);
        StoreU32(ctx, p0 + 8u, slot);
        StoreU16(ctx, p0 + 25u, LoadU16(ctx, frame + 50u));
        p2 = LoadU8(ctx, frame + 52u);
        StoreU8(ctx, p0 + 27u, p2);
        StoreU8(ctx, p0 + 32u, p1);
        StoreU32(ctx, p0 + 28u, 0u);
        StoreU16(ctx, p0 + 33u, LoadU16(ctx, frame + 47u));
        StoreU8(ctx, p0 + 35u, LoadU8(ctx, frame + 49u));
        p0 = p0 + 36u;
        StoreU32(ctx, self + 596u, p0);
        goto done;
      }
      p0 = self + 592u;
      block = LoadU32(ctx, p0);
      span = p0 - block;
      p2 = static_cast<uint32_t>(static_cast<int32_t>(span) / static_cast<int32_t>(36u));
      count = p2 + 1u;
      if ((count >= 119304648u)) { goto alloc_assert_path; }
      result = p2 << 1;
      p0 = result < count;
      count = (p2 < 59652323u) ? p0 ? count : result : 119304647u;
      p0 = 0u;
    }
    if (count || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        if ((count >= 119304648u)) { goto alloc_limit_path; }
        p0 = count * 36u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 151u) {
        result = AllocateStringBuffer(ctx, p0);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 151u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p0 = result;
      } else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = (p2 * 36u) + p0;
      StoreU8(ctx, p2 + 4u, flag);
      StoreU32(ctx, p2, tag);
      StoreU16(ctx, p2 + 5u, LoadU16(ctx, frame + 53u));
      StoreU8(ctx, p2 + 7u, LoadU8(ctx, frame + 55u));
      StoreU8(ctx, p2 + 24u, tmp);
      StoreU32(ctx, p2 + 20u, word);
      StoreU32(ctx, p2 + 16u, aux);
      StoreF32(ctx, p2 + 12u, num);
      StoreU32(ctx, p2 + 8u, slot);
      StoreU16(ctx, p2 + 25u, LoadU16(ctx, frame + 50u));
      StoreU8(ctx, p2 + 27u, LoadU8(ctx, frame + 52u));
      StoreU8(ctx, p2 + 32u, p1);
      StoreU32(ctx, p2 + 28u, 0u);
      StoreU16(ctx, p2 + 33u, LoadU16(ctx, frame + 47u));
      StoreU8(ctx, p2 + 35u, LoadU8(ctx, frame + 49u));
      p1 = p2 + (static_cast<uint32_t>(static_cast<int32_t>(span) / -36) * 36u);
      if (static_cast<uint32_t>(static_cast<int32_t>(span) >= 1u)) {
        CopyBytes(ctx, p1, block, span);
      }
      StoreU32(ctx, self + 600u, ((count * 36u) + p0));
      StoreU32(ctx, self + 596u, (p2 + 36u));
      StoreU32(ctx, self + 592u, p1);
      p0 = !(block);
      if (p0 != 0u) { goto done; }
      DeallocateRawBlock(ctx, block);
      goto done;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    HostRet0(ctx);
    p0 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 67u)) > -1);
    if (p0 != 0u) { goto unreachable_path; }
    goto unreachable_path;
  }
  // Merge point key_len16_setter.
key_len16_setter:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, 1701869940u);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    block = frame + 32u;
    count = frame + 56u;
    span = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 152u) {
    HostInvokeViiiiii(ctx, 1959u, block, self, count, 745008u, span, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 152u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    block = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    block = block != 1u;
  }
  if (block || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      block = LoadU32(ctx, frame + 32u);
      count = LoadU32(ctx, block + 32u);
      span = LoadU8(ctx, block + 39u);
      result = static_cast<uint32_t>((span & 0x80u) != 0u);
      count = result ? count : span == 16u;
    }
    if (count || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p2 = block + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 153u) {
        result = HostInvokeIiiiii(ctx, 1354u, p2, 0u, 4294967295u, 740751u, 16u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 153u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p2 = result;
      }
      if (ResumePhase(ctx)) {
      } else {
        block = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        block = block == 1u;
        if (block != 0u) { goto pending_throw_path; }
      }
      p2 = !(p2);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      block = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 67u)) <= -1);
      if (block != 0u) {
        block = LoadU32(ctx, frame + 56u);
        DeallocateRawBlock(ctx, block);
      }
      p2 = !(p2);
      if (p2 != 0u) { goto key_backlog_up_setter; }
      self = LoadU32(ctx, p0 + 120u);
      p0 = !(self);
    }
    if (p0 || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 154u) {
        HostFailAssertion(ctx, 857u, 745009u, 745017u, 734u, 745073u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 154u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p0 = p0 != 1u;
        if (p0 != 0u) { goto trap_path; }
        goto pending_throw_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, self + 608u);
      if (p0 != LoadU32(ctx, self + 612u)) {
        StoreU8(ctx, p0 + 4u, flag);
        StoreU32(ctx, p0, tag);
        StoreU16(ctx, p0 + 5u, LoadU16(ctx, frame + 53u));
        StoreU8(ctx, p0 + 7u, LoadU8(ctx, frame + 55u));
        StoreU8(ctx, p0 + 24u, tmp);
        StoreU32(ctx, p0 + 20u, word);
        StoreU32(ctx, p0 + 16u, aux);
        StoreF32(ctx, p0 + 12u, num);
        StoreU32(ctx, p0 + 8u, slot);
        StoreU16(ctx, p0 + 25u, LoadU16(ctx, frame + 50u));
        p2 = LoadU8(ctx, frame + 52u);
        StoreU8(ctx, p0 + 27u, p2);
        StoreU8(ctx, p0 + 32u, p1);
        StoreU32(ctx, p0 + 28u, 0u);
        StoreU16(ctx, p0 + 33u, LoadU16(ctx, frame + 47u));
        StoreU8(ctx, p0 + 35u, LoadU8(ctx, frame + 49u));
        p0 = p0 + 36u;
        StoreU32(ctx, self + 608u, p0);
        goto done;
      }
      p0 = self + 604u;
      block = LoadU32(ctx, p0);
      span = p0 - block;
      p2 = static_cast<uint32_t>(static_cast<int32_t>(span) / static_cast<int32_t>(36u));
      count = p2 + 1u;
      if ((count >= 119304648u)) { goto alloc_assert_path; }
      result = p2 << 1;
      p0 = result < count;
      count = (p2 < 59652323u) ? p0 ? count : result : 119304647u;
      p0 = 0u;
    }
    if (count || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        if ((count >= 119304648u)) { goto alloc_limit_path; }
        p0 = count * 36u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 155u) {
        result = AllocateStringBuffer(ctx, p0);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 155u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p0 = result;
      } else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = (p2 * 36u) + p0;
      StoreU8(ctx, p2 + 4u, flag);
      StoreU32(ctx, p2, tag);
      StoreU16(ctx, p2 + 5u, LoadU16(ctx, frame + 53u));
      StoreU8(ctx, p2 + 7u, LoadU8(ctx, frame + 55u));
      StoreU8(ctx, p2 + 24u, tmp);
      StoreU32(ctx, p2 + 20u, word);
      StoreU32(ctx, p2 + 16u, aux);
      StoreF32(ctx, p2 + 12u, num);
      StoreU32(ctx, p2 + 8u, slot);
      StoreU16(ctx, p2 + 25u, LoadU16(ctx, frame + 50u));
      StoreU8(ctx, p2 + 27u, LoadU8(ctx, frame + 52u));
      StoreU8(ctx, p2 + 32u, p1);
      StoreU32(ctx, p2 + 28u, 0u);
      StoreU16(ctx, p2 + 33u, LoadU16(ctx, frame + 47u));
      StoreU8(ctx, p2 + 35u, LoadU8(ctx, frame + 49u));
      p1 = p2 + (static_cast<uint32_t>(static_cast<int32_t>(span) / -36) * 36u);
      if (static_cast<uint32_t>(static_cast<int32_t>(span) >= 1u)) {
        CopyBytes(ctx, p1, block, span);
      }
      StoreU32(ctx, self + 612u, ((count * 36u) + p0));
      StoreU32(ctx, self + 608u, (p2 + 36u));
      StoreU32(ctx, self + 604u, p1);
      p0 = !(block);
      if (p0 != 0u) { goto done; }
      DeallocateRawBlock(ctx, block);
      goto done;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    HostRet0(ctx);
    p0 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 67u)) > -1);
    if (p0 != 0u) { goto unreachable_path; }
    goto unreachable_path;
  }
  // Merge point key_backlog_up_setter.
key_backlog_up_setter:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, 1701869940u);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    block = frame + 32u;
    count = frame + 56u;
    span = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 156u) {
    HostInvokeViiiiii(ctx, 1959u, block, self, count, 745008u, span, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 156u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    block = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    block = block != 1u;
  }
  if (block || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      block = LoadU32(ctx, frame + 32u);
      count = LoadU32(ctx, block + 32u);
      span = LoadU8(ctx, block + 39u);
      result = static_cast<uint32_t>((span & 0x80u) != 0u);
      count = result ? count : span == 13u;
    }
    if (count || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p2 = block + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 157u) {
        result = HostInvokeIiiiii(ctx, 1354u, p2, 0u, 4294967295u, 740768u, 13u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 157u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p2 = result;
      }
      if (ResumePhase(ctx)) {
      } else {
        block = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        block = block == 1u;
        if (block != 0u) { goto pending_throw_path; }
      }
      p2 = !(p2);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      block = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 67u)) <= -1);
      if (block != 0u) {
        block = LoadU32(ctx, frame + 56u);
        DeallocateRawBlock(ctx, block);
      }
      p2 = !(p2);
      if (p2 != 0u) { goto key_len14_setter; }
      self = LoadU32(ctx, p0 + 120u);
      p0 = !(self);
    }
    if (p0 || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 158u) {
        HostFailAssertion(ctx, 857u, 745009u, 745017u, 734u, 745073u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 158u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p0 = p0 != 1u;
        if (p0 != 0u) { goto trap_path; }
        goto pending_throw_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, self + 620u);
      if (p0 != LoadU32(ctx, self + 624u)) {
        StoreU8(ctx, p0 + 4u, flag);
        StoreU32(ctx, p0, tag);
        StoreU16(ctx, p0 + 5u, LoadU16(ctx, frame + 53u));
        StoreU8(ctx, p0 + 7u, LoadU8(ctx, frame + 55u));
        StoreU8(ctx, p0 + 24u, tmp);
        StoreU32(ctx, p0 + 20u, word);
        StoreU32(ctx, p0 + 16u, aux);
        StoreF32(ctx, p0 + 12u, num);
        StoreU32(ctx, p0 + 8u, slot);
        StoreU16(ctx, p0 + 25u, LoadU16(ctx, frame + 50u));
        p2 = LoadU8(ctx, frame + 52u);
        StoreU8(ctx, p0 + 27u, p2);
        StoreU8(ctx, p0 + 32u, p1);
        StoreU32(ctx, p0 + 28u, 0u);
        StoreU16(ctx, p0 + 33u, LoadU16(ctx, frame + 47u));
        StoreU8(ctx, p0 + 35u, LoadU8(ctx, frame + 49u));
        p0 = p0 + 36u;
        StoreU32(ctx, self + 620u, p0);
        goto done;
      }
      p0 = self + 616u;
      block = LoadU32(ctx, p0);
      span = p0 - block;
      p2 = static_cast<uint32_t>(static_cast<int32_t>(span) / static_cast<int32_t>(36u));
      count = p2 + 1u;
      if ((count >= 119304648u)) { goto alloc_assert_path; }
      result = p2 << 1;
      p0 = result < count;
      count = (p2 < 59652323u) ? p0 ? count : result : 119304647u;
      p0 = 0u;
    }
    if (count || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        if ((count >= 119304648u)) { goto alloc_limit_path; }
        p0 = count * 36u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 159u) {
        result = AllocateStringBuffer(ctx, p0);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 159u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p0 = result;
      } else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = (p2 * 36u) + p0;
      StoreU8(ctx, p2 + 4u, flag);
      StoreU32(ctx, p2, tag);
      StoreU16(ctx, p2 + 5u, LoadU16(ctx, frame + 53u));
      StoreU8(ctx, p2 + 7u, LoadU8(ctx, frame + 55u));
      StoreU8(ctx, p2 + 24u, tmp);
      StoreU32(ctx, p2 + 20u, word);
      StoreU32(ctx, p2 + 16u, aux);
      StoreF32(ctx, p2 + 12u, num);
      StoreU32(ctx, p2 + 8u, slot);
      StoreU16(ctx, p2 + 25u, LoadU16(ctx, frame + 50u));
      StoreU8(ctx, p2 + 27u, LoadU8(ctx, frame + 52u));
      StoreU8(ctx, p2 + 32u, p1);
      StoreU32(ctx, p2 + 28u, 0u);
      StoreU16(ctx, p2 + 33u, LoadU16(ctx, frame + 47u));
      StoreU8(ctx, p2 + 35u, LoadU8(ctx, frame + 49u));
      p1 = p2 + (static_cast<uint32_t>(static_cast<int32_t>(span) / -36) * 36u);
      if (static_cast<uint32_t>(static_cast<int32_t>(span) >= 1u)) {
        CopyBytes(ctx, p1, block, span);
      }
      StoreU32(ctx, self + 624u, ((count * 36u) + p0));
      StoreU32(ctx, self + 620u, (p2 + 36u));
      StoreU32(ctx, self + 616u, p1);
      p0 = !(block);
      if (p0 != 0u) { goto done; }
      DeallocateRawBlock(ctx, block);
      goto done;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    HostRet0(ctx);
    p0 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 67u)) > -1);
    if (p0 != 0u) { goto unreachable_path; }
    goto unreachable_path;
  }
  // Merge point key_len14_setter.
key_len14_setter:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 60u, 0u);
    StoreU32(ctx, frame + 56u, 1701869940u);
    StoreU8(ctx, frame + 67u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 16u, frame + 56u);
    block = frame + 32u;
    count = frame + 56u;
    span = frame + 16u;
    p2 = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 160u) {
    HostInvokeViiiiii(ctx, 1959u, block, self, count, 745008u, span, frame);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 160u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    self = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    self = self != 1u;
  }
  if (self || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      self = LoadU32(ctx, frame + 32u);
      count = LoadU32(ctx, self + 32u);
      block = LoadU8(ctx, self + 39u);
      span = static_cast<uint32_t>((block & 0x80u) != 0u);
      block = span ? count : block == 14u;
    }
    if (block || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p2 = self + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 161u) {
        result = HostInvokeIiiiii(ctx, 1354u, p2, 0u, 4294967295u, 740782u, 14u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 161u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p2 = result;
      }
      if (ResumePhase(ctx)) {
      } else {
        self = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        self = self == 1u;
        if (self != 0u) { goto pending_throw_path; }
      }
      p2 = !(p2);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 67u)) <= -1)) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
      }
      p2 = !(p2);
      if (p2 != 0u) { goto done; }
      self = LoadU32(ctx, p0 + 120u);
      p0 = !(self);
    }
    if (p0 || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 162u) {
        HostFailAssertion(ctx, 857u, 745009u, 745017u, 734u, 745073u);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 162u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p0 = p0 != 1u;
        if (p0 != 0u) { goto trap_path; }
        goto pending_throw_path;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, self + 632u);
      if (p0 != LoadU32(ctx, self + 636u)) {
        StoreU8(ctx, p0 + 4u, flag);
        StoreU32(ctx, p0, tag);
        StoreU16(ctx, p0 + 5u, LoadU16(ctx, frame + 53u));
        StoreU8(ctx, p0 + 7u, LoadU8(ctx, frame + 55u));
        StoreU8(ctx, p0 + 24u, tmp);
        StoreU32(ctx, p0 + 20u, word);
        StoreU32(ctx, p0 + 16u, aux);
        StoreF32(ctx, p0 + 12u, num);
        StoreU32(ctx, p0 + 8u, slot);
        StoreU16(ctx, p0 + 25u, LoadU16(ctx, frame + 50u));
        p2 = LoadU8(ctx, frame + 52u);
        StoreU8(ctx, p0 + 27u, p2);
        StoreU8(ctx, p0 + 32u, p1);
        StoreU32(ctx, p0 + 28u, 0u);
        StoreU16(ctx, p0 + 33u, LoadU16(ctx, frame + 47u));
        StoreU8(ctx, p0 + 35u, LoadU8(ctx, frame + 49u));
        p0 = p0 + 36u;
        StoreU32(ctx, self + 632u, p0);
        goto done;
      }
      p0 = self + 628u;
      block = LoadU32(ctx, p0);
      span = p0 - block;
      p2 = static_cast<uint32_t>(static_cast<int32_t>(span) / static_cast<int32_t>(36u));
      count = p2 + 1u;
      if ((count >= 119304648u)) { goto alloc_assert_path; }
      result = p2 << 1;
      p0 = result < count;
      count = (p2 < 59652323u) ? p0 ? count : result : 119304647u;
      p0 = 0u;
    }
    if (count || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        if ((count >= 119304648u)) { goto alloc_limit_path; }
        p0 = count * 36u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 163u) {
        result = AllocateStringBuffer(ctx, p0);
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
      #endif
        resume_point = 163u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
        p0 = result;
      } else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = (p2 * 36u) + p0;
      StoreU8(ctx, p2 + 4u, flag);
      StoreU32(ctx, p2, tag);
      StoreU16(ctx, p2 + 5u, LoadU16(ctx, frame + 53u));
      StoreU8(ctx, p2 + 7u, LoadU8(ctx, frame + 55u));
      StoreU8(ctx, p2 + 24u, tmp);
      StoreU32(ctx, p2 + 20u, word);
      StoreU32(ctx, p2 + 16u, aux);
      StoreF32(ctx, p2 + 12u, num);
      StoreU32(ctx, p2 + 8u, slot);
      StoreU16(ctx, p2 + 25u, LoadU16(ctx, frame + 50u));
      StoreU8(ctx, p2 + 27u, LoadU8(ctx, frame + 52u));
      StoreU8(ctx, p2 + 32u, p1);
      StoreU32(ctx, p2 + 28u, 0u);
      StoreU16(ctx, p2 + 33u, LoadU16(ctx, frame + 47u));
      StoreU8(ctx, p2 + 35u, LoadU8(ctx, frame + 49u));
      p1 = p2 + (static_cast<uint32_t>(static_cast<int32_t>(span) / -36) * 36u);
      if (static_cast<uint32_t>(static_cast<int32_t>(span) >= 1u)) {
        CopyBytes(ctx, p1, block, span);
      }
      StoreU32(ctx, self + 636u, ((count * 36u) + p0));
      StoreU32(ctx, self + 632u, (p2 + 36u));
      StoreU32(ctx, self + 628u, p1);
      p0 = !(block);
      if (p0 != 0u) { goto done; }
      DeallocateRawBlock(ctx, block);
      goto done;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    HostRet0(ctx);
    p0 = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, frame + 67u)) > -1);
    if (p0 != 0u) { goto unreachable_path; }
    p0 = LoadU32(ctx, frame + 56u);
    DeallocateRawBlock(ctx, p0);
    goto unreachable_path;
  }
  // Merge point trap_path.
trap_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  // Merge point unreachable_path.
unreachable_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = HostRet0(ctx);
    HostTempRet0(ctx);
    if (LoadS8(ctx, frame + 67u) <= -1) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 56u));
    }
    HostOsRelease(ctx, p2);
    TrapUnreachable();
  }
  // Merge point pending_throw_path.
pending_throw_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = FindMatchingCatch(ctx, 0u);
    HostTempRet0(ctx);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 164u) {
    Panic(ctx, p0);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 164u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  // Merge point alloc_assert_path.
alloc_assert_path:;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 165u) {
    RaiseAllocatorAssert(ctx, p0);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 165u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  // Merge point alloc_limit_path.
alloc_limit_path:;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 166u) {
    RaiseAllocLimitError(ctx, 744387u);
  #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
  #endif
    resume_point = 166u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  goto done;
  // Merge point suspend_point.
suspend_point:;
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
  result = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, result, p0);
  StoreU32(ctx, result + 4u, p1);
  StoreU32(ctx, result + 8u, p2);
  StoreU32(ctx, result + 12u, frame);
  StoreU32(ctx, result + 16u, tmp);
  StoreU32(ctx, result + 20u, slot);
  StoreU32(ctx, result + 24u, block);
  StoreU32(ctx, result + 28u, aux);
  StoreU32(ctx, result + 32u, self);
  StoreU32(ctx, result + 36u, tag);
  StoreU32(ctx, result + 40u, count);
  StoreU32(ctx, result + 44u, span);
  StoreU32(ctx, result + 48u, flag);
  StoreF32(ctx, result + 52u, num);
  StoreU32(ctx, result + 56u, word);
  StoreF64(ctx, result + 60u, dbl);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  // Merge point done.
done:;
  LeaveFrame();
}
}  // namespace Compositor
}  // namespace artc
