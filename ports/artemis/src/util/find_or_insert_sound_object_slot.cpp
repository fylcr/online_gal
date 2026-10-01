#include "util/engine_context.h"

namespace artc {

void InsertSoundObjectSlot(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
void Panic(EngineContext* ctx, uint32_t a0);

namespace {
// The buckets_ member named by an assert.
constexpr uint32_t kFindOrInsertSoundObjectSlotBucketsText = 0xB5B6Cu;
// The boost/unordered header path named by an assert.
constexpr uint32_t kFindOrInsertSoundObjectSlotHeaderPathText = 0xB5B75u;
// The get_bucket member named by an assert.
constexpr uint32_t kFindOrInsertSoundObjectSlotGetBucketText = 0xB5BB8u;
// The pointer test named by an assert.
constexpr uint32_t kFindOrInsertSoundObjectSlotPxText = 0xB5E31u;
// The boost/smart_ptr header path named by an assert.
constexpr uint32_t kFindOrInsertSoundObjectSlotSmartPtrText = 0xB5E39u;
// The operator-> member named by an assert.
constexpr uint32_t kFindOrInsertSoundObjectSlotOperatorText = 0xB5E71u;
// The word a pending result is published in.
constexpr uint32_t kFindOrInsertSoundObjectSlotStatusAddr = 0xD015Cu;
// The low bits a node's stored index keeps.
constexpr uint32_t kFindOrInsertSoundObjectSlotIndexMask = 0x7FFFFFFFu;
constexpr uint32_t kFindOrInsertSoundObjectSlotSaveFrameBytes = 16u;
constexpr uint32_t kFindOrInsertSoundObjectSlotResumeWordBytes = 4u;
}  // namespace

// Hashes the key into the sound table, inserts the value on a miss, and then
// asserts the shared pointer reachable through the node it landed on.
void FindOrInsertSoundObjectSlot(EngineContext* ctx, uint32_t a, uint32_t b) {
  EnterFrame();
  uint32_t top = 0u;
  uint32_t arg = 0u;
  uint32_t third = 0u;
  uint32_t fourth = 0u;
  uint32_t fifth = 0u;
  // The frame base, and later the scratch slot on the guest stack.
  uint32_t slot = 0u;
  // The bucket's chain head, a node pointer, and a flag.
  uint32_t peer = 0u;
  // The bucket index of the key.
  uint32_t index = 0u;
  // The index the current node has stored.
  uint32_t stored = 0u;
  // The await code a suspend was resumed with.
  uint32_t await_code = 0u;

  if (ResumePhase(ctx) == 2u) {
    top = ReturnFrameSp(ctx);
    arg = ReturnFrameSp(ctx);
    arg = LoadU32(ctx, arg);
    arg = arg - kFindOrInsertSoundObjectSlotSaveFrameBytes;
    StoreU32(ctx, top, arg);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top);
    slot = top;
    a = LoadU32(ctx, slot);
    b = LoadU32(ctx, slot + 4u);
    peer = LoadU32(ctx, slot + 8u);
    slot = LoadU32(ctx, slot + 12u);
  }
  if (ResumePhase(ctx) == 2u) {
    top = ReturnFrameSp(ctx);
    arg = ReturnFrameSp(ctx);
    arg = LoadU32(ctx, arg);
    arg = arg - kFindOrInsertSoundObjectSlotResumeWordBytes;
    StoreU32(ctx, top, arg);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top);
    await_code = LoadU32(ctx, top);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Take a scratch slot from the guest stack and hash the key there.
    top = GuestStackSp(ctx);
    arg = 16u;
    top = top - arg;
    slot = top;
    GuestStackSp(ctx) = top;
    top = slot;
    arg = b;
    StoreU32(ctx, top + 4u, arg);
    top = b;
    arg = LoadU32(ctx, a + 24u);
    index = REM_U(top, arg);
    top = LoadU32(ctx, a + 28u);
    peer = top == 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (peer != 0u) {
      goto lookup_done;
    }
    peer = LoadU32(ctx, a + 40u);
    if (peer == 0u) {
      goto bucket_missing;
    }
    // Walk the bucket's chain for a node holding the same key.
    top = index;
    arg = 2u;
    top = top << (arg & 31);
    arg = peer;
    top = top + arg;
    peer = LoadU32(ctx, top);
    if (peer == 0u) {
      goto lookup_done;
    }
    peer = LoadU32(ctx, peer);
    if (peer == 0u) {
      goto lookup_done;
    }
    top = LoadU32(ctx, peer + 8u);
    arg = b;
    top = top != arg;
    if (top != 0u) {
      stored = LoadU32(ctx, peer + 4u);
      while (true) {
        top = index;
        arg = stored;
        third = kFindOrInsertSoundObjectSlotIndexMask;
        arg = arg & third;
        top = top != arg;
        if (top != 0u) {
          goto lookup_done;
        }
        // Skip the nodes whose stored index has the counted bit set.
        while (true) {
          peer = LoadU32(ctx, peer);
          if (peer == 0u) {
            goto lookup_done;
          }
          stored = LoadU32(ctx, peer + 4u);
          if ((int32_t)stored < 0) {
            continue;
          }
          break;
        }
        top = LoadU32(ctx, peer + 8u);
        arg = b;
        top = top != arg;
        if (top != 0u) {
          continue;
        }
        break;
      }
    }
    // Hand the slot and the table's bucket array to the insert step.
    b = slot;
    arg = 8u;
    b = b + arg;
    peer = slot;
    arg = 4u;
    peer = peer + arg;
    top = a;
    arg = 20u;
    top = top + arg;
    a = top;
  }
  if (await_code == 0u || ResumePhase(ctx) == kResumeRunning) {
    top = b;
    arg = a;
    third = peer;
    InsertSoundObjectSlot(ctx, top, arg, third);
    top = 0u;
    arg = ResumePhase(ctx);
    arg = arg == kResumeSuspended;
    if (arg != 0u) {
      goto save_frame;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    top = LoadU32(ctx, slot + 8u);
    a = LoadU32(ctx, top + 20u);
    b = a == 0u;
  }
  top = b;
  arg = ResumePhase(ctx);
  arg = arg == kResumeRewinding;
  top = top | arg;
  if (top != 0u) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kFindOrInsertSoundObjectSlotStatusAddr, 0u);
    }
    if (await_code == 1u || ResumePhase(ctx) == kResumeRunning) {
      top = 857u;
      arg = kFindOrInsertSoundObjectSlotPxText;
      third = kFindOrInsertSoundObjectSlotSmartPtrText;
      fourth = 734u;
      fifth = kFindOrInsertSoundObjectSlotOperatorText;
      HostFailAssertion(ctx, top, arg, third, fourth, fifth);
      top = 1u;
      arg = ResumePhase(ctx);
      arg = arg == kResumeSuspended;
      if (arg != 0u) {
        goto save_frame;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      a = LoadU32(ctx, kFindOrInsertSoundObjectSlotStatusAddr);
      StoreU32(ctx, kFindOrInsertSoundObjectSlotStatusAddr, 0u);
      if (a != 1u) {
        goto resume_path;
      }
      top = 0u;
      top = FindMatchingCatch(ctx, top);
      a = top;
      top = HostTempRet0(ctx);
      b = top;
    }
    if (await_code == 2u || ResumePhase(ctx) == kResumeRunning) {
      top = a;
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
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    top = LoadU32(ctx, a);
    b = LoadU32(ctx, top + 60u);
  }
  if (await_code == 3u || ResumePhase(ctx) == kResumeRunning) {
    top = a;
    arg = b;
    CallTableT1(ctx, arg, top);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    top = 3u;
    arg = ResumePhase(ctx);
    arg = arg == kResumeSuspended;
    if (arg != 0u) {
      goto save_frame;
    }
  }
lookup_done:;
  // Give the scratch slot back before leaving.
  if (ResumePhase(ctx) == kResumeRunning) {
    top = slot;
    arg = 16u;
    top = top + arg;
    GuestStackSp(ctx) = top;
    LeaveFrame();
    return;
  }
bucket_missing:;
  if (ResumePhase(ctx) == kResumeRunning) {
    HostAssertFail(ctx, kFindOrInsertSoundObjectSlotBucketsText,
                   kFindOrInsertSoundObjectSlotHeaderPathText, 2956u,
                   kFindOrInsertSoundObjectSlotGetBucketText);
  }
resume_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  LeaveFrame();
  return;
save_frame:;
  // Push the word naming the call in flight, then the save frame; a rewind
  // reads them back in that order.
  await_code = top;
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top);
  StoreU32(ctx, top, await_code);
  top = ReturnFrameSp(ctx);
  arg = ReturnFrameSp(ctx);
  arg = LoadU32(ctx, arg);
  arg = arg + kFindOrInsertSoundObjectSlotResumeWordBytes;
  StoreU32(ctx, top, arg);
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top);
  await_code = top;
  StoreU32(ctx, await_code, a);
  StoreU32(ctx, await_code + 4u, b);
  StoreU32(ctx, await_code + 8u, peer);
  StoreU32(ctx, await_code + 12u, slot);
  top = ReturnFrameSp(ctx);
  arg = ReturnFrameSp(ctx);
  arg = LoadU32(ctx, arg);
  arg = arg + 16u;
  StoreU32(ctx, top, arg);
  LeaveFrame();
  return;
}

}  // namespace artc
