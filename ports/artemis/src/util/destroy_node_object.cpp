// destroy_node_object.cpp -- destroys a node object: the payload is released, the header is cleared, and the storage returns to the pool..
//
// DestroyNodeObject -- destroys a node object: the payload is released, the header is cleared, and the storage returns to the pool.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 185795-185795 (f437); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void ReleaseNodeTree(EngineContext* ctx, uint32_t state, uint32_t node);
void InitNodeStorage(EngineContext* ctx, uint32_t a);

namespace {

constexpr uint32_t kF437ConstB5E84 = 0xB5E84u;  // 
constexpr uint32_t kF437ConstB5F40 = 0xB5F40u;  // 
constexpr uint32_t kF437ConstFFFFFFC0 = 0xFFFFFFC0u;  // 
constexpr uint32_t kF437ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF437SaveFrameBytes = 8u;
constexpr uint32_t kF437ResumeWordBytes = 4u;

}  // namespace

uint32_t DestroyNodeObject(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF437SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l1 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 4u);
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF437ResumeWordBytes;
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
    i0 = a;
    i1 = kF437ConstB5F40;
    StoreU32(ctx, i0 + 84u, i1);
    i0 = a;
    i1 = kF437ConstB5E84;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i0 = LoadS8(ctx, i0 + 123u);
    i1 = kF437ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 112u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = a;
    i1 = 100u;
    i0 += i1;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 104u);
    ReleaseNodeTree(ctx, i0, i1);
    i0 = a;
    i0 = LoadS8(ctx, i0 + 99u);
    i1 = kF437ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 88u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = a;
    i1 = 60u;
    i0 += i1;
    l1 = i0;
  }
  i0 = l2;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l1;
    InitNodeStorage(ctx, i0);
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = kF437ConstFFFFFFC0;
    i0 -= i1;
    i0 = LoadU32(ctx, i0);
    l1 = i0;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 68u);
    l2 = i1;
    i0 = i0 == i1;
    if (i0) {goto block_8;}
    loop_9: 
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
      i0 = l2;
      i1 = l1;
      i2 = 4u;
      i1 += i2;
      l1 = i1;
      i0 = i0 != i1;
      if (i0) {goto loop_9;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 68u);
    l1 = i0;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 64u);
    l2 = i1;
    i0 = i0 == i1;
    if (i0) {goto block_8;}
    i0 = a;
    i1 = l1;
    i2 = l2;
    i1 -= i2;
    i2 = 4u;
    i1 -= i2;
    i2 = 2u;
    i1 >>= (i2 & 31);
    i2 = kF437ConstFFFFFFFF;
    i1 ^= i2;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i2 = l1;
    i1 += i2;
    StoreU32(ctx, i0 + 68u, i1);
    block_8:;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 60u);
    l1 = i0;
    if (i0) {
      i0 = l1;
      DeallocateRawBlock(ctx, i0);
    }
    i0 = a;
    i0 = LoadS8(ctx, i0 + 59u);
    i1 = kF437ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 48u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = a;
    i0 = LoadS8(ctx, i0 + 39u);
    i1 = kF437ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 28u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = a;
    i0 = LoadS8(ctx, i0 + 15u);
    i1 = kF437ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 4u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = a;
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
  i2 = kF437ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l2 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l2;
  i1 = l1;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 8u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
