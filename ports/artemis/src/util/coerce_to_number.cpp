#include "util/engine_context.h"

namespace artc {

// Reads the numeric field of a string as a double.
double ReadNumericFieldAsDouble(EngineContext* ctx, uint32_t a0, uint32_t a1);
// Reads a packed integer out of a string.
uint32_t ReadPackedI64Value(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {
// The bytes a saved frame occupies.
constexpr uint32_t kCoerceToNumberSaveFrameBytes = 36u;
// The width of one resume word.
constexpr uint32_t kCoerceToNumberResumeWordBytes = 4u;
}  // namespace

// Turns a value into the number arithmetic needs. It reads the value either as
// a numeric string field or as a packed integer, stores the number back into
// the value, and answers whether the text it consumed made up a number.
uint32_t CoerceToNumber(EngineContext* ctx, uint32_t state, uint32_t value) {
  EnterFrame();
  uint32_t top = 0u;
  uint32_t arg = 0u;
  uint32_t third = 0u;
  // A word the frame bookkeeping carries: a resumed await code, a call result,
  // and finally the address the spill writes through.
  uint32_t carry = 0u;
  // The text being read, advancing as characters are consumed.
  uint32_t text = 0u;
  // The frame this step saves its state in.
  uint32_t frame = 0u;
  // The character under consideration, or a flag.
  uint32_t ch = 0u;
  // The guest stack pointer this frame owns.
  uint32_t stack = 0u;
  // The answer handed back to the caller.
  uint32_t result = 0u;
  // The double being read or stored.
  double dval = 0;
  // The value held across a suspend point.
  double pending = 0;
  // The double kept in the frame.
  double stored = 0;
  // The double written out to the value.
  double out = 0;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Pop the saved frame and restore the words it holds.
    top = ReturnFrameSp(ctx);
    arg = ReturnFrameSp(ctx);
    arg = LoadU32(ctx, arg);
    arg = arg - kCoerceToNumberSaveFrameBytes;
    StoreU32(ctx, top, arg);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top);
    frame = top;
    top = LoadU32(ctx, frame);
    state = top;
    value = LoadU32(ctx, frame + 4u);
    text = LoadU32(ctx, frame + 8u);
    ch = LoadU32(ctx, frame + 12u);
    stack = LoadU32(ctx, frame + 16u);
    result = LoadU32(ctx, frame + 20u);
    dval = LoadF64(ctx, frame + 24u);
    stored = dval;
    frame = LoadU32(ctx, frame + 32u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Drop the resume word this frame left behind.
    top = ReturnFrameSp(ctx);
    arg = ReturnFrameSp(ctx);
    arg = LoadU32(ctx, arg);
    arg = arg - kCoerceToNumberResumeWordBytes;
    StoreU32(ctx, top, arg);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top);
    carry = LoadU32(ctx, top);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Reserve a sixteen byte scratch area and point the text at it.
    top = GuestStackSp(ctx);
    top = top - 16u;
    stack = top;
    GuestStackSp(ctx) = top;
    text = stack + 12u;
  }
  if (carry == 0u || ResumePhase(ctx) == kResumeRunning) {
    arg = state;
    third = text;
    dval = ReadNumericFieldAsDouble(ctx, arg, third);
    pending = dval;
    top = 0u;
    arg = ResumePhase(ctx);
    arg = arg == kResumeSuspended;
    if (arg != 0u) {
      goto save_frame;
    }
    dval = pending;
    stored = dval;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Store the double back into the value and take the next character.
    out = stored;
    StoreF64(ctx, value, out);
    text = LoadU32(ctx, stack + 12u);
    ch = text == state;
  }
  if (ResumePhase(ctx)) {
    top = frame;
  } else {
    if (ch) {
      goto not_a_number;
    }
    ch = LoadU8(ctx, text);
    top = ch | 32u;
    top = top == 120u;
  }
  arg = ResumePhase(ctx);
  arg = arg == kResumeRewinding;
  top = top | arg;
  if (top) {
    // The text did not name a number, so read a packed integer instead.
    top = text;
    arg = stack + 12u;
    third = ResumePhase(ctx);
    top = third ? top : arg;
    text = top;
    if (carry == 1u || ResumePhase(ctx) == kResumeRunning) {
      arg = state;
      third = text;
      top = ReadPackedI64Value(ctx, arg, third, 16u);
      carry = top;
      top = 1u;
      arg = ResumePhase(ctx);
      arg = arg == kResumeSuspended;
      if (arg != 0u) {
        goto save_frame;
      }
      state = carry;
    }
    if (ResumePhase(ctx)) {
      top = ch;
    } else {
      out = (double)(state);
      StoreF64(ctx, value, out);
      text = LoadU32(ctx, stack + 12u);
      top = LoadU8(ctx, text);
    }
    ch = top;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (ch == 0u) {
      result = 1u;
      goto not_a_number;
    }
    // A number is made of digits, or of the letters of a number word.
    top = ch;
    arg = 32u;
    top = top == arg;
    arg = ch - 9u;
    third = 5u;
    arg = arg < third;
    top = top | arg;
    if (top == 0u) {
      goto not_a_number;
    }
    do {
      state = LoadU8(ctx, text + 1u);
      text = text + 1u;
      top = state;
      arg = 32u;
      top = top == arg;
      arg = state;
      third = 9u;
      arg = arg - third;
      third = 5u;
      arg = arg < third;
      top = top | arg;
    } while (top);
    result = state == 0u;
  }
not_a_number:;
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = stack + 16u;
    top = result;
    LeaveFrame();
    return top;
  }
  TrapUnreachable();
save_frame:;
  carry = top;
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top);
  StoreU32(ctx, top, carry);
  top = ReturnFrameSp(ctx);
  arg = ReturnFrameSp(ctx);
  arg = LoadU32(ctx, arg);
  arg = arg + kCoerceToNumberResumeWordBytes;
  StoreU32(ctx, top, arg);
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top);
  carry = top;
  arg = state;
  StoreU32(ctx, top, arg);
  StoreU32(ctx, carry + 4u, value);
  StoreU32(ctx, carry + 8u, text);
  StoreU32(ctx, carry + 12u, ch);
  StoreU32(ctx, carry + 16u, stack);
  StoreU32(ctx, carry + 20u, result);
  out = stored;
  StoreF64(ctx, carry + 24u, out);
  StoreU32(ctx, carry + 32u, frame);
  top = ReturnFrameSp(ctx);
  arg = ReturnFrameSp(ctx);
  arg = LoadU32(ctx, arg);
  arg = arg + kCoerceToNumberSaveFrameBytes;
  StoreU32(ctx, top, arg);
  top = 0u;
  LeaveFrame();
  return top;
}

}  // namespace artc
