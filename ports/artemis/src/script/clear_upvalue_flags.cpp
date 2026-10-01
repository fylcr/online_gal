#include "util/engine_context.h"

namespace artc {

// Clears the flags of one node and everything below it.
void ClearFlagsRecursive(EngineContext* ctx, uint32_t state, uint32_t node);

// Clears the flags of every local slot of one scope.
void ClearLocalFlags(EngineContext* ctx, uint32_t state);

// Clears the cached flags of the upvalues a closure holds: first the cell in its
// own list, then the one in its shared list, each descending one step further
// when that cell is still shared. The closure is marked as cleared afterwards.
void ClearUpvalueFlags(EngineContext* ctx, uint32_t state) {
  EnterFrame();
  uint32_t const closure = LoadU32(ctx, state + 16u);
  // The closure's own cached flag words go first.
  StoreU32(ctx, closure + 44u, 0u);
  StoreU64(ctx, closure + 36u, 0ull);
  uint32_t upvalue = LoadU32(ctx, closure + 112u);
  if ((LoadU8(ctx, upvalue + 5u) & 3u) != 0u) {
    ClearFlagsRecursive(ctx, closure, upvalue);
    upvalue = LoadU32(ctx, closure + 112u);  // the recursion may have replaced it
  }
  // Only a cell that is still shared is followed to its successor.
  if (static_cast<int32_t>(LoadU32(ctx, upvalue + 80u)) >= 4) {
    upvalue = LoadU32(ctx, upvalue + 72u);
    if ((LoadU8(ctx, upvalue + 5u) & 3u) != 0u) {
      ClearFlagsRecursive(ctx, closure, upvalue);
    }
  }
  uint32_t const record = LoadU32(ctx, state + 16u);
  if (static_cast<int32_t>(LoadU32(ctx, record + 104u)) >= 4) {
    uint32_t const shared = LoadU32(ctx, record + 96u);
    if ((LoadU8(ctx, shared + 5u) & 3u) != 0u) {
      ClearFlagsRecursive(ctx, closure, shared);
    }
  }
  ClearLocalFlags(ctx, closure);
  StoreU8(ctx, closure + 21u, 1u);
  LeaveFrame();
}

}  // namespace artc
