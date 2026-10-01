#include "script/value_slot.h"
#include "util/engine_context.h"

namespace artc {

// Resolves (index, key) to a value record and pushes a copy of it onto the
// state's value stack. An address outside its container resolves to the shared
// nil record, so the push needs no special case.
void PushTableField(EngineContext* ctx, uint32_t state, uint32_t index, uint32_t key) {
  EnterFrame();
  uint32_t const slot = ResolveValueSlot(ctx, state, index);
  uint32_t const table = LoadU32(ctx, slot);
  uint32_t const rec = LookupValue(ctx, table, key);
  uint32_t const top = LoadU32(ctx, state + 8u);
  StoreU64(ctx, top, LoadU64(ctx, rec));
  StoreU32(ctx, top + 8u, LoadU32(ctx, rec + 8u));
  StoreU32(ctx, state + 8u, top + 16u);
  LeaveFrame();
}

}  // namespace artc
