#include "util/engine_context.h"

namespace artc {

// Shifts the 128-bit pair (`hi`, `lo`) left by `bytes` bits and stores the halves
// to `dst` and `dst + 8`. A count with bit 6 set drops the low half entirely, and
// a count of zero writes the pair back unchanged.
void ShiftWideDigits(EngineContext* ctx, uint32_t dst, uint64_t hi, uint64_t lo, uint32_t bytes) {
  EnterFrame();
  if (bytes & 64u) {
    lo = hi << ((bytes - 64u) & 63u);
    hi = 0ull;
  } else if (bytes != 0u) {
    lo = (lo << (bytes & 63u)) | (hi >> ((64u - bytes) & 63u));
    hi = hi << (bytes & 63u);
  }
  StoreU64(ctx, dst, hi);
  StoreU64(ctx, dst + 8u, lo);
  LeaveFrame();
}

}  // namespace artc
