#include "util/engine_context.h"

namespace artc {

void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
uint32_t ReallocateRawBlock(EngineContext* ctx, uint32_t block, uint32_t size);

// The resize shim under the allocator: a zero new size deallocates the block and
// answers zero, any other size hands it to the raw reallocator and answers that
// call.
uint32_t ResizeRawBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size,
                        uint32_t new_size) {
  EnterFrame();
  uint32_t result;
  if (new_size == 0u) {
    DeallocateRawBlock(ctx, block);
    result = 0u;
  } else {
    result = ReallocateRawBlock(ctx, block, new_size);
  }
  LeaveFrame();
  return result;
}

}  // namespace artc
