#include "util/engine_context.h"

namespace artc {

// Skips ASCII whitespace -- space and the control range tab through carriage
// return -- then reads an optional sign and the digits behind it. Answers the
// number as a 32-bit two's complement value, zero when no digits follow.
uint32_t SkipWhitespace(EngineContext* ctx, uint32_t text) {
  EnterFrame();
  uint32_t cursor = text;
  uint32_t byte = 0u;

  do {
    cursor = text;
    ++text;
    byte = static_cast<uint32_t>(LoadS8(ctx, cursor));
  } while (byte == ' ' || byte - 9u < 5u);

  byte = static_cast<uint32_t>(LoadS8(ctx, cursor));
  bool negative = false;
  const uint32_t marker = byte - '+';
  if (marker == 0u || marker == 2u) {  // a leading '+' or '-'
    if (marker == 2u) negative = true;
    byte = static_cast<uint32_t>(LoadS8(ctx, text));
    cursor = text;
  }

  // The digits accumulate negated, so one flag at the end picks the sign.
  uint32_t negated = 0u;
  if (byte - '0' < 10u) {
    do {
      negated = negated * 10u - static_cast<uint32_t>(LoadS8(ctx, cursor)) + '0';
      text = static_cast<uint32_t>(LoadS8(ctx, cursor + 1u));
      ++cursor;
    } while (text - '0' < 10u);
  }
  const uint32_t magnitude = 0u - negated;
  LeaveFrame();
  return negative ? negated : magnitude;
}

}  // namespace artc
