#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
void DeallocateRawBlock(EngineContext* ctx, uint32_t a0);
uint32_t AllocateStringBuffer(EngineContext* ctx, uint32_t a0);
uint32_t CopyBytes(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t StringLength(EngineContext* ctx, uint32_t text);
void RaiseStringLengthError(EngineContext* ctx, uint32_t a);

namespace {

constexpr uint32_t kCleanupStringSlotAddr = 0xD015Cu;  // where a pending string is parked
constexpr uint32_t kCleanupHeapFlag = 0x80000000u;  // marks a text whose storage is on the heap
constexpr uint32_t kCleanupSizeMask = 0xFFFFFFF0u;  // rounds an allocation up to sixteen
constexpr uint32_t kCleanupConstFFFFFFFF = 0xFFFFFFFFu;  // the -1 a clean sign byte holds
constexpr uint32_t kCleanupSaveFrameBytes = 48u;  // the frame this step saves on suspend
constexpr uint32_t kCleanupResumeWordBytes = 4u;  // the resume word above it

}  // namespace

// Splits the text that pending resources hold into the chunks the host call
// takes: each chunk is copied into a buffer allocated for it, the call is
// dispatched with that buffer, and the text left over is carried across the
// suspension in the save frame. A text past its length limit raises the length
// error, and a host call that failed releases its buffer and traps.
void CleanupPendingResources(EngineContext* ctx, uint32_t p0, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4) {
  EnterFrame();
  uint32_t frame = 0u;
  uint32_t mark = 0u;
  uint32_t split_at = 0u;
  uint32_t resume_point = 0u;
  uint32_t needle = 0u;
  uint32_t scratch = 0u;
  uint32_t text = 0u;
  uint32_t alloc = 0u;
  uint32_t hold = 0u;
  uint32_t cursor = 0u;
  // Rewind: drop the 48-byte frame, then the resume word above it.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kCleanupSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    p0 = LoadU32(ctx, frame);
    p2 = LoadU32(ctx, frame + 8u);
    p3 = LoadU32(ctx, frame + 12u);
    p4 = LoadU32(ctx, frame + 16u);
    text = LoadU32(ctx, frame + 20u);
    scratch = LoadU32(ctx, frame + 24u);
    hold = LoadU32(ctx, frame + 28u);
    cursor = LoadU32(ctx, frame + 32u);
    mark = LoadU32(ctx, frame + 36u);
    split_at = LoadU32(ctx, frame + 40u);
    needle = LoadU32(ctx, frame + 44u);
    p1 = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kCleanupResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    text = GuestStackSp(ctx) - 16u;
    GuestStackSp(ctx) = text;
    StoreU32(ctx, text + 8u, 0u);
    StoreU64(ctx, text, 0ull);
    scratch = LoadU32(ctx, p1 + 4u);
    hold = (scratch < LoadU32(ctx, p1 + 8u));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (hold != 0u) {
      StoreU64(ctx, scratch, LoadU64(ctx, text));
      hold = LoadU32(ctx, text + 8u);
      StoreU32(ctx, scratch + 8u, hold);
      scratch = scratch + 12u;
      StoreU32(ctx, p1 + 4u, scratch);
      goto scan_text;
    }
    StoreU32(ctx, kCleanupStringSlotAddr, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
    HostOsSchedule(ctx, 1412u, p1, text);
    resume_point = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    scratch = LoadU32(ctx, kCleanupStringSlotAddr);
    StoreU32(ctx, kCleanupStringSlotAddr, 0u);
    if (scratch == 1u) goto host_result_top;
    scratch = (static_cast<int32_t>(LoadS8(ctx, text + 11u)) >
        static_cast<int32_t>(kCleanupConstFFFFFFFF));
    if (scratch != 0u) goto scan_text;
    scratch = LoadU32(ctx, text);
    DeallocateRawBlock(ctx, scratch);
  }
  // An empty first byte means there is nothing to dispatch.
  scan_text:;
  if (ResumePhase(ctx) == kResumeRunning) {
    hold = LoadU8(ctx, p0);
    scratch = (hold == 0u);
  }
  if (scratch != 0u && ResumePhase(ctx) == kResumeRunning) goto frame_cleanup;
  // Walk the text one chunk at a time, looking for the byte that ends it.
  walk_text:;
  if (ResumePhase(ctx) == kResumeRunning) {
    cursor = p0;
    p0 = LoadS8(ctx, p3 + 11u);
    scratch = (static_cast<int32_t>(p0) > static_cast<int32_t>(kCleanupConstFFFFFFFF));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (scratch != 0u) {
      mark = p0 & 255u;
      p0 = 0u;
      match_local_bytes:;
      scratch = (p0 == mark);
      if (scratch != 0u) goto chunk_exhausted;
      scratch = p0 + p3;
      p0 = p0 + 1u;
      scratch = (LoadU8(ctx, scratch) != (hold & 255u));
      if (scratch != 0u) goto match_local_bytes;
      goto split_point;
    }
    alloc = LoadU32(ctx, p3);
    mark = LoadU32(ctx, p3 + 4u);
    p0 = 0u;
    match_bytes:;
    scratch = (p0 == mark);
    if (scratch != 0u) goto chunk_exhausted;
    scratch = p0 + alloc;
    p0 = p0 + 1u;
    needle = hold & 255u;
    scratch = (needle != LoadU8(ctx, scratch));
    if (scratch != 0u) goto match_bytes;
    goto split_point;
    chunk_exhausted:;
    mark = cursor;
    needle = hold & 255u;
    p0 = (needle == 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (p0 != 0u) goto text_end;
    p0 = (static_cast<int32_t>(LoadS8(ctx, p2 + 11u)) < 0 ? LoadU32(ctx, p2) : p2);
    alloc = hold;
    scratch = cursor;
    compare_tail:;
    mark = LoadU8(ctx, p0);
    if (mark == 0u) {
      mark = scratch;
      goto text_end;
    }
    mark = ((alloc & 255u) != mark);
    if (mark != 0u) goto sequence_length;
    p0 = p0 + 1u;
    alloc = LoadU8(ctx, scratch + 1u);
    mark = scratch + 1u;
    scratch = mark;
    if (alloc != 0u) goto compare_tail;
    text_end:;
    p0 = (split_at == 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (p0 != 0u) goto append_descriptor;
    hold = cursor - split_at;
    p0 = (hold >= kCleanupSizeMask);
    if (p0 != 0u) goto raise_length;
    p0 = (hold >= 11u);
  }
  if (p0 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    p0 = ResumePhase(ctx) ? p0 : ((hold + 16u) & kCleanupSizeMask);
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
      alloc = AllocateStringBuffer(ctx, p0);
      resume_point = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      scratch = alloc;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = p0 | kCleanupHeapFlag;
      StoreU32(ctx, text + 8u, p0);
      StoreU32(ctx, text, scratch);
      StoreU32(ctx, text + 4u, hold);
      goto copy_chunk;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, text + 11u, hold);
    scratch = text;
    p0 = (hold == 0u);
    if (p0 != 0u) goto copy_done;
  }
  // Copy the chunk into the buffer, then terminate it.
  copy_chunk:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = CopyBytes(ctx, scratch, split_at, hold);
  }
  copy_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, (scratch + hold), 0u);
    p0 = LoadU32(ctx, p1 + 4u);
    StoreU32(ctx, kCleanupStringSlotAddr, 0u);
    hold = p0 - 12u;
    scratch = LoadU8(ctx, text + 11u);
    p0 = ((static_cast<int32_t>(scratch << (24u & 31)) >> (24u & 31)) < 0);
    cursor = p0 ? LoadU32(ctx, text) : text;
    p0 = p0 ? LoadU32(ctx, text + 4u) : scratch;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
    alloc = HostDispatchInvoke(ctx, 860u, hold, cursor, p0);
    resume_point = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    p0 = alloc;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = LoadU32(ctx, kCleanupStringSlotAddr);
    StoreU32(ctx, kCleanupStringSlotAddr, 0u);
    p0 = (p0 == 1u);
    if (p0 != 0u) goto host_result_dispatch;
    p0 = (static_cast<int32_t>(LoadS8(ctx, text + 11u)) >
        static_cast<int32_t>(kCleanupConstFFFFFFFF));
    if (p0 != 0u) goto append_descriptor;
    p0 = LoadU32(ctx, text);
    DeallocateRawBlock(ctx, p0);
  }
  // Append the chunk to the pending list, growing the list if it is full.
  append_descriptor:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, text + 8u, 0u);
    StoreU64(ctx, text, 0ull);
    p0 = LoadU32(ctx, p1 + 4u);
    scratch = (p0 >= LoadU32(ctx, p1 + 8u));
    if (scratch != 0u) goto schedule_after_copy;
    StoreU64(ctx, p0, LoadU64(ctx, text));
    scratch = LoadU32(ctx, text + 8u);
    StoreU32(ctx, p0 + 8u, scratch);
    p0 = p0 + 12u;
    StoreU32(ctx, p1 + 4u, p0);
    goto continue_scan;
  }
  host_result_dispatch:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = HostRet0(ctx);
    HostTempRet0(ctx);
    if (static_cast<int32_t>(LoadS8(ctx, text + 11u)) >
        static_cast<int32_t>(kCleanupConstFFFFFFFF)) goto release_and_trap;
    goto finished;
  }
  schedule_after_copy:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, kCleanupStringSlotAddr, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
    HostOsSchedule(ctx, 1412u, p1, text);
    resume_point = 3u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = LoadU32(ctx, kCleanupStringSlotAddr);
    StoreU32(ctx, kCleanupStringSlotAddr, 0u);
    p0 = (p0 == 1u);
    if (p0 != 0u) goto host_result_schedule;
    p0 = (static_cast<int32_t>(LoadS8(ctx, text + 11u)) >
        static_cast<int32_t>(kCleanupConstFFFFFFFF));
    if (p0 != 0u) goto continue_scan;
    p0 = LoadU32(ctx, text);
    DeallocateRawBlock(ctx, p0);
  }
  continue_scan:;
  if (ResumePhase(ctx) == kResumeRunning) {
    cursor = mark - 1u;
    split_at = 0u;
    goto advance_char;
  }
  host_result_schedule:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = HostRet0(ctx);
    HostTempRet0(ctx);
    if (static_cast<int32_t>(LoadS8(ctx, text + 11u)) >
        static_cast<int32_t>(kCleanupConstFFFFFFFF)) goto release_and_trap;
    goto finished;
  }
  sequence_length:;
  if (ResumePhase(ctx) == kResumeRunning) {
    split_at = split_at ? split_at : cursor;
    // The lead byte of the character under the cursor says how long it runs.
    switch (p4) {
    case 0: goto one_byte;
    case 1: goto two_byte;
    case 2: goto multi_byte;
    default: goto advance_char;
    }
    one_byte:;
    p0 = 1u;
    scratch = ((((hold ^ 32u) & 255u) - 161u) < 60u);
    if (scratch != 0u) goto advance_count;
    goto advance_char;
    two_byte:;
    p0 = 1u;
    scratch = (needle == 142u);
    if (scratch != 0u) goto advance_count;
    scratch = (((hold + 95u) & 255u) < 84u);
    if (scratch != 0u) goto advance_count;
    goto advance_char;
    multi_byte:;
    scratch = 128u;
    p0 = kCleanupConstFFFFFFFF;
    hold = static_cast<uint32_t>(static_cast<int32_t>(hold << (24u & 31)) >> (24u & 31));
    mark = ((hold & 128u) == 0u);
    if (mark != 0u) goto advance_char;
    measure_prefix:;
    p0 = p0 + 1u;
    scratch = static_cast<uint32_t>(static_cast<int32_t>(scratch) >> (1u & 31));
    mark = hold & scratch;
    if (mark != 0u) goto measure_prefix;
    advance_count:;
    cursor = p0 + cursor;
    goto advance_char;
  }
  // The text before the split point is dispatched now; the rest waits.
  split_point:;
  p0 = ResumePhase(ctx) ? p0 : (split_at == 0u);
  if (ResumePhase(ctx) == kResumeRunning) {
    if (p0 != 0u) goto split_at_newline;
    hold = cursor - split_at;
    p0 = (hold >= kCleanupSizeMask);
    if (p0 != 0u) goto raise_length;
    p0 = (hold >= 11u);
  }
  if (p0 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    p0 = ResumePhase(ctx) ? p0 : ((hold + 16u) & kCleanupSizeMask);
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
      alloc = AllocateStringBuffer(ctx, p0);
      resume_point = 4u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      scratch = alloc;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = p0 | kCleanupHeapFlag;
      StoreU32(ctx, text + 8u, p0);
      StoreU32(ctx, text, scratch);
      StoreU32(ctx, text + 4u, hold);
      goto copy_tail;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, text + 11u, hold);
    scratch = text;
    p0 = (hold == 0u);
    if (p0 != 0u) goto terminate_tail;
  }
  copy_tail:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = CopyBytes(ctx, scratch, split_at, hold);
  }
  terminate_tail:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, (scratch + hold), 0u);
    p0 = LoadU32(ctx, p1 + 4u);
    StoreU32(ctx, kCleanupStringSlotAddr, 0u);
    hold = p0 - 12u;
    scratch = LoadU8(ctx, text + 11u);
    p0 = ((static_cast<int32_t>(scratch << (24u & 31)) >> (24u & 31)) < 0);
    mark = p0 ? LoadU32(ctx, text) : text;
    p0 = p0 ? LoadU32(ctx, text + 4u) : scratch;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
    alloc = HostDispatchInvoke(ctx, 860u, hold, mark, p0);
    resume_point = 5u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    p0 = alloc;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = LoadU32(ctx, kCleanupStringSlotAddr);
    StoreU32(ctx, kCleanupStringSlotAddr, 0u);
    p0 = (p0 == 1u);
    if (p0 != 0u) goto host_result_tail;
    p0 = (static_cast<int32_t>(LoadS8(ctx, text + 11u)) >
        static_cast<int32_t>(kCleanupConstFFFFFFFF));
    if (p0 != 0u) goto split_at_newline;
    p0 = LoadU32(ctx, text);
    DeallocateRawBlock(ctx, p0);
  }
  split_at_newline:;
  if (ResumePhase(ctx) == kResumeRunning) {
    scratch = cursor + 1u;
    p0 = (LoadU8(ctx, cursor + 1u) == 10u);
    cursor = p0 ? cursor : scratch;
    split_at = p0 ? 0u : scratch;
    goto advance_char;
  }
  host_result_tail:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = HostRet0(ctx);
    HostTempRet0(ctx);
    if (static_cast<int32_t>(LoadS8(ctx, text + 11u)) >
        static_cast<int32_t>(kCleanupConstFFFFFFFF)) goto release_and_trap;
    goto finished;
  }
  advance_char:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = cursor + 1u;
    hold = LoadU8(ctx, cursor + 1u);
    if (hold != 0u) goto walk_text;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = (split_at == 0u);
    if (p0 != 0u) goto frame_cleanup;
    p2 = StringLength(ctx, split_at);
    p0 = (p2 >= kCleanupSizeMask);
    if (p0 != 0u) goto raise_length;
    p0 = (p2 >= 11u);
  }
  if (p0 != 0u || ResumePhase(ctx) == kResumeRewinding) {
    p0 = ResumePhase(ctx) ? p0 : ((p2 + 16u) & kCleanupSizeMask);
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
      alloc = AllocateStringBuffer(ctx, p0);
      resume_point = 6u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
      scratch = alloc;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = p0 | kCleanupHeapFlag;
      StoreU32(ctx, text + 8u, p0);
      StoreU32(ctx, text, scratch);
      StoreU32(ctx, text + 4u, p2);
      hold = text;
      goto copy_tail_chunk;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, text + 11u, p2);
    hold = text;
    scratch = text;
    p0 = (p2 == 0u);
    if (p0 != 0u) goto tail_ready;
  }
  copy_tail_chunk:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = CopyBytes(ctx, scratch, split_at, p2);
  }
  tail_ready:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, (p2 + scratch), 0u);
    p0 = LoadU32(ctx, p1 + 4u);
    p1 = LoadU8(ctx, hold + 11u);
    StoreU32(ctx, kCleanupStringSlotAddr, 0u);
    p2 = p0 - 12u;
    p0 = ((static_cast<int32_t>(p1 << (24u & 31)) >> (24u & 31)) < 0);
    p3 = p0 ? LoadU32(ctx, text) : text;
    p0 = p0 ? LoadU32(ctx, text + 4u) : p1;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 7u) {
    alloc = HostDispatchInvoke(ctx, 860u, p2, p3, p0);
    resume_point = 7u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    p0 = alloc;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = LoadU32(ctx, kCleanupStringSlotAddr);
    StoreU32(ctx, kCleanupStringSlotAddr, 0u);
    p0 = (p0 == 1u);
    if (p0 != 0u) goto host_result_final;
    p0 = (static_cast<int32_t>(LoadS8(ctx, hold + 11u)) >
        static_cast<int32_t>(kCleanupConstFFFFFFFF));
    if (p0 != 0u) goto frame_cleanup;
    p0 = LoadU32(ctx, text);
    DeallocateRawBlock(ctx, p0);
  }
  // Nothing left to dispatch: hand the scratch frame back.
  frame_cleanup:;
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = text + 16u;
    goto exit_path;
  }
  host_result_final:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = HostRet0(ctx);
    HostTempRet0(ctx);
    if (static_cast<int32_t>(LoadS8(ctx, hold + 11u)) >
        static_cast<int32_t>(kCleanupConstFFFFFFFF)) goto release_and_trap;
    DeallocateRawBlock(ctx, LoadU32(ctx, text));
    goto release_and_trap;
  }
  host_result_top:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = HostRet0(ctx);
    HostTempRet0(ctx);
    if (static_cast<int32_t>(LoadS8(ctx, text + 11u)) >
        static_cast<int32_t>(kCleanupConstFFFFFFFF)) goto release_and_trap;
    goto finished;
  }
  release_and_trap:;
  if (ResumePhase(ctx) == kResumeRunning) {
    HostOsRelease(ctx, p0);
    TrapUnreachable();
  }
  // The text is past the length limit the count can hold.
  raise_length:;
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 8u) {
    RaiseStringLengthError(ctx, text);
    resume_point = 8u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  finished:;
  if (ResumePhase(ctx) == kResumeRunning) {
    DeallocateRawBlock(ctx, LoadU32(ctx, text));
    HostOsRelease(ctx, p0);
    TrapUnreachable();
  }
  goto exit_path;
  suspend_point:;
  // Suspend: push the resume point, then the 48-byte frame with the live state.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kCleanupResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, p0);
  StoreU32(ctx, frame + 4u, p1);
  StoreU32(ctx, frame + 8u, p2);
  StoreU32(ctx, frame + 12u, p3);
  StoreU32(ctx, frame + 16u, p4);
  StoreU32(ctx, frame + 20u, text);
  StoreU32(ctx, frame + 24u, scratch);
  StoreU32(ctx, frame + 28u, hold);
  StoreU32(ctx, frame + 32u, cursor);
  StoreU32(ctx, frame + 36u, mark);
  StoreU32(ctx, frame + 40u, split_at);
  StoreU32(ctx, frame + 44u, needle);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kCleanupSaveFrameBytes);
  exit_path:;
  LeaveFrame();
}

}  // namespace artc
