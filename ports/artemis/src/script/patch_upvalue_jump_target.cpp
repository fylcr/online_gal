// patch_upvalue_jump_target.cpp -- patches upvalue jump targets: the pending upvalue jumps are resolved once the binding is known..
//
// PatchUpvalueJumpTarget -- patches upvalue jump targets: the pending upvalue jumps are resolved once the binding is known.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 802664-802664 (f1721); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void EmitJumpInstruction(EngineContext* ctx, uint32_t state, uint32_t register_hint, uint32_t target);
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size);
uint32_t RaiseBlockTooBig(EngineContext* ctx, uint32_t a);
uint32_t BindUpvalueCell(EngineContext* ctx, uint32_t state, uint32_t local, uint32_t upvalue);

namespace {

constexpr uint32_t kF1721ConstFFFFFFF = 0xFFFFFFFu;  // 
constexpr uint32_t kF1721Const15555555 = 0x15555555u;  // 
constexpr uint32_t kF1721Const3FFFFFFF = 0x3FFFFFFFu;  // 
constexpr uint32_t kF1721SaveFrameBytes = 32u;
constexpr uint32_t kF1721ResumeWordBytes = 4u;

}  // namespace

void PatchUpvalueJumpTarget(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
      l9 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1721SaveFrameBytes;
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
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 24u);
    l7 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 28u);
    l9 = i0;
    i0 = l2;
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
    i2 = kF1721ResumeWordBytes;
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
    i0 = LoadU32(ctx, i0 + 52u);
    l7 = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 48u);
    l6 = i0;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = l6;
    i0 = LoadU8(ctx, i0 + 50u);
    l1 = i0;
    if (i0) {
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 24u);
      l5 = i0;
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 24u);
      l9 = i0;
      loop_5: 
        i0 = l1;
        i1 = 1u;
        i0 -= i1;
        l1 = i0;
        i1 = 255u;
        i0 &= i1;
        l4 = i0;
        i1 = 1u;
        i0 <<= (i1 & 31);
        i1 = l6;
        i0 += i1;
        i0 = LoadU16(ctx, i0 + 172u);
        i1 = 12u;
        i0 *= i1;
        i1 = l5;
        i0 += i1;
        i1 = l9;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l4;
        if (i0) {goto loop_5;}
      i0 = l6;
      i1 = 0u;
      StoreU8(ctx, i0 + 50u, i1);
    }
  }
  i0 = l8;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = 0u;
    i2 = 0u;
    EmitJumpInstruction(ctx, i0, i1, i2);
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 24u);
    l1 = i0;
    i1 = 1u;
    i0 += i1;
    i1 = kF1721Const3FFFFFFF;
    i0 = i0 <= i1;
    l5 = i0;
    i0 = l3;
    l4 = i0;
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
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 12u);
      l5 = i0;
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 44u);
      i1 = 2u;
      i0 <<= (i1 & 31);
      l9 = i0;
      i0 = l1;
      i1 = 2u;
      i0 <<= (i1 & 31);
      l1 = i0;
    }
    i0 = l8;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l7;
      i1 = l5;
      i2 = l9;
      i3 = l1;
      i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
      l2 = i0;
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l2;
      l1 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_8;}
  }
  i0 = l8;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l7;
    i0 = RaiseBlockTooBig(ctx, i0);
    l2 = i0;
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l1;
    StoreU32(ctx, i0 + 12u, i1);
    i0 = l3;
    i1 = l6;
    i1 = LoadU32(ctx, i1 + 24u);
    l1 = i1;
    StoreU32(ctx, i0 + 44u, i1);
    i0 = l1;
    i1 = 1u;
    i0 += i1;
    i1 = kF1721Const3FFFFFFF;
    i0 = i0 <= i1;
    l5 = i0;
    i0 = l3;
    l4 = i0;
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
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 20u);
      l5 = i0;
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 48u);
      i1 = 2u;
      i0 <<= (i1 & 31);
      l9 = i0;
      i0 = l1;
      i1 = 2u;
      i0 <<= (i1 & 31);
      l1 = i0;
    }
    i0 = l8;
    i1 = 3u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l7;
      i1 = l5;
      i2 = l9;
      i3 = l1;
      i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
      l2 = i0;
      i0 = 3u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l2;
      l1 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_14;}
  }
  i0 = l8;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l7;
    i0 = RaiseBlockTooBig(ctx, i0);
    l2 = i0;
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  block_14:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l1;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = l3;
    i1 = l6;
    i1 = LoadU32(ctx, i1 + 24u);
    StoreU32(ctx, i0 + 48u, i1);
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 40u);
    l1 = i0;
    i1 = 1u;
    i0 += i1;
    i1 = kF1721ConstFFFFFFF;
    i0 = i0 <= i1;
    l5 = i0;
    i0 = l3;
    l4 = i0;
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
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 8u);
      l5 = i0;
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 40u);
      i1 = kF1721ResumeWordBytes;
      i0 <<= (i1 & 31);
      l9 = i0;
      i0 = l1;
      i1 = 4u;
      i0 <<= (i1 & 31);
      l1 = i0;
    }
    i0 = l8;
    i1 = 5u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l7;
      i1 = l5;
      i2 = l9;
      i3 = l1;
      i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
      l2 = i0;
      i0 = 5u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l2;
      l1 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_20;}
  }
  i0 = l8;
  i1 = 6u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l7;
    i0 = RaiseBlockTooBig(ctx, i0);
    l2 = i0;
    i0 = 6u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  block_20:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l3;
    i1 = l6;
    i1 = LoadU32(ctx, i1 + 40u);
    StoreU32(ctx, i0 + 40u, i1);
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 44u);
    l1 = i0;
    i1 = 1u;
    i0 += i1;
    i1 = kF1721Const3FFFFFFF;
    i0 = i0 <= i1;
    l5 = i0;
    i0 = l3;
    l4 = i0;
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
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 16u);
      l5 = i0;
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 52u);
      i1 = 2u;
      i0 <<= (i1 & 31);
      l9 = i0;
      i0 = l1;
      i1 = 2u;
      i0 <<= (i1 & 31);
      l1 = i0;
    }
    i0 = l8;
    i1 = 7u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l7;
      i1 = l5;
      i2 = l9;
      i3 = l1;
      i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
      l2 = i0;
      i0 = 7u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l2;
      l1 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_26;}
  }
  i0 = l8;
  i1 = 8u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l7;
    i0 = RaiseBlockTooBig(ctx, i0);
    l2 = i0;
    i0 = 8u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  block_26:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l1;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = l3;
    i1 = l6;
    i1 = LoadU32(ctx, i1 + 44u);
    StoreU32(ctx, i0 + 52u, i1);
    i0 = l6;
    i0 = LoadS16(ctx, i0 + 48u);
    l1 = i0;
    i1 = 1u;
    i0 += i1;
    i1 = kF1721Const15555555;
    i0 = i0 <= i1;
    l5 = i0;
    i0 = l3;
    l4 = i0;
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
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 24u);
      l5 = i0;
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 56u);
      i1 = 12u;
      i0 *= i1;
      l9 = i0;
      i0 = l1;
      i1 = 12u;
      i0 *= i1;
      l1 = i0;
    }
    i0 = l8;
    i1 = 9u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l7;
      i1 = l5;
      i2 = l9;
      i3 = l1;
      i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
      l2 = i0;
      i0 = 9u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l2;
      l1 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_32;}
  }
  i0 = l8;
  i1 = 10u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l7;
    i0 = RaiseBlockTooBig(ctx, i0);
    l2 = i0;
    i0 = 10u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l2;
    l1 = i0;
  }
  block_32:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l1;
    StoreU32(ctx, i0 + 24u, i1);
    i0 = l3;
    i1 = l6;
    i1 = LoadS16(ctx, i1 + 48u);
    StoreU32(ctx, i0 + 56u, i1);
    i0 = l3;
    i0 = LoadU8(ctx, i0 + 72u);
    i1 = 2u;
    i0 <<= (i1 & 31);
    l5 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 28u);
    l4 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 36u);
    i1 = 2u;
    i0 <<= (i1 & 31);
    l1 = i0;
  }
  i0 = l8;
  i1 = 11u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l7;
    i1 = l4;
    i2 = l1;
    i3 = l5;
    i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
    l2 = i0;
    i0 = 11u;
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
    i0 = l3;
    i1 = l4;
    StoreU32(ctx, i0 + 28u, i1);
    i0 = l3;
    i1 = l3;
    i1 = LoadU8(ctx, i1 + 72u);
    l4 = i1;
    StoreU32(ctx, i0 + 36u, i1);
    i0 = a;
    i1 = l6;
    i1 = LoadU32(ctx, i1 + 8u);
    StoreU32(ctx, i0 + 48u, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = 285u;
    i0 -= i1;
    i1 = 1u;
    i0 = i0 <= i1;
    l3 = i0;
  }
  i0 = l3;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      l3 = i0;
      i0 = LoadU32(ctx, i0 + 24u);
      a = i0;
      i1 = 16u;
      i0 += i1;
      l4 = i0;
      i0 = a;
      i0 = LoadU32(ctx, i0 + 12u);
      a = i0;
    }
    i0 = l8;
    i1 = 12u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l3;
      i1 = l4;
      i2 = a;
      i0 = BindUpvalueCell(ctx, i0, i1, i2);
      i0 = 12u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l7;
    i1 = l7;
    i1 = LoadU32(ctx, i1 + 8u);
    i2 = 32u;
    i1 -= i2;
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
  i2 = kF1721ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l2 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l2;
  i1 = l1;
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
  i1 = l9;
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
