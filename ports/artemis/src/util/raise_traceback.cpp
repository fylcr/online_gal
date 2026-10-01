#include "util/engine_context.h"

namespace artc {

// Cross-TU callees: the output flush, the keyed-entry steps and the traceback
// frame and slot formatters this path drives.
uint32_t FlushOutputRoomIfFull(EngineContext* ctx, uint32_t state, uint32_t limit, uint32_t chunk);
void InternKeyIntoTableSlot(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
void InsertFormattedKeyEntry(EngineContext* ctx, uint32_t a, uint32_t b);
uint32_t StepCallValueFrame(EngineContext* ctx, uint32_t a);
uint32_t TrimLocalSlotsAboveLevel(EngineContext* ctx, uint32_t proto, uint32_t level,
                                  uint32_t result);
uint32_t FormatTracebackFrame(EngineContext* ctx, uint32_t state, uint32_t name, uint32_t out);
void FormatTracebackSlotEntry(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

namespace {

constexpr uint32_t kRaiseTracebackConst29D74 = 0x29D74u;
constexpr uint32_t kRaiseTracebackMsgSD = 0x29D77u;  // "%s:%d: "
constexpr uint32_t kRaiseTracebackConst29D7F = 0x29D7Fu;
constexpr uint32_t kRaiseTracebackSaveFrameBytes = 16u;
constexpr uint32_t kRaiseTracebackResumeWordBytes = 4u;

}  // namespace

// Prefixes one error message with its "%s:%d:" traceback source position: a
// rewinding pass re-reads the frame the raise saved, a running pass lays out a
// fresh scratch frame and runs one of the six steps below. Answers a value word.
uint32_t RaiseTraceback(EngineContext* ctx, uint32_t value, uint32_t message, uint32_t tag) {
  EnterFrame();
  uint32_t resume_code = 0u;
  uint32_t frame_base = 0u;
  uint32_t phase = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the saved frame and read its words back in the saved order.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t frame_top = ReturnFrameSp(ctx);
    frame_top = LoadU32(ctx, frame_top) - kRaiseTracebackSaveFrameBytes;
    StoreU32(ctx, sp, frame_top);
    uint32_t frame = ReturnFrameSp(ctx);
    frame = LoadU32(ctx, frame);
    value = LoadU32(ctx, frame);
    tag = LoadU32(ctx, frame + 8u);
    frame_base = LoadU32(ctx, frame + 12u);
    message = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word naming the call that was in flight.
    uint32_t sp = ReturnFrameSp(ctx);
    uint32_t frame_top = ReturnFrameSp(ctx);
    frame_top = LoadU32(ctx, frame_top) - kRaiseTracebackResumeWordBytes;
    StoreU32(ctx, sp, frame_top);
    uint32_t word = ReturnFrameSp(ctx);
    word = LoadU32(ctx, word);
    resume_code = LoadU32(ctx, word);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    frame_base = GuestStackSp(ctx) - 128u;
    GuestStackSp(ctx) = frame_base;
    StoreU32(ctx, frame_base + 20u, tag);
    tag = TrimLocalSlotsAboveLevel(ctx, value, 1u, frame_base + 24u) == 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (tag != 0u) {
      goto keyed_entry_arm;
    }
    tag = frame_base + 24u;
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_code == 0u) {
    // Rewinding only reaches an arm whose code already matches this one; this is
    // the code a running pass resumes with.
    uint32_t formatted = FormatTracebackFrame(ctx, value, kRaiseTracebackConst29D74, tag);
    resume_code = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
    tag = formatted;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    tag = LoadU32(ctx, frame_base + 44u);
    // Signed compare: a word below one skips the keyed-entry arm below.
    if ((int32_t)tag < (int32_t)1u) {
      goto keyed_entry_arm;
    }
    StoreU32(ctx, frame_base + 4u, tag);
    tag = frame_base + 60u;
    StoreU32(ctx, frame_base, tag);
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_code == 1u) {
    uint32_t flushed = FlushOutputRoomIfFull(ctx, value, kRaiseTracebackMsgSD, frame_base);
    resume_code = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
    tag = flushed;
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    goto exit_path;
  }
keyed_entry_arm:;
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_code == 2u) {
    InternKeyIntoTableSlot(ctx, value, kRaiseTracebackConst29D7F, 0u);
    resume_code = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
  }
exit_path:;
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    tag = LoadU32(ctx, frame_base + 20u);
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_code == 3u) {
    FormatTracebackSlotEntry(ctx, value, message, tag);
    resume_code = 3u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_code == 4u) {
    InsertFormattedKeyEntry(ctx, value, 2u);
    resume_code = 4u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_code == 5u) {
    uint32_t stepped = StepCallValueFrame(ctx, value);
    resume_code = 5u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
    value = stepped;
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning) {
    // The running pass hands its scratch frame back to the guest stack.
    GuestStackSp(ctx) = frame_base + 128u;
    LeaveFrame();
    return value;
  }
  TrapUnreachable();
suspend_here:;
  // Push the resume word naming the call that was in flight, then the frame the
  // rewind path pops; both go in the order the pop above reads them back.
  uint32_t slot = ReturnFrameSp(ctx);
  slot = LoadU32(ctx, slot);
  StoreU32(ctx, slot, resume_code);
  uint32_t sp = ReturnFrameSp(ctx);
  uint32_t word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + kRaiseTracebackResumeWordBytes;
  StoreU32(ctx, sp, word_top);
  uint32_t base = ReturnFrameSp(ctx);
  base = LoadU32(ctx, base);
  StoreU32(ctx, base, value);
  StoreU32(ctx, base + 4u, message);
  StoreU32(ctx, base + 8u, tag);
  StoreU32(ctx, base + 12u, frame_base);
  sp = ReturnFrameSp(ctx);
  word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + kRaiseTracebackSaveFrameBytes;
  StoreU32(ctx, sp, word_top);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
