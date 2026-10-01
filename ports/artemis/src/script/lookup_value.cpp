#include "util/engine_context.h"

namespace artc {

// Resolves `key` in the table's value store and answers the record that holds it:
// a flat array part for small positive keys, otherwise a 32-byte hash node
// compared by its f64 key. A miss answers the shared nil record.
uint32_t LookupValue(EngineContext* ctx, uint32_t table, uint32_t key) {
  EnterFrame();
  uint32_t result = kNilValueAddr;
  uint32_t const index = key - 1u;
  if (index < LoadU32(ctx, table + 28u)) {
    // Flat array part: key-1 lands inside the declared length.
    result = LoadU32(ctx, table + 12u) + (index << 4);
    LeaveFrame();
    return result;
  }
  double const num = static_cast<double>(static_cast<int32_t>(key));
  uint32_t node = 0u;
  if (key == 0u) {
    node = LoadU32(ctx, table + 16u);
  } else {
    // The bucket divisor is the mask forced odd, so the remainder never divides
    // by zero; a key of zero hashes to the first bucket.
    uint64_t const bits = ReinterpretF64(num);
    uint32_t const mixed = static_cast<uint32_t>(bits + (bits >> 32));
    uint32_t const shift = LoadU8(ctx, table + 7u);
    uint32_t const divisor = ((0xFFFFFFFFu << (shift & 31u)) ^ 0xFFFFFFFFu) | 1u;
    node = LoadU32(ctx, table + 16u) + ((mixed % divisor) << 5);
  }
  // Walk the chain; the first node is always visited, misses end at nil.
  for (uint32_t cur = node;;) {
    if (LoadU32(ctx, cur + 24u) == 3u && LoadF64(ctx, cur + 16u) == num) {
      result = cur;
      break;
    }
    uint32_t const next = LoadU32(ctx, cur + 28u);
    if (next == 0u) {
      break;
    }
    cur = next;
  }
  LeaveFrame();
  return result;
}

}  // namespace artc
