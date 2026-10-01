#include "util/engine_context.h"

namespace artc {

// Scales a value by two raised to a signed order and answers the scaled value.
// The scaling happens in two steps so the exponent stays inside the double range,
// and whatever order is left over is folded into the result's exponent field.
double EncodeSignedOrderAsDouble(EngineContext* ctx, double value, uint32_t order) {
  EnterFrame();
  if (static_cast<int32_t>(order) >= 1024) {
    value *= 8.9884656743115795e+307;  // 2^1023
    if (static_cast<int32_t>(order) < 2047) {
      order -= 1023u;
    } else {
      value *= 8.9884656743115795e+307;
      // Orders past the double's top are clamped before the fold.
      order = static_cast<int32_t>(order) < 3069 ? order : 3069u;
      order -= 2046u;
    }
  } else if (static_cast<int32_t>(order) <= -1023) {
    value *= 2.2250738585072014e-308;  // 2^-1022
    if (static_cast<int32_t>(order) > -2045) {
      order += 1022u;
    } else {
      value *= 2.2250738585072014e-308;
      order = static_cast<int32_t>(order) > -3066 ? order : static_cast<uint32_t>(-3066);
      order += 2044u;
    }
  }
  uint32_t const exponent_bits = order + 1023u;
  value *= f64_reinterpret_i64(static_cast<uint64_t>(exponent_bits) << 52);
  LeaveFrame();
  return value;
}

}  // namespace artc
