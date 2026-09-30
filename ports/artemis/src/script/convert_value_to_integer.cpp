#include "util/engine_context.h"

namespace artc {

// Materializes a value that is not yet a number, answering the record it stands
// for afterwards.
uint32_t MaterializeNumberValue(EngineContext* ctx, uint32_t state, uint32_t slot);

namespace {

// The kind word a value record carries when it holds a number already, and the
// kind a record is given while it is being built.
constexpr uint32_t kConvertValueToIntegerNumberKind = 3u;
constexpr uint32_t kConvertValueToIntegerPendingKind = 5u;

// The two slots below zero that stand for shared records this step hands back
// whole; the three of them sit inside the state at these offsets.
constexpr uint32_t kConvertValueToIntegerSpecialSlot = 0xFFFFD8EEu;
constexpr uint32_t kConvertValueToIntegerSpecialSlotBias = 10002u;
constexpr uint32_t kConvertValueToIntegerConstantArea = 72u;
constexpr uint32_t kConvertValueToIntegerPendingArea = 88u;
constexpr uint32_t kConvertValueToIntegerBuiltArea = 96u;

// The first slot that stands for a constant, and the base the slots below it are
// counted down from.
constexpr uint32_t kConvertValueToIntegerConstantSlotBase = 0xFFFFD8F1u;
constexpr uint32_t kConvertValueToIntegerNegativeBase = 0x27118u;

// The record's own byte that has to be small enough for the slot to be a
// constant.
constexpr uint32_t kConvertValueToIntegerSizeByteOffset = 7u;

// Two to the thirty-first: a value at or past it cannot be narrowed, so the
// answer saturates to the smallest integer.
constexpr double kConvertValueToIntegerIntegerLimit = 2147483648.0;
constexpr uint32_t kConvertValueToIntegerSaturated = 0x80000000u;

// The suspend frame: the words this body saves, and the resume word pushed with
// them, whose number names the call that was in flight.
constexpr uint32_t kConvertValueToIntegerSaveFrameBytes = 8u;
constexpr uint32_t kConvertValueToIntegerResumeWordBytes = 4u;
constexpr uint32_t kConvertValueToIntegerMaterializeCall = 0u;

}  // namespace

// Narrows one value to its integer payload. A value that does not hold a number
// yet is materialized first, and a number whose magnitude is two to the
// thirty-first or more saturates to the smallest integer.
uint32_t ConvertValueToInteger(EngineContext* ctx, uint32_t state, uint32_t slot) {
  EnterFrame();
  // The suspend frame base: a rewind pops it and the suspend path writes its two
  // words into it.
  uint32_t frame = 0u;
  // The sixteen bytes of scratch the materialized value is built in.
  uint32_t scratch = 0u;
  // Whether the value already holds a number, so nothing has to be built.
  uint32_t is_number = 0u;
  // The call a rewinding pass re-issues, and the number a spilling pass saves.
  uint32_t resume_point = 0u;
  uint32_t resume_code = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 8-byte frame and read its words back in the saved
    // order.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kConvertValueToIntegerSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    slot = LoadU32(ctx, frame);
    scratch = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word pushed with that frame. It names the call
    // that was in flight, so only that call is issued again.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kConvertValueToIntegerResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The scratch the value is built in lives on the guest stack, so it lasts
    // exactly as long as the call does.
    GuestStackSp(ctx) -= 16u;
    scratch = GuestStackSp(ctx);
    // A positive slot names its own record inside the table; the slots below it
    // name a constant, one of the three shared records, or nothing at all.
    if (static_cast<int32_t>(slot) >= 1) {
      uint32_t const record = LoadU32(ctx, state + 12u) + (slot << 4u) - 16u;
      slot = record < LoadU32(ctx, state + 8u) ? record : kNilValueAddr;
    } else if (static_cast<int32_t>(slot) >= static_cast<int32_t>(kConvertValueToIntegerConstantSlotBase)) {
      slot = LoadU32(ctx, state + 8u) + (slot << 4u);
    } else {
      switch (slot + kConvertValueToIntegerSpecialSlotBias) {
        case 0:
          slot = state + kConvertValueToIntegerConstantArea;
          break;
        case 1: {
          uint32_t const value = LoadU32(ctx, LoadU32(ctx, LoadU32(ctx, state + 20u) + 4u));
          slot = LoadU32(ctx, value + 12u);
          StoreU32(ctx, state + kConvertValueToIntegerBuiltArea, kConvertValueToIntegerPendingKind);
          StoreU32(ctx, state + kConvertValueToIntegerPendingArea, slot);
          slot = state + kConvertValueToIntegerPendingArea;
          break;
        }
        case 2:
          slot = LoadU32(ctx, state + 16u) + kConvertValueToIntegerBuiltArea;
          break;
        default: {
          // A slot below the shared records names a constant of the record the
          // state points at, as long as that record is large enough to hold it.
          uint32_t const value = LoadU32(ctx, LoadU32(ctx, LoadU32(ctx, state + 20u) + 4u));
          uint32_t const constant = value + ((0u - slot) << 4u) - kConvertValueToIntegerNegativeBase;
          uint32_t const room = kConvertValueToIntegerSpecialSlot - slot;
          slot = static_cast<int32_t>(LoadU8(ctx, value + kConvertValueToIntegerSizeByteOffset)) <
                         static_cast<int32_t>(room)
                     ? kNilValueAddr
                     : constant;
          break;
        }
      }
    }
    is_number = static_cast<uint32_t>(LoadU32(ctx, slot + 8u) == kConvertValueToIntegerNumberKind);
  }
  if (is_number) {
    goto number_ready;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == kConvertValueToIntegerMaterializeCall) {
    uint32_t const answer = MaterializeNumberValue(ctx, slot, scratch);
    resume_code = kConvertValueToIntegerMaterializeCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    slot = answer;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (slot != 0u) {
      // The value is a number now, so its double can be read below.
      goto number_ready;
    }
    // There is no value at all, so the answer is no integer either.
    slot = 0u;
    goto answer_ready;
  }
number_ready:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The number's own double is narrowed, and one at or past the limit
    // saturates to the smallest integer.
    double const number = LoadF64(ctx, slot);
    if (wasm_fabs(number) < kConvertValueToIntegerIntegerLimit) {
      slot = I32_TRUNC_S_F64(number);
    } else {
      slot = kConvertValueToIntegerSaturated;
    }
  }
answer_ready:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The scratch reserved on the guest stack goes back, and the answer is the
    // integer the value holds.
    GuestStackSp(ctx) = scratch + 16u;
    LeaveFrame();
    return slot;
  }
  TrapUnreachable();
suspend_point:;
  // Push the resume word naming the call that was in flight, then the frame the
  // rewind pops; the words are saved in the order that pop reads them.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kConvertValueToIntegerResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, slot);
  StoreU32(ctx, frame + 4u, scratch);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kConvertValueToIntegerSaveFrameBytes);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
