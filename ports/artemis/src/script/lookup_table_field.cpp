// lookup_table_field.cpp — table field lookup by a general value key.
//
// LookupTableField returns the 16-byte value record a script table holds for a
// key, or the shared nil record when the table has no such field. It is the
// general entry point: the key's value-type tag picks a main position among the
// table's 32-byte hash nodes, and the chain hanging off that node is walked with
// the comparison that fits the key family. The tag space is 0 nil, 1 boolean,
// 2 light userdata, 3 number, 4 string; a nil key names no field at all, a
// string key is interned so its node compares by object identity against the
// key word itself, and the remaining families hash their key word.
//
// Number keys probe the flat array part first — one 16-byte record per
// consecutive index below the declared array size — and only then fall back to
// the node chain, whose walk compares the stored key number directly. Numbers
// that are not integer-valued（or that sit outside the signed 32-bit range, in
// which case the fold below parks a sentinel that can never compare equal
// again）hash their f64 bit pattern instead, and every non-number family
// compares whole key records through the shared raw-equality product.
//
// Node layout: the value record at +0, the key payload at +16, the key's type
// tag at +24, the next node at +28. Table layout: the bucket shift at +7, the
// array part base at +12, the hash node base at +16, the array part size at
// +28. A bucket is either the power-of-two mask of that shift or the same mask
// forced odd and used as a modulo divisor, so the divisor is never zero even
// for an empty table.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//
#include "util/engine_context.h"
#include <cmath>

namespace artc {

// Forward declaration — defined in script/raw_value_equal.cpp.
// PROVISIONAL: the generated f603 body is not registered in rewrite-names.tsv
// yet, so this callee name and its presumed product file are both provisional.
uint32_t RawValueEqual(EngineContext* ctx, uint32_t lhs, uint32_t rhs);

uint32_t LookupTableField(EngineContext* ctx, uint32_t table, uint32_t key) {
  EnterFrame();
  // The first magnitude a signed 32-bit fold cannot represent, and the sentinel
  // parked for such a key: a value whose back-conversion can never equal the key
  // again, so the key is left to the float path.
  double const kFoldLimit = 2147483648.0;
  uint32_t const kUnfoldableKey = 0x80000000u;
  uint32_t node = 0u;
  uint32_t key_fold = 0u;
  uint64_t key_bits = 0u;
  uint64_t key_bits_high = 0u;
  double key_number = 0.0;
  double key_number_as_int = 0.0;
  switch (LoadU32(ctx, key + 8u)) {
    case 0u:
      // A nil key names no field.
      LeaveFrame();
      return kNilValueAddr;
    case 1u: {
      // Boolean-shaped keys mask their key word down to a node index.
      uint32_t const bool_node_base = LoadU32(ctx, table + 16u);
      uint32_t const bool_key_word = LoadU32(ctx, key);
      uint32_t const bool_shift = LoadU8(ctx, table + 7u);
      uint32_t const bool_node_mask = (0xFFFFFFFFu << (bool_shift & 31u)) ^ 0xFFFFFFFFu;
      node = bool_node_base + ((bool_key_word & bool_node_mask) << 5);
      break;
    }
    case 2u: {
      // Light-userdata-shaped keys whose key word is a modulo hash.
      uint32_t const light_node_base = LoadU32(ctx, table + 16u);
      uint32_t const light_key_word = LoadU32(ctx, key);
      uint32_t const light_shift = LoadU8(ctx, table + 7u);
      uint32_t const light_divisor = ((0xFFFFFFFFu << (light_shift & 31u)) ^ 0xFFFFFFFFu) | 1u;
      node = light_node_base + ((light_key_word % light_divisor) << 5);
      break;
    }
    case 3u: {
      // Number key. The fold below mirrors the VM's own test for "this number
      // can index the array part": a magnitude at or past 2^31 keeps the
      // unfoldable sentinel, whose back-conversion can never equal the key
      // again, so such a key is routed as the float it is.
      key_number = LoadF64(ctx, key);
      key_fold = std::fabs(key_number) < kFoldLimit
                     ? static_cast<uint32_t>(static_cast<int32_t>(key_number))
                     : kUnfoldableKey;
      key_number_as_int = static_cast<double>(static_cast<int32_t>(key_fold));
      if (key_number_as_int == key_number) {
        uint32_t const array_index = key_fold - 1u;
        if (array_index < LoadU32(ctx, table + 28u)) {
          // Array part: one 16-byte record per consecutive index.
          LeaveFrame();
          return LoadU32(ctx, table + 12u) + (array_index << 4);
        }
        if (key_fold == 0u) {
          // Number zero sits at the head of the node array.
          node = LoadU32(ctx, table + 16u);
        } else {
          key_bits = ReinterpretF64(key_number_as_int);
          key_bits_high = key_bits >> 32;
          uint32_t const num_node_base = LoadU32(ctx, table + 16u);
          uint32_t const num_mixed = static_cast<uint32_t>(key_bits + key_bits_high);
          uint32_t const num_shift = LoadU8(ctx, table + 7u);
          uint32_t const num_divisor = ((0xFFFFFFFFu << (num_shift & 31u)) ^ 0xFFFFFFFFu) | 1u;
          node = num_node_base + ((num_mixed % num_divisor) << 5);
        }
        // The numeric family's chain compares the stored key number directly,
        // and an integer-valued key never reaches the general comparison.
        for (;;) {
          if (LoadU32(ctx, node + 24u) == 3u &&
              key_number_as_int == LoadF64(ctx, node + 16u)) {
            LeaveFrame();
            return node;
          }
          uint32_t const num_next = LoadU32(ctx, node + 28u);
          if (num_next == 0u) {
            break;
          }
          node = num_next;
        }
        LeaveFrame();
        return kNilValueAddr;
      }
      // Not integer-valued: hash the f64 bit pattern（number zero keeps the
      // head of the node array）and take the general chain below.
      if (key_number == 0.0) {
        node = LoadU32(ctx, table + 16u);
      } else {
        key_bits = ReinterpretF64(key_number);
        key_bits_high = key_bits >> 32;
        uint32_t const float_node_base = LoadU32(ctx, table + 16u);
        uint32_t const float_mixed = static_cast<uint32_t>(key_bits + key_bits_high);
        uint32_t const float_shift = LoadU8(ctx, table + 7u);
        uint32_t const float_divisor = ((0xFFFFFFFFu << (float_shift & 31u)) ^ 0xFFFFFFFFu) | 1u;
        node = float_node_base + ((float_mixed % float_divisor) << 5);
      }
      break;
    }
    case 4u: {
      // String key: the string carries its own cached hash and is interned, so
      // the node compares the key word by identity.
      uint32_t const str_node_base = LoadU32(ctx, table + 16u);
      uint32_t const str_key = LoadU32(ctx, key);
      uint32_t const str_key_hash = LoadU32(ctx, str_key + 8u);
      uint32_t const str_shift = LoadU8(ctx, table + 7u);
      uint32_t const str_node_mask = (0xFFFFFFFFu << (str_shift & 31u)) ^ 0xFFFFFFFFu;
      node = str_node_base + ((str_key_hash & str_node_mask) << 5);
      for (;;) {
        if (LoadU32(ctx, node + 24u) == 4u && LoadU32(ctx, node + 16u) == str_key) {
          LeaveFrame();
          return node;
        }
        uint32_t const str_next = LoadU32(ctx, node + 28u);
        if (str_next == 0u) {
          break;
        }
        node = str_next;
      }
      LeaveFrame();
      return kNilValueAddr;
    }
    default: {
      // Tables, functions, userdata and threads hash their key word the same
      // modulo way. The source dispatches this family separately from the
      // light-userdata one above, so it stays its own arm.
      uint32_t const ptr_node_base = LoadU32(ctx, table + 16u);
      uint32_t const ptr_key_word = LoadU32(ctx, key);
      uint32_t const ptr_shift = LoadU8(ctx, table + 7u);
      uint32_t const ptr_divisor = ((0xFFFFFFFFu << (ptr_shift & 31u)) ^ 0xFFFFFFFFu) | 1u;
      node = ptr_node_base + ((ptr_key_word % ptr_divisor) << 5);
      break;
    }
  }
  // General chain: each node's key record is compared against the key through
  // the shared raw-equality product, and the walk ends on the shared nil
  // record.
  for (;;) {
    if (RawValueEqual(ctx, node + 16u, key)) {
      LeaveFrame();
      return node;
    }
    uint32_t const chain_next = LoadU32(ctx, node + 28u);
    if (chain_next == 0u) {
      break;
    }
    node = chain_next;
  }
  LeaveFrame();
  return kNilValueAddr;
}

}  // namespace artc
