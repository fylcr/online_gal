#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size);
uint32_t RaiseBlockTooBig(EngineContext* ctx, uint32_t a);

namespace {

constexpr uint32_t kReserveRecordLimit = 0xAAAAAAAu;  // most records the block may hold
constexpr uint32_t kReserveRecordBytes = 24u;  // bytes per upvalue record
constexpr uint32_t kReserveSaveFrameBytes = 20u;
constexpr uint32_t kReserveResumeWordBytes = 4u;

}  // namespace

// Reserves room for `count` more closure upvalue records: the record block is
// grown when it can still hold them, otherwise the block-too-big error is
// raised.  The closure table lives in the compiler state -- +20 free pointer,
// +36 last record, +40 record block, +48 record count.
//
// Suspendable: either call can suspend, so every value that outlives a suspend
// goes into the frame below, and the resume code (0 or 1) names the call that
// has to be replayed.
void ReserveClosureRecords(EngineContext* ctx, uint32_t state, uint32_t count) {
  EnterFrame();
  uint32_t can_grow = 0u;
  uint32_t block = 0u;
  uint32_t old_block = 0u;
  uint32_t old_span = 0u;
  uint32_t new_span = 0u;
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 20-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kReserveSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    count = LoadU32(ctx, frame + 4u);
    new_span = LoadU32(ctx, frame + 8u);
    old_block = LoadU32(ctx, frame + 12u);
    old_span = LoadU32(ctx, frame + 16u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame that saves the resume code.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kReserveResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    old_block = LoadU32(ctx, state + 40u);
    can_grow = static_cast<uint32_t>(count + 1u <= kReserveRecordLimit);
  }
  if (can_grow != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      old_span = LoadU32(ctx, state + 48u) * kReserveRecordBytes;
      new_span = count * kReserveRecordBytes;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      block = ReallocateBlock(ctx, state, old_block, old_span, new_span);
      resume_point = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    }
    if (ResumePhase(ctx) == kResumeRunning) { goto commit; }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    block = RaiseBlockTooBig(ctx, state);
    resume_point = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
commit:
  // Publish the new block: its base, the record count, the last record, and the
  // free pointer moved by the same whole-record offset the block moved by.
  if (ResumePhase(ctx) == kResumeRunning) {
    uint32_t const free_offset =
        I32_DIV_S(LoadU32(ctx, state + 20u) - old_block, kReserveRecordBytes) * kReserveRecordBytes;
    StoreU32(ctx, state + 48u, count);
    StoreU32(ctx, state + 40u, block);
    StoreU32(ctx, state + 36u, block + count * kReserveRecordBytes - kReserveRecordBytes);
    StoreU32(ctx, state + 20u, block + free_offset);
  }
  goto exit_path;
suspend_point:
  // Suspend: write the resume code, then push the frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kReserveResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, state);
    StoreU32(ctx, frame + 4u, count);
    StoreU32(ctx, frame + 8u, new_span);
    StoreU32(ctx, frame + 12u, old_block);
    StoreU32(ctx, frame + 16u, old_span);
  }
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kReserveSaveFrameBytes);
exit_path:
  LeaveFrame();
}

}  // namespace artc
