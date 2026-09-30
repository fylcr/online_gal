#include "util/engine_context.h"

namespace artc {

namespace {

// The table's own fields: the entry array, the entry count, the hash part, and
// the byte its bucket mask is taken from.
constexpr uint32_t kFindNumericKeySlotArrayOffset = 12u;
constexpr uint32_t kFindNumericKeySlotKeyTableOffset = 16u;
constexpr uint32_t kFindNumericKeySlotCountOffset = 28u;
constexpr uint32_t kFindNumericKeySlotSizeByteOffset = 7u;

// One hash entry is thirty-two bytes and chains to the next through its last
// word; the number it holds sits at +16 with its kind at +24, and a word at +8
// that is not zero means the entry is taken.
constexpr uint32_t kFindNumericKeySlotEntryShift = 5u;
constexpr uint32_t kFindNumericKeySlotKeyOffset = 16u;
constexpr uint32_t kFindNumericKeySlotKindOffset = 24u;
constexpr uint32_t kFindNumericKeySlotChainOffset = 28u;
constexpr uint32_t kFindNumericKeySlotValueOffset = 8u;

// The kind word an entry carries when it holds a number, the shared record that
// stands for a table with no hash part, and the bound the widening probes stop
// at.
constexpr uint32_t kFindNumericKeySlotNumberKind = 3u;
constexpr uint32_t kFindNumericKeySlotNoTable = 0x2BBD0u;
constexpr uint32_t kFindNumericKeySlotProbeLimit = 0x7FFFFFFEu;

// Probes the table's hash part for one numeric key: the entry that holds the key
// is answered, and the shared nil record when no entry does.
uint32_t FindNumericKeySlotProbe(EngineContext* ctx, uint32_t state, uint32_t key_table, uint32_t key) {
  // The key's own hash: the two halves of its double added, taken modulo the
  // table's bucket mask, and scaled by the entry size.
  double const key_number = static_cast<double>(static_cast<int32_t>(key));
  uint32_t entry = key_table;
  if (key != 0u) {
    uint64_t const bits = i64_reinterpret_f64(key_number);
    uint32_t const halves = static_cast<uint32_t>(bits + (bits >> 32));
    uint32_t const mask =
        ((0xFFFFFFFFu << LoadU8(ctx, state + kFindNumericKeySlotSizeByteOffset)) ^ 0xFFFFFFFFu) | 1u;
    entry += REM_U(halves, mask) << kFindNumericKeySlotEntryShift;
  }
  // The chain is followed from there: the first entry holding this number is the
  // one answered, and the nil record when the chain ends first.
  for (;;) {
    if (LoadU32(ctx, entry + kFindNumericKeySlotKindOffset) == kFindNumericKeySlotNumberKind &&
        LoadF64(ctx, entry + kFindNumericKeySlotKeyOffset) == key_number) {
      return entry;
    }
    entry = LoadU32(ctx, entry + kFindNumericKeySlotChainOffset);
    if (entry == 0u) {
      return kNilValueAddr;
    }
  }
}

}  // namespace

// Answers the slot a numeric key would occupy in the table. A table that already
// holds entries is narrowed down by a binary search over them; one that holds
// none is probed through its hash part, widening the probe until an empty entry
// is found and narrowing that one down in turn.
uint32_t FindNumericKeySlot(EngineContext* ctx, uint32_t state) {
  EnterFrame();
  // The array the entries are held in, and the bounds the search works between.
  uint32_t array = 0u;
  uint32_t upper = 0u;
  uint32_t lower = 0u;
  // The entry the walk stands on: the index it names first, and then the entry
  // itself.
  uint32_t entry = 0u;
  // The width the probes widen by.
  uint32_t width = 0u;
  // The table's hash part, and the key the probe below is made for.
  uint32_t key_table = 0u;
  uint32_t key = 0u;

  upper = LoadU32(ctx, state + kFindNumericKeySlotCountOffset);
  if (upper == 0u) {
    // The table holds no entries, so only its hash part can name a slot.
    goto hash_path;
  }
  array = LoadU32(ctx, state + kFindNumericKeySlotArrayOffset);
  // The table's last slot is already taken, so the array part cannot hold the
  // key and the hash part has to name the slot instead.
  if (LoadU32(ctx, array + (upper << 4u) - 8u) != 0u) {
    goto hash_path;
  }
  if (upper >= 2u) {
    // The entries are held in key order, so the middle one decides which half
    // the key falls in.
    while (upper - lower > 1u) {
      uint32_t const middle = (upper + lower) >> 1u;
      uint32_t const probe_key = LoadU32(ctx, array + (middle << 4u) - 8u);
      upper = probe_key ? upper : middle;
      lower = probe_key ? middle : lower;
    }
  }
  goto search_done;
hash_path:;
  key_table = LoadU32(ctx, state + kFindNumericKeySlotKeyTableOffset);
  if (key_table == kFindNumericKeySlotNoTable) {
    // No hash part either, so the table is empty and the slot is its first one.
    LeaveFrame();
    return upper;
  }
  // The hash part is probed from the entry after the last held one, and the
  // probes widen until an empty entry is found.
  lower = upper;
  entry = upper + 1u;
probe_next:;
  key = entry;
  entry -= 1u;
  entry = upper > entry ? array + (entry << 4u)
                        : FindNumericKeySlotProbe(ctx, state, key_table, key);
probe_slot:;
  if (LoadU32(ctx, entry + kFindNumericKeySlotValueOffset) == 0u) {
    // An empty entry was found, so the search narrows down to the slot the key
    // belongs at.
    while (key - lower > 1u) {
      uint32_t const middle = (lower + key) >> 1u;
      uint32_t const index = middle - 1u;
      entry = upper > index ? array + (index << 4u)
                            : FindNumericKeySlotProbe(ctx, state, key_table, middle);
      uint32_t const filled = LoadU32(ctx, entry + kFindNumericKeySlotValueOffset);
      key = filled ? key : middle;
      lower = filled ? middle : lower;
    }
    goto search_done;
  }
  // The entry is taken, so the next probe starts twice as far out.
  width = 1u;
  lower = key;
  key = key << 1u;
  if (key < kFindNumericKeySlotProbeLimit) {
    goto probe_next;
  }
scan_next:;
  lower = width - 1u;
  entry = upper > lower ? array + (lower << 4u)
                        : FindNumericKeySlotProbe(ctx, state, key_table, width);
  width += 1u;
  if (LoadU32(ctx, entry + kFindNumericKeySlotValueOffset) != 0u) {
    goto scan_next;
  }
search_done:;
  LeaveFrame();
  return lower;
}

}  // namespace artc
