#include "util/engine_context.h"

namespace artc {

// Releases a raw block back to the allocator.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
// Hands back a fresh buffer of the requested size.
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
// Raises the error an arena that cannot grow any further ends in.
void RaiseAllocLimitError(EngineContext* ctx, uint32_t a0);
// Fails the assertion that guards the allocator's own bookkeeping.
void RaiseAllocatorAssert(EngineContext* ctx, uint32_t a0);

namespace {
// The text of the "exceeds maximum supported size" error, and the limits the
// arena sizing is held against.
constexpr uint32_t kAllocateGuardedBufferTooBigText = 0x8B974u;
constexpr uint32_t kAllocateGuardedBufferMaxCount = 0xAAAAAAAu;
constexpr uint32_t kAllocateGuardedBufferMaxCapacity = 0x15555555u;
constexpr uint32_t kAllocateGuardedBufferMaxEntries = 0x15555556u;
// The all-bits word a signed byte compare is held against, the words a save
// frame costs, and the word that names the call in flight.
constexpr uint32_t kAllocateGuardedBufferAllBits = 0xFFFFFFFFu;
constexpr uint32_t kAllocateGuardedBufferSaveFrameBytes = 24u;
constexpr uint32_t kAllocateGuardedBufferResumeWordBytes = 4u;
}  // namespace

// Grows an arena so one more entry fits: a fresh block is allocated, the live
// entries are moved to the end of it, the entry at size is written in, and the
// old storage is released. The allocator's limit check runs only once the raw
// allocation has happened.
void AllocateGuardedBuffer(EngineContext* ctx, uint32_t state, uint32_t size) {
  EnterFrame();
  uint32_t top = 0u;
  uint32_t arg = 0u;
  uint32_t third = 0u;
  uint32_t fourth = 0u;
  // The save frame's own word: the byte size asked of the allocator, and later
  // the region that comes back.
  uint32_t hold = 0u;
  // The arena's base, and while sizing it carries the entry capacity.
  uint32_t base = 0u;
  // The number of live entries, and later the end of the moved region.
  uint32_t count = 0u;
  // The room the arena is grown to, then the destination the entries are
  // walked down to, and at the end the new base.
  uint32_t room = 0u;
  // Scratch that carries the doubled capacity, the raw allocation and, in the
  // spill, the base of the save frame.
  uint32_t scratch = 0u;
  // The step of the walk that releases the entries the move left behind.
  uint32_t walk = 0u;
  // The call a rewinding pass replays, and the word a suspending pass pushes.
  uint32_t resume_index = 0u;
  uint32_t resume_code = 0u;
  uint64_t word = 0;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the save frame and read its words back.
    top = ReturnFrameSp(ctx);
    arg = ReturnFrameSp(ctx);
    arg = LoadU32(ctx, arg) - kAllocateGuardedBufferSaveFrameBytes;
    StoreU32(ctx, top, arg);
    count = ReturnFrameSp(ctx);
    count = LoadU32(ctx, count);
    state = LoadU32(ctx, count);
    size = LoadU32(ctx, count + 4u);
    base = LoadU32(ctx, count + 8u);
    hold = LoadU32(ctx, count + 12u);
    room = LoadU32(ctx, count + 16u);
    count = LoadU32(ctx, count + 20u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word, which names the call in flight.
    top = ReturnFrameSp(ctx);
    arg = ReturnFrameSp(ctx);
    arg = LoadU32(ctx, arg) - kAllocateGuardedBufferResumeWordBytes;
    StoreU32(ctx, top, arg);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top);
    resume_index = LoadU32(ctx, top);
  }
  top = ResumePhase(ctx);
  if (top != 0u) {
    top = scratch;
  } else {
    // Count the live entries and test whether one more still fits.
    top = LoadU32(ctx, state + 4u);
    arg = LoadU32(ctx, state);
    base = arg;
    top = top - arg;
    arg = 12u;
    top = I32_DIV_S(top, arg);
    count = top;
    arg = 1u;
    top = top + arg;
    room = top;
    arg = kAllocateGuardedBufferMaxEntries;
    top = top < arg;
  }
  arg = ResumePhase(ctx) == kResumeRewinding;
  top = top | arg;
  if (top != 0u) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // Grow the capacity to at least twice what the entries need.
      top = room;
      arg = LoadU32(ctx, state + 8u);
      third = base;
      arg = arg - third;
      third = 12u;
      arg = I32_DIV_S(arg, third);
      base = arg;
      third = 1u;
      arg = arg << (third & 31);
      scratch = arg;
      third = room;
      fourth = scratch;
      third = third > fourth;
      top = third ? top : arg;
      room = top;
      arg = kAllocateGuardedBufferMaxCapacity;
      third = base;
      fourth = kAllocateGuardedBufferMaxCount;
      third = third < fourth;
      top = third ? top : arg;
      base = top;
    }
    top = base;
    arg = ResumePhase(ctx) == kResumeRewinding;
    top = top | arg;
    if (top != 0u) {
      if (ResumePhase(ctx) == kResumeRunning) {
        // A capacity past the last safe entry count takes the error path.
        top = base;
        arg = kAllocateGuardedBufferMaxEntries;
        top = top >= arg;
        hold = top;
        if (top != 0u) {
          goto limit_raise;
        }
        top = base;
        arg = 12u;
        top = top * arg;
        hold = top;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_index == 0u) {
        // The raw allocation is the call that can suspend.
        top = hold;
        top = AllocateStringBuffer(ctx, top);
        scratch = top;
        resume_code = 0u;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto save_frame;
        }
        top = scratch;
      } else {
        top = hold;
      }
      hold = top;
      if (ResumePhase(ctx) == kResumeRunning) {
        // Write the new entry past the last live one, then move the live
        // entries down into the tail of the new block.
        top = hold;
        arg = count;
        third = 12u;
        arg = arg * third;
        top = top + arg;
        room = top;
        arg = size;
        word = LoadU64(ctx, arg);
        StoreU64(ctx, top, word);
        top = room;
        arg = LoadU32(ctx, size + 8u);
        StoreU32(ctx, top + 8u, arg);
        word = 0ull;
        StoreU64(ctx, size, word);
        StoreU32(ctx, size + 8u, 0u);
        top = hold;
        arg = base;
        third = 12u;
        arg = arg * third;
        top = top + arg;
        hold = top;
        top = room;
        arg = 12u;
        top = top + arg;
        count = top;
        top = LoadU32(ctx, state + 4u);
        size = top;
        arg = LoadU32(ctx, state);
        base = arg;
        top = top == arg;
        if (top != 0u) {
          goto guard_exit;
        }
        do {
          room = room - 12u;
          size = size - 12u;
          word = LoadU64(ctx, size);
          StoreU64(ctx, room, word);
          top = LoadU32(ctx, size + 8u);
          StoreU32(ctx, room + 8u, top);
          word = 0ull;
          StoreU64(ctx, size, word);
          StoreU32(ctx, size + 8u, 0u);
          top = size != base;
        } while (top != 0u);
        StoreU32(ctx, state + 8u, hold);
        top = LoadU32(ctx, state + 4u);
        hold = top;
        StoreU32(ctx, state + 4u, count);
        top = LoadU32(ctx, state);
        base = top;
        StoreU32(ctx, state, room);
        top = hold;
        arg = base;
        top = top == arg;
        if (top != 0u) {
          goto release_done;
        }
        // Release the entries the move left behind.
        do {
          walk = hold - 12u;
          top = hold - 1u;
          top = LoadS8(ctx, top);
          arg = kAllocateGuardedBufferAllBits;
          top = (uint32_t)((int32_t)top <= (int32_t)arg);
          if (top != 0u) {
            top = LoadU32(ctx, walk);
            DeallocateRawBlock(ctx, top);
          }
          hold = walk;
          top = walk != base;
        } while (top != 0u);
        goto release_done;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_index == 1u) {
    // The allocator's bookkeeping assertion.
    RaiseAllocatorAssert(ctx, state);
    resume_code = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto save_frame;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
limit_raise:;
  if (ResumePhase(ctx) == kResumeRunning || resume_index == 2u) {
    // The size the allocator was asked for exceeds what it supports.
    top = kAllocateGuardedBufferTooBigText;
    RaiseAllocLimitError(ctx, top);
    resume_code = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto save_frame;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
guard_exit:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Publish the new base, end and capacity limit.
    StoreU32(ctx, state + 8u, hold);
    StoreU32(ctx, state + 4u, count);
    StoreU32(ctx, state, room);
  }
release_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    if (base != 0u) {
      DeallocateRawBlock(ctx, base);
    }
  }
  LeaveFrame();
  return;
save_frame:;
  // Push the word naming the call in flight, then the save frame; a rewind
  // reads them back in that order.
  scratch = resume_code;
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top);
  StoreU32(ctx, top, scratch);
  top = ReturnFrameSp(ctx);
  arg = ReturnFrameSp(ctx);
  arg = LoadU32(ctx, arg) + kAllocateGuardedBufferResumeWordBytes;
  StoreU32(ctx, top, arg);
  scratch = ReturnFrameSp(ctx);
  scratch = LoadU32(ctx, scratch);
  StoreU32(ctx, scratch, state);
  StoreU32(ctx, scratch + 4u, size);
  StoreU32(ctx, scratch + 8u, base);
  StoreU32(ctx, scratch + 12u, hold);
  StoreU32(ctx, scratch + 16u, room);
  StoreU32(ctx, scratch + 20u, count);
  top = ReturnFrameSp(ctx);
  arg = ReturnFrameSp(ctx);
  arg = LoadU32(ctx, arg) + kAllocateGuardedBufferSaveFrameBytes;
  StoreU32(ctx, top, arg);
  LeaveFrame();
}

}  // namespace artc
