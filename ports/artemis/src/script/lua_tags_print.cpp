#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step drives; these names are
// provisional until the entities behind them are rewritten.
uint32_t LuaTagReadElement(EngineContext* ctx, uint32_t container, uint32_t kind, uint32_t index);
void LuaTagAppendBytes(EngineContext* ctx, uint32_t text, uint32_t source, uint32_t length);
void LuaTagAppendValue(EngineContext* ctx, uint32_t container, uint32_t value, uint32_t kind);
void LuaTagPushValue(EngineContext* ctx, uint32_t container, uint32_t value);
uint32_t LuaTagLookupField(EngineContext* ctx, uint32_t object, uint32_t name);

namespace {

// The `px != 0` null check of the shared-ownership pointer this step dereferences.
constexpr uint32_t kTagPrintAssertCondition = 0xB5E31u;  // "px != 0"
constexpr uint32_t kTagPrintAssertFile = 0xB5E39u;  // the boost shared_ptr.hpp path
constexpr uint32_t kTagPrintAssertName = 0xB5E71u;  // "operator->"
// The pending-exception word the assert calls below report through.
constexpr uint32_t kTagPrintThrowLatch = 0xD015Cu;
// The field the step looks up on the text run.
constexpr uint32_t kTagPrintFieldName = 0xAF58Fu;  // "print"
// The empty name handed to the invoked slot.
constexpr uint32_t kTagPrintEmptyName = 0xB5E30u;
// The tag stored in the header the step builds on its scratch frame.
constexpr uint32_t kTagPrintDataTag = 0x61746164u;  // "data"
// The marker of a text run that has no owner.
constexpr uint32_t kTagPrintNoOwner = 0xFFFFFFFFu;
// The host slot the step invokes.
constexpr uint32_t kTagPrintInvokeSlot = 1959u;
// The guest frame the step builds, and the frame it saves while suspended.
constexpr uint32_t kTagPrintScratchBytes = 48u;
constexpr uint32_t kTagPrintSaveFrameBytes = 24u;
constexpr uint32_t kTagPrintResumeWordBytes = 4u;

}  // namespace

namespace LuaEngine {

// Prints one `print` tag: on a 48-byte guest scratch frame it appends the tag's four-byte group
// to a local text run when the tag carries no owner, appends that run and the tag's tail to the
// text object, and finally looks the `print` field up on it. Every call that can run guest code
// is a suspend gate: a rewinding pass restores the frame slots below and replays only the call
// its resume point names. `a` is the tag -- and the temporary the flattened driver keeps in that
// slot -- and `b` the text object.
void TagPrint(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  // The call this frame suspended in; 0 on the running pass.
  uint32_t resume_point = 0u;
  uint32_t frame = 0u;
  uint32_t result = 0u;
  uint32_t text = 0u;
  uint32_t slot = 0u;
  uint32_t scratch = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 24-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kTagPrintSaveFrameBytes);
    result = LoadU32(ctx, ReturnFrameSp(ctx));
    a = LoadU32(ctx, result);
    frame = LoadU32(ctx, result + 8u);
    text = LoadU32(ctx, result + 12u);
    slot = LoadU32(ctx, result + 16u);
    scratch = LoadU32(ctx, result + 20u);
    b = LoadU32(ctx, result + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed with that frame.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kTagPrintResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The scratch frame this step keeps its temporaries in.
    frame = GuestStackSp(ctx) - kTagPrintScratchBytes;
    GuestStackSp(ctx) = frame;
    text = static_cast<uint32_t>(!LoadU8(ctx, b + 12u));
  }
  if (text != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      result = LuaTagReadElement(ctx, b, 2u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      text = result;
    }
    // A running pass that already holds a text run skips the copy below.
    if (ResumePhase(ctx) == kResumeRunning && text != 0u) {
      goto owner_ready;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    text = LoadU32(ctx, b + 4u);
    slot = static_cast<uint32_t>(!text);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kTagPrintThrowLatch, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
      HostFailAssertion(ctx, 857u, kTagPrintAssertCondition, kTagPrintAssertFile, 734u,
                        kTagPrintAssertName);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      a = LoadU32(ctx, kTagPrintThrowLatch);
      StoreU32(ctx, kTagPrintThrowLatch, 0u);
      a = static_cast<uint32_t>(a != 1u);
      if (a != 0u) {
        goto unreachable_trap;
      }
      goto throw_unwind;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The tag has no text run of its own: its four-byte group goes into the local one.
    slot = a + 40u;
    LuaTagAppendBytes(ctx, text, slot, 4u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
  }
owner_ready:;
  // A suspended pass keeps the text run it restored; a running one takes the tag's own.
  text = ResumePhase(ctx) != kResumeRunning ? text : a + 4u;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
    LuaTagAppendValue(ctx, b, text, 3u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  a = ResumePhase(ctx) != kResumeRunning ? a : a + 16u;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
    LuaTagPushValue(ctx, b, a);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 3u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU8(ctx, b + 12u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (slot != 0u) {
      goto print_done;
    }
    // No tail text: the local run keeps the no-owner marker in its header.
    StoreU32(ctx, frame + 12u, kTagPrintNoOwner);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
    result = LuaTagReadElement(ctx, b, 1u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 4u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    slot = result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (slot != 0u) {
      goto print_done;
    }
    b = LoadU32(ctx, b + 4u);
    slot = static_cast<uint32_t>(!b);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kTagPrintThrowLatch, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
      HostFailAssertion(ctx, 857u, kTagPrintAssertCondition, kTagPrintAssertFile, 734u,
                        kTagPrintAssertName);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 5u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      a = LoadU32(ctx, kTagPrintThrowLatch);
      StoreU32(ctx, kTagPrintThrowLatch, 0u);
      a = static_cast<uint32_t>(a != 1u);
      if (a != 0u) {
        goto unreachable_trap;
      }
      goto throw_unwind;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The header of the run this step hands to the invoked slot.
    LuaTagAppendBytes(ctx, b, frame + 12u, 4u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    if (LoadU32(ctx, frame + 12u) != 0u) {
      goto print_done;
    }
    StoreU8(ctx, frame + 4u, 0u);
    StoreU32(ctx, frame, kTagPrintDataTag);
    StoreU8(ctx, frame + 11u, 4u);
    StoreU32(ctx, kTagPrintThrowLatch, 0u);
    StoreU32(ctx, frame + 24u, frame);
    slot = frame + 24u;
    scratch = frame + 16u;
    b = frame + 32u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
    HostInvokeViiiiii(ctx, kTagPrintInvokeSlot, b, a, frame, kTagPrintEmptyName, slot, scratch);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 6u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    a = LoadU32(ctx, kTagPrintThrowLatch);
    StoreU32(ctx, kTagPrintThrowLatch, 0u);
    a = static_cast<uint32_t>(a == 1u);
    if (a != 0u) {
      goto unwind_cleanup;
    }
    // Move the invoked slot's header onto the text run and swap the two over.
    a = LoadU32(ctx, frame + 32u);
    StoreU32(ctx, frame + 40u, LoadU32(ctx, a + 36u));
    StoreU64(ctx, frame + 32u, LoadU64(ctx, a + 28u));
    StoreU32(ctx, a + 36u, LoadU32(ctx, text + 8u));
    StoreU64(ctx, a + 28u, LoadU64(ctx, text));
    StoreU32(ctx, text + 8u, LoadU32(ctx, frame + 40u));
    StoreU64(ctx, text, LoadU64(ctx, frame + 32u));
    if (static_cast<int32_t>(LoadS8(ctx, frame + 11u)) <= static_cast<int32_t>(kTagPrintNoOwner)) {
      a = LoadU32(ctx, frame);
      DeallocateRawBlock(ctx, a);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 7u) {
    result = LuaTagLookupField(ctx, text, kTagPrintFieldName);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 7u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    a = result;
  }
print_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = frame + kTagPrintScratchBytes;
    LeaveFrame();
    return;
  }
unwind_cleanup:;
  if (ResumePhase(ctx) == kResumeRunning) {
    a = HostRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    HostTempRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    if (static_cast<int32_t>(LoadS8(ctx, frame + 11u)) <= static_cast<int32_t>(kTagPrintNoOwner)) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame));
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
    }
    HostOsRelease(ctx, a);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
  }
unreachable_trap:;
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
throw_unwind:;
  if (ResumePhase(ctx) == kResumeRunning) {
    a = FindMatchingCatch(ctx, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    HostTempRet0(ctx);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 8u) {
    Panic(ctx, a);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 8u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  LeaveFrame();
  return;
suspend_point:;
  // Suspend: push the resume point, then the 24-byte frame holding the slots above.
  result = resume_point;
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), result);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kTagPrintResumeWordBytes);
  result = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, result, a);
  StoreU32(ctx, result + 4u, b);
  StoreU32(ctx, result + 8u, frame);
  StoreU32(ctx, result + 12u, text);
  StoreU32(ctx, result + 16u, slot);
  StoreU32(ctx, result + 20u, scratch);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kTagPrintSaveFrameBytes);
  LeaveFrame();
  return;
}

}  // namespace LuaEngine
}  // namespace artc
