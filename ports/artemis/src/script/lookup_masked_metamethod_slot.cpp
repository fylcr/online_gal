#include "util/engine_context.h"

namespace artc {

// Walks `metatable`'s metamethod chain for the entry `event_name` names, the
// bucket mask coming from the metatable's shift byte. A chain that runs out
// answers the shared nil record.
uint32_t LookupMaskedMetamethodSlot(EngineContext* ctx, uint32_t metatable, uint32_t event_name) {
  EnterFrame();
  uint32_t const base = LoadU32(ctx, metatable + 16u);
  uint32_t const key = LoadU32(ctx, event_name + 8u);
  uint32_t const shift = LoadU8(ctx, metatable + 7u);
  uint32_t const mask = (0xFFFFFFFFu << (shift & 31u)) ^ 0xFFFFFFFFu;
  uint32_t node = base + ((key & mask) << 5u);
  while (LoadU32(ctx, node + 24u) != 4u || LoadU32(ctx, node + 16u) != event_name) {
    node = LoadU32(ctx, node + 28u);
    if (node == 0u) {
      node = kNilValueAddr;
      break;
    }
  }
  LeaveFrame();
  return node;
}

}  // namespace artc
