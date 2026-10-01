#include "util/engine_context.h"

namespace artc {

// Clears the low flag bits of one node, then walks the chain that node heads:
// every kind-7 node recurses into its child while that child's flags are still
// set and steps on to the next node, and the other kinds park the node the state
// keeps and stop. Answers nothing.
void ClearFlagsRecursive(EngineContext* ctx, uint32_t state, uint32_t node) {
  EnterFrame();
  uint32_t flags = LoadU8(ctx, node + 5u);
  for (;;) {
    flags &= 252u;
    StoreU8(ctx, node + 5u, flags);
    const uint32_t kind = LoadU8(ctx, node + 4u);
    if (kind == 7u) {
      StoreU8(ctx, node + 5u, flags | 4u);
      const uint32_t child = LoadU32(ctx, node + 8u);
      if (child != 0u && (LoadU8(ctx, child + 5u) & 3u) != 0u) {
        ClearFlagsRecursive(ctx, state, child);
      }
      node = LoadU32(ctx, node + 12u);
      flags = LoadU8(ctx, node + 5u);
      if ((flags & 3u) == 0u) {
        LeaveFrame();
        return;
      }
      continue;
    }
    if (kind == 10u) {
      // A group: its first member is cleared too, unless the group is only the
      // node's own inline storage, in which case the node is marked instead.
      const uint32_t group = LoadU32(ctx, node + 8u);
      uint32_t member = group;
      if (static_cast<int32_t>(LoadU32(ctx, group + 8u)) >= 4) {
        const uint32_t first = LoadU32(ctx, group);
        if ((LoadU8(ctx, first + 5u) & 3u) != 0u) {
          ClearFlagsRecursive(ctx, state, first);
          member = LoadU32(ctx, node + 8u);
        }
      }
      if (member != node + 16u) {
        LeaveFrame();
        return;
      }
      StoreU8(ctx, node + 5u, LoadU8(ctx, node + 5u) | 4u);
      LeaveFrame();
      return;
    }
    switch (kind) {
      case 5u:
        StoreU32(ctx, node + 24u, LoadU32(ctx, state + 36u));
        break;
      case 6u:
        StoreU32(ctx, node + 8u, LoadU32(ctx, state + 36u));
        break;
      case 8u:
        StoreU32(ctx, node + 108u, LoadU32(ctx, state + 36u));
        break;
      case 9u:
        StoreU32(ctx, node + 68u, LoadU32(ctx, state + 36u));
        break;
      default:
        LeaveFrame();
        return;
    }
    StoreU32(ctx, state + 36u, node);
    LeaveFrame();
    return;
  }
}

}  // namespace artc
