#include "util/engine_context.h"

namespace artc {

void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
void ReleaseNodeTree(EngineContext* ctx, uint32_t state, uint32_t node);
uint32_t AppendStringText(EngineContext* ctx, uint32_t text, uint32_t source);

namespace {
// The word written into the copy's header.
constexpr uint32_t kCopyScriptBlockHeaderWord = 0xB5F40u;
// The word a pending result is published in.
constexpr uint32_t kCopyScriptBlockStatusAddr = 0xD015Cu;
// The all-bits word a signed size byte is held against.
constexpr uint32_t kCopyScriptBlockAllBits = 0xFFFFFFFFu;
constexpr uint32_t kCopyScriptBlockSaveFrameBytes = 32u;
constexpr uint32_t kCopyScriptBlockResumeWordBytes = 4u;
}  // namespace

// Copy-constructs a script block: it clones the name string, deep-copies the
// tagged map through the host insert slot, copies the boxed member, and
// releases everything it built when a pending result was raised.
uint32_t CopyScriptBlock(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t top = 0u;
  uint32_t arg = 0u;
  // The frame base, a call result, and a cursor walking the copied map.
  uint32_t hold = 0u;
  // A restored word, a walk pointer, and the published status.
  uint32_t cur = 0u;
  // The source word, a flag, and a walk pointer.
  uint32_t peer = 0u;
  // The address of the copy's twentieth byte.
  uint32_t slot = 0u;
  // The cloned name string.
  uint32_t name = 0u;
  // The head word of the copied map.
  uint32_t head = 0u;
  // The end of the source map.
  uint32_t tail = 0u;
  // The await code a suspend was resumed with.
  uint32_t await_code = 0u;
  uint64_t zero = 0;

  if (ResumePhase(ctx) == 2u) {
    top = ReturnFrameSp(ctx);
    arg = ReturnFrameSp(ctx);
    arg = LoadU32(ctx, arg);
    arg = arg - kCopyScriptBlockSaveFrameBytes;
    StoreU32(ctx, top, arg);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top);
    hold = top;
    a = LoadU32(ctx, hold);
    cur = LoadU32(ctx, hold + 8u);
    peer = LoadU32(ctx, hold + 12u);
    slot = LoadU32(ctx, hold + 16u);
    name = LoadU32(ctx, hold + 20u);
    head = LoadU32(ctx, hold + 24u);
    tail = LoadU32(ctx, hold + 28u);
    b = LoadU32(ctx, hold + 4u);
  }
  if (ResumePhase(ctx) == 2u) {
    top = ReturnFrameSp(ctx);
    arg = ReturnFrameSp(ctx);
    arg = LoadU32(ctx, arg);
    arg = arg - kCopyScriptBlockResumeWordBytes;
    StoreU32(ctx, top, arg);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top);
    await_code = LoadU32(ctx, top);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, a, kCopyScriptBlockHeaderWord);
    peer = b + 4u;
    cur = a + 4u;
  }
  if (await_code == 0u || ResumePhase(ctx) == kResumeRunning) {
    hold = AppendStringText(ctx, cur, peer);
    top = 0u;
    arg = ResumePhase(ctx);
    arg = arg == kResumeSuspended;
    if (arg != 0u) {
      goto save_frame;
    }
    name = hold;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = a + 20u;
    zero = 0ull;
    StoreU64(ctx, slot, zero);
    StoreU32(ctx, a + 16u, slot);
    head = a + 16u;
    cur = LoadU32(ctx, b + 16u);
    tail = b + 20u;
    peer = cur != tail;
  }
  // Copy the source map entry by entry until the walk runs off its end.
  if (peer != 0u || ResumePhase(ctx) == kResumeRewinding) {
    while (true) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kCopyScriptBlockStatusAddr, 0u);
        peer = cur;
        cur = cur + 16u;
      }
      if (await_code == 1u || ResumePhase(ctx) == kResumeRunning) {
        hold = HostInvokeIiiii(ctx, 2364u, head, slot, cur, cur);
        top = 1u;
        arg = ResumePhase(ctx);
        arg = arg == kResumeSuspended;
        if (arg != 0u) {
          goto save_frame;
        }
        cur = hold;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        cur = LoadU32(ctx, kCopyScriptBlockStatusAddr);
        StoreU32(ctx, kCopyScriptBlockStatusAddr, 0u);
        if (cur == 1u) {
          goto release_and_trap;
        }
        hold = LoadU32(ctx, peer + 4u);
        if (hold == 0u) {
          cur = LoadU32(ctx, peer + 8u);
          if (LoadU32(ctx, cur) != peer) {
            hold = peer + 8u;
            do {
              peer = LoadU32(ctx, hold);
              hold = peer + 8u;
              cur = LoadU32(ctx, peer + 8u);
            } while (LoadU32(ctx, cur) != peer);
          }
        } else {
          do {
            cur = hold;
            hold = LoadU32(ctx, hold);
          } while (hold != 0u);
        }
        peer = cur != tail;
        if (peer != 0u) {
          continue;
        }
      }
      break;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, kCopyScriptBlockStatusAddr, 0u);
    peer = b + 28u;
    cur = a + 28u;
  }
  if (await_code == 2u || ResumePhase(ctx) == kResumeRunning) {
    HostOsQuery(ctx, 873u, cur, peer);
    top = 2u;
    arg = ResumePhase(ctx);
    arg = arg == kResumeSuspended;
    if (arg != 0u) {
      goto save_frame;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    cur = LoadU32(ctx, kCopyScriptBlockStatusAddr);
    StoreU32(ctx, kCopyScriptBlockStatusAddr, 0u);
    if (cur == 1u) {
      goto release_and_trap;
    }
    top = LoadU32(ctx, b + 40u);
    StoreU32(ctx, a + 40u, top);
    top = a;
    LeaveFrame();
    return top;
  }
release_and_trap:;
  // A pending result was raised, so release the copy and trap.
  if (ResumePhase(ctx) == kResumeRunning) {
    a = HostRet0(ctx);
    top = HostTempRet0(ctx);
    arg = LoadU32(ctx, slot);
    ReleaseNodeTree(ctx, head, arg);
    top = LoadS8(ctx, name + 11u);
    arg = kCopyScriptBlockAllBits;
    top = (uint32_t)((int32_t)top <= (int32_t)arg);
    if (top != 0u) {
      top = LoadU32(ctx, name);
      DeallocateRawBlock(ctx, top);
    }
    top = a;
    HostOsRelease(ctx, top);
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
  arg = arg + kCopyScriptBlockResumeWordBytes;
  StoreU32(ctx, top, arg);
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top);
  hold = top;
  StoreU32(ctx, hold, a);
  StoreU32(ctx, hold + 4u, b);
  StoreU32(ctx, hold + 8u, cur);
  StoreU32(ctx, hold + 12u, peer);
  StoreU32(ctx, hold + 16u, slot);
  StoreU32(ctx, hold + 20u, name);
  StoreU32(ctx, hold + 24u, head);
  StoreU32(ctx, hold + 28u, tail);
  top = ReturnFrameSp(ctx);
  arg = ReturnFrameSp(ctx);
  arg = LoadU32(ctx, arg);
  arg = arg + kCopyScriptBlockSaveFrameBytes;
  StoreU32(ctx, top, arg);
  top = 0u;
  LeaveFrame();
  return top;
}

}  // namespace artc
