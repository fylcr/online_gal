#include "util/engine_context.h"

namespace artc {

// Encodes a wide character as at most four UTF-8 bytes, refusing the surrogate
// block; a null destination only asks for the byte count, and an unencodable
// character sets errno and answers (uint32_t)-1.
uint32_t WideCharToMultibyte(EngineContext* ctx, uint32_t dest, uint32_t wide, uint32_t state) {
  EnterFrame();
  // The locale's multibyte limit cell, the errno cell, and the encoding masks.
  uint32_t const kMbCurMaxAddr = 0xC8524u;
  uint32_t const kErrnoAddr = 0xCF7E0u;
  uint32_t const kCodeUnitMask = 0xFFFFFF80u;
  uint32_t const kSurrogateMask = 0xFFFFE000u;
  uint32_t const kPlaneLimit = 0xFFFFFu;
  uint32_t const kErrnoEilseq = 25u;
  if (dest == 0u) {
    LeaveFrame();
    return 1u;
  }
  if (wide <= 127u) {
    StoreU8(ctx, dest, wide);
    LeaveFrame();
    return 1u;
  }
  if (LoadU32(ctx, LoadU32(ctx, kMbCurMaxAddr)) == 0u) {
    // A single-byte locale: only a code unit goes through, as that one byte.
    if ((wide & kCodeUnitMask) == 0xDF80u) {
      StoreU8(ctx, dest, wide);
      LeaveFrame();
      return 1u;
    }
    StoreU32(ctx, kErrnoAddr, kErrnoEilseq);
    LeaveFrame();
    return 0xFFFFFFFFu;
  }
  if (wide <= 2047u) {
    // Two bytes, each written behind the one after it.
    StoreU8(ctx, dest + 1u, (wide & 63u) | 128u);
    StoreU8(ctx, dest, (wide >> 6u) | 192u);
    LeaveFrame();
    return 2u;
  }
  uint32_t const in_plain_plane = static_cast<uint32_t>((wide & kSurrogateMask) != 0xE000u);
  if (in_plain_plane == 0u || wide < 0xD800u) {
    // Three bytes -- the whole range except the surrogate block.
    StoreU8(ctx, dest + 2u, (wide & 63u) | 128u);
    StoreU8(ctx, dest, (wide >> 12u) | 224u);
    StoreU8(ctx, dest + 1u, ((wide >> 6u) & 63u) | 128u);
    LeaveFrame();
    return 3u;
  }
  if (wide - 0x10000u <= kPlaneLimit) {
    StoreU8(ctx, dest + 3u, (wide & 63u) | 128u);
    StoreU8(ctx, dest, (wide >> 18u) | 240u);
    StoreU8(ctx, dest + 2u, ((wide >> 6u) & 63u) | 128u);
    StoreU8(ctx, dest + 1u, ((wide >> 12u) & 63u) | 128u);
    LeaveFrame();
    return 4u;
  }
  StoreU32(ctx, kErrnoAddr, kErrnoEilseq);
  LeaveFrame();
  return 0xFFFFFFFFu;
}

}  // namespace artc
