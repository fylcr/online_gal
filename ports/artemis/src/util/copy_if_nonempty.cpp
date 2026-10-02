// copy_if_nonempty.cpp — guarded guest memory block copy.
//
// CopyIfNonEmpty is the engine's zero-length-safe copy entry: callers pass a
// count that may legitimately be empty, and skipping the copy keeps the bulk
// path untouched. Any non-empty run goes straight to the block copy.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//
#include "util/engine_context.h"

namespace artc {

void CopyIfNonEmpty(EngineContext* ctx, uint32_t dst, uint32_t src, uint32_t n) {
  EnterFrame();
  if (n != 0u) {
    CopyBytes(ctx, dst, src, n);
  }
  LeaveFrame();
}

}  // namespace artc
