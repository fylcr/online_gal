// initialize_table_array.cpp -- sizes a table array for a constructor, raising the overflow error when the hash depth the size implies cannot fit the table..
//
// InitializeTableArray -- sizes a table array for a constructor, raising the overflow error when the hash depth the size implies cannot fit the table.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 795160-795459 (f1709); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t BucketLog2(EngineContext* ctx, uint32_t value);
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size);
uint32_t RaiseBlockTooBig(EngineContext* ctx, uint32_t a);
void RaiseScriptError(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

namespace {

constexpr uint32_t kF1709Const2BBD0 = 0x2BBD0u;  // 
constexpr uint32_t kF1709MsgTableOverflow = 0x2BBF0u;  // table overflow
constexpr uint32_t kF1709SaveFrameBytes = 20u;
constexpr uint32_t kF1709ResumeWordBytes = 4u;

}  // namespace

void InitializeTableArray(EngineContext* ctx, uint32_t func_state, uint32_t table, uint32_t size) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1709SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    func_state = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 4u);
    table = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    size = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    l5 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1709ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l6 = i0;
  }
  i0 = l3;
  i1 = size;
  i1 = !(i1);
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l3 = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    if (i0) {
      i0 = kF1709Const2BBD0;
      size = i0;
      i0 = table;
      i1 = kF1709Const2BBD0;
      StoreU32(ctx, i0 + 16u, i1);
      goto block_3;
    }
    i0 = size;
    i1 = 1u;
    i0 -= i1;
    i0 = BucketLog2(ctx, i0);
    size = i0;
    i1 = 1u;
    i0 += i1;
    l5 = i0;
    i0 = size;
    i1 = 26u;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    size = i0;
  }
  i0 = size;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l6;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = func_state;
      i1 = kF1709MsgTableOverflow;
      i2 = 0u;
      RaiseScriptError(ctx, i0, i1, i2);
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 1u;
    i1 = l5;
    i0 <<= (i1 & 31);
    l4 = i0;
    i0 = l5;
    i1 = 26u;
    i0 = i0 <= i1;
    size = i0;
  }
  i0 = size;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = size;
    i1 = l4;
    i2 = 5u;
    i1 <<= (i2 & 31);
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    size = i0;
    i0 = l6;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = func_state;
      i1 = 0u;
      i2 = 0u;
      i3 = size;
      i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
      l3 = i0;
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l3;
      size = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = table;
      i1 = size;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = table;
      i1 = 16u;
      i0 += i1;
      goto block_8;
    }
  }
  i0 = l6;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = func_state;
    i0 = RaiseBlockTooBig(ctx, i0);
    l3 = i0;
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    size = i0;
  }
  i0 = ResumePhase(ctx);
  if (i0) {
    i0 = func_state;
  } else {
    i0 = table;
    i1 = size;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = l5;
    i1 = 31u;
    i0 = i0 == i1;
    if (i0) {goto block_3;}
    i0 = table;
    i1 = 16u;
    i0 += i1;
  }
  block_8:;
  func_state = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = size;
    i1 = 0u;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = size;
    j1 = 0ull;
    StoreU64(ctx, i0 + 24u, j1);
    i0 = l4;
    i1 = 2u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {goto block_17;}
    i0 = l5;
    i0 = !(i0);
    if (i0) {goto block_17;}
    i0 = 1u;
    size = i0;
    loop_18: 
      i0 = func_state;
      i0 = LoadU32(ctx, i0);
      i1 = size;
      i2 = 5u;
      i1 <<= (i2 & 31);
      i0 += i1;
      l3 = i0;
      i1 = 0u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 24u, j1);
      i0 = l4;
      i1 = size;
      i2 = 1u;
      i1 += i2;
      size = i1;
      i0 = i0 != i1;
      if (i0) {goto loop_18;}
    block_17:;
    i0 = func_state;
    i0 = LoadU32(ctx, i0);
    size = i0;
  }
  block_3:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = table;
    i1 = l5;
    StoreU8(ctx, i0 + 7u, i1);
    i0 = table;
    i1 = size;
    i2 = l4;
    i3 = 5u;
    i2 <<= (i3 & 31);
    i1 += i2;
    StoreU32(ctx, i0 + 20u, i1);
  }
  goto func_exit;
  spill_frame:;
  l3 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l3;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1709ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = func_state;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = table;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = size;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l3;
  i1 = l5;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l3;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 20u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
