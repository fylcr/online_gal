#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step drives.
uint32_t ResolveExprValue(EngineContext* ctx, uint32_t state, uint32_t tag);
void InternKeyIntoTableSlot(EngineContext* ctx, uint32_t state, uint32_t bytes, uint32_t length);
void LuaCheckAny(EngineContext* ctx, uint32_t state, uint32_t index);
void LuaGetField(EngineContext* ctx, uint32_t state, uint32_t index, uint32_t name);
uint32_t LuaGetMetatable(EngineContext* ctx, uint32_t state, uint32_t index);
uint32_t LuaRawEqual(EngineContext* ctx, uint32_t state, uint32_t index_a, uint32_t index_b);

namespace LuaStdlib {
// LuaStdlib::InitIoType -- the io.type helper: reads the value at stack index 1
// as a file handle and pushes "file", "closed file", or nil for the caller.
uint32_t InitIoType(EngineContext* ctx, uint32_t state) {
  EnterFrame();
  constexpr uint32_t kFileHandleTypeName = 174442u;   // "FILE*"
  constexpr uint32_t kClosedFileText = 174943u;       // "closed file"
  constexpr uint32_t kOpenFileText = 174955u;         // "file"
  constexpr uint32_t kRegistryIndex = 4294957296u;  // -10000, the Lua registry pseudo-index
  constexpr uint32_t kBelowTopIndex = 4294967294u;    // -2
  constexpr uint32_t kTopIndex = 4294967295u;         // -1
  constexpr uint32_t kIoTypeSaveBytes = 8u;           // saved state words below the resume code
  constexpr uint32_t kIoTypeResumeBytes = 4u;
  uint32_t value = 0u;
  uint32_t resume_point = 0u;
  uint32_t frame = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the locals this step saved when it suspended.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kIoTypeSaveBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    value = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word naming the call this step stopped at.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kIoTypeResumeBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    LuaCheckAny(ctx, state, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The handle is the userdata payload, or zero when index 1 is no userdata.
    value = ResolveExprValue(ctx, state, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    // Fetch the metatable the file handles are expected to carry.
    LuaGetField(ctx, state, kRegistryIndex, kFileHandleTypeName);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (value == 0u) { goto not_a_file; }
    if (LuaGetMetatable(ctx, state, 1u) == 0u) { goto not_a_file; }
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    if (LuaRawEqual(ctx, state, kBelowTopIndex, kTopIndex) != 0u) {
      goto file_metatable_matched;
    }
  // Merge point not_a_file: the argument is not a file handle, so nil is pushed
  // in its place and one result is reported.
  not_a_file:;
    {
      const uint32_t slot = LoadU32(ctx, state + 8u);
      StoreU32(ctx, slot + 8u, 0u);
      StoreU32(ctx, state + 8u, slot + 16u);
    }
    LeaveFrame();
    return 1u;
  // Merge point file_metatable_matched: the handle reports its own state.
  file_metatable_matched:;
    value = (LoadU32(ctx, value) == 0u);
  }
  if (value != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
      InternKeyIntoTableSlot(ctx, state, kClosedFileText, 11u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_point = 2u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      LeaveFrame();
      return 1u;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
    InternKeyIntoTableSlot(ctx, state, kOpenFileText, 4u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 3u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    LeaveFrame();
    return 1u;
  }
  TrapUnreachable();
suspend_point:;
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kIoTypeResumeBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, value);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kIoTypeSaveBytes);
  LeaveFrame();
  return 0u;
}

}  // namespace LuaStdlib
}  // namespace artc
