#include "util/engine_context.h"

namespace artc {

// Forward declarations -- the raw block pool, the tagged allocator and the byte
// copier.
uint32_t AllocateRawBlock(EngineContext* ctx, uint32_t a0);
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
uint32_t CopyBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t AllocateTaggedBlock(EngineContext* ctx, uint32_t header, uint32_t size);

namespace {

// The word the allocator publishes a rejected request's code in.
constexpr uint32_t kReallocateRawBlockErrorAddr = 0xCF7E0u;

}  // namespace

// Reallocates one raw block: a null block allocates fresh and a zero size frees,
// the rest is folded to the size the block header can carry, and the block is
// grown where it stands or moved and the old contents copied across. Returns the
// new block, or null when the request could not be served.
uint32_t ReallocateRawBlock(EngineContext* ctx, uint32_t block, uint32_t size) {
  EnterFrame();
  if (block == 0u) {
    LeaveFrame();
    return AllocateRawBlock(ctx, size);
  }
  if (size >= 0xFFFFFFC0u) {
    StoreU32(ctx, kReallocateRawBlockErrorAddr, 48u);
    LeaveFrame();
    return 0u;
  }
  // Short requests always take a sixteen-byte header; longer ones are rounded up
  // to the header's own granularity.
  uint32_t const aligned_size = size < 11u ? 16u : ((size + 11u) & 0xFFFFFFF8u);
  uint32_t const moved = AllocateTaggedBlock(ctx, block - 8u, aligned_size);
  if (moved) {
    LeaveFrame();
    return moved + 8u;
  }
  uint32_t const fresh = AllocateRawBlock(ctx, size);
  if (fresh == 0u) {
    LeaveFrame();
    return 0u;
  }
  // The header word in front of the old block: its low two bits pick the header
  // size, which the mask below folds in as a negative base.
  uint32_t const header_word = LoadU32(ctx, block - 4u);
  uint32_t const capacity = ((header_word & 3u) ? 0xFFFFFFFCu : 0xFFFFFFF8u) + (header_word & 0xFFFFFFF8u);
  // Only what the old block could hold is copied; the rest of the new one is
  // left as it was allocated.
  CopyBytes(ctx, fresh, block, capacity < size ? capacity : size);
  DeallocateRawBlock(ctx, block);
  LeaveFrame();
  return fresh;
}

}  // namespace artc
