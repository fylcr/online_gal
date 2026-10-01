#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step drives; all three are
// provisional names for entities that are not rewritten yet.
void LuaPushCFunction(EngineContext* ctx, uint32_t state, uint32_t opener, uint32_t upvalues);
void LuaPushString(EngineContext* ctx, uint32_t state, uint32_t bytes);
void LuaCall(EngineContext* ctx, uint32_t state, uint32_t args, uint32_t results);

namespace LuaEngine {

// Opens the standard libraries one after another: it pushes the library opener,
// pushes the library name and calls the opener on that name.  Returns nothing;
// every one of the three steps per library can suspend.
void OpenLibs(EngineContext* ctx, uint32_t state) {
  EnterFrame();
  constexpr uint32_t kBaseOpener = 738u;
  constexpr uint32_t kBaseName = 0x2A8C1u;  // ""
  constexpr uint32_t kPackageOpener = 739u;
  constexpr uint32_t kPackageName = 0x2A8C2u;  // "package"
  constexpr uint32_t kTableOpener = 740u;
  constexpr uint32_t kTableName = 0x2A8CAu;  // "table"
  constexpr uint32_t kIoOpener = 741u;
  constexpr uint32_t kIoName = 0x2A8D0u;  // "io"
  constexpr uint32_t kOsOpener = 742u;
  constexpr uint32_t kOsName = 0x2A8D3u;  // "os"
  constexpr uint32_t kStringOpener = 743u;
  constexpr uint32_t kStringName = 0x2A8D6u;  // "string"
  constexpr uint32_t kMathOpener = 744u;
  constexpr uint32_t kMathName = 0x2A8DDu;  // "math"
  constexpr uint32_t kDebugOpener = 745u;
  constexpr uint32_t kDebugName = 0x2A8E2u;  // "debug"
  constexpr uint32_t kSaveFrameBytes = 4u;
  constexpr uint32_t kResumeWordBytes = 4u;
  // The call this frame suspended in; 0 on the running pass.
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    state = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed with that frame.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    LuaPushCFunction(ctx, state, kBaseOpener, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    LuaPushString(ctx, state, kBaseName);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
    LuaCall(ctx, state, 1u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
    LuaPushCFunction(ctx, state, kPackageOpener, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 3u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
    LuaPushString(ctx, state, kPackageName);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 4u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
    LuaCall(ctx, state, 1u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 5u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
    LuaPushCFunction(ctx, state, kTableOpener, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 6u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 7u) {
    LuaPushString(ctx, state, kTableName);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 7u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 8u) {
    LuaCall(ctx, state, 1u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 8u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 9u) {
    LuaPushCFunction(ctx, state, kIoOpener, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 9u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 10u) {
    LuaPushString(ctx, state, kIoName);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 10u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 11u) {
    LuaCall(ctx, state, 1u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 11u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 12u) {
    LuaPushCFunction(ctx, state, kOsOpener, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 12u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 13u) {
    LuaPushString(ctx, state, kOsName);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 13u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 14u) {
    LuaCall(ctx, state, 1u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 14u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 15u) {
    LuaPushCFunction(ctx, state, kStringOpener, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 15u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 16u) {
    LuaPushString(ctx, state, kStringName);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 16u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 17u) {
    LuaCall(ctx, state, 1u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 17u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 18u) {
    LuaPushCFunction(ctx, state, kMathOpener, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 18u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 19u) {
    LuaPushString(ctx, state, kMathName);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 19u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 20u) {
    LuaCall(ctx, state, 1u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 20u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 21u) {
    LuaPushCFunction(ctx, state, kDebugOpener, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 21u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 22u) {
    LuaPushString(ctx, state, kDebugName);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 22u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 23u) {
    LuaCall(ctx, state, 1u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 23u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  LeaveFrame();
  return;
suspend_point:;
  // Suspend: push the resume point, then the 4-byte frame holding the state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), state);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  LeaveFrame();
  return;
}

}  // namespace LuaEngine

}  // namespace artc
