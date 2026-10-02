// unordered_find_entry.cpp -- the unordered-map find: hash mod bucket count, walk the bucket chain comparing keys..
//
// UnorderedFindEntry -- the unordered-map find: hash mod bucket count, walk the bucket chain comparing keys.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 232347-232347 (f592); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

constexpr uint32_t kF592MsgBuckets = 0x8FAFCu;  // buckets_
constexpr uint32_t kF592MsgBoostBoostUnorderedDetail = 0x8FB05u;  // ../../../../boost_1_66_0/boost/unordered/detail/implementation.hpp
constexpr uint32_t kF592MsgGetBucket = 0x8FB48u;  // get_bucket
constexpr uint32_t kF592Const7FFFFFFF = 0x7FFFFFFFu;  // 
constexpr uint32_t kF592ConstFFFFFFFF = 0xFFFFFFFFu;  // 

}  // namespace

uint32_t UnorderedFindEntry(EngineContext* ctx, uint32_t map, uint32_t hash, uint32_t key, uint32_t out) {
  EnterFrame();
  uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u;
  i0 = hash;
  i1 = map;
  i1 = LoadU32(ctx, i1 + 4u);
  i0 = REM_U(i0, i1);
  l6 = i0;
  i0 = 0u;
  hash = i0;
  i0 = map;
  i0 = LoadU32(ctx, i0 + 8u);
  i0 = !(i0);
  if (i0) {goto block_0;}
  i0 = map;
  i0 = LoadU32(ctx, i0 + 20u);
  map = i0;
  if (i0) {
    i0 = l6;
    i1 = 2u;
    i0 <<= (i1 & 31);
    i1 = map;
    i0 += i1;
    i0 = LoadU32(ctx, i0);
    map = i0;
    i0 = !(i0);
    if (i0) {goto block_0;}
    i0 = map;
    i0 = LoadU32(ctx, i0);
    map = i0;
    i0 = !(i0);
    if (i0) {goto block_0;}
    i0 = key;
    i0 = LoadU32(ctx, i0);
    hash = i0;
    i0 = key;
    i0 = LoadU8(ctx, i0 + 11u);
    out = i0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    l7 = i0;
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    l4 = i0;
    i0 = key;
    i0 = LoadU32(ctx, i0 + 4u);
    i1 = out;
    i2 = l4;
    i0 = i2 ? i0 : i1;
    l5 = i0;
    i0 = l7;
    i1 = kF592ConstFFFFFFFF;
    i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
    if (i0) {
      i0 = hash;
      i1 = key;
      i2 = l4;
      i0 = i2 ? i0 : i1;
      key = i0;
      loop_5: 
        i0 = l5;
        i1 = map;
        i1 = LoadU32(ctx, i1 + 12u);
        i2 = map;
        i2 = LoadU8(ctx, i2 + 19u);
        hash = i2;
        i3 = hash;
        i4 = 24u;
        i3 <<= (i4 & 31);
        i4 = 24u;
        i3 = (uint32_t)((int32_t)i3 >> (i4 & 31));
        i4 = 0u;
        i3 = (uint32_t)((int32_t)i3 < (int32_t)i4);
        hash = i3;
        i1 = i3 ? i1 : i2;
        i0 = i0 == i1;
        if (i0) {
          i0 = l5;
          i0 = !(i0);
          if (i0) {goto spill_frame;}
          i0 = key;
          i1 = map;
          i2 = 8u;
          i1 += i2;
          out = i1;
          i1 = LoadU32(ctx, i1);
          i2 = out;
          i3 = hash;
          i1 = i3 ? i1 : i2;
          i2 = l5;
          i0 = CompareBytes(ctx, i0, i1, i2);
          i0 = !(i0);
          if (i0) {goto spill_frame;}
        }
        i0 = 0u;
        hash = i0;
        i0 = l6;
        i1 = map;
        i1 = LoadU32(ctx, i1 + 4u);
        i2 = kF592Const7FFFFFFF;
        i1 &= i2;
        i0 = i0 != i1;
        if (i0) {goto block_0;}
        loop_7: 
          i0 = map;
          i0 = LoadU32(ctx, i0);
          map = i0;
          i0 = !(i0);
          if (i0) {goto block_0;}
          i0 = map;
          i0 = LoadU32(ctx, i0 + 4u);
          i1 = 0u;
          i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
          if (i0) {goto loop_7;}
        goto loop_5;
      TrapUnreachable();
    }
    i0 = l5;
    i0 = !(i0);
    if (i0) {goto block_2;}
    i0 = hash;
    i1 = 255u;
    i0 &= i1;
    l9 = i0;
    dispatch_loop: 
      i0 = l5;
      i1 = map;
      i1 = LoadU32(ctx, i1 + 12u);
      i2 = map;
      i2 = LoadU8(ctx, i2 + 19u);
      hash = i2;
      i3 = hash;
      i4 = 24u;
      i3 <<= (i4 & 31);
      i4 = 24u;
      i3 = (uint32_t)((int32_t)i3 >> (i4 & 31));
      i4 = 0u;
      i3 = (uint32_t)((int32_t)i3 < (int32_t)i4);
      hash = i3;
      i1 = i3 ? i1 : i2;
      i0 = i0 != i1;
      if (i0) {goto block_9;}
      i0 = out;
      l7 = i0;
      i0 = key;
      l4 = i0;
      i0 = l9;
      i1 = map;
      i2 = 8u;
      i1 += i2;
      l8 = i1;
      i1 = LoadU32(ctx, i1);
      i2 = l8;
      i3 = hash;
      i1 = i3 ? i1 : i2;
      hash = i1;
      i1 = LoadU8(ctx, i1);
      i0 = i0 != i1;
      if (i0) {goto block_9;}
      loop_10: 
        i0 = l7;
        i1 = 1u;
        i0 -= i1;
        l7 = i0;
        i0 = !(i0);
        if (i0) {goto spill_frame;}
        i0 = hash;
        i0 = LoadU8(ctx, i0 + 1u);
        l8 = i0;
        i0 = hash;
        i1 = 1u;
        i0 += i1;
        hash = i0;
        i0 = l4;
        i1 = 1u;
        i0 += i1;
        l4 = i0;
        i0 = LoadU8(ctx, i0);
        i1 = l8;
        i0 = i0 == i1;
        if (i0) {goto loop_10;}
      block_9:;
      i0 = 0u;
      hash = i0;
      i0 = l6;
      i1 = map;
      i1 = LoadU32(ctx, i1 + 4u);
      i2 = kF592Const7FFFFFFF;
      i1 &= i2;
      i0 = i0 != i1;
      if (i0) {goto block_0;}
      loop_11: 
        i0 = map;
        i0 = LoadU32(ctx, i0);
        map = i0;
        i0 = !(i0);
        if (i0) {goto block_0;}
        i0 = map;
        i0 = LoadU32(ctx, i0 + 4u);
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        if (i0) {goto loop_11;}
      goto dispatch_loop;
    TrapUnreachable();
  }
  i0 = kF592MsgBuckets;
  i1 = kF592MsgBoostBoostUnorderedDetail;
  i2 = 2956u;
  i3 = kF592MsgGetBucket;
  HostAssertFail(ctx, i0, i1, i2, i3);
  TrapUnreachable();
  block_2:;
  i0 = map;
  i0 = LoadU8(ctx, i0 + 19u);
  hash = i0;
  i1 = 24u;
  i0 <<= (i1 & 31);
  i1 = 24u;
  i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
  i1 = 0u;
  i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
  out = i0;
  i0 = map;
  i0 = LoadU32(ctx, i0 + 12u);
  i1 = hash;
  i2 = out;
  i0 = i2 ? i0 : i1;
  i0 = !(i0);
  if (i0) {goto spill_frame;}
  i0 = map;
  hash = i0;
  loop_12: 
    i0 = l6;
    i1 = hash;
    i1 = LoadU32(ctx, i1 + 4u);
    i2 = kF592Const7FFFFFFF;
    i1 &= i2;
    i0 = i0 != i1;
    if (i0) {
      i0 = 0u;
      goto func_exit;
    }
    loop_15: 
      i0 = hash;
      i0 = LoadU32(ctx, i0);
      hash = i0;
      if (i0) {
        i0 = hash;
        i0 = LoadU32(ctx, i0 + 4u);
        i1 = 0u;
        i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
        if (i0) {goto loop_15;}
        goto block_14;
      }
    i0 = 0u;
    goto func_exit;
    block_14:;
    i0 = hash;
    i0 = LoadU8(ctx, i0 + 19u);
    map = i0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
    i1 = 0u;
    i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
    out = i0;
    i0 = hash;
    i0 = LoadU32(ctx, i0 + 12u);
    i1 = map;
    i2 = out;
    i0 = i2 ? i0 : i1;
    if (i0) {goto loop_12;}
  goto block_0;
  spill_frame:;
  i0 = map;
  hash = i0;
  block_0:;
  i0 = hash;
  func_exit:;
  LeaveFrame();
  return i0;
}
}
