#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
void CompileOperation(EngineContext* ctx, uint32_t state, uint32_t expression);
void ThrowScriptError(EngineContext* ctx, uint32_t a0, uint32_t a1);
void RaiseScriptError(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
void EnsureTableArrayRoom(EngineContext* ctx, uint32_t a);
uint32_t Call(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

namespace {

constexpr uint32_t kCallValueStackOverflowMessage = 0x2A841u;  // "C stack overflow"
constexpr uint32_t kCallValueDepthCap = 200u;  // deepest call window that is accepted
constexpr uint32_t kCallValueDepthLimit = 225u;  // past this the step reports an overflow
constexpr uint32_t kCallValueSaveFrameBytes = 16u;  // saved state words below the resume code
constexpr uint32_t kCallValueResumeWordBytes = 4u;  // resume code slot

}  // namespace

// Calls a value: the callable's depth slot is raised, the argument window is
// prepared and the call is entered.  The call can suspend, so the arguments and
// the depth the step works with are kept in the frame.
void CallValue(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t depth = 0u;        // the depth slot this call takes
  uint32_t depth16 = 0u;      // that same value wrapped to 16 bits
  uint32_t depth_ok = 0u;     // whether the depth is still under the cap
  uint32_t overflow = 0u;     // whether the depth error has to be raised
  uint32_t result = 0u;       // what the call hands back
  uint32_t room_needed = 0u;  // whether the table array has to grow
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 16-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kCallValueSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    a = LoadU32(ctx, frame);
    b = LoadU32(ctx, frame + 4u);
    c = LoadU32(ctx, frame + 8u);
    depth = LoadU32(ctx, frame + 12u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame that saves the resume code.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kCallValueResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Take the depth slot for this call; the counter is 16 bits wide.
    depth = LoadU16(ctx, a + 52u) + 1u;
    StoreU16(ctx, a + 52u, depth);
    depth16 = depth & 65535u;
    depth_ok = depth16 < kCallValueDepthCap;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (depth_ok != 0u) { goto call_step; }
    // Rewinding always replays the error check below.
    overflow = depth16 == kCallValueDepthCap;
  }
  if (overflow != 0u || ResumePhase(ctx) == kResumeRewinding) {
    // The window is full, so the call is refused unless it is being replayed.
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      RaiseScriptError(ctx, a, kCallValueStackOverflowMessage, 0u);
      resume_point = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto call_step; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if ((depth & 65535u) >= kCallValueDepthLimit) { goto overflow_path; }
  }
call_step:
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    result = LuaEngine::Call(ctx, a, b, c);
    resume_point = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    b = result;
  }
  b = ResumePhase(ctx) != kResumeRunning ? b : b == 0u;
  if (b != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
      CompileOperation(ctx, a, 1u);
      resume_point = 2u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Give the depth slot back and see whether the table array has room.
    StoreU16(ctx, a + 52u, LoadU16(ctx, a + 52u) - 1u);
    b = LoadU32(ctx, a + 16u);
    c = LoadU32(ctx, b + 68u);
    room_needed = c >= LoadU32(ctx, b + 64u);
  }
  if (room_needed != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
      EnsureTableArrayRoom(ctx, a);
      resume_point = 3u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) { goto exit_path; }
overflow_path:
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
    ThrowScriptError(ctx, a, 5u);
    resume_point = 4u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) { TrapUnreachable(); }
  goto exit_path;
suspend_point:
  // Suspend: write the resume code, then push the frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kCallValueResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, a);
    StoreU32(ctx, frame + 4u, b);
    StoreU32(ctx, frame + 8u, c);
    StoreU32(ctx, frame + 12u, depth);
  }
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kCallValueSaveFrameBytes);
exit_path:
  LeaveFrame();
}

}  // namespace artc
