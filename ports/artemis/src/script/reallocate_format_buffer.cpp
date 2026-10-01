#include "util/engine_context.h"

namespace artc {

// Defined in another product.
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size);
uint32_t RaiseBlockTooBig(EngineContext* ctx, uint32_t state);

namespace {

constexpr uint32_t kReallocateFormatBufferSpaceLimit = 0xFFFFFFFDu;  // highest capacity the space can describe
constexpr uint32_t kReallocateFormatBufferSaveFrameBytes = 20u;  // state, buffer, block, new capacity, capacity
constexpr uint32_t kReallocateFormatBufferResumeWordBytes = 4u;
constexpr uint32_t kReallocateFormatBufferReallocCall = 0u;  // the block reallocator was in flight
constexpr uint32_t kReallocateFormatBufferRaiseCall = 1u;  // the block-too-big raise was in flight

}  // namespace

// Grows the format buffer so that it can hold the given amount of text, and
// returns the block the buffer holds afterwards -- the one it already had when
// that is large enough, otherwise a freshly reallocated one. A capacity that the
// guest space cannot describe raises the block-too-big error instead.
//
// The step is suspendable: each gate re-runs the call its resume word names.
uint32_t ReallocateFormatBuffer(EngineContext* ctx, uint32_t state, uint32_t buffer, uint32_t size) {
  EnterFrame();
  uint32_t saved_frame = 0u;
  uint32_t call_result = 0u;
  uint32_t resume_word = 0u;
  uint32_t resume_code = 0u;
  uint32_t new_capacity = 0u;
  uint32_t capacity = 0u;
  uint32_t result = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kReallocateFormatBufferSaveFrameBytes);
    saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, saved_frame);
    size = LoadU32(ctx, saved_frame + 8u);
    new_capacity = LoadU32(ctx, saved_frame + 12u);
    capacity = LoadU32(ctx, saved_frame + 16u);
    buffer = LoadU32(ctx, saved_frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kReallocateFormatBufferResumeWordBytes);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    capacity = LoadU32(ctx, buffer + 8u);
    if (capacity >= size) {
      // Already large enough: hand back the block the buffer holds.
      result = LoadU32(ctx, buffer);
      goto exit_path;
    }
    new_capacity = (size > 32u) ? size : 32u;
    // The size slot now carries the growth verdict; it holds the block below.
    size = static_cast<uint32_t>((new_capacity + 1u) <= kReallocateFormatBufferSpaceLimit);
  }
  if (size != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The block the buffer holds now is the one that gets reallocated.
      size = LoadU32(ctx, buffer);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_word == kReallocateFormatBufferReallocCall) {
      call_result = ReallocateBlock(ctx, state, size, capacity, new_capacity);
      resume_code = kReallocateFormatBufferReallocCall;
      if (ResumePhase(ctx) == kResumeSuspended) {goto suspend_point;}
      size = call_result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {goto commit;}
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_word == kReallocateFormatBufferRaiseCall) {
    call_result = RaiseBlockTooBig(ctx, state);
    resume_code = kReallocateFormatBufferRaiseCall;
    if (ResumePhase(ctx) == kResumeSuspended) {goto suspend_point;}
    size = call_result;
  }
  commit:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, buffer + 8u, new_capacity);
    StoreU32(ctx, buffer, size);
    result = size;
    goto exit_path;
  }
  TrapUnreachable();
  // Suspend here: the resume word naming the call in flight goes first, then the
  // five saved words, and the caller's frame pointer is left above both.
  suspend_point:;
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kReallocateFormatBufferResumeWordBytes);
  saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, saved_frame, state);
  StoreU32(ctx, saved_frame + 4u, buffer);
  StoreU32(ctx, saved_frame + 8u, size);
  StoreU32(ctx, saved_frame + 12u, new_capacity);
  StoreU32(ctx, saved_frame + 16u, capacity);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kReallocateFormatBufferSaveFrameBytes);
  result = 0u;
  exit_path:;
  LeaveFrame();
  return result;
}
}  // namespace artc
