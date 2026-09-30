// insert_tagged_tree_node.cpp -- allocates one forty-byte tagged-tree node, constructs its key and value through the host copy slot, descends by the tag byte and rebalances the tree..
//
// InsertTaggedTreeNode -- allocates one forty-byte tagged-tree node, constructs its key and value through the host copy slot, descends by the tag byte and rebalances the tree.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 2146042-2146042 (f3799); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
void UnlinkListNode(EngineContext* ctx, uint32_t node, uint32_t victim);

namespace {

constexpr uint32_t kF3799ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF3799ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF3799SaveFrameBytes = 20u;
constexpr uint32_t kF3799ResumeWordBytes = 4u;

}  // namespace

void InsertTaggedTreeNode(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF3799SaveFrameBytes;
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
    l4 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
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
    i2 = kF3799ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l6 = i0;
  }
  i0 = l6;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 40u;
    i0 = AllocateStringBuffer(ctx, i0);
    l2 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3799ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 16u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l6;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 873u;
    i1 = l3;
    i2 = b;
    i0 = HostOsQuery(ctx, i0, i1, i2);
    l2 = i0;
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3799ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3799ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 != i1;
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
      i0 = kF3799ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 28u;
      i0 += i1;
      l5 = i0;
      i0 = b;
      i1 = 12u;
      i0 += i1;
      b = i0;
    }
    i0 = l6;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 873u;
      i1 = l5;
      i2 = b;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l2 = i0;
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l2;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3799ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3799ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      if (i0) {goto block_7;}
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 27u);
      i1 = kF3799ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_8;}
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_8;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
  }
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    DeallocateRawBlock(ctx, i0);
    i0 = b;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 4u);
    b = i0;
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 16u);
      i1 = l3;
      i2 = l4;
      i2 = LoadU8(ctx, i2 + 27u);
      l3 = i2;
      i3 = 24u;
      i2 <<= (i3 & 31);
      i3 = 24u;
      i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      l2 = i2;
      i0 = i2 ? i0 : i1;
      l7 = i0;
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 20u);
      i1 = l3;
      i2 = l2;
      i0 = i2 ? i0 : i1;
      l2 = i0;
      loop_21: 
        i0 = b;
        i0 = LoadU8(ctx, i0 + 27u);
        l3 = i0;
        i1 = 24u;
        i0 <<= (i1 & 31);
        i1 = 24u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l5 = i0;
        i0 = l2;
        i1 = b;
        i1 = LoadU32(ctx, i1 + 20u);
        i2 = l3;
        i3 = l5;
        i1 = i3 ? i1 : i2;
        l3 = i1;
        i0 = i0 > i1;
        l6 = i0;
        i0 = l3;
        i1 = l2;
        i2 = l6;
        i0 = i2 ? i0 : i1;
        l6 = i0;
        if (i0) {
          i0 = l7;
          i1 = b;
          i2 = 16u;
          i1 += i2;
          l8 = i1;
          i1 = LoadU32(ctx, i1);
          i2 = l8;
          i3 = l5;
          i1 = i3 ? i1 : i2;
          i2 = l6;
          i0 = CompareBytes(ctx, i0, i1, i2);
          l5 = i0;
          if (i0) {goto block_25;}
        }
        i0 = l2;
        i1 = l3;
        i0 = i0 < i1;
        if (i0) {goto block_24;}
        goto block_23;
        block_25:;
        i0 = l5;
        i1 = kF3799ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {goto block_23;}
        block_24:;
        i0 = b;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        i0 = !(i0);
        if (i0) {goto block_19;}
        goto block_22;
        block_23:;
        i0 = b;
        i0 = LoadU32(ctx, i0 + 4u);
        l3 = i0;
        if (i0) {goto block_22;}
        i0 = b;
        i1 = 4u;
        i0 += i1;
        goto block_18;
        block_22:;
        i0 = l3;
        b = i0;
        goto loop_21;
      TrapUnreachable();
    }
    i0 = a;
    i1 = 4u;
    i0 += i1;
    b = i0;
    block_19:;
    i0 = b;
    block_18:;
    l3 = i0;
    i0 = l4;
    i1 = b;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l4;
    j1 = 0ull;
    StoreU64(ctx, i0, j1);
    i0 = l3;
    i1 = l4;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    b = i0;
    if (i0) {
      i0 = a;
      i1 = b;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
    }
    i0 = a;
    i0 = LoadU32(ctx, i0 + 4u);
    i1 = l4;
    UnlinkListNode(ctx, i0, i1);
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 8u);
    i2 = 1u;
    i1 += i2;
    StoreU32(ctx, i0 + 8u, i1);
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
  i2 = kF3799ResumeWordBytes;
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
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l2;
  i1 = l5;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 20u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
