#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);

namespace {

// A pending comparison's chain entry is an instruction word whose offset field
// counts backwards to the entry before it, biased by that field's own maximum;
// the all-ones word ends the chain and the two opcodes below are the marker a
// pending comparison left behind and the jump it becomes once the comparison is
// folded in.
constexpr uint32_t kPatchComparisonNoJump = 0xFFFFFFFFu;
constexpr uint32_t kPatchComparisonMarkerOpcode = 27u;
constexpr uint32_t kPatchComparisonJumpOpcode = 26u;
constexpr uint32_t kPatchComparisonOpcodeMask = 63u;

// The offset field starts at bit 14 and is biased by its maximum, so a distance
// that will not fit the field has to be refused rather than wrapped; the
// exclusive bound one past the maximum is what the running test compares to. The
// bits below the field — the opcode and its A argument — are what a rebuilt word
// keeps.
constexpr uint32_t kPatchComparisonOffsetShift = 14u;
constexpr uint32_t kPatchComparisonMaxOffset = 0x1FFFFu;
constexpr uint32_t kPatchComparisonOffsetLimit = 0x20000u;
constexpr uint32_t kPatchComparisonOffsetBias = 0x7FFFC000u;
constexpr uint32_t kPatchComparisonArgMask = 0x3FFFu;

// A discharged marker rebuilds its two nine-bit argument ranges: the range at
// bit 14 stays where it is and the range at bit 23 moves down to bit 6.
constexpr uint32_t kPatchComparisonKeptMask = 0x7FC000u;
constexpr uint32_t kPatchComparisonMovedMask = 0x7FC0u;
constexpr uint32_t kPatchComparisonMovedShift = 17u;

// The per-opcode mode bytes, signed and one byte per opcode, lie in the guest's
// constant data 312 bytes above the shared nil record; a negative mode means the
// word before this one is still accumulating operands.
constexpr uint32_t kPatchComparisonModeTable = kNilValueAddr + 312u;

// The text this body raises when a folded distance will not fit its field.
constexpr uint32_t kPatchComparisonTooLongText = kNilValueAddr - 3988u;

// The save frame the suspend path writes and a rewind pops, and the resume word
// pushed above it.
constexpr uint32_t kPatchComparisonSaveFrameBytes = 36u;
constexpr uint32_t kPatchComparisonResumeWordBytes = 4u;

}  // namespace

// Folds the comparison on `expression` into the branch that tests it: the chain
// of entries a pending comparison left behind hangs off `state[32]`, and the walk
// below follows it to its end, re-encoding each entry's link and turning the
// comparison's marker into a jump to this instruction. `state[24]` is the
// expression the previous step compiled and `state[28]` the one recorded here.
// The step is suspendable: a gate replays the call its resume code names, and the
// save frame holds state @0, instruction @4, expression @8, word @12, slot @16,
// cursor @20, target @24, code @28 and marker @32.
void PatchComparisonIntoJump(EngineContext* ctx, uint32_t state, uint32_t instruction, uint32_t expression) {
  EnterFrame();
  // The save frame base: a rewind pops it and the suspend path writes its nine
  // words into it.
  uint32_t frame = 0u;
  // The chain entry being examined, its address — then the offset it encodes and
  // the word the tail is rebuilt from — and the marker rebuilt from it.
  uint32_t word = 0u;
  uint32_t slot = 0u;
  uint32_t marker = 0u;
  // The walk position, the chain index it continues at, the code block the walk
  // reads and the pc after the word the walk came from.
  uint32_t cursor = 0u;
  uint32_t target = 0u;
  uint32_t code = 0u;
  uint32_t next_pc = 0u;
  // The resume point a rewind pops, naming the call that was in flight.
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 36-byte frame the suspend path saved and read it in the
    // order it was written.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kPatchComparisonSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    instruction = LoadU32(ctx, frame + 4u);
    expression = LoadU32(ctx, frame + 8u);
    word = LoadU32(ctx, frame + 12u);
    slot = LoadU32(ctx, frame + 16u);
    cursor = LoadU32(ctx, frame + 20u);
    target = LoadU32(ctx, frame + 24u);
    code = LoadU32(ctx, frame + 28u);
    marker = LoadU32(ctx, frame + 32u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed above that frame.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kPatchComparisonResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: remember whether this expression is the one the previous step
    // compiled.
    word = static_cast<uint32_t>(LoadU32(ctx, state + 24u) == expression);
  }
  if (word != 0u || ResumePhase(ctx) == kResumeRewinding) {
    bool const rewinding = ResumePhase(ctx) == kResumeRewinding;
    uint32_t latch = next_pc;
    if (ResumePhase(ctx) == kResumeRunning) {
      // The comparison this step folds in is recorded, and a marker that names no
      // target leaves the chain as it stands.
      StoreU32(ctx, state + 28u, expression);
      if (instruction == kPatchComparisonNoJump) {
        goto running_done;
      }
      expression = LoadU32(ctx, state + 32u);
      if (expression == kPatchComparisonNoJump) {
        goto store_head;
      }
      code = LoadU32(ctx, LoadU32(ctx, state) + 12u);
    walk_chain:;
      cursor = expression;
      slot = code + (expression << 2u);
      word = LoadU32(ctx, slot);
      uint32_t const back = (word >> kPatchComparisonOffsetShift) - kPatchComparisonMaxOffset;
      expression = back == kPatchComparisonNoJump ? kPatchComparisonNoJump : cursor + back + 1u;
      if (expression != kPatchComparisonNoJump) {
        goto walk_chain;
      }
      target = slot;
      instruction = instruction + (cursor ^ kPatchComparisonNoJump);
      expression = static_cast<uint32_t>(static_cast<int32_t>(instruction) >> 31);
      cursor = instruction + expression;
      expression = static_cast<uint32_t>(static_cast<int32_t>(expression ^ cursor) >=
                                         static_cast<int32_t>(kPatchComparisonOffsetLimit));
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      latch = expression;
    }
    if (rewinding || latch != 0u) {
      if (ResumePhase(ctx) == kResumeRunning) {
        // Running: the state the error text is raised against.
        state = LoadU32(ctx, state + 12u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
        RaiseSyntaxError(ctx, state, kPatchComparisonTooLongText);
        resume_point = 0u;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        // The word the tail is rebuilt from: a running pass reads it back, a
        // rewind keeps the one the frame restored.
        word = LoadU32(ctx, slot);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The chain's tail is rewritten: the offset field is replaced by the
      // distance to this instruction, biased by the field's own maximum.
      uint32_t const tail = (word & kPatchComparisonArgMask) |
                            (instruction << kPatchComparisonOffsetShift);
      StoreU32(ctx, target, tail + kPatchComparisonOffsetBias);
      goto exit_path;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: a marker that names no target is the whole chain.
    word = static_cast<uint32_t>(instruction == kPatchComparisonNoJump);
    if (word != 0u) {
      goto running_done;
    }
  }
patch_loop:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The entry at this pc, the word in it and the offset that word carries: it
    // is stored biased by the field's maximum and counts backwards from here.
    cursor = LoadU32(ctx, LoadU32(ctx, state) + 12u) + (instruction << 2u);
    word = LoadU32(ctx, cursor);
    slot = (word >> kPatchComparisonOffsetShift) - kPatchComparisonMaxOffset;
    target = static_cast<uint32_t>(slot == kPatchComparisonNoJump);
    next_pc = instruction + slot + 1u;
    if (static_cast<int32_t>(instruction) >= static_cast<int32_t>(1u)) {
      // A marker that follows a word still accumulating operands belongs to that
      // word, not to itself: the per-opcode mode byte says so.
      code = cursor - 4u;
      slot = LoadU32(ctx, code);
      uint32_t const mode_slot = kPatchComparisonModeTable + (slot & kPatchComparisonOpcodeMask);
      if (LoadS8(ctx, mode_slot) < 0) {
        goto classify_entry;
      }
    }
    // Otherwise this word is the one the walk is completing.
    slot = word;
    code = cursor;
  classify_entry:;
    marker = static_cast<uint32_t>((slot & kPatchComparisonOpcodeMask) ==
                                   kPatchComparisonMarkerOpcode);
    target = target != 0u ? kPatchComparisonNoJump : next_pc;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (marker != 0u) {
      // A marker is completed in place: the argument ranges the link used are
      // rebuilt as the offset field's neighbours and the jump opcode is stamped
      // over the marker's. Only the first range is kept in the working word.
      marker = slot & kPatchComparisonKeptMask;
      uint32_t const moved = (slot >> kPatchComparisonMovedShift) & kPatchComparisonMovedMask;
      StoreU32(ctx, code, marker | moved | kPatchComparisonJumpOpcode);
      // The distance the completed jump takes: from this marker to the pc the
      // walk is heading for. The absolute value is taken without branching — the
      // shift's sign word, the sum it makes, and the two exclusive-ored.
      instruction = expression + (instruction ^ kPatchComparisonNoJump);
      slot = static_cast<uint32_t>(static_cast<int32_t>(instruction) >> 31);
      uint32_t const folded = instruction + slot;
      word = static_cast<uint32_t>(static_cast<int32_t>(slot ^ folded) >
                                   static_cast<int32_t>(kPatchComparisonMaxOffset));
      if (word != 0u) {
        goto reload_stream;
      }
      goto reload_entry;
    }
    // The same test for a word that is not a jump. The two tests stay apart
    // rather than sharing one.
    instruction = expression + (instruction ^ kPatchComparisonNoJump);
    slot = static_cast<uint32_t>(static_cast<int32_t>(instruction) >> 31);
    code = instruction + slot;
    slot = static_cast<uint32_t>(static_cast<int32_t>(slot ^ code) <
                                 static_cast<int32_t>(kPatchComparisonOffsetLimit));
    if (slot != 0u) {
      goto patch_entry;
    }
  reload_stream:;
    word = LoadU32(ctx, state + 12u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
    RaiseSyntaxError(ctx, word, kPatchComparisonTooLongText);
    resume_point = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
reload_entry:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The completed word is read back, so the walk below rebuilds it from what
    // the array now holds rather than from what the walk decoded.
    word = LoadU32(ctx, cursor);
  }
patch_entry:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The offset field: everything above the opcode and A arguments is replaced
    // by the link to the next entry, biased by the field's own maximum.
    uint32_t const encoded = (word & kPatchComparisonArgMask) |
                             (instruction << kPatchComparisonOffsetShift);
    StoreU32(ctx, cursor, encoded + kPatchComparisonOffsetBias);
    instruction = target;
    word = static_cast<uint32_t>(target != kPatchComparisonNoJump);
    if (word != 0u) {
      goto patch_loop;
    }
  }
running_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    goto exit_path;
  }
store_head:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the chain now starts at this instruction.
    StoreU32(ctx, state + 32u, instruction);
  }
  goto exit_path;
suspend_point:;
  // Push the resume point naming the call in flight, then the frame this pass's
  // save words read back. The resume word goes below the frame pointer, which
  // then advances past it to make room for the nine saved words.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kPatchComparisonResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, instruction);
  StoreU32(ctx, frame + 8u, expression);
  StoreU32(ctx, frame + 12u, word);
  StoreU32(ctx, frame + 16u, slot);
  StoreU32(ctx, frame + 20u, cursor);
  StoreU32(ctx, frame + 24u, target);
  StoreU32(ctx, frame + 28u, code);
  StoreU32(ctx, frame + 32u, marker);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kPatchComparisonSaveFrameBytes);
exit_path:;
  LeaveFrame();
}

}  // namespace artc
