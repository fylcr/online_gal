#include "util/engine_context.h"

namespace artc {

uint32_t CopyBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

// Moves `count` bytes from `src` to `dst`, choosing a copy direction and a store
// width that keep an overlapping destination from clobbering bytes it has not
// read yet. Answers the destination, or the bulk copy's own result.
uint32_t MoveMemoryRange(EngineContext* ctx, uint32_t dst, uint32_t src, uint32_t count) {
  EnterFrame();
  uint32_t gap_limit = 0u;
  uint32_t mismatch = 0u;
  uint32_t cursor = 0u;
  uint32_t byte = 0u;
  uint32_t word = 0u;
  uint32_t copied = 0u;
  if (dst == src) {
    goto finish;
  }
  // The ranges cannot touch when the source sits at least `count` bytes above
  // the destination, or vice versa. Taking the wrapped negative of twice the
  // length folds both directions into a single comparison.
  gap_limit = 0u - (count << (1u & 31));
  if (src - dst - count <= gap_limit) {
    copied = CopyBytes(ctx, dst, src, count);
    LeaveFrame();
    return copied;
  }
  // The low pair of bits says whether word stores can ever line up at all.
  mismatch = (dst ^ src) & 3u;
  if (dst < src) {
    if (mismatch != 0u) {
      cursor = dst;
      goto byte_forward;
    }
    if ((dst & 3u) == 0u) {
      cursor = dst;
      goto word_forward;
    }
    // Copying upwards: consume the leading bytes until the destination is
    // word-aligned, since the source trails behind the writes.
    cursor = dst;
  align_forward:;
    if (count == 0u) {
      goto finish;
    }
    byte = LoadU8(ctx, src);
    StoreU8(ctx, cursor, byte);
    src += 1u;
    count -= 1u;
    cursor += 1u;
    if ((cursor & 3u) != 0u) {
      goto align_forward;
    }
    goto word_forward;
  } else {
    // Copying downwards: the tail goes first, so every byte is read before the
    // destination can have overwritten it.
    if (mismatch != 0u) {
      goto byte_backward;
    }
    if (((dst + count) & 3u) != 0u) {
    align_backward:;
      if (count == 0u) {
        goto finish;
      }
      count -= 1u;
      cursor = dst + count;
      byte = LoadU8(ctx, src + count);
      StoreU8(ctx, cursor, byte);
      if ((cursor & 3u) != 0u) {
        goto align_backward;
      }
    }
    if (count <= 3u) {
      goto byte_backward;
    }
  word_backward:;
    count -= 4u;
    cursor = dst + count;
    word = LoadU32(ctx, src + count);
    StoreU32(ctx, cursor, word);
    if (count > 3u) {
      goto word_backward;
    }
  byte_backward:;
    if (count == 0u) {
      goto finish;
    }
  byte_backward_loop:;
    count -= 1u;
    cursor = dst + count;
    byte = LoadU8(ctx, src + count);
    StoreU8(ctx, cursor, byte);
    if (count != 0u) {
      goto byte_backward_loop;
    }
    goto finish;
  }
word_forward:;
  // Whole words while four or more bytes remain, then the trailing bytes.
  if (count <= 3u) {
    goto byte_forward;
  }
word_forward_loop:;
  word = LoadU32(ctx, src);
  StoreU32(ctx, cursor, word);
  src += 4u;
  cursor += 4u;
  count -= 4u;
  if (count > 3u) {
    goto word_forward_loop;
  }
byte_forward:;
  if (count == 0u) {
    goto finish;
  }
byte_forward_loop:;
  byte = LoadU8(ctx, src);
  StoreU8(ctx, cursor, byte);
  cursor += 1u;
  src += 1u;
  count -= 1u;
  if (count != 0u) {
    goto byte_forward_loop;
  }
finish:;
  LeaveFrame();
  return dst;
}

}  // namespace artc
