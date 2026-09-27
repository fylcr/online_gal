// clear_latched_flags.cpp — consume a latched flag mask.
//
// ClearLatchedFlags reads and clears the mask the record has latched at +52,
// then drops exactly those bits from its flag word at +40. The handle is
// relocated to its owning record through the descriptor delta behind the
// handle's first word, so either form of reference reaches the same record.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//
#include "util/engine_context.h"

namespace artc {

void ClearLatchedFlags(EngineContext* ctx, uint32_t handle) {
  EnterFrame();
  uint32_t const descriptor = LoadU32(ctx, handle);
  uint32_t const record = handle + LoadU32(ctx, descriptor - 148u);
  uint32_t const latched = LoadU32(ctx, record + 52u);
  StoreU32(ctx, record + 52u, 0u);
  StoreU32(ctx, record + 40u, LoadU32(ctx, record + 40u) & ~latched);
  LeaveFrame();
}

}  // namespace artc
