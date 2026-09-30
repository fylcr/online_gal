// apply_command_skip_status.cpp -- applies the command-skip status: the skip flag is written and the pending command step is released..
//
// ApplyCommandSkipStatus -- applies the command-skip status: the skip flag is written and the pending command step is released.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 283103-283103 (f742); R1 transcribe-keep.
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

namespace {

constexpr uint32_t kF742MsgSStatusCommandskip = 0xB4A2Eu;  // s.status.commandskip
constexpr uint32_t kF742ConstB4A36 = 0xB4A36u;  // 
constexpr uint32_t kF742ConstB4A3E = 0xB4A3Eu;  // 
constexpr uint32_t kF742MsgPx = 0xB5E31u;  // px != 0
constexpr uint32_t kF742MsgBoostBoostSmartPtr = 0xB5E39u;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
constexpr uint32_t kF742MsgOperator = 0xB5E71u;  // operator->
constexpr uint32_t kF742ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF742ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF742SaveFrameBytes = 20u;
constexpr uint32_t kF742ResumeWordBytes = 4u;

}  // namespace

void ApplyCommandSkipStatus(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF742SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = l3;
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
    i2 = kF742ResumeWordBytes;
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
    i0 = GuestStackSp(ctx);
    i1 = 16u;
    i0 -= i1;
    l4 = i0;
    GuestStackSp(ctx) = i0;
    i0 = b;
    i0 = !(i0);
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    if (i0) {goto block_8;}
    i0 = c;
    if (i0) {goto block_8;}
    i0 = a;
    i0 = LoadU8(ctx, i0 + 48u);
    i0 = !(i0);
    if (i0) {goto block_5;}
    block_8:;
    i0 = a;
    i0 = LoadU8(ctx, i0 + 49u);
    i1 = b;
    i0 = i0 == i1;
    if (i0) {goto block_5;}
    i0 = a;
    i1 = b;
    StoreU8(ctx, i0 + 49u, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 112u);
    l5 = i0;
    i0 = !(i0);
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
      i0 = kF742ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l6;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF742MsgPx;
      i2 = kF742MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF742MsgOperator;
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
      i0 = kF742ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF742ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      if (i0) {goto running_exit;}
      i0 = 0u;
      i0 = FindMatchingCatch(ctx, i0);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
    }
    i0 = l6;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      Panic(ctx, i0);
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
  }
  i0 = l6;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 32u;
    i0 = AllocateStringBuffer(ctx, i0);
    l3 = i0;
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = c;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    j1 = 9223372174293729300ull;
    StoreU64(ctx, i0 + 4u, j1);
    i0 = c;
    i1 = 0u;
    StoreU8(ctx, i0 + 20u, i1);
    i0 = c;
    i1 = kF742ConstB4A3E;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 16u, i1);
    i0 = c;
    i1 = kF742ConstB4A36;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0 + 8u, j1);
    i0 = c;
    i1 = kF742MsgSStatusCommandskip;
    j1 = LoadU64(ctx, i1);
    StoreU64(ctx, i0, j1);
    i0 = l5;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 20u);
    c = i0;
    i0 = kF742ConstD015C;
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
    i0 = c;
    i1 = l5;
    i2 = l4;
    i3 = b;
    i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
    l3 = i0;
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  if (i0) {
    i0 = c;
  } else {
    i0 = kF742ConstD015C;
    i0 = LoadU32(ctx, i0);
    c = i0;
    i0 = kF742ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = 1u;
    i0 = i0 != i1;
  }
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
      i1 = kF742ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = b;
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_18;}
      i0 = a;
      i1 = 13852u;
      i0 += i1;
      b = i0;
    }
    i0 = l6;
    i1 = 4u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = b;
      i0 = FormatLuaEventFilterLine(ctx, i0, i1);
      l3 = i0;
      i0 = 4u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l3;
      a = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_5;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF742ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = a;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_18:;
  i0 = b;
  i1 = a;
  i2 = 13896u;
  i1 += i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  b = i0;
  i0 = l6;
  i1 = 5u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = b;
    i0 = FormatLuaEventFilterLine(ctx, i0, i1);
    i0 = 5u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 16u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  running_exit:;
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
  i2 = kF742ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = c;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l3;
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l3;
  i1 = l5;
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
