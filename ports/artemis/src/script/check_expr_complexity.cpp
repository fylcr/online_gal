#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
void DischargeExpression(EngineContext* ctx, uint32_t a0, uint32_t a1);
void CompileExpression(EngineContext* ctx, uint32_t func_state, uint32_t code, uint32_t line);

namespace {

constexpr uint32_t kCheckExprTooComplexMessage = 0x2A371u;  // too complex message
constexpr uint32_t kCheckExprRegisterLimit = 249u;  // hard register ceiling
constexpr uint32_t kCheckExprSaveFrameBytes = 24u;  // saved state words below the resume code
constexpr uint32_t kCheckExprResumeWordBytes = 4u;  // resume code slot

}  // namespace

// Checks the register budget of an expression before it is emitted: the
// expression's token is inspected, the register cursor is raised, and a syntax
// error is raised when the expression is too complex.  The register table and
// the cursor live in the frame because two of the steps suspend.
void CheckExprComplexity(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t slot = 0u;    // the cursor handed to the emit below
  uint32_t limit = 0u;   // the register bound the token asks for
  uint32_t cursor = 0u;  // the register cursor, later an at-limit flag
  uint32_t state = 0u;   // the function's register table
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 24-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kCheckExprSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    a = LoadU32(ctx, frame);
    b = LoadU32(ctx, frame + 4u);
    slot = LoadU32(ctx, frame + 8u);
    limit = LoadU32(ctx, frame + 12u);
    cursor = LoadU32(ctx, frame + 16u);
    state = LoadU32(ctx, frame + 20u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame that saves the resume code.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kCheckExprResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    // Hand the expression back before its own budget is charged.
    DischargeExpression(ctx, a, b);
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (LoadU32(ctx, b) != 12u) { goto skip_limit; }
    limit = LoadU32(ctx, b + 8u);
    if ((limit & 256u) != 0u) { goto skip_limit; }
    if ((int32_t)limit < (int32_t)LoadU8(ctx, a + 50u)) { goto skip_limit; }
    // The token claims a register of its own, so one slot is given back.
    StoreU32(ctx, a + 36u, LoadU32(ctx, a + 36u) - 1u);
skip_limit:
    cursor = LoadU32(ctx, a + 36u);
    limit = cursor + 1u;
    state = LoadU32(ctx, a);
    slot = (int32_t)cursor < (int32_t)LoadU8(ctx, state + 75u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (slot != 0u) {
      // Room left: the cursor moves up to the bound the token asked for.
      slot = limit;
      goto install_cursor;
    }
    cursor = (int32_t)cursor >= (int32_t)kCheckExprRegisterLimit;
    slot = limit;
  }
  if (cursor != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, a + 12u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
      RaiseSyntaxError(ctx, slot, kCheckExprTooComplexMessage);
      resume_point = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // A fresh pass rebuilds the register table and the cursor; a resumed one
      // keeps the copy its frame restored.
      state = LoadU32(ctx, a);
      slot = LoadU32(ctx, a + 36u) + 1u;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The bound the token asked for becomes the table's new limit.
    StoreU8(ctx, state + 75u, limit);
  }
install_cursor:
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, a + 36u, slot);
    slot = slot - 1u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
    // The emit below can suspend with the cursor already installed.
    CompileExpression(ctx, a, b, slot);
    resume_point = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  goto exit_path;
suspend_point:
  // Suspend: write the resume code, then push the frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kCheckExprResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, a);
    StoreU32(ctx, frame + 4u, b);
    StoreU32(ctx, frame + 8u, slot);
    StoreU32(ctx, frame + 12u, limit);
    StoreU32(ctx, frame + 16u, cursor);
    StoreU32(ctx, frame + 20u, state);
  }
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kCheckExprSaveFrameBytes);
exit_path:
  LeaveFrame();
}

}  // namespace artc
