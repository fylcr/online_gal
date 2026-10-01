#include "util/engine_context.h"

namespace artc {

// Cross-product callees: the heap free, the byte comparator, the string
// allocator and the intrusive-list unlink.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
void UnlinkListNode(EngineContext* ctx, uint32_t node, uint32_t victim);

namespace {

constexpr uint32_t kInsertTaggedTreeNodeSwapAddr = 0xD015Cu;
constexpr uint32_t kInsertTaggedTreeNodeConstFFFFFFFF = 0xFFFFFFFFu;
constexpr uint32_t kInsertTaggedTreeNodeSaveFrameBytes = 20u;
constexpr uint32_t kInsertTaggedTreeNodeResumeWordBytes = 4u;

}  // namespace

// Allocates one forty-byte tagged node, resolves its key and value through the
// host lookup slot, descends the tree by the stored tag byte and links the new
// node into the sibling chain. Answers nothing.
void InsertTaggedTreeNode(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t frame = 0u;
  uint32_t node = 0u;
  uint32_t cursor = 0u;
  uint32_t entry = 0u;
  uint32_t resume_code = 0u;
  uint32_t key = 0u;
  uint32_t child_slot = 0u;
  uint32_t limit = 0u;
  uint32_t neg = 0u;
  uint32_t cmp = 0u;
  uint32_t order = 0u;
  uint32_t shift = 0u;
  uint32_t phase = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the saved frame and read its words back in the saved order.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t frame_top = ReturnFrameSp(ctx);
    frame_top = LoadU32(ctx, frame_top) - kInsertTaggedTreeNodeSaveFrameBytes;
    StoreU32(ctx, sp, frame_top);
    frame = ReturnFrameSp(ctx);
    frame = LoadU32(ctx, frame);
    a = LoadU32(ctx, frame);
    cursor = LoadU32(ctx, frame + 8u);
    node = LoadU32(ctx, frame + 12u);
    entry = LoadU32(ctx, frame + 16u);
    b = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word that names the call that was in flight.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t word_top = ReturnFrameSp(ctx);
    word_top = LoadU32(ctx, word_top) - kInsertTaggedTreeNodeResumeWordBytes;
    StoreU32(ctx, sp, word_top);
    uint32_t word = ReturnFrameSp(ctx);
    word = LoadU32(ctx, word);
    resume_code = LoadU32(ctx, word);
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_code == 0u) {
    // Resume code 0: allocate the node itself.
    uint32_t fresh = AllocateStringBuffer(ctx, 40u);
    resume_code = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
    node = fresh;
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    // The key field sits at +16; the swap slot is cleared before the lookup.
    StoreU32(ctx, kInsertTaggedTreeNodeSwapAddr, 0u);
    cursor = node + 16u;
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_code == kResumeSuspended) {
    // Resume code 1: look the entry up under the tree root.
    uint32_t query = HostOsQuery(ctx, 873u, cursor, b);
    resume_code = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
    entry = query;
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    // Take the pending swap word and clear the slot: a word other than one
    // means the lookup above already had an entry to hand back.
    uint32_t saved = LoadU32(ctx, kInsertTaggedTreeNodeSwapAddr);
    StoreU32(ctx, kInsertTaggedTreeNodeSwapAddr, 0u);
    entry = static_cast<uint32_t>(saved != 1u);
  }
  phase = ResumePhase(ctx);
  if (entry != 0u || phase == kResumeRewinding) {
    phase = ResumePhase(ctx);
    if (phase == kResumeRunning) {
      // The node's own value field goes to +28 and the chain starts twelve
      // bytes into the root handle.
      StoreU32(ctx, kInsertTaggedTreeNodeSwapAddr, 0u);
      entry = node + 28u;
      b = b + 12u;
    }
    phase = ResumePhase(ctx);
    if (phase == kResumeRunning || resume_code == kResumeRewinding) {
      // Resume code 2: look the entry up again from the advanced root.
      uint32_t query = HostOsQuery(ctx, 873u, entry, b);
      resume_code = 2u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_here;
      }
      b = query;
    }
    phase = ResumePhase(ctx);
    if (phase == kResumeRunning) {
      // A pending failure word means the lookup found nothing to hand back.
      b = LoadU32(ctx, kInsertTaggedTreeNodeSwapAddr);
      StoreU32(ctx, kInsertTaggedTreeNodeSwapAddr, 0u);
      if (b != 1u) {
        goto commit_here;
      }
      b = HostRet0(ctx);
      (void)HostTempRet0(ctx);
      // A tag byte that still signs non-negative keeps its payload block.
      uint32_t tag_byte = LoadS8(ctx, node + 27u);
      a = static_cast<uint32_t>(static_cast<int32_t>(tag_byte) >
                                static_cast<int32_t>(kInsertTaggedTreeNodeConstFFFFFFFF));
      if (a != 0u) {
        goto teardown_here;
      }
      a = LoadU32(ctx, cursor);
      DeallocateRawBlock(ctx, a);
      goto teardown_here;
    }
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    b = HostRet0(ctx);
    a = HostTempRet0(ctx);
  }
teardown_here:;
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    DeallocateRawBlock(ctx, node);
    HostOsRelease(ctx, b);
    TrapUnreachable();
  }
commit_here:;
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    b = LoadU32(ctx, a + 4u);
    if (b != 0u) {
      // Descend from the root: the tag byte says whether the key sits inline or
      // behind a pointer, then the walk follows the child links down.
      uint32_t key_word = LoadU32(ctx, node + 16u);
      uint32_t tag = LoadU8(ctx, node + 27u);
      shift = 24u;
      uint32_t wide = tag << (shift & 31);
      shift = 24u;
      wide = static_cast<uint32_t>(static_cast<int32_t>(wide) >> (shift & 31));
      neg = static_cast<uint32_t>(static_cast<int32_t>(wide) < 0);
      key = neg != 0u ? key_word : cursor;
      uint32_t value_word = LoadU32(ctx, node + 20u);
      limit = neg != 0u ? value_word : tag;
      cursor = tag;
    descend:;
      tag = LoadU8(ctx, b + 27u);
      cursor = tag;
      shift = 24u;
      wide = tag << (shift & 31);
      shift = 24u;
      wide = static_cast<uint32_t>(static_cast<int32_t>(wide) >> (shift & 31));
      neg = static_cast<uint32_t>(static_cast<int32_t>(wide) < 0);
      value_word = LoadU32(ctx, b + 20u);
      cursor = neg != 0u ? value_word : cursor;
      order = static_cast<uint32_t>(limit > cursor);
      order = order != 0u ? cursor : limit;
      if (order != 0u) {
        // One side of this compare is a pointer and the other is inline.
        child_slot = b + 16u;
        uint32_t child = LoadU32(ctx, child_slot);
        uint32_t other = neg != 0u ? child : child_slot;
        cmp = CompareBytes(ctx, key, other, order);
        if (cmp != 0u) {
          goto after_key_compare;
        }
      }
      if (limit < cursor) {
        goto take_left_child;
      }
      goto take_right_child;
    after_key_compare:;
      if (static_cast<int32_t>(cmp) >
          static_cast<int32_t>(kInsertTaggedTreeNodeConstFFFFFFFF)) {
        goto take_right_child;
      }
    take_left_child:;
      cursor = LoadU32(ctx, b);
      if (cursor == 0u) {
        goto link_under_node;
      }
      goto walk_next;
    take_right_child:;
      cursor = LoadU32(ctx, b + 4u);
      if (cursor != 0u) {
        goto walk_next;
      }
      cursor = b + 4u;
      goto have_link_slot;
    walk_next:;
      b = cursor;
      goto descend;
      TrapUnreachable();
    }
    b = a + 4u;
  link_under_node:;
    cursor = b;
  have_link_slot:;
    StoreU32(ctx, node + 8u, b);
    StoreU64(ctx, node, 0ull);
    StoreU32(ctx, cursor, node);
    uint32_t tree = LoadU32(ctx, a);
    b = LoadU32(ctx, tree);
    if (b != 0u) {
      StoreU32(ctx, a, b);
      node = LoadU32(ctx, cursor);
    }
    uint32_t victim = LoadU32(ctx, a + 4u);
    UnlinkListNode(ctx, victim, node);
    uint32_t count = LoadU32(ctx, a + 8u);
    count = count + 1u;
    StoreU32(ctx, a + 8u, count);
  }
  LeaveFrame();
  return;
suspend_here:;
  // Store the resume word, then the saved frame words in pop order: the two
  // rewind prologues above read them back in exactly this sequence.
  uint32_t slot = ReturnFrameSp(ctx);
  slot = LoadU32(ctx, slot);
  StoreU32(ctx, slot, resume_code);
  uint32_t sp = ReturnFrameSp(ctx);
  uint32_t word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + kInsertTaggedTreeNodeResumeWordBytes;
  StoreU32(ctx, sp, word_top);
  frame = ReturnFrameSp(ctx);
  frame = LoadU32(ctx, frame);
  StoreU32(ctx, frame, a);
  StoreU32(ctx, frame + 4u, b);
  StoreU32(ctx, frame + 8u, cursor);
  StoreU32(ctx, frame + 12u, node);
  StoreU32(ctx, frame + 16u, entry);
  sp = ReturnFrameSp(ctx);
  word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + kInsertTaggedTreeNodeSaveFrameBytes;
  StoreU32(ctx, sp, word_top);
  LeaveFrame();
}

}  // namespace artc
