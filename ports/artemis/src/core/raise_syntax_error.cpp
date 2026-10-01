#include "util/engine_context.h"

namespace artc {

// The lexer error the refusal is reported through.
void LexError(EngineContext* ctx, uint32_t lex_state, uint32_t message, uint32_t token);

// Raises the syntax error the lexer reports, taking the token word out of the
// lexer state. Suspendable: the state, the message word and the token travel
// through the return frame.
void RaiseSyntaxError(EngineContext* ctx, uint32_t lex_state, uint32_t message) {
  EnterFrame();
  // The token word the error carries; a running pass reads it fresh from the
  // lexer state below.
  uint32_t token = 0u;
  // Zero means the error below never completed, so it is issued again.
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 12-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 12u);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    lex_state = LoadU32(ctx, frame);
    message = LoadU32(ctx, frame + 4u);
    token = LoadU32(ctx, frame + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed with that frame.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    token = LoadU32(ctx, lex_state + 16u);
  }
  // A running pass always owes the error; a rewinding one owes it only at the
  // resume point it popped above.
  uint32_t issue_error = static_cast<uint32_t>(!resume_point);
  issue_error = ResumePhase(ctx) != kResumeRunning ? issue_error : 1u;
  if (issue_error) {
    LexError(ctx, lex_state, message, token);
    if (ResumePhase(ctx) == kResumeSuspended) {
      // The error reports zero, which is also the resume point saved below.
      goto spill_point;
    }
  }
  LeaveFrame();
  return;
spill_point:;
  // Suspend: push the resume point, then a 12-byte frame holding the lexer state,
  // the message word and the token the rewinding pop restores.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), 0u);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, saved_frame, lex_state);
  StoreU32(ctx, saved_frame + 4u, message);
  StoreU32(ctx, saved_frame + 8u, token);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 12u);
  LeaveFrame();
  return;
}

}  // namespace artc
