#include "util/engine_context.h"

namespace artc {

// Reads the run of decimal digits at the character cursor `cursor` points at,
// advances the cursor past them, and answers the number they spell (zero when the
// run is empty). A signed byte compare keeps every non-digit, including the bytes
// above ASCII, out of the run.
uint32_t ParseDecimalNumber(EngineContext* ctx, uint32_t cursor) {
  EnterFrame();
  uint32_t text = LoadU32(ctx, cursor);
  uint32_t value = 0;
  for (int32_t c = LoadS8(ctx, text); c >= '0' && c <= '9'; c = LoadS8(ctx, text)) {
    text += 1;
    StoreU32(ctx, cursor, text);
    value = value * 10u + static_cast<uint32_t>(c - '0');
  }
  LeaveFrame();
  return value;
}

}  // namespace artc
