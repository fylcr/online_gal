// unordered_insert.cpp -- inserts one key/value pair into the unordered map: find, prepare the slot, and grow when the pair is new..
//
// UnorderedInsert -- inserts one key/value pair into the unordered map: find, prepare the slot, and grow when the pair is new.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 2135876-2135876 (f3784); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
uint32_t UnorderedPrepareSlot(EngineContext* ctx, uint32_t map, uint32_t key, uint32_t hash);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
uint32_t UnorderedFindEntry(EngineContext* ctx, uint32_t map, uint32_t hash, uint32_t key, uint32_t out);

namespace {

constexpr uint32_t kF3784ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF3784Const61C88647 = 0x61C88647u;  // 
constexpr uint32_t kF3784SaveFrameBytes = 28u;
constexpr uint32_t kF3784ResumeWordBytes = 4u;

}  // namespace

void UnorderedInsert(EngineContext* ctx, uint32_t map, uint32_t key, uint32_t value) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF3784SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    map = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 4u);
    key = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    value = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 16u);
    l6 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 20u);
    l7 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 24u);
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF3784ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l8 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = value;
    i0 = LoadU32(ctx, i0 + 4u);
    i1 = value;
    i1 = LoadU8(ctx, i1 + 11u);
    l3 = i1;
    i2 = l3;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    l3 = i2;
    i0 = i2 ? i0 : i1;
    l6 = i0;
    if (i0) {
      i0 = l6;
      i1 = value;
      i1 = LoadU32(ctx, i1);
      i2 = value;
      i3 = l3;
      i1 = i3 ? i1 : i2;
      l3 = i1;
      i0 += i1;
      l6 = i0;
      loop_5: 
        i0 = l7;
        i1 = l3;
        i1 = LoadS8(ctx, i1);
        i2 = l7;
        i3 = 6u;
        i2 <<= (i3 & 31);
        i3 = l7;
        i4 = 2u;
        i3 >>= (i4 & 31);
        i2 += i3;
        i1 += i2;
        i2 = kF3784Const61C88647;
        i1 -= i2;
        i0 ^= i1;
        l7 = i0;
        i0 = l6;
        i1 = l3;
        i2 = 1u;
        i1 += i2;
        l3 = i1;
        i0 = i0 != i1;
        if (i0) {goto loop_5;}
    }
    i0 = key;
    i1 = l7;
    i2 = value;
    i3 = key;
    i3 = LoadU8(ctx, i3);
    i4 = key;
    i3 += i4;
    i4 = 1u;
    i3 += i4;
    i0 = UnorderedFindEntry(ctx, i0, i1, i2, i3);
    l3 = i0;
    i0 = !(i0);
    l4 = i0;
  }
  i0 = l4;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l8;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 32u;
      i0 = AllocateStringBuffer(ctx, i0);
      l4 = i0;
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 24u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 16u, j1);
      i0 = l3;
      i1 = 8u;
      i0 += i1;
      l6 = i0;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = kF3784ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 873u;
      i1 = l6;
      i2 = value;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3784ConstD015C;
      i0 = LoadU32(ctx, i0);
      value = i0;
      i0 = kF3784ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = value;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_6;}
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0 + 20u, j1);
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 28u, i1);
      i0 = 1u;
      l5 = i0;
    }
    i0 = l8;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = key;
      i1 = l3;
      i2 = l7;
      i0 = UnorderedPrepareSlot(ctx, i0, i1, i2);
      l4 = i0;
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
    } else {
      i0 = l3;
    }
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = map;
    i1 = l5;
    StoreU8(ctx, i0 + 4u, i1);
    i0 = map;
    i1 = l3;
    StoreU32(ctx, i0, i1);
    goto func_exit;
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    map = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    DeallocateRawBlock(ctx, i0);
    i0 = map;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  goto func_exit;
  spill_frame:;
  l4 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l4;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF3784ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  i1 = map;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = key;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l4;
  i1 = value;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l4;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l4;
  i1 = l6;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l4;
  i1 = l7;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l4;
  i1 = l5;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 28u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
