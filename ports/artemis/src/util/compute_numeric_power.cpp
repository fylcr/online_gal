#include "util/engine_context.h"
#include <cmath>

namespace artc {

// Forward declarations of the callees below.
double EncodeSignedOrderAsDouble(EngineContext* ctx, double a, uint32_t b);

namespace {

constexpr uint32_t kPowerTableBoundLow = 0x3988Fu;  // the lower log-table boundary
constexpr uint32_t kPowerScaleBias = 0x80000u;  // the scale factor's fixed bias
constexpr uint32_t kPowerTableBoundHigh = 0xBB67Au;  // the upper log-table boundary
constexpr uint32_t kPowerLogEntryHigh = 0xBF100u;  // the table row's high word
constexpr uint32_t kPowerLogEntryMid = 0xBF110u;  // the table row's middle word
constexpr uint32_t kPowerLogEntryLow = 0xBF120u;  // the table row's low word
constexpr uint32_t kPowerMantissaMask = 0xFFFFFu;  // the mantissa field
constexpr uint32_t kPowerImplicitBit = 0x100000u;  // the implicit mantissa bit
constexpr uint32_t kPowerScaleTopBit = 0x20000000u;  // the scale factor's top bit
constexpr uint32_t kPowerExpLowWord = 0x3F6F3400u;  // the low word of the 2^k boundary
constexpr uint32_t kPowerHalfHigh = 0x3FE00000u;  // the high word of 0.5
constexpr uint32_t kPowerHalfHighUp = 0x3FE00001u;  // the high word just above 0.5
constexpr uint32_t kPowerBelowOne = 0x3FEFFFFEu;  // the high word just below 1.0
constexpr uint32_t kPowerOneBelowHigh = 0x3FEFFFFFu;  // the field just below 1.0
constexpr uint32_t kPowerOneHigh = 0x3FF00000u;  // the high word of 1.0
constexpr uint32_t kPowerAboveOne = 0x3FF00001u;  // the high word just above 1.0
constexpr uint32_t kPowerValueMask = 0x3FFFFFFFu;  // the value mask of a high word
constexpr uint32_t kPowerTwoHigh = 0x40000000u;  // the high word of 2.0
constexpr uint32_t kPowerOverflowExponent = 0x40900000u;  // the exponent that overflows
constexpr uint32_t kPowerExpHighWord = 0x4090CC00u;  // the high word of the 2^k boundary
constexpr uint32_t kPowerHugeExponent = 0x41400000u;  // the exponent past the word shift
constexpr uint32_t kPowerOverflowLimit = 0x41E00001u;  // the exponent whose result overflows
constexpr uint32_t kPowerIntegerLimit = 0x433FFFFFu;  // the largest integral exponent
constexpr uint32_t kPowerBaseOverflow = 0x43F00001u;  // the base whose power overflows
constexpr uint32_t kPowerInfiniteHigh = 0x7FF00000u;  // the high word of infinity
constexpr uint32_t kPowerExponentMask = 0x7FFFFC00u;  // the exponent field of a product
constexpr uint32_t kPowerAbsMask = 0x7FFFFFFFu;  // the sign-bit mask
constexpr uint32_t kPowerFieldMask = 0xFFF00000u;  // the sign-extended field mask
constexpr uint32_t kPowerSubnormalLow = 0xFFFFFBCCu;  // the subnormal bias, low word
constexpr uint32_t kPowerSubnormalHigh = 0xFFFFFC01u;  // the subnormal bias, high word

}  // namespace

// Raises the base to the exponent for a numeric host call.
//
// The two operands are split into high and low words, the exponent is
// reduced to an integer and a fraction, the logarithm of the base is
// read out of the three-word table row and the exponential series is
// summed; sign and the integer exponent decide the final multiplier.
double ComputeNumericPower(EngineContext* ctx, double base, double exponent) {
  EnterFrame();
  uint64_t bits = 0;
  uint64_t base_bits = 0;
  double result = 0.0;
  double sign = 0.0;
  double log_high = 0.0;
  double delta = 0.0;
  double log_mid = 0.0;
  double log_low = 0.0;
  double reciprocal = 0.0;
  double squared = 0.0;
  uint32_t exponent_high_abs = 0u;
  uint32_t exponent_high = 0u;
  uint32_t base_high_abs = 0u;
  uint32_t base_high = 0u;
  uint32_t exponent_low = 0u;
  uint32_t exponent_biased = 0u;
  uint32_t integer_exponent = 0u;
  uint32_t base_low = 0u;
  uint64_t saved_bits = 0;
  double power = 0.0;
  double scale = 0.0;
  double product = 0.0;
  double saved_delta = 0.0;
  // The exponent is first reduced to an integer and a fraction of it.
  result = 1.0;
  bits = ReinterpretF64(exponent);
  exponent_high = static_cast<uint32_t>(bits >> (32ull & 63));
  exponent_high_abs = exponent_high & kPowerAbsMask;
  exponent_low = static_cast<uint32_t>(bits);
  if ((exponent_high_abs | exponent_low) == 0u) goto result_ready;
  base_bits = ReinterpretF64(base);
  base_high = static_cast<uint32_t>(base_bits >> (32ull & 63));
  base_low = static_cast<uint32_t>(base_bits);
  if (base_high == kPowerOneHigh && base_low == 0u) goto result_ready;
  base_high_abs = base_high & kPowerAbsMask;
  if (base_high_abs > kPowerInfiniteHigh) goto sum_shortcut;
  if ((base_high_abs == kPowerInfiniteHigh) && (base_low != 0u)) goto sum_shortcut;
  if (exponent_high_abs > kPowerInfiniteHigh) goto sum_shortcut;
  if (exponent_low == 0u) goto classify_base;
  if (exponent_high_abs != kPowerInfiniteHigh) goto classify_base;
  sum_shortcut:;
  power = base + exponent;
  goto exit_path;
  // Non-integral exponent: fall through to the logarithm path with a parity of zero.
  classify_base:;
  if (static_cast<int32_t>(base_high) >= 0) goto noninteger_exponent;
  if (exponent_high_abs > kPowerIntegerLimit) goto parity_ready;
  if (exponent_high_abs < kPowerOneHigh) goto noninteger_exponent;
  exponent_biased = exponent_high_abs >> (20u & 31);
  if (exponent_high_abs >= kPowerHugeExponent) {
    integer_exponent = 1075u - exponent_biased;
    exponent_biased = exponent_low >> (integer_exponent & 31);
    if (exponent_low != (exponent_biased << (integer_exponent & 31))) goto parity_ready;
    integer_exponent = 2u - (exponent_biased & 1u);
    goto parity_ready;
  }
  if (exponent_low != 0u) goto magnitude_path;
  exponent_low = 1043u - exponent_biased;
  exponent_biased = exponent_high_abs >> (exponent_low & 31);
  if (exponent_high_abs != (exponent_biased << (exponent_low & 31))) goto base_specials;
  integer_exponent = 2u - (exponent_biased & 1u);
  goto base_specials;
  noninteger_exponent:;
  integer_exponent = 0u;
  parity_ready:;
  if (exponent_low != 0u) goto magnitude_path;
  base_specials:;
  if (exponent_high_abs == kPowerInfiniteHigh) {
    if ((base_low | (base_high_abs - kPowerOneHigh)) == 0u) goto result_ready;
    if (base_high_abs >= kPowerOneHigh) {
      power = (static_cast<int32_t>(exponent_high) >= 0 ? exponent : 0.0);
      goto exit_path;
    }
    power = (static_cast<int32_t>(exponent_high) >= 0 ? 0.0 : -(exponent));
    goto exit_path;
  }
  if (exponent_high_abs == kPowerOneHigh) {
    if (static_cast<int32_t>(exponent_high) >= 0) {
      power = base;
      goto exit_path;
    }
    power = 1.0 / base;
    goto exit_path;
  }
  if (exponent_high == kPowerTwoHigh) {
    power = base * base;
    goto exit_path;
  }
  if (static_cast<int32_t>(base_high) < 0) goto magnitude_path;
  if (exponent_high != kPowerHalfHigh) goto magnitude_path;
  power = std::sqrt(base);
  goto exit_path;
  magnitude_path:;
  result = std::fabs(base);
  if (base_low != 0u) goto sign_and_ranges;
  if (base_high_abs && ((base_high & kPowerValueMask) != kPowerOneHigh)) goto sign_and_ranges;
  result = (static_cast<int32_t>(exponent_high) < 0 ? (1.0 / result) : result);
  if (static_cast<int32_t>(base_high) >= 0) goto result_ready;
  if (((base_high_abs - kPowerOneHigh) | integer_exponent) == 0u) {
    base = result - result;
    power = base / base;
    goto exit_path;
  }
  power = (integer_exponent == 1u) ? -(result) : result;
  goto exit_path;
  sign_and_ranges:;
  sign = 1.0;
  if (static_cast<int32_t>(base_high) >= 0) goto range_checks;
  switch (integer_exponent) {
  case 0: goto nan_result;
  case 1: goto negative_result;
  default: goto range_checks;
  }
  nan_result:;
  base = base - base;
  power = base / base;
  goto exit_path;
  negative_result:;
  sign = -1.0;
  range_checks:;
  if (exponent_high_abs >= kPowerOverflowLimit) {
    if (exponent_high_abs >= kPowerBaseOverflow) {
      if (base_high_abs <= kPowerOneBelowHigh) {
        power = (static_cast<int32_t>(exponent_high) < 0 ? INFINITY : 0.0);
        goto exit_path;
      }
      power = (static_cast<int32_t>(exponent_high) > 0 ? INFINITY : 0.0);
      goto exit_path;
    }
    if (base_high_abs <= kPowerBelowOne) {
      power = (static_cast<int32_t>(exponent_high) < 0 ? ((sign * (1.0000000000000001e+300))
          * (1.0000000000000001e+300)) : ((sign * (1e-300)) * (1e-300)));
      goto exit_path;
    }
    if (base_high_abs >= kPowerAboveOne) {
      power = (static_cast<int32_t>(exponent_high) > 0 ? ((sign * (1.0000000000000001e+300))
          * (1.0000000000000001e+300)) : ((sign * (1e-300)) * (1e-300)));
      goto exit_path;
    }
    base = result - 1.0;
    result = base * 1.4426950216293335;
    log_mid = (base * (1.9259629911266175e-08)) + (((base * base) * (0.5 - (base * ((base
        * -0.25) + 0.33333333333333331)))) * -1.4426950408889634);
    base = f64_reinterpret_i64((ReinterpretF64((result + log_mid)) & 18446744069414584320ull));
    power = base - result;
    goto log_ready;
  }
  base = result * 9007199254740992.0;
  exponent_high_abs = (base_high_abs < kPowerImplicitBit);
  result = exponent_high_abs ? base : result;
  base_high_abs = (exponent_high_abs ? static_cast<uint32_t>(ReinterpretF64(base) >> (32ull
      & 63)) : base_high_abs);
  base_high = base_high_abs & kPowerMantissaMask;
  exponent_high = base_high | kPowerOneHigh;
  base_high_abs = static_cast<uint32_t>(static_cast<int32_t>(base_high_abs) >> (20u & 31))
      + (exponent_high_abs ? kPowerSubnormalLow : kPowerSubnormalHigh);
  exponent_high_abs = 0u;
  if (base_high < kPowerTableBoundLow) goto table_ready;
  if (base_high < kPowerTableBoundHigh) {
    exponent_high_abs = 1u;
    goto table_ready;
  }
  exponent_high = exponent_high + kPowerFieldMask;
  base_high_abs = base_high_abs + 1u;
  // Read the table row for the reduced base and split its logarithm in two.
  table_ready:;
  base_high = exponent_high_abs << (3u & 31);
  log_low = LoadF64(ctx, base_high + kPowerLogEntryLow);
  log_mid = f64_reinterpret_i64(((ReinterpretF64(result) & 4294967295ull)
      | (static_cast<uint64_t>(exponent_high) << (32ull & 63))));
  log_high = LoadF64(ctx, base_high + kPowerLogEntryHigh);
  delta = log_mid - log_high;
  reciprocal = 1.0 / (log_high + log_mid);
  result = delta * reciprocal;
  base = f64_reinterpret_i64((ReinterpretF64(result) & 18446744069414584320ull));
  squared = base * base;
  saved_delta = delta;
  delta = f64_reinterpret_i64(static_cast<uint64_t>(
      (((static_cast<int32_t>(exponent_high) >> (1u & 31)) | kPowerScaleTopBit) +
      (exponent_high_abs << (18u & 31)) + kPowerScaleBias) << (32ull & 63)));
  log_mid = reciprocal * ((saved_delta - (base * delta)) - (base * (log_mid - (delta - log_high))));
  scale = base;
  product = (result + base) * log_mid;
  base = result * result;
  // Sum the logarithmic series.
  log_high = product + ((base * base) * ((base * ((base * ((base * ((base * ((base
      * 0.20697501780033842) + 0.23066074577556175)) + 0.27272812380853401))
      + 0.33333332981837743)) + 0.42857142857855018)) + 0.59999999999999465));
  base = f64_reinterpret_i64((ReinterpretF64(((squared + 3.0) + log_high))
      & 18446744069414584320ull));
  delta = scale * base;
  result = (log_mid * base) + (result * (log_high - ((base - 3.0) - squared)));
  base = f64_reinterpret_i64((ReinterpretF64((delta + result)) & 18446744069414584320ull));
  log_high = base * 0.96179670095443726;
  log_mid = LoadF64(ctx, (base_high + kPowerLogEntryMid));
  result = static_cast<double>(static_cast<int32_t>(base_high_abs));
  base = f64_reinterpret_i64((ReinterpretF64(((log_low + (log_high + log_mid)) + result))
      & 18446744069414584320ull));
  power = ((base - result) - log_low) - log_high;
  log_ready:;
  log_high = power;
  log_low = f64_reinterpret_i64((bits & 18446744069414584320ull));
  result = base * log_low;
  base = ((log_mid - log_high) * exponent) + ((exponent - log_low) * base);
  exponent = result + base;
  bits = ReinterpretF64(exponent);
  exponent_high_abs = static_cast<uint32_t>(bits);
  exponent_high = static_cast<uint32_t>(bits >> (32ull & 63));
  if (static_cast<int32_t>(exponent_high) >= static_cast<int32_t>(kPowerOverflowExponent)) {
    if ((exponent_high_abs | (exponent_high - kPowerOverflowExponent)) != 0u) goto overflow_result;
    if ((base + (8.0085662595372941e-17)) <= (exponent - result)) goto renormalize;
    goto overflow_result;
  }
  if ((exponent_high & kPowerExponentMask) < kPowerExpHighWord) goto renormalize;
  if ((exponent_high_abs | (exponent_high + kPowerExpLowWord)) != 0u) goto underflow_result;
  if ((exponent - result) < base) goto renormalize;
  goto underflow_result;
  // Fold the result back into the exponent range and rescale by the sign.
  renormalize:;
  exponent_high_abs = 0u;
  log_high = sign;
  base_high_abs = exponent_high & kPowerAbsMask;
  if (base_high_abs >= kPowerHalfHighUp) {
    base_high_abs = exponent_high + (kPowerImplicitBit >> (((base_high_abs >> (20u & 31))
        - 1022u) & 31));
    base_high = (base_high_abs >> (20u & 31)) & 2047u;
    exponent_high_abs = ((base_high_abs & kPowerMantissaMask) | kPowerImplicitBit) >> ((1043u
        - base_high) & 31);
    exponent_high_abs = (static_cast<int32_t>(exponent_high) < 0 ? (0u - exponent_high_abs)
        : exponent_high_abs);
    result = result - f64_reinterpret_i64((static_cast<uint64_t>(base_high_abs
        & static_cast<uint32_t>((static_cast<int32_t>(kPowerFieldMask) >> ((base_high - 1023u)
        & 31)))) << (32ull & 63)));
    saved_bits = ReinterpretF64((base + result));
  } else {
    saved_bits = bits;
  }
  exponent = f64_reinterpret_i64((saved_bits & 18446744069414584320ull));
  sign = exponent * 0.69314718246459961;
  result = ((base - (exponent - result)) * 0.69314718055994529) + (exponent
      * (-1.904654299957768e-09));
  base = sign + result;
  exponent = base * base;
  delta = (exponent * (4.1381367970572385e-08)) + (-1.6533902205465252e-06);
  exponent = base - (exponent * ((exponent * ((exponent * ((exponent * delta)
      + (6.6137563214379344e-05))) + -0.0027777777777015593)) + 0.16666666666666602));
  delta = (base * exponent) / (exponent - 2.0);
  exponent = result - (base - sign);
  sign = base * exponent;
  base = (base - (delta - (exponent + sign))) + 1.0;
  bits = ReinterpretF64(base);
  exponent_high = static_cast<uint32_t>(bits >> (32ull & 63)) + (exponent_high_abs << (20u & 31));
  power = log_high;
  if (static_cast<int32_t>(exponent_high) <= static_cast<int32_t>(kPowerMantissaMask)) {
    scale = EncodeSignedOrderAsDouble(ctx, base, exponent_high_abs);
    goto scale_ready;
  }
  scale = f64_reinterpret_i64(((bits & 4294967295ull) | (static_cast<uint64_t>(exponent_high)
      << (32ull & 63))));
  scale_ready:;
  result = power * scale;
  result_ready:;
  power = result;
  goto exit_path;
  overflow_result:;
  power = (sign * (1.0000000000000001e+300)) * (1.0000000000000001e+300);
  goto exit_path;
  underflow_result:;
  power = (sign * (1e-300)) * (1e-300);
  exit_path:;
  LeaveFrame();
  return power;
}

}  // namespace artc
