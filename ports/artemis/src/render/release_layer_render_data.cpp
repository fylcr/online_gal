// release_layer_render_data.cpp -- releases a layer's render data: the render chain and the buffers hanging off it are freed..
//
// ReleaseLayerRenderData -- releases a layer's render data: the render chain and the buffers hanging off it are freed.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 594997-594997 (f1408); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void Panic(EngineContext* ctx, uint32_t a0);

namespace {

constexpr uint32_t kF1408ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF1408ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF1408SaveFrameBytes = 20u;
constexpr uint32_t kF1408ResumeWordBytes = 4u;

}  // namespace

void ReleaseLayerRenderData(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1408SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 4u);
    l1 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = l5;
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
    i2 = kF1408ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 584u);
    l1 = i0;
    i0 = !(i0);
    l2 = i0;
  }
  i0 = l2;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 572u);
      l1 = i0;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 576u);
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l2 = i0;
    }
    i0 = l2;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      loop_10: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l1;
          l2 = i0;
          i0 = a;
          i0 = LoadU32(ctx, i0 + 492u);
          i1 = a;
          i1 = LoadU32(ctx, i1 + 504u);
          i2 = l1;
          i1 += i2;
          l1 = i1;
          i2 = 68u;
          i1 = DIV_U(i1, i2);
          l3 = i1;
          i2 = 2u;
          i1 <<= (i2 & 31);
          i0 += i1;
          i0 = LoadU32(ctx, i0);
          l4 = i0;
          i0 = l4;
          i1 = l1;
          i2 = l3;
          i3 = 68u;
          i2 *= i3;
          i1 -= i2;
          i2 = 60u;
          i1 *= i2;
          i0 += i1;
          l3 = i0;
          i1 = 0u;
          StoreU32(ctx, i0 + 48u, i1);
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 52u);
          l1 = i0;
          i0 = l3;
          i1 = 0u;
          StoreU32(ctx, i0 + 52u, i1);
          i0 = l1;
          i0 = !(i0);
          l4 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          if (i0) {goto block_12;}
          i0 = l1;
          i1 = l1;
          i1 = LoadU32(ctx, i1 + 4u);
          l3 = i1;
          i2 = 1u;
          i1 -= i2;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 != i1;
          l4 = i0;
          if (i0) {goto block_12;}
          i0 = l1;
          i0 = LoadU32(ctx, i0);
          l4 = i0;
          i0 = LoadU32(ctx, i0 + 8u);
          l3 = i0;
          i0 = kF1408ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l6;
        i0 = !(i0);
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l3;
          i1 = l1;
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
          i0 = kF1408ConstD015C;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          i0 = kF1408ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 == i1;
          if (i0) {goto block_3;}
          i0 = l1;
          i1 = l1;
          i1 = LoadU32(ctx, i1 + 8u);
          l3 = i1;
          i2 = 1u;
          i1 -= i2;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 != i1;
          l4 = i0;
          if (i0) {goto block_12;}
          i0 = l1;
          i0 = LoadU32(ctx, i0);
          l4 = i0;
          i0 = LoadU32(ctx, i0 + 12u);
          l3 = i0;
          i0 = kF1408ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l6;
        i1 = 1u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l3;
          i1 = l1;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 1u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF1408ConstD015C;
          i0 = LoadU32(ctx, i0);
          l1 = i0;
          i0 = kF1408ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l1;
          i1 = 1u;
          i0 = i0 == i1;
          l1 = i0;
          if (i0) {goto block_3;}
        }
        block_12:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          i1 = 1u;
          i0 += i1;
          l1 = i0;
          i0 = a;
          i0 = LoadU32(ctx, i0 + 576u);
          l4 = i0;
          i1 = l2;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          l2 = i0;
          if (i0) {goto loop_10;}
        }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = 0u;
      StoreU32(ctx, i0 + 580u, i1);
      i0 = a;
      j1 = 18446744073709551615ull;
      StoreU64(ctx, i0 + 572u, j1);
      goto func_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = 628u;
    i0 += i1;
    l4 = i0;
    i0 = l1;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 376u);
    l2 = i0;
  }
  i0 = l6;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l1;
    i1 = l4;
    i2 = l2;
    CallTableT0(ctx, i2, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = 0u;
    StoreU32(ctx, i0 + 584u, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 588u);
    l1 = i0;
    i0 = a;
    i1 = 0u;
    StoreU32(ctx, i0 + 588u, i1);
    i0 = l1;
    i0 = !(i0);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {goto block_23;}
    i0 = l1;
    i1 = l1;
    i1 = LoadU32(ctx, i1 + 4u);
    l2 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_23;}
    i0 = l1;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = kF1408ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l6;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = l1;
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
    i0 = kF1408ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF1408ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_3;}
    i0 = l1;
    i1 = l1;
    i1 = LoadU32(ctx, i1 + 8u);
    l2 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_23;}
    i0 = l1;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    l2 = i0;
    i0 = kF1408ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l6;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = l1;
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
    i0 = kF1408ConstD015C;
    i0 = LoadU32(ctx, i0);
    l1 = i0;
    i0 = kF1408ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l1;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_3;}
  }
  block_23:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = kF1408ConstFFFFFFFF;
    StoreU32(ctx, i0 + 572u, i1);
    goto func_exit;
  }
  block_3:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    a = i0;
    i0 = HostTempRet0(ctx);
  }
  i0 = l6;
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
  goto func_exit;
  spill_frame:;
  l5 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l5;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1408ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l5 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = l1;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l5;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l5;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l5;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 20u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
