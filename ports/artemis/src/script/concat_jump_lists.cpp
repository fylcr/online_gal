#include "util/engine_context.h"

namespace artc {

// Raises the parser's own syntax error with the text given.
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);

// Appends one instruction word to a function's code array.
uint32_t EmitInstruction(EngineContext* ctx, uint32_t func_state, uint32_t instruction, uint32_t line);

namespace {

// The head word this step appends to the code array.
constexpr uint32_t kConcatJumpListsHead = 0x7FFF8016u;

// One entry of a jump list holds a jump offset biased by its own maximum, and
// the offset field of a re-aimed entry starts at bit 14.
constexpr uint32_t kConcatJumpListsOffsetShift = 14u;
constexpr uint32_t kConcatJumpListsMaxJumpOffset = 0x1FFFFu;
constexpr uint32_t kConcatJumpListsJumpOffsetLimit = 0x20000u;
constexpr uint32_t kConcatJumpListsOffsetBiasBits = 0x7FFFC000u;

// The field a re-aimed entry keeps below the offset, and the all-ones word that
// ends a list.
constexpr uint32_t kConcatJumpListsArgumentMask = 0x3FFFu;
constexpr uint32_t kConcatJumpListsNoJump = 0xFFFFFFFFu;

// The text this body raises when the distance will not fit its field.
constexpr uint32_t kConcatJumpListsTooLongText = 0x2A394u;

// The suspend frame: the words this body saves, and the resume word pushed with
// them, whose number names the call that was in flight.
constexpr uint32_t kConcatJumpListsSaveFrameBytes = 36u;
constexpr uint32_t kConcatJumpListsResumeWordBytes = 4u;
constexpr uint32_t kConcatJumpListsEmitCall = 0u;
constexpr uint32_t kConcatJumpListsRaiseCall = 1u;

}  // namespace

// Splices one jump list onto the tail of another: the head word is appended to
// the code array, the list the function state held is walked to its last entry,
// and that entry is re-aimed at the new instruction. The answer is the pc the
// head word took, or the all-ones word when there was no list to walk.
uint32_t ConcatJumpLists(EngineContext* ctx, uint32_t state) {
  EnterFrame();
  // The suspend frame base: a rewind pops it and the suspend path writes its
  // nine words into it.
  uint32_t frame = 0u;
  // The line the head word is recorded at, then whether there was a list to
  // walk, then the entry the walk stands on and the distance it resolves. One
  // cell, because the suspend frame carries it in one word.
  uint32_t cursor = 0u;
  // The entry index the walk has reached, and then the sign word of the distance
  // and whether it is too wide for its field.
  uint32_t index = 0u;
  // The word the entry the walk stands on holds.
  uint32_t word = 0u;
  // The list the function state held, and the head index the new word took.
  uint32_t pending = 0u;
  uint32_t chain_head = 0u;
  // The code array the entries are held in, and then the address of the entry
  // the walk ends at; and that same address while the entry is re-aimed.
  uint32_t array = 0u;
  uint32_t entry = 0u;
  // Whether the distance the walk resolved is too wide for its field.
  uint32_t overflow = 0u;
  // The call a rewinding pass re-issues, and the number a spilling pass saves.
  uint32_t resume_point = 0u;
  uint32_t resume_code = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 36-byte frame and read its words back in the saved
    // order.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kConcatJumpListsSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    index = LoadU32(ctx, frame + 8u);
    word = LoadU32(ctx, frame + 12u);
    pending = LoadU32(ctx, frame + 16u);
    chain_head = LoadU32(ctx, frame + 20u);
    array = LoadU32(ctx, frame + 24u);
    entry = LoadU32(ctx, frame + 28u);
    overflow = LoadU32(ctx, frame + 32u);
    cursor = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word pushed with that frame. It names the call
    // that was in flight, so only that call is issued again.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kConcatJumpListsResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The list the state held is taken over and the state's own head is cleared;
    // the line the new word is recorded at comes from the code owner.
    pending = LoadU32(ctx, state + 32u);
    StoreU32(ctx, state + 32u, kConcatJumpListsNoJump);
    cursor = LoadU32(ctx, LoadU32(ctx, state + 12u) + 8u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == kConcatJumpListsEmitCall) {
    uint32_t const answer = EmitInstruction(ctx, state, kConcatJumpListsHead, cursor);
    resume_code = kConcatJumpListsEmitCall;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    chain_head = answer;
  }
  // The walk is owed when the state held a list, and a rewinding pass owes it
  // because it has to reach the call it suspended in.
  cursor = ResumePhase(ctx) ? cursor : static_cast<uint32_t>(pending != kConcatJumpListsNoJump);
  cursor |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (cursor) {
    if (ResumePhase(ctx) == kResumeRunning) {
      if (chain_head == kConcatJumpListsNoJump) {
        // There is no list to walk, so that word is the answer.
        LeaveFrame();
        return pending;
      }
      array = LoadU32(ctx, LoadU32(ctx, state) + 12u);
      // Walk to the last entry: each one carries the index of the entry that
      // follows it, and the all-ones word ends the list.
      cursor = chain_head;
      do {
        index = cursor;
        entry = array + (cursor << 2);
        word = LoadU32(ctx, entry);
        cursor = (word >> kConcatJumpListsOffsetShift) - kConcatJumpListsMaxJumpOffset;
        cursor = cursor == kConcatJumpListsNoJump ? kConcatJumpListsNoJump : index + cursor + 1u;
      } while (cursor != kConcatJumpListsNoJump);
      array = entry;
      // The distance from the last entry back to the head word. Its absolute
      // value is taken without branching, and the far end of the offset range is
      // what the running test compares to.
      cursor = pending + (index ^ 0xFFFFFFFFu);
      index = static_cast<uint32_t>(static_cast<int32_t>(cursor) >> 31);
      overflow = static_cast<uint32_t>(static_cast<int32_t>(index ^ (cursor + index)) >=
                                       static_cast<int32_t>(kConcatJumpListsJumpOffsetLimit));
    }
    if (static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding) | overflow) {
      if (ResumePhase(ctx) == kResumeRunning) {
        // The raise names the code owner the entries are held for.
        state = LoadU32(ctx, state + 12u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == kConcatJumpListsRaiseCall) {
        RaiseSyntaxError(ctx, state, kConcatJumpListsTooLongText);
        resume_code = kConcatJumpListsRaiseCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        // The entry's word is read back, so the re-aim below works from what the
        // array now holds.
        word = LoadU32(ctx, entry);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The entry keeps its own arguments and takes the distance in the field
      // above them, biased by the field's own maximum.
      StoreU32(ctx, array, (word & kConcatJumpListsArgumentMask) |
                               (cursor << kConcatJumpListsOffsetShift) +
                                   kConcatJumpListsOffsetBiasBits);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The list was re-aimed and the new word took this pc.
    LeaveFrame();
    return chain_head;
  }
  TrapUnreachable();
suspend_point:;
  // Push the resume word naming the call that was in flight, then the frame the
  // rewind pops; the words are saved in the order that pop reads them.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kConcatJumpListsResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, cursor);
  StoreU32(ctx, frame + 8u, index);
  StoreU32(ctx, frame + 12u, word);
  StoreU32(ctx, frame + 16u, pending);
  StoreU32(ctx, frame + 20u, chain_head);
  StoreU32(ctx, frame + 24u, array);
  StoreU32(ctx, frame + 28u, entry);
  StoreU32(ctx, frame + 32u, overflow);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kConcatJumpListsSaveFrameBytes);
  LeaveFrame();
  return 0u;
}

}  // namespace artc
