// pack_manager_b.cpp -- the second file and index tag body: it drives setScriptStack through the registry table helper and reports its Japanese argument error..
//
// PackManager::FileDispatchB -- the second file and index tag body: it drives setScriptStack through the registry table helper and reports its Japanese argument error.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 1371754-1371754 (f3086); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void Panic(EngineContext* ctx, uint32_t a0);
void ResizeValueStack(EngineContext* ctx, uint32_t a0, uint32_t a1);
void PushTableField(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void SetStackTop(EngineContext* ctx, uint32_t stack, uint32_t index);
uint32_t QueryValueType(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t CompareNulTerminatedStrings(EngineContext* ctx, uint32_t left, uint32_t right);
void MakeStringFromBytes(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t GetRegistryTableField(EngineContext* ctx, uint32_t a, uint32_t b);

namespace {

constexpr uint32_t kF3086MsgFile = 0xBAE4Cu;  // file
constexpr uint32_t kF3086MsgIndex = 0xBB108u;  // index
constexpr uint32_t kF3086ConstBB33F = 0xBB33Fu;  // 
constexpr uint32_t kF3086ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF3086ConstFFFFD8F0 = 0xFFFFD8F0u;  // 
constexpr uint32_t kF3086ConstFFFFFFFD = 0xFFFFFFFDu;  // 
constexpr uint32_t kF3086ConstFFFFFFFE = 0xFFFFFFFEu;  // 
constexpr uint32_t kF3086ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF3086SaveFrameBytes = 28u;
constexpr uint32_t kF3086ResumeWordBytes = 4u;

}  // namespace

namespace PackManager {
void FileDispatchB(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u; uint32_t i6 = 0u;
  uint64_t j1 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF3086SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l7 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 24u);
    l6 = i0;
    i0 = l7;
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
    i2 = kF3086ResumeWordBytes;
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
    i0 = GuestStackSp(ctx);
    i1 = 80u;
    i0 -= i1;
    l5 = i0;
    GuestStackSp(ctx) = i0;
    i0 = b;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i1 = kF3086ConstFFFFD8F0;
    i2 = b;
    i2 = LoadU32(ctx, i2 + 4u);
    PushTableField(ctx, i0, i1, i2);
    i0 = kF3086ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = kF3086ConstFFFFFFFF;
    i0 = QueryValueType(ctx, i0, i1);
    l3 = i0;
    i0 = kF3086ConstD015C;
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = kF3086ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 1u;
    i0 = i0 != i1;
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    if (i0) {
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3086ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      if (i0) {goto block_6;}
      goto running_exit;
    }
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = kF3086ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = kF3086ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3086ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3086ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto running_exit;}
    i0 = a;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
    block_6:;
    i0 = l3;
    i1 = 5u;
    i0 = i0 != i1;
    l2 = i0;
  }
  i0 = l2;
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
      i0 = a;
      i1 = 1u;
      i2 = kF3086ConstBB33F;
      MakeStringFromBytes(ctx, i0, i1, i2);
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_8;}
  }
  i0 = l2;
  i1 = l5;
  i2 = 56u;
  i1 += i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l2 = i0;
  i0 = l8;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = b;
    i0 = GetRegistryTableField(ctx, i0, i1);
    l7 = i0;
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = kF3086ConstFFFFFFFE;
    StoreU32(ctx, i0 + 48u, i1);
    i0 = l5;
    j1 = 4294967294ull;
    StoreU64(ctx, i0 + 40u, j1);
    i0 = l5;
    j1 = 0ull;
    StoreU64(ctx, i0 + 32u, j1);
  }
  loop_14: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 32u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l8;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3375u;
      i1 = b;
      i2 = l2;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l7 = i0;
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_15;}
    }
    i0 = l2;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        i0 = LoadU32(ctx, i0 + 44u);
        a = i0;
        i0 = !(i0);
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        if (i0) {goto block_21;}
        i0 = l5;
        i0 = LoadU32(ctx, i0 + 48u);
        l2 = i0;
        i1 = kF3086ConstFFFFFFFE;
        i0 = i0 == i1;
        l3 = i0;
        if (i0) {goto block_21;}
        i0 = kF3086ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l8;
      i1 = 3u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1002u;
        i1 = a;
        i2 = kF3086ConstFFFFD8F0;
        i3 = l2;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 3u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3086ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3086ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 == i1;
        a = i0;
        if (i0) {goto running_exit;}
      }
      block_21:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        i0 = LoadU32(ctx, i0 + 36u);
        a = i0;
        i0 = !(i0);
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        if (i0) {goto block_26;}
        i0 = l5;
        i0 = LoadU32(ctx, i0 + 40u);
        l2 = i0;
        i1 = kF3086ConstFFFFFFFE;
        i0 = i0 == i1;
        l3 = i0;
        if (i0) {goto block_26;}
        i0 = kF3086ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l8;
      i1 = 4u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1002u;
        i1 = a;
        i2 = kF3086ConstFFFFD8F0;
        i3 = l2;
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
        i0 = kF3086ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3086ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 == i1;
        a = i0;
        if (i0) {goto running_exit;}
      }
      block_26:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 12u);
        a = i0;
        i0 = !(i0);
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        if (i0) {goto block_31;}
        i0 = b;
        i0 = LoadU32(ctx, i0 + 16u);
        l2 = i0;
        i1 = kF3086ConstFFFFFFFE;
        i0 = i0 == i1;
        l3 = i0;
        if (i0) {goto block_31;}
        i0 = kF3086ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l8;
      i1 = 5u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1002u;
        i1 = a;
        i2 = kF3086ConstFFFFD8F0;
        i3 = l2;
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
        i0 = kF3086ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3086ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 == i1;
        a = i0;
        if (i0) {goto running_exit;}
      }
      block_31:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 4u);
        a = i0;
        i0 = !(i0);
        if (i0) {goto block_8;}
        i0 = b;
        i0 = LoadU32(ctx, i0 + 8u);
        b = i0;
        i1 = kF3086ConstFFFFFFFE;
        i0 = i0 == i1;
        l2 = i0;
        if (i0) {goto block_8;}
        i0 = kF3086ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l8;
      i1 = 6u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1002u;
        i1 = a;
        i2 = kF3086ConstFFFFD8F0;
        i3 = b;
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
        i0 = kF3086ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3086ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_8;}
        goto running_exit;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0 + 12u);
      l2 = i0;
      i0 = !(i0);
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      if (i0) {
        i0 = kF3086ConstFFFFFFFE;
        l3 = i0;
        goto block_51;
      }
      i0 = b;
      i0 = LoadU32(ctx, i0 + 16u);
      l3 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3086ConstFFFFD8F0;
      i2 = l3;
      PushTableField(ctx, i0, i1, i2);
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_50;}
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 7u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1001u;
      i1 = l2;
      i2 = kF3086ConstFFFFD8F0;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l7 = i0;
      i0 = 7u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      l4 = i0;
      if (i0) {goto block_50;}
    }
    block_51:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3086ConstFFFFD8F0;
      i2 = l3;
      PushTableField(ctx, i0, i1, i2);
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      l4 = i0;
      if (i0) {goto block_49;}
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3086ConstFFFFFFFF;
      i0 = QueryValueType(ctx, i0, i1);
      l4 = i0;
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 != i1;
      l6 = i0;
      if (i0) {
        i0 = kF3086ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = kF3086ConstFFFFFFFE;
        ResizeValueStack(ctx, i0, i1);
        i0 = kF3086ConstD015C;
        i0 = LoadU32(ctx, i0);
        l6 = i0;
        i0 = kF3086ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l6;
        i1 = 1u;
        i0 = i0 != i1;
        if (i0) {goto block_57;}
        goto running_exit;
      }
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3086ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 != i1;
      l4 = i0;
      if (i0) {goto block_48;}
      goto running_exit;
      block_57:;
      i0 = l2;
      i0 = !(i0);
      l6 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      if (i0) {goto block_59;}
      i0 = l3;
      i1 = kF3086ConstFFFFFFFE;
      i0 = i0 == i1;
      l6 = i0;
      if (i0) {goto block_59;}
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 8u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1002u;
      i1 = l2;
      i2 = kF3086ConstFFFFD8F0;
      i3 = l3;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 8u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto running_exit;}
    }
    block_59:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = 4u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_39;}
      i0 = b;
      i0 = LoadU32(ctx, i0 + 12u);
      l2 = i0;
      i0 = l5;
      i1 = kF3086ConstFFFFFFFE;
      StoreU32(ctx, i0 + 20u, i1);
      i0 = l5;
      i1 = l2;
      StoreU32(ctx, i0 + 16u, i1);
    }
    i0 = l2;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 16u);
        l3 = i0;
        i0 = kF3086ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = kF3086ConstFFFFD8F0;
        i2 = l3;
        PushTableField(ctx, i0, i1, i2);
        i0 = kF3086ConstD015C;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        i0 = kF3086ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 == i1;
        l3 = i0;
        if (i0) {goto block_46;}
        i0 = kF3086ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l8;
      i1 = 9u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1001u;
        i1 = l2;
        i2 = kF3086ConstFFFFD8F0;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l7 = i0;
        i0 = 9u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l7;
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3086ConstD015C;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        i0 = kF3086ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 == i1;
        l3 = i0;
        if (i0) {goto block_46;}
        i0 = l5;
        i1 = l2;
        StoreU32(ctx, i0 + 20u, i1);
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 16u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l8;
    i1 = 10u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3367u;
      i1 = l2;
      i2 = 0u;
      i3 = 0u;
      i4 = 0u;
      i5 = 0u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l7 = i0;
      i0 = 10u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_47;}
      i0 = l5;
      i0 = LoadU32(ctx, i0 + 16u);
      l2 = i0;
      i0 = !(i0);
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      if (i0) {goto block_71;}
      i0 = l5;
      i0 = LoadU32(ctx, i0 + 20u);
      l4 = i0;
      i1 = kF3086ConstFFFFFFFE;
      i0 = i0 == i1;
      l6 = i0;
      if (i0) {goto block_71;}
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 11u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1002u;
      i1 = l2;
      i2 = kF3086ConstFFFFD8F0;
      i3 = l4;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 11u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto running_exit;}
    }
    block_71:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = kF3086MsgFile;
      i0 = CompareNulTerminatedStrings(ctx, i0, i1);
      l2 = i0;
      if (i0) {goto block_40;}
      i0 = b;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 12u);
      i0 -= i1;
      i1 = 4u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l4 = i0;
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 == i1;
      l6 = i0;
      if (i0) {goto block_45;}
      i0 = b;
      i0 = LoadU32(ctx, i0 + 8u);
      l6 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3086ConstFFFFD8F0;
      i2 = l6;
      PushTableField(ctx, i0, i1, i2);
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 == i1;
      l6 = i0;
      if (i0) {goto block_45;}
      i0 = b;
      i0 = LoadU32(ctx, i0 + 16u);
      l6 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3086ConstFFFFD8F0;
      i2 = l6;
      PushTableField(ctx, i0, i1, i2);
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 == i1;
      l6 = i0;
      if (i0) {goto block_45;}
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = l4;
      i2 = 2u;
      i1 += i2;
      SetStackTop(ctx, i0, i1);
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 == i1;
      l6 = i0;
      if (i0) {goto block_44;}
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
    }
    i0 = l8;
    i1 = 12u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 980u;
      i1 = l2;
      i2 = l4;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 12u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      l4 = i0;
      if (i0) {goto block_44;}
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3086ConstFFFFFFFF;
      i0 = QueryValueType(ctx, i0, i1);
      l4 = i0;
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 != i1;
      l6 = i0;
      if (i0) {
        i0 = kF3086ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = kF3086ConstFFFFFFFE;
        ResizeValueStack(ctx, i0, i1);
        i0 = kF3086ConstD015C;
        i0 = LoadU32(ctx, i0);
        l6 = i0;
        i0 = kF3086ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l6;
        i1 = 1u;
        i0 = i0 != i1;
        l6 = i0;
        if (i0) {goto block_78;}
        goto running_exit;
      }
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3086ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 != i1;
      l3 = i0;
      if (i0) {goto block_43;}
      goto running_exit;
      block_78:;
      i0 = l2;
      if (i0) {
        i0 = kF3086ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = kF3086ConstFFFFFFFD;
        ResizeValueStack(ctx, i0, i1);
        i0 = kF3086ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3086ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto running_exit;}
      }
      i0 = l4;
      i1 = 4u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_40;}
      i0 = b;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = l2;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 12u);
      i0 -= i1;
      i1 = 4u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l3 = i0;
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      l4 = i0;
      if (i0) {goto block_41;}
      i0 = l5;
      i1 = l3;
      i2 = 2u;
      i1 += i2;
      l4 = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l5;
      i1 = l3;
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = b;
      i0 = LoadU32(ctx, i0 + 8u);
      l3 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3086ConstFFFFD8F0;
      i2 = l3;
      PushTableField(ctx, i0, i1, i2);
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_41;}
      i0 = b;
      i0 = LoadU32(ctx, i0 + 16u);
      l3 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3086ConstFFFFD8F0;
      i2 = l3;
      PushTableField(ctx, i0, i1, i2);
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_41;}
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 16u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l8;
    i1 = 13u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3380u;
      i1 = l2;
      i2 = l5;
      i3 = 0u;
      i4 = 0u;
      i5 = 0u;
      i6 = 0u;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 13u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_42;}
      i0 = a;
      i0 = LoadU32(ctx, i0 + 20u);
      l3 = i0;
      i1 = 2752u;
      i0 += i1;
      l2 = i0;
      i0 = l3;
      i1 = 2763u;
      i0 += i1;
      i0 = LoadS8(ctx, i0);
      i1 = kF3086ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l2;
      i1 = l5;
      j1 = LoadU64(ctx, i1 + 16u);
      StoreU64(ctx, i0, j1);
      i0 = l2;
      i1 = l5;
      i1 = LoadU32(ctx, i1 + 24u);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l5;
      i1 = 0u;
      StoreU8(ctx, i0 + 27u, i1);
      i0 = l5;
      i1 = 0u;
      StoreU8(ctx, i0 + 16u, i1);
      i0 = l5;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = !(i0);
      l3 = i0;
      if (i0) {goto block_39;}
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3086ConstFFFFFFFD;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto running_exit;}
      goto block_39;
    }
    block_50:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      l2 = i0;
      goto block_13;
    }
    block_49:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      l4 = i0;
    }
    block_48:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i0 = !(i0);
      if (i0) {goto block_13;}
      i0 = l3;
      i1 = kF3086ConstFFFFFFFE;
      i0 = i0 == i1;
      l4 = i0;
      if (i0) {goto block_13;}
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 14u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1002u;
      i1 = l2;
      i2 = kF3086ConstFFFFD8F0;
      i3 = l3;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 14u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_13;}
      goto running_exit;
    }
    block_47:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = l5;
      i0 = LoadU32(ctx, i0 + 16u);
      l2 = i0;
      i0 = !(i0);
      l3 = i0;
      if (i0) {goto block_13;}
      i0 = l5;
      i0 = LoadU32(ctx, i0 + 20u);
      l3 = i0;
      i1 = kF3086ConstFFFFFFFE;
      i0 = i0 == i1;
      l4 = i0;
      if (i0) {goto block_13;}
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 15u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1002u;
      i1 = l2;
      i2 = kF3086ConstFFFFD8F0;
      i3 = l3;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 15u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_13;}
      goto running_exit;
    }
    block_46:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      l2 = i0;
      goto block_13;
    }
    block_45:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      l2 = i0;
      goto block_13;
    }
    block_44:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      l3 = i0;
    }
    block_43:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i0 = !(i0);
      l3 = i0;
      if (i0) {goto block_13;}
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3086ConstFFFFFFFD;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_13;}
      goto running_exit;
    }
    block_42:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = l5;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = !(i0);
      l3 = i0;
      if (i0) {goto block_13;}
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3086ConstFFFFFFFD;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_13;}
      goto running_exit;
    }
    block_41:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      l2 = i0;
      goto block_13;
    }
    block_40:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = kF3086MsgIndex;
      i0 = CompareNulTerminatedStrings(ctx, i0, i1);
      l2 = i0;
      if (i0) {goto block_39;}
      i0 = b;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 12u);
      i0 -= i1;
      i1 = 4u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l3 = i0;
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      l4 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      if (i0) {goto block_103;}
      i0 = b;
      i0 = LoadU32(ctx, i0 + 8u);
      l4 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3086ConstFFFFD8F0;
      i2 = l4;
      PushTableField(ctx, i0, i1, i2);
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      l4 = i0;
      if (i0) {goto block_103;}
      i0 = b;
      i0 = LoadU32(ctx, i0 + 16u);
      l4 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3086ConstFFFFD8F0;
      i2 = l4;
      PushTableField(ctx, i0, i1, i2);
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      l4 = i0;
      if (i0) {goto block_103;}
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = l3;
      i2 = 2u;
      i1 += i2;
      SetStackTop(ctx, i0, i1);
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      l4 = i0;
      if (i0) {goto block_102;}
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l8;
    i1 = 16u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 980u;
      i1 = l2;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 16u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_102;}
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3086ConstFFFFFFFF;
      i0 = QueryValueType(ctx, i0, i1);
      l3 = i0;
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 != i1;
      l4 = i0;
      if (i0) {
        i0 = kF3086ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = kF3086ConstFFFFFFFE;
        ResizeValueStack(ctx, i0, i1);
        i0 = kF3086ConstD015C;
        i0 = LoadU32(ctx, i0);
        l4 = i0;
        i0 = kF3086ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 1u;
        i0 = i0 != i1;
        l4 = i0;
        if (i0) {goto block_107;}
        goto running_exit;
      }
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3086ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 != i1;
      l3 = i0;
      if (i0) {goto block_101;}
      goto running_exit;
      block_107:;
      i0 = l2;
      if (i0) {
        i0 = kF3086ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = kF3086ConstFFFFFFFD;
        ResizeValueStack(ctx, i0, i1);
        i0 = kF3086ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3086ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto running_exit;}
      }
      i0 = l3;
      i1 = 3u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_39;}
      i0 = b;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = l2;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 12u);
      i0 -= i1;
      i1 = 4u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l3 = i0;
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      l4 = i0;
      if (i0) {goto block_99;}
      i0 = l5;
      i1 = l3;
      i2 = 2u;
      i1 += i2;
      l4 = i1;
      StoreU32(ctx, i0 + 24u, i1);
      i0 = l5;
      i1 = l3;
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 20u, i1);
      i0 = b;
      i0 = LoadU32(ctx, i0 + 8u);
      l3 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3086ConstFFFFD8F0;
      i2 = l3;
      PushTableField(ctx, i0, i1, i2);
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_99;}
      i0 = b;
      i0 = LoadU32(ctx, i0 + 16u);
      l3 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3086ConstFFFFD8F0;
      i2 = l3;
      PushTableField(ctx, i0, i1, i2);
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_99;}
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 16u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l8;
    i1 = 17u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3376u;
      i1 = l2;
      i2 = 0u;
      i3 = 0u;
      i4 = 0u;
      i5 = 0u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l7 = i0;
      i0 = 17u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_100;}
      i0 = a;
      i0 = LoadU32(ctx, i0 + 20u);
      i1 = 2764u;
      i0 += i1;
      i1 = l2;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i0 = LoadU32(ctx, i0 + 16u);
      l2 = i0;
      i0 = !(i0);
      l3 = i0;
      if (i0) {goto block_39;}
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3086ConstFFFFFFFD;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_39;}
      goto running_exit;
    }
    block_103:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      l2 = i0;
      goto block_13;
    }
    block_102:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      l3 = i0;
    }
    block_101:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i0 = !(i0);
      l3 = i0;
      if (i0) {goto block_13;}
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3086ConstFFFFFFFD;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_13;}
      goto running_exit;
    }
    block_100:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = l5;
      i0 = LoadU32(ctx, i0 + 16u);
      l2 = i0;
      i0 = !(i0);
      l3 = i0;
      if (i0) {goto block_13;}
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3086ConstFFFFFFFD;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_13;}
      goto running_exit;
    }
    block_99:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      l2 = i0;
      goto block_13;
    }
    block_39:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l8;
    i1 = 18u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3379u;
      i1 = b;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 18u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3086ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3086ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto loop_14;}
    }
    block_15:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    l2 = i0;
  }
  block_13:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 44u);
    l2 = i0;
    i0 = !(i0);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    if (i0) {goto block_122;}
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 48u);
    l3 = i0;
    i1 = kF3086ConstFFFFFFFE;
    i0 = i0 == i1;
    if (i0) {goto block_122;}
    i0 = kF3086ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 19u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1002u;
    i1 = l2;
    i2 = kF3086ConstFFFFD8F0;
    i3 = l3;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 19u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3086ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3086ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto running_exit;}
  }
  block_122:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 36u);
    l2 = i0;
    i0 = !(i0);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    if (i0) {goto block_127;}
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 40u);
    l5 = i0;
    i1 = kF3086ConstFFFFFFFE;
    i0 = i0 == i1;
    if (i0) {goto block_127;}
    i0 = kF3086ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 20u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1002u;
    i1 = l2;
    i2 = kF3086ConstFFFFD8F0;
    i3 = l5;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 20u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3086ConstD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF3086ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto running_exit;}
  }
  block_127:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i0 = LoadU32(ctx, i0 + 12u);
    l5 = i0;
    i0 = !(i0);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {goto block_132;}
    i0 = b;
    i0 = LoadU32(ctx, i0 + 16u);
    l2 = i0;
    i1 = kF3086ConstFFFFFFFE;
    i0 = i0 == i1;
    if (i0) {goto block_132;}
    i0 = kF3086ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 21u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1002u;
    i1 = l5;
    i2 = kF3086ConstFFFFD8F0;
    i3 = l2;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 21u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3086ConstD015C;
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = kF3086ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l5;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto running_exit;}
  }
  block_132:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i0 = LoadU32(ctx, i0 + 4u);
    l5 = i0;
    i0 = !(i0);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {goto block_138;}
    i0 = b;
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
    i1 = kF3086ConstFFFFFFFE;
    i0 = i0 == i1;
    if (i0) {goto block_138;}
    i0 = kF3086ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l8;
  i1 = 22u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1002u;
    i1 = l5;
    i2 = kF3086ConstFFFFD8F0;
    i3 = b;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 22u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3086ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3086ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_137;}
  }
  block_138:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_137:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto running_exit;}
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    i1 = 80u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
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
  i0 = l8;
  i1 = 23u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 23u;
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
  goto func_exit;
  spill_frame:;
  l7 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l7;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF3086ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l7 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l7;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l7;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l7;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l7;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l7;
  i1 = l5;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l7;
  i1 = l6;
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
}
