// close_function_scope.cpp -- the function-scope teardown: closes the open upvalues, restores the scope chain, clears the local and upvalue flag bytes, and flushes the proto and string slots the function left behind..
//
// CloseFunctionScope -- the function-scope teardown: closes the open upvalues, restores the scope chain, clears the local and upvalue flag bytes, and flushes the proto and string slots the function left behind.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 487980-489073 (f1125); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void ResizeTableArray(EngineContext* ctx, uint32_t a, uint32_t b);
void ClearLocalFlags(EngineContext* ctx, uint32_t a);
void ClearUpvalueFlags(EngineContext* ctx, uint32_t a);
void InvokeMetaField(EngineContext* ctx, uint32_t a);
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size);
void ClearFlagsRecursive(EngineContext* ctx, uint32_t state, uint32_t node);
uint32_t FetchMetamethod(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t LoadProtoSlot(EngineContext* ctx, uint32_t a);
uint32_t ReshapeTaggedNodeChain(EngineContext* ctx, uint32_t state, uint32_t node, uint32_t tag);

namespace {

constexpr uint32_t kF1125ConstFFFFFFFD = 0xFFFFFFFDu;  // 
constexpr uint32_t kF1125ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF1125SaveFrameBytes = 32u;
constexpr uint32_t kF1125ResumeWordBytes = 4u;

}  // namespace

uint32_t CloseFunctionScope(EngineContext* ctx, uint32_t state) {
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
    i2 = kF1125SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 24u);
    l7 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 28u);
    l9 = i0;
    i0 = l4;
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
    i2 = kF1125ResumeWordBytes;
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
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    l2 = i0;
    i0 = LoadU8(ctx, i0 + 21u);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    switch (i0) {
      case 0: goto block_10;
      case 1: goto block_9;
      case 2: goto block_7;
      case 3: goto block_6;
      case 4: goto block_5;
      default: goto running_exit;
    }
    block_10:;
    i0 = state;
    ClearUpvalueFlags(ctx, i0);
    i0 = 0u;
    goto func_exit;
    block_9:;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 36u);
    l1 = i0;
  }
  i0 = l1;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l8;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i0 = LoadProtoSlot(ctx, i0);
      l4 = i0;
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      state = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      goto func_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 140u);
    l1 = i0;
    i1 = l2;
    i2 = 120u;
    i1 += i2;
    l3 = i1;
    i0 = i0 == i1;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    if (i0) {goto block_15;}
    loop_17: 
      i0 = l1;
      i0 = LoadU8(ctx, i0 + 5u);
      i1 = 7u;
      i0 &= i1;
      if (i0) {goto block_18;}
      i0 = l1;
      i0 = LoadU32(ctx, i0 + 8u);
      l6 = i0;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = 4u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      if (i0) {goto block_18;}
      i0 = l6;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = LoadU8(ctx, i0 + 5u);
      i1 = 3u;
      i0 &= i1;
      i0 = !(i0);
      if (i0) {goto block_18;}
      i0 = l2;
      i1 = l6;
      ClearFlagsRecursive(ctx, i0, i1);
      block_18:;
      i0 = l3;
      i1 = l1;
      i1 = LoadU32(ctx, i1 + 20u);
      l1 = i1;
      i0 = i0 != i1;
      l5 = i0;
      if (i0) {goto loop_17;}
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 36u);
    i0 = !(i0);
    l1 = i0;
    if (i0) {goto block_15;}
  }
  loop_19: 
    i0 = l8;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i0 = LoadProtoSlot(ctx, i0);
      l4 = i0;
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l1 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 36u);
      l1 = i0;
      if (i0) {goto loop_19;}
    }
  block_15:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 44u);
    l1 = i0;
    i0 = l2;
    i1 = 0u;
    StoreU32(ctx, i0 + 44u, i1);
    i0 = l2;
    i1 = l1;
    StoreU32(ctx, i0 + 36u, i1);
    i0 = state;
    i0 = LoadU8(ctx, i0 + 5u);
    i1 = 3u;
    i0 &= i1;
    if (i0) {
      i0 = l2;
      i1 = state;
      ClearFlagsRecursive(ctx, i0, i1);
    }
    i0 = l2;
    ClearLocalFlags(ctx, i0);
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 36u);
    l1 = i0;
  }
  i0 = l1;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    loop_25: 
      i0 = l8;
      i1 = 2u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i0 = LoadProtoSlot(ctx, i0);
        l4 = i0;
        i0 = 2u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l4;
        l1 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 36u);
        l1 = i0;
        if (i0) {goto loop_25;}
      }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 40u);
    l1 = i0;
    i0 = l2;
    i1 = 0u;
    StoreU32(ctx, i0 + 40u, i1);
    i0 = l2;
    i1 = l1;
    StoreU32(ctx, i0 + 36u, i1);
  }
  i0 = l1;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    loop_30: 
      i0 = l8;
      i1 = 3u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i0 = LoadProtoSlot(ctx, i0);
        l4 = i0;
        i0 = 3u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l4;
        l1 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 36u);
        l1 = i0;
        if (i0) {goto loop_30;}
      }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    l7 = i0;
    i0 = LoadU32(ctx, i0 + 112u);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    l1 = i0;
    if (i0) {
      loop_35: 
        i0 = l1;
        i0 = LoadU8(ctx, i0 + 5u);
        l6 = i0;
        i1 = 8u;
        i0 &= i1;
        i0 = !(i0);
        i1 = 0u;
        i2 = l6;
        i3 = 3u;
        i2 &= i3;
        i0 = i2 ? i0 : i1;
        i0 = !(i0);
        l5 = i0;
        if (i0) {
          i0 = l1;
          l3 = i0;
          goto block_36;
        }
        i0 = l1;
        i0 = LoadU32(ctx, i0 + 8u);
        l5 = i0;
        i0 = !(i0);
        if (i0) {goto block_39;}
        i0 = l5;
        i0 = LoadU8(ctx, i0 + 6u);
        i1 = 4u;
        i0 &= i1;
        if (i0) {goto block_39;}
        i0 = l5;
        i1 = 2u;
        i2 = state;
        i2 = LoadU32(ctx, i2 + 16u);
        i2 = LoadU32(ctx, i2 + 196u);
        i0 = FetchMetamethod(ctx, i0, i1, i2);
        l5 = i0;
        if (i0) {goto block_38;}
        i0 = l1;
        i0 = LoadU8(ctx, i0 + 5u);
        l6 = i0;
        block_39:;
        i0 = l1;
        i1 = l6;
        i2 = 8u;
        i1 |= i2;
        StoreU8(ctx, i0 + 5u, i1);
        i0 = l1;
        l3 = i0;
        goto block_36;
        block_38:;
        i0 = l1;
        i1 = l1;
        i1 = LoadU8(ctx, i1 + 5u);
        i2 = 8u;
        i1 |= i2;
        StoreU8(ctx, i0 + 5u, i1);
        i0 = l1;
        i0 = LoadU32(ctx, i0 + 16u);
        l6 = i0;
        i0 = l3;
        i1 = l1;
        i1 = LoadU32(ctx, i1);
        StoreU32(ctx, i0, i1);
        i0 = l6;
        i1 = l9;
        i0 += i1;
        i1 = 24u;
        i0 += i1;
        l9 = i0;
        i0 = l7;
        i0 = LoadU32(ctx, i0 + 48u);
        l6 = i0;
        i0 = !(i0);
        l5 = i0;
        if (i0) {
          i0 = l1;
          i1 = l1;
          StoreU32(ctx, i0, i1);
          goto block_40;
        }
        i0 = l1;
        i1 = l6;
        i1 = LoadU32(ctx, i1);
        StoreU32(ctx, i0, i1);
        i0 = l7;
        i0 = LoadU32(ctx, i0 + 48u);
        l5 = i0;
        i1 = l1;
        StoreU32(ctx, i0, i1);
        block_40:;
        i0 = l7;
        i1 = l1;
        StoreU32(ctx, i0 + 48u, i1);
        block_36:;
        i0 = l3;
        i0 = LoadU32(ctx, i0);
        l1 = i0;
        if (i0) {goto loop_35;}
    }
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 48u);
    l1 = i0;
    if (i0) {
      loop_43: 
        i0 = l1;
        i0 = LoadU32(ctx, i0);
        l1 = i0;
        i0 = LoadU8(ctx, i0 + 5u);
        i1 = 248u;
        i0 &= i1;
        l3 = i0;
        i1 = l2;
        i1 = LoadU8(ctx, i1 + 20u);
        i2 = 3u;
        i1 &= i2;
        i0 |= i1;
        state = i0;
        i0 = l1;
        i1 = state;
        StoreU8(ctx, i0 + 5u, i1);
        i0 = l2;
        i1 = l1;
        ClearFlagsRecursive(ctx, i0, i1);
        i0 = l1;
        i1 = l2;
        i1 = LoadU32(ctx, i1 + 48u);
        i0 = i0 != i1;
        if (i0) {goto loop_43;}
    }
    i0 = 0u;
    l7 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 36u);
    state = i0;
  }
  i0 = state;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    loop_45: 
      i0 = l8;
      i1 = 4u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i0 = LoadProtoSlot(ctx, i0);
        l4 = i0;
        i0 = 4u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l4;
        state = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = state;
        i1 = l7;
        i0 += i1;
        l7 = i0;
        i0 = l2;
        i0 = LoadU32(ctx, i0 + 36u);
        state = i0;
        if (i0) {goto loop_45;}
      }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 44u);
    l3 = i0;
    if (i0) {
      loop_50: 
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 28u);
        l1 = i0;
        i0 = !(i0);
        if (i0) {goto block_51;}
        i0 = l3;
        i0 = LoadU8(ctx, i0 + 5u);
        i1 = 16u;
        i0 &= i1;
        i0 = !(i0);
        if (i0) {goto block_51;}
        loop_52: 
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 12u);
          i1 = l1;
          i2 = 1u;
          i1 -= i2;
          l1 = i1;
          i2 = 4u;
          i1 <<= (i2 & 31);
          i0 += i1;
          state = i0;
          i0 = LoadU32(ctx, i0 + 8u);
          l6 = i0;
          i1 = 4u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          if (i0) {goto block_53;}
          i0 = l6;
          i1 = 4u;
          i0 = i0 == i1;
          if (i0) {
            i0 = state;
            i0 = LoadU32(ctx, i0);
            state = i0;
            i0 = LoadU8(ctx, i0 + 5u);
            i1 = 252u;
            i0 &= i1;
            l5 = i0;
            i0 = state;
            i1 = l5;
            StoreU8(ctx, i0 + 5u, i1);
            goto block_53;
          }
          i0 = state;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU8(ctx, i0 + 5u);
          l5 = i0;
          i1 = 3u;
          i0 &= i1;
          i0 = !(i0);
          if (i0) {
            i0 = l6;
            i1 = 7u;
            i0 = i0 != i1;
            if (i0) {goto block_53;}
            i0 = l5;
            i1 = 8u;
            i0 &= i1;
            i0 = !(i0);
            if (i0) {goto block_53;}
          }
          i0 = state;
          i1 = 0u;
          StoreU32(ctx, i0 + 8u, i1);
          block_53:;
          i0 = l1;
          if (i0) {goto loop_52;}
        block_51:;
        i0 = kF1125ConstFFFFFFFF;
        i1 = l3;
        i1 = LoadU8(ctx, i1 + 7u);
        i0 <<= (i1 & 31);
        i1 = kF1125ConstFFFFFFFF;
        i0 ^= i1;
        state = i0;
        loop_56: 
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 16u);
          i1 = state;
          l1 = i1;
          i2 = 5u;
          i1 <<= (i2 & 31);
          i0 += i1;
          state = i0;
          i0 = LoadU32(ctx, i0 + 8u);
          l6 = i0;
          i0 = !(i0);
          if (i0) {goto block_57;}
          i0 = state;
          i0 = LoadU32(ctx, i0 + 24u);
          l5 = i0;
          i1 = 4u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          if (i0) {goto block_59;}
          i0 = l5;
          i1 = 4u;
          i0 = i0 == i1;
          if (i0) {
            i0 = state;
            i0 = LoadU32(ctx, i0 + 16u);
            l6 = i0;
            i0 = LoadU8(ctx, i0 + 5u);
            i1 = 252u;
            i0 &= i1;
            l5 = i0;
            i0 = l6;
            i1 = l5;
            StoreU8(ctx, i0 + 5u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l6 = i0;
            goto block_59;
          }
          i0 = state;
          i0 = LoadU32(ctx, i0 + 16u);
          i0 = LoadU8(ctx, i0 + 5u);
          i1 = 3u;
          i0 &= i1;
          if (i0) {goto block_58;}
          block_59:;
          i0 = l6;
          i1 = 4u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          if (i0) {goto block_57;}
          i0 = l6;
          i1 = 4u;
          i0 = i0 == i1;
          if (i0) {
            i0 = state;
            i0 = LoadU32(ctx, i0);
            state = i0;
            i0 = LoadU8(ctx, i0 + 5u);
            i1 = 252u;
            i0 &= i1;
            l5 = i0;
            i0 = state;
            i1 = l5;
            StoreU8(ctx, i0 + 5u, i1);
            goto block_57;
          }
          i0 = state;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU8(ctx, i0 + 5u);
          l5 = i0;
          i1 = 3u;
          i0 &= i1;
          if (i0) {goto block_58;}
          i0 = l6;
          i1 = 7u;
          i0 = i0 != i1;
          if (i0) {goto block_57;}
          i0 = l5;
          i1 = 8u;
          i0 &= i1;
          i0 = !(i0);
          if (i0) {goto block_57;}
          block_58:;
          i0 = state;
          i1 = 0u;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = state;
          i0 = LoadU32(ctx, i0 + 24u);
          i1 = 4u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          if (i0) {goto block_57;}
          i0 = state;
          i1 = 11u;
          StoreU32(ctx, i0 + 24u, i1);
          block_57:;
          i0 = l1;
          i1 = 1u;
          i0 -= i1;
          state = i0;
          i0 = l1;
          if (i0) {goto loop_56;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 24u);
        l3 = i0;
        if (i0) {goto loop_50;}
    }
    i0 = l2;
    i1 = 0u;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l2;
    i1 = 2u;
    StoreU8(ctx, i0 + 21u, i1);
    i0 = l2;
    i1 = l2;
    i2 = 28u;
    i1 += i2;
    StoreU32(ctx, i0 + 32u, i1);
    i0 = l2;
    i1 = l2;
    i1 = LoadU8(ctx, i1 + 20u);
    i2 = 3u;
    i1 ^= i2;
    StoreU8(ctx, i0 + 20u, i1);
    i0 = l2;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 68u);
    i2 = l7;
    i3 = l9;
    i2 += i3;
    i1 -= i2;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = 0u;
    goto func_exit;
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 24u);
    l1 = i1;
    i2 = 1u;
    i1 += i2;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 68u);
    l3 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i1 = l1;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    l1 = i0;
  }
  i0 = l8;
  i1 = 5u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l1;
    i2 = kF1125ConstFFFFFFFD;
    i0 = ReshapeTaggedNodeChain(ctx, i0, i1, i2);
    l4 = i0;
    i0 = 5u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    state = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 24u);
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 8u);
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    if (i0) {
      i0 = l2;
      i1 = 3u;
      StoreU8(ctx, i0 + 21u, i1);
    }
    i0 = l2;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 72u);
    i2 = l2;
    i2 = LoadU32(ctx, i2 + 68u);
    i3 = l3;
    i2 -= i3;
    i1 += i2;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = 10u;
    goto func_exit;
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 68u);
    l6 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 32u);
    l1 = i0;
  }
  i0 = l8;
  i1 = 6u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l1;
    i2 = 40u;
    i0 = ReshapeTaggedNodeChain(ctx, i0, i1, i2);
    l4 = i0;
    i0 = 6u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i1 = l1;
    StoreU32(ctx, i0 + 32u, i1);
    i0 = l1;
    i0 = LoadU32(ctx, i0);
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
      i0 = state;
      i0 = LoadU32(ctx, i0 + 16u);
      l1 = i0;
      i0 = LoadU32(ctx, i0 + 8u);
      l3 = i0;
      i1 = kF1125ResumeWordBytes;
      i0 = I32_DIV_S(i0, i1);
      l9 = i0;
      i0 = l3;
      i1 = 65u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l5 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      if (i0) {goto block_72;}
      i0 = l9;
      i1 = l1;
      i1 = LoadU32(ctx, i1 + 4u);
      i0 = i0 <= i1;
      l5 = i0;
      if (i0) {goto block_72;}
      i0 = l3;
      i1 = 1u;
      i0 >>= (i1 & 31);
      l3 = i0;
    }
    i0 = l8;
    i1 = 7u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = l3;
      ResizeTableArray(ctx, i0, i1);
      i0 = 7u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    block_72:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l1;
      i0 = LoadU32(ctx, i0 + 60u);
      l3 = i0;
      i1 = 65u;
      i0 = i0 >= i1;
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
        i0 = state;
        l5 = i0;
        i0 = l1;
        i0 = LoadU32(ctx, i0 + 52u);
        l7 = i0;
        i0 = l3;
        i1 = 1u;
        i0 >>= (i1 & 31);
        state = i0;
      }
      i0 = l8;
      i1 = 8u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l5;
        i1 = l7;
        i2 = l3;
        i3 = state;
        i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
        l4 = i0;
        i0 = 8u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l4;
        l3 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l1;
        i1 = state;
        StoreU32(ctx, i0 + 60u, i1);
        i0 = l1;
        i1 = l3;
        StoreU32(ctx, i0 + 52u, i1);
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i1 = 4u;
      StoreU8(ctx, i0 + 21u, i1);
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i1 = l2;
    i1 = LoadU32(ctx, i1 + 72u);
    i2 = l2;
    i2 = LoadU32(ctx, i2 + 68u);
    i3 = l6;
    i2 -= i3;
    i1 += i2;
    StoreU32(ctx, i0 + 72u, i1);
    i0 = 400u;
    goto func_exit;
  }
  block_5:;
  i0 = ResumePhase(ctx);
  if (i0) {
    i0 = l3;
  } else {
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 48u);
  }
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l8;
    i1 = 9u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      InvokeMetaField(ctx, i0);
      i0 = 9u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = 100u;
      l1 = i0;
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 72u);
      state = i0;
      i1 = 101u;
      i0 = i0 < i1;
      if (i0) {goto running_exit;}
      i0 = l2;
      i1 = state;
      i2 = 100u;
      i1 -= i2;
      StoreU32(ctx, i0 + 72u, i1);
      i0 = 100u;
      goto func_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i1 = 0u;
    StoreU32(ctx, i0 + 76u, i1);
    i0 = l2;
    i1 = 0u;
    StoreU8(ctx, i0 + 21u, i1);
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
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
  i2 = kF1125ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  i1 = state;
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
  i0 = l4;
  i1 = l5;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l4;
  i1 = l6;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l4;
  i1 = l7;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l4;
  i1 = l9;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 32u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
