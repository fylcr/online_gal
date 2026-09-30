// video_player.cpp -- the video element tag dispatch: it drives playback through the input and video handles and folds the player state back into the session..
//
// VideoPlayer::Dispatch -- the video element tag dispatch: it drives playback through the input and video handles and folds the player state back into the session.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 1984534-1984534 (f3698); R1 transcribe-keep.
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
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t SkipWhitespace(EngineContext* ctx, uint32_t text);
void UnorderedLinkBucketNode(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f);
uint32_t PollEventKind(EngineContext* ctx);
void PersistSessionState(EngineContext* ctx, uint32_t a, uint32_t b);
void CopyTaggedTreeKeyToText(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f);

namespace {

constexpr uint32_t kF3698MsgVideo = 0xAF973u;  // video
constexpr uint32_t kF3698ConstAF977 = 0xAF977u;  // 
constexpr uint32_t kF3698MsgInput = 0xB4CA5u;  // input
constexpr uint32_t kF3698ConstB4CA9 = 0xB4CA9u;  // 
constexpr uint32_t kF3698ConstB5E30 = 0xB5E30u;  // 
constexpr uint32_t kF3698MsgPx = 0xB5E31u;  // px != 0
constexpr uint32_t kF3698MsgBoostBoostSmartPtr = 0xB5E39u;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
constexpr uint32_t kF3698MsgOperator = 0xB5E71u;  // operator->
constexpr uint32_t kF3698ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF3698Const656D6974 = 0x656D6974u;  // 
constexpr uint32_t kF3698ConstFFFFFFC0 = 0xFFFFFFC0u;  // 
constexpr uint32_t kF3698ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF3698SaveFrameBytes = 44u;
constexpr uint32_t kF3698ResumeWordBytes = 4u;

}  // namespace

namespace VideoPlayer {
void Dispatch(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l13 = 0u; uint32_t l14 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u; uint32_t i6 = 0u;
  uint64_t j1 = 0; uint64_t j3 = 0;
      l11 = 0, l12 = 0, l13 = 0, l14 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF3698SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l7 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 24u);
    l6 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 28u);
    l8 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 32u);
    l10 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 36u);
    l11 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 40u);
    l12 = i0;
    i0 = l7;
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
    i2 = kF3698ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l9 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  i1 = 0u;
  i2 = ResumePhase(ctx);
  i2 = !(i2);
  if (i2) {
    i2 = GuestStackSp(ctx);
    i3 = kF3698ConstFFFFFFC0;
    i2 += i3;
    l3 = i2;
    GuestStackSp(ctx) = i2;
    i2 = l3;
    i3 = 5u;
    StoreU8(ctx, i2 + 35u, i3);
    i2 = l3;
    i3 = 0u;
    StoreU8(ctx, i2 + 29u, i3);
    i2 = l3;
    i3 = kF3698ConstB4CA9;
    i3 = LoadU8(ctx, i3);
    StoreU8(ctx, i2 + 28u, i3);
    i2 = l3;
    i3 = kF3698MsgInput;
    i3 = LoadU32(ctx, i3);
    StoreU32(ctx, i2 + 24u, i3);
    i2 = b;
    i3 = 16u;
    i2 += i3;
    l11 = i2;
    i2 = b;
    i2 = LoadU32(ctx, i2 + 20u);
    c = i2;
    i2 = !(i2);
    l5 = i2;
  }
  i2 = l5;
  i0 = i2 ? i0 : i1;
  if (i0) {goto block_10;}
  loop_13: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = c;
      i0 = LoadU8(ctx, i0 + 27u);
      l5 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l4 = i0;
      i0 = c;
      i0 = LoadU32(ctx, i0 + 20u);
      i1 = l5;
      i2 = l4;
      i0 = i2 ? i0 : i1;
      l5 = i0;
      i1 = 5u;
      i0 = i0 < i1;
      l8 = i0;
      i0 = l5;
      i1 = 5u;
      i2 = l8;
      i0 = i2 ? i0 : i1;
      l6 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      if (i0) {
        i0 = l3;
        i1 = 24u;
        i0 += i1;
        i1 = c;
        i2 = 16u;
        i1 += i2;
        l10 = i1;
        i1 = LoadU32(ctx, i1);
        i2 = l10;
        i3 = l4;
        i1 = i3 ? i1 : i2;
        l4 = i1;
        i2 = l6;
        i0 = CompareBytes(ctx, i0, i1, i2);
        l10 = i0;
        i0 = !(i0);
        if (i0) {
          i0 = l5;
          i1 = 5u;
          i0 = i0 <= i1;
          l5 = i0;
          if (i0) {goto block_21;}
          goto block_15;
        }
        i0 = l10;
        i1 = kF3698ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        l5 = i0;
        if (i0) {goto block_15;}
        block_21:;
        i0 = l4;
        i1 = l3;
        i2 = 24u;
        i1 += i2;
        i2 = l6;
        i0 = CompareBytes(ctx, i0, i1, i2);
        l5 = i0;
        i0 = !(i0);
        l4 = i0;
        if (i0) {goto block_19;}
        i0 = l5;
        i1 = kF3698ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        l5 = i0;
        if (i0) {goto block_18;}
        goto block_16;
      }
      i0 = l5;
      i1 = 5u;
      i0 = i0 > i1;
      l5 = i0;
      if (i0) {goto block_15;}
      block_19:;
      i0 = l8;
      if (i0) {goto block_16;}
      block_18:;
      i0 = l3;
      i1 = 5u;
      StoreU8(ctx, i0 + 19u, i1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 13u, i1);
      i0 = l3;
      i1 = kF3698MsgInput;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = kF3698ConstB4CA9;
      i1 = LoadU8(ctx, i1);
      StoreU8(ctx, i0 + 12u, i1);
      i0 = kF3698ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = l3;
      i2 = 8u;
      i1 += i2;
      StoreU32(ctx, i0 + 48u, i1);
      i0 = l3;
      i1 = 8u;
      i0 += i1;
      l5 = i0;
      i0 = l3;
      i1 = 48u;
      i0 += i1;
      l4 = i0;
      i0 = l3;
      i1 = 40u;
      i0 += i1;
      l6 = i0;
      i0 = l3;
      i1 = 56u;
      i0 += i1;
      c = i0;
    }
    i0 = l9;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1959u;
      i1 = c;
      i2 = l11;
      i3 = l5;
      i4 = kF3698ConstB5E30;
      i5 = l4;
      i6 = l6;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3698ConstD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kF3698ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 == i1;
      c = i0;
      if (i0) {goto block_6;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 56u);
      l5 = i0;
      i1 = 28u;
      i0 += i1;
      c = i0;
      i0 = l5;
      i0 = LoadS8(ctx, i0 + 39u);
      i1 = kF3698ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l5 = i0;
      if (i0) {
        i0 = c;
        i0 = LoadU32(ctx, i0);
      } else {
        i0 = c;
      }
      i0 = SkipWhitespace(ctx, i0);
      l12 = i0;
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 19u);
      i1 = kF3698ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 8u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l12;
      i1 = 2u;
      i0 = i0 != i1;
      c = i0;
      if (i0) {goto block_10;}
      i0 = a;
      i0 = LoadU8(ctx, i0 + 48u);
      if (i0) {
        i0 = a;
        i0 = LoadU8(ctx, i0 + 49u);
        c = i0;
        if (i0) {goto block_9;}
        i0 = a;
        i0 = LoadU8(ctx, i0 + 50u);
        c = i0;
        if (i0) {goto block_9;}
      }
      i0 = a;
      i0 = LoadU32(ctx, i0 + 13212u);
      c = i0;
      if (i0) {goto block_9;}
      i0 = 2u;
      l12 = i0;
      goto block_10;
    }
    block_16:;
    i0 = c;
    i1 = c;
    i2 = 4u;
    i1 += i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    c = i0;
    block_15:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = c;
      i0 = LoadU32(ctx, i0);
      c = i0;
      if (i0) {goto loop_13;}
    }
  block_10:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 26u, i1);
    i0 = l3;
    i1 = 25971u;
    StoreU16(ctx, i0 + 24u, i1);
    i0 = l3;
    i1 = 2u;
    StoreU8(ctx, i0 + 35u, i1);
    i0 = b;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
  }
  i0 = l5;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = c;
    i1 = l5;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    c = i0;
    loop_33: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = c;
        i0 = LoadU8(ctx, i0 + 27u);
        l6 = i0;
        i1 = 24u;
        i0 <<= (i1 & 31);
        i1 = 24u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l8 = i0;
        i0 = c;
        i0 = LoadU32(ctx, i0 + 20u);
        i1 = l6;
        i2 = l8;
        i0 = i2 ? i0 : i1;
        l6 = i0;
        i1 = 2u;
        i0 = i0 < i1;
        l10 = i0;
        i0 = l6;
        i1 = 2u;
        i2 = l10;
        i0 = i2 ? i0 : i1;
        l4 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        if (i0) {
          i0 = l3;
          i1 = 24u;
          i0 += i1;
          i1 = c;
          i2 = 16u;
          i1 += i2;
          l7 = i1;
          i1 = LoadU32(ctx, i1);
          i2 = l7;
          i3 = l8;
          i1 = i3 ? i1 : i2;
          l8 = i1;
          i2 = l4;
          i0 = CompareBytes(ctx, i0, i1, i2);
          l7 = i0;
          i0 = !(i0);
          if (i0) {
            i0 = l6;
            i1 = 2u;
            i0 = i0 <= i1;
            l6 = i0;
            if (i0) {goto block_41;}
            goto block_35;
          }
          i0 = l7;
          i1 = kF3698ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          l6 = i0;
          if (i0) {goto block_35;}
          block_41:;
          i0 = l8;
          i1 = l3;
          i2 = 24u;
          i1 += i2;
          i2 = l4;
          i0 = CompareBytes(ctx, i0, i1, i2);
          l6 = i0;
          i0 = !(i0);
          l4 = i0;
          if (i0) {goto block_39;}
          i0 = l6;
          i1 = kF3698ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          l4 = i0;
          if (i0) {goto block_38;}
          goto block_36;
        }
        i0 = l6;
        i1 = 2u;
        i0 = i0 > i1;
        l4 = i0;
        if (i0) {goto block_35;}
        block_39:;
        i0 = l10;
        if (i0) {goto block_36;}
        block_38:;
        i0 = l3;
        i1 = 0u;
        StoreU8(ctx, i0 + 26u, i1);
        i0 = l3;
        i1 = 25971u;
        StoreU16(ctx, i0 + 24u, i1);
        i0 = l3;
        i1 = 2u;
        StoreU8(ctx, i0 + 35u, i1);
        i0 = kF3698ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = l3;
        i2 = 24u;
        i1 += i2;
        StoreU32(ctx, i0 + 56u, i1);
        i0 = l3;
        i1 = 24u;
        i0 += i1;
        l5 = i0;
        i0 = l3;
        i1 = 56u;
        i0 += i1;
        l4 = i0;
        i0 = l3;
        i1 = 48u;
        i0 += i1;
        l6 = i0;
        i0 = l3;
        i1 = 8u;
        i0 += i1;
        c = i0;
      }
      i0 = l9;
      i1 = 1u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1959u;
        i1 = c;
        i2 = l11;
        i3 = l5;
        i4 = kF3698ConstB5E30;
        i5 = l4;
        i6 = l6;
        HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
        i0 = 1u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3698ConstD015C;
        i0 = LoadU32(ctx, i0);
        c = i0;
        i0 = kF3698ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = 1u;
        l6 = i0;
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
          i0 = a;
          i1 = 14348u;
          i0 += i1;
          i0 = LoadU32(ctx, i0);
          c = i0;
          i0 = !(i0);
          if (i0) {goto block_48;}
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 8u);
          l4 = i0;
          i0 = LoadU32(ctx, i0 + 32u);
          l10 = i0;
          i0 = l4;
          i0 = LoadU8(ctx, i0 + 39u);
          l5 = i0;
          i1 = 24u;
          i0 <<= (i1 & 31);
          i1 = 24u;
          i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
          i1 = 0u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          l8 = i0;
          i0 = l10;
          i1 = l5;
          i2 = l8;
          i0 = i2 ? i0 : i1;
          l5 = i0;
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 28u);
          i1 = l4;
          i2 = 28u;
          i1 += i2;
          i2 = l8;
          i0 = i2 ? i0 : i1;
          l8 = i0;
          loop_49: 
            i0 = c;
            i0 = LoadU8(ctx, i0 + 27u);
            l4 = i0;
            i1 = 24u;
            i0 <<= (i1 & 31);
            i1 = 24u;
            i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
            i1 = 0u;
            i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
            l7 = i0;
            i0 = l5;
            i1 = c;
            i1 = LoadU32(ctx, i1 + 20u);
            i2 = l4;
            i3 = l7;
            i1 = i3 ? i1 : i2;
            l4 = i1;
            i0 = i0 > i1;
            l14 = i0;
            i0 = l4;
            i1 = l5;
            i2 = l14;
            i0 = i2 ? i0 : i1;
            l10 = i0;
            if (i0) {
              i0 = l8;
              i1 = c;
              i2 = 16u;
              i1 += i2;
              l13 = i1;
              i1 = LoadU32(ctx, i1);
              i2 = l13;
              i3 = l7;
              i1 = i3 ? i1 : i2;
              l7 = i1;
              i2 = l10;
              i0 = CompareBytes(ctx, i0, i1, i2);
              l13 = i0;
              i0 = !(i0);
              if (i0) {
                i0 = l4;
                i1 = l5;
                i0 = i0 <= i1;
                l4 = i0;
                if (i0) {goto block_54;}
                goto block_50;
              }
              i0 = l13;
              i1 = kF3698ConstFFFFFFFF;
              i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
              l4 = i0;
              if (i0) {goto block_50;}
              block_54:;
              i0 = l7;
              i1 = l8;
              i2 = l10;
              i0 = CompareBytes(ctx, i0, i1, i2);
              l4 = i0;
              i0 = !(i0);
              l10 = i0;
              if (i0) {goto block_52;}
              i0 = l4;
              i1 = kF3698ConstFFFFFFFF;
              i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
              l4 = i0;
              if (i0) {goto block_51;}
              i0 = 0u;
              l6 = i0;
              goto block_48;
            }
            i0 = l4;
            i1 = l5;
            i0 = i0 > i1;
            l4 = i0;
            if (i0) {goto block_50;}
            block_52:;
            i0 = l14;
            if (i0) {goto block_51;}
            i0 = 0u;
            l6 = i0;
            goto block_48;
            block_51:;
            i0 = c;
            i1 = 4u;
            i0 += i1;
            c = i0;
            block_50:;
            i0 = c;
            i0 = LoadU32(ctx, i0);
            c = i0;
            if (i0) {goto loop_49;}
          block_48:;
          i0 = l3;
          i0 = LoadS8(ctx, i0 + 35u);
          i1 = kF3698ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          c = i0;
          if (i0) {
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 24u);
            c = i0;
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l6;
          if (i0) {goto block_9;}
          i0 = a;
          i1 = 14344u;
          i0 += i1;
          l5 = i0;
          i0 = l3;
          i1 = 0u;
          StoreU8(ctx, i0 + 28u, i1);
          i0 = l3;
          i1 = kF3698Const656D6974;
          StoreU32(ctx, i0 + 24u, i1);
          i0 = l3;
          i1 = 4u;
          StoreU8(ctx, i0 + 35u, i1);
          i0 = b;
          i0 = LoadU32(ctx, i0 + 20u);
          c = i0;
        }
        i0 = c;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          loop_59: 
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = c;
              i0 = LoadU8(ctx, i0 + 27u);
              b = i0;
              i1 = 24u;
              i0 <<= (i1 & 31);
              i1 = 24u;
              i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
              i1 = 0u;
              i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
              l4 = i0;
              i0 = c;
              i0 = LoadU32(ctx, i0 + 20u);
              i1 = b;
              i2 = l4;
              i0 = i2 ? i0 : i1;
              b = i0;
              i1 = 4u;
              i0 = i0 < i1;
              l8 = i0;
              i0 = b;
              i1 = 4u;
              i2 = l8;
              i0 = i2 ? i0 : i1;
              l6 = i0;
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = l6;
              if (i0) {
                i0 = l3;
                i1 = 24u;
                i0 += i1;
                i1 = c;
                i2 = 16u;
                i1 += i2;
                l10 = i1;
                i1 = LoadU32(ctx, i1);
                i2 = l10;
                i3 = l4;
                i1 = i3 ? i1 : i2;
                l4 = i1;
                i2 = l6;
                i0 = CompareBytes(ctx, i0, i1, i2);
                l10 = i0;
                i0 = !(i0);
                if (i0) {
                  i0 = b;
                  i1 = 4u;
                  i0 = i0 <= i1;
                  b = i0;
                  if (i0) {goto block_67;}
                  goto block_61;
                }
                i0 = l10;
                i1 = kF3698ConstFFFFFFFF;
                i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
                b = i0;
                if (i0) {goto block_61;}
                block_67:;
                i0 = l4;
                i1 = l3;
                i2 = 24u;
                i1 += i2;
                i2 = l6;
                i0 = CompareBytes(ctx, i0, i1, i2);
                b = i0;
                i0 = !(i0);
                l4 = i0;
                if (i0) {goto block_65;}
                i0 = b;
                i1 = kF3698ConstFFFFFFFF;
                i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
                b = i0;
                if (i0) {goto block_64;}
                goto block_62;
              }
              i0 = b;
              i1 = 4u;
              i0 = i0 > i1;
              b = i0;
              if (i0) {goto block_61;}
              block_65:;
              i0 = l8;
              if (i0) {goto block_62;}
              block_64:;
              i0 = a;
              i0 = LoadU32(ctx, i0 + 14336u);
              b = i0;
              if (i0) {goto block_57;}
              i0 = kF3698ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
            }
            i0 = l9;
            i1 = 2u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 857u;
              i1 = kF3698MsgPx;
              i2 = kF3698MsgBoostBoostSmartPtr;
              i3 = 734u;
              i4 = kF3698MsgOperator;
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
              i0 = kF3698ConstD015C;
              i0 = LoadU32(ctx, i0);
              a = i0;
              i0 = kF3698ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = a;
              i1 = 1u;
              i0 = i0 != i1;
              a = i0;
              if (i0) {goto block_8;}
              goto running_exit;
            }
            block_62:;
            i0 = c;
            i1 = c;
            i2 = 4u;
            i1 += i2;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            c = i0;
            block_61:;
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = c;
              i0 = LoadU32(ctx, i0);
              c = i0;
              if (i0) {goto loop_59;}
            }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = a;
          i1 = 2775u;
          i0 += i1;
          i1 = 1u;
          StoreU8(ctx, i0, i1);
          i0 = l3;
          i1 = 0u;
          StoreU8(ctx, i0 + 26u, i1);
          i0 = l3;
          i1 = 25971u;
          StoreU16(ctx, i0 + 24u, i1);
          i0 = l3;
          i1 = 2u;
          StoreU8(ctx, i0 + 35u, i1);
          i0 = kF3698ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = l3;
          i2 = 24u;
          i1 += i2;
          StoreU32(ctx, i0 + 56u, i1);
          i0 = l3;
          i1 = 24u;
          i0 += i1;
          c = i0;
          i0 = l3;
          i1 = 56u;
          i0 += i1;
          l4 = i0;
          i0 = l3;
          i1 = 48u;
          i0 += i1;
          l6 = i0;
          i0 = l3;
          i1 = 8u;
          i0 += i1;
          b = i0;
        }
        i0 = l9;
        i1 = 3u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1959u;
          i1 = b;
          i2 = l11;
          i3 = c;
          i4 = kF3698ConstB5E30;
          i5 = l4;
          i6 = l6;
          HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
          i0 = 3u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3698ConstD015C;
          i0 = LoadU32(ctx, i0);
          b = i0;
          i0 = kF3698ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = b;
          i1 = 1u;
          i0 = i0 == i1;
          b = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = b;
          if (i0) {goto block_75;}
          i0 = kF3698ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 8u);
          i1 = 28u;
          i0 += i1;
          c = i0;
          i0 = a;
          i1 = 2776u;
          i0 += i1;
          b = i0;
        }
        i0 = l9;
        i1 = 4u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 870u;
          i1 = b;
          i2 = c;
          i0 = HostOsQuery(ctx, i0, i1, i2);
          l7 = i0;
          i0 = 4u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l7;
          c = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3698ConstD015C;
          i0 = LoadU32(ctx, i0);
          c = i0;
          i0 = kF3698ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = c;
          i1 = 1u;
          i0 = i0 == i1;
          c = i0;
          if (i0) {goto block_75;}
          i0 = l3;
          i0 = LoadS8(ctx, i0 + 35u);
          i1 = kF3698ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 24u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l3;
          i1 = b;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l3;
          i1 = 8u;
          i0 += i1;
          l4 = i0;
          i0 = l3;
          i1 = 56u;
          i0 += i1;
          l6 = i0;
          i0 = l3;
          i1 = 24u;
          i0 += i1;
          c = i0;
        }
        i0 = l9;
        i1 = 5u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = c;
          i1 = l5;
          i2 = b;
          i3 = kF3698ConstB5E30;
          i4 = l4;
          i5 = l6;
          CopyTaggedTreeKeyToText(ctx, i0, i1, i2, i3, i4, i5);
          i0 = 5u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = a;
          i1 = 2788u;
          i0 += i1;
          b = i0;
          i1 = l3;
          i1 = LoadU32(ctx, i1 + 24u);
          i1 = LoadU32(ctx, i1 + 28u);
          c = i1;
          StoreU32(ctx, i0, i1);
          goto block_29;
        }
        block_75:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          c = i0;
          i0 = HostTempRet0(ctx);
          i0 = l3;
          i0 = LoadS8(ctx, i0 + 35u);
          i1 = kF3698ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_5;}
          goto block_3;
        }
        block_57:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i1 = 0u;
          StoreU8(ctx, i0 + 26u, i1);
          i0 = l3;
          i1 = 25971u;
          StoreU16(ctx, i0 + 24u, i1);
          i0 = l3;
          i1 = 2u;
          StoreU8(ctx, i0 + 35u, i1);
          i0 = kF3698ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = l3;
          i2 = 24u;
          i1 += i2;
          StoreU32(ctx, i0 + 56u, i1);
          i0 = l3;
          i1 = 24u;
          i0 += i1;
          l4 = i0;
          i0 = l3;
          i1 = 56u;
          i0 += i1;
          l6 = i0;
          i0 = l3;
          i1 = 48u;
          i0 += i1;
          l8 = i0;
          i0 = l3;
          i1 = 8u;
          i0 += i1;
          c = i0;
        }
        i0 = l9;
        i1 = 6u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1959u;
          i1 = c;
          i2 = l11;
          i3 = l4;
          i4 = kF3698ConstB5E30;
          i5 = l6;
          i6 = l8;
          HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
          i0 = 6u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3698ConstD015C;
          i0 = LoadU32(ctx, i0);
          c = i0;
          i0 = kF3698ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = c;
          i1 = 1u;
          i0 = i0 == i1;
          c = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = c;
          if (i0) {goto block_87;}
          i0 = l3;
          i1 = l3;
          i1 = LoadU32(ctx, i1 + 8u);
          i2 = 28u;
          i1 += i2;
          c = i1;
          StoreU32(ctx, i0 + 56u, i1);
          i0 = kF3698ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = 56u;
          i0 += i1;
          l6 = i0;
          i0 = l3;
          i1 = 48u;
          i0 += i1;
          l8 = i0;
          i0 = l3;
          i1 = 8u;
          i0 += i1;
          l4 = i0;
        }
        i0 = l9;
        i1 = 7u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2609u;
          i1 = l4;
          i2 = l5;
          i3 = c;
          i4 = kF3698ConstB5E30;
          i5 = l6;
          i6 = l8;
          HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
          i0 = 7u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3698ConstD015C;
          i0 = LoadU32(ctx, i0);
          c = i0;
          i0 = kF3698ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = c;
          i1 = 1u;
          i0 = i0 == i1;
          c = i0;
          if (i0) {goto block_87;}
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 8u);
          i0 = LoadU32(ctx, i0 + 28u);
          c = i0;
          i0 = kF3698ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l9;
        i1 = 8u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2610u;
          i1 = b;
          i2 = c;
          i0 = HostOsQuery(ctx, i0, i1, i2);
          l7 = i0;
          i0 = 8u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l7;
          b = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3698ConstD015C;
          i0 = LoadU32(ctx, i0);
          c = i0;
          i0 = kF3698ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = c;
          i1 = 1u;
          i0 = i0 == i1;
          c = i0;
          if (i0) {goto block_87;}
          i0 = l3;
          i0 = LoadS8(ctx, i0 + 35u);
          i1 = kF3698ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 24u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = b;
          i0 = !(i0);
          c = i0;
          if (i0) {goto block_9;}
          i0 = a;
          i1 = 2772u;
          i0 += i1;
          i1 = 1u;
          StoreU8(ctx, i0, i1);
          i0 = l3;
          i1 = 0u;
          StoreU8(ctx, i0 + 28u, i1);
          i0 = l3;
          i1 = kF3698Const656D6974;
          StoreU32(ctx, i0 + 24u, i1);
          i0 = l3;
          i1 = 4u;
          StoreU8(ctx, i0 + 35u, i1);
          i0 = kF3698ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = l3;
          i2 = 24u;
          i1 += i2;
          StoreU32(ctx, i0 + 56u, i1);
          i0 = l3;
          i1 = 24u;
          i0 += i1;
          l5 = i0;
          i0 = l3;
          i1 = 56u;
          i0 += i1;
          l4 = i0;
          i0 = l3;
          i1 = 48u;
          i0 += i1;
          l6 = i0;
          i0 = l3;
          i1 = 8u;
          i0 += i1;
          c = i0;
        }
        i0 = l9;
        i1 = 9u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1959u;
          i1 = c;
          i2 = l11;
          i3 = l5;
          i4 = kF3698ConstB5E30;
          i5 = l4;
          i6 = l6;
          HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
          i0 = 9u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3698ConstD015C;
          i0 = LoadU32(ctx, i0);
          c = i0;
          i0 = kF3698ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = c;
          i1 = 1u;
          i0 = i0 == i1;
          c = i0;
          if (i0) {goto block_86;}
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 8u);
          l5 = i0;
          i1 = 28u;
          i0 += i1;
          c = i0;
          i0 = a;
          i1 = 2768u;
          i0 += i1;
          i1 = l5;
          i1 = LoadS8(ctx, i1 + 39u);
          i2 = kF3698ConstFFFFFFFF;
          i1 = (uint32_t)((int32_t)i1 <= (int32_t)i2);
          if (i1) {
            i1 = c;
            i1 = LoadU32(ctx, i1);
          } else {
            i1 = c;
          }
          i1 = SkipWhitespace(ctx, i1);
          c = i1;
          i2 = b;
          i1 += i2;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i0 = LoadS8(ctx, i0 + 35u);
          i1 = kF3698ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          b = i0;
          if (i0) {goto block_29;}
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 24u);
          b = i0;
          DeallocateRawBlock(ctx, i0);
          goto block_29;
        }
        block_87:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          c = i0;
          i0 = HostTempRet0(ctx);
          i0 = l3;
          i0 = LoadS8(ctx, i0 + 35u);
          i1 = kF3698ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_5;}
          goto block_3;
        }
        block_86:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          c = i0;
          i0 = HostTempRet0(ctx);
          i0 = l3;
          i0 = LoadS8(ctx, i0 + 35u);
          i1 = kF3698ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_5;}
          goto block_3;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = HostRet0(ctx);
        c = i0;
        i0 = HostTempRet0(ctx);
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 35u);
        i1 = kF3698ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        a = i0;
        if (i0) {goto block_5;}
        goto block_3;
      }
      block_36:;
      i0 = c;
      i1 = c;
      i2 = 4u;
      i1 += i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      c = i0;
      block_35:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = c;
        i0 = LoadU32(ctx, i0);
        c = i0;
        if (i0) {goto loop_33;}
      }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = 5u;
    StoreU8(ctx, i0 + 35u, i1);
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 29u, i1);
    i0 = l3;
    i1 = kF3698MsgVideo;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l3;
    i1 = kF3698ConstAF977;
    i1 = LoadU8(ctx, i1);
    b = i1;
    StoreU8(ctx, i0 + 28u, i1);
  }
  i0 = l5;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = c;
    i1 = l5;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    c = i0;
    loop_103: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = c;
        i0 = LoadU8(ctx, i0 + 27u);
        b = i0;
        i1 = 24u;
        i0 <<= (i1 & 31);
        i1 = 24u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l4 = i0;
        i0 = c;
        i0 = LoadU32(ctx, i0 + 20u);
        i1 = b;
        i2 = l4;
        i0 = i2 ? i0 : i1;
        b = i0;
        i1 = 5u;
        i0 = i0 < i1;
        l8 = i0;
        i0 = b;
        i1 = 5u;
        i2 = l8;
        i0 = i2 ? i0 : i1;
        l6 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        if (i0) {
          i0 = l3;
          i1 = 24u;
          i0 += i1;
          i1 = c;
          i2 = 16u;
          i1 += i2;
          l10 = i1;
          i1 = LoadU32(ctx, i1);
          i2 = l10;
          i3 = l4;
          i1 = i3 ? i1 : i2;
          l4 = i1;
          i2 = l6;
          i0 = CompareBytes(ctx, i0, i1, i2);
          l10 = i0;
          i0 = !(i0);
          if (i0) {
            i0 = b;
            i1 = 5u;
            i0 = i0 <= i1;
            b = i0;
            if (i0) {goto block_111;}
            goto block_105;
          }
          i0 = l10;
          i1 = kF3698ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          b = i0;
          if (i0) {goto block_105;}
          block_111:;
          i0 = l4;
          i1 = l3;
          i2 = 24u;
          i1 += i2;
          i2 = l6;
          i0 = CompareBytes(ctx, i0, i1, i2);
          b = i0;
          i0 = !(i0);
          l4 = i0;
          if (i0) {goto block_109;}
          i0 = b;
          i1 = kF3698ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          b = i0;
          if (i0) {goto block_108;}
          goto block_106;
        }
        i0 = b;
        i1 = 5u;
        i0 = i0 > i1;
        b = i0;
        if (i0) {goto block_105;}
        block_109:;
        i0 = l8;
        if (i0) {goto block_106;}
        block_108:;
        i0 = a;
        i1 = 2792u;
        i0 += i1;
        i1 = 1u;
        StoreU8(ctx, i0, i1);
        i0 = l3;
        i1 = 5u;
        StoreU8(ctx, i0 + 35u, i1);
        i0 = l3;
        i1 = kF3698MsgVideo;
        i1 = LoadU32(ctx, i1);
        StoreU32(ctx, i0 + 24u, i1);
        i0 = l3;
        i1 = kF3698ConstAF977;
        i1 = LoadU8(ctx, i1);
        StoreU8(ctx, i0 + 28u, i1);
        i0 = l3;
        i1 = 0u;
        StoreU8(ctx, i0 + 29u, i1);
        i0 = kF3698ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = l3;
        i2 = 24u;
        i1 += i2;
        StoreU32(ctx, i0 + 56u, i1);
        i0 = l3;
        i1 = 24u;
        i0 += i1;
        c = i0;
        i0 = l3;
        i1 = 56u;
        i0 += i1;
        l5 = i0;
        i0 = l3;
        i1 = 48u;
        i0 += i1;
        l4 = i0;
        i0 = l3;
        i1 = 8u;
        i0 += i1;
        b = i0;
      }
      i0 = l9;
      i1 = 10u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1959u;
        i1 = b;
        i2 = l11;
        i3 = c;
        i4 = kF3698ConstB5E30;
        i5 = l5;
        i6 = l4;
        HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
        i0 = 10u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3698ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3698ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_7;}
        i0 = kF3698ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 8u);
        i1 = 28u;
        i0 += i1;
        c = i0;
        i0 = a;
        i1 = 2796u;
        i0 += i1;
        b = i0;
      }
      i0 = l9;
      i1 = 11u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 870u;
        i1 = b;
        i2 = c;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l7 = i0;
        i0 = 11u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l7;
        b = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3698ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3698ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_7;}
        i0 = l3;
        i0 = LoadS8(ctx, i0 + 35u);
        i1 = kF3698ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        b = i0;
        if (i0) {goto block_29;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 24u);
        b = i0;
        DeallocateRawBlock(ctx, i0);
        goto block_29;
      }
      block_106:;
      i0 = c;
      i1 = c;
      i2 = 4u;
      i1 += i2;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      c = i0;
      block_105:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = c;
        i0 = LoadU32(ctx, i0);
        c = i0;
        if (i0) {goto loop_103;}
      }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  i1 = 0u;
  i2 = ResumePhase(ctx);
  i2 = !(i2);
  if (i2) {
    i2 = l3;
    i3 = 0u;
    StoreU8(ctx, i2 + 32u, i3);
    i2 = l3;
    j3 = 8028073573554152307ull;
    StoreU64(ctx, i2 + 24u, j3);
    i2 = l3;
    i3 = 8u;
    StoreU8(ctx, i2 + 35u, i3);
    i2 = l5;
    i2 = !(i2);
    b = i2;
  }
  i2 = b;
  i0 = i2 ? i0 : i1;
  if (i0) {goto block_124;}
  loop_127: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i0 = LoadU8(ctx, i0 + 27u);
      b = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l6 = i0;
      i0 = l5;
      i0 = LoadU32(ctx, i0 + 20u);
      i1 = b;
      i2 = l6;
      i0 = i2 ? i0 : i1;
      b = i0;
      i1 = 8u;
      i0 = i0 < i1;
      l4 = i0;
      i0 = b;
      i1 = 8u;
      i2 = l4;
      i0 = i2 ? i0 : i1;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = c;
      if (i0) {
        i0 = l3;
        i1 = 24u;
        i0 += i1;
        i1 = l5;
        i2 = 16u;
        i1 += i2;
        l8 = i1;
        i1 = LoadU32(ctx, i1);
        i2 = l8;
        i3 = l6;
        i1 = i3 ? i1 : i2;
        l6 = i1;
        i2 = c;
        i0 = CompareBytes(ctx, i0, i1, i2);
        l8 = i0;
        i0 = !(i0);
        if (i0) {
          i0 = b;
          i1 = 8u;
          i0 = i0 <= i1;
          b = i0;
          if (i0) {goto block_135;}
          goto block_129;
        }
        i0 = l8;
        i1 = kF3698ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        b = i0;
        if (i0) {goto block_129;}
        block_135:;
        i0 = l6;
        i1 = l3;
        i2 = 24u;
        i1 += i2;
        i2 = c;
        i0 = CompareBytes(ctx, i0, i1, i2);
        b = i0;
        i0 = !(i0);
        c = i0;
        if (i0) {goto block_133;}
        i0 = b;
        i1 = kF3698ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        b = i0;
        if (i0) {goto block_132;}
        goto block_130;
      }
      i0 = b;
      i1 = 8u;
      i0 = i0 > i1;
      b = i0;
      if (i0) {goto block_129;}
      block_133:;
      i0 = l4;
      if (i0) {goto block_130;}
      block_132:;
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 16u, i1);
      i0 = l3;
      j1 = 8028073573554152307ull;
      StoreU64(ctx, i0 + 8u, j1);
      i0 = l3;
      i1 = 8u;
      StoreU8(ctx, i0 + 19u, i1);
      i0 = kF3698ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = l3;
      i2 = 8u;
      i1 += i2;
      StoreU32(ctx, i0 + 48u, i1);
      i0 = l3;
      i1 = 8u;
      i0 += i1;
      c = i0;
      i0 = l3;
      i1 = 48u;
      i0 += i1;
      l5 = i0;
      i0 = l3;
      i1 = 40u;
      i0 += i1;
      l4 = i0;
      i0 = l3;
      i1 = 56u;
      i0 += i1;
      b = i0;
    }
    i0 = l9;
    i1 = 12u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1959u;
      i1 = b;
      i2 = l11;
      i3 = c;
      i4 = kF3698ConstB5E30;
      i5 = l5;
      i6 = l4;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 12u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3698ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3698ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
      if (i0) {goto block_123;}
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 56u);
      c = i0;
      i1 = 28u;
      i0 += i1;
      b = i0;
      i0 = c;
      i0 = LoadS8(ctx, i0 + 39u);
      i1 = kF3698ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
      } else {
        i0 = b;
      }
      i0 = SkipWhitespace(ctx, i0);
      b = i0;
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 19u);
      i1 = kF3698ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      c = i0;
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 8u);
        c = i0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = b;
      i1 = 1u;
      i0 -= i1;
      b = i0;
      switch (i0) {
        case 0: goto block_122;
        case 1: goto block_121;
        default: goto block_124;
      }
    }
    block_130:;
    i0 = l5;
    i1 = l5;
    i2 = 4u;
    i1 += i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l5 = i0;
    block_129:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      if (i0) {goto loop_127;}
    }
  block_124:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = 2772u;
    i0 += i1;
    i1 = 1u;
    StoreU8(ctx, i0, i1);
    i0 = PollEventKind(ctx);
    c = i0;
    i0 = l3;
    i1 = 0u;
    StoreU8(ctx, i0 + 28u, i1);
    i0 = l3;
    i1 = kF3698Const656D6974;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l3;
    i1 = 4u;
    StoreU8(ctx, i0 + 35u, i1);
    i0 = kF3698ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = l3;
    i2 = 24u;
    i1 += i2;
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    i1 = 24u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i1 = 56u;
    i0 += i1;
    l4 = i0;
    i0 = l3;
    i1 = 48u;
    i0 += i1;
    l6 = i0;
    i0 = l3;
    i1 = 8u;
    i0 += i1;
    b = i0;
  }
  i0 = l9;
  i1 = 13u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = b;
    i2 = l11;
    i3 = l5;
    i4 = kF3698ConstB5E30;
    i5 = l4;
    i6 = l6;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 13u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3698ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3698ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    b = i0;
    if (i0) {goto block_119;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    l5 = i0;
    i1 = 28u;
    i0 += i1;
    b = i0;
    i0 = a;
    i1 = 2768u;
    i0 += i1;
    l4 = i0;
    i1 = l5;
    i1 = LoadS8(ctx, i1 + 39u);
    i2 = kF3698ConstFFFFFFFF;
    i1 = (uint32_t)((int32_t)i1 <= (int32_t)i2);
    l5 = i1;
    if (i1) {
      i1 = b;
      i1 = LoadU32(ctx, i1);
    } else {
      i1 = b;
    }
    i1 = SkipWhitespace(ctx, i1);
    i2 = c;
    i1 += i2;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 35u);
    i1 = kF3698ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 24u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = a;
    i0 = LoadU8(ctx, i0 + 48u);
    if (i0) {
      i0 = a;
      i0 = LoadU8(ctx, i0 + 49u);
      b = i0;
      if (i0) {goto block_118;}
      i0 = a;
      i0 = LoadU8(ctx, i0 + 50u);
      b = i0;
      if (i0) {goto block_118;}
    }
    i0 = a;
    i0 = LoadU32(ctx, i0 + 13212u);
    b = i0;
    if (i0) {goto block_118;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
    b = i0;
    if (i0) {goto block_120;}
    i0 = kF3698ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l9;
  i1 = 14u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 857u;
    i1 = kF3698MsgPx;
    i2 = kF3698MsgBoostBoostSmartPtr;
    i3 = 734u;
    i4 = kF3698MsgOperator;
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
    i0 = kF3698ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF3698ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 != i1;
    a = i0;
    if (i0) {goto block_8;}
    goto running_exit;
  }
  block_123:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 19u);
    i1 = kF3698ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_5;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    DeallocateRawBlock(ctx, i0);
    i0 = c;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_122:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = 2773u;
    i0 += i1;
    i1 = 1u;
    StoreU8(ctx, i0, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
    b = i0;
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
      i0 = kF3698ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l9;
    i1 = 15u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3698MsgPx;
      i2 = kF3698MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3698MsgOperator;
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
      i0 = kF3698ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3698ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_8;}
      goto running_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i0 = LoadU32(ctx, i0 + 100u);
    b = i0;
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
      i0 = kF3698ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l9;
    i1 = 16u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3698MsgPx;
      i2 = kF3698MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3698MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 16u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3698ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3698ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_8;}
      goto running_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = a;
    i2 = 13308u;
    i1 += i2;
    c = i1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 8u;
    i0 += i1;
    l6 = i0;
    i0 = l3;
    i1 = 56u;
    i0 += i1;
    l8 = i0;
    i0 = l3;
    i1 = 24u;
    i0 += i1;
    l5 = i0;
    i0 = a;
    i1 = 13536u;
    i0 += i1;
    l4 = i0;
  }
  i0 = l9;
  i1 = 17u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l5;
    i1 = l4;
    i2 = c;
    i3 = kF3698ConstB5E30;
    i4 = l6;
    i5 = l8;
    UnorderedLinkBucketNode(ctx, i0, i1, i2, i3, i4, i5);
    i0 = 17u;
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
    i0 = LoadU32(ctx, i0 + 380u);
    l5 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 24u);
    i1 = 28u;
    i0 += i1;
    c = i0;
  }
  i0 = l9;
  i1 = 18u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = c;
    i2 = 1u;
    i3 = l5;
    CallTableT5(ctx, i3, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    i0 = 18u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_29;}
  block_121:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = 2774u;
    i0 += i1;
    b = i0;
    i1 = 1u;
    StoreU8(ctx, i0, i1);
    goto block_29;
  }
  block_120:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i0 = LoadU32(ctx, i0 + 100u);
    b = i0;
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
      i0 = kF3698ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l9;
    i1 = 19u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3698MsgPx;
      i2 = kF3698MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3698MsgOperator;
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
      i0 = kF3698ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3698ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_8;}
      goto running_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = a;
    i2 = 13308u;
    i1 += i2;
    c = i1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = 8u;
    i0 += i1;
    l6 = i0;
    i0 = l3;
    i1 = 56u;
    i0 += i1;
    l8 = i0;
    i0 = l3;
    i1 = 24u;
    i0 += i1;
    l5 = i0;
    i0 = a;
    i1 = 13536u;
    i0 += i1;
    l4 = i0;
  }
  i0 = l9;
  i1 = 20u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l5;
    i1 = l4;
    i2 = c;
    i3 = kF3698ConstB5E30;
    i4 = l6;
    i5 = l8;
    UnorderedLinkBucketNode(ctx, i0, i1, i2, i3, i4, i5);
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
    i0 = LoadU32(ctx, i0 + 380u);
    l5 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 24u);
    i1 = 28u;
    i0 += i1;
    c = i0;
  }
  i0 = l9;
  i1 = 21u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = c;
    i2 = 1u;
    i3 = l5;
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
  if (i0) {goto block_29;}
  block_119:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 35u);
    i1 = kF3698ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_5;}
    goto block_3;
  }
  block_118:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
    b = i0;
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
      i0 = kF3698ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l9;
    i1 = 22u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3698MsgPx;
      i2 = kF3698MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3698MsgOperator;
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
      i0 = kF3698ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3698ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_8;}
      goto running_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i0 = LoadU32(ctx, i0 + 100u);
    b = i0;
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
      i0 = kF3698ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l9;
    i1 = 23u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 857u;
      i1 = kF3698MsgPx;
      i2 = kF3698MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3698MsgOperator;
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
      i0 = kF3698ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3698ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_8;}
      goto running_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 380u);
    l5 = i0;
    i0 = a;
    i1 = 13524u;
    i0 += i1;
    c = i0;
  }
  i0 = l9;
  i1 = 24u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = c;
    i2 = 1u;
    i3 = l5;
    CallTableT5(ctx, i3, i0, i1, i2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    i0 = 24u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_29:;
  i0 = b;
  i1 = 1u;
  i2 = 4u;
  i3 = l12;
  i4 = 1u;
  i3 = i3 == i4;
  i1 = i3 ? i1 : i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  b = i0;
  i0 = l9;
  i1 = 25u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = b;
    PersistSessionState(ctx, i0, i1);
    i0 = 25u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_9:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = kF3698ConstFFFFFFC0;
    i0 -= i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    TrapUnreachable();
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 35u);
    i1 = kF3698ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_5;}
    goto block_3;
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    c = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 19u);
    i1 = kF3698ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_5;}
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    HostOsRelease(ctx, i0);
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
  i0 = l9;
  i1 = 26u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 26u;
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
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 24u);
    DeallocateRawBlock(ctx, i0);
    i0 = c;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  goto func_exit;
  spill_frame:;
  l7 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l7;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF3698ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l7 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l7;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l7;
  i1 = c;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l7;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l7;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l7;
  i1 = l5;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l7;
  i1 = l6;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l7;
  i1 = l8;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l7;
  i1 = l10;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = l7;
  i1 = l11;
  StoreU32(ctx, i0 + 36u, i1);
  i0 = l7;
  i1 = l12;
  StoreU32(ctx, i0 + 40u, i1);
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
