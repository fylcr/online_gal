#include "util/engine_context.h"

namespace artc {

// Forward declarations -- the raw block pool and the nested node tree walker.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void ReleaseNodeTree(EngineContext* ctx, uint32_t state, uint32_t node);

// Frees a node tree whose payload is not worth saving: both children go first,
// the node is stamped as gone, then the pieces only the tree owned are handed
// back to the pool and the node itself goes last. Returns nothing.
void ReleaseNoSaveNodeTree(EngineContext* ctx, uint32_t state, uint32_t node) {
  EnterFrame();
  if (node) {
    ReleaseNoSaveNodeTree(ctx, state, LoadU32(ctx, node));
    ReleaseNoSaveNodeTree(ctx, state, LoadU32(ctx, node + 4u));
    // The stamp the tree walker reads to tell a dead node from a live one.
    StoreU32(ctx, node + 28u, 0xB5F40u);
    if (LoadS8(ctx, node + 67u) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, node + 56u));
    }
    ReleaseNodeTree(ctx, node + 44u, LoadU32(ctx, node + 48u));
    if (LoadS8(ctx, node + 43u) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, node + 32u));
    }
    if (LoadS8(ctx, node + 27u) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, node + 16u));
    }
    DeallocateRawBlock(ctx, node);
  }
  LeaveFrame();
}

}  // namespace artc
