#include "util/engine_context.h"

namespace artc {

// Raises the parser's own syntax error with the text given.
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);

namespace {

// One entry of the pending list holds a jump offset biased by its own maximum,
// and the offset field of a re-aimed entry starts at bit 14.
constexpr uint32_t kFixPendingJumpTargetsOffsetShift = 14u;
constexpr uint32_t kFixPendingJumpTargetsMaxJumpOffset = 0x1FFFFu;
constexpr uint32_t kFixPendingJumpTargetsJumpOffsetLimit = 0x20000u;
constexpr uint32_t kFixPendingJumpTargetsOffsetBiasBits = 0x7FFFC000u;

// The field a re-aimed entry keeps below the offset, and the all-ones word that
// means there is no entry and no offset at all.
constexpr uint32_t kFixPendingJumpTargetsArgumentMask = 0x3FFFu;
constexpr uint32_t kFixPendingJumpTargetsNoJump = 0xFFFFFFFFu;

// The text this body raises when the distance will not fit its field.
constexpr uint32_t kFixPendingJumpTargetsTooLongText = 0x2A394u;

// The suspend frame: the words this body saves, and the resume word pushed with
// them, whose number names the call that was in flight.
constexpr uint32_t kFixPendingJumpTargetsSaveFrameBytes = 24u;
constexpr uint32_t kFixPendingJumpTargetsResumeWordBytes = 4u;
constexpr uint32_t kFixPendingJumpTargetsRaiseCall = 0u;

}  // namespace

// Re-aims one instruction's pending jumps at the pc given. The list of pending
// entries is walked to its last entry, that entry is re-encoded with the
// distance to the pc, and a list that is still empty is seeded with one entry. A
// distance too wide for the offset field raises "control structure too long".
void FixPendingJumpTargets(EngineContext* ctx, uint32_t state, uint32_t head, uint32_t target) {
  EnterFrame();
  // The suspend frame base: a rewind pops it and the suspend path writes its six
  // words into it.
  uint32_t frame = 0u;
  // The entry the walk stands on: whether there is a list at all, then the word
  // it holds, and at the end the address of the entry to re-aim. One cell,
  // because the suspend frame carries it in one word.
  uint32_t cursor = 0u;
  // The address of the entry the walk ends at, and the word that entry holds.
  uint32_t entry = 0u;
  uint32_t word = 0u;
  // The code array the entries are held in, and whether the distance the walk
  // resolved is too wide for its field.
  uint32_t code_base = 0u;
  uint32_t overflow = 0u;
  // The call a rewinding pass re-issues, and the number a spilling pass saves.
  uint32_t resume_point = 0u;
  uint32_t resume_code = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 24-byte frame and read its words back in the saved
    // order.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kFixPendingJumpTargetsSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    head = LoadU32(ctx, frame + 4u);
    target = LoadU32(ctx, frame + 8u);
    cursor = LoadU32(ctx, frame + 12u);
    word = LoadU32(ctx, frame + 16u);
    entry = LoadU32(ctx, frame + 20u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume word pushed with that frame. It names the call
    // that was in flight, so only that call is issued again.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kFixPendingJumpTargetsResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  // The walk is owed when there is a list to re-aim, and a rewinding pass owes it
  // because it has to reach the call it suspended in.
  cursor = ResumePhase(ctx) ? cursor : static_cast<uint32_t>(target != kFixPendingJumpTargetsNoJump);
  cursor |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (cursor) {
    if (ResumePhase(ctx) == kResumeRunning) {
      cursor = LoadU32(ctx, head);
      if (cursor == kFixPendingJumpTargetsNoJump) {
        // No pending jump at all, so the list is seeded further down.
        goto seed_entry;
      }
      code_base = LoadU32(ctx, LoadU32(ctx, state) + 12u);
      // Walk to the last entry: each one carries the entry that follows it, and
      // the all-ones word ends the list.
      do {
        head = cursor;
        entry = code_base + (cursor << 2);
        word = LoadU32(ctx, entry);
        cursor = (word >> kFixPendingJumpTargetsOffsetShift) - kFixPendingJumpTargetsMaxJumpOffset;
        cursor = cursor == kFixPendingJumpTargetsNoJump ? kFixPendingJumpTargetsNoJump
                                                       : head + cursor + 1u;
      } while (cursor != kFixPendingJumpTargetsNoJump);
      cursor = entry;
      // The distance from the last entry back to the pc. Its absolute value is
      // taken without branching, and the far end of the offset range is what the
      // running test compares to.
      head = target + (head ^ 0xFFFFFFFFu);
      target = static_cast<uint32_t>(static_cast<int32_t>(head) >> 31);
      overflow = static_cast<uint32_t>(static_cast<int32_t>(target ^ (head + target)) >=
                                       static_cast<int32_t>(kFixPendingJumpTargetsJumpOffsetLimit));
    }
    if (static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding) | overflow) {
      if (ResumePhase(ctx) == kResumeRunning) {
        // The raise names the code owner the entries are held for.
        state = LoadU32(ctx, state + 12u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == kFixPendingJumpTargetsRaiseCall) {
        RaiseSyntaxError(ctx, state, kFixPendingJumpTargetsTooLongText);
        resume_code = kFixPendingJumpTargetsRaiseCall;
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
      // The distance takes the field above the arguments, biased by the field's
      // own maximum, and the entry keeps its own arguments.
      head = head << kFixPendingJumpTargetsOffsetShift;
      StoreU32(ctx, cursor, (head | (word & kFixPendingJumpTargetsArgumentMask)) +
                                kFixPendingJumpTargetsOffsetBiasBits);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The list was re-aimed, so nothing is left to seed.
    goto done;
  }
seed_entry:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The pc itself becomes the list's only pending entry.
    StoreU32(ctx, head, target);
  }
done:;
  LeaveFrame();
  return;
suspend_point:;
  // Push the resume word naming the call that was in flight, then the frame the
  // rewind pops; the words are saved in the order that pop reads them.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kFixPendingJumpTargetsResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, head);
  StoreU32(ctx, frame + 8u, target);
  StoreU32(ctx, frame + 12u, cursor);
  StoreU32(ctx, frame + 16u, word);
  StoreU32(ctx, frame + 20u, entry);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kFixPendingJumpTargetsSaveFrameBytes);
  LeaveFrame();
  return;
}

}  // namespace artc
