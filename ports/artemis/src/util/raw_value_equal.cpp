#include "util/engine_context.h"

namespace artc {

// Raw equality on two value records: differing tag words are unequal; an equal
// double tag compares the two doubles, tag zero is equal outright, and any other
// tag compares the first word.
uint32_t RawValueEqual(EngineContext* ctx, uint32_t left, uint32_t right) {
  EnterFrame();
  const uint32_t tag = LoadU32(ctx, left + 8u);
  uint32_t equal = 0u;
  if (tag == LoadU32(ctx, right + 8u)) {
    if (tag == 3u) {
      equal = LoadF64(ctx, left) == LoadF64(ctx, right);
    } else if (tag != 0u) {
      equal = LoadU32(ctx, left) == LoadU32(ctx, right);
    } else {
      equal = 1u;
    }
  }
  LeaveFrame();
  return equal;
}

}  // namespace artc
