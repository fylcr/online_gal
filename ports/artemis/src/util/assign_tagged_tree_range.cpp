#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
uint32_t CompareBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void ReleaseNodeTree(EngineContext* ctx, uint32_t state, uint32_t node);
void UnlinkListNode(EngineContext* ctx, uint32_t node, uint32_t victim);
void InsertTaggedTreeNode(EngineContext* ctx, uint32_t a, uint32_t b);

namespace {

constexpr uint32_t kAssignRangePendingFlag = 0xD015Cu;  // the pending-call flag word
constexpr uint32_t kAssignRangeMinusOne = 0xFFFFFFFFu;  // -1 as the compare result sees it
constexpr uint32_t kAssignRangeSaveFrameBytes = 36u;  // live state bytes pushed on suspend
constexpr uint32_t kAssignRangeResumeWordBytes = 4u;  // resume word pushed above that frame

}  // namespace

// Replaces the nodes of a tagged tree that fall inside the key range [b, c):
// the range is unlinked and released, then rebuilt from the bound key nodes
// the comparison walk lands on.
//
// Every callee here can suspend, so each call site carries a resume point naming
// it, and a, b, c, candidate, cursor, node, parent, upper_node and head_cell are
// the live words pushed before the step returns; the two rewind gates at the top
// restore them.
void AssignTaggedTreeRange(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  EnterFrame();
  uint32_t resume_point = 0u;
  uint32_t cmp = 0u;
  uint32_t min_len = 0u;
  uint32_t key_cell = 0u;
  uint32_t candidate = 0u;
  uint32_t cursor = 0u;
  uint32_t scratch = 0u;
  uint32_t node = 0u;
  uint32_t parent = 0u;
  uint32_t upper_node = 0u;
  uint32_t head_cell = 0u;
  // Rewind: drop the 36-byte frame, then the resume word above it.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kAssignRangeSaveFrameBytes);
    scratch = LoadU32(ctx, ReturnFrameSp(ctx));
    a = LoadU32(ctx, scratch);
    c = LoadU32(ctx, scratch + 8u);
    candidate = LoadU32(ctx, scratch + 12u);
    cursor = LoadU32(ctx, scratch + 16u);
    node = LoadU32(ctx, scratch + 20u);
    parent = LoadU32(ctx, scratch + 24u);
    upper_node = LoadU32(ctx, scratch + 28u);
    head_cell = LoadU32(ctx, scratch + 32u);
    b = LoadU32(ctx, scratch + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kAssignRangeResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // An empty tree has nothing to re-key.
    candidate = (LoadU32(ctx, a + 8u) == 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (candidate != 0u) goto finished;
    candidate = LoadU32(ctx, a);
    head_cell = a + 4u;
    StoreU32(ctx, a, head_cell);
    StoreU32(ctx, LoadU32(ctx, a + 4u) + 8u, 0u);
    StoreU64(ctx, a + 4u, 0ull);
    parent = LoadU32(ctx, candidate + 4u);
    node = parent ? parent : candidate;
    candidate = (node == 0u);
    if (candidate != 0u) {
      ReleaseNodeTree(ctx, a, node);
      goto finished;
    }
    // Descend to the leftmost node: it holds the lowest key in the tree.
    parent = LoadU32(ctx, node + 8u);
    if (parent == 0u) goto leftmost_ready;
    candidate = LoadU32(ctx, parent);
    if (node == candidate) {
      StoreU32(ctx, parent, 0u);
      candidate = LoadU32(ctx, parent + 4u);
      if (candidate == 0u) {
        cursor = parent;
        goto leftmost_ready;
      }
      seek_min_left:;
      cursor = candidate;
      candidate = LoadU32(ctx, candidate);
      if (candidate != 0u) goto seek_min_left;
      candidate = LoadU32(ctx, cursor + 4u);
      if (candidate != 0u) goto seek_min_left;
      goto leftmost_ready;
    }
    StoreU32(ctx, parent + 4u, 0u);
    if (candidate == 0u) {
      cursor = parent;
      goto leftmost_ready;
    }
    seek_min_right:;
    cursor = candidate;
    candidate = LoadU32(ctx, candidate);
    if (candidate != 0u) goto seek_min_right;
    candidate = LoadU32(ctx, cursor + 4u);
    if (candidate != 0u) goto seek_min_right;
    leftmost_ready:;
    candidate = (b == c);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (candidate != 0u) {
      parent = node;
      goto release_range;
    }
    upper_node = b;
  }
  // Walk down the tree comparing the node key span with the target range.
  scan_range:;
  if (ResumePhase(ctx) == kResumeRunning) {
    parent = cursor;
    StoreU32(ctx, kAssignRangePendingFlag, 0u);
    cursor = node + 16u;
    b = upper_node + 16u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    scratch = HostOsQuery(ctx, 870u, cursor, b);
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    b = scratch;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    b = LoadU32(ctx, kAssignRangePendingFlag);
    StoreU32(ctx, kAssignRangePendingFlag, 0u);
    b = (b == 1u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (b != 0u) goto release_and_trap;
    StoreU32(ctx, kAssignRangePendingFlag, 0u);
    candidate = upper_node + 28u;
    b = node + 28u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    scratch = HostOsQuery(ctx, 870u, b, candidate);
    resume_point = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    b = scratch;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    b = LoadU32(ctx, kAssignRangePendingFlag);
    StoreU32(ctx, kAssignRangePendingFlag, 0u);
    b = (b == 1u);
    if (b != 0u) goto release_and_trap;
    b = LoadU32(ctx, head_cell);
    if (b == 0u) {
      b = head_cell;
      candidate = b;
      goto link_node;
    }
    candidate = LoadU8(ctx, node + 27u);
    scratch = (static_cast<int32_t>(static_cast<uint32_t>(
      static_cast<int32_t>((candidate << (24u & 31))) >> (24u & 31))) < 0);
    candidate = scratch ? LoadU32(ctx, node + 20u) : candidate;
    scratch = scratch ? LoadU32(ctx, node + 16u) : cursor;
    descend_key:;
    cursor = LoadU8(ctx, b + 27u);
    cmp = (static_cast<int32_t>(static_cast<uint32_t>(
      static_cast<int32_t>((cursor << (24u & 31))) >> (24u & 31))) < 0);
    cursor = cmp ? LoadU32(ctx, b + 20u) : cursor;
    min_len = (cursor < candidate);
    min_len = min_len ? cursor : candidate;
    if (min_len != 0u) {
      key_cell = b + 16u;
      cmp = CompareBytes(ctx, scratch, (cmp ? LoadU32(ctx, key_cell) : key_cell), min_len);
      if (cmp != 0u) goto compare_bytes;
    }
    if (candidate < cursor) goto take_left;
    goto take_right;
    compare_bytes:;
    if (static_cast<int32_t>(cmp) > static_cast<int32_t>(kAssignRangeMinusOne)) goto take_right;
    take_left:;
    cursor = LoadU32(ctx, b);
    if (cursor != 0u) goto descend_next;
    candidate = b;
    goto link_node;
    take_right:;
    cursor = LoadU32(ctx, b + 4u);
    if (cursor != 0u) goto descend_next;
    candidate = b + 4u;
    goto link_node;
    descend_next:;
    b = cursor;
    goto descend_key;
    TrapUnreachable();
  }
  // A callee left a throw pending: release the subtree and rethrow through it.
  release_and_trap:;
  if (ResumePhase(ctx) == kResumeRunning) {
    c = HostRet0(ctx);
    HostTempRet0(ctx);
    ReleaseNodeTree(ctx, a, node);
    if (parent != 0u) {
      walk_up_tree:;
      b = LoadU32(ctx, parent + 8u);
      if (b != 0u) {
        parent = b;
        goto walk_up_tree;
      }
      ReleaseNodeTree(ctx, a, parent);
    }
    HostOsRelease(ctx, c);
    TrapUnreachable();
  }
  // Splice the node into the tree and drop it from the ordered list.
  link_node:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, node + 8u, b);
    StoreU64(ctx, node, 0ull);
    StoreU32(ctx, candidate, node);
    b = LoadU32(ctx, LoadU32(ctx, a));
    if (b != 0u) {
      StoreU32(ctx, a, b);
      node = LoadU32(ctx, candidate);
    }
    UnlinkListNode(ctx, LoadU32(ctx, a + 4u), node);
    StoreU32(ctx, a + 8u, LoadU32(ctx, a + 8u) + 1u);
    cursor = 0u;
    if (parent == 0u) goto node_linked;
    candidate = LoadU32(ctx, parent + 8u);
    if (candidate == 0u) goto node_linked;
    b = LoadU32(ctx, candidate);
    if (parent == b) {
      StoreU32(ctx, candidate, 0u);
      b = LoadU32(ctx, candidate + 4u);
      if (b == 0u) {
        cursor = candidate;
        goto node_linked;
      }
      relink_left:;
      cursor = b;
      b = LoadU32(ctx, b);
      if (b != 0u) goto relink_left;
      b = LoadU32(ctx, cursor + 4u);
      if (b != 0u) goto relink_left;
      goto node_linked;
    }
    StoreU32(ctx, candidate + 4u, 0u);
    if (b == 0u) {
      cursor = candidate;
      goto node_linked;
    }
    relink_right:;
    cursor = b;
    b = LoadU32(ctx, b);
    if (b != 0u) goto relink_right;
    b = LoadU32(ctx, cursor + 4u);
    if (b != 0u) goto relink_right;
    node_linked:;
    candidate = LoadU32(ctx, upper_node + 4u);
    if (candidate == 0u) {
      b = LoadU32(ctx, upper_node + 8u);
      if (upper_node == LoadU32(ctx, b)) goto range_done;
      candidate = upper_node + 8u;
      walk_up_chain:;
      node = LoadU32(ctx, candidate);
      candidate = node + 8u;
      b = LoadU32(ctx, node + 8u);
      if (LoadU32(ctx, b) != node) goto walk_up_chain;
      goto range_done;
    }
    descend_chain:;
    b = candidate;
    candidate = LoadU32(ctx, b);
    if (candidate != 0u) goto descend_chain;
    range_done:;
    candidate = (b == c);
    if (candidate != 0u) goto release_range;
    upper_node = b;
    node = parent;
    if (node != 0u) goto scan_range;
  }
  // Drop the nodes the walk landed on, then release the subtree behind them.
  release_range:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ReleaseNodeTree(ctx, a, parent);
    candidate = (cursor == 0u);
    if (candidate != 0u) goto finished;
    release_subtree:;
    candidate = LoadU32(ctx, cursor + 8u);
    if (candidate != 0u) {
      cursor = candidate;
      goto release_subtree;
    }
    ReleaseNodeTree(ctx, a, cursor);
  }
  finished:;
  cursor = ResumePhase(ctx) ? cursor : (b != c);
  if (cursor != 0u || ResumePhase(ctx) == kResumeRewinding) {
    // Re-insert the range key by key, rebalancing as each node is linked.
    insert_range:;
    if (ResumePhase(ctx) == kResumeRunning) {
      candidate = b + 16u;
      cursor = b;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
      InsertTaggedTreeNode(ctx, a, candidate);
      resume_point = 2u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      candidate = LoadU32(ctx, b + 4u);
      if (candidate == 0u) {
        b = LoadU32(ctx, cursor + 8u);
        candidate = (cursor == LoadU32(ctx, b));
        if (candidate != 0u) goto range_rebuilt;
        cursor = cursor + 8u;
        walk_insert_parent:;
        candidate = LoadU32(ctx, cursor);
        cursor = candidate + 8u;
        b = LoadU32(ctx, candidate + 8u);
        candidate = (LoadU32(ctx, b) != candidate);
        if (candidate != 0u) goto walk_insert_parent;
        goto range_rebuilt;
      }
      descend_insert:;
      b = candidate;
      candidate = LoadU32(ctx, b);
      if (candidate != 0u) goto descend_insert;
      range_rebuilt:;
      cursor = (b != c);
      if (cursor != 0u) goto insert_range;
    }
  }
  goto exit_path;
  suspend_point:;
  // Suspend: push the resume point, then the 36-byte frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kAssignRangeResumeWordBytes);
  scratch = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, scratch, a);
  StoreU32(ctx, scratch + 4u, b);
  StoreU32(ctx, scratch + 8u, c);
  StoreU32(ctx, scratch + 12u, candidate);
  StoreU32(ctx, scratch + 16u, cursor);
  StoreU32(ctx, scratch + 20u, node);
  StoreU32(ctx, scratch + 24u, parent);
  StoreU32(ctx, scratch + 28u, upper_node);
  StoreU32(ctx, scratch + 32u, head_cell);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kAssignRangeSaveFrameBytes);
  exit_path:;
  LeaveFrame();
}

}  // namespace artc
