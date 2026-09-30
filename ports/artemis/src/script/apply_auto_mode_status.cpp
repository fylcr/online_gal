// apply_auto_mode_status.cpp -- applies the auto-mode status: the auto-read flag is written into the status slot and the layer state follows it..
//
// ApplyAutoModeStatus -- applies the auto-mode status: the auto-read flag is written into the status slot and the layer state follows it.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 283946-283946 (f744); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void Panic(EngineContext* ctx, uint32_t a0);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
uint32_t FormatLuaEventFilterLine(EngineContext* ctx, uint32_t a0, uint32_t a1);
void UnorderedInsertBucketNode(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f);

namespace {

constexpr uint32_t kF744ConstAF3ED = 0xAF3EDu;  // 
constexpr uint32_t kF744MsgSStatusAutomode = 0xB4A1Cu;  // s.status.automode
constexpr uint32_t kF744ConstB4A24 = 0xB4A24u;  // 
constexpr uint32_t kF744ConstB4A2C = 0xB4A2Cu;  // 
constexpr uint32_t kF744ConstB5E30 = 0xB5E30u;  // 
constexpr uint32_t kF744MsgPx = 0xB5E31u;  // px != 0
constexpr uint32_t kF744MsgBoostBoostSmartPtr = 0xB5E39u;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
constexpr uint32_t kF744MsgOperator = 0xB5E71u;  // operator->
constexpr uint32_t kF744ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF744ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF744SaveFrameBytes = 28u;
constexpr uint32_t kF744ResumeWordBytes = 4u;

}  // namespace

void ApplyAutoModeStatus(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF744SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l6 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 24u);
    l8 = i0;
    i0 = l6;
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
    i2 = kF744ResumeWordBytes;
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
    i1 = 32u;
    i0 -= i1;
    l4 = i0;
    GuestStackSp(ctx) = i0;
    i0 = b;
    i0 = !(i0);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    if (i0) {goto block_11;}
    i0 = c;
    if (i0) {goto block_11;}
    i0 = a;
    i0 = LoadU8(ctx, i0 + 34u);
    i0 = !(i0);
    if (i0) {goto block_9;}
    block_11:;
    i0 = a;
    i0 = LoadU8(ctx, i0 + 35u);
    i1 = b;
    i0 = i0 == i1;
    if (i0) {goto block_9;}
    i0 = a;
    i1 = b;
    StoreU8(ctx, i0 + 35u, i1);
    i0 = a;
    i1 = 13308u;
    i0 += i1;
    c = i0;
  }
  i0 = b;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = c;
      StoreU32(ctx, i0 + 24u, i1);
      i0 = l4;
      i1 = 24u;
      i0 += i1;
      l3 = i0;
      i0 = l4;
      i1 = 16u;
      i0 += i1;
      l5 = i0;
      i0 = a;
      i1 = 36u;
      i0 += i1;
      b = i0;
    }
    i0 = l7;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l4;
      i1 = b;
      i2 = c;
      i3 = kF744ConstB5E30;
      i4 = l3;
      i5 = l5;
      UnorderedInsertBucketNode(ctx, i0, i1, i2, i3, i4, i5);
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU8(ctx, i0 + 32u);
      i0 = !(i0);
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      if (i0) {goto block_16;}
      i0 = l4;
      i1 = c;
      StoreU32(ctx, i0 + 24u, i1);
      i0 = l4;
      i1 = 16u;
      i0 += i1;
      l5 = i0;
      i0 = l4;
      i1 = 24u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l7;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l4;
      i1 = b;
      i2 = c;
      i3 = kF744ConstB5E30;
      i4 = l3;
      i5 = l5;
      UnorderedInsertBucketNode(ctx, i0, i1, i2, i3, i4, i5);
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = LoadU32(ctx, i0 + 40u);
      l8 = i0;
      i0 = l3;
      i0 = LoadU8(ctx, i0 + 47u);
      l5 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l6 = i0;
      i0 = l8;
      i1 = l5;
      i2 = l6;
      i0 = i2 ? i0 : i1;
      i0 = !(i0);
      l5 = i0;
    }
    i0 = l5;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF744ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 36u;
        i0 += i1;
        l3 = i0;
      }
      i0 = l7;
      i1 = 2u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1354u;
        i1 = l3;
        i2 = 0u;
        i3 = kF744ConstFFFFFFFF;
        i4 = kF744ConstAF3ED;
        i5 = 0u;
        i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
        l6 = i0;
        i0 = 2u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l6;
        l3 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF744ConstD015C;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i0 = kF744ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 1u;
        i0 = i0 == i1;
        l5 = i0;
        if (i0) {goto block_6;}
        i0 = l3;
        i0 = !(i0);
        l3 = i0;
        if (i0) {goto block_16;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 16u);
      l3 = i0;
      i0 = !(i0);
      l5 = i0;
    }
    i0 = l5;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF744ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 3u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF744MsgPx;
        i2 = kF744MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF744MsgOperator;
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
        i0 = kF744ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF744ConstD015C;
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
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 24u);
      l3 = i0;
      i0 = !(i0);
      l5 = i0;
    }
    i0 = l5;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF744ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 4u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF744MsgPx;
        i2 = kF744MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF744MsgOperator;
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
        i0 = kF744ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF744ConstD015C;
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
      i0 = l4;
      i1 = c;
      StoreU32(ctx, i0 + 24u, i1);
      i0 = l4;
      i1 = 16u;
      i0 += i1;
      l8 = i0;
      i0 = l4;
      i1 = 24u;
      i0 += i1;
      l5 = i0;
    }
    i0 = l7;
    i1 = 5u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l4;
      i1 = b;
      i2 = c;
      i3 = kF744ConstB5E30;
      i4 = l5;
      i5 = l8;
      UnorderedInsertBucketNode(ctx, i0, i1, i2, i3, i4, i5);
      i0 = 5u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 304u);
      c = i0;
      i0 = l4;
      i0 = LoadU32(ctx, i0);
      i1 = 36u;
      i0 += i1;
      b = i0;
    }
    i0 = l7;
    i1 = 6u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l4;
      i1 = l3;
      i2 = b;
      i3 = c;
      CallTableT5(ctx, i3, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = 6u;
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
      b = i0;
      i0 = 0u;
      l3 = i0;
      i0 = l4;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = !(i0);
      l5 = i0;
      if (i0) {
        i0 = l3;
      } else {
        i0 = c;
        i0 = LoadU32(ctx, i0);
        i1 = 152u;
        i0 -= i1;
        i0 = LoadU32(ctx, i0);
        i1 = c;
        i0 += i1;
      }
      c = i0;
      i0 = !(i0);
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      if (i0) {goto block_43;}
      i0 = c;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 124u);
      l3 = i0;
      i0 = kF744ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 7u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l3;
      i1 = c;
      i2 = 1u;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 7u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF744ConstD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kF744ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 != i1;
      c = i0;
      if (i0) {goto block_43;}
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = b;
      i0 = !(i0);
      if (i0) {goto block_7;}
      i0 = b;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 4u);
      c = i1;
      i2 = 1u;
      i1 -= i2;
      l3 = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 != i1;
      if (i0) {goto block_7;}
      i0 = b;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      c = i0;
      i0 = kF744ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 8u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = b;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 8u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF744ConstD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kF744ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 != i1;
      c = i0;
    }
    i0 = c;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i1 = b;
        i1 = LoadU32(ctx, i1 + 8u);
        c = i1;
        i2 = 1u;
        i1 -= i2;
        l3 = i1;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = c;
        i1 = 1u;
        i0 = i0 != i1;
        if (i0) {goto block_7;}
        i0 = b;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        c = i0;
        i0 = kF744ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 9u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = c;
        i1 = b;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 9u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF744ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF744ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 != i1;
        b = i0;
        if (i0) {goto block_7;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto running_exit;}
    block_43:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i0 = !(i0);
      c = i0;
      if (i0) {goto block_16;}
      i0 = b;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 4u);
      c = i1;
      i2 = 1u;
      i1 -= i2;
      l3 = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 != i1;
      c = i0;
      if (i0) {goto block_16;}
      i0 = b;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      c = i0;
      i0 = kF744ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 10u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = b;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 10u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF744ConstD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kF744ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto running_exit;}
      i0 = b;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 8u);
      c = i1;
      i2 = 1u;
      i1 -= i2;
      l3 = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 != i1;
      c = i0;
      if (i0) {goto block_16;}
      i0 = b;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      c = i0;
      i0 = kF744ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 11u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = b;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 11u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF744ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF744ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
      if (i0) {goto running_exit;}
    }
    block_16:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 112u);
      c = i0;
      i0 = !(i0);
      b = i0;
    }
    i0 = b;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF744ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l7;
      i1 = 12u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF744MsgPx;
        i2 = kF744MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF744MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 12u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF744ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF744ConstD015C;
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
    i0 = l7;
    i1 = 13u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 32u;
      i0 = AllocateStringBuffer(ctx, i0);
      l6 = i0;
      i0 = 13u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l6;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = b;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      j1 = 9223372174293729297ull;
      StoreU64(ctx, i0 + 4u, j1);
      i0 = b;
      i1 = 0u;
      StoreU8(ctx, i0 + 17u, i1);
      i0 = b;
      i1 = kF744ConstB4A2C;
      i1 = LoadU8(ctx, i1);
      l3 = i1;
      StoreU8(ctx, i0 + 16u, i1);
      i0 = b;
      i1 = kF744ConstB4A24;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0 + 8u, j1);
      i0 = b;
      i1 = kF744MsgSStatusAutomode;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = c;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 20u);
      b = i0;
      i0 = kF744ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 14u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = c;
      i2 = l4;
      i3 = 1u;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l6 = i0;
      i0 = 14u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l6;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF744ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF744ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
    }
    i0 = b;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 11u);
        i1 = kF744ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = a;
        i1 = 13764u;
        i0 += i1;
        b = i0;
      }
      i0 = l7;
      i1 = 15u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = b;
        i0 = FormatLuaEventFilterLine(ctx, i0, i1);
        l6 = i0;
        i0 = 15u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l6;
        a = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_9;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 11u);
      i1 = kF744ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      if (i0) {goto block_7;}
      i0 = l4;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
      i0 = a;
      HostOsRelease(ctx, i0);
      TrapUnreachable();
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = c;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = a;
    i1 = 36u;
    i0 += i1;
    l3 = i0;
    i0 = l4;
    i1 = 16u;
    i0 += i1;
    l5 = i0;
    i0 = l4;
    i1 = 24u;
    i0 += i1;
    b = i0;
  }
  i0 = l7;
  i1 = 16u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l4;
    i1 = l3;
    i2 = c;
    i3 = kF744ConstB5E30;
    i4 = b;
    i5 = l5;
    UnorderedInsertBucketNode(ctx, i0, i1, i2, i3, i4, i5);
    i0 = 16u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = LoadU32(ctx, i0 + 40u);
    l8 = i0;
    i0 = b;
    i0 = LoadU8(ctx, i0 + 47u);
    l5 = i0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l6 = i0;
    i0 = l8;
    i1 = l5;
    i2 = l6;
    i0 = i2 ? i0 : i1;
    i0 = !(i0);
    l5 = i0;
  }
  i0 = l5;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF744ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 36u;
      i0 += i1;
      b = i0;
    }
    i0 = l7;
    i1 = 17u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1354u;
      i1 = b;
      i2 = 0u;
      i3 = kF744ConstFFFFFFFF;
      i4 = kF744ConstAF3ED;
      i5 = 0u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l6 = i0;
      i0 = 17u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l6;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF744ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF744ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto running_exit;}
      i0 = b;
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_77;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
    b = i0;
    i0 = !(i0);
    l5 = i0;
  }
  i0 = l5;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF744ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 18u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF744MsgPx;
      i2 = kF744MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF744MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 18u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF744ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF744ConstD015C;
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
    i0 = b;
    i0 = LoadU32(ctx, i0 + 24u);
    b = i0;
    i0 = !(i0);
    l5 = i0;
  }
  i0 = l5;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF744ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 19u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF744MsgPx;
      i2 = kF744MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF744MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 19u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF744ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF744ConstD015C;
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
    i0 = l4;
    i1 = c;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l4;
    i1 = 16u;
    i0 += i1;
    l8 = i0;
    i0 = l4;
    i1 = 24u;
    i0 += i1;
    l5 = i0;
  }
  i0 = l7;
  i1 = 20u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l4;
    i1 = l3;
    i2 = c;
    i3 = kF744ConstB5E30;
    i4 = l5;
    i5 = l8;
    UnorderedInsertBucketNode(ctx, i0, i1, i2, i3, i4, i5);
    i0 = 20u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 304u);
    l3 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0);
    i1 = 36u;
    i0 += i1;
    c = i0;
  }
  i0 = l7;
  i1 = 21u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l4;
    i1 = b;
    i2 = c;
    i3 = l3;
    CallTableT5(ctx, i3, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    i0 = 21u;
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
    b = i0;
    i0 = 0u;
    l3 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0);
    c = i0;
    if (i0) {
      i0 = c;
      i0 = LoadU32(ctx, i0);
      i1 = 152u;
      i0 -= i1;
      i0 = LoadU32(ctx, i0);
      i1 = c;
      i0 += i1;
    } else {
      i0 = l3;
    }
    c = i0;
    i0 = !(i0);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    if (i0) {goto block_102;}
    i0 = c;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 124u);
    l3 = i0;
    i0 = kF744ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 22u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l3;
    i1 = c;
    i2 = 0u;
    HostOsSchedule(ctx, i0, i1, i2);
    i0 = 22u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF744ConstD015C;
    i0 = LoadU32(ctx, i0);
    c = i0;
    i0 = kF744ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = 1u;
    i0 = i0 != i1;
    c = i0;
    if (i0) {goto block_102;}
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = b;
    i0 = !(i0);
    if (i0) {goto block_7;}
    i0 = b;
    i1 = b;
    i1 = LoadU32(ctx, i1 + 4u);
    c = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = c;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_7;}
    i0 = b;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
    i0 = kF744ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 23u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = c;
    i1 = b;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 23u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF744ConstD015C;
    i0 = LoadU32(ctx, i0);
    c = i0;
    i0 = kF744ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = 1u;
    i0 = i0 != i1;
    c = i0;
  }
  i0 = c;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 8u);
      c = i1;
      i2 = 1u;
      i1 -= i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 != i1;
      if (i0) {goto block_7;}
      i0 = b;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      c = i0;
      i0 = kF744ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 24u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = c;
      i1 = b;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 24u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF744ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF744ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_7;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto running_exit;}
  block_102:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i0 = !(i0);
    c = i0;
    if (i0) {goto block_77;}
    i0 = b;
    i1 = b;
    i1 = LoadU32(ctx, i1 + 4u);
    c = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = c;
    i1 = 1u;
    i0 = i0 != i1;
    c = i0;
    if (i0) {goto block_77;}
    i0 = b;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
    i0 = kF744ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 25u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = c;
    i1 = b;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 25u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF744ConstD015C;
    i0 = LoadU32(ctx, i0);
    c = i0;
    i0 = kF744ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto running_exit;}
    i0 = b;
    i1 = b;
    i1 = LoadU32(ctx, i1 + 8u);
    c = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = c;
    i1 = 1u;
    i0 = i0 != i1;
    c = i0;
    if (i0) {goto block_77;}
    i0 = b;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    c = i0;
    i0 = kF744ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 26u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = c;
    i1 = b;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 26u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF744ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF744ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    b = i0;
    if (i0) {goto running_exit;}
  }
  block_77:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 112u);
    c = i0;
    i0 = !(i0);
    b = i0;
  }
  i0 = b;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF744ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l7;
    i1 = 27u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF744MsgPx;
      i2 = kF744MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF744MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 27u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF744ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF744ConstD015C;
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
  i0 = l7;
  i1 = 28u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 32u;
    i0 = AllocateStringBuffer(ctx, i0);
    l6 = i0;
    i0 = 28u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l6;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = b;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    j1 = 9223372174293729297ull;
    StoreU64(ctx, i0 + 4u, j1);
    i0 = b;
    i1 = 0u;
    StoreU8(ctx, i0 + 17u, i1);
    i0 = b;
    i1 = kF744ConstB4A2C;
    i1 = LoadU8(ctx, i1);
    StoreU8(ctx, i0 + 16u, i1);
    i0 = b;
    i1 = kF744ConstB4A24;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0 + 8u, j1);
    i0 = b;
    i1 = kF744MsgSStatusAutomode;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0, j1);
    i0 = c;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 20u);
    b = i0;
    i0 = kF744ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 29u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = c;
    i2 = l4;
    i3 = 0u;
    i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
    l6 = i0;
    i0 = 29u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l6;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF744ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF744ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_8;}
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF744ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = a;
    i1 = 2772u;
    i0 += i1;
    i0 = LoadU8(ctx, i0);
    if (i0) {goto block_129;}
    i0 = a;
    i1 = 2773u;
    i0 += i1;
    i0 = LoadU8(ctx, i0);
    if (i0) {goto block_129;}
    i0 = a;
    i1 = 2774u;
    i0 += i1;
    i0 = LoadU8(ctx, i0);
    if (i0) {goto block_129;}
    i0 = a;
    i1 = 2775u;
    i0 += i1;
    i0 = LoadU8(ctx, i0);
    if (i0) {goto block_129;}
    i0 = a;
    i1 = 2792u;
    i0 += i1;
    i0 = LoadU8(ctx, i0);
    if (i0) {goto block_129;}
    i0 = a;
    i1 = 2768u;
    i0 += i1;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    block_129:;
    i0 = a;
    i1 = 13808u;
    i0 += i1;
    b = i0;
  }
  i0 = l7;
  i1 = 30u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = b;
    i0 = FormatLuaEventFilterLine(ctx, i0, i1);
    l6 = i0;
    i0 = 30u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l6;
    a = i0;
  }
  block_9:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 32u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF744ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_7;}
    i0 = l4;
    i0 = LoadU32(ctx, i0);
    DeallocateRawBlock(ctx, i0);
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    a = i0;
    i0 = HostTempRet0(ctx);
  }
  i0 = l7;
  i1 = 31u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 31u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
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
  i0 = l7;
  i1 = 32u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 32u;
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
  l6 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l6;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF744ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l6 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l6;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l6;
  i1 = c;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l6;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l6;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l6;
  i1 = l5;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l6;
  i1 = l8;
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
