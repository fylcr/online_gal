// append_text_bytes.cpp -- appends bytes to the text block, growing the buffer first and raising the length error when the text would overflow..
//
// AppendTextBytes -- appends bytes to the text block, growing the buffer first and raising the length error when the text would overflow.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 387582-387582 (f970); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
void RaiseStringLengthError(EngineContext* ctx, uint32_t a0);
void CopyIfNonEmpty(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

constexpr uint32_t kF970Const80000000 = 0x80000000u;  // 
constexpr uint32_t kF970ConstFFFFFFEF = 0xFFFFFFEFu;  // 
constexpr uint32_t kF970ConstFFFFFFF0 = 0xFFFFFFF0u;  // 
constexpr uint32_t kF970SaveFrameBytes = 28u;
constexpr uint32_t kF970ResumeWordBytes = 4u;

}  // namespace

void AppendTextBytes(EngineContext* ctx, uint32_t text, uint32_t bytes, uint32_t length) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF970SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    text = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 4u);
    bytes = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    length = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l6 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 20u);
    l7 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 24u);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF970ResumeWordBytes;
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
    i0 = GuestStackSp(ctx);
    i1 = 16u;
    i0 -= i1;
    l6 = i0;
    GuestStackSp(ctx) = i0;
    i0 = length;
    i1 = kF970ConstFFFFFFEF;
    i0 = i0 <= i1;
    l3 = i0;
  }
  i0 = l3;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l3;
    i1 = length;
    i2 = 10u;
    i1 = i1 <= i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l3 = i0;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      if (i0) {
        i0 = text;
        i1 = length;
        StoreU8(ctx, i0 + 11u, i1);
        i0 = text;
        l3 = i0;
        goto block_6;
      }
      i0 = text;
      l4 = i0;
      i0 = length;
      i1 = 11u;
      i0 = i0 >= i1;
      if (i0) {
        i0 = length;
        i1 = 16u;
        i0 += i1;
        i1 = kF970ConstFFFFFFF0;
        i0 &= i1;
        l3 = i0;
        i1 = l3;
        i2 = 1u;
        i1 -= i2;
        l3 = i1;
        i2 = l3;
        i3 = 11u;
        i2 = i2 == i3;
        i0 = i2 ? i0 : i1;
      } else {
        i0 = 10u;
      }
      l3 = i0;
      i1 = 1u;
      i0 += i1;
      l7 = i0;
    }
    i0 = l8;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l7;
      i0 = AllocateStringBuffer(ctx, i0);
      l5 = i0;
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = l3;
      StoreU32(ctx, i0, i1);
      i0 = text;
      i1 = l7;
      i2 = kF970Const80000000;
      i1 |= i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = text;
      i1 = length;
      StoreU32(ctx, i0 + 4u, i1);
    }
    block_6:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = bytes;
      i2 = length;
      CopyIfNonEmpty(ctx, i0, i1, i2);
      i0 = l6;
      i1 = 0u;
      StoreU8(ctx, i0 + 15u, i1);
      i0 = length;
      i1 = l3;
      i0 += i1;
      i1 = l6;
      i1 = LoadU8(ctx, i1 + 15u);
      StoreU8(ctx, i0, i1);
      i0 = l6;
      i1 = 16u;
      i0 += i1;
      GuestStackSp(ctx) = i0;
      goto func_exit;
    }
  }
  i0 = l8;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = text;
    RaiseStringLengthError(ctx, i0);
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
  i2 = kF970ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l5 = i0;
  i1 = text;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = bytes;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l5;
  i1 = length;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l5;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l5;
  i1 = l6;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l5;
  i1 = l7;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l5;
  i1 = l4;
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
