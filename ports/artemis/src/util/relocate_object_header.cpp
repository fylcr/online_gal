#include "util/engine_context.h"

namespace artc {

// Moves an object header: the block named by the state's offset-16 field gives up
// its offset-28 word to `source`, takes `source` in that word's place, and the
// copy carries the caller's tag and the low two bits of the old flag byte.
void RelocateObjectHeader(EngineContext* ctx, uint32_t state, uint32_t source, uint32_t tag) {
  EnterFrame();
  const uint32_t slab = LoadU32(ctx, state + 16u);
  StoreU32(ctx, source, LoadU32(ctx, slab + 28u));
  StoreU32(ctx, slab + 28u, source);
  const uint32_t flags = LoadU8(ctx, slab + 20u);
  StoreU8(ctx, source + 4u, tag);
  StoreU8(ctx, source + 5u, flags & 3u);
  LeaveFrame();
}

}  // namespace artc
