// discharge_expression.cpp — resolve a storage-kind expression descriptor to a
// register value, emitting the load it needs.
//
// DischargeExpression is the parser's "make this expression readable" step. An
// expression descriptor carries a kind word at +0, then the union at +8 （the
// source register or constant index at +8, the auxiliary operand at +12）, then
// the true/false jump-list words. Discharging resolves the kinds that still name
// storage into a concrete value. An upvalue, a global or an indexed field gets
// its load bytecode （OP_GETUPVAL, OP_GETGLOBAL, OP_GETTABLE） packed into one
// instruction word and handed to the code generator, which appends the word and
// records the source line it belongs to; the descriptor is then parked as
// relocatable with the new instruction's pc in its info word. Two kinds emit
// nothing and are closed in place instead: a local already sits in its own
// register, so the descriptor merely stops being a variable reference, and an
// open call or vararg result is resolved against the instruction that produced
// it — the register is read out of that instruction's A field, or the vararg
// window is closed by forcing its B field to 2. Every other kind （constants, and
// values already in a register or parked on an uninspected jump） is left exactly
// as it came in.
//
// Appending an instruction can grow the code and line arrays, and that growth is
// where the guest may suspend, so the body is laid out as the generated driver
// runs it rather than as a plain switch. The three emission sites are its only
// guest calls, and each is gated on the resume point the rewind popped: a
// running pass takes every site, a rewind re-issues only the site that
// suspended. The blocks stay apart in driver order — the frame pop, the resume
// point pop, the running dispatch, the three sites with the ladder between them,
// and the two exits, one that parks the descriptor and one that unwinds with a
// fresh suspend frame holding the function state, the descriptor, the packed
// word and the line. That save point writes its four words in the order 0, 4, 8,
// 12 while the rewind reads them 0, 8, 12, 4, and the frame pointer is replaced
// only after every other word is in hand; merging a ladder guard or folding a
// site's select into a short-circuit test would change that order rather than
// merely read nicer.
//
// The two free-register tests around the indexed load are the parser's own
// register allocator: an operand that is not a constant and sits at or above the
// active-local count releases one register. They stay separate and in source
// order because the second one re-reads the operand word after it has released.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//
#include "util/engine_context.h"

namespace artc {

// Forward declaration — defined in script/emit_instruction.cpp.
// Provisional: that is the generated f340 body — the suspendable instruction
// emitter these sites call （it appends the word to the function's code array,
// records the line, fixes up the pending jump list and answers with the new pc）
// — and is not yet registered in rewrite-names.tsv. The declaration is written
// to match the assembler's cross-product prototype block exactly, so this
// product also checks on its own.
uint32_t EmitInstruction(EngineContext* ctx, uint32_t func_state, uint32_t instruction, uint32_t line);

void DischargeExpression(EngineContext* ctx, uint32_t func_state, uint32_t expr) {
  EnterFrame();
  // The kind word is biased by a local's kind （6） before it is dispatched on, so
  // the dispatch below addresses the storage kinds from zero: local, upvalue,
  // global, indexed.
  uint32_t const kLocalKind = 6u;
  // The kinds a descriptor takes once it holds a value: parked on an instruction
  // that still has to be placed （11） and already in a register （12）.
  uint32_t const kRelocatable = 11u;
  uint32_t const kNonRelocatable = 12u;
  // The three load opcodes the emission sites pack: an upvalue, a global and an
  // indexed field （4, 5, 6）.
  uint32_t const kOpGetUpvalue = 4u;
  uint32_t const kOpGetGlobal = 5u;
  uint32_t const kOpGetTable = 6u;
  // An instruction word's C field starts at bit 14 and its B field at bit 23, so
  // the mask below B is 8388607 — everything a window close keeps.
  uint32_t const kCArgShift = 14u;
  uint32_t const kBArgShift = 23u;
  uint32_t const kBArgMask = (1u << kBArgShift) - 1u;
  // An operand carrying the constant bit （256） names a constant and owns no
  // register, so it never takes part in the free-register tests below.
  uint32_t const kConstantOperand = 256u;
  // The descriptor's kind word, biased above; a site then reuses the same word
  // for the instruction it packs and hands to the emitter.
  uint32_t packed = 0u;
  // The frame base a rewind pops, then the pc the emitter answers with.
  uint32_t frame = 0u;
  // The line the emitted instruction belongs to: the lexer state's current line,
  // read through the function state on a running pass and carried across a
  // suspend.
  uint32_t line = 0u;
  // Which emission site suspended this body; only that site is issued again on a
  // rewind. Zero is the first of the three.
  uint32_t resume_point = 0u;
  // The site number a suspend publishes as its resume point.
  uint32_t resume_site = 0u;
  // Whether this pass issues the call at the site it stands on. Declared with
  // the other locals because the ladder's jumps are all forward.
  uint32_t issue = 0u;
  // The frame base the suspend point below writes its four words into.
  uint32_t saved_frame = 0u;
  // Scratch for the two instruction-address chains below: the prototype's code
  // array base, plus the pc the descriptor holds, read in that order.
  uint32_t code_base = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: lower the frame pointer by the 16-byte frame the suspend path
    // saved and read that frame. The words come out in the generated pop's order
    // — function state, packed word, line, and only then the descriptor.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 16u);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    func_state = LoadU32(ctx, frame);
    packed = LoadU32(ctx, frame + 8u);
    line = LoadU32(ctx, frame + 12u);
    expr = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the resume point pushed together with that frame. It names
    // the site that suspended, so only that call is issued again below.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - 4u);
    resume_point = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Running: bias the kind word by a local's kind, the first kind that still
    // names storage, so the dispatch below starts at zero.
    packed = LoadU32(ctx, expr) - kLocalKind;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    switch (packed) {
      case 0: goto local_register;
      case 1: goto upvalue_load;
      case 2: goto global_load;
      case 3: goto indexed_load;
      case 4: goto no_discharge;
      case 5: goto no_discharge;
      case 6: goto no_discharge;
      case 7: goto call_result;
      case 8: goto vararg_close;
      default: goto no_discharge;
    }
  local_register:;
    // A local needs nothing emitted: its register already holds the value, so the
    // descriptor only stops being a variable reference.
    StoreU32(ctx, expr, kNonRelocatable);
    goto return_path;
  upvalue_load:;
    // An upvalue: the word packs its load with the upvalue index in the B field,
    // and the third argument is the line the new instruction belongs to.
    line = LoadU32(ctx, LoadU32(ctx, func_state + 12u) + 8u);
    packed = (LoadU32(ctx, expr + 8u) << kBArgShift) | kOpGetUpvalue;
  }
  // Site one: a running pass always owes the call, a rewind owes it only at the
  // resume point it popped above. The select reads the phase for truthiness, so
  // any non-running phase keeps the popped answer.
  issue = static_cast<uint32_t>(!resume_point);
  issue = ResumePhase(ctx) != kResumeRunning ? issue : 1u;
  if (issue) {
    frame = EmitInstruction(ctx, func_state, packed, line);
    resume_site = 0u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    func_state = frame;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto discharge_done;
  }
global_load:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // A global: the word packs its load with the constant index in the Bx field
    // （bits 14 up）, which is also where the line is read from.
    line = LoadU32(ctx, LoadU32(ctx, func_state + 12u) + 8u);
    packed = (LoadU32(ctx, expr + 8u) << kCArgShift) | kOpGetGlobal;
  }
  // Site two: the same gate, at the second resume point.
  issue = static_cast<uint32_t>(resume_point == 1u);
  issue = ResumePhase(ctx) != kResumeRunning ? issue : 1u;
  if (issue) {
    frame = EmitInstruction(ctx, func_state, packed, line);
    resume_site = 1u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    func_state = frame;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto discharge_done;
  }
indexed_load:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // An indexed field: the two operands that are not constants release their
    // registers before the load is packed, auxiliary first, and the word ends up
    // with the table register in B and the key register in C. The generated
    // order is kept, including the re-read of the operand word after the second
    // release.
    packed = LoadU32(ctx, expr + 12u);
    if ((packed & kConstantOperand) != 0u) {
      goto aux_released;
    }
    if (static_cast<int32_t>(LoadU8(ctx, func_state + 50u)) > static_cast<int32_t>(packed)) {
      goto aux_released;
    }
    StoreU32(ctx, func_state + 36u, LoadU32(ctx, func_state + 36u) - 1u);
  aux_released:;
    packed = LoadU32(ctx, expr + 8u);
    if ((packed & kConstantOperand) != 0u) {
      goto info_released;
    }
    if (static_cast<int32_t>(LoadU8(ctx, func_state + 50u)) > static_cast<int32_t>(packed)) {
      goto info_released;
    }
    StoreU32(ctx, func_state + 36u, LoadU32(ctx, func_state + 36u) - 1u);
    packed = LoadU32(ctx, expr + 8u);
  info_released:;
    line = LoadU32(ctx, LoadU32(ctx, func_state + 12u) + 8u);
    packed = packed << kBArgShift;
    packed |= LoadU32(ctx, expr + 12u) << kCArgShift;
    packed |= kOpGetTable;
  }
  // Site three: the same gate, at the third resume point.
  issue = static_cast<uint32_t>(resume_point == 2u);
  issue = ResumePhase(ctx) != kResumeRunning ? issue : 1u;
  if (issue) {
    frame = EmitInstruction(ctx, func_state, packed, line);
    resume_site = 2u;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    func_state = frame;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto discharge_done;
  }
call_result:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // An open call: its result sits in whichever register the instruction's A
    // field names, so the descriptor's info word becomes that register and the
    // descriptor itself becomes the value rather than a call reference. The pc
    // it carries points at the instruction, read through the prototype's code
    // array.
    StoreU32(ctx, expr, kNonRelocatable);
    // The prototype pointer, then its code array, then the pc; the two word
    // reads are pulled apart because the generated body takes them in that
    // order and a single expression would leave it unspecified.
    code_base = LoadU32(ctx, LoadU32(ctx, func_state) + 12u);
    packed = LoadU32(ctx, code_base + (LoadU32(ctx, expr + 8u) << 2));
    StoreU32(ctx, expr + 8u, (packed >> 6) & 255u);
    goto return_path;
  }
vararg_close:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // An open vararg: point at the instruction that opened it — the function
    // state word is reused as that address, as the generated body reuses it, and
    // nothing below reads it as a state again — then close the window by keeping
    // everything below the B field and writing B = 2. The two reads that build
    // the address are pulled apart for the same reason as the site above.
    code_base = LoadU32(ctx, LoadU32(ctx, func_state) + 12u);
    func_state = code_base + (LoadU32(ctx, expr + 8u) << 2);
    packed = (LoadU32(ctx, func_state) & kBArgMask) | (2u << kBArgShift);
    StoreU32(ctx, func_state, packed);
    StoreU32(ctx, expr, kRelocatable);
  }
no_discharge:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // Nothing to place: every other kind already names a readable value.
    goto return_path;
  }
discharge_done:;
  if (ResumePhase(ctx) == kResumeRunning) {
    // The site's emission is placed: the descriptor parks as relocatable with the
    // emitter's pc in its info word.
    StoreU32(ctx, expr, kRelocatable);
    StoreU32(ctx, expr + 8u, func_state);
  }
  goto return_path;
suspend_point:;
  // Push the resume point, then the 16-byte frame holding the function state, the
  // descriptor, the packed word and the line.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_site);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 4u);
  saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, saved_frame, func_state);
  StoreU32(ctx, saved_frame + 4u, expr);
  StoreU32(ctx, saved_frame + 8u, packed);
  StoreU32(ctx, saved_frame + 12u, line);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + 16u);
  LeaveFrame();
  return;
return_path:;
  LeaveFrame();
}

}  // namespace artc
