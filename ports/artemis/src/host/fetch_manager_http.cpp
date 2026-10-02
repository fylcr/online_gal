// fetch_manager_http.cpp -- the httpget/httppost tag surface: validates the url, key, value and header parameters, builds the multipart or urlencoded body and drives the fetch step..
//
// HttpDispatch -- the httpget/httppost tag surface: validates the url, key, value and header parameters, builds the multipart or urlencoded body and drives the fetch step.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 1530958-1530958 (f3293); R1 transcribe-keep.
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
uint32_t CopyBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void ReleaseNodeTree(EngineContext* ctx, uint32_t state, uint32_t node);
uint32_t SkipWhitespace(EngineContext* ctx, uint32_t text);
uint32_t SetStringFromSource(EngineContext* ctx, uint32_t text, uint32_t source);
uint32_t DestroyFileObject(EngineContext* ctx, uint32_t a);
void AssignTaggedTreeRange(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t FindLastCharInString(EngineContext* ctx, uint32_t a, uint32_t b);

namespace {

constexpr uint32_t kF3293Const8B830 = 0x8B830u;  // 
constexpr uint32_t kF3293Const90610 = 0x90610u;  // 
constexpr uint32_t kF3293Const90680 = 0x90680u;  // 
constexpr uint32_t kF3293ConstAE298 = 0xAE298u;  // 
constexpr uint32_t kF3293ConstAEE0C = 0xAEE0Cu;  // 
constexpr uint32_t kF3293ConstB832A = 0xB832Au;  // 
constexpr uint32_t kF3293ConstB832C = 0xB832Cu;  // 
constexpr uint32_t kF3293MsgHeaderKey = 0xB832Eu;  // header_key
constexpr uint32_t kF3293ConstB8336 = 0xB8336u;  // 
constexpr uint32_t kF3293MsgHeaderValue = 0xB8339u;  // header_value
constexpr uint32_t kF3293ConstB8341 = 0xB8341u;  // 
constexpr uint32_t kF3293MsgHttpget = 0xB8346u;  // httpget
constexpr uint32_t kF3293ConstB834E = 0xB834Eu;  // 
constexpr uint32_t kF3293MsgHttppost = 0xB8376u;  // httppost
constexpr uint32_t kF3293ConstB837F = 0xB837Fu;  // 
constexpr uint32_t kF3293ConstB8381 = 0xB8381u;  // 
constexpr uint32_t kF3293MsgValue = 0xB8383u;  // value
constexpr uint32_t kF3293ConstB8387 = 0xB8387u;  // 
constexpr uint32_t kF3293ConstB8389 = 0xB8389u;  // 
constexpr uint32_t kF3293ConstB838B = 0xB838Bu;  // 
constexpr uint32_t kF3293MsgContentDispositionFormData = 0xB838Du;  // Content-Disposition: form-data; name="
constexpr uint32_t kF3293ConstB8395 = 0xB8395u;  // 
constexpr uint32_t kF3293ConstB839D = 0xB839Du;  // 
constexpr uint32_t kF3293ConstB83A5 = 0xB83A5u;  // 
constexpr uint32_t kF3293ConstB83AB = 0xB83ABu;  // 
constexpr uint32_t kF3293ConstB83B4 = 0xB83B4u;  // 
constexpr uint32_t kF3293MsgFilename = 0xB83B6u;  // "; filename="
constexpr uint32_t kF3293ConstB83C4 = 0xB83C4u;  // 
constexpr uint32_t kF3293ConstB83F7 = 0xB83F7u;  // 
constexpr uint32_t kF3293MsgApplicationXWwwForm = 0xB843Cu;  // application/x-www-form-urlencoded
constexpr uint32_t kF3293MsgContentType = 0xB845Eu;  // Content-Type
constexpr uint32_t kF3293ConstB8466 = 0xB8466u;  // 
constexpr uint32_t kF3293MsgMultipartFormDataBoundary = 0xB846Bu;  // multipart/form-data; boundary=
constexpr uint32_t kF3293ConstB8473 = 0xB8473u;  // 
constexpr uint32_t kF3293ConstB847B = 0xB847Bu;  // 
constexpr uint32_t kF3293ConstB8481 = 0xB8481u;  // 
constexpr uint32_t kF3293ConstB84F3 = 0xB84F3u;  // 
constexpr uint32_t kF3293MsgPx = 0xB8506u;  // px != 0
constexpr uint32_t kF3293MsgBoostBoostSmartPtr = 0xB850Eu;  // ../../../../boost_1_66_0/boost/smart_ptr/shared_ptr.hpp
constexpr uint32_t kF3293MsgOperator = 0xB8546u;  // operator->
constexpr uint32_t kF3293ConstB90E8 = 0xB90E8u;  // 
constexpr uint32_t kF3293ConstCF12C = 0xCF12Cu;  // 
constexpr uint32_t kF3293ConstD015C = 0xD015Cu;  // 
constexpr uint32_t kF3293Const656C6966 = 0x656C6966u;  // 
constexpr uint32_t kF3293ConstFFFFFFC0 = 0xFFFFFFC0u;  // 
constexpr uint32_t kF3293ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF3293ResumeWordBytes = 4u;

}  // namespace

namespace FetchManager {
void HttpDispatch(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l13 = 0u; uint32_t l14 = 0u; uint32_t l15 = 0u; uint32_t l16 = 0u; uint32_t l17 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u; uint32_t i6 = 0u;
  uint64_t j1 = 0; uint64_t j3 = 0;
      l10 = 0, l11 = 0, l12 = 0, l13 = 0, l14 = 0, l15 = 0, l16 = 0, l17 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF3293ConstFFFFFFC0;
    i1 += i2;
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
    l4 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 16u);
    l3 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 20u);
    l6 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 24u);
    l8 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 28u);
    l9 = i0;
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
    i0 = LoadU32(ctx, i0 + 48u);
    l14 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 52u);
    l15 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 56u);
    l16 = i0;
    i0 = l7;
    i0 = LoadU32(ctx, i0 + 60u);
    l17 = i0;
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
    i2 = kF3293ResumeWordBytes;
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
    i1 = 480u;
    i0 -= i1;
    l4 = i0;
    GuestStackSp(ctx) = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 12u);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {goto block_24;}
    i0 = a;
    i1 = 24u;
    i0 += i1;
    l15 = i0;
    i0 = b;
    i1 = 4u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l5;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l15;
    i1 = l2;
    i0 = SetStringFromSource(ctx, i0, i1);
    l7 = i0;
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
  } else {
    i0 = l2;
  }
  i1 = a;
  i2 = 20u;
  i1 += i2;
  i2 = b;
  i1 = i1 != i2;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  l2 = i0;
  i1 = ResumePhase(ctx);
  i2 = 2u;
  i1 = i1 == i2;
  i0 |= i1;
  if (i0) {
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i1 = 20u;
      i0 += i1;
      l6 = i0;
      i0 = a;
      i1 = 36u;
      i0 += i1;
      l2 = i0;
      i0 = b;
      i0 = LoadU32(ctx, i0 + 16u);
      l3 = i0;
    }
    i0 = l5;
    i1 = 1u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l2;
      i1 = l3;
      i2 = l6;
      AssignTaggedTreeRange(ctx, i0, i1, i2);
      i0 = 1u;
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
    i1 = 48u;
    i0 += i1;
    l2 = i0;
    i0 = b;
    i1 = 28u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l5;
  i1 = 2u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l2;
    i1 = l3;
    i0 = SetStringFromSource(ctx, i0, i1);
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
    i0 = a;
    i1 = b;
    i1 = LoadU32(ctx, i1 + 40u);
    StoreU32(ctx, i0 + 60u, i1);
    i0 = l4;
    i1 = 3u;
    StoreU8(ctx, i0 + 139u, i1);
    i0 = l4;
    i1 = kF3293ConstB832A;
    i1 = LoadU16(ctx, i1);
    StoreU16(ctx, i0 + 128u, i1);
    i0 = l4;
    i1 = kF3293ConstB832C;
    i1 = LoadU8(ctx, i1);
    StoreU8(ctx, i0 + 130u, i1);
    i0 = l4;
    i1 = 0u;
    StoreU8(ctx, i0 + 131u, i1);
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = l4;
    i2 = 128u;
    i1 += i2;
    StoreU32(ctx, i0 + 352u, i1);
    i0 = a;
    i1 = 36u;
    i0 += i1;
    l12 = i0;
    i0 = l4;
    i1 = 352u;
    i0 += i1;
    l6 = i0;
    i0 = l4;
    i1 = 336u;
    i0 += i1;
    l8 = i0;
    i0 = l4;
    i1 = 40u;
    i0 += i1;
    l2 = i0;
    i0 = l4;
    i1 = 128u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l5;
  i1 = 3u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = l2;
    i2 = l12;
    i3 = l3;
    i4 = kF3293ConstB84F3;
    i5 = l6;
    i6 = l8;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 3u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3293ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {goto block_39;}
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 368u;
    i0 += i1;
    l2 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 40u);
    i1 = 28u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l5;
  i1 = 4u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 2356u;
    i1 = l2;
    i2 = l3;
    i0 = HostOsQuery(ctx, i0, i1, i2);
    l7 = i0;
    i0 = 4u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
    i0 = l7;
    l11 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3293ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_39;}
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 139u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 128u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = a;
    i0 = LoadU8(ctx, i0 + 8u);
    l2 = i0;
    if (i0) {goto block_36;}
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 128u;
    i0 += i1;
    l2 = i0;
    i0 = l11;
    i1 = 36u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l5;
  i1 = 5u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 873u;
    i1 = l2;
    i2 = l3;
    i0 = HostOsQuery(ctx, i0, i1, i2);
    l7 = i0;
    i0 = 5u;
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
    i0 = kF3293ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_38;}
    i0 = l4;
    i1 = l6;
    i1 = LoadU32(ctx, i1);
    i2 = l4;
    i3 = 128u;
    i2 += i3;
    i3 = l6;
    i3 = LoadS8(ctx, i3 + 11u);
    i4 = 0u;
    i3 = (uint32_t)((int32_t)i3 < (int32_t)i4);
    i1 = i3 ? i1 : i2;
    StoreU32(ctx, i0 + 352u, i1);
    i0 = l11;
    i1 = 48u;
    i0 += i1;
    l2 = i0;
    i0 = l11;
    i0 = LoadS8(ctx, i0 + 59u);
    l3 = i0;
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l11;
      i0 = LoadU32(ctx, i0 + 52u);
      if (i0) {goto block_48;}
      i0 = 0u;
      goto block_46;
    }
    i0 = l3;
    if (i0) {goto block_47;}
    i0 = 0u;
    goto block_46;
    block_48:;
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    block_47:;
    i0 = l2;
    i0 = SkipWhitespace(ctx, i0);
    block_46:;
    l3 = i0;
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = l3;
    StoreU16(ctx, i0 + 336u, i1);
    i0 = l4;
    i1 = 336u;
    i0 += i1;
    l8 = i0;
    i0 = l4;
    i1 = 40u;
    i0 += i1;
    l2 = i0;
    i0 = l4;
    i1 = 352u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l5;
  i1 = 6u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 3123u;
    i1 = l2;
    i2 = l3;
    i3 = l8;
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
    i0 = kF3293ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_37;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 44u);
    l2 = i0;
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 40u);
    l3 = i0;
    i0 = l4;
    j1 = 0ull;
    StoreU64(ctx, i0 + 40u, j1);
    i0 = a;
    i1 = l3;
    StoreU32(ctx, i0 + 12u, i1);
    i0 = a;
    i0 = LoadU32(ctx, i0 + 16u);
    l3 = i0;
    i0 = a;
    i1 = l2;
    StoreU32(ctx, i0 + 16u, i1);
    i0 = l3;
    i0 = !(i0);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {goto block_52;}
    i0 = l3;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 4u);
    l2 = i1;
    i2 = 1u;
    i1 -= i2;
    l8 = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 != i1;
    l2 = i0;
    if (i0) {goto block_52;}
    i0 = l3;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = kF3293ConstD015C;
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
    i0 = l2;
    i1 = l3;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 7u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3293ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto running_exit;}
    i0 = l3;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 8u);
    l2 = i1;
    i2 = 1u;
    i1 -= i2;
    l8 = i1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 != i1;
    l2 = i0;
    if (i0) {goto block_52;}
    i0 = l3;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    l2 = i0;
    i0 = kF3293ConstD015C;
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
    i0 = l2;
    i1 = l3;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 8u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3293ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto running_exit;}
  }
  block_52:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 44u);
    l3 = i0;
    i0 = !(i0);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {goto block_59;}
    i0 = l3;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 4u);
    l2 = i1;
    i2 = 1u;
    i1 -= i2;
    l8 = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 != i1;
    l2 = i0;
    if (i0) {goto block_59;}
    i0 = l3;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = kF3293ConstD015C;
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
    i0 = l2;
    i1 = l3;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 9u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3293ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto running_exit;}
    i0 = l3;
    i1 = l3;
    i1 = LoadU32(ctx, i1 + 8u);
    l2 = i1;
    i2 = 1u;
    i1 -= i2;
    l8 = i1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 != i1;
    l2 = i0;
    if (i0) {goto block_59;}
    i0 = l3;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    l2 = i0;
    i0 = kF3293ConstD015C;
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
    i0 = l2;
    i1 = l3;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 10u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3293ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto running_exit;}
  }
  block_59:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l6;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    l2 = i0;
    if (i0) {goto block_35;}
    i0 = l6;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_35;
  }
  block_39:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 139u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_5;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 128u);
    DeallocateRawBlock(ctx, i0);
    i0 = b;
    HostOsRelease(ctx, i0);
    TrapUnreachable();
  }
  block_38:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_6;
  }
  block_37:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    i0 = l6;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_6;}
    i0 = l6;
    i0 = LoadU32(ctx, i0);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_6;
  }
  block_36:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l11;
    i1 = 36u;
    i0 += i1;
    l2 = i0;
    i0 = l4;
    i1 = 40u;
    i0 += i1;
    l3 = i0;
  }
  i0 = l5;
  i1 = 11u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 873u;
    i1 = l3;
    i2 = l2;
    i0 = HostOsQuery(ctx, i0, i1, i2);
    l7 = i0;
    i0 = 11u;
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
    i0 = kF3293ConstD015C;
    i0 = LoadU32(ctx, i0);
    l6 = i0;
    i0 = kF3293ConstD015C;
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
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 128u;
      i0 += i1;
      l6 = i0;
    }
    i0 = l5;
    i1 = 12u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3124u;
      i1 = l6;
      i2 = l3;
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
      i0 = kF3293ConstD015C;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 == i1;
      l6 = i0;
      if (i0) {goto block_75;}
      i0 = l3;
      i0 = LoadS8(ctx, i0 + 11u);
      i1 = kF3293ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l3;
        i0 = LoadU32(ctx, i0);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = l4;
      l8 = i0;
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 139u);
      l3 = i0;
      i1 = kF3293ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l6 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 132u);
        i0 = !(i0);
        if (i0) {goto block_85;}
        i0 = 0u;
        l3 = i0;
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 128u);
        goto block_83;
      }
      i0 = l3;
      i0 = !(i0);
      if (i0) {goto block_85;}
      i0 = 0u;
      l3 = i0;
      i0 = l4;
      i1 = 128u;
      i0 += i1;
      goto block_83;
      block_85:;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 352u;
      i0 += i1;
      l3 = i0;
    }
    i0 = l5;
    i1 = 13u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 873u;
      i1 = l3;
      i2 = l2;
      i0 = HostOsQuery(ctx, i0, i1, i2);
      l7 = i0;
      i0 = 13u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
      i0 = l7;
      l2 = i0;
    }
    i0 = ResumePhase(ctx);
    if (i0) {
      i0 = l6;
    } else {
      i0 = kF3293ConstD015C;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = 1u;
      l3 = i0;
      i0 = l6;
      i1 = 1u;
      i0 = i0 == i1;
      l6 = i0;
      if (i0) {goto block_74;}
      i0 = l4;
      i1 = 352u;
      i0 += i1;
      i1 = l2;
      i1 = LoadS8(ctx, i1 + 11u);
      i2 = kF3293ConstFFFFFFFF;
      i1 = (uint32_t)((int32_t)i1 > (int32_t)i2);
      l9 = i1;
      if (i1) {goto block_83;}
      i0 = l2;
      i0 = LoadU32(ctx, i0);
    }
    block_83:;
    l6 = i0;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l8;
      i1 = l6;
      StoreU32(ctx, i0 + 336u, i1);
      i0 = l11;
      i1 = 48u;
      i0 += i1;
      l2 = i0;
      i0 = l11;
      i0 = LoadS8(ctx, i0 + 59u);
      l6 = i0;
      i1 = kF3293ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l11;
        i0 = LoadU32(ctx, i0 + 52u);
        if (i0) {goto block_92;}
        i0 = 0u;
        goto block_90;
      }
      i0 = l6;
      if (i0) {goto block_91;}
      i0 = 0u;
      goto block_90;
      block_92:;
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      block_91:;
      i0 = l2;
      i0 = SkipWhitespace(ctx, i0);
      block_90:;
      l2 = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = l2;
      StoreU16(ctx, i0 + 320u, i1);
      i0 = l4;
      i1 = 336u;
      i0 += i1;
      l6 = i0;
      i0 = l4;
      i1 = 320u;
      i0 += i1;
      l8 = i0;
      i0 = l4;
      i1 = 40u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l5;
    i1 = 14u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3123u;
      i1 = l2;
      i2 = l6;
      i3 = l8;
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
      i0 = kF3293ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_73;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 44u);
      l6 = i0;
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 40u);
      l2 = i0;
      i0 = l4;
      j1 = 0ull;
      StoreU64(ctx, i0 + 40u, j1);
      i0 = a;
      i1 = l2;
      StoreU32(ctx, i0 + 12u, i1);
      i0 = a;
      i0 = LoadU32(ctx, i0 + 16u);
      l2 = i0;
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
      if (i0) {goto block_96;}
      i0 = l2;
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 4u);
      l6 = i1;
      i2 = 1u;
      i1 -= i2;
      l8 = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 != i1;
      l6 = i0;
      if (i0) {goto block_96;}
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      l6 = i0;
      i0 = kF3293ConstD015C;
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
      i0 = l6;
      i1 = l2;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 15u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3293ConstD015C;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto running_exit;}
      i0 = l2;
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 8u);
      l6 = i1;
      i2 = 1u;
      i1 -= i2;
      l8 = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 != i1;
      l6 = i0;
      if (i0) {goto block_96;}
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      l6 = i0;
      i0 = kF3293ConstD015C;
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
      i0 = l6;
      i1 = l2;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 16u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3293ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto running_exit;}
    }
    block_96:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 44u);
      l2 = i0;
      i0 = !(i0);
      l6 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      if (i0) {goto block_103;}
      i0 = l2;
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 4u);
      l6 = i1;
      i2 = 1u;
      i1 -= i2;
      l8 = i1;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 != i1;
      l6 = i0;
      if (i0) {goto block_103;}
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 8u);
      l6 = i0;
      i0 = kF3293ConstD015C;
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
      i0 = l6;
      i1 = l2;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 17u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3293ConstD015C;
      i0 = LoadU32(ctx, i0);
      l6 = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 == i1;
      l6 = i0;
      if (i0) {goto block_71;}
      i0 = l2;
      i1 = l2;
      i1 = LoadU32(ctx, i1 + 8u);
      l6 = i1;
      i2 = 1u;
      i1 -= i2;
      l8 = i1;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l6;
      i1 = 1u;
      i0 = i0 != i1;
      l6 = i0;
      if (i0) {goto block_103;}
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 12u);
      l6 = i0;
      i0 = kF3293ConstD015C;
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
      i0 = l6;
      i1 = l2;
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
      i0 = kF3293ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_71;}
    }
    block_103:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l3;
      i0 = !(i0);
      if (i0) {goto block_110;}
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 363u);
      i1 = kF3293ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      if (i0) {goto block_110;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 352u);
      DeallocateRawBlock(ctx, i0);
      block_110:;
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 139u);
      i1 = kF3293ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      l2 = i0;
      if (i0) {goto block_35;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 128u);
      l2 = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_35;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_6;
  }
  block_75:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = LoadS8(ctx, i0 + 11u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_6;}
    i0 = l3;
    i0 = LoadU32(ctx, i0);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_6;
  }
  block_74:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_72;
  }
  block_73:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    i0 = l3;
    i0 = !(i0);
    a = i0;
    if (i0) {goto block_72;}
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 363u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_72;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 352u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_72:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 139u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_6;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 128u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_6;
  }
  block_71:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    i0 = FindMatchingCatch(ctx, i0);
    a = i0;
    i0 = HostTempRet0(ctx);
    b = i0;
  }
  i0 = l5;
  i1 = 19u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 19u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto block_23;}
  block_35:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = !(i0);
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
      i0 = kF3293ConstD015C;
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
      i0 = 857u;
      i1 = kF3293MsgPx;
      i2 = kF3293MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3293MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 20u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3293ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_23;}
      goto running_exit;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3293ConstD015C;
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
    i0 = 3125u;
    i1 = l3;
    i2 = 1u;
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
    i0 = kF3293ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto block_10;}
    i0 = l4;
    j1 = 0ull;
    StoreU64(ctx, i0 + 340u, j1);
    i0 = l4;
    i1 = l4;
    i2 = 336u;
    i1 += i2;
    i2 = 4u;
    i1 |= i2;
    l2 = i1;
    StoreU32(ctx, i0 + 336u, i1);
    i0 = 0u;
    l6 = i0;
  }
  loop_128: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 128u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l5;
    i1 = 22u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1284u;
      i1 = l2;
      i2 = l6;
      HostOsSchedule(ctx, i0, i1, i2);
      i0 = 22u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3293ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_11;}
      i0 = l4;
      i1 = kF3293ConstB8336;
      i1 = LoadU16(ctx, i1);
      StoreU16(ctx, i0 + 328u, i1);
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 2560u;
      StoreU16(ctx, i0 + 330u, i1);
      i0 = l4;
      i1 = kF3293MsgHeaderKey;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0 + 320u, j1);
      i0 = l4;
      i1 = 320u;
      i0 += i1;
      l8 = i0;
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 128u);
      i1 = l4;
      i2 = 128u;
      i1 += i2;
      i2 = l4;
      i2 = LoadS8(ctx, i2 + 139u);
      l3 = i2;
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      l2 = i2;
      i0 = i2 ? i0 : i1;
      l9 = i0;
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 132u);
      l10 = i0;
      i1 = l3;
      i2 = 255u;
      i1 &= i2;
      l3 = i1;
      i2 = l2;
      i0 = i2 ? i0 : i1;
      l2 = i0;
    }
    i0 = l5;
    i1 = 23u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 860u;
      i1 = l8;
      i2 = l9;
      i3 = l2;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l7 = i0;
      i0 = 23u;
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
      i0 = kF3293ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_12;}
      i0 = l4;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 48u, i1);
      i0 = l4;
      i1 = l3;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0 + 40u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 331u);
      i1 = kF3293ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l2 = i0;
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 320u);
        l2 = i0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = kF3293ConstD015C;
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
      i1 = 16u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l7 = i0;
      i0 = 24u;
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
      i0 = kF3293ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_13;}
      i0 = l4;
      i1 = l3;
      StoreU32(ctx, i0 + 304u, i1);
      i0 = l4;
      j1 = 9223372105574252556ull;
      StoreU64(ctx, i0 + 308u, j1);
      i0 = l3;
      i1 = 0u;
      StoreU8(ctx, i0 + 12u, i1);
      i0 = l3;
      i1 = kF3293ConstB8341;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l3;
      i1 = kF3293MsgHeaderValue;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0, j1);
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 304u;
      i0 += i1;
      l8 = i0;
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 128u);
      i1 = l4;
      i2 = 128u;
      i1 += i2;
      i2 = l4;
      i2 = LoadU8(ctx, i2 + 139u);
      l3 = i2;
      i3 = 24u;
      i2 <<= (i3 & 31);
      i3 = 24u;
      i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      l2 = i2;
      i0 = i2 ? i0 : i1;
      l9 = i0;
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 132u);
      l10 = i0;
      i1 = l3;
      i2 = l2;
      i0 = i2 ? i0 : i1;
      l2 = i0;
    }
    i0 = l5;
    i1 = 25u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 860u;
      i1 = l8;
      i2 = l9;
      i3 = l2;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l7 = i0;
      i0 = 25u;
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
      i0 = kF3293ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      if (i0) {goto block_14;}
      i0 = l4;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 328u, i1);
      i0 = l4;
      i1 = l3;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0 + 320u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 315u);
      i1 = kF3293ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l2 = i0;
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 304u);
        l2 = i0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = a;
      i0 = LoadU32(ctx, i0 + 40u);
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
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 44u);
        i1 = l4;
        i1 = LoadU8(ctx, i1 + 51u);
        l2 = i1;
        i2 = l2;
        i3 = 24u;
        i2 <<= (i3 & 31);
        i3 = 24u;
        i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
        l9 = i2;
        i3 = 0u;
        i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
        l2 = i2;
        i0 = i2 ? i0 : i1;
        l8 = i0;
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 40u);
        i1 = l4;
        i2 = 40u;
        i1 += i2;
        l13 = i1;
        i2 = l2;
        i0 = i2 ? i0 : i1;
        l10 = i0;
        i0 = l3;
        l2 = i0;
      }
      loop_142: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          i0 = LoadU8(ctx, i0 + 27u);
          l13 = i0;
          i1 = 24u;
          i0 <<= (i1 & 31);
          i1 = 24u;
          i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
          i1 = 0u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          l14 = i0;
          i0 = l2;
          i0 = LoadU32(ctx, i0 + 20u);
          i1 = l13;
          i2 = l14;
          i0 = i2 ? i0 : i1;
          l13 = i0;
          i1 = l8;
          i0 = i0 < i1;
          l16 = i0;
          i0 = l13;
          i1 = l8;
          i2 = l16;
          i0 = i2 ? i0 : i1;
          l17 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l17;
          if (i0) {
            i0 = l10;
            i1 = l2;
            i2 = 16u;
            i1 += i2;
            l7 = i1;
            i1 = LoadU32(ctx, i1);
            i2 = l7;
            i3 = l14;
            i1 = i3 ? i1 : i2;
            l14 = i1;
            i2 = l17;
            i0 = CompareBytes(ctx, i0, i1, i2);
            l7 = i0;
            i0 = !(i0);
            if (i0) {
              i0 = l8;
              i1 = l13;
              i0 = i0 >= i1;
              l13 = i0;
              if (i0) {goto block_150;}
              goto block_144;
            }
            i0 = l7;
            i1 = kF3293ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            l13 = i0;
            if (i0) {goto block_144;}
            block_150:;
            i0 = l14;
            i1 = l10;
            i2 = l17;
            i0 = CompareBytes(ctx, i0, i1, i2);
            l13 = i0;
            i0 = !(i0);
            l14 = i0;
            if (i0) {goto block_148;}
            i0 = l13;
            i1 = kF3293ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            l13 = i0;
            if (i0) {goto block_147;}
            goto block_145;
          }
          i0 = l8;
          i1 = l13;
          i0 = i0 < i1;
          l13 = i0;
          if (i0) {goto block_144;}
          block_148:;
          i0 = l16;
          if (i0) {goto block_145;}
          block_147:;
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 320u);
          l10 = i0;
          i1 = l4;
          i2 = 320u;
          i1 += i2;
          i2 = l4;
          i2 = LoadU8(ctx, i2 + 331u);
          l2 = i2;
          i3 = 24u;
          i2 <<= (i3 & 31);
          i3 = 24u;
          i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
          l17 = i2;
          i3 = 0u;
          i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
          l8 = i2;
          i0 = i2 ? i0 : i1;
          l13 = i0;
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 324u);
          i1 = l2;
          i2 = l8;
          i0 = i2 ? i0 : i1;
          l2 = i0;
        }
        loop_153: 
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            i0 = LoadU8(ctx, i0 + 27u);
            l8 = i0;
            i1 = 24u;
            i0 <<= (i1 & 31);
            i1 = 24u;
            i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
            i1 = 0u;
            i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
            l14 = i0;
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 20u);
            i1 = l8;
            i2 = l14;
            i0 = i2 ? i0 : i1;
            l8 = i0;
            i1 = l2;
            i0 = i0 < i1;
            l16 = i0;
            i0 = l8;
            i1 = l2;
            i2 = l16;
            i0 = i2 ? i0 : i1;
            l10 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l10;
            if (i0) {
              i0 = l13;
              i1 = l3;
              i2 = 16u;
              i1 += i2;
              l7 = i1;
              i1 = LoadU32(ctx, i1);
              i2 = l7;
              i3 = l14;
              i1 = i3 ? i1 : i2;
              l14 = i1;
              i2 = l10;
              i0 = CompareBytes(ctx, i0, i1, i2);
              l7 = i0;
              i0 = !(i0);
              if (i0) {
                i0 = l2;
                i1 = l8;
                i0 = i0 >= i1;
                l8 = i0;
                if (i0) {goto block_161;}
                goto block_155;
              }
              i0 = l7;
              i1 = kF3293ConstFFFFFFFF;
              i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
              l8 = i0;
              if (i0) {goto block_155;}
              block_161:;
              i0 = l14;
              i1 = l13;
              i2 = l10;
              i0 = CompareBytes(ctx, i0, i1, i2);
              l8 = i0;
              i0 = !(i0);
              l10 = i0;
              if (i0) {goto block_159;}
              i0 = l8;
              i1 = kF3293ConstFFFFFFFF;
              i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
              l8 = i0;
              if (i0) {goto block_158;}
              goto block_156;
            }
            i0 = l2;
            i1 = l8;
            i0 = i0 < i1;
            l8 = i0;
            if (i0) {goto block_155;}
            block_159:;
            i0 = l16;
            if (i0) {goto block_156;}
            block_158:;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = l4;
            i2 = 320u;
            i1 += i2;
            StoreU32(ctx, i0 + 288u, i1);
            i0 = l4;
            i1 = 288u;
            i0 += i1;
            l8 = i0;
            i0 = l4;
            i1 = 272u;
            i0 += i1;
            l9 = i0;
            i0 = l4;
            i1 = 304u;
            i0 += i1;
            l2 = i0;
            i0 = l4;
            i1 = 320u;
            i0 += i1;
            l3 = i0;
          }
          i0 = l5;
          i1 = 26u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 1640u;
            i1 = l2;
            i2 = l12;
            i3 = l3;
            i4 = kF3293ConstB84F3;
            i5 = l8;
            i6 = l9;
            HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
            i0 = 26u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 1u;
            i0 = i0 == i1;
            l2 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l2;
            if (i0) {goto block_165;}
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 304u);
            l3 = i0;
            i0 = l4;
            i1 = l4;
            i2 = 40u;
            i1 += i2;
            StoreU32(ctx, i0 + 288u, i1);
            i0 = l4;
            i1 = 40u;
            i0 += i1;
            l8 = i0;
            i0 = l4;
            i1 = 288u;
            i0 += i1;
            l9 = i0;
            i0 = l4;
            i1 = 272u;
            i0 += i1;
            l10 = i0;
            i0 = l4;
            i1 = 304u;
            i0 += i1;
            l2 = i0;
          }
          i0 = l5;
          i1 = 27u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 1640u;
            i1 = l2;
            i2 = l12;
            i3 = l8;
            i4 = kF3293ConstB84F3;
            i5 = l9;
            i6 = l10;
            HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
            i0 = 27u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 == i1;
            l2 = i0;
            if (i0) {goto block_165;}
            i0 = l4;
            i1 = l4;
            i1 = LoadU32(ctx, i1 + 304u);
            i2 = 28u;
            i1 += i2;
            l2 = i1;
            StoreU32(ctx, i0 + 288u, i1);
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = 304u;
            i0 += i1;
            l8 = i0;
            i0 = l4;
            i1 = 288u;
            i0 += i1;
            l10 = i0;
            i0 = l4;
            i1 = 272u;
            i0 += i1;
            l13 = i0;
            i0 = l4;
            i1 = 336u;
            i0 += i1;
            l9 = i0;
          }
          i0 = l5;
          i1 = 28u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 1640u;
            i1 = l8;
            i2 = l9;
            i3 = l2;
            i4 = kF3293ConstB84F3;
            i5 = l10;
            i6 = l13;
            HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
            i0 = 28u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 == i1;
            l2 = i0;
            if (i0) {goto block_165;}
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 304u);
            i1 = 28u;
            i0 += i1;
            l2 = i0;
            i0 = l3;
            i1 = 28u;
            i0 += i1;
            l3 = i0;
          }
          i0 = l5;
          i1 = 29u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 870u;
            i1 = l2;
            i2 = l3;
            i0 = HostOsQuery(ctx, i0, i1, i2);
            l7 = i0;
            i0 = 29u;
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
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 1u;
            i0 = i0 != i1;
            l2 = i0;
            if (i0) {goto block_152;}
          }
          block_165:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = HostRet0(ctx);
            b = i0;
            i0 = HostTempRet0(ctx);
            i0 = l4;
            i0 = LoadS8(ctx, i0 + 331u);
            i1 = kF3293ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            a = i0;
            if (i0) {goto block_9;}
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 320u);
            a = i0;
            DeallocateRawBlock(ctx, i0);
            goto block_9;
          }
          block_156:;
          i0 = l3;
          i1 = l3;
          i2 = 4u;
          i1 += i2;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          l3 = i0;
          block_155:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            if (i0) {goto loop_153;}
          }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l17;
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 320u);
            DeallocateRawBlock(ctx, i0);
            i0 = l4;
            i0 = LoadS8(ctx, i0 + 51u);
            l9 = i0;
          }
          i0 = l9;
          i1 = 24u;
          i0 <<= (i1 & 31);
          i1 = 24u;
          i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 40u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 139u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          l2 = i0;
          if (i0) {goto block_127;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 128u);
          l2 = i0;
          DeallocateRawBlock(ctx, i0);
          goto block_127;
        }
        block_152:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 331u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 320u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 51u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 40u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 139u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          l2 = i0;
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 128u);
            l2 = i0;
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l6;
          i1 = 1u;
          i0 += i1;
          l6 = i0;
          goto loop_128;
        }
        block_145:;
        i0 = l2;
        i1 = l2;
        i2 = 4u;
        i1 += i2;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        l2 = i0;
        block_144:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          if (i0) {goto loop_142;}
        }
    }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 331u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 320u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 51u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 40u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 139u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    l2 = i0;
    if (i0) {goto block_127;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 128u);
    l2 = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_127:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  i1 = 0u;
  i2 = ResumePhase(ctx);
  i2 = !(i2);
  if (i2) {
    i2 = l4;
    i3 = 0u;
    StoreU8(ctx, i2 + 136u, i3);
    i2 = l4;
    j3 = 7308604897068083558ull;
    StoreU64(ctx, i2 + 128u, j3);
    i2 = l4;
    i3 = 8u;
    StoreU8(ctx, i2 + 139u, i3);
    i2 = a;
    i2 = LoadU32(ctx, i2 + 40u);
    l2 = i2;
    i2 = !(i2);
    l3 = i2;
  }
  i2 = l3;
  i0 = i2 ? i0 : i1;
  if (i0) {goto block_187;}
  loop_190: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i0 = LoadU8(ctx, i0 + 27u);
      l3 = i0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
      i1 = 0u;
      i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
      l8 = i0;
      i0 = l2;
      i0 = LoadU32(ctx, i0 + 20u);
      i1 = l3;
      i2 = l8;
      i0 = i2 ? i0 : i1;
      l3 = i0;
      i1 = 8u;
      i0 = i0 < i1;
      l13 = i0;
      i0 = l3;
      i1 = 8u;
      i2 = l13;
      i0 = i2 ? i0 : i1;
      l6 = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l6;
      if (i0) {
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        i1 = l2;
        i2 = 16u;
        i1 += i2;
        l10 = i1;
        i1 = LoadU32(ctx, i1);
        l14 = i1;
        i2 = l10;
        i3 = l8;
        i1 = i3 ? i1 : i2;
        l8 = i1;
        i2 = l6;
        i0 = CompareBytes(ctx, i0, i1, i2);
        l10 = i0;
        i0 = !(i0);
        l9 = i0;
        if (i0) {
          i0 = l3;
          i1 = 8u;
          i0 = i0 <= i1;
          l3 = i0;
          if (i0) {goto block_199;}
          goto block_193;
        }
        i0 = l10;
        i1 = kF3293ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        l3 = i0;
        if (i0) {goto block_193;}
        block_199:;
        i0 = l8;
        i1 = l4;
        i2 = 128u;
        i1 += i2;
        i2 = l6;
        i0 = CompareBytes(ctx, i0, i1, i2);
        l3 = i0;
        i0 = !(i0);
        l6 = i0;
        if (i0) {goto block_197;}
        i0 = l3;
        i1 = kF3293ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        l3 = i0;
        if (i0) {goto block_196;}
        goto block_194;
      }
      i0 = l3;
      i1 = 8u;
      i0 = i0 > i1;
      l3 = i0;
      if (i0) {goto block_193;}
      block_197:;
      i0 = l13;
      if (i0) {goto block_194;}
      block_196:;
      i0 = a;
      i0 = LoadU32(ctx, i0 + 12u);
      l3 = i0;
      if (i0) {goto block_191;}
      i0 = kF3293ConstD015C;
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
      i0 = 857u;
      i1 = kF3293MsgPx;
      i2 = kF3293MsgBoostBoostSmartPtr;
      i3 = 734u;
      i4 = kF3293MsgOperator;
      HostFailAssertion(ctx, i0, i1, i2, i3, i4);
      i0 = 30u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3293ConstD015C;
      i0 = LoadU32(ctx, i0);
      a = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = a;
      i1 = 1u;
      i0 = i0 != i1;
      a = i0;
      if (i0) {goto block_23;}
      goto running_exit;
    }
    block_194:;
    i0 = l2;
    i1 = l2;
    i2 = 4u;
    i1 += i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l2 = i0;
    block_193:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      if (i0) {goto loop_190;}
      goto block_187;
    }
    block_191:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 0u;
    StoreU8(ctx, i0 + 136u, i1);
    i0 = l4;
    j1 = 7308604897068083558ull;
    StoreU64(ctx, i0 + 128u, j1);
    i0 = l4;
    i1 = 8u;
    StoreU8(ctx, i0 + 139u, i1);
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = l4;
    i2 = 128u;
    i1 += i2;
    StoreU32(ctx, i0 + 320u, i1);
    i0 = l4;
    i1 = 128u;
    i0 += i1;
    l6 = i0;
    i0 = l4;
    i1 = 320u;
    i0 += i1;
    l8 = i0;
    i0 = l4;
    i1 = 304u;
    i0 += i1;
    l9 = i0;
    i0 = l4;
    i1 = 40u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l5;
  i1 = 31u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 1959u;
    i1 = l2;
    i2 = l12;
    i3 = l6;
    i4 = kF3293ConstB84F3;
    i5 = l8;
    i6 = l9;
    HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
    i0 = 31u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3293ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    if (i0) {goto block_207;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 40u);
    l2 = i0;
    i0 = l3;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 60u);
    l6 = i0;
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 28u;
    i0 += i1;
    l2 = i0;
  }
  i0 = l5;
  i1 = 32u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = l6;
    i1 = l3;
    i2 = l2;
    HostOsSchedule(ctx, i0, i1, i2);
    i0 = 32u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3293ConstD015C;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto block_207;}
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 139u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    l2 = i0;
    if (i0) {goto block_187;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 128u);
    l2 = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_187;
  }
  block_207:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 139u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_7;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 128u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_7;
  }
  block_187:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = a;
    i0 = LoadU8(ctx, i0 + 35u);
    l2 = i0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    l6 = i0;
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l8 = i0;
    i0 = a;
    i0 = LoadU32(ctx, i0 + 28u);
    l3 = i0;
    i1 = l2;
    i2 = l8;
    i0 = i2 ? i0 : i1;
    i1 = 7u;
    i0 = i0 != i1;
    l8 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l8;
    if (i0) {goto block_214;}
    i0 = kF3293ConstD015C;
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
    i0 = 1354u;
    i1 = l15;
    i2 = 0u;
    i3 = kF3293ConstFFFFFFFF;
    i4 = kF3293MsgHttpget;
    i5 = 7u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l7 = i0;
    i0 = 33u;
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
    i0 = kF3293ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 != i1;
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
      i0 = l3;
      if (i0) {
        i0 = a;
        i0 = LoadU32(ctx, i0 + 28u);
        l3 = i0;
        i0 = a;
        i0 = LoadU8(ctx, i0 + 35u);
        l2 = i0;
        l6 = i0;
        goto block_214;
      }
      i0 = a;
      i0 = LoadU32(ctx, i0 + 12u);
      b = i0;
      i0 = !(i0);
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
        i0 = kF3293ConstD015C;
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
        i0 = 857u;
        i1 = kF3293MsgPx;
        i2 = kF3293MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3293MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 34u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_23;}
        goto running_exit;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = b;
      i0 = LoadU32(ctx, i0);
      i0 = LoadU32(ctx, i0 + 52u);
      l3 = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 336u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l5;
    i1 = 35u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l3;
      i1 = b;
      i2 = l2;
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
      i0 = kF3293ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3293ConstD015C;
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
      if (i0) {goto block_230;}
      i0 = a;
      i0 = LoadU32(ctx, i0 + 12u);
      b = i0;
      i0 = !(i0);
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
        i0 = kF3293ConstD015C;
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
        i0 = 857u;
        i1 = kF3293MsgPx;
        i2 = kF3293MsgBoostBoostSmartPtr;
        i3 = 734u;
        i4 = kF3293MsgOperator;
        HostFailAssertion(ctx, i0, i1, i2, i3, i4);
        i0 = 36u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_23;}
        goto running_exit;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 128u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l5;
    i1 = 37u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3126u;
      i1 = l2;
      i2 = l11;
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
      i0 = kF3293ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 != i1;
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
        i0 = b;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 16u);
        l3 = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 128u);
        i1 = l4;
        i2 = 128u;
        i1 += i2;
        l6 = i1;
        i2 = l4;
        i2 = LoadS8(ctx, i2 + 139u);
        i3 = 0u;
        i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
        l8 = i2;
        i0 = i2 ? i0 : i1;
        l2 = i0;
      }
      i0 = l5;
      i1 = 38u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l3;
        i1 = b;
        i2 = l2;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l7 = i0;
        i0 = 38u;
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
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 == i1;
        l2 = i0;
        if (i0) {goto block_239;}
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 139u);
        i1 = kF3293ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        l2 = i0;
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 128u);
          l2 = i0;
          DeallocateRawBlock(ctx, i0);
        }
        i0 = b;
        i0 = !(i0);
        b = i0;
        if (i0) {goto block_213;}
        i0 = kF3293ConstD015C;
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
        i0 = 3127u;
        i1 = a;
        i2 = kF3293ConstFFFFFFFF;
        i3 = kF3293ConstB834E;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 39u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto block_230;}
        i0 = a;
        i1 = 0u;
        StoreU32(ctx, i0 + 12u, i1);
        i0 = a;
        i0 = LoadU32(ctx, i0 + 16u);
        b = i0;
        i0 = a;
        i1 = 0u;
        StoreU32(ctx, i0 + 16u, i1);
        i0 = b;
        i0 = !(i0);
        a = i0;
        if (i0) {goto block_213;}
        i0 = b;
        i1 = b;
        i1 = LoadU32(ctx, i1 + 4u);
        a = i1;
        i2 = 1u;
        i1 -= i2;
        l2 = i1;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_213;}
        i0 = b;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 8u);
        a = i0;
        i0 = kF3293ConstD015C;
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
        i0 = a;
        i1 = b;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 40u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
      }
      i0 = a;
      i1 = ResumePhase(ctx);
      i2 = 2u;
      i1 = i1 == i2;
      i0 |= i1;
      if (i0) {
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = b;
          i1 = b;
          i1 = LoadU32(ctx, i1 + 8u);
          a = i1;
          i2 = 1u;
          i1 -= i2;
          l2 = i1;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_213;}
          i0 = b;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 12u);
          a = i0;
          i0 = kF3293ConstD015C;
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
          i0 = a;
          i1 = b;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 41u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_213;}
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto running_exit;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      a = i0;
      goto block_7;
    }
    block_239:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 139u);
      i1 = kF3293ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_7;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 128u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_7;
    }
    block_230:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      a = i0;
      goto block_7;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto running_exit;}
  block_214:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l3;
    i1 = l2;
    i2 = l6;
    i3 = 24u;
    i2 <<= (i3 & 31);
    i3 = 24u;
    i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
    i3 = 0u;
    i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
    l6 = i2;
    i0 = i2 ? i0 : i1;
    i1 = 8u;
    i0 = i0 != i1;
    l2 = i0;
    if (i0) {goto block_213;}
    i0 = kF3293ConstD015C;
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
    i0 = 1354u;
    i1 = l15;
    i2 = 0u;
    i3 = kF3293ConstFFFFFFFF;
    i4 = kF3293MsgHttppost;
    i5 = 8u;
    i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
    l7 = i0;
    i0 = 42u;
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
    i0 = kF3293ConstD015C;
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l2;
    i1 = 1u;
    i0 = i0 == i1;
    l2 = i0;
    if (i0) {goto running_exit;}
    i0 = l3;
    if (i0) {goto block_213;}
    i0 = l4;
    i1 = 0u;
    StoreU32(ctx, i0 + 328u, i1);
    i0 = l4;
    j1 = 0ull;
    StoreU64(ctx, i0 + 320u, j1);
    i0 = l4;
    i1 = 0u;
    StoreU32(ctx, i0 + 312u, i1);
    i0 = l4;
    j1 = 0ull;
    StoreU64(ctx, i0 + 304u, j1);
    i0 = l4;
    i1 = 0u;
    StoreU32(ctx, i0 + 296u, i1);
    i0 = l4;
    j1 = 0ull;
    StoreU64(ctx, i0 + 288u, j1);
    i0 = l4;
    i1 = 40u;
    i0 += i1;
    l2 = i0;
    i1 = 4u;
    i0 |= i1;
    l17 = i0;
    i0 = 0u;
    l6 = i0;
  }
  loop_262: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 272u;
      i0 += i1;
      l2 = i0;
    }
    i0 = l5;
    i1 = 43u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 1284u;
      i1 = l2;
      i2 = l6;
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
      i0 = kF3293ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_16;}
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 3u;
      StoreU8(ctx, i0 + 139u, i1);
      i0 = l4;
      i1 = kF3293ConstB837F;
      i1 = LoadU16(ctx, i1);
      StoreU16(ctx, i0 + 128u, i1);
      i0 = l4;
      i1 = kF3293ConstB8381;
      i1 = LoadU8(ctx, i1);
      StoreU8(ctx, i0 + 130u, i1);
      i0 = l4;
      i1 = 0u;
      StoreU8(ctx, i0 + 131u, i1);
      i0 = l4;
      i1 = 128u;
      i0 += i1;
      l8 = i0;
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 272u);
      i1 = l4;
      i2 = 272u;
      i1 += i2;
      i2 = l4;
      i2 = LoadS8(ctx, i2 + 283u);
      l3 = i2;
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      l2 = i2;
      i0 = i2 ? i0 : i1;
      l9 = i0;
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 276u);
      l10 = i0;
      i1 = l3;
      i2 = 255u;
      i1 &= i2;
      l3 = i1;
      i2 = l2;
      i0 = i2 ? i0 : i1;
      l2 = i0;
    }
    i0 = l5;
    i1 = 44u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 860u;
      i1 = l8;
      i2 = l9;
      i3 = l2;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l7 = i0;
      i0 = 44u;
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
      i0 = kF3293ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_18;}
      i0 = l4;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 264u, i1);
      i0 = l4;
      i1 = l3;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0 + 256u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 139u);
      i1 = kF3293ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 128u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 5u;
      StoreU8(ctx, i0 + 139u, i1);
      i0 = l4;
      i1 = kF3293MsgValue;
      i1 = LoadU32(ctx, i1);
      StoreU32(ctx, i0 + 128u, i1);
      i0 = l4;
      i1 = kF3293ConstB8387;
      i1 = LoadU8(ctx, i1);
      StoreU8(ctx, i0 + 132u, i1);
      i0 = l4;
      i1 = 0u;
      StoreU8(ctx, i0 + 133u, i1);
      i0 = l4;
      i1 = 128u;
      i0 += i1;
      l8 = i0;
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 272u);
      i1 = l4;
      i2 = 272u;
      i1 += i2;
      i2 = l4;
      i2 = LoadS8(ctx, i2 + 283u);
      l3 = i2;
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      l2 = i2;
      i0 = i2 ? i0 : i1;
      l9 = i0;
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 276u);
      l10 = i0;
      i1 = l3;
      i2 = 255u;
      i1 &= i2;
      l3 = i1;
      i2 = l2;
      i0 = i2 ? i0 : i1;
      l2 = i0;
    }
    i0 = l5;
    i1 = 45u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 860u;
      i1 = l8;
      i2 = l9;
      i3 = l2;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l7 = i0;
      i0 = 45u;
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
      i0 = kF3293ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_20;}
      i0 = l4;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 248u, i1);
      i0 = l4;
      i1 = l3;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0 + 240u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 139u);
      i1 = kF3293ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 128u);
        DeallocateRawBlock(ctx, i0);
      }
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 0u;
      StoreU8(ctx, i0 + 132u, i1);
      i0 = l4;
      i1 = kF3293Const656C6966;
      StoreU32(ctx, i0 + 128u, i1);
      i0 = l4;
      i1 = 4u;
      StoreU8(ctx, i0 + 139u, i1);
      i0 = l4;
      i1 = 128u;
      i0 += i1;
      l8 = i0;
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 272u);
      i1 = l4;
      i2 = 272u;
      i1 += i2;
      i2 = l4;
      i2 = LoadS8(ctx, i2 + 283u);
      l3 = i2;
      i3 = 0u;
      i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
      l2 = i2;
      i0 = i2 ? i0 : i1;
      l9 = i0;
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 276u);
      l10 = i0;
      i1 = l3;
      i2 = 255u;
      i1 &= i2;
      l3 = i1;
      i2 = l2;
      i0 = i2 ? i0 : i1;
      l2 = i0;
    }
    i0 = l5;
    i1 = 46u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 860u;
      i1 = l8;
      i2 = l9;
      i3 = l2;
      i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
      l7 = i0;
      i0 = 46u;
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
      i0 = kF3293ConstD015C;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l2;
      i1 = 1u;
      i0 = i0 == i1;
      l2 = i0;
      if (i0) {goto block_22;}
      i0 = l4;
      i1 = l3;
      i1 = LoadU32(ctx, i1 + 8u);
      StoreU32(ctx, i0 + 232u, i1);
      i0 = l4;
      i1 = l3;
      j1 = LoadU64(ctx, i1);
      StoreU64(ctx, i0 + 224u, j1);
      i0 = l3;
      j1 = 0ull;
      StoreU64(ctx, i0, j1);
      i0 = l3;
      i1 = 0u;
      StoreU32(ctx, i0 + 8u, i1);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 139u);
      i1 = kF3293ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
      l2 = i0;
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 128u);
        l2 = i0;
        DeallocateRawBlock(ctx, i0);
      }
      i0 = a;
      i0 = LoadU32(ctx, i0 + 40u);
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
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 260u);
        i1 = l4;
        i1 = LoadU8(ctx, i1 + 267u);
        l2 = i1;
        i2 = l2;
        i3 = 24u;
        i2 <<= (i3 & 31);
        i3 = 24u;
        i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
        i3 = 0u;
        i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
        l2 = i2;
        i0 = i2 ? i0 : i1;
        l8 = i0;
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 256u);
        l9 = i0;
        i1 = l4;
        i2 = 256u;
        i1 += i2;
        l10 = i1;
        i2 = l2;
        i0 = i2 ? i0 : i1;
        l13 = i0;
        i0 = l3;
        l2 = i0;
      }
      loop_277: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          i0 = LoadU8(ctx, i0 + 27u);
          l15 = i0;
          i1 = 24u;
          i0 <<= (i1 & 31);
          i1 = 24u;
          i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
          i1 = 0u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          l9 = i0;
          i0 = l2;
          i0 = LoadU32(ctx, i0 + 20u);
          i1 = l15;
          i2 = l9;
          i0 = i2 ? i0 : i1;
          l15 = i0;
          i1 = l8;
          i0 = i0 < i1;
          l14 = i0;
          i0 = l15;
          i1 = l8;
          i2 = l14;
          i0 = i2 ? i0 : i1;
          l10 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l10;
          if (i0) {
            i0 = l13;
            i1 = l2;
            i2 = 16u;
            i1 += i2;
            l16 = i1;
            i1 = LoadU32(ctx, i1);
            i2 = l16;
            i3 = l9;
            i1 = i3 ? i1 : i2;
            l9 = i1;
            i2 = l10;
            i0 = CompareBytes(ctx, i0, i1, i2);
            l16 = i0;
            i0 = !(i0);
            if (i0) {
              i0 = l8;
              i1 = l15;
              i0 = i0 >= i1;
              if (i0) {goto block_285;}
              goto block_279;
            }
            i0 = l16;
            i1 = kF3293ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            if (i0) {goto block_279;}
            block_285:;
            i0 = l9;
            i1 = l13;
            i2 = l10;
            i0 = CompareBytes(ctx, i0, i1, i2);
            l15 = i0;
            i0 = !(i0);
            l9 = i0;
            if (i0) {goto block_283;}
            i0 = l15;
            i1 = kF3293ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            l9 = i0;
            if (i0) {goto block_282;}
            goto block_280;
          }
          i0 = l8;
          i1 = l15;
          i0 = i0 < i1;
          l9 = i0;
          if (i0) {goto block_279;}
          block_283:;
          i0 = l14;
          if (i0) {goto block_280;}
          block_282:;
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 244u);
          i1 = l4;
          i1 = LoadU8(ctx, i1 + 251u);
          l2 = i1;
          i2 = l2;
          i3 = 24u;
          i2 <<= (i3 & 31);
          i3 = 24u;
          i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
          i3 = 0u;
          i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
          l2 = i2;
          i0 = i2 ? i0 : i1;
          l8 = i0;
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 240u);
          l9 = i0;
          i1 = l4;
          i2 = 240u;
          i1 += i2;
          l10 = i1;
          i2 = l2;
          i0 = i2 ? i0 : i1;
          l15 = i0;
          i0 = l3;
          l2 = i0;
        }
        loop_300: 
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l2;
            i0 = LoadU8(ctx, i0 + 27u);
            l10 = i0;
            i1 = 24u;
            i0 <<= (i1 & 31);
            i1 = 24u;
            i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
            i1 = 0u;
            i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
            l14 = i0;
            i0 = l2;
            i0 = LoadU32(ctx, i0 + 20u);
            i1 = l10;
            i2 = l14;
            i0 = i2 ? i0 : i1;
            l10 = i0;
            i1 = l8;
            i0 = i0 < i1;
            l16 = i0;
            i0 = l10;
            i1 = l8;
            i2 = l16;
            i0 = i2 ? i0 : i1;
            l9 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l9;
            if (i0) {
              i0 = l15;
              i1 = l2;
              i2 = 16u;
              i1 += i2;
              l7 = i1;
              i1 = LoadU32(ctx, i1);
              i2 = l7;
              i3 = l14;
              i1 = i3 ? i1 : i2;
              l14 = i1;
              i2 = l9;
              i0 = CompareBytes(ctx, i0, i1, i2);
              l7 = i0;
              i0 = !(i0);
              if (i0) {
                i0 = l8;
                i1 = l10;
                i0 = i0 >= i1;
                l10 = i0;
                if (i0) {goto block_308;}
                goto block_302;
              }
              i0 = l7;
              i1 = kF3293ConstFFFFFFFF;
              i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
              l10 = i0;
              if (i0) {goto block_302;}
              block_308:;
              i0 = l14;
              i1 = l15;
              i2 = l9;
              i0 = CompareBytes(ctx, i0, i1, i2);
              l10 = i0;
              i0 = !(i0);
              l9 = i0;
              if (i0) {goto block_306;}
              i0 = l10;
              i1 = kF3293ConstFFFFFFFF;
              i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
              l9 = i0;
              if (i0) {goto block_305;}
              goto block_303;
            }
            i0 = l8;
            i1 = l10;
            i0 = i0 < i1;
            l9 = i0;
            if (i0) {goto block_302;}
            block_306:;
            i0 = l16;
            if (i0) {goto block_303;}
            block_305:;
            i0 = l4;
            i1 = l4;
            i1 = LoadU32(ctx, i1 + 324u);
            i2 = l4;
            i2 = LoadS8(ctx, i2 + 331u);
            l3 = i2;
            i3 = 255u;
            i2 &= i3;
            i3 = l3;
            i4 = 0u;
            i3 = (uint32_t)((int32_t)i3 < (int32_t)i4);
            i1 = i3 ? i1 : i2;
            l2 = i1;
            i2 = 0u;
            i1 = i1 != i2;
            l3 = i1;
            StoreU8(ctx, i0 + 203u, i1);
            i0 = l2;
            if (i0) {
              i0 = l4;
              i1 = 192u;
              i0 += i1;
              i1 = kF3293ConstB8389;
              i2 = l3;
              i0 = CopyBytes(ctx, i0, i1, i2);
            }
            i0 = l3;
            i1 = l4;
            i2 = 192u;
            i1 += i2;
            i0 |= i1;
            i1 = 0u;
            StoreU8(ctx, i0, i1);
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = l4;
            i2 = 256u;
            i1 += i2;
            StoreU32(ctx, i0 + 80u, i1);
            i0 = l4;
            i1 = 80u;
            i0 += i1;
            l8 = i0;
            i0 = l4;
            i1 = kF3293ConstFFFFFFC0;
            i0 -= i1;
            l9 = i0;
            i0 = l4;
            i1 = 96u;
            i0 += i1;
            l2 = i0;
            i0 = l4;
            i1 = 256u;
            i0 += i1;
            l3 = i0;
          }
          i0 = l5;
          i1 = 47u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 1640u;
            i1 = l2;
            i2 = l12;
            i3 = l3;
            i4 = kF3293ConstB84F3;
            i5 = l8;
            i6 = l9;
            HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
            i0 = 47u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 1u;
            i0 = i0 != i1;
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
              i0 = l4;
              i0 = LoadU32(ctx, i0 + 96u);
              l3 = i0;
              i1 = 28u;
              i0 += i1;
              l2 = i0;
              i0 = LoadU32(ctx, i0);
              l8 = i0;
              i0 = l3;
              i0 = LoadU32(ctx, i0 + 32u);
              l13 = i0;
              i0 = l3;
              i0 = LoadU8(ctx, i0 + 39u);
              l3 = i0;
              i0 = kF3293ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = l4;
              i1 = 192u;
              i0 += i1;
              l10 = i0;
              i0 = l8;
              i1 = l2;
              l9 = i1;
              i2 = l3;
              i3 = 24u;
              i2 <<= (i3 & 31);
              i3 = 24u;
              i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
              i3 = 0u;
              i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
              l2 = i2;
              i0 = i2 ? i0 : i1;
              l8 = i0;
              i0 = l13;
              i1 = l3;
              i2 = l2;
              i0 = i2 ? i0 : i1;
              l2 = i0;
            }
            i0 = l5;
            i1 = 48u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = 860u;
              i1 = l10;
              i2 = l8;
              i3 = l2;
              i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
              l7 = i0;
              i0 = 48u;
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
              i0 = kF3293ConstD015C;
              i0 = LoadU32(ctx, i0);
              l2 = i0;
              i0 = kF3293ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = l2;
              i1 = 1u;
              i0 = i0 != i1;
              l2 = i0;
              if (i0) {goto block_299;}
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = HostRet0(ctx);
            b = i0;
            i0 = HostTempRet0(ctx);
            a = i0;
            goto block_298;
          }
          block_303:;
          i0 = l2;
          i1 = l2;
          i2 = 4u;
          i1 += i2;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          l2 = i0;
          block_302:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l2;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            if (i0) {goto loop_300;}
          }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 224u);
          l9 = i0;
          i1 = l4;
          i2 = 224u;
          i1 += i2;
          i2 = l4;
          i2 = LoadU8(ctx, i2 + 235u);
          l2 = i2;
          i3 = 24u;
          i2 <<= (i3 & 31);
          i3 = 24u;
          i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
          i3 = 0u;
          i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
          l8 = i2;
          i0 = i2 ? i0 : i1;
          l10 = i0;
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 228u);
          i1 = l2;
          i2 = l8;
          i0 = i2 ? i0 : i1;
          l2 = i0;
        }
        loop_344: 
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            i0 = LoadU8(ctx, i0 + 27u);
            l8 = i0;
            i1 = 24u;
            i0 <<= (i1 & 31);
            i1 = 24u;
            i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
            i1 = 0u;
            i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
            l14 = i0;
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 20u);
            i1 = l8;
            i2 = l14;
            i0 = i2 ? i0 : i1;
            l8 = i0;
            i1 = l2;
            i0 = i0 < i1;
            l16 = i0;
            i0 = l8;
            i1 = l2;
            i2 = l16;
            i0 = i2 ? i0 : i1;
            l9 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l9;
            if (i0) {
              i0 = l10;
              i1 = l3;
              i2 = 16u;
              i1 += i2;
              l7 = i1;
              i1 = LoadU32(ctx, i1);
              i2 = l7;
              i3 = l14;
              i1 = i3 ? i1 : i2;
              l14 = i1;
              i2 = l9;
              i0 = CompareBytes(ctx, i0, i1, i2);
              l7 = i0;
              i0 = !(i0);
              if (i0) {
                i0 = l2;
                i1 = l8;
                i0 = i0 >= i1;
                l8 = i0;
                if (i0) {goto block_352;}
                goto block_346;
              }
              i0 = l7;
              i1 = kF3293ConstFFFFFFFF;
              i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
              l8 = i0;
              if (i0) {goto block_346;}
              block_352:;
              i0 = l14;
              i1 = l10;
              i2 = l9;
              i0 = CompareBytes(ctx, i0, i1, i2);
              l8 = i0;
              i0 = !(i0);
              l9 = i0;
              if (i0) {goto block_350;}
              i0 = l8;
              i1 = kF3293ConstFFFFFFFF;
              i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
              l8 = i0;
              if (i0) {goto block_349;}
              goto block_347;
            }
            i0 = l2;
            i1 = l8;
            i0 = i0 < i1;
            l8 = i0;
            if (i0) {goto block_346;}
            block_350:;
            i0 = l16;
            if (i0) {goto block_347;}
            block_349:;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = 128u;
            i0 += i1;
            l3 = i0;
            i1 = kF3293ConstAE298;
            StoreU32(ctx, i0, i1);
            i0 = kF3293ConstCF12C;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = l3;
            j1 = 0ull;
            StoreU64(ctx, i0 + 8u, j1);
            i0 = l3;
            i1 = l2;
            StoreU32(ctx, i0 + 4u, i1);
            i0 = l3;
            j1 = 0ull;
            StoreU64(ctx, i0 + 16u, j1);
            i0 = l3;
            l8 = i0;
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 1u;
            i0 = i0 == i1;
            l2 = i0;
            if (i0) {goto block_342;}
            i0 = l4;
            i1 = kF3293Const90680;
            StoreU32(ctx, i0 + 152u, i1);
            i0 = l4;
            j1 = 0ull;
            StoreU64(ctx, i0 + 177u, j1);
            i0 = l4;
            j1 = 0ull;
            StoreU64(ctx, i0 + 172u, j1);
            i0 = l4;
            j1 = 0ull;
            StoreU64(ctx, i0 + 164u, j1);
            i0 = l4;
            j1 = 0ull;
            StoreU64(ctx, i0 + 156u, j1);
            i0 = l4;
            i1 = kF3293ConstB90E8;
            StoreU32(ctx, i0 + 128u, i1);
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = l4;
            i2 = 224u;
            i1 += i2;
            StoreU32(ctx, i0 + 208u, i1);
            i0 = l4;
            i1 = 208u;
            i0 += i1;
            l9 = i0;
            i0 = l4;
            i1 = 192u;
            i0 += i1;
            l10 = i0;
            i0 = l4;
            i1 = 40u;
            i0 += i1;
            l2 = i0;
            i0 = l4;
            i1 = 224u;
            i0 += i1;
            l3 = i0;
          }
          i0 = l5;
          i1 = 49u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 1640u;
            i1 = l2;
            i2 = l12;
            i3 = l3;
            i4 = kF3293ConstB84F3;
            i5 = l9;
            i6 = l10;
            HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
            i0 = 49u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 1u;
            i0 = i0 == i1;
            l2 = i0;
            if (i0) {goto block_328;}
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 40u);
            l3 = i0;
            i1 = 28u;
            i0 += i1;
            l2 = i0;
            i0 = l3;
            i0 = LoadS8(ctx, i0 + 39u);
            i1 = kF3293ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            if (i0) {
              i0 = l2;
              i0 = LoadU32(ctx, i0);
              l2 = i0;
            }
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = 128u;
            i0 += i1;
            l3 = i0;
          }
          i0 = l5;
          i1 = 50u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 2647u;
            i1 = l3;
            i2 = l2;
            i0 = HostOsQuery(ctx, i0, i1, i2);
            l7 = i0;
            i0 = 50u;
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
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 == i1;
            l2 = i0;
            if (i0) {goto block_328;}
            i0 = l3;
            if (i0) {goto block_330;}
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = l4;
            i1 = LoadU32(ctx, i1 + 172u);
            StoreU32(ctx, i0 + 40u, i1);
            i0 = l4;
            i1 = 120u;
            i0 += i1;
            l2 = i0;
            i0 = l4;
            i1 = 40u;
            i0 += i1;
            l3 = i0;
          }
          i0 = l5;
          i1 = 51u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 1287u;
            i1 = l2;
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
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 1u;
            i0 = i0 == i1;
            l2 = i0;
            if (i0) {goto block_331;}
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 120u);
            l3 = i0;
            if (i0) {goto block_343;}
            i0 = kF3293ConstD015C;
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
            i0 = 857u;
            i1 = kF3293MsgPx;
            i2 = kF3293MsgBoostBoostSmartPtr;
            i3 = 734u;
            i4 = kF3293MsgOperator;
            HostFailAssertion(ctx, i0, i1, i2, i3, i4);
            i0 = 52u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_23;}
            goto running_exit;
          }
          block_347:;
          i0 = l3;
          i1 = l3;
          i2 = 4u;
          i1 += i2;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          l3 = i0;
          block_346:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            if (i0) {goto loop_344;}
          }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = a;
          i0 = LoadU32(ctx, i0 + 4u);
          l3 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = l13;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = l15;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = l4;
          i1 = l4;
          i1 = LoadU32(ctx, i1 + 224u);
          i2 = l4;
          i3 = 224u;
          i2 += i3;
          l8 = i2;
          i3 = l4;
          i3 = LoadS8(ctx, i3 + 235u);
          i4 = 0u;
          i3 = (uint32_t)((int32_t)i3 < (int32_t)i4);
          l9 = i3;
          i1 = i3 ? i1 : i2;
          l2 = i1;
          StoreU32(ctx, i0 + 8u, i1);
        }
        i0 = l5;
        i1 = 53u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2590u;
          i1 = l3;
          i2 = 1u;
          i3 = b;
          i4 = kF3293ConstB83F7;
          i5 = l4;
          HostInitResources(ctx, i0, i1, i2, i3, i4, i5);
          i0 = 53u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {goto block_289;}
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_288;
        }
        block_343:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 12u);
          l2 = i0;
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 4u);
          l3 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = 128u;
          i0 += i1;
          l9 = i0;
          i0 = l2;
          i1 = l3;
          i0 += i1;
          l2 = i0;
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 172u);
          l3 = i0;
        }
        i0 = l5;
        i1 = 54u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2052u;
          i1 = l9;
          i2 = l2;
          i3 = l3;
          i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
          l7 = i0;
          i0 = 54u;
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
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 == i1;
          l2 = i0;
          if (i0) {goto block_341;}
          i0 = kF3293ConstD015C;
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
          i0 = 1032u;
          i1 = 48u;
          i0 = HostFoldLayoutOffset(ctx, i0, i1);
          l7 = i0;
          i0 = 55u;
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
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 == i1;
          l2 = i0;
          if (i0) {goto block_340;}
          i0 = l4;
          i1 = l3;
          StoreU32(ctx, i0 + 80u, i1);
          i0 = l4;
          j1 = 9223372243013206054ull;
          StoreU64(ctx, i0 + 84u, j1);
          i0 = l3;
          i1 = 0u;
          StoreU8(ctx, i0 + 38u, i1);
          i0 = l3;
          i1 = kF3293ConstB83AB;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0 + 30u, j1);
          i0 = l3;
          i1 = kF3293ConstB83A5;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0 + 24u, j1);
          i0 = l3;
          i1 = kF3293ConstB839D;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0 + 16u, j1);
          i0 = l3;
          i1 = kF3293ConstB8395;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0 + 8u, j1);
          i0 = l3;
          i1 = kF3293MsgContentDispositionFormData;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0, j1);
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = l4;
          i2 = 256u;
          i1 += i2;
          StoreU32(ctx, i0 + 64u, i1);
          i0 = l4;
          i1 = kF3293ConstFFFFFFC0;
          i0 -= i1;
          l9 = i0;
          i0 = l4;
          i1 = 472u;
          i0 += i1;
          l10 = i0;
          i0 = l4;
          i1 = 40u;
          i0 += i1;
          l2 = i0;
          i0 = l4;
          i1 = 256u;
          i0 += i1;
          l3 = i0;
        }
        i0 = l5;
        i1 = 56u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1640u;
          i1 = l2;
          i2 = l12;
          i3 = l3;
          i4 = kF3293ConstB84F3;
          i5 = l9;
          i6 = l10;
          HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
          i0 = 56u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 == i1;
          l2 = i0;
          if (i0) {goto block_339;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 40u);
          l3 = i0;
          i1 = 28u;
          i0 += i1;
          l2 = i0;
          i0 = LoadU32(ctx, i0);
          l13 = i0;
          i0 = l3;
          i0 = LoadU32(ctx, i0 + 32u);
          l15 = i0;
          i0 = l3;
          i0 = LoadU8(ctx, i0 + 39u);
          l3 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = 80u;
          i0 += i1;
          l10 = i0;
          i0 = l13;
          i1 = l2;
          i2 = l3;
          i3 = 24u;
          i2 <<= (i3 & 31);
          i3 = 24u;
          i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
          i3 = 0u;
          i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
          l2 = i2;
          i0 = i2 ? i0 : i1;
          l9 = i0;
          i0 = l15;
          i1 = l3;
          i2 = l2;
          i0 = i2 ? i0 : i1;
          l2 = i0;
        }
        i0 = l5;
        i1 = 57u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 860u;
          i1 = l10;
          i2 = l9;
          i3 = l2;
          i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
          l7 = i0;
          i0 = 57u;
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
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 == i1;
          l2 = i0;
          if (i0) {goto block_339;}
          i0 = l4;
          i1 = l3;
          i1 = LoadU32(ctx, i1 + 8u);
          StoreU32(ctx, i0 + 104u, i1);
          i0 = l4;
          i1 = l3;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0 + 96u, j1);
          i0 = l3;
          j1 = 0ull;
          StoreU64(ctx, i0, j1);
          i0 = l3;
          i1 = 0u;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = 96u;
          i0 += i1;
          l2 = i0;
        }
        i0 = l5;
        i1 = 58u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 997u;
          i1 = l2;
          i2 = kF3293MsgFilename;
          i0 = HostOsQuery(ctx, i0, i1, i2);
          l7 = i0;
          i0 = 58u;
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
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 == i1;
          l2 = i0;
          if (i0) {goto block_338;}
          i0 = l4;
          i1 = l3;
          i1 = LoadU32(ctx, i1 + 8u);
          StoreU32(ctx, i0 + 200u, i1);
          i0 = l4;
          i1 = l3;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0 + 192u, j1);
          i0 = l3;
          j1 = 0ull;
          StoreU64(ctx, i0, j1);
          i0 = l3;
          i1 = 0u;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = l4;
          i2 = 224u;
          i1 += i2;
          StoreU32(ctx, i0 + 464u, i1);
          i0 = l4;
          i1 = 464u;
          i0 += i1;
          l9 = i0;
          i0 = l4;
          i1 = 456u;
          i0 += i1;
          l10 = i0;
          i0 = l4;
          i1 = 472u;
          i0 += i1;
          l2 = i0;
          i0 = l4;
          i1 = 224u;
          i0 += i1;
          l3 = i0;
        }
        i0 = l5;
        i1 = 59u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 1640u;
          i1 = l2;
          i2 = l12;
          i3 = l3;
          i4 = kF3293ConstB84F3;
          i5 = l9;
          i6 = l10;
          HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
          i0 = 59u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 == i1;
          l2 = i0;
          if (i0) {goto block_337;}
          i0 = l4;
          i1 = kF3293Const8B830;
          StoreU32(ctx, i0 + 40u, i1);
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 472u);
          i1 = 28u;
          i0 += i1;
          l2 = i0;
        }
        i0 = l5;
        i1 = 60u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 873u;
          i1 = l17;
          i2 = l2;
          i0 = HostOsQuery(ctx, i0, i1, i2);
          l7 = i0;
          i0 = 60u;
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
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 == i1;
          l2 = i0;
          if (i0) {goto block_337;}
          i0 = l4;
          i1 = 47u;
          StoreU8(ctx, i0 + 56u, i1);
          i0 = l4;
          i1 = kF3293ConstCF12C;
          i1 = LoadU32(ctx, i1);
          StoreU32(ctx, i0 + 60u, i1);
          i0 = l3;
          i1 = 47u;
          i0 = FindLastCharInString(ctx, i0, i1);
          l2 = i0;
          i1 = kF3293ConstFFFFFFFF;
          i0 = i0 == i1;
          l9 = i0;
        }
        i0 = l9;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = kF3293ConstFFFFFFC0;
            i0 -= i1;
            l2 = i0;
          }
          i0 = l5;
          i1 = 61u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 873u;
            i1 = l2;
            i2 = l3;
            i0 = HostOsQuery(ctx, i0, i1, i2);
            l7 = i0;
            i0 = 61u;
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
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 1u;
            i0 = i0 != i1;
            l2 = i0;
            if (i0) {goto block_382;}
            goto block_327;
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = kF3293ConstFFFFFFC0;
          i0 -= i1;
          l9 = i0;
          i0 = l2;
          i1 = 1u;
          i0 += i1;
          l2 = i0;
        }
        i0 = l5;
        i1 = 62u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 854u;
          i1 = l9;
          i2 = l3;
          i3 = l2;
          i4 = kF3293ConstFFFFFFFF;
          i5 = l17;
          i0 = HostInvokeIiiiii(ctx, i0, i1, i2, i3, i4, i5);
          l7 = i0;
          i0 = 62u;
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
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 == i1;
          l2 = i0;
          if (i0) {goto block_327;}
        }
        block_382:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = 192u;
          i0 += i1;
          l9 = i0;
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 64u);
          i1 = l4;
          i2 = kF3293ConstFFFFFFC0;
          i1 -= i2;
          i2 = l4;
          i2 = LoadU8(ctx, i2 + 75u);
          l3 = i2;
          i3 = 24u;
          i2 <<= (i3 & 31);
          i3 = 24u;
          i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
          i3 = 0u;
          i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
          l2 = i2;
          i0 = i2 ? i0 : i1;
          l10 = i0;
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 68u);
          l13 = i0;
          i1 = l3;
          i2 = l2;
          i0 = i2 ? i0 : i1;
          l2 = i0;
        }
        i0 = l5;
        i1 = 63u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 860u;
          i1 = l9;
          i2 = l10;
          i3 = l2;
          i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
          l7 = i0;
          i0 = 63u;
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
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 == i1;
          l2 = i0;
          if (i0) {goto block_336;}
          i0 = l4;
          i1 = l3;
          i1 = LoadU32(ctx, i1 + 8u);
          StoreU32(ctx, i0 + 216u, i1);
          i0 = l4;
          i1 = l3;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0 + 208u, j1);
          i0 = l3;
          j1 = 0ull;
          StoreU64(ctx, i0, j1);
          i0 = l3;
          i1 = 0u;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = 120u;
          i0 += i1;
          l9 = i0;
          i0 = l4;
          i1 = 112u;
          i0 += i1;
          l2 = i0;
          i0 = l4;
          i1 = 208u;
          i0 += i1;
          l3 = i0;
        }
        i0 = l5;
        i1 = 64u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 3128u;
          i1 = l2;
          i2 = l3;
          i3 = l9;
          HostSyncSlot(ctx, i0, i1, i2, i3);
          i0 = 64u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 == i1;
          l2 = i0;
          if (i0) {goto block_334;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 308u);
          l3 = i0;
          i1 = l4;
          i1 = LoadU32(ctx, i1 + 312u);
          i0 = i0 < i1;
          l2 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 112u);
            l2 = i0;
            i0 = l3;
            i1 = 0u;
            StoreU32(ctx, i0 + 4u, i1);
            i0 = l3;
            i1 = l2;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 116u);
            l2 = i0;
            i0 = l4;
            i1 = 0u;
            StoreU32(ctx, i0 + 116u, i1);
            i0 = l3;
            i1 = l2;
            StoreU32(ctx, i0 + 4u, i1);
            i0 = l4;
            i1 = l3;
            i2 = 8u;
            i1 += i2;
            l2 = i1;
            StoreU32(ctx, i0 + 308u, i1);
            i0 = l4;
            i1 = 0u;
            StoreU32(ctx, i0 + 112u, i1);
            goto block_395;
          }
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = 304u;
          i0 += i1;
          l2 = i0;
          i0 = l4;
          i1 = 112u;
          i0 += i1;
          l3 = i0;
        }
        i0 = l5;
        i1 = 65u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 3129u;
          i1 = l2;
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
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 == i1;
          l2 = i0;
          if (i0) {goto block_335;}
        }
        block_395:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 116u);
          l3 = i0;
          i0 = !(i0);
          l2 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          if (i0) {goto block_401;}
          i0 = l3;
          i1 = l3;
          i1 = LoadU32(ctx, i1 + 4u);
          l2 = i1;
          i2 = 1u;
          i1 -= i2;
          l9 = i1;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {goto block_401;}
          i0 = l3;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 8u);
          l2 = i0;
          i0 = kF3293ConstD015C;
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
          i0 = l2;
          i1 = l3;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 66u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 == i1;
          if (i0) {goto running_exit;}
          i0 = l3;
          i1 = l3;
          i1 = LoadU32(ctx, i1 + 8u);
          l2 = i1;
          i2 = 1u;
          i1 -= i2;
          l9 = i1;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {goto block_401;}
          i0 = l3;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 12u);
          l2 = i0;
          i0 = kF3293ConstD015C;
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
          i0 = l2;
          i1 = l3;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 67u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 == i1;
          l2 = i0;
          if (i0) {goto running_exit;}
        }
        block_401:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 219u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 208u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 75u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 64u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l4;
          i1 = kF3293Const8B830;
          StoreU32(ctx, i0 + 40u, i1);
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 55u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 44u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 203u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 192u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 107u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 96u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 91u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 80u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 292u);
          l3 = i0;
          i1 = l4;
          i1 = LoadU32(ctx, i1 + 296u);
          i0 = i0 != i1;
          l2 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          if (i0) {
            i0 = l3;
            i1 = l4;
            i1 = LoadU32(ctx, i1 + 120u);
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = l4;
            i1 = LoadU32(ctx, i1 + 124u);
            l2 = i1;
            StoreU32(ctx, i0 + 4u, i1);
            i0 = l2;
            if (i0) {
              i0 = l2;
              i1 = l2;
              i1 = LoadU32(ctx, i1 + 4u);
              i2 = 1u;
              i1 += i2;
              l9 = i1;
              StoreU32(ctx, i0 + 4u, i1);
            }
            i0 = l4;
            i1 = l3;
            i2 = 8u;
            i1 += i2;
            l2 = i1;
            StoreU32(ctx, i0 + 292u, i1);
            goto block_414;
          }
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = 288u;
          i0 += i1;
          l2 = i0;
          i0 = l4;
          i1 = 120u;
          i0 += i1;
          l3 = i0;
        }
        i0 = l5;
        i1 = 68u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 3130u;
          i1 = l2;
          i2 = l3;
          HostOsSchedule(ctx, i0, i1, i2);
          i0 = 68u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 == i1;
          l2 = i0;
          if (i0) {goto block_341;}
        }
        block_414:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 124u);
          l3 = i0;
          i0 = !(i0);
          l2 = i0;
          if (i0) {goto block_329;}
          i0 = l3;
          i1 = l3;
          i1 = LoadU32(ctx, i1 + 4u);
          l2 = i1;
          i2 = 1u;
          i1 -= i2;
          l9 = i1;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {goto block_329;}
          i0 = l3;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 8u);
          l2 = i0;
          i0 = kF3293ConstD015C;
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
          i0 = l2;
          i1 = l3;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 69u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 != i1;
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
            i0 = l3;
            i1 = l3;
            i1 = LoadU32(ctx, i1 + 8u);
            l2 = i1;
            i2 = 1u;
            i1 -= i2;
            l9 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 != i1;
            l2 = i0;
            if (i0) {goto block_329;}
            i0 = l3;
            i0 = LoadU32(ctx, i0);
            i0 = LoadU32(ctx, i0 + 12u);
            l2 = i0;
            i0 = kF3293ConstD015C;
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
            i0 = l2;
            i1 = l3;
            HostReleaseSlotPair(ctx, i0, i1);
            i0 = 70u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 1u;
            i0 = i0 != i1;
            l2 = i0;
            if (i0) {goto block_329;}
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto running_exit;}
        block_342:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_288;
        }
        block_341:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_322;
        }
        block_340:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_322;
        }
        block_339:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_323;
        }
        block_338:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_324;
        }
        block_337:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_325;
        }
        block_336:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_332;
        }
        block_335:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 116u);
          a = i0;
          i0 = !(i0);
          l2 = i0;
          if (i0) {goto block_333;}
          i0 = a;
          i1 = a;
          i1 = LoadU32(ctx, i1 + 4u);
          l3 = i1;
          i2 = 1u;
          i1 -= i2;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {goto block_333;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = LoadU32(ctx, i0 + 8u);
          l3 = i0;
          i0 = kF3293ConstD015C;
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
          i0 = l3;
          i1 = a;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 71u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 != i1;
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
            i0 = a;
            i1 = a;
            i1 = LoadU32(ctx, i1 + 8u);
            l3 = i1;
            i2 = 1u;
            i1 -= i2;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = l3;
            i1 = 1u;
            i0 = i0 != i1;
            l2 = i0;
            if (i0) {goto block_333;}
            i0 = a;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = LoadU32(ctx, i0 + 12u);
            l3 = i0;
            i0 = kF3293ConstD015C;
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
            i0 = l3;
            i1 = a;
            HostReleaseSlotPair(ctx, i0, i1);
            i0 = 72u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_333;}
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto running_exit;}
        block_334:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
        }
        block_333:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 219u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_332;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 208u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
        }
        block_332:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 75u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_326;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 64u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
          goto block_326;
        }
        block_331:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_321;
        }
        block_330:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = a;
          i0 = LoadU32(ctx, i0 + 4u);
          l3 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = l4;
          i1 = LoadU32(ctx, i1 + 224u);
          i2 = l4;
          i3 = 224u;
          i2 += i3;
          l9 = i2;
          i3 = l4;
          i3 = LoadS8(ctx, i3 + 235u);
          i4 = 0u;
          i3 = (uint32_t)((int32_t)i3 < (int32_t)i4);
          l10 = i3;
          i1 = i3 ? i1 : i2;
          StoreU32(ctx, i0 + 16u, i1);
          i0 = l4;
          i1 = 16u;
          i0 += i1;
          l2 = i0;
        }
        i0 = l5;
        i1 = 73u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 2590u;
          i1 = l3;
          i2 = 1u;
          i3 = b;
          i4 = kF3293ConstB83C4;
          i5 = l2;
          HostInitResources(ctx, i0, i1, i2, i3, i4, i5);
          i0 = 73u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 == i1;
          l2 = i0;
          if (i0) {goto block_328;}
        }
        block_329:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i1 = kF3293Const90610;
          StoreU32(ctx, i0 + 128u, i1);
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 180u);
          l3 = i0;
          i0 = !(i0);
          l2 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          if (i0) {goto block_451;}
          i0 = l3;
          i1 = l3;
          i1 = LoadU32(ctx, i1 + 4u);
          l2 = i1;
          i2 = 1u;
          i1 -= i2;
          l9 = i1;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {goto block_451;}
          i0 = l3;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 8u);
          l2 = i0;
          i0 = kF3293ConstD015C;
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
          i0 = l2;
          i1 = l3;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 74u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 == i1;
          if (i0) {goto running_exit;}
          i0 = l3;
          i1 = l3;
          i1 = LoadU32(ctx, i1 + 8u);
          l2 = i1;
          i2 = 1u;
          i1 -= i2;
          l9 = i1;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {goto block_451;}
          i0 = l3;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 12u);
          l2 = i0;
          i0 = kF3293ConstD015C;
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
          i0 = l2;
          i1 = l3;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 75u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 == i1;
          l2 = i0;
          if (i0) {goto running_exit;}
        }
        block_451:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i1 = kF3293Const90680;
          StoreU32(ctx, i0 + 152u, i1);
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 167u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          l2 = i0;
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 156u);
            l2 = i0;
            DeallocateRawBlock(ctx, i0);
          }
        }
        i0 = l5;
        i1 = 76u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l8;
          i0 = DestroyFileObject(ctx, i0);
          l7 = i0;
          i0 = 76u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l7;
          l2 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto block_289;}
        block_328:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_321;
        }
        block_327:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
        }
        block_326:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i1 = kF3293Const8B830;
          StoreU32(ctx, i0 + 40u, i1);
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 55u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_325;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 44u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
        }
        block_325:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 203u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_324;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 192u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
        }
        block_324:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 107u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_323;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 96u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
        }
        block_323:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 91u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_322;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 80u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
        }
        block_322:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 124u);
          a = i0;
          i0 = !(i0);
          l2 = i0;
          if (i0) {goto block_321;}
          i0 = a;
          i1 = a;
          i1 = LoadU32(ctx, i1 + 4u);
          l3 = i1;
          i2 = 1u;
          i1 -= i2;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {goto block_321;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = LoadU32(ctx, i0 + 8u);
          l3 = i0;
          i0 = kF3293ConstD015C;
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
          i0 = l3;
          i1 = a;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 77u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 != i1;
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
            i0 = a;
            i1 = a;
            i1 = LoadU32(ctx, i1 + 8u);
            l3 = i1;
            i2 = 1u;
            i1 -= i2;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = l3;
            i1 = 1u;
            i0 = i0 != i1;
            l2 = i0;
            if (i0) {goto block_321;}
            i0 = a;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = LoadU32(ctx, i0 + 12u);
            l3 = i0;
            i0 = kF3293ConstD015C;
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
            i0 = l3;
            i1 = a;
            HostReleaseSlotPair(ctx, i0, i1);
            i0 = 78u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_321;}
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto running_exit;}
        block_321:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i1 = kF3293Const90610;
          StoreU32(ctx, i0 + 128u, i1);
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 180u);
          a = i0;
          i0 = !(i0);
          l2 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          if (i0) {goto block_476;}
          i0 = a;
          i1 = a;
          i1 = LoadU32(ctx, i1 + 4u);
          l3 = i1;
          i2 = 1u;
          i1 -= i2;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {goto block_476;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = LoadU32(ctx, i0 + 8u);
          l3 = i0;
          i0 = kF3293ConstD015C;
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
          i0 = l3;
          i1 = a;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 79u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 == i1;
          l2 = i0;
          if (i0) {goto block_475;}
          i0 = a;
          i1 = a;
          i1 = LoadU32(ctx, i1 + 8u);
          l3 = i1;
          i2 = 1u;
          i1 -= i2;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {goto block_476;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = LoadU32(ctx, i0 + 12u);
          l3 = i0;
          i0 = kF3293ConstD015C;
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
          i0 = l3;
          i1 = a;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 80u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 == i1;
          a = i0;
          if (i0) {goto block_475;}
        }
        block_476:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i1 = kF3293Const90680;
          StoreU32(ctx, i0 + 152u, i1);
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 167u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          a = i0;
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 156u);
            a = i0;
            DeallocateRawBlock(ctx, i0);
          }
        }
        i0 = l5;
        i1 = 81u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l8;
          i0 = DestroyFileObject(ctx, i0);
          l7 = i0;
          i0 = 81u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
          i0 = l7;
          a = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto block_288;}
        block_475:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto running_exit;}
        block_299:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i1 = l3;
          i1 = LoadU32(ctx, i1 + 8u);
          StoreU32(ctx, i0 + 216u, i1);
          i0 = l4;
          i1 = l3;
          j1 = LoadU64(ctx, i1);
          StoreU64(ctx, i0 + 208u, j1);
          i0 = l3;
          j1 = 0ull;
          StoreU64(ctx, i0, j1);
          i0 = l3;
          i1 = 0u;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l4;
          i1 = 208u;
          i0 += i1;
          l2 = i0;
        }
        i0 = l5;
        i1 = 82u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 997u;
          i1 = l2;
          i2 = kF3293ConstB838B;
          i0 = HostOsQuery(ctx, i0, i1, i2);
          l7 = i0;
          i0 = 82u;
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
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 != i1;
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
            i0 = l4;
            i1 = l3;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 48u, i1);
            i0 = l4;
            i1 = l3;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 40u, j1);
            i0 = l3;
            j1 = 0ull;
            StoreU64(ctx, i0, j1);
            i0 = l3;
            i1 = 0u;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = l4;
            i2 = 240u;
            i1 += i2;
            StoreU32(ctx, i0 + 80u, i1);
            i0 = l4;
            i1 = 80u;
            i0 += i1;
            l8 = i0;
            i0 = l4;
            i1 = kF3293ConstFFFFFFC0;
            i0 -= i1;
            l9 = i0;
            i0 = l4;
            i1 = 96u;
            i0 += i1;
            l2 = i0;
            i0 = l4;
            i1 = 240u;
            i0 += i1;
            l3 = i0;
          }
          i0 = l5;
          i1 = 83u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 1640u;
            i1 = l2;
            i2 = l12;
            i3 = l3;
            i4 = kF3293ConstB84F3;
            i5 = l8;
            i6 = l9;
            HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
            i0 = 83u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 1u;
            i0 = i0 == i1;
            l2 = i0;
            if (i0) {goto block_490;}
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 96u);
            l3 = i0;
            i1 = 28u;
            i0 += i1;
            l2 = i0;
            i0 = LoadU32(ctx, i0);
            l8 = i0;
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 32u);
            l13 = i0;
            i0 = l3;
            i0 = LoadU8(ctx, i0 + 39u);
            l3 = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = 40u;
            i0 += i1;
            l10 = i0;
            i0 = l8;
            i1 = l2;
            l9 = i1;
            i2 = l3;
            i3 = 24u;
            i2 <<= (i3 & 31);
            i3 = 24u;
            i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
            i3 = 0u;
            i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
            l2 = i2;
            i0 = i2 ? i0 : i1;
            l8 = i0;
            i0 = l13;
            i1 = l3;
            i2 = l2;
            i0 = i2 ? i0 : i1;
            l2 = i0;
          }
          i0 = l5;
          i1 = 84u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 860u;
            i1 = l10;
            i2 = l8;
            i3 = l2;
            i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
            l7 = i0;
            i0 = 84u;
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
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 == i1;
            l2 = i0;
            if (i0) {goto block_490;}
            i0 = l4;
            i1 = l3;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 136u, i1);
            i0 = l4;
            i1 = l3;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 128u, j1);
            i0 = l3;
            j1 = 0ull;
            StoreU64(ctx, i0, j1);
            i0 = l3;
            i1 = 0u;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = 320u;
            i0 += i1;
            l8 = i0;
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 128u);
            i1 = l4;
            i2 = 128u;
            i1 += i2;
            i2 = l4;
            i2 = LoadU8(ctx, i2 + 139u);
            l3 = i2;
            i3 = 24u;
            i2 <<= (i3 & 31);
            i3 = 24u;
            i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
            i3 = 0u;
            i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
            l2 = i2;
            i0 = i2 ? i0 : i1;
            l9 = i0;
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 132u);
            l10 = i0;
            i1 = l3;
            i2 = l2;
            i0 = i2 ? i0 : i1;
            l2 = i0;
          }
          i0 = l5;
          i1 = 85u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 860u;
            i1 = l8;
            i2 = l9;
            i3 = l2;
            i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
            l7 = i0;
            i0 = 85u;
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
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 1u;
            i0 = i0 == i1;
            l2 = i0;
            if (i0) {goto block_489;}
            i0 = l4;
            i0 = LoadS8(ctx, i0 + 139u);
            i1 = kF3293ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            if (i0) {
              i0 = l4;
              i0 = LoadU32(ctx, i0 + 128u);
              DeallocateRawBlock(ctx, i0);
            }
            i0 = l4;
            i0 = LoadS8(ctx, i0 + 51u);
            i1 = kF3293ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            if (i0) {
              i0 = l4;
              i0 = LoadU32(ctx, i0 + 40u);
              DeallocateRawBlock(ctx, i0);
            }
            i0 = l4;
            i0 = LoadS8(ctx, i0 + 219u);
            i1 = kF3293ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            if (i0) {
              i0 = l4;
              i0 = LoadU32(ctx, i0 + 208u);
              DeallocateRawBlock(ctx, i0);
            }
            i0 = l4;
            i0 = LoadS8(ctx, i0 + 203u);
            i1 = kF3293ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            l2 = i0;
            if (i0) {
              i0 = l4;
              i0 = LoadU32(ctx, i0 + 192u);
              l2 = i0;
              DeallocateRawBlock(ctx, i0);
            }
            i0 = kF3293ConstD015C;
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
            i1 = 48u;
            i0 = HostFoldLayoutOffset(ctx, i0, i1);
            l7 = i0;
            i0 = 86u;
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
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 == i1;
            l2 = i0;
            if (i0) {goto block_297;}
            i0 = l4;
            i1 = l3;
            StoreU32(ctx, i0 + 208u, i1);
            i0 = l4;
            j1 = 9223372243013206054ull;
            StoreU64(ctx, i0 + 212u, j1);
            i0 = l3;
            i1 = 0u;
            StoreU8(ctx, i0 + 38u, i1);
            i0 = l3;
            i1 = kF3293ConstB83AB;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 30u, j1);
            i0 = l3;
            i1 = kF3293ConstB83A5;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 24u, j1);
            i0 = l3;
            i1 = kF3293ConstB839D;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 16u, j1);
            i0 = l3;
            i1 = kF3293ConstB8395;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 8u, j1);
            i0 = l3;
            i1 = kF3293MsgContentDispositionFormData;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = l4;
            i2 = 256u;
            i1 += i2;
            StoreU32(ctx, i0 + 80u, i1);
            i0 = l4;
            i1 = 80u;
            i0 += i1;
            l8 = i0;
            i0 = l4;
            i1 = kF3293ConstFFFFFFC0;
            i0 -= i1;
            l9 = i0;
            i0 = l4;
            i1 = 192u;
            i0 += i1;
            l2 = i0;
            i0 = l4;
            i1 = 256u;
            i0 += i1;
            l3 = i0;
          }
          i0 = l5;
          i1 = 87u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 1640u;
            i1 = l2;
            i2 = l12;
            i3 = l3;
            i4 = kF3293ConstB84F3;
            i5 = l8;
            i6 = l9;
            HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
            i0 = 87u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 1u;
            i0 = i0 == i1;
            l2 = i0;
            if (i0) {goto block_296;}
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 192u);
            l3 = i0;
            i1 = 28u;
            i0 += i1;
            l2 = i0;
            i0 = LoadU32(ctx, i0);
            l8 = i0;
            i0 = l3;
            i0 = LoadU32(ctx, i0 + 32u);
            l13 = i0;
            i0 = l3;
            i0 = LoadU8(ctx, i0 + 39u);
            l3 = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = 208u;
            i0 += i1;
            l10 = i0;
            i0 = l8;
            i1 = l2;
            l9 = i1;
            i2 = l3;
            i3 = 24u;
            i2 <<= (i3 & 31);
            i3 = 24u;
            i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
            i3 = 0u;
            i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
            l2 = i2;
            i0 = i2 ? i0 : i1;
            l8 = i0;
            i0 = l13;
            i1 = l3;
            i2 = l2;
            i0 = i2 ? i0 : i1;
            l2 = i0;
          }
          i0 = l5;
          i1 = 88u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 860u;
            i1 = l10;
            i2 = l8;
            i3 = l2;
            i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
            l7 = i0;
            i0 = 88u;
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
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 == i1;
            l2 = i0;
            if (i0) {goto block_296;}
            i0 = l4;
            i1 = l3;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 48u, i1);
            i0 = l4;
            i1 = l3;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 40u, j1);
            i0 = l3;
            j1 = 0ull;
            StoreU64(ctx, i0, j1);
            i0 = l3;
            i1 = 0u;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = 40u;
            i0 += i1;
            l2 = i0;
          }
          i0 = l5;
          i1 = 89u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 997u;
            i1 = l2;
            i2 = kF3293ConstB83B4;
            i0 = HostOsQuery(ctx, i0, i1, i2);
            l7 = i0;
            i0 = 89u;
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
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 == i1;
            l2 = i0;
            if (i0) {goto block_295;}
            i0 = l4;
            i1 = l3;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 136u, i1);
            i0 = l4;
            i1 = l3;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 128u, j1);
            i0 = l3;
            j1 = 0ull;
            StoreU64(ctx, i0, j1);
            i0 = l3;
            i1 = 0u;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = l4;
            i2 = 240u;
            i1 += i2;
            StoreU32(ctx, i0 + 80u, i1);
            i0 = l4;
            i1 = 80u;
            i0 += i1;
            l8 = i0;
            i0 = l4;
            i1 = kF3293ConstFFFFFFC0;
            i0 -= i1;
            l9 = i0;
            i0 = l4;
            i1 = 192u;
            i0 += i1;
            l2 = i0;
            i0 = l4;
            i1 = 240u;
            i0 += i1;
            l3 = i0;
          }
          i0 = l5;
          i1 = 90u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 1640u;
            i1 = l2;
            i2 = l12;
            i3 = l3;
            i4 = kF3293ConstB84F3;
            i5 = l8;
            i6 = l9;
            HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
            i0 = 90u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 1u;
            i0 = i0 == i1;
            l2 = i0;
            if (i0) {goto block_293;}
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 192u);
            i1 = 28u;
            i0 += i1;
            l8 = i0;
            i0 = l4;
            i1 = 96u;
            i0 += i1;
            l2 = i0;
            i0 = l4;
            i1 = 128u;
            i0 += i1;
            l3 = i0;
          }
          i0 = l5;
          i1 = 91u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 3131u;
            i1 = l2;
            i2 = l3;
            i3 = l8;
            HostSyncSlot(ctx, i0, i1, i2, i3);
            i0 = 91u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 1u;
            i0 = i0 == i1;
            l2 = i0;
            if (i0) {goto block_293;}
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 308u);
            l3 = i0;
            i1 = l4;
            i1 = LoadU32(ctx, i1 + 312u);
            i0 = i0 < i1;
            l2 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l2;
            if (i0) {
              i0 = l4;
              i0 = LoadU32(ctx, i0 + 96u);
              l2 = i0;
              i0 = l3;
              i1 = 0u;
              StoreU32(ctx, i0 + 4u, i1);
              i0 = l3;
              i1 = l2;
              StoreU32(ctx, i0, i1);
              i0 = l4;
              i0 = LoadU32(ctx, i0 + 100u);
              l2 = i0;
              i0 = l4;
              i1 = 0u;
              StoreU32(ctx, i0 + 100u, i1);
              i0 = l3;
              i1 = l2;
              StoreU32(ctx, i0 + 4u, i1);
              i0 = l4;
              i1 = l3;
              i2 = 8u;
              i1 += i2;
              l2 = i1;
              StoreU32(ctx, i0 + 308u, i1);
              i0 = l4;
              i1 = 0u;
              StoreU32(ctx, i0 + 96u, i1);
              goto block_517;
            }
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = 304u;
            i0 += i1;
            l2 = i0;
            i0 = l4;
            i1 = 96u;
            i0 += i1;
            l3 = i0;
          }
          i0 = l5;
          i1 = 92u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = 3129u;
            i1 = l2;
            i2 = l3;
            HostOsSchedule(ctx, i0, i1, i2);
            i0 = 92u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 1u;
            i0 = i0 == i1;
            l2 = i0;
            if (i0) {goto block_294;}
          }
          block_517:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 100u);
            l3 = i0;
            i0 = !(i0);
            l2 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l2;
            if (i0) {goto block_523;}
            i0 = l3;
            i1 = l3;
            i1 = LoadU32(ctx, i1 + 4u);
            l2 = i1;
            i2 = 1u;
            i1 -= i2;
            l8 = i1;
            StoreU32(ctx, i0 + 4u, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 != i1;
            l2 = i0;
            if (i0) {goto block_523;}
            i0 = l3;
            i0 = LoadU32(ctx, i0);
            i0 = LoadU32(ctx, i0 + 8u);
            l2 = i0;
            i0 = kF3293ConstD015C;
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
            i0 = l2;
            i1 = l3;
            HostReleaseSlotPair(ctx, i0, i1);
            i0 = 93u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 == i1;
            l2 = i0;
            if (i0) {goto block_287;}
            i0 = l3;
            i1 = l3;
            i1 = LoadU32(ctx, i1 + 8u);
            l2 = i1;
            i2 = 1u;
            i1 -= i2;
            l8 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = l2;
            i1 = 1u;
            i0 = i0 != i1;
            l2 = i0;
            if (i0) {goto block_523;}
            i0 = l3;
            i0 = LoadU32(ctx, i0);
            i0 = LoadU32(ctx, i0 + 12u);
            l2 = i0;
            i0 = kF3293ConstD015C;
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
            i0 = l2;
            i1 = l3;
            HostReleaseSlotPair(ctx, i0, i1);
            i0 = 94u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = l3;
            i1 = 1u;
            i0 = i0 == i1;
            l2 = i0;
            if (i0) {goto block_287;}
          }
          block_523:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l4;
            i0 = LoadS8(ctx, i0 + 139u);
            i1 = kF3293ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            if (i0) {
              i0 = l4;
              i0 = LoadU32(ctx, i0 + 128u);
              DeallocateRawBlock(ctx, i0);
            }
            i0 = l4;
            i0 = LoadS8(ctx, i0 + 51u);
            i1 = kF3293ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            if (i0) {
              i0 = l4;
              i0 = LoadU32(ctx, i0 + 40u);
              DeallocateRawBlock(ctx, i0);
            }
            i0 = l4;
            i0 = LoadS8(ctx, i0 + 219u);
            i1 = kF3293ConstFFFFFFFF;
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            l2 = i0;
            if (i0) {goto block_289;}
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 208u);
            l2 = i0;
            DeallocateRawBlock(ctx, i0);
            goto block_289;
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_487;
        }
        block_490:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_488;
        }
        block_489:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 139u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_488;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 128u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
        }
        block_488:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 51u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_487;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 40u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
        }
        block_487:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 219u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_298;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 208u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
        }
        block_298:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 203u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_288;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 192u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
          goto block_288;
        }
        block_297:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_288;
        }
        block_296:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_290;
        }
        block_295:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
          goto block_291;
        }
        block_294:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 100u);
          a = i0;
          i0 = !(i0);
          l2 = i0;
          if (i0) {goto block_292;}
          i0 = a;
          i1 = a;
          i1 = LoadU32(ctx, i1 + 4u);
          l3 = i1;
          i2 = 1u;
          i1 -= i2;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {goto block_292;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = LoadU32(ctx, i0 + 8u);
          l3 = i0;
          i0 = kF3293ConstD015C;
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
          i0 = l3;
          i1 = a;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 95u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l3;
          i1 = 1u;
          i0 = i0 != i1;
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
            i0 = a;
            i1 = a;
            i1 = LoadU32(ctx, i1 + 8u);
            l3 = i1;
            i2 = 1u;
            i1 -= i2;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = l3;
            i1 = 1u;
            i0 = i0 != i1;
            l2 = i0;
            if (i0) {goto block_292;}
            i0 = a;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = LoadU32(ctx, i0 + 12u);
            l3 = i0;
            i0 = kF3293ConstD015C;
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
            i0 = l3;
            i1 = a;
            HostReleaseSlotPair(ctx, i0, i1);
            i0 = 96u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = kF3293ConstD015C;
            i0 = LoadU32(ctx, i0);
            a = i0;
            i0 = kF3293ConstD015C;
            i1 = 0u;
            StoreU32(ctx, i0, i1);
            i0 = a;
            i1 = 1u;
            i0 = i0 != i1;
            a = i0;
            if (i0) {goto block_292;}
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto running_exit;}
        block_293:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = HostRet0(ctx);
          b = i0;
          i0 = HostTempRet0(ctx);
          a = i0;
        }
        block_292:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 139u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_291;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 128u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
        }
        block_291:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 51u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_290;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 40u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
        }
        block_290:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 219u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_288;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 208u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
          goto block_288;
        }
        block_289:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 235u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 224u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 251u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 240u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 267u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 256u);
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 283u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
          l2 = i0;
          if (i0) {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 272u);
            l2 = i0;
            DeallocateRawBlock(ctx, i0);
          }
          i0 = l6;
          i1 = 1u;
          i0 += i1;
          l6 = i0;
          goto loop_262;
        }
        block_288:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i0 = LoadS8(ctx, i0 + 235u);
          i1 = kF3293ConstFFFFFFFF;
          i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
          a = i0;
          if (i0) {goto block_21;}
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 224u);
          a = i0;
          DeallocateRawBlock(ctx, i0);
          goto block_21;
        }
        block_287:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto running_exit;}
        block_280:;
        i0 = l2;
        i1 = l2;
        i2 = 4u;
        i1 += i2;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        l2 = i0;
        block_279:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          if (i0) {goto loop_277;}
        }
    }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 235u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 224u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 251u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 240u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 267u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 256u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 283u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 272u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 292u);
    l2 = i0;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 288u);
    i0 = i0 == i1;
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
      i0 = kF3293ConstD015C;
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
      i0 = 1032u;
      i1 = 16u;
      i0 = HostFoldLayoutOffset(ctx, i0, i1);
      l7 = i0;
      i0 = 97u;
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
      i0 = kF3293ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 != i1;
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
        i0 = l4;
        i1 = b;
        StoreU32(ctx, i0 + 128u, i1);
        i0 = l4;
        j1 = 9223372105574252556ull;
        StoreU64(ctx, i0 + 132u, j1);
        i0 = b;
        i1 = 0u;
        StoreU8(ctx, i0 + 12u, i1);
        i0 = b;
        i1 = kF3293ConstB8466;
        i1 = LoadU32(ctx, i1);
        StoreU32(ctx, i0 + 8u, i1);
        i0 = b;
        i1 = kF3293MsgContentType;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0, j1);
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = l4;
        i2 = 128u;
        i1 += i2;
        StoreU32(ctx, i0 + 272u, i1);
        i0 = l4;
        i1 = 336u;
        i0 += i1;
        l2 = i0;
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        l3 = i0;
        i0 = l4;
        i1 = 272u;
        i0 += i1;
        l6 = i0;
        i0 = l4;
        i1 = 256u;
        i0 += i1;
        l8 = i0;
        i0 = l4;
        i1 = 40u;
        i0 += i1;
        b = i0;
      }
      i0 = l5;
      i1 = 98u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1959u;
        i1 = b;
        i2 = l2;
        i3 = l3;
        i4 = kF3293ConstB84F3;
        i5 = l6;
        i6 = l8;
        HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
        i0 = 98u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto block_573;}
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 40u);
        i1 = 28u;
        i0 += i1;
        b = i0;
      }
      i0 = l5;
      i1 = 99u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 996u;
        i1 = b;
        i2 = kF3293MsgApplicationXWwwForm;
        i0 = HostOsQuery(ctx, i0, i1, i2);
        l7 = i0;
        i0 = 99u;
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
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto block_573;}
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 139u);
        i1 = kF3293ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 128u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = a;
        i0 = LoadU32(ctx, i0 + 12u);
        b = i0;
        i0 = !(i0);
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
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l5;
        i1 = 100u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3293MsgPx;
          i2 = kF3293MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3293MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 100u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_23;}
          goto running_exit;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 52u);
        l3 = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 336u;
        i0 += i1;
        l2 = i0;
      }
      i0 = l5;
      i1 = 101u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l3;
        i1 = b;
        i2 = l2;
        HostOsSchedule(ctx, i0, i1, i2);
        i0 = 101u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto block_566;}
        i0 = a;
        i0 = LoadU32(ctx, i0 + 12u);
        b = i0;
        i0 = !(i0);
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
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l5;
        i1 = 102u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3293MsgPx;
          i2 = kF3293MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3293MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 102u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_23;}
          goto running_exit;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 128u;
        i0 += i1;
        l2 = i0;
      }
      i0 = l5;
      i1 = 103u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 3126u;
        i1 = l2;
        i2 = l11;
        HostOsSchedule(ctx, i0, i1, i2);
        i0 = 103u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 == i1;
        l2 = i0;
        if (i0) {goto block_572;}
        i0 = b;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 28u);
        l3 = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        l2 = i0;
        i0 = b;
        l6 = i0;
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 128u);
        i1 = l4;
        i2 = 128u;
        i1 += i2;
        i2 = l4;
        i2 = LoadS8(ctx, i2 + 139u);
        i3 = 0u;
        i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
        i0 = i2 ? i0 : i1;
        l8 = i0;
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 320u);
        i1 = l4;
        i2 = 320u;
        i1 += i2;
        i2 = l4;
        i2 = LoadU8(ctx, i2 + 331u);
        b = i2;
        i3 = 24u;
        i2 <<= (i3 & 31);
        i3 = 24u;
        i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
        i3 = 0u;
        i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
        l3 = i2;
        i0 = i2 ? i0 : i1;
        l9 = i0;
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 324u);
        l12 = i0;
        i1 = b;
        i2 = l3;
        i0 = i2 ? i0 : i1;
        b = i0;
      }
      i0 = l5;
      i1 = 104u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l2;
        i1 = l6;
        i2 = l8;
        i3 = l9;
        i4 = b;
        i0 = HostInvokeIiiii(ctx, i0, i1, i2, i3, i4);
        l7 = i0;
        i0 = 104u;
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
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 == i1;
        l2 = i0;
        if (i0) {goto block_571;}
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 139u);
        i1 = kF3293ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        l2 = i0;
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 128u);
          l2 = i0;
          DeallocateRawBlock(ctx, i0);
        }
        i0 = b;
        i0 = !(i0);
        b = i0;
        if (i0) {goto block_567;}
        i0 = kF3293ConstD015C;
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
        i0 = 3127u;
        i1 = a;
        i2 = kF3293ConstFFFFFFFF;
        i3 = kF3293ConstB834E;
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
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto block_566;}
        i0 = a;
        i1 = 0u;
        StoreU32(ctx, i0 + 12u, i1);
        i0 = a;
        i0 = LoadU32(ctx, i0 + 16u);
        b = i0;
        i0 = a;
        i1 = 0u;
        StoreU32(ctx, i0 + 16u, i1);
        i0 = b;
        i0 = !(i0);
        a = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        if (i0) {goto block_603;}
        i0 = b;
        i1 = b;
        i1 = LoadU32(ctx, i1 + 4u);
        a = i1;
        i2 = 1u;
        i1 -= i2;
        l2 = i1;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_603;}
        i0 = b;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 8u);
        a = i0;
        i0 = kF3293ConstD015C;
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
        i0 = a;
        i1 = b;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 106u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 == i1;
        a = i0;
        if (i0) {goto running_exit;}
        i0 = b;
        i1 = b;
        i1 = LoadU32(ctx, i1 + 8u);
        a = i1;
        i2 = 1u;
        i1 -= i2;
        l2 = i1;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_603;}
        i0 = b;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        a = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l5;
      i1 = 107u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = b;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 107u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 == i1;
        a = i0;
        if (i0) {goto running_exit;}
      }
      block_603:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 288u);
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
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 292u);
          l3 = i0;
          i1 = b;
          i0 = i0 == i1;
          l2 = i0;
          i0 = b;
          a = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        i1 = 0u;
        i2 = l2;
        i0 = i2 ? i0 : i1;
        i0 = !(i0);
        if (i0) {
          loop_614: 
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = l3;
              i1 = 4u;
              i0 -= i1;
              i0 = LoadU32(ctx, i0);
              a = i0;
              i0 = !(i0);
              l2 = i0;
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = l2;
              if (i0) {goto block_616;}
              i0 = a;
              i1 = a;
              i1 = LoadU32(ctx, i1 + 4u);
              l2 = i1;
              i2 = 1u;
              i1 -= i2;
              l6 = i1;
              StoreU32(ctx, i0 + 4u, i1);
              i0 = l2;
              i1 = 1u;
              i0 = i0 != i1;
              l2 = i0;
              if (i0) {goto block_616;}
              i0 = a;
              i0 = LoadU32(ctx, i0);
              i0 = LoadU32(ctx, i0 + 8u);
              l2 = i0;
              i0 = kF3293ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
            }
            i0 = l5;
            i1 = 108u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = l2;
              i1 = a;
              HostReleaseSlotPair(ctx, i0, i1);
              i0 = 108u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3293ConstD015C;
              i0 = LoadU32(ctx, i0);
              l2 = i0;
              i0 = kF3293ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = l2;
              i1 = 1u;
              i0 = i0 == i1;
              if (i0) {goto running_exit;}
              i0 = a;
              i1 = a;
              i1 = LoadU32(ctx, i1 + 8u);
              l2 = i1;
              i2 = 1u;
              i1 -= i2;
              l6 = i1;
              StoreU32(ctx, i0 + 8u, i1);
              i0 = l2;
              i1 = 1u;
              i0 = i0 != i1;
              l2 = i0;
              if (i0) {goto block_616;}
              i0 = a;
              i0 = LoadU32(ctx, i0);
              i0 = LoadU32(ctx, i0 + 12u);
              l2 = i0;
              i0 = kF3293ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
            }
            i0 = l5;
            i1 = 109u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = l2;
              i1 = a;
              HostReleaseSlotPair(ctx, i0, i1);
              i0 = 109u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3293ConstD015C;
              i0 = LoadU32(ctx, i0);
              a = i0;
              i0 = kF3293ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = a;
              i1 = 1u;
              i0 = i0 == i1;
              a = i0;
              if (i0) {goto running_exit;}
            }
            block_616:;
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = l3;
              i1 = 8u;
              i0 -= i1;
              l3 = i0;
              i1 = b;
              i0 = i0 != i1;
              a = i0;
              if (i0) {goto loop_614;}
            }
          i0 = ResumePhase(ctx);
          if (i0) {
            i0 = a;
          } else {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 288u);
          }
          a = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i1 = b;
          StoreU32(ctx, i0 + 292u, i1);
          i0 = a;
          DeallocateRawBlock(ctx, i0);
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 304u);
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
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 308u);
          l3 = i0;
          i1 = b;
          i0 = i0 == i1;
          l2 = i0;
          i0 = b;
          a = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        i1 = 0u;
        i2 = l2;
        i0 = i2 ? i0 : i1;
        i0 = !(i0);
        if (i0) {
          loop_630: 
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = l3;
              i1 = 4u;
              i0 -= i1;
              i0 = LoadU32(ctx, i0);
              a = i0;
              i0 = !(i0);
              l2 = i0;
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = l2;
              if (i0) {goto block_632;}
              i0 = a;
              i1 = a;
              i1 = LoadU32(ctx, i1 + 4u);
              l2 = i1;
              i2 = 1u;
              i1 -= i2;
              l6 = i1;
              StoreU32(ctx, i0 + 4u, i1);
              i0 = l2;
              i1 = 1u;
              i0 = i0 != i1;
              l2 = i0;
              if (i0) {goto block_632;}
              i0 = a;
              i0 = LoadU32(ctx, i0);
              i0 = LoadU32(ctx, i0 + 8u);
              l2 = i0;
              i0 = kF3293ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
            }
            i0 = l5;
            i1 = 110u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = l2;
              i1 = a;
              HostReleaseSlotPair(ctx, i0, i1);
              i0 = 110u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3293ConstD015C;
              i0 = LoadU32(ctx, i0);
              l2 = i0;
              i0 = kF3293ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = l2;
              i1 = 1u;
              i0 = i0 == i1;
              if (i0) {goto running_exit;}
              i0 = a;
              i1 = a;
              i1 = LoadU32(ctx, i1 + 8u);
              l2 = i1;
              i2 = 1u;
              i1 -= i2;
              l6 = i1;
              StoreU32(ctx, i0 + 8u, i1);
              i0 = l2;
              i1 = 1u;
              i0 = i0 != i1;
              l2 = i0;
              if (i0) {goto block_632;}
              i0 = a;
              i0 = LoadU32(ctx, i0);
              i0 = LoadU32(ctx, i0 + 12u);
              l2 = i0;
              i0 = kF3293ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
            }
            i0 = l5;
            i1 = 111u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = l2;
              i1 = a;
              HostReleaseSlotPair(ctx, i0, i1);
              i0 = 111u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = kF3293ConstD015C;
              i0 = LoadU32(ctx, i0);
              a = i0;
              i0 = kF3293ConstD015C;
              i1 = 0u;
              StoreU32(ctx, i0, i1);
              i0 = a;
              i1 = 1u;
              i0 = i0 == i1;
              a = i0;
              if (i0) {goto running_exit;}
            }
            block_632:;
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = l3;
              i1 = 8u;
              i0 -= i1;
              l3 = i0;
              i1 = b;
              i0 = i0 != i1;
              a = i0;
              if (i0) {goto loop_630;}
            }
          i0 = ResumePhase(ctx);
          if (i0) {
            i0 = a;
          } else {
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 304u);
          }
          a = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l4;
          i1 = b;
          StoreU32(ctx, i0 + 308u, i1);
          i0 = a;
          DeallocateRawBlock(ctx, i0);
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 331u);
        i1 = kF3293ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
        a = i0;
        if (i0) {goto block_213;}
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 320u);
        a = i0;
        DeallocateRawBlock(ctx, i0);
        goto block_213;
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      a = i0;
      goto block_15;
    }
    block_573:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 139u);
      i1 = kF3293ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_15;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 128u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_15;
    }
    block_572:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      a = i0;
      goto block_15;
    }
    block_571:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 139u);
      i1 = kF3293ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_15;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 128u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_15;
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = l4;
    i1 = 240u;
    i0 += i1;
    b = i0;
  }
  i0 = l5;
  i1 = 112u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = 3132u;
    i1 = b;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 112u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3293ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
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
      i0 = l4;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 240u);
      StoreU32(ctx, i0 + 32u, i1);
      i0 = l4;
      i1 = l4;
      i1 = LoadU32(ctx, i1 + 244u);
      b = i1;
      StoreU32(ctx, i0 + 36u, i1);
      i0 = b;
      if (i0) {
        i0 = b;
        i1 = b;
        i1 = LoadU32(ctx, i1 + 4u);
        i2 = 1u;
        i1 += i2;
        StoreU32(ctx, i0 + 4u, i1);
      }
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l4;
      i1 = 32u;
      i0 += i1;
      l2 = i0;
      i0 = l4;
      i1 = 304u;
      i0 += i1;
      l3 = i0;
      i0 = l4;
      i1 = 128u;
      i0 += i1;
      b = i0;
    }
    i0 = l5;
    i1 = 113u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = 3133u;
      i1 = b;
      i2 = l2;
      i3 = l3;
      HostSyncSlot(ctx, i0, i1, i2, i3);
      i0 = 113u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3293ConstD015C;
      i0 = LoadU32(ctx, i0);
      b = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = b;
      i1 = 1u;
      i0 = i0 != i1;
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
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 36u);
        b = i0;
        i0 = !(i0);
        l2 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        if (i0) {goto block_674;}
        i0 = b;
        i1 = b;
        i1 = LoadU32(ctx, i1 + 4u);
        l3 = i1;
        i2 = 1u;
        i1 -= i2;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 != i1;
        l2 = i0;
        if (i0) {goto block_674;}
        i0 = b;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = LoadU32(ctx, i0 + 8u);
        l3 = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l5;
      i1 = 114u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l3;
        i1 = b;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 114u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto running_exit;}
        i0 = b;
        i1 = b;
        i1 = LoadU32(ctx, i1 + 8u);
        l3 = i1;
        i2 = 1u;
        i1 -= i2;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 != i1;
        l2 = i0;
        if (i0) {goto block_674;}
        i0 = b;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = LoadU32(ctx, i0 + 12u);
        l3 = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l5;
      i1 = 115u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l3;
        i1 = b;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 115u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto running_exit;}
      }
      block_674:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l5;
      i1 = 116u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1032u;
        i1 = 32u;
        i0 = HostFoldLayoutOffset(ctx, i0, i1);
        l7 = i0;
        i0 = 116u;
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
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 == i1;
        l2 = i0;
        if (i0) {goto block_669;}
        i0 = l4;
        i1 = b;
        StoreU32(ctx, i0 + 40u, i1);
        i0 = l4;
        j1 = 9223372174293729310ull;
        StoreU64(ctx, i0 + 44u, j1);
        i0 = b;
        i1 = 0u;
        StoreU8(ctx, i0 + 30u, i1);
        i0 = b;
        i1 = kF3293ConstB8481;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0 + 22u, j1);
        i0 = b;
        i1 = kF3293ConstB847B;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0 + 16u, j1);
        i0 = b;
        i1 = kF3293ConstB8473;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0 + 8u, j1);
        i0 = b;
        i1 = kF3293MsgMultipartFormDataBoundary;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0, j1);
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 40u;
        i0 += i1;
        l2 = i0;
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 128u);
        i1 = l4;
        i2 = 128u;
        i1 += i2;
        i2 = l4;
        i2 = LoadU8(ctx, i2 + 139u);
        b = i2;
        i3 = 24u;
        i2 <<= (i3 & 31);
        i3 = 24u;
        i2 = (uint32_t)((int32_t)i2 >> (i3 & 31));
        i3 = 0u;
        i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
        l3 = i2;
        i0 = i2 ? i0 : i1;
        l6 = i0;
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 132u);
        l8 = i0;
        i1 = b;
        i2 = l3;
        i0 = i2 ? i0 : i1;
        b = i0;
      }
      i0 = l5;
      i1 = 117u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 860u;
        i1 = l2;
        i2 = l6;
        i3 = b;
        i0 = HostDispatchInvoke(ctx, i0, i1, i2, i3);
        l7 = i0;
        i0 = 117u;
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
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 == i1;
        l2 = i0;
        if (i0) {goto block_668;}
        i0 = b;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        i0 = l4;
        i1 = b;
        i1 = LoadU32(ctx, i1 + 4u);
        StoreU32(ctx, i0 + 224u, i1);
        i0 = l4;
        i1 = b;
        i1 = LoadU32(ctx, i1 + 7u);
        StoreU32(ctx, i0 + 227u, i1);
        i0 = b;
        j1 = 0ull;
        StoreU64(ctx, i0, j1);
        i0 = b;
        i0 = LoadS8(ctx, i0 + 11u);
        l2 = i0;
        i0 = b;
        i1 = 0u;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l5;
      i1 = 118u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1032u;
        i1 = 16u;
        i0 = HostFoldLayoutOffset(ctx, i0, i1);
        l7 = i0;
        i0 = 118u;
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
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        l12 = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l12;
        i1 = 1u;
        i0 = i0 == i1;
        l6 = i0;
        if (i0) {goto block_667;}
        i0 = l4;
        i1 = b;
        StoreU32(ctx, i0 + 272u, i1);
        i0 = l4;
        j1 = 9223372105574252556ull;
        StoreU64(ctx, i0 + 276u, j1);
        i0 = b;
        i1 = 0u;
        StoreU8(ctx, i0 + 12u, i1);
        i0 = b;
        i1 = kF3293ConstB8466;
        i1 = LoadU32(ctx, i1);
        StoreU32(ctx, i0 + 8u, i1);
        i0 = b;
        i1 = kF3293MsgContentType;
        j1 = LoadU64(ctx, i1);
        StoreU64(ctx, i0, j1);
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = l4;
        i2 = 272u;
        i1 += i2;
        StoreU32(ctx, i0 + 208u, i1);
        i0 = l4;
        i1 = 336u;
        i0 += i1;
        l6 = i0;
        i0 = l4;
        i1 = 272u;
        i0 += i1;
        l8 = i0;
        i0 = l4;
        i1 = 208u;
        i0 += i1;
        l9 = i0;
        i0 = l4;
        i1 = 192u;
        i0 += i1;
        l12 = i0;
        i0 = l4;
        i1 = 256u;
        i0 += i1;
        b = i0;
      }
      i0 = l5;
      i1 = 119u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 1959u;
        i1 = b;
        i2 = l6;
        i3 = l8;
        i4 = kF3293ConstB84F3;
        i5 = l9;
        i6 = l12;
        HostInvokeViiiiii(ctx, i0, i1, i2, i3, i4, i5, i6);
        i0 = 119u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto block_666;}
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 256u);
        b = i0;
        i0 = LoadS8(ctx, i0 + 39u);
        i1 = kF3293ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        l6 = i0;
        if (i0) {
          i0 = b;
          i0 = LoadU32(ctx, i0 + 28u);
          l6 = i0;
          DeallocateRawBlock(ctx, i0);
        }
        i0 = b;
        i1 = l3;
        StoreU32(ctx, i0 + 28u, i1);
        i0 = b;
        i1 = l4;
        i1 = LoadU32(ctx, i1 + 224u);
        StoreU32(ctx, i0 + 32u, i1);
        i0 = b;
        i1 = l4;
        i1 = LoadU32(ctx, i1 + 227u);
        l3 = i1;
        StoreU32(ctx, i0 + 35u, i1);
        i0 = b;
        i1 = l2;
        StoreU8(ctx, i0 + 39u, i1);
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 283u);
        i1 = kF3293ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 272u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 51u);
        i1 = kF3293ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 40u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = a;
        i0 = LoadU32(ctx, i0 + 12u);
        b = i0;
        i0 = !(i0);
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
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l5;
        i1 = 120u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3293MsgPx;
          i2 = kF3293MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3293MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 120u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_23;}
          goto running_exit;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 52u);
        l3 = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 336u;
        i0 += i1;
        l2 = i0;
      }
      i0 = l5;
      i1 = 121u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l3;
        i1 = b;
        i2 = l2;
        HostOsSchedule(ctx, i0, i1, i2);
        i0 = 121u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto block_663;}
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 240u);
        b = i0;
        i0 = !(i0);
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
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l5;
        i1 = 122u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3293MsgPx;
          i2 = kF3293MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3293MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 122u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_23;}
          goto running_exit;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 272u;
        i0 += i1;
        l2 = i0;
      }
      i0 = l5;
      i1 = 123u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 3134u;
        i1 = l2;
        i2 = b;
        HostOsSchedule(ctx, i0, i1, i2);
        i0 = 123u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto block_658;}
        i0 = a;
        i0 = LoadU32(ctx, i0 + 12u);
        l3 = i0;
        i0 = !(i0);
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
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l5;
        i1 = 124u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3293MsgPx;
          i2 = kF3293MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3293MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 124u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_23;}
          goto running_exit;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i1 = 40u;
        i0 += i1;
        b = i0;
      }
      i0 = l5;
      i1 = 125u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 3126u;
        i1 = b;
        i2 = l11;
        HostOsSchedule(ctx, i0, i1, i2);
        i0 = 125u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto block_662;}
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 272u);
        b = i0;
        i0 = !(i0);
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
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l5;
        i1 = 126u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = 857u;
          i1 = kF3293MsgPx;
          i2 = kF3293MsgBoostBoostSmartPtr;
          i3 = 734u;
          i4 = kF3293MsgOperator;
          HostFailAssertion(ctx, i0, i1, i2, i3, i4);
          i0 = 126u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_23;}
          goto running_exit;
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        i0 = LoadU32(ctx, i0 + 16u);
        l2 = i0;
        i0 = l3;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 28u);
        l12 = i0;
        i0 = b;
        i0 = LoadU32(ctx, i0 + 12u);
        l6 = i0;
        i0 = b;
        i0 = LoadU32(ctx, i0 + 4u);
        b = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 40u);
        i1 = l4;
        i2 = 40u;
        i1 += i2;
        i2 = l4;
        i2 = LoadS8(ctx, i2 + 51u);
        i3 = 0u;
        i2 = (uint32_t)((int32_t)i2 < (int32_t)i3);
        i0 = i2 ? i0 : i1;
        l8 = i0;
        i0 = b;
        i1 = l6;
        i0 += i1;
        b = i0;
      }
      i0 = l5;
      i1 = 127u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l12;
        i1 = l3;
        i2 = l8;
        i3 = b;
        i4 = l2;
        i0 = HostInvokeIiiii(ctx, i0, i1, i2, i3, i4);
        l7 = i0;
        i0 = 127u;
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
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 == i1;
        l2 = i0;
        if (i0) {goto block_661;}
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 51u);
        i1 = kF3293ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        l2 = i0;
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 40u);
          l2 = i0;
          DeallocateRawBlock(ctx, i0);
        }
        i0 = b;
        i0 = !(i0);
        b = i0;
        if (i0) {goto block_648;}
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l5;
      i1 = 128u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = 3127u;
        i1 = a;
        i2 = kF3293ConstFFFFFFFF;
        i3 = kF3293ConstB834E;
        HostSyncSlot(ctx, i0, i1, i2, i3);
        i0 = 128u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        b = i0;
        if (i0) {goto block_660;}
        i0 = a;
        i1 = 0u;
        StoreU32(ctx, i0 + 12u, i1);
        i0 = a;
        i0 = LoadU32(ctx, i0 + 16u);
        b = i0;
        i0 = a;
        i1 = 0u;
        StoreU32(ctx, i0 + 16u, i1);
        i0 = b;
        i0 = !(i0);
        a = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = a;
        if (i0) {goto block_726;}
        i0 = b;
        i1 = b;
        i1 = LoadU32(ctx, i1 + 4u);
        a = i1;
        i2 = 1u;
        i1 -= i2;
        l2 = i1;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_726;}
        i0 = b;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 8u);
        a = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l5;
      i1 = 129u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = b;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 129u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 == i1;
        a = i0;
        if (i0) {goto running_exit;}
        i0 = b;
        i1 = b;
        i1 = LoadU32(ctx, i1 + 8u);
        a = i1;
        i2 = 1u;
        i1 -= i2;
        l2 = i1;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_726;}
        i0 = b;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        a = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l5;
      i1 = 130u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = a;
        i1 = b;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 130u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 == i1;
        a = i0;
        if (i0) {goto running_exit;}
      }
      block_726:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 276u);
        a = i0;
        i0 = !(i0);
        b = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = b;
        if (i0) {goto block_733;}
        i0 = a;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 4u);
        b = i1;
        i2 = 1u;
        i1 -= i2;
        l2 = i1;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 != i1;
        b = i0;
        if (i0) {goto block_733;}
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 8u);
        b = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l5;
      i1 = 131u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = b;
        i1 = a;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 131u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 == i1;
        if (i0) {goto running_exit;}
        i0 = a;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 8u);
        b = i1;
        i2 = 1u;
        i1 -= i2;
        l2 = i1;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 != i1;
        b = i0;
        if (i0) {goto block_733;}
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 12u);
        b = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l5;
      i1 = 132u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = b;
        i1 = a;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 132u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 == i1;
        a = i0;
        if (i0) {goto running_exit;}
      }
      block_733:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l4;
        i0 = LoadS8(ctx, i0 + 139u);
        i1 = kF3293ConstFFFFFFFF;
        i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
        if (i0) {
          i0 = l4;
          i0 = LoadU32(ctx, i0 + 128u);
          DeallocateRawBlock(ctx, i0);
        }
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 244u);
        a = i0;
        i0 = !(i0);
        b = i0;
        if (i0) {goto block_565;}
        i0 = a;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 4u);
        b = i1;
        i2 = 1u;
        i1 -= i2;
        l2 = i1;
        StoreU32(ctx, i0 + 4u, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 != i1;
        b = i0;
        if (i0) {goto block_565;}
        i0 = a;
        i0 = LoadU32(ctx, i0);
        i0 = LoadU32(ctx, i0 + 8u);
        b = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l5;
      i1 = 133u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = b;
        i1 = a;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 133u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        b = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = b;
        i1 = 1u;
        i0 = i0 != i1;
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
          i0 = a;
          i1 = a;
          i1 = LoadU32(ctx, i1 + 8u);
          b = i1;
          i2 = 1u;
          i1 -= i2;
          l2 = i1;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = b;
          i1 = 1u;
          i0 = i0 != i1;
          b = i0;
          if (i0) {goto block_565;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 12u);
          b = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l5;
        i1 = 134u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = b;
          i1 = a;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 134u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto block_565;}
        }
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {goto running_exit;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 36u);
      a = i0;
      i0 = !(i0);
      l2 = i0;
      if (i0) {goto block_655;}
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 4u);
      l3 = i1;
      i2 = 1u;
      i1 -= i2;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_655;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = LoadU32(ctx, i0 + 8u);
      l3 = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 135u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l3;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 135u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3293ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 != i1;
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
        i0 = a;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 8u);
        l3 = i1;
        i2 = 1u;
        i1 -= i2;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 != i1;
        l2 = i0;
        if (i0) {goto block_655;}
        i0 = a;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = LoadU32(ctx, i0 + 12u);
        l3 = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l5;
      i1 = 136u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l3;
        i1 = a;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 136u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_655;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto running_exit;}
    block_669:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = l4;
      i1 = 128u;
      i0 += i1;
      l2 = i0;
      i0 = HostTempRet0(ctx);
      a = i0;
      goto block_656;
    }
    block_668:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      a = i0;
      goto block_664;
    }
    block_667:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      a = i0;
      goto block_665;
    }
    block_666:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 283u);
      i1 = kF3293ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_665;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 272u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
    }
    block_665:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i1 = kF3293ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_664;}
      i0 = l3;
      DeallocateRawBlock(ctx, i0);
    }
    block_664:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = 128u;
      i0 += i1;
      l2 = i0;
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 51u);
      i1 = kF3293ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_656;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 40u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_656;
    }
    block_663:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = l4;
      i1 = 128u;
      i0 += i1;
      l2 = i0;
      i0 = HostTempRet0(ctx);
      a = i0;
      goto block_656;
    }
    block_662:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      a = i0;
      goto block_659;
    }
    block_661:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      i0 = l4;
      i0 = LoadS8(ctx, i0 + 51u);
      i1 = kF3293ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_659;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 40u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
      goto block_659;
    }
    block_660:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      a = i0;
    }
    block_659:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 276u);
      a = i0;
      i0 = !(i0);
      l2 = i0;
      if (i0) {goto block_657;}
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 4u);
      l3 = i1;
      i2 = 1u;
      i1 -= i2;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_657;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = LoadU32(ctx, i0 + 8u);
      l3 = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 137u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l3;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 137u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3293ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 != i1;
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
        i0 = a;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 8u);
        l3 = i1;
        i2 = 1u;
        i1 -= i2;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 != i1;
        l2 = i0;
        if (i0) {goto block_657;}
        i0 = a;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = LoadU32(ctx, i0 + 12u);
        l3 = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l5;
      i1 = 138u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l3;
        i1 = a;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 138u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_657;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto running_exit;}
    block_658:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = HostRet0(ctx);
      b = i0;
      i0 = HostTempRet0(ctx);
      a = i0;
    }
    block_657:;
    i0 = l2;
    i1 = l4;
    i2 = 128u;
    i1 += i2;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    l2 = i0;
    block_656:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l2;
      i0 = LoadS8(ctx, i0 + 11u);
      i1 = kF3293ConstFFFFFFFF;
      i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
      a = i0;
      if (i0) {goto block_655;}
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 128u);
      a = i0;
      DeallocateRawBlock(ctx, i0);
    }
    block_655:;
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 244u);
      a = i0;
      i0 = !(i0);
      l2 = i0;
      if (i0) {goto block_15;}
      i0 = a;
      i1 = a;
      i1 = LoadU32(ctx, i1 + 4u);
      l3 = i1;
      i2 = 1u;
      i1 -= i2;
      StoreU32(ctx, i0 + 4u, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 != i1;
      l2 = i0;
      if (i0) {goto block_15;}
      i0 = a;
      i0 = LoadU32(ctx, i0);
      l2 = i0;
      i0 = LoadU32(ctx, i0 + 8u);
      l3 = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
    }
    i0 = l5;
    i1 = 139u;
    i0 = i0 == i1;
    i1 = 1u;
    i2 = ResumePhase(ctx);
    i0 = i2 ? i0 : i1;
    if (i0) {
      i0 = l3;
      i1 = a;
      HostReleaseSlotPair(ctx, i0, i1);
      i0 = 139u;
      i1 = ResumePhase(ctx);
      i2 = 1u;
      i1 = i1 == i2;
      if (i1) {goto spill_frame;}
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = kF3293ConstD015C;
      i0 = LoadU32(ctx, i0);
      l3 = i0;
      i0 = kF3293ConstD015C;
      i1 = 0u;
      StoreU32(ctx, i0, i1);
      i0 = l3;
      i1 = 1u;
      i0 = i0 != i1;
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
        i0 = a;
        i1 = a;
        i1 = LoadU32(ctx, i1 + 8u);
        l3 = i1;
        i2 = 1u;
        i1 -= i2;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l3;
        i1 = 1u;
        i0 = i0 != i1;
        l2 = i0;
        if (i0) {goto block_15;}
        i0 = a;
        i0 = LoadU32(ctx, i0);
        l2 = i0;
        i0 = LoadU32(ctx, i0 + 12u);
        l3 = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
      }
      i0 = l5;
      i1 = 140u;
      i0 = i0 == i1;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = l3;
        i1 = a;
        HostReleaseSlotPair(ctx, i0, i1);
        i0 = 140u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = kF3293ConstD015C;
        i0 = LoadU32(ctx, i0);
        a = i0;
        i0 = kF3293ConstD015C;
        i1 = 0u;
        StoreU32(ctx, i0, i1);
        i0 = a;
        i1 = 1u;
        i0 = i0 != i1;
        a = i0;
        if (i0) {goto block_15;}
      }
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {goto running_exit;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_15;
  }
  block_648:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 276u);
    a = i0;
    i0 = !(i0);
    b = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
    if (i0) {goto block_786;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 4u);
    b = i1;
    i2 = 1u;
    i1 -= i2;
    l2 = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_786;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l5;
  i1 = 141u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = a;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 141u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3293ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto running_exit;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 8u);
    b = i1;
    i2 = 1u;
    i1 -= i2;
    l2 = i1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_786;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    b = i0;
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l5;
  i1 = 142u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = a;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 142u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3293ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto running_exit;}
  }
  block_786:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 139u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 128u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 244u);
    a = i0;
    i0 = !(i0);
    b = i0;
    if (i0) {goto block_567;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 4u);
    b = i1;
    i2 = 1u;
    i1 -= i2;
    l2 = i1;
    StoreU32(ctx, i0 + 4u, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_567;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 8u);
    b = i0;
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l5;
  i1 = 143u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = a;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 143u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3293ConstD015C;
    i0 = LoadU32(ctx, i0);
    b = i0;
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto running_exit;}
    i0 = a;
    i1 = a;
    i1 = LoadU32(ctx, i1 + 8u);
    b = i1;
    i2 = 1u;
    i1 -= i2;
    l2 = i1;
    StoreU32(ctx, i0 + 8u, i1);
    i0 = b;
    i1 = 1u;
    i0 = i0 != i1;
    b = i0;
    if (i0) {goto block_567;}
    i0 = a;
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0 + 12u);
    b = i0;
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
  }
  i0 = l5;
  i1 = 144u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = b;
    i1 = a;
    HostReleaseSlotPair(ctx, i0, i1);
    i0 = 144u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = kF3293ConstD015C;
    i0 = LoadU32(ctx, i0);
    a = i0;
    i0 = kF3293ConstD015C;
    i1 = 0u;
    StoreU32(ctx, i0, i1);
    i0 = a;
    i1 = 1u;
    i0 = i0 == i1;
    a = i0;
    if (i0) {goto running_exit;}
  }
  block_567:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 288u);
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
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 292u);
      l3 = i0;
      i1 = b;
      i0 = i0 == i1;
      l2 = i0;
      i0 = b;
      a = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    i1 = 0u;
    i2 = l2;
    i0 = i2 ? i0 : i1;
    i0 = !(i0);
    if (i0) {
      loop_803: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i1 = 4u;
          i0 -= i1;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = !(i0);
          l2 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          if (i0) {goto block_805;}
          i0 = a;
          i1 = a;
          i1 = LoadU32(ctx, i1 + 4u);
          l2 = i1;
          i2 = 1u;
          i1 -= i2;
          l6 = i1;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {goto block_805;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 8u);
          l2 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l5;
        i1 = 145u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l2;
          i1 = a;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 145u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 == i1;
          if (i0) {goto running_exit;}
          i0 = a;
          i1 = a;
          i1 = LoadU32(ctx, i1 + 8u);
          l2 = i1;
          i2 = 1u;
          i1 -= i2;
          l6 = i1;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {goto block_805;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 12u);
          l2 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l5;
        i1 = 146u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l2;
          i1 = a;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 146u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 == i1;
          a = i0;
          if (i0) {goto running_exit;}
        }
        block_805:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i1 = 8u;
          i0 -= i1;
          l3 = i0;
          i1 = b;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto loop_803;}
        }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = a;
      } else {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 288u);
      }
      a = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = b;
      StoreU32(ctx, i0 + 292u, i1);
      i0 = a;
      DeallocateRawBlock(ctx, i0);
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 304u);
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
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 308u);
      l3 = i0;
      i1 = b;
      i0 = i0 == i1;
      l2 = i0;
      i0 = b;
      a = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    i1 = 0u;
    i2 = l2;
    i0 = i2 ? i0 : i1;
    i0 = !(i0);
    if (i0) {
      loop_819: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i1 = 4u;
          i0 -= i1;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = !(i0);
          l2 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          if (i0) {goto block_821;}
          i0 = a;
          i1 = a;
          i1 = LoadU32(ctx, i1 + 4u);
          l2 = i1;
          i2 = 1u;
          i1 -= i2;
          l6 = i1;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {goto block_821;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 8u);
          l2 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l5;
        i1 = 147u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l2;
          i1 = a;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 147u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 == i1;
          if (i0) {goto running_exit;}
          i0 = a;
          i1 = a;
          i1 = LoadU32(ctx, i1 + 8u);
          l2 = i1;
          i2 = 1u;
          i1 -= i2;
          l6 = i1;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {goto block_821;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 12u);
          l2 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l5;
        i1 = 148u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l2;
          i1 = a;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 148u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 == i1;
          a = i0;
          if (i0) {goto running_exit;}
        }
        block_821:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i1 = 8u;
          i0 -= i1;
          l3 = i0;
          i1 = b;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto loop_819;}
        }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = a;
      } else {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 304u);
      }
      a = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = b;
      StoreU32(ctx, i0 + 308u, i1);
      i0 = a;
      DeallocateRawBlock(ctx, i0);
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 331u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_213;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 320u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_213;
  }
  block_566:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_15;
  }
  block_565:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 288u);
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
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 292u);
      l3 = i0;
      i1 = b;
      i0 = i0 == i1;
      l2 = i0;
      i0 = b;
      a = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    i1 = 0u;
    i2 = l2;
    i0 = i2 ? i0 : i1;
    i0 = !(i0);
    if (i0) {
      loop_837: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i1 = 4u;
          i0 -= i1;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = !(i0);
          l2 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          if (i0) {goto block_839;}
          i0 = a;
          i1 = a;
          i1 = LoadU32(ctx, i1 + 4u);
          l2 = i1;
          i2 = 1u;
          i1 -= i2;
          l6 = i1;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {goto block_839;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 8u);
          l2 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l5;
        i1 = 149u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l2;
          i1 = a;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 149u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 == i1;
          if (i0) {goto running_exit;}
          i0 = a;
          i1 = a;
          i1 = LoadU32(ctx, i1 + 8u);
          l2 = i1;
          i2 = 1u;
          i1 -= i2;
          l6 = i1;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {goto block_839;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 12u);
          l2 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l5;
        i1 = 150u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l2;
          i1 = a;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 150u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 == i1;
          a = i0;
          if (i0) {goto running_exit;}
        }
        block_839:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i1 = 8u;
          i0 -= i1;
          l3 = i0;
          i1 = b;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto loop_837;}
        }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = a;
      } else {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 288u);
      }
      a = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = b;
      StoreU32(ctx, i0 + 292u, i1);
      i0 = a;
      DeallocateRawBlock(ctx, i0);
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 304u);
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
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 308u);
      l3 = i0;
      i1 = b;
      i0 = i0 == i1;
      l2 = i0;
      i0 = b;
      a = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    i1 = 0u;
    i2 = l2;
    i0 = i2 ? i0 : i1;
    i0 = !(i0);
    if (i0) {
      loop_853: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i1 = 4u;
          i0 -= i1;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = !(i0);
          l2 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          if (i0) {goto block_855;}
          i0 = a;
          i1 = a;
          i1 = LoadU32(ctx, i1 + 4u);
          l2 = i1;
          i2 = 1u;
          i1 -= i2;
          l6 = i1;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {goto block_855;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 8u);
          l2 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l5;
        i1 = 151u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l2;
          i1 = a;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 151u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l2 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 == i1;
          if (i0) {goto running_exit;}
          i0 = a;
          i1 = a;
          i1 = LoadU32(ctx, i1 + 8u);
          l2 = i1;
          i2 = 1u;
          i1 -= i2;
          l6 = i1;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l2;
          i1 = 1u;
          i0 = i0 != i1;
          l2 = i0;
          if (i0) {goto block_855;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          i0 = LoadU32(ctx, i0 + 12u);
          l2 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l5;
        i1 = 152u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l2;
          i1 = a;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 152u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 == i1;
          a = i0;
          if (i0) {goto running_exit;}
        }
        block_855:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i1 = 8u;
          i0 -= i1;
          l3 = i0;
          i1 = b;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto loop_853;}
        }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = a;
      } else {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 304u);
      }
      a = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = b;
      StoreU32(ctx, i0 + 308u, i1);
      i0 = a;
      DeallocateRawBlock(ctx, i0);
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 331u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_213;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 320u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_213:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 336u;
    i0 += i1;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 340u);
    b = i1;
    ReleaseNodeTree(ctx, i0, i1);
    i0 = l11;
    i1 = kF3293ConstAEE0C;
    StoreU32(ctx, i0, i1);
    i0 = l11;
    i0 = LoadS8(ctx, i0 + 83u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l11;
      i0 = LoadU32(ctx, i0 + 72u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l11;
    i0 = LoadS8(ctx, i0 + 71u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l11;
      i0 = LoadU32(ctx, i0 + 60u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l11;
    i0 = LoadS8(ctx, i0 + 59u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l11;
      i0 = LoadU32(ctx, i0 + 48u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l11;
    i0 = LoadS8(ctx, i0 + 47u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l11;
      i0 = LoadU32(ctx, i0 + 36u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l11;
    i0 = LoadS8(ctx, i0 + 35u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l11;
      i0 = LoadU32(ctx, i0 + 24u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l11;
    i1 = kF3293Const8B830;
    StoreU32(ctx, i0, i1);
    i0 = l11;
    i0 = LoadS8(ctx, i0 + 15u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_24;}
    i0 = l11;
    i0 = LoadU32(ctx, i0 + 4u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_24:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 480u;
    i0 += i1;
    GuestStackSp(ctx) = i0;
    goto func_exit;
  }
  block_23:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    TrapUnreachable();
  }
  block_22:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 139u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_21;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 128u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_21:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 251u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_19;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 240u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_19;
  }
  block_20:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 139u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_19;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 128u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_19:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 267u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_17;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 256u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_17;
  }
  block_18:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 139u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_17;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 128u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_17:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 283u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_15;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 272u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_15;
  }
  block_16:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
  }
  block_15:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 288u);
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
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 292u);
      l3 = i0;
      i1 = l2;
      i0 = i0 == i1;
      l6 = i0;
      i0 = l2;
      a = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    i1 = 0u;
    i2 = l6;
    i0 = i2 ? i0 : i1;
    i0 = !(i0);
    if (i0) {
      loop_885: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i1 = 4u;
          i0 -= i1;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = !(i0);
          l6 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l6;
          if (i0) {goto block_887;}
          i0 = a;
          i1 = a;
          i1 = LoadU32(ctx, i1 + 4u);
          l12 = i1;
          i2 = 1u;
          i1 -= i2;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = l12;
          i1 = 1u;
          i0 = i0 != i1;
          l6 = i0;
          if (i0) {goto block_887;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          l6 = i0;
          i0 = LoadU32(ctx, i0 + 8u);
          l12 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l5;
        i1 = 153u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l12;
          i1 = a;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 153u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l12 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l12;
          i1 = 1u;
          i0 = i0 == i1;
          if (i0) {goto running_exit;}
          i0 = a;
          i1 = a;
          i1 = LoadU32(ctx, i1 + 8u);
          l12 = i1;
          i2 = 1u;
          i1 -= i2;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l12;
          i1 = 1u;
          i0 = i0 != i1;
          l6 = i0;
          if (i0) {goto block_887;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          l6 = i0;
          i0 = LoadU32(ctx, i0 + 12u);
          l12 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l5;
        i1 = 154u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l12;
          i1 = a;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 154u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 == i1;
          a = i0;
          if (i0) {goto running_exit;}
        }
        block_887:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i1 = 8u;
          i0 -= i1;
          l3 = i0;
          i1 = l2;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto loop_885;}
        }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = a;
      } else {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 288u);
      }
      a = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = l2;
      StoreU32(ctx, i0 + 292u, i1);
      i0 = a;
      DeallocateRawBlock(ctx, i0);
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 304u);
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
      i0 = l4;
      i0 = LoadU32(ctx, i0 + 308u);
      l3 = i0;
      i1 = l2;
      i0 = i0 == i1;
      l6 = i0;
      i0 = l2;
      a = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    i1 = 0u;
    i2 = l6;
    i0 = i2 ? i0 : i1;
    i0 = !(i0);
    if (i0) {
      loop_902: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i1 = 4u;
          i0 -= i1;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = !(i0);
          l6 = i0;
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l6;
          if (i0) {goto block_904;}
          i0 = a;
          i1 = a;
          i1 = LoadU32(ctx, i1 + 4u);
          l12 = i1;
          i2 = 1u;
          i1 -= i2;
          StoreU32(ctx, i0 + 4u, i1);
          i0 = l12;
          i1 = 1u;
          i0 = i0 != i1;
          l6 = i0;
          if (i0) {goto block_904;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          l6 = i0;
          i0 = LoadU32(ctx, i0 + 8u);
          l12 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l5;
        i1 = 155u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l12;
          i1 = a;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 155u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          l12 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = l12;
          i1 = 1u;
          i0 = i0 == i1;
          if (i0) {goto block_896;}
          i0 = a;
          i1 = a;
          i1 = LoadU32(ctx, i1 + 8u);
          l12 = i1;
          i2 = 1u;
          i1 -= i2;
          StoreU32(ctx, i0 + 8u, i1);
          i0 = l12;
          i1 = 1u;
          i0 = i0 != i1;
          l6 = i0;
          if (i0) {goto block_904;}
          i0 = a;
          i0 = LoadU32(ctx, i0);
          l6 = i0;
          i0 = LoadU32(ctx, i0 + 12u);
          l12 = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
        }
        i0 = l5;
        i1 = 156u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = l12;
          i1 = a;
          HostReleaseSlotPair(ctx, i0, i1);
          i0 = 156u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = kF3293ConstD015C;
          i0 = LoadU32(ctx, i0);
          a = i0;
          i0 = kF3293ConstD015C;
          i1 = 0u;
          StoreU32(ctx, i0, i1);
          i0 = a;
          i1 = 1u;
          i0 = i0 == i1;
          a = i0;
          if (i0) {goto block_896;}
        }
        block_904:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l3;
          i1 = 8u;
          i0 -= i1;
          l3 = i0;
          i1 = l2;
          i0 = i0 != i1;
          a = i0;
          if (i0) {goto loop_902;}
        }
      i0 = ResumePhase(ctx);
      if (i0) {
        i0 = a;
      } else {
        i0 = l4;
        i0 = LoadU32(ctx, i0 + 304u);
      }
      a = i0;
    }
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = l4;
      i1 = l2;
      StoreU32(ctx, i0 + 308u, i1);
      i0 = a;
      DeallocateRawBlock(ctx, i0);
    }
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 331u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_7;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 320u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_7;
  }
  block_896:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {goto running_exit;}
  block_14:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 315u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_9;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 304u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_9;
  }
  block_13:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_9;
  }
  block_12:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 331u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_8;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 320u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
    goto block_8;
  }
  block_11:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_7;
  }
  block_10:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = HostRet0(ctx);
    b = i0;
    i0 = HostTempRet0(ctx);
    a = i0;
    goto block_6;
  }
  block_9:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 51u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_8;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 40u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_8:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i0 = LoadS8(ctx, i0 + 139u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_7;}
    i0 = l4;
    i0 = LoadU32(ctx, i0 + 128u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_7:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l4;
    i1 = 336u;
    i0 += i1;
    a = i0;
    i1 = l4;
    i1 = LoadU32(ctx, i1 + 340u);
    ReleaseNodeTree(ctx, i0, i1);
  }
  block_6:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = l11;
    i1 = kF3293ConstAEE0C;
    StoreU32(ctx, i0, i1);
    i0 = l11;
    i0 = LoadS8(ctx, i0 + 83u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l11;
      i0 = LoadU32(ctx, i0 + 72u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l11;
    i0 = LoadS8(ctx, i0 + 71u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l11;
      i0 = LoadU32(ctx, i0 + 60u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l11;
    i0 = LoadS8(ctx, i0 + 59u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l11;
      i0 = LoadU32(ctx, i0 + 48u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l11;
    i0 = LoadS8(ctx, i0 + 47u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l11;
      i0 = LoadU32(ctx, i0 + 36u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l11;
    i0 = LoadS8(ctx, i0 + 35u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = l11;
      i0 = LoadU32(ctx, i0 + 24u);
      DeallocateRawBlock(ctx, i0);
    }
    i0 = l11;
    i1 = kF3293Const8B830;
    StoreU32(ctx, i0, i1);
    i0 = l11;
    i0 = LoadS8(ctx, i0 + 15u);
    i1 = kF3293ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
    a = i0;
    if (i0) {goto block_5;}
    i0 = l11;
    i0 = LoadU32(ctx, i0 + 4u);
    a = i0;
    DeallocateRawBlock(ctx, i0);
  }
  block_5:;
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = b;
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
  i1 = 157u;
  i0 = i0 == i1;
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = a;
    Panic(ctx, i0);
    i0 = 157u;
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
  i2 = kF3293ResumeWordBytes;
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
  i1 = l4;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l7;
  i1 = l3;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l7;
  i1 = l6;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l7;
  i1 = l8;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l7;
  i1 = l9;
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
  i0 = l7;
  i1 = l14;
  StoreU32(ctx, i0 + 48u, i1);
  i0 = l7;
  i1 = l15;
  StoreU32(ctx, i0 + 52u, i1);
  i0 = l7;
  i1 = l16;
  StoreU32(ctx, i0 + 56u, i1);
  i0 = l7;
  i1 = l17;
  StoreU32(ctx, i0 + 60u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF3293ConstFFFFFFC0;
  i1 -= i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
}
