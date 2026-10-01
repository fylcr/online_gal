#include "util/engine_context.h"

namespace artc {

// The instruction sink a fresh branch word is appended through
// (script/emit_instruction.cpp).
uint32_t EmitInstruction(EngineContext* ctx, uint32_t func_state, uint32_t opcode, uint32_t operands);

namespace {

// A branch word reads `(end << 23) | (opcode << 6) | kind`: the branch's own
// opcode in the eight-bit field, the last opcode it covers in the nine-bit field
// above it, and the branch kind in the low six bits. `end` is built by
// subtracting one from the shifted first-opcode-plus-count so the low tail stays
// untouched.
constexpr uint32_t kEmitBranchWordKind = 3u;
constexpr uint32_t kEmitBranchWordKindMask = 0x3Fu;
constexpr uint32_t kEmitBranchWordOpcodeShift = 6u;
constexpr uint32_t kEmitBranchWordOpcodeMask = 0xFFu;
constexpr uint32_t kEmitBranchWordEndShift = 23u;
constexpr uint32_t kEmitBranchWordEndMask = 0x7FFFFFu;
constexpr uint32_t kEmitBranchWordEndBias = 0x800000u;  // one << 23
constexpr uint32_t kEmitBranchWordSaveFrameBytes = 12u;
constexpr uint32_t kEmitBranchWordResumeWordBytes = 4u;

}  // namespace

// Makes the word that carries a branch cover the `operands` opcodes starting at
// `opcode`. When the word already sitting in front of the code cursor is a branch
// of the same kind whose range contains `opcode`, its range is widened in place;
// otherwise a fresh word is built and appended through the instruction sink.
void EmitBranchWord(EngineContext* ctx, uint32_t func_state, uint32_t opcode, uint32_t operands) {
  EnterFrame();
  uint32_t target = 0u;
  uint32_t pending = 0u;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the saved parameters back out of the event frame.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kEmitBranchWordSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    func_state = LoadU32(ctx, frame);
    opcode = LoadU32(ctx, frame + 4u);
    operands = LoadU32(ctx, frame + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the second pop names the value the suspended call produced. The
    // two pops stay separate -- the driver keeps one region at a time.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kEmitBranchWordResumeWordBytes);
    pending = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }

  if (ResumePhase(ctx) == kResumeRunning) {
    target = LoadU32(ctx, func_state + 24u);
    if (static_cast<int32_t>(target) > static_cast<int32_t>(LoadU32(ctx, func_state + 28u))) {
      if (target == 0) {
        // No word to widen yet: the caller's own limit decides whether one is
        // needed at all.
        if (static_cast<int32_t>(LoadU8(ctx, func_state + 50u)) <= static_cast<int32_t>(opcode)) {
          LeaveFrame();
          return;
        }
      } else {
        uint32_t const entry = LoadU32(ctx, LoadU32(ctx, func_state) + 12u) + (target << 2) - 4u;
        uint32_t const word = LoadU32(ctx, entry);
        if ((word & kEmitBranchWordKindMask) == kEmitBranchWordKind &&
            static_cast<int32_t>((word >> kEmitBranchWordOpcodeShift) & kEmitBranchWordOpcodeMask) <=
                static_cast<int32_t>(opcode) &&
            static_cast<int32_t>((word >> kEmitBranchWordEndShift) + 1u) >=
                static_cast<int32_t>(opcode)) {
          uint32_t const end = opcode + operands - 1u;
          if ((word >> kEmitBranchWordEndShift) < end) {
            StoreU32(ctx, entry,
                     (word & kEmitBranchWordEndMask) | (end << kEmitBranchWordEndShift));
          }
          LeaveFrame();
          return;
        }
      }
    }
    // The word in front of the cursor does not cover the branch, so a fresh one
    // is built with the range the branch needs.
    opcode = (((opcode + operands) << kEmitBranchWordEndShift) - kEmitBranchWordEndBias) |
             (opcode << kEmitBranchWordOpcodeShift) | kEmitBranchWordKind;
    operands = LoadU32(ctx, LoadU32(ctx, func_state + 12u) + 8u);
  }

  // The sink is entered unless a suspended frame already carries its answer.
  if (ResumePhase(ctx) == kResumeRunning || pending == 0u) {
    EmitInstruction(ctx, func_state, opcode, operands);
    if (ResumePhase(ctx) == kResumeSuspended) {
      // Suspended inside the sink: push the parameters so the rewind above can
      // rebuild them, and hand the driver zero.
      StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), 0u);
      StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
      uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
      StoreU32(ctx, frame, func_state);
      StoreU32(ctx, frame + 4u, opcode);
      StoreU32(ctx, frame + 8u, operands);
      StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 12u);
    }
  }
  LeaveFrame();
}

}  // namespace artc
