// message_rp_dispatch.cpp -- the page turn dispatch step: the queued page turn request is applied one message layer step..
//
// MessageLayer::RpDispatch -- the page turn dispatch step: the queued page turn request is applied one message layer step.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 1615809-1615809 (f3548); R1 transcribe-keep.
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
uint32_t SetStringFromSource(EngineContext* ctx, uint32_t text, uint32_t source);
void UnorderedLinkBucketNode(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f);
void FindTaggedTreeEntry(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);

namespace {

constexpr uint32_t kF3548ConstAF595 = 0xAF595u;  // 
constexpr uint32_t kF3548ConstB5E30 = 0xB5E30u;  // 
constexpr uint32_t kF3548MsgPx = 0xB5E31u;  // px != 0
constexpr uint32_t kF3548MsgBoostBoostSmartPtr = 0xB5E39u;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
constexpr uint32_t kF3548MsgOperator = 0xB5E71u;  // operator->
constexpr uint32_t kF3548ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF3548ConstFFFFFFC0 = 0xFFFFFFC0u;  // 
constexpr uint32_t kF3548ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF3548SaveFrameBytes = 44u;
constexpr uint32_t kF3548ResumeWordBytes = 4u;

}  // namespace

namespace MessageLayer {
void RpDispatch(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u; uint64_t l10 = 0; float l11 = 0; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u;
  uint64_t j0 = 0; uint64_t j1 = 0;
  float f0 = 0; float f1 = 0; float f2 = 0;
      l9 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF3548SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l8 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 4u);
    l1 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 24u);
    l7 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 28u);
    l9 = i0;
    i0 = l8;
    f0 = LoadF32(ctx, i0 + 32u);
    l11 = f0;
    i0 = l8;
    j0 = LoadU64(ctx, i0 + 36u);
    l10 = j0;
    i0 = l8;
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
    i2 = kF3548ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    i1 = kF3548ConstFFFFFFC0;
    i0 += i1;
    l3 = i0;
    GuestStackSp(ctx) = i0;
    i0 = a;
    i1 = 13699u;
    i0 += i1;
    i0 = LoadS8(ctx, i0);
    l1 = i0;
    i1 = kF3548ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    l2 = i0;
    if (i0) {
      i0 = a;
      i1 = 13692u;
      i0 += i1;
      i0 = LoadU32(ctx, i0);
      goto running_exit;
    }
    i0 = l1;
    i1 = 255u;
    i0 &= i1;
    running_exit:;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    if (i0) {goto block_9;}
    i0 = a;
    i1 = 13688u;
    i0 += i1;
    l6 = i0;
    i0 = l3;
    i1 = a;
    i2 = 2752u;
    i1 += i2;
    l1 = i1;
    StoreU32(ctx, i0 + 48u, i1);
    i0 = a;
    i1 = 2684u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = 48u;
    i0 += i1;
    l7 = i0;
    i0 = l3;
    i1 = 56u;
    i0 += i1;
    l9 = i0;
    i0 = l3;
    i1 = 32u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l4;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = l5;
    i2 = l1;
    i3 = kF3548ConstB5E30;
    i4 = l7;
    i5 = l9;
    UnorderedLinkBucketNode(ctx, i0, i1, i2, i3, i4, i5);
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
    i1 = 2764u;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l7 = i0;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 32u);
    l2 = i1;
    i1 = LoadU32(ctx, i1 + 32u);
    l9 = i1;
    i2 = l2;
    i2 = LoadU32(ctx, i2 + 28u);
    i1 -= i2;
    i2 = 2u;
    i1 = (uint32_t)((int32_t)i1 >> (i2 & 31));
    i0 = i0 >= i1;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {goto block_15;}
    i0 = l3;
    i1 = l1;
    StoreU32(ctx, i0 + 48u, i1);
    i0 = l3;
    i1 = 48u;
    i0 += i1;
    l7 = i0;
    i0 = l3;
    i1 = 56u;
    i0 += i1;
    l9 = i0;
    i0 = l3;
    i1 = 32u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l4;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = l5;
    i2 = l1;
    i3 = kF3548ConstB5E30;
    i4 = l7;
    i5 = l9;
    UnorderedLinkBucketNode(ctx, i0, i1, i2, i3, i4, i5);
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 32u);
    i0 = LoadU32(ctx, i0 + 28u);
    i1 = a;
    i1 = LoadU32(ctx, i1 + 2764u);
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    l1 = i0;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l1;
    i0 = LoadU8(ctx, i0 + 15u);
    l5 = i0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l7 = i0;
    i0 = l2;
    i1 = l5;
    i2 = l7;
    i0 = i2 ? i0 : i1;
    i1 = 2u;
    i0 = i0 != i1;
    l2 = i0;
    if (i0) {goto block_15;}
    i0 = kF3548ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l1;
    i1 = 4u;
    i0 += i1;
    l1 = i0;
  }
  i0 = l4;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1354u;
    i1 = l1;
    i2 = 0u;
    i3 = kF3548ConstFFFFFFFF;
    i4 = kF3548ConstAF595;
    i5 = 2u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l8 = i0;
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3548ConstD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF3548ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_8;}
    i0 = l1;
    i0 = !(i0);
    l1 = i0;
    if (i0) {goto block_14;}
  }
  block_15:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = a;
    i2 = 13308u;
    i1 += i2;
    l1 = i1;
    StoreU32(ctx, i0 + 48u, i1);
    i0 = a;
    i1 = 13668u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = 48u;
    i0 += i1;
    l7 = i0;
    i0 = l3;
    i1 = 56u;
    i0 += i1;
    l9 = i0;
    i0 = l3;
    i1 = 32u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l4;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = l5;
    i2 = l1;
    i3 = kF3548ConstB5E30;
    i4 = l7;
    i5 = l9;
    FindTaggedTreeEntry(ctx, i0, i1, i2, i3, i4, i5);
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 32u);
    i0 = LoadU8(ctx, i0 + 33u);
    i0 = !(i0);
    if (i0) {goto block_9;}
    i0 = l3;
    i1 = l1;
    StoreU32(ctx, i0 + 48u, i1);
    i0 = l3;
    i1 = 48u;
    i0 += i1;
    l7 = i0;
    i0 = l3;
    i1 = 56u;
    i0 += i1;
    l9 = i0;
    i0 = l3;
    i1 = 32u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l4;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = l5;
    i2 = l1;
    i3 = kF3548ConstB5E30;
    i4 = l7;
    i5 = l9;
    FindTaggedTreeEntry(ctx, i0, i1, i2, i3, i4, i5);
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 32u);
    l2 = i0;
    i0 = LoadS8(ctx, i0 + 47u);
    l7 = i0;
    i1 = kF3548ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 40u);
      goto block_26;
    }
    i0 = l7;
    i1 = 255u;
    i0 &= i1;
    block_26:;
    i0 = !(i0);
    if (i0) {goto block_9;}
    i0 = l3;
    i1 = l1;
    StoreU32(ctx, i0 + 48u, i1);
    i0 = l3;
    i1 = 48u;
    i0 += i1;
    l7 = i0;
    i0 = l3;
    i1 = 56u;
    i0 += i1;
    l9 = i0;
    i0 = l3;
    i1 = 32u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l4;
  i1 = 5u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = l5;
    i2 = l1;
    i3 = kF3548ConstB5E30;
    i4 = l7;
    i5 = l9;
    FindTaggedTreeEntry(ctx, i0, i1, i2, i3, i4, i5);
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
    i0 = LoadU32(ctx, i0 + 32u);
    i1 = 36u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l4;
  i1 = 6u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = l2;
    i0 = SetStringFromSource(ctx, i0, i1);
    l8 = i0;
    i0 = 6u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = l1;
    StoreU32(ctx, i0 + 48u, i1);
    i0 = l3;
    i1 = 48u;
    i0 += i1;
    l7 = i0;
    i0 = l3;
    i1 = 56u;
    i0 += i1;
    l9 = i0;
    i0 = l3;
    i1 = 32u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l4;
  i1 = 7u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = l5;
    i2 = l1;
    i3 = kF3548ConstB5E30;
    i4 = l7;
    i5 = l9;
    FindTaggedTreeEntry(ctx, i0, i1, i2, i3, i4, i5);
    i0 = 7u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 32u);
    i1 = kF3548ConstFFFFFFC0;
    i0 -= i1;
    goto block_13;
  }
  block_14:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = a;
    i2 = 13308u;
    i1 += i2;
    l1 = i1;
    StoreU32(ctx, i0 + 48u, i1);
    i0 = a;
    i1 = 13668u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = 48u;
    i0 += i1;
    l7 = i0;
    i0 = l3;
    i1 = 56u;
    i0 += i1;
    l9 = i0;
    i0 = l3;
    i1 = 32u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l4;
  i1 = 8u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = l5;
    i2 = l1;
    i3 = kF3548ConstB5E30;
    i4 = l7;
    i5 = l9;
    FindTaggedTreeEntry(ctx, i0, i1, i2, i3, i4, i5);
    i0 = 8u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 32u);
    i0 = LoadU8(ctx, i0 + 34u);
    i0 = !(i0);
    if (i0) {goto block_9;}
    i0 = l3;
    i1 = l1;
    StoreU32(ctx, i0 + 48u, i1);
    i0 = l3;
    i1 = 48u;
    i0 += i1;
    l7 = i0;
    i0 = l3;
    i1 = 56u;
    i0 += i1;
    l9 = i0;
    i0 = l3;
    i1 = 32u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l4;
  i1 = 9u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = l5;
    i2 = l1;
    i3 = kF3548ConstB5E30;
    i4 = l7;
    i5 = l9;
    FindTaggedTreeEntry(ctx, i0, i1, i2, i3, i4, i5);
    i0 = 9u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 32u);
    l2 = i0;
    i0 = LoadS8(ctx, i0 + 59u);
    l7 = i0;
    i1 = kF3548ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 52u);
      goto block_39;
    }
    i0 = l7;
    i1 = 255u;
    i0 &= i1;
    block_39:;
    i0 = !(i0);
    if (i0) {goto block_9;}
    i0 = l3;
    i1 = l1;
    StoreU32(ctx, i0 + 48u, i1);
    i0 = l3;
    i1 = 48u;
    i0 += i1;
    l7 = i0;
    i0 = l3;
    i1 = 56u;
    i0 += i1;
    l9 = i0;
    i0 = l3;
    i1 = 32u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l4;
  i1 = 10u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = l5;
    i2 = l1;
    i3 = kF3548ConstB5E30;
    i4 = l7;
    i5 = l9;
    FindTaggedTreeEntry(ctx, i0, i1, i2, i3, i4, i5);
    i0 = 10u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 32u);
    i1 = 48u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l4;
  i1 = 11u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = l2;
    i0 = SetStringFromSource(ctx, i0, i1);
    l8 = i0;
    i0 = 11u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l8;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = l1;
    StoreU32(ctx, i0 + 48u, i1);
    i0 = l3;
    i1 = 48u;
    i0 += i1;
    l7 = i0;
    i0 = l3;
    i1 = 56u;
    i0 += i1;
    l9 = i0;
    i0 = l3;
    i1 = 32u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l4;
  i1 = 12u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = l5;
    i2 = l1;
    i3 = kF3548ConstB5E30;
    i4 = l7;
    i5 = l9;
    FindTaggedTreeEntry(ctx, i0, i1, i2, i3, i4, i5);
    i0 = 12u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  if (i0) {
    i0 = l1;
  } else {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 32u);
    i1 = 76u;
    i0 += i1;
  }
  block_13:;
  l1 = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    j0 = LoadU64(ctx, i0);
    l10 = j0;
    i0 = l3;
    i1 = a;
    i2 = 13308u;
    i1 += i2;
    l1 = i1;
    StoreU32(ctx, i0 + 48u, i1);
    i0 = a;
    i1 = 13668u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = 48u;
    i0 += i1;
    l7 = i0;
    i0 = l3;
    i1 = 56u;
    i0 += i1;
    l9 = i0;
    i0 = l3;
    i1 = 32u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l4;
  i1 = 13u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = l5;
    i2 = l1;
    i3 = kF3548ConstB5E30;
    i4 = l7;
    i5 = l9;
    FindTaggedTreeEntry(ctx, i0, i1, i2, i3, i4, i5);
    i0 = 13u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 32u);
    i0 = LoadU8(ctx, i0 + 32u);
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
      i0 = a;
      i0 = LoadU32(ctx, i0 + 16u);
      a = i0;
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
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 14u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3548MsgPx;
        i2 = kF3548MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3548MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 14u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3548ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_7;}
        goto block_6;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 24u);
      a = i0;
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
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 15u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3548MsgPx;
        i2 = kF3548MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3548MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 15u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3548ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_7;}
        goto block_6;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 32u;
      i0 += i1;
      l1 = i0;
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 304u);
      l2 = i0;
    }
    i0 = l4;
    i1 = 16u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l1;
      i1 = a;
      i2 = l6;
      i3 = l2;
      CallTableT5(ctx, i3, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      i0 = 16u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 36u);
      a = i0;
      i0 = 0u;
      l2 = i0;
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 32u);
      l1 = i0;
      i0 = !(i0);
      l5 = i0;
      if (i0) {
        i0 = l2;
      } else {
        i0 = l1;
        i0 = LoadU32(ctx, i0);
        i1 = 152u;
        i0 -= i1;
        i0 = LoadU32(ctx, i0);
        i1 = l1;
        i0 += i1;
      }
      l1 = i0;
      i0 = !(i0);
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      if (i0) {goto block_70;}
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = LoadU32(ctx, i0 + 124u);
      l6 = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 17u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l6;
      i1 = l1;
      i2 = 1u;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 17u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3548ConstD015C;
      i0 = LoadU32(ctx, i0);
      l1 = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l1;
      i1 = 1u;
      i0 = i0 != i1;
      l1 = i0;
      if (i0) {goto block_70;}
      i0 = HostRet0(ctx);
      l1 = i0;
      i0 = HostTempRet0(ctx);
      i0 = a;
      i0 = !(i0);
      l3 = i0;
      if (i0) {goto block_50;}
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 4u);
      l3 = i1;
      i2 = 1u;
      i1 -= i2;
      l2 = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 != i1;
      l3 = i0;
      if (i0) {goto block_50;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      l3 = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 18u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l3;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 18u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3548ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 != i1;
      l3 = i0;
    }
    i0 = l3;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 8u);
        l3 = i1;
        i2 = 1u;
        i1 -= i2;
        l2 = i1;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 != i1;
        l3 = i0;
        if (i0) {goto block_50;}
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        l3 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 19u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l3;
        i1 = a;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 19u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3548ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_50;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_6;}
    block_70:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = !(i0);
      if (i0) {goto block_9;}
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 4u);
      l1 = i1;
      i2 = 1u;
      i1 -= i2;
      l2 = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l1;
      i1 = 1u;
      i0 = i0 != i1;
      if (i0) {goto block_9;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      l1 = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 20u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l1;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 20u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3548ConstD015C;
      i0 = LoadU32(ctx, i0);
      l1 = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l1;
      i1 = 1u;
      i0 = i0 != i1;
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
        i0 = a;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 8u);
        l1 = i1;
        i2 = 1u;
        i1 -= i2;
        l2 = i1;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l1;
        i1 = 1u;
        i0 = i0 != i1;
        if (i0) {goto block_9;}
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        l1 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 21u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l1;
        i1 = a;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 21u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3548ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_9;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_6;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
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
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 22u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3548MsgPx;
      i2 = kF3548MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3548MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 22u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3548ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_7;}
      goto block_6;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 24u);
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
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 23u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3548MsgPx;
      i2 = kF3548MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3548MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 23u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3548ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_7;}
      goto block_6;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 304u);
    l5 = i0;
    i0 = kF3548ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 48u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l4;
  i1 = 24u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l5;
    i1 = l2;
    i2 = l1;
    i3 = l6;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 24u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3548ConstD015C;
    i0 = LoadU32(ctx, i0);
    l1 = i0;
    i0 = kF3548ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l1;
    i1 = 1u;
    i0 = i0 != i1;
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
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 48u);
      l1 = i0;
      i0 = !(i0);
      if (i0) {goto block_49;}
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 84u);
      l5 = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      j0 = l10;
      i0 = (uint32_t)(j0);
      l2 = i0;
      f0 = (float)(int32_t)(i0);
      l11 = f0;
    }
    i0 = l4;
    i1 = 25u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l5;
      i1 = l1;
      f2 = l11;
      HostInvokeVif(ctx, i0, i1, f2);
      i0 = 25u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3548ConstD015C;
      i0 = LoadU32(ctx, i0);
      l1 = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l1;
      i1 = 1u;
      i0 = i0 == i1;
      l1 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l1;
      if (i0) {goto block_110;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 48u);
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
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 26u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3548MsgPx;
        i2 = kF3548MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3548MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 26u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3548ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_7;}
        goto block_6;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 88u);
      l5 = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      j0 = l10;
      j1 = 32ull;
      j0 >>= (j1 & 63);
      i0 = (uint32_t)(j0);
      l2 = i0;
      f0 = (float)(int32_t)(i0);
      l11 = f0;
    }
    i0 = l4;
    i1 = 27u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l5;
      i1 = l1;
      f2 = l11;
      HostInvokeVif(ctx, i0, i1, f2);
      i0 = 27u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3548ConstD015C;
      i0 = LoadU32(ctx, i0);
      l1 = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l1;
      i1 = 1u;
      i0 = i0 == i1;
      l1 = i0;
      if (i0) {goto block_110;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 48u);
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
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 28u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3548MsgPx;
        i2 = kF3548MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3548MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 28u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3548ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_7;}
        goto block_6;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l1;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = LoadU32(ctx, i0 + 124u);
      l5 = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 29u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l5;
      i1 = l1;
      i2 = 1u;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 29u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3548ConstD015C;
      i0 = LoadU32(ctx, i0);
      l1 = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l1;
      i1 = 1u;
      i0 = i0 == i1;
      l1 = i0;
      if (i0) {goto block_110;}
      i0 = a;
      i0 = LoadU32(ctx, i0 + 13680u);
      l5 = i0;
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
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 30u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 857u;
        i1 = kF3548MsgPx;
        i2 = kF3548MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3548MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 30u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3548ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_7;}
        goto block_6;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 32u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 43u, i1);
      i0 = l3;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 48u);
      StoreU32(ctx, i0 + 24u, i1);
      i0 = l3;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 52u);
      l1 = i1;
      StoreU32(ctx, i0 + 28u, i1);
      i0 = l1;
      if (i0) {
        i0 = l1;
        i1 = l1;
        i1 = LoadU32(ctx, i1 + 4u);
        i2 = 1u;
        i1 += i2;
        StoreU32(ctx, i0 + 4u, i1);
      }
      i0 = l5;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 324u);
      l1 = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 24u;
      i0 += i1;
      l7 = i0;
      i0 = l3;
      i1 = 32u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l4;
    i1 = 31u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l1;
      i1 = l5;
      i2 = l2;
      i3 = l7;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 31u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3548ConstD015C;
      i0 = LoadU32(ctx, i0);
      l1 = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l1;
      i1 = 1u;
      i0 = i0 != i1;
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
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 28u);
        l1 = i0;
        i0 = !(i0);
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        if (i0) {goto block_142;}
        i0 = l1;
        i1 = l1;
        i1 = LoadU32(ctx, i1 + 4u);
        l5 = i1;
        i2 = 1u;
        i1 -= i2;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = l5;
        i1 = 1u;
        i0 = i0 != i1;
        l2 = i0;
        if (i0) {goto block_142;}
        i0 = l1;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = LoadU32(ctx, i0 + 8u);
        l5 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 32u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l5;
        i1 = l1;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 32u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3548ConstD015C;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_6;}
        i0 = l1;
        i1 = l1;
        i1 = LoadU32(ctx, i1 + 8u);
        l5 = i1;
        i2 = 1u;
        i1 -= i2;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l5;
        i1 = 1u;
        i0 = i0 != i1;
        l2 = i0;
        if (i0) {goto block_142;}
        i0 = l1;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = LoadU32(ctx, i0 + 12u);
        l5 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 33u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l5;
        i1 = l1;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 33u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3548ConstD015C;
        i0 = LoadU32(ctx, i0);
        l1 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l1;
        i1 = 1u;
        i0 = i0 == i1;
        l1 = i0;
        if (i0) {goto block_6;}
      }
      block_142:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 43u);
        i1 = kF3548ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 32u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = a;
        i0 = LoadU32(ctx, i0 + 13680u);
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
          i0 = kF3548ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l4;
        i1 = 34u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3548MsgPx;
          i2 = kF3548MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3548MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 34u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3548ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3548ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_7;}
          goto block_6;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l1;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = LoadU32(ctx, i0 + 348u);
        l5 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 35u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l5;
        i1 = l1;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 35u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3548ConstD015C;
        i0 = LoadU32(ctx, i0);
        l1 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l1;
        i1 = 1u;
        i0 = i0 == i1;
        l1 = i0;
        if (i0) {goto block_110;}
        i0 = a;
        i0 = LoadU32(ctx, i0 + 16u);
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
          i0 = kF3548ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l4;
        i1 = 36u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3548MsgPx;
          i2 = kF3548MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3548MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 36u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3548ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3548ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_7;}
          goto block_6;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l1;
        i0 = LoadU32(ctx, i0 + 100u);
        l5 = i0;
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
          i0 = kF3548ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l4;
        i1 = 37u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3548MsgPx;
          i2 = kF3548MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3548MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 37u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3548ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3548ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_7;}
          goto block_6;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = 0u;
        l7 = i0;
        i0 = l3;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 13680u);
        l1 = i1;
        if (i1) {
          i1 = l1;
          i1 = LoadU32(ctx, i1);
          i2 = 152u;
          i1 -= i2;
          i1 = LoadU32(ctx, i1);
          i2 = l1;
          i1 += i2;
        } else {
          i1 = l7;
        }
        StoreU32(ctx, i0 + 16u, i1);
        i0 = l3;
        i1 = a;
        i2 = 13684u;
        i1 += i2;
        i1 = LoadU32(ctx, i1);
        l1 = i1;
        StoreU32(ctx, i0 + 20u, i1);
        i0 = l1;
        if (i0) {
          i0 = l1;
          i1 = l1;
          i1 = LoadU32(ctx, i1 + 4u);
          i2 = 1u;
          i1 += i2;
          StoreU32(ctx, i0 + 4u, i1);
        }
        i0 = l5;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 368u);
        l1 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 16u;
        i0 += i1;
        l2 = i0;
      }
      i0 = l4;
      i1 = 38u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l1;
        i1 = l5;
        i2 = l2;
        HostOsSchedule(ctx, i0, i1, i2);
        i0 = 38u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3548ConstD015C;
        i0 = LoadU32(ctx, i0);
        l1 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l1;
        i1 = 1u;
        i0 = i0 == i1;
        l1 = i0;
        if (i0) {goto block_137;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 20u);
        l1 = i0;
        i0 = !(i0);
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        if (i0) {goto block_173;}
        i0 = l1;
        i1 = l1;
        i1 = LoadU32(ctx, i1 + 4u);
        l5 = i1;
        i2 = 1u;
        i1 -= i2;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = l5;
        i1 = 1u;
        i0 = i0 != i1;
        l2 = i0;
        if (i0) {goto block_173;}
        i0 = l1;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = LoadU32(ctx, i0 + 8u);
        l5 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 39u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l5;
        i1 = l1;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 39u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3548ConstD015C;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_6;}
        i0 = l1;
        i1 = l1;
        i1 = LoadU32(ctx, i1 + 8u);
        l5 = i1;
        i2 = 1u;
        i1 -= i2;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l5;
        i1 = 1u;
        i0 = i0 != i1;
        l2 = i0;
        if (i0) {goto block_173;}
        i0 = l1;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = LoadU32(ctx, i0 + 12u);
        l5 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 40u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l5;
        i1 = l1;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 40u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3548ConstD015C;
        i0 = LoadU32(ctx, i0);
        l1 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l1;
        i1 = 1u;
        i0 = i0 == i1;
        l1 = i0;
        if (i0) {goto block_6;}
      }
      block_173:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 16u);
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
          i0 = kF3548ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l4;
        i1 = 41u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3548MsgPx;
          i2 = kF3548MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3548MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 41u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3548ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3548ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_7;}
          goto block_6;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l1;
        i0 = LoadU32(ctx, i0 + 24u);
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
          i0 = kF3548ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l4;
        i1 = 42u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3548MsgPx;
          i2 = kF3548MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3548MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 42u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3548ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3548ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_7;}
          goto block_6;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        j1 = 0ull;
        StoreU64(ctx, i0 + 8u, j1);
        i0 = l1;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 324u);
        l5 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 8u;
        i0 += i1;
        l2 = i0;
      }
      i0 = l4;
      i1 = 43u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l5;
        i1 = l1;
        i2 = l6;
        i3 = l2;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 43u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3548ConstD015C;
        i0 = LoadU32(ctx, i0);
        l1 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l1;
        i1 = 1u;
        i0 = i0 == i1;
        l1 = i0;
        if (i0) {goto block_136;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 12u);
        l1 = i0;
        i0 = !(i0);
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        if (i0) {goto block_194;}
        i0 = l1;
        i1 = l1;
        i1 = LoadU32(ctx, i1 + 4u);
        l6 = i1;
        i2 = 1u;
        i1 -= i2;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = l6;
        i1 = 1u;
        i0 = i0 != i1;
        l2 = i0;
        if (i0) {goto block_194;}
        i0 = l1;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = LoadU32(ctx, i0 + 8u);
        l6 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 44u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l6;
        i1 = l1;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 44u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3548ConstD015C;
        i0 = LoadU32(ctx, i0);
        l6 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l6;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_6;}
        i0 = l1;
        i1 = l1;
        i1 = LoadU32(ctx, i1 + 8u);
        l6 = i1;
        i2 = 1u;
        i1 -= i2;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l6;
        i1 = 1u;
        i0 = i0 != i1;
        l2 = i0;
        if (i0) {goto block_194;}
        i0 = l1;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = LoadU32(ctx, i0 + 12u);
        l6 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 45u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l6;
        i1 = l1;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 45u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3548ConstD015C;
        i0 = LoadU32(ctx, i0);
        l1 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l1;
        i1 = 1u;
        i0 = i0 == i1;
        l1 = i0;
        if (i0) {goto block_6;}
      }
      block_194:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 16u);
        a = i0;
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
          i0 = kF3548ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l4;
        i1 = 46u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3548MsgPx;
          i2 = kF3548MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3548MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 46u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3548ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3548ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_7;}
          goto block_6;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 24u);
        a = i0;
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
          i0 = kF3548ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l4;
        i1 = 47u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3548MsgPx;
          i2 = kF3548MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3548MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 47u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3548ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3548ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_7;}
          goto block_6;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 348u);
        l1 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 48u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l1;
        i1 = a;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 48u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3548ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 == i1;
        a = i0;
        if (i0) {goto block_110;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 52u);
        a = i0;
        i0 = !(i0);
        if (i0) {goto block_9;}
        i0 = a;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 4u);
        l1 = i1;
        i2 = 1u;
        i1 -= i2;
        l2 = i1;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = l1;
        i1 = 1u;
        i0 = i0 != i1;
        if (i0) {goto block_9;}
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 8u);
        l1 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 49u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l1;
        i1 = a;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 49u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3548ConstD015C;
        i0 = LoadU32(ctx, i0);
        l1 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l1;
        i1 = 1u;
        i0 = i0 != i1;
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
          i0 = a;
          i1 = a;
          i1 = LoadU32(ctx, i1 + 8u);
          l1 = i1;
          i2 = 1u;
          i1 -= i2;
          l2 = i1;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l1;
          i1 = 1u;
          i0 = i0 != i1;
          if (i0) {goto block_9;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 12u);
          l1 = i0;
          i0 = kF3548ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l4;
        i1 = 50u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l1;
          i1 = a;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 50u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3548ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3548ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_9;}
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_6;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      l1 = i0;
      i0 = HostTempRet0(ctx);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 28u);
      a = i0;
      i0 = !(i0);
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      if (i0) {goto block_223;}
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 4u);
      l6 = i1;
      i2 = 1u;
      i1 -= i2;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_223;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = LoadU32(ctx, i0 + 8u);
      l6 = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 51u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l6;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 51u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3548ConstD015C;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_6;}
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 8u);
      l6 = i1;
      i2 = 1u;
      i1 -= i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_223;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = LoadU32(ctx, i0 + 12u);
      l6 = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 52u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l6;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 52u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3548ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      a = i0;
      if (i0) {goto block_6;}
    }
    block_223:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 43u);
      i1 = kF3548ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_109;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 32u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_109;
    }
    block_137:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      l1 = i0;
      i0 = HostTempRet0(ctx);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 20u);
      a = i0;
      i0 = !(i0);
      if (i0) {goto block_109;}
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 4u);
      l6 = i1;
      i2 = 1u;
      i1 -= i2;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 != i1;
      if (i0) {goto block_109;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = LoadU32(ctx, i0 + 8u);
      l6 = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 53u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l6;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 53u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3548ConstD015C;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 != i1;
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
        i1 = a;
        i1 = LoadU32(ctx, i1 + 8u);
        l6 = i1;
        i2 = 1u;
        i1 -= i2;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l6;
        i1 = 1u;
        i0 = i0 != i1;
        if (i0) {goto block_109;}
        i0 = a;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = LoadU32(ctx, i0 + 12u);
        l6 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 54u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l6;
        i1 = a;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 54u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3548ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_109;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_6;}
    block_136:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      l1 = i0;
      i0 = HostTempRet0(ctx);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 12u);
      a = i0;
      i0 = !(i0);
      if (i0) {goto block_109;}
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 4u);
      l6 = i1;
      i2 = 1u;
      i1 -= i2;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 != i1;
      if (i0) {goto block_109;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = LoadU32(ctx, i0 + 8u);
      l6 = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 55u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l6;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 55u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l2;
    } else {
      i0 = kF3548ConstD015C;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
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
        i0 = a;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 8u);
        l6 = i1;
        i2 = 1u;
        i1 -= i2;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l6;
        i1 = 1u;
        i0 = i0 != i1;
        if (i0) {goto block_109;}
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        l6 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 56u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l6;
        i1 = a;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 56u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3548ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_109;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_6;}
    block_110:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      l1 = i0;
      i0 = HostTempRet0(ctx);
      a = i0;
    }
    block_109:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 52u);
      a = i0;
      i0 = !(i0);
      l3 = i0;
      if (i0) {goto block_50;}
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 4u);
      l3 = i1;
      i2 = 1u;
      i1 -= i2;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 != i1;
      l3 = i0;
      if (i0) {goto block_50;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      l3 = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l4;
    i1 = 57u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l3;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 57u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3548ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3548ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 != i1;
      l3 = i0;
    }
    i0 = l3;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 8u);
        l3 = i1;
        i2 = 1u;
        i1 -= i2;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 != i1;
        l3 = i0;
        if (i0) {goto block_50;}
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        l3 = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l4;
      i1 = 58u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l3;
        i1 = a;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 58u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3548ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3548ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_50;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_6;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l1 = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
  }
  block_50:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_49:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 52u);
    a = i0;
    i0 = !(i0);
    if (i0) {goto block_9;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 4u);
    l1 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l1;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_9;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    l1 = i0;
    i0 = kF3548ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l4;
  i1 = 59u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l1;
    i1 = a;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 59u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3548ConstD015C;
    i0 = LoadU32(ctx, i0);
    l1 = i0;
    i0 = kF3548ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l1;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_6;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 8u);
    l1 = i1;
    i2 = 1u;
    i1 -= i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l1;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_9;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    l1 = i0;
    i0 = kF3548ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l4;
  i1 = 60u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l1;
    i1 = a;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 60u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3548ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF3548ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_6;}
  }
  block_9:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = kF3548ConstFFFFFFC0;
    i0 -= i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    a = i0;
    i0 = HostTempRet0(ctx);
  }
  i0 = l4;
  i1 = 61u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 61u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
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
  i0 = l4;
  i1 = 62u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 62u;
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
  l8 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l8;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF3548ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l8 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l8;
  i1 = l1;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l8;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l8;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l8;
  i1 = l5;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l8;
  i1 = l6;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l8;
  i1 = l7;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l8;
  i1 = l9;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l8;
  f1 = l11;
  StoreF32(ctx, i0 + 32u, f1);
  i0 = l8;
  j1 = l10;
  StoreU64(ctx, i0 + 36u, j1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 44u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
}
