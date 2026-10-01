#include "util/engine_context.h"

namespace artc {

// Puts the stream into its writable state: the pending-read marker is cleared,
// the write cursor is pointed at the start of the buffer, and 0xFFFFFFFF reports
// a stream that is already marked unwritable.
uint32_t PrepareStreamWrite(EngineContext* ctx, uint32_t stream) {
  EnterFrame();
  const uint32_t pending = LoadU8(ctx, stream + 74u);
  StoreU8(ctx, stream + 74u, pending | (pending - 1u));

  const uint32_t flags = LoadU32(ctx, stream);
  if ((flags & 8u) != 0u) {
    StoreU32(ctx, stream, flags | 32u);
    LeaveFrame();
    return 0xFFFFFFFFu;
  }

  StoreU64(ctx, stream + 4u, 0ull);
  const uint32_t cursor = LoadU32(ctx, stream + 44u);
  StoreU32(ctx, stream + 28u, cursor);
  StoreU32(ctx, stream + 20u, cursor);
  const uint32_t room = LoadU32(ctx, stream + 48u);
  StoreU32(ctx, stream + 16u, room + cursor);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
