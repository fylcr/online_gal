// clear_struct_fields.cpp -- clears the struct fields: the field words of the structure are zeroed..
//
// ClearStructFields -- clears the struct fields: the field words of the structure are zeroed.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 294668-294668 (f763); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF763ConstFFFFFF = 0xFFFFFFu;  // 
constexpr uint32_t kF763Const3F800000 = 0x3F800000u;  // 

}  // namespace

uint32_t ClearStructFields(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t i0 = 0u; uint32_t i1 = 0u;
  uint64_t j1 = 0;
  i0 = a;
  j1 = 0ull;
  StoreU64(ctx, i0 + 64u, j1);
  i0 = a;
  i1 = kF763Const3F800000;
  StoreU32(ctx, i0 + 60u, i1);
  i0 = a;
  j1 = 4575657222473777152ull;
  StoreU64(ctx, i0 + 52u, j1);
  i0 = a;
  i1 = 1u;
  StoreU8(ctx, i0 + 48u, i1);
  i0 = a;
  i1 = 0u;
  StoreU32(ctx, i0 + 44u, i1);
  i0 = a;
  j1 = 1065353216ull;
  StoreU64(ctx, i0 + 36u, j1);
  i0 = a;
  j1 = 4575657222473777152ull;
  StoreU64(ctx, i0 + 28u, j1);
  i0 = a;
  i1 = 1u;
  StoreU8(ctx, i0 + 24u, i1);
  i0 = a;
  j1 = 0ull;
  StoreU64(ctx, i0 + 16u, j1);
  i0 = a;
  j1 = 4895412796094676992ull;
  StoreU64(ctx, i0 + 8u, j1);
  i0 = a;
  j1 = 0ull;
  StoreU64(ctx, i0, j1);
  i0 = a;
  j1 = 24ull;
  StoreU64(ctx, i0 + 92u, j1);
  i0 = a;
  j1 = 34359738368ull;
  StoreU64(ctx, i0 + 100u, j1);
  i0 = a;
  j1 = 0ull;
  StoreU64(ctx, i0 + 108u, j1);
  i0 = a;
  j1 = 0ull;
  StoreU64(ctx, i0 + 72u, j1);
  i0 = a;
  j1 = 0ull;
  StoreU64(ctx, i0 + 80u, j1);
  i0 = a;
  i1 = 0u;
  StoreU32(ctx, i0 + 88u, i1);
  i0 = a;
  j1 = 0ull;
  StoreU64(ctx, i0 + 116u, j1);
  i0 = a;
  j1 = 0ull;
  StoreU64(ctx, i0 + 124u, j1);
  i0 = a;
  i1 = 0u;
  StoreU32(ctx, i0 + 132u, i1);
  i0 = a;
  j1 = 0ull;
  StoreU64(ctx, i0 + 140u, j1);
  i0 = a;
  i1 = kF763ConstFFFFFF;
  StoreU32(ctx, i0 + 136u, i1);
  i0 = a;
  j1 = 0ull;
  StoreU64(ctx, i0 + 148u, j1);
  i0 = a;
  i1 = 256u;
  StoreU16(ctx, i0 + 156u, i1);
  i0 = a;
  LeaveFrame();
  return i0;
}
}
