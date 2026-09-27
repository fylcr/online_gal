// compare_bytes.cpp — bounded guest-memory byte comparison.
//
// CompareBytes walks two guest byte ranges in lockstep and reports the
// unsigned difference of the first mismatching pair. Callers treat the raw
// difference as a three-way ordering key (negative outcomes wrap as unsigned),
// so the walk must stop at the very first divergence and a zero result must
// mean "equal or empty". Neither range is assumed to be NUL-terminated: the
// count is the only thing that bounds the walk. Addresses advance with the
// guest's own wrapping arithmetic, so a range may legally cross the top of
// the address space.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//
#include "util/engine_context.h"

namespace artc {

uint32_t CompareBytes(EngineContext* ctx, uint32_t lhs, uint32_t rhs, uint32_t n) {
  EnterFrame();
  uint32_t result = 0;
  for (uint32_t i = 0; i < n; ++i) {
    uint32_t a = LoadU8(ctx, lhs + i);
    uint32_t b = LoadU8(ctx, rhs + i);
    if (a != b) {
      result = a - b;
      break;
    }
  }
  LeaveFrame();
  return result;
}

}  // namespace artc
