// step_record_resume.cpp — one asyncify resume step for a script record.
//
// StepRecordResume advances one record a single step and, on a rewind pass,
// rebuilds the two values that step handed on: the record itself and the state
// carried with it. The record keeps two 16-byte state blocks — the live one at
// +32, the previous one at +16. On the running pass its +4 word is shadowed
// into +8, and where the +32 block still holds an unmarked tag that block is
// moved down into the +(16..31) pair and the slot takes the mark; that is the
// copy which ends the step early, and the only path that returns before the
// refill runs. A record that is already marked instead hands its +24 word to
// the refill and stores the answer back at +16.
//
// The refill can suspend the whole resume, so the step keeps the runner's
// return-frame discipline: a rewind pass pops the saved words back off the
// event return-frame stack at entry, and a suspend pushes them again before
// returning, in front of an empty result slot the runner appended. The stack
// pointer lives in the word the frame pointer itself names, so every pop and
// push is a read-modify-write of that word rather than a bare pointer bump.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//
#include "util/engine_context.h"

namespace artc {

// Forward declaration — defined in script/refill_record_state.cpp.
// Provisional: that is the generated f1727 body, not registered yet. The
// declaration is written to match the assembler's cross-product prototype
// block exactly, so this product also checks on its own.
uint32_t RefillRecordState(EngineContext* ctx, uint32_t record, uint32_t arg);

// Provisional name and location: no call site pins the record's owning
// subsystem, so the step is named for what it does to the record.
void StepRecordResume(EngineContext* ctx, uint32_t record) {
  EnterFrame();
  // The tag the +32 slot takes once its 16-byte state block has moved down into
  // the +(16..31) pair; the refill returns it as one of its own outcomes.
  uint32_t const kStateMark = 287u;
  uint32_t carried = 0u;
  uint32_t result = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewind pop: drop the frame pointer by 8 and read the block it now names,
    // which restores the record and the value carried with it.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 8u);
    carried = LoadU32(ctx, ReturnFrameSp(ctx));
    record = LoadU32(ctx, carried);
    carried = LoadU32(ctx, carried + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // A second, 4-byte pop names the refill's own result slot.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    result = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, record + 8u, LoadU32(ctx, record + 4u));
    if (LoadU32(ctx, record + 32u) != kStateMark) {
      // An unmarked live block: move it into the previous-state pair, mark the
      // slot, and leave — this step never reaches the refill.
      carried = record + 32u;
      StoreU64(ctx, record + 16u, LoadU64(ctx, carried));
      StoreU64(ctx, record + 24u, LoadU64(ctx, carried + 8u));
      StoreU32(ctx, record + 32u, kStateMark);
      LeaveFrame();
      return;
    }
    // Already marked: the refill works on the second half of the pair.
    carried = record + 24u;
  }
  // The refill runs unconditionally on the running pass, and on a suspended or
  // rewinding pass only when the pop above handed back a zero result: the pop
  // yields zero exactly when the helper suspended the previous time round.
  bool const no_result = (result == 0u);
  bool const past_running = (ResumePhase(ctx) != kResumeRunning);
  if (past_running ? no_result : true) {
    result = RefillRecordState(ctx, record, carried);
    // The runner stages a zero here right after the call, whatever the outcome;
    // only the suspend path below hands it on.
    uint32_t const abandoned = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      // Suspended: the runner appends an empty result slot of its own, then the
      // record and the carried value, for the rewind pass to pop back.
      result = abandoned;
      StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), result);
      StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
      result = LoadU32(ctx, ReturnFrameSp(ctx));
      StoreU32(ctx, result, record);
      StoreU32(ctx, result + 4u, carried);
      StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 8u);
      LeaveFrame();
      return;
    }
    // The refill ran to completion: its answer is the value carried onward.
    carried = result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, record + 16u, carried);
  }
  LeaveFrame();
}

}  // namespace artc
