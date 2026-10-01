#include "util/engine_context.h"

namespace artc {

// Walks two NUL-terminated byte strings in step and answers the unsigned
// difference of the first pair that differs — or of the pair at the left
// string's terminator. Nothing beyond that first difference is read.
uint32_t CompareNulTerminatedStrings(EngineContext* ctx, uint32_t left, uint32_t right) {
  EnterFrame();
  uint32_t right_byte = LoadU8(ctx, right);
  uint32_t left_byte = LoadU8(ctx, left);
  if (left_byte != 0u && left_byte == right_byte) {
    for (;;) {
      right_byte = LoadU8(ctx, right + 1u);
      left_byte = LoadU8(ctx, left + 1u);
      if (left_byte == 0u || left_byte != right_byte) {
        break;
      }
      ++right;
      ++left;
    }
  }
  LeaveFrame();
  return left_byte - right_byte;
}

}  // namespace artc
