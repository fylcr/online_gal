#include "util/engine_context.h"

namespace artc {

// Forward declarations of callees defined elsewhere in the tree.
void Panic(EngineContext* ctx, uint32_t a0);

namespace {

constexpr uint32_t kUnorderedReleaseBucketTableMsgBuckets = 0x90EFCu;  // buckets_
constexpr uint32_t kUnorderedReleaseBucketTableMsgBoostDetail = 0x90F05u;
constexpr uint32_t kUnorderedReleaseBucketTableMsgGetBucket = 0x90F48u;
constexpr uint32_t kUnorderedReleaseBucketTableMsgPx = 0x9107Au;
constexpr uint32_t kUnorderedReleaseBucketTableMsgBoostSharedPtr = 0x91082u;
constexpr uint32_t kUnorderedReleaseBucketTableMsgOperator = 0x910BAu;
constexpr uint32_t kUnorderedReleaseBucketTableLastException = 0xD015Cu;
constexpr uint32_t kUnorderedReleaseBucketTableSaveFrameBytes = 12u;
constexpr uint32_t kUnorderedReleaseBucketTableResumeWordBytes = 4u;

}  // namespace

// Releases every bucket chain of an unordered table and gives the bucket storage
// back to the pool: the chains hanging off the table are drained one by one and
// each one is released through the table's release dispatch.
void UnorderedReleaseBucketTable(EngineContext* ctx, uint32_t a) {
  uint32_t chain = 0u;        // bucket chain being drained
  uint32_t entry = 0u;        // chain entry handed to the release dispatch
  uint32_t frame = 0u;        // frame the suspended registers were saved in
  uint32_t resume_code = 0u;  // code of the suspension being resumed
  uint32_t suspend_code = 0u; // code stored when this call suspends
  uint32_t tmp = 0u;
  uint32_t tmp2 = 0u;
  EnterFrame();
  if (ResumePhase(ctx) == kResumeRewinding) {
    tmp = ReturnFrameSp(ctx);
    tmp2 = ReturnFrameSp(ctx);
    tmp2 = LoadU32(ctx, tmp2);
    tmp2 -= kUnorderedReleaseBucketTableSaveFrameBytes;
    StoreU32(ctx, tmp, tmp2);
    tmp = ReturnFrameSp(ctx);
    tmp = LoadU32(ctx, tmp);
    frame = tmp;
    a = LoadU32(ctx, frame);
    tmp = LoadU32(ctx, frame + 4u);
    chain = tmp;
    tmp = LoadU32(ctx, frame + 8u);
    entry = tmp;
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    tmp = ReturnFrameSp(ctx);
    tmp2 = ReturnFrameSp(ctx);
    tmp2 = LoadU32(ctx, tmp2);
    tmp2 -= kUnorderedReleaseBucketTableResumeWordBytes;
    StoreU32(ctx, tmp, tmp2);
    tmp = ReturnFrameSp(ctx);
    tmp = LoadU32(ctx, tmp);
    resume_code = LoadU32(ctx, tmp);
  }
  if (ResumePhase(ctx) == 0u) {
    tmp = LoadU32(ctx, a + 28u);
    chain = (tmp == 0u);
  }
  if (ResumePhase(ctx) == 0u) {
    if (chain != 0u) {
      goto release_done;
    }
    chain = LoadU32(ctx, a + 40u);
    entry = (chain == 0u);
  }
  if ((entry | (ResumePhase(ctx) == kResumeRewinding)) != 0u) {
    if (ResumePhase(ctx) == 0u) {
      StoreU32(ctx, kUnorderedReleaseBucketTableLastException, 0u);
    }
    tmp = (resume_code == 0u);
    tmp = (ResumePhase(ctx) != 0u) ? tmp : 1u;
    if (tmp != 0u) {
      HostFailAssertion(ctx, 857u, kUnorderedReleaseBucketTableMsgBuckets,
                        kUnorderedReleaseBucketTableMsgBoostDetail, 2956u,
                        kUnorderedReleaseBucketTableMsgGetBucket);
      suspend_code = 0u;
      tmp = ResumePhase(ctx);
      if (tmp == kResumeSuspended) {
        goto save_suspend_frame;
      }
    }
    if (ResumePhase(ctx) == 0u) {
      a = LoadU32(ctx, kUnorderedReleaseBucketTableLastException);
      StoreU32(ctx, kUnorderedReleaseBucketTableLastException, 0u);
      a = (a != 1u);
      if (a != 0u) {
        goto unhandled_exception;
      }
      goto exit_path;
    }
  }
  if (ResumePhase(ctx) == 0u) {
    tmp = LoadU32(ctx, a + 24u);
    tmp = tmp << (2u & 31u);
    tmp = chain + tmp;
    a = LoadU32(ctx, tmp);
    if (a == 0u) {
      goto release_done;
    }
  }
  for (;;) {
    if (ResumePhase(ctx) == 0u) {
      chain = LoadU32(ctx, a + 20u);
      entry = (chain == 0u);
    }
    if ((entry | (ResumePhase(ctx) == kResumeRewinding)) != 0u) {
      if (ResumePhase(ctx) == 0u) {
        StoreU32(ctx, kUnorderedReleaseBucketTableLastException, 0u);
      }
      tmp = (resume_code == 1u);
      tmp = (ResumePhase(ctx) != 0u) ? tmp : 1u;
      if (tmp != 0u) {
        HostFailAssertion(ctx, 857u, kUnorderedReleaseBucketTableMsgPx,
                          kUnorderedReleaseBucketTableMsgBoostSharedPtr, 734u,
                          kUnorderedReleaseBucketTableMsgOperator);
        suspend_code = 1u;
        tmp = ResumePhase(ctx);
        if (tmp == kResumeSuspended) {
          goto save_suspend_frame;
        }
      }
      if (ResumePhase(ctx) == 0u) {
        a = LoadU32(ctx, kUnorderedReleaseBucketTableLastException);
        StoreU32(ctx, kUnorderedReleaseBucketTableLastException, 0u);
        a = (a != 1u);
        if (a != 0u) {
          goto unhandled_exception;
        }
        goto exit_path;
      }
    }
    if (ResumePhase(ctx) == 0u) {
      tmp = LoadU32(ctx, chain);
      entry = LoadU32(ctx, tmp + 16u);
    }
    tmp = (resume_code == kResumeRewinding);
    tmp = (ResumePhase(ctx) != 0u) ? tmp : 1u;
    if (tmp != 0u) {
      CallTableT1(ctx, entry, chain);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      suspend_code = 2u;
      tmp = ResumePhase(ctx);
      if (tmp == kResumeSuspended) {
        goto save_suspend_frame;
      }
    }
    if (ResumePhase(ctx) == 0u) {
      a = LoadU32(ctx, a);
      if (a != 0u) {
        continue;
      }
    }
    break;
  }
release_done:;
  if (ResumePhase(ctx) == 0u) {
    goto frame_stored;
  }
unhandled_exception:;
  if (ResumePhase(ctx) == 0u) {
    TrapUnreachable();
  }
exit_path:;
  if (ResumePhase(ctx) == 0u) {
    a = FindMatchingCatch(ctx, 0u);
    HostTempRet0(ctx);
  }
  tmp = (resume_code == 3u);
  tmp = (ResumePhase(ctx) != 0u) ? tmp : 1u;
  if (tmp != 0u) {
    Panic(ctx, a);
    suspend_code = 3u;
    tmp = ResumePhase(ctx);
    if (tmp == kResumeSuspended) {
      goto save_suspend_frame;
    }
  }
  if (ResumePhase(ctx) == 0u) {
    TrapUnreachable();
  }
  goto frame_stored;
save_suspend_frame:;
  resume_code = suspend_code;
  tmp = ReturnFrameSp(ctx);
  tmp = LoadU32(ctx, tmp);
  StoreU32(ctx, tmp, resume_code);
  tmp = ReturnFrameSp(ctx);
  tmp2 = ReturnFrameSp(ctx);
  tmp2 = LoadU32(ctx, tmp2);
  tmp2 += kUnorderedReleaseBucketTableResumeWordBytes;
  StoreU32(ctx, tmp, tmp2);
  tmp = ReturnFrameSp(ctx);
  tmp = LoadU32(ctx, tmp);
  frame = tmp;
  StoreU32(ctx, frame, a);
  StoreU32(ctx, frame + 4u, chain);
  StoreU32(ctx, frame + 8u, entry);
  tmp = ReturnFrameSp(ctx);
  tmp2 = ReturnFrameSp(ctx);
  tmp2 = LoadU32(ctx, tmp2);
  tmp2 += kUnorderedReleaseBucketTableSaveFrameBytes;
  StoreU32(ctx, tmp, tmp2);
frame_stored:;
  LeaveFrame();
}
}  // namespace artc
