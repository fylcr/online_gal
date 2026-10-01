#include "util/engine_context.h"

namespace artc {

// Resets the field words of the structure at `structure`: the embedded
// sub-objects go back to their defaults and the whole block is rewritten from
// +0 up to +156. Answers the structure it was handed.
uint32_t ClearStructFields(EngineContext* ctx, uint32_t structure) {
  EnterFrame();
  StoreU64(ctx, structure + 64u, 0ull);
  StoreU32(ctx, structure + 60u, 0x3F800000u);  // 1.0f
  StoreU64(ctx, structure + 52u, 0x3F8000003F800000ull);  // two 1.0f
  StoreU8(ctx, structure + 48u, 1u);
  StoreU32(ctx, structure + 44u, 0u);
  StoreU64(ctx, structure + 36u, 0x3F800000ull);  // 1.0f, zero above
  StoreU64(ctx, structure + 28u, 0x3F8000003F800000ull);
  StoreU8(ctx, structure + 24u, 1u);
  StoreU64(ctx, structure + 16u, 0ull);
  StoreU64(ctx, structure + 8u, 0x43F0000044200000ull);  // 480.0f, 640.0f
  StoreU64(ctx, structure + 0u, 0ull);
  StoreU64(ctx, structure + 92u, 24ull);
  StoreU64(ctx, structure + 100u, 34359738368ull);  // eight in the high half
  StoreU64(ctx, structure + 108u, 0ull);
  StoreU64(ctx, structure + 72u, 0ull);
  StoreU64(ctx, structure + 80u, 0ull);
  StoreU32(ctx, structure + 88u, 0u);
  StoreU64(ctx, structure + 116u, 0ull);
  StoreU64(ctx, structure + 124u, 0ull);
  StoreU32(ctx, structure + 132u, 0u);
  StoreU64(ctx, structure + 140u, 0ull);
  StoreU32(ctx, structure + 136u, 0xFFFFFFu);
  StoreU64(ctx, structure + 148u, 0ull);
  StoreU16(ctx, structure + 156u, 256u);
  LeaveFrame();
  return structure;
}

}  // namespace artc
