#include "util/engine_context.h"

namespace artc {

namespace {
// The digit table the formatter reads: sixteen uppercase digits followed by the
// case and prefix bytes the callers OR into the result.
constexpr uint32_t kHexDigitTable = 0xBFE60u;  // 0123456789ABCDEF-0X+0X 0X-0x+0x 0x
}  // namespace

// Writes `value` in hexadecimal backwards: one digit per four bits, indexed out of
// the table above with `or_mask` OR-ed into every byte, and answers the start of
// the digit run. Callers pass the end of the buffer the digits are written into.
uint32_t FormatUnsignedHex(EngineContext* ctx, uint64_t value, uint32_t end, uint32_t or_mask) {
  EnterFrame();
  while (value != 0) {
    --end;
    const uint32_t digit = LoadU8(ctx, kHexDigitTable + static_cast<uint32_t>(value & 15u));
    StoreU8(ctx, end, digit | or_mask);
    value >>= 4;
  }
  LeaveFrame();
  return end;
}

}  // namespace artc
