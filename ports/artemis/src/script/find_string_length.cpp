#include "util/engine_context.h"

namespace artc {

uint32_t StringLength(EngineContext* ctx, uint32_t text);

// Finds where the text ends: the address of the first NUL, or of the first stop
// byte when one is given. Scans a word at a time, falling back to a byte at a
// time to reach a word boundary and to pin down the exact hit.
uint32_t FindStringLength(EngineContext* ctx, uint32_t text, uint32_t stop_byte) {
  EnterFrame();
  uint32_t const stop = stop_byte & 0xFFu;
  if (stop == 0u) {
    // Only a NUL ends the text, so the plain length plus the start is already the
    // address of that NUL.
    uint32_t const length = StringLength(ctx, text);
    LeaveFrame();
    return length + text;
  }
  // Unaligned: walk a byte at a time until the cursor sits on a word boundary.
  if (text & 3u) {
    for (;;) {
      uint32_t const byte = LoadU8(ctx, text);
      if (byte == 0u || byte == stop) {
        LeaveFrame();
        return text;
      }
      text += 1u;
      if ((text & 3u) == 0u) {
        break;
      }
    }
  }
  uint32_t word = LoadU32(ctx, text);
  uint32_t broadcast = 0u;
  // A word holding a zero byte answers this test: subtracting one from every lane
  // underflows the zero lane and sets its high bit. The stop byte answers the same
  // test once it is broadcast into every lane and folded into the word.
  bool zero_byte = ((word ^ 0xFFFFFFFFu) & (word - 0x01010101u) & 0x80808080u) != 0u;
  if (!zero_byte) {
    broadcast = stop * 0x01010101u;
    for (;;) {
      word ^= broadcast;
      uint32_t const inverted = word ^ 0xFFFFFFFFu;
      if ((inverted & (word - 0x01010101u) & 0x80808080u) != 0u) {
        break;
      }
      word = LoadU32(ctx, text + 4u);
      text += 4u;
      if (((word - 0x01010101u) & (word ^ 0xFFFFFFFFu) & 0x80808080u) != 0u) {
        break;
      }
    }
  }
  // The word test only narrows the hit to four bytes; walk them to the exact one.
  for (;;) {
    uint32_t const at = text;
    uint32_t const byte = LoadU8(ctx, at);
    if (byte != 0u) {
      text = at + 1u;
      if (byte != stop) {
        continue;
      }
    }
    LeaveFrame();
    return at;
  }
}

}  // namespace artc
