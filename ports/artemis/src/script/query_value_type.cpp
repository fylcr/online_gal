#include "script/value_slot.h"
#include "util/engine_context.h"

namespace artc {

// Reports the type tag stored with the value record `index` addresses. An index
// that resolves to the shared nil slot reports no type (all bits set), which is
// what callers test before touching a value.
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
