#include "util/engine_context.h"

namespace artc {

// Forward declaration -- the byte comparer.
uint32_t CompareBytes(EngineContext* ctx, uint32_t lhs, uint32_t rhs, uint32_t n);

// Walks the pool's free chain for a block whose key matches the request
// described by `size`. The block found is written through `node`, and the
// address of the slot that references it is answered.
uint32_t FindFreeBlockOnChain(EngineContext* ctx, uint32_t pool, uint32_t node, uint32_t size) {
  EnterFrame();
  uint32_t link = pool + 4u;
  uint32_t block = LoadU32(ctx, pool + 4u);
  if (block == 0u) {
    StoreU32(ctx, node, link);
    LeaveFrame();
    return link;
  }

  // The tag byte says whether the key and the length live behind a pointer or
  // inside the descriptor itself.
  const uint32_t descriptor_data = LoadU32(ctx, size);
  const uint32_t descriptor_tag = LoadU8(ctx, size + 11u);
  const bool external = (static_cast<int32_t>(descriptor_tag << 24u) >> 24) < 0;
  const uint32_t descriptor_length = LoadU32(ctx, size + 4u);
  const uint32_t search_data = external ? descriptor_data : size;
  const uint32_t search_length = external ? descriptor_length : descriptor_tag;

  for (;;) {
    const uint32_t tag = LoadU8(ctx, block + 27u);
    const bool indirect = (static_cast<int32_t>(tag << 24u) >> 24) < 0;
    const uint32_t header_length = LoadU32(ctx, block + 20u);
    const uint32_t block_length = indirect ? header_length : tag;
    const bool too_small = search_length > block_length;
    const uint32_t compare_length = too_small ? block_length : search_length;
    uint32_t block_data = 0u;

    if (compare_length != 0u) {
      const uint32_t key_slot = block + 16u;
      const uint32_t key_field = LoadU32(ctx, key_slot);
      block_data = indirect ? key_field : key_slot;
      const uint32_t cmp = CompareBytes(ctx, search_data, block_data, compare_length);
      if (cmp == 0u) {
        if (block_length > search_length) goto step_node;
        goto probe_reversed;
      }
      if (static_cast<int32_t>(cmp) >= 0) goto probe_reversed;
      goto step_node;
    }
    if (block_length > search_length) goto step_node;
    if (block_length < search_length) goto step_span;
    goto answer_slot;

  // The key sorts after the block's own key, so the probe is repeated with the
  // operands the other way round.
  probe_reversed: {
    const uint32_t reverse = CompareBytes(ctx, block_data, search_data, compare_length);
    if (reverse != 0u) {
      if (static_cast<int32_t>(reverse) >= 0) goto answer_slot;
      goto step_span;
    }
    if (too_small) goto step_span;
    goto answer_slot;
  }

  // Follow the link the block carries itself.
  step_node: {
    const uint32_t following = LoadU32(ctx, block);
    if (following == 0u) {
      StoreU32(ctx, node, block);
      LeaveFrame();
      return block;
    }
    link = block;
    block = following;
    continue;
  }

  // Follow the sibling slot listed ahead of the block.
  step_span: {
    link = block + 4u;
    const uint32_t following = LoadU32(ctx, block + 4u);
    if (following == 0u) goto answer_slot;
    block = following;
    continue;
  }

  answer_slot:
    StoreU32(ctx, node, block);
    LeaveFrame();
    return link;
  }
}

}  // namespace artc
