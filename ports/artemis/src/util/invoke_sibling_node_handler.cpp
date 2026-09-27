// invoke_sibling_node_handler.cpp -- the same call after the back pointer is used to reach the owning node first..
//
// InvokeSiblingNodeHandler -- the same call after the back pointer is used to reach the owning node first.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 2600634-2600755 (f4882); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF4882ResumeWordBytes = 4u;

}  // namespace

void InvokeSiblingNodeHandler(EngineContext* ctx, uint32_t node, uint32_t slot, float scale) {
  EnterFrame();
  uint32_t l3 = 0u; uint32_t l4 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  float f0 = 0; float f1 = 0; float f2 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = 16u;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    node = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 4u);
    slot = i0;
    i0 = l3;
    f0 = LoadF32(ctx, i0 + 8u);
    scale = f0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF4882ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = node;
    i0 = LoadU32(ctx, i0);
    i1 = 24u;
    i0 -= i1;
    i0 = LoadU32(ctx, i0);
    i1 = node;
    i0 += i1;
    node = i0;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 148u);
    l3 = i0;
  }
  i0 = l4;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = node;
    i1 = slot;
    f2 = scale;
    i3 = 1u;
    i4 = l3;
    CallTableT16(ctx, i4, i0, i1, f2, i3);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
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
  i2 = kF4882ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  i1 = node;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = slot;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l4;
  f1 = scale;
  StoreF32(ctx, i0 + 8u, f1);
  i0 = l4;
  i1 = l3;
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
