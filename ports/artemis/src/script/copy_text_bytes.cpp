#include "util/engine_context.h"

namespace artc {

// Forward declaration — defined in util/string_length.cpp.
uint32_t StringLength(EngineContext* ctx, uint32_t text);

// Appends the text behind `src` to the text behind `dest`, stopping at the
// source's terminator or after `max_bytes` bytes, and re-terminates the result.
// Answers `dest`.
uint32_t CopyTextBytes(EngineContext* ctx, uint32_t dest, uint32_t src, uint32_t max_bytes) {
  EnterFrame();
  uint32_t cursor = dest + StringLength(ctx, dest);
  uint32_t remaining = max_bytes;
  while (remaining != 0) {
    uint32_t const byte = LoadU8(ctx, src);
    if (byte == 0) {
      break;
    }
    StoreU8(ctx, cursor, byte);
    cursor += 1u;
    src += 1u;
    --remaining;
  }
  StoreU8(ctx, cursor, 0u);
  LeaveFrame();
  return dest;
}

}  // namespace artc
