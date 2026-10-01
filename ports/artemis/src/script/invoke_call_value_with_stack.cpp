#include "util/engine_context.h"

namespace artc {

// Defined elsewhere in the tree.
void ThrowScriptError(EngineContext* ctx, uint32_t state, uint32_t code);
void GrowValueStack(EngineContext* ctx, uint32_t state, uint32_t bytes);
void CallValue(EngineContext* ctx, uint32_t state, uint32_t value, uint32_t argument);

namespace {

constexpr uint32_t kInvokeCallValueWithStackSaveFrameBytes = 12u;  // state, window, verdict
constexpr uint32_t kInvokeCallValueWithStackResumeWordBytes = 4u;
constexpr uint32_t kInvokeCallValueWithStackTypeErrorCall = 0u;  // the window type error raise was in flight
constexpr uint32_t kInvokeCallValueWithStackGrowCall = 1u;  // the value stack growth was in flight
constexpr uint32_t kInvokeCallValueWithStackInvokeCall = 2u;  // the call itself was in flight
constexpr uint32_t kInvokeCallValueWithStackCallErrorCall = 3u;  // the call error raise was in flight

}  // namespace

// Invokes the callable whose value sits on the value stack: the call's window is
// located off the state, its contents are laid out for the call, the value stack
// is grown when the window does not fit, and the call is entered. A window whose
// tag is not the callable one raises the call error first, and a call that has
// been entered raises it again. The step suspends around its four calls; each
// gate replays the one its resume code names.
void InvokeCallValueWithStack(EngineContext* ctx, uint32_t state) {
  EnterFrame();
  uint32_t frame = 0u;
  uint32_t window = 0u;
  uint32_t verdict = 0u;
  uint32_t need_grow = 0u;
  uint32_t source = 0u;
  uint32_t resume_code = 0u;
  uint32_t resume_word = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kInvokeCallValueWithStackSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    window = LoadU32(ctx, frame + 4u);
    verdict = LoadU32(ctx, frame + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kInvokeCallValueWithStackResumeWordBytes);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    window = LoadU32(ctx, state + 116u);
  }
  if (window != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The window the call runs in, and whether its tag is the callable one.
      window = window + LoadU32(ctx, state + 32u);
      verdict = static_cast<uint32_t>(LoadU32(ctx, window + 8u) != 6u);
    }
    if (verdict != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning || resume_word == kInvokeCallValueWithStackTypeErrorCall) {
        ThrowScriptError(ctx, state, 5u);
        resume_code = kInvokeCallValueWithStackTypeErrorCall;
        if (ResumePhase(ctx) == kResumeSuspended) {goto suspend_point;}
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        // Lay the window's value and its tag out on the value stack, then find
        // out whether the stack still has room for the frame the call builds.
        verdict = LoadU32(ctx, state + 8u);
        source = verdict - 16u;
        StoreU64(ctx, verdict, LoadU64(ctx, source));
        StoreU32(ctx, verdict + 8u, LoadU32(ctx, source + 8u));
        verdict = LoadU32(ctx, state + 8u) - 16u;
        StoreU64(ctx, verdict, LoadU64(ctx, window));
        StoreU32(ctx, verdict + 8u, LoadU32(ctx, window + 8u));
        uint32_t const stack_limit = LoadU32(ctx, state + 28u);
        window = LoadU32(ctx, state + 8u);
        need_grow = static_cast<uint32_t>((int32_t)(stack_limit - window) <= (int32_t)16u);
      } else {
        // Nothing to lay out on the way back; the frame's own verdict answers.
        need_grow = verdict;
      }
      if (need_grow != 0u || ResumePhase(ctx) == kResumeRewinding) {
        if (ResumePhase(ctx) == kResumeRunning || resume_word == kInvokeCallValueWithStackGrowCall) {
          GrowValueStack(ctx, state, 1u);
          resume_code = kInvokeCallValueWithStackGrowCall;
          if (ResumePhase(ctx) == kResumeSuspended) {goto suspend_point;}
        }
        if (ResumePhase(ctx) == kResumeRunning) {
          window = LoadU32(ctx, state + 8u);
        }
        if (ResumePhase(ctx) == kResumeRunning) {
          StoreU32(ctx, state + 8u, window + 16u);
          window = window - 16u;
        }
        if (ResumePhase(ctx) == kResumeRunning || resume_word == kInvokeCallValueWithStackInvokeCall) {
          CallValue(ctx, state, window, 1u);
          resume_code = kInvokeCallValueWithStackInvokeCall;
          if (ResumePhase(ctx) == kResumeSuspended) {goto suspend_point;}
        }
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_word == kInvokeCallValueWithStackCallErrorCall) {
    ThrowScriptError(ctx, state, 2u);
    resume_code = kInvokeCallValueWithStackCallErrorCall;
    if (ResumePhase(ctx) == kResumeSuspended) {goto suspend_point;}
  }
  // The running pass is done; the frame write below belongs to a suspend only.
  goto exit_path;
  // Suspend here: the resume word naming the call in flight goes first, then the
  // three saved words, and the caller's frame pointer is left above both.
  suspend_point:;
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kInvokeCallValueWithStackResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, window);
  StoreU32(ctx, frame + 8u, verdict);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kInvokeCallValueWithStackSaveFrameBytes);
  exit_path:;
  LeaveFrame();
}
}  // namespace artc
