#include "util/engine_context.h"

namespace artc {

// Forward declaration -- the raw block pool.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);

// Unlinks a node from an ordered chain: the entries its cursor chains over have
// their payloads freed, the node's link word is cleared, the chain's spare blocks
// go back to the pool and the slot cursor is re-stamped. Returns nothing.
void UnlinkOrderedChainNode(EngineContext* ctx, uint32_t chain) {
  EnterFrame();
  // One bucket holds 341 entries of twelve bytes each.
  uint32_t const slot = LoadU32(ctx, chain + 16u);
  uint32_t const bucket_index = slot / 341u;
  uint32_t bucket = LoadU32(ctx, chain + 4u);
  uint32_t bucket_end = LoadU32(ctx, chain + 8u);
  if (bucket != bucket_end) {
    uint32_t bucket_slot = (bucket_index << 2u) + bucket;
    uint32_t entry = LoadU32(ctx, bucket_slot) + ((slot - bucket_index * 341u) * 12u);
    uint32_t const target = LoadU32(ctx, chain + 20u) + slot;
    uint32_t const target_bucket = target / 341u;
    uint32_t const target_array = LoadU32(ctx, (target_bucket << 2u) + bucket);
    uint32_t const target_entry = target_array + ((target - target_bucket * 341u) * 12u);
    if (target_entry != entry) {
      do {
        // A tag byte below zero marks an entry that owns the payload it points at.
        if (LoadS8(ctx, entry + 11u) < 0) {
          DeallocateRawBlock(ctx, LoadU32(ctx, entry));
        }
        entry += 12u;
        // An entry array runs out at one bucket's worth of bytes, and the walk
        // carries on in the bucket after it.
        if (entry - LoadU32(ctx, bucket_slot) == 4092u) {
          entry = LoadU32(ctx, bucket_slot + 4u);
          bucket_slot += 4u;
        }
      } while (entry != target_entry);
      bucket = LoadU32(ctx, chain + 4u);
      bucket_end = LoadU32(ctx, chain + 8u);
    }
  }
  StoreU32(ctx, chain + 20u, 0u);
  uint32_t count = static_cast<uint32_t>(static_cast<int32_t>(bucket_end - bucket) >> 2u);
  while (count > 2u) {
    DeallocateRawBlock(ctx, LoadU32(ctx, bucket));
    bucket = LoadU32(ctx, chain + 4u) + 4u;
    StoreU32(ctx, chain + 4u, bucket);
    count = static_cast<uint32_t>(static_cast<int32_t>(LoadU32(ctx, chain + 8u) - bucket) >> 2u);
  }
  // The cursor is left half a bucket ahead when one block is left and a whole
  // bucket ahead when two are.
  if (count == 1u) {
    StoreU32(ctx, chain + 16u, 170u);
  } else if (count == 2u) {
    StoreU32(ctx, chain + 16u, 341u);
  }
  LeaveFrame();
}

}  // namespace artc
