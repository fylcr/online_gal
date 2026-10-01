#include "util/engine_context.h"

namespace artc {

uint32_t FindStringLength(EngineContext* ctx, uint32_t value, uint32_t literal);

// Tag test on a pool entry: answers the entry that `pool` and `key` resolve to
// when its tag byte matches the low byte of `key`, and zero when it does not.
uint32_t FindPoolEntryByTagByte(EngineContext* ctx, uint32_t pool, uint32_t key) {
  EnterFrame();
  const uint32_t entry = FindStringLength(ctx, pool, key);
  const uint32_t tag = LoadU8(ctx, entry);
  const uint32_t result = tag == (key & 0xFFu) ? entry : 0u;
  LeaveFrame();
  return result;
}

}  // namespace artc
