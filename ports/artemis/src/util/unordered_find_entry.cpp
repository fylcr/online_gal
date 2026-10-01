#include "util/engine_context.h"

namespace artc {

// Forward declarations of callees defined elsewhere in the tree.
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

constexpr uint32_t kUnorderedFindEntryMsgBuckets = 0x8FAFCu;      // buckets_
constexpr uint32_t kUnorderedFindEntryMsgBoostHeader = 0x8FB05u;  // unordered/detail/implementation.hpp
constexpr uint32_t kUnorderedFindEntryMsgGetBucket = 0x8FB48u;    // get_bucket
constexpr uint32_t kUnorderedFindEntryBucketMask = 0x7FFFFFFFu;
constexpr uint32_t kUnorderedFindEntrySignedLimit = 0xFFFFFFFFu;

}  // namespace

// Finds the entry of an unordered map that matches a key: hashes the key, walks
// that bucket's chain comparing the stored bytes, and returns the matching node
// (or zero); a chain whose needle ran out leaves through the suspend label.
uint32_t UnorderedFindEntry(EngineContext* ctx, uint32_t map, uint32_t hash, uint32_t key,
                            uint32_t out) {
  uint32_t bucket = 0u;      // hash modulo the bucket count
  uint32_t cursor = 0u;      // needle compared against the chain entry
  uint32_t key_cursor = 0u;  // second needle cursor used by the byte loop
  uint32_t length = 0u;      // sign-extended key length
  uint32_t neg = 0u;         // the key uses the long form when set
  uint32_t counter = 0u;     // bytes still to compare
  uint32_t hash_byte = 0u;   // low byte of the hash, the chain's filter byte
  uint32_t entry_ptr = 0u;   // address of the entry payload
  uint32_t byte = 0u;        // byte read from the needle
  uint32_t result = 0u;      // matching node handed back, or zero
  uint32_t tmp = 0u;
  uint32_t tmp2 = 0u;
  uint32_t tmp3 = 0u;
  EnterFrame();
  tmp = LoadU32(ctx, map + 4u);
  bucket = REM_U(hash, tmp);
  hash = 0u;
  if (LoadU32(ctx, map + 8u) == 0u) {
    goto done;
  }
  map = LoadU32(ctx, map + 20u);
  if (map != 0u) {
    tmp = bucket << (2u & 31u);
    tmp += map;
    map = LoadU32(ctx, tmp);
    if (map == 0u) {
      goto done;
    }
    map = LoadU32(ctx, map);
    if (map == 0u) {
      goto done;
    }
    hash = LoadU32(ctx, key);
    tmp = LoadU8(ctx, key + 11u);
    out = tmp;
    tmp = tmp << (24u & 31u);
    tmp = (uint32_t)((int32_t)tmp >> (24u & 31u));
    length = tmp;
    neg = (uint32_t)((int32_t)tmp < 0);
    tmp = LoadU32(ctx, key + 4u);
    cursor = (neg != 0u) ? tmp : out;
    tmp = length;
    tmp2 = kUnorderedFindEntrySignedLimit;
    tmp = (uint32_t)((int32_t)tmp <= (int32_t)tmp2);
    if (tmp != 0u) {
      // Short key: walk the chain comparing the needle byte for byte.
      key = (neg != 0u) ? hash : key;
      for (;;) {
        tmp = LoadU32(ctx, map + 12u);
        tmp2 = LoadU8(ctx, map + 19u);
        hash = tmp2;
        tmp3 = hash;
        tmp3 = tmp3 << (24u & 31u);
        tmp3 = (uint32_t)((int32_t)tmp3 >> (24u & 31u));
        tmp3 = (uint32_t)((int32_t)tmp3 < 0);
        hash = tmp3;
        tmp = (tmp3 != 0u) ? tmp : tmp2;
        if (cursor == tmp) {
          if (cursor == 0u) {
            goto suspend_exit;
          }
          tmp = map + 8u;
          out = tmp;
          tmp2 = LoadU32(ctx, out);
          tmp3 = (hash != 0u) ? tmp2 : out;
          tmp = CompareBytes(ctx, key, tmp3, cursor);
          if (tmp == 0u) {
            goto suspend_exit;
          }
        }
        hash = 0u;
        tmp = LoadU32(ctx, map + 4u);
        tmp2 = kUnorderedFindEntryBucketMask;
        tmp &= tmp2;
        if (bucket != tmp) {
          goto done;
        }
        for (;;) {
          map = LoadU32(ctx, map);
          if (map == 0u) {
            goto done;
          }
          tmp = LoadU32(ctx, map + 4u);
          if ((int32_t)tmp < 0) {
            continue;
          }
          break;
        }
      }
      TrapUnreachable();
    }
    // Long key: the needle is hashed and compared by its leading filter byte.
    if (cursor == 0u) {
      goto compute_result;
    }
    hash_byte = hash & 255u;
    for (;;) {
      tmp = LoadU32(ctx, map + 12u);
      tmp2 = LoadU8(ctx, map + 19u);
      hash = tmp2;
      tmp3 = hash;
      tmp3 = tmp3 << (24u & 31u);
      tmp3 = (uint32_t)((int32_t)tmp3 >> (24u & 31u));
      tmp3 = (uint32_t)((int32_t)tmp3 < 0);
      hash = tmp3;
      tmp = (tmp3 != 0u) ? tmp : tmp2;
      if (cursor != tmp) {
        goto chain_end;
      }
      counter = out;
      key_cursor = key;
      entry_ptr = map + 8u;
      tmp = LoadU32(ctx, entry_ptr);
      tmp2 = (hash != 0u) ? tmp : entry_ptr;
      hash = tmp2;
      tmp = LoadU8(ctx, hash);
      if (hash_byte != tmp) {
        goto chain_end;
      }
      for (;;) {
        counter -= 1u;
        if (counter == 0u) {
          goto suspend_exit;
        }
        tmp = LoadU8(ctx, hash + 1u);
        byte = tmp;
        hash += 1u;
        key_cursor += 1u;
        tmp = LoadU8(ctx, key_cursor);
        if (tmp == byte) {
          continue;
        }
        break;
      }
    chain_end:;
      hash = 0u;
      tmp = LoadU32(ctx, map + 4u);
      tmp2 = kUnorderedFindEntryBucketMask;
      tmp &= tmp2;
      if (bucket != tmp) {
        goto done;
      }
      for (;;) {
        map = LoadU32(ctx, map);
        if (map == 0u) {
          goto done;
        }
        tmp = LoadU32(ctx, map + 4u);
        if ((int32_t)tmp < 0) {
          continue;
        }
        break;
      }
    }
    TrapUnreachable();
  }
  HostAssertFail(ctx, kUnorderedFindEntryMsgBuckets, kUnorderedFindEntryMsgBoostHeader,
                 2956u, kUnorderedFindEntryMsgGetBucket);
  TrapUnreachable();
compute_result:;
  hash = LoadU8(ctx, map + 19u);
  hash = hash << (24u & 31u);
  hash = (uint32_t)((int32_t)hash >> (24u & 31u));
  out = (uint32_t)((int32_t)hash < 0);
  tmp = LoadU32(ctx, map + 12u);
  tmp = (out != 0u) ? tmp : hash;
  if (tmp == 0u) {
    goto suspend_exit;
  }
  hash = map;
  for (;;) {
    tmp = LoadU32(ctx, hash + 4u);
    tmp2 = kUnorderedFindEntryBucketMask;
    tmp &= tmp2;
    if (bucket != tmp) {
      result = 0u;
      goto frame_stored;
    }
    for (;;) {
      hash = LoadU32(ctx, hash);
      if (hash == 0u) {
        result = 0u;
        goto frame_stored;
      }
      tmp = LoadU32(ctx, hash + 4u);
      if ((int32_t)tmp < 0) {
        continue;
      }
      break;
    }
  chain_matched:;
    map = LoadU8(ctx, hash + 19u);
    map = map << (24u & 31u);
    map = (uint32_t)((int32_t)map >> (24u & 31u));
    out = (uint32_t)((int32_t)map < 0);
    tmp = LoadU32(ctx, hash + 12u);
    tmp = (out != 0u) ? tmp : map;
    if (tmp == 0u) {
      goto done;
    }
  }
suspend_exit:;
  hash = map;
done:;
  result = hash;
frame_stored:;
  LeaveFrame();
  return result;
}
}  // namespace artc
