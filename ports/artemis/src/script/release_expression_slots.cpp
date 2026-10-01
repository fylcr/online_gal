#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
void CheckExprComplexity(EngineContext* ctx, uint32_t a, uint32_t b);
void EmitBranchWord(EngineContext* ctx, uint32_t state, uint32_t opcode, uint32_t operands);
void ReserveRegisters(EngineContext* ctx, uint32_t state, uint32_t count);
void AllocateExpressionRegister(EngineContext* ctx, uint32_t state, uint32_t instruction, uint32_t expression);

namespace {

constexpr uint32_t kReleaseExpressionSlotsSaveFrameBytes = 16u;  // live state pushed on suspend
constexpr uint32_t kReleaseExpressionSlotsResumeWordBytes = 4u;  // resume word above frame

}  // namespace

// Releases the expression registers an instruction consumed: the incoming count
// rebases the instruction index, and the count the compiler state now holds
// selects the path — the widest operands step the index first, the ordinary ones
// have their expression checked, and every path that goes on reserves the
// registers the branch needs and emits the branch word. The step is suspendable:
// each gate replays the call its resume code names, and the save frame holds
// state @0, instruction @4, count @8 and the compiler-state pointer @12.
void ReleaseExpressionSlots(EngineContext* ctx, uint32_t state, uint32_t instruction, uint32_t count, uint32_t unused) {
  EnterFrame();
  // The save frame base: a rewind pops it and the suspend path writes its four
  // words into it.
  uint32_t frame = 0u;
  // The resume point a rewind pops, naming the call that was in flight.
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 16-byte frame the suspend path saved and read it in the
    // order it was written.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kReleaseExpressionSlotsSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    instruction = LoadU32(ctx, frame + 4u);
    count = LoadU32(ctx, frame + 8u);
    unused = LoadU32(ctx, frame + 12u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed above that frame.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kReleaseExpressionSlotsResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the count the incoming call was given is dropped from the
    // instruction index, and the count to release is read from the compiler
    // state.
    instruction -= count;
    count = LoadU32(ctx, unused);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The count selects the path: nothing to release, an ordinary operand whose
    // expression is checked first, or one of the widest, whose index is stepped.
    switch (count) {
      case 0: goto emit_branch;
      case 13:
      case 14: goto widen_operand;
      default: goto check_expression;
    }
  widen_operand:;
    count = instruction + 1u;
    instruction = static_cast<int32_t>(instruction) < 0 ? 0u : count;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    AllocateExpressionRegister(ctx, state, unused, instruction);
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: a register short of two leaves nothing to reserve.
    count = static_cast<uint32_t>(static_cast<int32_t>(instruction) < 2);
    if (count != 0u) {
      goto exit_path;
    }
    instruction -= 1u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    ReserveRegisters(ctx, state, instruction);
    resume_point = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto exit_path;
  }
check_expression:;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
    CheckExprComplexity(ctx, state, unused);
    resume_point = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
emit_branch:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: only an instruction that still owes registers reaches the branch
    // word, which the compiler state names.
    if (static_cast<int32_t>(instruction) < static_cast<int32_t>(1u)) {
      goto exit_path;
    }
    count = LoadU32(ctx, state + 36u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
    ReserveRegisters(ctx, state, instruction);
    resume_point = 3u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
    EmitBranchWord(ctx, state, count, instruction);
    resume_point = 4u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  goto exit_path;
suspend_point:;
  // Push the resume point naming the call in flight, then the frame this pass's
  // save words read back. The resume word goes below the frame pointer, which
  // then advances past it to make room for the four saved words.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kReleaseExpressionSlotsResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, instruction);
  StoreU32(ctx, frame + 8u, count);
  StoreU32(ctx, frame + 12u, unused);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kReleaseExpressionSlotsSaveFrameBytes);
exit_path:;
  LeaveFrame();
}

}  // namespace artc
