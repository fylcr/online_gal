#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step drives.
uint32_t CopyBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t StringLength(EngineContext* ctx, uint32_t text);
uint32_t InternString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void GrowValueStack(EngineContext* ctx, uint32_t a, uint32_t b);
void RaiseScriptError(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t FetchObjectMetamethod(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
void CallValue(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
void FormatConcatenationError(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t MeasureFormattedNumber(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t ReallocateFormatBuffer(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

namespace {

constexpr uint32_t kFormatNumberValueFormat = 0x2BED9u;          // "%.14g", the number format
constexpr uint32_t kFormatNumberValueOverflowMessage = 0x2BF07u; // "string length overflow"
constexpr uint32_t kFormatNumberValueLengthLimit = 0xFFFFFFFDu;  // largest usable string length
constexpr uint32_t kFormatNumberValueFrameBytes = 44u;           // saved frame: eleven words
constexpr uint32_t kFormatNumberValueResumeWordBytes = 4u;       // resume word written before it

}  // namespace

// Turns the run of `value` values starting at constant index `options` into
// strings: a double is measured with the "%.14g" format and interned, a string
// keeps its pieces, and anything else is converted through its metamethod.
void FormatNumberValue(EngineContext* ctx, uint32_t state, uint32_t value, uint32_t options) {
  EnterFrame();
  uint32_t slot = 0u;        // constant slot pointer, or a piece's tag
  uint32_t length = 0u;      // string length, or the byte count written so far
  uint32_t count = 0u;       // value slot pointer, or the bytes of one piece
  uint32_t target = 0u;      // slot pointer being filled
  uint32_t piece = 0u;       // a slot's tag, or the string of one piece
  uint32_t scratch = 0u;     // base of the 80-byte scratch area
  uint32_t entry = 0u;       // slot pointer, or the format buffer
  uint32_t staged = 0u;      // value staged for the next call
  uint32_t span = 0u;        // byte length of one piece
  uint32_t resume_point = 0u;
  uint32_t frame = 0u;
  uint32_t call_result = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the frame this step saved when it suspended.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kFormatNumberValueFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    options = LoadU32(ctx, frame + 8u);
    slot = LoadU32(ctx, frame + 12u);
    length = LoadU32(ctx, frame + 16u);
    count = LoadU32(ctx, frame + 20u);
    target = LoadU32(ctx, frame + 24u);
    piece = LoadU32(ctx, frame + 28u);
    scratch = LoadU32(ctx, frame + 32u);
    entry = LoadU32(ctx, frame + 36u);
    staged = LoadU32(ctx, frame + 40u);
    value = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word naming the call this step stopped at.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kFormatNumberValueResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Claim the 80-byte scratch area the pieces are formatted through.
    slot = GuestStackSp(ctx);
    scratch = slot - 80u;
    GuestStackSp(ctx) = scratch;
  }
next_argument:
  if (ResumePhase(ctx) == kResumeRunning) {
    // The constant slot this argument names, plus the slots on both sides.
    slot = LoadU32(ctx, state + 12u) + (options << 4u);
    count = slot - 16u;
    entry = slot + 16u;
    slot = (LoadU32(ctx, slot - 8u) - 3u) > 1u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (slot) goto via_metamethod;
    target = entry - 16u;
    slot = LoadU32(ctx, entry - 8u) - 3u;
    switch (slot) {
      case 0: goto format_double;
      case 1: goto value_is_string;
      default: goto via_metamethod;
    }
  via_metamethod:
    length = FetchObjectMetamethod(ctx, state, count, 15u);
    piece = !LoadU32(ctx, length + 8u);
    slot = entry - 16u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (piece) {
      length = FetchObjectMetamethod(ctx, state, slot, 15u);
      piece = !LoadU32(ctx, length + 8u);
      if (piece) goto raise_concat_error;
    }
    // Push the metamethod, its argument and the format options for the call.
    entry = LoadU32(ctx, state + 32u);
    target = LoadU32(ctx, state + 8u);
    StoreU64(ctx, target, LoadU64(ctx, length));
    StoreU32(ctx, target + 8u, LoadU32(ctx, length + 8u));
    length = LoadU32(ctx, state + 8u);
    StoreU64(ctx, length + 16u, LoadU64(ctx, count));
    StoreU32(ctx, length + 24u, LoadU32(ctx, count + 8u));
    length = LoadU32(ctx, state + 8u);
    StoreU64(ctx, length + 32u, LoadU64(ctx, slot));
    StoreU32(ctx, length + 40u, LoadU32(ctx, slot + 8u));
    target = LoadU32(ctx, state + 8u);
    slot = (uint32_t)((int32_t)(LoadU32(ctx, state + 28u) - target) <= (int32_t)48u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      GrowValueStack(ctx, state, 3u);
      resume_point = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    if (ResumePhase(ctx) == kResumeRunning) target = LoadU32(ctx, state + 8u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = target + 48u;
    StoreU32(ctx, state + 8u, slot);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    CallValue(ctx, state, target, 1u);
    resume_point = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Pop the call's result and copy it back into the format buffer.
    slot = LoadU32(ctx, state + 8u) - 16u;
    StoreU32(ctx, state + 8u, slot);
    length = LoadU32(ctx, state + 32u);
    count = length + (count - entry);
    StoreU64(ctx, count, LoadU64(ctx, slot));
    StoreU32(ctx, count + 8u, LoadU32(ctx, slot + 8u));
    slot = 2u;
    goto advance_argument;
  }
raise_concat_error:
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
    FormatConcatenationError(ctx, state, count, slot);
    resume_point = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = 2u;
    goto advance_argument;
  }
format_double:
  if (ResumePhase(ctx) == kResumeRunning) {
    // Stage the double, measure it, then intern the measured text.
    StoreF64(ctx, scratch + 32u, LoadF64(ctx, target));
    slot = scratch + 48u;
    length = scratch + 32u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
    call_result = MeasureFormattedNumber(ctx, slot, kFormatNumberValueFormat, length);
    resume_point = 3u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    slot = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = scratch + 48u;
    length = StringLength(ctx, scratch + 48u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
    call_result = InternString(ctx, state, slot, length);
    resume_point = 4u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    slot = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, target + 8u, 4u);
    StoreU32(ctx, target, slot);
  }
value_is_string:
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, target);
    length = LoadU32(ctx, slot + 12u);
  }
  if (length || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      count = (uint32_t)((int32_t)value <= (int32_t)1u);
      slot = 1u;
    }
    if (count && ResumePhase(ctx) == kResumeRunning) goto buffer_ready;
    for (;;) {
      if (ResumePhase(ctx) == kResumeRunning) {
        count = target - (slot << 4u);
        piece = LoadU32(ctx, count + 8u) - 3u;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        switch (piece) {
          case 0: goto copy_piece_double;
          case 1: goto piece_ready;
          default: goto buffer_ready;
        }
      copy_piece_double:
        StoreF64(ctx, scratch + 16u, LoadF64(ctx, count));
        staged = scratch + 16u;
        piece = scratch + 48u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
        call_result = MeasureFormattedNumber(ctx, piece, kFormatNumberValueFormat, staged);
        resume_point = 5u;
        if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
        piece = call_result;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        staged = StringLength(ctx, scratch + 48u);
        piece = scratch + 48u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
        call_result = InternString(ctx, state, piece, staged);
        resume_point = 6u;
        if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
        piece = call_result;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, count + 8u, 4u);
        StoreU32(ctx, count, piece);
      }
      piece_ready:
      if (ResumePhase(ctx) == kResumeRunning) {
        count = LoadU32(ctx, LoadU32(ctx, count) + 12u);
        piece = count >= kFormatNumberValueLengthLimit - length;
      }
      if (piece || ResumePhase(ctx) == kResumeRewinding) {
        if (ResumePhase(ctx) == kResumeRunning || resume_point == 7u) {
          RaiseScriptError(ctx, state, kFormatNumberValueOverflowMessage, 0u);
          resume_point = 7u;
          if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        length = length + count;
        slot = slot + 1u;
        count = slot != value;
        if (count) continue;
      }
      break;
    }
    slot = (ResumePhase(ctx) == kResumeRunning) ? value : slot;
  buffer_ready:
    if (ResumePhase(ctx) == kResumeRunning) {
      target = 0u;
      count = LoadU32(ctx, state + 16u) + 52u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 8u) {
      call_result = ReallocateFormatBuffer(ctx, state, count, length);
      resume_point = 8u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      length = call_result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Copy the formatted pieces into the freshly sized buffer.
      count = slot;
      for (;;) {
        piece = LoadU32(ctx, entry - (count << 4u));
        span = LoadU32(ctx, piece + 12u);
        staged = CopyBytes(ctx, length + target, piece + 16u, span);
        target = target + span;
        piece = (uint32_t)((int32_t)count > (int32_t)1u);
        count = count - 1u;
        if (piece) continue;
        break;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 9u) {
      call_result = InternString(ctx, state, length, target);
      resume_point = 9u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      count = call_result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      length = entry - (slot << 4u);
      StoreU32(ctx, length + 8u, 4u);
      StoreU32(ctx, length, count);
      goto advance_argument;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The value was not the string the run expected: format its double.
    slot = 2u;
    length = LoadU32(ctx, count + 8u) != 3u;
    if (length) goto advance_argument;
    StoreF64(ctx, scratch, LoadF64(ctx, count));
    length = scratch + 48u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 10u) {
    call_result = MeasureFormattedNumber(ctx, length, kFormatNumberValueFormat, scratch);
    resume_point = 10u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    length = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    target = StringLength(ctx, scratch + 48u);
    length = scratch + 48u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 11u) {
    call_result = InternString(ctx, state, length, target);
    resume_point = 11u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    length = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, count + 8u, 4u);
    StoreU32(ctx, count, length);
  }
advance_argument:
  if (ResumePhase(ctx) == kResumeRunning) {
    // One argument is done: step the format index and the count forward.
    slot = slot - 1u;
    options = options - slot;
    value = value - slot;
    slot = (uint32_t)((int32_t)value > (int32_t)1u);
    if (slot) goto next_argument;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = scratch + 80u;
  }
  goto step_done;
suspend_point:
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kFormatNumberValueResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, value);
  StoreU32(ctx, frame + 8u, options);
  StoreU32(ctx, frame + 12u, slot);
  StoreU32(ctx, frame + 16u, length);
  StoreU32(ctx, frame + 20u, count);
  StoreU32(ctx, frame + 24u, target);
  StoreU32(ctx, frame + 28u, piece);
  StoreU32(ctx, frame + 32u, scratch);
  StoreU32(ctx, frame + 36u, entry);
  StoreU32(ctx, frame + 40u, staged);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kFormatNumberValueFrameBytes);
step_done:
  LeaveFrame();
}

}  // namespace artc
