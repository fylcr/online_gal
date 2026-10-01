#include "util/engine_context.h"

namespace artc {

namespace {

constexpr uint32_t kFreeLog2Bias3C000 = 0x3C000u;  // log2 probe: contributes the 2s bit
constexpr uint32_t kFreeLog2Bias7F000 = 0x7F000u;  // log2 probe: contributes the 4s bit
constexpr uint32_t kFreeLog2BiasFFF00 = 0xFFF00u;  // log2 probe: contributes the 8s bit
constexpr uint32_t kFreeSmallMap = 0xD1370u;  // bitmap of the occupied small bins
constexpr uint32_t kFreeTreeMap = 0xD1374u;  // bitmap of the occupied tree bins
constexpr uint32_t kFreeDvSize = 0xD1378u;  // size of the designated victim chunk
constexpr uint32_t kFreeTopSize = 0xD137Cu;  // size of the wilderness chunk
constexpr uint32_t kFreeLeastAddr = 0xD1380u;  // lowest address the arena has handed out
constexpr uint32_t kFreeDv = 0xD1384u;  // the designated victim chunk
constexpr uint32_t kFreeTop = 0xD1388u;  // the wilderness chunk
constexpr uint32_t kFreeSmallBins = 0xD1398u;  // base of the small-bin array
constexpr uint32_t kFreeTreeBins = 0xD14A0u;  // base of the tree-bin array
constexpr uint32_t kFreeTreeIndexLimit = 0xFFFFFFu;  // largest size with a computed tree index
constexpr uint32_t kFreeChunkSizeMask = 0xFFFFFFF8u;  // masks the size out of a chunk head word
constexpr uint32_t kFreeUseFlagMask = 0xFFFFFFFEu;  // masks the in-use flag out of a head word

}  // namespace

// Returns a block to the free list, coalescing it with the free chunks in
// front of and behind it and threading the merged block into the small-bin
// or tree-bin array of the allocator.
//
// state is the block's offset, block its size and next_chunk the block that
// follows it; cursor, scratch, value and slot walk the bin and treebin links,
// parent is the node above the one being unlinked, and link is the link the
// block is threaded onto.
void ReclaimFreeBlock(EngineContext* ctx, uint32_t state, uint32_t block) {
  EnterFrame();
  uint32_t cursor = 0u;
  uint32_t scratch = 0u;
  uint32_t value = 0u;
  uint32_t next_chunk = 0u;
  uint32_t parent = 0u;
  uint32_t slot = 0u;
  uint32_t link = 0u;
  next_chunk = state + block;
  scratch = LoadU32(ctx, state + 4u);
  if ((scratch & 1u) != 0u) goto check_next_chunk;
  if ((scratch & 3u) == 0u) goto done;
  cursor = LoadU32(ctx, state);
  block = cursor + block;
  state = state - cursor;
  if (state != LoadU32(ctx, kFreeDv)) {
    value = LoadU32(ctx, kFreeLeastAddr);
    if (cursor <= 255u) {
      value = LoadU32(ctx, state + 8u);
      cursor = cursor >> (3u & 31);
      scratch = LoadU32(ctx, state + 12u);
      if (scratch == value) {
        StoreU32(ctx, kFreeSmallMap,
                 LoadU32(ctx, kFreeSmallMap) & I32_ROTL(kFreeUseFlagMask, cursor));
        goto check_next_chunk;
      }
      StoreU32(ctx, value + 12u, scratch);
      StoreU32(ctx, scratch + 8u, value);
      goto check_next_chunk;
    }
    parent = LoadU32(ctx, state + 24u);
    scratch = LoadU32(ctx, state + 12u);
    if (scratch != state) {
      cursor = LoadU32(ctx, state + 8u);
      StoreU32(ctx, cursor + 12u, scratch);
      StoreU32(ctx, scratch + 8u, cursor);
      goto detach_merged;
    }
    cursor = state + 20u;
    value = LoadU32(ctx, cursor);
    if (value != 0u) goto walk_tree;
    cursor = state + 16u;
    value = LoadU32(ctx, cursor);
    if (value != 0u) goto walk_tree;
    scratch = 0u;
    goto detach_merged;
    walk_tree:;
    next_tree_node:;
    slot = cursor;
    scratch = value;
    cursor = scratch + 20u;
    value = LoadU32(ctx, cursor);
    if (value != 0u) goto next_tree_node;
    cursor = scratch + 16u;
    value = LoadU32(ctx, scratch + 16u);
    if (value != 0u) goto next_tree_node;
    StoreU32(ctx, slot, 0u);
    detach_merged:;
    if (parent == 0u) goto check_next_chunk;
    cursor = LoadU32(ctx, state + 28u);
    value = (cursor << (2u & 31)) + kFreeTreeBins;
    if (LoadU32(ctx, value) == state) {
      StoreU32(ctx, value, scratch);
      if (scratch != 0u) goto relink_subtree;
      StoreU32(ctx, kFreeTreeMap, LoadU32(ctx, kFreeTreeMap) & I32_ROTL(kFreeUseFlagMask, cursor));
      goto check_next_chunk;
    }
    StoreU32(ctx, (parent + ((LoadU32(ctx, parent + 16u) == state) ? 16u : 20u)), scratch);
    if (scratch == 0u) goto check_next_chunk;
    relink_subtree:;
    StoreU32(ctx, scratch + 24u, parent);
    cursor = LoadU32(ctx, state + 16u);
    if (cursor != 0u) {
      StoreU32(ctx, scratch + 16u, cursor);
      StoreU32(ctx, cursor + 24u, scratch);
    }
    cursor = LoadU32(ctx, state + 20u);
    if (cursor == 0u) goto check_next_chunk;
    StoreU32(ctx, scratch + 20u, cursor);
    StoreU32(ctx, cursor + 24u, scratch);
    goto check_next_chunk;
  }
  scratch = LoadU32(ctx, next_chunk + 4u);
  if ((scratch & 3u) != 3u) goto check_next_chunk;
  StoreU32(ctx, kFreeDvSize, block);
  StoreU32(ctx, next_chunk + 4u, scratch & kFreeUseFlagMask);
  StoreU32(ctx, state + 4u, block | 1u);
  StoreU32(ctx, next_chunk, block);
  goto exit_path;
  check_next_chunk:;
  scratch = LoadU32(ctx, next_chunk + 4u);
  if ((scratch & 2u) == 0u) {
    if (next_chunk == LoadU32(ctx, kFreeTop)) {
      StoreU32(ctx, kFreeTop, state);
      block = LoadU32(ctx, kFreeTopSize) + block;
      StoreU32(ctx, kFreeTopSize, block);
      StoreU32(ctx, state + 4u, block | 1u);
      if (LoadU32(ctx, kFreeDv) != state) goto done;
      StoreU32(ctx, kFreeDvSize, 0u);
      StoreU32(ctx, kFreeDv, 0u);
      goto exit_path;
    }
    if (next_chunk == LoadU32(ctx, kFreeDv)) {
      StoreU32(ctx, kFreeDv, state);
      block = LoadU32(ctx, kFreeDvSize) + block;
      StoreU32(ctx, kFreeDvSize, block);
      StoreU32(ctx, state + 4u, block | 1u);
      StoreU32(ctx, (state + block), block);
      goto exit_path;
    }
    cursor = LoadU32(ctx, kFreeLeastAddr);
    block = (scratch & kFreeChunkSizeMask) + block;
    if (scratch <= 255u) {
      value = LoadU32(ctx, next_chunk + 8u);
      scratch = scratch >> (3u & 31);
      cursor = LoadU32(ctx, next_chunk + 12u);
      if (value == cursor) {
        StoreU32(ctx, kFreeSmallMap,
                 LoadU32(ctx, kFreeSmallMap) & I32_ROTL(kFreeUseFlagMask, scratch));
        goto merged_ready;
      }
      StoreU32(ctx, value + 12u, cursor);
      StoreU32(ctx, cursor + 8u, value);
      goto merged_ready;
    }
    parent = LoadU32(ctx, next_chunk + 24u);
    scratch = LoadU32(ctx, next_chunk + 12u);
    if (next_chunk != scratch) {
      value = cursor;
      cursor = LoadU32(ctx, next_chunk + 8u);
      StoreU32(ctx, cursor + 12u, scratch);
      StoreU32(ctx, scratch + 8u, cursor);
      goto detach_absorbed;
    }
    cursor = next_chunk + 20u;
    value = LoadU32(ctx, cursor);
    if (value != 0u) goto walk_absorbed_tree;
    cursor = next_chunk + 16u;
    value = LoadU32(ctx, cursor);
    if (value != 0u) goto walk_absorbed_tree;
    scratch = 0u;
    goto detach_absorbed;
    walk_absorbed_tree:;
    next_absorbed_node:;
    slot = cursor;
    scratch = value;
    cursor = scratch + 20u;
    value = LoadU32(ctx, cursor);
    if (value != 0u) goto next_absorbed_node;
    cursor = scratch + 16u;
    value = LoadU32(ctx, scratch + 16u);
    if (value != 0u) goto next_absorbed_node;
    StoreU32(ctx, slot, 0u);
    detach_absorbed:;
    if (parent == 0u) goto merged_ready;
    cursor = LoadU32(ctx, next_chunk + 28u);
    value = (cursor << (2u & 31)) + kFreeTreeBins;
    if (next_chunk == LoadU32(ctx, value)) {
      StoreU32(ctx, value, scratch);
      if (scratch != 0u) goto relink_absorbed_subtree;
      StoreU32(ctx, kFreeTreeMap, LoadU32(ctx, kFreeTreeMap) & I32_ROTL(kFreeUseFlagMask, cursor));
      goto merged_ready;
    }
    StoreU32(ctx, (parent + ((next_chunk == LoadU32(ctx, parent + 16u)) ? 16u : 20u)), scratch);
    if (scratch == 0u) goto merged_ready;
    relink_absorbed_subtree:;
    StoreU32(ctx, scratch + 24u, parent);
    cursor = LoadU32(ctx, next_chunk + 16u);
    if (cursor != 0u) {
      StoreU32(ctx, scratch + 16u, cursor);
      StoreU32(ctx, cursor + 24u, scratch);
    }
    cursor = LoadU32(ctx, next_chunk + 20u);
    if (cursor == 0u) goto merged_ready;
    StoreU32(ctx, scratch + 20u, cursor);
    StoreU32(ctx, cursor + 24u, scratch);
    merged_ready:;
    StoreU32(ctx, state + 4u, block | 1u);
    StoreU32(ctx, (state + block), block);
    if (LoadU32(ctx, kFreeDv) != state) goto link_chunk;
    StoreU32(ctx, kFreeDvSize, block);
    goto exit_path;
  }
  StoreU32(ctx, next_chunk + 4u, scratch & kFreeUseFlagMask);
  StoreU32(ctx, state + 4u, block | 1u);
  StoreU32(ctx, (state + block), block);
  link_chunk:;
  if (block <= 255u) {
    scratch = block >> (3u & 31);
    block = (scratch << (3u & 31)) + kFreeSmallBins;
    cursor = LoadU32(ctx, kFreeSmallMap);
    scratch = 1u << (scratch & 31);
    if ((cursor & scratch) == 0u) {
      StoreU32(ctx, kFreeSmallMap, cursor | scratch);
      link = block;
      goto bin_linked;
    }
    link = LoadU32(ctx, block + 8u);
    bin_linked:;
    cursor = link;
    StoreU32(ctx, block + 8u, state);
    StoreU32(ctx, cursor + 12u, state);
    StoreU32(ctx, state + 12u, block);
    StoreU32(ctx, state + 8u, cursor);
    goto exit_path;
  }
  cursor = 31u;
  StoreU64(ctx, state + 16u, 0ull);
  if (block <= kFreeTreeIndexLimit) {
    cursor = block >> (8u & 31);
    scratch = ((cursor + kFreeLog2BiasFFF00) >> (16u & 31)) & 8u;
    cursor = cursor << (scratch & 31);
    link = cursor;
    cursor = ((cursor + kFreeLog2Bias7F000) >> (16u & 31)) & 4u;
    value = link << (cursor & 31);
    link = value;
    value = ((value + kFreeLog2Bias3C000) >> (16u & 31)) & 2u;
    scratch = ((link << (value & 31)) >> (15u & 31)) - (value | (cursor | scratch));
    value = scratch << (1u & 31);
    cursor = (value | ((block >> ((scratch + 21u) & 31)) & 1u)) + 28u;
  }
  StoreU32(ctx, state + 28u, cursor);
  scratch = (cursor << (2u & 31)) + kFreeTreeBins;
  value = LoadU32(ctx, kFreeTreeMap);
  slot = 1u << (cursor & 31);
  if ((value & slot) == 0u) {
    StoreU32(ctx, kFreeTreeMap, value | slot);
    StoreU32(ctx, scratch, state);
    StoreU32(ctx, state + 24u, scratch);
    goto tree_bin_root;
  }
  cursor = block << (((cursor == 31u) ? 0u : (25u - (cursor >> (1u & 31)))) & 31);
  scratch = LoadU32(ctx, scratch);
  descend_tree:;
  value = scratch;
  if ((LoadU32(ctx, scratch + 4u) & kFreeChunkSizeMask) == block) goto tree_bin_match;
  scratch = cursor >> (29u & 31);
  cursor = cursor << (1u & 31);
  slot = (scratch & 4u) + value;
  scratch = LoadU32(ctx, slot + 16u);
  if (scratch != 0u) goto descend_tree;
  StoreU32(ctx, slot + 16u, state);
  StoreU32(ctx, state + 24u, value);
  tree_bin_root:;
  StoreU32(ctx, state + 12u, state);
  StoreU32(ctx, state + 8u, state);
  goto exit_path;
  tree_bin_match:;
  block = LoadU32(ctx, value + 8u);
  StoreU32(ctx, block + 12u, state);
  StoreU32(ctx, value + 8u, state);
  StoreU32(ctx, state + 24u, 0u);
  StoreU32(ctx, state + 12u, value);
  StoreU32(ctx, state + 8u, block);
  done:;
  exit_path:;
  LeaveFrame();
}

}  // namespace artc
