#include "util/engine_context.h"

namespace artc {

uint32_t FindLastByte(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

// String wrapper over the backwards byte scan: resolves the string's data pointer
// and length -- both inline for the short form, a base pointer and a word for the
// long form -- and answers the offset the scan reports.
uint32_t FindLastCharInString(EngineContext* ctx, uint32_t str, uint32_t ch) {
  EnterFrame();
  // The high bit of the size byte tells the short form from the long one.
  const uint32_t data = (LoadU8(ctx, str + 11u) >> 7) ? LoadU32(ctx, str) : str;
  const uint32_t length =
      (LoadU8(ctx, str + 11u) >> 7) ? LoadU32(ctx, str + 4u) : LoadU8(ctx, str + 11u);
  const uint32_t result = FindLastByte(ctx, data, length, ch);
  LeaveFrame();
  return result;
}

}  // namespace artc
