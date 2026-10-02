// reallocate_raw_block.cpp -- reallocates one raw block: a null block allocates fresh, a zero size frees, and the request folds to the aligned size behind the block header..
//
// ReallocateRawBlock -- reallocates one raw block: a null block allocates fresh, a zero size frees, and the request folds to the aligned size behind the block header.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 175827-175827 (f399); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t AllocateRawBlock(EngineContext* ctx, uint32_t a0);
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
uint32_t CopyBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t AllocateTaggedBlock(EngineContext* ctx, uint32_t header, uint32_t size);

namespace {

constexpr uint32_t kF399ConstCF7E0 = 0xCF7E0u;  // 
constexpr uint32_t kF399ConstFFFFFFC0 = 0xFFFFFFC0u;  // 
constexpr uint32_t kF399ConstFFFFFFF8 = 0xFFFFFFF8u;  // 
constexpr uint32_t kF399ConstFFFFFFFC = 0xFFFFFFFCu;  // 

}  // namespace

uint32_t ReallocateRawBlock(EngineContext* ctx, uint32_t block, uint32_t size) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  i0 = block;
  i0 = !(i0);
  if (i0) {
    i0 = size;
    i0 = AllocateRawBlock(ctx, i0);
    goto func_exit;
  }
  i0 = size;
  i1 = kF399ConstFFFFFFC0;
  i0 = i0 >= i1;
  if (i0) {
    i0 = kF399ConstCF7E0;
    i1 = 48u;
    StoreU32(ctx, i0, i1);
    i0 = 0u;
    goto func_exit;
  }
  i0 = block;
  i1 = 8u;
  i0 -= i1;
  i1 = 16u;
  i2 = size;
  i3 = 11u;
  i2 += i3;
  i3 = kF399ConstFFFFFFF8;
  i2 &= i3;
  i3 = size;
  i4 = 11u;
  i3 = i3 < i4;
  i1 = i3 ? i1 : i2;
  i0 = AllocateTaggedBlock(ctx, i0, i1);
  l2 = i0;
  if (i0) {
    i0 = l2;
    i1 = 8u;
    i0 += i1;
    goto func_exit;
  }
  i0 = size;
  i0 = AllocateRawBlock(ctx, i0);
  l2 = i0;
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    goto func_exit;
  }
  i0 = kF399ConstFFFFFFFC;
  i1 = kF399ConstFFFFFFF8;
  i2 = block;
  i3 = 4u;
  i2 -= i3;
  i2 = LoadU32(ctx, i2);
  l3 = i2;
  i3 = 3u;
  i2 &= i3;
  i0 = i2 ? i0 : i1;
  l4 = i0;
  i0 = l4;
  i1 = l3;
  i2 = kF399ConstFFFFFFF8;
  i1 &= i2;
  i0 += i1;
  l3 = i0;
  i1 = size;
  i0 = i0 < i1;
  l4 = i0;
  i0 = l2;
  i1 = block;
  i2 = l3;
  i3 = size;
  i4 = l4;
  i2 = i4 ? i2 : i3;
  i0 = CopyBytes(ctx, i0, i1, i2);
  i0 = block;
  DeallocateRawBlock(ctx, i0);
  i0 = l2;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
