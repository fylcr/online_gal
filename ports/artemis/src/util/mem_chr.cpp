#include "util/engine_context.h"

namespace artc {

// The libc byte search: walks the bytes in front of the first aligned address,
// then probes four bytes at a time with the has-zero trick, and finishes
// byte-at-a-time for the tail. Returns the address of the first matching byte,
// or zero when the range runs out.
uint32_t MemChr(EngineContext* ctx, uint32_t cursor, uint32_t value, uint32_t count) {
  EnterFrame();
  // The byte being looked for, the word built from it, and the word the probe is
  // currently on.
  uint32_t byte = 0u;
  uint32_t broadcast = 0u;
  uint32_t mismatch = 0u;
  if (count == 0u) {
    goto scan_done;
  }
  if ((cursor & 3u) == 0u) {
    goto scan_done;
  }
  byte = value & 0xFFu;
  do {
    if (byte == LoadU8(ctx, cursor)) {
      goto probe_byte;
    }
    cursor += 1u;
    count -= 1u;
    if (count == 0u) {
      goto scan_done;
    }
  } while ((cursor & 3u) != 0u);
 scan_done:
  if (count == 0u) {
    goto answer_zero;
  }
 probe_byte:
  // The byte the walk stopped on is tested again here, so a match reached by the
  // alignment scan re-reads it.
  if (LoadU8(ctx, cursor) == (value & 0xFFu)) {
    goto scan_tail;
  }
  if (count < 4u) {
    goto scan_tail;
  }
  // The byte spread across four lanes, so a whole word can be compared at once.
  broadcast = (value & 0xFFu) * 0x1010101u;
  do {
    mismatch = LoadU32(ctx, cursor) ^ broadcast;
    // A matching lane leaves a zero byte behind; the subtraction borrows into
    // that lane's top bit, which is the bit the mask keeps.
    if (((mismatch ^ 0xFFFFFFFFu) & (mismatch - 0x1010101u) & 0x80808080u) != 0u) {
      goto scan_tail;
    }
    cursor += 4u;
    count -= 4u;
  } while (count > 3u);
 scan_tail:
  if (count == 0u) {
    goto answer_zero;
  }
  value &= 0xFFu;
  do {
    if (LoadU8(ctx, cursor) == value) {
      LeaveFrame();
      return cursor;
    }
    cursor += 1u;
    count -= 1u;
  } while (count != 0u);
 answer_zero:
  LeaveFrame();
  return 0u;
}

}  // namespace artc
