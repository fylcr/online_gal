#include "util/engine_context.h"

namespace artc {

// Clears the flags of one node and everything below it.
void ClearFlagsRecursive(EngineContext* ctx, uint32_t state, uint32_t node);

// Clears the local flags of the enclosing scope. Each of the scope's nine local
// slots is visited in turn, and a live slot whose node carries either low flag
// bit is cleared through it.
void ClearLocalFlags(EngineContext* ctx, uint32_t state) {
  EnterFrame();
  uint32_t node = LoadU32(ctx, state + 152u);
  if (node != 0u && (LoadU8(ctx, node + 5u) & 3u) != 0u) {
    ClearFlagsRecursive(ctx, state, node);
  }
  node = LoadU32(ctx, state + 156u);
  if (node != 0u && (LoadU8(ctx, node + 5u) & 3u) != 0u) {
    ClearFlagsRecursive(ctx, state, node);
  }
  node = LoadU32(ctx, state + 160u);
  if (node != 0u && (LoadU8(ctx, node + 5u) & 3u) != 0u) {
    ClearFlagsRecursive(ctx, state, node);
  }
  node = LoadU32(ctx, state + 164u);
  if (node != 0u && (LoadU8(ctx, node + 5u) & 3u) != 0u) {
    ClearFlagsRecursive(ctx, state, node);
  }
  node = LoadU32(ctx, state + 168u);
  if (node != 0u && (LoadU8(ctx, node + 5u) & 3u) != 0u) {
    ClearFlagsRecursive(ctx, state, node);
  }
  node = LoadU32(ctx, state + 172u);
  if (node != 0u && (LoadU8(ctx, node + 5u) & 3u) != 0u) {
    ClearFlagsRecursive(ctx, state, node);
  }
  node = LoadU32(ctx, state + 176u);
  if (node != 0u && (LoadU8(ctx, node + 5u) & 3u) != 0u) {
    ClearFlagsRecursive(ctx, state, node);
  }
  node = LoadU32(ctx, state + 180u);
  if (node != 0u && (LoadU8(ctx, node + 5u) & 3u) != 0u) {
    ClearFlagsRecursive(ctx, state, node);
  }
  node = LoadU32(ctx, state + 184u);
  if (node != 0u && (LoadU8(ctx, node + 5u) & 3u) != 0u) {
    ClearFlagsRecursive(ctx, state, node);
  }
  LeaveFrame();
}

}  // namespace artc
