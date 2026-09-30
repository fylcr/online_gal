// pump_event_queue.cpp -- pumps one round of pending events through the dispatch steps and answers the round outcome..
//
// PumpEventQueue -- pumps one round of pending events through the dispatch steps and answers the round outcome.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 1199229-1199229 (f2801); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void Panic(EngineContext* ctx, uint32_t a0);
uint32_t PollEventKind(EngineContext* ctx);
void RunEventStep(EngineContext* ctx, uint32_t a0);
// PROVISIONAL nested-namespace forwarders (the dispatch products land later).
namespace AutoReadTimer {
  void Dispatch(EngineContext* ctx, uint32_t a, uint32_t b);
}
namespace BacklogView {
  void DispatchAux(EngineContext* ctx, uint32_t a, uint32_t b);
}

namespace {

constexpr uint32_t kF2801MsgPx = 0xBE5BDu;  // px != 0
constexpr uint32_t kF2801MsgBoostBoostSmartPtr = 0xBE5C5u;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
constexpr uint32_t kF2801MsgOperator = 0xBE5FDu;  // operator->
constexpr uint32_t kF2801ConstCF7D4 = 0xCF7D4u;  // 
constexpr uint32_t kF2801ConstCF7DC = 0xCF7DCu;  // 
constexpr uint32_t kF2801ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF2801SaveFrameBytes = 8u;
constexpr uint32_t kF2801ResumeWordBytes = 4u;

}  // namespace

void PumpEventQueue(EngineContext* ctx) {
  EnterFrame();
  uint32_t l0 = 0u; uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF2801SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    l0 = i0;
    i0 = l3;
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
    i2 = kF2801ResumeWordBytes;
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
    HostYieldToHost(ctx);
    i0 = kF2801ConstCF7DC;
    i0 = LoadU8(ctx, i0);
    i0 = !(i0);
    l1 = i0;
    i0 = kF2801ConstCF7D4;
    i0 = LoadU32(ctx, i0);
    l0 = i0;
  }
  i0 = l1;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l1;
    i1 = l0;
    i1 = !(i1);
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l1 = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF2801ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l2;
      i0 = !(i0);
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF2801MsgPx;
        i2 = kF2801MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF2801MsgOperator;
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
        i0 = kF2801ConstD015C;
        i0 = LoadU32(ctx, i0);
        l0 = i0;
        i0 = kF2801ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l0;
        i1 = 1u;
        i0 = i0 != i1;
        l0 = i0;
        if (i0) {goto block_5;}
        goto running_exit;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l0;
      i0 = LoadU32(ctx, i0 + 24u);
      l0 = i0;
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
        i0 = kF2801ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF2801MsgPx;
        i2 = kF2801MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF2801MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 1u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF2801ConstD015C;
        i0 = LoadU32(ctx, i0);
        l0 = i0;
        i0 = kF2801ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l0;
        i1 = 1u;
        i0 = i0 != i1;
        l0 = i0;
        if (i0) {goto block_5;}
        goto running_exit;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l0;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      l1 = i0;
    }
    i0 = l2;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l0;
      i1 = l1;
      i0 = CallTableT2(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      l3 = i0;
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l3;
      l0 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l0;
      if (i0) {goto block_6;}
      i0 = kF2801ConstCF7D4;
      i0 = LoadU32(ctx, i0);
      l0 = i0;
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
        i0 = kF2801ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l2;
      i1 = 3u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF2801MsgPx;
        i2 = kF2801MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF2801MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 3u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF2801ConstD015C;
        i0 = LoadU32(ctx, i0);
        l0 = i0;
        i0 = kF2801ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l0;
        i1 = 1u;
        i0 = i0 != i1;
        l0 = i0;
        if (i0) {goto block_5;}
        goto running_exit;
      }
    }
    i0 = l2;
    i1 = 4u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l0;
      i1 = 0u;
      BacklogView::DispatchAux(ctx, i0, i1);
      i0 = 4u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF2801ConstCF7DC;
      i1 = 1u;
      StoreU8(ctx, i0, i1);
      goto func_exit;
    }
  }
  i0 = l1;
  i1 = l0;
  i1 = !(i1);
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l1 = i0;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF2801ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l2;
    i1 = 5u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF2801MsgPx;
      i2 = kF2801MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF2801MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 5u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF2801ConstD015C;
      i0 = LoadU32(ctx, i0);
      l0 = i0;
      i0 = kF2801ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l0;
      i1 = 1u;
      i0 = i0 != i1;
      l0 = i0;
      if (i0) {goto block_5;}
      goto running_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = PollEventKind(ctx);
    l1 = i0;
  }
  i0 = l2;
  i1 = 6u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l0;
    i1 = l1;
    AutoReadTimer::Dispatch(ctx, i0, i1);
    i0 = 6u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  if (i0) {
    i0 = l1;
  } else {
    i0 = kF2801ConstCF7D4;
    i0 = LoadU32(ctx, i0);
    l0 = i0;
    i0 = !(i0);
  }
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF2801ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l2;
    i1 = 7u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF2801MsgPx;
      i2 = kF2801MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF2801MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 7u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF2801ConstD015C;
      i0 = LoadU32(ctx, i0);
      l0 = i0;
      i0 = kF2801ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l0;
      i1 = 1u;
      i0 = i0 != i1;
      l0 = i0;
      if (i0) {goto block_5;}
      goto running_exit;
    }
  }
  i0 = l2;
  i1 = 8u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l0;
    RunEventStep(ctx, i0);
    i0 = 8u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
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
    l0 = i0;
    i0 = HostTempRet0(ctx);
  }
  i0 = l2;
  i1 = 9u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l0;
    Panic(ctx, i0);
    i0 = 9u;
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
  i2 = kF2801ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = l0;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = l1;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 8u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
