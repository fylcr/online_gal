// integer_to_string.cpp -- the signed 32-bit decimal formatter: the sign is split off, a minus is written for negatives and the destination string is filled with the digits of the absolute value..
//
// IntegerToString -- the signed 32-bit decimal formatter: the sign is split off, a minus is written for negatives and the destination string is filled with the digits of the absolute value.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 289439-289439 (f755); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);

namespace {

constexpr uint32_t kF755ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF755Const7FFFFFFF = 0x7FFFFFFFu;  // 
constexpr uint32_t kF755ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF755ResumeWordBytes = 4u;

}  // namespace

void IntegerToString(EngineContext* ctx, uint32_t dest, uint32_t value) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u; uint32_t i6 = 0u; uint32_t i7 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = 32u;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0);
    dest = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 24u);
    l7 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 28u);
    l8 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 4u);
    value = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF755ResumeWordBytes;
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
    i1 = 48u;
    i0 -= i1;
    l3 = i0;
    GuestStackSp(ctx) = i0;
    i0 = dest;
    j1 = 0ull;
    StoreU64(ctx, i0, j1);
    i0 = dest;
    i1 = 0u;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = l3;
    i2 = 23u;
    i1 += i2;
    l8 = i1;
    StoreU32(ctx, i0 + 28u, i1);
    i0 = l3;
    i1 = l3;
    i2 = 2u;
    i1 |= i2;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = value;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF755ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 48u;
    StoreU32(ctx, i0 + 44u, i1);
    i0 = l3;
    i1 = 48u;
    StoreU8(ctx, i0 + 40u, i1);
    i0 = l3;
    i1 = l8;
    StoreU32(ctx, i0 + 36u, i1);
    i0 = l5;
    i1 = l5;
    i2 = 31u;
    i1 = (uint32_t)((int32_t)i1 >> (i2 & 31));
    value = i1;
    i0 += i1;
    l6 = i0;
    i0 = l3;
    i1 = value;
    i2 = l6;
    i1 ^= i2;
    StoreU32(ctx, i0 + 32u, i1);
    i0 = l3;
    i1 = 32u;
    i0 += i1;
    value = i0;
  }
  i0 = l9;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1275u;
    i1 = value;
    i0 = HostFoldLayoutOffset(ctx, i0, i1);
    l2 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    value = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF755ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF755ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    if (i0) {goto block_6;}
    i0 = l5;
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    if (i0) {
      i0 = 0u;
    } else {
      i0 = value;
      i1 = 1u;
      i0 -= i1;
      value = i0;
      i1 = 45u;
      StoreU8(ctx, i0, i1);
      i0 = dest;
      i0 = LoadU8(ctx, i0 + 11u);
    }
    l6 = i0;
    i0 = l3;
    i1 = value;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l3;
    i1 = l8;
    StoreU32(ctx, i0 + 28u, i1);
    i0 = l8;
    i1 = value;
    i0 -= i1;
    l5 = i0;
    i0 = l6;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    i1 = kF755ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    l7 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    if (i0) {
      i0 = l5;
      i1 = dest;
      i1 = LoadU32(ctx, i1 + 8u);
      i2 = kF755Const7FFFFFFF;
      i1 &= i2;
      i2 = 1u;
      i1 -= i2;
      l4 = i1;
      i0 = i0 <= i1;
      if (i0) {goto block_10;}
      i0 = dest;
      i0 = LoadU32(ctx, i0 + 4u);
      l6 = i0;
      goto block_12;
    }
    i0 = 10u;
    l4 = i0;
    i0 = dest;
    i1 = l5;
    i2 = 10u;
    i1 = i1 <= i2;
    if (i1) {goto block_9;}
    block_12:;
    i0 = kF755ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = l4;
    i0 -= i1;
    l7 = i0;
  }
  i0 = l9;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1276u;
    i1 = dest;
    i2 = l4;
    i3 = l7;
    i4 = l6;
    i5 = 0u;
    i6 = l6;
    i7 = 0u;
    HostInvokeViiiiiii(ctx, i0, i1, i2, i3, i4, i5, i6, i7);
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF755ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF755ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_6;}
    i0 = dest;
    l7 = i0;
    i1 = dest;
    i1 = LoadS8(ctx, i1 + 11u);
    i2 = kF755ConstFFFFFFFF;
    i1 = (uint32_t)((int32_t)i1 > (int32_t)i2);
    if (i1) {goto block_9;}
  }
  block_10:;
  i0 = ResumePhase(ctx);
  if (i0) {
    i0 = l7;
  } else {
    i0 = dest;
    i0 = LoadU32(ctx, i0);
  }
  block_9:;
  l7 = i0;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    l4 = i0;
    i0 = value;
    i1 = l8;
    i0 = i0 != i1;
    if (i0) {
      loop_19: 
        i0 = l4;
        i1 = value;
        i1 = LoadU8(ctx, i1);
        StoreU8(ctx, i0, i1);
        i0 = l4;
        i1 = 1u;
        i0 += i1;
        l4 = i0;
        i0 = l8;
        i1 = value;
        i2 = 1u;
        i1 += i2;
        value = i1;
        i0 = i0 != i1;
        if (i0) {goto loop_19;}
    }
    i0 = l4;
    i1 = 0u;
    StoreU8(ctx, i0, i1);
    i0 = dest;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF755ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = dest;
      i1 = l5;
      StoreU32(ctx, i0 + 4u, i1);
      goto block_20;
    }
    i0 = dest;
    i1 = l5;
    StoreU8(ctx, i0 + 11u, i1);
    block_20:;
    i0 = l3;
    i1 = 48u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    value = i0;
    i0 = HostTempRet0(ctx);
    i0 = dest;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF755ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = dest;
      i0 = LoadU32(ctx, i0);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = value;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
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
  i2 = kF755ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l2 = i0;
  i1 = dest;
  StoreU32(ctx, i0, i1);
  i0 = l2;
  i1 = value;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l2;
  i1 = l3;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l2;
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l2;
  i1 = l5;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l2;
  i1 = l6;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l2;
  i1 = l7;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l2;
  i1 = l8;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 32u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
