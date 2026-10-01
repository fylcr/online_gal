#include "util/engine_context.h"

namespace artc {

namespace {

constexpr uint32_t kResetGlobalSlotTableCountAddr = 0xCF134u;      // table 1: bucket count
constexpr uint32_t kResetGlobalSlotTableSizeAddr = 0xCF138u;       // table 1: size word
constexpr uint32_t kResetGlobalSlotTableBucketsAddr = 0xCF144u;    // table 1: bucket array
constexpr uint32_t kResetGlobalSlotTableCount2Addr = 0xCF150u;     // table 2: bucket count
constexpr uint32_t kResetGlobalSlotTableSize2Addr = 0xCF154u;      // table 2: size word
constexpr uint32_t kResetGlobalSlotTableBuckets2Addr = 0xCF160u;   // table 2: bucket array
constexpr uint32_t kResetGlobalSlotTableSlotMask = 0x7FFFFFFFu;    // bucket index bits
constexpr uint32_t kResetGlobalSlotTableAssertMsg = 0x8FAFCu;      // "buckets_"
constexpr uint32_t kResetGlobalSlotTableAssertFile = 0x8FB05u;     // boost header path
constexpr uint32_t kResetGlobalSlotTableAssertLine = 2956u;        // line in that header
constexpr uint32_t kResetGlobalSlotTableAssertFunc = 0x8FB48u;     // "get_bucket"
constexpr uint32_t kResetGlobalSlotTableSaveFrameBytes = 12u;      // live state pushed on suspend
constexpr uint32_t kResetGlobalSlotTableResumeWordBytes = 4u;      // resume word above that frame

}  // namespace

// Drops the entry named by the slot id `a` from both global slot tables.  For
// each table the bucket is `a % bucket count`, the bucket chain is walked for
// the entry whose key word at +8 equals `a`, and that entry's pending call is
// issued through the call table.  An empty table or a chain that runs out is
// skipped; a null bucket array reaches the get_bucket assertion instead.
// Suspendable: the two calls are gated on resume codes 0 and 1.
//
// Frame, 12 bytes: [0] a, [4] entry, [8] slot.
void ResetGlobalSlotTable(EngineContext* ctx, uint32_t a) {
  EnterFrame();
  uint32_t entry = 0u;       // the chain entry walked; also a table's size/array word
  uint32_t slot = 0u;        // the bucket index of `a`
  uint32_t hash = 0u;        // the entry's stored hash word
  uint32_t resume_point = 0u;

  // Rewinding: pop the 12-byte frame the suspend path saved.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kResetGlobalSlotTableSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    a = LoadU32(ctx, frame);
    entry = LoadU32(ctx, frame + 4u);
    slot = LoadU32(ctx, frame + 8u);
  }

  // Rewinding: pop the resume word pushed below that frame.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx),
        LoadU32(ctx, ReturnFrameSp(ctx)) - kResetGlobalSlotTableResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }

  if (ResumePhase(ctx) == kResumeRunning) {
    slot = REM_U(a, LoadU32(ctx, kResetGlobalSlotTableCountAddr));
    entry = static_cast<uint32_t>(LoadU32(ctx, kResetGlobalSlotTableSizeAddr) == 0u);
  }

  if (ResumePhase(ctx) == kResumeRunning) {
    if (entry != 0u) { goto second_table; }
    entry = LoadU32(ctx, kResetGlobalSlotTableBucketsAddr);
    if (entry == 0u) { goto assert_bucket; }
    entry = LoadU32(ctx, entry + (slot << 2u));
    if (entry == 0u) { goto second_table; }
    entry = LoadU32(ctx, entry);
    if (entry == 0u) { goto second_table; }
    if (LoadU32(ctx, entry + 8u) != a) {
      hash = LoadU32(ctx, entry + 4u);
match_first:;
      if (slot != (hash & kResetGlobalSlotTableSlotMask)) { goto second_table; }
walk_first:;
      entry = LoadU32(ctx, entry);
      if (entry == 0u) { goto second_table; }
      hash = LoadU32(ctx, entry + 4u);
      if (static_cast<int32_t>(hash) < 0) { goto walk_first; }
      if (LoadU32(ctx, entry + 8u) != a) { goto match_first; }
    }
    entry = LoadU32(ctx, entry + 12u);
    if (entry == 0u) { goto second_table; }
    a = LoadU32(ctx, LoadU32(ctx, entry) + 16u);
  }

  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    CallTableT1(ctx, a, entry);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }
  if (ResumePhase(ctx) == kResumeRunning) { goto exit_path; }

second_table:;
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = REM_U(a, LoadU32(ctx, kResetGlobalSlotTableCount2Addr));
    entry = static_cast<uint32_t>(LoadU32(ctx, kResetGlobalSlotTableSize2Addr) == 0u);
  }

  if (ResumePhase(ctx) == kResumeRunning) {
    if (entry != 0u) { goto second_done; }
    entry = LoadU32(ctx, kResetGlobalSlotTableBuckets2Addr);
    if (entry == 0u) { goto assert_bucket; }
    entry = LoadU32(ctx, entry + (slot << 2u));
    if (entry == 0u) { goto second_done; }
    entry = LoadU32(ctx, entry);
    if (entry == 0u) { goto second_done; }
    if (LoadU32(ctx, entry + 8u) != a) {
      hash = LoadU32(ctx, entry + 4u);
match_second:;
      if (slot != (hash & kResetGlobalSlotTableSlotMask)) { goto second_done; }
walk_second:;
      entry = LoadU32(ctx, entry);
      if (entry == 0u) { goto second_done; }
      hash = LoadU32(ctx, entry + 4u);
      if (static_cast<int32_t>(hash) < 0) { goto walk_second; }
      if (LoadU32(ctx, entry + 8u) != a) { goto match_second; }
    }
    a = LoadU32(ctx, entry + 12u);
    if (a == 0u) { goto second_done; }
    entry = LoadU32(ctx, LoadU32(ctx, a) + 16u);
  }

  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    CallTableT1(ctx, entry, a);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) { goto suspend_point; }
  }

second_done:;
  if (ResumePhase(ctx) == kResumeRunning) { goto exit_path; }

assert_bucket:;
  if (ResumePhase(ctx) == kResumeRunning) {
    HostAssertFail(ctx, kResetGlobalSlotTableAssertMsg, kResetGlobalSlotTableAssertFile,
        kResetGlobalSlotTableAssertLine, kResetGlobalSlotTableAssertFunc);
    TrapUnreachable();
  }
  goto exit_path;

suspend_point:;
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kResetGlobalSlotTableResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, a);
    StoreU32(ctx, frame + 4u, entry);
    StoreU32(ctx, frame + 8u, slot);
  }
  StoreU32(ctx, ReturnFrameSp(ctx),
      LoadU32(ctx, ReturnFrameSp(ctx)) + kResetGlobalSlotTableSaveFrameBytes);

exit_path:;
  LeaveFrame();
}
}  // namespace artc
