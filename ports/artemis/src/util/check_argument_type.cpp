#include "util/engine_context.h"

namespace artc {

// Reads the type of one value and answers with it; the type is named by a small
// integer, with all-bits-set standing for "no type at all".
uint32_t QueryValueType(EngineContext* ctx, uint32_t a0, uint32_t a1);
// Writes one of the parser's diagnostic texts, returning early when the output
// room runs out.
uint32_t FlushOutputRoomIfFull(EngineContext* ctx, uint32_t state, uint32_t limit, uint32_t chunk);
// Reports one argument whose type did not match what the callee wanted.
uint32_t FormatArgumentError(EngineContext* ctx, uint32_t state, uint32_t index, uint32_t actual);

namespace {
// The two texts this body reports with, and the text that stands in for a type
// that is not there at all.
constexpr uint32_t kCheckArgumentTypeNoValueText = 0x29CF7u;  // no value
constexpr uint32_t kCheckArgumentTypeExpectedGotText = 0x29D60u;  // %s expected, got %s
// The table of type-name pointers, indexed by the type number; the type number
// with every bit set means "no type".
constexpr uint32_t kCheckArgumentTypeTypeNameTable = 0x2BD80u;
constexpr uint32_t kCheckArgumentTypeNoType = 0xFFFFFFFFu;
// The save frame this body pops on a rewind, and the resume word pushed with it.
constexpr uint32_t kCheckArgumentTypeSaveFrameBytes = 16u;
constexpr uint32_t kCheckArgumentTypeResumeWordBytes = 4u;
}  // namespace

// Checks that the value of `value` has the type `expected` says, and reports
// through the parser's own texts when it does not. The two texts are built into
// a sixteen-byte argument record on the guest stack, so that a text may name
// both the wanted and the found type.
void CheckArgumentType(EngineContext* ctx, uint32_t value, uint32_t expected, uint32_t index) {
  EnterFrame();
  // The base of the save frame, the stack pointer it comes off, and the value
  // found at that pointer.
  uint32_t frame = 0u;
  uint32_t sp = 0u;
  uint32_t top = 0u;
  // The address of the type-name table, and the name looked up out of it.
  uint32_t name = 0u;
  // Whether the two types disagreed, and the text that stands for the type that
  // was found.
  uint32_t mismatch = 0u;
  uint32_t actual_text = 0u;
  // The sixteen-byte argument record the texts are built from.
  uint32_t arg_record = 0u;
  // What the text builder answered with.
  uint32_t text = 0u;
  // The call a rewinding pass re-issues, and the word a suspending pass pushes.
  uint32_t resume_point = 0u;
  uint32_t resume_code = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the save frame and read its words back in the order they
    // were written.
    sp = ReturnFrameSp(ctx);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top) - kCheckArgumentTypeSaveFrameBytes;
    StoreU32(ctx, sp, top);
    sp = ReturnFrameSp(ctx);
    frame = LoadU32(ctx, sp);
    value = LoadU32(ctx, frame);
    index = LoadU32(ctx, frame + 8u);
    arg_record = LoadU32(ctx, frame + 12u);
    expected = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word pushed with that frame. It names the call
    // that was in flight, so only that call is issued again.
    sp = ReturnFrameSp(ctx);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top) - kCheckArgumentTypeResumeWordBytes;
    StoreU32(ctx, sp, top);
    sp = ReturnFrameSp(ctx);
    top = LoadU32(ctx, sp);
    resume_point = LoadU32(ctx, top);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The argument record lives on the guest stack, and the type query says
    // whether the wanted type and the value's own type disagree.
    arg_record = GuestStackSp(ctx) - 16u;
    GuestStackSp(ctx) = arg_record;
    mismatch = QueryValueType(ctx, value, expected) != index;
  }
  if (mismatch != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The text for the wanted type, then the one for the type actually found;
      // a type that is not there at all stands in as the text "no value".
      if (index == kCheckArgumentTypeNoType) {
        index = kCheckArgumentTypeNoValueText;
      } else {
        name = LoadU32(ctx, (index << (2u & 31)) + kCheckArgumentTypeTypeNameTable);
        index = name;
      }
      actual_text = kCheckArgumentTypeNoValueText;
      name = QueryValueType(ctx, value, expected);
      if (name != kCheckArgumentTypeNoType) {
        actual_text = LoadU32(ctx, (name << (2u & 31)) + kCheckArgumentTypeTypeNameTable);
      }
      StoreU32(ctx, arg_record + 4u, actual_text);
      StoreU32(ctx, arg_record, index);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      // The first text: "%s expected, got %s". Its answer becomes the index of
      // the second call.
      text = FlushOutputRoomIfFull(ctx, value, kCheckArgumentTypeExpectedGotText, arg_record);
      resume_code = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto save_frame;
      }
      index = text;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
      // The second text: the error that names the offending argument.
      FormatArgumentError(ctx, value, expected, index);
      resume_code = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto save_frame;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = arg_record + 16u;
  }
  LeaveFrame();
  return;
save_frame:;
  // Push the word naming the call in flight, then the save frame itself; the
  // words go out in the order a rewind reads them back.
  frame = ReturnFrameSp(ctx);
  top = LoadU32(ctx, frame);
  StoreU32(ctx, top, resume_code);
  sp = ReturnFrameSp(ctx);
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top) + kCheckArgumentTypeResumeWordBytes;
  StoreU32(ctx, sp, top);
  sp = ReturnFrameSp(ctx);
  frame = LoadU32(ctx, sp);
  StoreU32(ctx, frame, value);
  StoreU32(ctx, frame + 4u, expected);
  StoreU32(ctx, frame + 8u, index);
  StoreU32(ctx, frame + 12u, arg_record);
  sp = ReturnFrameSp(ctx);
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top) + kCheckArgumentTypeSaveFrameBytes;
  StoreU32(ctx, sp, top);
  LeaveFrame();
}

}  // namespace artc
