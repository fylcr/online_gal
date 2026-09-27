// resize_value_stack.cpp — value stack depth change.
//
// ResizeValueStack moves the state's value stack to a new depth. A
// non-negative depth lands at exactly that many records from the base and
// clears the type field of every record the stack grows into (so fresh slots
// read as nil and stale payloads are unreachable). A negative depth retreats
// relative to the current top with -1 leaving it unchanged, which is how the
// caller-side "pop n-1" arithmetic naturally folds.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//
#include "util/engine_context.h"

namespace artc {

void ResizeValueStack(EngineContext* ctx, uint32_t state, uint32_t depth) {
  EnterFrame();
  if (static_cast<int32_t>(depth) >= 0) {
    uint32_t const limit = LoadU32(ctx, state + 12u) + (depth << 4);
    for (uint32_t rec = LoadU32(ctx, state + 8u); rec < limit; rec += 16u) {
      StoreU32(ctx, rec + 8u, 0u);
    }
    StoreU32(ctx, state + 8u, limit);
  } else {
    uint32_t const top = LoadU32(ctx, state + 8u);
    StoreU32(ctx, state + 8u, top + (depth << 4) + 16u);
  }
  LeaveFrame();
}

}  // namespace artc
