// finish_table_index.cpp — the slot a table write finishes a new key in.
//
// FinishTableIndex produces the 32-byte hash node a new key belongs in, so the
// caller can write the value there. It is the tail of the VM's keyed write: the
// key's type picks a main position among the table's nodes（the flat array part
// is a separate structure and is not consulted here）, and only when that node
// is already taken does the slot get hunted down from the table's free-position
// cursor, which walks the node array downwards. A table with no free node is
// grown first: every key is recounted once by array slice and once by hash
// node, those counts pick the array part's new size, and the resize hands the
// work back through a fresh lookup of the same key. A taken main position whose
// occupant is not in its own main position is relinked — the occupant is copied
// into the free slot, the main position is emptied, and the new key lands
// there; an occupant that is in its own main position simply has the free slot
// chained after it.
//
// The body is suspendable, because the resize and the two self-calls can run
// guest code. Its 112-byte scratch frame is carved off the guest stack（which
// grows down）and handed back on every running exit; on suspension the nine
// words that stay live — the state, the table, the key, the free-position
// cursor, the node, the hash-part base, the new hash size, the new array size
// and the scratch frame itself — go onto the asyncify stack, preceded by the
// resume point naming the call that was in flight. The two rewinding pops stay
// apart in driver order（frame first, then the resume point — the reverse of
// the push order, which is the asyncify stack contract）, and a resumed frame
// replays only the call its resume point names. The two failure texts — a nil
// key and a NaN number key — are raised from the ladder that follows the
// post-resize lookup, which is why the key's type tag is read twice: once to
// pick the main position, once to judge the key.
//
// Node layout: value record at +0, key payload at +16, key's type tag at +24,
// next node at +28. Table layout: the bucket shift at +7, the array part base
// at +12, the hash node base at +16, the free-position cursor at +20, the array
// part size at +28. A bucket is the power-of-two mask of that shift, and the
// divisor is that mask forced odd, so a modulo bucket never divides by zero
// even for an empty table. The absent-key value a failed lookup answers with
// and the dummy node a table with no hash part points at are two separate
// objects in the guest's data segment, one value record and one node apart.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//
#include "util/engine_context.h"
#include <cmath>

namespace artc {

// Forward declaration — defined in script/lookup_table_field.cpp.
uint32_t LookupTableField(EngineContext* ctx, uint32_t table, uint32_t key);
// Forward declaration — defined in script/raise_script_error.cpp.
// PROVISIONAL: the generated f313 body is not registered in rewrite-names.tsv
// yet, so this callee name and its presumed product file are both provisional.
void RaiseScriptError(EngineContext* ctx, uint32_t state, uint32_t message, uint32_t argument);
// Forward declaration — defined in script/resize_table.cpp.
// PROVISIONAL: the generated f1112 body is not registered in rewrite-names.tsv
// yet, so this callee name and its presumed product file are both provisional.
void LookupOrCreateTableField(EngineContext* ctx, uint32_t state, uint32_t table, uint32_t asize, uint32_t nsize);
// Forward declaration — defined in util/bulk_fill.cpp.
// PROVISIONAL: the generated f205 body is not registered in rewrite-names.tsv
// yet, so this callee name and its presumed product file are both provisional.
uint32_t FillBytes(EngineContext* ctx, uint32_t dst, uint32_t value, uint32_t length);
// Forward declaration — defined in script/bucket_log2.cpp.
// PROVISIONAL: the generated f1120 body is not registered in rewrite-names.tsv
// yet, so this callee name and its presumed product file are both provisional.
uint32_t BucketLog2(EngineContext* ctx, uint32_t value);

namespace {

// The array-eligible key count table holds one entry per power-of-two slice,
// which is why the recount zeroes 27 of them（108 bytes）.
constexpr uint32_t kSliceCount = 27u;
constexpr uint32_t kSliceTableBytes = 108u;
constexpr uint32_t kNodeBytes = 32u;
constexpr uint32_t kScratchBytes = 112u;
constexpr uint32_t kSuspendFrameBytes = 36u;
// The largest array index the array part can hold: the last slice of the count
// table, and the fold limit both key counts share.
constexpr uint32_t kMaxArrayKey = 0x3FFFFFFu;
// The magnitude a signed 32-bit fold cannot represent, and the sentinel parked
// for such a key: a value whose back-conversion can never equal the key again,
// so the key is counted as a hash key and only counted as a hash key.
constexpr double kFoldLimit = 2147483648.0;
constexpr uint32_t kUnfoldableKey = 0x80000000u;
// Both diagnostics, the absent-key value a failed lookup answers with, and the
// dummy node a table with no hash part points at all live in the guest's data
// segment beside the shared nil record, so they are named off that anchor
// instead of as raw pool addresses.
constexpr uint32_t kIndexNilTextAddr = kNilValueAddr + 2155u;
constexpr uint32_t kIndexNaNTextAddr = kNilValueAddr + 2174u;
constexpr uint32_t kDummyNodeAddr = kNilValueAddr + 2216u;

// The resume points a suspension spills: zero is the resize, then the two key
// diagnostics, then the two self-calls. A rewinding pass replays only the call
// its resume point names.
constexpr uint32_t kResizeCall = 0u;
constexpr uint32_t kIndexNilKeyCall = 1u;
constexpr uint32_t kGrownInsertCall = 2u;
constexpr uint32_t kIndexNaNKeyCall = 3u;
constexpr uint32_t kSecondInsertCall = 4u;

}  // namespace

uint32_t FinishTableIndex(EngineContext* ctx, uint32_t state, uint32_t table, uint32_t key) {
  EnterFrame();
  // The hash node the key belongs to, and then the node the post-resize lookup
  // answers with: one register carries both, and the running exit hands it back.
  uint32_t slot = 0u;
  // The free-position cursor, the free node it settles on, and the slice's key
  // count while the array part is recounted.
  uint32_t free_pos = 0u;
  // The hash part's base node while a bucket is computed, then the main
  // position of the occupant of a taken bucket — and the short-lived flags the
  // key counts need.
  uint32_t othern = 0u;
  // The asyncify frame pointer on the two rewinding and spilling blocks; the
  // source also reuses this register for the size recount's running counts and
  // for the self-calls' results.
  uint32_t frame = 0u;
  // The register that carries the most: the free-position candidate's
  // predecessor while a slot is hunted, the chain predecessor while a node is
  // relinked, the key's type tag on the validation ladder, and finally the
  // table's new hash size — which is why it is part of the suspend frame.
  uint32_t work = 0u;
  // The bucket counter that walks the hash part, the array index and the
  // candidate size the array recount uses.
  uint32_t index = 0u;
  // The new array size the recount settles on, and the slice index before that.
  uint32_t asize = 0u;
  // The size recount's running and settled counts.
  uint32_t count = 0u;
  uint32_t array_use = 0u;
  uint32_t total_use = 0u;
  uint32_t entry_hit = 0u;
  // The resume point the suspend path pushed, and the code a suspension spills.
  uint32_t resume_point = 0u;
  uint32_t suspend_code = 0u;
  // The guest scratch frame the count table is built in, and the count table
  // itself, one 32-bit entry per power-of-two slice.
  uint32_t sp = 0u;
  uint32_t array_counts = 0u;
  // The key's f64 bit pattern, the key read back as a number, and whether the
  // key itself was counted into a slice.
  uint64_t key_bits = 0u;
  double key_number = 0.0;
  uint32_t extra_hit = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 36-byte frame the suspend path saved, and restore both
    // parameters and the live cursors from it. The table is the last word read,
    // so it replaces the parameter only after every other word has been taken.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSuspendFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    key = LoadU32(ctx, frame + 8u);
    free_pos = LoadU32(ctx, frame + 12u);
    slot = LoadU32(ctx, frame + 16u);
    othern = LoadU32(ctx, frame + 20u);
    work = LoadU32(ctx, frame + 24u);
    asize = LoadU32(ctx, frame + 28u);
    sp = LoadU32(ctx, frame + 32u);
    table = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed together with that frame. Zero
    // means the resize never completed, so it is issued again below.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: carve the scratch frame the count table is built in, and pick
    // the key's main position from its type tag. The array part is not
    // consulted: it is reached through the value, not through this node.
    sp = GuestStackSp(ctx) - kScratchBytes;
    GuestStackSp(ctx) = sp;
    switch (LoadU32(ctx, key + 8u) - 1u) {
      case 0u: goto hash_boolean;
      case 1u: goto hash_light_userdata;
      case 2u: goto hash_number;
      case 3u: goto hash_string;
      default: goto hash_pointer;
    }
  hash_number:;
    {
      // A number key: zero sits at the head of the node array, every other
      // number hashes its f64 bit pattern mixed down to a bucket.
      double const key_read = LoadF64(ctx, key);
      key_number = key_read;
      if (key_read == 0.0) {
        othern = LoadU32(ctx, table + 16u);
        slot = othern;
        goto main_position;
      }
      othern = LoadU32(ctx, table + 16u);
      key_bits = ReinterpretF64(key_number);
      uint32_t const mixed = static_cast<uint32_t>((key_bits >> 32) + key_bits);
      uint32_t const shift = LoadU8(ctx, table + 7u);
      uint32_t const divisor = ((0xFFFFFFFFu << (shift & 31u)) ^ 0xFFFFFFFFu) | 1u;
      slot = othern + ((mixed % divisor) << 5);
      goto main_position;
    }
  hash_string:;
    {
      // A string key carries its own cached hash, so the node compares the key
      // word by identity and the hash is masked straight into a bucket.
      othern = LoadU32(ctx, table + 16u);
      uint32_t const key_word = LoadU32(ctx, LoadU32(ctx, key) + 8u);
      uint32_t const shift = LoadU8(ctx, table + 7u);
      uint32_t const bucket_mask = (0xFFFFFFFFu << (shift & 31u)) ^ 0xFFFFFFFFu;
      slot = othern + ((key_word & bucket_mask) << 5);
      goto main_position;
    }
  hash_boolean:;
    {
      // A boolean key masks its key word into a bucket.
      othern = LoadU32(ctx, table + 16u);
      uint32_t const key_word = LoadU32(ctx, key);
      uint32_t const shift = LoadU8(ctx, table + 7u);
      uint32_t const bucket_mask = (0xFFFFFFFFu << (shift & 31u)) ^ 0xFFFFFFFFu;
      slot = othern + ((key_word & bucket_mask) << 5);
      goto main_position;
    }
  hash_light_userdata:;
    {
      // A light-userdata key hashes its key word modulo the odd divisor.
      othern = LoadU32(ctx, table + 16u);
      uint32_t const key_word = LoadU32(ctx, key);
      uint32_t const shift = LoadU8(ctx, table + 7u);
      uint32_t const divisor = ((0xFFFFFFFFu << (shift & 31u)) ^ 0xFFFFFFFFu) | 1u;
      slot = othern + ((key_word % divisor) << 5);
      goto main_position;
    }
  hash_pointer:;
    {
      // Tables, functions, userdata and threads hash their key word the same
      // modulo way. The source dispatches this family separately from the
      // light-userdata one above, so it stays its own arm.
      othern = LoadU32(ctx, table + 16u);
      uint32_t const key_word = LoadU32(ctx, key);
      uint32_t const shift = LoadU8(ctx, table + 7u);
      uint32_t const divisor = ((0xFFFFFFFFu << (shift & 31u)) ^ 0xFFFFFFFFu) | 1u;
      slot = othern + ((key_word % divisor) << 5);
    }
  main_position:;
    work = static_cast<uint32_t>(slot != kDummyNodeAddr);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: a real main position whose value slot is still empty is the
    // answer, and so is a dummy table's, which is where the new key goes when
    // there is no hash part to hold it. Anything else needs a free place.
    if (work) {
      uint32_t const slot_empty = static_cast<uint32_t>(!LoadU32(ctx, slot + 8u));
      work = slot_empty;
      if (slot_empty) {
        goto store_key;
      }
    }
    free_pos = LoadU32(ctx, table + 20u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: hunt a free place from the free-position cursor, which walks the
    // node array downwards and stops at the base. A cursor that walks out of
    // the array means the table has to grow, below.
    for (;;) {
      work = free_pos;
      free_pos = free_pos - kNodeBytes;
      StoreU32(ctx, table + 20u, free_pos);
      if (othern >= work) {
        goto grow_table;
      }
      if (LoadU32(ctx, work - 8u)) {
        continue;
      }
      break;
    }
    // A free place was found, and the occupant of the main position decides
    // how the new node is threaded in, so the bucket is recomputed from that
    // occupant's own key.
    switch (LoadU32(ctx, slot + 24u) - 1u) {
      case 0u: goto othern_boolean;
      case 1u: goto othern_light_userdata;
      case 2u: goto othern_number;
      case 3u: goto othern_string;
      default: goto othern_pointer;
    }
  grow_table:;
    // No free place: recount every key. The count table lives in the scratch
    // frame, and the array part is walked slice by slice first.
    array_counts = FillBytes(ctx, sp, 0u, kSliceTableBytes);
    slot = LoadU32(ctx, table + 28u);
    index = 1u;
    frame = 1u;
    for (;;) {
      // One slice: the range (2^(lg-1), 2^lg] of array indices, clamped to the
      // array part's size.
      work = frame;
      if (static_cast<int32_t>(slot) < static_cast<int32_t>(frame)) {
        work = slot;
        if (static_cast<int32_t>(index) > static_cast<int32_t>(work)) {
          goto array_counted;
        }
      }
      free_pos = 0u;
      if (static_cast<int32_t>(work) >= static_cast<int32_t>(index)) {
        total_use = LoadU32(ctx, table + 12u);
        for (;;) {
          // Every 16-byte value record whose type tag is not nil counts.
          uint32_t const entry_tag = LoadU32(ctx, total_use + (index << 4) - 8u);
          free_pos = free_pos + static_cast<uint32_t>(entry_tag != 0u);
          uint32_t const more = static_cast<uint32_t>(work != index);
          index = index + 1u;
          if (more) {
            continue;
          }
          break;
        }
        index = work + 1u;
      }
      {
        uint32_t const slice = array_counts + (asize << 2);
        uint32_t const slice_sum = free_pos + LoadU32(ctx, slice);
        StoreU32(ctx, slice, slice_sum);
      }
      frame = frame << 1;
      array_use = free_pos + array_use;
      asize = asize + 1u;
      if (asize != kSliceCount) {
        continue;
      }
      break;
    }
  array_counted:;
    // Then the hash part, node by node, from the top bucket down to the base.
    // The bucket mask doubles as the countdown because the node array is
    // indexed by the same mask.
    index = (0xFFFFFFFFu << LoadU8(ctx, table + 7u)) ^ 0xFFFFFFFFu;
    work = 0u;
    frame = 0u;
    for (;;) {
      free_pos = othern + (index << 5);
      if (LoadU32(ctx, free_pos + 8u)) {
        // A node that is not a number is a hash key; a number is also a hash
        // key unless it folds exactly onto an array index in range.
        slot = 0u;
        othern = static_cast<uint32_t>(LoadU32(ctx, free_pos + 24u) != 3u);
        if (othern) {
          goto node_counted;
        }
        key_number = LoadF64(ctx, free_pos + 16u);
        if (std::fabs(key_number) < kFoldLimit) {
          free_pos = static_cast<uint32_t>(static_cast<int32_t>(key_number));
          goto key_folded;
        }
        free_pos = kUnfoldableKey;
      key_folded:;
        othern = static_cast<uint32_t>(static_cast<double>(static_cast<int32_t>(free_pos)) != key_number);
        if (othern) {
          goto node_counted;
        }
        free_pos = free_pos - 1u;
        othern = static_cast<uint32_t>(free_pos > kMaxArrayKey);
        if (othern) {
          goto node_counted;
        }
        slot = 1u;
        free_pos = array_counts + (BucketLog2(ctx, free_pos) << 2);
        othern = LoadU32(ctx, free_pos + 4u) + 1u;
        StoreU32(ctx, free_pos + 4u, othern);
      node_counted:;
        frame = frame + slot;
        work = work + 1u;
      }
      if (index) {
        index = index - 1u;
        othern = LoadU32(ctx, table + 16u);
        continue;
      }
      break;
    }
    // The key being written counts too, on the same terms.
    slot = 0u;
    index = 1u;
    extra_hit = 0u;
    if (LoadU32(ctx, key + 8u) != 3u) {
      goto extra_key_counted;
    }
    key_number = LoadF64(ctx, key);
    if (std::fabs(key_number) < kFoldLimit) {
      free_pos = static_cast<uint32_t>(static_cast<int32_t>(key_number));
      goto extra_key_folded;
    }
    free_pos = kUnfoldableKey;
  extra_key_folded:;
    othern = static_cast<uint32_t>(static_cast<double>(static_cast<int32_t>(free_pos)) != key_number);
    if (othern) {
      goto extra_key_counted;
    }
    free_pos = free_pos - 1u;
    othern = static_cast<uint32_t>(free_pos > kMaxArrayKey);
    if (othern) {
      goto extra_key_counted;
    }
    free_pos = array_counts + (BucketLog2(ctx, free_pos) << 2);
    othern = LoadU32(ctx, free_pos + 4u) + 1u;
    StoreU32(ctx, free_pos + 4u, othern);
    extra_hit = 1u;
  extra_key_counted:;
    total_use = extra_hit + frame + array_use;
    if (static_cast<int32_t>(total_use) < 1) {
      // Nothing can go into the array part at all.
      frame = 0u;
      asize = 0u;
      goto sizes_computed;
    }
    // Pick the array size: every slice that more than halves its range keeps
    // the keys under it, and the loop stops once every counted key is covered.
    asize = 0u;
    frame = 0u;
    free_pos = 0u;
    othern = 0u;
    for (;;) {
      uint32_t const prev_optimal = asize;
      asize = LoadU32(ctx, array_counts + (othern << 2));
      entry_hit = static_cast<uint32_t>(static_cast<int32_t>(asize) > 0);
      count = free_pos + asize;
      slot = entry_hit & static_cast<uint32_t>(static_cast<int32_t>(count) > static_cast<int32_t>(slot));
      asize = slot ? index : prev_optimal;
      frame = slot ? count : frame;
      free_pos = entry_hit ? count : free_pos;
      slot = static_cast<uint32_t>(total_use == free_pos);
      if (slot) {
        goto sizes_computed;
      }
      othern = othern + 1u;
      slot = index & 0x7FFFFFFFu;
      index = index << 1;
      if (static_cast<int32_t>(slot) < static_cast<int32_t>(total_use)) {
        continue;
      }
      break;
    }
  sizes_computed:;
    // The hash part keeps every key the array part did not take, plus one so
    // that the divisor it is sized by is never zero.
    work = work + array_use - frame + 1u;
  }
  {
    // Running always owes the resize; a rewind owes it only at the resume point
    // it popped above. The select reads the phase for truthiness, so any
    // non-running phase keeps the popped answer.
    uint32_t const resize_pending = static_cast<uint32_t>(!resume_point);
    if (ResumePhase(ctx) ? resize_pending : 1u) {
      LookupOrCreateTableField(ctx, state, table, asize, work);
      if (ResumePhase(ctx) == kResumeSuspended) {
        suspend_code = kResizeCall;
        goto suspend_frame;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: look the key up in the table just grown. A hit needs no new node
    // at all, and the flag byte is cleared first so the table's cached tag
    // methods stay invalidated either way.
    slot = LookupTableField(ctx, table, key);
    StoreU8(ctx, table + 6u, 0u);
    if (slot != kNilValueAddr) {
      goto running_exit;
    }
    work = LoadU32(ctx, key + 8u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The key's own type decides whether it can be written at all: nil names no
    // field, a number that is NaN names no field either, and everything else is
    // written through the call the free-position hunt left undone.
    switch (work) {
      case 0u: goto nil_key;
      case 1u: goto validate_other;
      case 2u: goto validate_other;
      case 3u: goto check_number_key;
      default: goto validate_other;
    }
  nil_key:;
  }
  {
    uint32_t const nil_key_pending = static_cast<uint32_t>(resume_point == kIndexNilKeyCall);
    if (ResumePhase(ctx) ? nil_key_pending : 1u) {
      RaiseScriptError(ctx, state, kIndexNilTextAddr, 0u);
      if (ResumePhase(ctx) == kResumeSuspended) {
        suspend_code = kIndexNilKeyCall;
        goto suspend_frame;
      }
    }
  }
  {
    uint32_t const grown_insert_pending = static_cast<uint32_t>(resume_point == kGrownInsertCall);
    if (ResumePhase(ctx) ? grown_insert_pending : 1u) {
      frame = FinishTableIndex(ctx, state, table, key);
      if (ResumePhase(ctx) == kResumeSuspended) {
        suspend_code = kGrownInsertCall;
        goto suspend_frame;
      }
      slot = frame;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto running_exit;
  }
check_number_key:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // A number key is still unusable when it is NaN, and the engine's only test
    // for that is the read value against its own copy.
    double const key_read = LoadF64(ctx, key);
    key_number = key_read;
    work = static_cast<uint32_t>(key_read == key_number);
    if (work) {
      goto validate_other;
    }
  }
  {
    uint32_t const nan_key_pending = static_cast<uint32_t>(resume_point == kIndexNaNKeyCall);
    if (ResumePhase(ctx) ? nan_key_pending : 1u) {
      RaiseScriptError(ctx, state, kIndexNaNTextAddr, 0u);
      if (ResumePhase(ctx) == kResumeSuspended) {
        suspend_code = kIndexNaNKeyCall;
        goto suspend_frame;
      }
    }
  }
validate_other:;
  {
    uint32_t const second_insert_pending = static_cast<uint32_t>(resume_point == kSecondInsertCall);
    if (ResumePhase(ctx) ? second_insert_pending : 1u) {
      frame = FinishTableIndex(ctx, state, table, key);
      if (ResumePhase(ctx) == kResumeSuspended) {
        suspend_code = kSecondInsertCall;
        goto suspend_frame;
      }
      slot = frame;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto running_exit;
  }
othern_number:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The occupant is a number, so its own main position is the same bucket
    // computation the key went through above.
    key_number = LoadF64(ctx, slot + 16u);
    if (key_number == 0.0) {
      goto link_node;
    }
    key_bits = ReinterpretF64(key_number);
    uint32_t const mixed = static_cast<uint32_t>((key_bits >> 32) + key_bits);
    uint32_t const shift = LoadU8(ctx, table + 7u);
    uint32_t const divisor = ((0xFFFFFFFFu << (shift & 31u)) ^ 0xFFFFFFFFu) | 1u;
    othern = othern + ((mixed % divisor) << 5);
    goto link_node;
  }
othern_string:;
  if (ResumePhase(ctx) == kResumeRunning) {
    uint32_t const key_word = LoadU32(ctx, LoadU32(ctx, slot + 16u) + 8u);
    uint32_t const shift = LoadU8(ctx, table + 7u);
    uint32_t const bucket_mask = (0xFFFFFFFFu << (shift & 31u)) ^ 0xFFFFFFFFu;
    othern = othern + ((key_word & bucket_mask) << 5);
    goto link_node;
  }
othern_boolean:;
  if (ResumePhase(ctx) == kResumeRunning) {
    uint32_t const key_word = LoadU32(ctx, slot + 16u);
    uint32_t const shift = LoadU8(ctx, table + 7u);
    uint32_t const bucket_mask = (0xFFFFFFFFu << (shift & 31u)) ^ 0xFFFFFFFFu;
    othern = othern + ((key_word & bucket_mask) << 5);
    goto link_node;
  }
othern_light_userdata:;
  if (ResumePhase(ctx) == kResumeRunning) {
    uint32_t const key_word = LoadU32(ctx, slot + 16u);
    uint32_t const shift = LoadU8(ctx, table + 7u);
    uint32_t const divisor = ((0xFFFFFFFFu << (shift & 31u)) ^ 0xFFFFFFFFu) | 1u;
    othern = othern + ((key_word % divisor) << 5);
    goto link_node;
  }
othern_pointer:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The occupant hashes its key word the same modulo way, and this arm is the
    // one that falls straight through into the relink below.
    uint32_t const key_word = LoadU32(ctx, slot + 16u);
    uint32_t const shift = LoadU8(ctx, table + 7u);
    uint32_t const divisor = ((0xFFFFFFFFu << (shift & 31u)) ^ 0xFFFFFFFFu) | 1u;
    othern = othern + ((key_word % divisor) << 5);
  }
link_node:;
  if (ResumePhase(ctx) == kResumeRunning) {
    if (slot != othern) {
      // The occupant is out of its main position, so it moves into the free
      // place and the main position is emptied for the new key. The chain is
      // walked to the occupant's predecessor and re-hung on the free place.
      for (;;) {
        work = othern;
        othern = LoadU32(ctx, work + 28u);
        if (slot != othern) {
          continue;
        }
        break;
      }
      StoreU32(ctx, work + 28u, free_pos);
      StoreU64(ctx, free_pos + 24u, LoadU64(ctx, slot + 24u));
      StoreU64(ctx, free_pos + 16u, LoadU64(ctx, slot + 16u));
      StoreU64(ctx, free_pos + 8u, LoadU64(ctx, slot + 8u));
      StoreU64(ctx, free_pos, LoadU64(ctx, slot));
      StoreU32(ctx, slot + 28u, 0u);
      StoreU32(ctx, slot + 8u, 0u);
      goto store_key;
    }
    // The occupant sits in its own main position, so the free place is chained
    // after it and becomes the new key's node.
    StoreU32(ctx, work - 4u, LoadU32(ctx, slot + 28u));
    StoreU32(ctx, slot + 28u, free_pos);
    slot = free_pos;
  }
store_key:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The node is empty, so the key goes in and the value is left for the
    // caller. A collectable key in a black table then has to be barriered: the
    // table is greyed again and relinked into the gray-again list.
    StoreU64(ctx, slot + 16u, LoadU64(ctx, key));
    StoreU32(ctx, slot + 24u, LoadU32(ctx, key + 8u));
    if (static_cast<int32_t>(LoadU32(ctx, key + 8u)) < 4) {
      goto running_exit;
    }
    if (!(LoadU8(ctx, LoadU32(ctx, key) + 5u) & 3u)) {
      goto running_exit;
    }
    if (!(LoadU8(ctx, table + 5u) & 4u)) {
      goto running_exit;
    }
    state = LoadU32(ctx, state + 16u);
    StoreU8(ctx, table + 5u, LoadU8(ctx, table + 5u) & 251u);
    StoreU32(ctx, table + 24u, LoadU32(ctx, state + 40u));
    StoreU32(ctx, state + 40u, table);
  }
running_exit:;
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = sp + kScratchBytes;
    LeaveFrame();
    return slot;
  }
  TrapUnreachable();
suspend_frame:;
  // Push the resume point, then the 36-byte frame holding both parameters, the
  // free-position cursor, the node, the hash-part base, the new hash size, the
  // new array size and the scratch frame, and report the suspension as zero.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), suspend_code);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, table);
  StoreU32(ctx, frame + 8u, key);
  StoreU32(ctx, frame + 12u, free_pos);
  StoreU32(ctx, frame + 16u, slot);
  StoreU32(ctx, frame + 20u, othern);
  StoreU32(ctx, frame + 24u, work);
  StoreU32(ctx, frame + 28u, asize);
  StoreU32(ctx, frame + 32u, sp);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSuspendFrameBytes);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
