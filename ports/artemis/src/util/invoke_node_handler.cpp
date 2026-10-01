#include "util/engine_context.h"

namespace artc {

// Calls a node's own handler through the numeric builtin table, handing it the
// node, the scale and the call number. The slot and the table word survive a
// suspend inside the step's frame. Returns nothing.
void InvokeNodeHandler(EngineContext* ctx, uint32_t node, uint32_t slot, float scale) {
  EnterFrame();
  // The reused slots: the handler table word, and the resume word naming the
  // call in flight.
  uint32_t handler_table = 0u;
  uint32_t resume_word = 0u;
  uint32_t spilled = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 16-byte frame the suspend path pushed and rebuild the
    // node, the slot, the scale and the table word from it.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 16u);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    node = LoadU32(ctx, saved_frame);
    slot = LoadU32(ctx, saved_frame + 4u);
    scale = LoadF32(ctx, saved_frame + 8u);
    handler_table = LoadU32(ctx, saved_frame + 12u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the second pop names the call that was in flight. The two pops
    // stay separate -- the driver keeps one region at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The table the slot indexes hangs one indirection off the node.
    handler_table = LoadU32(ctx, LoadU32(ctx, node) + 148u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_word == 0u) : 1u) {
    CallTableT16(ctx, handler_table, node, slot, scale, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    spilled = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
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
    StoreU32(ctx, saved_frame, node);
    StoreU32(ctx, saved_frame + 4u, slot);
    StoreF32(ctx, saved_frame + 8u, scale);
    StoreU32(ctx, saved_frame + 12u, handler_table);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 16u);
  }
 exit_path:;
  LeaveFrame();
}

}  // namespace artc
