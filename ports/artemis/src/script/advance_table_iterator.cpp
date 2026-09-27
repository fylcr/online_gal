// advance_table_iterator.cpp -- steps the table cursor to the next present element and answers whether one exists..
//
// AdvanceTableIterator -- steps the table cursor to the next present element and answers whether one exists.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 208054-208054 (f521); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t NextTableEntry(EngineContext* ctx, uint32_t state, uint32_t table, uint32_t key);

namespace {

constexpr uint32_t kF521Const27118 = 0x27118u;  // 
constexpr uint32_t kF521Const2B328 = 0x2B328u;  // 
constexpr uint32_t kF521ConstFFFFD8EE = 0xFFFFD8EEu;  // 
constexpr uint32_t kF521ConstFFFFD8F1 = 0xFFFFD8F1u;  // 
constexpr uint32_t kF521ConstFFFFFFF0 = 0xFFFFFFF0u;  // 
constexpr uint32_t kF521SaveFrameBytes = 16u;
constexpr uint32_t kF521ResumeWordBytes = 4u;

}  // namespace

uint32_t AdvanceTableIterator(EngineContext* ctx, uint32_t state, uint32_t table) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF521SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    table = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF521ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    l3 = i0;
    i0 = table;
    i1 = 1u;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 12u);
      i1 = table;
      i2 = 4u;
      i1 <<= (i2 & 31);
      i0 += i1;
      i1 = 16u;
      i0 -= i1;
      table = i0;
      i1 = kF521Const2B328;
      i2 = state;
      i2 = LoadU32(ctx, i2 + 8u);
      i3 = table;
      i2 = i2 > i3;
      i0 = i2 ? i0 : i1;
      goto running_exit;
    }
    i0 = table;
    i1 = kF521ConstFFFFD8F1;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = table;
      i2 = 4u;
      i1 <<= (i2 & 31);
      i0 += i1;
      goto running_exit;
    }
    i0 = table;
    i1 = 10002u;
    i0 += i1;
    switch (i0) {
      case 0: goto block_8;
      case 1: goto block_9;
      case 2: goto block_10;
      default: goto block_7;
    }
    block_10:;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = 96u;
    i0 += i1;
    goto running_exit;
    block_9:;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 20u);
    i0 = LoadU32(ctx, i0 + 4u);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    table = i0;
    i0 = state;
    i1 = 5u;
    StoreU32(ctx, i0 + 96u, i1);
    i0 = state;
    i1 = table;
    StoreU32(ctx, i0 + 88u, i1);
    i0 = state;
    i1 = 88u;
    i0 += i1;
    goto running_exit;
    block_8:;
    i0 = state;
    i1 = 72u;
    i0 += i1;
    goto running_exit;
    block_7:;
    i0 = kF521Const2B328;
    i1 = state;
    i1 = LoadU32(ctx, i1 + 20u);
    i1 = LoadU32(ctx, i1 + 4u);
    i1 = LoadU32(ctx, i1);
    l4 = i1;
    i2 = 0u;
    i3 = table;
    i2 -= i3;
    i3 = 4u;
    i2 <<= (i3 & 31);
    i1 += i2;
    i2 = kF521Const27118;
    i1 -= i2;
    i2 = l4;
    i2 = LoadU8(ctx, i2 + 7u);
    i3 = kF521ConstFFFFD8EE;
    i4 = table;
    i3 -= i4;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    running_exit:;
    i0 = LoadU32(ctx, i0);
    table = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = 16u;
    i0 -= i1;
    l4 = i0;
  }
  i0 = l2;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l3;
    i1 = table;
    i2 = l4;
    i0 = NextTableEntry(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    table = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i1 = state;
    i1 = LoadU32(ctx, i1 + 8u);
    i2 = 16u;
    i3 = kF521ConstFFFFFFF0;
    i4 = table;
    i2 = i4 ? i2 : i3;
    i1 += i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = table;
    goto func_exit;
  }
  TrapUnreachable();
  spill_frame:;
  l2 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF521ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l2 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l2;
  i1 = table;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l2;
  i1 = l4;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l2;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 16u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
