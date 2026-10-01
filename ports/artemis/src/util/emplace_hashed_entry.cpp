#include "util/engine_context.h"

namespace artc {

void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);

namespace {
// The buckets_ member named by an assert.
constexpr uint32_t kEmplaceHashedEntryBucketsText = 0xB5B6Cu;
// The boost/unordered header path named by an assert.
constexpr uint32_t kEmplaceHashedEntryHeaderPathText = 0xB5B75u;
// The get_bucket member named by an assert.
constexpr uint32_t kEmplaceHashedEntryGetBucketText = 0xB5BB8u;
// The word a pending result is published in.
constexpr uint32_t kEmplaceHashedEntryStatusAddr = 0xD015Cu;
// The low bits a node's stored index keeps.
constexpr uint32_t kEmplaceHashedEntryIndexMask = 0x7FFFFFFFu;
constexpr uint32_t kEmplaceHashedEntrySaveFrameBytes = 24u;
constexpr uint32_t kEmplaceHashedEntryResumeWordBytes = 4u;
}  // namespace

// Looks a hash up in the bucket chain of a hash table and either reports the
// node that already holds it through the out parameter or links a freshly
// allocated node in, counting the element as inserted.
void EmplaceHashedEntry(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
  EnterFrame();
  uint32_t top = 0u;
  uint32_t arg = 0u;
  uint32_t third = 0u;
  uint32_t fourth = 0u;
  // The frame base, an index, a node pointer, and a staged await code.
  uint32_t hold = 0u;
  // The hash value, and a slot address.
  uint32_t hash = 0u;
  // The bucket index of the hash.
  uint32_t index = 0u;
  // The table's element count.
  uint32_t count = 0u;
  // The await code a suspend was resumed with.
  uint32_t await_code = 0u;
  uint64_t zero = 0;

  if (ResumePhase(ctx) == 2u) {
    top = ReturnFrameSp(ctx);
    arg = ReturnFrameSp(ctx);
    arg = LoadU32(ctx, arg);
    arg = arg - kEmplaceHashedEntrySaveFrameBytes;
    StoreU32(ctx, top, arg);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top);
    hold = top;
    a = LoadU32(ctx, hold);
    c = LoadU32(ctx, hold + 8u);
    d = LoadU32(ctx, hold + 12u);
    hash = LoadU32(ctx, hold + 16u);
    count = LoadU32(ctx, hold + 20u);
    b = LoadU32(ctx, hold + 4u);
  }
  if (ResumePhase(ctx) == 2u) {
    top = ReturnFrameSp(ctx);
    arg = ReturnFrameSp(ctx);
    arg = LoadU32(ctx, arg);
    arg = arg - kEmplaceHashedEntryResumeWordBytes;
    StoreU32(ctx, top, arg);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top);
    await_code = LoadU32(ctx, top);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The bucket of the hash, and whether the table is still empty.
    hash = LoadU32(ctx, c);
    arg = LoadU32(ctx, b + 4u);
    index = REM_U(hash, arg);
    count = LoadU32(ctx, b + 8u);
    c = count == 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (c != 0u) {
      goto lookup_done;
    }
    c = LoadU32(ctx, b + 20u);
    if (c != 0u) {
      // Walk the bucket's chain for a node holding the same hash.
      top = index;
      arg = 2u;
      top = top << (arg & 31);
      arg = c;
      top = top + arg;
      c = LoadU32(ctx, top);
      if (c == 0u) {
        goto lookup_done;
      }
      c = LoadU32(ctx, c);
      if (c == 0u) {
        goto lookup_done;
      }
      top = hash;
      arg = LoadU32(ctx, c + 8u);
      top = top != arg;
      if (top != 0u) {
        hold = LoadU32(ctx, c + 4u);
        while (true) {
          top = index;
          arg = hold;
          third = kEmplaceHashedEntryIndexMask;
          arg = arg & third;
          top = top != arg;
          if (top != 0u) {
            goto lookup_done;
          }
          // Skip the nodes whose stored index has the counted bit set.
          while (true) {
            c = LoadU32(ctx, c);
            if (c == 0u) {
              goto lookup_done;
            }
            hold = LoadU32(ctx, c + 4u);
            if ((int32_t)hold < 0) {
              continue;
            }
            break;
          }
          top = LoadU32(ctx, c + 8u);
          arg = hash;
          top = top != arg;
          if (top != 0u) {
            continue;
          }
          break;
        }
      }
      top = a;
      arg = c;
      StoreU32(ctx, top, arg);
      top = a;
      arg = 0u;
      StoreU8(ctx, top + 4u, arg);
      LeaveFrame();
      return;
    }
    goto resume_path;
lookup_done:;
  }
  if (await_code == 0u || ResumePhase(ctx) == kResumeRunning) {
    top = 12u;
    top = AllocateStringBuffer(ctx, top);
    hold = top;
    top = 0u;
    arg = ResumePhase(ctx);
    arg = arg == kResumeSuspended;
    if (arg != 0u) {
      goto save_frame;
    }
    c = hold;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    zero = 0ull;
    StoreU64(ctx, c, zero);
    arg = LoadU32(ctx, d);
    StoreU32(ctx, c + 8u, arg);
    StoreU32(ctx, kEmplaceHashedEntryStatusAddr, 0u);
    top = count;
    arg = 1u;
    top = top + arg;
    d = top;
  }
  if (await_code == 1u || ResumePhase(ctx) == kResumeRunning) {
    top = 2750u;
    arg = b;
    third = d;
    HostOsSchedule(ctx, top, arg, third);
    top = 1u;
    arg = ResumePhase(ctx);
    arg = arg == kResumeSuspended;
    if (arg != 0u) {
      goto save_frame;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    d = LoadU32(ctx, kEmplaceHashedEntryStatusAddr);
    StoreU32(ctx, kEmplaceHashedEntryStatusAddr, 0u);
    if (d != 1u) {
      // Link the new node into its bucket chain.
      arg = LoadU32(ctx, b + 4u);
      index = arg;
      hold = REM_U(hash, arg);
      d = LoadU32(ctx, b + 20u);
      if (d == 0u) {
        goto skip_insert;
      }
      arg = c;
      third = hold;
      third = third & kEmplaceHashedEntryIndexMask;
      StoreU32(ctx, arg + 4u, third);
      arg = hold;
      third = 2u;
      arg = arg << (third & 31);
      third = d;
      arg = arg + third;
      hash = arg;
      hold = LoadU32(ctx, arg);
      if (hold != 0u) {
        goto insert_tail;
      }
      arg = index;
      third = 2u;
      arg = arg << (third & 31);
      third = d;
      arg = arg + third;
      hold = arg;
      index = LoadU32(ctx, arg);
      if (index != 0u) {
        arg = index;
        arg = LoadU32(ctx, arg + 4u);
        third = 2u;
        arg = arg << (third & 31);
        third = d;
        arg = arg + third;
        third = c;
        StoreU32(ctx, arg, third);
      }
      arg = hash;
      third = hold;
      StoreU32(ctx, arg, third);
      arg = c;
      third = hold;
      third = LoadU32(ctx, third);
      StoreU32(ctx, arg, third);
      goto link_entry;
    }
    arg = HostRet0(ctx);
    a = arg;
    arg = HostTempRet0(ctx);
    arg = c;
    DeallocateRawBlock(ctx, arg);
    arg = a;
    HostOsRelease(ctx, arg);
    TrapUnreachable();
insert_tail:;
    arg = c;
    third = hold;
    third = LoadU32(ctx, third);
    StoreU32(ctx, arg, third);
    arg = hash;
    hold = LoadU32(ctx, arg);
link_entry:;
    arg = hold;
    third = c;
    StoreU32(ctx, arg, third);
    arg = b;
    third = b;
    third = LoadU32(ctx, third + 8u);
    fourth = 1u;
    third = third + fourth;
    StoreU32(ctx, arg + 8u, third);
    arg = a;
    third = c;
    StoreU32(ctx, arg, third);
    arg = a;
    third = 1u;
    StoreU8(ctx, arg + 4u, third);
    LeaveFrame();
    return;
skip_insert:;
  }
resume_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    HostAssertFail(ctx, kEmplaceHashedEntryBucketsText, kEmplaceHashedEntryHeaderPathText, 2956u,
                   kEmplaceHashedEntryGetBucketText);
    TrapUnreachable();
  }
  LeaveFrame();
  return;
save_frame:;
  // Push the word naming the call in flight, then the save frame; a rewind
  // reads them back in that order.
  hold = top;
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top);
  StoreU32(ctx, top, hold);
  top = ReturnFrameSp(ctx);
  arg = ReturnFrameSp(ctx);
  arg = LoadU32(ctx, arg);
  arg = arg + kEmplaceHashedEntryResumeWordBytes;
  StoreU32(ctx, top, arg);
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top);
  hold = top;
  StoreU32(ctx, hold, a);
  StoreU32(ctx, hold + 4u, b);
  StoreU32(ctx, hold + 8u, c);
  StoreU32(ctx, hold + 12u, d);
  StoreU32(ctx, hold + 16u, hash);
  StoreU32(ctx, hold + 20u, count);
  top = ReturnFrameSp(ctx);
  arg = ReturnFrameSp(ctx);
  arg = LoadU32(ctx, arg);
  arg = arg + 24u;
  StoreU32(ctx, top, arg);
  LeaveFrame();
  return;
}

}  // namespace artc
