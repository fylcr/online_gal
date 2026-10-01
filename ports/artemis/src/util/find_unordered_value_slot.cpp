#include "util/engine_context.h"

namespace artc {

// Forward declaration -- the shared_ptr precondition failure below.
void Panic(EngineContext* ctx, uint32_t a0);

// Looks `key` up in the unordered map `entry` and answers the value slot it
// resolves to: the key hashes into the bucket table, the bucket's chain is
// walked past the nodes whose group word is negative, and the node that matches
// goes through one table dispatch. A miss trips the shared_ptr precondition and
// panics. Suspendable -- each call below spills and is replayed on rewind.
// `entry` enters as the map and walks on to the bucket table, the chain node and
// the value slot; `key` is the lookup key, then the miss flag, then the
// dispatch slot.
uint32_t FindUnorderedValueSlot(EngineContext* ctx, uint32_t entry, uint32_t key) {
  EnterFrame();
  uint32_t const kMsgBuckets = 0xB5B6Cu;          // "buckets_"
  uint32_t const kMsgUnorderedDetail = 0xB5B75u;  // boost/unordered/detail/implementation.hpp
  uint32_t const kMsgGetBucket = 0xB5BB8u;        // "get_bucket"
  uint32_t const kMsgPx = 0xB5E31u;               // "px != 0"
  uint32_t const kMsgSharedPtr = 0xB5E39u;        // boost/smart_ptr/shared_ptr.hpp
  uint32_t const kMsgOperator = 0xB5E71u;         // "operator->"
  uint32_t const kPendingThrowCell = 0xD015Cu;    // the cell the throw handoff leaves set
  uint32_t const kGroupWordMask = 0x7FFFFFFFu;    // clears the chain's single-group flag
  uint32_t const kSaveFrameBytes = 12u;
  uint32_t const kResumeWordBytes = 4u;
  uint32_t frame = 0u;
  uint32_t result = 0u;
  uint32_t resume_word = 0u;
  uint32_t call_code = 0u;
  uint32_t bucket = 0u;
  uint32_t group = 0u;
  uint32_t no_table = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 12-byte frame -- the map, the key, the result.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    entry = LoadU32(ctx, frame);
    key = LoadU32(ctx, frame + 4u);
    result = LoadU32(ctx, frame + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the second pop names the call that was in flight.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    bucket = key % LoadU32(ctx, entry + 24u);
    no_table = static_cast<uint32_t>(LoadU32(ctx, entry + 28u) == 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (no_table) { LeaveFrame(); return 0u; }
    // The bucket table, the bucket the hash landed in, then that bucket's head.
    entry = LoadU32(ctx, entry + 40u);
    if (entry == 0u) {
      // No bucket array at all: the index precondition fires and traps.
      HostAssertFail(ctx, kMsgBuckets, kMsgUnorderedDetail, 2956u, kMsgGetBucket);
      TrapUnreachable();
    }
    entry = LoadU32(ctx, entry + (bucket << 2));
    if (entry == 0u) { LeaveFrame(); return 0u; }
    entry = LoadU32(ctx, entry);
    if (entry == 0u) { LeaveFrame(); return 0u; }
    if (LoadU32(ctx, entry + 8u) != key) {
      // Not this node: follow the chain while its group word stays negative and
      // stop at the first group that matches the bucket index.
      group = LoadU32(ctx, entry + 4u);
      for (;;) {
        if (bucket != (group & kGroupWordMask)) { LeaveFrame(); return 0u; }
        do {
          entry = LoadU32(ctx, entry);
          if (entry == 0u) { LeaveFrame(); return 0u; }
          group = LoadU32(ctx, entry + 4u);
        } while (static_cast<int32_t>(group) < 0);
        if (LoadU32(ctx, entry + 8u) != key) continue;
        break;
      }
    }
    entry = LoadU32(ctx, entry + 20u);
    key = static_cast<uint32_t>(entry == 0u);
  }
  if (key != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kPendingThrowCell, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_word == 0u) {
      HostFailAssertion(ctx, 857u, kMsgPx, kMsgSharedPtr, 734u, kMsgOperator);
      if (ResumePhase(ctx) == kResumeSuspended) {
        call_code = 0u;
        goto suspend_frame;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The host parks the caught type in the pending cell; only a live throw
      // (type one) finds a matching catch and picks up its temporary slot.
      entry = LoadU32(ctx, kPendingThrowCell);
      StoreU32(ctx, kPendingThrowCell, 0u);
      if (entry != 1u) TrapUnreachable();
      entry = FindMatchingCatch(ctx, 0u);
      key = HostTempRet0(ctx);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_word == 1u) {
      Panic(ctx, entry);
      if (ResumePhase(ctx) == kResumeSuspended) {
        call_code = 1u;
        goto suspend_frame;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) TrapUnreachable();
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    key = LoadU32(ctx, LoadU32(ctx, entry) + 28u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_word == 2u) {
    const uint32_t dispatched = CallTableT2(ctx, key, entry);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    if (ResumePhase(ctx) == kResumeSuspended) {
      call_code = 2u;
      goto suspend_frame;
    }
    result = dispatched;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    LeaveFrame();
    return result;
  }
  TrapUnreachable();
suspend_frame:;
  // Suspended: park the word naming the call above the frame, step past it, then
  // write the 12-byte frame a rewind pass reads back.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), call_code);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, entry);
  StoreU32(ctx, frame + 4u, key);
  StoreU32(ctx, frame + 8u, result);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
