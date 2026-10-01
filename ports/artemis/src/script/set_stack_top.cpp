#include "util/engine_context.h"

namespace artc {

// Pushes a copy of the value a stack index names: the index is resolved to its
// 16-byte value record and the record is copied whole to the stack top, which
// then advances one record. Answers nothing.
void SetStackTop(EngineContext* ctx, uint32_t stack, uint32_t index) {
  EnterFrame();
  int32_t const kPseudoIndexFloor = -9999;
  uint32_t const kGlobalsOffset = 10002u;
  uint32_t const kUpvalueIndexLimit = 0xFFFFD8EEu;
  uint32_t const kUpvalueSlotBias = 0x27118u;
  uint32_t slot = 0u;
  if (static_cast<int32_t>(index) >= 1) {
    const uint32_t base = LoadU32(ctx, stack + 12u);
    const uint32_t pushed = base + (index << 4u) - 16u;
    // A positive index is only a real slot while it sits inside the array part.
    slot = (LoadU32(ctx, stack + 8u) > pushed) ? pushed : kNilValueAddr;
  } else if (static_cast<int32_t>(index) >= kPseudoIndexFloor) {
    slot = LoadU32(ctx, stack + 8u) + (index << 4u);
  } else {
    switch (index + kGlobalsOffset) {
      case 0u:
        slot = stack + 72u;
        break;
      case 1u: {
        const uint32_t owner = LoadU32(ctx, LoadU32(ctx, LoadU32(ctx, stack + 20u) + 4u));
        const uint32_t environment = LoadU32(ctx, owner + 12u);
        StoreU32(ctx, stack + 96u, 5u);
        StoreU32(ctx, stack + 88u, environment);
        slot = stack + 88u;
        break;
      }
      case 2u:
        slot = LoadU32(ctx, stack + 16u) + 96u;
        break;
      default: {
        const uint32_t base = LoadU32(ctx, LoadU32(ctx, LoadU32(ctx, stack + 20u) + 4u));
        const uint32_t pushed = base + ((0u - index) << 4u) - kUpvalueSlotBias;
        const uint32_t upvalue_index = kUpvalueIndexLimit - index;
        slot = (static_cast<int32_t>(LoadU8(ctx, base + 7u)) < static_cast<int32_t>(upvalue_index))
                   ? kNilValueAddr
                   : pushed;
        break;
      }
    }
  }
  const uint32_t top = LoadU32(ctx, stack + 8u);
  StoreU64(ctx, top, LoadU64(ctx, slot));
  StoreU32(ctx, top + 8u, LoadU32(ctx, slot + 8u));
  StoreU32(ctx, stack + 8u, LoadU32(ctx, stack + 8u) + 16u);
  LeaveFrame();
}

}  // namespace artc
