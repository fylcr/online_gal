// acquire_block_from_free_list.cpp -- takes a block off the free list, splitting it when the request is smaller than the block..
//
// AcquireBlockFromFreeList -- takes a block off the free list, splitting it when the request is smaller than the block.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 2146707-2146707 (f3804); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
uint32_t CoalesceFreeBlockRange(EngineContext* ctx, uint32_t pool, uint32_t block, uint32_t size, uint32_t node, uint32_t limit);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
void UnlinkListNode(EngineContext* ctx, uint32_t node, uint32_t victim);

namespace {

constexpr uint32_t kF3804ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF3804ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF3804SaveFrameBytes = 24u;
constexpr uint32_t kF3804ResumeWordBytes = 4u;

}  // namespace

uint32_t AcquireBlockFromFreeList(EngineContext* ctx, uint32_t pool, uint32_t node, uint32_t size, uint32_t header) {
  EnterFrame();
  uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF3804SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    pool = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    size = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 12u);
    header = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 4u);
    node = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF3804ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l7 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    i1 = 16u;
    i0 -= i1;
    l6 = i0;
    GuestStackSp(ctx) = i0;
    i0 = pool;
    i1 = node;
    i2 = l6;
    i3 = 12u;
    i2 += i3;
    i3 = l6;
    i4 = 8u;
    i3 += i4;
    i4 = size;
    i0 = CoalesceFreeBlockRange(ctx, i0, i1, i2, i3, i4);
    size = i0;
    i0 = LoadU32(ctx, i0);
    node = i0;
    i0 = !(i0);
    l4 = i0;
  }
  i0 = l4;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l7;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 40u;
      i0 = AllocateStringBuffer(ctx, i0);
      l5 = i0;
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      node = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3804ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = node;
      i1 = 16u;
      i0 += i1;
      l4 = i0;
    }
    i0 = l7;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 873u;
      i1 = l4;
      i2 = header;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l5 = i0;
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3804ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3804ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 != i1;
      l4 = i0;
    }
    i0 = l4;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3804ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = node;
        i1 = 28u;
        i0 += i1;
        l4 = i0;
        i0 = header;
        i1 = 12u;
        i0 += i1;
        header = i0;
      }
      i0 = l7;
      i1 = 2u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 873u;
        i1 = l4;
        i2 = header;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        i0 = 2u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3804ConstD015C;
        i0 = LoadU32(ctx, i0);
        header = i0;
        i0 = kF3804ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = header;
        i1 = 1u;
        i0 = i0 != i1;
        if (i0) {goto block_8;}
        i0 = HostRet0(ctx);
        pool = i0;
        i0 = HostTempRet0(ctx);
        i0 = node;
        i0 = LoadS8(ctx, i0 + 27u);
        i1 = kF3804ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        size = i0;
        if (i0) {goto block_9;}
        i0 = node;
        i0 = LoadU32(ctx, i0 + 16u);
        size = i0;
        DeallocateRawBlock(ctx, i0);
        goto block_9;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      pool = i0;
      i0 = HostTempRet0(ctx);
      size = i0;
    }
    block_9:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = node;
      DeallocateRawBlock(ctx, i0);
      i0 = pool;
      HostOsRelease(ctx, i0);
      TrapUnreachable();
    }
    block_8:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = node;
      i1 = l6;
      i1 = LoadU32(ctx, i1 + 12u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = node;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = size;
      i1 = node;
      StoreU32(ctx, i0, i1);
      i0 = node;
      l4 = i0;
      i0 = pool;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0);
      header = i0;
      if (i0) {
        i0 = pool;
        i1 = header;
        StoreU32(ctx, i0, i1);
        i0 = size;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
      }
      i0 = pool;
      i0 = LoadU32(ctx, i0 + 4u);
      i1 = l4;
      UnlinkListNode(ctx, i0, i1);
      i0 = pool;
      i1 = pool;
      i1 = LoadU32(ctx, i1 + 8u);
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 8u, i1);
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i1 = 16u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    i0 = node;
    goto func_exit;
  }
  TrapUnreachable();
  spill_frame:;
  l5 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l5;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF3804ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l5 = i0;
  i1 = pool;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = node;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l5;
  i1 = size;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l5;
  i1 = header;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l5;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l5;
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
