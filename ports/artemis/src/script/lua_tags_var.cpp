// lua_tags_var.cpp -- the var tag dispatch: one suspendable step that wraps and reflects a script value through the object layer..
//
// LuaEngine::VarDispatch -- the var tag dispatch: one suspendable step that wraps and reflects a script value through the object layer.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 1404642-1404642 (f3112); R1 transcribe-keep.
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

namespace {

constexpr uint32_t kF3112Const89168 = 0x89168u;  // 
constexpr uint32_t kF3112Const89C90 = 0x89C90u;  // 
constexpr uint32_t kF3112ConstBADFC = 0xBADFCu;  // 
constexpr uint32_t kF3112MsgClua = 0xBAE04u;  // CLua
constexpr uint32_t kF3112MsgInclude = 0xBAE09u;  // include
constexpr uint32_t kF3112MsgDebug = 0xBAE11u;  // debug
constexpr uint32_t kF3112MsgDebugskip = 0xBAE17u;  // debugSkip
constexpr uint32_t kF3112MsgSethttpresponse = 0xBAE21u;  // setHttpResponse
constexpr uint32_t kF3112ConstBAE31 = 0xBAE31u;  // 
constexpr uint32_t kF3112MsgRandom = 0xBAE35u;  // random
constexpr uint32_t kF3112MsgConvertencoding = 0xBAE3Cu;  // convertEncoding
constexpr uint32_t kF3112MsgFile = 0xBAE4Cu;  // file
constexpr uint32_t kF3112MsgIsfileexists = 0xBAE51u;  // isFileExists
constexpr uint32_t kF3112MsgSetmagicpath = 0xBAE5Eu;  // setMagicPath
constexpr uint32_t kF3112ConstBAE6B = 0xBAE6Bu;  // 
constexpr uint32_t kF3112MsgEnqueuetag = 0xBAE6Fu;  // enqueueTag
constexpr uint32_t kF3112ConstBAE7A = 0xBAE7Au;  // 
constexpr uint32_t kF3112MsgSettagfilter = 0xBAE7Eu;  // setTagFilter
constexpr uint32_t kF3112MsgSeteventfilter = 0xBAE8Bu;  // setEventFilter
constexpr uint32_t kF3112MsgSetlogfilter = 0xBAE9Au;  // setLogFilter
constexpr uint32_t kF3112MsgGetscriptstatus = 0xBAEA7u;  // getScriptStatus
constexpr uint32_t kF3112MsgSetscriptstatus = 0xBAEB7u;  // setScriptStatus
constexpr uint32_t kF3112MsgGetscriptwaitreason = 0xBAEC7u;  // getScriptWaitReason
constexpr uint32_t kF3112MsgGetscriptstack = 0xBAEDBu;  // getScriptStack
constexpr uint32_t kF3112MsgSetscriptstack = 0xBAEEAu;  // setScriptStack
constexpr uint32_t kF3112MsgGetscriptblock = 0xBAEF9u;  // getScriptBlock
constexpr uint32_t kF3112MsgRestorefontcache = 0xBAF08u;  // restoreFontCache
constexpr uint32_t kF3112MsgSeteventhandler = 0xBAF19u;  // setEventHandler
constexpr uint32_t kF3112MsgBindsurface = 0xBAF29u;  // bindSurface
constexpr uint32_t kF3112MsgBindsurfaceasync = 0xBAF35u;  // bindSurfaceAsync
constexpr uint32_t kF3112MsgUnbindsurface = 0xBAF46u;  // unbindSurface
constexpr uint32_t kF3112MsgClearsurfaceloadqueue = 0xBAF54u;  // clearSurfaceLoadQueue
constexpr uint32_t kF3112MsgIsloadingsurface = 0xBAF6Au;  // isLoadingSurface
constexpr uint32_t kF3112MsgLoadpngcomments = 0xBAF7Bu;  // loadPngComments
constexpr uint32_t kF3112MsgIspush = 0xBAF8Bu;  // isPush
constexpr uint32_t kF3112MsgIsdown = 0xBAF92u;  // isDown
constexpr uint32_t kF3112MsgIsdownedge = 0xBAF99u;  // isDownEdge
constexpr uint32_t kF3112MsgIsupedge = 0xBAFA4u;  // isUpEdge
constexpr uint32_t kF3112MsgIsdecide = 0xBAFADu;  // isDecide
constexpr uint32_t kF3112MsgGetmousepoint = 0xBAFB6u;  // getMousePoint
constexpr uint32_t kF3112MsgGettouchcount = 0xBAFC4u;  // getTouchCount
constexpr uint32_t kF3112MsgGettouchpoint = 0xBAFD2u;  // getTouchPoint
constexpr uint32_t kF3112MsgSetusemultitouch = 0xBAFE0u;  // setUseMultiTouch
constexpr uint32_t kF3112MsgSetusetouchhold = 0xBAFF1u;  // setUseTouchHold
constexpr uint32_t kF3112MsgSetflicksensitivity = 0xBB001u;  // setFlickSensitivity
constexpr uint32_t kF3112MsgOverridekey = 0xBB015u;  // overrideKey
constexpr uint32_t kF3112MsgCallshellexecute = 0xBB021u;  // callShellExecute
constexpr uint32_t kF3112MsgWriteclipboard = 0xBB032u;  // writeClipboard
constexpr uint32_t kF3112MsgDumpmemorystatus = 0xBB041u;  // dumpMemoryStatus
constexpr uint32_t kF3112ConstBB764 = 0xBB764u;  // 
constexpr uint32_t kF3112ConstBBB0C = 0xBBB0Cu;  // 
constexpr uint32_t kF3112MsgTostring = 0xBBC77u;  // __tostring
constexpr uint32_t kF3112ConstBBDD0 = 0xBBDD0u;  // 
constexpr uint32_t kF3112ConstBBF00 = 0xBBF00u;  // 
constexpr uint32_t kF3112ConstBC01C = 0xBC01Cu;  // 
constexpr uint32_t kF3112ConstBC144 = 0xBC144u;  // 
constexpr uint32_t kF3112ConstCD5D4 = 0xCD5D4u;  // 
constexpr uint32_t kF3112ConstCF768 = 0xCF768u;  // 
constexpr uint32_t kF3112ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF3112ConstFFFFD8F0 = 0xFFFFD8F0u;  // 
constexpr uint32_t kF3112ConstFFFFFFFE = 0xFFFFFFFEu;  // 
constexpr uint32_t kF3112ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF3112SaveFrameBytes = 24u;
constexpr uint32_t kF3112ResumeWordBytes = 4u;

}  // namespace

namespace LuaEngine {
uint32_t VarDispatch(EngineContext* ctx, uint32_t a, uint32_t b) {
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
    i2 = kF3112SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l4 = i0;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 16u);
    l6 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 20u);
    l7 = i0;
    i0 = l4;
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
    i2 = kF3112ResumeWordBytes;
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
    i0 = GuestStackSp(ctx);
    l3 = i0;
    i1 = 48u;
    i0 -= i1;
    l6 = i0;
    GuestStackSp(ctx) = i0;
    i0 = a;
    j1 = 0ull;
    StoreU64(ctx, i0 + 4u, j1);
    i0 = a;
    i1 = kF3112ConstBADFC;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 0u;
    StoreU32(ctx, i0 + 12u, i1);
    i0 = kF3112ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l5;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 3306u;
    i0 = HostInvokeI(ctx, i0);
    l4 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l4;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3112ConstD015C;
    i0 = LoadU32(ctx, i0);
    l7 = i0;
    i0 = kF3112ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l7;
    i1 = 1u;
    i0 = i0 != i1;
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
      j1 = 18446744065119617024ull;
      StoreU64(ctx, i0 + 40u, j1);
      i0 = a;
      j1 = 18446744065119617024ull;
      StoreU64(ctx, i0 + 32u, j1);
      i0 = a;
      j1 = 18446744065119617024ull;
      StoreU64(ctx, i0 + 24u, j1);
      i0 = a;
      i1 = b;
      StoreU32(ctx, i0 + 20u, i1);
      i0 = a;
      i1 = l2;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = kF3112ConstD015C;
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
      i0 = 3307u;
      i1 = l2;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 1u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3112ConstD015C;
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
      i0 = a;
      i0 = LoadU32(ctx, i0 + 16u);
      b = i0;
      i0 = kF3112ConstD015C;
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
      i0 = 3308u;
      i1 = b;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 2u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
      if (i0) {goto block_21;}
      i0 = a;
      i0 = LoadU32(ctx, i0 + 16u);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 24u;
      i0 += i1;
      b = i0;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = b;
      i1 = l2;
      StoreU32(ctx, i0, i1);
      i0 = b;
      l7 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
      if (i0) {goto block_20;}
      i0 = kF3112ConstD015C;
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
      i0 = 3310u;
      i1 = l6;
      i2 = kF3112MsgClua;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l4 = i0;
      i0 = 3u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_19;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = l6;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      if (i0) {goto block_29;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_29;}
      i0 = l6;
      i1 = kF3112ConstBB764;
      StoreU32(ctx, i0 + 40u, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstCF768;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = l6;
      i1 = kF3112Const89168;
      StoreU32(ctx, i0 + 32u, i1);
      i0 = kF3112ConstCD5D4;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = b;
      i0 = LoadU32(ctx, i0 + 4u);
      i1 = l6;
      i1 = LoadU32(ctx, i1 + 40u);
      StoreU32(ctx, i0 + 36u, i1);
      i0 = b;
      i0 = LoadU32(ctx, i0 + 4u);
      l4 = i0;
      i1 = l2;
      StoreU32(ctx, i0 + 40u, i1);
      i0 = l4;
      i1 = l6;
      i1 = LoadU32(ctx, i1 + 32u);
      l2 = i1;
      StoreU32(ctx, i0 + 48u, i1);
      i0 = b;
      i0 = LoadU32(ctx, i0 + 4u);
      i1 = l3;
      StoreU32(ctx, i0 + 44u, i1);
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_27;}
      block_29:;
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      l3 = i0;
      i0 = l6;
      i0 = LoadU32(ctx, i0);
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
        i0 = l6;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 4u);
        l3 = i0;
      }
      i0 = l5;
      i1 = 4u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l6;
        i1 = l3;
        CallTableT1(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 4u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto block_14;}
    block_27:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 40u;
      i0 += i1;
      l3 = i0;
      i0 = l6;
      i1 = 32u;
      i0 += i1;
      b = i0;
    }
    i0 = l5;
    i1 = 5u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3314u;
      i1 = l6;
      i2 = l3;
      i3 = b;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l4 = i0;
      i0 = 5u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_18;}
      i0 = kF3112ConstD015C;
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
      i0 = 1032u;
      i1 = 20u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 6u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 3316u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgTostring;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBB0C;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 7u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 7u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 8u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 8u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3318u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgInclude;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBDD0;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 9u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 9u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 10u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 10u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3319u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgDebug;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBDD0;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 11u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 11u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 12u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 12u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3320u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgDebugskip;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBDD0;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 13u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 13u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 14u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 14u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3321u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgSethttpresponse;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBDD0;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 15u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 15u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 16u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 16u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3322u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112ConstBAE31;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBF00;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 17u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
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
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 18u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 18u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3323u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgRandom;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBF00;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 19u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 19u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 20u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 20u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3324u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgConvertencoding;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBC01C;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 21u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
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
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 22u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 22u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3325u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgFile;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBC01C;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 23u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 23u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 24u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 24u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3326u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgIsfileexists;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBC01C;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 25u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 25u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 26u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 26u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3327u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgSetmagicpath;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBDD0;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 27u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 27u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 28u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 28u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3328u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112ConstBAE6B;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBDD0;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 29u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 29u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 30u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 30u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3329u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgEnqueuetag;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBDD0;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 31u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 31u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 32u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 32u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3330u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112ConstBAE7A;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBC01C;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 33u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 33u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 34u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 34u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3331u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgSettagfilter;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBDD0;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 35u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 35u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 36u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 36u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3332u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgSeteventfilter;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBDD0;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 37u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 37u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 38u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 38u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3333u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgSetlogfilter;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBDD0;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 39u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 39u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 40u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 40u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3334u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgGetscriptstatus;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBF00;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 41u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 41u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 42u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 42u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3335u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgSetscriptstatus;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBDD0;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 43u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 43u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 44u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 44u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3336u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgGetscriptwaitreason;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBF00;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 45u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 45u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 46u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 46u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3337u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgGetscriptstack;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBF00;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 47u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 47u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 48u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 48u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3338u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgSetscriptstack;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBDD0;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 49u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 49u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 50u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 50u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3339u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgGetscriptblock;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBC01C;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 51u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 51u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 52u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 52u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3340u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgRestorefontcache;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBDD0;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 53u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 53u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 54u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 54u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3341u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgSeteventhandler;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBDD0;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 55u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 55u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 56u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 56u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3342u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgBindsurface;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBDD0;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 57u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 57u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 58u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 58u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3343u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgBindsurfaceasync;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBDD0;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 59u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 59u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 60u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 60u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3344u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgUnbindsurface;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBDD0;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 61u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 61u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 62u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 62u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3345u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgClearsurfaceloadqueue;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBC144;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 63u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 63u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 64u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 64u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3346u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgIsloadingsurface;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBC01C;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 65u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 65u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 66u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 66u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3347u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgLoadpngcomments;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBC01C;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 67u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 67u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 68u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 68u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3348u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgIspush;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBC01C;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 69u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 69u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 70u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 70u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3349u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgIsdown;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBC01C;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 71u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 71u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 72u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 72u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3350u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgIsdownedge;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBC01C;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 73u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 73u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 74u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 74u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3351u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgIsupedge;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBC01C;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 75u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 75u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 76u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 76u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3352u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgIsdecide;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBC01C;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 77u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 77u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 78u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 78u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3353u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgGetmousepoint;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBF00;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 79u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 79u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 80u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 80u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3354u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgGettouchcount;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBF00;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 81u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 81u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 82u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 82u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3355u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgGettouchpoint;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBF00;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 83u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 83u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 84u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 84u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3356u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgSetusemultitouch;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBDD0;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 85u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 85u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 86u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 86u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3357u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgSetusetouchhold;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBDD0;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 87u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 87u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 88u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 88u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3358u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgSetflicksensitivity;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBDD0;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 89u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 89u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 90u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 90u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3359u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgOverridekey;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBDD0;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 91u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 91u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 92u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 92u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3360u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgCallshellexecute;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBC01C;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 93u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 93u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 94u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 94u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3361u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgWriteclipboard;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBDD0;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 95u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 95u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 96u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1032u;
      i1 = 24u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 96u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l2;
      i1 = kF3112Const89C90;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      l3 = i0;
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l4 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {
        i0 = HostRet0(ctx);
        b = i0;
        i0 = HostTempRet0(ctx);
        l3 = i0;
        i0 = l2;
        DeallocateRawBlock(ctx, i0);
        goto block_15;
      }
      i0 = l2;
      i1 = 0u;
      StoreU32(ctx, i0 + 16u, i1);
      i0 = l2;
      i1 = 3362u;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = l2;
      i1 = kF3112MsgDumpmemorystatus;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l2;
      i1 = kF3112ConstBBF00;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 97u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3317u;
      i1 = b;
      i2 = l3;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 97u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 16u;
      i0 += i1;
      l2 = i0;
      i1 = b;
      i1 = LoadU32(ctx, i1);
      l3 = i1;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
      if (i0) {goto block_17;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 98u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3364u;
      i1 = l7;
      i2 = l2;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 98u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 == i1;
      b = i0;
      if (i0) {goto block_16;}
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      b = i0;
    }
    i0 = b;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 4u);
        l3 = i0;
      }
      i0 = l5;
      i1 = 99u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = b;
        i1 = l3;
        CallTableT1(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 99u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i0 = LoadU32(ctx, i0);
      b = i0;
    }
    i0 = b;
    i1 = ResumePhase(ctx);
    i2 = 2u;
    i1 = i1 == i2;
    i0 |= i1;
    if (i0) {
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 4u);
        l3 = i0;
      }
      i0 = l5;
      i1 = 100u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = b;
        i1 = l3;
        CallTableT1(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 100u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 16u);
      b = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 101u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3365u;
      i1 = b;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l4 = i0;
      i0 = 101u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l4;
      b = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
      b = i0;
      if (i0) {goto block_6;}
    }
    block_21:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      l3 = i0;
      goto block_14;
    }
    block_20:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      l3 = i0;
      goto block_14;
    }
    block_19:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      l3 = i0;
      goto block_14;
    }
    block_18:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      l3 = i0;
      goto block_15;
    }
    block_17:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      l3 = i0;
      goto block_15;
    }
    block_16:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      l3 = i0;
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
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
        i0 = l2;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 4u);
        l3 = i0;
      }
      i0 = l5;
      i1 = 102u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l3;
        CallTableT1(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 102u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    block_15:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      i0 = LoadU32(ctx, i0);
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
        i0 = l6;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 4u);
        l3 = i0;
      }
      i0 = l5;
      i1 = 103u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l6;
        i1 = l3;
        CallTableT1(ctx, i1, i0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        i0 = 103u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
    }
    block_14:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 40u);
      l6 = i0;
      i0 = !(i0);
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      if (i0) {goto block_295;}
      i0 = a;
      i0 = LoadU32(ctx, i0 + 44u);
      l2 = i0;
      i1 = kF3112ConstFFFFFFFE;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_295;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 104u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1002u;
      i1 = l6;
      i2 = kF3112ConstFFFFD8F0;
      i3 = l2;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 104u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 == i1;
      l3 = i0;
      if (i0) {goto block_5;}
    }
    block_295:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 32u);
      l6 = i0;
      i0 = !(i0);
      l3 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      if (i0) {goto block_300;}
      i0 = a;
      i0 = LoadU32(ctx, i0 + 36u);
      l2 = i0;
      i1 = kF3112ConstFFFFFFFE;
      i0 = i0 == i1;
      if (i0) {goto block_300;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 105u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1002u;
      i1 = l6;
      i2 = kF3112ConstFFFFD8F0;
      i3 = l2;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 105u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_5;}
    }
    block_300:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 24u);
      l6 = i0;
      i0 = !(i0);
      if (i0) {goto block_7;}
      i0 = a;
      i0 = LoadU32(ctx, i0 + 28u);
      l2 = i0;
      i1 = kF3112ConstFFFFFFFE;
      i0 = i0 == i1;
      if (i0) {goto block_7;}
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 106u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1002u;
      i1 = l6;
      i2 = kF3112ConstFFFFD8F0;
      i3 = l2;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 106u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3112ConstD015C;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = kF3112ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 != i1;
      if (i0) {goto block_7;}
      goto block_5;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadS8(ctx, i0 + 15u);
    i1 = kF3112ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = a;
      i0 = LoadU32(ctx, i0 + 4u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = b;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i1 = 48u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    i0 = a;
    goto func_exit;
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    a = i0;
    i0 = HostTempRet0(ctx);
  }
  i0 = l5;
  i1 = 107u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 107u;
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
  l4 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l4;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF3112ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l4 = i0;
  i1 = a;
  StoreU32(ctx, i0, i1);
  i0 = l4;
  i1 = b;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l4;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l4;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l4;
  i1 = l6;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l4;
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
}
