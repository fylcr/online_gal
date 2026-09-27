// index_table_value.cpp -- the table index with metamethod fallback and call plumbing, answering the looked-up value..
//
// IndexTableValue -- the table index with metamethod fallback and call plumbing, answering the looked-up value.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 347488-347488 (f896); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t ResolveTableField(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void GrowValueStack(EngineContext* ctx, uint32_t a0, uint32_t a1);
void RaiseScriptError(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t FetchObjectMetamethod(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void CallValue(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t FetchMetamethod(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void RaiseTypeError(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

constexpr uint32_t kF896MsgIndex = 0x2BEDFu;  // index
constexpr uint32_t kF896MsgLoopInSettable = 0x2BEF6u;  // loop in settable
constexpr uint32_t kF896ConstFFFFFFC0 = 0xFFFFFFC0u;  // 
constexpr uint32_t kF896SaveFrameBytes = 36u;
constexpr uint32_t kF896ResumeWordBytes = 4u;

}  // namespace

void IndexTableValue(EngineContext* ctx, uint32_t state, uint32_t table, uint32_t key, uint32_t result) {
  EnterFrame();
  uint32_t l10 = 0u; uint64_t l11 = 0; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  uint64_t j0 = 0; uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF896SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    key = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 12u);
    result = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 24u);
    l7 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 28u);
    l8 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 32u);
    l10 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 4u);
    table = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF896ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l9 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    i1 = 16u;
    i0 -= i1;
    l8 = i0;
    GuestStackSp(ctx) = i0;
    i0 = table;
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
  }
  loop_5: 
    i0 = l4;
    i1 = l4;
    i2 = 5u;
    i1 = i1 == i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l4 = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = table;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
      }
      i0 = l9;
      i0 = !(i0);
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = l4;
        i2 = key;
        i0 = ResolveTableField(ctx, i0, i1, i2);
        l5 = i0;
        i0 = 0u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l5;
        l7 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l7;
        i0 = LoadU32(ctx, i0 + 8u);
        if (i0) {goto block_11;}
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 8u);
        l6 = i0;
        i0 = !(i0);
        if (i0) {goto block_11;}
        i0 = l6;
        i0 = LoadU8(ctx, i0 + 6u);
        i1 = 2u;
        i0 &= i1;
        if (i0) {goto block_11;}
        i0 = l6;
        i1 = 1u;
        i2 = state;
        i2 = LoadU32(ctx, i2 + 16u);
        i2 = LoadU32(ctx, i2 + 192u);
        i0 = FetchMetamethod(ctx, i0, i1, i2);
        l6 = i0;
        if (i0) {goto block_6;}
        block_11:;
        i0 = l7;
        i1 = result;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0, j1);
        i0 = l7;
        i1 = result;
        i1 = LoadU32(ctx, i1 + 8u);
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l4;
        i1 = 0u;
        StoreU8(ctx, i0 + 6u, i1);
        i0 = result;
        i0 = LoadU32(ctx, i0 + 8u);
        i1 = 4u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        if (i0) {goto running_exit;}
        i0 = result;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU8(ctx, i0 + 5u);
        i1 = 3u;
        i0 &= i1;
        i0 = !(i0);
        if (i0) {goto running_exit;}
        i0 = l4;
        i0 = LoadU8(ctx, i0 + 5u);
        i1 = 4u;
        i0 &= i1;
        i0 = !(i0);
        if (i0) {goto running_exit;}
        i0 = state;
        i0 = LoadU32(ctx, i0 + 16u);
        state = i0;
        i0 = l4;
        i1 = l4;
        i1 = LoadU8(ctx, i1 + 5u);
        i2 = 251u;
        i1 &= i2;
        StoreU8(ctx, i0 + 5u, i1);
        i0 = l4;
        i1 = state;
        i1 = LoadU32(ctx, i1 + 40u);
        StoreU32(ctx, i0 + 24u, i1);
        i0 = state;
        i1 = l4;
        StoreU32(ctx, i0 + 40u, i1);
        goto running_exit;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i1 = table;
      i2 = 1u;
      i0 = FetchObjectMetamethod(ctx, i0, i1, i2);
      l6 = i0;
      i0 = LoadU32(ctx, i0 + 8u);
      l4 = i0;
      if (i0) {goto block_6;}
    }
    i0 = l9;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = table;
      i2 = kF896MsgIndex;
      RaiseTypeError(ctx, i0, i1, i2);
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    block_6:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 8u);
      l4 = i0;
      i1 = 6u;
      i0 = i0 == i1;
      l7 = i0;
    }
    i0 = l7;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = table;
      } else {
        i0 = state;
        i0 = LoadU32(ctx, i0 + 8u);
        l4 = i0;
        i1 = l6;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0, j1);
        i0 = l4;
        i1 = l6;
        i1 = LoadU32(ctx, i1 + 8u);
        l6 = i1;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = state;
        i0 = LoadU32(ctx, i0 + 8u);
        l4 = i0;
        i1 = table;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0 + 16u, j1);
        i0 = l4;
        i1 = table;
        i1 = LoadU32(ctx, i1 + 8u);
        StoreU32(ctx, i0 + 24u, i1);
        i0 = state;
        i0 = LoadU32(ctx, i0 + 8u);
        table = i0;
        i1 = key;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0 + 32u, j1);
        i0 = table;
        i1 = key;
        i1 = LoadU32(ctx, i1 + 8u);
        StoreU32(ctx, i0 + 40u, i1);
        i0 = state;
        i0 = LoadU32(ctx, i0 + 8u);
        table = i0;
        i1 = result;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0 + 48u, j1);
        i0 = table;
        i1 = result;
        i1 = LoadU32(ctx, i1 + 8u);
        key = i1;
        StoreU32(ctx, i0 + 56u, i1);
        i0 = state;
        i0 = LoadU32(ctx, i0 + 28u);
        i1 = state;
        i1 = LoadU32(ctx, i1 + 8u);
        l4 = i1;
        i0 -= i1;
        i1 = 64u;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      }
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = l9;
        i1 = 2u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = state;
          i1 = 4u;
          GrowValueStack(ctx, i0, i1);
          i0 = 2u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        if (i0) {
          i0 = l4;
        } else {
          i0 = state;
          i0 = LoadU32(ctx, i0 + 8u);
        }
        l4 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = state;
        i1 = l4;
        i2 = kF896ConstFFFFFFC0;
        i1 -= i2;
        StoreU32(ctx, i0 + 8u, i1);
      }
      i0 = l9;
      i1 = 3u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = l4;
        i2 = 0u;
        CallValue(ctx, i0, i1, i2);
        i0 = 3u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto running_exit;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      j0 = LoadU64(ctx, i0);
      l11 = j0;
      i0 = l8;
      i1 = l4;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l8;
      j1 = l11;
      StoreU64(ctx, i0, j1);
      i0 = l8;
      table = i0;
      i0 = l10;
      i1 = 1u;
      i0 += i1;
      l10 = i0;
      i1 = 100u;
      i0 = i0 != i1;
      l6 = i0;
      if (i0) {goto loop_5;}
    }
  i0 = l9;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = kF896MsgLoopInSettable;
    i2 = 0u;
    RaiseScriptError(ctx, i0, i1, i2);
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l8;
    i1 = 16u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
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
  i2 = kF896ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l5 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = table;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l5;
  i1 = key;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l5;
  i1 = result;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l5;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l5;
  i1 = l6;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l5;
  i1 = l7;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l5;
  i1 = l8;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l5;
  i1 = l10;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 36u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
