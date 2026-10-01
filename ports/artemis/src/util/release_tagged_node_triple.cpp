#include "util/engine_context.h"

namespace artc {

void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);

namespace {
// The shared empty-tail record a released node's tail link is pointed back at.
constexpr uint32_t kReleaseTaggedNodeTripleClearedTail = 0xB6AD4u;
}  // namespace

// Releases a tagged node triple: both links are followed, the tail link is cleared
// to the shared empty record, the two owned payloads are dropped when their flag
// bytes read negative, and the node goes back to the pool.
void ReleaseTaggedNodeTriple(EngineContext* ctx, uint32_t owner, uint32_t node) {
  EnterFrame();
  if (node != 0u) {
    ReleaseTaggedNodeTriple(ctx, owner, LoadU32(ctx, node));
    ReleaseTaggedNodeTriple(ctx, owner, LoadU32(ctx, node + 4u));
    StoreU32(ctx, node + 28u, kReleaseTaggedNodeTripleClearedTail);
    if (LoadS8(ctx, node + 47u) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, node + 36u));
    }
    if (LoadS8(ctx, node + 27u) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, node + 16u));
    }
    DeallocateRawBlock(ctx, node);
  }
  LeaveFrame();
}

}  // namespace artc
