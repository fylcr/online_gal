#include "util/engine_context.h"

namespace artc {

// Counts the bytes before the first zero byte. An unaligned head is walked a byte
// at a time; the aligned body then reads one word per step and drops back to
// single bytes inside whichever word holds the terminator.
uint32_t StringLength(EngineContext* ctx, uint32_t text) {
  EnterFrame();
  uint32_t cursor = text;
  uint32_t base = 0u;
  uint32_t word = 0u;

  if ((cursor & 3u) != 0u) {
    if (LoadU8(ctx, cursor) == 0u) {
      LeaveFrame();
      return 0u;
    }
    for (;;) {
      ++cursor;
      if ((cursor & 3u) == 0u) break;
      if (LoadU8(ctx, cursor) == 0u) {
        LeaveFrame();
        return cursor - text;
      }
    }
  }

  for (;;) {
    base = cursor;
    cursor += 4u;
    word = LoadU32(ctx, base);
    // A zero byte borrows from its neighbour and shows up as a set high bit.
    if (((word ^ 0xFFFFFFFFu) & (word - 0x01010101u) & 0x80808080u) != 0u) break;
  }

  if ((word & 255u) == 0u) {
    LeaveFrame();
    return base - text;
  }
  do {
    word = LoadU8(ctx, base + 1u);
    ++base;
  } while (word != 0u);

  LeaveFrame();
  return base - text;
}

}  // namespace artc
