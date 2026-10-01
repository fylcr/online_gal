#include "util/engine_context.h"

namespace artc {

// Registered callee, defined in util/reverse_bits32.cpp.
uint32_t ReverseBits32(EngineContext* ctx, uint32_t value);

namespace {

// Table geometry plus the polynomial and the mask each index bit folds in.
constexpr uint32_t kInitCrc32TableInitialisedFlag = 0xCF224u;
constexpr uint32_t kInitCrc32TableTableBase = 0xCF230u;
constexpr uint32_t kInitCrc32TablePolynomial = 0x4C11DB7u;
constexpr uint32_t kInitCrc32TableXorBit5 = 0x9823B6Eu;
constexpr uint32_t kInitCrc32TableXorBit6 = 0x130476DCu;
constexpr uint32_t kInitCrc32TableHighSeed = 0x2608EDB8u;
constexpr uint32_t kInitCrc32TableTopBit = 0x80000000u;
constexpr uint32_t kInitCrc32TableMinusOne = 0xFFFFFFFFu;

}  // namespace

// Builds the 256-entry CRC-32 table on first use: for every index value the
// low byte is reflected, folded through the CRC-32 polynomial, and the result
// is stored bit-reversed at the slot that reflection selects. Answers nothing.
void InitCrc32Table(EngineContext* ctx) {
  EnterFrame();
  uint32_t index = 0u;
  uint32_t index_byte = 0u;
  uint32_t crc = 0u;
  uint32_t reflected = 0u;
  uint32_t table_slot = 0u;
  uint32_t top_byte = 0u;
  int32_t signed_index = 0;
  uint32_t high_seed = 0u;
  uint32_t xored = 0u;
  uint32_t shifted = 0u;
  uint32_t folded = 0u;
  uint32_t result = 0u;

  if (LoadU8(ctx, kInitCrc32TableInitialisedFlag) == 0u) {
    next_index:;
    // Reflect the low byte of the index; the shift counts stay masked to five
    // bits, the way the engine masks them.
    reflected = (index << (1u & 31u)) & 16u;
    reflected |= (index << (3u & 31u)) & 32u;
    reflected |= (index << (5u & 31u)) & 64u;
    reflected |= index << (7u & 31u);
    index_byte = index & 255u;
    reflected |= index_byte >> (7u & 31u);
    reflected |= (index_byte >> (1u & 31u)) & 8u;
    reflected |= (index_byte >> (3u & 31u)) & 4u;
    reflected |= (index_byte >> (5u & 31u)) & 2u;
    reflected &= 255u;
    // The finished table word is four bytes wide.
    table_slot = (reflected << (2u & 31u)) + kInitCrc32TableTableBase;

    // Seed the accumulator: an index byte that signs negative starts from the
    // high seed, anything else from zero.
    high_seed = kInitCrc32TableHighSeed;
    top_byte = index << (24u & 31u);
    signed_index = static_cast<int32_t>(top_byte) >> (24u & 31u);
    if (signed_index > static_cast<int32_t>(kInitCrc32TableMinusOne)) {
      crc = 0u;
    } else {
      crc = high_seed;
    }

    // Fold in the mask each index bit selects, then step the polynomial once
    // per bit.
    xored = crc ^ kInitCrc32TableXorBit6;
    if ((index & 64u) != 0u) {
      crc = xored;
    }
    xored = crc ^ kInitCrc32TableXorBit5;
    if ((index & 32u) != 0u) {
      crc = xored;
    }
    xored = crc ^ kInitCrc32TablePolynomial;
    if ((index & 16u) != 0u) {
      crc = xored;
    }
    xored = crc ^ kInitCrc32TableTopBit;
    if ((index & 8u) != 0u) {
      crc = xored;
    }
    // The polynomial step: the doubled word stands, unless the top bit was set
    // and the polynomial has to be folded back in.
    shifted = crc << (1u & 31u);
    folded = shifted ^ kInitCrc32TablePolynomial;
    crc = (static_cast<int32_t>(crc) > static_cast<int32_t>(kInitCrc32TableMinusOne))
              ? shifted
              : folded;
    xored = crc ^ kInitCrc32TableTopBit;
    if ((index & 4u) != 0u) {
      crc = xored;
    }
    shifted = crc << (1u & 31u);
    folded = shifted ^ kInitCrc32TablePolynomial;
    crc = (static_cast<int32_t>(crc) > static_cast<int32_t>(kInitCrc32TableMinusOne))
              ? shifted
              : folded;
    xored = crc ^ kInitCrc32TableTopBit;
    if ((index & 2u) != 0u) {
      crc = xored;
    }
    shifted = crc << (1u & 31u);
    folded = shifted ^ kInitCrc32TablePolynomial;
    crc = (static_cast<int32_t>(crc) > static_cast<int32_t>(kInitCrc32TableMinusOne))
              ? shifted
              : folded;
    xored = crc ^ kInitCrc32TableTopBit;
    if ((index & 1u) != 0u) {
      crc = xored;
    }
    shifted = crc << (1u & 31u);
    folded = shifted ^ kInitCrc32TablePolynomial;
    crc = (static_cast<int32_t>(crc) > static_cast<int32_t>(kInitCrc32TableMinusOne))
              ? shifted
              : folded;

    result = ReverseBits32(ctx, crc);
    StoreU32(ctx, table_slot, result);

    // The next index is built from the saved byte, and the loop ends when that
    // sum no longer fits in a byte.
    index = index_byte + 1u;
    if ((index & 255u) == index) {
      goto next_index;
    }
  }
  StoreU8(ctx, kInitCrc32TableInitialisedFlag, 1u);
  LeaveFrame();
}

}  // namespace artc
