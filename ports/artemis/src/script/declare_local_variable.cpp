#include "util/engine_context.h"

namespace artc {

// Builds one of the parser's diagnostic texts in the guest's string pool, and
// answers with the string it stands for.
uint32_t FormatPushString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
// Records an error against the lexer state.
void LexError(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
// Notes one entry in the parser's list of named locals.
void StoreListEntry(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
// Grows one of the parser's arrays, answering with the block it moved to.
uint32_t GrowArray(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f);

namespace {

// The function record the state names, and the fields this step reads out of it
// or out of the prototype it belongs to.
constexpr uint32_t kDeclareLocalVariableFunctionOffset = 48u;
constexpr uint32_t kDeclareLocalVariableCountOffset = 48u;
constexpr uint32_t kDeclareLocalVariableAdditionOffset = 50u;
constexpr uint32_t kDeclareLocalVariableElementsOffset = 52u;
constexpr uint32_t kDeclareLocalVariableArrayOffset = 24u;
constexpr uint32_t kDeclareLocalVariableCapacityOffset = 56u;
constexpr uint32_t kDeclareLocalVariableParentOffset = 60u;
constexpr uint32_t kDeclareLocalVariableHomeOffset = 16u;
constexpr uint32_t kDeclareLocalVariableNamesOffset = 12u;
constexpr uint32_t kDeclareLocalVariableSlotMapOffset = 172u;
constexpr uint32_t kDeclareLocalVariableFlagOffset = 5u;

// How many local slots a function may declare, and the two flags a name record
// has to carry before the local is noted down as named.
constexpr uint32_t kDeclareLocalVariableSlotLimit = 200u;
constexpr uint32_t kDeclareLocalVariableNameFlags = 3u;
constexpr uint32_t kDeclareLocalVariableOwnerFlag = 4u;

// One slot of the locals array takes twelve bytes, and the array never grows
// past the parser's own maximum.
constexpr uint32_t kDeclareLocalVariableSlotSize = 12u;
constexpr uint32_t kDeclareLocalVariableArrayLimit = 32767u;

// The four texts this body reports with.
constexpr uint32_t kDeclareLocalVariableMainText = 0x2B6C0u;  // main function has more than %d %s
constexpr uint32_t kDeclareLocalVariableLineText = 0x2B6E2u;  // function at line %d has more than %d %s
constexpr uint32_t kDeclareLocalVariableLocalsText = 0x2B70Fu;  // local variables
constexpr uint32_t kDeclareLocalVariableTooManyText = 0x2B71Fu;  // too many local variables

// The suspend frame: the words this body saves, the resume word pushed with
// them, and the numbers that name the call a frame suspended in.
constexpr uint32_t kDeclareLocalVariableSaveFrameBytes = 44u;
constexpr uint32_t kDeclareLocalVariableResumeWordBytes = 4u;
constexpr uint32_t kDeclareLocalVariableMainTextCall = 0u;
constexpr uint32_t kDeclareLocalVariableLineTextCall = 1u;
constexpr uint32_t kDeclareLocalVariableErrorCall = 2u;
constexpr uint32_t kDeclareLocalVariableGrowCall = 3u;

}  // namespace

// Reserves one local slot for the function the state names, records the name
// against it, and reports through the parser's own texts when the function has
// more locals than a function may declare.
void DeclareLocalVariable(EngineContext* ctx, uint32_t state, uint32_t name, uint32_t slot) {
  EnterFrame();
  // The suspend frame base: a rewind pops it and the suspend path writes its
  // eleven words into it.
  uint32_t frame = 0u;
  // What a built string or a grown array answered with.
  uint32_t call_answer = 0u;
  // The call a rewinding pass re-issues, and the number a spilling pass saves.
  uint32_t resume_point = 0u;
  uint32_t resume_code = 0u;
  // The count of locals already declared, then the array of slots, then the
  // entry the loop below walks: one cell, because the suspend frame carries it
  // in one word.
  uint32_t cursor = 0u;
  // The function's record and the prototype above it.
  uint32_t func = 0u;
  uint32_t proto = 0u;
  // The array of local slots, and how many entries it holds.
  uint32_t array = 0u;
  uint32_t entries = 0u;
  // The address of the array's capacity word.
  uint32_t capacity_addr = 0u;
  // The record one of the diagnostic texts is built from, then whether the
  // second text is owed at all.
  uint32_t work = 0u;
  uint32_t second_report = 0u;
  // Whether this call has more locals than the limit allows.
  uint32_t too_many = 0u;
  // The thirty-two bytes of argument record the texts are built from, reserved
  // on the guest stack.
  uint32_t arg_record = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 44-byte frame and read its words back in the saved
    // order.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kDeclareLocalVariableSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    slot = LoadU32(ctx, frame + 8u);
    cursor = LoadU32(ctx, frame + 12u);
    work = LoadU32(ctx, frame + 16u);
    array = LoadU32(ctx, frame + 20u);
    proto = LoadU32(ctx, frame + 24u);
    arg_record = LoadU32(ctx, frame + 28u);
    func = LoadU32(ctx, frame + 32u);
    entries = LoadU32(ctx, frame + 36u);
    capacity_addr = LoadU32(ctx, frame + 40u);
    name = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word pushed with that frame. It names the call
    // that was in flight, so only that call is issued again.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kDeclareLocalVariableResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The argument record for the texts lives on the guest stack, and the
    // function's own record says whether one more local still fits.
    arg_record = GuestStackSp(ctx) - 32u;
    GuestStackSp(ctx) = arg_record;
    func = LoadU32(ctx, state + kDeclareLocalVariableFunctionOffset);
    work = func;
    too_many = static_cast<uint32_t>(static_cast<int32_t>(
                   LoadU8(ctx, func + kDeclareLocalVariableAdditionOffset) + slot) >=
               static_cast<int32_t>(kDeclareLocalVariableSlotLimit));
  }
  if (static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding) | too_many) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The prototype records whether the function is still inside its own body,
      // and the record the text is built from.
      proto = LoadU32(ctx, LoadU32(ctx, func) + kDeclareLocalVariableParentOffset);
      second_report = static_cast<uint32_t>(proto == 0u);
      cursor = LoadU32(ctx, func + kDeclareLocalVariableHomeOffset);
    }
    if (static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding) | second_report) {
      if (ResumePhase(ctx) == kResumeRunning) {
        // The main body's own report: "main function has more than %d %s".
        StoreU32(ctx, arg_record + 4u, kDeclareLocalVariableLocalsText);
        StoreU32(ctx, arg_record, kDeclareLocalVariableSlotLimit);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == kDeclareLocalVariableMainTextCall) {
        call_answer = FormatPushString(ctx, cursor, kDeclareLocalVariableMainText, arg_record);
        resume_code = kDeclareLocalVariableMainTextCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        cursor = call_answer;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        goto second_report_done;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // A nested body's report: "function at line %d has more than %d %s".
      StoreU32(ctx, arg_record + 24u, kDeclareLocalVariableLocalsText);
      StoreU32(ctx, arg_record + 20u, kDeclareLocalVariableSlotLimit);
      StoreU32(ctx, arg_record + 16u, proto);
      work = arg_record + 16u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == kDeclareLocalVariableLineTextCall) {
      call_answer = FormatPushString(ctx, cursor, kDeclareLocalVariableLineText, work);
      resume_code = kDeclareLocalVariableLineTextCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      cursor = call_answer;
    }
  second_report_done:;
    if (ResumePhase(ctx) == kResumeRunning) {
      work = LoadU32(ctx, func + kDeclareLocalVariableNamesOffset);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == kDeclareLocalVariableErrorCall) {
      LexError(ctx, work, cursor, 0u);
      resume_code = kDeclareLocalVariableErrorCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    // A running pass takes the record the state holds; a rewinding pass keeps
    // the one the frame restored.
    work = ResumePhase(ctx) ? work : LoadU32(ctx, state + kDeclareLocalVariableFunctionOffset);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The locals array the prototype owns, and whether it already holds more
    // entries than the function has declared.
    proto = LoadU32(ctx, work);
    cursor = LoadU32(ctx, proto + kDeclareLocalVariableCapacityOffset);
    too_many = static_cast<uint32_t>(static_cast<int32_t>(cursor) >
                                     static_cast<int32_t>(LoadS16(ctx, work + kDeclareLocalVariableCountOffset)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (too_many) {
      // The array already has room for one more slot.
      array = LoadU32(ctx, proto + kDeclareLocalVariableArrayOffset);
      goto array_ready;
    }
    // No room: the parser's array grower takes the old block, the address of its
    // capacity word, the record the entries are held for, and the slot size.
    entries = LoadU32(ctx, proto + kDeclareLocalVariableArrayOffset);
    capacity_addr = proto + kDeclareLocalVariableCapacityOffset;
    array = LoadU32(ctx, state + kDeclareLocalVariableElementsOffset);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == kDeclareLocalVariableGrowCall) {
    call_answer = GrowArray(ctx, array, entries, capacity_addr, kDeclareLocalVariableSlotSize,
                            kDeclareLocalVariableArrayLimit, kDeclareLocalVariableTooManyText);
    resume_code = kDeclareLocalVariableGrowCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    array = call_answer;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The grown block takes the old one's place, and every slot the growth added
    // is cleared before the new local goes in.
    StoreU32(ctx, proto + kDeclareLocalVariableArrayOffset, array);
    entries = LoadU32(ctx, proto + kDeclareLocalVariableCapacityOffset);
    if (static_cast<int32_t>(entries) <= static_cast<int32_t>(cursor)) {
      goto array_ready;
    }
    do {
      StoreU32(ctx, cursor * kDeclareLocalVariableSlotSize + array, 0u);
      cursor += 1u;
    } while (entries != cursor);
  }
array_ready:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The new slot takes the name, and a name whose flags say so is noted down
    // in the parser's list of named locals as well.
    cursor = LoadS16(ctx, work + kDeclareLocalVariableCountOffset);
    StoreU32(ctx, cursor * kDeclareLocalVariableSlotSize + array, name);
    if ((LoadU8(ctx, name + kDeclareLocalVariableFlagOffset) & kDeclareLocalVariableNameFlags) != 0u &&
        (LoadU8(ctx, proto + kDeclareLocalVariableFlagOffset) & kDeclareLocalVariableOwnerFlag) != 0u) {
      StoreListEntry(ctx, LoadU32(ctx, state + kDeclareLocalVariableElementsOffset), proto, name);
      cursor = LoadU16(ctx, work + kDeclareLocalVariableCountOffset);
    }
    // One more local is declared, and the slot map takes the name's number.
    StoreU16(ctx, work + kDeclareLocalVariableCountOffset, cursor + 1u);
    StoreU16(ctx, func + ((slot + LoadU8(ctx, func + kDeclareLocalVariableAdditionOffset)) << 1u) +
                          kDeclareLocalVariableSlotMapOffset,
             cursor);
    GuestStackSp(ctx) = arg_record + 32u;
  }
  LeaveFrame();
  return;
suspend_point:;
  // Push the resume word naming the call that was in flight, then the frame the
  // rewind pops; the words are saved in the order that pop reads them.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kDeclareLocalVariableResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, name);
  StoreU32(ctx, frame + 8u, slot);
  StoreU32(ctx, frame + 12u, cursor);
  StoreU32(ctx, frame + 16u, work);
  StoreU32(ctx, frame + 20u, array);
  StoreU32(ctx, frame + 24u, proto);
  StoreU32(ctx, frame + 28u, arg_record);
  StoreU32(ctx, frame + 32u, func);
  StoreU32(ctx, frame + 36u, entries);
  StoreU32(ctx, frame + 40u, capacity_addr);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kDeclareLocalVariableSaveFrameBytes);
  LeaveFrame();
}

}  // namespace artc
