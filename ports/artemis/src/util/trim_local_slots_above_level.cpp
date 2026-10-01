#include "util/engine_context.h"

namespace artc {

// Walks a function prototype's local slot records downwards and drops the slots
// that belong to levels above the one asked for, then writes the number of slots
// left into the result's slot count. Returns one when the walk finished and zero
// when it ran past the last record.
uint32_t TrimLocalSlotsAboveLevel(EngineContext* ctx, uint32_t proto, uint32_t level, uint32_t result) {
  EnterFrame();
  uint32_t slot = LoadU32(ctx, proto + 20u);
  if (static_cast<int32_t>(level) >= 1) {
    uint32_t const base = LoadU32(ctx, proto + 40u);
    do {
      if (slot <= base) {
        LeaveFrame();
        return 0u;
      }
      level -= 1u;
      // A record whose byte at offset six is clear spends more than one level.
      if (LoadU8(ctx, LoadU32(ctx, LoadU32(ctx, slot + 4u)) + 6u) == 0u) {
        level -= LoadU32(ctx, slot + 20u);
      }
      // The records are laid out downwards, one every twenty-four bytes.
      slot -= 24u;
    } while (static_cast<int32_t>(level) > 0);
  }
  if (level == 0u) {
    proto = LoadU32(ctx, proto + 40u);
    if (slot <= proto) {
      LeaveFrame();
      return 0u;
    }
    StoreU32(ctx, result + 96u, I32_DIV_S(slot - proto, 24u));
    LeaveFrame();
    return 1u;
  }
  StoreU32(ctx, result + 96u, 0u);
  LeaveFrame();
  return 1u;
}

}  // namespace artc
