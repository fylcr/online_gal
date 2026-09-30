// copy_script_block.cpp -- the CScriptBlock copy constructor: clones the name string, deep-copies the tagged map through the host insert slot, copies the boxed member, and restores the map on the throw path..
//
// CopyScriptBlock -- the CScriptBlock copy constructor: clones the name string, deep-copies the tagged map through the host insert slot, copies the boxed member, and restores the map on the throw path.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 131754-131754 (f284); R1 transcribe-keep.
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
uint32_t AppendStringText(EngineContext* ctx, uint32_t text, uint32_t source);

namespace {

constexpr uint32_t kF284ConstB5F40 = 0xB5F40u;  // 
constexpr uint32_t kF284ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF284ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF284SaveFrameBytes = 32u;
constexpr uint32_t kF284ResumeWordBytes = 4u;

}  // namespace

uint32_t CopyScriptBlock(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF284SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 24u);
    l7 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 28u);
    l9 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 4u);
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF284ResumeWordBytes;
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
    i0 = a;
    i1 = kF284ConstB5F40;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 4u;
    i0 += i1;
    l4 = i0;
    i0 = a;
    i1 = 4u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l8;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l3;
    i1 = l4;
    i0 = AppendStringText(ctx, i0, i1);
    l2 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = 20u;
    i0 += i1;
    l5 = i0;
    j1 = 0ull;
    StoreU64(ctx, i0, j1);
    i0 = a;
    i1 = l5;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = a;
    i1 = 16u;
    i0 += i1;
    l7 = i0;
    i0 = b;
    i0 = LoadU32(ctx, i0 + 16u);
    l3 = i0;
    i1 = b;
    i2 = 20u;
    i1 += i2;
    l9 = i1;
    i0 = i0 != i1;
    l4 = i0;
  }
  i0 = l4;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    loop_9: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF284ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        l4 = i0;
        i1 = 16u;
        i0 += i1;
        l3 = i0;
      }
      i0 = l8;
      i1 = 1u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 2364u;
        i1 = l7;
        i2 = l5;
        i3 = l3;
        i4 = l3;
        i0 = HostInvokeIiiii(ctx, i0, i1, i2, i3, i4);
        l2 = i0;
        i0 = 1u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l2;
        l3 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF284ConstD015C;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        i0 = kF284ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_5;}
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 4u);
        l2 = i0;
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 8u);
          l3 = i0;
          i0 = LoadU32(ctx, i0);
          i1 = l4;
          i0 = i0 == i1;
          if (i0) {goto block_13;}
          i0 = l4;
          i1 = 8u;
          i0 += i1;
          l2 = i0;
          loop_15: 
            i0 = l2;
            i0 = LoadU32(ctx, i0);
            l4 = i0;
            i1 = 8u;
            i0 += i1;
            l2 = i0;
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 8u);
            l3 = i0;
            i0 = LoadU32(ctx, i0);
            i1 = l4;
            i0 = i0 != i1;
            if (i0) {goto loop_15;}
          goto block_13;
        }
        loop_16: 
          i0 = l2;
          l3 = i0;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          if (i0) {goto loop_16;}
        block_13:;
        i0 = l3;
        i1 = l9;
        i0 = i0 != i1;
        l4 = i0;
        if (i0) {goto loop_9;}
      }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF284ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 28u;
    i0 += i1;
    l4 = i0;
    i0 = a;
    i1 = 28u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l8;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 873u;
    i1 = l3;
    i2 = l4;
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
    i0 = kF284ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF284ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_5;}
    i0 = a;
    i1 = b;
    i1 = LoadU32(ctx, i1 + 40u);
    StoreU32(ctx, i0 + 40u, i1);
    i0 = a;
    goto func_exit;
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = l7;
    i1 = l5;
    i1 = LoadU32(ctx, i1);
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l6;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF284ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l6;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = a;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
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
  i2 = kF284ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l2 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l2;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l2;
  i1 = l3;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l2;
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l2;
  i1 = l5;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l2;
  i1 = l6;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l2;
  i1 = l7;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l2;
  i1 = l9;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 32u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
