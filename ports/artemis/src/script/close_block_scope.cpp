// close_block_scope.cpp -- closes every scope opened past the current block, releasing their local slots and jumps..
//
// CloseBlockScope -- closes every scope opened past the current block, releasing their local slots and jumps.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 299636-299636 (f781); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void StepRecordResume(EngineContext* ctx, uint32_t a0);
void LexError(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t EmitOpInstruction(EngineContext* ctx, uint32_t func_state, uint32_t op, uint32_t arg_a, uint32_t arg_b, uint32_t line);
void PatchJumpChain(EngineContext* ctx, uint32_t state, uint32_t instruction);
uint32_t CompileStatement(EngineContext* ctx, uint32_t state);

namespace {

constexpr uint32_t kF781MsgChunkHasTooMany = 0x2B633u;  // chunk has too many syntax levels
constexpr uint32_t kF781Const8010007 = 0x8010007u;  // 
constexpr uint32_t kF781ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF781SaveFrameBytes = 28u;
constexpr uint32_t kF781ResumeWordBytes = 4u;

}  // namespace

void CloseBlockScope(EngineContext* ctx, uint32_t state) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
      l9 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF781SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l1 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 8u);
    l3 = i0;
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 16u);
    l5 = i0;
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 24u);
    l8 = i0;
    i0 = l1;
    i0 = LoadU32(ctx, i0 + 4u);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF781ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l7 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    i1 = 16u;
    i0 -= i1;
    l5 = i0;
    GuestStackSp(ctx) = i0;
    i0 = state;
    i0 = LoadU32(ctx, i0 + 48u);
    l4 = i0;
    i0 = l5;
    i1 = 0u;
    StoreU8(ctx, i0 + 10u, i1);
    i0 = l5;
    i1 = kF781ConstFFFFFFFF;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l4;
    i0 = LoadU8(ctx, i0 + 50u);
    l2 = i0;
    i0 = l5;
    i1 = 0u;
    StoreU8(ctx, i0 + 9u, i1);
    i0 = l5;
    i1 = l2;
    StoreU8(ctx, i0 + 8u, i1);
    i0 = l5;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 20u);
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = l5;
    StoreU32(ctx, i0 + 20u, i1);
    i0 = state;
    i0 = LoadU32(ctx, i0 + 52u);
    l3 = i0;
    i0 = LoadU16(ctx, i0 + 52u);
    i1 = 1u;
    i0 += i1;
    l2 = i0;
    i0 = l3;
    i1 = l2;
    StoreU16(ctx, i0 + 52u, i1);
    i0 = l2;
    i1 = 65535u;
    i0 &= i1;
    i1 = 201u;
    i0 = i0 >= i1;
    l3 = i0;
  }
  i0 = l3;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l7;
    i0 = !(i0);
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = state;
      i1 = kF781MsgChunkHasTooMany;
      i2 = 0u;
      LexError(ctx, i0, i1, i2);
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
  }
  loop_7: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 16u);
      i1 = 260u;
      i0 -= i1;
      l2 = i0;
      i1 = 27u;
      i0 = i0 <= i1;
      i1 = 0u;
      i2 = 1u;
      i3 = l2;
      i2 <<= (i3 & 31);
      i3 = kF781Const8010007;
      i2 &= i3;
      i0 = i2 ? i0 : i1;
      i0 = !(i0);
      l3 = i0;
    }
    i0 = l3;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = l7;
      i1 = 1u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i0 = CompileStatement(ctx, i0);
        l1 = i0;
        i0 = 1u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l1;
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = l3;
      } else {
        i0 = state;
        i0 = LoadU32(ctx, i0 + 16u);
        i1 = 59u;
        i0 = i0 == i1;
      }
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = l7;
        i1 = 2u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = state;
          StepRecordResume(ctx, i0);
          i0 = 2u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = state;
        i0 = LoadU32(ctx, i0 + 48u);
        l6 = i0;
        i0 = LoadU8(ctx, i0 + 50u);
        l3 = i0;
        i0 = l6;
        i1 = l3;
        StoreU32(ctx, i0 + 36u, i1);
        i0 = l2;
        i0 = !(i0);
        l3 = i0;
        if (i0) {goto loop_7;}
      }
    }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = state;
    i0 = LoadU32(ctx, i0 + 52u);
    state = i0;
    i0 = LoadU16(ctx, i0 + 52u);
    i1 = 1u;
    i0 -= i1;
    l3 = i0;
    i0 = state;
    i1 = l3;
    StoreU16(ctx, i0 + 52u, i1);
    i0 = l4;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 20u);
    l6 = i1;
    i1 = LoadU32(ctx, i1);
    StoreU32(ctx, i0 + 20u, i1);
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    i0 = LoadU32(ctx, i0 + 48u);
    l2 = i0;
    i0 = LoadU8(ctx, i0 + 50u);
    state = i0;
    i1 = l6;
    i1 = LoadU8(ctx, i1 + 8u);
    l8 = i1;
    i0 = i0 > i1;
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 24u);
      l1 = i0;
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 24u);
      l3 = i0;
      loop_19: 
        i0 = l3;
        i1 = l2;
        i2 = state;
        i3 = 1u;
        i2 -= i3;
        state = i2;
        i3 = 255u;
        i2 &= i3;
        l9 = i2;
        i3 = 1u;
        i2 <<= (i3 & 31);
        i1 += i2;
        i1 = LoadU16(ctx, i1 + 172u);
        i2 = 12u;
        i1 *= i2;
        i0 += i1;
        i1 = l1;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l8;
        i1 = l9;
        i0 = i0 < i1;
        if (i0) {goto loop_19;}
      i0 = l2;
      i1 = l8;
      StoreU8(ctx, i0 + 50u, i1);
    }
    i0 = l6;
    i0 = LoadU8(ctx, i0 + 9u);
    state = i0;
  }
  i0 = state;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = l7;
    i1 = 3u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l4;
      i1 = 35u;
      i2 = l8;
      i3 = 0u;
      i4 = 0u;
      i0 = EmitOpInstruction(ctx, i0, i1, i2, i3, i4);
      l1 = i0;
      i0 = 3u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l1;
      state = i0;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = l4;
    i1 = LoadU8(ctx, i1 + 50u);
    StoreU32(ctx, i0 + 36u, i1);
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 4u);
    state = i0;
  }
  i0 = l7;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l4;
    i1 = state;
    PatchJumpChain(ctx, i0, i1);
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = 16u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
  }
  goto func_exit;
  spill_frame:;
  l1 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l1;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF781ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l1 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l1;
  i1 = l2;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l1;
  i1 = l3;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l1;
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l1;
  i1 = l5;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l1;
  i1 = l6;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l1;
  i1 = l8;
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
