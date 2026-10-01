#include "util/engine_context.h"

namespace artc {

// Writes `value` in decimal backwards, one digit per step, and answers the start
// of the digit run. Callers pass the end of the buffer the digits are written
// into.
uint32_t FormatUnsignedDecimal(EngineContext* ctx, uint64_t value, uint32_t dest) {
  EnterFrame();
  uint64_t remaining = value;
  // A 32-bit divide cannot carry the top of a 64-bit value, so the high digits
  // come off first, while there are any.
  if (remaining >= 4294967296ull) {
    bool high = true;
    while (high) {
      --dest;
      StoreU8(ctx, dest, static_cast<uint32_t>(remaining % 10ull) | '0');
      high = remaining > 42949672959ull;
      remaining /= 10ull;
    }
  }
  uint32_t rest = static_cast<uint32_t>(remaining);
  while (rest != 0u) {
    --dest;
    StoreU8(ctx, dest, (rest % 10u) | '0');
    rest /= 10u;
  }
  LeaveFrame();
  return dest;
}

}  // namespace artc
