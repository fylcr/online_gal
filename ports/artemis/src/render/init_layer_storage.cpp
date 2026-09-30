// init_layer_storage.cpp -- initialises the layer storage: the layer slots are zeroed and the storage chain is primed..
//
// InitLayerStorage -- initialises the layer storage: the layer slots are zeroed and the storage chain is primed.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 337651-337651 (f860); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);

namespace {

constexpr uint32_t kF860SaveFrameBytes = 28u;
constexpr uint32_t kF860ResumeWordBytes = 4u;

}  // namespace

void InitLayerStorage(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
      l9 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF860SaveFrameBytes;
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
    i2 = kF860ResumeWordBytes;
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
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i1 = 30u;
    i0 = DIV_U(i0, i1);
    l3 = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 4u);
    l1 = i0;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 8u);
    l6 = i1;
    i0 = i0 == i1;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    if (i0) {
      i0 = a;
      i1 = 20u;
      i0 += i1;
      l7 = i0;
      goto running_exit;
    }
    i0 = a;
    i1 = 20u;
    i0 += i1;
    l7 = i0;
    i0 = l3;
    i1 = 2u;
    i0 <<= (i1 & 31);
    i1 = l1;
    i0 += i1;
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    i1 = l5;
    i2 = l3;
    i3 = 30u;
    i2 *= i3;
    i1 -= i2;
    i2 = 136u;
    i1 *= i2;
    i0 += i1;
    l3 = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 20u);
    i1 = l5;
    i0 += i1;
    l5 = i0;
    i1 = 30u;
    i0 = DIV_U(i0, i1);
    l9 = i0;
    i1 = 2u;
    i0 <<= (i1 & 31);
    i1 = l1;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = l2;
    i1 = l5;
    i2 = l9;
    i3 = 30u;
    i2 *= i3;
    i1 -= i2;
    i2 = 136u;
    i1 *= i2;
    i0 += i1;
    l5 = i0;
    i1 = l3;
    i0 = i0 == i1;
    if (i0) {goto running_exit;}
  }
  loop_7: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0);
      l1 = i0;
    }
    i0 = l8;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l3;
      i1 = l1;
      i0 = CallTableT2(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l2 = i0;
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l2;
      l1 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 136u;
      i0 += i1;
      l3 = i0;
      i1 = l4;
      i1 = LoadU32(ctx, i1);
      i0 -= i1;
      i1 = 4080u;
      i0 = i0 == i1;
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 4u);
        l3 = i0;
        i0 = l4;
        i1 = 4u;
        i0 += i1;
        l4 = i0;
      }
      i0 = l3;
      i1 = l5;
      i0 = i0 != i1;
      l1 = i0;
      if (i0) {goto loop_7;}
    }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 8u);
    l6 = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 4u);
    l1 = i0;
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l6;
    i1 = l1;
    i0 -= i1;
    i1 = 2u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    l3 = i0;
    i1 = 2u;
    i0 = i0 > i1;
    if (i0) {
      loop_15: 
        i0 = l1;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
        i0 = a;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 4u);
        i2 = 4u;
        i1 += i2;
        l1 = i1;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = a;
        i0 = LoadU32(ctx, i0 + 8u);
        i1 = l1;
        i0 -= i1;
        i1 = 2u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        l3 = i0;
        i1 = 2u;
        i0 = i0 > i1;
        if (i0) {goto loop_15;}
    }
    i0 = 15u;
    l1 = i0;
    i0 = l3;
    i1 = 1u;
    i0 -= i1;
    switch (i0) {
      case 0: goto block_17;
      case 1: goto block_18;
      default: goto block_16;
    }
    block_18:;
    i0 = 30u;
    l1 = i0;
    block_17:;
    i0 = a;
    i1 = l1;
    StoreU32(ctx, i0 + 16u, i1);
    block_16:;
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
  i2 = kF860ResumeWordBytes;
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
