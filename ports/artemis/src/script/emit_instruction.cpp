#include "util/engine_context.h"

namespace artc {

// The parser's array grower (script/grow_array.cpp): reallocates the code or
// line block named by a capacity word and answers with the new block.
uint32_t GrowArray(EngineContext* ctx, uint32_t state, uint32_t data, uint32_t capacity_addr,
                   uint32_t element_size, uint32_t limit, uint32_t message);
// The parser's syntax-error raiser (core/raise_syntax_error.cpp): formats the
// text against the lexer state and raises it.
void RaiseSyntaxError(EngineContext* ctx, uint32_t lex_state, uint32_t message);

namespace {

// The three guarded calls, in the order the suspend path publishes them; only
// the one that was in flight is re-issued by a rewinding frame.
constexpr uint32_t kRaiseErrorCall = 0u;
constexpr uint32_t kGrowCodeCall = 1u;
constexpr uint32_t kGrowLineCall = 2u;

// The parser's "no jump" sentinel: a pending-list head, a marker's offset and
// the field's own bias all treat the all-ones word as nothing here.
constexpr uint32_t kNoJump = 0xFFFFFFFFu;

// One instruction word: the opcode is its low six bits and the A argument the
// next eight, so the range a completed word keeps below the offset field ends
// at bit 13; the ranges a discharged marker rebuilds reach up to bit 22.
constexpr uint32_t kOpcodeMask = 63u;
constexpr uint32_t kOpcodeAndArgMask = 0x3FFFu;

// The two opcodes this body judges: the jump emitted while its target is still
// unknown, and the jump a discharged marker becomes.
constexpr uint32_t kJumpOpcode = 27u;
constexpr uint32_t kDischargedJumpOpcode = 26u;

// The jump offset starts at bit 14 and is biased by its own maximum, so a
// distance that will not fit the field has to be refused rather than wrapped;
// the exclusive bound one past the maximum is what a running test compares to.
constexpr uint32_t kOffsetShift = 14u;
constexpr uint32_t kMaxJumpOffset = 0x1FFFFu;
constexpr uint32_t kJumpOffsetLimit = 0x20000u;
constexpr uint32_t kOffsetBiasBits = 0x7FFFC000u;

// The two nine-bit argument ranges a discharged marker rebuilds: the range at
// 14 stays where it is and the range at 23 moves down to 6.
constexpr uint32_t kKeptFieldMask = 0x7FC000u;
constexpr uint32_t kMovedFieldMask = 0x7FC0u;
constexpr uint32_t kMovedFieldShift = 17u;

// One element of either array is a word wide, and neither array may grow past
// the parser's own maximum integer.
constexpr uint32_t kWordSize = 4u;
constexpr uint32_t kMaxInt = 0x7FFFFFFDu;

// The table of per-opcode mode bytes, signed and one byte per opcode: it lies in
// the guest's constant data beside the shared nil record, 312 bytes above it.
constexpr uint32_t kOpcodeModeTableAddr = kNilValueAddr + 312u;

// The two texts this body raises. Both sit in the guest's message pool below the
// nil record: a jump offset too wide for its field, and an array past its limit.
constexpr uint32_t kLongStructureTextAddr = kNilValueAddr - 3988u;
constexpr uint32_t kCodeOverflowTextAddr = kNilValueAddr - 3937u;

}  // namespace

// The parser's code sink: discharges the pending jump list, appends one
// instruction word to the function's code array and its line number to the line
// array, and answers with the pc the word took.
uint32_t EmitInstruction(EngineContext* ctx, uint32_t func_state, uint32_t instruction, uint32_t line) {
  EnterFrame();
  // The save frame base: a rewind pops it and the suspend path writes its twelve
  // words into it.
  uint32_t frame = 0u;
  // The pc the pending-list walk stands on, and — since the walk reuses the one
  // cell — the offset it resolves, the code and line blocks the grow calls work
  // on, the instruction count and a capacity address.
  uint32_t pc = 0u;
  // The record holding both arrays: the code block at +12 with its capacity at
  // +44, the line block at +20 with its capacity at +48.
  uint32_t arrays = 0u;
  // The instruction count the new word lands at.
  uint32_t count = 0u;
  // The walk's resolved jump destination, and later the capacity word handed to
  // the code grow call.
  uint32_t target = 0u;
  // The marker word the walk is completing, and before that the offset test's
  // sign word.
  uint32_t patched = 0u;
  // The instruction word read at the walk's pc.
  uint32_t word = 0u;
  // The slot that word was read from.
  uint32_t word_addr = 0u;
  // Whether the walk's word is a jump instruction, and before that whether its
  // offset was the no-jump sentinel.
  uint32_t marker = 0u;
  // The walk's one scratch cell: the pending-list flag the walk is
  // gated on, then the code owner, the marker slot, the offset test's answer,
  // the allocator handle and the line block. It keeps one name because the
  // suspend frame carries it in one word.
  uint32_t cursor = 0u;
  // What a guarded call answers with; a grow call hands back the new block.
  uint32_t call_answer = 0u;
  // Which guarded call this frame suspended in.
  uint32_t resume_code = 0u;
  // The resume point a rewind pops, naming the call that was in flight.
  uint32_t resume_point = 0u;
  // The pc the emitted word took, which is the count from before the increment;
  // the function-state word is reused for this answer.
  uint32_t emitted_pc = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 48-byte frame the suspend path saved and read it. The
    // words come out in the saved order — the frame base, then the function
    // state, the line and the eight working words, and only last the instruction
    // word, which is the final word taken from the frame.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 48u);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    func_state = LoadU32(ctx, frame);
    line = LoadU32(ctx, frame + 8u);
    cursor = LoadU32(ctx, frame + 12u);
    pc = LoadU32(ctx, frame + 16u);
    arrays = LoadU32(ctx, frame + 20u);
    count = LoadU32(ctx, frame + 24u);
    target = LoadU32(ctx, frame + 28u);
    patched = LoadU32(ctx, frame + 32u);
    word = LoadU32(ctx, frame + 36u);
    word_addr = LoadU32(ctx, frame + 40u);
    marker = LoadU32(ctx, frame + 44u);
    instruction = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed together with that frame. It names
    // the call that suspended, so only that call is issued again below.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: read the instruction count, the code owner and the pending-list
    // head, and remember up front whether there is a list to discharge.
    count = LoadU32(ctx, func_state + 24u);
    arrays = LoadU32(ctx, func_state);
    pc = LoadU32(ctx, func_state + 32u);
    cursor = static_cast<uint32_t>(pc != kNoJump);
  }
  // Running always owes the walk, a rewind owes it because a suspended frame has
  // to be rewound to its call; the two are merged into the one gate here.
  cursor |= static_cast<uint32_t>(ResumePhase(ctx) == kResumeRewinding);
  if (cursor) {
    // A running pass starts at the code owner; a rewind keeps the owner the
    // frame restored. The select reads the phase for truthiness, so any
    // non-running phase keeps the restored one.
    cursor = ResumePhase(ctx) ? cursor : arrays;
  fixup_chain:;
    if (ResumePhase(ctx) == kResumeRunning) {
      // The marker slot at this pc, then the word in it, then the offset that
      // word carries: it is stored biased by the field's maximum and counts
      // backwards from here.
      word_addr = LoadU32(ctx, cursor + 12u) + (pc << 2);
      word = LoadU32(ctx, word_addr);
      target = (word >> kOffsetShift) - kMaxJumpOffset;
      marker = static_cast<uint32_t>(target == kNoJump);
      target = pc + target + 1u;
      if (static_cast<int32_t>(pc) >= static_cast<int32_t>(1u)) {
        // A marker that follows a word still accumulating operands belongs to
        // that word, not to itself: the per-opcode mode byte says so, and a
        // negative mode means the word before this one is unfinished.
        cursor = word_addr - 4u;
        patched = LoadU32(ctx, cursor);
        if (static_cast<int32_t>(static_cast<int8_t>(
                LoadU8(ctx, kOpcodeModeTableAddr + (patched & kOpcodeMask)))) < 0) {
          goto keep_previous;
        }
      }
      // Otherwise this word is the one the walk is completing.
      patched = word;
      cursor = word_addr;
    keep_previous:;
      // A pending word whose offset was the sentinel names no destination, so
      // the walk ends here; and the word is a jump only when its opcode says so.
      target = marker ? kNoJump : target;
      marker = static_cast<uint32_t>((patched & kOpcodeMask) == kJumpOpcode);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (marker) {
        // A jump marker is completed in place: the argument ranges the link used
        // are rebuilt as the offset field's neighbours and the jump opcode is
        // stamped over the marker's. Only the first range is kept in the working
        // word, as the marker keeps it.
        word = patched & kKeptFieldMask;
        StoreU32(ctx, cursor,
                 (word | ((patched >> kMovedFieldShift) & kMovedFieldMask)) | kDischargedJumpOpcode);
        // The distance the completed jump takes: from this marker forward to the
        // pc now being emitted. The absolute value is taken without branching —
        // the shift's sign word, the sum it makes, and the two exclusive-ored —
        // and a distance wider than the field raises the parser's own text.
        pc = count + (pc ^ 0xFFFFFFFFu);
        cursor = static_cast<uint32_t>(static_cast<int32_t>(pc) >> 31);
        patched = cursor + pc;
        cursor = static_cast<uint32_t>(static_cast<int32_t>(cursor ^ patched) >
                                       static_cast<int32_t>(kMaxJumpOffset));
        if (cursor) {
          goto offset_overflow;
        }
        goto patch_done;
      }
      // The same test for a word that is not a jump. The two tests stay apart
      // rather than sharing one.
      pc = count + (pc ^ 0xFFFFFFFFu);
      cursor = static_cast<uint32_t>(static_cast<int32_t>(pc) >> 31);
      patched = cursor + pc;
      cursor = static_cast<uint32_t>(static_cast<int32_t>(cursor ^ patched) <
                                     static_cast<int32_t>(kJumpOffsetLimit));
      if (cursor) {
        goto word_done;
      }
    offset_overflow:;
      // The distance will not fit the field, so the parser raises its own text
      // against the lexer state the function state names.
      cursor = LoadU32(ctx, func_state + 12u);
    }
    {
      // Running always raises here, but this block is only reached when a
      // distance overflowed; a rewind raises only at the resume point that was
      // the raise itself.
      uint32_t const taken = static_cast<uint32_t>(!resume_point);
      if (ResumePhase(ctx) ? taken : 1u) {
        RaiseSyntaxError(ctx, cursor, kLongStructureTextAddr);
        resume_code = kRaiseErrorCall;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_frame;
        }
      }
    }
  patch_done:;
    if (ResumePhase(ctx) == kResumeRunning) {
      // The completed word is read back, so the walk below rebuilds it from what
      // the array now holds rather than from the marker it replaced.
      word = LoadU32(ctx, word_addr);
    }
  word_done:;
    if (ResumePhase(ctx) == kResumeRunning) {
      // The offset field: everything above the opcode and A arguments is
      // replaced by the distance, biased by the field's own maximum, and the
      // result is written back into the marker slot.
      pc = pc << kOffsetShift;
      StoreU32(ctx, word_addr, (pc | (word & kOpcodeAndArgMask)) + kOffsetBiasBits);
      cursor = static_cast<uint32_t>(target != kNoJump);
      if (cursor) {
        // Another pending jump: the link this marker carried is the next one, so
        // the walk restarts at the code owner and that pc.
        cursor = LoadU32(ctx, func_state);
        pc = target;
        goto fixup_chain;
      }
    }
  }
  // The walk is over. Running refreshes the count it has been moving; a rewind
  // keeps the count the frame restored.
  count = ResumePhase(ctx) ? count : LoadU32(ctx, func_state + 24u);
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: the list is discharged, so its head is cleared before the arrays
    // are touched, and the count is tested against the code block's capacity.
    StoreU32(ctx, func_state + 32u, kNoJump);
    cursor = static_cast<uint32_t>(static_cast<int32_t>(count) <
                                   static_cast<int32_t>(LoadU32(ctx, arrays + 44u)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (cursor) {
      // The code block already has room: take it and skip the grower.
      pc = LoadU32(ctx, arrays + 12u);
      goto code_ready;
    }
    // No room: the allocator, the block, and the capacity word's own address are
    // what the grower is handed.
    target = arrays + 44u;
    cursor = LoadU32(ctx, func_state + 16u);
    pc = LoadU32(ctx, arrays + 12u);
  }
  {
    // Site one: a running pass always owes the call, a rewind owes it only at
    // the resume point it popped above.
    uint32_t const taken = static_cast<uint32_t>(resume_point == kGrowCodeCall);
    if (ResumePhase(ctx) ? taken : 1u) {
      call_answer = GrowArray(ctx, cursor, pc, target, kWordSize, kMaxInt, kCodeOverflowTextAddr);
      resume_code = kGrowCodeCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_frame;
      }
      pc = call_answer;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The grower may have moved the block, so the record takes the new one, and
    // the count is read back before the word is written.
    StoreU32(ctx, arrays + 12u, pc);
    count = LoadU32(ctx, func_state + 24u);
  }
code_ready:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The word goes at the count, and the count is read back once more, this
    // time to be tested against the line block's capacity. The instruction word
    // this call was given is spent by now, so its cell carries that test.
    cursor = pc + (count << 2);
    StoreU32(ctx, cursor, instruction);
    pc = LoadU32(ctx, func_state + 24u);
    instruction = static_cast<uint32_t>(static_cast<int32_t>(pc) <
                                       static_cast<int32_t>(LoadU32(ctx, arrays + 48u)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (instruction) {
      // The line block already has room: take it and skip the grower.
      cursor = LoadU32(ctx, arrays + 20u);
      goto line_ready;
    }
    // No room: the allocator takes the place of the spent instruction word, and
    // the line block with the capacity word's address goes with it.
    cursor = LoadU32(ctx, arrays + 20u);
    pc = arrays + 48u;
    instruction = LoadU32(ctx, func_state + 16u);
  }
  {
    // Site two: the same gate, at the second resume point.
    uint32_t const taken = static_cast<uint32_t>(resume_point == kGrowLineCall);
    if (ResumePhase(ctx) ? taken : 1u) {
      call_answer = GrowArray(ctx, instruction, cursor, pc, kWordSize, kMaxInt, kCodeOverflowTextAddr);
      resume_code = kGrowLineCall;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_frame;
      }
      cursor = call_answer;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The line block is committed the same way, and the count is read back for
    // the write below.
    StoreU32(ctx, arrays + 20u, cursor);
    pc = LoadU32(ctx, func_state + 24u);
  }
line_ready:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The line goes at the count as well, and the answer is the count the word
    // was written at, which the increment below steps past.
    StoreU32(ctx, cursor + (pc << 2), line);
    emitted_pc = LoadU32(ctx, func_state + 24u);
    StoreU32(ctx, func_state + 24u, emitted_pc + 1u);
    goto return_path;
  }
  TrapUnreachable();
suspend_frame:;
  // Push the resume point naming the call in flight, then the frame this one's
  // own parameter spill reads back. The code word goes below the frame pointer,
  // which then advances past it to make room for the twelve saved words.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, func_state);
  StoreU32(ctx, frame + 4u, instruction);
  StoreU32(ctx, frame + 8u, line);
  StoreU32(ctx, frame + 12u, cursor);
  StoreU32(ctx, frame + 16u, pc);
  StoreU32(ctx, frame + 20u, arrays);
  StoreU32(ctx, frame + 24u, count);
  StoreU32(ctx, frame + 28u, target);
  StoreU32(ctx, frame + 32u, patched);
  StoreU32(ctx, frame + 36u, word);
  StoreU32(ctx, frame + 40u, word_addr);
  StoreU32(ctx, frame + 44u, marker);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 48u);
  LeaveFrame();
  return 0u;
return_path:;
  LeaveFrame();
  return emitted_pc;
}

}  // namespace artc
