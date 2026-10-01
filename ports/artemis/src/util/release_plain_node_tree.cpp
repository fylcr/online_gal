#include "util/engine_context.h"

namespace artc {

// Cross-product callees, defined in other product translation units.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void Panic(EngineContext* ctx, uint32_t a0);

namespace {

constexpr uint32_t kReleasePlainNodeTreeConstD015C = 0xD015Cu;
constexpr uint32_t kReleasePlainNodeTreeConstFFFFFFFF = 0xFFFFFFFFu;
constexpr uint32_t kReleasePlainNodeTreeSaveFrameBytes = 12u;
constexpr uint32_t kReleasePlainNodeTreeResumeWordBytes = 4u;

}  // namespace

// Releases a plain node tree: both child links are released recursively, the
// child records' counts are dropped (the host pair release runs once a count
// reaches one) and the node block itself goes back to the heap. Answers nothing.
void ReleasePlainNodeTree(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t frame = 0u;
  uint32_t slot = 0u;
  uint32_t resume_code = 0u;
  uint32_t phase = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the saved frame and read its words back in the saved order.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t frame_top = ReturnFrameSp(ctx);
    frame_top = LoadU32(ctx, frame_top) - kReleasePlainNodeTreeSaveFrameBytes;
    StoreU32(ctx, sp, frame_top);
    frame = ReturnFrameSp(ctx);
    uint32_t base = LoadU32(ctx, frame);
    a = LoadU32(ctx, base);
    b = LoadU32(ctx, base + 4u);
    slot = LoadU32(ctx, base + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word that names the call that was in flight.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t word_top = ReturnFrameSp(ctx);
    word_top = LoadU32(ctx, word_top) - kReleasePlainNodeTreeResumeWordBytes;
    StoreU32(ctx, sp, word_top);
    uint32_t word = ReturnFrameSp(ctx);
    word = LoadU32(ctx, word);
    resume_code = LoadU32(ctx, word);
  }
  uint32_t entered = b | static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (entered != 0u) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // A running pass starts from the node's first child link.
      slot = LoadU32(ctx, b);
    }
    phase = ResumePhase(ctx);
    if (phase == kResumeRunning || resume_code == 0u) {
      ReleasePlainNodeTree(ctx, a, slot);
      resume_code = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_here;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, b + 4u);
    }
    phase = ResumePhase(ctx);
    if (phase == kResumeRunning || resume_code == 1u) {
      ReleasePlainNodeTree(ctx, a, slot);
      resume_code = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_here;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The node's second child link hangs at +32 and doubles as the flag the
      // count release below tests.
      a = LoadU32(ctx, b + 32u);
      slot = static_cast<uint32_t>(a == 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Drop the first child record's count; at one, its tail goes back through
      // the host pair release and the pending-throw cell is cleared.
      if (slot == 0u) {
        goto free_node;
      }
      uint32_t refs = LoadU32(ctx, a + 4u);
      slot = refs;
      StoreU32(ctx, a + 4u, refs - 1u);
      if (refs != 1u) {
        goto free_node;
      }
      uint32_t tail = LoadU32(ctx, a);
      slot = LoadU32(ctx, tail + 8u);
      StoreU32(ctx, kReleasePlainNodeTreeConstD015C, 0u);
    }
    phase = ResumePhase(ctx);
    if (phase == kResumeRunning || resume_code == 2u) {
      HostReleaseSlotPair(ctx, slot, a);
      resume_code = 2u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_here;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Read and clear the pending-throw cell: a one hands control to the
      // unwind path instead of releasing the second child record.
      slot = LoadU32(ctx, kReleasePlainNodeTreeConstD015C);
      StoreU32(ctx, kReleasePlainNodeTreeConstD015C, 0u);
      if (slot == 1u) {
        goto unwind_exception;
      }
      uint32_t refs = LoadU32(ctx, a + 8u);
      slot = refs;
      StoreU32(ctx, a + 8u, refs - 1u);
      if (refs != 1u) {
        goto free_node;
      }
      uint32_t tail = LoadU32(ctx, a);
      slot = LoadU32(ctx, tail + 12u);
      StoreU32(ctx, kReleasePlainNodeTreeConstD015C, 0u);
    }
    phase = ResumePhase(ctx);
    if (phase == kResumeRunning || resume_code == 3u) {
      HostReleaseSlotPair(ctx, slot, a);
      resume_code = 3u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_here;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      a = LoadU32(ctx, kReleasePlainNodeTreeConstD015C);
      StoreU32(ctx, kReleasePlainNodeTreeConstD015C, 0u);
      a = static_cast<uint32_t>(a == 1u);
      if (a != 0u) {
        goto unwind_exception;
      }
    }
    free_node:;
    if (ResumePhase(ctx) == kResumeRunning) {
      // A node whose link byte signed as negative owns a payload block of its
      // own; every node gives its node block back.
      if (static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, b + 27u)) <=
                                static_cast<int32_t>(kReleasePlainNodeTreeConstFFFFFFFF))) {
        uint32_t payload = LoadU32(ctx, b + 16u);
        a = payload;
        DeallocateRawBlock(ctx, payload);
      }
      DeallocateRawBlock(ctx, b);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    LeaveFrame();
    return;
  }
unwind_exception:;
  if (ResumePhase(ctx) == kResumeRunning) {
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
  // rewind pops; the words go in the order that pop reads them back.
  uint32_t resume_word = ReturnFrameSp(ctx);
  resume_word = LoadU32(ctx, resume_word);
  StoreU32(ctx, resume_word, resume_code);
  uint32_t sp = ReturnFrameSp(ctx);
  uint32_t word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + kReleasePlainNodeTreeResumeWordBytes;
  StoreU32(ctx, sp, word_top);
  uint32_t base = ReturnFrameSp(ctx);
  base = LoadU32(ctx, base);
  StoreU32(ctx, base, a);
  StoreU32(ctx, base + 4u, b);
  StoreU32(ctx, base + 8u, slot);
  sp = ReturnFrameSp(ctx);
  word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + kReleasePlainNodeTreeSaveFrameBytes;
  StoreU32(ctx, sp, word_top);
  LeaveFrame();
}

}  // namespace artc
