#include "util/engine_context.h"

namespace artc {

// Returns the block to the pool it came from.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
// Merges the blocks neighbouring the one at `block` that are already free.
uint32_t CoalesceFreeBlockRange(EngineContext* ctx, uint32_t pool, uint32_t block, uint32_t size,
                                uint32_t node, uint32_t limit);
// Reserves a fresh buffer of the given size.
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
// Takes `victim` out of the list `node` heads.
void UnlinkListNode(EngineContext* ctx, uint32_t node, uint32_t victim);

namespace {
// The lock word the pool keeps, and the "not the answer" value a byte compare
// is measured against.
constexpr uint32_t kAcquireBlockFromFreeListLockWord = 0xD015Cu;
constexpr uint32_t kAcquireBlockFromFreeListAllBits = 0xFFFFFFFFu;
// The words a save frame costs, and the word that names a call in flight.
constexpr uint32_t kAcquireBlockFromFreeListSaveFrameBytes = 24u;
constexpr uint32_t kAcquireBlockFromFreeListResumeWordBytes = 4u;
}  // namespace

// Takes a block off the pool's free list, splitting it when the request is
// smaller than the block found, and merges it with its free neighbours first.
// A request the free list cannot serve falls back on a fresh block; the lock
// word around the pool decides which of the two answers the caller gets.
uint32_t AcquireBlockFromFreeList(EngineContext* ctx, uint32_t pool, uint32_t node, uint32_t size,
                                  uint32_t header) {
  EnterFrame();
  // The value this step hands on: a flag before the fallback, the address of
  // the block's own word after it, and a status word once the host answered.
  uint32_t carry = 0u;
  // The base of the save frame and the stack pointer it comes off.
  uint32_t frame = 0u;
  uint32_t sp = 0u;
  uint32_t top = 0u;
  // The fallback block, and what the host call answered with.
  uint32_t fresh = 0u;
  uint32_t answer = 0u;
  // The sixteen bytes of guest stack the coalescer writes its answer into.
  uint32_t scratch = 0u;
  // The call a rewinding pass re-issues, and the word a suspending pass pushes.
  uint32_t resume_point = 0u;
  uint32_t resume_code = 0u;
  uint64_t zero64 = 0;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the save frame and read its words back in the order they
    // were written.
    sp = ReturnFrameSp(ctx);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top) - kAcquireBlockFromFreeListSaveFrameBytes;
    StoreU32(ctx, sp, top);
    sp = ReturnFrameSp(ctx);
    frame = LoadU32(ctx, sp);
    pool = LoadU32(ctx, frame);
    size = LoadU32(ctx, frame + 8u);
    header = LoadU32(ctx, frame + 12u);
    carry = LoadU32(ctx, frame + 16u);
    scratch = LoadU32(ctx, frame + 20u);
    node = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word. It names the call that was in flight, so
    // only that call is issued again.
    sp = ReturnFrameSp(ctx);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top) - kAcquireBlockFromFreeListResumeWordBytes;
    StoreU32(ctx, sp, top);
    sp = ReturnFrameSp(ctx);
    top = LoadU32(ctx, sp);
    resume_point = LoadU32(ctx, top);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Merge with the free neighbours first; the coalescer also reports through
    // the guest stack how much of the request it could serve.
    scratch = GuestStackSp(ctx) - 16u;
    GuestStackSp(ctx) = scratch;
    size = CoalesceFreeBlockRange(ctx, pool, node, scratch + 12u, scratch + 8u, size);
    node = LoadU32(ctx, size);
    carry = (node == 0u);
  }
  if (carry != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      // No free block fits: reserve a fresh one, which is the step that can
      // suspend. Until it returns, the node keeps its old value.
      fresh = AllocateStringBuffer(ctx, 40u);
      resume_code = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto save_frame;
      }
      node = fresh;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Clear the lock word, and point the carry at the block's own word.
      StoreU32(ctx, kAcquireBlockFromFreeListLockWord, 0u);
      carry = node + 16u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
      // Let the host write the block's word; this is the step that can suspend.
      answer = HostOsQuery(ctx, 873u, carry, header);
      resume_code = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto save_frame;
      }
      carry = answer;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The host's answer decides whether the block was actually handed over.
      carry = LoadU32(ctx, kAcquireBlockFromFreeListLockWord);
      StoreU32(ctx, kAcquireBlockFromFreeListLockWord, 0u);
      carry = (carry != 1u);
    }
    if (carry != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        // Clear the lock word again and aim the next host write at the block's
        // own header, which sits twelve bytes further on.
        StoreU32(ctx, kAcquireBlockFromFreeListLockWord, 0u);
        carry = node + 28u;
        header = header + 12u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
        // The second host write, which can suspend as well; its answer is
        // dropped, since the lock word reports the outcome.
        HostOsQuery(ctx, 873u, carry, header);
        resume_code = 2u;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto save_frame;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        header = LoadU32(ctx, kAcquireBlockFromFreeListLockWord);
        StoreU32(ctx, kAcquireBlockFromFreeListLockWord, 0u);
        if (header != 1u) {
          goto install_block;
        }
        // The pool was handed over: take its word and drop the block again,
        // unless the block carries its size in its own header.
        pool = HostRet0(ctx);
        HostTempRet0(ctx);
        size = (uint32_t)((int32_t)LoadS8(ctx, node + 27u) >
                          (int32_t)kAcquireBlockFromFreeListAllBits);
        if (size != 0u) {
          goto lock_was_free;
        }
        size = LoadU32(ctx, node + 16u);
        DeallocateRawBlock(ctx, size);
        goto lock_was_free;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      pool = HostRet0(ctx);
      size = HostTempRet0(ctx);
    }
  lock_was_free:;
    if (ResumePhase(ctx) == kResumeRunning) {
      // Nothing was handed over: give the block back and let the pool go, then
      // trap, because the request cannot be served.
      DeallocateRawBlock(ctx, node);
      HostOsRelease(ctx, pool);
      TrapUnreachable();
    }
  install_block:;
    if (ResumePhase(ctx) == kResumeRunning) {
      // Link the block into the chain: it keeps the coalescer's size, and the
      // pool's list drops the node that used to stand in for it.
      top = LoadU32(ctx, scratch + 12u);
      StoreU32(ctx, node + 8u, top);
      zero64 = 0ull;
      StoreU64(ctx, node, zero64);
      StoreU32(ctx, size, node);
      carry = node;
      header = LoadU32(ctx, pool);
      header = LoadU32(ctx, header);
      if (header != 0u) {
        StoreU32(ctx, pool, header);
        carry = LoadU32(ctx, size);
      }
      top = LoadU32(ctx, pool + 4u);
      UnlinkListNode(ctx, top, carry);
      top = LoadU32(ctx, pool + 8u) + 1u;
      StoreU32(ctx, pool + 8u, top);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = scratch + 16u;
    LeaveFrame();
    return node;
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
  top = LoadU32(ctx, top) + kAcquireBlockFromFreeListResumeWordBytes;
  StoreU32(ctx, sp, top);
  sp = ReturnFrameSp(ctx);
  frame = LoadU32(ctx, sp);
  StoreU32(ctx, frame, pool);
  StoreU32(ctx, frame + 4u, node);
  StoreU32(ctx, frame + 8u, size);
  StoreU32(ctx, frame + 12u, header);
  StoreU32(ctx, frame + 16u, carry);
  StoreU32(ctx, frame + 20u, scratch);
  sp = ReturnFrameSp(ctx);
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top) + 24u;
  StoreU32(ctx, sp, top);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
