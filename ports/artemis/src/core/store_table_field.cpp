#include "util/engine_context.h"

namespace artc {

// Looks a table field up, creating it when the table does not hold it yet.
void LookupOrCreateTableField(EngineContext* ctx, uint32_t state, uint32_t table, uint32_t key, uint32_t value);

namespace {
// The tag a field carries when it has a slot of its own.
constexpr uint32_t kStoreTableFieldDirectSlotMarker = 0x2BBD0u;
}  // namespace

// Stores a value into a table field. A field tagged for direct access goes
// through the first lookup with a zero value; anything else hands the field's own
// bit to the second. Suspendable, so the state, the table, the key and the live
// flag word travel through the return frame.
void StoreTableField(EngineContext* ctx, uint32_t state, uint32_t table, uint32_t key) {
  EnterFrame();
  // The direct-slot answer the first gate tests, and later the field's own bit
  // mask; both share the one saved word.
  uint32_t field_flag = 0u;
  // The resume point the step is at, and the one the suspend path saves.
  uint32_t resume_point = 0u;
  uint32_t resume_at = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 16-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 16u);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    table = LoadU32(ctx, frame + 4u);
    key = LoadU32(ctx, frame + 8u);
    field_flag = LoadU32(ctx, frame + 12u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed with that frame.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    field_flag = LoadU32(ctx, table + 16u) == kStoreTableFieldDirectSlotMarker;
  }
  if (field_flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    // The direct-slot path stores a zero value, and is replayed only at resume
    // point zero.
    uint32_t issue_lookup = static_cast<uint32_t>(!resume_point);
    issue_lookup = ResumePhase(ctx) != kResumeRunning ? issue_lookup : 1u;
    if (issue_lookup) {
      LookupOrCreateTableField(ctx, state, table, key, 0u);
      if (ResumePhase(ctx) == kResumeSuspended) {
        resume_at = 0u;
        goto spill_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      LeaveFrame();
      return;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // One bit of the field's flags byte, chosen by the byte's low five bits.
    field_flag = 1u << (LoadU8(ctx, table + 7u) & 31);
  }
  // The metamethod path hands the field's own bit over, and is replayed only at
  // resume point one.
  if (resume_point == 1u || ResumePhase(ctx) == kResumeRewinding) {
    LookupOrCreateTableField(ctx, state, table, key, field_flag);
    if (ResumePhase(ctx) == kResumeSuspended) {
      resume_at = 1u;
      goto spill_point;
    }
  }
  LeaveFrame();
  return;
spill_point:;
  // Suspend: push the resume point, then the 16-byte frame holding the state,
  // the table, the key and the live flag word the rewinding pop restores.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_at);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, saved_frame, state);
  StoreU32(ctx, saved_frame + 4u, table);
  StoreU32(ctx, saved_frame + 8u, key);
  StoreU32(ctx, saved_frame + 12u, field_flag);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 16u);
  LeaveFrame();
  return;
}

}  // namespace artc
