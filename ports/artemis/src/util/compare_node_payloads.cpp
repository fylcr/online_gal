#include "util/engine_context.h"

namespace artc {

// Compares two nodes by the payload word their first field points at and answers
// the three-way result (-1, 0 or 1). Nothing is written.
uint32_t CompareNodePayloads(EngineContext* ctx, uint32_t left, uint32_t right) {
  EnterFrame();
  uint32_t const lhs = LoadU32(ctx, LoadU32(ctx, left));
  uint32_t const rhs = LoadU32(ctx, LoadU32(ctx, right));
  LeaveFrame();
  return (lhs > rhs) - (lhs < rhs);
}

}  // namespace artc
