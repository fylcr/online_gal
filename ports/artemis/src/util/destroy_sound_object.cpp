#include "util/engine_context.h"

namespace artc {

void ReleaseMapFieldSubtree(EngineContext* ctx, uint32_t a);
void Panic(EngineContext* ctx, uint32_t a0);
void ReleaseSharedRef(EngineContext* ctx, uint32_t a0);
void ReleaseNodeSubtreeA(EngineContext* ctx, uint32_t a, uint32_t b);
void ReleaseNodeSubtreeB(EngineContext* ctx, uint32_t a, uint32_t b);

namespace {
// The class word written into the object's header.
constexpr uint32_t kDestroySoundObjectClassWord = 0x90E80u;
// The word a pending result is published in.
constexpr uint32_t kDestroySoundObjectStatusAddr = 0xD015Cu;
constexpr uint32_t kDestroySoundObjectSaveFrameBytes = 20u;
constexpr uint32_t kDestroySoundObjectResumeWordBytes = 4u;
}  // namespace

// Tears a sound object down: it walks the object's node chain releasing each
// slot through the host, releases the owned maps and the shared pointer, and
// finishes each step of the teardown as the driver's await code names it.
uint32_t DestroySoundObject(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t top = 0u;
  uint32_t arg = 0u;
  uint32_t third = 0u;
  uint32_t fourth = 0u;
  uint32_t fifth = 0u;
  // The frame base, a walk pointer, and the word the spill stages.
  uint32_t hold = 0u;
  // A walk pointer, a flag, and a shared pointer.
  uint32_t cur = 0u;
  // The current node, the published status, and a shared pointer.
  uint32_t peer = 0u;
  // The address of a node's twenty-eighth byte.
  uint32_t slot = 0u;
  // The end of the node chain.
  uint32_t limit = 0u;
  // The await code a suspend was resumed with.
  uint32_t await_code = 0u;

  if (ResumePhase(ctx) == 2u) {
    top = ReturnFrameSp(ctx);
    arg = ReturnFrameSp(ctx);
    arg = LoadU32(ctx, arg);
    arg = arg - kDestroySoundObjectSaveFrameBytes;
    StoreU32(ctx, top, arg);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top);
    hold = top;
    a = LoadU32(ctx, hold);
    cur = LoadU32(ctx, hold + 4u);
    slot = LoadU32(ctx, hold + 12u);
    limit = LoadU32(ctx, hold + 16u);
    peer = LoadU32(ctx, hold + 8u);
  }
  if (ResumePhase(ctx) == 2u) {
    top = ReturnFrameSp(ctx);
    arg = ReturnFrameSp(ctx);
    arg = LoadU32(ctx, arg);
    arg = arg - kDestroySoundObjectResumeWordBytes;
    StoreU32(ctx, top, arg);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top);
    await_code = LoadU32(ctx, top);
  }
  top = ResumePhase(ctx) == kResumeRunning;
  arg = 0u;
  if (ResumePhase(ctx) == kResumeRunning) {
    third = a;
    fourth = kDestroySoundObjectClassWord;
    StoreU32(ctx, third, fourth);
    third = a;
    fourth = 28u;
    third = third + fourth;
    slot = third;
    third = a;
    peer = LoadU32(ctx, third + 28u);
    fourth = a;
    fifth = 32u;
    fourth = fourth + fifth;
    limit = fourth;
    cur = peer == fourth;
  }
  top = cur ? top : arg;
  if (top != 0u) {
    goto release_maps;
  }
  // Hand every slot of the chain back to the host, one node at a time.
  while (true) {
    if (ResumePhase(ctx) == kResumeRunning) {
      cur = peer;
      peer = LoadU32(ctx, peer + 20u);
      StoreU32(ctx, kDestroySoundObjectStatusAddr, 0u);
    }
    if (await_code == 0u || ResumePhase(ctx) == kResumeRunning) {
      HostReleaseSlotPair(ctx, 1592u, peer);
      top = 0u;
      arg = ResumePhase(ctx);
      arg = arg == kResumeSuspended;
      if (arg != 0u) {
        goto save_frame;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      peer = LoadU32(ctx, kDestroySoundObjectStatusAddr);
      StoreU32(ctx, kDestroySoundObjectStatusAddr, 0u);
      peer = peer == 1u;
      if (peer != 0u) {
        goto walk_aborted;
      }
      hold = LoadU32(ctx, cur + 4u);
      if (hold == 0u) {
        peer = LoadU32(ctx, cur + 8u);
        if (LoadU32(ctx, peer) != cur) {
          cur = cur + 8u;
          do {
            hold = LoadU32(ctx, cur);
            cur = hold + 8u;
            peer = LoadU32(ctx, hold + 8u);
          } while (LoadU32(ctx, peer) != hold);
        }
      } else {
        do {
          peer = hold;
          hold = LoadU32(ctx, hold);
        } while (hold != 0u);
      }
      cur = peer != limit;
      if (cur != 0u) {
        continue;
      }
      goto release_maps;
    }
    break;
  }
walk_aborted:;
  // A pending result interrupted the walk, so unwind what was released.
  if (ResumePhase(ctx) == kResumeRunning) {
    top = 0u;
    top = FindMatchingCatch(ctx, top);
    peer = top;
    top = HostTempRet0(ctx);
    top = slot;
    ReleaseMapFieldSubtree(ctx, top);
    cur = a;
    arg = 16u;
    cur = cur + arg;
    hold = LoadU32(ctx, cur + 4u);
    third = cur;
    fourth = hold;
    ReleaseNodeSubtreeA(ctx, third, fourth);
    top = a;
    arg = 8u;
    top = top + arg;
    a = top;
  }
  if (await_code == 1u || ResumePhase(ctx) == kResumeRunning) {
    top = a;
    ReleaseSharedRef(ctx, top);
    top = 1u;
    arg = ResumePhase(ctx);
    arg = arg == kResumeSuspended;
    if (arg != 0u) {
      goto save_frame;
    }
  }
  if (await_code == 2u || ResumePhase(ctx) == kResumeRunning) {
    top = peer;
    Panic(ctx, top);
    top = 2u;
    arg = ResumePhase(ctx);
    arg = arg == kResumeSuspended;
    if (arg != 0u) {
      goto save_frame;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
release_maps:;
  // Release the two owned maps, then the shared pointer of each slot.
  if (ResumePhase(ctx) == kResumeRunning) {
    top = slot;
    arg = LoadU32(ctx, a + 32u);
    ReleaseNodeSubtreeB(ctx, top, arg);
    top = a;
    arg = 16u;
    top = top + arg;
    arg = LoadU32(ctx, a + 20u);
    ReleaseNodeSubtreeA(ctx, top, arg);
    peer = LoadU32(ctx, a + 12u);
    cur = peer == 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    top = cur;
    if (top != 0u) {
      goto finish;
    }
    top = peer;
    arg = LoadU32(ctx, peer + 4u);
    cur = arg;
    arg = 1u;
    cur = cur - arg;
    StoreU32(ctx, top + 4u, cur);
    top = cur;
    arg = 1u;
    top = top != arg;
    if (top != 0u) {
      goto finish;
    }
    top = peer;
    top = LoadU32(ctx, top);
    cur = LoadU32(ctx, top + 8u);
    StoreU32(ctx, kDestroySoundObjectStatusAddr, 0u);
  }
  if (await_code == 3u || ResumePhase(ctx) == kResumeRunning) {
    top = cur;
    arg = peer;
    HostReleaseSlotPair(ctx, top, arg);
    top = 3u;
    arg = ResumePhase(ctx);
    arg = arg == kResumeSuspended;
    if (arg != 0u) {
      goto save_frame;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    cur = LoadU32(ctx, kDestroySoundObjectStatusAddr);
    StoreU32(ctx, kDestroySoundObjectStatusAddr, 0u);
    if (cur == 1u) {
      goto panic_path;
    }
    top = peer;
    arg = LoadU32(ctx, peer + 8u);
    cur = arg;
    arg = 1u;
    cur = cur - arg;
    StoreU32(ctx, top + 8u, cur);
    top = cur;
    arg = 1u;
    top = top != arg;
    if (top != 0u) {
      goto finish;
    }
    top = peer;
    top = LoadU32(ctx, top);
    cur = LoadU32(ctx, top + 12u);
    StoreU32(ctx, kDestroySoundObjectStatusAddr, 0u);
  }
  if (await_code == 4u || ResumePhase(ctx) == kResumeRunning) {
    top = cur;
    arg = peer;
    HostReleaseSlotPair(ctx, top, arg);
    top = 4u;
    arg = ResumePhase(ctx);
    arg = arg == kResumeSuspended;
    if (arg != 0u) {
      goto save_frame;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    peer = LoadU32(ctx, kDestroySoundObjectStatusAddr);
    StoreU32(ctx, kDestroySoundObjectStatusAddr, 0u);
    if (peer == 1u) {
      goto panic_path;
    }
  }
finish:;
  if (ResumePhase(ctx) == kResumeRunning) {
    top = a;
    LeaveFrame();
    return top;
  }
panic_path:;
  // A pending result was raised, so panic and trap.
  if (ResumePhase(ctx) == kResumeRunning) {
    top = 0u;
    top = FindMatchingCatch(ctx, top);
    a = top;
    top = HostTempRet0(ctx);
  }
  if (await_code == 5u || ResumePhase(ctx) == kResumeRunning) {
    top = a;
    Panic(ctx, top);
    top = 5u;
    arg = ResumePhase(ctx);
    arg = arg == kResumeSuspended;
    if (arg != 0u) {
      goto save_frame;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  TrapUnreachable();
save_frame:;
  // Push the word naming the call in flight, then the save frame; a rewind
  // reads them back in that order.
  hold = top;
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top);
  StoreU32(ctx, top, hold);
  top = ReturnFrameSp(ctx);
  arg = ReturnFrameSp(ctx);
  arg = LoadU32(ctx, arg);
  arg = arg + kDestroySoundObjectResumeWordBytes;
  StoreU32(ctx, top, arg);
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top);
  hold = top;
  StoreU32(ctx, hold, a);
  StoreU32(ctx, hold + 4u, cur);
  StoreU32(ctx, hold + 8u, peer);
  StoreU32(ctx, hold + 12u, slot);
  StoreU32(ctx, hold + 16u, limit);
  top = ReturnFrameSp(ctx);
  arg = ReturnFrameSp(ctx);
  arg = LoadU32(ctx, arg);
  arg = arg + 20u;
  StoreU32(ctx, top, arg);
  top = 0u;
  LeaveFrame();
  return top;
}

}  // namespace artc
