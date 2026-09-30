// release_engine_frame_resources.cpp -- releases the engine frame resources: the per-frame pools and the view state are freed at the end of a frame..
//
// ReleaseEngineFrameResources -- releases the engine frame resources: the per-frame pools and the view state are freed at the end of a frame.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 1228605-1228605 (f2892); R1 transcribe-keep.
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

constexpr uint32_t kF2892MsgPx = 0xBCA76u;  // px != 0
constexpr uint32_t kF2892MsgBoostBoostSmartPtr = 0xBCA7Eu;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
constexpr uint32_t kF2892MsgOperator = 0xBCAB6u;  // operator->
constexpr uint32_t kF2892ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF2892SaveFrameBytes = 24u;
constexpr uint32_t kF2892ResumeWordBytes = 4u;

}  // namespace

void ReleaseEngineFrameResources(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  float f2 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF2892SaveFrameBytes;
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
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 16u);
    l6 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF2892ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 48u);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {goto block_6;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 24u);
    l2 = i0;
    i0 = !(i0);
    l1 = i0;
  }
  i0 = l1;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF2892ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l3;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF2892MsgPx;
      i2 = kF2892MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF2892MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF2892ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF2892ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_5;}
      goto running_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = 196u;
    i0 += i1;
    l6 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    l1 = i0;
  }
  i0 = l3;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = l6;
    f2 = 1.0f;
    i3 = l1;
    CallTableT13(ctx, i3, i0, i1, f2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 24u);
    l2 = i0;
    i0 = !(i0);
    l1 = i0;
  }
  i0 = l1;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF2892ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l3;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF2892MsgPx;
      i2 = kF2892MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF2892MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF2892ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF2892ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_5;}
      goto running_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 140u);
    l1 = i0;
  }
  i0 = l3;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = l1;
    CallTableT1(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 108u);
    l2 = i0;
    i1 = a;
    i2 = 112u;
    i1 += i2;
    l5 = i1;
    i0 = i0 != i1;
    l1 = i0;
  }
  i0 = l1;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    loop_26: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        l4 = i0;
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 28u);
        l2 = i0;
        i0 = !(i0);
        l1 = i0;
      }
      i0 = l1;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF2892ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 4u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF2892MsgPx;
          i2 = kF2892MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF2892MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 4u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF2892ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF2892ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_5;}
          goto running_exit;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        l1 = i0;
      }
      i0 = l3;
      i1 = 5u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l6;
        f2 = 1.0f;
        i3 = l1;
        CallTableT13(ctx, i3, i0, i1, f2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 5u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 28u);
        l2 = i0;
        i0 = !(i0);
        l1 = i0;
      }
      i0 = l1;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF2892ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 6u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF2892MsgPx;
          i2 = kF2892MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF2892MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 6u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF2892ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF2892ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_5;}
          goto running_exit;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 140u);
        l1 = i0;
      }
      i0 = l3;
      i1 = 7u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l1;
        CallTableT1(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 7u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 4u);
        l1 = i0;
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i1 = l4;
          i1 = LoadU32(ctx, i1 + 8u);
          l2 = i1;
          i1 = LoadU32(ctx, i1);
          i0 = i0 == i1;
          if (i0) {goto block_44;}
          i0 = l4;
          i1 = 8u;
          i0 += i1;
          l1 = i0;
          loop_46: 
            i0 = l1;
            i0 = LoadU32(ctx, i0);
            l4 = i0;
            i1 = 8u;
            i0 += i1;
            l1 = i0;
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 8u);
            l2 = i0;
            i0 = LoadU32(ctx, i0);
            i1 = l4;
            i0 = i0 != i1;
            l4 = i0;
            if (i0) {goto loop_46;}
          goto block_44;
        }
        loop_47: 
          i0 = l1;
          l2 = i0;
          i0 = LoadU32(ctx, i0);
          l1 = i0;
          if (i0) {goto loop_47;}
        block_44:;
        i0 = l2;
        i1 = l5;
        i0 = i0 != i1;
        l1 = i0;
        if (i0) {goto loop_26;}
      }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 76u);
    l2 = i0;
    i1 = a;
    i2 = 80u;
    i1 += i2;
    l4 = i1;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_6;}
  }
  loop_49: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      a = i0;
      i0 = LoadU8(ctx, i0 + 36u);
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
        i0 = LoadU32(ctx, i0 + 28u);
        l2 = i0;
        i0 = !(i0);
        l1 = i0;
      }
      i0 = l1;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF2892ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 8u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF2892MsgPx;
          i2 = kF2892MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF2892MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 8u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF2892ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF2892ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_5;}
          goto running_exit;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i1 = 4u;
        i0 += i1;
        l1 = i0;
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 4u);
        i0 = LoadU32(ctx, i0 + 12u);
        l2 = i0;
      }
      i0 = l3;
      i1 = 9u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l1;
        i1 = l6;
        f2 = 1.0f;
        i3 = l2;
        CallTableT13(ctx, i3, i0, i1, f2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 9u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 28u);
        l2 = i0;
        i0 = !(i0);
        l1 = i0;
      }
      i0 = l1;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF2892ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l3;
        i1 = 10u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF2892MsgPx;
          i2 = kF2892MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF2892MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 10u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF2892ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF2892ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_5;}
          goto running_exit;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 140u);
        l1 = i0;
      }
      i0 = l3;
      i1 = 11u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l1;
        CallTableT1(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 11u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 4u);
      l1 = i0;
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 8u);
        l2 = i0;
        i0 = LoadU32(ctx, i0);
        i1 = a;
        i0 = i0 == i1;
        l1 = i0;
        if (i0) {goto block_70;}
        i0 = a;
        i1 = 8u;
        i0 += i1;
        l1 = i0;
        loop_72: 
          i0 = l1;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i1 = 8u;
          i0 += i1;
          l1 = i0;
          i0 = a;
          i0 = LoadU32(ctx, i0 + 8u);
          l2 = i0;
          i0 = LoadU32(ctx, i0);
          i1 = a;
          i0 = i0 != i1;
          if (i0) {goto loop_72;}
        goto block_70;
      }
      loop_73: 
        i0 = l1;
        l2 = i0;
        i0 = LoadU32(ctx, i0);
        l1 = i0;
        if (i0) {goto loop_73;}
      block_70:;
      i0 = l2;
      i1 = l4;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto loop_49;}
    }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    goto func_exit;
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    TrapUnreachable();
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    a = i0;
    i0 = HostTempRet0(ctx);
  }
  i0 = l3;
  i1 = 12u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 12u;
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
  l3 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l3;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF2892ResumeWordBytes;
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
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l3;
  i1 = l6;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l3;
  i1 = l5;
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
