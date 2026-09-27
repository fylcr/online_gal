// string_length.cpp -- the classic word-at-a-time zero-byte scan: aligned words first, then byte by byte..
//
// StringLength -- the classic word-at-a-time zero-byte scan: aligned words first, then byte by byte.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 113410-113410 (f207); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {


namespace {

constexpr uint32_t kF207Const1010101 = 0x1010101u;  // 
constexpr uint32_t kF207Const80808080 = 0x80808080u;  // 
constexpr uint32_t kF207ConstFFFFFFFF = 0xFFFFFFFFu;  // 

}  // namespace

uint32_t StringLength(EngineContext* ctx, uint32_t text) {
  EnterFrame();
  uint32_t l1 = 0u; uint32_t l2 = 0u; uint32_t l3 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u;
  i0 = text;
  l1 = i0;
  i1 = 3u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto spill_frame;}
  i0 = text;
  i0 = LoadU8(ctx, i0);
  i0 = !(i0);
  if (i0) {
    i0 = 0u;
    goto func_exit;
  }
  loop_3: 
    i0 = l1;
    i1 = 1u;
    i0 += i1;
    l1 = i0;
    i1 = 3u;
    i0 &= i1;
    i0 = !(i0);
    if (i0) {goto spill_frame;}
    i0 = l1;
    i0 = LoadU8(ctx, i0);
    if (i0) {goto loop_3;}
  goto block_0;
  spill_frame:;
  loop_4: 
    i0 = l1;
    l2 = i0;
    i1 = 4u;
    i0 += i1;
    l1 = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0);
    l3 = i0;
    i1 = kF207ConstFFFFFFFF;
    i0 ^= i1;
    i1 = l3;
    i2 = kF207Const1010101;
    i1 -= i2;
    i0 &= i1;
    i1 = kF207Const80808080;
    i0 &= i1;
    i0 = !(i0);
    if (i0) {goto loop_4;}
  i0 = l3;
  i1 = 255u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {
    i0 = l2;
    i1 = text;
    i0 -= i1;
    goto func_exit;
  }
  loop_6: 
    i0 = l2;
    i0 = LoadU8(ctx, i0 + 1u);
    l3 = i0;
    i0 = l2;
    i1 = 1u;
    i0 += i1;
    l1 = i0;
    l2 = i0;
    i0 = l3;
    if (i0) {goto loop_6;}
  block_0:;
  i0 = l1;
  i1 = text;
  i0 -= i1;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
