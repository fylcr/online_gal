#include "util/engine_context.h"

namespace artc {

// Forward declarations of the callees below.
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);

namespace {

// A pending jump list's entry is an instruction word whose offset field counts
// backwards to the entry before it, biased by that field's own maximum; the
// all-ones word ends the chain.
constexpr uint32_t kPatchJumpChainNoJump = 0xFFFFFFFFu;
constexpr uint32_t kPatchJumpChainOffsetShift = 14u;
constexpr uint32_t kPatchJumpChainMaxOffset = 0x1FFFFu;
constexpr uint32_t kPatchJumpChainOffsetLimit = 0x20000u;
constexpr uint32_t kPatchJumpChainOffsetBias = 0x7FFFC000u;

// Everything below the offset field is kept when the entry's link is re-encoded.
constexpr uint32_t kPatchJumpChainArgMask = 0x3FFFu;

// The text this body raises when the retargeted distance will not fit the field.
constexpr uint32_t kPatchJumpChainTooLongText = kNilValueAddr - 3988u;

// The save frame the suspend path writes and a rewind pops, and the resume word
// pushed above it.
constexpr uint32_t kPatchJumpChainSaveFrameBytes = 24u;
constexpr uint32_t kPatchJumpChainResumeWordBytes = 4u;

}  // namespace

// Walks the pending jump list hanging off `state[32]` back to its first entry and
// re-encodes that entry's link as the distance to `instruction`, so the whole
// list is retargeted at this instruction; the instruction then becomes the new
// chain head. `state[24]` is the instruction the previous step compiled and
// `state[28]` the one recorded here, and a distance too wide for the field is
// raised against the parser. The step is suspendable: the gate replays the call
// its resume code names, and the save frame holds state @0, instruction @4,
// slot @8, cursor @12, word @16 and entry @20.
void PatchJumpChain(EngineContext* ctx, uint32_t state, uint32_t instruction) {
  EnterFrame();
  // The save frame base: a rewind pops it and the suspend path writes its six
  // words into it.
  uint32_t frame = 0u;
  // The chain entry's address and the word in it.
  uint32_t entry = 0u;
  uint32_t word = 0u;
  // The chain slot being followed — its index, then the offset it encodes, then
  // its address — and the walk position carried across the loop.
  uint32_t slot = 0u;
  uint32_t cursor = 0u;
  // The code block the walk reads, and the resume point a rewind pops, naming the
  // call that was in flight.
  uint32_t code = 0u;
  uint32_t resume_point = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 24-byte frame the suspend path saved and read it in the
    // order it was written.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kPatchJumpChainSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    instruction = LoadU32(ctx, frame + 4u);
    slot = LoadU32(ctx, frame + 8u);
    cursor = LoadU32(ctx, frame + 12u);
    word = LoadU32(ctx, frame + 16u);
    entry = LoadU32(ctx, frame + 20u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed above that frame.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kPatchJumpChainResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the instruction this step walked is recorded, and whether it names
    // no target decides whether there is a chain to walk at all.
    StoreU32(ctx, state + 28u, LoadU32(ctx, state + 24u));
    slot = static_cast<uint32_t>(instruction != kPatchJumpChainNoJump);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    bool const rewinding = ResumePhase(ctx) == kResumeRewinding;
    uint32_t latch = code;
    if (ResumePhase(ctx) == kResumeRunning) {
      // The chain hangs off the state's own head: a head that names no target
      // leaves the chain as it stands.
      slot = LoadU32(ctx, state + 32u);
      if (slot == kPatchJumpChainNoJump) {
        goto store_head;
      }
      code = LoadU32(ctx, LoadU32(ctx, state) + 12u);
    walk_chain:;
      cursor = slot;
      entry = code + (slot << 2u);
      word = LoadU32(ctx, entry);
      uint32_t const back = (word >> kPatchJumpChainOffsetShift) - kPatchJumpChainMaxOffset;
      slot = back == kPatchJumpChainNoJump ? kPatchJumpChainNoJump : cursor + back + 1u;
      if (slot != kPatchJumpChainNoJump) {
        goto walk_chain;
      }
      slot = entry;
      cursor = (cursor ^ kPatchJumpChainNoJump) + instruction;
      instruction = static_cast<uint32_t>(static_cast<int32_t>(cursor) >> 31);
      code = instruction + cursor;
      instruction = static_cast<uint32_t>(static_cast<int32_t>(instruction ^ code) >=
                                          static_cast<int32_t>(kPatchJumpChainOffsetLimit));
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      latch = instruction;
    }
    if (rewinding || latch != 0u) {
      if (ResumePhase(ctx) == kResumeRunning) {
        // Running: the state the error text is raised against.
        state = LoadU32(ctx, state + 12u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
        RaiseSyntaxError(ctx, state, kPatchJumpChainTooLongText);
        resume_point = 0u;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        // The word the re-encoding below reads: a running pass reads it back, a
        // rewind keeps the one the frame restored.
        word = LoadU32(ctx, entry);
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The first entry's link is replaced: the offset field takes the distance
      // to this instruction, biased by the field's own maximum, and everything
      // below the field is kept.
      uint32_t const encoded = (cursor << kPatchJumpChainOffsetShift) |
                               (word & kPatchJumpChainArgMask);
      StoreU32(ctx, slot, encoded + kPatchJumpChainOffsetBias);
    }
  }
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
  // then advances past it to make room for the six saved words.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kPatchJumpChainResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, instruction);
  StoreU32(ctx, frame + 8u, slot);
  StoreU32(ctx, frame + 12u, cursor);
  StoreU32(ctx, frame + 16u, word);
  StoreU32(ctx, frame + 20u, entry);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kPatchJumpChainSaveFrameBytes);
exit_path:;
  LeaveFrame();
}

}  // namespace artc
