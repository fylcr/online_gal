// assign_tagged_tree_range.cpp -- clears the destination tagged tree and re-inserts every node of the [first,last) range as a fresh forty-byte node with deep-copied members..
//
// AssignTaggedTreeRange -- clears the destination tagged tree and re-inserts every node of the [first,last) range as a fresh forty-byte node with deep-copied members.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 151176-151176 (f353); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void ReleaseNodeTree(EngineContext* ctx, uint32_t state, uint32_t node);
void UnlinkListNode(EngineContext* ctx, uint32_t node, uint32_t victim);
void InsertTaggedTreeNode(EngineContext* ctx, uint32_t a, uint32_t b);

namespace {

constexpr uint32_t kF353ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF353ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF353SaveFrameBytes = 36u;
constexpr uint32_t kF353ResumeWordBytes = 4u;

}  // namespace

void AssignTaggedTreeRange(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l13 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  uint64_t j1 = 0;
      l11 = 0, l12 = 0, l13 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF353SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
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
    l8 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 32u);
    l9 = i0;
    i0 = l5;
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
    i2 = kF353ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l10 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 8u);
    i0 = !(i0);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    if (i0) {goto running_exit;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = a;
    i1 = a;
    i2 = kF353ResumeWordBytes;
    i1 += i2;
    l9 = i1;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 4u);
    i1 = 0u;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = a;
    j1 = 0ull;
    StoreU64(ctx, i0 + 4u, j1);
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    l7 = i0;
    i1 = l3;
    i2 = l7;
    i0 = i2 ? i0 : i1;
    l6 = i0;
    i0 = !(i0);
    l3 = i0;
    if (i0) {
      i0 = a;
      i1 = l6;
      ReleaseNodeTree(ctx, i0, i1);
      goto running_exit;
    }
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 8u);
    l7 = i0;
    i0 = !(i0);
    if (i0) {goto block_7;}
    i0 = l6;
    i1 = l7;
    i1 = LoadU32(ctx, i1);
    l3 = i1;
    i0 = i0 == i1;
    if (i0) {
      i0 = l7;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l7;
      i0 = LoadU32(ctx, i0 + 4u);
      l3 = i0;
      i0 = !(i0);
      if (i0) {
        i0 = l7;
        l4 = i0;
        goto block_7;
      }
      loop_10: 
        i0 = l3;
        l4 = i0;
        i0 = l3;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        if (i0) {goto loop_10;}
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 4u);
        l3 = i0;
        if (i0) {goto loop_10;}
      goto block_7;
    }
    i0 = l7;
    i1 = 0u;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l3;
    i0 = !(i0);
    if (i0) {
      i0 = l7;
      l4 = i0;
      goto block_7;
    }
    loop_12: 
      i0 = l3;
      l4 = i0;
      i0 = l3;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      if (i0) {goto loop_12;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 4u);
      l3 = i0;
      if (i0) {goto loop_12;}
    block_7:;
    i0 = b;
    i1 = c;
    i0 = i0 == i1;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    if (i0) {
      i0 = l6;
      l7 = i0;
      goto block_13;
    }
    i0 = b;
    l8 = i0;
  }
  loop_16: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      l7 = i0;
      i0 = kF353ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 16u;
      i0 += i1;
      l4 = i0;
      i0 = l8;
      i1 = 16u;
      i0 += i1;
      b = i0;
    }
    i0 = l10;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 870u;
      i1 = l4;
      i2 = b;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l5 = i0;
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF353ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF353ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      if (i0) {goto block_21;}
      i0 = kF353ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l8;
      i1 = 28u;
      i0 += i1;
      l3 = i0;
      i0 = l6;
      i1 = 28u;
      i0 += i1;
      b = i0;
    }
    i0 = l10;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 870u;
      i1 = b;
      i2 = l3;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l5 = i0;
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF353ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF353ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
      if (i0) {goto block_21;}
      i0 = l9;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = !(i0);
      if (i0) {
        i0 = l9;
        b = i0;
        l3 = i0;
        goto block_20;
      }
      i0 = l6;
      i0 = LoadU8(ctx, i0 + 27u);
      l3 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l5 = i0;
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 20u);
      i1 = l3;
      i2 = l5;
      i0 = i2 ? i0 : i1;
      l3 = i0;
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 16u);
      i1 = l4;
      i2 = l5;
      i0 = i2 ? i0 : i1;
      l5 = i0;
      loop_26: 
        i0 = b;
        i0 = LoadU8(ctx, i0 + 27u);
        l4 = i0;
        i1 = 24u;
        i0 <<= (i1 & 31);
        i1 = 24u;
        i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        l11 = i0;
        i0 = b;
        i0 = LoadU32(ctx, i0 + 20u);
        i1 = l4;
        i2 = l11;
        i0 = i2 ? i0 : i1;
        l4 = i0;
        i1 = l3;
        i0 = i0 < i1;
        l12 = i0;
        i0 = l4;
        i1 = l3;
        i2 = l12;
        i0 = i2 ? i0 : i1;
        l12 = i0;
        if (i0) {
          i0 = l5;
          i1 = b;
          i2 = 16u;
          i1 += i2;
          l13 = i1;
          i1 = LoadU32(ctx, i1);
          i2 = l13;
          i3 = l11;
          i1 = i3 ? i1 : i2;
          i2 = l12;
          i0 = CompareBytes(ctx, i0, i1, i2);
          l11 = i0;
          if (i0) {goto block_30;}
        }
        i0 = l3;
        i1 = l4;
        i0 = i0 < i1;
        if (i0) {goto block_29;}
        goto block_28;
        block_30:;
        i0 = l11;
        i1 = kF353ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        if (i0) {goto block_28;}
        block_29:;
        i0 = b;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        if (i0) {goto block_27;}
        i0 = b;
        l3 = i0;
        goto block_20;
        block_28:;
        i0 = b;
        i0 = LoadU32(ctx, i0 + 4u);
        l4 = i0;
        if (i0) {goto block_27;}
        i0 = b;
        i1 = 4u;
        i0 += i1;
        l3 = i0;
        goto block_20;
        block_27:;
        i0 = l4;
        b = i0;
        goto loop_26;
      TrapUnreachable();
    }
    block_21:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      c = i0;
      i0 = HostTempRet0(ctx);
      i0 = a;
      i1 = l6;
      ReleaseNodeTree(ctx, i0, i1);
      i0 = l7;
      if (i0) {
        loop_34: 
          i0 = l7;
          i0 = LoadU32(ctx, i0 + 8u);
          b = i0;
          if (i0) {
            i0 = b;
            l7 = i0;
            goto loop_34;
          }
        i0 = a;
        i1 = l7;
        ReleaseNodeTree(ctx, i0, i1);
      }
      i0 = c;
      HostOsRelease(ctx, i0);
      TrapUnreachable();
    }
    block_20:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i1 = b;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l6;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l3;
      i1 = l6;
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
        l6 = i0;
      }
      i0 = a;
      i0 = LoadU32(ctx, i0 + 4u);
      i1 = l6;
      UnlinkListNode(ctx, i0, i1);
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 8u);
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = 0u;
      l4 = i0;
      i0 = l7;
      i0 = !(i0);
      if (i0) {goto block_38;}
      i0 = l7;
      i0 = LoadU32(ctx, i0 + 8u);
      l3 = i0;
      i0 = !(i0);
      if (i0) {goto block_38;}
      i0 = l7;
      i1 = l3;
      i1 = LoadU32(ctx, i1);
      b = i1;
      i0 = i0 == i1;
      if (i0) {
        i0 = l3;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i0 = LoadU32(ctx, i0 + 4u);
        b = i0;
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          l4 = i0;
          goto block_38;
        }
        loop_41: 
          i0 = b;
          l4 = i0;
          i0 = b;
          i0 = LoadU32(ctx, i0);
          b = i0;
          if (i0) {goto loop_41;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 4u);
          b = i0;
          if (i0) {goto loop_41;}
        goto block_38;
      }
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = b;
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        l4 = i0;
        goto block_38;
      }
      loop_43: 
        i0 = b;
        l4 = i0;
        i0 = b;
        i0 = LoadU32(ctx, i0);
        b = i0;
        if (i0) {goto loop_43;}
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 4u);
        b = i0;
        if (i0) {goto loop_43;}
      block_38:;
      i0 = l8;
      i0 = LoadU32(ctx, i0 + 4u);
      l3 = i0;
      i0 = !(i0);
      if (i0) {
        i0 = l8;
        i1 = l8;
        i1 = LoadU32(ctx, i1 + 8u);
        b = i1;
        i1 = LoadU32(ctx, i1);
        i0 = i0 == i1;
        if (i0) {goto block_44;}
        i0 = l8;
        i1 = 8u;
        i0 += i1;
        l3 = i0;
        loop_46: 
          i0 = l3;
          i0 = LoadU32(ctx, i0);
          l6 = i0;
          i1 = 8u;
          i0 += i1;
          l3 = i0;
          i0 = l6;
          i0 = LoadU32(ctx, i0 + 8u);
          b = i0;
          i0 = LoadU32(ctx, i0);
          i1 = l6;
          i0 = i0 != i1;
          if (i0) {goto loop_46;}
        goto block_44;
      }
      loop_47: 
        i0 = l3;
        b = i0;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        if (i0) {goto loop_47;}
      block_44:;
      i0 = b;
      i1 = c;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_13;}
      i0 = b;
      l8 = i0;
      i0 = l7;
      l6 = i0;
      if (i0) {goto loop_16;}
    }
  block_13:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = l7;
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l4;
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto running_exit;}
    loop_49: 
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 8u);
      l3 = i0;
      if (i0) {
        i0 = l3;
        l4 = i0;
        goto loop_49;
      }
    i0 = a;
    i1 = l4;
    ReleaseNodeTree(ctx, i0, i1);
  }
  running_exit:;
  i0 = l4;
  i1 = b;
  i2 = c;
  i1 = i1 != i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l4 = i0;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    loop_52: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i1 = 16u;
        i0 += i1;
        l3 = i0;
        i0 = b;
        l4 = i0;
      }
      i0 = l10;
      i1 = 2u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = l3;
        InsertTaggedTreeNode(ctx, i0, i1);
        i0 = 2u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 4u);
        l3 = i0;
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i1 = l4;
          i1 = LoadU32(ctx, i1 + 8u);
          b = i1;
          i1 = LoadU32(ctx, i1);
          i0 = i0 == i1;
          l3 = i0;
          if (i0) {goto block_56;}
          i0 = l4;
          i1 = 8u;
          i0 += i1;
          l4 = i0;
          loop_58: 
            i0 = l4;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            i1 = 8u;
            i0 += i1;
            l4 = i0;
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 8u);
            b = i0;
            i0 = LoadU32(ctx, i0);
            i1 = l3;
            i0 = i0 != i1;
            l3 = i0;
            if (i0) {goto loop_58;}
          goto block_56;
        }
        loop_59: 
          i0 = l3;
          b = i0;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          if (i0) {goto loop_59;}
        block_56:;
        i0 = b;
        i1 = c;
        i0 = i0 != i1;
        l4 = i0;
        if (i0) {goto loop_52;}
      }
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
  i2 = kF353ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l5 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l5;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l5;
  i1 = c;
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
  i1 = l8;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l5;
  i1 = l9;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 36u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
