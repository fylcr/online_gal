#include "util/engine_context.h"

namespace artc {

// Copies n bytes of guest memory from src to dst and answers dst. Large blocks
// hand off to the host bulk copy; smaller ones walk 4-byte and 64-byte words
// when the pair shares an alignment, otherwise strided bytes, plus a byte tail.
uint32_t CopyBytes(EngineContext* ctx, uint32_t dst, uint32_t src, uint32_t n) {
  EnterFrame();
  if (n >= 512u) {
    // Big blocks: the host bulk copy beats the guest loop.
    HostBulkCopy(ctx, dst, src, n);
  } else {
    uint32_t const end = dst + n;
    uint32_t p = dst;
    if (((dst ^ src) & 3u) == 0u) {
      // Same alignment class: byte prologue up to a 4-byte boundary ...
      if (static_cast<int32_t>(n) >= 1 && (dst & 3u) != 0u) {
        for (;;) {
          StoreU8(ctx, p, LoadU8(ctx, src));
          src += 1u;
          p += 1u;
          if (end <= p || (p & 3u) == 0u) {
            break;
          }
        }
      }
      // ... then 64-byte phases and 4-byte phases over the word-aligned span.
      uint32_t const aligned_end = end & ~3u;
      if (aligned_end >= 64u && aligned_end - 64u >= p) {
        do {
          StoreU32(ctx, static_cast<uint64_t>(p) + 0u, LoadU32(ctx, static_cast<uint64_t>(src) + 0u));
          StoreU32(ctx, static_cast<uint64_t>(p) + 4u, LoadU32(ctx, static_cast<uint64_t>(src) + 4u));
          StoreU32(ctx, static_cast<uint64_t>(p) + 8u, LoadU32(ctx, static_cast<uint64_t>(src) + 8u));
          StoreU32(ctx, static_cast<uint64_t>(p) + 12u, LoadU32(ctx, static_cast<uint64_t>(src) + 12u));
          StoreU32(ctx, static_cast<uint64_t>(p) + 16u, LoadU32(ctx, static_cast<uint64_t>(src) + 16u));
          StoreU32(ctx, static_cast<uint64_t>(p) + 20u, LoadU32(ctx, static_cast<uint64_t>(src) + 20u));
          StoreU32(ctx, static_cast<uint64_t>(p) + 24u, LoadU32(ctx, static_cast<uint64_t>(src) + 24u));
          StoreU32(ctx, static_cast<uint64_t>(p) + 28u, LoadU32(ctx, static_cast<uint64_t>(src) + 28u));
          StoreU32(ctx, static_cast<uint64_t>(p) + 32u, LoadU32(ctx, static_cast<uint64_t>(src) + 32u));
          StoreU32(ctx, static_cast<uint64_t>(p) + 36u, LoadU32(ctx, static_cast<uint64_t>(src) + 36u));
          StoreU32(ctx, static_cast<uint64_t>(p) + 40u, LoadU32(ctx, static_cast<uint64_t>(src) + 40u));
          StoreU32(ctx, static_cast<uint64_t>(p) + 44u, LoadU32(ctx, static_cast<uint64_t>(src) + 44u));
          StoreU32(ctx, static_cast<uint64_t>(p) + 48u, LoadU32(ctx, static_cast<uint64_t>(src) + 48u));
          StoreU32(ctx, static_cast<uint64_t>(p) + 52u, LoadU32(ctx, static_cast<uint64_t>(src) + 52u));
          StoreU32(ctx, static_cast<uint64_t>(p) + 56u, LoadU32(ctx, static_cast<uint64_t>(src) + 56u));
          StoreU32(ctx, static_cast<uint64_t>(p) + 60u, LoadU32(ctx, static_cast<uint64_t>(src) + 60u));
          src += 64u;
          p += 64u;
        } while (aligned_end - 64u >= p);
      }
      if (p < aligned_end) {
        do {
          StoreU32(ctx, p, LoadU32(ctx, src));
          src += 4u;
          p += 4u;
        } while (aligned_end > p);
      }
    } else if (end >= 4u && end - 4u >= dst) {
      // Mismatched alignment: strided byte copies over the 4-byte span ...
      do {
        StoreU8(ctx, static_cast<uint64_t>(p) + 0u, LoadU8(ctx, static_cast<uint64_t>(src) + 0u));
        StoreU8(ctx, static_cast<uint64_t>(p) + 1u, LoadU8(ctx, static_cast<uint64_t>(src) + 1u));
        StoreU8(ctx, static_cast<uint64_t>(p) + 2u, LoadU8(ctx, static_cast<uint64_t>(src) + 2u));
        StoreU8(ctx, static_cast<uint64_t>(p) + 3u, LoadU8(ctx, static_cast<uint64_t>(src) + 3u));
        src += 4u;
        p += 4u;
      } while (end - 4u >= p);
    }
    // Byte tail: whatever the word phases left behind.
    if (p < end) {
      do {
        StoreU8(ctx, p, LoadU8(ctx, src));
        src += 1u;
        p += 1u;
      } while (end != p);
    }
  }
  LeaveFrame();
  return dst;
}

}  // namespace artc
