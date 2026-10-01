#include "util/engine_context.h"

namespace artc {

// Pushes a formatted string built from the text given and the arguments it
// names.
uint32_t FormatPushString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

// The table of token texts, four bytes an entry.
constexpr uint32_t kFormatCharValueTokenTable = 0x2A82Cu;

// The two texts a character value is rendered with: 'char(%d)' for a code that
// cannot be printed, and the text after it for one that can.
constexpr uint32_t kFormatCharValueCharText = 0x2ACB0u;
constexpr uint32_t kFormatCharValueText = 0x2ACB9u;

// The code the two texts divide at, and the two codes that are not printable
// however small they are.
constexpr uint32_t kFormatCharValueMaxChar = 256u;
constexpr uint32_t kFormatCharValueDeleteCode = 127u;
constexpr uint32_t kFormatCharValueFirstPrintableCode = 32u;

// The argument record this step reserves on the guest stack, and the slot its
// first argument goes in.
constexpr uint32_t kFormatCharValueRecordBytes = 32u;
constexpr uint32_t kFormatCharValueValueOffset = 16u;

// The suspend frame: the words this body saves, and the resume word pushed with
// them, whose number names the call that was in flight.
constexpr uint32_t kFormatCharValueSaveFrameBytes = 12u;
constexpr uint32_t kFormatCharValueResumeWordBytes = 4u;
constexpr uint32_t kFormatCharValueCharCall = 0u;
constexpr uint32_t kFormatCharValueTextCall = 1u;

}  // namespace

// Renders one character value as text and answers the string it was rendered
// into. A code past the character range is answered from the token table
// instead, and a code that cannot be printed is rendered as 'char(%d)'.
uint32_t FormatCharValue(EngineContext* ctx, uint32_t state, uint32_t value) {
  EnterFrame();
  // The suspend frame base: a rewind pops it and the suspend path writes its
  // three words into it.
  uint32_t frame = 0u;
  // The argument record built on the guest stack for the text.
  uint32_t record = 0u;
  // Whether the value is rendered as a character at all, and then which of the
  // two texts is the one used.
  uint32_t guard = 0u;
  // The call a rewinding pass re-issues, and the number a spilling pass saves.
  uint32_t resume_point = 0u;
  uint32_t resume_code = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 12-byte frame and read its words back in the saved
    // order.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kFormatCharValueSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    value = LoadU32(ctx, frame + 4u);
    record = LoadU32(ctx, frame + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word pushed with that frame. It names the call
    // that was in flight, so only that call is issued again.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kFormatCharValueResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The argument record lives on the guest stack, so it lasts exactly as long
    // as the call does.
    GuestStackSp(ctx) -= kFormatCharValueRecordBytes;
    record = GuestStackSp(ctx);
    guard = static_cast<uint32_t>(static_cast<int32_t>(value) <= static_cast<int32_t>(kFormatCharValueMaxChar));
  }
  guard |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (guard) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // A code that cannot be printed is named by its own number; a code that
      // can is rendered against the text the parser holds for the rest.
      guard = static_cast<uint32_t>(value == kFormatCharValueDeleteCode ||
                                    value < kFormatCharValueFirstPrintableCode);
      state = LoadU32(ctx, state + 52u);
    }
    guard |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
    if (guard) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, record + kFormatCharValueValueOffset, value);
        value = record + kFormatCharValueValueOffset;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == kFormatCharValueCharCall) {
        uint32_t const answer = FormatPushString(ctx, state, kFormatCharValueCharText, value);
        resume_code = kFormatCharValueCharCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        state = answer;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        goto rendered;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, record, value);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == kFormatCharValueTextCall) {
      uint32_t const answer = FormatPushString(ctx, state, kFormatCharValueText, record);
      resume_code = kFormatCharValueTextCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      state = answer;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      goto rendered;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A code past the character range is answered by the token table.
    state = LoadU32(ctx, kFormatCharValueTokenTable + (value << 2));
  }
rendered:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The record reserved on the guest stack goes back, and the answer is the
    // string the call rendered.
    GuestStackSp(ctx) = record + kFormatCharValueRecordBytes;
    LeaveFrame();
    return state;
  }
  TrapUnreachable();
suspend_point:;
  // Push the resume word naming the call that was in flight, then the frame the
  // rewind pops; the words are saved in the order that pop reads them.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kFormatCharValueResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, value);
  StoreU32(ctx, frame + 8u, record);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kFormatCharValueSaveFrameBytes);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
