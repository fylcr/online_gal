#include "util/engine_context.h"

namespace artc {

// Folds `mask` to the bit width of the field it describes: masks that already
// fit a five-bit index answer themselves, wider ones are halved into range and
// the width is rebuilt from the step count.
uint32_t MaskToBitWidth(EngineContext* ctx, uint32_t mask) {
  EnterFrame();
  uint32_t value = mask;
  uint32_t width;
  if (mask < 16u) {
    width = 8u;
  } else {
    width = 1u;
    uint32_t sum;
    do {
      ++width;
      sum = value + 1u;
      // The bound is tested on the sum, so a value at the top of the range
      // wraps to zero and ends the loop instead of halving forever.
      value = sum >> 1u;
    } while (sum > 31u);
    width <<= 3u;
  }
  LeaveFrame();
  return value < 8u ? value : ((value - 8u) | width);
}

}  // namespace artc
