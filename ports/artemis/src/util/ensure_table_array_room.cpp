#include "util/engine_context.h"

namespace artc {

// Grows one function's array part and answers with the block it moved to.
uint32_t CloseFunctionScope(EngineContext* ctx, uint32_t state);

namespace {
// The largest array step the percentage field may ask for, the save frame this
// body pops on a rewind, and the resume word pushed with that frame.
constexpr uint32_t kEnsureTableArrayRoomLargestStep = 0x7FFFFFFEu;
constexpr uint32_t kEnsureTableArrayRoomSaveFrameBytes = 20u;
constexpr uint32_t kEnsureTableArrayRoomResumeWordBytes = 4u;
}  // namespace

// Decides how much room the array part of the table in `state` gets next. The
// running pass folds the room handed back by the last closing pass into the
// table's credit and derives a fresh credit from the table's percentage field;
// the loop below then spends that credit one block at a time.
void EnsureTableArrayRoom(EngineContext* ctx, uint32_t state) {
  EnterFrame();
  // The table record, the stack pointer the frame comes off, and the value found
  // at that pointer.
  uint32_t table = 0u;
  uint32_t sp = 0u;
  uint32_t top = 0u;
  // What a closing pass answered with.
  uint32_t answer = 0u;
  // The credit still to spend, and the room the last closing pass added.
  uint32_t credit = 0u;
  uint32_t added = 0u;
  // The frame's fifth word: a difference the running pass never reads, and then
  // the table's own spend flag.
  uint32_t flag = 0u;
  // The resume code read back off the stack, and the base of the save frame.
  uint32_t resume_code = 0u;
  uint32_t frame = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the save frame and read its words back in the order they
    // were written.
    sp = ReturnFrameSp(ctx);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top) - kEnsureTableArrayRoomSaveFrameBytes;
    StoreU32(ctx, sp, top);
    sp = ReturnFrameSp(ctx);
    frame = LoadU32(ctx, sp);
    state = LoadU32(ctx, frame);
    table = LoadU32(ctx, frame + 4u);
    credit = LoadU32(ctx, frame + 8u);
    flag = LoadU32(ctx, frame + 16u);
    added = LoadU32(ctx, frame + 12u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word pushed with that frame. It names the call
    // that was in flight, so only that call is issued again.
    sp = ReturnFrameSp(ctx);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top) - kEnsureTableArrayRoomResumeWordBytes;
    StoreU32(ctx, sp, top);
    sp = ReturnFrameSp(ctx);
    top = LoadU32(ctx, sp);
    resume_code = LoadU32(ctx, top);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The running pass takes the table out of the state, adds back the room the
    // last pass handed over, and turns the table's percentage field into the
    // credit the loop below spends.
    table = LoadU32(ctx, state + 16u);
    credit = LoadU32(ctx, table + 76u);
    top = LoadU32(ctx, table + 68u);
    added = LoadU32(ctx, table + 64u);
    credit = (top - added) + credit;
    StoreU32(ctx, table + 76u, credit);
    credit = LoadU32(ctx, table + 84u) * 10u;
    credit = credit != 0u ? credit : kEnsureTableArrayRoomLargestStep;
  }
  // One pass per block of array room: the closer below is the call that may
  // suspend, and the loop stops once the table says it is done or the credit is
  // used up.
  while (true) {
    if (ResumePhase(ctx) == kResumeRunning || resume_code == 0u) {
      answer = CloseFunctionScope(ctx, state);
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto save_frame;
      }
      added = answer;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      flag = LoadU8(ctx, table + 21u);
      if (flag == 0u) {
        break;
      }
      credit -= added;
      if (static_cast<int32_t>(credit) > 0) {
        continue;
      }
    }
    break;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (flag != 0u) {
      // Credit left over: either one more whole page fits, or the page is taken
      // back out of the credit.
      top = LoadU32(ctx, table + 76u);
      if (top <= 1023u) {
        top = LoadU32(ctx, table + 68u);
        top = top + 1024u;
        StoreU32(ctx, table + 64u, top);
      } else {
        top = top - 1024u;
        StoreU32(ctx, table + 76u, top);
        top = LoadU32(ctx, table + 68u);
        StoreU32(ctx, table + 64u, top);
      }
    } else {
      // No credit at all: the array gets the share the percentage field names.
      added = LoadU32(ctx, table + 80u);
      top = LoadU32(ctx, table + 72u);
      top = DIV_U(top, 100u);
      added = added * top;
      StoreU32(ctx, table + 64u, added);
    }
  }
  LeaveFrame();
  return;
save_frame:;
  // Push the word naming the call in flight, then the save frame itself; the
  // words go out in the order a rewind reads them back.
  frame = ReturnFrameSp(ctx);
  top = LoadU32(ctx, frame);
  StoreU32(ctx, top, 0u);
  sp = ReturnFrameSp(ctx);
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top) + kEnsureTableArrayRoomResumeWordBytes;
  StoreU32(ctx, sp, top);
  sp = ReturnFrameSp(ctx);
  frame = LoadU32(ctx, sp);
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, table);
  StoreU32(ctx, frame + 8u, credit);
  StoreU32(ctx, frame + 12u, added);
  StoreU32(ctx, frame + 16u, flag);
  sp = ReturnFrameSp(ctx);
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top) + kEnsureTableArrayRoomSaveFrameBytes;
  StoreU32(ctx, sp, top);
  LeaveFrame();
}

}  // namespace artc
