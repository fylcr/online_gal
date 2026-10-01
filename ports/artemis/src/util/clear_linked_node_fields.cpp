#include "util/engine_context.h"

namespace artc {

// Releases a raw block back to the allocator.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);

namespace {
// The all-bits word a signed byte compare is held against, the words a save
// frame costs, and the word that names the call in flight.
constexpr uint32_t kClearLinkedNodeFieldsAllBits = 0xFFFFFFFFu;
constexpr uint32_t kClearLinkedNodeFieldsSaveFrameBytes = 20u;
constexpr uint32_t kClearLinkedNodeFieldsResumeWordBytes = 4u;
}  // namespace

// Clears a node's own link fields: both child links are followed and cleared
// in turn, then the tail link is unlinked from the list and the node itself is
// released.
void ClearLinkedNodeFields(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t top = 0u;
  uint32_t arg = 0u;
  // The link the walk follows, and the scratch that carries a call's result
  // and, later, the base of the save frame.
  uint32_t link = 0u;
  uint32_t scratch = 0u;
  // The tail link, and the flag that says the list head already is the tail.
  uint32_t tail = 0u;
  uint32_t same_tail = 0u;
  // The call a rewinding pass replays, and the word a suspending pass pushes.
  uint32_t resume_index = 0u;
  uint32_t resume_code = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the save frame and read its words back.
    top = ReturnFrameSp(ctx);
    arg = ReturnFrameSp(ctx);
    arg = LoadU32(ctx, arg) - kClearLinkedNodeFieldsSaveFrameBytes;
    StoreU32(ctx, top, arg);
    scratch = ReturnFrameSp(ctx);
    scratch = LoadU32(ctx, scratch);
    a = LoadU32(ctx, scratch);
    link = LoadU32(ctx, scratch + 8u);
    tail = LoadU32(ctx, scratch + 12u);
    same_tail = LoadU32(ctx, scratch + 16u);
    b = LoadU32(ctx, scratch + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word, which names the call in flight.
    top = ReturnFrameSp(ctx);
    arg = ReturnFrameSp(ctx);
    arg = LoadU32(ctx, arg) - kClearLinkedNodeFieldsResumeWordBytes;
    StoreU32(ctx, top, arg);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top);
    resume_index = LoadU32(ctx, top);
  }
  top = b;
  arg = ResumePhase(ctx) == kResumeRewinding;
  top = top | arg;
  if (top != 0u) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // A running pass reads the first child link to clear.
      link = LoadU32(ctx, b);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_index == 0u) {
      ClearLinkedNodeFields(ctx, a, link);
      resume_code = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto save_frame;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      link = LoadU32(ctx, b + 4u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_index == 1u) {
      ClearLinkedNodeFields(ctx, a, link);
      resume_code = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto save_frame;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      tail = LoadU32(ctx, b + 28u);
    }
    top = tail;
    arg = ResumePhase(ctx) == kResumeRewinding;
    top = top | arg;
    if (top != 0u) {
      if (ResumePhase(ctx) == kResumeRunning) {
        // Read the head of the list the tail link belongs to.
        arg = b;
        arg = LoadU32(ctx, arg + 32u);
        a = arg;
        top = tail == arg;
        same_tail = top;
        link = tail;
      }
      if (same_tail == 0u || ResumePhase(ctx) == kResumeRewinding) {
        // Walk the list back one node at a time, calling the table entry the
        // node names, until the walk comes round to the tail link again.
        while (true) {
          if (ResumePhase(ctx) == kResumeRunning) {
            a = a - 44u;
            top = LoadU32(ctx, a);
            top = LoadU32(ctx, top);
            link = top;
          }
          if (ResumePhase(ctx) == kResumeRunning || resume_index == 2u) {
            top = CallTableT2(ctx, link, a);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
            SyncMemoryBase(ctx);
#endif
            scratch = top;
            resume_code = 2u;
            if (ResumePhase(ctx) == kResumeSuspended) {
              goto save_frame;
            }
            link = scratch;
          }
          if (ResumePhase(ctx) == kResumeRunning) {
            top = a != tail;
            link = top;
            if (top != 0u) {
              continue;
            }
          }
          break;
        }
      }
      if (ResumePhase(ctx) != kResumeRunning) {
        top = link;
      } else {
        top = LoadU32(ctx, b + 28u);
      }
      link = top;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Unlink the node and release the link the walk ended on.
      StoreU32(ctx, b + 32u, tail);
      DeallocateRawBlock(ctx, link);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      top = LoadS8(ctx, b + 27u);
      arg = kClearLinkedNodeFieldsAllBits;
      top = (uint32_t)((int32_t)top <= (int32_t)arg);
      if (top != 0u) {
        top = LoadU32(ctx, b + 16u);
        DeallocateRawBlock(ctx, top);
      }
      DeallocateRawBlock(ctx, b);
    }
  }
  LeaveFrame();
  return;
save_frame:;
  // Push the word naming the call in flight, then the save frame; a rewind
  // reads them back in that order.
  scratch = resume_code;
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top);
  StoreU32(ctx, top, scratch);
  top = ReturnFrameSp(ctx);
  arg = ReturnFrameSp(ctx);
  arg = LoadU32(ctx, arg) + kClearLinkedNodeFieldsResumeWordBytes;
  StoreU32(ctx, top, arg);
  scratch = ReturnFrameSp(ctx);
  scratch = LoadU32(ctx, scratch);
  StoreU32(ctx, scratch, a);
  StoreU32(ctx, scratch + 4u, b);
  StoreU32(ctx, scratch + 8u, link);
  StoreU32(ctx, scratch + 12u, tail);
  StoreU32(ctx, scratch + 16u, same_tail);
  top = ReturnFrameSp(ctx);
  arg = ReturnFrameSp(ctx);
  arg = LoadU32(ctx, arg) + kClearLinkedNodeFieldsSaveFrameBytes;
  StoreU32(ctx, top, arg);
  LeaveFrame();
}

}  // namespace artc
