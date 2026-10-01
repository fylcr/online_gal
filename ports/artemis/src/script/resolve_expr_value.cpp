#include "util/engine_context.h"

namespace artc {

// Resolves a value-slot tag to the 16-byte value record it names: a positive
// tag indexes the slot array, a tag down to the pseudo-index floor counts back
// from the top, and the three pseudo-indices below that name the globals, the
// environment and the registry. Answers the record, or the shared nil record
// when the tag names nothing.
uint32_t ResolveExprValue(EngineContext* ctx, uint32_t state, uint32_t tag) {
  EnterFrame();
  int32_t const kPseudoIndexFloor = -9999;
  uint32_t const kGlobalsOffset = 10002u;
  // The index one below the globals pseudo-index, from which the upvalue
  // pseudo-indices count down, and the bias that folds such a tag into the
  // upvalue array's own addressing.
  uint32_t const kUpvalueIndexLimit = 0xFFFFD8EEu;
  uint32_t const kUpvalueSlotBias = 0x27118u;
  uint32_t value = 0u;
  if (static_cast<int32_t>(tag) >= 1) {
    const uint32_t base = LoadU32(ctx, state + 12u);
    const uint32_t slot = base + (tag << 4u) - 16u;
    // A positive tag is only a real slot while it sits inside the array part.
    value = (slot < LoadU32(ctx, state + 8u)) ? slot : kNilValueAddr;
  } else if (static_cast<int32_t>(tag) >= kPseudoIndexFloor) {
    value = LoadU32(ctx, state + 8u) + (tag << 4u);
  } else {
    switch (tag + kGlobalsOffset) {
      case 0u:
        value = state + 72u;
        break;
      case 1u: {
        // The environment pseudo-index is materialised as a value record of its
        // own, at a fixed pair of words inside the state.
        const uint32_t owner = LoadU32(ctx, LoadU32(ctx, LoadU32(ctx, state + 20u) + 4u));
        const uint32_t environment = LoadU32(ctx, owner + 12u);
        StoreU32(ctx, state + 96u, 5u);
        StoreU32(ctx, state + 88u, environment);
        value = state + 88u;
        break;
      }
      case 2u:
        value = LoadU32(ctx, state + 16u) + 96u;
        break;
      default: {
        // An upvalue pseudo-index: the index counts off the limit and only
        // names a record while the owner's upvalue count covers it.
        const uint32_t base = LoadU32(ctx, LoadU32(ctx, LoadU32(ctx, state + 20u) + 4u));
        const uint32_t slot = base + ((0u - tag) << 4u) - kUpvalueSlotBias;
        const uint32_t upvalue_index = kUpvalueIndexLimit - tag;
        value = (static_cast<int32_t>(LoadU8(ctx, base + 7u)) < static_cast<int32_t>(upvalue_index))
                    ? kNilValueAddr
                    : slot;
        break;
      }
    }
  }
  switch (LoadU32(ctx, value + 8u) - 2u) {
    case 0u:
      LeaveFrame();
      return LoadU32(ctx, value);
    case 5u:
      LeaveFrame();
      return LoadU32(ctx, value) + 24u;
    default:
      break;
  }
  LeaveFrame();
  return 0u;
}

}  // namespace artc
