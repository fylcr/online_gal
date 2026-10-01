#include "util/engine_context.h"

namespace artc {

void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);

// Releases a two-child node tree depth-first: both links are followed first, then
// the two owned blocks behind the flag bytes that read negative, and finally the
// node itself.
void ReleaseNodeTree(EngineContext* ctx, uint32_t state, uint32_t node) {
  EnterFrame();
  if (node != 0u) {
    ReleaseNodeTree(ctx, state, LoadU32(ctx, node));
    ReleaseNodeTree(ctx, state, LoadU32(ctx, node + 4u));
    // A negative flag byte marks the field as a separately owned block.
    if (LoadS8(ctx, node + 39u) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, node + 28u));
    }
    if (LoadS8(ctx, node + 27u) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, node + 16u));
    }
    DeallocateRawBlock(ctx, node);
  }
  LeaveFrame();
}

}  // namespace artc
