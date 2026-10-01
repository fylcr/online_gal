#include "util/engine_context.h"

namespace artc {

void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);

// Releases a node's tagged children: both links are followed, the owned payload
// goes when its flag byte reads negative, and the node is returned to the pool.
void ReleaseTaggedChildNodes(EngineContext* ctx, uint32_t owner, uint32_t node) {
  EnterFrame();
  if (node != 0u) {
    ReleaseTaggedChildNodes(ctx, owner, LoadU32(ctx, node));
    ReleaseTaggedChildNodes(ctx, owner, LoadU32(ctx, node + 4u));
    if (LoadS8(ctx, node + 27u) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, node + 16u));
    }
    DeallocateRawBlock(ctx, node);
  }
  LeaveFrame();
}

}  // namespace artc
