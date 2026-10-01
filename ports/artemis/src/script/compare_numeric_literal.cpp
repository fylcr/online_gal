#include "util/engine_context.h"

namespace artc {

// The byte-range and integer-reading helpers this comparison leans on
// (util/compare_bytes.cpp, util/skip_whitespace.cpp).
uint32_t CompareBytes(EngineContext* ctx, uint32_t lhs, uint32_t rhs, uint32_t n);
uint32_t SkipWhitespace(EngineContext* ctx, uint32_t text);

namespace {

constexpr uint32_t kNumericLiteralSign = 45u;       // '-'
constexpr uint32_t kNumericLiteralDigitZero = 48u;  // '0'

}  // namespace

// Compares two numeric literals. When both are a bare integer -- their digit run
// covers the whole literal -- the parsed values are compared as signed 32-bit
// numbers; otherwise the raw byte ranges are, shorter first. Answers 1 when `lhs`
// sorts before `rhs`, 0 otherwise.
uint32_t CompareNumericLiteral(EngineContext* ctx, uint32_t state, uint32_t lhs, uint32_t rhs) {
  EnterFrame();

  // A literal hands out its total length either as a byte whose top bit is clear
  // or, for a long one, as a word behind that flagged byte; a long literal also
  // keeps its bytes in the word in front of that.
  uint32_t const lhs_long_len = LoadU32(ctx, lhs + 4u);
  uint32_t const lhs_tag = LoadU8(ctx, lhs + 11u);
  int32_t const lhs_sign = static_cast<int32_t>(lhs_tag << 24) >> 24;
  bool const lhs_is_long = lhs_sign < 0;
  uint32_t const lhs_len = lhs_is_long ? lhs_long_len : lhs_tag;

  uint32_t lhs_digits_ok = 0u;
  uint32_t lhs_word0 = 0u;
  uint32_t lhs_data = 0u;
  uint32_t rhs_tag = 0u;
  uint32_t rhs_tag_signed = 0u;
  uint32_t rhs_long_len = 0u;
  uint32_t rhs_word0 = 0u;
  uint32_t rhs_data = 0u;
  uint32_t rhs_len = 0u;
  uint32_t rhs_digits_ok = 0u;
  bool rhs_is_long = false;
  bool bytes_decide = false;

  if (lhs_len != 0u) {
    lhs_word0 = LoadU32(ctx, lhs);
    lhs_data = lhs_is_long ? lhs_word0 : lhs;
    lhs_digits_ok = 1u;
    if ((lhs_word0 & 0xFFu) == kNumericLiteralSign || lhs_is_long ||
        LoadU8(ctx, lhs) - kNumericLiteralDigitZero < 10u) {
      // A digit run starts at the head -- the literal opens with a minus, it is a
      // long literal, or its first byte is a digit -- so walk it. A leading minus
      // belongs to the literal and is taken without a digit test. `digits_ok` then
      // says the run stopped short of the literal's last byte.
      state = 0u;
      for (;;) {
        if (state == lhs_len) break;
        uint32_t const byte = LoadU8(ctx, state + lhs_data);
        if (state != 0u || byte != kNumericLiteralSign) {
          if (byte - kNumericLiteralDigitZero > 9u) break;
        }
        lhs_digits_ok = lhs_len > state + 1u;
        state += 1u;
      }
    }
    if (lhs_digits_ok != 0u) {
      // The left literal carries bytes beyond its digits, so the ranges decide.
      rhs_long_len = LoadU32(ctx, rhs + 4u);
      rhs_tag = LoadU8(ctx, rhs + 11u);
      rhs_tag_signed = rhs_tag;
      bytes_decide = true;
    }
  }

  if (!bytes_decide) {
    rhs_word0 = LoadU32(ctx, rhs);
    rhs_tag = LoadU8(ctx, rhs + 11u);
    rhs_tag_signed = static_cast<int32_t>(rhs_tag << 24) >> 24;
    rhs_is_long = rhs_tag_signed < 0;
    rhs_long_len = LoadU32(ctx, rhs + 4u);
    rhs_len = rhs_is_long ? rhs_long_len : rhs_tag;
    rhs_digits_ok = 1u;
    if (rhs_len != 0u) {
      rhs_is_long = rhs_tag_signed < 0;
      rhs_data = rhs_is_long ? rhs_word0 : rhs;
      state = 0u;
      if ((rhs_word0 & 0xFFu) == kNumericLiteralSign || rhs_is_long ||
          LoadU8(ctx, rhs) - kNumericLiteralDigitZero < 10u) {
        for (;;) {
          if (state == rhs_len) break;
          uint32_t const byte = LoadU8(ctx, state + rhs_data);
          if (state != 0u || byte != kNumericLiteralSign) {
            if (byte - kNumericLiteralDigitZero > 9u) break;
          }
          rhs_digits_ok = rhs_len > state + 1u;
          state += 1u;
        }
      }
      if (rhs_digits_ok != 0u) bytes_decide = true;
    }
  }

  if (bytes_decide) {
    // Compare as byte ranges: the first mismatching pair inside the common prefix
    // decides, and a range that is a prefix of the other sorts first.
    rhs_is_long = rhs_tag_signed < 0;
    rhs_len = rhs_is_long ? rhs_long_len : rhs_tag;
    bool const lhs_longer = lhs_len > rhs_len;
    uint32_t const common = lhs_longer ? rhs_len : lhs_len;
    uint32_t mismatch = 0u;
    if (common != 0u) {
      lhs_word0 = LoadU32(ctx, lhs);
      lhs_data = lhs_is_long ? lhs_word0 : lhs;
      rhs_word0 = LoadU32(ctx, rhs);
      rhs_data = rhs_is_long ? rhs_word0 : rhs;
      mismatch = CompareBytes(ctx, lhs_data, rhs_data, common);
    }
    if (mismatch == 0u) mismatch = rhs_len > lhs_len ? 0xFFFFFFFFu : lhs_longer;
    LeaveFrame();
    return mismatch >> 31;
  }

  // Both literals are a bare integer, so their parsed values decide.
  lhs_word0 = LoadU32(ctx, lhs);
  lhs_data = lhs_is_long ? lhs_word0 : lhs;
  uint32_t const lhs_value = SkipWhitespace(ctx, lhs_data);
  rhs_data = rhs_is_long ? rhs_word0 : rhs;
  uint32_t const rhs_value = SkipWhitespace(ctx, rhs_data);
  LeaveFrame();
  return static_cast<uint32_t>(static_cast<int32_t>(lhs_value) <
                               static_cast<int32_t>(rhs_value));
}

}  // namespace artc
