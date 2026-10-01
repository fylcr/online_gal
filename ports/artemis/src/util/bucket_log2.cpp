#include "util/engine_context.h"

namespace artc {

// The 256-entry pool step table that sits beside the shared nil record.
constexpr uint32_t kBucketLog2TableAddr = kNilValueAddr + 24u;

// Maps a byte length to the pool step reserved for it: eight-bit steps advance
// the base offset, and the table beside the nil record supplies the step within
// the last group.
uint32_t BucketLog2(EngineContext* ctx, uint32_t value) {
  EnterFrame();
  uint32_t base = 0xFFFFFFFFu;
  uint32_t shifted = value;
  if (shifted >= 256u) {
    uint32_t more;
    do {
      base += 8u;
      more = shifted > 65535u;
      shifted >>= 8u;
    } while (more != 0u);
  }
  LeaveFrame();
  return base + LoadU8(ctx, shifted + kBucketLog2TableAddr);
}

}  // namespace artc
