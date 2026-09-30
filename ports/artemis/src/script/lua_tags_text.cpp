// lua_tags_text.cpp -- the print text queue step: one suspendable step that appends a text run to the message layer print queue..
//
// MessageLayer::QueuePrintText -- the print text queue step: one suspendable step that appends a text run to the message layer print queue.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 1461573-1461573 (f3219); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
uint32_t CopyBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

constexpr uint32_t kF3219MsgPrint = 0xB94DDu;  // print
constexpr uint32_t kF3219ConstB94E1 = 0xB94E1u;  // 
constexpr uint32_t kF3219MsgAllocatorTAllocateSize = 0xB962Fu;  // allocator<T>::allocate(size_t n) 'n' exceeds maximum supported size
constexpr uint32_t kF3219ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF3219Const1FFFFFFF = 0x1FFFFFFFu;  // 
constexpr uint32_t kF3219Const3FFFFFFF = 0x3FFFFFFFu;  // 
constexpr uint32_t kF3219Const40000000 = 0x40000000u;  // 
constexpr uint32_t kF3219Const61746164 = 0x61746164u;  // 
constexpr uint32_t kF3219ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF3219SaveFrameBytes = 48u;
constexpr uint32_t kF3219ResumeWordBytes = 4u;

}  // namespace

namespace MessageLayer {
void QueuePrintText(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l13 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u;
      l10 = 0, l11 = 0, l12 = 0, l13 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF3219SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
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
    l8 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 32u);
    l9 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 36u);
    l10 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 40u);
    l12 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 44u);
    l13 = i0;
    i0 = l4;
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
    i2 = kF3219ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l11 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    i1 = 48u;
    i0 -= i1;
    l5 = i0;
    GuestStackSp(ctx) = i0;
    i0 = b;
    i0 = LoadU32(ctx, i0 + 4u);
    l7 = i0;
    i0 = b;
    i0 = LoadU8(ctx, i0 + 11u);
    l8 = i0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    l13 = i0;
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    if (i0) {
      i0 = l8;
      l2 = i0;
      i0 = l13;
      i0 = !(i0);
      l9 = i0;
      if (i0) {goto running_exit;}
      loop_6: 
        i0 = b;
        i1 = l3;
        i0 += i1;
        i0 = LoadU8(ctx, i0);
        i1 = 32u;
        i0 = i0 != i1;
        l9 = i0;
        if (i0) {
          i0 = l3;
          l2 = i0;
          goto running_exit;
        }
        i0 = l8;
        i1 = l3;
        i2 = 1u;
        i1 += i2;
        l3 = i1;
        i0 = i0 != i1;
        l9 = i0;
        if (i0) {goto loop_6;}
      goto running_exit;
    }
    i0 = l7;
    l2 = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto running_exit;}
    i0 = b;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = 0u;
    l2 = i0;
    dispatch_loop: 
      i0 = l2;
      i1 = l3;
      i0 += i1;
      i0 = LoadU8(ctx, i0);
      i1 = 32u;
      i0 = i0 != i1;
      l9 = i0;
      if (i0) {goto running_exit;}
      i0 = l7;
      i1 = l2;
      i2 = 1u;
      i1 += i2;
      l2 = i1;
      i0 = i0 != i1;
      l9 = i0;
      if (i0) {goto dispatch_loop;}
    i0 = l7;
    l2 = i0;
    running_exit:;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 196u);
      l9 = i0;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 8u);
      l12 = i1;
      i0 = i0 >= i1;
      l3 = i0;
      if (i0) {goto block_13;}
      i0 = a;
      i0 = LoadU8(ctx, i0 + 89u);
      l10 = i0;
      i0 = l9;
      l3 = i0;
      loop_16: 
        i0 = l10;
        i1 = l3;
        i1 = LoadU8(ctx, i1);
        l6 = i1;
        i0 = i0 == i1;
        if (i0) {
          i0 = l3;
          i0 = LoadU8(ctx, i0 + 1u);
          i1 = 32u;
          i0 = i0 != i1;
          l6 = i0;
          if (i0) {goto block_17;}
          i0 = l3;
          i1 = 1u;
          i0 += i1;
          i1 = l9;
          i2 = kF3219ConstFFFFFFFF;
          i1 ^= i2;
          l3 = i1;
          i0 += i1;
          l2 = i0;
          goto block_17;
        }
        i0 = l6;
        i1 = 32u;
        i0 = i0 != i1;
        l6 = i0;
        if (i0) {goto block_17;}
        i0 = l12;
        i1 = l3;
        i2 = 1u;
        i1 += i2;
        l3 = i1;
        i0 = i0 > i1;
        l6 = i0;
        if (i0) {goto loop_16;}
        block_17:;
      i0 = l2;
      if (i0) {goto block_13;}
    }
  }
  i0 = l11;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 44u;
    i0 = AllocateStringBuffer(ctx, i0);
    l4 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    l10 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = 5u;
    StoreU8(ctx, i0 + 43u, i1);
    i0 = l5;
    i1 = kF3219MsgPrint;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 32u, i1);
    i0 = l5;
    i1 = kF3219ConstB94E1;
    i1 = LoadU8(ctx, i1);
    StoreU8(ctx, i0 + 36u, i1);
    i0 = l5;
    i1 = 0u;
    StoreU8(ctx, i0 + 37u, i1);
    i0 = l5;
    i1 = 0u;
    StoreU8(ctx, i0 + 20u, i1);
    i0 = l5;
    i1 = kF3219Const61746164;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = l5;
    i1 = 4u;
    StoreU8(ctx, i0 + 27u, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
    l2 = i0;
    i0 = kF3219ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 16u;
    i0 += i1;
    l7 = i0;
    i0 = l5;
    i1 = 32u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l11;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 3195u;
    i1 = l10;
    i2 = l3;
    i3 = l7;
    i4 = b;
    i5 = l2;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l4 = i0;
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3219ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3219ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = 1u;
    l6 = i0;
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_11;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 96u);
    b = i0;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 100u);
    l2 = i1;
    i0 = i0 < i1;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    if (i0) {
      i0 = b;
      i1 = l10;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = b;
      i2 = 4u;
      i1 += i2;
      b = i1;
      StoreU32(ctx, i0 + 96u, i1);
      goto block_23;
    }
    i0 = b;
    i1 = a;
    i2 = 92u;
    i1 += i2;
    b = i1;
    i1 = LoadU32(ctx, i1);
    l8 = i1;
    i0 -= i1;
    l9 = i0;
    i1 = 2u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    l7 = i0;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    i1 = kF3219Const40000000;
    i0 = i0 >= i1;
    l6 = i0;
  }
  i0 = l6;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3219ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = 0u;
      l6 = i0;
    }
    i0 = l11;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1053u;
      i1 = b;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3219ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3219ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_11;}
      goto block_9;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = l2;
    i2 = l8;
    i1 -= i2;
    l2 = i1;
    i2 = 1u;
    i1 = (uint32_t)((int32_t)i1 >> (i2 & 31));
    b = i1;
    i0 = i0 > i1;
    l6 = i0;
    i0 = l3;
    i1 = b;
    i2 = l6;
    i0 = i2 ? i0 : i1;
    i1 = kF3219Const3FFFFFFF;
    i2 = l2;
    i3 = 2u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = kF3219Const1FFFFFFF;
    i2 = i2 < i3;
    l2 = i2;
    i0 = i2 ? i0 : i1;
    l3 = i0;
    i0 = !(i0);
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    if (i0) {
      i0 = 0u;
      l2 = i0;
      goto block_32;
    }
    i0 = l3;
    i1 = kF3219Const40000000;
    i0 = i0 >= i1;
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
      i0 = kF3219ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = 0u;
      l6 = i0;
    }
    i0 = l11;
    i1 = 3u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1033u;
      i1 = kF3219MsgAllocatorTAllocateSize;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 3u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3219ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3219ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      if (i0) {goto block_9;}
      goto block_11;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    l6 = i0;
    i0 = kF3219ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 2u;
    i0 <<= (i1 & 31);
    b = i0;
  }
  i0 = l11;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1032u;
    i1 = b;
    i0 = HostFoldLayoutOffset(ctx, i0, i1);
    l4 = i0;
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3219ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3219ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    b = i0;
    if (i0) {goto block_11;}
  }
  block_32:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i1 = l7;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i0 += i1;
    b = i0;
    i1 = l10;
    StoreU32(ctx, i0, i1);
    i0 = l9;
    i1 = 1u;
    i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
    l7 = i0;
    if (i0) {
      i0 = l2;
      i1 = l8;
      i2 = l9;
      i0 = CopyBytes(ctx, i0, i1, i2);
      l7 = i0;
    }
    i0 = a;
    i1 = l3;
    i2 = 2u;
    i1 <<= (i2 & 31);
    i2 = l2;
    i1 += i2;
    l3 = i1;
    StoreU32(ctx, i0 + 100u, i1);
    i0 = a;
    i1 = b;
    i2 = 4u;
    i1 += i2;
    b = i1;
    StoreU32(ctx, i0 + 96u, i1);
    i0 = a;
    i1 = l2;
    StoreU32(ctx, i0 + 92u, i1);
    i0 = l8;
    i0 = !(i0);
    a = i0;
    if (i0) {goto block_23;}
    i0 = l8;
    DeallocateRawBlock(ctx, i0);
  }
  block_23:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i0 = LoadS8(ctx, i0 + 27u);
    i1 = kF3219ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l5;
      i0 = LoadU32(ctx, i0 + 16u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l5;
    i0 = LoadS8(ctx, i0 + 43u);
    i1 = kF3219ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_12;}
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 32u);
    DeallocateRawBlock(ctx, i0);
    goto block_12;
  }
  block_13:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    i1 = l8;
    i2 = l13;
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    i0 = i2 ? i0 : i1;
    i1 = l2;
    i0 = i0 <= i1;
    l3 = i0;
    if (i0) {goto block_12;}
  }
  i0 = l11;
  i1 = 5u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 44u;
    i0 = AllocateStringBuffer(ctx, i0);
    l4 = i0;
    i0 = 5u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    l10 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = 5u;
    StoreU8(ctx, i0 + 43u, i1);
    i0 = l5;
    i1 = kF3219MsgPrint;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 32u, i1);
    i0 = l5;
    i1 = kF3219ConstB94E1;
    i1 = LoadU8(ctx, i1);
    l3 = i1;
    StoreU8(ctx, i0 + 36u, i1);
    i0 = l5;
    i1 = 0u;
    StoreU8(ctx, i0 + 37u, i1);
    i0 = l5;
    i1 = 0u;
    StoreU8(ctx, i0 + 20u, i1);
    i0 = l5;
    i1 = kF3219Const61746164;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = l5;
    i1 = 4u;
    StoreU8(ctx, i0 + 27u, i1);
    i0 = kF3219ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l11;
  i1 = 6u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 854u;
    i1 = l5;
    i2 = b;
    i3 = l2;
    i4 = kF3219ConstFFFFFFFF;
    i5 = b;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l4 = i0;
    i0 = 6u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    l12 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3219ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3219ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = 1u;
    l6 = i0;
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
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
      i0 = LoadU32(ctx, i0 + 16u);
      b = i0;
      i0 = kF3219ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 16u;
      i0 += i1;
      l3 = i0;
      i0 = l5;
      i1 = 32u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l11;
    i1 = 7u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3195u;
      i1 = l10;
      i2 = l2;
      i3 = l3;
      i4 = l12;
      i5 = b;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l4 = i0;
      i0 = 7u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3219ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3219ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_52;}
      i0 = a;
      i0 = LoadU32(ctx, i0 + 96u);
      b = i0;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 100u);
      l2 = i1;
      i0 = i0 < i1;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      if (i0) {
        i0 = b;
        i1 = l10;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = b;
        i2 = 4u;
        i1 += i2;
        StoreU32(ctx, i0 + 96u, i1);
        goto block_59;
      }
      i0 = b;
      i1 = a;
      i2 = 92u;
      i1 += i2;
      b = i1;
      i1 = LoadU32(ctx, i1);
      l8 = i1;
      i0 -= i1;
      l9 = i0;
      i1 = 2u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l7 = i0;
      i1 = 1u;
      i0 += i1;
      l3 = i0;
      i1 = kF3219Const40000000;
      i0 = i0 >= i1;
      l6 = i0;
    }
    i0 = l6;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3219ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = 0u;
        l6 = i0;
      }
      i0 = l11;
      i1 = 8u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1053u;
        i1 = b;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 8u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3219ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3219ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        if (i0) {goto block_9;}
        goto block_52;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = l2;
      i2 = l8;
      i1 -= i2;
      l2 = i1;
      i2 = 1u;
      i1 = (uint32_t)((int32_t)i1 >> (i2 & 31));
      b = i1;
      i0 = i0 > i1;
      l6 = i0;
      i0 = l3;
      i1 = b;
      i2 = l6;
      i0 = i2 ? i0 : i1;
      i1 = kF3219Const3FFFFFFF;
      i2 = l2;
      i3 = 2u;
      i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
      i3 = kF3219Const1FFFFFFF;
      i2 = i2 < i3;
      l2 = i2;
      i0 = i2 ? i0 : i1;
      l3 = i0;
      i0 = !(i0);
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      if (i0) {
        i0 = 0u;
        l2 = i0;
        goto block_68;
      }
      i0 = l3;
      i1 = kF3219Const40000000;
      i0 = i0 >= i1;
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
        i0 = kF3219ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = 0u;
        l6 = i0;
      }
      i0 = l11;
      i1 = 9u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1033u;
        i1 = kF3219MsgAllocatorTAllocateSize;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 9u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3219ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3219ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        if (i0) {goto block_9;}
        goto block_52;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = 0u;
      l6 = i0;
      i0 = kF3219ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 2u;
      i0 <<= (i1 & 31);
      b = i0;
    }
    i0 = l11;
    i1 = 10u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = b;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 10u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3219ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3219ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_52;}
    }
    block_68:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i1 = l7;
      i2 = 2u;
      i1 <<= (i2 & 31);
      i0 += i1;
      b = i0;
      i1 = l10;
      StoreU32(ctx, i0, i1);
      i0 = l9;
      i1 = 1u;
      i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
      if (i0) {
        i0 = l2;
        i1 = l8;
        i2 = l9;
        i0 = CopyBytes(ctx, i0, i1, i2);
      }
      i0 = a;
      i1 = l3;
      i2 = 2u;
      i1 <<= (i2 & 31);
      i2 = l2;
      i1 += i2;
      StoreU32(ctx, i0 + 100u, i1);
      i0 = a;
      i1 = b;
      i2 = 4u;
      i1 += i2;
      StoreU32(ctx, i0 + 96u, i1);
      i0 = a;
      i1 = l2;
      StoreU32(ctx, i0 + 92u, i1);
      i0 = l8;
      i0 = !(i0);
      if (i0) {goto block_59;}
      i0 = l8;
      DeallocateRawBlock(ctx, i0);
    }
    block_59:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l12;
      i0 = LoadS8(ctx, i0 + 11u);
      i1 = kF3219ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l12;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l5;
      i0 = LoadS8(ctx, i0 + 27u);
      i1 = kF3219ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l5;
        i0 = LoadU32(ctx, i0 + 16u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l5;
      i0 = LoadS8(ctx, i0 + 43u);
      i1 = kF3219ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      if (i0) {goto block_12;}
      i0 = l5;
      i0 = LoadU32(ctx, i0 + 32u);
      DeallocateRawBlock(ctx, i0);
      goto block_12;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    goto block_51;
  }
  block_52:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l12;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF3219ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    if (i0) {goto block_51;}
    i0 = l12;
    i0 = LoadU32(ctx, i0);
    DeallocateRawBlock(ctx, i0);
  }
  block_51:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i0 = LoadS8(ctx, i0 + 27u);
    i1 = kF3219ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l5;
      i0 = LoadU32(ctx, i0 + 16u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l5;
    i0 = LoadS8(ctx, i0 + 43u);
    i1 = kF3219ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l5;
      i0 = LoadU32(ctx, i0 + 32u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l6;
    i0 = !(i0);
    if (i0) {goto block_10;}
    i0 = l10;
    DeallocateRawBlock(ctx, i0);
    goto block_10;
  }
  block_12:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = 48u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_11:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l5;
    i0 = LoadS8(ctx, i0 + 27u);
    i1 = kF3219ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l5;
      i0 = LoadU32(ctx, i0 + 16u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l5;
    i0 = LoadS8(ctx, i0 + 43u);
    i1 = kF3219ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l5;
      i0 = LoadU32(ctx, i0 + 32u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l6;
    i0 = !(i0);
    if (i0) {goto block_10;}
    i0 = l10;
    DeallocateRawBlock(ctx, i0);
    i0 = l2;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_10:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_9:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    TrapUnreachable();
  }
  goto func_exit;
  spill_frame:;
  l4 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l4;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF3219ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = b;
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
  i1 = l8;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l4;
  i1 = l9;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = l4;
  i1 = l10;
  StoreU32(ctx, i0 + 36u, i1);
  i0 = l4;
  i1 = l12;
  StoreU32(ctx, i0 + 40u, i1);
  i0 = l4;
  i1 = l13;
  StoreU32(ctx, i0 + 44u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 48u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
}
