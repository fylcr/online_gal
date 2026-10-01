#include "util/engine_context.h"

namespace artc {

// Forward declarations -- the parser steps and the emitters this walk drives.
uint32_t FormatPushString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void RaiseSyntaxError(EngineContext* ctx, uint32_t a, uint32_t b);
void StepRecordResume(EngineContext* ctx, uint32_t a0);
uint32_t FormatCharValue(EngineContext* ctx, uint32_t a, uint32_t b);
uint32_t EmitOpInstruction(EngineContext* ctx, uint32_t func_state, uint32_t op, uint32_t arg_a,
                           uint32_t arg_b, uint32_t line);
void PatchJumpChain(EngineContext* ctx, uint32_t state, uint32_t instruction);
uint32_t ConcatJumpLists(EngineContext* ctx, uint32_t state);
void ReserveRegisters(EngineContext* ctx, uint32_t state, uint32_t count);
void CloseBlockScope(EngineContext* ctx, uint32_t state);
void PatchComparisonIntoJump(EngineContext* ctx, uint32_t state, uint32_t instruction,
                             uint32_t expression);
uint32_t EmitInstructionABC(EngineContext* ctx, uint32_t state, uint32_t opcode, uint32_t arg_a,
                            uint32_t arg_c);

namespace {

// "'%s' expected", by message table index.
constexpr uint32_t kParseExpressionExpectedMsg = 0x2B625u;
// The operand both comparison opcodes below are emitted with.
constexpr uint32_t kParseExpressionTestOperand = 0x1FFFEu;
// The jump value that marks a list with no targets yet.
constexpr uint32_t kParseExpressionNoJump = 0xFFFFFFFFu;
constexpr uint32_t kParseExpressionSaveFrameBytes = 44u;
constexpr uint32_t kParseExpressionResumeWordBytes = 4u;

}  // namespace

// Compiles the expression that starts at the current token into the destination
// slot: it opens a compiler block for the depth registers the expression needs,
// records where each of the operand's new locals starts, and leaves the
// expression's jump lists patched into the comparison that consumes them.  The
// first parameter is the lexer state (word +16 holds the current token, +48 the
// compiler handle); then the current token, the destination slot, the register
// depth and the operand flag.  Every parser call below can suspend.
void ParseExpression(EngineContext* ctx, uint32_t state, uint32_t token, uint32_t slot,
                     uint32_t depth, uint32_t flag) {
  EnterFrame();
  // The frame saves one word per slot below, so each slot keeps a single name
  // across the whole walk even where a later region reuses it: `scratch` is the
  // record base, then the lexer handle, then the active-local count, then the
  // loop counter; `scope` is the local-array index base and then the block
  // frame; `value` is the token flag, then a call result, then a jump list.
  uint32_t func_state = 0u;
  uint32_t scratch = 0u;
  uint32_t scope = 0u;
  uint32_t node = 0u;
  uint32_t value = 0u;
  uint32_t level = 0u;
  uint32_t resume_point = 0u;
  uint32_t record_base = 0u;
  uint32_t prev_level = 0u;
  uint32_t entry_value = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 44-byte frame the suspend path pushed and rebuild the
    // eleven saved slots from it.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kParseExpressionSaveFrameBytes);
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    slot = LoadU32(ctx, frame + 8u);
    depth = LoadU32(ctx, frame + 12u);
    flag = LoadU32(ctx, frame + 16u);
    func_state = LoadU32(ctx, frame + 20u);
    scratch = LoadU32(ctx, frame + 24u);
    scope = LoadU32(ctx, frame + 28u);
    node = LoadU32(ctx, frame + 32u);
    value = LoadU32(ctx, frame + 36u);
    level = LoadU32(ctx, frame + 40u);
    token = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // The second pop names the call that was in flight.
    StoreU32(ctx, ReturnFrameSp(ctx),
             LoadU32(ctx, ReturnFrameSp(ctx)) - kParseExpressionResumeWordBytes);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Claim the 32-byte scratch area: an expression descriptor in its first
    // half and the block frame this expression opens in its second.
    node = GuestStackSp(ctx) - 32u;
    GuestStackSp(ctx) = node;
    func_state = LoadU32(ctx, state + 48u);
    // Three new locals start at the current pc: the active-local count grows by
    // three, and that count doubles as the index base of the local-variable
    // array of the three records that follow.
    scope = LoadU8(ctx, func_state + 50u) + 3u;
    StoreU8(ctx, func_state + 50u, scope);
    scratch = LoadU32(ctx, LoadU32(ctx, func_state) + 24u);
    scope = func_state + ((scope & 255u) << 1u);
    level = LoadU32(ctx, func_state + 24u);
    StoreU32(ctx, scratch + LoadU16(ctx, scope + 166u) * 12u + 4u, level);
    StoreU32(ctx, scratch + LoadU16(ctx, scope + 168u) * 12u + 4u, level);
    StoreU32(ctx, scratch + LoadU16(ctx, scope + 170u) * 12u + 4u, level);
    value = LoadU32(ctx, state + 16u) != 259u;
  }
  if (value != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The lexer handle the diagnostic below is formatted for.
      scratch = LoadU32(ctx, state + 52u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 0u) {
      // Build the single character the diagnostic quotes.
      value = FormatCharValue(ctx, state, 259u);
      resume_point = 0u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, node, value);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 1u) {
      // Format the "'%s' expected" message over that character.
      value = FormatPushString(ctx, scratch, kParseExpressionExpectedMsg, node);
      resume_point = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 2u) {
      RaiseSyntaxError(ctx, state, value);
      resume_point = 2u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 3u) {
    StepRecordResume(ctx, state);
    resume_point = 3u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 4u) {
      // Emit the comparison test the flag asks for.
      value = EmitInstructionABC(ctx, func_state, 32u, token, kParseExpressionTestOperand);
      resume_point = 4u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      goto merge_done;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 5u) {
      // Merge the jump lists the expression produced so far.
      value = ConcatJumpLists(ctx, func_state);
      resume_point = 5u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
  }
merge_done:
  if (ResumePhase(ctx) == kResumeRunning) {
    // Chain the block frame kept in the node's second half onto the function
    // state's block list, then add the register depth to the active-local count.
    StoreU8(ctx, node + 26u, 0u);
    StoreU32(ctx, node + 20u, kParseExpressionNoJump);
    scratch = LoadU8(ctx, func_state + 50u);
    StoreU8(ctx, node + 25u, 0u);
    StoreU8(ctx, node + 24u, scratch);
    StoreU32(ctx, node + 16u, LoadU32(ctx, func_state + 20u));
    StoreU32(ctx, func_state + 20u, node + 16u);
    func_state = LoadU32(ctx, state + 48u);
    scratch = LoadU8(ctx, func_state + 50u) + depth;
    StoreU8(ctx, func_state + 50u, scratch);
    if (depth != 0u) {
      // Every level the depth covers records that its local starts here.
      level = scratch & 255u;
      entry_value = LoadU32(ctx, func_state + 24u);
      record_base = LoadU32(ctx, LoadU32(ctx, func_state) + 24u);
      for (uint32_t step = depth; step != 0u; step--) {
        uint32_t const record_offset =
            LoadU16(ctx, func_state + ((level - step) << 1u) + 172u) * 12u;
        StoreU32(ctx, record_base + record_offset + 4u, entry_value);
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 6u) {
    ReserveRegisters(ctx, func_state, depth);
    resume_point = 6u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 7u) {
    CloseBlockScope(ctx, state);
    resume_point = 7u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Pop the block frame and drop the locals it covered: every level above the
    // block's own count records that its local ends here.
    scope = LoadU32(ctx, func_state + 20u);
    StoreU32(ctx, func_state + 20u, LoadU32(ctx, scope));
    state = LoadU32(ctx, LoadU32(ctx, func_state + 12u) + 48u);
    scratch = LoadU8(ctx, state + 50u);
    level = LoadU8(ctx, scope + 8u);
    if (scratch > level) {
      entry_value = LoadU32(ctx, state + 24u);
      record_base = LoadU32(ctx, LoadU32(ctx, state) + 24u);
      do {
        scratch -= 1u;
        prev_level = scratch & 255u;
        uint32_t const record_offset =
            LoadU16(ctx, state + (prev_level << 1u) + 172u) * 12u;
        StoreU32(ctx, record_base + record_offset + 8u, entry_value);
      } while (prev_level > level);
      StoreU8(ctx, state + 50u, level);
    }
    state = LoadU8(ctx, scope + 9u);
  }
  if (state != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 8u) {
      // Close the block's upvalues.
      state = EmitOpInstruction(ctx, func_state, 35u, level, 0u, 0u);
      resume_point = 8u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, func_state + 36u, LoadU8(ctx, func_state + 50u));
    state = LoadU32(ctx, scope + 4u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 9u) {
    // Patch the jump list the block frame carried.
    PatchJumpChain(ctx, func_state, state);
    resume_point = 9u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 10u) {
    PatchJumpChain(ctx, func_state, value);
    resume_point = 10u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (flag != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning || resume_point == 11u) {
      // Emit the opposite comparison test.
      scratch = EmitInstructionABC(ctx, func_state, 31u, token, kParseExpressionTestOperand);
      resume_point = 11u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Point the instruction the expression compiled into at the slot.
      uint32_t const code = LoadU32(ctx, LoadU32(ctx, func_state) + 20u);
      state = code + (LoadU32(ctx, func_state + 24u) << 2u) - 4u;
      StoreU32(ctx, state, slot);
      goto slot_stored;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 12u) {
    state = EmitOpInstruction(ctx, func_state, 33u, token, 0u, depth);
    resume_point = 12u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    uint32_t const code = LoadU32(ctx, LoadU32(ctx, func_state) + 20u);
    state = code + (LoadU32(ctx, func_state + 24u) << 2u) - 4u;
    StoreU32(ctx, state, slot);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 13u) {
    scratch = ConcatJumpLists(ctx, func_state);
    resume_point = 13u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
slot_stored:
  // A rewinding pass keeps the level the frame restored; a fresh run raises it.
  state = (ResumePhase(ctx) != kResumeRunning) ? state : (value + 1u);
  if (ResumePhase(ctx) == kResumeRunning || resume_point == 14u) {
    PatchComparisonIntoJump(ctx, func_state, scratch, state);
    resume_point = 14u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = node + 32u;
  }
  goto exit_path;
suspend_point:
  // Suspending: store the resume point, then push the frame with the live slots.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_point);
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kParseExpressionResumeWordBytes);
  {
    uint32_t const frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, frame, state);
    StoreU32(ctx, frame + 4u, token);
    StoreU32(ctx, frame + 8u, slot);
    StoreU32(ctx, frame + 12u, depth);
    StoreU32(ctx, frame + 16u, flag);
    StoreU32(ctx, frame + 20u, func_state);
    StoreU32(ctx, frame + 24u, scratch);
    StoreU32(ctx, frame + 28u, scope);
    StoreU32(ctx, frame + 32u, node);
    StoreU32(ctx, frame + 36u, value);
    StoreU32(ctx, frame + 40u, level);
  }
  StoreU32(ctx, ReturnFrameSp(ctx),
           LoadU32(ctx, ReturnFrameSp(ctx)) + kParseExpressionSaveFrameBytes);
exit_path:
  LeaveFrame();
}
}  // namespace artc
