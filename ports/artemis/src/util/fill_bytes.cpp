#include "util/engine_context.h"

namespace artc {

namespace {
// One byte broadcast into all four lanes of a word, and the mask that rounds a
// span down to a whole number of words.
constexpr uint32_t kFillBytesByteRepeat = 0x1010101u;
constexpr uint32_t kFillBytesWordMask = 0xFFFFFFFCu;
}  // namespace

// Writes the low byte of `value` across `length` bytes at `dst`: the two end
// bytes and the short-span ladder first, then the unaligned head and tail words,
// then whole words. Answers with the address it was given.
uint32_t FillBytes(EngineContext* ctx, uint32_t dst, uint32_t value, uint32_t length) {
  EnterFrame();
  if (length == 0u) {
    LeaveFrame();
    return dst;
  }
  uint32_t end = dst + length;
  StoreU8(ctx, end - 1u, value);
  StoreU8(ctx, dst, value);
  if (length < 3u) {
    LeaveFrame();
    return dst;
  }
  StoreU8(ctx, end - 2u, value);
  StoreU8(ctx, dst + 1u, value);
  StoreU8(ctx, end - 3u, value);
  StoreU8(ctx, dst + 2u, value);
  if (length < 7u) {
    LeaveFrame();
    return dst;
  }
  StoreU8(ctx, end - 4u, value);
  StoreU8(ctx, dst + 3u, value);
  if (length < 9u) {
    LeaveFrame();
    return dst;
  }
  // Past nine bytes the work is done a word at a time: the byte is spread over
  // a whole word, the head and tail words go in first, and the loop runs between
  // them.
  const uint32_t head_align = (0u - dst) & 3u;
  const uint32_t head = dst + head_align;
  value = (value & 255u) * kFillBytesByteRepeat;
  StoreU32(ctx, head, value);
  const uint32_t body = (length - head_align) & kFillBytesWordMask;
  const uint32_t tail = head + body;
  StoreU32(ctx, tail - 4u, value);
  if (body < 9u) {
    LeaveFrame();
    return dst;
  }
  StoreU32(ctx, head + 8u, value);
  StoreU32(ctx, head + 4u, value);
  StoreU32(ctx, tail - 8u, value);
  StoreU32(ctx, tail - 12u, value);
  if (body < 25u) {
    LeaveFrame();
    return dst;
  }
  StoreU32(ctx, head + 24u, value);
  StoreU32(ctx, head + 20u, value);
  StoreU32(ctx, head + 16u, value);
  StoreU32(ctx, head + 12u, value);
  StoreU32(ctx, tail - 16u, value);
  StoreU32(ctx, tail - 20u, value);
  StoreU32(ctx, tail - 24u, value);
  StoreU32(ctx, tail - 28u, value);
  // The word now goes over both halves of a double word, and the loop writes it
  // eight bytes at a time until fewer than thirty-two bytes are left.
  const uint32_t band = (head & 4u) | 24u;
  uint32_t remaining = body - band;
  if (remaining < 32u) {
    LeaveFrame();
    return dst;
  }
  uint64_t lane = static_cast<uint64_t>(value);
  lane = lane | (lane << (32u & 63u));
  uint32_t cursor = head + band;
  do {
    StoreU64(ctx, cursor + 24u, lane);
    StoreU64(ctx, cursor + 16u, lane);
    StoreU64(ctx, cursor + 8u, lane);
    StoreU64(ctx, cursor, lane);
    cursor += 32u;
    remaining -= 32u;
  } while (remaining > 31u);
  LeaveFrame();
  return dst;
}

}  // namespace artc
