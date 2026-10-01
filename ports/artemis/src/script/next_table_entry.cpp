#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
void RaiseScriptError(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t RawValueEqual(EngineContext* ctx, uint32_t a, uint32_t b);

namespace {

constexpr uint32_t kNextTableEntryInvalidKeyMessage = 0x2BBB9u;  // "invalid key to 'next'"
constexpr uint32_t kNextTableEntryIntMin = 0x80000000u;          // out-of-range truncation result
constexpr uint32_t kNextTableEntryAllOnes = 0xFFFFFFFFu;         // mask base and "no slot" cursor
constexpr uint32_t kNextTableEntrySaveFrameBytes = 16u;          // frame words
constexpr uint32_t kNextTableEntryResumeWordBytes = 4u;          // resume code slot

}  // namespace

// Walks to the next pair: the successor of the current entry is read and a key
// that is not in the table raises the "invalid key to 'next'" error.  The calls
// can suspend, so the state, the table, the key and the cursor are in the frame.
uint32_t NextTableEntry(EngineContext* ctx, uint32_t state, uint32_t table, uint32_t key) {
  EnterFrame();
  uint32_t cursor = 0u;        // entry address, then entry index, then bucket address
  uint32_t slot = 0u;          // the slot the walk is looking at
  uint32_t resume_point = 0u;
  uint32_t kind = 0u;          // the key's tag
  uint32_t entry_count = 0u;   // entries in the array part
  uint32_t bucket_count = 0u;  // buckets in the hash part
  uint32_t index = 0u;
  uint32_t result = 0u;
  double key_value = 0.0;      // the key's numeric value
  uint64_t key_bits = 0;       // ... and the bits the hash of it is built from
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 16-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kNextTableEntrySaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    table = LoadU32(ctx, frame + 4u);
    key = LoadU32(ctx, frame + 8u);
    cursor = LoadU32(ctx, frame + 12u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame that saves the resume code.
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kNextTableEntryResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    kind = LoadU32(ctx, key + 8u);
    cursor = kNextTableEntryAllOnes;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    switch (kind) {
      case 0: goto index_ready;
      case 1: goto key_word_masked;
      case 2: goto record_word_modulo;
      case 3: goto integer_key;
      case 4: goto record_word_masked;
      default: goto key_word_modulo;
    }
integer_key:
    // A whole number: a usable index is the entry before it, anything else is
    // hashed like the other keys.
    key_value = LoadF64(ctx, key);
    if (wasm_fabs(key_value) < 2147483648.0) {
      cursor = I32_TRUNC_S_F64(key_value);
    } else {
      cursor = kNextTableEntryIntMin;
    }
    if ((int32_t)cursor < 1) { goto hash_lookup; }
    if ((double)(int32_t)cursor != key_value) { goto hash_lookup; }
    if ((int32_t)cursor > (int32_t)LoadU32(ctx, table + 28u)) { goto hash_lookup; }
    cursor -= 1u;
    goto index_ready;
hash_lookup:
    cursor = LoadU32(ctx, table + 16u);
    if (key_value != 0.0) {
      key_bits = ReinterpretF64(key_value);
      key_bits += key_bits >> 32ull;
      index = (uint32_t)key_bits;
      index = REM_U(index, ((kNextTableEntryAllOnes << (LoadU8(ctx, table + 7u) & 31u)) ^
          kNextTableEntryAllOnes) | 1u);
      cursor = cursor + (index << 5u);
    }
    goto entry_walk;
record_word_masked:
    cursor = LoadU32(ctx, table + 16u);
    cursor = cursor + ((LoadU32(ctx, LoadU32(ctx, key) + 8u) &
        ((kNextTableEntryAllOnes << (LoadU8(ctx, table + 7u) & 31u)) ^
        kNextTableEntryAllOnes)) << 5u);
    goto entry_walk;
key_word_masked:
    cursor = LoadU32(ctx, table + 16u);
    cursor = cursor + ((LoadU32(ctx, key) &
        ((kNextTableEntryAllOnes << (LoadU8(ctx, table + 7u) & 31u)) ^
        kNextTableEntryAllOnes)) << 5u);
    goto entry_walk;
record_word_modulo:
    cursor = LoadU32(ctx, table + 16u);
    cursor = cursor + (REM_U(LoadU32(ctx, LoadU32(ctx, key) + 8u),
        ((kNextTableEntryAllOnes << (LoadU8(ctx, table + 7u) & 31u)) ^
        kNextTableEntryAllOnes) | 1u) << 5u);
    goto entry_walk;
key_word_modulo:
    cursor = LoadU32(ctx, table + 16u);
    cursor = cursor + (REM_U(LoadU32(ctx, key),
        ((kNextTableEntryAllOnes << (LoadU8(ctx, table + 7u) & 31u)) ^
        kNextTableEntryAllOnes) | 1u) << 5u);
entry_walk:
    // Follow the bucket chain until an entry holding the key is found.
    for (;;) {
      if (RawValueEqual(ctx, cursor + 16u, key) != 0u ||
          (LoadU32(ctx, cursor + 24u) == 11u &&
           (int32_t)LoadU32(ctx, key + 8u) >= 4 &&
           LoadU32(ctx, cursor + 16u) == LoadU32(ctx, key))) {
        cursor = LoadU32(ctx, table + 28u) +
            ((int32_t)(cursor - LoadU32(ctx, table + 16u)) >> 5u);
        goto index_ready;
      }
      cursor = LoadU32(ctx, cursor + 28u);
      if (cursor == 0u) { break; }
    }
    cursor = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    // RaiseScriptError can suspend, so the values that outlive it are in the frame.
    RaiseScriptError(ctx, state, kNextTableEntryInvalidKeyMessage, 0u);
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
index_ready:
  if (ResumePhase(ctx) == kResumeRunning) {
    resume_point = cursor + 1u;
    entry_count = LoadU32(ctx, table + 28u);
    for (;;) {
      slot = cursor;
      cursor = cursor + 1u;
      if ((int32_t)cursor >= (int32_t)entry_count) { goto scan_buckets; }
      if (LoadU32(ctx, LoadU32(ctx, table + 12u) + (cursor << 4u) + 8u) == 0u) { continue; }
      break;
    }
    // The array part has a free slot: the key becomes the pair before it.
    StoreU32(ctx, key + 8u, 3u);
    StoreF64(ctx, key, (double)(int32_t)(slot + 2u));
    slot = LoadU32(ctx, table + 12u) + (cursor << 4u);
    StoreU64(ctx, key + 16u, LoadU64(ctx, slot));
    goto entry_found;
scan_buckets:
    cursor = (int32_t)entry_count > (int32_t)resume_point ? 0u : resume_point - entry_count;
    bucket_count = 1u << LoadU8(ctx, table + 7u);
    if ((int32_t)cursor < (int32_t)bucket_count) {
      slot = LoadU32(ctx, table + 16u);
      for (;;) {
        if (LoadU32(ctx, slot + (cursor << 5u) + 8u) != 0u) {
          cursor = cursor << 5u;
          slot = slot + cursor;
          StoreU64(ctx, key, LoadU64(ctx, slot + 16u));
          StoreU32(ctx, key + 8u, LoadU32(ctx, slot + 24u));
          slot = cursor + LoadU32(ctx, table + 16u);
          StoreU64(ctx, key + 16u, LoadU64(ctx, slot));
          goto entry_found;
        }
        cursor = cursor + 1u;
        if ((int32_t)cursor >= (int32_t)bucket_count) { break; }
      }
    }
    result = 0u;
    goto exit_path;
entry_found:
    StoreU32(ctx, key + 24u, LoadU32(ctx, slot + 8u));
    result = 1u;
    goto exit_path;
  }
  TrapUnreachable();
suspend_point:
  // Suspend: write the resume code, then push the frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kNextTableEntryResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, state);
    StoreU32(ctx, frame + 4u, table);
    StoreU32(ctx, frame + 8u, key);
    StoreU32(ctx, frame + 12u, cursor);
  }
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kNextTableEntrySaveFrameBytes);
  result = 0u;
exit_path:
  LeaveFrame();
  return result;
}

}  // namespace artc
