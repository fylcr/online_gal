#include "util/engine_context.h"

namespace artc {

void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);

// Post-order release of one map node subtree: both links are followed before the
// node itself goes back to the pool.
void ReleaseNodeSubtreeB(EngineContext* ctx, uint32_t owner, uint32_t node) {
  EnterFrame();
  if (node != 0u) {
    ReleaseNodeSubtreeB(ctx, owner, LoadU32(ctx, node));
    ReleaseNodeSubtreeB(ctx, owner, LoadU32(ctx, node + 4u));
    DeallocateRawBlock(ctx, node);
  }
  LeaveFrame();
}

}  // namespace artc
