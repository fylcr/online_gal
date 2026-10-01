#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);

namespace {

constexpr uint32_t kInitNodeSaveFrameBytes = 28u;  // saved state words below the resume code
constexpr uint32_t kInitNodeResumeWordBytes = 4u;  // resume code slot

}  // namespace

// Initialises the node storage: the live node slots are walked through the
// chunk table and handed to the storage callback, then the chain is freed down
// to its two spare blocks and the slot count is republished.  Suspendable: the
// callback can suspend, so every value that outlives it is kept in the frame.
void InitNodeStorage(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t value = 0u;
  uint32_t called = 0u;
  uint32_t slot = 0u;
  uint32_t chunk = 0u;
  uint32_t nodes = 0u;
  uint32_t last_slot = 0u;
  uint32_t table_end = 0u;
  uint32_t slot_ref = 0u;
  uint32_t resume_point = 0u;
  uint32_t index = 0u;
  uint32_t chunk_table = 0u;
  uint32_t base = 0u;
  uint32_t chain = 0u;
  uint32_t new_nodes = 0u;
  uint32_t table_empty = 0u;
  uint32_t chunk_index = 0u;
  uint32_t left = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 28-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kInitNodeSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    a = LoadU32(ctx, frame);
    value = LoadU32(ctx, frame + 4u);
    slot = LoadU32(ctx, frame + 8u);
    chunk = LoadU32(ctx, frame + 12u);
    last_slot = LoadU32(ctx, frame + 16u);
    table_end = LoadU32(ctx, frame + 20u);
    slot_ref = LoadU32(ctx, frame + 24u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame that saves the resume code.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kInitNodeResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    nodes = LoadU32(ctx, a + 16u);
    chunk_index = DIV_U(nodes, 93u);
    chunk_table = LoadU32(ctx, a + 4u);
    table_end = LoadU32(ctx, a + 8u);
    table_empty = chunk_table == table_end;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot_ref = a + 20u;
    if (table_empty != 0u) goto zero_slot;
    // The slot the count names, then one past it: the walk stops at the last.
    chunk = chunk_table + (chunk_index << 2u);
    slot = LoadU32(ctx, chunk) + (nodes - chunk_index * 93u) * 44u;
    nodes = LoadU32(ctx, a + 20u) + nodes;
    index = DIV_U(nodes, 93u);
    base = LoadU32(ctx, chunk_table + (index << 2u));
    last_slot = base + (nodes - index * 93u) * 44u;
    if (last_slot == slot) goto zero_slot;
  }
walk_slots:
  if (ResumePhase(ctx) == kResumeRunning) {
    value = LoadU32(ctx, LoadU32(ctx, slot));
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    called = CallTableT2(ctx, value, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
    value = called;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Step to the next slot, hopping to the next block at the end of this one.
    slot += 44u;
    if (slot - LoadU32(ctx, chunk) == 4092u) {
      slot = LoadU32(ctx, chunk + 4u);
      chunk += 4u;
    }
    if (slot != last_slot) goto walk_slots;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    table_end = LoadU32(ctx, a + 8u);
    chain = LoadU32(ctx, a + 4u);
  }
zero_slot:
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, slot_ref, 0u);
    left = (uint32_t)((int32_t)(table_end - chain) >> 2);
    if (left > 2u) {
    free_chain:
      DeallocateRawBlock(ctx, LoadU32(ctx, chain));
      chain = LoadU32(ctx, a + 4u) + 4u;
      StoreU32(ctx, a + 4u, chain);
      left = (uint32_t)((int32_t)(LoadU32(ctx, a + 8u) - chain) >> 2);
      if (left > 2u) goto free_chain;
    }
    // Two spare blocks stay behind: the ones the next layer starts from.
    new_nodes = 46u;
    switch (left - 1u) {
      case 0: goto set_nodes;
      case 1: goto double_nodes;
      default: goto leave_nodes;
    }
  double_nodes:
    new_nodes = 93u;
  set_nodes:
    StoreU32(ctx, a + 16u, new_nodes);
  leave_nodes:;
  }
  goto exit_path;
suspend_point:
  // Suspend: write the resume code, then push the frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kInitNodeResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, a);
    StoreU32(ctx, frame + 4u, value);
    StoreU32(ctx, frame + 8u, slot);
    StoreU32(ctx, frame + 12u, chunk);
    StoreU32(ctx, frame + 16u, last_slot);
    StoreU32(ctx, frame + 20u, table_end);
    StoreU32(ctx, frame + 24u, slot_ref);
  }
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kInitNodeSaveFrameBytes);
exit_path:
  LeaveFrame();
}

}  // namespace artc
