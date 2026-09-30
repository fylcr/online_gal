// rebucket_unordered_node_array.cpp -- re-buckets the unordered table's node array: the bucket chain is released and every node is linked again under the new bucket count..
//
// RebucketUnorderedNodeArray -- re-buckets the unordered table's node array: the bucket chain is released and every node is linked again under the new bucket count.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 623555-623555 (f1459); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
void UnlinkListNode(EngineContext* ctx, uint32_t node, uint32_t victim);

namespace {

constexpr uint32_t kF1459SaveFrameBytes = 36u;
constexpr uint32_t kF1459ResumeWordBytes = 4u;

}  // namespace

uint32_t RebucketUnorderedNodeArray(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l10 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u;
  uint64_t j1 = 0;
      l9 = 0, l10 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1459SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
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
    l9 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 32u);
    l10 = i0;
    i0 = l5;
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
    i2 = kF1459ResumeWordBytes;
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
    i1 = 24u;
    i0 += i1;
    l2 = i0;
    i0 = LoadU32(ctx, i0);
    l1 = i0;
    if (i0) {
      i0 = a;
      i1 = 24u;
      i0 += i1;
      l2 = i0;
      loop_7: 
        i0 = l1;
        i0 = LoadU32(ctx, i0 + 16u);
        l3 = i0;
        i1 = 14u;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {
          i0 = l1;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          if (i0) {goto block_8;}
          i0 = l1;
          l2 = i0;
          goto block_5;
        }
        i0 = l3;
        i1 = 14u;
        i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
        if (i0) {goto block_5;}
        i0 = l1;
        i1 = 4u;
        i0 += i1;
        l2 = i0;
        i0 = l1;
        i0 = LoadU32(ctx, i0 + 4u);
        l3 = i0;
        i0 = !(i0);
        if (i0) {goto block_5;}
        i0 = l2;
        l1 = i0;
        block_8:;
        i0 = l1;
        l2 = i0;
        i0 = l3;
        l1 = i0;
        goto loop_7;
      TrapUnreachable();
    }
    i0 = l2;
    l1 = i0;
    block_5:;
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = !(i0);
    l6 = i0;
  }
  i0 = l6;
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
      i0 = 24u;
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
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 20u, i1);
      i0 = l3;
      i1 = 14u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l3;
      i1 = l1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l2;
      i1 = l3;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      l6 = i0;
      i0 = a;
      i0 = LoadU32(ctx, i0 + 20u);
      i0 = LoadU32(ctx, i0);
      l1 = i0;
      i0 = !(i0);
      l4 = i0;
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i1 = l1;
        StoreU32(ctx, i0 + 20u, i1);
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        l6 = i0;
      }
      i0 = a;
      i0 = LoadU32(ctx, i0 + 24u);
      l2 = i0;
      i1 = l6;
      UnlinkListNode(ctx, i0, i1);
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 28u);
      i2 = 1u;
      i1 += i2;
      l1 = i1;
      StoreU32(ctx, i0 + 28u, i1);
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    l6 = i0;
    i0 = l3;
    i0 = LoadU8(ctx, i0 + 20u);
    i0 = !(i0);
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l1;
    if (i0) {goto block_15;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 4u);
    l1 = i0;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 20u);
    l2 = i0;
  }
  i0 = l8;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l1;
    i1 = 136u;
    i2 = l2;
    i0 = CallTableT3(ctx, i2, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    l5 = i0;
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l6 = i0;
  }
  block_15:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = 36u;
    i0 += i1;
    l2 = i0;
    i0 = LoadU32(ctx, i0);
    l1 = i0;
    if (i0) {
      i0 = a;
      i1 = 36u;
      i0 += i1;
      l2 = i0;
      loop_22: 
        i0 = l1;
        i0 = LoadU32(ctx, i0 + 16u);
        l3 = i0;
        i1 = 14u;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {
          i0 = l1;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          if (i0) {goto block_23;}
          i0 = l1;
          l2 = i0;
          goto block_20;
        }
        i0 = l3;
        i1 = 14u;
        i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
        if (i0) {goto block_20;}
        i0 = l1;
        i1 = 4u;
        i0 += i1;
        l2 = i0;
        i0 = l1;
        i0 = LoadU32(ctx, i0 + 4u);
        l3 = i0;
        i0 = !(i0);
        if (i0) {goto block_20;}
        i0 = l2;
        l1 = i0;
        block_23:;
        i0 = l1;
        l2 = i0;
        i0 = l3;
        l1 = i0;
        goto loop_22;
      TrapUnreachable();
    }
    i0 = l2;
    l1 = i0;
    block_20:;
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = !(i0);
    l4 = i0;
  }
  i0 = l4;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l8;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 24u;
      i0 = AllocateStringBuffer(ctx, i0);
      l5 = i0;
      i0 = 2u;
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
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 20u, i1);
      i0 = l3;
      i1 = 14u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l3;
      i1 = l1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l2;
      i1 = l3;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      l4 = i0;
      i0 = a;
      i0 = LoadU32(ctx, i0 + 32u);
      i0 = LoadU32(ctx, i0);
      l1 = i0;
      i0 = !(i0);
      l7 = i0;
      i0 = !(i0);
      if (i0) {
        i0 = a;
        i1 = l1;
        StoreU32(ctx, i0 + 32u, i1);
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
      }
      i0 = a;
      i0 = LoadU32(ctx, i0 + 36u);
      l2 = i0;
      i1 = l4;
      UnlinkListNode(ctx, i0, i1);
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 40u);
      i2 = 1u;
      i1 += i2;
      l1 = i1;
      StoreU32(ctx, i0 + 40u, i1);
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i0 = LoadU8(ctx, i0 + 20u);
    i0 = !(i0);
    l2 = i0;
    i0 = 0u;
    l1 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {goto block_30;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 4u);
    l1 = i0;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 20u);
    l2 = i0;
  }
  i0 = l8;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l1;
    i1 = 137u;
    i2 = l2;
    i0 = CallTableT3(ctx, i2, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    l5 = i0;
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l1 = i0;
  }
  block_30:;
  i0 = a;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  i1 = a;
  i2 = l9;
  i3 = l1;
  i4 = l1;
  i5 = l6;
  i4 |= i5;
  i5 = ResumePhase(ctx);
  i3 = i5 ? i3 : i4;
  l1 = i3;
  i4 = ResumePhase(ctx);
  i2 = i4 ? i2 : i3;
  l9 = i2;
  i3 = ResumePhase(ctx);
  i4 = 2u;
  i3 = i3 == i4;
  i2 |= i3;
  i0 = i2 ? i0 : i1;
  a = i0;
  i0 = l9;
  i0 = !(i0);
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = 12u;
      i0 += i1;
      l6 = i0;
      i0 = LoadU32(ctx, i0);
      l1 = i0;
      if (i0) {
        i0 = a;
        i1 = 12u;
        i0 += i1;
        l2 = i0;
        loop_38: 
          i0 = l1;
          i0 = LoadU32(ctx, i0 + 16u);
          l3 = i0;
          i1 = 14u;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          if (i0) {
            i0 = l1;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            if (i0) {goto block_39;}
            i0 = l1;
            l2 = i0;
            goto block_36;
          }
          i0 = l3;
          i1 = 14u;
          i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
          if (i0) {goto block_36;}
          i0 = l1;
          i1 = 4u;
          i0 += i1;
          l2 = i0;
          i0 = l1;
          i0 = LoadU32(ctx, i0 + 4u);
          l3 = i0;
          i0 = !(i0);
          if (i0) {goto block_36;}
          i0 = l2;
          l1 = i0;
          block_39:;
          i0 = l1;
          l2 = i0;
          i0 = l3;
          l1 = i0;
          goto loop_38;
        TrapUnreachable();
      }
      i0 = l6;
      l1 = i0;
      l2 = i0;
      block_36:;
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = !(i0);
      l4 = i0;
    }
    i0 = l4;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l8;
      i1 = 4u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 32u;
        i0 = AllocateStringBuffer(ctx, i0);
        l5 = i0;
        i0 = 4u;
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
        i0 = l3;
        j1 = 0ull;
        StoreU64(ctx, i0 + 24u, j1);
        i0 = l3;
        i1 = 14u;
        StoreU32(ctx, i0 + 16u, i1);
        i0 = l3;
        i1 = l1;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l3;
        j1 = 0ull;
        StoreU64(ctx, i0, j1);
        i0 = l3;
        i1 = l3;
        i2 = 24u;
        i1 += i2;
        StoreU32(ctx, i0 + 20u, i1);
        i0 = l2;
        i1 = l3;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        l4 = i0;
        i0 = a;
        i0 = LoadU32(ctx, i0 + 8u);
        i0 = LoadU32(ctx, i0);
        l1 = i0;
        i0 = !(i0);
        l7 = i0;
        i0 = !(i0);
        if (i0) {
          i0 = a;
          i1 = l1;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l2;
          i0 = LoadU32(ctx, i0);
          l4 = i0;
        }
        i0 = a;
        i0 = LoadU32(ctx, i0 + 12u);
        l2 = i0;
        i1 = l4;
        UnlinkListNode(ctx, i0, i1);
        i0 = a;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 16u);
        i2 = 1u;
        i1 += i2;
        l1 = i1;
        StoreU32(ctx, i0 + 16u, i1);
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = 12u;
      i0 += i1;
      l10 = i0;
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 20u);
      l3 = i0;
    }
    loop_46: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l10;
        l2 = i0;
        i0 = l6;
        i0 = LoadU32(ctx, i0);
        l1 = i0;
        i0 = !(i0);
        if (i0) {
          i0 = l6;
          l1 = i0;
          l2 = i0;
          goto block_49;
        }
        loop_51: 
          i0 = l1;
          i0 = LoadU32(ctx, i0 + 16u);
          l4 = i0;
          i1 = 14u;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          if (i0) {
            i0 = l1;
            i0 = LoadU32(ctx, i0);
            l4 = i0;
            if (i0) {goto block_52;}
            i0 = l1;
            l2 = i0;
            goto block_49;
          }
          i0 = l4;
          i1 = 14u;
          i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
          if (i0) {goto block_49;}
          i0 = l1;
          i1 = 4u;
          i0 += i1;
          l2 = i0;
          i0 = l1;
          i0 = LoadU32(ctx, i0 + 4u);
          l4 = i0;
          i0 = !(i0);
          if (i0) {goto block_49;}
          i0 = l2;
          l1 = i0;
          block_52:;
          i0 = l1;
          l2 = i0;
          i0 = l4;
          l1 = i0;
          goto loop_51;
        TrapUnreachable();
        block_49:;
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i0 = !(i0);
        l7 = i0;
      }
      i0 = l7;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = l8;
        i1 = 5u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 32u;
          i0 = AllocateStringBuffer(ctx, i0);
          l5 = i0;
          i0 = 5u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l5;
          l4 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          j1 = 0ull;
          StoreU64(ctx, i0 + 24u, j1);
          i0 = l4;
          i1 = 14u;
          StoreU32(ctx, i0 + 16u, i1);
          i0 = l4;
          i1 = l1;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l4;
          j1 = 0ull;
          StoreU64(ctx, i0, j1);
          i0 = l4;
          i1 = l4;
          i2 = 24u;
          i1 += i2;
          StoreU32(ctx, i0 + 20u, i1);
          i0 = l2;
          i1 = l4;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          l7 = i0;
          i0 = a;
          i0 = LoadU32(ctx, i0 + 8u);
          i0 = LoadU32(ctx, i0);
          l1 = i0;
          if (i0) {
            i0 = a;
            i1 = l1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = l2;
            i0 = LoadU32(ctx, i0);
            l7 = i0;
          }
          i0 = a;
          i0 = LoadU32(ctx, i0 + 12u);
          l2 = i0;
          i1 = l7;
          UnlinkListNode(ctx, i0, i1);
          i0 = a;
          i1 = a;
          i1 = LoadU32(ctx, i1 + 16u);
          i2 = 1u;
          i1 += i2;
          l1 = i1;
          StoreU32(ctx, i0 + 16u, i1);
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        i1 = l4;
        i2 = 24u;
        i1 += i2;
        l1 = i1;
        i0 = i0 == i1;
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        if (i0) {goto block_59;}
        i0 = a;
        i0 = LoadU32(ctx, i0 + 4u);
        l2 = i0;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 20u);
        l7 = i0;
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 16u);
        l4 = i0;
      }
      i0 = l8;
      i1 = 6u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l4;
        i2 = l7;
        i0 = CallTableT3(ctx, i2, i0, i1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        l5 = i0;
        i0 = 6u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l5;
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        if (i0) {goto block_59;}
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 4u);
        l1 = i0;
        i0 = !(i0);
        l2 = i0;
        if (i0) {
          i0 = l3;
          i1 = 8u;
          i0 += i1;
          l2 = i0;
          i0 = l3;
          l1 = i0;
          i0 = LoadU32(ctx, i0 + 8u);
          l3 = i0;
          i0 = LoadU32(ctx, i0);
          l4 = i0;
          i0 = l1;
          i1 = l4;
          i0 = i0 == i1;
          l1 = i0;
          if (i0) {goto loop_46;}
          loop_64: 
            i0 = l2;
            i0 = LoadU32(ctx, i0);
            l1 = i0;
            i1 = 8u;
            i0 += i1;
            l2 = i0;
            i0 = l1;
            i0 = LoadU32(ctx, i0 + 8u);
            l3 = i0;
            i0 = LoadU32(ctx, i0);
            l4 = i0;
            i1 = l1;
            i0 = i0 != i1;
            l1 = i0;
            if (i0) {goto loop_64;}
          goto loop_46;
        }
        loop_65: 
          i0 = l1;
          l3 = i0;
          i0 = l1;
          i0 = LoadU32(ctx, i0);
          l1 = i0;
          if (i0) {goto loop_65;}
        goto loop_46;
      }
      block_59:;
    i0 = a;
    i1 = l1;
    i2 = l3;
    i1 = i1 != i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    a = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    goto func_exit;
  }
  TrapUnreachable();
  spill_frame:;
  l5 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l5;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1459ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l5 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = l1;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l5;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l5;
  i1 = l3;
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
  i1 = l9;
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
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
