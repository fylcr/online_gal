#include "util/engine_context.h"

namespace artc {

// Forward declaration -- the byte comparer.
uint32_t CompareBytes(EngineContext* ctx, uint32_t lhs, uint32_t rhs, uint32_t n);

// Walks the ordered chain behind `a`, following whichever link the key ordering
// selects at each step, and answers the entry that matches the request described
// by `b`. The head slot is answered when the walk finds nothing.
uint32_t FindOrderedChainEntry(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  const uint32_t head_link = a + 4u;
  uint32_t cursor = LoadU32(ctx, a + 4u);
  if (cursor == 0u) {
    LeaveFrame();
    return head_link;
  }

  // The tag byte says whether the key and the length live behind a pointer or
  // inside the descriptor itself.
  const uint32_t descriptor_data = LoadU32(ctx, b);
  const uint32_t descriptor_tag = LoadU8(ctx, b + 11u);
  const bool external = (static_cast<int32_t>(descriptor_tag << 24u) >> 24) < 0;
  const uint32_t descriptor_length = LoadU32(ctx, b + 4u);
  const uint32_t search_data = external ? descriptor_data : b;
  const uint32_t search_length = external ? descriptor_length : descriptor_tag;

  uint32_t link = head_link;
  for (;;) {
    const uint32_t tag = LoadU8(ctx, cursor + 27u);
    const bool indirect = (static_cast<int32_t>(tag << 24u) >> 24) < 0;
    const uint32_t header_length = LoadU32(ctx, cursor + 20u);
    const uint32_t block_length = indirect ? header_length : tag;
    const bool longer = block_length > search_length;
    const uint32_t compare_length = longer ? block_length : search_length;

    // A nonzero compare answers the ordering outright; otherwise the lengths
    // settle it, with the block that cannot hold the request sorting low.
    uint32_t order = 0u;
    if (compare_length != 0u) {
      const uint32_t key_slot = cursor + 16u;
      const uint32_t key_field = LoadU32(ctx, key_slot);
      const uint32_t key = indirect ? key_field : key_slot;
      order = CompareBytes(ctx, key, search_data, compare_length);
    }
    if (order == 0u) {
      order = (search_length > block_length) ? 0xFFFFFFFFu : static_cast<uint32_t>(longer);
    }

    link = (static_cast<int32_t>(order) < 0) ? link : cursor;
    // The high bit of the ordering picks which of the two links the node keeps.
    cursor = LoadU32(ctx, cursor + ((order >> 29) & 4u));
    if (cursor == 0u) break;
  }

  if (link == head_link) {
    LeaveFrame();
    return head_link;
  }

  const uint32_t candidate_tag = LoadU8(ctx, link + 27u);
  const bool candidate_indirect = (static_cast<int32_t>(candidate_tag << 24u) >> 24) < 0;
  const uint32_t candidate_header = LoadU32(ctx, link + 20u);
  const uint32_t candidate_length = candidate_indirect ? candidate_header : candidate_tag;
  const uint32_t need = (candidate_length < search_length) ? candidate_length : search_length;

  if (need != 0u) {
    const uint32_t key_slot = link + 16u;
    const uint32_t key_field = LoadU32(ctx, key_slot);
    const uint32_t key = candidate_indirect ? key_field : key_slot;
    const uint32_t final = CompareBytes(ctx, search_data, key, need);
    if (final != 0u) {
      LeaveFrame();
      return static_cast<int32_t>(final) >= 0 ? link : head_link;
    }
  }
  LeaveFrame();
  return search_length < candidate_length ? head_link : link;
}

}  // namespace artc
