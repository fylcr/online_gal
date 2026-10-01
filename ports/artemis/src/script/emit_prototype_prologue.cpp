#include "util/engine_context.h"

namespace artc {

// Appends one instruction word to a function's code array.
uint32_t EmitInstruction(EngineContext* ctx, uint32_t func_state, uint32_t instruction, uint32_t line);

namespace {

// The flag word that means the prototype takes no flags.
constexpr uint32_t kEmitPrototypePrologueNoFlags = 0xFFFFFFFFu;

// Where each field of a prologue word starts: the flags, the upvalue count and
// the parameter words, below the opcode.
constexpr uint32_t kEmitPrototypePrologueFlagShift = 23u;
constexpr uint32_t kEmitPrototypePrologueUpvalueShift = 6u;
constexpr uint32_t kEmitPrototypePrologueWordsShift = 14u;
constexpr uint32_t kEmitPrototypePrologueOpcode = 34u;

// The parameter count lives in one field as a number of full words; the count
// itself may not run past what the fields can name.
constexpr uint32_t kEmitPrototypePrologueParametersPerWord = 50u;
constexpr uint32_t kEmitPrototypePrologueMaxParameterCount = 25550u;

// The suspend frame: the words this body saves, and the resume word pushed with
// them, whose number names the word that was in flight.
constexpr uint32_t kEmitPrototypePrologueSaveFrameBytes = 24u;
constexpr uint32_t kEmitPrototypePrologueResumeWordBytes = 4u;
constexpr uint32_t kEmitPrototypePrologueFirstWordCall = 0u;
constexpr uint32_t kEmitPrototypePrologueSecondWordCall = 1u;
constexpr uint32_t kEmitPrototypePrologueParameterCall = 2u;

}  // namespace

// Emits a prototype prologue: the first word reserves the function's registers
// and lays out the parameters, the second carries the same fields without them
// and the third carries the parameter words alone. A parameter count the operand
// fields cannot hold is reported as a flag rather than as a count.
void EmitPrototypePrologue(EngineContext* ctx, uint32_t func_state, uint32_t upvalue_count,
                           uint32_t parameter_count, uint32_t flag_word) {
  EnterFrame();
  // The suspend frame base: a rewind pops it and the suspend path writes its six
  // words into it.
  uint32_t frame = 0u;
  // The word being built; once the word is emitted the same cell carries the pc
  // it took and then the line the next word goes at.
  uint32_t word = 0u;
  // The upvalue count moved into its field, and the number of words the
  // parameters need.
  uint32_t upvalue_bits = 0u;
  uint32_t parameter_words = 0u;
  // The guarded call a rewinding pass re-issues, and the number a spilling pass
  // saves.
  uint32_t resume_code = 0u;
  // Whether the parameter count fits the fields at all.
  uint32_t parameters_fit = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 24-byte frame and read its words back in the saved
    // order.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kEmitPrototypePrologueSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    func_state = LoadU32(ctx, frame);
    word = LoadU32(ctx, frame + 8u);
    flag_word = LoadU32(ctx, frame + 12u);
    upvalue_bits = LoadU32(ctx, frame + 16u);
    parameter_words = LoadU32(ctx, frame + 20u);
    upvalue_count = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word pushed with that frame. It names the word
    // that was in flight, so only that word is emitted again.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kEmitPrototypePrologueResumeWordBytes);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The flags move up to their own field — all ones means none — and the
    // parameter count becomes the number of words it needs, or the flag that
    // says it fits the fields at all.
    flag_word = flag_word == kEmitPrototypePrologueNoFlags ? 0u : flag_word << kEmitPrototypePrologueFlagShift;
    upvalue_bits = upvalue_count << kEmitPrototypePrologueUpvalueShift;
    parameter_words = I32_DIV_S(parameter_count - 1u, kEmitPrototypePrologueParametersPerWord) + 1u;
    parameters_fit = static_cast<uint32_t>(parameter_count <= kEmitPrototypePrologueMaxParameterCount);
  }
  // A running pass owes the whole prologue, a rewinding one owes only the words
  // it has not replayed yet; the two are merged into the one gate here.
  parameters_fit |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (parameters_fit) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The first word: the flags, the upvalue field, the parameter words' field
      // and the opcode, with the line the function's line record holds.
      word = flag_word | upvalue_bits | (parameter_words << kEmitPrototypePrologueWordsShift) |
             kEmitPrototypePrologueOpcode;
      flag_word = LoadU32(ctx, LoadU32(ctx, func_state + 12u) + 8u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kEmitPrototypePrologueFirstWordCall) {
      uint32_t const answer = EmitInstruction(ctx, func_state, word, flag_word);
      resume_code = kEmitPrototypePrologueFirstWordCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      word = answer;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The running pass has no earlier word left to replay.
      goto second_half;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The second word: the same fields, this time without the parameter words.
    word = flag_word | upvalue_bits | kEmitPrototypePrologueOpcode;
    flag_word = LoadU32(ctx, LoadU32(ctx, func_state + 12u) + 8u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kEmitPrototypePrologueSecondWordCall) {
    uint32_t const answer = EmitInstruction(ctx, func_state, word, flag_word);
    resume_code = kEmitPrototypePrologueSecondWordCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    word = answer;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    flag_word = LoadU32(ctx, LoadU32(ctx, func_state + 12u) + 8u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kEmitPrototypePrologueParameterCall) {
    EmitInstruction(ctx, func_state, parameter_words, flag_word);
    resume_code = kEmitPrototypePrologueParameterCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
second_half:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The prologue leaves the function one register past its upvalue count.
    StoreU32(ctx, func_state + 36u, upvalue_count + 1u);
  }
  // Nothing is in flight here, so no frame is saved.
  goto done;
suspend_point:;
  // Push the resume word naming the word that was in flight, then the frame the
  // rewind pops; the words are saved in the order that pop reads them.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kEmitPrototypePrologueResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, func_state);
  StoreU32(ctx, frame + 4u, upvalue_count);
  StoreU32(ctx, frame + 8u, word);
  StoreU32(ctx, frame + 12u, flag_word);
  StoreU32(ctx, frame + 16u, upvalue_bits);
  StoreU32(ctx, frame + 20u, parameter_words);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kEmitPrototypePrologueSaveFrameBytes);
done:;
  LeaveFrame();
}

}  // namespace artc
