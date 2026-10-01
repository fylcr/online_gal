#include "util/engine_context.h"

namespace artc {

namespace {

// The collector step's table index, read only after the raw heap has refused.
constexpr uint32_t kCollectorStepIndexAddr = 856940u;
// The out-of-memory failure leaves as the engine's bad_alloc-shaped exception:
// a four-byte host record stamped with the second type word and thrown with the
// typeinfo and destructor words.
constexpr uint32_t kThrowRecordBytes = 4u;
constexpr uint32_t kThrowTypeWordFirst = 798992u;
constexpr uint32_t kThrowTypeWordSecond = 798956u;
constexpr uint32_t kThrowTypeInfoAddr = 799044u;
constexpr uint32_t kThrowDestructorWord = 2270u;
// The one await site names itself zero, which the resume test reads as a bare zero.
constexpr uint32_t kCollectorStepCall = 0u;

}  // namespace

// Hands out guest-heap bytes: the raw heap is asked first and its pointer
// answers the request, otherwise one collector step is run and the heap asked
// again, and only a heap that refuses with no collector step registered becomes
// the engine's out-of-memory exception. Answers the block, or zero when it
// suspended.
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t bytes) {
  EnterFrame();
  uint32_t slot = 0u;
  uint32_t resume_word = 0u;
  uint32_t gate = 0u;
  uint32_t result = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 8-byte frame the suspend path pushed and rebuild the
    // request size and the slot from it.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 8u);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    bytes = LoadU32(ctx, saved_frame);
    slot = LoadU32(ctx, saved_frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the second pop names the call that was in flight. The two pops
    // stay separate -- one region is read back at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  // A fresh request of zero bytes is served as one byte, and only on a running
  // pass -- a rewinding pass keeps the size its frame restored.
  bytes = ResumePhase(ctx) ? bytes : (bytes ? bytes : 1u);
 retry_alloc:;
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = AllocateRawBlock(ctx, bytes);
    if (slot) {
      goto alloc_done;
    }
    slot = LoadU32(ctx, kCollectorStepIndexAddr);
  }
  // The rewinding bit is orred into the slot rather than tested beside it.
  gate = slot | static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (gate) {
    if (ResumePhase(ctx) ? static_cast<uint32_t>(!resume_word) : 1u) {
      CallCollectorStep(ctx, slot);
      resume_word = kCollectorStepCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The step returned without suspending, so the heap is asked again.
      goto retry_alloc;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running and out of road: no collector step is registered.
    slot = HostAllocateException(ctx, kThrowRecordBytes);
    StoreU32(ctx, slot, kThrowTypeWordFirst);
    StoreU32(ctx, slot, kThrowTypeWordSecond);
    HostThrowException(ctx, slot, kThrowTypeInfoAddr, kThrowDestructorWord);
    TrapUnreachable();
  }
 alloc_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    result = slot;
    goto exit_path;
  }
  TrapUnreachable();
 suspend_point:;
  {
    // The stack pointer lives in the word the frame pointer names, so every push
    // is a read-modify-write of that word.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_word);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, saved_frame, bytes);
    StoreU32(ctx, saved_frame + 4u, slot);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 8u);
  }
  result = 0u;
 exit_path:;
  LeaveFrame();
  return result;
}

}  // namespace artc
