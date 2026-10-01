#include "util/engine_context.h"

namespace artc {

void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void Panic(EngineContext* ctx, uint32_t a0);

namespace {

constexpr uint32_t kReleaseAssertedNodeTreeConstD015C = 0xD015Cu;
constexpr uint32_t kReleaseAssertedNodeTreeConstFFFFFFFF = 0xFFFFFFFFu;
constexpr uint32_t kReleaseAssertedNodeTreeSaveFrameBytes = 12u;
constexpr uint32_t kReleaseAssertedNodeTreeResumeWordBytes = 4u;

}  // namespace

// Releases a node tree along the assertion route: the tail payload's assertion
// flag decides whether the tagged tail is freed with the node, and a claimed
// pending slot hands its slot pair back to the host. Answers nothing.
void ReleaseAssertedNodeTree(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t payload = 0u;
  uint32_t resume_code = 0u;
  uint32_t phase = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the frame the callee saved and read its words back.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t frame_top = ReturnFrameSp(ctx);
    frame_top = LoadU32(ctx, frame_top) - kReleaseAssertedNodeTreeSaveFrameBytes;
    StoreU32(ctx, sp, frame_top);
    uint32_t frame = ReturnFrameSp(ctx);
    frame = LoadU32(ctx, frame);
    a = LoadU32(ctx, frame);
    b = LoadU32(ctx, frame + 4u);
    payload = LoadU32(ctx, frame + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word that names the call that was in flight.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t word_top = ReturnFrameSp(ctx);
    word_top = LoadU32(ctx, word_top) - kReleaseAssertedNodeTreeResumeWordBytes;
    StoreU32(ctx, sp, word_top);
    uint32_t word = ReturnFrameSp(ctx);
    word = LoadU32(ctx, word);
    resume_code = LoadU32(ctx, word);
  }
  uint32_t entered = b | static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (entered != 0u) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // A running pass starts from the node's first child link.
      payload = LoadU32(ctx, b);
    }
    phase = ResumePhase(ctx);
    if (phase == kResumeRunning || resume_code == 0u) {
      ReleaseAssertedNodeTree(ctx, a, payload);
      resume_code = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_here;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      payload = LoadU32(ctx, b + 4u);
    }
    phase = ResumePhase(ctx);
    if (phase == kResumeRunning || resume_code == 1u) {
      ReleaseAssertedNodeTree(ctx, a, payload);
      resume_code = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_here;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The tail payload hangs at +32; a keeps its address, and its emptiness
      // is what the walk below tests before it releases anything.
      payload = LoadU32(ctx, b + 32u);
      a = payload;
      payload = static_cast<uint32_t>(payload == 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (payload != 0u) {
        goto walk_done;
      }
      // Drop the tail's count in place and then release the pair it names.
      payload = LoadU32(ctx, a + 4u);
      payload = payload - 1u;
      StoreU32(ctx, a + 4u, payload);
      if (payload != 1u) {
        goto walk_done;
      }
      payload = LoadU32(ctx, a);
      payload = LoadU32(ctx, payload + 8u);
      StoreU32(ctx, kReleaseAssertedNodeTreeConstD015C, 0u);
    }
    phase = ResumePhase(ctx);
    if (phase == kResumeRunning || resume_code == 2u) {
      HostReleaseSlotPair(ctx, payload, a);
      resume_code = 2u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_here;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // A pending slot names one payload; claiming it repeats the same count
      // decrement and pair release as the step before.
      payload = LoadU32(ctx, kReleaseAssertedNodeTreeConstD015C);
      StoreU32(ctx, kReleaseAssertedNodeTreeConstD015C, 0u);
      if (payload == 1u) {
        goto resume_catch;
      }
      payload = LoadU32(ctx, a + 8u);
      payload = payload - 1u;
      StoreU32(ctx, a + 8u, payload);
      if (payload != 1u) {
        goto walk_done;
      }
      payload = LoadU32(ctx, a);
      payload = LoadU32(ctx, payload + 12u);
      StoreU32(ctx, kReleaseAssertedNodeTreeConstD015C, 0u);
    }
    phase = ResumePhase(ctx);
    if (phase == kResumeRunning || resume_code == 3u) {
      HostReleaseSlotPair(ctx, payload, a);
      resume_code = 3u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_here;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      a = LoadU32(ctx, kReleaseAssertedNodeTreeConstD015C);
      StoreU32(ctx, kReleaseAssertedNodeTreeConstD015C, 0u);
      a = static_cast<uint32_t>(a == 1u);
      if (a != 0u) {
        goto resume_catch;
      }
    }
    walk_done:;
    if (ResumePhase(ctx) == kResumeRunning) {
      // Signed compare: a link byte below zero marks a node whose payload block
      // at +16 has to be freed as well, before its own node block goes back.
      uint32_t owns_payload =
          static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, b + 27u)) <=
                                static_cast<int32_t>(kReleaseAssertedNodeTreeConstFFFFFFFF));
      a = owns_payload;
      if (owns_payload != 0u) {
        a = LoadU32(ctx, b + 16u);
        DeallocateRawBlock(ctx, a);
      }
      DeallocateRawBlock(ctx, b);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    LeaveFrame();
    return;
  }
resume_catch:;
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    a = FindMatchingCatch(ctx, 0u);
    HostTempRet0(ctx);
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_code == 4u) {
    Panic(ctx, a);
    resume_code = 4u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  LeaveFrame();
  return;
suspend_here:;
  // Push the resume word naming the call that was in flight, then the frame the
  // rewind pops; the words go back in the order that pop reads them.
  uint32_t slot = ReturnFrameSp(ctx);
  slot = LoadU32(ctx, slot);
  StoreU32(ctx, slot, resume_code);
  uint32_t sp = ReturnFrameSp(ctx);
  uint32_t word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + kReleaseAssertedNodeTreeResumeWordBytes;
  StoreU32(ctx, sp, word_top);
  uint32_t base = ReturnFrameSp(ctx);
  base = LoadU32(ctx, base);
  StoreU32(ctx, base, a);
  StoreU32(ctx, base + 4u, b);
  StoreU32(ctx, base + 8u, payload);
  sp = ReturnFrameSp(ctx);
  word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + kReleaseAssertedNodeTreeSaveFrameBytes;
  StoreU32(ctx, sp, word_top);
  LeaveFrame();
}

}  // namespace artc
