#include "util/engine_context.h"

namespace artc {

void ReleaseNodeSubtreeB(EngineContext* ctx, uint32_t a, uint32_t b);

// Releases the map subtree hung off the field at offset four of `owner`.
void ReleaseMapFieldSubtree(EngineContext* ctx, uint32_t owner) {
  EnterFrame();
  ReleaseNodeSubtreeB(ctx, owner, LoadU32(ctx, owner + 4u));
  LeaveFrame();
}

}  // namespace artc
