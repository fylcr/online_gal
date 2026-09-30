// load_proto_slot.cpp -- loads a prototype slot: the prototype's slot word is fetched and projected onto the stack..
//
// LoadProtoSlot -- loads a prototype slot: the prototype's slot word is fetched and projected onto the stack.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 300042-300042 (f782); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void ReserveClosureRecords(EngineContext* ctx, uint32_t state, uint32_t count);
void ClearFlagsRecursive(EngineContext* ctx, uint32_t state, uint32_t node);
uint32_t FindPoolEntryByTagByte(EngineContext* ctx, uint32_t pool, uint32_t key);
void RelocateElementArray(EngineContext* ctx, uint32_t state, uint32_t table);
uint32_t FetchMetamethod(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

namespace {

constexpr uint32_t kF782ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF782SaveFrameBytes = 16u;
constexpr uint32_t kF782ResumeWordBytes = 4u;

}  // namespace

uint32_t LoadProtoSlot(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF782SaveFrameBytes;
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
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF782ResumeWordBytes;
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
    i0 = LoadU32(ctx, i0 + 36u);
    l2 = i0;
    i0 = LoadU8(ctx, i0 + 5u);
    i1 = 4u;
    i0 |= i1;
    l3 = i0;
    i0 = l2;
    i1 = l3;
    StoreU8(ctx, i0 + 5u, i1);
    i0 = l2;
    i0 = LoadU8(ctx, i0 + 4u);
    i1 = 5u;
    i0 -= i1;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    switch (i0) {
      case 0: goto block_12;
      case 1: goto block_11;
      case 2: goto block_7;
      case 3: goto block_10;
      case 4: goto block_8;
      default: goto block_7;
    }
    block_12:;
    i0 = a;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 24u);
    StoreU32(ctx, i0 + 36u, i1);
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 8u);
    l1 = i0;
    i0 = !(i0);
    if (i0) {goto block_13;}
    i0 = l1;
    i0 = LoadU8(ctx, i0 + 5u);
    i1 = 3u;
    i0 &= i1;
    if (i0) {
      i0 = a;
      i1 = l1;
      ClearFlagsRecursive(ctx, i0, i1);
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 8u);
      l1 = i0;
      i0 = !(i0);
      if (i0) {goto block_13;}
    }
    i0 = l1;
    i0 = LoadU8(ctx, i0 + 6u);
    i1 = 8u;
    i0 &= i1;
    if (i0) {goto block_13;}
    i0 = l1;
    i1 = 3u;
    i2 = a;
    i2 = LoadU32(ctx, i2 + 200u);
    i0 = FetchMetamethod(ctx, i0, i1, i2);
    l1 = i0;
    i0 = !(i0);
    if (i0) {goto block_13;}
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = 4u;
    i0 = i0 != i1;
    if (i0) {goto block_13;}
    i0 = l1;
    i0 = LoadU32(ctx, i0);
    i1 = 16u;
    i0 += i1;
    l1 = i0;
    i1 = 107u;
    i0 = FindPoolEntryByTagByte(ctx, i0, i1);
    l3 = i0;
    i0 = l3;
    i1 = l1;
    i2 = 118u;
    i1 = FindPoolEntryByTagByte(ctx, i1, i2);
    l1 = i1;
    i0 |= i1;
    if (i0) {
      i0 = l2;
      i1 = l2;
      i1 = LoadU8(ctx, i1 + 5u);
      i2 = 231u;
      i1 &= i2;
      i2 = l1;
      i3 = 0u;
      i2 = i2 != i3;
      i3 = 4u;
      i2 <<= (i3 & 31);
      i3 = l3;
      i4 = 0u;
      i3 = i3 != i4;
      i4 = 3u;
      i3 <<= (i4 & 31);
      i2 |= i3;
      i1 |= i2;
      StoreU8(ctx, i0 + 5u, i1);
      i0 = l2;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 44u);
      StoreU32(ctx, i0 + 24u, i1);
      i0 = a;
      i1 = l2;
      StoreU32(ctx, i0 + 44u, i1);
    }
    i0 = l3;
    i1 = 0u;
    i2 = l1;
    i0 = i2 ? i0 : i1;
    if (i0) {goto block_5;}
    i0 = l1;
    if (i0) {goto block_6;}
    i0 = l3;
    i1 = 0u;
    i0 = i0 != i1;
    l7 = i0;
    block_13:;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 28u);
    l1 = i0;
    if (i0) {
      loop_17: 
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 12u);
        i1 = l1;
        i2 = 1u;
        i1 -= i2;
        l1 = i1;
        i2 = 4u;
        i1 <<= (i2 & 31);
        i0 += i1;
        l3 = i0;
        i0 = LoadU32(ctx, i0 + 8u);
        i1 = 4u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        if (i0) {goto block_18;}
        i0 = l3;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        i0 = LoadU8(ctx, i0 + 5u);
        i1 = 3u;
        i0 &= i1;
        i0 = !(i0);
        if (i0) {goto block_18;}
        i0 = a;
        i1 = l3;
        ClearFlagsRecursive(ctx, i0, i1);
        block_18:;
        i0 = l1;
        if (i0) {goto loop_17;}
    }
    i0 = kF782ConstFFFFFFFF;
    i1 = l2;
    i1 = LoadU8(ctx, i1 + 7u);
    i0 <<= (i1 & 31);
    i1 = kF782ConstFFFFFFFF;
    i0 ^= i1;
    l1 = i0;
    i0 = l7;
    if (i0) {
      loop_21: 
        i0 = l1;
        l3 = i0;
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 16u);
        i1 = l1;
        i2 = 5u;
        i1 <<= (i2 & 31);
        i0 += i1;
        l4 = i0;
        i0 = LoadU32(ctx, i0 + 8u);
        l1 = i0;
        if (i0) {
          i0 = l1;
          i1 = 4u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          if (i0) {goto block_22;}
          i0 = l4;
          i0 = LoadU32(ctx, i0);
          l1 = i0;
          i0 = LoadU8(ctx, i0 + 5u);
          i1 = 3u;
          i0 &= i1;
          i0 = !(i0);
          if (i0) {goto block_22;}
          i0 = a;
          i1 = l1;
          ClearFlagsRecursive(ctx, i0, i1);
          goto block_22;
        }
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 24u);
        i1 = 4u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        if (i0) {goto block_22;}
        i0 = l4;
        i1 = 11u;
        StoreU32(ctx, i0 + 24u, i1);
        block_22:;
        i0 = l3;
        i1 = 1u;
        i0 -= i1;
        l1 = i0;
        i0 = l3;
        if (i0) {goto loop_21;}
      goto block_19;
    }
    loop_24: 
      i0 = l1;
      l3 = i0;
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 16u);
      i1 = l1;
      i2 = 5u;
      i1 <<= (i2 & 31);
      i0 += i1;
      l6 = i0;
      l1 = i0;
      i1 = 24u;
      i0 += i1;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = l1;
      i0 = LoadU32(ctx, i0 + 8u);
      l4 = i0;
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        i1 = 4u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        if (i0) {goto block_25;}
        i0 = l1;
        i1 = 11u;
        StoreU32(ctx, i0 + 24u, i1);
        goto block_25;
      }
      i0 = l5;
      i1 = 4u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      if (i0) {goto block_27;}
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 16u);
      l1 = i0;
      i0 = LoadU8(ctx, i0 + 5u);
      i1 = 3u;
      i0 &= i1;
      i0 = !(i0);
      if (i0) {goto block_27;}
      i0 = a;
      i1 = l1;
      ClearFlagsRecursive(ctx, i0, i1);
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 8u);
      l4 = i0;
      block_27:;
      i0 = l4;
      i1 = 4u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      if (i0) {goto block_25;}
      i0 = l6;
      i0 = LoadU32(ctx, i0);
      l1 = i0;
      i0 = LoadU8(ctx, i0 + 5u);
      i1 = 3u;
      i0 &= i1;
      i0 = !(i0);
      if (i0) {goto block_25;}
      i0 = a;
      i1 = l1;
      ClearFlagsRecursive(ctx, i0, i1);
      block_25:;
      i0 = l3;
      i1 = 1u;
      i0 -= i1;
      l1 = i0;
      i0 = l3;
      if (i0) {goto loop_24;}
    block_19:;
    i0 = l7;
    i0 = !(i0);
    if (i0) {goto running_exit;}
    goto block_5;
    block_11:;
    i0 = a;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 36u, i1);
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 12u);
    l1 = i0;
    i0 = LoadU8(ctx, i0 + 5u);
    i1 = 3u;
    i0 &= i1;
    if (i0) {
      i0 = a;
      i1 = l1;
      ClearFlagsRecursive(ctx, i0, i1);
    }
    i0 = l2;
    i0 = LoadU8(ctx, i0 + 6u);
    if (i0) {
      i0 = 0u;
      l1 = i0;
      i0 = l2;
      i0 = LoadU8(ctx, i0 + 7u);
      l3 = i0;
      i0 = !(i0);
      if (i0) {
        i0 = 0u;
        l3 = i0;
        goto block_29;
      }
      loop_32: 
        i0 = l2;
        i1 = l1;
        i2 = 4u;
        i1 <<= (i2 & 31);
        i0 += i1;
        l4 = i0;
        i0 = LoadU32(ctx, i0 + 32u);
        i1 = 4u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        if (i0) {goto block_33;}
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 24u);
        l4 = i0;
        i0 = LoadU8(ctx, i0 + 5u);
        i1 = 3u;
        i0 &= i1;
        i0 = !(i0);
        if (i0) {goto block_33;}
        i0 = a;
        i1 = l4;
        ClearFlagsRecursive(ctx, i0, i1);
        i0 = l2;
        i0 = LoadU8(ctx, i0 + 7u);
        l3 = i0;
        block_33:;
        i0 = l1;
        i1 = 1u;
        i0 += i1;
        l1 = i0;
        i1 = l3;
        i2 = 255u;
        i1 &= i2;
        i0 = i0 < i1;
        if (i0) {goto loop_32;}
      goto block_29;
    }
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 16u);
    l1 = i0;
    i0 = LoadU8(ctx, i0 + 5u);
    i1 = 3u;
    i0 &= i1;
    if (i0) {
      i0 = a;
      i1 = l1;
      ClearFlagsRecursive(ctx, i0, i1);
    }
    i0 = 0u;
    l1 = i0;
    i0 = l2;
    i0 = LoadU8(ctx, i0 + 7u);
    l3 = i0;
    i0 = !(i0);
    if (i0) {
      i0 = 0u;
      l3 = i0;
      goto block_29;
    }
    loop_36: 
      i0 = l2;
      i1 = l1;
      i2 = 2u;
      i1 <<= (i2 & 31);
      i0 += i1;
      i0 = LoadU32(ctx, i0 + 20u);
      l4 = i0;
      i0 = LoadU8(ctx, i0 + 5u);
      i1 = 3u;
      i0 &= i1;
      if (i0) {
        i0 = a;
        i1 = l4;
        ClearFlagsRecursive(ctx, i0, i1);
        i0 = l2;
        i0 = LoadU8(ctx, i0 + 7u);
        l3 = i0;
      }
      i0 = l1;
      i1 = 1u;
      i0 += i1;
      l1 = i0;
      i1 = l3;
      i2 = 255u;
      i1 &= i2;
      i0 = i0 < i1;
      if (i0) {goto loop_36;}
    block_29:;
    i0 = l3;
    i1 = 255u;
    i0 &= i1;
    a = i0;
    i0 = l2;
    i0 = LoadU8(ctx, i0 + 6u);
    if (i0) {
      i0 = a;
      i1 = 4u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 += i1;
      goto func_exit;
    }
    i0 = a;
    i1 = 2u;
    i0 <<= (i1 & 31);
    i1 = 20u;
    i0 += i1;
    goto func_exit;
    block_10:;
    i0 = a;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 108u);
    StoreU32(ctx, i0 + 36u, i1);
    i0 = l2;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 40u);
    StoreU32(ctx, i0 + 108u, i1);
    i0 = a;
    i1 = l2;
    StoreU32(ctx, i0 + 40u, i1);
    i0 = l2;
    i1 = l2;
    i1 = LoadU8(ctx, i1 + 5u);
    i2 = 251u;
    i1 &= i2;
    StoreU8(ctx, i0 + 5u, i1);
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 80u);
    i1 = 4u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    if (i0) {goto block_39;}
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 72u);
    l1 = i0;
    i0 = LoadU8(ctx, i0 + 5u);
    i1 = 3u;
    i0 &= i1;
    i0 = !(i0);
    if (i0) {goto block_39;}
    i0 = a;
    i1 = l1;
    ClearFlagsRecursive(ctx, i0, i1);
    block_39:;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
    l3 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 40u);
    l1 = i0;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 20u);
    l7 = i1;
    i0 = i0 <= i1;
    if (i0) {
      i0 = l4;
      l3 = i0;
      loop_41: 
        i0 = l1;
        i0 = LoadU32(ctx, i0 + 8u);
        l5 = i0;
        i1 = l3;
        i0 = i0 > i1;
        l6 = i0;
        i0 = l5;
        i1 = l3;
        i2 = l6;
        i0 = i2 ? i0 : i1;
        l3 = i0;
        i0 = l7;
        i1 = l1;
        i2 = 24u;
        i1 += i2;
        l1 = i1;
        i0 = i0 >= i1;
        if (i0) {goto loop_41;}
    }
    i0 = l4;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 32u);
    l1 = i1;
    i0 = i0 > i1;
    if (i0) {
      loop_43: 
        i0 = l1;
        i0 = LoadU32(ctx, i0 + 8u);
        i1 = 4u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        if (i0) {goto block_44;}
        i0 = l1;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i0 = LoadU8(ctx, i0 + 5u);
        i1 = 3u;
        i0 &= i1;
        i0 = !(i0);
        if (i0) {goto block_44;}
        i0 = a;
        i1 = l5;
        ClearFlagsRecursive(ctx, i0, i1);
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 8u);
        l4 = i0;
        block_44:;
        i0 = l4;
        i1 = l1;
        i2 = 16u;
        i1 += i2;
        l1 = i1;
        i0 = i0 > i1;
        if (i0) {goto loop_43;}
    }
    i0 = l1;
    i1 = l3;
    i0 = i0 <= i1;
    if (i0) {
      loop_46: 
        i0 = l1;
        i1 = 0u;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l3;
        i1 = l1;
        i2 = 16u;
        i1 += i2;
        l1 = i1;
        i0 = i0 >= i1;
        if (i0) {goto loop_46;}
    }
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 48u);
    l4 = i0;
    i1 = 20000u;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    if (i0) {goto block_47;}
    i0 = l3;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 32u);
    i0 -= i1;
    l1 = i0;
    i0 = l4;
    i1 = 17u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l3 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 20u);
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 40u);
    i0 -= i1;
    i1 = 24u;
    i0 = I32_DIV_S(i0, i1);
    a = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    if (i0) {goto block_49;}
    i0 = l4;
    i1 = a;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    a = i0;
    if (i0) {goto block_49;}
    i0 = l4;
    i1 = 1u;
    i0 >>= (i1 & 31);
    a = i0;
  }
  i0 = l8;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = a;
    ReserveClosureRecords(ctx, i0, i1);
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_49:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 44u);
    a = i0;
    i1 = l1;
    i2 = 2u;
    i1 = (uint32_t)((int32_t)i1 >> (i2 & 31));
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    l1 = i0;
    if (i0) {goto block_47;}
    i0 = a;
    i1 = 91u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l1 = i0;
    if (i0) {goto block_47;}
    i0 = a;
    i1 = 1u;
    i0 >>= (i1 & 31);
    a = i0;
  }
  i0 = l8;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = a;
    RelocateElementArray(ctx, i0, i1);
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  block_47:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 48u);
    i1 = 24u;
    i0 *= i1;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 44u);
    i2 = 4u;
    i1 <<= (i2 & 31);
    i0 += i1;
    i1 = 120u;
    i0 += i1;
    goto func_exit;
  }
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 68u);
    StoreU32(ctx, i0 + 36u, i1);
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 32u);
    l1 = i0;
    if (i0) {
      i0 = l1;
      i1 = l1;
      i1 = LoadU8(ctx, i1 + 5u);
      i2 = 252u;
      i1 &= i2;
      StoreU8(ctx, i0 + 5u, i1);
    }
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 40u);
    l4 = i0;
    i1 = 1u;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    if (i0) {
      i0 = 0u;
      l1 = i0;
      loop_58: 
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 8u);
        i1 = l1;
        i2 = 4u;
        i1 <<= (i2 & 31);
        i0 += i1;
        l3 = i0;
        i0 = LoadU32(ctx, i0 + 8u);
        i1 = 4u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        if (i0) {goto block_59;}
        i0 = l3;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        i0 = LoadU8(ctx, i0 + 5u);
        i1 = 3u;
        i0 &= i1;
        i0 = !(i0);
        if (i0) {goto block_59;}
        i0 = a;
        i1 = l3;
        ClearFlagsRecursive(ctx, i0, i1);
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 40u);
        l4 = i0;
        block_59:;
        i0 = l4;
        i1 = l1;
        i2 = 1u;
        i1 += i2;
        l1 = i1;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {goto loop_58;}
    }
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 36u);
    l4 = i0;
    i1 = 1u;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    if (i0) {
      i0 = 0u;
      l1 = i0;
      loop_61: 
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 28u);
        i1 = l1;
        i2 = 2u;
        i1 <<= (i2 & 31);
        i0 += i1;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        if (i0) {
          i0 = l3;
          i1 = l3;
          i1 = LoadU8(ctx, i1 + 5u);
          i2 = 252u;
          i1 &= i2;
          StoreU8(ctx, i0 + 5u, i1);
          i0 = l2;
          i0 = LoadU32(ctx, i0 + 36u);
          l4 = i0;
        }
        i0 = l4;
        i1 = l1;
        i2 = 1u;
        i1 += i2;
        l1 = i1;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {goto loop_61;}
    }
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 52u);
    l4 = i0;
    i1 = 1u;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    if (i0) {
      i0 = 0u;
      l1 = i0;
      loop_64: 
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 16u);
        i1 = l1;
        i2 = 2u;
        i1 <<= (i2 & 31);
        i0 += i1;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        i0 = !(i0);
        if (i0) {goto block_65;}
        i0 = l3;
        i0 = LoadU8(ctx, i0 + 5u);
        i1 = 3u;
        i0 &= i1;
        i0 = !(i0);
        if (i0) {goto block_65;}
        i0 = a;
        i1 = l3;
        ClearFlagsRecursive(ctx, i0, i1);
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 52u);
        l4 = i0;
        block_65:;
        i0 = l4;
        i1 = l1;
        i2 = 1u;
        i1 += i2;
        l1 = i1;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {goto loop_64;}
    }
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 56u);
    a = i0;
    i1 = 1u;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    if (i0) {
      i0 = 0u;
      l1 = i0;
      loop_67: 
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 24u);
        i1 = l1;
        i2 = 12u;
        i1 *= i2;
        i0 += i1;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        if (i0) {
          i0 = l3;
          i1 = l3;
          i1 = LoadU8(ctx, i1 + 5u);
          i2 = 252u;
          i1 &= i2;
          StoreU8(ctx, i0 + 5u, i1);
          i0 = l2;
          i0 = LoadU32(ctx, i0 + 56u);
          a = i0;
        }
        i0 = l1;
        i1 = 1u;
        i0 += i1;
        l1 = i0;
        i1 = a;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        if (i0) {goto loop_67;}
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 52u);
      l4 = i0;
    }
    i0 = a;
    i1 = 12u;
    i0 *= i1;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 40u);
    i2 = l2;
    i2 = LoadU32(ctx, i2 + 80u);
    i1 += i2;
    i2 = 4u;
    i1 <<= (i2 & 31);
    i0 += i1;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 48u);
    i2 = l2;
    i2 = LoadU32(ctx, i2 + 44u);
    i3 = l4;
    i2 += i3;
    i1 += i2;
    l3 = i1;
    i2 = l2;
    i2 = LoadU32(ctx, i2 + 36u);
    i1 += i2;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    a = i0;
    i1 = 92u;
    i0 += i1;
    l1 = i0;
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    goto func_exit;
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF782ConstFFFFFFFF;
    i1 = l2;
    i1 = LoadU8(ctx, i1 + 7u);
    i0 <<= (i1 & 31);
    i1 = kF782ConstFFFFFFFF;
    i0 ^= i1;
    l1 = i0;
    i0 = l3;
    if (i0) {
      loop_72: 
        i0 = l1;
        a = i0;
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 16u);
        i1 = l1;
        i2 = 5u;
        i1 <<= (i2 & 31);
        i0 += i1;
        l1 = i0;
        i0 = LoadU32(ctx, i0 + 8u);
        if (i0) {goto block_73;}
        i0 = l1;
        i0 = LoadU32(ctx, i0 + 24u);
        i1 = 4u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        if (i0) {goto block_73;}
        i0 = l1;
        i1 = 11u;
        StoreU32(ctx, i0 + 24u, i1);
        block_73:;
        i0 = a;
        i1 = 1u;
        i0 -= i1;
        l1 = i0;
        i0 = a;
        if (i0) {goto loop_72;}
      goto block_5;
    }
    loop_74: 
      i0 = l1;
      l3 = i0;
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 16u);
      i1 = l1;
      i2 = 5u;
      i1 <<= (i2 & 31);
      i0 += i1;
      l5 = i0;
      l1 = i0;
      i1 = 24u;
      i0 += i1;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = l1;
      i0 = LoadU32(ctx, i0 + 8u);
      if (i0) {
        i0 = l4;
        i1 = 4u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        if (i0) {goto block_75;}
        i0 = l5;
        i0 = LoadU32(ctx, i0 + 16u);
        l1 = i0;
        i0 = LoadU8(ctx, i0 + 5u);
        i1 = 3u;
        i0 &= i1;
        i0 = !(i0);
        if (i0) {goto block_75;}
        i0 = a;
        i1 = l1;
        ClearFlagsRecursive(ctx, i0, i1);
        goto block_75;
      }
      i0 = l4;
      i1 = 4u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      if (i0) {goto block_75;}
      i0 = l1;
      i1 = 11u;
      StoreU32(ctx, i0 + 24u, i1);
      block_75:;
      i0 = l3;
      i1 = 1u;
      i0 -= i1;
      l1 = i0;
      i0 = l3;
      if (i0) {goto loop_74;}
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i1 = l2;
    i1 = LoadU8(ctx, i1 + 5u);
    i2 = 251u;
    i1 &= i2;
    StoreU8(ctx, i0 + 5u, i1);
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 28u);
    i1 = 4u;
    i0 <<= (i1 & 31);
    i1 = 32u;
    i2 = l2;
    i2 = LoadU8(ctx, i2 + 7u);
    i1 <<= (i2 & 31);
    i0 += i1;
    i1 = 32u;
    i0 += i1;
    goto func_exit;
  }
  TrapUnreachable();
  spill_frame:;
  l4 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l4;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF782ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = l1;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l4;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l4;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 16u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
