#include "util/engine_context.h"

namespace artc {

// Forward declarations -- callees owned by other products.
void ThrowScriptError(EngineContext* ctx, uint32_t state, uint32_t code);
void RaiseScriptError(EngineContext* ctx, uint32_t state, uint32_t message, uint32_t argument);

namespace {
// The failure reported when the request does not exceed the count already held.
constexpr uint32_t kGrowArrayMessage = 0x2AF6Eu;  // memory allocation error: block too big
// The largest block the heap can address.
constexpr uint32_t kGrowArraySpaceLimit = 0xFFFFFFFDu;
// The two regions the suspend path pushes: the word naming the call that was in
// flight, and the nine-word block holding every argument.
constexpr uint32_t kGrowArrayResumeWordBytes = 4u;
constexpr uint32_t kGrowArraySaveFrameBytes = 36u;
}  // namespace

// Grows the element array whose count word `count_field` points at so that it
// holds at least `count` elements of `stride` bytes: the storage is reallocated
// through the heap's table entry, then the count word and the heap's live-byte
// tally are written back. A request that does not exceed the count already held
// is refused with the caller's message instead. The reallocation is suspendable,
// so the body carries the replay gates for it.
uint32_t GrowArray(EngineContext* ctx, uint32_t state, uint32_t size, uint32_t count_field,
                   uint32_t stride, uint32_t count, uint32_t heap) {
  EnterFrame();
  uint32_t resume_word = 0u;
  uint32_t saved_frame = 0u;
  uint32_t flag = 0u;
  uint32_t live_bytes = 0u;
  uint32_t owner = 0u;
  uint32_t fits = 0u;
  uint32_t answer = 0u;
  uint32_t resume_code = 0u;
  uint32_t result = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: drop the saved argument block and rebuild the arguments from
    // the nine words it holds.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kGrowArraySaveFrameBytes);
    saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, saved_frame);
    count_field = LoadU32(ctx, saved_frame + 8u);
    stride = LoadU32(ctx, saved_frame + 12u);
    count = LoadU32(ctx, saved_frame + 16u);
    heap = LoadU32(ctx, saved_frame + 20u);
    flag = LoadU32(ctx, saved_frame + 24u);
    live_bytes = LoadU32(ctx, saved_frame + 28u);
    owner = LoadU32(ctx, saved_frame + 32u);
    size = LoadU32(ctx, saved_frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: drop the word naming the call that was in flight. It sits right
    // below the argument block, so it is dropped on its own.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kGrowArrayResumeWordBytes);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the count the array holds now, and whether it has already reached
    // half of what was asked for.
    live_bytes = LoadU32(ctx, count_field);
    flag = static_cast<uint32_t>(static_cast<int32_t>(live_bytes) >=
                                 static_cast<int32_t>(I32_DIV_S(count, 2u)));
  }
  // A request no larger than the count already held is refused. The verdict is
  // kept so the rewinding pass replays the refusal instead of deciding again.
  flag |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (flag) {
    if (ResumePhase(ctx) == kResumeRunning) {
      flag = static_cast<uint32_t>(static_cast<int32_t>(count) > static_cast<int32_t>(live_bytes));
      if (flag) {
        goto resize_step;
      }
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_word == 0u) : 1u) {
      // Running refuses here; rewinding replays the refusal only when the word
      // names this call. This slot still carries the caller's message id.
      RaiseScriptError(ctx, state, heap, 0u);
      resume_code = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      goto resize_step;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the array doubles, and never falls below four elements.
    count = live_bytes << 1u;
    flag = static_cast<uint32_t>(static_cast<int32_t>(count) > 4);
    count = flag ? count : 4u;
  }
 resize_step:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: whether a block of the doubled count still fits the address
    // space the elements are addressed in.
    flag = DIV_U(kGrowArraySpaceLimit, stride);
    fits = static_cast<uint32_t>(flag >= count + 1u);
  }
  if (fits || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // Running: the storage moves through the heap's own reallocation entry.
      heap = LoadU32(ctx, state + 16u);
      owner = LoadU32(ctx, heap + 16u);
      flag = size;
      live_bytes = stride * LoadU32(ctx, count_field);
      size = stride * count;
      stride = LoadU32(ctx, heap + 12u);  // from here on, the entry to call
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_word == 1u) : 1u) {
      answer = CallTableT7(ctx, stride, owner, flag, live_bytes, size);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    flag = ResumePhase(ctx) ? flag : static_cast<uint32_t>(size == 0);
    if (ResumePhase(ctx) == kResumeRunning) {
      if (flag) {
        goto commit;
      }
      if (answer) {
        goto commit;
      }
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_word == 2u) : 1u) {
      ThrowScriptError(ctx, state, 4u);
      resume_code = 2u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
   commit:;
    if (ResumePhase(ctx) == kResumeRunning) {
      // Running: the moved bytes are booked as live and the new count stands.
      StoreU32(ctx, heap + 68u, LoadU32(ctx, heap + 68u) + (size - live_bytes));
      StoreU32(ctx, count_field, count);
      result = answer;
      goto exit_path;
    }
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_word == 3u) : 1u) {
    RaiseScriptError(ctx, state, kGrowArrayMessage, 0u);
    resume_code = 3u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the count is committed even though the storage did not move, and
    // the failure leaves as a null answer.
    StoreU32(ctx, count_field, count);
    result = 0u;
    goto exit_path;
  }
  TrapUnreachable();
 suspend_point:;
  {
    // Suspend: push the word naming the call in flight, step past it, then the
    // nine-word argument block below it.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kGrowArrayResumeWordBytes);
    saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, saved_frame, state);
    StoreU32(ctx, saved_frame + 4u, size);
    StoreU32(ctx, saved_frame + 8u, count_field);
    StoreU32(ctx, saved_frame + 12u, stride);
    StoreU32(ctx, saved_frame + 16u, count);
    StoreU32(ctx, saved_frame + 20u, heap);
    StoreU32(ctx, saved_frame + 24u, flag);
    StoreU32(ctx, saved_frame + 28u, live_bytes);
    StoreU32(ctx, saved_frame + 32u, owner);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kGrowArraySaveFrameBytes);
  }
  result = 0u;
 exit_path:;
  LeaveFrame();
  return result;
}

}  // namespace artc
