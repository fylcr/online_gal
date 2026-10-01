#include "util/engine_context.h"

namespace artc {

// Resolves the variable an operand stand for, through the record the chain is
// held in.
uint32_t ResolveVariableKind(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3);

// Raises the script error the text and its arguments make.
void RaiseScriptError(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

namespace {

// The texts this step raises, and the key name both of them carry.
constexpr uint32_t kFormatConcatenationErrorLongText = 0x2A718u;
constexpr uint32_t kFormatConcatenationErrorShortText = 0x2A73Bu;
constexpr uint32_t kFormatConcatenationErrorName = 0x2A754u;

// The table the type text is looked up in by the slot's own kind field.
constexpr uint32_t kFormatConcatenationErrorTypeTable = 0x2BD80u;

// The two kinds that stand at their own slot rather than being looked up in the
// chain.
constexpr uint32_t kFormatConcatenationErrorOperandKind = 3u;
constexpr uint32_t kFormatConcatenationErrorKindCount = 2u;

// The chain's entries: sixteen bytes each, and the record the two texts are
// built from is forty-eight bytes with its key field at the end.
constexpr uint32_t kFormatConcatenationErrorEntryBytes = 16u;
constexpr uint32_t kFormatConcatenationErrorRecordBytes = 48u;
constexpr uint32_t kFormatConcatenationErrorFieldOffset = 44u;

// The suspend frame: the words this body saves, and the resume word pushed with
// them, whose number names the call that was in flight.
constexpr uint32_t kFormatConcatenationErrorSaveFrameBytes = 16u;
constexpr uint32_t kFormatConcatenationErrorResumeWordBytes = 4u;
constexpr uint32_t kFormatConcatenationErrorLongCall = 0u;
constexpr uint32_t kFormatConcatenationErrorShortCall = 1u;

}  // namespace

// Renders the two operand types of a concatenation into the text the script
// error carries. The long text names both operands and the key that was looked
// up; the short one is raised when the chain holds no key for the operand.
void FormatConcatenationError(EngineContext* ctx, uint32_t state, uint32_t entry, uint32_t slot) {
  EnterFrame();
  // The suspend frame base: a rewind pops it and the suspend path writes its
  // four words into it.
  uint32_t frame = 0u;
  // The record the two texts are built from.
  uint32_t record = 0u;
  // The operand's own kind, and then the text the type table holds for the
  // slot's element kind.
  uint32_t type = 0u;
  // The record the operand chain lives in, the entry it ends at, and whether it
  // is already at that end.
  uint32_t chain = 0u;
  uint32_t chain_end = 0u;
  uint32_t at_end = 0u;
  // The call a rewinding pass re-issues, and the number a spilling pass saves.
  uint32_t resume_point = 0u;
  uint32_t resume_code = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 16-byte frame and read its words back in the saved
    // order.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kFormatConcatenationErrorSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    entry = LoadU32(ctx, frame + 4u);
    record = LoadU32(ctx, frame + 8u);
    type = LoadU32(ctx, frame + 12u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word pushed with that frame. It names the call
    // that was in flight, so only that call is issued again.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kFormatConcatenationErrorResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The record the two texts are built from lives on the guest stack, and its
    // own key field starts empty.
    GuestStackSp(ctx) -= kFormatConcatenationErrorRecordBytes;
    record = GuestStackSp(ctx);
    type = LoadU32(ctx, entry + 8u);
    StoreU32(ctx, record + kFormatConcatenationErrorFieldOffset, 0u);
    // One of the two operands stands at its own slot; the other is carried by
    // the entry the first one names.
    slot = type - kFormatConcatenationErrorOperandKind < kFormatConcatenationErrorKindCount ? slot : entry;
    type = LoadU32(ctx, kFormatConcatenationErrorTypeTable + (LoadU32(ctx, slot + 8u) << 2));
    chain = LoadU32(ctx, state + 20u);
    entry = LoadU32(ctx, chain);
    chain_end = LoadU32(ctx, chain + 8u);
    at_end = static_cast<uint32_t>(entry >= chain_end);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Walk the chain to the entry whose slot the operand stands at; the short
    // text is the one raised when the chain holds no key for it.
    if (at_end) {
      goto short_path;
    }
    while (entry != slot && chain_end > entry + kFormatConcatenationErrorEntryBytes) {
      entry += kFormatConcatenationErrorEntryBytes;
    }
    if (entry != slot) {
      goto short_path;
    }
    entry = ResolveVariableKind(ctx, state, chain,
                                static_cast<uint32_t>(static_cast<int32_t>(
                                    slot - LoadU32(ctx, state + 12u)) >> 4),
                                record + kFormatConcatenationErrorFieldOffset);
    if (entry == 0u) {
      goto short_path;
    }
    // The key's own text and the two operands are the long text's arguments.
    StoreU32(ctx, record + 16u, kFormatConcatenationErrorName);
    StoreU32(ctx, record + 20u, entry);
    StoreU32(ctx, record + 24u, LoadU32(ctx, record + kFormatConcatenationErrorFieldOffset));
    StoreU32(ctx, record + 28u, type);
    entry = record + 16u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == kFormatConcatenationErrorLongCall) {
    RaiseScriptError(ctx, state, kFormatConcatenationErrorLongText, entry);
    resume_code = kFormatConcatenationErrorLongCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The long text was raised, so the short one is not reached.
    goto done;
  }
short_path:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The short text's own arguments: the key name and the operand's type text.
    StoreU32(ctx, record + 4u, type);
    StoreU32(ctx, record, kFormatConcatenationErrorName);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == kFormatConcatenationErrorShortCall) {
    RaiseScriptError(ctx, state, kFormatConcatenationErrorShortText, record);
    resume_code = kFormatConcatenationErrorShortCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The record reserved on the guest stack goes back.
    GuestStackSp(ctx) = record + kFormatConcatenationErrorRecordBytes;
  }
  LeaveFrame();
  return;
suspend_point:;
  // Push the resume word naming the call that was in flight, then the frame the
  // rewind pops; the words are saved in the order that pop reads them.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kFormatConcatenationErrorResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, entry);
  StoreU32(ctx, frame + 8u, record);
  StoreU32(ctx, frame + 12u, type);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kFormatConcatenationErrorSaveFrameBytes);
  LeaveFrame();
  return;
}

}  // namespace artc
