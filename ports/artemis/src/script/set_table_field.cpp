#include "script/value_slot.h"
#include "util/engine_context.h"

namespace artc {

// Forward declaration -- the numeric get-or-create lookup.
uint32_t GetTableFieldByNumber(EngineContext* ctx, uint32_t state, uint32_t table, uint32_t key);

// Copies the value record on top of the value stack into the field a pseudo
// index and key name, creating the key's record when the container lacks it and
// then running the incremental collector's write barrier for it. Answers nothing.
void SetTableField(EngineContext* ctx, uint32_t state, uint32_t index, uint32_t key) {
  EnterFrame();
  uint32_t resume_result = 0u;
  uint32_t table = 0u;
  uint32_t slot = 0u;
  uint32_t target = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewind: read this entry's five saved words back, then the result word
    // below them.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 20u);
    table = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, table);
    index = LoadU32(ctx, table + 4u);
    key = LoadU32(ctx, table + 8u);
    slot = LoadU32(ctx, table + 12u);
    table = LoadU32(ctx, table + 16u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_result = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = ResolveValueSlot(ctx, state, index);
    table = LoadU32(ctx, slot);
    index = LoadU32(ctx, state + 8u);
  }
  bool const at_own_site = (resume_result == 0u);
  bool const past_running = (ResumePhase(ctx) != kResumeRunning);
  if (past_running ? at_own_site : true) {
    resume_result = GetTableFieldByNumber(ctx, state, table, key);
    uint32_t const abandoned = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      // Suspended: publish the zero left for the result, then append
      // this entry's frame behind it.
      StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), abandoned);
      StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
      uint32_t const frame_base = LoadU32(ctx, ReturnFrameSp(ctx));
      StoreU32(ctx, frame_base, state);
      StoreU32(ctx, frame_base + 4u, index);
      StoreU32(ctx, frame_base + 8u, key);
      StoreU32(ctx, frame_base + 12u, slot);
      StoreU32(ctx, frame_base + 16u, table);
      StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 20u);
      LeaveFrame();
      return;
    }
    key = resume_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The source record sits one whole record below the captured top.
    index = index - 16u;
    StoreU64(ctx, key, LoadU64(ctx, index));
    StoreU32(ctx, key + 8u, LoadU32(ctx, index + 8u));
    // The barrier, three gates deep; the live top is re-read because the lookup
    // may have moved the value stack under this entry.
    index = LoadU32(ctx, state + 8u);
    if (static_cast<int32_t>(LoadU32(ctx, index - 8u)) < 4) {
      goto consume_value;
    }
    if ((LoadU8(ctx, LoadU32(ctx, index - 16u) + 5u) & 3u) == 0u) {
      goto consume_value;
    }
    target = LoadU32(ctx, slot);
    if ((LoadU8(ctx, target + 5u) & 4u) == 0u) {
      goto consume_value;
    }
    index = LoadU32(ctx, state + 16u);
    // Clears the marked bit before the target is relinked.
    StoreU8(ctx, target + 5u, LoadU8(ctx, target + 5u) & 251u);
    StoreU32(ctx, target + 24u, LoadU32(ctx, index + 40u));
    StoreU32(ctx, index + 40u, target);
    index = LoadU32(ctx, state + 8u);
  consume_value:
    StoreU32(ctx, state + 8u, index - 16u);
  }
  LeaveFrame();
}

}  // namespace artc
