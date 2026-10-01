#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
void UnlinkListNode(EngineContext* ctx, uint32_t node, uint32_t victim);

namespace {

constexpr uint32_t kRebuildBucketSaveFrameBytes = 40u;  // live state bytes pushed on suspend
constexpr uint32_t kRebuildBucketResumeWordBytes = 4u;  // resume word pushed above that frame

}  // namespace

// Grows the bucket array of an unordered table: the node chain is walked, the
// entries whose bucket changed are unlinked and re-linked under the new count,
// and the collision handler is dispatched through the entry's table.
//
// Every callee here can suspend, so each call site carries a resume point naming
// it, and a, b, node, link_cell, block, value, flag, target, saved and cursor are
// the live words pushed before the step returns; the two rewind gates at the top
// restore them.
uint32_t RebuildUnorderedBucketArray(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t saved = 0u;
  uint32_t cursor = 0u;
  uint32_t node = 0u;
  uint32_t link_cell = 0u;
  uint32_t block = 0u;
  uint32_t value = 0u;
  uint32_t new_node = 0u;
  uint32_t frame = 0u;
  uint32_t flag = 0u;
  uint32_t target = 0u;
  uint32_t resume_point = 0u;
  uint32_t result = 0u;
  // Rewind: drop the 40-byte frame, then the resume word above it.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kRebuildBucketSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    a = LoadU32(ctx, frame);
    node = LoadU32(ctx, frame + 8u);
    link_cell = LoadU32(ctx, frame + 12u);
    block = LoadU32(ctx, frame + 16u);
    value = LoadU32(ctx, frame + 20u);
    flag = LoadU32(ctx, frame + 24u);
    target = LoadU32(ctx, frame + 28u);
    saved = LoadU32(ctx, frame + 32u);
    cursor = LoadU32(ctx, frame + 36u);
    b = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kRebuildBucketResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  // First bucket: walk its tree looking for the entry's slot.
  if (ResumePhase(ctx) == kResumeRunning) {
    link_cell = a + 24u;
    node = LoadU32(ctx, link_cell);
    if (node != 0u) {
      link_cell = a + 24u;
      descend_node:;
      block = LoadU32(ctx, node + 16u);
      if (static_cast<int32_t>(block) > static_cast<int32_t>(b)) {
        block = LoadU32(ctx, node);
        if (block != 0u) goto step_child;
        link_cell = node;
        goto leaf_slot;
      }
      if (static_cast<int32_t>(b) <= static_cast<int32_t>(block)) goto leaf_slot;
      link_cell = node + 4u;
      block = LoadU32(ctx, node + 4u);
      if (block == 0u) goto leaf_slot;
      node = link_cell;
      step_child:;
      link_cell = node;
      node = block;
      goto descend_node;
      TrapUnreachable();
    }
    node = link_cell;
    leaf_slot:;
    block = LoadU32(ctx, link_cell);
    value = (block == 0u);
  }
  if (value != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      // No slot in this bucket: allocate the entry node and link it in.
      new_node = AllocateStringBuffer(ctx, 24u);
      resume_point = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      block = new_node;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU8(ctx, block + 20u, 0u);
      StoreU32(ctx, block + 16u, b);
      StoreU32(ctx, block + 8u, node);
      StoreU64(ctx, block, 0ull);
      StoreU32(ctx, link_cell, block);
      value = block;
      node = LoadU32(ctx, LoadU32(ctx, a + 20u));
      flag = (node == 0u);
      if (flag == 0u) {
        StoreU32(ctx, a + 20u, node);
        value = LoadU32(ctx, link_cell);
      }
      link_cell = LoadU32(ctx, a + 24u);
      UnlinkListNode(ctx, link_cell, value);
      node = LoadU32(ctx, a + 28u) + 1u;
      StoreU32(ctx, a + 28u, node);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    value = 0u;
    node = !(LoadU8(ctx, block + 20u));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (node != 0u) goto value_ready;
    node = LoadU32(ctx, a + 4u);
    link_cell = LoadU32(ctx, LoadU32(ctx, node) + 32u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
    #endif
    new_node = CallTableT3(ctx, link_cell, node, 136u);
    resume_point = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    value = new_node;
  }
  value_ready:;
  if (ResumePhase(ctx) == kResumeRunning) {
    link_cell = a + 36u;
    node = LoadU32(ctx, link_cell);
    if (node != 0u) {
      link_cell = a + 36u;
      descend_child:;
      block = LoadU32(ctx, node + 16u);
      if (static_cast<int32_t>(block) > static_cast<int32_t>(b)) {
        block = LoadU32(ctx, node);
        if (block != 0u) goto step_down;
        link_cell = node;
        goto leaf_linked;
      }
      if (static_cast<int32_t>(b) <= static_cast<int32_t>(block)) goto leaf_linked;
      link_cell = node + 4u;
      block = LoadU32(ctx, node + 4u);
      if (block == 0u) goto leaf_linked;
      node = link_cell;
      step_down:;
      link_cell = node;
      node = block;
      goto descend_child;
      TrapUnreachable();
    }
    node = link_cell;
    leaf_linked:;
    block = LoadU32(ctx, link_cell);
    flag = (block == 0u);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
      new_node = AllocateStringBuffer(ctx, 24u);
      resume_point = 2u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      block = new_node;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU8(ctx, block + 20u, 0u);
      StoreU32(ctx, block + 16u, b);
      StoreU32(ctx, block + 8u, node);
      StoreU64(ctx, block, 0ull);
      StoreU32(ctx, link_cell, block);
      flag = block;
      node = LoadU32(ctx, LoadU32(ctx, a + 32u));
      target = (node == 0u);
      if (target == 0u) {
        StoreU32(ctx, a + 32u, node);
        flag = LoadU32(ctx, link_cell);
      }
      link_cell = LoadU32(ctx, a + 36u);
      UnlinkListNode(ctx, link_cell, flag);
      node = LoadU32(ctx, a + 40u) + 1u;
      StoreU32(ctx, a + 40u, node);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    link_cell = 0u;
    node = !(LoadU8(ctx, block + 20u));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (node != 0u) goto table_call_done;
    node = LoadU32(ctx, a + 4u);
    link_cell = LoadU32(ctx, LoadU32(ctx, node) + 32u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
    #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
    #endif
    new_node = CallTableT3(ctx, link_cell, node, 137u);
    resume_point = 3u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    link_cell = new_node;
  }
  table_call_done:;
  // Merge the call outcomes: running takes the rebuilt value, rewinding keeps
  // the restored one.
  node = ResumePhase(ctx) ? node : (link_cell | value);
  saved = ResumePhase(ctx) ? saved : node;
  a = (saved | (ResumePhase(ctx) == 2u)) ? (ResumePhase(ctx) ? a : 1u) : a;
  if (!(saved) || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      flag = a + 12u;
      node = LoadU32(ctx, flag);
      if (node != 0u) {
        link_cell = a + 12u;
        descend_list:;
        block = LoadU32(ctx, node + 16u);
        if (static_cast<int32_t>(block) > static_cast<int32_t>(b)) {
          block = LoadU32(ctx, node);
          if (block != 0u) goto step_list_child;
          link_cell = node;
          goto list_slot;
        }
        if (static_cast<int32_t>(b) <= static_cast<int32_t>(block)) goto list_slot;
        link_cell = node + 4u;
        block = LoadU32(ctx, node + 4u);
        if (block == 0u) goto list_slot;
        node = link_cell;
        step_list_child:;
        link_cell = node;
        node = block;
        goto descend_list;
        TrapUnreachable();
      }
      node = flag;
      link_cell = node;
      list_slot:;
      block = LoadU32(ctx, link_cell);
      value = (block == 0u);
    }
    if (value != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
        new_node = AllocateStringBuffer(ctx, 32u);
        resume_point = 4u;
        if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
        block = new_node;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU64(ctx, block + 24u, 0ull);
        StoreU32(ctx, block + 16u, b);
        StoreU32(ctx, block + 8u, node);
        StoreU64(ctx, block, 0ull);
        StoreU32(ctx, block + 20u, block + 24u);
        StoreU32(ctx, link_cell, block);
        value = block;
        node = LoadU32(ctx, LoadU32(ctx, a + 8u));
        target = (node == 0u);
        if (target == 0u) {
          StoreU32(ctx, a + 8u, node);
          value = LoadU32(ctx, link_cell);
        }
        link_cell = LoadU32(ctx, a + 12u);
        UnlinkListNode(ctx, link_cell, value);
        node = LoadU32(ctx, a + 16u) + 1u;
        StoreU32(ctx, a + 16u, node);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      cursor = a + 12u;
      block = LoadU32(ctx, block + 20u);
    }
    // Rewalk the chain after the handler changed the bucket.
    rescan_chain:;
    if (ResumePhase(ctx) == kResumeRunning) {
      link_cell = cursor;
      node = LoadU32(ctx, flag);
      if (node == 0u) {
        node = flag;
        link_cell = node;
        goto chain_tail;
      }
      walk_chain:;
      value = LoadU32(ctx, node + 16u);
      if (static_cast<int32_t>(value) > static_cast<int32_t>(b)) {
        value = LoadU32(ctx, node);
        if (value != 0u) goto step_chain_child;
        link_cell = node;
        goto chain_tail;
      }
      if (static_cast<int32_t>(b) <= static_cast<int32_t>(value)) goto chain_tail;
      link_cell = node + 4u;
      value = LoadU32(ctx, node + 4u);
      if (value == 0u) goto chain_tail;
      node = link_cell;
      step_chain_child:;
      link_cell = node;
      node = value;
      goto walk_chain;
      TrapUnreachable();
      chain_tail:;
      value = LoadU32(ctx, link_cell);
      target = (value == 0u);
    }
    if (target != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
        new_node = AllocateStringBuffer(ctx, 32u);
        resume_point = 5u;
        if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
        value = new_node;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU64(ctx, value + 24u, 0ull);
        StoreU32(ctx, value + 16u, b);
        StoreU32(ctx, value + 8u, node);
        StoreU64(ctx, value, 0ull);
        StoreU32(ctx, value + 20u, value + 24u);
        StoreU32(ctx, link_cell, value);
        target = value;
        node = LoadU32(ctx, LoadU32(ctx, a + 8u));
        if (node != 0u) {
          StoreU32(ctx, a + 8u, node);
          target = LoadU32(ctx, link_cell);
        }
        link_cell = LoadU32(ctx, a + 12u);
        UnlinkListNode(ctx, link_cell, target);
        node = LoadU32(ctx, a + 16u) + 1u;
        StoreU32(ctx, a + 16u, node);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      node = value + 24u;
      link_cell = (block == node);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (link_cell != 0u) goto rescan_done;
      link_cell = LoadU32(ctx, a + 4u);
      target = LoadU32(ctx, LoadU32(ctx, link_cell) + 32u);
      value = LoadU32(ctx, block + 16u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
      #if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
      #endif
      new_node = CallTableT3(ctx, target, link_cell, value);
      resume_point = 6u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      link_cell = new_node;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (link_cell != 0u) goto rescan_done;
      node = LoadU32(ctx, block + 4u);
      link_cell = (node == 0u);
      if (link_cell != 0u) {
        link_cell = block + 8u;
        node = block;
        block = LoadU32(ctx, node + 8u);
        value = LoadU32(ctx, block);
        node = (node == value);
        if (node != 0u) goto rescan_chain;
        walk_parents:;
        node = LoadU32(ctx, link_cell);
        link_cell = node + 8u;
        block = LoadU32(ctx, node + 8u);
        value = LoadU32(ctx, block);
        node = (value != node);
        if (node != 0u) goto walk_parents;
        goto rescan_chain;
      }
      walk_leftmost:;
      block = node;
      node = LoadU32(ctx, node);
      if (node != 0u) goto walk_leftmost;
      goto rescan_chain;
    }
    rescan_done:;
    a = ResumePhase(ctx) ? a : (node != block);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    result = a;
    goto exit_path;
  }
  TrapUnreachable();
  suspend_point:;
  // Suspend: push the resume point, then the 40-byte frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kRebuildBucketResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, a);
  StoreU32(ctx, frame + 4u, b);
  StoreU32(ctx, frame + 8u, node);
  StoreU32(ctx, frame + 12u, link_cell);
  StoreU32(ctx, frame + 16u, block);
  StoreU32(ctx, frame + 20u, value);
  StoreU32(ctx, frame + 24u, flag);
  StoreU32(ctx, frame + 28u, target);
  StoreU32(ctx, frame + 32u, saved);
  StoreU32(ctx, frame + 36u, cursor);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kRebuildBucketSaveFrameBytes);
  result = 0u;
  exit_path:;
  LeaveFrame();
  return result;
}

}  // namespace artc
