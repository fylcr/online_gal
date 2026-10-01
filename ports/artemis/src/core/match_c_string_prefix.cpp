#include "util/engine_context.h"

namespace artc {

// Forward declaration — defined in util/compare_nul_terminated_strings.cpp.
uint32_t CompareNulTerminatedStrings(EngineContext* ctx, uint32_t left, uint32_t right);

// Compares two string handles for the same text. Equal handles match; otherwise
// the data words at +4 decide — compared as raw pointers when `allow_text_compare`
// is zero, and by walking both C strings when it is not.
uint32_t MatchCStringPrefix(EngineContext* ctx, uint32_t left, uint32_t right,
                            uint32_t allow_text_compare) {
  EnterFrame();
  uint32_t result;
  if (allow_text_compare == 0) {
    result = LoadU32(ctx, left + 4u) == LoadU32(ctx, right + 4u);
  } else if (left == right) {
    result = 1u;
  } else {
    result = CompareNulTerminatedStrings(ctx, LoadU32(ctx, left + 4u),
                                         LoadU32(ctx, right + 4u)) == 0;
  }
  LeaveFrame();
  return result;
}

}  // namespace artc
