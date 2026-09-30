#include "util/engine_context.h"

namespace artc {

// Writes `value` in octal backwards: one digit per three bits, each stored in the
// byte below the previous one, and answers the start of the digit run. Callers
// pass the end of the buffer the digits are written into.
uint32_t FormatUnsignedOctal(EngineContext* ctx, uint64_t value, uint32_t end) {
  EnterFrame();
  while (value != 0) {
    --end;
    StoreU8(ctx, end, static_cast<uint32_t>(value & 7u) | '0');
    value >>= 3;
  }
  LeaveFrame();
  return end;
}

}  // namespace artc
