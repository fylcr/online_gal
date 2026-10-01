#include "util/engine_context.h"

namespace artc {

// Forward declaration -- defined in script/refill_record_state.cpp.
uint32_t RefillRecordState(EngineContext* ctx, uint32_t record, uint32_t arg);

// Advances a script record one resume step: the live 16-byte state block moves
// down into the previous-state pair and the slot takes the mark, or, when the
// block is already marked, the refill is asked for the next state and its answer
// is stored back. Answers nothing.
void StepRecordResume(EngineContext* ctx, uint32_t record) {
  EnterFrame();
  // The tag the +32 slot takes once its state block has moved down.
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
      // The unmarked path leaves before the refill is ever reached.
      carried = record + 32u;
      StoreU64(ctx, record + 16u, LoadU64(ctx, carried));
      StoreU64(ctx, record + 24u, LoadU64(ctx, carried + 8u));
      StoreU32(ctx, record + 32u, kStateMark);
      LeaveFrame();
      return;
    }
    carried = record + 24u;
  }
  // The pop above yields zero exactly when the helper suspended last time round,
  // which is the only thing that gates the call on a resumed pass.
  bool const no_result = (result == 0u);
  bool const past_running = (ResumePhase(ctx) != kResumeRunning);
  if (past_running ? no_result : true) {
    result = RefillRecordState(ctx, record, carried);
    uint32_t const abandoned = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      // Suspended: append an empty result slot, then the record and the carried
      // value, for the rewind pass to pop back.
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
    carried = result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, record + 16u, carried);
  }
  LeaveFrame();
}

}  // namespace artc
