#include "util/engine_context.h"

namespace artc {

void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);

// Releases a linked node chain depth-first: both child links are freed first, then
// the tail link is handed to the node's next slot and freed, then the inline
// payload when the size byte is negative, and finally the node itself.
void ReleaseLinkedNodeTree(EngineContext* ctx, uint32_t owner, uint32_t node) {
  EnterFrame();
  if (node != 0u) {
    ReleaseLinkedNodeTree(ctx, owner, LoadU32(ctx, node));
    ReleaseLinkedNodeTree(ctx, owner, LoadU32(ctx, node + 4u));
    const uint32_t tail = LoadU32(ctx, node + 28u);
    if (tail != 0u) {
      StoreU32(ctx, node + 32u, tail);
      DeallocateRawBlock(ctx, tail);
    }
    // A negative size byte marks the payload as a separately owned block.
    if (LoadS8(ctx, node + 27u) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, node + 16u));
    }
    DeallocateRawBlock(ctx, node);
  }
  LeaveFrame();
}

}  // namespace artc
