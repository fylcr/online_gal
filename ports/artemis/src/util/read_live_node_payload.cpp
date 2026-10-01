#include "util/engine_context.h"

namespace artc {

// The liveness guard on a payload read: answers the payload word when the node's
// offset-8 word names a record of kind 6 whose pointer carries a set flag byte,
// and zero for any other node.
uint32_t ReadLiveNodePayload(EngineContext* ctx, uint32_t node) {
  EnterFrame();
  uint32_t payload = 0u;
  const uint32_t record = LoadU32(ctx, node + 8u) - 16u;
  if (LoadU32(ctx, record + 8u) == 6u) {
    const uint32_t object = LoadU32(ctx, record);
    if (LoadU8(ctx, object + 6u) != 0) {
      payload = LoadU32(ctx, object + 16u);
    }
  }
  LeaveFrame();
  return payload;
}

}  // namespace artc
