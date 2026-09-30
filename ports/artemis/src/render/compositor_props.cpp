// compositor_props.cpp -- the lyprop layer-property dispatcher: it matches the property tag against time, textTween, textClearTween, video and sound and applies the matching layer property write..
//
// Compositor::LyPropDispatch -- the lyprop layer-property dispatcher: it matches the property tag against time, textTween, textClearTween, video and sound and applies the matching layer property write.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 1379679-1379679 (f3088); R1 transcribe-keep.
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
void SetStackTop(EngineContext* ctx, uint32_t stack, uint32_t index);
void AppendTableSlot(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

namespace {

constexpr uint32_t kF3088MsgTime = 0xBB304u;  // time
constexpr uint32_t kF3088MsgTexttween = 0xBB309u;  // textTween
constexpr uint32_t kF3088MsgTextcleartween = 0xBB313u;  // textClearTween
constexpr uint32_t kF3088MsgSound = 0xBB322u;  // sound
constexpr uint32_t kF3088MsgVideo = 0xBB328u;  // video
constexpr uint32_t kF3088ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF3088ConstFFFFD8F0 = 0xFFFFD8F0u;  // 
constexpr uint32_t kF3088ConstFFFFFFFE = 0xFFFFFFFEu;  // 
constexpr uint32_t kF3088ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF3088SaveFrameBytes = 28u;
constexpr uint32_t kF3088ResumeWordBytes = 4u;

}  // namespace

namespace Compositor {
void LyPropDispatch(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF3088SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l5 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 12u);
    l4 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 16u);
    l3 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = l5;
    i0 = LoadU32(ctx, i0 + 24u);
    l8 = i0;
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
    i2 = kF3088ResumeWordBytes;
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
    i1 = 32u;
    i0 -= i1;
    l4 = i0;
    GuestStackSp(ctx) = i0;
    i0 = b;
    i0 = LoadU32(ctx, i0 + 16u);
    l2 = i0;
  }
  i0 = l7;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = 0u;
    i2 = 0u;
    AppendTableSlot(ctx, i0, i1, i2);
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i1 = kF3088ConstFFFFFFFE;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = a;
    i1 = l2;
    StoreU32(ctx, i0, i1);
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = kF3088ConstFFFFFFFF;
    SetStackTop(ctx, i0, i1);
    i0 = kF3088ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    if (i0) {goto block_8;}
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 1u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1001u;
    i1 = l2;
    i2 = kF3088ConstFFFFD8F0;
    i0 = HostOsQuery(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 1u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3088ConstD015C;
    i0 = LoadU32(ctx, i0);
    l6 = i0;
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l6;
    i1 = 1u;
    i0 = i0 == i1;
    l6 = i0;
    if (i0) {goto block_8;}
    i0 = a;
    i1 = l3;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = kF3088ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3088ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 != i1;
    l2 = i0;
    if (i0) {goto block_7;}
    goto block_6;
  }
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    a = i0;
    i0 = HostTempRet0(ctx);
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = kF3088ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3088ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_6;}
    i0 = a;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    i0 = LoadU32(ctx, i0 + 20u);
    l2 = i0;
    i1 = 2772u;
    i0 += i1;
    i0 = LoadU8(ctx, i0);
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
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3088ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l3;
      i0 = LoadU32(ctx, i0 + 8u);
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 12u);
      i0 -= i1;
      i1 = 4u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      l6 = i0;
      i0 = kF3088ConstD015C;
      i0 = LoadU32(ctx, i0);
      l8 = i0;
      i0 = kF3088ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l8;
      i1 = 1u;
      i0 = i0 == i1;
      l8 = i0;
      if (i0) {goto block_26;}
      i0 = l4;
      i1 = a;
      StoreU32(ctx, i0 + 24u, i1);
      i0 = kF3088ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l6;
      i2 = 1u;
      i1 += i2;
      l6 = i1;
      StoreU32(ctx, i0 + 20u, i1);
    }
    i0 = l7;
    i1 = 2u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 969u;
      i1 = l3;
      i2 = kF3088MsgTime;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3088ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3088ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_26;}
      i0 = kF3088ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 16u;
      i0 += i1;
      l3 = i0;
      i0 = l2;
      i1 = 2768u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l7;
    i1 = 3u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3389u;
      i1 = l3;
      i2 = l2;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l5 = i0;
      i0 = 3u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l5;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l2;
    } else {
      i0 = kF3088ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3088ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_27;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 16u);
      l2 = i0;
      if (i0) {
        i0 = kF3088ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = kF3088ConstFFFFFFFE;
        ResizeValueStack(ctx, i0, i1);
        i0 = kF3088ConstD015C;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = kF3088ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l2;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto block_6;}
      }
      i0 = b;
      i0 = LoadU32(ctx, i0 + 20u);
    }
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i1 = 2773u;
    i0 += i1;
    i0 = LoadU8(ctx, i0);
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_24;}
    i0 = l4;
    i1 = 1u;
    StoreU32(ctx, i0 + 12u, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
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
    i0 = kF3088ConstD015C;
    i0 = LoadU32(ctx, i0);
    l6 = i0;
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l6;
    i1 = 1u;
    i0 = i0 != i1;
    l6 = i0;
  }
  i0 = l6;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = a;
      StoreU32(ctx, i0 + 24u, i1);
      i0 = kF3088ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      i2 = 1u;
      i1 += i2;
      l3 = i1;
      StoreU32(ctx, i0 + 20u, i1);
    }
    i0 = l7;
    i1 = 4u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 969u;
      i1 = l2;
      i2 = kF3088MsgTexttween;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 4u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3088ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3088ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_25;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_13;
  }
  block_27:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    b = i0;
    i0 = !(i0);
    l4 = i0;
    if (i0) {goto block_13;}
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF3088ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3088ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_13;}
    goto block_6;
  }
  block_26:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_13;
  }
  block_25:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 12u;
    i0 += i1;
    l3 = i0;
    i0 = l4;
    i1 = 16u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l7;
  i1 = 5u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 3390u;
    i1 = l2;
    i2 = l3;
    i0 = HostOsQuery(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 5u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3088ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_23;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l2 = i0;
    if (i0) {
      i0 = kF3088ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3088ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3088ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3088ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_6;}
    }
    i0 = b;
    i0 = LoadU32(ctx, i0 + 20u);
    l2 = i0;
  }
  block_24:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i1 = 2774u;
    i0 += i1;
    i0 = LoadU8(ctx, i0);
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_21;}
    i0 = l4;
    i1 = 1u;
    StoreU32(ctx, i0 + 12u, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
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
    i0 = kF3088ConstD015C;
    i0 = LoadU32(ctx, i0);
    l6 = i0;
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l6;
    i1 = 1u;
    i0 = i0 != i1;
    l6 = i0;
  }
  i0 = l6;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = a;
      StoreU32(ctx, i0 + 24u, i1);
      i0 = kF3088ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      i2 = 1u;
      i1 += i2;
      l3 = i1;
      StoreU32(ctx, i0 + 20u, i1);
    }
    i0 = l7;
    i1 = 6u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 969u;
      i1 = l2;
      i2 = kF3088MsgTextcleartween;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 6u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3088ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3088ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_22;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_13;
  }
  block_23:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    b = i0;
    i0 = !(i0);
    l4 = i0;
    if (i0) {goto block_13;}
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF3088ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3088ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_13;}
    goto block_6;
  }
  block_22:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 12u;
    i0 += i1;
    l3 = i0;
    i0 = l4;
    i1 = 16u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l7;
  i1 = 7u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 3390u;
    i1 = l2;
    i2 = l3;
    i0 = HostOsQuery(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 7u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3088ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_20;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l2 = i0;
    if (i0) {
      i0 = kF3088ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3088ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3088ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3088ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_6;}
    }
    i0 = b;
    i0 = LoadU32(ctx, i0 + 20u);
    l2 = i0;
  }
  block_21:;
  i0 = ResumePhase(ctx);
  if (i0) {
    i0 = l8;
  } else {
    i0 = l2;
    i1 = 2775u;
    i0 += i1;
    i0 = LoadU8(ctx, i0);
    i0 = !(i0);
    l3 = i0;
    if (i0) {goto block_18;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = l3;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = l3;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 12u);
    i0 -= i1;
    i1 = 4u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    l6 = i0;
    i0 = kF3088ConstD015C;
    i0 = LoadU32(ctx, i0);
    l8 = i0;
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l8;
    i1 = 1u;
    i0 = i0 != i1;
  }
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = a;
      StoreU32(ctx, i0 + 24u, i1);
      i0 = kF3088ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l6;
      i2 = 1u;
      i1 += i2;
      l6 = i1;
      StoreU32(ctx, i0 + 20u, i1);
    }
    i0 = l7;
    i1 = 8u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 969u;
      i1 = l3;
      i2 = kF3088MsgSound;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 8u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3088ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3088ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 != i1;
      l3 = i0;
      if (i0) {goto block_19;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_13;
  }
  block_20:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    b = i0;
    i0 = !(i0);
    l4 = i0;
    if (i0) {goto block_13;}
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF3088ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3088ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_13;}
    goto block_6;
  }
  block_19:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 16u;
    i0 += i1;
    l3 = i0;
    i0 = l2;
    i1 = 2776u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l7;
  i1 = 9u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 3391u;
    i1 = l3;
    i2 = l2;
    i0 = HostOsQuery(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 9u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l5;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3088ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_17;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l2 = i0;
    if (i0) {
      i0 = kF3088ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = kF3088ConstFFFFFFFE;
      ResizeValueStack(ctx, i0, i1);
      i0 = kF3088ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3088ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_6;}
    }
    i0 = b;
    i0 = LoadU32(ctx, i0 + 20u);
    l2 = i0;
  }
  block_18:;
  i0 = ResumePhase(ctx);
  if (i0) {
    i0 = l6;
  } else {
    i0 = l2;
    i1 = 2792u;
    i0 += i1;
    i0 = LoadU8(ctx, i0);
    i0 = !(i0);
    b = i0;
    if (i0) {goto block_15;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = b;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = b;
    i0 = LoadU32(ctx, i0 + 8u);
    i1 = b;
    i1 = LoadU32(ctx, i1 + 12u);
    i0 -= i1;
    i1 = 4u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    l3 = i0;
    i0 = kF3088ConstD015C;
    i0 = LoadU32(ctx, i0);
    l6 = i0;
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l6;
    i1 = 1u;
    i0 = i0 != i1;
  }
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = a;
      StoreU32(ctx, i0 + 24u, i1);
      i0 = kF3088ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l3;
      i2 = 1u;
      i1 += i2;
      StoreU32(ctx, i0 + 20u, i1);
    }
    i0 = l7;
    i1 = 10u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 969u;
      i1 = b;
      i2 = kF3088MsgVideo;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 10u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3088ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3088ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_16;}
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
    goto block_13;
  }
  block_17:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    b = i0;
    i0 = !(i0);
    l4 = i0;
    if (i0) {goto block_13;}
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF3088ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3088ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_13;}
    goto block_6;
  }
  block_16:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 2796u;
    i0 += i1;
    l2 = i0;
    i0 = l4;
    i1 = 16u;
    i0 += i1;
    b = i0;
  }
  i0 = l7;
  i1 = 11u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 3391u;
    i1 = b;
    i2 = l2;
    i0 = HostOsQuery(ctx, i0, i1, i2);
    l5 = i0;
    i0 = 11u;
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
    i0 = kF3088ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    b = i0;
    if (i0) {goto block_14;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    a = i0;
    i0 = !(i0);
    b = i0;
    if (i0) {goto block_15;}
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = kF3088ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3088ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_6;}
  }
  block_15:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 32u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_14:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    l2 = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    b = i0;
    i0 = !(i0);
    l4 = i0;
    if (i0) {goto block_13;}
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = kF3088ConstFFFFFFFE;
    ResizeValueStack(ctx, i0, i1);
    i0 = kF3088ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    b = i0;
    if (i0) {goto block_6;}
  }
  block_13:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = !(i0);
    l4 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    if (i0) {goto block_86;}
    i0 = a;
    i0 = LoadU32(ctx, i0 + 4u);
    a = i0;
    i1 = kF3088ConstFFFFFFFE;
    i0 = i0 == i1;
    if (i0) {goto block_86;}
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l7;
  i1 = 12u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1002u;
    i1 = b;
    i2 = kF3088ConstFFFFD8F0;
    i3 = a;
    HostSyncSlot(ctx, i0, i1, i2, i3);
    i0 = 12u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3088ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF3088ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto block_6;}
  }
  block_86:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    a = i0;
    i0 = HostTempRet0(ctx);
  }
  i0 = l7;
  i1 = 13u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 13u;
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
  l5 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l5;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF3088ResumeWordBytes;
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
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l5;
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l5;
  i1 = l3;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l5;
  i1 = l6;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l5;
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
}
