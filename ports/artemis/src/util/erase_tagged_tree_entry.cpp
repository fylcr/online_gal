#include "util/engine_context.h"

namespace artc {

// Forward declarations -- the block freer, the node-tree releaser and the
// rebalancer.
void DeallocateRawBlock(EngineContext* ctx, uint32_t block);
void ReleaseNodeTree(EngineContext* ctx, uint32_t state, uint32_t node);
void EraseTreeRebalance(EngineContext* ctx, uint32_t a, uint32_t b);

// Unlinks one node from the tagged tree, rebalances from the tree's root and
// destroys the values the node held. Answers the node that takes its place, or
// the node itself when it has no successor.
uint32_t EraseTaggedTreeEntry(EngineContext* ctx, uint32_t tree, uint32_t node) {
  EnterFrame();
  uint32_t const kErasedMarker = 0xB5F40u;
  uint32_t cursor = LoadU32(ctx, node + 4u);
  uint32_t successor = 0u;
  if (cursor == 0u) {
    // A leaf: the successor is the node the parent's link points at, kept while
    // that link still points back at the node being erased.
    successor = LoadU32(ctx, node + 8u);
    if (LoadU32(ctx, successor) != node) {
      uint32_t link_cell = node + 8u;
      for (;;) {
        cursor = LoadU32(ctx, link_cell);
        link_cell = cursor + 8u;
        successor = LoadU32(ctx, cursor + 8u);
        if (LoadU32(ctx, successor) == cursor) {
          break;
        }
      }
    }
  } else {
    // Otherwise the successor is the leftmost node of the right subtree, whose
    // own left link is null.
    while (cursor != 0u) {
      successor = cursor;
      cursor = LoadU32(ctx, cursor);
    }
  }
  if (LoadU32(ctx, tree) == node) {
    StoreU32(ctx, tree, successor);
  }
  StoreU32(ctx, tree + 8u, LoadU32(ctx, tree + 8u) - 1u);
  EraseTreeRebalance(ctx, LoadU32(ctx, tree + 4u), node);
  StoreU32(ctx, node + 20u, kErasedMarker);
  if (static_cast<int32_t>(LoadS8(ctx, node + 59u)) <= -1) {
    DeallocateRawBlock(ctx, LoadU32(ctx, node + 48u));
  }
  ReleaseNodeTree(ctx, node + 36u, LoadU32(ctx, node + 40u));
  if (static_cast<int32_t>(LoadS8(ctx, node + 35u)) <= -1) {
    DeallocateRawBlock(ctx, LoadU32(ctx, node + 24u));
  }
  DeallocateRawBlock(ctx, node);
  LeaveFrame();
  return successor;
}

}  // namespace artc
