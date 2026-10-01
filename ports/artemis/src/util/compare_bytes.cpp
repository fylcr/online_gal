#include "util/engine_context.h"

namespace artc {

// Walks two guest byte ranges in lockstep and answers the unsigned difference of
// the first mismatching pair, zero when they are equal or empty. Neither range is
// assumed NUL-terminated: the count is the only bound.
uint32_t CompareBytes(EngineContext* ctx, uint32_t lhs, uint32_t rhs, uint32_t n) {
  EnterFrame();
  uint32_t result = 0;
  for (uint32_t i = 0; i < n; ++i) {
    uint32_t a = LoadU8(ctx, lhs + i);
    uint32_t b = LoadU8(ctx, rhs + i);
    if (a != b) {
      result = a - b;
      break;
    }
  }
  LeaveFrame();
  return result;
}

}  // namespace artc
