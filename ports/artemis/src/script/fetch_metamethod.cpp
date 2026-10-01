#include "util/engine_context.h"

namespace artc {

// Forward declaration — defined in script/lookup_masked_metamethod_slot.cpp.
uint32_t LookupMaskedMetamethodSlot(EngineContext* ctx, uint32_t metatable, uint32_t event_name);

// Looks `event_name` up in `metatable`'s metamethod chain. A hit answers the
// record it found; a miss latches the no-event bit for `event` in the metatable's
// flag byte and answers zero, which callers read as "no metamethod".
uint32_t FetchMetamethod(EngineContext* ctx, uint32_t metatable, uint32_t event,
                         uint32_t event_name) {
  EnterFrame();
  uint32_t const slot = LookupMaskedMetamethodSlot(ctx, metatable, event_name);
  if (LoadU32(ctx, slot + 8u) != 0u) {
    LeaveFrame();
    return slot;
  }
  // One flag byte covers 32 events, so the event index is its bit position.
  StoreU8(ctx, metatable + 6u, LoadU8(ctx, metatable + 6u) | (1u << (event & 31u)));
  LeaveFrame();
  return 0u;
}

}  // namespace artc
