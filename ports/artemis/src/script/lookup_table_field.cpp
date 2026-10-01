#include "util/engine_context.h"
#include <cmath>

namespace artc {

// Forward declaration -- defined in script/raw_value_equal.cpp.
uint32_t RawValueEqual(EngineContext* ctx, uint32_t lhs, uint32_t rhs);

// Looks a general value key up in a table: the key's type tag picks a bucket
// among the table's 32-byte hash nodes and that node's chain is walked with the
// comparison the key family calls for. Answers the 16-byte value record the
// table holds for the key, or the shared nil record when it holds none.
uint32_t LookupTableField(EngineContext* ctx, uint32_t table, uint32_t key) {
  EnterFrame();
  // The first magnitude a signed 32-bit fold cannot represent; such a key keeps
  // this sentinel, whose back-conversion can never compare equal to it again.
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
      LeaveFrame();
      return kNilValueAddr;
    case 1u: {
      uint32_t const bool_node_base = LoadU32(ctx, table + 16u);
      uint32_t const bool_key_word = LoadU32(ctx, key);
      uint32_t const bool_shift = LoadU8(ctx, table + 7u);
      uint32_t const bool_node_mask = (0xFFFFFFFFu << (bool_shift & 31u)) ^ 0xFFFFFFFFu;
      node = bool_node_base + ((bool_key_word & bool_node_mask) << 5);
      break;
    }
    case 2u: {
      // The mask is forced odd so that it can divide, and is never zero.
      uint32_t const light_node_base = LoadU32(ctx, table + 16u);
      uint32_t const light_key_word = LoadU32(ctx, key);
      uint32_t const light_shift = LoadU8(ctx, table + 7u);
      uint32_t const light_divisor = ((0xFFFFFFFFu << (light_shift & 31u)) ^ 0xFFFFFFFFu) | 1u;
      node = light_node_base + ((light_key_word % light_divisor) << 5);
      break;
    }
    case 3u: {
      // The fold mirrors the VM's own test for "this number can index the array
      // part": a magnitude at or past 2^31 keeps the unfoldable sentinel and the
      // key is routed as the float it is.
      key_number = LoadF64(ctx, key);
      key_fold = std::fabs(key_number) < kFoldLimit
                     ? static_cast<uint32_t>(static_cast<int32_t>(key_number))
                     : kUnfoldableKey;
      key_number_as_int = static_cast<double>(static_cast<int32_t>(key_fold));
      if (key_number_as_int == key_number) {
        uint32_t const array_index = key_fold - 1u;
        if (array_index < LoadU32(ctx, table + 28u)) {
          // The array part holds one 16-byte record per consecutive index.
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
        // This chain compares the stored key number itself, never the raw key
        // record the general chain below would read.
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
      // Not integer-valued, so the f64 bit pattern is hashed instead.
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
      // A string carries its own cached hash, so the chain below compares the
      // key word by identity.
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
      uint32_t const ptr_node_base = LoadU32(ctx, table + 16u);
      uint32_t const ptr_key_word = LoadU32(ctx, key);
      uint32_t const ptr_shift = LoadU8(ctx, table + 7u);
      uint32_t const ptr_divisor = ((0xFFFFFFFFu << (ptr_shift & 31u)) ^ 0xFFFFFFFFu) | 1u;
      node = ptr_node_base + ((ptr_key_word % ptr_divisor) << 5);
      break;
    }
  }
  // The walk compares each node's key record and ends on the shared nil record.
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
