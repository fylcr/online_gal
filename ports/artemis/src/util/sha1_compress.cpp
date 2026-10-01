#include "util/engine_context.h"

namespace artc {

namespace {

// One left rotate of a 32-bit word.
uint32_t RotateLeft(uint32_t value, uint32_t bits) {
  return (value << bits) | (value >> (32u - bits));
}

}  // namespace

// Folds one sixty-four byte message block into the five chaining words at the head
// of the hash record, leaving them holding the incoming value plus whatever the
// eighty rounds added. Returns nothing.
void Sha1CompressBlock(EngineContext* ctx, uint32_t hash) {
  EnterFrame();
  // The message block sits directly behind the chaining words.
  uint32_t const block = hash + 20u;
  uint32_t schedule[80];
  // First sixteen words, each assembled big-endian from its four bytes with the
  // least significant byte read first.
  for (uint32_t i = 0; i != 16u; ++i) {
    uint32_t const base = block + (i << 2);
    uint32_t assembled = LoadU8(ctx, base + 3u);
    assembled |= LoadU8(ctx, base) << 24u;
    assembled |= LoadU8(ctx, base + 1u) << 16u;
    assembled |= LoadU8(ctx, base + 2u) << 8u;
    schedule[i] = assembled;
  }
  // Expand to eighty words: one left rotate per word over four earlier ones.
  for (uint32_t i = 16u; i != 80u; ++i) {
    uint32_t const mixed =
        schedule[i - 16u] ^ (schedule[i - 14u] ^ (schedule[i - 3u] ^ schedule[i - 8u]));
    schedule[i] = RotateLeft(mixed, 1u);
  }
  // Bind the round window to the chaining words; the incoming values are kept
  // beside them so the final store can feed forward.
  uint32_t const e_in = LoadU32(ctx, hash + 16u);
  uint32_t const d_in = LoadU32(ctx, hash + 12u);
  uint32_t const c_in = LoadU32(ctx, hash + 8u);
  uint32_t const b_in = LoadU32(ctx, hash + 4u);
  uint32_t const a_in = LoadU32(ctx, hash);
  uint32_t a = a_in, b = b_in, c = c_in, d = d_in, e = e_in;
  for (uint32_t i = 0; i != 80u; ++i) {
    // The round's message word is read before the round is chosen.
    uint32_t const word = schedule[i];
    uint32_t f, k;
    if (i <= 19u) {
      k = 0x5A827999u;
      f = (c & b) | (~b & d);
    } else if (i <= 39u) {
      k = 0x6ED9EBA1u;
      f = c ^ b ^ d;
    } else if (i <= 59u) {
      k = 0x8F1BBCDCu;
      f = (c & d) | (b & (c | d));
    } else {
      k = 0xCA62C1D6u;
      f = c ^ b ^ d;
    }
    uint32_t const tmp = word + (f + (RotateLeft(a, 5u) + e) + k);
    // Slide the window down one round; each read above still saw the previous
    // round's word, and the last three assignments rebuild it in place.
    e = d;
    d = c;
    c = RotateLeft(b, 30u);
    b = a;
    a = tmp;
  }
  StoreU32(ctx, hash + 16u, e + e_in);
  StoreU32(ctx, hash + 12u, d + d_in);
  StoreU32(ctx, hash + 8u, c + c_in);
  StoreU32(ctx, hash + 4u, b + b_in);
  StoreU32(ctx, hash, a + a_in);
  LeaveFrame();
}

}  // namespace artc
