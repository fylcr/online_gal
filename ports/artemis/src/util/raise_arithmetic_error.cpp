#include "util/engine_context.h"

namespace artc {

// Cross-product callees declared here: the script error raiser, the value
// materializer and the variable resolver.
void RaiseScriptError(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
uint32_t MaterializeNumberValue(EngineContext* ctx, uint32_t state, uint32_t slot);
uint32_t ResolveVariableKind(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2,
                             uint32_t a3);

namespace {

// "attempt to %s %s '%s' (a %s value)": the form that names the variable.
constexpr uint32_t kRaiseArithmeticErrorFormatWithName = 0x2A718u;
// "attempt to %s a %s value": the form used when no variable name is found.
constexpr uint32_t kRaiseArithmeticErrorFormatNoName = 0x2A73Bu;
// The operation text both formats start with: "perform arithmetic on".
constexpr uint32_t kRaiseArithmeticErrorOperation = 0x2A760u;
// Table of type-name pointers, indexed by the operand's type tag.
constexpr uint32_t kRaiseArithmeticErrorTypeNameTable = 0x2BD80u;
// The negation of the 64-byte scratch frame: adding it claims the frame off
// the guest stack, subtracting it gives the frame back.
constexpr uint32_t kRaiseArithmeticErrorScratchFrameNeg = 0xFFFFFFC0u;
constexpr uint32_t kRaiseArithmeticErrorSaveFrameBytes = 32u;
constexpr uint32_t kRaiseArithmeticErrorResumeWordBytes = 4u;

}  // namespace

// Reports arithmetic on an operand that is not a number: it materializes the
// operand, finds the variable name it came from when one is on the stack, and
// hands the finished message to RaiseScriptError. Answers nothing.
void RaiseArithmeticError(EngineContext* ctx, uint32_t state, uint32_t value, uint32_t meta) {
  EnterFrame();
  uint32_t scratch = 0u;
  uint32_t slot = 0u;
  uint32_t records = 0u;
  uint32_t scan_at_limit = 0u;
  uint32_t limit = 0u;
  uint32_t resume_word = 0u;
  uint32_t resume_code = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the saved frame and read its words back in the saved order.
    uint32_t frame_sp = ReturnFrameSp(ctx);
    uint32_t frame_top = ReturnFrameSp(ctx);
    frame_top = LoadU32(ctx, frame_top) - kRaiseArithmeticErrorSaveFrameBytes;
    StoreU32(ctx, frame_sp, frame_top);
    uint32_t frame = ReturnFrameSp(ctx);
    frame = LoadU32(ctx, frame);
    state = LoadU32(ctx, frame);
    meta = LoadU32(ctx, frame + 8u);
    scratch = LoadU32(ctx, frame + 12u);
    slot = LoadU32(ctx, frame + 16u);
    records = LoadU32(ctx, frame + 20u);
    scan_at_limit = LoadU32(ctx, frame + 24u);
    limit = LoadU32(ctx, frame + 28u);
    value = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word that names the call that was in flight.
    uint32_t word_sp = ReturnFrameSp(ctx);
    uint32_t word_top = ReturnFrameSp(ctx);
    word_top = LoadU32(ctx, word_top) - kRaiseArithmeticErrorResumeWordBytes;
    StoreU32(ctx, word_sp, word_top);
    uint32_t word = ReturnFrameSp(ctx);
    word = LoadU32(ctx, word);
    resume_word = LoadU32(ctx, word);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // A running pass claims the 64-byte scratch frame off the guest stack.
    scratch = GuestStackSp(ctx) + kRaiseArithmeticErrorScratchFrameNeg;
    GuestStackSp(ctx) = scratch;
    slot = scratch + 40u;
  }
  uint32_t phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_word == 0u) {
    uint32_t materialized = MaterializeNumberValue(ctx, value, slot);
    resume_code = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
    slot = materialized;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, scratch + 60u, 0u);
    // The object whose type name the message prints: the operand that is not a
    // number, falling back to `value` when the slot already holds one.
    if (slot == 0u) {
      meta = value;
    }
    // The type tag picks a pointer out of the name table, four bytes per entry.
    uint32_t type_ofs = LoadU32(ctx, meta + 8u);
    type_ofs <<= (2u & 31u);
    slot = LoadU32(ctx, kRaiseArithmeticErrorTypeNameTable + type_ofs);
    records = LoadU32(ctx, state + 20u);
    value = LoadU32(ctx, records);
    limit = LoadU32(ctx, records + 8u);
    scan_at_limit = value >= limit;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (scan_at_limit != 0u) {
      goto scan_done;
    }
    // Walk the 16-byte stack records up to the operand's own boundary.
    while (value != meta) {
      value = value + 16u;
      if (limit > value) {
        continue;
      }
      goto scan_done;
    }
    // Signed shift: the distance from the record base is scaled down to a
    // record index, so a distance below the base stays negative here.
    uint32_t record_index =
        static_cast<uint32_t>(static_cast<int32_t>(meta - LoadU32(ctx, state + 12u)) >> (4u & 31u));
    value = ResolveVariableKind(ctx, state, records, record_index, scratch + 60u);
    if (value == 0u) {
      goto scan_done;
    }
    // Build the message frame the named form reads: operation, name, kind, text.
    uint32_t operation = kRaiseArithmeticErrorOperation;
    StoreU32(ctx, scratch + 16u, operation);
    StoreU32(ctx, scratch + 20u, value);
    uint32_t kind_name = LoadU32(ctx, scratch + 60u);
    StoreU32(ctx, scratch + 24u, kind_name);
    StoreU32(ctx, scratch + 28u, slot);
    value = scratch + 16u;
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_word == kResumeSuspended) {
    RaiseScriptError(ctx, state, kRaiseArithmeticErrorFormatWithName, value);
    resume_code = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto frame_released;
  }
scan_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Build the unnamed message frame the short form reads: text, then the
    // operation; the writes go up from the frame base.
    StoreU32(ctx, scratch + 4u, slot);
    uint32_t operation = kRaiseArithmeticErrorOperation;
    StoreU32(ctx, scratch, operation);
  }
  phase = ResumePhase(ctx);
  if (phase == kResumeRunning || resume_word == kResumeRewinding) {
    RaiseScriptError(ctx, state, kRaiseArithmeticErrorFormatNoName, scratch);
    resume_code = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_here;
    }
  }
frame_released:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Give the scratch frame back before leaving.
    GuestStackSp(ctx) = scratch - kRaiseArithmeticErrorScratchFrameNeg;
  }
  LeaveFrame();
  return;
suspend_here:;
  // Push the resume word that names the call in flight, then the saved frame;
  // both go down in the order that the pop reads them back.
  uint32_t word_slot = ReturnFrameSp(ctx);
  word_slot = LoadU32(ctx, word_slot);
  StoreU32(ctx, word_slot, resume_code);
  uint32_t sp = ReturnFrameSp(ctx);
  uint32_t word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + kRaiseArithmeticErrorResumeWordBytes;
  StoreU32(ctx, sp, word_top);
  uint32_t frame = ReturnFrameSp(ctx);
  frame = LoadU32(ctx, frame);
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, value);
  StoreU32(ctx, frame + 8u, meta);
  StoreU32(ctx, frame + 12u, scratch);
  StoreU32(ctx, frame + 16u, slot);
  StoreU32(ctx, frame + 20u, records);
  StoreU32(ctx, frame + 24u, scan_at_limit);
  StoreU32(ctx, frame + 28u, limit);
  sp = ReturnFrameSp(ctx);
  word_top = ReturnFrameSp(ctx);
  word_top = LoadU32(ctx, word_top) + kRaiseArithmeticErrorSaveFrameBytes;
  StoreU32(ctx, sp, word_top);
  LeaveFrame();
}

}  // namespace artc
