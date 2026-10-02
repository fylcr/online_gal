// resize_table_array.cpp -- resizes a table array: the array part is reallocated and the entries are moved..
//
// ResizeTableArray -- resizes a table array: the array part is reallocated and the entries are moved.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 481175-481175 (f1114); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t FillBytes(EngineContext* ctx, uint32_t dst, uint32_t value, uint32_t length);
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size);
uint32_t RaiseBlockTooBig(EngineContext* ctx, uint32_t a);

namespace {

constexpr uint32_t kF1114Const3FFFFFFF = 0x3FFFFFFFu;  // 
constexpr uint32_t kF1114SaveFrameBytes = 28u;
constexpr uint32_t kF1114ResumeWordBytes = 4u;

}  // namespace

void ResizeTableArray(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1114SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 12u);
    l5 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 24u);
    l7 = i0;
    i0 = l2;
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
    i2 = kF1114ResumeWordBytes;
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
    i0 = LoadU32(ctx, i0 + 16u);
    i0 = LoadU8(ctx, i0 + 21u);
    i1 = 2u;
    i0 = i0 != i1;
    l3 = i0;
  }
  i0 = l3;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l3;
    i1 = b;
    i2 = 1u;
    i1 += i2;
    i2 = kF1114Const3FFFFFFF;
    i1 = i1 <= i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l3 = i0;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l3;
      i1 = b;
      i2 = 2u;
      i1 <<= (i2 & 31);
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      l3 = i0;
      i0 = l8;
      i0 = !(i0);
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = 0u;
        i2 = 0u;
        i3 = l3;
        i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
        l2 = i0;
        i0 = 0u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l2;
        l3 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto block_6;}
    }
    i0 = l8;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i0 = RaiseBlockTooBig(ctx, i0);
      l2 = i0;
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l2;
      l3 = i0;
    }
    block_6:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 16u);
      l6 = i0;
      i0 = b;
      i1 = 1u;
      i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i1 = 0u;
        i2 = b;
        i3 = 2u;
        i2 <<= (i3 & 31);
        i0 = FillBytes(ctx, i0, i1, i2);
      }
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 8u);
      l4 = i0;
      i1 = 1u;
      i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
      if (i0) {
        i0 = b;
        i1 = 1u;
        i0 -= i1;
        l9 = i0;
        loop_13: 
          i0 = l6;
          i0 = LoadU32(ctx, i0);
          i1 = l7;
          i2 = 2u;
          i1 <<= (i2 & 31);
          i0 += i1;
          i0 = LoadU32(ctx, i0);
          l5 = i0;
          if (i0) {
            loop_15: 
              i0 = l5;
              i0 = LoadU32(ctx, i0);
              l4 = i0;
              i0 = l5;
              i1 = l9;
              i2 = l5;
              i2 = LoadU32(ctx, i2 + 8u);
              i1 &= i2;
              i2 = 2u;
              i1 <<= (i2 & 31);
              i2 = l3;
              i1 += i2;
              l2 = i1;
              i1 = LoadU32(ctx, i1);
              StoreU32(ctx, i0, i1);
              i0 = l2;
              i1 = l5;
              StoreU32(ctx, i0, i1);
              i0 = l4;
              l5 = i0;
              if (i0) {goto loop_15;}
            i0 = l6;
            i0 = LoadU32(ctx, i0 + 8u);
            l4 = i0;
          }
          i0 = l7;
          i1 = 1u;
          i0 += i1;
          l7 = i0;
          i1 = l4;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          if (i0) {goto loop_13;}
      }
      i0 = l6;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = l4;
      i1 = 2u;
      i0 <<= (i1 & 31);
      l4 = i0;
    }
    i0 = l8;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = a;
      i1 = l5;
      i2 = l4;
      i3 = 0u;
      i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = l3;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = b;
      StoreU32(ctx, i0 + 8u, i1);
    }
  }
  goto func_exit;
  spill_frame:;
  l2 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1114ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l2 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l2;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l2;
  i1 = l3;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l2;
  i1 = l5;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l2;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l2;
  i1 = l6;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l2;
  i1 = l7;
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
