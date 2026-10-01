#include "util/engine_context.h"

namespace artc {

uint32_t FillBytes(EngineContext* ctx, uint32_t dst, uint32_t value, uint32_t length);
uint32_t MatchCStringPrefix(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

namespace {
// The adjustment that reserves the scratch frame on the guest stack.
constexpr uint32_t kCombineComparisonResultsFrameAdjust = 0xFFFFFFC0u;
constexpr uint32_t kCombineComparisonResultsSaveFrameBytes = 24u;
constexpr uint32_t kCombineComparisonResultsResumeWordBytes = 4u;
}  // namespace

// Merges the two operand results of a three-way comparison into one value. It
// fills a scratch frame, asks the host to insert the left operand, and then
// picks the result out of the frame according to the comparison's outcome word.
uint32_t CombineComparisonResults(EngineContext* ctx, uint32_t left, uint32_t right,
                                  uint32_t result, uint32_t box) {
  EnterFrame();
  uint32_t top = 0u;
  uint32_t arg = 0u;
  uint32_t third = 0u;
  uint32_t fourth = 0u;
  uint32_t fifth = 0u;
  uint32_t sixth = 0u;
  uint32_t seventh = 0u;
  // The scratch frame, the restored word, and the boxed member.
  uint32_t frame = 0u;
  // The frame base, and the left operand's string.
  uint32_t hold = 0u;
  // The left operand's body, and the word the spill stages.
  uint32_t off = 0u;
  // The await code a suspend was resumed with.
  uint32_t await_code = 0u;

  if (ResumePhase(ctx) == 2u) {
    top = ReturnFrameSp(ctx);
    arg = ReturnFrameSp(ctx);
    arg = LoadU32(ctx, arg);
    arg = arg - kCombineComparisonResultsSaveFrameBytes;
    StoreU32(ctx, top, arg);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top);
    hold = top;
    left = LoadU32(ctx, hold);
    right = LoadU32(ctx, hold + 4u);
    result = LoadU32(ctx, hold + 8u);
    box = LoadU32(ctx, hold + 12u);
    frame = LoadU32(ctx, hold + 16u);
    hold = LoadU32(ctx, hold + 20u);
  }
  if (ResumePhase(ctx) == 2u) {
    top = ReturnFrameSp(ctx);
    arg = ReturnFrameSp(ctx);
    arg = LoadU32(ctx, arg);
    arg = arg - kCombineComparisonResultsResumeWordBytes;
    StoreU32(ctx, top, arg);
    top = ReturnFrameSp(ctx);
    top = LoadU32(ctx, top);
    await_code = LoadU32(ctx, top);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    top = GuestStackSp(ctx);
    arg = kCombineComparisonResultsFrameAdjust;
    top = top + arg;
    frame = top;
    GuestStackSp(ctx) = top;
    top = LoadU32(ctx, left);
    off = top;
    arg = 4u;
    top = top - arg;
    hold = LoadU32(ctx, top);
    top = off;
    arg = 8u;
    top = top - arg;
    off = LoadU32(ctx, top);
    StoreU32(ctx, frame + 20u, box);
    StoreU32(ctx, frame + 16u, right);
    StoreU32(ctx, frame + 12u, left);
    StoreU32(ctx, frame + 8u, result);
    right = 0u;
    top = frame + 24u;
    arg = 0u;
    third = 39u;
    box = FillBytes(ctx, top, arg, third);
    top = MatchCStringPrefix(ctx, hold, result, 0u);
    result = top;
    left = left + off;
  }
  top = result;
  arg = ResumePhase(ctx);
  arg = arg == 2u;
  top = top | arg;
  if (top != 0u) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, frame + 56u, 1u);
      top = LoadU32(ctx, hold);
      result = LoadU32(ctx, top + 20u);
      right = frame + 8u;
    }
    if (await_code == 0u || ResumePhase(ctx) == kResumeRunning) {
      top = hold;
      arg = right;
      third = left;
      fourth = left;
      fifth = 1u;
      sixth = 0u;
      seventh = result;
      CallTableT15(ctx, seventh, top, arg, third, fourth, fifth, sixth);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      top = 0u;
      arg = ResumePhase(ctx);
      arg = arg == kResumeSuspended;
      if (arg != 0u) {
        goto save_frame;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      top = LoadU32(ctx, frame + 32u);
      arg = 1u;
      top = top == arg;
      third = left;
      fourth = 0u;
      top = top ? third : fourth;
      right = top;
      goto unwind;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    top = LoadU32(ctx, hold);
    box = LoadU32(ctx, top + 24u);
    result = frame + 8u;
  }
  if (await_code == 1u || ResumePhase(ctx) == kResumeRunning) {
    top = hold;
    arg = result;
    third = left;
    fourth = 1u;
    fifth = 0u;
    sixth = box;
    CallTableT11(ctx, sixth, top, arg, third, fourth, fifth);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    top = 1u;
    arg = ResumePhase(ctx);
    arg = arg == kResumeSuspended;
    if (arg != 0u) {
      goto save_frame;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    top = LoadU32(ctx, frame + 44u);
    if (top == 0u) {
      // No operand went missing, so the result is one of the three operands.
      top = LoadU32(ctx, frame + 28u);
      arg = LoadU32(ctx, frame + 40u);
      third = 1u;
      arg = arg == third;
      top = arg ? top : 0u;
      arg = LoadU32(ctx, frame + 36u);
      third = 1u;
      arg = arg == third;
      top = arg ? top : 0u;
      arg = LoadU32(ctx, frame + 48u);
      third = 1u;
      arg = arg == third;
      top = arg ? top : 0u;
      right = top;
      goto unwind;
    }
    if (top == 1u) {
      top = LoadU32(ctx, frame + 32u);
      arg = 1u;
      top = top != arg;
      if (top != 0u) {
        if (LoadU32(ctx, frame + 48u) != 0u) {
          goto unwind;
        }
        top = LoadU32(ctx, frame + 36u);
        arg = 1u;
        top = top != arg;
        if (top != 0u) {
          goto unwind;
        }
        top = LoadU32(ctx, frame + 40u);
        arg = 1u;
        top = top != arg;
        if (top != 0u) {
          goto unwind;
        }
      }
      right = LoadU32(ctx, frame + 24u);
    } else {
      goto unwind;
    }
  }
unwind:;
  if (ResumePhase(ctx) == kResumeRunning) {
    top = frame;
    arg = kCombineComparisonResultsFrameAdjust;
    top = top - arg;
    GuestStackSp(ctx) = top;
    top = right;
    LeaveFrame();
    return top;
  }
  TrapUnreachable();
save_frame:;
  // Push the word naming the call in flight, then the save frame; a rewind
  // reads them back in that order.
  off = top;
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top);
  StoreU32(ctx, top, off);
  top = ReturnFrameSp(ctx);
  arg = ReturnFrameSp(ctx);
  arg = LoadU32(ctx, arg);
  arg = arg + kCombineComparisonResultsResumeWordBytes;
  StoreU32(ctx, top, arg);
  top = ReturnFrameSp(ctx);
  top = LoadU32(ctx, top);
  off = top;
  StoreU32(ctx, off, left);
  StoreU32(ctx, off + 4u, right);
  StoreU32(ctx, off + 8u, result);
  StoreU32(ctx, off + 12u, box);
  StoreU32(ctx, off + 16u, frame);
  StoreU32(ctx, off + 20u, hold);
  top = ReturnFrameSp(ctx);
  arg = ReturnFrameSp(ctx);
  arg = LoadU32(ctx, arg);
  arg = arg + kCombineComparisonResultsSaveFrameBytes;
  StoreU32(ctx, top, arg);
  top = 0u;
  LeaveFrame();
  return top;
}

}  // namespace artc
