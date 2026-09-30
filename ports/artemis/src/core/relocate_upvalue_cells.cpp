// relocate_upvalue_cells.cpp -- relocates upvalue cells: every closed cell is re-pointed after the value stack moved..
//
// RelocateUpvalueCells -- relocates upvalue cells: every closed cell is re-pointed after the value stack moved.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 489910-489910 (f1129); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void RelocateElementArray(EngineContext* ctx, uint32_t state, uint32_t table);

namespace {

constexpr uint32_t kF1129SaveFrameBytes = 28u;
constexpr uint32_t kF1129ResumeWordBytes = 4u;

}  // namespace

void RelocateUpvalueCells(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1129SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    b = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    l4 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l5 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 16u);
    l6 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 20u);
    l8 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 24u);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1129ResumeWordBytes;
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
    i1 = 112u;
    i0 -= i1;
    l4 = i0;
    GuestStackSp(ctx) = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 68u);
    l8 = i0;
    i0 = !(i0);
    l7 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    if (i0) {goto running_exit;}
    i0 = a;
    i0 = LoadU8(ctx, i0 + 57u);
    i0 = !(i0);
    if (i0) {goto running_exit;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 20u);
    l7 = i0;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 32u);
    l5 = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 8u);
    l6 = i0;
    i0 = l4;
    i1 = c;
    StoreU32(ctx, i0 + 28u, i1);
    i0 = l4;
    i1 = b;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l4;
    i1 = b;
    i2 = 4u;
    i1 = i1 != i2;
    if (i1) {
      i1 = l7;
      i2 = a;
      i2 = LoadU32(ctx, i2 + 40u);
      i1 -= i2;
      i2 = 24u;
      i1 = I32_DIV_S(i1, i2);
    } else {
      i1 = 0u;
    }
    StoreU32(ctx, i0 + 104u, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 28u);
    i1 = l6;
    i0 -= i1;
    i1 = 320u;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
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
      i0 = a;
      i0 = LoadU32(ctx, i0 + 44u);
      b = i0;
      i1 = 20u;
      i0 += i1;
      i1 = b;
      i2 = 1u;
      i1 <<= (i2 & 31);
      i2 = b;
      i3 = 20u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      i0 = i2 ? i0 : i1;
      b = i0;
    }
    i0 = l9;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = b;
      RelocateElementArray(ctx, i0, i1);
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
    i0 = LoadU32(ctx, i0 + 20u);
    i1 = a;
    i1 = LoadU32(ctx, i1 + 8u);
    i2 = 320u;
    i1 += i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = a;
    i1 = 0u;
    StoreU8(ctx, i0 + 57u, i1);
    i0 = l4;
    i1 = 8u;
    i0 += i1;
    b = i0;
  }
  i0 = l9;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    i1 = b;
    i2 = l8;
    CallTableT0(ctx, i2, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
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
    i1 = 1u;
    StoreU8(ctx, i0 + 57u, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 20u);
    i1 = a;
    i1 = LoadU32(ctx, i1 + 32u);
    b = i1;
    i2 = l3;
    i3 = l5;
    i2 -= i3;
    i1 += i2;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = a;
    i1 = l6;
    i2 = l5;
    i1 -= i2;
    i2 = b;
    i1 += i2;
    StoreU32(ctx, i0 + 8u, i1);
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 112u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
  }
  goto func_exit;
  spill_frame:;
  c = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = c;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1129ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  c = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = c;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = c;
  i1 = l4;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = c;
  i1 = l5;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = c;
  i1 = l6;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = c;
  i1 = l8;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = c;
  i1 = l3;
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
