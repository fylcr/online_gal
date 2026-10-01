#include "util/engine_context.h"

namespace artc {

// Cross-product callees, defined in util/reallocate_block.cpp and core/raise_block_too_big.cpp.
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size);
uint32_t RaiseBlockTooBig(EngineContext* ctx, uint32_t a);

namespace {

constexpr uint32_t kRelocateElementArrayConstFFFFFFF = 0xFFFFFFFu;
constexpr uint32_t kRelocateElementArraySaveFrameBytes = 24u;
constexpr uint32_t kRelocateElementArrayResumeWordBytes = 4u;

}  // namespace

// Moves the element array of `table` to a block with room for `table + 7` records
// and repoints every owner and every record word at the new block. Suspendable --
// each call below replays with the resume code its own frame spilled.
void RelocateElementArray(EngineContext* ctx, uint32_t state, uint32_t table) {
  EnterFrame();
  // The size gate, then its byte size, then the block the relocation answered with.
  uint32_t in_range = 0u;
  uint32_t new_array = 0u;
  uint32_t old_block = 0u;
  uint32_t count = 0u;
  uint32_t old_bytes = 0u;
  uint32_t resume_code = 0u;
  uint32_t frame = 0u;
  uint32_t result = 0u;
  uint32_t phase = 0u;
  // The owner list, the record list and the block they are repointed at.
  uint32_t link = 0u;
  uint32_t cursor = 0u;
  uint32_t new_base = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the saved frame and read its words back in the saved order.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t frame_top = ReturnFrameSp(ctx);
    frame_top = LoadU32(ctx, frame_top) - kRelocateElementArraySaveFrameBytes;
    StoreU32(ctx, sp, frame_top);
    frame = ReturnFrameSp(ctx);
    uint32_t base = LoadU32(ctx, frame);
    state = LoadU32(ctx, base);
    new_array = LoadU32(ctx, base + 8u);
    old_block = LoadU32(ctx, base + 12u);
    count = LoadU32(ctx, base + 16u);
    old_bytes = LoadU32(ctx, base + 20u);
    table = LoadU32(ctx, base + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word that names the call that was in flight.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t word_top = ReturnFrameSp(ctx);
    word_top = LoadU32(ctx, word_top) - kRelocateElementArrayResumeWordBytes;
    StoreU32(ctx, sp, word_top);
    uint32_t word = ReturnFrameSp(ctx);
    word = LoadU32(ctx, word);
    resume_code = LoadU32(ctx, word);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A running pass starts from the count the table asks to make room for.
    count = table + 6u;
    old_block = LoadU32(ctx, state + 32u);
    in_range = static_cast<uint32_t>((table + 7u) <= kRelocateElementArrayConstFFFFFFF);
  }
  uint32_t entered = in_range | static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (entered != 0u) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The count and the array the records live in both move in whole records.
      old_bytes = LoadU32(ctx, state + 44u) << (4u & 31u);
      new_array = count << (4u & 31u);
    }
    phase = ResumePhase(ctx);
    if (phase == kResumeRunning || resume_code == 0u) {
      result = ReallocateBlock(ctx, state, old_block, old_bytes, new_array);
      resume_code = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_here;
      }
      new_array = result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      goto capacity_set;
    }
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_code == 1u) {
    result = RaiseBlockTooBig(ctx, state);
    resume_code = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
    new_array = result;
  }
capacity_set:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The array is in place: record the count it now holds and the offsets the
    // fields, the owners and the records have to move by.
    StoreU32(ctx, state + 44u, count);
    StoreU32(ctx, state + 32u, new_array);
    uint32_t array_end = new_array + (table << (4u & 31u));
    StoreU32(ctx, state + 28u, array_end);
    uint32_t offset = LoadU32(ctx, state + 8u) - old_block;
    StoreU32(ctx, state + 8u, new_array + offset);
    uint32_t owner = LoadU32(ctx, state + 104u);
    if (owner == 0u) { goto owners_done; }
    link = LoadU32(ctx, owner + 8u);
    link = link - old_block + new_array;
    StoreU32(ctx, owner + 8u, link);
    cursor = LoadU32(ctx, owner);
    if (cursor == 0u) { goto owners_done; }
  repoint_owner:;
    new_base = LoadU32(ctx, state + 32u);
    link = LoadU32(ctx, cursor + 8u) - old_block + new_base;
    StoreU32(ctx, cursor + 8u, link);
    cursor = LoadU32(ctx, cursor);
    if (cursor != 0u) { goto repoint_owner; }
  owners_done:;
    uint32_t array_base = LoadU32(ctx, state + 32u);
    uint32_t records = LoadU32(ctx, state + 40u);
    uint32_t chain_end = LoadU32(ctx, state + 20u);
    if (records <= chain_end) {
    repoint_record:;
      // A record's link pair is written back to front, the order the pop reads it.
      link = LoadU32(ctx, records + 8u) - old_block + array_base;
      StoreU32(ctx, records + 8u, link);
      link = LoadU32(ctx, records) - old_block + array_base;
      StoreU32(ctx, records, link);
      link = LoadU32(ctx, records + 4u) - old_block + array_base;
      StoreU32(ctx, records + 4u, link);
      records = records + 24u;
      if (chain_end >= records) { goto repoint_record; }
    }
    uint32_t used = LoadU32(ctx, state + 12u) - old_block;
    StoreU32(ctx, state + 12u, used + array_base);
  }
  LeaveFrame();
  return;
suspend_here:;
  // Push the resume word naming the call that was in flight, then the frame the
  // rewind pops; the words go in the order that pop reads them back.
  uint32_t slot = ReturnFrameSp(ctx);
  slot = LoadU32(ctx, slot);
  StoreU32(ctx, slot, resume_code);
  uint32_t sp = ReturnFrameSp(ctx);
  uint32_t word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + kRelocateElementArrayResumeWordBytes;
  StoreU32(ctx, sp, word_top);
  frame = ReturnFrameSp(ctx);
  frame = LoadU32(ctx, frame);
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, table);
  StoreU32(ctx, frame + 8u, new_array);
  StoreU32(ctx, frame + 12u, old_block);
  StoreU32(ctx, frame + 16u, count);
  StoreU32(ctx, frame + 20u, old_bytes);
  sp = ReturnFrameSp(ctx);
  word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + kRelocateElementArraySaveFrameBytes;
  StoreU32(ctx, sp, word_top);
  LeaveFrame();
}

}  // namespace artc
