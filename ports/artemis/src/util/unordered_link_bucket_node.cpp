// unordered_link_bucket_node.cpp -- links a node onto a bucket chain and keeps the table's node count and load factor in step with it..
//
// UnorderedLinkBucketNode -- links a node onto a bucket chain and keeps the table's node count and load factor in step with it.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 130272-130272 (f272); R1 transcribe-keep.
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

constexpr uint32_t kF272ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF272ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF272SaveFrameBytes = 32u;
constexpr uint32_t kF272ResumeWordBytes = 4u;

}  // namespace

void UnorderedLinkBucketNode(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l13 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF272SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l6 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 12u);
    d = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 16u);
    e = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 20u);
    f = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 24u);
    l7 = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 28u);
    l8 = i0;
    i0 = l6;
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
    i2 = kF272ResumeWordBytes;
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
    i0 = b;
    i0 = LoadU32(ctx, i0 + 4u);
    d = i0;
    if (i0) {
      i0 = c;
      i0 = LoadU32(ctx, i0);
      i1 = c;
      i2 = c;
      i2 = LoadU8(ctx, i2 + 11u);
      f = i2;
      i3 = 24u;
      i2 <<= (i3 & 31);
      i3 = 24u;
      i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      l7 = i2;
      i0 = i2 ? i0 : i1;
      l10 = i0;
      i0 = c;
      i0 = LoadU32(ctx, i0 + 4u);
      i1 = f;
      i2 = l7;
      i0 = i2 ? i0 : i1;
      l7 = i0;
      i0 = b;
      i1 = 4u;
      i0 += i1;
      f = i0;
      loop_7: 
        i0 = d;
        i0 = LoadU8(ctx, i0 + 27u);
        c = i0;
        i1 = 24u;
        i0 <<= (i1 & 31);
        i1 = 24u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l8 = i0;
        i0 = l7;
        i1 = d;
        i1 = LoadU32(ctx, i1 + 20u);
        i2 = c;
        i3 = l8;
        i1 = i3 ? i1 : i2;
        c = i1;
        i0 = i0 > i1;
        l11 = i0;
        i0 = c;
        i1 = l7;
        i2 = l11;
        i0 = i2 ? i0 : i1;
        l12 = i0;
        if (i0) {
          i0 = l10;
          i1 = d;
          i2 = 16u;
          i1 += i2;
          l6 = i1;
          i1 = LoadU32(ctx, i1);
          i2 = l6;
          i3 = l8;
          i1 = i3 ? i1 : i2;
          l8 = i1;
          i2 = l12;
          i0 = CompareBytes(ctx, i0, i1, i2);
          l6 = i0;
          i0 = !(i0);
          if (i0) {
            i0 = c;
            i1 = l7;
            i0 = i0 > i1;
            if (i0) {goto block_13;}
            goto block_12;
          }
          i0 = l6;
          i1 = kF272ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          if (i0) {goto block_12;}
          goto block_13;
        }
        i0 = c;
        i1 = l7;
        i0 = i0 <= i1;
        if (i0) {goto block_11;}
        block_13:;
        i0 = d;
        i0 = LoadU32(ctx, i0);
        c = i0;
        if (i0) {goto block_8;}
        goto block_5;
        block_12:;
        i0 = l8;
        i1 = l10;
        i2 = l12;
        i0 = CompareBytes(ctx, i0, i1, i2);
        c = i0;
        if (i0) {goto block_10;}
        block_11:;
        i0 = l11;
        if (i0) {goto block_9;}
        goto running_exit;
        block_10:;
        i0 = c;
        i1 = kF272ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {goto running_exit;}
        block_9:;
        i0 = d;
        i1 = 4u;
        i0 += i1;
        f = i0;
        i0 = d;
        i0 = LoadU32(ctx, i0 + 4u);
        c = i0;
        i0 = !(i0);
        l8 = i0;
        if (i0) {goto running_exit;}
        i0 = f;
        d = i0;
        block_8:;
        i0 = d;
        f = i0;
        i0 = c;
        d = i0;
        goto loop_7;
      TrapUnreachable();
    }
    i0 = b;
    i1 = 4u;
    i0 += i1;
    d = i0;
    block_5:;
    i0 = d;
    f = i0;
    running_exit:;
    i0 = a;
    l7 = i0;
    i0 = f;
    i0 = LoadU32(ctx, i0);
    c = i0;
  }
  i0 = b;
  i1 = 0u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  i1 = b;
  i2 = l13;
  i3 = c;
  i4 = ResumePhase(ctx);
  i2 = i4 ? i2 : i3;
  l13 = i2;
  i3 = ResumePhase(ctx);
  i4 = 2u;
  i3 = i3 == i4;
  i2 |= i3;
  i0 = i2 ? i0 : i1;
  b = i0;
  i0 = l13;
  i0 = !(i0);
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l9;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 40u;
      i0 = AllocateStringBuffer(ctx, i0);
      l6 = i0;
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l6;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = e;
      i0 = LoadU32(ctx, i0);
      e = i0;
      i0 = kF272ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 16u;
      i0 += i1;
      l8 = i0;
    }
    i0 = l9;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 873u;
      i1 = l8;
      i2 = e;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = b;
    } else {
      i0 = kF272ConstD015C;
      i0 = LoadU32(ctx, i0);
      e = i0;
      i0 = kF272ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = e;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_16;}
      i0 = c;
      i1 = 0u;
      StoreU32(ctx, i0 + 36u, i1);
      i0 = c;
      j1 = 0ull;
      StoreU64(ctx, i0 + 28u, j1);
      i0 = c;
      i1 = d;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = c;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = f;
      i1 = c;
      StoreU32(ctx, i0, i1);
      i0 = c;
      e = i0;
      i0 = b;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0);
      d = i0;
      if (i0) {
        i0 = b;
        i1 = d;
        StoreU32(ctx, i0, i1);
        i0 = f;
        i0 = LoadU32(ctx, i0);
        e = i0;
      }
      i0 = b;
      i0 = LoadU32(ctx, i0 + 4u);
      i1 = e;
      UnlinkListNode(ctx, i0, i1);
      i0 = b;
      i1 = b;
      i1 = LoadU32(ctx, i1 + 8u);
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = 1u;
    }
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    i1 = b;
    StoreU8(ctx, i0 + 4u, i1);
    i0 = a;
    i1 = c;
    StoreU32(ctx, i0, i1);
    goto func_exit;
  }
  block_16:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = c;
    DeallocateRawBlock(ctx, i0);
    i0 = a;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  goto func_exit;
  spill_frame:;
  l6 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l6;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF272ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l6 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l6;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l6;
  i1 = c;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l6;
  i1 = d;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l6;
  i1 = e;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l6;
  i1 = f;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l6;
  i1 = l7;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l6;
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
