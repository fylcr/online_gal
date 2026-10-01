#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
void StepRecordResume(EngineContext* ctx, uint32_t a0);
void LexError(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t EmitOpInstruction(EngineContext* ctx, uint32_t func_state, uint32_t op, uint32_t arg_a, uint32_t arg_b, uint32_t line);
void PatchJumpChain(EngineContext* ctx, uint32_t state, uint32_t instruction);
uint32_t CompileStatement(EngineContext* ctx, uint32_t state);

namespace {

constexpr uint32_t kCloseBlockTooManyLevelsMessage = 0x2B633u;  // too many syntax levels
constexpr uint32_t kCloseBlockCloserMask = 0x8010007u;  // bits of the closing kinds
constexpr uint32_t kCloseBlockFirstCloser = 260u;  // first closing kind
constexpr uint32_t kCloseBlockCloserSpan = 27u;  // closing kinds 260..287
constexpr uint32_t kCloseBlockSemicolon = 59u;  // ';'
constexpr uint32_t kCloseBlockNoSlot = 0xFFFFFFFFu;  // empty slot marker
constexpr uint32_t kCloseBlockLevelLimit = 201u;  // nesting limit
constexpr uint32_t kCloseBlockJumpOpcode = 35u;  // the jump op that is emitted
constexpr uint32_t kCloseBlockRecordBytes = 16u;  // guest-stack scope record
constexpr uint32_t kCloseBlockSaveFrameBytes = 28u;  // saved state words below the resume code
constexpr uint32_t kCloseBlockResumeWordBytes = 4u;  // resume code slot

}  // namespace

// Closes every scope opened past the current block: a scope record is pushed,
// the block's scopes are walked, their local slots are released and the jump
// that leaves the block is patched.  The calls in the middle suspend, so the
// scope pointers and the level counters are kept in the frame.
void CloseBlockScope(EngineContext* ctx, uint32_t state) {
  EnterFrame();
  uint32_t level = 0u;        // the scope level, later a scope pointer
  uint32_t flag = 0u;         // a level flag, later the slot table
  uint32_t scope = 0u;        // the scope that is being closed
  uint32_t scratch = 0u;      // the 16-byte record taken off the guest stack
  uint32_t inner = 0u;        // the innermost scope, later a statement
  uint32_t scope_level = 0u;  // the level the closed scope starts at
  uint32_t slot_index = 0u;   // index of the slot being released
  uint32_t result = 0u;       // what a callee hands back
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 28-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kCloseBlockSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    flag = LoadU32(ctx, frame + 8u);
    scope = LoadU32(ctx, frame + 12u);
    scratch = LoadU32(ctx, frame + 16u);
    inner = LoadU32(ctx, frame + 20u);
    scope_level = LoadU32(ctx, frame + 24u);
    level = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame that saves the resume code.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kCloseBlockResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A 16-byte scope record is taken off the guest stack and linked in.
    scratch = GuestStackSp(ctx) - kCloseBlockRecordBytes;
    GuestStackSp(ctx) = scratch;
    scope = LoadU32(ctx, state + 48u);
    StoreU8(ctx, scratch + 10u, 0u);
    StoreU32(ctx, scratch + 4u, kCloseBlockNoSlot);
    level = LoadU8(ctx, scope + 50u);
    StoreU8(ctx, scratch + 9u, 0u);
    StoreU8(ctx, scratch + 8u, level);
    StoreU32(ctx, scratch, LoadU32(ctx, scope + 20u));
    StoreU32(ctx, scope + 20u, scratch);
    // The block's level is raised; the limit is reported below.
    flag = LoadU32(ctx, state + 52u);
    level = LoadU16(ctx, flag + 52u) + 1u;
    StoreU16(ctx, flag + 52u, level);
    flag = (level & 65535u) >= kCloseBlockLevelLimit;
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      // Too many nested levels: report and give the block up.
      LexError(ctx, state, kCloseBlockTooManyLevelsMessage, 0u);
      resume_point = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
  }
close_scope:
  if (ResumePhase(ctx) == kResumeRunning) {
    // The token closes a scope only when its kind is one of the closers.
    level = LoadU32(ctx, state + 16u) - kCloseBlockFirstCloser;
    flag = level <= kCloseBlockCloserSpan
        && ((1u << (level & 31u)) & kCloseBlockCloserMask) != 0u;
    flag = !flag;
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
      // Compile the nested statement the scope holds, one scope at a time.
      result = CompileStatement(ctx, state);
      resume_point = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      level = result;
    }
    // A fresh run recomputes the decision from the token after the scope; a
    // resumed one keeps the answer its frame restored.
    if (ResumePhase(ctx) == kResumeRunning) {
      flag = LoadU32(ctx, state + 16u) == kCloseBlockSemicolon;
    }
    if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
        StepRecordResume(ctx, state);
        resume_point = 2u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The scope's level becomes its cursor for the next statement.
      inner = LoadU32(ctx, state + 48u);
      flag = LoadU8(ctx, inner + 50u);
      StoreU32(ctx, inner + 36u, flag);
      flag = level == 0u;
      if (flag != 0u) { goto close_scope; }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The block leaves its scope: the level drops and the scope is unlinked.
    state = LoadU32(ctx, state + 52u);
    flag = LoadU16(ctx, state + 52u) - 1u;
    StoreU16(ctx, state + 52u, flag);
    inner = LoadU32(ctx, scope + 20u);
    StoreU32(ctx, scope + 20u, LoadU32(ctx, inner));
    level = LoadU32(ctx, scope + 12u);
    level = LoadU32(ctx, level + 48u);
    state = LoadU8(ctx, level + 50u);
    scope_level = LoadU8(ctx, inner + 8u);
    if (state > scope_level) {
      // Release the local slots the closed scope still holds.
      result = LoadU32(ctx, level + 24u);
      flag = LoadU32(ctx, LoadU32(ctx, level) + 24u);
free_slots:
      state = state - 1u;
      slot_index = state & 255u;
      StoreU32(ctx, flag + LoadU16(ctx, level + slot_index * 2u + 172u) * 12u + 8u,
          result);
      if (scope_level < slot_index) { goto free_slots; }
      StoreU8(ctx, level + 50u, scope_level);
    }
    state = LoadU8(ctx, inner + 9u);
  }
  if (state != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
      // Emit the jump that leaves the block.
      result = EmitOpInstruction(ctx, scope, kCloseBlockJumpOpcode, scope_level, 0u, 0u);
      resume_point = 3u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
      state = result;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The scope's slots are handed back before the jump is patched.
    StoreU32(ctx, scope + 36u, LoadU8(ctx, scope + 50u));
    state = LoadU32(ctx, inner + 4u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
    PatchJumpChain(ctx, scope, state);
    resume_point = 4u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Give the 16-byte scope record back to the guest stack.
    GuestStackSp(ctx) = scratch + kCloseBlockRecordBytes;
  }
  goto exit_path;
suspend_point:
  // Suspend: write the resume code, then push the frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kCloseBlockResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, state);
    StoreU32(ctx, frame + 4u, level);
    StoreU32(ctx, frame + 8u, flag);
    StoreU32(ctx, frame + 12u, scope);
    StoreU32(ctx, frame + 16u, scratch);
    StoreU32(ctx, frame + 20u, inner);
    StoreU32(ctx, frame + 24u, scope_level);
  }
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kCloseBlockSaveFrameBytes);
exit_path:
  LeaveFrame();
}

}  // namespace artc
