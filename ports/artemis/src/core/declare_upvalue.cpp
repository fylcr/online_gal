#include "util/engine_context.h"

namespace artc {

// Defined elsewhere in the tree.
void RelocateUpvalueCells(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

namespace {

// Cell index that names no cell.
constexpr uint32_t kDeclareUpvalueNoCell = 0xFFFFFFFFu;
// Bytes of the caller's frame spilled while this step is suspended.
constexpr uint32_t kDeclareUpvalueSaveFrameBytes = 20u;
// Bytes of the resume word that rides with the spilled frame.
constexpr uint32_t kDeclareUpvalueResumeWordBytes = 4u;

}  // namespace

// Declares the local slot `local` as closed over by an upvalue: relocates the
// state's cell list so the slot is represented by a cell, reusing the cell the
// state already holds for the slot when there is one.
void DeclareUpvalue(EngineContext* ctx, uint32_t state, uint32_t local) {
  EnterFrame();
  uint32_t frame = 0u;
  uint32_t cell = 0u;
  uint32_t slot_head = 0u;
  uint32_t flags = 0u;
  uint32_t cell_array = 0u;
  uint32_t array_size = 0u;
  uint32_t resume_code = 0u;
  uint32_t next_resume_code = 0u;
  uint32_t sp = 0u;
  uint32_t prev = 0u;
  uint32_t word = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    sp = ReturnFrameSp(ctx);
    prev = ReturnFrameSp(ctx);
    prev = LoadU32(ctx, prev);
    StoreU32(ctx, sp, prev - kDeclareUpvalueSaveFrameBytes);
    sp = ReturnFrameSp(ctx);
    frame = LoadU32(ctx, sp);
    state = LoadU32(ctx, frame);
    local = LoadU32(ctx, frame + 4u);
    cell = LoadU32(ctx, frame + 8u);
    flags = LoadU32(ctx, frame + 12u);
    slot_head = LoadU32(ctx, frame + 16u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    sp = ReturnFrameSp(ctx);
    prev = ReturnFrameSp(ctx);
    prev = LoadU32(ctx, prev);
    StoreU32(ctx, sp, prev - kDeclareUpvalueResumeWordBytes);
    word = ReturnFrameSp(ctx);
    word = LoadU32(ctx, word);
    resume_code = LoadU32(ctx, word);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Field 24 holds the head slot of the open-cell list; the slot being
    // declared becomes the new head.
    slot_head = LoadU32(ctx, state + 24u);
    StoreU32(ctx, state + 24u, local);
    flags = LoadU8(ctx, state + 56u);
    // Bit 5 of the flags marks a slot that is already closed over.
    cell = flags & 32u;
  }
  if (cell != 0u || ResumePhase(ctx) == kResumeRewinding) {
    // Replay the relocation that closes the slot over the cell it already has.
    if (ResumePhase(ctx) == kResumeRunning || resume_code == 0u) {
      RelocateUpvalueCells(ctx, state, 5u, kDeclareUpvalueNoCell);
      next_resume_code = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_frame;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      LeaveFrame();
      return;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Bit 3 clear means the slot has no cell yet and one must be relocated.
    cell = (flags & 8u) == 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (cell != 0u) {
      goto cell_ready;
    }
    cell = LoadU32(ctx, state + 64u);
    if (cell != 0u) {
      goto cell_ready;
    }
    cell = LoadU32(ctx, state + 60u);
    StoreU32(ctx, state + 64u, cell);
  }
  if (resume_code == 1u || ResumePhase(ctx) == kResumeRunning) {
    RelocateUpvalueCells(ctx, state, 3u, kDeclareUpvalueNoCell);
    next_resume_code = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_frame;
    }
  }
cell_ready:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Bit 2 set means the slot is already handled.
    flags = (flags & 4u) == 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (flags != 0u) {
      goto declare_done;
    }
    cell = LoadU32(ctx, state + 20u);
    cell = LoadU32(ctx, cell + 4u);
    cell = LoadU32(ctx, cell);
    cell = LoadU32(ctx, cell + 16u);
    array_size = LoadU32(ctx, cell + 12u);
    // The slot's block index inside the array, used to find its cell.
    flags = (uint32_t)((int32_t)(local - array_size) >> (2u & 31u)) - 1u;
    cell_array = LoadU32(ctx, cell + 20u);
    if (cell_array == 0u) {
      cell = 0u;
      if (local <= slot_head) {
        goto array_checked;
      }
      if (flags == 0u) {
        goto array_checked;
      }
      goto declare_done;
    }
    cell = LoadU32(ctx, cell_array + (flags << (2u & 31u)));
    if (local <= slot_head) {
      goto array_checked;
    }
    if (flags == 0u) {
      goto array_checked;
    }
    if (cell == LoadU32(ctx, cell_array + (slot_head - array_size) - 4u)) {
      goto declare_done;
    }
  }
array_checked:;
  if (resume_code == 2u || ResumePhase(ctx) == kResumeRunning) {
    RelocateUpvalueCells(ctx, state, 2u, cell);
    next_resume_code = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_frame;
    }
  }
declare_done:;
  LeaveFrame();
  return;
suspend_frame:;
  // The call suspended: spill the live locals above the return frame and hand
  // the resume code back to the runner so it can replay this step.
  resume_code = next_resume_code;
  sp = ReturnFrameSp(ctx);
  sp = LoadU32(ctx, sp);
  StoreU32(ctx, sp, resume_code);
  sp = ReturnFrameSp(ctx);
  prev = ReturnFrameSp(ctx);
  prev = LoadU32(ctx, prev);
  StoreU32(ctx, sp, prev + kDeclareUpvalueResumeWordBytes);
  sp = ReturnFrameSp(ctx);
  frame = LoadU32(ctx, sp);
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, local);
  StoreU32(ctx, frame + 8u, cell);
  StoreU32(ctx, frame + 12u, flags);
  StoreU32(ctx, frame + 16u, slot_head);
  sp = ReturnFrameSp(ctx);
  prev = ReturnFrameSp(ctx);
  prev = LoadU32(ctx, prev);
  StoreU32(ctx, sp, prev + kDeclareUpvalueSaveFrameBytes);
  LeaveFrame();
  return;
}

}  // namespace artc
