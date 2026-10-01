#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step drives.
void ReserveClosureRecords(EngineContext* ctx, uint32_t state, uint32_t count);
void ClearFlagsRecursive(EngineContext* ctx, uint32_t state, uint32_t node);
uint32_t FindPoolEntryByTagByte(EngineContext* ctx, uint32_t pool, uint32_t key);
void RelocateElementArray(EngineContext* ctx, uint32_t state, uint32_t table);
uint32_t FetchMetamethod(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

namespace {

constexpr uint32_t kLoadProtoSlotAllBits = 0xFFFFFFFFu;   // all bits set, for the mask below
constexpr uint32_t kLoadProtoSlotFrameBytes = 16u;        // saved frame: four words
constexpr uint32_t kLoadProtoSlotResumeWordBytes = 4u;    // resume word written before it

}  // namespace

// Loads the prototype slot held at state+36: marks the node busy, walks it by
// kind so its sub-nodes drop their flags, then re-measures the chain and grows
// the closure records or the element array to fit it.
uint32_t LoadProtoSlot(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t entry = 0u;   // a node being walked, or a chain index
  uint32_t node = 0u;    // the prototype slot node being loaded
  uint32_t slot = 0u;    // the node kind, an index, or a node address
  uint32_t value = 0u;   // a node address, or a flag word
  uint32_t other = 0u;   // a scratch address
  uint32_t next = 0u;    // a scratch address, or a chain bound
  uint32_t linked = 0u;  // whether the node is already linked into a chain
  uint32_t resume_point = 0u;
  uint32_t frame = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the frame this step saved when it suspended.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kLoadProtoSlotFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    a = LoadU32(ctx, frame);
    entry = LoadU32(ctx, frame + 4u);
    node = LoadU32(ctx, frame + 8u);
    slot = LoadU32(ctx, frame + 12u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word naming the call this step stopped at.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kLoadProtoSlotResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The prototype slot names the node; mark it busy while it is loaded.
    node = LoadU32(ctx, a + 36u);
    slot = LoadU8(ctx, node + 5u) | 4u;
    StoreU8(ctx, node + 5u, slot);
    slot = LoadU8(ctx, node + 4u) - 5u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    switch (slot) {
      case 0: goto detach_node;
      case 1: goto release_slots;
      case 2: goto store_result;
      case 3: goto resize_chain;
      case 4: goto clear_tree;
      default: goto store_result;
    }
  detach_node:
    StoreU32(ctx, a + 36u, LoadU32(ctx, node + 24u));
    entry = LoadU32(ctx, node + 8u);
    if (!entry) goto walk_children;
    if (LoadU8(ctx, entry + 5u) & 3u) {
      ClearFlagsRecursive(ctx, a, entry);
      entry = LoadU32(ctx, node + 8u);
      if (!entry) goto walk_children;
    }
    if (LoadU8(ctx, entry + 6u) & 8u) goto walk_children;
    entry = FetchMetamethod(ctx, entry, 3u, LoadU32(ctx, a + 200u));
    if (!entry) goto walk_children;
    if (LoadU32(ctx, entry + 8u) != 4u) goto walk_children;
    entry = LoadU32(ctx, entry) + 16u;
    slot = FindPoolEntryByTagByte(ctx, entry, 107u);
    entry = FindPoolEntryByTagByte(ctx, entry, 118u);
    if (slot || entry) {
      StoreU8(ctx, node + 5u,
              (LoadU8(ctx, node + 5u) & 231u) | ((entry != 0u) << 4u) | ((slot != 0u) << 3u));
      StoreU32(ctx, node + 24u, LoadU32(ctx, a + 44u));
      StoreU32(ctx, a + 44u, node);
    }
    if (slot && entry) goto clear_flag;
    if (entry) goto clear_chain;
    linked = (slot != 0u);
  walk_children:
    entry = LoadU32(ctx, node + 28u);
    if (entry) {
      // Release the array slots below the node, newest first.
      do {
        entry = entry - 1u;
        slot = LoadU32(ctx, node + 12u) + (entry << 4u);
        if ((uint32_t)((int32_t)LoadU32(ctx, slot + 8u) < (int32_t)4u)) goto next_array_slot;
        slot = LoadU32(ctx, slot);
        if (!(LoadU8(ctx, slot + 5u) & 3u)) goto next_array_slot;
        ClearFlagsRecursive(ctx, a, slot);
      next_array_slot:
        ;
      } while (entry);
    }
    entry = (kLoadProtoSlotAllBits << LoadU8(ctx, node + 7u)) ^ kLoadProtoSlotAllBits;
    if (linked) {
      // The node is linked: release one bucket chain.
      do {
        slot = entry;
        value = LoadU32(ctx, node + 16u) + (entry << 5u);
        entry = LoadU32(ctx, value + 8u);
        if (entry) {
          if ((uint32_t)((int32_t)entry < (int32_t)4u)) goto next_bucket;
          entry = LoadU32(ctx, value);
          if (!(LoadU8(ctx, entry + 5u) & 3u)) goto next_bucket;
          ClearFlagsRecursive(ctx, a, entry);
          goto next_bucket;
        }
        if ((uint32_t)((int32_t)LoadU32(ctx, value + 24u) < (int32_t)4u)) goto next_bucket;
        StoreU32(ctx, value + 24u, 11u);
      next_bucket:
        entry = slot - 1u;
      } while (slot);
      goto scan_done;
    }
    // Not linked: release every bucket of the chain, keeping the flags word.
    do {
      slot = entry;
      next = LoadU32(ctx, node + 16u) + (entry << 5u);
      other = LoadU32(ctx, next + 24u);
      value = LoadU32(ctx, next + 8u);
      if (!value) {
        if ((uint32_t)((int32_t)other < (int32_t)4u)) goto next_slot;
        StoreU32(ctx, next + 24u, 11u);
        goto next_slot;
      }
      if ((uint32_t)((int32_t)other < (int32_t)4u)) goto have_value;
      entry = LoadU32(ctx, next + 16u);
      if (!(LoadU8(ctx, entry + 5u) & 3u)) goto have_value;
      ClearFlagsRecursive(ctx, a, entry);
      value = LoadU32(ctx, next + 8u);
    have_value:
      if ((uint32_t)((int32_t)value < (int32_t)4u)) goto next_slot;
      entry = LoadU32(ctx, next);
      if (!(LoadU8(ctx, entry + 5u) & 3u)) goto next_slot;
      ClearFlagsRecursive(ctx, a, entry);
    next_slot:
      entry = slot - 1u;
    } while (slot);
  scan_done:
    if (!linked) goto finish;
    goto clear_flag;
  release_slots:
    StoreU32(ctx, a + 36u, LoadU32(ctx, node + 8u));
    entry = LoadU32(ctx, node + 12u);
    if (LoadU8(ctx, entry + 5u) & 3u) {
      ClearFlagsRecursive(ctx, a, entry);
    }
    if (LoadU8(ctx, node + 6u)) {
      // A field table: walk the 16-byte records.
      entry = 0u;
      slot = LoadU8(ctx, node + 7u);
      if (!slot) {
        slot = 0u;
        goto slot_count;
      }
      do {
        value = node + (entry << 4u);
        if ((uint32_t)((int32_t)LoadU32(ctx, value + 32u) < (int32_t)4u)) goto next_record;
        value = LoadU32(ctx, value + 24u);
        if (!(LoadU8(ctx, value + 5u) & 3u)) goto next_record;
        ClearFlagsRecursive(ctx, a, value);
        slot = LoadU8(ctx, node + 7u);
      next_record:
        entry = entry + 1u;
      } while (entry < (slot & 255u));
      goto slot_count;
    }
    entry = LoadU32(ctx, node + 16u);
    if (LoadU8(ctx, entry + 5u) & 3u) {
      ClearFlagsRecursive(ctx, a, entry);
    }
    entry = 0u;
    slot = LoadU8(ctx, node + 7u);
    if (!slot) {
      slot = 0u;
      goto slot_count;
    }
    do {
      value = LoadU32(ctx, node + (entry << 2u) + 20u);
      if (LoadU8(ctx, value + 5u) & 3u) {
        ClearFlagsRecursive(ctx, a, value);
        slot = LoadU8(ctx, node + 7u);
      }
      entry = entry + 1u;
    } while (entry < (slot & 255u));
  slot_count:
    a = slot & 255u;
    if (LoadU8(ctx, node + 6u)) {
      LeaveFrame();
      return (a << 4u) + 24u;
    }
    LeaveFrame();
    return (a << 2u) + 20u;
  resize_chain:
    StoreU32(ctx, a + 36u, LoadU32(ctx, node + 108u));
    StoreU32(ctx, node + 108u, LoadU32(ctx, a + 40u));
    StoreU32(ctx, a + 40u, node);
    StoreU8(ctx, node + 5u, LoadU8(ctx, node + 5u) & 251u);
    if ((uint32_t)((int32_t)LoadU32(ctx, node + 80u) < (int32_t)4u)) goto measure_chain;
    entry = LoadU32(ctx, node + 72u);
    if (!(LoadU8(ctx, entry + 5u) & 3u)) goto measure_chain;
    ClearFlagsRecursive(ctx, a, entry);
  measure_chain:
    value = LoadU32(ctx, node + 8u);
    entry = LoadU32(ctx, node + 40u);
    next = LoadU32(ctx, node + 20u);
    if (entry <= next) {
      // First pass: find the highest chain index still in use.
      slot = value;
      do {
        other = LoadU32(ctx, entry + 8u);
        slot = (other > slot) ? other : slot;
        entry = entry + 24u;
      } while (next >= entry);
    }
    entry = LoadU32(ctx, node + 32u);
    if (value > entry) {
      // Second pass: release the nodes the chain no longer reaches.
      do {
        if ((uint32_t)((int32_t)LoadU32(ctx, entry + 8u) < (int32_t)4u)) goto next_chain_node;
        other = LoadU32(ctx, entry);
        if (!(LoadU8(ctx, other + 5u) & 3u)) goto next_chain_node;
        ClearFlagsRecursive(ctx, a, other);
        value = LoadU32(ctx, node + 8u);
      next_chain_node:
        entry = entry + 16u;
      } while (value > entry);
    }
    if (entry <= slot) {
      // Blank the indices that were freed.
      do {
        StoreU32(ctx, entry + 8u, 0u);
        entry = entry + 16u;
      } while (slot >= entry);
    }
    value = LoadU32(ctx, node + 48u);
    a = (uint32_t)((int32_t)value > (int32_t)20000u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (a) goto final_size;
    entry = slot - LoadU32(ctx, node + 32u);
    slot = (uint32_t)((int32_t)value < (int32_t)17u);
    a = I32_DIV_S(LoadU32(ctx, node + 20u) - LoadU32(ctx, node + 40u), 24u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (slot) goto closure_records_ready;
    a = (uint32_t)((int32_t)value <= (int32_t)(a << 2u));
    if (a) goto closure_records_ready;
    a = value >> 1u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    ReserveClosureRecords(ctx, node, a);
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
closure_records_ready:
  if (ResumePhase(ctx) == kResumeRunning) {
    a = LoadU32(ctx, node + 44u);
    entry = (uint32_t)((int32_t)a <= (int32_t)((uint32_t)((int32_t)entry >> 2u)));
    if (entry) goto final_size;
    entry = (uint32_t)((int32_t)a < (int32_t)91u);
    if (entry) goto final_size;
    a = a >> 1u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    RelocateElementArray(ctx, node, a);
    resume_point = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
final_size:
  if (ResumePhase(ctx) == kResumeRunning) {
    LeaveFrame();
    return LoadU32(ctx, node + 48u) * 24u + (LoadU32(ctx, node + 44u) << 4u) + 120u;
  }
clear_tree:
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, a + 36u, LoadU32(ctx, node + 68u));
    entry = LoadU32(ctx, node + 32u);
    if (entry) {
      StoreU8(ctx, entry + 5u, LoadU8(ctx, entry + 5u) & 252u);
    }
    value = LoadU32(ctx, node + 40u);
    if ((uint32_t)((int32_t)value >= (int32_t)1u)) {
      // Release the slots the node owns directly.
      entry = 0u;
      do {
        slot = LoadU32(ctx, node + 8u) + (entry << 4u);
        if ((uint32_t)((int32_t)LoadU32(ctx, slot + 8u) < (int32_t)4u)) goto next_own_slot;
        slot = LoadU32(ctx, slot);
        if (!(LoadU8(ctx, slot + 5u) & 3u)) goto next_own_slot;
        ClearFlagsRecursive(ctx, a, slot);
        value = LoadU32(ctx, node + 40u);
      next_own_slot:
        entry = entry + 1u;
      } while (value > entry);
    }
    value = LoadU32(ctx, node + 36u);
    if ((uint32_t)((int32_t)value >= (int32_t)1u)) {
      // Then the chain of child nodes.
      entry = 0u;
      do {
        slot = LoadU32(ctx, LoadU32(ctx, node + 28u) + (entry << 2u));
        if (slot) {
          StoreU8(ctx, slot + 5u, LoadU8(ctx, slot + 5u) & 252u);
          value = LoadU32(ctx, node + 36u);
        }
        entry = entry + 1u;
      } while (value > entry);
    }
    value = LoadU32(ctx, node + 52u);
    if ((uint32_t)((int32_t)value >= (int32_t)1u)) {
      entry = 0u;
      do {
        slot = LoadU32(ctx, LoadU32(ctx, node + 16u) + (entry << 2u));
        if (!slot) goto next_child;
        if (!(LoadU8(ctx, slot + 5u) & 3u)) goto next_child;
        ClearFlagsRecursive(ctx, a, slot);
        value = LoadU32(ctx, node + 52u);
      next_child:
        entry = entry + 1u;
      } while (value > entry);
    }
    a = LoadU32(ctx, node + 56u);
    if ((uint32_t)((int32_t)a >= (int32_t)1u)) {
      entry = 0u;
      do {
        slot = LoadU32(ctx, LoadU32(ctx, node + 24u) + (entry * 12u));
        if (slot) {
          StoreU8(ctx, slot + 5u, LoadU8(ctx, slot + 5u) & 252u);
          a = LoadU32(ctx, node + 56u);
        }
        entry = entry + 1u;
      } while ((uint32_t)((int32_t)entry < (int32_t)a));
      value = LoadU32(ctx, node + 52u);
    }
    a = a * 12u + ((LoadU32(ctx, node + 40u) + LoadU32(ctx, node + 80u)) << 4u);
    next = LoadU32(ctx, node + 48u) + LoadU32(ctx, node + 44u) + value;
    entry = (next + LoadU32(ctx, node + 36u)) << 2u;
    a = a + entry;
    entry = a + 92u;
  }
store_result:
  if (ResumePhase(ctx) == kResumeRunning) {
    LeaveFrame();
    return entry;
  }
clear_chain:
  if (ResumePhase(ctx) == kResumeRunning) {
    entry = (kLoadProtoSlotAllBits << LoadU8(ctx, node + 7u)) ^ kLoadProtoSlotAllBits;
    if (slot) {
      // Release one bucket chain.
      do {
        a = entry;
        entry = LoadU32(ctx, node + 16u) + (entry << 5u);
        if (LoadU32(ctx, entry + 8u)) goto next_chain_bucket;
        if ((uint32_t)((int32_t)LoadU32(ctx, entry + 24u) < (int32_t)4u)) goto next_chain_bucket;
        StoreU32(ctx, entry + 24u, 11u);
      next_chain_bucket:
        entry = a - 1u;
      } while (a);
      goto clear_flag;
    }
    do {
      slot = entry;
      other = LoadU32(ctx, node + 16u) + (entry << 5u);
      entry = other;
      value = LoadU32(ctx, other + 24u);
      if (LoadU32(ctx, entry + 8u)) {
        if ((uint32_t)((int32_t)value < (int32_t)4u)) goto next_chain_slot;
        entry = LoadU32(ctx, other + 16u);
        if (!(LoadU8(ctx, entry + 5u) & 3u)) goto next_chain_slot;
        ClearFlagsRecursive(ctx, a, entry);
        goto next_chain_slot;
      }
      if ((uint32_t)((int32_t)value < (int32_t)4u)) goto next_chain_slot;
      StoreU32(ctx, entry + 24u, 11u);
    next_chain_slot:
      entry = slot - 1u;
    } while (slot);
  }
clear_flag:
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, node + 5u, LoadU8(ctx, node + 5u) & 251u);
  }
finish:
  if (ResumePhase(ctx) == kResumeRunning) {
    LeaveFrame();
    return (LoadU32(ctx, node + 28u) << 4u) + (32u << LoadU8(ctx, node + 7u)) + 32u;
  }
  TrapUnreachable();
suspend_point:
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kLoadProtoSlotResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, a);
  StoreU32(ctx, frame + 4u, entry);
  StoreU32(ctx, frame + 8u, node);
  StoreU32(ctx, frame + 12u, slot);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kLoadProtoSlotFrameBytes);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
