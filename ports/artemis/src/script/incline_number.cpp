#include "util/engine_context.h"

namespace artc {

// Defined in other products.
uint32_t NextCharSlow(EngineContext* ctx, uint32_t cursor);
void LexError(EngineContext* ctx, uint32_t state, uint32_t message, uint32_t argument);

namespace {

constexpr uint32_t kInclineNumberChunkLimitMessage = 0x2AD0Au;  // chunk has too many lines
constexpr uint32_t kInclineNumberLineLimit = 0x7FFFFFFCu;  // highest count the line counter may reach
constexpr uint32_t kInclineNumberSaveFrameBytes = 16u;  // lex state, current byte, target, previous byte
constexpr uint32_t kInclineNumberResumeWordBytes = 4u;
constexpr uint32_t kInclineNumberReadCall = 0u;  // the first cursor read was in flight
constexpr uint32_t kInclineNumberSkipCall = 1u;  // the read stepping over a doubled newline was in flight
constexpr uint32_t kInclineNumberLimitCall = 2u;  // the line limit error was in flight

}  // namespace

// Steps the lexer over the byte under the cursor: the byte is written back into
// the lexer state, the chunk's line count is adjusted, and the cursor moves past
// it. Two newline bytes in a row count as one line, and a count that runs past
// the chunk limit raises the chunk-has-too-many-lines error instead.
// The slots below are reused across the step: `current` also carries the limit
// verdict, `target` the pointer the byte is written through, `scratch` the result
// of a cursor read.
void InclineNumber(EngineContext* ctx, uint32_t lex_state) {
  EnterFrame();
  uint32_t current = 0u;
  uint32_t target = 0u;
  uint32_t scratch = 0u;
  uint32_t previous = 0u;
  uint32_t resume_word = 0u;
  uint32_t resume_code = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kInclineNumberSaveFrameBytes);
    previous = LoadU32(ctx, ReturnFrameSp(ctx));
    lex_state = LoadU32(ctx, previous);
    current = LoadU32(ctx, previous + 4u);
    target = LoadU32(ctx, previous + 8u);
    previous = LoadU32(ctx, previous + 12u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kInclineNumberResumeWordBytes);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    previous = LoadU32(ctx, lex_state);
    current = LoadU32(ctx, lex_state + 56u);
    scratch = LoadU32(ctx, current);
    StoreU32(ctx, current, scratch - 1u);
    target = lex_state;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (scratch != 0u) {
      // The cursor already has the next byte: step it and read.
      uint32_t const next = LoadU32(ctx, current + 4u);
      StoreU32(ctx, current + 4u, next + 1u);
      current = LoadU8(ctx, next);
      goto have_byte;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_word == kInclineNumberReadCall) {
    scratch = NextCharSlow(ctx, current);
    resume_code = kInclineNumberReadCall;
    if (ResumePhase(ctx) == kResumeSuspended) {goto suspend_point;}
    current = scratch;
  }
  have_byte:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, target, current);
    target = current - 10u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    switch (target) {
      case 0u:  // a line feed
      case 3u:  // a carriage return
        // A newline byte counts unless the very same byte came just before.
        current = static_cast<uint32_t>(current == previous);
        if (current != 0u) {goto trim_line;}
        current = LoadU32(ctx, lex_state + 56u);
        previous = LoadU32(ctx, current);
        target = previous - 1u;
        StoreU32(ctx, current, target);
        target = lex_state;
        break;
      default:
        goto trim_line;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (previous != 0u) {
      uint32_t const next = LoadU32(ctx, current + 4u);
      StoreU32(ctx, current + 4u, next + 1u);
      current = LoadU8(ctx, next);
      goto step_done;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_word == kInclineNumberSkipCall) {
    scratch = NextCharSlow(ctx, current);
    resume_code = kInclineNumberSkipCall;
    if (ResumePhase(ctx) == kResumeSuspended) {goto suspend_point;}
    current = scratch;
  }
  step_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, target, current);
  }
  trim_line:;
  if (ResumePhase(ctx) == kResumeRunning) {
    current = LoadU32(ctx, lex_state + 4u);
    StoreU32(ctx, lex_state + 4u, current + 1u);
    current = static_cast<uint32_t>((int32_t)current >= (int32_t)kInclineNumberLineLimit);
  }
  if (current != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      current = LoadU32(ctx, lex_state + 16u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_word == kInclineNumberLimitCall) {
      LexError(ctx, lex_state, kInclineNumberChunkLimitMessage, current);
      resume_code = kInclineNumberLimitCall;
      if (ResumePhase(ctx) == kResumeSuspended) {goto suspend_point;}
    }
  }
  // The running pass is done; the frame write below belongs to a suspend only.
  goto exit_path;
  // Suspend here: the resume word naming the call in flight goes first, then the
  // four saved words, and the caller's frame pointer is left above both.
  suspend_point:;
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kInclineNumberResumeWordBytes);
  scratch = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, scratch, lex_state);
  StoreU32(ctx, scratch + 4u, current);
  StoreU32(ctx, scratch + 8u, target);
  StoreU32(ctx, scratch + 12u, previous);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kInclineNumberSaveFrameBytes);
  exit_path:;
  LeaveFrame();
}
}  // namespace artc
