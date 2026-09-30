// pack_manager_a.cpp -- the file and index tag dispatcher: it resolves the pseudo-object fields, builds the getScriptBlock result through the table helpers and reports the two Japanese getScriptBlock argument errors..
//
// PackManager::FileDispatchA -- the file and index tag dispatcher: it resolves the pseudo-object fields, builds the getScriptBlock result through the table helpers and reports the two Japanese getScriptBlock argument errors.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 1368480-1368480 (f3085); R1 transcribe-keep.
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
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void ResizeValueStack(EngineContext* ctx, uint32_t a0, uint32_t a1);
void PushTableField(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void SetStackTop(EngineContext* ctx, uint32_t stack, uint32_t index);
uint32_t QueryValueType(EngineContext* ctx, uint32_t a0, uint32_t a1);
uint32_t CompareNulTerminatedStrings(EngineContext* ctx, uint32_t left, uint32_t right);
void MakeStringFromBytes(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
void AppendTableSlot(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

namespace {

constexpr uint32_t kF3085MsgFile = 0xBAE4Cu;  // file
constexpr uint32_t kF3085MsgIndex = 0xBB108u;  // index
constexpr uint32_t kF3085ConstBB35E = 0xBB35Eu;  // 
constexpr uint32_t kF3085ConstBB37D = 0xBB37Du;  // 
constexpr uint32_t kF3085ConstBB7BC = 0xBB7BCu;  // 
constexpr uint32_t kF3085ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF3085ConstFFFFD8F0 = 0xFFFFD8F0u;  // 
constexpr uint32_t kF3085ConstFFFFFFFD = 0xFFFFFFFDu;  // 
constexpr uint32_t kF3085ConstFFFFFFFE = 0xFFFFFFFEu;  // 
constexpr uint32_t kF3085ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF3085SaveFrameBytes = 48u;
constexpr uint32_t kF3085ResumeWordBytes = 4u;

}  // namespace

namespace PackManager {
void FileDispatchA(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l13 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u; uint32_t i6 = 0u;
  uint64_t j1 = 0;
      l11 = 0, l12 = 0, l13 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF3085SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l7 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 8u);
    c = i0;
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
    i0 = LoadU32(ctx, i0 + 28u);
    l8 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 32u);
    l10 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 36u);
    l11 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 40u);
    l12 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 44u);
    l13 = i0;
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
    i2 = kF3085ResumeWordBytes;
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
    i0 = GuestStackSp(ctx);
    i1 = 96u;
    i0 -= i1;
    l4 = i0;
    GuestStackSp(ctx) = i0;
    i0 = c;
    i0 = LoadU32(ctx, i0);
    l6 = i0;
    i1 = kF3085ConstFFFFD8F0;
    i2 = c;
    i2 = LoadU32(ctx, i2 + 4u);
    PushTableField(ctx, i0, i1, i2);
    i0 = kF3085ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l6;
    i1 = kF3085ConstFFFFFFFF;
    i0 = QueryValueType(ctx, i0, i1);
    l3 = i0;
    i0 = kF3085ConstD015C;
    i0 = LoadU32(ctx, i0);
    l10 = i0;
    i0 = kF3085ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l10;
    i1 = 1u;
    i0 = i0 != i1;
    l5 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l5;
    if (i0) {
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = kF3085ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 != i1;
      l6 = i0;
      if (i0) {goto block_6;}
      goto running_exit;
    }
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = kF3085ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l6;
    i1 = kF3085ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3085ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3085ConstD015C;
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
    l3 = i0;
  }
  i0 = l3;
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
      i0 = b;
      i1 = 1u;
      i2 = kF3085ConstBB35E;
      MakeStringFromBytes(ctx, i0, i1, i2);
      i0 = 0u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0 + 16u);
      b = i0;
    }
    i0 = l9;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = b;
      i1 = 0u;
      i2 = 0u;
      AppendTableSlot(ctx, i0, i1, i2);
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i1 = kF3085ConstFFFFFFFE;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = a;
      i1 = b;
      StoreU32(ctx, i0, i1);
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = kF3085ConstFFFFFFFF;
      SetStackTop(ctx, i0, i1);
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 == i1;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = c;
      if (i0) {goto block_16;}
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l9;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1001u;
      i1 = b;
      i2 = kF3085ConstFFFFD8F0;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l7 = i0;
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_16;}
      i0 = a;
      i1 = c;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = kF3085ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_10;}
      goto running_exit;
    }
    block_16:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = kF3085ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto running_exit;}
      i0 = a;
      HostOsRelease(ctx, i0);
      TrapUnreachable();
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 0u;
    StoreU32(ctx, i0 + 88u, i1);
    i0 = l4;
    j1 = 0ull;
    StoreU64(ctx, i0 + 80u, j1);
    i0 = kF3085ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 56u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l9;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 3374u;
    i1 = l3;
    i2 = c;
    i0 = HostOsQuery(ctx, i0, i1, i2);
    l7 = i0;
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    l6 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3085ConstD015C;
    i0 = LoadU32(ctx, i0);
    c = i0;
    i0 = kF3085ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = 1u;
    i0 = i0 == i1;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    if (i0) {goto block_26;}
    i0 = l4;
    i1 = kF3085ConstFFFFFFFE;
    StoreU32(ctx, i0 + 48u, i1);
    i0 = l4;
    j1 = 4294967294ull;
    StoreU64(ctx, i0 + 40u, j1);
    i0 = l4;
    j1 = 0ull;
    StoreU64(ctx, i0 + 32u, j1);
    i0 = 0u;
    l10 = i0;
  }
  loop_29: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 32u;
      i0 += i1;
      c = i0;
    }
    i0 = l9;
    i1 = 4u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3375u;
      i1 = l6;
      i2 = c;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l7 = i0;
      i0 = 4u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      c = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_30;}
    }
    i0 = c;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 44u);
        c = i0;
        i0 = !(i0);
        l3 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        if (i0) {goto block_36;}
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 48u);
        l3 = i0;
        i1 = kF3085ConstFFFFFFFE;
        i0 = i0 == i1;
        l5 = i0;
        if (i0) {goto block_36;}
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l9;
      i1 = 5u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1002u;
        i1 = c;
        i2 = kF3085ConstFFFFD8F0;
        i3 = l3;
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
        i0 = kF3085ConstD015C;
        i0 = LoadU32(ctx, i0);
        c = i0;
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = c;
        i1 = 1u;
        i0 = i0 == i1;
        c = i0;
        if (i0) {goto running_exit;}
      }
      block_36:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 36u);
        c = i0;
        i0 = !(i0);
        l3 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        if (i0) {goto block_42;}
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 40u);
        l3 = i0;
        i1 = kF3085ConstFFFFFFFE;
        i0 = i0 == i1;
        l5 = i0;
        if (i0) {goto block_42;}
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l9;
      i1 = 6u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1002u;
        i1 = c;
        i2 = kF3085ConstFFFFD8F0;
        i3 = l3;
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
        i0 = kF3085ConstD015C;
        i0 = LoadU32(ctx, i0);
        c = i0;
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = c;
        i1 = 1u;
        i0 = i0 == i1;
        c = i0;
        if (i0) {goto block_41;}
      }
      block_42:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        i0 = LoadU32(ctx, i0 + 12u);
        c = i0;
        i0 = !(i0);
        l3 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        if (i0) {goto block_48;}
        i0 = l6;
        i0 = LoadU32(ctx, i0 + 16u);
        l3 = i0;
        i1 = kF3085ConstFFFFFFFE;
        i0 = i0 == i1;
        l5 = i0;
        if (i0) {goto block_48;}
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l9;
      i1 = 7u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1002u;
        i1 = c;
        i2 = kF3085ConstFFFFD8F0;
        i3 = l3;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 7u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3085ConstD015C;
        i0 = LoadU32(ctx, i0);
        c = i0;
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = c;
        i1 = 1u;
        i0 = i0 == i1;
        c = i0;
        if (i0) {goto block_47;}
      }
      block_48:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        i0 = LoadU32(ctx, i0 + 4u);
        c = i0;
        i0 = !(i0);
        l3 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l3;
        if (i0) {goto block_54;}
        i0 = l6;
        i0 = LoadU32(ctx, i0 + 8u);
        l6 = i0;
        i1 = kF3085ConstFFFFFFFE;
        i0 = i0 == i1;
        l3 = i0;
        if (i0) {goto block_54;}
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l9;
      i1 = 8u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1002u;
        i1 = c;
        i2 = kF3085ConstFFFFD8F0;
        i3 = l6;
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
        i0 = kF3085ConstD015C;
        i0 = LoadU32(ctx, i0);
        c = i0;
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = c;
        i1 = 1u;
        i0 = i0 == i1;
        c = i0;
        if (i0) {goto block_53;}
      }
      block_54:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 20u);
        l13 = i0;
        i1 = 2688u;
        i0 += i1;
        i0 = LoadU32(ctx, i0);
        c = i0;
        i0 = !(i0);
        if (i0) {goto block_25;}
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 80u);
        i1 = l4;
        i2 = 80u;
        i1 += i2;
        l8 = i1;
        i2 = l4;
        i2 = LoadU8(ctx, i2 + 91u);
        l6 = i2;
        i3 = 24u;
        i2 <<= (i3 & 31);
        i3 = 24u;
        i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
        i3 = 0u;
        i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
        l3 = i2;
        i0 = i2 ? i0 : i1;
        l5 = i0;
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 84u);
        i1 = l6;
        i2 = l3;
        i0 = i2 ? i0 : i1;
        l6 = i0;
      }
      loop_59: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = c;
          i0 = LoadU8(ctx, i0 + 27u);
          l3 = i0;
          i1 = 24u;
          i0 <<= (i1 & 31);
          i1 = 24u;
          i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
          i1 = 0u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          l11 = i0;
          i0 = l6;
          i1 = c;
          i1 = LoadU32(ctx, i1 + 20u);
          i2 = l3;
          i3 = l11;
          i1 = i3 ? i1 : i2;
          l3 = i1;
          i0 = i0 > i1;
          l12 = i0;
          i0 = l3;
          i1 = l6;
          i2 = l12;
          i0 = i2 ? i0 : i1;
          l8 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l8;
          if (i0) {
            i0 = l5;
            i1 = c;
            i2 = 16u;
            i1 += i2;
            l7 = i1;
            i1 = LoadU32(ctx, i1);
            i2 = l7;
            i3 = l11;
            i1 = i3 ? i1 : i2;
            l11 = i1;
            i2 = l8;
            i0 = CompareBytes(ctx, i0, i1, i2);
            l7 = i0;
            i0 = !(i0);
            if (i0) {
              i0 = l3;
              i1 = l6;
              i0 = i0 <= i1;
              l3 = i0;
              if (i0) {goto block_68;}
              goto block_62;
            }
            i0 = l7;
            i1 = kF3085ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            l3 = i0;
            if (i0) {goto block_62;}
            block_68:;
            i0 = l11;
            i1 = l5;
            i2 = l8;
            i0 = CompareBytes(ctx, i0, i1, i2);
            l3 = i0;
            i0 = !(i0);
            l8 = i0;
            if (i0) {goto block_66;}
            i0 = l3;
            i1 = kF3085ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            l3 = i0;
            if (i0) {goto block_65;}
            goto block_63;
          }
          i0 = l3;
          i1 = l6;
          i0 = i0 > i1;
          l3 = i0;
          if (i0) {goto block_62;}
          block_66:;
          i0 = l12;
          if (i0) {goto block_63;}
          block_65:;
          i0 = kF3085ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = l4;
          i2 = 80u;
          i1 += i2;
          StoreU32(ctx, i0 + 32u, i1);
          i0 = l13;
          i1 = 2684u;
          i0 += i1;
          l3 = i0;
          i0 = l4;
          i1 = 80u;
          i0 += i1;
          l6 = i0;
          i0 = l4;
          i1 = 32u;
          i0 += i1;
          l5 = i0;
          i0 = l4;
          i1 = 16u;
          i0 += i1;
          l8 = i0;
          i0 = l4;
          i1 = 56u;
          i0 += i1;
          c = i0;
        }
        i0 = l9;
        i1 = 9u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2709u;
          i1 = c;
          i2 = l3;
          i3 = l6;
          i4 = kF3085ConstBB7BC;
          i5 = l5;
          i6 = l8;
          HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
          i0 = 9u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3085ConstD015C;
          i0 = LoadU32(ctx, i0);
          c = i0;
          i0 = kF3085ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = c;
          i1 = 1u;
          i0 = i0 == i1;
          c = i0;
          if (i0) {goto block_9;}
          i0 = l10;
          i1 = l4;
          i1 = LoadU32(ctx, i1 + 56u);
          c = i1;
          i1 = LoadU32(ctx, i1 + 32u);
          l3 = i1;
          i2 = c;
          i2 = LoadU32(ctx, i2 + 28u);
          i1 -= i2;
          i2 = 2u;
          i1 = (uint32_t)((int32_t)i1 >> (i2 & 31));
          i0 = i0 < i1;
          c = i0;
          if (i0) {goto block_60;}
          goto block_25;
        }
        block_63:;
        i0 = c;
        i1 = c;
        i2 = 4u;
        i1 += i2;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        c = i0;
        block_62:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = c;
          i0 = LoadU32(ctx, i0);
          c = i0;
          if (i0) {goto loop_59;}
          goto block_25;
        }
        block_60:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 20u);
        c = i0;
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = l4;
        i2 = 80u;
        i1 += i2;
        StoreU32(ctx, i0 + 32u, i1);
        i0 = l4;
        i1 = 56u;
        i0 += i1;
        l3 = i0;
        i0 = l4;
        i1 = 80u;
        i0 += i1;
        l6 = i0;
        i0 = l4;
        i1 = 32u;
        i0 += i1;
        l5 = i0;
        i0 = l4;
        i1 = 16u;
        i0 += i1;
        l8 = i0;
        i0 = c;
        i1 = 2684u;
        i0 += i1;
        c = i0;
      }
      i0 = l9;
      i1 = 10u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 2709u;
        i1 = l3;
        i2 = c;
        i3 = l6;
        i4 = kF3085ConstBB7BC;
        i5 = l5;
        i6 = l8;
        HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
        i0 = 10u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3085ConstD015C;
        i0 = LoadU32(ctx, i0);
        c = i0;
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = c;
        i1 = 1u;
        i0 = i0 == i1;
        c = i0;
        if (i0) {goto block_9;}
        i0 = l10;
        i1 = 2u;
        i0 <<= (i1 & 31);
        l3 = i0;
        i1 = l4;
        i1 = LoadU32(ctx, i1 + 56u);
        i1 = LoadU32(ctx, i1 + 28u);
        i0 += i1;
        i0 = LoadU32(ctx, i0);
        c = i0;
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l9;
      i1 = 11u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 3396u;
        i1 = a;
        i2 = b;
        i3 = c;
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
        i0 = kF3085ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 == i1;
        a = i0;
        if (i0) {goto block_9;}
        goto block_24;
      }
      block_53:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto running_exit;}
      block_47:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto running_exit;}
      block_41:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto running_exit;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 12u);
      c = i0;
      i0 = !(i0);
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      if (i0) {
        i0 = kF3085ConstFFFFFFFE;
        l3 = i0;
        goto block_91;
      }
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 16u);
      l3 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = kF3085ConstFFFFD8F0;
      i2 = l3;
      PushTableField(ctx, i0, i1, i2);
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_90;}
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l9;
    i1 = 12u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1001u;
      i1 = c;
      i2 = kF3085ConstFFFFD8F0;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l7 = i0;
      i0 = 12u;
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
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_90;}
    }
    block_91:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = kF3085ConstFFFFD8F0;
      i2 = l3;
      PushTableField(ctx, i0, i1, i2);
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_89;}
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = kF3085ConstFFFFFFFF;
      i0 = QueryValueType(ctx, i0, i1);
      l5 = i0;
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      l8 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l8;
      i1 = 1u;
      i0 = i0 != i1;
      l8 = i0;
      if (i0) {
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = c;
        i1 = kF3085ConstFFFFFFFE;
        ResizeValueStack(ctx, i0, i1);
        i0 = kF3085ConstD015C;
        i0 = LoadU32(ctx, i0);
        l8 = i0;
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l8;
        i1 = 1u;
        i0 = i0 != i1;
        if (i0) {goto block_97;}
        goto running_exit;
      }
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = kF3085ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_88;}
      goto running_exit;
      block_97:;
      i0 = c;
      i0 = !(i0);
      l8 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l8;
      if (i0) {goto block_99;}
      i0 = l3;
      i1 = kF3085ConstFFFFFFFE;
      i0 = i0 == i1;
      l8 = i0;
      if (i0) {goto block_99;}
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l9;
    i1 = 13u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1002u;
      i1 = c;
      i2 = kF3085ConstFFFFD8F0;
      i3 = l3;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 13u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 == i1;
      c = i0;
      if (i0) {goto running_exit;}
    }
    block_99:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      i1 = 4u;
      i0 = i0 != i1;
      c = i0;
      if (i0) {goto block_79;}
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 12u);
      c = i0;
      i0 = l4;
      i1 = kF3085ConstFFFFFFFE;
      StoreU32(ctx, i0 + 20u, i1);
      i0 = l4;
      i1 = c;
      StoreU32(ctx, i0 + 16u, i1);
    }
    i0 = c;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l6;
        i0 = LoadU32(ctx, i0 + 16u);
        l3 = i0;
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = c;
        i1 = kF3085ConstFFFFD8F0;
        i2 = l3;
        PushTableField(ctx, i0, i1, i2);
        i0 = kF3085ConstD015C;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 == i1;
        l3 = i0;
        if (i0) {goto block_86;}
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l9;
      i1 = 14u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1001u;
        i1 = c;
        i2 = kF3085ConstFFFFD8F0;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l7 = i0;
        i0 = 14u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
        i0 = l7;
        c = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3085ConstD015C;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 == i1;
        l3 = i0;
        if (i0) {goto block_86;}
        i0 = l4;
        i1 = c;
        StoreU32(ctx, i0 + 20u, i1);
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 16u;
      i0 += i1;
      c = i0;
    }
    i0 = l9;
    i1 = 15u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3367u;
      i1 = c;
      i2 = 0u;
      i3 = 0u;
      i4 = 0u;
      i5 = 0u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l7 = i0;
      i0 = 15u;
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
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 == i1;
      c = i0;
      if (i0) {goto block_87;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 16u);
      c = i0;
      i0 = !(i0);
      l5 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      if (i0) {goto block_111;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 20u);
      l5 = i0;
      i1 = kF3085ConstFFFFFFFE;
      i0 = i0 == i1;
      l8 = i0;
      if (i0) {goto block_111;}
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l9;
    i1 = 16u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1002u;
      i1 = c;
      i2 = kF3085ConstFFFFD8F0;
      i3 = l5;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 16u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 == i1;
      c = i0;
      if (i0) {goto running_exit;}
    }
    block_111:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = kF3085MsgFile;
      i0 = CompareNulTerminatedStrings(ctx, i0, i1);
      c = i0;
      if (i0) {goto block_80;}
      i0 = l6;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = c;
      i1 = LoadU32(ctx, i1 + 12u);
      i0 -= i1;
      i1 = 4u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l5 = i0;
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      l8 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l8;
      i1 = 1u;
      i0 = i0 == i1;
      l8 = i0;
      if (i0) {goto block_85;}
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 8u);
      l8 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = kF3085ConstFFFFD8F0;
      i2 = l8;
      PushTableField(ctx, i0, i1, i2);
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      l8 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l8;
      i1 = 1u;
      i0 = i0 == i1;
      l8 = i0;
      if (i0) {goto block_85;}
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 16u);
      l8 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = kF3085ConstFFFFD8F0;
      i2 = l8;
      PushTableField(ctx, i0, i1, i2);
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      l8 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l8;
      i1 = 1u;
      i0 = i0 == i1;
      l8 = i0;
      if (i0) {goto block_85;}
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = l5;
      i2 = 2u;
      i1 += i2;
      SetStackTop(ctx, i0, i1);
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      l8 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l8;
      i1 = 1u;
      i0 = i0 == i1;
      l8 = i0;
      if (i0) {goto block_84;}
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 += i1;
      l5 = i0;
    }
    i0 = l9;
    i1 = 17u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 980u;
      i1 = c;
      i2 = l5;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 17u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_84;}
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = kF3085ConstFFFFFFFF;
      i0 = QueryValueType(ctx, i0, i1);
      l5 = i0;
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      l8 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l8;
      i1 = 1u;
      i0 = i0 != i1;
      l8 = i0;
      if (i0) {
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = c;
        i1 = kF3085ConstFFFFFFFE;
        ResizeValueStack(ctx, i0, i1);
        i0 = kF3085ConstD015C;
        i0 = LoadU32(ctx, i0);
        l8 = i0;
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l8;
        i1 = 1u;
        i0 = i0 != i1;
        l8 = i0;
        if (i0) {goto block_118;}
        goto running_exit;
      }
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = kF3085ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_83;}
      goto running_exit;
      block_118:;
      i0 = c;
      if (i0) {
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = c;
        i1 = kF3085ConstFFFFFFFD;
        ResizeValueStack(ctx, i0, i1);
        i0 = kF3085ConstD015C;
        i0 = LoadU32(ctx, i0);
        c = i0;
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = c;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto running_exit;}
      }
      i0 = l5;
      i1 = 4u;
      i0 = i0 != i1;
      c = i0;
      if (i0) {goto block_80;}
      i0 = l6;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = c;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = c;
      i1 = LoadU32(ctx, i1 + 12u);
      i0 -= i1;
      i1 = 4u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l3 = i0;
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_81;}
      i0 = l4;
      i1 = l3;
      i2 = 2u;
      i1 += i2;
      l5 = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l4;
      i1 = l3;
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 8u);
      l3 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = kF3085ConstFFFFD8F0;
      i2 = l3;
      PushTableField(ctx, i0, i1, i2);
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_81;}
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 16u);
      l3 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = kF3085ConstFFFFD8F0;
      i2 = l3;
      PushTableField(ctx, i0, i1, i2);
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 == i1;
      c = i0;
      if (i0) {goto block_81;}
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 16u;
      i0 += i1;
      c = i0;
    }
    i0 = l9;
    i1 = 18u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3380u;
      i1 = c;
      i2 = l4;
      i3 = 0u;
      i4 = 0u;
      i5 = 0u;
      i6 = 0u;
      HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
      i0 = 18u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 == i1;
      c = i0;
      if (i0) {goto block_82;}
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 91u);
      i1 = kF3085ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 80u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l4;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 24u);
      StoreU32(ctx, i0 + 88u, i1);
      i0 = l4;
      i1 = l4;
      j1 = LoadU64(ctx, i1 + 16u);
      StoreU64(ctx, i0 + 80u, j1);
      i0 = l4;
      i1 = 0u;
      StoreU8(ctx, i0 + 16u, i1);
      i0 = l4;
      i1 = 0u;
      StoreU8(ctx, i0 + 27u, i1);
      i0 = l4;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = !(i0);
      l3 = i0;
      if (i0) {goto block_79;}
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = kF3085ConstFFFFFFFD;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 == i1;
      c = i0;
      if (i0) {goto running_exit;}
      goto block_79;
    }
    block_90:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
      goto block_28;
    }
    block_89:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
    }
    block_88:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = c;
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_28;}
      i0 = l3;
      i1 = kF3085ConstFFFFFFFE;
      i0 = i0 == i1;
      b = i0;
      if (i0) {goto block_28;}
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l9;
    i1 = 19u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1002u;
      i1 = c;
      i2 = kF3085ConstFFFFD8F0;
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
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_28;}
      goto running_exit;
    }
    block_87:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 16u);
      b = i0;
      i0 = !(i0);
      c = i0;
      if (i0) {goto block_28;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 20u);
      c = i0;
      i1 = kF3085ConstFFFFFFFE;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_28;}
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l9;
    i1 = 20u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1002u;
      i1 = b;
      i2 = kF3085ConstFFFFD8F0;
      i3 = c;
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
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_28;}
      goto running_exit;
    }
    block_86:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
      goto block_28;
    }
    block_85:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
      goto block_28;
    }
    block_84:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
    }
    block_83:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = c;
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_28;}
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = kF3085ConstFFFFFFFD;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_28;}
      goto running_exit;
    }
    block_82:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = l4;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = !(i0);
      c = i0;
      if (i0) {goto block_28;}
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = kF3085ConstFFFFFFFD;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_28;}
      goto running_exit;
    }
    block_81:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
      goto block_28;
    }
    block_80:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i1 = kF3085MsgIndex;
      i0 = CompareNulTerminatedStrings(ctx, i0, i1);
      c = i0;
      if (i0) {goto block_79;}
      i0 = l6;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = c;
      i1 = LoadU32(ctx, i1 + 12u);
      i0 -= i1;
      i1 = 4u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l3 = i0;
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l5;
      if (i0) {goto block_143;}
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 8u);
      l5 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = kF3085ConstFFFFD8F0;
      i2 = l5;
      PushTableField(ctx, i0, i1, i2);
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_143;}
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 16u);
      l5 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = kF3085ConstFFFFD8F0;
      i2 = l5;
      PushTableField(ctx, i0, i1, i2);
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_143;}
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = l3;
      i2 = 2u;
      i1 += i2;
      SetStackTop(ctx, i0, i1);
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_142;}
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l9;
    i1 = 21u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 980u;
      i1 = c;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 21u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_142;}
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = kF3085ConstFFFFFFFF;
      i0 = QueryValueType(ctx, i0, i1);
      l3 = i0;
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      l5 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l5;
      i1 = 1u;
      i0 = i0 != i1;
      l5 = i0;
      if (i0) {
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = c;
        i1 = kF3085ConstFFFFFFFE;
        ResizeValueStack(ctx, i0, i1);
        i0 = kF3085ConstD015C;
        i0 = LoadU32(ctx, i0);
        l5 = i0;
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l5;
        i1 = 1u;
        i0 = i0 != i1;
        l5 = i0;
        if (i0) {goto block_147;}
        goto running_exit;
      }
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = kF3085ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_141;}
      goto running_exit;
      block_147:;
      i0 = c;
      if (i0) {
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = c;
        i1 = kF3085ConstFFFFFFFD;
        ResizeValueStack(ctx, i0, i1);
        i0 = kF3085ConstD015C;
        i0 = LoadU32(ctx, i0);
        c = i0;
        i0 = kF3085ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = c;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto running_exit;}
      }
      i0 = l3;
      i1 = 3u;
      i0 = i0 != i1;
      c = i0;
      if (i0) {goto block_79;}
      i0 = l6;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = c;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = c;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = c;
      i1 = LoadU32(ctx, i1 + 12u);
      i0 -= i1;
      i1 = 4u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l3 = i0;
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      l10 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l10;
      i1 = 1u;
      i0 = i0 == i1;
      l5 = i0;
      if (i0) {goto block_139;}
      i0 = l4;
      i1 = l3;
      i2 = 2u;
      i1 += i2;
      l5 = i1;
      StoreU32(ctx, i0 + 24u, i1);
      i0 = l4;
      i1 = l3;
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 20u, i1);
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 8u);
      l3 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = kF3085ConstFFFFD8F0;
      i2 = l3;
      PushTableField(ctx, i0, i1, i2);
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_139;}
      i0 = l6;
      i0 = LoadU32(ctx, i0 + 16u);
      l3 = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = kF3085ConstFFFFD8F0;
      i2 = l3;
      PushTableField(ctx, i0, i1, i2);
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 == i1;
      c = i0;
      if (i0) {goto block_139;}
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 16u;
      i0 += i1;
      c = i0;
    }
    i0 = l9;
    i1 = 22u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3376u;
      i1 = c;
      i2 = 0u;
      i3 = 0u;
      i4 = 0u;
      i5 = 0u;
      i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
      l7 = i0;
      i0 = 22u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l10 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 == i1;
      c = i0;
      if (i0) {goto block_140;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 16u);
      c = i0;
      i0 = !(i0);
      l3 = i0;
      if (i0) {goto block_79;}
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = kF3085ConstFFFFFFFD;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 != i1;
      c = i0;
      if (i0) {goto block_79;}
      goto running_exit;
    }
    block_143:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
      goto block_28;
    }
    block_142:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
    }
    block_141:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = c;
      i0 = !(i0);
      b = i0;
      if (i0) {goto block_28;}
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = kF3085ConstFFFFFFFD;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_28;}
      goto running_exit;
    }
    block_140:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 16u);
      b = i0;
      i0 = !(i0);
      c = i0;
      if (i0) {goto block_28;}
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = kF3085ConstFFFFFFFD;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_28;}
      goto running_exit;
    }
    block_139:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      a = i0;
      i0 = HostTempRet0(ctx);
      b = i0;
      goto block_28;
    }
    block_79:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l9;
    i1 = 23u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3379u;
      i1 = l6;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 23u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3085ConstD015C;
      i0 = LoadU32(ctx, i0);
      c = i0;
      i0 = kF3085ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = c;
      i1 = 1u;
      i0 = i0 != i1;
      c = i0;
      if (i0) {goto loop_29;}
    }
    block_30:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
  }
  block_28:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 44u);
    b = i0;
    i0 = !(i0);
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    if (i0) {goto block_162;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 48u);
    c = i0;
    i1 = kF3085ConstFFFFFFFE;
    i0 = i0 == i1;
    if (i0) {goto block_162;}
    i0 = kF3085ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l9;
  i1 = 24u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1002u;
    i1 = b;
    i2 = kF3085ConstFFFFD8F0;
    i3 = c;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 24u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3085ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3085ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    b = i0;
    if (i0) {goto running_exit;}
  }
  block_162:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 36u);
    b = i0;
    i0 = !(i0);
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    if (i0) {goto block_167;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 40u);
    c = i0;
    i1 = kF3085ConstFFFFFFFE;
    i0 = i0 == i1;
    if (i0) {goto block_167;}
    i0 = kF3085ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l9;
  i1 = 25u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1002u;
    i1 = b;
    i2 = kF3085ConstFFFFD8F0;
    i3 = c;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 25u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3085ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3085ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    b = i0;
    if (i0) {goto running_exit;}
  }
  block_167:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 12u);
    b = i0;
    i0 = !(i0);
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    if (i0) {goto block_173;}
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 16u);
    c = i0;
    i1 = kF3085ConstFFFFFFFE;
    i0 = i0 == i1;
    if (i0) {goto block_173;}
    i0 = kF3085ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l9;
  i1 = 26u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1002u;
    i1 = b;
    i2 = kF3085ConstFFFFD8F0;
    i3 = c;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 26u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3085ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3085ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    b = i0;
    if (i0) {goto block_172;}
  }
  block_173:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 80u;
    i0 += i1;
    c = i0;
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 4u);
    b = i0;
    i0 = !(i0);
    if (i0) {goto block_8;}
    i0 = l6;
    i0 = LoadU32(ctx, i0 + 8u);
    l6 = i0;
    i1 = kF3085ConstFFFFFFFE;
    i0 = i0 == i1;
    if (i0) {goto block_8;}
    i0 = kF3085ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l9;
  i1 = 27u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1002u;
    i1 = b;
    i2 = kF3085ConstFFFFD8F0;
    i3 = l6;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 27u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3085ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3085ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_8;}
    goto running_exit;
  }
  block_172:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto running_exit;}
  block_26:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = l4;
    i1 = 80u;
    i0 += i1;
    c = i0;
    i0 = HostTempRet0(ctx);
    goto block_8;
  }
  block_25:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3085ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l9;
  i1 = 28u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 3370u;
    i1 = b;
    i2 = 1u;
    i3 = kF3085ConstBB37D;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 28u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3085ConstD015C;
    i0 = LoadU32(ctx, i0);
    c = i0;
    i0 = kF3085ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = 1u;
    i0 = i0 == i1;
    c = i0;
    if (i0) {goto block_9;}
    i0 = b;
    i0 = LoadU32(ctx, i0 + 16u);
    b = i0;
    i0 = kF3085ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l9;
  i1 = 29u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1007u;
    i1 = b;
    i2 = 0u;
    i3 = 0u;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 29u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3085ConstD015C;
    i0 = LoadU32(ctx, i0);
    c = i0;
    i0 = kF3085ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = 1u;
    i0 = i0 == i1;
    c = i0;
    if (i0) {goto block_9;}
    i0 = a;
    i1 = kF3085ConstFFFFFFFE;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = a;
    i1 = b;
    StoreU32(ctx, i0, i1);
    i0 = kF3085ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF3085ConstFFFFFFFF;
    SetStackTop(ctx, i0, i1);
    i0 = kF3085ConstD015C;
    i0 = LoadU32(ctx, i0);
    c = i0;
    i0 = kF3085ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = c;
    i1 = 1u;
    i0 = i0 == i1;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    if (i0) {goto block_186;}
    i0 = kF3085ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l9;
  i1 = 30u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1001u;
    i1 = b;
    i2 = kF3085ConstFFFFD8F0;
    i0 = HostOsQuery(ctx, i0, i1, i2);
    l7 = i0;
    i0 = 30u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    c = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3085ConstD015C;
    i0 = LoadU32(ctx, i0);
    l6 = i0;
    i0 = kF3085ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l6;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_186;}
    i0 = a;
    i1 = c;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = kF3085ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF3085ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3085ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF3085ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 != i1;
    a = i0;
    if (i0) {goto block_24;}
    goto running_exit;
  }
  block_186:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = kF3085ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF3085ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3085ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3085ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 80u;
    i0 += i1;
    c = i0;
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto block_8;}
    goto running_exit;
  }
  block_24:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 91u);
    i1 = kF3085ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_10;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 80u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_10:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 96u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_9:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i1 = 80u;
    i0 += i1;
    c = i0;
  }
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = c;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF3085ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 80u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = a;
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
  i0 = l9;
  i1 = 31u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 31u;
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
  i2 = kF3085ResumeWordBytes;
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
  i1 = c;
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
  i0 = l7;
  i1 = l8;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l7;
  i1 = l10;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = l7;
  i1 = l11;
  StoreU32(ctx, i0 + 36u, i1);
  i0 = l7;
  i1 = l12;
  StoreU32(ctx, i0 + 40u, i1);
  i0 = l7;
  i1 = l13;
  StoreU32(ctx, i0 + 44u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 48u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
}
