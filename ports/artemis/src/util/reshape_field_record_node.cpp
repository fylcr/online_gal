// reshape_field_record_node.cpp -- reshapes a node whose payload is a record of fixed-width fields..
//
// ReshapeFieldRecordNode -- reshapes a node whose payload is a record of fixed-width fields.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 2733955-2733955 (f5629); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size);

namespace {

constexpr uint32_t kF5629SaveFrameBytes = 16u;
constexpr uint32_t kF5629ResumeWordBytes = 4u;

}  // namespace

void ReshapeFieldRecordNode(EngineContext* ctx, uint32_t state, uint32_t node) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF5629SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    node = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF5629ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = node;
    i0 = LoadU32(ctx, i0 + 88u);
    l2 = i0;
    i0 = node;
    i0 = LoadU32(ctx, i0 + 84u);
    l4 = i0;
    if (i0) {
      i0 = l4;
      i1 = l2;
      StoreU32(ctx, i0 + 88u, i1);
      i0 = node;
      i0 = LoadU32(ctx, i0 + 88u);
      l2 = i0;
      goto running_exit;
    }
    i0 = state;
    i0 = LoadU32(ctx, i0 + 16u);
    i1 = l2;
    StoreU32(ctx, i0 + 256u, i1);
    running_exit:;
    i0 = l2;
    if (i0) {
      i0 = l2;
      i1 = l4;
      StoreU32(ctx, i0 + 84u, i1);
    }
    i0 = node;
    i0 = LoadU32(ctx, i0 + 80u);
    i1 = 4u;
    i0 <<= (i1 & 31);
    l4 = i0;
    i0 = node;
    i0 = LoadU32(ctx, i0 + 76u);
    l2 = i0;
  }
  i0 = l5;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l2;
    i2 = l4;
    i3 = 0u;
    i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
    l3 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = node;
    i0 = LoadU32(ctx, i0 + 44u);
    i1 = 2u;
    i0 <<= (i1 & 31);
    l4 = i0;
    i0 = node;
    i0 = LoadU32(ctx, i0 + 12u);
    l2 = i0;
  }
  i0 = l5;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l2;
    i2 = l4;
    i3 = 0u;
    i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
    l3 = i0;
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = node;
    i0 = LoadU32(ctx, i0 + 52u);
    i1 = 2u;
    i0 <<= (i1 & 31);
    l4 = i0;
    i0 = node;
    i0 = LoadU32(ctx, i0 + 16u);
    l2 = i0;
  }
  i0 = l5;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l2;
    i2 = l4;
    i3 = 0u;
    i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
    l3 = i0;
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = node;
    i0 = LoadU32(ctx, i0 + 40u);
    i1 = 4u;
    i0 <<= (i1 & 31);
    l4 = i0;
    i0 = node;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
  }
  i0 = l5;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l2;
    i2 = l4;
    i3 = 0u;
    i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
    l3 = i0;
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = node;
    i0 = LoadU32(ctx, i0 + 48u);
    i1 = 2u;
    i0 <<= (i1 & 31);
    l4 = i0;
    i0 = node;
    i0 = LoadU32(ctx, i0 + 20u);
    l2 = i0;
  }
  i0 = l5;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l2;
    i2 = l4;
    i3 = 0u;
    i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
    l3 = i0;
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = node;
    i0 = LoadU32(ctx, i0 + 56u);
    i1 = 12u;
    i0 *= i1;
    l4 = i0;
    i0 = node;
    i0 = LoadU32(ctx, i0 + 24u);
    l2 = i0;
  }
  i0 = l5;
  i1 = 5u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l2;
    i2 = l4;
    i3 = 0u;
    i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
    l3 = i0;
    i0 = 5u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = node;
    i0 = LoadU32(ctx, i0 + 36u);
    i1 = 2u;
    i0 <<= (i1 & 31);
    l4 = i0;
    i0 = node;
    i0 = LoadU32(ctx, i0 + 28u);
    l2 = i0;
  }
  i0 = l5;
  i1 = 6u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = l2;
    i2 = l4;
    i3 = 0u;
    i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
    i0 = 6u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = l5;
  i1 = 7u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = state;
    i1 = node;
    i2 = 92u;
    i3 = 0u;
    i0 = ReallocateBlock(ctx, i0, i1, i2, i3);
    i0 = 7u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  goto func_exit;
  spill_frame:;
  l3 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l3;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF5629ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = node;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l3;
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 16u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
