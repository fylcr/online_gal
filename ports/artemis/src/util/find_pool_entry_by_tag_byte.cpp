// find_pool_entry_by_tag_byte.cpp -- one lookup step through the pool chain, comparing the entry tag byte..
//
// FindPoolEntryByTagByte -- one lookup step through the pool chain, comparing the entry tag byte.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 137627-137627 (f309); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t FindStringLength(EngineContext* ctx, uint32_t value, uint32_t literal);

namespace {


}  // namespace

uint32_t FindPoolEntryByTagByte(EngineContext* ctx, uint32_t pool, uint32_t key) {
  EnterFrame();
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  i0 = pool;
  i1 = key;
  i0 = FindStringLength(ctx, i0, i1);
  pool = i0;
  i1 = 0u;
  i2 = pool;
  i2 = LoadU8(ctx, i2);
  i3 = key;
  i4 = 255u;
  i3 &= i4;
  i2 = i2 == i3;
  i0 = i2 ? i0 : i1;
  LeaveFrame();
  return i0;
}
}
