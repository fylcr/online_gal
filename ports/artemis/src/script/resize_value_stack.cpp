#include "util/engine_context.h"

namespace artc {

// Moves the state's value stack to a new depth. A non-negative depth lands that
// many records from the base and clears the type field of every record it grows
// into; a negative depth retreats from the current top with -1 standing still.
void ResizeValueStack(EngineContext* ctx, uint32_t state, uint32_t depth) {
  EnterFrame();
  if (static_cast<int32_t>(depth) >= 0) {
    uint32_t const limit = LoadU32(ctx, state + 12u) + (depth << 4);
    for (uint32_t rec = LoadU32(ctx, state + 8u); rec < limit; rec += 16u) {
      StoreU32(ctx, rec + 8u, 0u);
    }
    StoreU32(ctx, state + 8u, limit);
  } else {
    uint32_t const top = LoadU32(ctx, state + 8u);
    StoreU32(ctx, state + 8u, top + (depth << 4) + 16u);
  }
  LeaveFrame();
}

}  // namespace artc
