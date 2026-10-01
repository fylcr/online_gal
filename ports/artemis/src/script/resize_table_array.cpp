#include "util/engine_context.h"

namespace artc {

// Callees defined in other files.
uint32_t FillBytes(EngineContext* ctx, uint32_t dst, uint32_t value, uint32_t length);
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size,
                         uint32_t new_size);
uint32_t RaiseBlockTooBig(EngineContext* ctx, uint32_t a);

namespace {

// Most entries a table's array part may hold.
constexpr uint32_t kMaxTableArrayEntries = 0x3FFFFFFFu;

}  // namespace

// Grows the array part of a table to `count` entries: allocates a block for the
// new array, relinks the nodes the old array held into it, then updates the
// table's array pointer and array size. Suspendable -- the block allocator and
// the "array part too big" report can suspend.
void ResizeTableArray(EngineContext* ctx, uint32_t table, uint32_t count) {
  EnterFrame();
  constexpr uint32_t kSaveFrameBytes = 28u;
  constexpr uint32_t kResumeWordBytes = 4u;
  uint32_t resume_point = 0u;
  // The resume word the suspend path publishes.
  uint32_t resume_at = 0u;
  // Byte size of the array part being built; a rewind reads it back.
  uint32_t array_bytes = 0u;
  uint32_t array_base = 0u;
  uint32_t resizable = 0u;
  uint32_t room = 0u;
  uint32_t buckets = 0u;
  uint32_t node_base = 0u;
  uint32_t node_bytes = 0u;
  uint32_t node_count = 0u;
  uint32_t bucket_index = 0u;
  uint32_t bucket_node = 0u;
  uint32_t next_node = 0u;
  uint32_t hash_mask = 0u;
  uint32_t slot = 0u;
  uint32_t result = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the frame the suspend path pushed.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    table = LoadU32(ctx, frame);
    array_bytes = LoadU32(ctx, frame + 8u);
    node_base = LoadU32(ctx, frame + 12u);
    node_bytes = LoadU32(ctx, frame + 16u);
    buckets = LoadU32(ctx, frame + 20u);
    bucket_index = LoadU32(ctx, frame + 24u);
    count = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word pushed with that frame.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    resizable = LoadU8(ctx, LoadU32(ctx, table + 16u) + 21u) != 2u;
  }
  if (resizable || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      room = (count + 1u) <= kMaxTableArrayEntries;
    }
    if (room || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        array_bytes = count << 2;
      }
      // A rewind replays only the allocation its resume word names; a running
      // pass makes it.
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
        result = ReallocateBlock(ctx, table, 0u, 0u, array_bytes);
        resume_at = 0u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend; }
        array_base = result;
      }
      // A running pass never reports the array as too big: only the pass that
      // is replaying that report resumes here.
      if (ResumePhase(ctx) != kResumeRunning && resume_point == 1u) {
        result = RaiseBlockTooBig(ctx, table);
        resume_at = 1u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend; }
        array_base = result;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        buckets = LoadU32(ctx, table + 16u);
        if (static_cast<int32_t>(count) >= 1) {
          FillBytes(ctx, array_base, 0u, count << 2);
        }
        node_count = LoadU32(ctx, buckets + 8u);
        if (static_cast<int32_t>(node_count) >= 1) {
          hash_mask = count - 1u;
          do {
            bucket_node = LoadU32(ctx, LoadU32(ctx, buckets) + (bucket_index << 2));
            while (bucket_node != 0u) {
              next_node = LoadU32(ctx, bucket_node);
              slot = array_base + ((hash_mask & LoadU32(ctx, bucket_node + 8u)) << 2);
              StoreU32(ctx, bucket_node, LoadU32(ctx, slot));
              StoreU32(ctx, slot, bucket_node);
              bucket_node = next_node;
            }
            node_count = LoadU32(ctx, buckets + 8u);
            bucket_index += 1u;
          } while (static_cast<int32_t>(bucket_index) < static_cast<int32_t>(node_count));
        }
        node_base = LoadU32(ctx, buckets);
        node_bytes = node_count << 2;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
        ReallocateBlock(ctx, table, node_base, node_bytes, 0u);
        resume_at = 2u;
        if (ResumePhase(ctx) == kResumeSuspended) { goto suspend; }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, buckets, array_base);
        StoreU32(ctx, buckets + 8u, count);
      }
    }
  }
  LeaveFrame();
  return;
suspend:;
  // Suspend: publish the resume word, then push the frame holding the locals the
  // resumed pass reads back.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_at);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
  uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, saved_frame, table);
  StoreU32(ctx, saved_frame + 4u, count);
  StoreU32(ctx, saved_frame + 8u, array_bytes);
  StoreU32(ctx, saved_frame + 12u, node_base);
  StoreU32(ctx, saved_frame + 16u, node_bytes);
  StoreU32(ctx, saved_frame + 20u, buckets);
  StoreU32(ctx, saved_frame + 24u, bucket_index);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  LeaveFrame();
  return;
}

}  // namespace artc
