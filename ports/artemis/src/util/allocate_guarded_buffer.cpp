// allocate_guarded_buffer.cpp -- allocates one buffer under the allocator guard: the limit raise and the assert path sit behind the raw allocation..
//
// AllocateGuardedBuffer -- allocates one buffer under the allocator guard: the limit raise and the assert path sit behind the raw allocation.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 2512105-2512105 (f4579); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
void RaiseAllocLimitError(EngineContext* ctx, uint32_t a0);
void RaiseAllocatorAssert(EngineContext* ctx, uint32_t a0);

namespace {

constexpr uint32_t kF4579MsgAllocatorTAllocateSize = 0x8B974u;  // allocator<T>::allocate(size_t n) 'n' exceeds maximum supported size
constexpr uint32_t kF4579ConstAAAAAAA = 0xAAAAAAAu;  // 
constexpr uint32_t kF4579Const15555555 = 0x15555555u;  // 
constexpr uint32_t kF4579Const15555556 = 0x15555556u;  // 
constexpr uint32_t kF4579ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF4579SaveFrameBytes = 24u;
constexpr uint32_t kF4579ResumeWordBytes = 4u;

}  // namespace

void AllocateGuardedBuffer(EngineContext* ctx, uint32_t state, uint32_t size) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF4579SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 4u);
    size = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    l2 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 20u);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF4579ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l7 = i0;
  }
  i0 = ResumePhase(ctx);
  if (i0) {
    i0 = l6;
  } else {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 4u);
    i1 = state;
    i1 = LoadU32(ctx, i1);
    l3 = i1;
    i0 -= i1;
    i1 = 12u;
    i0 = I32_DIV_S(i0, i1);
    l4 = i0;
    i1 = 1u;
    i0 += i1;
    l5 = i0;
    i1 = kF4579Const15555556;
    i0 = i0 < i1;
  }
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i1 = state;
      i1 = LoadU32(ctx, i1 + 8u);
      i2 = l3;
      i1 -= i2;
      i2 = 12u;
      i1 = I32_DIV_S(i1, i2);
      l3 = i1;
      i2 = 1u;
      i1 <<= (i2 & 31);
      l6 = i1;
      i2 = l5;
      i3 = l6;
      i2 = i2 > i3;
      i0 = i2 ? i0 : i1;
      l5 = i0;
      i1 = kF4579Const15555555;
      i2 = l3;
      i3 = kF4579ConstAAAAAAA;
      i2 = i2 < i3;
      i0 = i2 ? i0 : i1;
      l3 = i0;
    }
    i0 = l3;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i1 = kF4579Const15555556;
        i0 = i0 >= i1;
        l2 = i0;
        if (i0) {goto block_5;}
        i0 = l3;
        i1 = 12u;
        i0 *= i1;
        l2 = i0;
      }
      i0 = l7;
      i0 = !(i0);
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i0 = AllocateStringBuffer(ctx, i0);
        l6 = i0;
        i0 = 0u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l6;
      } else {
        i0 = l2;
      }
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i1 = l4;
      i2 = 12u;
      i1 *= i2;
      i0 += i1;
      l5 = i0;
      i1 = size;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = l5;
      i1 = size;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = size;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = size;
      i1 = 0u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = l3;
      i2 = 12u;
      i1 *= i2;
      i0 += i1;
      l2 = i0;
      i0 = l5;
      i1 = 12u;
      i0 += i1;
      l4 = i0;
      i0 = state;
      i0 = LoadU32(ctx, i0 + 4u);
      size = i0;
      i1 = state;
      i1 = LoadU32(ctx, i1);
      l3 = i1;
      i0 = i0 == i1;
      if (i0) {goto running_exit;}
      loop_14: 
        i0 = l5;
        i1 = 12u;
        i0 -= i1;
        l5 = i0;
        i1 = size;
        i2 = 12u;
        i1 -= i2;
        size = i1;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0, j1);
        i0 = l5;
        i1 = size;
        i1 = LoadU32(ctx, i1 + 8u);
        StoreU32(ctx, i0 + 8u, i1);
        i0 = size;
        j1 = 0ull;
        StoreU64(ctx, i0, j1);
        i0 = size;
        i1 = 0u;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = size;
        i1 = l3;
        i0 = i0 != i1;
        if (i0) {goto loop_14;}
      i0 = state;
      i1 = l2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = state;
      i0 = LoadU32(ctx, i0 + 4u);
      l2 = i0;
      i0 = state;
      i1 = l4;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = state;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = state;
      i1 = l5;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = l3;
      i0 = i0 == i1;
      if (i0) {goto block_3;}
      loop_15: 
        i0 = l2;
        i1 = 12u;
        i0 -= i1;
        state = i0;
        i0 = l2;
        i1 = 1u;
        i0 -= i1;
        i0 = LoadS8(ctx, i0);
        i1 = kF4579ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = state;
          i0 = LoadU32(ctx, i0);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = state;
        l2 = i0;
        i0 = state;
        i1 = l3;
        i0 = i0 != i1;
        if (i0) {goto loop_15;}
      goto block_3;
    }
  }
  i0 = l7;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    RaiseAllocatorAssert(ctx, i0);
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    TrapUnreachable();
  }
  block_5:;
  i0 = l7;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = kF4579MsgAllocatorTAllocateSize;
    RaiseAllocLimitError(ctx, i0);
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    TrapUnreachable();
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i1 = l2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = state;
    i1 = l4;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = state;
    i1 = l5;
    StoreU32(ctx, i0, i1);
  }
  block_3:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    if (i0) {
      i0 = l3;
      DeallocateRawBlock(ctx, i0);
    }
  }
  goto func_exit;
  spill_frame:;
  l6 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l6;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF4579ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l6 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l6;
  i1 = size;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l6;
  i1 = l3;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l6;
  i1 = l2;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l6;
  i1 = l5;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l6;
  i1 = l4;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 24u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
