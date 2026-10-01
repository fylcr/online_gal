#include "util/engine_context.h"

namespace artc {

// Sweeps the pool's marked node list: the list is unhooked and its bits cleared
// out of the pool's mask, then every live node on it has its finaliser called
// through the table. Returns nothing.
void SweepMarkedNodeList(EngineContext* ctx, uint32_t pool) {
  EnterFrame();
  // The slots the suspend path carries: the sweep cursor, the node whose finaliser
  // is being called, its table slot, the word naming the call that was in flight,
  // and the value handed over on the way out.
  uint32_t cursor = 0u;
  uint32_t node = 0u;
  uint32_t finaliser = 0u;
  uint32_t more = 0u;
  uint32_t resume_word = 0u;
  uint32_t spilled = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 16-byte frame the suspend path pushed and rebuild the
    // pool, the cursor, the node and the finaliser from it.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 16u);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    pool = LoadU32(ctx, saved_frame);
    cursor = LoadU32(ctx, saved_frame + 4u);
    node = LoadU32(ctx, saved_frame + 8u);
    finaliser = LoadU32(ctx, saved_frame + 12u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the second pop names the call that was in flight. The two pops
    // stay separate -- the driver keeps one region at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Unhook the marked list and clear its bits out of the pool's mask.
    uint32_t const marked = LoadU32(ctx, pool + 56u);
    StoreU32(ctx, pool + 56u, 0u);
    StoreU32(ctx, pool + 44u, LoadU32(ctx, pool + 44u) & (marked ^ 0xFFFFFFFFu));
    // The sweep runs over the live node array, one pointer per slot.
    cursor = LoadU32(ctx, pool + 824u);
    more = static_cast<uint32_t>(cursor != LoadU32(ctx, pool + 828u));
  }
  if (more | static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding)) {
    for (;;) {
      if (ResumePhase(ctx) == kResumeRunning) {
        node = LoadU32(ctx, cursor);
        finaliser = LoadU32(ctx, LoadU32(ctx, node) + 140u);
      }
      if (ResumePhase(ctx) ? static_cast<uint32_t>(!resume_word) : 1u) {
        CallTableT1(ctx, finaliser, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        spilled = 0u;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        cursor += 4u;
        more = static_cast<uint32_t>(cursor != LoadU32(ctx, pool + 828u));
        if (more) {
          continue;
        }
      }
      break;
    }
  }
  goto exit_path;
 suspend_point:;
  {
    // The stack pointer lives in the word the frame pointer names, so every push
    // is a read-modify-write of that word.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), spilled);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, saved_frame, pool);
    StoreU32(ctx, saved_frame + 4u, cursor);
    StoreU32(ctx, saved_frame + 8u, node);
    StoreU32(ctx, saved_frame + 12u, finaliser);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 16u);
  }
 exit_path:;
  LeaveFrame();
}

}  // namespace artc
