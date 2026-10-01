#include "util/engine_context.h"

namespace artc {

// Splices a node out of a doubly linked chain.  Either order is accepted: the
// node and the victim may be given the other way round, and the chain is
// walked back through the marking byte every link carries until the survivor
// is the node the walk started from.
void UnlinkListNode(EngineContext* ctx, uint32_t node, uint32_t victim) {
  uint32_t current = 0u;  // the node the walk is looking at
  uint32_t next = 0u;     // the successor link hanging off it
  uint32_t link = 0u;     // a back link of that successor
  uint32_t offset = 0u;   // 0 or 4: which link of a node to write
  uint32_t tmp = 0u;
  uint32_t tmp2 = 0u;
  EnterFrame();
  current = (node == victim);
  StoreU8(ctx, victim + 12u, current);
  if (current == 0u) {
    for (;;) {
      current = LoadU32(ctx, victim + 8u);
      if (LoadU8(ctx, current + 12u) != 0u) {
        break;
      }
      next = LoadU32(ctx, current + 8u);
      link = LoadU32(ctx, next);
      if (link == current) {
        link = LoadU32(ctx, next + 4u);
        if (link != 0u) {
          if (LoadU8(ctx, link + 12u) == 0u) {
            goto short_link;
          }
        }
        // Forward splice: the survivor only links across the victim.
        if (victim == LoadU32(ctx, current)) {
          victim = current;
        } else {
          victim = LoadU32(ctx, current + 4u);
          node = LoadU32(ctx, victim);
          StoreU32(ctx, current + 4u, node);
          if (node != 0u) {
            StoreU32(ctx, node + 8u, current);
            tmp = LoadU32(ctx, current + 8u);
          } else {
            tmp = next;
          }
          StoreU32(ctx, victim + 8u, tmp);
          node = LoadU32(ctx, current + 8u);
          tmp = LoadU32(ctx, node);
          offset = (current != tmp) << (2u & 31u);
          StoreU32(ctx, node + offset, victim);
          StoreU32(ctx, victim, current);
          StoreU32(ctx, current + 8u, victim);
          next = LoadU32(ctx, victim + 8u);
        }
        StoreU8(ctx, victim + 12u, 1u);
        StoreU8(ctx, next + 12u, 0u);
        node = LoadU32(ctx, next);
        victim = LoadU32(ctx, node + 4u);
        StoreU32(ctx, next, victim);
        if (victim != 0u) {
          StoreU32(ctx, victim + 8u, next);
        }
        tmp = LoadU32(ctx, next + 8u);
        StoreU32(ctx, node + 8u, tmp);
        victim = LoadU32(ctx, next + 8u);
        tmp = LoadU32(ctx, victim);
        offset = (tmp != next) << (2u & 31u);
        StoreU32(ctx, victim + offset, node);
        StoreU32(ctx, node + 4u, next);
        StoreU32(ctx, next + 8u, node);
        break;
      }
      if (link != 0u) {
        if (LoadU8(ctx, link + 12u) == 0u) {
          goto short_link;
        }
      }
      // Backward splice: the survivor keeps the victim's own back link.
      if (victim != LoadU32(ctx, current)) {
        victim = current;
      } else {
        node = LoadU32(ctx, victim + 4u);
        StoreU32(ctx, current, node);
        if (node != 0u) {
          StoreU32(ctx, node + 8u, current);
          tmp = LoadU32(ctx, current + 8u);
        } else {
          tmp = next;
        }
        StoreU32(ctx, victim + 8u, tmp);
        node = LoadU32(ctx, current + 8u);
        tmp = LoadU32(ctx, node);
        offset = (current != tmp) << (2u & 31u);
        StoreU32(ctx, node + offset, victim);
        StoreU32(ctx, victim + 4u, current);
        StoreU32(ctx, current + 8u, victim);
        next = LoadU32(ctx, victim + 8u);
      }
      StoreU8(ctx, victim + 12u, 1u);
      StoreU8(ctx, next + 12u, 0u);
      node = LoadU32(ctx, next + 4u);
      victim = LoadU32(ctx, node);
      StoreU32(ctx, next + 4u, victim);
      if (victim != 0u) {
        StoreU32(ctx, victim + 8u, next);
      }
      tmp = LoadU32(ctx, next + 8u);
      StoreU32(ctx, node + 8u, tmp);
      victim = LoadU32(ctx, next + 8u);
      tmp = LoadU32(ctx, victim);
      offset = (tmp != next) << (2u & 31u);
      StoreU32(ctx, victim + offset, node);
      StoreU32(ctx, node, next);
      StoreU32(ctx, next + 8u, node);
      break;
    short_link:;
      // The survivor is already adjacent: mark the three nodes and carry on
      // from it until the node the walk started from turns up again.
      StoreU8(ctx, current + 12u, 1u);
      tmp2 = (node == next);
      StoreU8(ctx, next + 12u, tmp2);
      StoreU8(ctx, link + 12u, 1u);
      victim = next;
      if (next != node) {
        continue;
      }
      break;
    }
  }
  LeaveFrame();
}
}  // namespace artc
