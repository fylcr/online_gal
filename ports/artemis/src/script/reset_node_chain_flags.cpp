#include "util/engine_context.h"

namespace artc {

// Forward declaration -- defined in util/clear_flags_recursive.cpp.
void ClearFlagsRecursive(EngineContext* ctx, uint32_t state, uint32_t node);

// Unlinks a node from the chain it sits on and relinks it ahead of the chain's
// current head, then refreshes its low flag bits from the owner unless the node
// already carries flags of its own. Answers nothing.
void ResetNodeChainFlags(EngineContext* ctx, uint32_t state, uint32_t node) {
  EnterFrame();
  uint32_t link = LoadU32(ctx, state + 16u);
  StoreU32(ctx, node, LoadU32(ctx, link + 28u));
  StoreU32(ctx, link + 28u, node);
  const uint32_t flags = LoadU8(ctx, node + 5u);
  if ((flags & 7u) == 0u) {
    if (LoadU8(ctx, link + 21u) != 1u) {
      // The owner is not this shape: take its low bits and keep the node's own.
      StoreU8(ctx, node + 5u, (LoadU8(ctx, link + 20u) & 3u) | (flags & 248u));
      LeaveFrame();
      return;
    }
    StoreU8(ctx, node + 5u, flags | 4u);
    link = LoadU32(ctx, node + 8u);
    if (static_cast<int32_t>(LoadU32(ctx, link + 8u)) < 4) {
      LeaveFrame();
      return;
    }
    link = LoadU32(ctx, link);
    if ((LoadU8(ctx, link + 5u) & 3u) == 0u) {
      LeaveFrame();
      return;
    }
    const uint32_t owner = LoadU32(ctx, state + 16u);
    if (LoadU8(ctx, owner + 21u) == 1u) {
      ClearFlagsRecursive(ctx, owner, link);
      LeaveFrame();
      return;
    }
    StoreU8(ctx, node + 5u, (LoadU8(ctx, owner + 20u) & 3u) | (flags & 248u));
  }
  LeaveFrame();
}

}  // namespace artc
