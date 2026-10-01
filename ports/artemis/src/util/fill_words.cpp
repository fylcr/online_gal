#include "util/engine_context.h"

namespace artc {

// The word-granular fill: one value written across `count` words at `dest`,
// answering `dest`. A zero count writes nothing.
uint32_t FillWords(EngineContext* ctx, uint32_t dest, uint32_t value, uint32_t count) {
  EnterFrame();
  if (count != 0u) {
    uint32_t cursor = dest;
    uint32_t remaining = count;
    while (remaining != 0u) {
      StoreU32(ctx, cursor, value);
      cursor += 4u;
      --remaining;
    }
  }
  LeaveFrame();
  return dest;
}

}  // namespace artc
