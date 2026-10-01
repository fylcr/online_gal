#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step drives.
void FormatNumberValue(EngineContext* ctx, uint32_t state, uint32_t value, uint32_t options);
uint32_t StringLength(EngineContext* ctx, uint32_t text);
uint32_t InternString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void GrowValueStack(EngineContext* ctx, uint32_t a, uint32_t b);
void StoreSuspendableEntry(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t FindPoolEntryByTagByte(EngineContext* ctx, uint32_t pool, uint32_t key);

namespace {

constexpr uint32_t kFormatNumberEmptyText = 0x2B440u;      // the string the result is built on
constexpr uint32_t kFormatNumberNullText = 0x2B441u;       // "(null)"
constexpr uint32_t kFormatNumberEntryTag = 0x2B448u;       // the tag a formatted entry is stored under
constexpr uint32_t kFormatNumberPercentText = 0x2B44Bu;    // "%"
constexpr uint32_t kFormatNumberAlignMask = 0xFFFFFFF8u;   // rounds a value address down to 8 bytes
constexpr uint32_t kFormatNumberFrameBytes = 72u;          // saved frame: eighteen words
constexpr uint32_t kFormatNumberResumeWordBytes = 4u;      // resume word written before it

}  // namespace

// Converts the value named by the options pointer into a string: the format
// text is walked for '%' specifiers, each one is formatted onto the value stack,
// and the whole run is interned as one entry, which becomes the result.
uint32_t FormatNumber(EngineContext* ctx, uint32_t state, uint32_t value, uint32_t options) {
  EnterFrame();
  uint32_t cursor = 0u;        // where the value under construction is pushed
  uint32_t call_result = 0u;   // the pending call's result
  uint32_t resume_point = 0u;
  uint32_t scratch = 0u;       // the 48-byte scratch block on the guest stack
  uint32_t pending = 0u;       // the state pointer, the pending span, or a "no %" flag
  uint32_t piece = 0u;         // a piece length, the format selector, or a flag
  uint32_t staged = 0u;        // the push depth, or a staged capacity flag
  uint32_t percent = 0u;       // the next '%' in the format text
  uint32_t grow_prefix = 0u;   // the staged capacity check for the prefix push
  uint32_t grow_piece = 0u;    // ... for the pushed specifier
  uint32_t grow_char = 0u;     // ... for the 'c' specifier
  uint32_t grow_int = 0u;      // ... for the 'd' specifier
  uint32_t grow_double = 0u;   // ... for the 'f' specifier
  uint32_t grow_string = 0u;   // ... for the 'p' specifier
  uint32_t grow_other = 0u;    // ... for an unknown specifier
  uint32_t grow_percent = 0u;  // ... for a literal "%%"
  uint32_t grow_final = 0u;    // ... for the finished string
  uint32_t next_cursor = 0u;   // the cursor to store once a push is done
  double staged_value = 0;     // the double staged for the 'f' specifier
  uint32_t frame = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the frame this step saved when it suspended.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kFormatNumberFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    options = LoadU32(ctx, frame + 8u);
    cursor = LoadU32(ctx, frame + 12u);
    scratch = LoadU32(ctx, frame + 16u);
    pending = LoadU32(ctx, frame + 20u);
    piece = LoadU32(ctx, frame + 24u);
    staged = LoadU32(ctx, frame + 28u);
    percent = LoadU32(ctx, frame + 32u);
    grow_prefix = LoadU32(ctx, frame + 36u);
    grow_piece = LoadU32(ctx, frame + 40u);
    grow_char = LoadU32(ctx, frame + 44u);
    grow_int = LoadU32(ctx, frame + 48u);
    grow_double = LoadU32(ctx, frame + 52u);
    grow_string = LoadU32(ctx, frame + 56u);
    grow_other = LoadU32(ctx, frame + 60u);
    grow_percent = LoadU32(ctx, frame + 64u);
    grow_final = LoadU32(ctx, frame + 68u);
    value = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word naming the call this step stopped at.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kFormatNumberResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Reserve the 48-byte scratch block and aim the cursor at the stack top.
    scratch = GuestStackSp(ctx) - 48u;
    GuestStackSp(ctx) = scratch;
    cursor = LoadU32(ctx, state + 8u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    call_result = InternString(ctx, state, kFormatNumberEmptyText, 0u);
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    staged = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Stage the string so far as the value under construction.
    StoreU32(ctx, cursor + 8u, 4u);
    StoreU32(ctx, cursor, staged);
    cursor = LoadU32(ctx, state + 8u);
    staged = (uint32_t)((int32_t)(LoadU32(ctx, state + 28u) - cursor) <= (int32_t)16u);
    pending = state;
  }
  grow_prefix = (ResumePhase(ctx) == kResumeRunning) ? staged : grow_prefix;
  if (grow_prefix || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
      GrowValueStack(ctx, state, 1u);
      resume_point = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    cursor = (ResumePhase(ctx) == kResumeRunning) ? LoadU32(ctx, state + 8u) : cursor;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Push the cursor and look for the next '%' in the format text.
    cursor = cursor + 16u;
    StoreU32(ctx, pending + 8u, cursor);
    percent = FindPoolEntryByTagByte(ctx, value, 37u);
    pending = (percent == 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    staged = 1u;
    if (pending) goto stringify_all;
  }
loop_format:
  pending = (ResumePhase(ctx) == kResumeRunning) ? (percent - value) : pending;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
    call_result = InternString(ctx, state, value, pending);
    resume_point = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    value = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Stage the text before the '%' as the next value.
    StoreU32(ctx, cursor + 8u, 4u);
    StoreU32(ctx, cursor, value);
    cursor = LoadU32(ctx, state + 8u);
    value = (uint32_t)((int32_t)(LoadU32(ctx, state + 28u) - cursor) <= (int32_t)16u);
  }
  if (value || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
      GrowValueStack(ctx, state, 1u);
      resume_point = 3u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    cursor = (ResumePhase(ctx) == kResumeRunning) ? LoadU32(ctx, state + 8u) : cursor;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, state + 8u, cursor + 16u);
    value = LoadS8(ctx, percent + 1u);
    piece = value - 99u;
    pending = state;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    switch (piece) {
      case 0: goto char_case;
      case 1: goto int_case;
      case 2: goto unknown_spec_case;
      case 3: goto double_case;
      case 4: goto unknown_spec_case;
      case 5: goto unknown_spec_case;
      case 6: goto unknown_spec_case;
      case 7: goto unknown_spec_case;
      case 8: goto unknown_spec_case;
      case 9: goto unknown_spec_case;
      case 10: goto unknown_spec_case;
      case 11: goto unknown_spec_case;
      case 12: goto unknown_spec_case;
      case 13: goto string_case;
      case 14: goto unknown_spec_case;
      case 15: goto unknown_spec_case;
      case 16: goto string_arg_case;
      default: goto check_literal_percent;
    }
  string_arg_case:
    value = LoadU32(ctx, options);
    value = (value != 0u) ? value : kFormatNumberNullText;
    piece = StringLength(ctx, value);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
    call_result = InternString(ctx, state, value, piece);
    resume_point = 4u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    value = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Stage the specifier's text as the next value and step past the argument.
    StoreU32(ctx, cursor + 24u, 4u);
    StoreU32(ctx, cursor + 16u, value);
    options = options + 4u;
    cursor = LoadU32(ctx, state + 8u);
    value = (uint32_t)((int32_t)(LoadU32(ctx, state + 28u) - cursor) <= (int32_t)16u);
  }
  grow_piece = (ResumePhase(ctx) == kResumeRunning) ? value : grow_piece;
  if (grow_piece || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
      GrowValueStack(ctx, state, 1u);
      resume_point = 5u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    cursor = (ResumePhase(ctx) == kResumeRunning) ? LoadU32(ctx, state + 8u) : cursor;
  }
  next_cursor = cursor + 16u;
  if (ResumePhase(ctx) == kResumeRunning) goto value_pushed;
char_case:
  if (ResumePhase(ctx) == kResumeRunning) {
    // A character argument goes through the scratch block as a one-byte string.
    value = LoadU32(ctx, options);
    StoreU8(ctx, scratch + 17u, 0u);
    StoreU8(ctx, scratch + 16u, value);
    piece = StringLength(ctx, scratch + 16u);
    value = scratch + 16u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
    call_result = InternString(ctx, state, value, piece);
    resume_point = 6u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    value = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, cursor + 24u, 4u);
    StoreU32(ctx, cursor + 16u, value);
    options = options + 4u;
    cursor = LoadU32(ctx, state + 8u);
    value = (uint32_t)((int32_t)(LoadU32(ctx, state + 28u) - cursor) <= (int32_t)16u);
  }
  grow_char = (ResumePhase(ctx) == kResumeRunning) ? value : grow_char;
  if (grow_char || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 7u) {
      GrowValueStack(ctx, state, 1u);
      resume_point = 7u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    cursor = (ResumePhase(ctx) == kResumeRunning) ? LoadU32(ctx, state + 8u) : cursor;
  }
  next_cursor = cursor + 16u;
  if (ResumePhase(ctx) == kResumeRunning) goto value_pushed;
int_case:
  if (ResumePhase(ctx) == kResumeRunning) {
    // An integer argument is pushed as an f64 value with tag 3.
    value = LoadU32(ctx, options);
    StoreU32(ctx, cursor + 24u, 3u);
    StoreF64(ctx, cursor + 16u, (double)(int32_t)value);
    options = options + 4u;
    cursor = LoadU32(ctx, state + 8u);
    value = (uint32_t)((int32_t)(LoadU32(ctx, state + 28u) - cursor) <= (int32_t)16u);
  }
  grow_int = (ResumePhase(ctx) == kResumeRunning) ? value : grow_int;
  if (grow_int || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 8u) {
      GrowValueStack(ctx, state, 1u);
      resume_point = 8u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    cursor = (ResumePhase(ctx) == kResumeRunning) ? LoadU32(ctx, state + 8u) : cursor;
  }
  next_cursor = cursor + 16u;
  if (ResumePhase(ctx) == kResumeRunning) goto value_pushed;
double_case:
  if (ResumePhase(ctx) == kResumeRunning) {
    // A float argument is aligned and pushed as an f64 value with tag 3.
    value = (options + 7u) & kFormatNumberAlignMask;
    staged_value = LoadF64(ctx, value);
    StoreU32(ctx, cursor + 24u, 3u);
    StoreF64(ctx, cursor + 16u, staged_value);
    options = value + 8u;
    cursor = LoadU32(ctx, state + 8u);
    value = (uint32_t)((int32_t)(LoadU32(ctx, state + 28u) - cursor) <= (int32_t)16u);
  }
  grow_double = (ResumePhase(ctx) == kResumeRunning) ? value : grow_double;
  if (grow_double || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 9u) {
      GrowValueStack(ctx, state, 1u);
      resume_point = 9u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    cursor = (ResumePhase(ctx) == kResumeRunning) ? LoadU32(ctx, state + 8u) : cursor;
  }
  next_cursor = cursor + 16u;
  if (ResumePhase(ctx) == kResumeRunning) goto value_pushed;
string_case:
  if (ResumePhase(ctx) == kResumeRunning) {
    // A pointer argument is copied into the scratch block as text.
    StoreU32(ctx, scratch, LoadU32(ctx, options));
    value = scratch + 16u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 10u) {
    StoreSuspendableEntry(ctx, value, kFormatNumberEntryTag, scratch);
    resume_point = 10u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    value = LoadU32(ctx, state + 8u);
    piece = StringLength(ctx, scratch + 16u);
    cursor = scratch + 16u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 11u) {
    call_result = InternString(ctx, state, cursor, piece);
    resume_point = 11u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    cursor = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, value + 8u, 4u);
    StoreU32(ctx, value, cursor);
    options = options + 4u;
    cursor = LoadU32(ctx, state + 8u);
    value = (uint32_t)((int32_t)(LoadU32(ctx, state + 28u) - cursor) <= (int32_t)16u);
  }
  grow_string = (ResumePhase(ctx) == kResumeRunning) ? value : grow_string;
  if (grow_string || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 12u) {
      GrowValueStack(ctx, state, 1u);
      resume_point = 12u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    cursor = (ResumePhase(ctx) == kResumeRunning) ? LoadU32(ctx, state + 8u) : cursor;
  }
  next_cursor = cursor + 16u;
  if (ResumePhase(ctx) == kResumeRunning) goto value_pushed;
check_literal_percent:
  if (ResumePhase(ctx) == kResumeRunning) {
    piece = (value == 37u);
    if (piece) goto literal_percent_case;
  }
unknown_spec_case:
  if (ResumePhase(ctx) == kResumeRunning) {
    // Anything else is rendered literally, preceded by its '%'.
    StoreU8(ctx, scratch + 18u, 0u);
    StoreU8(ctx, scratch + 17u, value);
    StoreU8(ctx, scratch + 16u, 37u);
    piece = StringLength(ctx, scratch + 16u);
    value = scratch + 16u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 13u) {
    call_result = InternString(ctx, state, value, piece);
    resume_point = 13u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    value = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, cursor + 24u, 4u);
    StoreU32(ctx, cursor + 16u, value);
    cursor = LoadU32(ctx, state + 8u);
    value = (uint32_t)((int32_t)(LoadU32(ctx, state + 28u) - cursor) <= (int32_t)16u);
  }
  grow_other = (ResumePhase(ctx) == kResumeRunning) ? value : grow_other;
  if (grow_other || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 14u) {
      GrowValueStack(ctx, state, 1u);
      resume_point = 14u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    cursor = (ResumePhase(ctx) == kResumeRunning) ? LoadU32(ctx, state + 8u) : cursor;
  }
  next_cursor = cursor + 16u;
  if (ResumePhase(ctx) == kResumeRunning) goto value_pushed;
literal_percent_case:
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 15u) {
    call_result = InternString(ctx, state, kFormatNumberPercentText, 1u);
    resume_point = 15u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    value = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, cursor + 24u, 4u);
    StoreU32(ctx, cursor + 16u, value);
    cursor = LoadU32(ctx, state + 8u);
    value = (uint32_t)((int32_t)(LoadU32(ctx, state + 28u) - cursor) <= (int32_t)16u);
  }
  grow_percent = (ResumePhase(ctx) == kResumeRunning) ? value : grow_percent;
  if (grow_percent || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 16u) {
      GrowValueStack(ctx, state, 1u);
      resume_point = 16u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    cursor = (ResumePhase(ctx) == kResumeRunning) ? LoadU32(ctx, state + 8u) : cursor;
  }
  // A resumed pass keeps the cursor its frame restored; a fresh pass steps
  // past the value it has just pushed.
  next_cursor = (ResumePhase(ctx) == kResumeRunning) ? (cursor + 16u) : cursor;
value_pushed:
  cursor = next_cursor;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Store the cursor and continue at the next '%'.
    StoreU32(ctx, pending + 8u, cursor);
    staged = staged + 2u;
    value = percent + 2u;
    percent = FindPoolEntryByTagByte(ctx, value, 37u);
    if (percent) goto loop_format;
  }
stringify_all:
  if (ResumePhase(ctx) == kResumeRunning) {
    options = StringLength(ctx, value);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 17u) {
    call_result = InternString(ctx, state, value, options);
    resume_point = 17u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    value = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Stage the finished string as the last value.
    StoreU32(ctx, cursor + 8u, 4u);
    StoreU32(ctx, cursor, value);
    options = state;
    cursor = LoadU32(ctx, state + 8u);
    value = (uint32_t)((int32_t)(LoadU32(ctx, state + 28u) - cursor) <= (int32_t)16u);
  }
  grow_final = (ResumePhase(ctx) == kResumeRunning) ? value : grow_final;
  if (grow_final || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 18u) {
      GrowValueStack(ctx, state, 1u);
      resume_point = 18u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    cursor = (ResumePhase(ctx) == kResumeRunning) ? LoadU32(ctx, state + 8u) : cursor;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    value = cursor + 16u;
    StoreU32(ctx, options + 8u, value);
    options = staged + 1u;
    value = (uint32_t)((int32_t)(value - LoadU32(ctx, state + 12u)) >> 4u);
    value = value - 1u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 19u) {
    FormatNumberValue(ctx, state, options, value);
    resume_point = 19u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Drop the values pushed for the result and hand back the string.
    value = state;
    state = LoadU32(ctx, state + 8u) - (staged << 4u);
    StoreU32(ctx, value + 8u, state);
    state = LoadU32(ctx, state - 16u);
    GuestStackSp(ctx) = scratch + 48u;
    LeaveFrame();
    return state + 16u;
  }
  TrapUnreachable();
suspend_point:
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kFormatNumberResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, value);
  StoreU32(ctx, frame + 8u, options);
  StoreU32(ctx, frame + 12u, cursor);
  StoreU32(ctx, frame + 16u, scratch);
  StoreU32(ctx, frame + 20u, pending);
  StoreU32(ctx, frame + 24u, piece);
  StoreU32(ctx, frame + 28u, staged);
  StoreU32(ctx, frame + 32u, percent);
  StoreU32(ctx, frame + 36u, grow_prefix);
  StoreU32(ctx, frame + 40u, grow_piece);
  StoreU32(ctx, frame + 44u, grow_char);
  StoreU32(ctx, frame + 48u, grow_int);
  StoreU32(ctx, frame + 52u, grow_double);
  StoreU32(ctx, frame + 56u, grow_string);
  StoreU32(ctx, frame + 60u, grow_other);
  StoreU32(ctx, frame + 64u, grow_percent);
  StoreU32(ctx, frame + 68u, grow_final);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kFormatNumberFrameBytes);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
