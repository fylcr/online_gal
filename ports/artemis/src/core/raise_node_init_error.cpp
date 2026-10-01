#include "util/engine_context.h"

namespace artc {

namespace {
// The data-segment words the report stamps the failed record with, the slot it
// clears, and the host os-query slot the report goes through.
constexpr uint32_t kRaiseNodeInitErrorWordC3110 = 0xC3110u;
constexpr uint32_t kRaiseNodeInitErrorWordC316C = 0xC316Cu;
constexpr uint32_t kRaiseNodeInitErrorSlotD015C = 0xD015Cu;
constexpr uint32_t kRaiseNodeInitErrorQuerySlot = 4222u;
}  // namespace

// Raises a node-initialisation error: stamps the failed record, tells the host
// through the os-query slot, and answers the record back. Suspendable, so the
// record, the flag word and the query argument travel through the return frame.
uint32_t RaiseNodeInitError(EngineContext* ctx, uint32_t record, uint32_t word) {
  EnterFrame();
  // The word after the record's header, kept live across the suspend.
  uint32_t record_tail = 0u;
  // Zero means the query below never completed, so it is issued again.
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 12-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 12u);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    record = LoadU32(ctx, frame);
    word = LoadU32(ctx, frame + 4u);
    record_tail = LoadU32(ctx, frame + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed with that frame.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Both words land on the record's first slot; the second store is the one
    // that survives.
    StoreU32(ctx, record, kRaiseNodeInitErrorWordC3110);
    StoreU32(ctx, record, kRaiseNodeInitErrorWordC316C);
    StoreU32(ctx, kRaiseNodeInitErrorSlotD015C, 0u);
    record_tail = record + 4u;
  }
  // A running pass always owes the query; a rewinding one owes it only at the
  // resume point it popped above.
  uint32_t issue_query = static_cast<uint32_t>(!resume_point);
  issue_query = ResumePhase(ctx) != kResumeRunning ? issue_query : 1u;
  if (issue_query) {
    HostOsQuery(ctx, kRaiseNodeInitErrorQuerySlot, record_tail, word);
    if (ResumePhase(ctx) == kResumeSuspended) {
      // The query reports zero, which is also the resume point saved below.
      goto spill_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    word = LoadU32(ctx, kRaiseNodeInitErrorSlotD015C);
    StoreU32(ctx, kRaiseNodeInitErrorSlotD015C, 0u);
    if (word != 1u) {
      LeaveFrame();
      return record;
    }
    record = HostRet0(ctx);
    HostTempRet0(ctx);
    HostOsRelease(ctx, record);
    TrapUnreachable();
  }
  TrapUnreachable();
spill_point:;
  // Suspend: push the resume point, then a 12-byte frame holding the record, the
  // flag word and the live argument the rewinding pop restores.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), 0u);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, saved_frame, record);
  StoreU32(ctx, saved_frame + 4u, word);
  StoreU32(ctx, saved_frame + 8u, record_tail);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 12u);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
