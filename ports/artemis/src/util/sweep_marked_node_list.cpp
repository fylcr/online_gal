// sweep_marked_node_list.cpp -- the first sweep list: the mark byte is cleared, the mask takes the cleared bit back out, and every live node on the chain gets its finaliser called through the table..
//
// SweepMarkedNodeList -- the first sweep list: the mark byte is cleared, the mask takes the cleared bit back out, and every live node on the chain gets its finaliser called through the table.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 2327430-2327588 (f4230); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF4230ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF4230SaveFrameBytes = 16u;
constexpr uint32_t kF4230ResumeWordBytes = 4u;

}  // namespace

void SweepMarkedNodeList(EngineContext* ctx, uint32_t pool) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF4230SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l1 = i0;
    i0 = LoadU32(ctx, i0);
    pool = i0;
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 4u);
    l3 = i0;
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 12u);
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF4230ResumeWordBytes;
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
    i0 = pool;
    i0 = LoadU32(ctx, i0 + 56u);
    l3 = i0;
    i0 = pool;
    i1 = 0u;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = pool;
    i1 = pool;
    i1 = LoadU32(ctx, i1 + 44u);
    i2 = l3;
    i3 = kF4230ConstFFFFFFFF;
    i2 ^= i3;
    i1 &= i2;
    StoreU32(ctx, i0 + 44u, i1);
    i0 = pool;
    i0 = LoadU32(ctx, i0 + 824u);
    l3 = i0;
    i1 = pool;
    i1 = LoadU32(ctx, i1 + 828u);
    i0 = i0 != i1;
    l4 = i0;
  }
  i0 = l4;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    loop_6: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 140u);
        l1 = i0;
      }
      i0 = l2;
      i0 = !(i0);
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l4;
        i1 = l1;
        CallTableT1(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 0u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i1 = 4u;
        i0 += i1;
        l3 = i0;
        i1 = pool;
        i1 = LoadU32(ctx, i1 + 828u);
        i0 = i0 != i1;
        l4 = i0;
        if (i0) {goto loop_6;}
      }
  }
  goto func_exit;
  spill_frame:;
  l2 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF4230ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l2 = i0;
  i1 = pool;
  StoreU32(ctx, i0, i1);
  i0 = l2;
  i1 = l3;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l2;
  i1 = l4;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l2;
  i1 = l1;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 16u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
