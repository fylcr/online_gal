// destroy_sound_object.cpp -- the CSoundObject teardown: releases the shared pointer and both owned maps before the storage is handed back..
//
// DestroySoundObject -- the CSoundObject teardown: releases the shared pointer and both owned maps before the storage is handed back.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 229805-229805 (f584); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void ReleaseMapFieldSubtree(EngineContext* ctx, uint32_t a);
void Panic(EngineContext* ctx, uint32_t a0);
void ReleaseSharedRef(EngineContext* ctx, uint32_t a0);
void ReleaseNodeSubtreeA(EngineContext* ctx, uint32_t a, uint32_t b);
void ReleaseNodeSubtreeB(EngineContext* ctx, uint32_t a, uint32_t b);

namespace {

constexpr uint32_t kF584Const90E80 = 0x90E80u;  // 
constexpr uint32_t kF584ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF584SaveFrameBytes = 20u;
constexpr uint32_t kF584ResumeWordBytes = 4u;

}  // namespace

uint32_t DestroySoundObject(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF584SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    l1 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l5 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 16u);
    l6 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF584ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  i1 = 0u;
  i2 = ResumePhase(ctx);
  i2 = !(i2);
  if (i2) {
    i2 = a;
    i3 = kF584Const90E80;
    StoreU32(ctx, i2, i3);
    i2 = a;
    i3 = 28u;
    i2 += i3;
    l5 = i2;
    i2 = a;
    i2 = LoadU32(ctx, i2 + 28u);
    l2 = i2;
    i3 = a;
    i4 = 32u;
    i3 += i4;
    l6 = i3;
    i2 = i2 == i3;
    l1 = i2;
  }
  i2 = l1;
  i0 = i2 ? i0 : i1;
  if (i0) {goto block_3;}
  loop_6: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      l1 = i0;
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 20u);
      l2 = i0;
      i0 = kF584ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1592u;
      i1 = l2;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF584ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF584ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_7;}
      i0 = l1;
      i0 = LoadU32(ctx, i0 + 4u);
      l3 = i0;
      i0 = !(i0);
      if (i0) {
        i0 = l1;
        i0 = LoadU32(ctx, i0 + 8u);
        l2 = i0;
        i0 = LoadU32(ctx, i0);
        i1 = l1;
        i0 = i0 == i1;
        if (i0) {goto block_11;}
        i0 = l1;
        i1 = 8u;
        i0 += i1;
        l1 = i0;
        loop_13: 
          i0 = l1;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          i1 = 8u;
          i0 += i1;
          l1 = i0;
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 8u);
          l2 = i0;
          i0 = LoadU32(ctx, i0);
          i1 = l3;
          i0 = i0 != i1;
          if (i0) {goto loop_13;}
        goto block_11;
      }
      loop_14: 
        i0 = l3;
        l2 = i0;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        if (i0) {goto loop_14;}
      block_11:;
      i0 = l2;
      i1 = l6;
      i0 = i0 != i1;
      l1 = i0;
      if (i0) {goto loop_6;}
      goto block_3;
    }
    block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l5;
    ReleaseMapFieldSubtree(ctx, i0);
    i0 = a;
    i1 = 16u;
    i0 += i1;
    l1 = i0;
    i0 = LoadU32(ctx, i0 + 4u);
    l3 = i0;
    i0 = l1;
    i1 = l3;
    ReleaseNodeSubtreeA(ctx, i0, i1);
    i0 = a;
    i1 = 8u;
    i0 += i1;
    a = i0;
  }
  i0 = l4;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    ReleaseSharedRef(ctx, i0);
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l4;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    Panic(ctx, i0);
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
  block_3:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 32u);
    ReleaseNodeSubtreeB(ctx, i0, i1);
    i0 = a;
    i1 = 16u;
    i0 += i1;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 20u);
    ReleaseNodeSubtreeA(ctx, i0, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 12u);
    l2 = i0;
    i0 = !(i0);
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    if (i0) {goto block_21;}
    i0 = l2;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 4u);
    l1 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l1;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_21;}
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    l1 = i0;
    i0 = kF584ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l4;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l1;
    i1 = l2;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF584ConstD015C;
    i0 = LoadU32(ctx, i0);
    l1 = i0;
    i0 = kF584ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l1;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_20;}
    i0 = l2;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 8u);
    l1 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l1;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_21;}
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    l1 = i0;
    i0 = kF584ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l4;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l1;
    i1 = l2;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF584ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF584ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_20;}
  }
  block_21:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    goto func_exit;
  }
  block_20:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    a = i0;
    i0 = HostTempRet0(ctx);
  }
  i0 = l4;
  i1 = 5u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 5u;
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
  i2 = kF584ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = l1;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l3;
  i1 = l5;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l3;
  i1 = l6;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 20u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
