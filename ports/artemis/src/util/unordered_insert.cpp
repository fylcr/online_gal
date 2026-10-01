#include "util/engine_context.h"

namespace artc {

// Forward declarations of callees defined elsewhere in the tree.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
uint32_t UnorderedPrepareSlot(EngineContext* ctx, uint32_t map, uint32_t key, uint32_t hash);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
uint32_t UnorderedFindEntry(EngineContext* ctx, uint32_t map, uint32_t hash, uint32_t key, uint32_t out);

namespace {

constexpr uint32_t kUnorderedInsertLastException = 0xD015Cu;
constexpr uint32_t kUnorderedInsertHashSeed = 0x61C88647u;
constexpr uint32_t kUnorderedInsertSaveFrameBytes = 28u;
constexpr uint32_t kUnorderedInsertResumeWordBytes = 4u;

}  // namespace

// Inserts one key/value pair into the unordered map: finds the entry of the
// key, builds the slot for a new key, and falls back to allocating a string
// buffer when the map has none yet.  Suspendable at every host call.
void UnorderedInsert(EngineContext* ctx, uint32_t map, uint32_t key, uint32_t value) {
  uint32_t frame = 0u;         // frame a suspension saved its registers in
  uint32_t entry = 0u;         // found or freshly built hash-table entry
  uint32_t end = 0u;           // hash walk bound, later a host call operand
  uint32_t hash = 0u;          // hash accumulated for the key
  uint32_t changed = 0u;       // the map's bucket count was rebuilt
  uint32_t resume_code = 0u;   // code of the suspension being resumed
  uint32_t need_insert = 0u;   // the key is not in the map yet
  uint32_t buffer = 0u;        // string buffer allocated for a fresh entry
  uint32_t slot = 0u;          // slot returned by the prepare step
  uint32_t suspend_code = 0u;  // code stored when this call suspends
  uint32_t tmp = 0u;
  uint32_t tmp2 = 0u;
  uint32_t tmp3 = 0u;
  uint32_t tmp4 = 0u;
  uint64_t zero64 = 0;
  EnterFrame();
  if (ResumePhase(ctx) == kResumeRewinding) {
    tmp = ReturnFrameSp(ctx);
    tmp2 = ReturnFrameSp(ctx);
    tmp2 = LoadU32(ctx, tmp2);
    tmp2 -= kUnorderedInsertSaveFrameBytes;
    StoreU32(ctx, tmp, tmp2);
    tmp = ReturnFrameSp(ctx);
    tmp = LoadU32(ctx, tmp);
    frame = tmp;
    map = LoadU32(ctx, frame);
    tmp = LoadU32(ctx, frame + 4u);
    key = tmp;
    tmp = LoadU32(ctx, frame + 8u);
    value = tmp;
    tmp = LoadU32(ctx, frame + 12u);
    entry = tmp;
    tmp = LoadU32(ctx, frame + 16u);
    end = tmp;
    tmp = LoadU32(ctx, frame + 20u);
    hash = tmp;
    tmp = LoadU32(ctx, frame + 24u);
    changed = tmp;
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    tmp = ReturnFrameSp(ctx);
    tmp2 = ReturnFrameSp(ctx);
    tmp2 = LoadU32(ctx, tmp2);
    tmp2 -= kUnorderedInsertResumeWordBytes;
    StoreU32(ctx, tmp, tmp2);
    tmp = ReturnFrameSp(ctx);
    tmp = LoadU32(ctx, tmp);
    resume_code = LoadU32(ctx, tmp);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = value;
    tmp = LoadU32(ctx, tmp + 4u);
    tmp2 = value;
    tmp2 = LoadU8(ctx, tmp2 + 11u);
    entry = tmp2;
    tmp3 = entry;
    tmp3 = tmp3 << (24u & 31u);
    tmp4 = 24u;
    tmp3 = (uint32_t)((int32_t)tmp3 >> (tmp4 & 31u));
    tmp4 = 0u;
    tmp3 = (uint32_t)((int32_t)tmp3 < (int32_t)tmp4);
    entry = tmp3;
    tmp = (tmp3 != 0u) ? tmp : tmp2;
    end = tmp;
    if (tmp != 0u) {
      tmp2 = end;
      tmp3 = value;
      tmp3 = LoadU32(ctx, tmp3);
      tmp4 = value;
      tmp2 = (entry != 0u) ? tmp3 : tmp4;
      entry = tmp2;
      tmp += tmp2;
      end = tmp;
      for (;;) {
        tmp = hash;
        tmp2 = LoadS8(ctx, entry);
        tmp3 = hash;
        tmp3 = tmp3 << (6u & 31u);
        tmp4 = hash;
        tmp4 = tmp4 >> (2u & 31u);
        tmp3 += tmp4;
        tmp2 += tmp3;
        tmp4 = kUnorderedInsertHashSeed;
        tmp2 -= tmp4;
        tmp ^= tmp2;
        hash = tmp;
        tmp = end;
        entry += 1u;
        tmp2 = (end != entry);
        if (tmp2 != 0u) {
          continue;
        }
        break;
      }
    }
    tmp4 = key;
    tmp4 = LoadU8(ctx, tmp4);
    tmp4 += key;
    tmp4 += 1u;
    entry = UnorderedFindEntry(ctx, key, hash, value, tmp4);
    tmp = (entry == 0u);
    need_insert = tmp;
  }
  if ((need_insert | (ResumePhase(ctx) == kResumeRewinding)) != 0u) {
    tmp = (resume_code == 0u);
    tmp = (ResumePhase(ctx) != 0u) ? tmp : 1u;
    if (tmp != 0u) {
      buffer = AllocateStringBuffer(ctx, 32u);
      suspend_code = 0u;
      tmp = ResumePhase(ctx);
      if (tmp == kResumeSuspended) {
        goto save_suspend_frame;
      }
      entry = buffer;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      zero64 = 0ull;
      StoreU64(ctx, entry, zero64);
      StoreU64(ctx, entry + 24u, zero64);
      StoreU64(ctx, entry + 16u, zero64);
      tmp = entry + 8u;
      end = tmp;
      StoreU64(ctx, tmp, zero64);
      StoreU32(ctx, kUnorderedInsertLastException, 0u);
    }
    tmp = (resume_code == kResumeSuspended);
    tmp = (ResumePhase(ctx) != 0u) ? tmp : 1u;
    if (tmp != 0u) {
      HostOsQuery(ctx, 873u, end, value);
      suspend_code = 1u;
      tmp = ResumePhase(ctx);
      if (tmp == kResumeSuspended) {
        goto save_suspend_frame;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      value = LoadU32(ctx, kUnorderedInsertLastException);
      StoreU32(ctx, kUnorderedInsertLastException, 0u);
      if (value == kResumeSuspended) {
        goto unhandled_exception;
      }
      zero64 = 0ull;
      StoreU64(ctx, entry + 20u, zero64);
      StoreU32(ctx, entry + 28u, 0u);
      changed = 1u;
    }
    tmp = (resume_code == kResumeRewinding);
    tmp = (ResumePhase(ctx) != 0u) ? tmp : 1u;
    if (tmp != 0u) {
      slot = UnorderedPrepareSlot(ctx, key, entry, hash);
      suspend_code = 2u;
      tmp = ResumePhase(ctx);
      if (tmp == kResumeSuspended) {
        goto save_suspend_frame;
      }
      tmp = slot;
    } else {
      tmp = entry;
    }
    entry = tmp;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, map + 4u, changed);
    StoreU32(ctx, map, entry);
    goto frame_stored;
  }
unhandled_exception:;
  if (ResumePhase(ctx) == kResumeRunning) {
    tmp = HostRet0(ctx);
    map = tmp;
    HostTempRet0(ctx);
    DeallocateRawBlock(ctx, entry);
    HostOsRelease(ctx, map);
    TrapUnreachable();
  }
  goto frame_stored;
save_suspend_frame:;
  tmp = suspend_code;
  tmp2 = ReturnFrameSp(ctx);
  tmp2 = LoadU32(ctx, tmp2);
  StoreU32(ctx, tmp2, tmp);
  tmp = ReturnFrameSp(ctx);
  tmp2 = ReturnFrameSp(ctx);
  tmp2 = LoadU32(ctx, tmp2);
  tmp2 += kUnorderedInsertResumeWordBytes;
  StoreU32(ctx, tmp, tmp2);
  tmp = ReturnFrameSp(ctx);
  tmp = LoadU32(ctx, tmp);
  frame = tmp;
  StoreU32(ctx, frame, map);
  StoreU32(ctx, frame + 4u, key);
  StoreU32(ctx, frame + 8u, value);
  StoreU32(ctx, frame + 12u, entry);
  StoreU32(ctx, frame + 16u, end);
  StoreU32(ctx, frame + 20u, hash);
  StoreU32(ctx, frame + 24u, changed);
  tmp = ReturnFrameSp(ctx);
  tmp2 = ReturnFrameSp(ctx);
  tmp2 = LoadU32(ctx, tmp2);
  tmp2 += kUnorderedInsertSaveFrameBytes;
  StoreU32(ctx, tmp, tmp2);
frame_stored:;
  LeaveFrame();
}
}  // namespace artc
