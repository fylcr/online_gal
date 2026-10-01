#include "util/engine_context.h"

namespace artc {

// Forward declaration for the callee this step drives.
void ReclaimFreeBlock(EngineContext* ctx, uint32_t state, uint32_t block);

namespace {

// The allocator's global bookkeeping words and bucket tables.
constexpr uint32_t kDataD1370 = 0xD1370u;  // in-use bitset of the small size classes
constexpr uint32_t kDataD1374 = 0xD1374u;  // in-use bitset of the tree size classes
constexpr uint32_t kDataD1378 = 0xD1378u;  // payload size of the top block
constexpr uint32_t kDataD137C = 0xD137Cu;  // payload size of the tail block
constexpr uint32_t kDataD1380 = 0xD1380u;  // lowest address the heap hands out
constexpr uint32_t kDataD1384 = 0xD1384u;  // address of the top (wilderness) block
constexpr uint32_t kDataD1388 = 0xD1388u;  // address of the tail block
constexpr uint32_t kDataD14A0 = 0xD14A0u;  // base of the tree size-class bucket table
constexpr uint32_t kDataD1550 = 0xD1550u;  // counter the split remainder threshold derives from
constexpr uint32_t kAllocateTaggedBlockSizeMask = 0xFFFFFFF8u;  // clears a block's three flag bits
constexpr uint32_t kAllocateTaggedBlockClearLowBit = 0xFFFFFFFEu;  // clears "in use" / "tree root"

}  // namespace

// Serves a request of `size` bytes from the free block at `header`.  Reads the
// block's header word for its size and flags, then either splits it in place,
// grows it into the adjacent top or tail block, or merges it with the following
// free block and unlinks that one from the size buckets.  Returns the block the
// caller may use, or 0 when the request cannot be served from here.  The block
// is stamped with its new size and flags before it is handed back.
uint32_t AllocateTaggedBlock(EngineContext* ctx, uint32_t header, uint32_t size) {
  EnterFrame();
  const uint32_t header_word = LoadU32(ctx, header + 4u);
  const uint32_t block_flags = header_word & 3u;
  const uint32_t block_size = header_word & kAllocateTaggedBlockSizeMask;
  const uint32_t next = header + block_size;
  const uint32_t lowest = LoadU32(ctx, kDataD1380);
  if (block_flags == 0u) {
    // Unallocated tail block: it can only be handed out whole, and only when
    // the request is past the 256-byte minimum and the leftover would be too
    // small to be worth keeping.
    if (size < 256u) {
      LeaveFrame();
      return 0u;
    }
    if (block_size >= size + 4u &&
        block_size - size <= (LoadU32(ctx, kDataD1550) << 1u)) {
      LeaveFrame();
      return header;
    }
    LeaveFrame();
    return 0u;
  }
  if (size <= block_size) {
    // Big enough as is: split off the leftover and return the whole block.
    const uint32_t remainder = block_size - size;
    if (remainder >= 16u) {
      StoreU32(ctx, header + 4u, (header_word & 1u) | size | 2u);
      const uint32_t split = header + size;
      StoreU32(ctx, split + 4u, remainder | 3u);
      StoreU32(ctx, next + 4u, LoadU32(ctx, next + 4u) | 1u);
      ReclaimFreeBlock(ctx, split, remainder);
    }
    LeaveFrame();
    return header;
  }
  if (next == LoadU32(ctx, kDataD1388)) {
    // Grow into the tail block: the request takes the block's size plus the
    // tail's, and the tail moves past the split.
    const uint32_t grown = block_size + LoadU32(ctx, kDataD137C);
    if (grown <= size) {
      LeaveFrame();
      return 0u;
    }
    StoreU32(ctx, header + 4u, (header_word & 1u) | size | 2u);
    const uint32_t split = header + size;
    const uint32_t tail_size = grown - size;
    StoreU32(ctx, split + 4u, tail_size | 1u);
    StoreU32(ctx, kDataD137C, tail_size);
    StoreU32(ctx, kDataD1388, split);
    LeaveFrame();
    return header;
  }
  if (next == LoadU32(ctx, kDataD1384)) {
    // Grow into the top (wilderness) block, which stays parked at the tail.
    const uint32_t grown = block_size + LoadU32(ctx, kDataD1378);
    if (grown < size) {
      LeaveFrame();
      return 0u;
    }
    const uint32_t remainder = grown - size;
    uint32_t top_block = 0u;
    uint32_t top_size = 0u;
    if (remainder >= 16u) {
      StoreU32(ctx, header + 4u, (header_word & 1u) | size | 2u);
      const uint32_t split = header + size;
      StoreU32(ctx, split + 4u, remainder | 1u);
      StoreU32(ctx, header + grown, remainder);
      StoreU32(ctx, header + grown + 4u,
               LoadU32(ctx, header + grown + 4u) & kAllocateTaggedBlockClearLowBit);
      top_block = split;
      top_size = remainder;
    } else {
      // No usable remainder: the top block just grows and neither global moves.
      StoreU32(ctx, header + 4u, (header_word & 1u) | grown | 2u);
      const uint32_t end = header + grown;
      StoreU32(ctx, end + 4u, LoadU32(ctx, end + 4u) | 1u);
    }
    StoreU32(ctx, kDataD1384, top_block);
    StoreU32(ctx, kDataD1378, top_size);
    LeaveFrame();
    return header;
  }
  // The following block is free and not the top: merge it into this one.
  const uint32_t next_word = LoadU32(ctx, next + 4u);
  if (next_word & 2u) {
    LeaveFrame();
    return 0u;
  }
  const uint32_t merged = (next_word & kAllocateTaggedBlockSizeMask) + block_size;
  if (merged < size) {
    LeaveFrame();
    return 0u;
  }
  const uint32_t leftover = merged - size;
  if (next_word <= 255u) {
    // Small block: unlink it from its size-class bucket list.  A block that is
    // its own neighbour was the bucket's last, so the class bit is cleared.
    const uint32_t link_prev = LoadU32(ctx, next + 8u);
    const uint32_t link_next = LoadU32(ctx, next + 12u);
    if (link_next == link_prev) {
      StoreU32(ctx, kDataD1370,
               LoadU32(ctx, kDataD1370) &
                 I32_ROTL(kAllocateTaggedBlockClearLowBit, next_word >> 3));
    } else {
      StoreU32(ctx, link_prev + 12u, link_next);
      StoreU32(ctx, link_next + 8u, link_prev);
    }
  } else {
    // Large block: unlink it from the size tree.
    const uint32_t tree_parent = LoadU32(ctx, next + 24u);
    uint32_t tree_next = LoadU32(ctx, next + 12u);
    if (next != tree_next) {
      uint32_t tree_prev = LoadU32(ctx, next + 8u);
      if (lowest <= tree_prev) {
        // The node's own slot is read here and the value dropped: the links
        // below are rewritten in place.
        (void)LoadU32(ctx, tree_prev + 12u);
      }
      StoreU32(ctx, tree_prev + 12u, tree_next);
      StoreU32(ctx, tree_next + 8u, tree_prev);
    } else {
      // No sibling link: descend to the deepest node below and detach that.
      uint32_t link_slot = next + 20u;
      uint32_t child = LoadU32(ctx, link_slot);
      if (child == 0u) {
        link_slot = next + 16u;
        child = LoadU32(ctx, link_slot);
      }
      if (child == 0u) {
        tree_next = 0u;
      } else {
        uint32_t parent_slot = 0u;
        while (child != 0u) {
          parent_slot = link_slot;
          tree_next = child;
          link_slot = child + 20u;
          child = LoadU32(ctx, link_slot);
          if (child != 0u) continue;
          link_slot = tree_next + 16u;
          child = LoadU32(ctx, tree_next + 16u);
        }
        StoreU32(ctx, parent_slot, 0u);
      }
    }
    if (tree_parent != 0u) {
      // Splice the detached subtree back where the unlinked block sat: the
      // class slot when the block was the tree's root, otherwise the parent's
      // own child slot.
      const uint32_t tree_class = LoadU32(ctx, next + 28u);
      const uint32_t tree_link = (tree_class << 2) + kDataD14A0;
      bool relink = tree_next != 0u;
      if (next == LoadU32(ctx, tree_link)) {
        StoreU32(ctx, tree_link, tree_next);
        if (tree_next == 0u) {
          StoreU32(ctx, kDataD1374,
                   LoadU32(ctx, kDataD1374) &
                     I32_ROTL(kAllocateTaggedBlockClearLowBit, tree_class));
          relink = false;
        }
      } else {
        // The parent keeps its left slot when the block was its left child.
        StoreU32(ctx, tree_parent +
                        (next == LoadU32(ctx, tree_parent + 16u) ? 16u : 20u),
                 tree_next);
      }
      if (relink) {
        StoreU32(ctx, tree_next + 24u, tree_parent);
        const uint32_t left = LoadU32(ctx, next + 16u);
        if (left != 0u) {
          StoreU32(ctx, tree_next + 16u, left);
          StoreU32(ctx, left + 24u, tree_next);
        }
        const uint32_t right = LoadU32(ctx, next + 20u);
        if (right != 0u) {
          StoreU32(ctx, tree_next + 20u, right);
          StoreU32(ctx, right + 24u, tree_next);
        }
      }
    }
  }
  if (leftover <= 15u) {
    // Too little to be worth a block of its own: keep it in the allocation.
    StoreU32(ctx, header + 4u, merged | (header_word & 1u) | 2u);
    const uint32_t end = header + merged;
    StoreU32(ctx, end + 4u, LoadU32(ctx, end + 4u) | 1u);
  } else {
    StoreU32(ctx, header + 4u, (header_word & 1u) | size | 2u);
    const uint32_t split = header + size;
    StoreU32(ctx, split + 4u, leftover | 3u);
    const uint32_t end = header + merged;
    StoreU32(ctx, end + 4u, LoadU32(ctx, end + 4u) | 1u);
    ReclaimFreeBlock(ctx, split, leftover);
  }
  LeaveFrame();
  return header;
}

}  // namespace artc
