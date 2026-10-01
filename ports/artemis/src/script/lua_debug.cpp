#include "util/engine_context.h"

namespace artc {

// Forward declaration — the tag registration helper, awaiting its thin
// call-through in util/engine_context.h.
void RegisterTagLibrary(EngineContext* ctx, uint32_t state, uint32_t name, uint32_t functions);

namespace {

constexpr uint32_t kLuaDebugTagText = 0x2A3DAu;  // "debug"
constexpr uint32_t kLuaDebugTagTable = 0x2A3E0u;  // the table registered under the tag
constexpr uint32_t kLuaDebugWordBytes = 4u;       // one spill slot: the resume code, then the state

}  // namespace

namespace LuaDebug {

// Registers the "debug" tag with the entry table behind it.  Suspendable: it
// answers 1 once the registration has gone through, and 0 when the call
// underneath suspended on its way out.
uint32_t Init(EngineContext* ctx, uint32_t state) {
  EnterFrame();
  uint32_t resume_point = 0u;
  uint32_t result = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame holding the state pointer.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kLuaDebugWordBytes);
    state = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume code that was pushed with that frame.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kLuaDebugWordBytes);
    result = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  } else {
    // A running pass has nothing to select: the saved word is still zero.
    result = resume_point;
  }
  // A running pass always registers; a rewinding one only at resume code 0.
  if (ResumePhase(ctx) == kResumeRunning || result == 0u) {
    RegisterTagLibrary(ctx, state, kLuaDebugTagText, kLuaDebugTagTable);
    result = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    LeaveFrame();
    return 1u;
  }
  TrapUnreachable();
suspend_point:
  // Suspend: write the resume code, then push the frame with the live state.
  resume_point = result;
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kLuaDebugWordBytes);
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), state);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kLuaDebugWordBytes);
  result = 0u;
  LeaveFrame();
  return result;
}

}  // namespace LuaDebug

}  // namespace artc
