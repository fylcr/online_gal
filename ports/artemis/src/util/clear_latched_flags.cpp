#include "util/engine_context.h"

namespace artc {

// Consumes the flag mask the record latched at +52 and drops exactly those bits
// from its flag word at +40. The handle is relocated to its owning record
// through the descriptor delta behind its first word.
void ClearLatchedFlags(EngineContext* ctx, uint32_t handle) {
  EnterFrame();
  uint32_t const descriptor = LoadU32(ctx, handle);
  uint32_t const record = handle + LoadU32(ctx, descriptor - 148u);
  uint32_t const latched = LoadU32(ctx, record + 52u);
  StoreU32(ctx, record + 52u, 0u);
  StoreU32(ctx, record + 40u, LoadU32(ctx, record + 40u) & ~latched);
  LeaveFrame();
}

}  // namespace artc
