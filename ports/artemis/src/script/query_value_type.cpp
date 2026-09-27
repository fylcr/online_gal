// query_value_type.cpp — value type tag at a stack or pseudo index.
//
// QueryValueType reports the type tag stored with the value record the index
// addresses. An index that resolves to the shared nil slot reports the type of
// nothing (all bits set), which is what callers test before touching a value.
// Resolution itself is shared with the field push, environment record rebuild
// included.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//
#include "script/value_slot.h"
#include "util/engine_context.h"

namespace artc {

uint32_t QueryValueType(EngineContext* ctx, uint32_t state, uint32_t index) {
  EnterFrame();
  uint32_t const kTypeNone = static_cast<uint32_t>(-1);
  uint32_t result = kTypeNone;
  uint32_t const slot = ResolveValueSlot(ctx, state, index);
  if (slot != kNilValueAddr) {
    result = LoadU32(ctx, slot + 8u);
  }
  LeaveFrame();
  return result;
}

}  // namespace artc
