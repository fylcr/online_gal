#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step drives.
void InitializeTableArray(EngineContext* ctx, uint32_t func_state, uint32_t table, uint32_t size);
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size,
                         uint32_t new_size);
uint32_t RaiseBlockTooBig(EngineContext* ctx, uint32_t a);
void RaiseScriptError(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t FinishTableIndex(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t LookupTableField(EngineContext* ctx, uint32_t a0, uint32_t a1);

namespace {

constexpr uint32_t kLookupTableFieldNil = 0x2B328u;          // sentinel slot: the field is absent
constexpr uint32_t kLookupTableFieldNilMessage = 0x2BB93u;   // "table index is nil"
constexpr uint32_t kLookupTableFieldNaNMessage = 0x2BBA6u;   // "table index is NaN"
constexpr uint32_t kLookupTableFieldNoArray = 0x2BBD0u;      // sentinel: the table has no array
constexpr uint32_t kLookupTableFieldKeyLimit = 0xFFFFFFFu;   // largest key the array part holds
constexpr uint32_t kLookupTableFieldAllBits = 0xFFFFFFFFu;   // all bits set, to build the bucket mask
constexpr uint32_t kLookupTableFieldFrameBytes = 48u;        // saved frame: twelve words
constexpr uint32_t kLookupTableFieldResumeWordBytes = 4u;    // resume word written before it

}  // namespace

// Looks a table field up and creates the slot when it is missing: the array
// part is grown for the key first, then the hash chain is probed and the value
// is written into the slot the probe settles on, running __index/__newindex.
void LookupOrCreateTableField(EngineContext* ctx, uint32_t state, uint32_t table, uint32_t key,
                              uint32_t value) {
  EnterFrame();
  uint32_t frame = 0u;
  uint32_t call_result = 0u;
  uint32_t index = 0u;
  uint32_t entry = 0u;
  uint32_t flag = 0u;
  uint32_t capacity = 0u;
  uint32_t resume_point = 0u;
  uint32_t table_bits = 0u;
  uint32_t scratch_frame = 0u;
  uint32_t array = 0u;
  uint32_t grow_bytes = 0u;
  uint64_t key_bits = 0;
  double key_value = 0;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the frame this step saved when it suspended.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kLookupTableFieldFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    key = LoadU32(ctx, frame + 8u);
    value = LoadU32(ctx, frame + 12u);
    index = LoadU32(ctx, frame + 16u);
    entry = LoadU32(ctx, frame + 20u);
    flag = LoadU32(ctx, frame + 24u);
    capacity = LoadU32(ctx, frame + 28u);
    table_bits = LoadU32(ctx, frame + 32u);
    scratch_frame = LoadU32(ctx, frame + 36u);
    array = LoadU32(ctx, frame + 40u);
    grow_bytes = LoadU32(ctx, frame + 44u);
    table = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word naming the call this step stopped at.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kLookupTableFieldResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Claim the 16-byte scratch area a new field is staged in.
    scratch_frame = GuestStackSp(ctx) - 16u;
    GuestStackSp(ctx) = scratch_frame;
    array = LoadU32(ctx, table + 16u);
    table_bits = LoadU8(ctx, table + 7u);
    capacity = LoadU32(ctx, table + 28u);
    index = (uint32_t)((int32_t)capacity < (int32_t)key);
  }
  if (index || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The key sits past the array's size, so the array has to grow.
      entry = key + 1u <= kLookupTableFieldKeyLimit;
      index = table;
    }
    if (entry || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        flag = capacity << 4u;
        grow_bytes = key << 4u;
        entry = LoadU32(ctx, table + 12u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
        call_result = ReallocateBlock(ctx, state, entry, flag, grow_bytes);
        resume_point = 0u;
        if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
        entry = call_result;
      }
      if (ResumePhase(ctx) == kResumeRunning) goto zero_new_slots;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
      call_result = RaiseBlockTooBig(ctx, state);
      resume_point = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      entry = call_result;
    }
  zero_new_slots:
    if (ResumePhase(ctx) == kResumeRunning) {
      // Record the new slot count and clear every slot it added.
      StoreU32(ctx, index + 12u, entry);
      index = LoadU32(ctx, table + 28u);
      while ((uint32_t)((int32_t)index < (int32_t)key)) {
        StoreU32(ctx, entry + (index << 4u) + 8u, 0u);
        index = index + 1u;
      }
      StoreU32(ctx, table + 28u, key);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
    InitializeTableArray(ctx, state, table, value);
    resume_point = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  value = (ResumePhase(ctx) == kResumeRunning) ? (uint32_t)((int32_t)key < (int32_t)capacity) : value;
  if (value || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The slot count is committed before the chain is probed.
      StoreU32(ctx, table + 28u, key);
      index = key;
    }
    for (;;) {
      if (ResumePhase(ctx) == kResumeRunning) {
        entry = LoadU32(ctx, table + 12u) + (index << 4u);
        flag = LoadU32(ctx, entry + 8u);
        value = index + 1u;
      }
      if (flag || ResumePhase(ctx) == kResumeRewinding) {
        if (ResumePhase(ctx) == kResumeRunning) {
          flag = LoadU32(ctx, table + 28u);
          index = flag > index;
        }
        if (ResumePhase(ctx) == kResumeRunning) {
          // Off the bucket hash when the walk stopped on a live slot.
          if (index) {
            index = entry;
            goto chain_match;
          }
          key_value = (double)(int32_t)value;
          if (value == 0u) {
            index = LoadU32(ctx, table + 16u);
          } else {
            key_bits = i64_reinterpret_f64(key_value);
            index = LoadU32(ctx, table + 16u) +
                    (REM_U((uint32_t)(key_bits + (key_bits >> 32ull)),
                           ((kLookupTableFieldAllBits << LoadU8(ctx, table + 7u)) ^
                            kLookupTableFieldAllBits) |
                               1u)
                     << 5u);
          }
          for (;;) {
            flag = LoadU32(ctx, index + 24u) == 3u;
            if (flag) {
              flag = LoadF64(ctx, index + 16u) == key_value;
              if (flag) goto chain_match;
            }
            index = LoadU32(ctx, index + 28u);
            if (index == 0u) break;
          }
          goto stage_value;
        chain_match:
          flag = (uint32_t)(index != kLookupTableFieldNil);
          if (flag) goto copy_slot_value;
          key_value = (double)(int32_t)value;
        stage_value:
          // Stage the new field in the scratch area for the index metamethod.
          StoreU32(ctx, scratch_frame + 8u, 3u);
          StoreF64(ctx, scratch_frame, key_value);
        }
        if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
          call_result = FinishTableIndex(ctx, state, table, scratch_frame);
          resume_point = 3u;
          if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
          index = call_result;
        }
      copy_slot_value:
        if (ResumePhase(ctx) == kResumeRunning) {
          // Copy the slot that was found over the one the index returned.
          StoreU64(ctx, index, LoadU64(ctx, entry));
          entry = LoadU32(ctx, entry + 8u);
          StoreU32(ctx, index + 8u, entry);
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        // The walk advances one slot and ends once it has covered the array.
        index = value;
        value = (uint32_t)(capacity != value);
        if (value) continue;
      }
      break;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The array part may still have to be grown for the same key.
    index = key + 1u <= kLookupTableFieldKeyLimit;
    value = table;
  }
  if (index || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      index = LoadU32(ctx, table + 12u);
      entry = capacity << 4u;
      key = key << 4u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
      call_result = ReallocateBlock(ctx, state, index, entry, key);
      resume_point = 4u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      key = call_result;
    }
    if (ResumePhase(ctx) == kResumeRunning) goto commit_array_size;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
    call_result = RaiseBlockTooBig(ctx, state);
    resume_point = 5u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    key = call_result;
  }
commit_array_size:
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, value + 12u, key);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The hash width decides the bucket mask and the size of one bucket.
    value = table_bits != 31u;
    key = 1u << table_bits;
  }
  if (value || ResumePhase(ctx) == kResumeRewinding) {
    entry = (ResumePhase(ctx) == kResumeRunning) ? key : entry;
    for (;;) {
      if (ResumePhase(ctx) == kResumeRunning) {
        // One bucket per probe: its address, then the value it holds.
        index = entry - 1u;
        capacity = array + (index << 5u);
        value = LoadU32(ctx, capacity + 8u);
      }
      if (value || ResumePhase(ctx) == kResumeRewinding) {
        if (ResumePhase(ctx) == kResumeRunning) {
          // The bucket's key decides which error the index raises.
          table_bits = capacity + 16u;
          value = LookupTableField(ctx, table, table_bits);
          StoreU8(ctx, table + 6u, 0u);
          flag = value == kLookupTableFieldNil;
        }
        if (flag || ResumePhase(ctx) == kResumeRewinding) {
          if (ResumePhase(ctx) == kResumeRunning) {
            value = LoadU32(ctx, capacity + 24u);
          }
          if (ResumePhase(ctx) == kResumeRunning) {
            switch (value) {
              case 0: goto index_is_nil;
              case 1: goto index_ready;
              case 2: goto index_ready;
              case 3: goto check_nan_key;
              default: goto index_ready;
            }
          index_is_nil:
          }
          if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
            RaiseScriptError(ctx, state, kLookupTableFieldNilMessage, 0u);
            resume_point = 6u;
            if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
          }
          if (ResumePhase(ctx) == kResumeRunning) goto index_ready;
        check_nan_key:
          if (ResumePhase(ctx) == kResumeRunning) {
            // A NaN key matches nothing, so it is rejected before indexing.
            key_value = LoadF64(ctx, table_bits);
            value = key_value == key_value;
            if (value) goto index_ready;
          }
          if (ResumePhase(ctx) == kResumeRunning || resume_point == 7u) {
            RaiseScriptError(ctx, state, kLookupTableFieldNaNMessage, 0u);
            resume_point = 7u;
            if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
          }
        index_ready:
          if (ResumePhase(ctx) == kResumeRunning || resume_point == 8u) {
            call_result = FinishTableIndex(ctx, state, table, table_bits);
            resume_point = 8u;
            if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
            value = call_result;
          }
        }
        if (ResumePhase(ctx) == kResumeRunning) {
          // Copy the bucket over the slot the index returned.
          StoreU64(ctx, value, LoadU64(ctx, capacity));
          capacity = LoadU32(ctx, capacity + 8u);
          StoreU32(ctx, value + 8u, capacity);
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        // Keep probing until the walk has entered the last bucket.
        value = (uint32_t)((int32_t)entry > (int32_t)1);
        entry = index;
        if (value) continue;
      }
      break;
    }
  }
  table = (ResumePhase(ctx) == kResumeRunning) ? (uint32_t)(array != kLookupTableFieldNoArray) : table;
  if (table || ResumePhase(ctx) == kResumeRewinding) {
    table = (ResumePhase(ctx) == kResumeRunning) ? (key << 5u) : table;
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 9u) {
      ReallocateBlock(ctx, state, array, table, 0u);
      resume_point = 9u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = scratch_frame + 16u;
  }
  goto step_done;
suspend_point:
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kLookupTableFieldResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, table);
  StoreU32(ctx, frame + 8u, key);
  StoreU32(ctx, frame + 12u, value);
  StoreU32(ctx, frame + 16u, index);
  StoreU32(ctx, frame + 20u, entry);
  StoreU32(ctx, frame + 24u, flag);
  StoreU32(ctx, frame + 28u, capacity);
  StoreU32(ctx, frame + 32u, table_bits);
  StoreU32(ctx, frame + 36u, scratch_frame);
  StoreU32(ctx, frame + 40u, array);
  StoreU32(ctx, frame + 44u, grow_bytes);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kLookupTableFieldFrameBytes);
step_done:
  LeaveFrame();
}

}  // namespace artc
