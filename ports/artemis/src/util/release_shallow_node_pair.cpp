#include "util/engine_context.h"

namespace artc {

void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void ReleaseNodeTree(EngineContext* ctx, uint32_t state, uint32_t node);

namespace {
// The shared empty-tail record a released node's tail link is pointed back at.
constexpr uint32_t kReleaseShallowNodePairClearedTail = 0xB5F40u;
}  // namespace

// Releases a shallow node pair: both links are followed, the tail link is cleared
// to the shared empty record, the owned payload is dropped, a nested subtree at
// offset 36 is released, and the node goes back to the pool.
void ReleaseShallowNodePair(EngineContext* ctx, uint32_t owner, uint32_t node) {
  EnterFrame();
  if (node != 0u) {
    ReleaseShallowNodePair(ctx, owner, LoadU32(ctx, node));
    ReleaseShallowNodePair(ctx, owner, LoadU32(ctx, node + 4u));
    StoreU32(ctx, node + 20u, kReleaseShallowNodePairClearedTail);
    if (LoadS8(ctx, node + 59u) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, node + 48u));
    }
    ReleaseNodeTree(ctx, node + 36u, LoadU32(ctx, node + 40u));
    if (LoadS8(ctx, node + 35u) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, node + 24u));
    }
    DeallocateRawBlock(ctx, node);
  }
  LeaveFrame();
}

}  // namespace artc
