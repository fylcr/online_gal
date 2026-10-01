#include "util/engine_context.h"

namespace artc {

// Forward declaration -- the byte copier this append hands the fitting case to.
void CopyIfNonEmpty(EngineContext* ctx, uint32_t dst, uint32_t src, uint32_t n);

// Appends a byte range to a guest string record: the range is copied in behind
// the record's current contents when its spare capacity still covers it, and
// otherwise the relocation is handed to the suspendable grow-and-replace helper
// first. Both record forms are honoured -- the long form's pointer, length and
// biased capacity word, and the short form's inline bytes. Answers the record,
// or zero when the helper suspended.
uint32_t AppendStringBytes(EngineContext* ctx, uint32_t text, uint32_t src, uint32_t n) {
  EnterFrame();
  uint32_t spill = 0u;
  uint32_t scratch_word = 0u;
  uint32_t text_ref = 0u;
  uint32_t text_size = 0u;
  uint32_t text_span = 0u;
  uint32_t call_result = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewind: the saved arguments and locals, then the scratch frame the
    // suspended run had taken.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 28u);
    spill = LoadU32(ctx, ReturnFrameSp(ctx));
    text = LoadU32(ctx, spill);
    src = LoadU32(ctx, spill + 4u);
    n = LoadU32(ctx, spill + 8u);
    text_ref = LoadU32(ctx, spill + 12u);
    text_size = LoadU32(ctx, spill + 16u);
    text_span = LoadU32(ctx, spill + 20u);
    spill = LoadU32(ctx, spill + 24u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewind: the grow call returns nothing, so the word it left above that
    // frame is always zero.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    call_result = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A scratch frame for the terminator round trip, then the record's capacity
    // and length from whichever of its two forms is live.
    spill = GuestStackSp(ctx) - 16u;
    GuestStackSp(ctx) = spill;
    text_ref = text;
    if ((LoadU8(ctx, text_ref + 11u) >> 7u) != 0u) {
      text_span = (LoadU32(ctx, text_ref + 8u) & 0x7FFFFFFFu) - 1u;
    } else {
      text_span = 10u;
    }
    if ((LoadU8(ctx, text_ref + 11u) >> 7u) != 0u) {
      text_size = LoadU32(ctx, text + 4u);
    } else {
      text_size = LoadU8(ctx, text + 11u);
    }
    // The append fits while the spare capacity still covers it.
    scratch_word = ((text_span - text_size) >= n) ? 1u : 0u;
    if (scratch_word != 0u) {
      if (n != 0u) {
        // Where the record's bytes live: behind the long form's pointer, or in
        // the short record itself.
        if ((LoadU8(ctx, text + 11u) >> 7u) != 0u) {
          text_span = LoadU32(ctx, text_ref);
        } else {
          text_span = text_ref;
        }
        CopyIfNonEmpty(ctx, text_size + text_span, src, n);
        uint32_t const new_size = n + text_size;
        if ((LoadU8(ctx, text_ref + 11u) >> 7u) != 0u) {
          StoreU32(ctx, text + 4u, new_size);
        } else {
          StoreU8(ctx, text + 11u, new_size);
        }
        // The terminator is parked in the scratch frame and read back out.
        StoreU8(ctx, spill + 15u, 0u);
        uint32_t const terminator = new_size + text_span;
        StoreU8(ctx, terminator, LoadU8(ctx, spill + 15u));
      }
      GuestStackSp(ctx) = spill + 16u;
      LeaveFrame();
      return text;
    }
    // The capacity cannot take the append, so the helper absorbs the overflow.
    text_ref = (n + text_size) - text_span;
  }
  uint32_t const call_reported_zero = (call_result == 0u) ? 1u : 0u;
  uint32_t const grow_gate =
      (ResumePhase(ctx) != kResumeRunning) ? call_reported_zero : 1u;
  if (grow_gate != 0u) {
    GrowStringAndReplace(ctx, text, text_span, text_ref, text_size, text_size, 0u, n, src);
    if (ResumePhase(ctx) == kResumeSuspended) {
      // Suspended: the void call's zero, then the arguments and locals.
      scratch_word = 0u;
      StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), scratch_word);
      StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
      scratch_word = LoadU32(ctx, ReturnFrameSp(ctx));
      StoreU32(ctx, scratch_word, text);
      StoreU32(ctx, scratch_word + 4u, src);
      StoreU32(ctx, scratch_word + 8u, n);
      StoreU32(ctx, scratch_word + 12u, text_ref);
      StoreU32(ctx, scratch_word + 16u, text_size);
      StoreU32(ctx, scratch_word + 20u, text_span);
      StoreU32(ctx, scratch_word + 24u, spill);
      StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 28u);
      LeaveFrame();
      return 0u;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = spill + 16u;
    LeaveFrame();
    return text;
  }
  TrapUnreachable();
  LeaveFrame();
  return 0u;
}

}  // namespace artc
