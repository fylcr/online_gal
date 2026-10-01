#include "util/engine_context.h"

namespace artc {

// Cross-product callee from the script host.
void ThrowScriptError(EngineContext* ctx, uint32_t a0, uint32_t a1);

namespace {

constexpr uint32_t kReallocateBlockResumeWordBytes = 4u;

}  // namespace

// Reallocates the payload of a guest block: a running pass hands the block to
// the allocator entry the runtime holds, settles the accounted heap total by the
// size delta and answers the block the caller keeps. A grow that came back with
// no block raises instead, and a zero new size frees. Answers zero on the rewind.
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size) {
  EnterFrame();
  uint32_t frame = 0u;
  uint32_t entry = 0u;
  uint32_t runtime = 0u;
  uint32_t table_index = 0u;
  uint32_t resume_code = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the saved frame and read its words back in store order.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t frame_top = ReturnFrameSp(ctx);
    frame_top = LoadU32(ctx, frame_top) - 28u;
    StoreU32(ctx, sp, frame_top);
    frame = ReturnFrameSp(ctx);
    frame = LoadU32(ctx, frame);
    state = LoadU32(ctx, frame);
    old_size = LoadU32(ctx, frame + 8u);
    new_size = LoadU32(ctx, frame + 12u);
    entry = LoadU32(ctx, frame + 16u);
    runtime = LoadU32(ctx, frame + 20u);
    table_index = LoadU32(ctx, frame + 24u);
    block = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word that names the call that was in flight.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t word_top = ReturnFrameSp(ctx);
    word_top = LoadU32(ctx, word_top) - kReallocateBlockResumeWordBytes;
    StoreU32(ctx, sp, word_top);
    uint32_t code_slot = ReturnFrameSp(ctx);
    uint32_t word_addr = LoadU32(ctx, code_slot);
    resume_code = LoadU32(ctx, word_addr);
  }
  uint32_t phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    // A running pass refreshes the allocator entry and its table slot.
    runtime = LoadU32(ctx, state + 16u);
    entry = LoadU32(ctx, runtime + 16u);
    table_index = LoadU32(ctx, runtime + 12u);
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_code == 0u) {
    uint32_t result = CallTableT7(ctx, table_index, entry, block, old_size, new_size);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
    block = result;
  }
  // The same word carries the table entry while a call is in flight and the
  // no-growth flag once the call has come back.
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    entry = new_size == 0u;
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    if (entry != 0u) { goto allocation_ok; }
    if (block != 0u) { goto allocation_ok; }
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_code == kResumeSuspended) {
    // A grow that came back with no block is an error the script must see.
    ThrowScriptError(ctx, state, 4u);
    resume_code = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
  }
allocation_ok:;
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    // Settle the heap total by the size delta and hand the block back.
    uint32_t live = LoadU32(ctx, runtime + 68u);
    live += new_size - old_size;
    StoreU32(ctx, runtime + 68u, live);
    LeaveFrame();
    return block;
  }
  TrapUnreachable();
suspend_here:;
  // Push the resume word naming the call that was in flight, then the frame the
  // rewind pops; the words go in the order that pop reads them back.
  uint32_t slot = ReturnFrameSp(ctx);
  slot = LoadU32(ctx, slot);
  StoreU32(ctx, slot, resume_code);
  uint32_t sp = ReturnFrameSp(ctx);
  uint32_t frame_top = ReturnFrameSp(ctx);
  frame_top = LoadU32(ctx, frame_top) + kReallocateBlockResumeWordBytes;
  StoreU32(ctx, sp, frame_top);
  frame = ReturnFrameSp(ctx);
  frame = LoadU32(ctx, frame);
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, block);
  StoreU32(ctx, frame + 8u, old_size);
  StoreU32(ctx, frame + 12u, new_size);
  StoreU32(ctx, frame + 16u, entry);
  StoreU32(ctx, frame + 20u, runtime);
  StoreU32(ctx, frame + 24u, table_index);
  sp = ReturnFrameSp(ctx);
  frame_top = ReturnFrameSp(ctx);
  frame_top = LoadU32(ctx, frame_top) + 28u;
  StoreU32(ctx, sp, frame_top);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
