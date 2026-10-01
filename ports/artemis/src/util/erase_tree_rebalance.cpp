#include "util/engine_context.h"

namespace artc {

// Unlinks a node from the tagged tree and repairs the red-black balance around
// the hole the removal leaves.  A node carries its left child at +0, its right
// child at +4, its parent at +8 and its colour byte at +12; `a` is the tree root
// and `b` the node to erase, so the whole routine walks guest memory by offset.
void EraseTreeRebalance(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t cursor = 0u;
  uint32_t parent = 0u;
  uint32_t replacement = 0u;
  uint32_t link_cell = 0u;
  uint32_t leaf_removed = 0u;
  uint32_t slot_colour = 0u;
  // Find the successor: the leftmost node of the erased node's right subtree.
  parent = b;
  replacement = LoadU32(ctx, parent);
  if (replacement != 0u) {
    cursor = LoadU32(ctx, b + 4u);
    if (cursor == 0u) goto relink_child;
    // Keep walking left while the candidate still has a left child.
    walk_child_chain:;
    parent = cursor;
    cursor = LoadU32(ctx, cursor);
    if (cursor != 0u) goto walk_child_chain;
  }
  replacement = LoadU32(ctx, parent + 4u);
  if (replacement != 0u) goto relink_child;
  replacement = 0u;
  leaf_removed = 1u;
  goto unlink_done;
  // No left child: this node is the successor, so re-parent it in place.
  relink_child:;
  StoreU32(ctx, replacement + 8u, LoadU32(ctx, parent + 8u));
  leaf_removed = 0u;
  // Splice the successor into the edge that led to the erased node.
  unlink_done:;
  link_cell = LoadU32(ctx, parent + 8u);
  cursor = LoadU32(ctx, link_cell);
  if (parent == cursor) {
    StoreU32(ctx, link_cell, replacement);
    if (a == parent) {
      cursor = 0u;
      a = replacement;
      goto child_spliced;
    }
    cursor = LoadU32(ctx, link_cell + 4u);
    goto child_spliced;
  }
  StoreU32(ctx, link_cell + 4u, replacement);
  child_spliced:;
  slot_colour = LoadU8(ctx, parent + 12u);
  // The successor is a different node: move it into the erased node's slot,
  // inheriting the child links, parent link and colour of the node it replaces.
  if (b != parent) {
    link_cell = LoadU32(ctx, b + 8u);
    StoreU32(ctx, parent + 8u, link_cell);
    StoreU32(ctx, (link_cell + ((LoadU32(ctx, LoadU32(ctx, b + 8u)) != b) << (2u & 31))), parent);
    link_cell = LoadU32(ctx, b);
    StoreU32(ctx, parent, link_cell);
    StoreU32(ctx, link_cell + 8u, parent);
    link_cell = LoadU32(ctx, b + 4u);
    StoreU32(ctx, parent + 4u, link_cell);
    if (link_cell != 0u) {
      StoreU32(ctx, link_cell + 8u, parent);
    }
    StoreU8(ctx, parent + 12u, LoadU8(ctx, b + 12u));
    a = (a == b) ? parent : a;
  }
  if (slot_colour == 0u) goto rebalance_skip;
  if (a == 0u) goto rebalance_skip;
  if (leaf_removed != 0u) {
    // Rebalance upwards from the erased slot until the black height is restored.
    fixup_loop:;
    b = LoadU8(ctx, cursor + 12u);
    parent = LoadU32(ctx, cursor + 8u);
    if (LoadU32(ctx, parent) != cursor) {
      if (b == 0u) {
        StoreU8(ctx, cursor + 12u, 1u);
        StoreU8(ctx, parent + 12u, 0u);
        b = LoadU32(ctx, parent + 4u);
        replacement = LoadU32(ctx, b);
        StoreU32(ctx, parent + 4u, replacement);
        if (replacement != 0u) {
          StoreU32(ctx, replacement + 8u, parent);
        }
        StoreU32(ctx, b + 8u, LoadU32(ctx, parent + 8u));
        replacement = LoadU32(ctx, parent + 8u);
        StoreU32(ctx, (((LoadU32(ctx, replacement) != parent) << (2u & 31)) + replacement), b);
        StoreU32(ctx, b, parent);
        StoreU32(ctx, parent + 8u, b);
        b = LoadU32(ctx, cursor);
        a = (b == a) ? cursor : a;
        cursor = LoadU32(ctx, b + 4u);
      }
      parent = LoadU32(ctx, cursor);
      if (parent != 0u) {
        if (LoadU8(ctx, parent + 12u) == 0u) goto left_child_black;
      }
      b = LoadU32(ctx, cursor + 4u);
      if (b != 0u) {
        if (LoadU8(ctx, b + 12u) == 0u) goto right_child_black;
      }
      StoreU8(ctx, cursor + 12u, 0u);
      cursor = LoadU32(ctx, cursor + 8u);
      if (cursor == a) {
        cursor = a;
        goto paint_black;
      }
      if (LoadU8(ctx, cursor + 12u) != 0u) goto descend_fixup;
      paint_black:;
      StoreU8(ctx, cursor + 12u, 1u);
      goto exit_path;
      left_child_black:;
      b = LoadU32(ctx, cursor + 4u);
      if (b != 0u) goto right_child_black;
      goto rotate_at_parent;
    }
    if (b != 0u) {
      b = cursor;
      goto examine_sibling;
    }
    StoreU8(ctx, cursor + 12u, 1u);
    StoreU8(ctx, parent + 12u, 0u);
    b = LoadU32(ctx, cursor + 4u);
    StoreU32(ctx, parent, b);
    if (b != 0u) {
      StoreU32(ctx, b + 8u, parent);
    }
    StoreU32(ctx, cursor + 8u, LoadU32(ctx, parent + 8u));
    replacement = LoadU32(ctx, parent + 8u);
    if (LoadU32(ctx, replacement) == parent) {
      StoreU32(ctx, replacement, cursor);
      b = LoadU32(ctx, parent);
      goto child_linked;
    }
    StoreU32(ctx, replacement + 4u, cursor);
    child_linked:;
    StoreU32(ctx, cursor + 4u, parent);
    StoreU32(ctx, parent + 8u, cursor);
    a = (a == parent) ? cursor : a;
    examine_sibling:;
    parent = LoadU32(ctx, b);
    if (parent == 0u) goto sibling_left_black;
    if (LoadU8(ctx, parent + 12u) != 0u) goto sibling_left_black;
    cursor = b;
    goto rotate_at_sibling;
    sibling_left_black:;
    cursor = LoadU32(ctx, b + 4u);
    if (cursor != 0u) {
      if (LoadU8(ctx, cursor + 12u) == 0u) goto sibling_right_black;
    }
    StoreU8(ctx, b + 12u, 0u);
    cursor = LoadU32(ctx, b + 8u);
    if (cursor != a) {
      if (LoadU8(ctx, cursor + 12u) != 0u) goto descend_fixup;
    }
    StoreU8(ctx, cursor + 12u, 1u);
    goto exit_path;
    sibling_right_black:;
    if (parent != 0u) {
      if (LoadU8(ctx, parent + 12u) == 0u) {
        cursor = b;
        goto rotate_at_sibling;
      }
      cursor = LoadU32(ctx, b + 4u);
    }
    StoreU8(ctx, cursor + 12u, 1u);
    StoreU8(ctx, b + 12u, 0u);
    a = LoadU32(ctx, cursor);
    StoreU32(ctx, b + 4u, a);
    if (a != 0u) {
      StoreU32(ctx, a + 8u, b);
    }
    StoreU32(ctx, cursor + 8u, LoadU32(ctx, b + 8u));
    a = LoadU32(ctx, b + 8u);
    parent = (LoadU32(ctx, a) != b) << (2u & 31);
    StoreU32(ctx, (a + parent), cursor);
    StoreU32(ctx, cursor, b);
    StoreU32(ctx, b + 8u, cursor);
    parent = b;
    rotate_at_sibling:;
    a = LoadU32(ctx, cursor + 8u);
    StoreU8(ctx, cursor + 12u, LoadU8(ctx, a + 12u));
    StoreU8(ctx, a + 12u, 1u);
    StoreU8(ctx, parent + 12u, 1u);
    b = LoadU32(ctx, a);
    cursor = LoadU32(ctx, b + 4u);
    StoreU32(ctx, a, cursor);
    if (cursor != 0u) {
      StoreU32(ctx, cursor + 8u, a);
    }
    StoreU32(ctx, b + 8u, LoadU32(ctx, a + 8u));
    cursor = LoadU32(ctx, a + 8u);
    parent = (LoadU32(ctx, cursor) != a) << (2u & 31);
    StoreU32(ctx, (cursor + parent), b);
    StoreU32(ctx, b + 4u, a);
    StoreU32(ctx, a + 8u, b);
    goto exit_path;
    descend_fixup:;
    b = LoadU32(ctx, cursor + 8u);
    cursor = (LoadU32(ctx, b) == cursor) << (2u & 31);
    cursor = LoadU32(ctx, (b + cursor));
    goto fixup_loop;
    TrapUnreachable();
  }
  StoreU8(ctx, replacement + 12u, 1u);
  rebalance_skip:;
  goto exit_path;
  // Rotate the sibling up and hand its colour to the new root of the subtree.
  right_child_black:;
  if (LoadU8(ctx, b + 12u) != 0u) goto rotate_at_parent;
  parent = cursor;
  goto rotate_and_exit;
  rotate_at_parent:;
  StoreU8(ctx, parent + 12u, 1u);
  StoreU8(ctx, cursor + 12u, 0u);
  a = LoadU32(ctx, parent + 4u);
  StoreU32(ctx, cursor, a);
  if (a != 0u) {
    StoreU32(ctx, a + 8u, cursor);
  }
  StoreU32(ctx, parent + 8u, LoadU32(ctx, cursor + 8u));
  a = LoadU32(ctx, cursor + 8u);
  b = (cursor != LoadU32(ctx, a)) << (2u & 31);
  StoreU32(ctx, (a + b), parent);
  StoreU32(ctx, parent + 4u, cursor);
  StoreU32(ctx, cursor + 8u, parent);
  b = cursor;
  rotate_and_exit:;
  a = LoadU32(ctx, parent + 8u);
  StoreU8(ctx, parent + 12u, LoadU8(ctx, a + 12u));
  StoreU8(ctx, a + 12u, 1u);
  StoreU8(ctx, b + 12u, 1u);
  b = LoadU32(ctx, a + 4u);
  cursor = LoadU32(ctx, b);
  StoreU32(ctx, a + 4u, cursor);
  if (cursor != 0u) {
    StoreU32(ctx, cursor + 8u, a);
  }
  StoreU32(ctx, b + 8u, LoadU32(ctx, a + 8u));
  cursor = LoadU32(ctx, a + 8u);
  parent = (LoadU32(ctx, cursor) != a) << (2u & 31);
  StoreU32(ctx, (cursor + parent), b);
  StoreU32(ctx, b, a);
  StoreU32(ctx, a + 8u, b);
  exit_path:;
  LeaveFrame();
}

}  // namespace artc
