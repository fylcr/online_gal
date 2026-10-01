#include "util/engine_context.h"

namespace artc {

// Looks up a masked metamethod slot and answers the method it holds.
uint32_t LookupMaskedMetamethodSlot(EngineContext* ctx, uint32_t a, uint32_t b);

// Fetches the metamethod of one object for one kind of access. Depending on what
// kind of object it is, the method is held either right behind the object header
// or in the interpreter's per-kind slot table; an empty slot answers the nil
// value instead.
uint32_t FetchObjectMetamethod(EngineContext* ctx, uint32_t state, uint32_t object, uint32_t event) {
  EnterFrame();
  uint32_t const kind = LoadU32(ctx, object + 8u);
  uint32_t slot_addr = 0u;
  if (kind == 5u) {
    // This kind of object carries its metatable inline.
    slot_addr = LoadU32(ctx, object) + 8u;
  } else if (kind == 7u) {
    slot_addr = LoadU32(ctx, object) + 8u;
  } else {
    // Every other kind keeps its methods in the interpreter's per-kind slot table.
    slot_addr = LoadU32(ctx, state + 16u) + (kind << 2u) + 152u;
  }
  uint32_t const value = LoadU32(ctx, slot_addr);
  if (value == 0u) {
    LeaveFrame();
    return kNilValueAddr;
  }
  uint32_t const mask = LoadU32(ctx, LoadU32(ctx, state + 16u) + (event << 2u) + 188u);
  LeaveFrame();
  return LookupMaskedMetamethodSlot(ctx, value, mask);
}

}  // namespace artc
