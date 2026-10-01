#include "util/engine_context.h"

namespace artc {

// The zero-length-safe copy entry: callers may pass an empty count, and skipping
// the copy keeps the bulk path untouched.
void CopyIfNonEmpty(EngineContext* ctx, uint32_t dst, uint32_t src, uint32_t n) {
  EnterFrame();
  if (n != 0u) {
    CopyBytes(ctx, dst, src, n);
  }
  LeaveFrame();
}

}  // namespace artc
