#include "util/engine_context.h"

namespace artc {

uint32_t ClearIntegerConstant(EngineContext* ctx, uint32_t slot, uint32_t tag);

// Answers bit 3 of the inline flag word at `index` in the state block's table,
// falling back to `ClearIntegerConstant` when the index is past the table or the
// word carries no inline entry.
uint32_t ReadInlineFlagBit3(EngineContext* ctx, uint32_t state, uint32_t index) {
  EnterFrame();
  uint32_t result = 0u;
  bool from_table = false;
  if (index <= 319u) {
    const uint32_t entry = LoadU32(ctx, (index << 2) + state + 8516u);
    if (entry & 63u) {
      result = (entry & 8u) >> 3;
      from_table = true;
    }
  }
  if (!from_table) {
    result = ClearIntegerConstant(ctx, state, index);
  }
  LeaveFrame();
  return result;
}

}  // namespace artc
