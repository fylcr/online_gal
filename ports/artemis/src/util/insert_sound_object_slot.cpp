#include "util/engine_context.h"

namespace artc {

// Cross-product callees: the guest heap's block release, the string-buffer
// allocator and the sound-object teardown.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
uint32_t DestroySoundObject(EngineContext* ctx, uint32_t a);

namespace {

constexpr uint32_t kInsertSoundObjectSlotConst90E80 = 0x90E80u;
// The assert texts the bucket lookup hands to the host: the shared "buckets_"
// label, the unordered header it names and the "get_bucket" function name.
constexpr uint32_t kInsertSoundObjectSlotMsgBuckets = 0x90EFCu;
constexpr uint32_t kInsertSoundObjectSlotMsgBoostUnorderedDetail = 0x90F05u;
constexpr uint32_t kInsertSoundObjectSlotMsgGetBucket = 0x90F48u;
constexpr uint32_t kInsertSoundObjectSlotConstD015C = 0xD015Cu;
constexpr uint32_t kInsertSoundObjectSlotConst7FFFFFFF = 0x7FFFFFFFu;
constexpr uint32_t kInsertSoundObjectSlotSaveFrameBytes = 28u;
constexpr uint32_t kInsertSoundObjectSlotResumeWordBytes = 4u;

}  // namespace

// Inserts one sound object into the table's bucket chain: a node that already
// holds the key is handed back, otherwise a fresh node is allocated,
// constructed in place and spliced into its bucket. Answers nothing; the node
// and a "was it new" flag go back through a.
void InsertSoundObjectSlot(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t frame = 0u;
  uint32_t link = 0u;
  uint32_t key = 0u;
  uint32_t bucket_index = 0u;
  uint32_t bucket_slot = 0u;
  uint32_t count = 0u;
  uint32_t resume_code = 0u;
  uint32_t phase = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the saved frame and read its words back in the saved order.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t frame_top = ReturnFrameSp(ctx);
    frame_top = LoadU32(ctx, frame_top) - kInsertSoundObjectSlotSaveFrameBytes;
    StoreU32(ctx, sp, frame_top);
    frame = ReturnFrameSp(ctx);
    frame = LoadU32(ctx, frame);
    a = LoadU32(ctx, frame);
    c = LoadU32(ctx, frame + 8u);
    link = LoadU32(ctx, frame + 12u);
    key = LoadU32(ctx, frame + 16u);
    bucket_index = LoadU32(ctx, frame + 20u);
    count = LoadU32(ctx, frame + 24u);
    b = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word naming the call that was in flight.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t word_top = ReturnFrameSp(ctx);
    word_top = LoadU32(ctx, word_top) - kInsertSoundObjectSlotResumeWordBytes;
    StoreU32(ctx, sp, word_top);
    uint32_t word = ReturnFrameSp(ctx);
    word = LoadU32(ctx, word);
    resume_code = LoadU32(ctx, word);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A running pass re-derives the key, its bucket index and the table size.
    key = LoadU32(ctx, c);
    uint32_t bucket_count = LoadU32(ctx, b + 4u);
    bucket_index = REM_U(key, bucket_count);
    count = LoadU32(ctx, b + 8u);
    c = (count == 0u);  // c doubles as the "the table is still empty" flag
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (c != 0u) {
      goto insert_new_node;
    }
    c = LoadU32(ctx, b + 20u);
    if (c != 0u) {
      // Walk the chain the key hashes into. The word at +4 is tagged: with the
      // top bit set it is a node pointer, otherwise it is the bucket index.
      c = LoadU32(ctx, c + (bucket_index << (2u & 31u)));
      if (c == 0u) {
        goto insert_new_node;
      }
      c = LoadU32(ctx, c);
      if (c == 0u) {
        goto insert_new_node;
      }
      if (key != LoadU32(ctx, c + 8u)) {
        link = LoadU32(ctx, c + 4u);
      test_candidate:;
        if (bucket_index != (link & kInsertSoundObjectSlotConst7FFFFFFF)) {
          goto insert_new_node;
        }
      follow_pointer:;
        c = LoadU32(ctx, c);
        if (c == 0u) {
          goto insert_new_node;
        }
        link = LoadU32(ctx, c + 4u);
        if (static_cast<uint32_t>(static_cast<int32_t>(link) < 0)) {
          goto follow_pointer;
        }
        if (LoadU32(ctx, c + 8u) != key) {
          goto test_candidate;
        }
      }
      // The key is already there: hand the node back with a false flag.
      StoreU32(ctx, a, c);
      StoreU8(ctx, a + 4u, 0u);
      LeaveFrame();
      return;
    }
    goto lookup_no_buckets;
  insert_new_node:;
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_code == 0u) {
    // No node holds the key: allocate one and stage it behind the resume word
    // for this call site.
    uint32_t node = AllocateStringBuffer(ctx, 52u);
    resume_code = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
    c = node;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Construct the node in place: two zeroed chains and their own pointers.
    StoreU64(ctx, c + 32u, 0ull);
    StoreU32(ctx, c + 24u, 0u);
    StoreU64(ctx, c + 16u, 0ull);
    uint32_t type_word = kInsertSoundObjectSlotConst90E80;
    StoreU32(ctx, c + 12u, type_word);
    StoreU32(ctx, c + 8u, key);
    StoreU64(ctx, c, 0ull);
    StoreU64(ctx, c + 44u, 0ull);
    uint32_t payload = c + 32u;
    StoreU32(ctx, c + 28u, payload);
    uint32_t tail = c + 44u;
    StoreU32(ctx, c + 40u, tail);
    StoreU32(ctx, kInsertSoundObjectSlotConstD015C, 0u);
    link = count + 1u;
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_code == 1u) {
    HostOsSchedule(ctx, 2141u, b, link);
    resume_code = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The pending-operation flag tells us whether the call in flight was ours.
    link = LoadU32(ctx, kInsertSoundObjectSlotConstD015C);
    StoreU32(ctx, kInsertSoundObjectSlotConstD015C, 0u);
    link = (link != 1u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (link != 0u) {
      uint32_t total = LoadU32(ctx, b + 4u);
      link = REM_U(key, total);
      uint32_t buckets = LoadU32(ctx, b + 20u);
      if (buckets == 0u) {
        goto insert_no_buckets;
      }
      uint32_t masked = link & kInsertSoundObjectSlotConst7FFFFFFF;
      StoreU32(ctx, c + 4u, masked);
      bucket_slot = buckets + (link << (2u & 31u));
      link = LoadU32(ctx, bucket_slot);
      if (link != 0u) {
        goto splice_before_head;
      }
      link = buckets + (total << (2u & 31u));
      uint32_t head = LoadU32(ctx, link);
      if (head != 0u) {
        // Backwards write: the node past the end of the bucket array names the
        // slot the new node has to be written into.
        uint32_t head_next = LoadU32(ctx, head + 4u);
        uint32_t head_slot = buckets + (head_next << (2u & 31u));
        StoreU32(ctx, head_slot, c);
      }
      StoreU32(ctx, bucket_slot, link);
      head = LoadU32(ctx, link);
      StoreU32(ctx, c, head);
      goto finish_insert;
    }
    // The call in flight was not ours: hand the half-built node to the
    // teardown path below.
    a = HostRet0(ctx);
    HostTempRet0(ctx);
    b = c + 12u;
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_code == 2u) {
    uint32_t cleaned = DestroySoundObject(ctx, b);
    resume_code = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
    b = cleaned;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    DeallocateRawBlock(ctx, c);
    HostOsRelease(ctx, a);
    TrapUnreachable();
  }
splice_before_head:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // c takes the head's place: its first word becomes the rest of the chain.
    uint32_t rest = LoadU32(ctx, link);
    StoreU32(ctx, c, rest);
    link = LoadU32(ctx, bucket_slot);
  }
finish_insert:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Link the node into the slot and bump the table's size.
    StoreU32(ctx, link, c);
    uint32_t size = LoadU32(ctx, b + 8u);
    StoreU32(ctx, b + 8u, size + 1u);
    StoreU32(ctx, a, c);
    StoreU8(ctx, a + 4u, 1u);
    LeaveFrame();
    return;
  }
insert_no_buckets:;
lookup_no_buckets:;
  if (ResumePhase(ctx) == kResumeRunning) {
    HostAssertFail(ctx, kInsertSoundObjectSlotMsgBuckets,
                   kInsertSoundObjectSlotMsgBoostUnorderedDetail, 2956u,
                   kInsertSoundObjectSlotMsgGetBucket);
    TrapUnreachable();
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
  word_top = LoadU32(ctx, word_top) + kInsertSoundObjectSlotResumeWordBytes;
  StoreU32(ctx, sp, word_top);
  frame = ReturnFrameSp(ctx);
  frame = LoadU32(ctx, frame);
  StoreU32(ctx, frame, a);
  StoreU32(ctx, frame + 4u, b);
  StoreU32(ctx, frame + 8u, c);
  StoreU32(ctx, frame + 12u, link);
  StoreU32(ctx, frame + 16u, key);
  StoreU32(ctx, frame + 20u, bucket_index);
  StoreU32(ctx, frame + 24u, count);
  sp = ReturnFrameSp(ctx);
  word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + kInsertSoundObjectSlotSaveFrameBytes;
  StoreU32(ctx, sp, word_top);
  LeaveFrame();
}

}  // namespace artc
