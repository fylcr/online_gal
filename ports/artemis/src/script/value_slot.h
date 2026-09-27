// value_slot.h — shared index resolution for the script VM's value records.
//
// ResolveValueSlot maps a stack or pseudo index to the address of the 16-byte
// value record it names: positive indexes are absolute stack records below the
// top, small negatives count back from the top, and the far-negative pseudo
// ones reach the state's embedded records, the environment record (rebuilt
// from the frame chain on demand), or an upvalue of the running closure. An
// index outside its container resolves to the shared nil record's address, so
// callers can read a canonical answer without a special case.
#pragma once
#include "util/engine_context.h"

namespace artc {

inline uint32_t ResolveValueSlot(EngineContext* ctx, uint32_t state, uint32_t index) {
  if (static_cast<int32_t>(index) >= 1) {
    // Absolute stack record, live only below the current top.
    uint32_t const rec = LoadU32(ctx, state + 12u) + (index << 4) - 16u;
    return (LoadU32(ctx, state + 8u) > rec) ? rec : kNilValueAddr;
  }
  if (static_cast<int32_t>(index) >= -9999) {
    // Counted back from the top (index -1 is the last pushed record).
    return LoadU32(ctx, state + 8u) + (index << 4);
  }
  switch (index + 10002u) {
    case 0:
      // The state's own embedded record.
      return state + 72u;
    case 1: {
      // The environment record, rebuilt from the frame chain on demand.
      uint32_t const chain =
          LoadU32(ctx, LoadU32(ctx, LoadU32(ctx, LoadU32(ctx, state + 20u) + 4u)) + 12u);
      StoreU32(ctx, state + 96u, 5u);
      StoreU32(ctx, state + 88u, chain);
      return state + 88u;
    }
    case 2:
      return LoadU32(ctx, state + 16u) + 96u;
    default: {
      // An upvalue pseudo-index, bounded by the closure's upvalue count.
      uint32_t const closure = LoadU32(ctx, LoadU32(ctx, LoadU32(ctx, state + 20u) + 4u));
      uint32_t const addr = closure + (((0u - index) - 10000u) << 4) - 24u;
      uint32_t const bound = static_cast<uint32_t>(-9998) - index;
      return (static_cast<int32_t>(LoadU8(ctx, closure + 7u)) < static_cast<int32_t>(bound))
                 ? kNilValueAddr
                 : addr;
    }
  }
}

}  // namespace artc
