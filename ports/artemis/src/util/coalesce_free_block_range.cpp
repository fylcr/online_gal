#include "util/engine_context.h"

namespace artc {

// Walks a pool's free chain for a block of the given size.
uint32_t FindFreeBlockOnChain(EngineContext* ctx, uint32_t pool, uint32_t node, uint32_t size);
// Orders two byte ranges.
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {
// The all-bits word a signed comparison result is held against.
constexpr uint32_t kCoalesceFreeBlockRangeAllBits = 0xFFFFFFFFu;
// The step from the pool head to its neighbouring word.
constexpr uint32_t kCoalesceFreeBlockRangeHeadOffset = 4u;
}  // namespace

// Coalesces a free block into one of its free neighbours. It reads both
// neighbouring size words, holds the larger of the two against the limit in a
// byte range comparison, merges the block into whichever neighbour that
// comparison selects, and otherwise hands the search for a block of the
// requested size on to the free chain walk.
uint32_t CoalesceFreeBlockRange(EngineContext* ctx, uint32_t pool, uint32_t block,
                                uint32_t size, uint32_t node, uint32_t limit) {
  EnterFrame();
  uint32_t top = 0u;
  uint32_t arg = 0u;
  uint32_t third = 0u;
  uint32_t fourth = 0u;
  // The size under inspection, and later a cursor walking a free chain.
  uint32_t cur = 0u;
  // A size byte's sign, then a block pointer, then a comparison result.
  uint32_t flag = 0u;
  // The block's neighbouring link, then the larger of the two sizes.
  uint32_t prev = 0u;
  // The size held against cur.
  uint32_t peer = 0u;
  // The pool's neighbouring word, and later a flag.
  uint32_t head = 0u;
  // A sign-extended size, and later a threshold flag.
  uint32_t hold = 0u;
  // A sign flag, and later a block pointer.
  uint32_t neg = 0u;
  // A scratch address, and later a range comparison's result.
  uint32_t slot = 0u;
  // Set when the two candidate sizes are out of order.
  uint32_t swap = 0u;

  swap = 0u;
  head = pool;
  arg = kCoalesceFreeBlockRangeHeadOffset;
  head = head + arg;
  top = head == block;
  if (top != 0u) {
    goto scan_prev;
  }
  // Sign-extend the size byte of the block and that of the limit, holding each
  // against the other's sign to tell a size from a pointer.
  top = LoadU8(ctx, block + 27u);
  cur = top;
  arg = 24u;
  top = top << (arg & 31);
  arg = 24u;
  top = (uint32_t)((int32_t)top >> (arg & 31));
  arg = 0u;
  flag = (uint32_t)((int32_t)top < (int32_t)arg);
  top = LoadU8(ctx, limit + 11u);
  prev = top;
  arg = 24u;
  top = top << (arg & 31);
  arg = 24u;
  top = (uint32_t)((int32_t)top >> (arg & 31));
  hold = top;
  arg = 0u;
  neg = (uint32_t)((int32_t)top < (int32_t)arg);
  top = LoadU32(ctx, block + 20u);
  arg = cur;
  third = flag;
  top = third ? top : arg;
  cur = top;
  arg = LoadU32(ctx, limit + 4u);
  third = prev;
  fourth = neg;
  arg = fourth ? arg : third;
  peer = arg;
  top = top < arg;
  swap = top;
  top = cur;
  arg = peer;
  third = swap;
  top = third ? top : arg;
  prev = top;
  if (top != 0u) {
    // Compare the limit's range against the block's, over the larger size.
    top = LoadU32(ctx, limit);
    arg = limit;
    third = neg;
    top = third ? top : arg;
    neg = top;
    arg = block;
    third = 16u;
    arg = arg + third;
    slot = arg;
    arg = LoadU32(ctx, arg);
    third = slot;
    fourth = flag;
    arg = fourth ? arg : third;
    flag = arg;
    third = prev;
    top = CompareBytes(ctx, top, arg, third);
    slot = top;
    if (top == 0u) {
      top = cur;
      arg = peer;
      top = top > arg;
      if (top != 0u) {
        goto scan_prev;
      }
      goto compare_alt;
    }
    top = slot;
    arg = kCoalesceFreeBlockRangeAllBits;
    top = (uint32_t)((int32_t)top > (int32_t)arg);
    if (top != 0u) {
      goto compare_alt;
    }
    goto scan_prev;
  }
  top = cur;
  arg = peer;
  top = top <= arg;
  if (top != 0u) {
    goto merge_entry;
  }
scan_prev:;
  // Walk to the block that links to this one, along the two link words.
  top = LoadU32(ctx, block);
  prev = top;
  node = block;
  top = LoadU32(ctx, pool);
  arg = block;
  top = top == arg;
  if (top != 0u) {
    goto settle_prev;
  }
  if (prev != 0u) {
    cur = prev;
    do {
      node = cur;
      cur = LoadU32(ctx, cur + 4u);
    } while (cur != 0u);
  } else {
    node = block;
    arg = 8u;
    node = node + arg;
    top = LoadU32(ctx, block + 8u);
    top = LoadU32(ctx, top);
    arg = block;
    top = top == arg;
    if (top != 0u) {
      do {
        top = node;
        top = LoadU32(ctx, top);
        cur = top;
        arg = 8u;
        top = top + arg;
        node = top;
        top = LoadU32(ctx, cur + 8u);
        top = LoadU32(ctx, top);
        arg = cur;
        top = top == arg;
      } while (top != 0u);
    }
    top = node;
    top = LoadU32(ctx, top);
    node = top;
  }
  // The same size selection for the block that was found.
  top = LoadU8(ctx, limit + 11u);
  cur = top;
  arg = 24u;
  top = top << (arg & 31);
  arg = 24u;
  top = (uint32_t)((int32_t)top >> (arg & 31));
  arg = 0u;
  flag = (uint32_t)((int32_t)top < (int32_t)arg);
  top = LoadU8(ctx, node + 27u);
  peer = top;
  arg = 24u;
  top = top << (arg & 31);
  arg = 24u;
  top = (uint32_t)((int32_t)top >> (arg & 31));
  arg = 0u;
  head = (uint32_t)((int32_t)top < (int32_t)arg);
  top = LoadU32(ctx, limit + 4u);
  arg = cur;
  third = flag;
  top = third ? top : arg;
  cur = top;
  arg = LoadU32(ctx, node + 20u);
  third = peer;
  fourth = head;
  arg = fourth ? arg : third;
  peer = arg;
  top = top < arg;
  hold = top;
  top = cur;
  arg = peer;
  third = hold;
  top = third ? top : arg;
  hold = top;
  if (top != 0u) {
    // Compare the ranges once more, now against the block the walk reached.
    arg = node;
    third = 16u;
    arg = arg + third;
    neg = arg;
    arg = LoadU32(ctx, arg);
    third = neg;
    fourth = head;
    arg = fourth ? arg : third;
    arg = limit;
    arg = LoadU32(ctx, arg);
    third = limit;
    fourth = flag;
    arg = fourth ? arg : third;
    third = hold;
    top = CompareBytes(ctx, top, arg, third);
    flag = top;
    if (top != 0u) {
      goto prev_result;
    }
  }
  top = cur;
  arg = peer;
  top = top > arg;
  if (top != 0u) {
    goto settle_prev;
  }
  goto search_prev;
prev_result:;
  top = flag;
  arg = kCoalesceFreeBlockRangeAllBits;
  top = (uint32_t)((int32_t)top > (int32_t)arg);
  if (top != 0u) {
    goto search_prev;
  }
settle_prev:;
  // Hand back the merged block, or the neighbour that is now adjacent to it.
  if (prev == 0u) {
    top = size;
    arg = block;
    StoreU32(ctx, top, arg);
    top = block;
    LeaveFrame();
    return top;
  }
  top = size;
  arg = node;
  StoreU32(ctx, top, arg);
  top = node;
  arg = 4u;
  top = top + arg;
  LeaveFrame();
  return top;
search_prev:;
  top = pool;
  arg = size;
  third = limit;
  top = FindFreeBlockOnChain(ctx, top, arg, third);
  LeaveFrame();
  return top;
compare_alt:;
  // The other ordering of the two ranges.
  top = flag;
  arg = neg;
  third = prev;
  top = CompareBytes(ctx, top, arg, third);
  cur = top;
  if (top != 0u) {
    goto alt_result;
  }
merge_entry:;
  top = swap;
  if (top != 0u) {
    goto scan_next;
  }
  goto merge_both;
alt_result:;
  top = cur;
  arg = kCoalesceFreeBlockRangeAllBits;
  top = (uint32_t)((int32_t)top > (int32_t)arg);
  if (top != 0u) {
    goto merge_both;
  }
scan_next:;
  // The second scan, entered when the two sizes came out out of order.
  top = LoadU32(ctx, block + 4u);
  prev = top;
  if (top != 0u) {
    cur = prev;
    do {
      node = cur;
      cur = LoadU32(ctx, cur);
    } while (cur != 0u);
  } else {
    top = LoadU32(ctx, block + 8u);
    node = top;
    top = LoadU32(ctx, top);
    arg = block;
    top = top == arg;
    if (top == 0u) {
      cur = block;
      arg = 8u;
      cur = cur + arg;
      do {
        top = LoadU32(ctx, cur);
        flag = top;
        arg = 8u;
        top = top + arg;
        cur = top;
        top = LoadU32(ctx, flag + 8u);
        node = top;
        top = LoadU32(ctx, top);
        arg = flag;
        top = top != arg;
      } while (top != 0u);
    }
  }
pick_next:;
  top = node;
  arg = head;
  top = top == arg;
  if (top != 0u) {
    goto settle_next;
  }
  top = LoadU8(ctx, node + 27u);
  cur = top;
  arg = 24u;
  top = top << (arg & 31);
  arg = 24u;
  top = (uint32_t)((int32_t)top >> (arg & 31));
  arg = 0u;
  flag = (uint32_t)((int32_t)top < (int32_t)arg);
  top = peer;
  arg = LoadU32(ctx, node + 20u);
  third = cur;
  fourth = flag;
  arg = fourth ? arg : third;
  cur = arg;
  top = top > arg;
  head = top;
  top = cur;
  arg = peer;
  third = head;
  top = third ? top : arg;
  head = top;
  if (top != 0u) {
    top = LoadU32(ctx, limit);
    arg = limit;
    third = hold;
    fourth = 0u;
    third = (uint32_t)((int32_t)third < (int32_t)fourth);
    top = third ? top : arg;
    arg = node;
    third = 16u;
    arg = arg + third;
    hold = arg;
    arg = LoadU32(ctx, arg);
    third = hold;
    fourth = flag;
    arg = fourth ? arg : third;
    third = head;
    top = CompareBytes(ctx, top, arg, third);
    flag = top;
    if (top != 0u) {
      goto next_result;
    }
  }
  top = cur;
  arg = peer;
  top = top > arg;
  if (top != 0u) {
    goto settle_next;
  }
  goto search_next;
next_result:;
  top = flag;
  arg = kCoalesceFreeBlockRangeAllBits;
  top = (uint32_t)((int32_t)top > (int32_t)arg);
  if (top != 0u) {
    goto search_next;
  }
settle_next:;
  if (prev == 0u) {
    top = size;
    arg = block;
    StoreU32(ctx, top, arg);
    top = block;
    arg = 4u;
    top = top + arg;
    LeaveFrame();
    return top;
  }
  top = size;
  arg = node;
  StoreU32(ctx, top, arg);
  top = node;
  LeaveFrame();
  return top;
search_next:;
  top = pool;
  arg = size;
  third = limit;
  top = FindFreeBlockOnChain(ctx, top, arg, third);
  LeaveFrame();
  return top;
merge_both:;
  // The block is not adjacent to either neighbour, so both links are written.
  top = size;
  arg = block;
  StoreU32(ctx, top, arg);
  top = node;
  arg = block;
  StoreU32(ctx, top, arg);
  top = node;
  LeaveFrame();
  return top;
}

}  // namespace artc
