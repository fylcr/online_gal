#include "util/engine_context.h"

namespace artc {

// Defined elsewhere in the tree.
uint32_t InternString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void EnsureTableArrayRoom(EngineContext* ctx, uint32_t a);
uint32_t GetTableFieldByString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

// Guest-frame bytes spilled while this step is suspended.
constexpr uint32_t kBindUpvalueCellFrameBytes = 16u;
// Bytes of the resume word that rides with the spilled frame.
constexpr uint32_t kBindUpvalueCellResumeWordBytes = 4u;

}  // namespace

// Binds the local slot `local` to its upvalue cell: the first time the slot is
// closed over the cell is interned, then the current value is copied across and
// the slot that now holds the shared value is answered.
uint32_t BindUpvalueCell(EngineContext* ctx, uint32_t state, uint32_t local, uint32_t upvalue) {
  EnterFrame();
  uint32_t frame = 0u;
  uint32_t holder = 0u;
  uint32_t resume_code = 0u;
  uint32_t next_resume_code = 0u;
  uint32_t slot = 0u;
  uint32_t sp = 0u;
  uint32_t prev = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    sp = ReturnFrameSp(ctx);
    prev = ReturnFrameSp(ctx);
    prev = LoadU32(ctx, prev);
    StoreU32(ctx, sp, prev - kBindUpvalueCellFrameBytes);
    sp = ReturnFrameSp(ctx);
    frame = LoadU32(ctx, sp);
    state = LoadU32(ctx, frame);
    upvalue = LoadU32(ctx, frame + 8u);
    holder = LoadU32(ctx, frame + 12u);
    local = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    sp = ReturnFrameSp(ctx);
    prev = ReturnFrameSp(ctx);
    prev = LoadU32(ctx, prev);
    StoreU32(ctx, sp, prev - kBindUpvalueCellResumeWordBytes);
    uint32_t word = ReturnFrameSp(ctx);
    word = LoadU32(ctx, word);
    resume_code = LoadU32(ctx, word);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    holder = LoadU32(ctx, state + 52u);
  }
  // Each call below re-runs on re-entry only while the resume code it wrote
  // when it suspended matches the code that was read back.
  if (ResumePhase(ctx) == kResumeRunning || resume_code == 0u) {
    slot = InternString(ctx, holder, local, upvalue);
    next_resume_code = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_frame;
    }
    local = slot;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    uint32_t record = LoadU32(ctx, state + 48u);
    state = LoadU32(ctx, record + 4u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == 1u) {
    slot = GetTableFieldByString(ctx, holder, state, local);
    next_resume_code = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_frame;
    }
    state = slot;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    upvalue = LoadU32(ctx, state + 8u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (upvalue != 0u) {
      goto binding_done;
    }
    StoreU32(ctx, state + 8u, 1u);
    StoreU32(ctx, state, 1u);
    state = LoadU32(ctx, holder + 16u);
    upvalue = LoadU32(ctx, state + 68u);
    if (upvalue < LoadU32(ctx, state + 64u)) {
      goto binding_done;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == 2u) {
    EnsureTableArrayRoom(ctx, holder);
    next_resume_code = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_frame;
    }
  }
binding_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    LeaveFrame();
    return local;
  }
  TrapUnreachable();
suspend_frame:;
  // The call suspended: spill the live locals above the return frame and hand
  // the resume code back to the runner so it can replay this step.
  uint32_t resume_slot = ReturnFrameSp(ctx);
  resume_slot = LoadU32(ctx, resume_slot);
  StoreU32(ctx, resume_slot, next_resume_code);
  sp = ReturnFrameSp(ctx);
  prev = ReturnFrameSp(ctx);
  prev = LoadU32(ctx, prev);
  prev += kBindUpvalueCellResumeWordBytes;
  StoreU32(ctx, sp, prev);
  sp = ReturnFrameSp(ctx);
  frame = LoadU32(ctx, sp);
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, local);
  StoreU32(ctx, frame + 8u, upvalue);
  StoreU32(ctx, frame + 12u, holder);
  sp = ReturnFrameSp(ctx);
  prev = ReturnFrameSp(ctx);
  prev = LoadU32(ctx, prev);
  StoreU32(ctx, sp, prev + kBindUpvalueCellFrameBytes);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
