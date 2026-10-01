#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this step drives.
void RelocateElementArray(EngineContext* ctx, uint32_t state, uint32_t table);

// Slides the live entries of an entry queue together and clears the tail past the
// holes.  An entry is 24 bytes and its first 16 bytes are the payload that moves.
// Returns the number of entries still queued.  Suspendable: relocating the element
// array and the queue callback can both suspend.
uint32_t CompactEntryQueue(EngineContext* ctx, uint32_t state, uint32_t queue) {
  EnterFrame();
  uint32_t cursor = 0u;
  uint32_t flag = 0u;
  uint32_t dest = 0u;
  uint32_t saved_cursor = 0u;
  uint32_t count = 0u;
  uint32_t callback = 0u;
  uint32_t scratch_frame = 0u;
  uint32_t value = 0u;
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 40-byte frame the suspend path saved.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 40u);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    queue = LoadU32(ctx, frame + 4u);
    cursor = LoadU32(ctx, frame + 8u);
    flag = LoadU32(ctx, frame + 12u);
    dest = LoadU32(ctx, frame + 16u);
    count = LoadU32(ctx, frame + 20u);
    callback = LoadU32(ctx, frame + 24u);
    scratch_frame = LoadU32(ctx, frame + 28u);
    value = LoadU32(ctx, frame + 32u);
    saved_cursor = LoadU32(ctx, frame + 36u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 4-byte frame that saves the resume point.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Claim the 112-byte scratch area this step needs on the guest stack.
    scratch_frame = GuestStackSp(ctx) - 112u;
    GuestStackSp(ctx) = scratch_frame;
    count = (LoadU8(ctx, state + 56u) & 2u) == 0u ? 1u : 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (count != 0u) {
      // Nothing to compact: the queue cursor only slides off the last entry.
      dest = LoadU32(ctx, state + 20u);
      goto skip_compaction;
    }
    callback = LoadU32(ctx, state + 68u);
    flag = callback == 0u ? 1u : 0u;
    count = LoadU32(ctx, state + 32u);
    cursor = count;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (flag != 0u) goto scan_entry;
    flag = LoadU8(ctx, state + 57u) == 0u ? 1u : 0u;
    if (flag != 0u) goto scan_entry;
    cursor = LoadU32(ctx, state + 20u);
    value = LoadU32(ctx, cursor + 8u);
    dest = LoadU32(ctx, state + 8u);
    StoreU32(ctx, scratch_frame + 28u, 0xFFFFFFFFu);
    StoreU32(ctx, scratch_frame + 8u, 1u);
    StoreU32(ctx, scratch_frame + 104u, I32_DIV_S(cursor - LoadU32(ctx, state + 40u), 24u));
    cursor = (uint32_t)((int32_t)(LoadU32(ctx, state + 28u) - dest) <= (int32_t)320);
  }
  if (cursor != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      cursor = LoadU32(ctx, state + 44u);
      flag = cursor << 1u;
      cursor = (int32_t)cursor < 20 ? cursor + 20u : flag;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      RelocateElementArray(ctx, state, cursor);
      resume_point = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    flag = LoadU32(ctx, state + 20u);
    StoreU32(ctx, flag + 8u, LoadU32(ctx, state + 8u) + 320u);
    StoreU8(ctx, state + 57u, 0u);
    cursor = scratch_frame + 8u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    CallTableT0(ctx, callback, state, cursor);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, state + 57u, 1u);
    StoreU32(ctx, LoadU32(ctx, state + 20u) + 8u, LoadU32(ctx, state + 32u) + (value - count));
    flag = LoadU32(ctx, state + 32u) + (dest - count);
    StoreU32(ctx, state + 8u, flag);
  }
scan_entry:
  if (ResumePhase(ctx) == kResumeRunning) {
    dest = LoadU32(ctx, state + 20u);
    uint32_t const entry = LoadU32(ctx, LoadU32(ctx, dest + 4u));
    flag = LoadU8(ctx, entry + 6u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (flag != 0u) goto settle_queue;
    flag = (LoadU8(ctx, state + 56u) & 2u) == 0u ? 1u : 0u;
    if (flag != 0u) goto settle_queue;
  }
next_node:
  if (ResumePhase(ctx) == kResumeRunning) {
    uint32_t const pending = LoadU32(ctx, dest + 20u);
    StoreU32(ctx, dest + 20u, pending - 1u);
    if (pending == 0u) goto settle_queue;
    value = LoadU32(ctx, state + 68u);  // the queue callback index
    flag = value == 0u ? 1u : 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (flag != 0u) goto check_more;
    if (LoadU8(ctx, state + 57u) == 0u) goto check_more;
    callback = LoadU32(ctx, state + 8u);
    saved_cursor = LoadU32(ctx, dest + 8u);
    StoreU32(ctx, scratch_frame + 104u, 0u);
    StoreU32(ctx, scratch_frame + 28u, 0xFFFFFFFFu);
    StoreU32(ctx, scratch_frame + 8u, 4u);
    flag = (uint32_t)((int32_t)(LoadU32(ctx, state + 28u) - callback) <= (int32_t)320);
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      dest = LoadU32(ctx, state + 44u);
      flag = dest << 1u;
      flag = (int32_t)dest < 20 ? dest + 20u : flag;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
      RelocateElementArray(ctx, state, flag);
      resume_point = 2u;
      if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, LoadU32(ctx, state + 20u) + 8u, LoadU32(ctx, state + 8u) + 320u);
    StoreU8(ctx, state + 57u, 0u);
    flag = scratch_frame + 8u;
    callback = callback - cursor;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
    CallTableT0(ctx, value, state, flag);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_point = 3u;
    if (ResumePhase(ctx) == kResumeSuspended) goto suspend_point;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, state + 57u, 1u);
    dest = LoadU32(ctx, state + 20u);
    StoreU32(ctx, dest + 8u, LoadU32(ctx, state + 32u) + (saved_cursor - cursor));
    cursor = LoadU32(ctx, state + 32u);
    StoreU32(ctx, state + 8u, cursor + callback);
    callback = LoadU8(ctx, state + 56u);
  }
check_more:
  if (ResumePhase(ctx) == kResumeRunning) {
    flag = (LoadU8(ctx, state + 56u) & 2u) == 0u ? 1u : 0u;
    if (flag != 0u) goto next_node;
  }
settle_queue:
  if (ResumePhase(ctx) == kResumeRunning) {
    queue = queue + cursor - count;
  }
skip_compaction:
  if (ResumePhase(ctx) == kResumeRunning) {
    // The retired entry is the one before the cursor; its trailing run of
    // entries is what gets moved and then cleared.
    cursor = dest - 24u;
    StoreU32(ctx, state + 20u, cursor);
    uint32_t const moved = LoadU32(ctx, dest + 16u);
    dest = LoadU32(ctx, dest + 4u);
    StoreU32(ctx, state + 12u, LoadU32(ctx, cursor));
    StoreU32(ctx, state + 24u, LoadU32(ctx, cursor + 12u));
    if (moved == 0u) goto store_cursor;
    cursor = moved;
  copy_head:
    if (LoadU32(ctx, state + 8u) > queue) {
      StoreU64(ctx, dest, LoadU64(ctx, queue));
      StoreU32(ctx, dest + 8u, LoadU32(ctx, queue + 8u));
      dest += 16u;
      queue += 16u;
      cursor -= 1u;
      if (cursor != 0u) goto copy_head;
      goto store_cursor;
    }
    if ((int32_t)cursor < 1) goto store_cursor;
  clear_tail:
    for (;;) {
      StoreU32(ctx, dest + 8u, 0u);
      dest += 16u;
      bool const more = (int32_t)cursor > 1;
      cursor -= 1u;
      if (!more) break;
    }
  store_cursor:
    StoreU32(ctx, state + 8u, dest);
    GuestStackSp(ctx) = scratch_frame + 112u;
    LeaveFrame();
    return moved + 1u;
  }
  TrapUnreachable();
suspend_point:
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, queue);
  StoreU32(ctx, frame + 8u, cursor);
  StoreU32(ctx, frame + 12u, flag);
  StoreU32(ctx, frame + 16u, dest);
  StoreU32(ctx, frame + 20u, count);
  StoreU32(ctx, frame + 24u, callback);
  StoreU32(ctx, frame + 28u, scratch_frame);
  StoreU32(ctx, frame + 32u, value);
  StoreU32(ctx, frame + 36u, saved_cursor);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 40u);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
