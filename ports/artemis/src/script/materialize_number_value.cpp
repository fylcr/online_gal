#include "util/engine_context.h"

namespace artc {

// Defined in another product.
uint32_t CoerceToNumber(EngineContext* ctx, uint32_t state, uint32_t value);

namespace {

// Suspended-frame words: state, slot, the value being built and the scratch block.
constexpr uint32_t kMaterializeNumberSaveFrameBytes = 16u;
constexpr uint32_t kMaterializeNumberResumeWordBytes = 4u;

}  // namespace

// Turns the numeric value held by `slot` into a TValue the arithmetic path can
// read directly, and returns the state to carry on from.  The CoerceToNumber
// call is suspendable: the resume word names the call that was in flight, so a
// rewound frame replays exactly that call.
uint32_t MaterializeNumberValue(EngineContext* ctx, uint32_t state, uint32_t slot) {
  EnterFrame();
  uint32_t result = 0u;
  uint32_t scratch = 0u;
  uint32_t resume_word = 0u;
  uint32_t resume_code = 0u;
  uint32_t step = 0u;
  uint32_t value_slot = 0u;
  uint32_t call_result = 0u;
  double value = 0;

  // Rewound frame: recover the arguments and the scratch block.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kMaterializeNumberSaveFrameBytes);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, saved_frame);
    slot = LoadU32(ctx, saved_frame + 4u);
    value_slot = LoadU32(ctx, saved_frame + 8u);
    scratch = LoadU32(ctx, saved_frame + 12u);
  }
  // The word naming the call that was in flight.
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kMaterializeNumberResumeWordBytes);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Reserve the scratch block the value is built in.
    scratch = GuestStackSp(ctx) - 16u;
    GuestStackSp(ctx) = scratch;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    step = LoadU32(ctx, state + 8u) - 3u;
    if (step == 1u) {
      // The value sits in the scratch block; skip the frame being consumed.
      value_slot = scratch + 8u;
      state = LoadU32(ctx, state) + 16u;
    } else if (step != 0u) {
      goto keep_state;
    } else {
      // Nothing of this value is left to materialize.
      goto running_done;
    }
  }
  // Only replay the call when it is the one the resume word names.
  if (ResumePhase(ctx) == kResumeRunning || resume_word == 0u) {
    call_result = CoerceToNumber(ctx, state, value_slot);
    resume_code = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    state = call_result;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (state == 0u) {
      // The call reported nothing to materialize; leave the value alone.
      goto keep_state;
    }
    value = LoadF64(ctx, scratch + 8u);
    StoreU32(ctx, slot + 8u, 3u);
    StoreF64(ctx, slot, value);
    state = slot;
    goto running_done;
  }
keep_state:;
  // A running pass has no state left to report.
  if (ResumePhase(ctx) == kResumeRunning) {
    state = 0u;
  }
running_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = scratch + 16u;
    result = state;
    goto exit_path;
  }
  TrapUnreachable();
suspend_point:;
  {
    // Push the word naming the call in flight, step past it, then the frame.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kMaterializeNumberResumeWordBytes);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, saved_frame, state);
    StoreU32(ctx, saved_frame + 4u, slot);
    StoreU32(ctx, saved_frame + 8u, value_slot);
    StoreU32(ctx, saved_frame + 12u, scratch);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kMaterializeNumberSaveFrameBytes);
    result = 0u;
  }
exit_path:;
  LeaveFrame();
  return result;
}

}  // namespace artc
