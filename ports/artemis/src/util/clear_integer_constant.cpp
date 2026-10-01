#include "util/engine_context.h"

namespace artc {

// Probes the inline-constant slot that `tag` and `slot` address: only the small
// constant class holding the sentinel one is cleared, and the answer describes
// the word as it was read.
uint32_t ClearIntegerConstant(EngineContext* ctx, uint32_t slot, uint32_t tag) {
  EnterFrame();
  uint32_t const entry = (tag << 2u) + slot;
  uint32_t const word = LoadU32(ctx, entry + 20u);
  if ((tag & 0xFFFFFFFEu) == 136u && word == 1u) {
    StoreU32(ctx, entry + 20u, 0u);
  }
  LeaveFrame();
  return word == 1u;
}

}  // namespace artc
