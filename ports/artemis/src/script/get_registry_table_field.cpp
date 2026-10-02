// get_registry_table_field.cpp -- reads one field by key out of the Lua registry pseudo-table into two cleared value slots..
//
// GetRegistryTableField -- reads one field by key out of the Lua registry pseudo-table into two cleared value slots.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 296211-296211 (f772); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void Panic(EngineContext* ctx, uint32_t a0);
void ResizeValueStack(EngineContext* ctx, uint32_t a0, uint32_t a1);
void PushTableField(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void SetStackTop(EngineContext* ctx, uint32_t stack, uint32_t index);

namespace {

constexpr uint32_t kF772ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF772ConstFFFFD8F0 = 0xFFFFD8F0u;  // 
constexpr uint32_t kF772ConstFFFFFFFD = 0xFFFFFFFDu;  // 
constexpr uint32_t kF772ConstFFFFFFFE = 0xFFFFFFFEu;  // 
constexpr uint32_t kF772ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF772SaveFrameBytes = 24u;
constexpr uint32_t kF772ResumeWordBytes = 4u;

}  // namespace

uint32_t GetRegistryTableField(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF772SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 16u);
    l6 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 20u);
    l7 = i0;
    i0 = l3;
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
    i2 = kF772ResumeWordBytes;
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
    i0 = b;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = a;
    j1 = 18446744065119617024ull;
    StoreU64(ctx, i0 + 12u, j1);
    i0 = a;
    j1 = 18446744065119617024ull;
    StoreU64(ctx, i0 + 4u, j1);
    i0 = a;
    i1 = l4;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i0 = LoadU32(ctx, i0 + 4u);
    b = i0;
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = kF772ConstFFFFD8F0;
    i2 = b;
    PushTableField(ctx, i0, i1, i2);
    i0 = kF772ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF772ConstD015C;
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
    if (i0) {goto block_10;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i1 = 0u;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = b;
    i1 = l2;
    i2 = 16u;
    i1 += i2;
    l2 = i1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = kF772ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    b = i0;
    if (i0) {goto block_10;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l5;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 987u;
    i1 = b;
    i2 = kF772ConstFFFFFFFE;
    i0 = HostOsQuery(ctx, i0, i1, i2);
    l3 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF772ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_10;}
    i0 = b;
    i0 = !(i0);
    b = i0;
    if (i0) {goto block_8;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF772ConstFFFFFFFE;
    SetStackTop(ctx, i0, i1);
    i0 = kF772ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_9;}
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l5;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1001u;
    i1 = b;
    i2 = kF772ConstFFFFD8F0;
    i0 = HostOsQuery(ctx, i0, i1, i2);
    l3 = i0;
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF772ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_9;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 12u);
    l2 = i0;
    i0 = a;
    i1 = b;
    StoreU32(ctx, i0 + 12u, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
    l7 = i0;
    i0 = a;
    i1 = l6;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = l2;
    i0 = !(i0);
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    if (i0) {goto block_16;}
    i0 = l7;
    i1 = kF772ConstFFFFFFFE;
    i0 = i0 == i1;
    if (i0) {goto block_16;}
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l5;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1002u;
    i1 = l2;
    i2 = kF772ConstFFFFD8F0;
    i3 = l7;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 2u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF772ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto running_exit;}
  }
  block_16:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF772ConstFFFFFFFD;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF772ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto running_exit;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF772ConstFFFFFFFF;
    SetStackTop(ctx, i0, i1);
    i0 = kF772ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_6;}
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l5;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1001u;
    i1 = b;
    i2 = kF772ConstFFFFD8F0;
    i0 = HostOsQuery(ctx, i0, i1, i2);
    l3 = i0;
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l3;
    l7 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF772ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_6;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 4u);
    l2 = i0;
    i0 = a;
    i1 = b;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
    i0 = a;
    i1 = l7;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l2;
    i0 = !(i0);
    if (i0) {goto block_7;}
    i0 = b;
    i1 = kF772ConstFFFFFFFE;
    i0 = i0 == i1;
    if (i0) {goto block_7;}
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l5;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1002u;
    i1 = l2;
    i2 = kF772ConstFFFFD8F0;
    i3 = b;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF772ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_7;}
    goto running_exit;
  }
  block_10:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_5;
  }
  block_9:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF772ConstFFFFFFFD;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF772ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_5;}
    goto running_exit;
  }
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = kF772ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF772ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto running_exit;}
    i0 = a;
    goto func_exit;
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = kF772ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF772ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto running_exit;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 12u);
    b = i0;
    i0 = !(i0);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    if (i0) {goto block_31;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i1 = kF772ConstFFFFFFFE;
    i0 = i0 == i1;
    if (i0) {goto block_31;}
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l5;
  i1 = 5u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1002u;
    i1 = b;
    i2 = kF772ConstFFFFD8F0;
    i3 = l4;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 5u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF772ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    b = i0;
    if (i0) {goto running_exit;}
  }
  block_31:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 4u);
    b = i0;
    i0 = !(i0);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    if (i0) {goto block_36;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 8u);
    a = i0;
    i1 = kF772ConstFFFFFFFE;
    i0 = i0 == i1;
    if (i0) {goto block_36;}
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l5;
  i1 = 6u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1002u;
    i1 = b;
    i2 = kF772ConstFFFFD8F0;
    i3 = a;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 6u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF772ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF772ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto running_exit;}
  }
  block_36:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  running_exit:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    a = i0;
    i0 = HostTempRet0(ctx);
  }
  i0 = l5;
  i1 = 7u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 7u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    TrapUnreachable();
  }
  TrapUnreachable();
  spill_frame:;
  l3 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l3;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF772ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l3;
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l3;
  i1 = l6;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l3;
  i1 = l7;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 24u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = 0u;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
