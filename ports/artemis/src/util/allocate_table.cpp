// allocate_table.cpp -- allocates a table object and initialises its array part..
//
// AllocateTable -- allocates a table object and initialises its array part.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 234039-234039 (f601); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void InitializeTableArray(EngineContext* ctx, uint32_t func_state, uint32_t table, uint32_t size);
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size);
uint32_t RaiseBlockTooBig(EngineContext* ctx, uint32_t a);
void RelocateObjectHeader(EngineContext* ctx, uint32_t state, uint32_t source, uint32_t tag);

namespace {

constexpr uint32_t kF601Const2BBD0 = 0x2BBD0u;  // 
constexpr uint32_t kF601ConstFFFFFFF = 0xFFFFFFFu;  // 
constexpr uint32_t kF601SaveFrameBytes = 24u;
constexpr uint32_t kF601ResumeWordBytes = 4u;

}  // namespace

uint32_t AllocateTable(EngineContext* ctx, uint32_t state, uint32_t entries, uint32_t array) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF601SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    array = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    entries = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF601ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l7 = i0;
  }
  i0 = l7;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = 0u;
    i2 = 0u;
    i3 = 32u;
    i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
    l3 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i1 = l4;
    i2 = 5u;
    RelocateObjectHeader(ctx, i0, i1, i2);
    i0 = l4;
    i1 = 0u;
    StoreU32(ctx, i0 + 28u, i1);
    i0 = l4;
    j1 = 0ull;
    StoreU64(ctx, i0 + 8u, j1);
    i0 = l4;
    i1 = kF601Const2BBD0;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = l4;
    i1 = 255u;
    StoreU16(ctx, i0 + 6u, i1);
    i0 = l4;
    l6 = i0;
    i0 = entries;
    i1 = 1u;
    i0 += i1;
    i1 = kF601ConstFFFFFFF;
    i0 = i0 <= i1;
    l5 = i0;
  }
  i0 = l5;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l5;
    i1 = entries;
    i2 = 4u;
    i1 <<= (i2 & 31);
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l5 = i0;
    i0 = l7;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = 0u;
      i2 = 0u;
      i3 = l5;
      i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
      l3 = i0;
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l3;
      l5 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_6;}
  }
  i0 = l7;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i0 = RaiseBlockTooBig(ctx, i0);
    l3 = i0;
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    l5 = i0;
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i1 = l5;
    StoreU32(ctx, i0 + 12u, i1);
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 28u);
    l6 = i0;
    i1 = entries;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {
      loop_12: 
        i0 = l5;
        i1 = l6;
        i2 = 4u;
        i1 <<= (i2 & 31);
        i0 += i1;
        i1 = 0u;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l6;
        i1 = 1u;
        i0 += i1;
        l6 = i0;
        i1 = entries;
        i0 = i0 != i1;
        if (i0) {goto loop_12;}
    }
    i0 = l4;
    i1 = entries;
    StoreU32(ctx, i0 + 28u, i1);
  }
  i0 = l7;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l4;
    i2 = array;
    InitializeTableArray(ctx, i0, i1, i2);
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    goto func_exit;
  }
  TrapUnreachable();
  spill_frame:;
  l3 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l3;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF601ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = entries;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = array;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l3;
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l3;
  i1 = l5;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l3;
  i1 = l6;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 24u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
