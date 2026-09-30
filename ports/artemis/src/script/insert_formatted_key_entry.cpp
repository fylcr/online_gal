// insert_formatted_key_entry.cpp -- inserts a formatted key entry: the key is formatted into the table slot's entry list..
//
// InsertFormattedKeyEntry -- inserts a formatted key entry: the key is formatted into the table slot's entry list.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 141417-141417 (f325); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void FormatNumberValue(EngineContext* ctx, uint32_t state, uint32_t value, uint32_t options);
uint32_t InternString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void GrowTableSlot(EngineContext* ctx, uint32_t a0);

namespace {

constexpr uint32_t kF325Const29D02 = 0x29D02u;  // 
constexpr uint32_t kF325SaveFrameBytes = 12u;
constexpr uint32_t kF325ResumeWordBytes = 4u;

}  // namespace

void InsertFormattedKeyEntry(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF325SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
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
    i2 = kF325ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
  }
  i0 = l2;
  i1 = b;
  i2 = 2u;
  i1 = (uint32_t)((int32_t)i1 >= (int32_t)i2);
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l2 = i0;
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
      l2 = i0;
      i0 = LoadU32(ctx, i0 + 68u);
      l3 = i0;
      i0 = l3;
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 64u);
      i0 = i0 >= i1;
      l2 = i0;
    }
    i0 = l2;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l4;
      i0 = !(i0);
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        GrowTableSlot(ctx, i0);
        i0 = 0u;
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
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = a;
      i1 = LoadU32(ctx, i1 + 12u);
      i0 -= i1;
      i1 = 4u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 1u;
      i0 -= i1;
      l2 = i0;
    }
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = b;
      i2 = l2;
      FormatNumberValue(ctx, i0, i1, i2);
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
      i1 = 8u;
      i0 += i1;
      l2 = i0;
      i0 = a;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = 1u;
      i2 = b;
      i1 -= i2;
      i2 = 4u;
      i1 <<= (i2 & 31);
      i0 += i1;
      goto running_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    if (i0) {goto block_3;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
  }
  i0 = l4;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = kF325Const29D02;
    i2 = 0u;
    i0 = InternString(ctx, i0, i1, i2);
    l3 = i0;
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  if (i0) {
    i0 = a;
  } else {
    i0 = b;
    i1 = 4u;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = b;
    i1 = l2;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 8u;
    i0 += i1;
    l2 = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = 16u;
    i0 += i1;
  }
  running_exit:;
  a = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i1 = a;
    StoreU32(ctx, i0, i1);
  }
  block_3:;
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
  i2 = kF325ResumeWordBytes;
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
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 12u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
