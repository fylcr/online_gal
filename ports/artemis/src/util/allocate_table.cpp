#include "util/engine_context.h"

namespace artc {

// Fills a freshly allocated table array.
void InitializeTableArray(EngineContext* ctx, uint32_t func_state, uint32_t table, uint32_t size);
// Allocates a block owned by the same state, which may grow the arena.
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size,
                         uint32_t new_size);
// Reports an allocation whose size the engine cannot address.
uint32_t RaiseBlockTooBig(EngineContext* ctx, uint32_t a);
// Moves an object's header into its new block.
void RelocateObjectHeader(EngineContext* ctx, uint32_t state, uint32_t source, uint32_t tag);

namespace {
// The tag every table object's header carries, and the largest entry count the
// engine still addresses.
constexpr uint32_t kAllocateTableHeaderTag = 0x2BBD0u;
constexpr uint32_t kAllocateTableLargestCount = 0xFFFFFFFu;
// The words a save frame costs, and the word that names a call in flight.
constexpr uint32_t kAllocateTableSaveFrameBytes = 24u;
constexpr uint32_t kAllocateTableResumeWordBytes = 4u;
}  // namespace

// Allocates a table object and its array part. The object is a block from the
// state's own pool, stamped through the object header, and the array grows with
// the entry count; an entry count past what the engine can address is reported
// instead of allocated.
uint32_t AllocateTable(EngineContext* ctx, uint32_t state, uint32_t entries, uint32_t array) {
  EnterFrame();
  // The base of the save frame and the stack pointer it comes off.
  uint32_t frame = 0u;
  uint32_t sp = 0u;
  uint32_t top = 0u;
  // The block each allocation step answers with.
  uint32_t fresh = 0u;
  // The table object, and the node linking it into the state's chain.
  uint32_t block = 0u;
  uint32_t node = 0u;
  // The entry count the array part has been filled up to.
  uint32_t filled = 0u;
  // The array part: whether its size fits, then its byte size, then its storage.
  uint32_t payload = 0u;
  // The call a rewinding pass re-issues, and the word a suspending pass pushes.
  uint32_t resume_point = 0u;
  uint32_t resume_code = 0u;
  uint64_t zero64 = 0;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the save frame and read its words back in the order they
    // were written.
    sp = ReturnFrameSp(ctx);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top) - kAllocateTableSaveFrameBytes;
    StoreU32(ctx, sp, top);
    sp = ReturnFrameSp(ctx);
    frame = LoadU32(ctx, sp);
    state = LoadU32(ctx, frame);
    array = LoadU32(ctx, frame + 8u);
    block = LoadU32(ctx, frame + 12u);
    payload = LoadU32(ctx, frame + 16u);
    node = LoadU32(ctx, frame + 20u);
    entries = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word. It names the call that was in flight, so
    // only that call is issued again.
    sp = ReturnFrameSp(ctx);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top) - kAllocateTableResumeWordBytes;
    StoreU32(ctx, sp, top);
    sp = ReturnFrameSp(ctx);
    top = LoadU32(ctx, sp);
    resume_point = LoadU32(ctx, top);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    // The table object itself, which can suspend while the arena grows. Until
    // it returns, the block keeps its old value.
    fresh = ReallocateBlock(ctx, state, 0u, 0u, 32u);
    resume_code = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto save_frame;
    }
    block = fresh;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Stamp the object: it takes the table tag, a zeroed object header and a
    // count of one, and it joins the state's chain as its node.
    RelocateObjectHeader(ctx, state, block, 5u);
    StoreU32(ctx, block + 28u, 0u);
    zero64 = 0ull;
    StoreU64(ctx, block + 8u, zero64);
    top = kAllocateTableHeaderTag;
    StoreU32(ctx, block + 16u, top);
    top = 255u;
    StoreU16(ctx, block + 6u, top);
    node = block;
    // Four bytes per entry, and the count has to stay inside what the engine
    // addresses.
    payload = entries + 1u <= kAllocateTableLargestCount;
  }
  if (payload != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      payload = entries << (4u & 31);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
      // The array part, which can suspend as well.
      fresh = ReallocateBlock(ctx, state, 0u, 0u, payload);
      resume_code = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto save_frame;
      }
      payload = fresh;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      goto storage_ready;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
    // The count does not fit: report it instead of allocating anything.
    fresh = RaiseBlockTooBig(ctx, state);
    resume_code = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto save_frame;
    }
    payload = fresh;
  }
storage_ready:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Record the array part, then clear every entry that only the size covers;
    // the object's own count is updated once the entries are there.
    StoreU32(ctx, node + 12u, payload);
    filled = LoadU32(ctx, block + 28u);
    if ((int32_t)filled < (int32_t)entries) {
      do {
        StoreU32(ctx, payload + (filled << (4u & 31)) + 8u, 0u);
        filled = filled + 1u;
      } while (filled != entries);
    }
    StoreU32(ctx, block + 28u, entries);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
    // Fill the array part from the caller's own array, which can suspend too.
    InitializeTableArray(ctx, state, block, array);
    resume_code = 3u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto save_frame;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    LeaveFrame();
    return block;
  }
  TrapUnreachable();
save_frame:;
  // Push the word naming the call in flight, then the save frame itself; the
  // words go out in the order a rewind reads them back.
  frame = ReturnFrameSp(ctx);
  top = LoadU32(ctx, frame);
  StoreU32(ctx, top, resume_code);
  sp = ReturnFrameSp(ctx);
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top) + kAllocateTableResumeWordBytes;
  StoreU32(ctx, sp, top);
  sp = ReturnFrameSp(ctx);
  frame = LoadU32(ctx, sp);
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, entries);
  StoreU32(ctx, frame + 8u, array);
  StoreU32(ctx, frame + 12u, block);
  StoreU32(ctx, frame + 16u, payload);
  StoreU32(ctx, frame + 20u, node);
  sp = ReturnFrameSp(ctx);
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top) + 24u;
  StoreU32(ctx, sp, top);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
