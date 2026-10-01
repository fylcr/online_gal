#include "util/engine_context.h"

namespace artc {

// Scans the `length` bytes at `start` backwards for `byte` and answers the index
// of the last one found, or -1 when it is absent. A zero length reads nothing.
uint32_t FindLastByte(EngineContext* ctx, uint32_t start, uint32_t length, uint32_t byte) {
  EnterFrame();
  uint32_t result = 0xFFFFFFFFu;
  if (length != 0u) {
    uint32_t end = start + length;
    while (start != end) {
      --end;
      if (LoadS8(ctx, end) == static_cast<int32_t>(byte)) {
        result = end - start;
        break;
      }
    }
  }
  LeaveFrame();
  return result;
}

}  // namespace artc
