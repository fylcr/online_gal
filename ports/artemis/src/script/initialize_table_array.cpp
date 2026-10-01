#include "util/engine_context.h"

namespace artc {

// Defined in other products.
uint32_t BucketLog2(EngineContext* ctx, uint32_t value);
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size);
uint32_t RaiseBlockTooBig(EngineContext* ctx, uint32_t state);
void RaiseScriptError(EngineContext* ctx, uint32_t state, uint32_t message, uint32_t argument);

namespace {

constexpr uint32_t kInitializeTableArrayDefaultSize = 0x2BBD0u;  // the array size a table starts out with
constexpr uint32_t kInitializeTableArrayOverflowMessage = 0x2BBF0u;  // table overflow
constexpr uint32_t kInitializeTableArraySaveFrameBytes = 20u;  // func_state, table, size, log2 size, slot count
constexpr uint32_t kInitializeTableArrayResumeWordBytes = 4u;
constexpr uint32_t kInitializeTableArrayOverflowCall = 0u;  // the overflow raise was in flight
constexpr uint32_t kInitializeTableArrayReallocCall = 1u;  // the array allocation was in flight
constexpr uint32_t kInitializeTableArrayBlockTooBigCall = 2u;  // the too-big raise was in flight

}  // namespace

// Sizes the array part of a table a constructor is building: derives the slot
// count from the requested size, allocates the array block, and clears the
// slack slots. A size that cannot fit the table raises the overflow error, and a
// size past the 31-bit limit is raised as a block-too-big error. The step
// suspends around its three calls; each gate replays the one its resume code
// names.
void InitializeTableArray(EngineContext* ctx, uint32_t func_state, uint32_t table, uint32_t size) {
  EnterFrame();
  uint32_t frame = 0u;
  uint32_t size_is_zero = 0u;
  uint32_t call_result = 0u;
  uint32_t slot = 0u;
  uint32_t resume_code = 0u;
  uint32_t array_pointer = 0u;
  uint32_t slot_count = 0u;
  uint32_t log2_size = 0u;
  uint32_t resume_word = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kInitializeTableArraySaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    func_state = LoadU32(ctx, frame);
    table = LoadU32(ctx, frame + 4u);
    size = LoadU32(ctx, frame + 8u);
    log2_size = LoadU32(ctx, frame + 12u);
    slot_count = LoadU32(ctx, frame + 16u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kInitializeTableArrayResumeWordBytes);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    size_is_zero = static_cast<uint32_t>(size == 0u);
    if (size_is_zero != 0u) {
      // No size asked for: the table takes its default array.
      size = kInitializeTableArrayDefaultSize;
      StoreU32(ctx, table + 16u, kInitializeTableArrayDefaultSize);
      goto settled;
    }
    // Two to the power of the size's log2 is the slot count; past 26 the size
    // cannot be represented and the caller's error case takes over.
    size = BucketLog2(ctx, size - 1u);
    log2_size = size + 1u;
    size = static_cast<uint32_t>((int32_t)size >= (int32_t)26u);
  }
  if (size != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_word == kInitializeTableArrayOverflowCall) {
      RaiseScriptError(ctx, func_state, kInitializeTableArrayOverflowMessage, 0u);
      resume_code = kInitializeTableArrayOverflowCall;
      if (ResumePhase(ctx) == kResumeSuspended) {goto suspend_point;}
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot_count = 1u << (log2_size & 31u);
    size = static_cast<uint32_t>(log2_size <= 26u);
  }
  if (size != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      size = slot_count << 5;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_word == kInitializeTableArrayReallocCall) {
      call_result = ReallocateBlock(ctx, func_state, 0u, 0u, size);
      resume_code = kInitializeTableArrayReallocCall;
      if (ResumePhase(ctx) == kResumeSuspended) {goto suspend_point;}
      size = call_result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, table + 16u, size);
      array_pointer = table + 16u;
      goto array_ready;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_word == kInitializeTableArrayBlockTooBigCall) {
    call_result = RaiseBlockTooBig(ctx, func_state);
    resume_code = kInitializeTableArrayBlockTooBigCall;
    if (ResumePhase(ctx) == kResumeSuspended) {goto suspend_point;}
    size = call_result;
  }
  if (ResumePhase(ctx) != kResumeRunning) {
    array_pointer = func_state;
  } else {
    StoreU32(ctx, table + 16u, size);
    if (log2_size == 31u) {goto settled;}
    array_pointer = table + 16u;
  }
  array_ready:;
  func_state = array_pointer;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Clear the array head, then the remaining slots when it has more than one.
    StoreU32(ctx, size + 8u, 0u);
    StoreU64(ctx, size + 24u, 0ull);
    if ((int32_t)slot_count >= (int32_t)2u && log2_size != 0u) {
      size = 1u;
      do {
        slot = LoadU32(ctx, func_state) + (size << 5);
        StoreU32(ctx, slot + 8u, 0u);
        StoreU64(ctx, slot + 24u, 0ull);
        size = size + 1u;
      } while (slot_count != size);
    }
    size = LoadU32(ctx, func_state);
  }
  settled:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The table records the log2 of its array and the byte end of the array.
    StoreU8(ctx, table + 7u, log2_size);
    StoreU32(ctx, table + 20u, size + (slot_count << 5));
  }
  // The running pass is done; the frame write below belongs to a suspend only.
  goto exit_path;
  // Suspend here: the resume word naming the call in flight goes first, then the
  // five saved words, and the caller's frame pointer is left above both.
  suspend_point:;
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kInitializeTableArrayResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, func_state);
  StoreU32(ctx, frame + 4u, table);
  StoreU32(ctx, frame + 8u, size);
  StoreU32(ctx, frame + 12u, log2_size);
  StoreU32(ctx, frame + 16u, slot_count);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kInitializeTableArraySaveFrameBytes);
  exit_path:;
  LeaveFrame();
}
}  // namespace artc
