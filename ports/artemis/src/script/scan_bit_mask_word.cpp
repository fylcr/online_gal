#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
uint32_t FindStringLength(EngineContext* ctx, uint32_t value, uint32_t literal);
uint32_t FillBytes(EngineContext* ctx, uint32_t dst, uint32_t value, uint32_t length);

// Scans a value for its first byte that a pattern's byte set spells: a one-byte
// set is answered by the shared string scan, a longer one is turned into a
// 256-bit scratch map first and the value walked against it. Answers the byte's
// offset in the value, or the value's length when no byte matches.
uint32_t ScanBitMaskWord(EngineContext* ctx, uint32_t value, uint32_t pattern) {
  EnterFrame();
  uint32_t result = 0u;
  uint32_t const scratch = GuestStackSp(ctx) - 32u;
  GuestStackSp(ctx) = scratch;
  result = LoadS8(ctx, pattern);
  if (result == 0u || LoadU8(ctx, pattern + 1u) == 0u) {
    result = FindStringLength(ctx, value, result);
  } else {
    FillBytes(ctx, scratch, 0u, 32u);
    uint32_t byte = LoadU8(ctx, pattern);
    while (byte != 0u) {
      // One bit per byte of the set, inside the 32-byte map.
      const uint32_t word = scratch + ((byte >> 3u) & 28u);
      result = word;
      StoreU32(ctx, word, LoadU32(ctx, word) | (1u << (byte & 31u)));
      byte = LoadU8(ctx, pattern + 1u);
      pattern += 1u;
    }
    result = value;
    byte = LoadU8(ctx, value);
    if (byte != 0u) {
      pattern = value;
      for (;;) {
        const uint32_t word = scratch + ((byte >> 3u) & 28u);
        if (((LoadU32(ctx, word) >> (byte & 31u)) & 1u) != 0u) {
          result = pattern;
          break;
        }
        byte = LoadU8(ctx, pattern + 1u);
        pattern += 1u;
        result = pattern;
        if (byte == 0u) {
          break;
        }
      }
    }
  }
  GuestStackSp(ctx) = scratch + 32u;
  LeaveFrame();
  return result - value;
}

}  // namespace artc
