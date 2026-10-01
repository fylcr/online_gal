#include "util/engine_context.h"

namespace artc {

void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);

namespace {
// The shared empty-tail record a released node's tail link is pointed back at.
constexpr uint32_t kReleaseNestedTaggedNodesClearedTail = 0xB6A90u;
}  // namespace

// Releases nested tagged nodes: both links are followed, the tail link is cleared
// to the shared empty record, the three owned payloads are dropped when their flag
// bytes read negative, and finally the node itself.
void ReleaseNestedTaggedNodes(EngineContext* ctx, uint32_t owner, uint32_t node) {
  EnterFrame();
  if (node != 0u) {
    ReleaseNestedTaggedNodes(ctx, owner, LoadU32(ctx, node));
    ReleaseNestedTaggedNodes(ctx, owner, LoadU32(ctx, node + 4u));
    StoreU32(ctx, node + 28u, kReleaseNestedTaggedNodesClearedTail);
    if (LoadS8(ctx, node + 59u) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, node + 48u));
    }
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
