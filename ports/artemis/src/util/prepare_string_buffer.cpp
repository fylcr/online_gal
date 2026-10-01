#include "util/engine_context.h"

namespace artc {

// Makes a string buffer ready to hold the requested byte count: a count that
// still fits is repacked out of the buffer's own inline bytes, and a longer one
// hands the buffer over to its external block. Answers the packed bytes, or
// 0xFFFFFFFF once the buffer has been handed over.
uint32_t PrepareStringBuffer(EngineContext* ctx, uint32_t buffer, uint32_t size) {
  EnterFrame();
  // One name per slot across every role it plays: the buffer's block word and
  // later the pointer its inline bytes run from, the bit offset of that run,
  // the wanted byte count and later the whole-word count, that count rounded up
  // to words, the buffer's own data pointer, and the packed bytes with the mask
  // they are taken with.
  uint32_t block = 0u;
  uint32_t inline_bytes = 0u;
  uint32_t offset = 0u;
  uint32_t wanted = 0u;
  uint32_t words = 0u;
  uint32_t rounded = 0u;
  uint32_t inline_data = 0u;
  uint32_t packed = 0u;
  uint32_t mask = 0u;
  if (size >= 33u) {
    block = LoadU32(ctx, buffer + 16u);
    goto use_external_block;
  }
  offset = LoadU32(ctx, buffer + 4u);
  wanted = offset + size;
  inline_data = LoadU32(ctx, buffer);
  block = LoadU32(ctx, buffer + 16u);
  // An inline run that starts before the block's last word still has room to
  // grow into; one that starts past its rounded word count does not.
  if (static_cast<int32_t>(inline_data) < static_cast<int32_t>(block - 4u)) {
    goto repack_inline;
  }
  rounded = static_cast<uint32_t>(static_cast<int32_t>(wanted + 7u) >> 3);
  if (static_cast<int32_t>(inline_data) > static_cast<int32_t>(block - rounded)) {
    goto use_external_block;
  }
  if (wanted == 0u) {
    LeaveFrame();
    return 0u;
  }
 repack_inline:;
  // The mask is indexed by the requested count and covers that many packed
  // bytes, which are read back little-endian from the buffer's byte pointer.
  mask = LoadU32(ctx, 0x18AD0u + (size << 2u));
  inline_bytes = LoadU32(ctx, buffer + 12u);
  packed = LoadU8(ctx, inline_bytes) >> (offset & 31u);
  if (static_cast<int32_t>(wanted) < 9) {
    goto store_layout;
  }
  packed |= LoadU8(ctx, inline_bytes + 1u) << ((8u - offset) & 31u);
  if (static_cast<int32_t>(wanted) < 17) {
    goto store_layout;
  }
  packed |= LoadU8(ctx, inline_bytes + 2u) << ((16u - offset) & 31u);
  if (static_cast<int32_t>(wanted) < 25) {
    goto store_layout;
  }
  packed |= LoadU8(ctx, inline_bytes + 3u) << ((24u - offset) & 31u);
  if (offset == 0u) {
    goto store_layout;
  }
  if (static_cast<int32_t>(wanted) < 33) {
    goto store_layout;
  }
  packed |= LoadU8(ctx, inline_bytes + 4u) << ((32u - offset) & 31u);
 store_layout:;
  StoreU32(ctx, buffer + 4u, wanted & 7u);
  words = I32_DIV_S(wanted, 8u);
  StoreU32(ctx, buffer, inline_data + words);
  StoreU32(ctx, buffer + 12u, words + inline_bytes);
  LeaveFrame();
  return packed & mask;
 use_external_block:;
  StoreU32(ctx, buffer + 4u, 1u);
  StoreU32(ctx, buffer, block);
  StoreU32(ctx, buffer + 12u, 0u);
  LeaveFrame();
  return 0xFFFFFFFFu;
}

}  // namespace artc
