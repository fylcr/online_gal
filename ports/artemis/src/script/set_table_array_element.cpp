// set_table_array_element.cpp -- the array-part write: the key is pushed, the field is set through the shared writer, and a number value is boxed into the value record on the way out..
//
// SetTableArrayElement -- the array-part write: the key is pushed, the field is set through the shared writer, and a number value is boxed into the value record on the way out.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 2757260-2757432 (f5726); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void PushTableField(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void SetTableField(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

constexpr uint32_t kF5726SaveFrameBytes = 12u;
constexpr uint32_t kF5726ResumeWordBytes = 4u;

}  // namespace

void SetTableArrayElement(EngineContext* ctx, uint32_t state, uint32_t table, uint32_t index) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  double d1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF5726SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    index = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = index;
    i0 = LoadU32(ctx, i0 + 4u);
    table = i0;
    i0 = index;
    i0 = LoadU32(ctx, i0 + 8u);
    index = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF5726ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  i1 = l4;
  i2 = index;
  i3 = 0u;
  i2 = (uint32_t)((int32_t)i2 >= (int32_t)i3);
  i3 = ResumePhase(ctx);
  i1 = i3 ? i1 : i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i1 = table;
      i2 = 9999u;
      i1 += i2;
      i2 = 9999u;
      i1 = i1 <= i2;
      if (i1) {
        i1 = state;
        i1 = LoadU32(ctx, i1 + 8u);
        i2 = state;
        i2 = LoadU32(ctx, i2 + 12u);
        i1 -= i2;
        i2 = 4u;
        i1 = (uint32_t)((int32_t)i1 >> (i2 & 31));
        i2 = table;
        i1 += i2;
        i2 = 1u;
        i1 += i2;
        table = i1;
      }
      i1 = table;
      i2 = 0u;
      PushTableField(ctx, i0, i1, i2);
    }
    i0 = l3;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = table;
      i2 = index;
      SetTableField(ctx, i0, i1, i2);
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 8u);
      l4 = i0;
      i1 = 3u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l4;
      i1 = index;
      d1 = (double)(int32_t)(i1);
      StoreF64(ctx, i0, d1);
      i0 = state;
      i1 = state;
      i1 = LoadU32(ctx, i1 + 8u);
      i2 = 16u;
      i1 += i2;
      StoreU32(ctx, i0 + 8u, i1);
    }
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = table;
      i2 = 0u;
      SetTableField(ctx, i0, i1, i2);
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
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
  i2 = kF5726ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = table;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = index;
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
