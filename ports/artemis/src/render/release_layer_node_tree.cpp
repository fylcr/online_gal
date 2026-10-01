#include "util/engine_context.h"

namespace artc {

// Forward declaration — defined in util/engine_context.h.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);

// Recursively frees the tagged node tree rooted at `node`: both child slots
// first, then the payload the node owns — only when its sign tag at +27 is
// negative — and the node itself last. `owner` is threaded to the callees.
void ReleaseLayerNodeTree(EngineContext* ctx, uint32_t owner, uint32_t node) {
  EnterFrame();
  if (node != 0u) {
    ReleaseLayerNodeTree(ctx, owner, LoadU32(ctx, node));
    ReleaseLayerNodeTree(ctx, owner, LoadU32(ctx, node + 4u));
    if (LoadS8(ctx, node + 27u) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, node + 16u));
    }
    DeallocateRawBlock(ctx, node);
  }
  LeaveFrame();
}

}  // namespace artc
