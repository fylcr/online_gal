#include "util/engine_context.h"

namespace artc {

// Forward declaration — defined in util/compare_nul_terminated_strings.cpp.
uint32_t CompareNulTerminatedStrings(EngineContext* ctx, uint32_t left, uint32_t right);

// The identifier and keyword comparison: delegates to the NUL-terminated byte
// compare and passes the three-way difference through unchanged.
uint32_t CompareScriptStrings(EngineContext* ctx, uint32_t left, uint32_t right) {
  EnterFrame();
  uint32_t const result = CompareNulTerminatedStrings(ctx, left, right);
  LeaveFrame();
  return result;
}

}  // namespace artc
