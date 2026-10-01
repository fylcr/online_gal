#include "util/engine_context.h"

namespace artc {

// Cross-TU callees: the string compare, the frame reader and the two traceback
// steps the message paths below call.
uint32_t CompareNulTerminatedStrings(EngineContext* ctx, uint32_t left, uint32_t right);
uint32_t RaiseTraceback(EngineContext* ctx, uint32_t value, uint32_t message, uint32_t tag);
uint32_t TrimLocalSlotsAboveLevel(EngineContext* ctx, uint32_t proto, uint32_t level,
                                  uint32_t result);
uint32_t FormatTracebackFrame(EngineContext* ctx, uint32_t state, uint32_t name, uint32_t out);

namespace {

constexpr uint32_t kFormatArgumentErrorMsgBadArgumentDS = 0x29D03u;  // "bad argument #%d (%s)"
constexpr uint32_t kFormatArgumentErrorConst29D19 = 0x29D19u;  // name word for the traceback frame
constexpr uint32_t kFormatArgumentErrorMsgMethod = 0x29D1Bu;  // "method"
constexpr uint32_t kFormatArgumentErrorMsgCallingSOnBad = 0x29D22u;  // "calling '%s' on bad self"
constexpr uint32_t kFormatArgumentErrorConst29D40 = 0x29D40u;  // fallback name word
constexpr uint32_t kFormatArgumentErrorMsgBadArgumentDTo = 0x29D42u;  // "bad argument #%d to '%s'"
constexpr uint32_t kFormatArgumentErrorSaveFrameBytes = 20u;
constexpr uint32_t kFormatArgumentErrorResumeWordBytes = 4u;

}  // namespace

// Formats the bad-argument error for one call: it reads the calling function's
// name and name word out of the frame, picks one of three message forms and
// raises it through the traceback path. Answers the error state word.
uint32_t FormatArgumentError(EngineContext* ctx, uint32_t state, uint32_t index, uint32_t actual) {
  EnterFrame();
  uint32_t frame = 0u;
  uint32_t slot = 0u;
  uint32_t resume_code = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the saved frame and read its words back in the saved order.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t frame_top = ReturnFrameSp(ctx);
    frame_top = LoadU32(ctx, frame_top) - kFormatArgumentErrorSaveFrameBytes;
    StoreU32(ctx, sp, frame_top);
    uint32_t base = ReturnFrameSp(ctx);
    base = LoadU32(ctx, base);
    state = LoadU32(ctx, base);
    actual = LoadU32(ctx, base + 8u);
    frame = LoadU32(ctx, base + 12u);
    slot = LoadU32(ctx, base + 16u);
    index = LoadU32(ctx, base + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word that names the call that was in flight.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t word_top = ReturnFrameSp(ctx);
    word_top = LoadU32(ctx, word_top) - kFormatArgumentErrorResumeWordBytes;
    StoreU32(ctx, sp, word_top);
    uint32_t word = ReturnFrameSp(ctx);
    word = LoadU32(ctx, word);
    resume_code = LoadU32(ctx, word);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A running pass takes its scratch frame from the guest stack.
    uint32_t sp = GuestStackSp(ctx);
    frame = sp - 160u;
    GuestStackSp(ctx) = frame;
    uint32_t trimmed = TrimLocalSlotsAboveLevel(ctx, state, 0u, frame + 56u);
    slot = trimmed == 0u;
  }
  uint32_t phase = ResumePhase(ctx);
  if (slot != 0u || phase == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // Put the caller's argument words back into the frame.
      StoreU32(ctx, frame + 4u, actual);
      StoreU32(ctx, frame, index);
    }
    phase = ResumePhase(ctx);
    if (phase == kResumeRunning || resume_code == 0u) {
      uint32_t raised = RaiseTraceback(ctx, state, kFormatArgumentErrorMsgBadArgumentDS, frame);
      resume_code = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_here;
      }
      state = raised;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      goto exit_path;
    }
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    slot = frame + 56u;
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_code == 1u) {
    uint32_t formatted = FormatTracebackFrame(ctx, state, kFormatArgumentErrorConst29D19, slot);
    resume_code = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
    slot = formatted;
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    uint32_t name_word = LoadU32(ctx, frame + 64u);
    slot = CompareNulTerminatedStrings(ctx, name_word, kFormatArgumentErrorMsgMethod);
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    // A method name word with a decremented argument of zero is the bad-self case.
    if (slot != 0u) {
      goto named_argument;
    }
    index = index - 1u;
    if (index != 0u) {
      goto named_argument;
    }
    uint32_t name = LoadU32(ctx, frame + 60u);
    StoreU32(ctx, frame + 16u, name);
    StoreU32(ctx, frame + 20u, actual);
    index = frame + 16u;
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_code == 2u) {
    uint32_t raised = RaiseTraceback(ctx, state, kFormatArgumentErrorMsgCallingSOnBad, index);
    resume_code = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
    state = raised;
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    goto exit_path;
  }
named_argument:;
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    slot = LoadU32(ctx, frame + 60u);
    if (slot == 0u) {
      // No name in the frame: fall back to the placeholder name word.
      uint32_t fallback = kFormatArgumentErrorConst29D40;
      StoreU32(ctx, frame + 60u, fallback);
      slot = kFormatArgumentErrorConst29D40;
    }
    StoreU32(ctx, frame + 40u, actual);
    StoreU32(ctx, frame + 36u, slot);
    StoreU32(ctx, frame + 32u, index);
    index = frame + 32u;
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_code == 3u) {
    uint32_t raised = RaiseTraceback(ctx, state, kFormatArgumentErrorMsgBadArgumentDTo, index);
    resume_code = 3u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
    state = raised;
  }
exit_path:;
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    // The running pass hands its scratch frame back to the guest stack.
    GuestStackSp(ctx) = frame + 160u;
    LeaveFrame();
    return state;
  }
  TrapUnreachable();
suspend_here:;
  // Push the resume word naming the call that was in flight, then the frame the
  // rewind pops; the two words go in the order the pop above reads them back.
  uint32_t word_slot = ReturnFrameSp(ctx);
  word_slot = LoadU32(ctx, word_slot);
  StoreU32(ctx, word_slot, resume_code);
  uint32_t sp = ReturnFrameSp(ctx);
  uint32_t word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + kFormatArgumentErrorResumeWordBytes;
  StoreU32(ctx, sp, word_top);
  uint32_t base = ReturnFrameSp(ctx);
  base = LoadU32(ctx, base);
  StoreU32(ctx, base, state);
  StoreU32(ctx, base + 4u, index);
  StoreU32(ctx, base + 8u, actual);
  StoreU32(ctx, base + 12u, frame);
  StoreU32(ctx, base + 16u, slot);
  sp = ReturnFrameSp(ctx);
  word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + kFormatArgumentErrorSaveFrameBytes;
  StoreU32(ctx, sp, word_top);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
